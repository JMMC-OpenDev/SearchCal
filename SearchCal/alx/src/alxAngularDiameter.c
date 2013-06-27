/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * @file
 * Function definition for angular diameter computation.
 *
 * @sa JMMC-MEM-2600-0009 document.
 */


/* 
 * System Headers
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>


/*
 * MCS Headers 
 */
#include "mcs.h"
#include "log.h"
#include "err.h"
#include "misc.h"


/* 
 * Local Headers
 */
#include "alx.h"
#include "alxPrivate.h"
#include "alxErrors.h"


/* Minimal diameter error (1%) */
#define MIN_DIAM_ERROR      1.0

/* Invalid or maximum diameter error (1000%) when the diameter error is negative */
#define MAX_DIAM_ERROR  1000.0


/* effective polynom domains */
/* disable in concurrent context (static vars) i.e. SOAP server */
static mcsDOUBLE effectiveDomainMin[alxNB_COLOR_INDEXES];
static mcsDOUBLE effectiveDomainMax[alxNB_COLOR_INDEXES];


/** alx dev flag */
static mcsLOGICAL alxDevFlag = mcsFALSE;

mcsLOGICAL alxGetDevFlag(void)
{
    return alxDevFlag;
}

void alxSetDevFlag(mcsLOGICAL flag)
{
    alxDevFlag = flag;
    logInfo("alxDevFlag: %s", isTrue(alxDevFlag) ? "true" : "false");
}


/*
 * Local Functions declaration
 */
static alxPOLYNOMIAL_ANGULAR_DIAMETER *alxGetPolynomialForAngularDiameter(void);

const char* alxGetDiamLabel(const alxDIAM diam);

/* 
 * Local functions definition
 */

/**
 * Compute the polynomial value y = p(x) = coeffs[0] + x * coeffs[1] + x^2 * coeffs[2] ... + x^n * coeffs[n]
 * @param len coefficient array length
 * @param coeffs coefficient array
 * @param x input value
 * @return polynomial value
 */
static mcsDOUBLE alxComputePolynomial(mcsUINT32 len, mcsDOUBLE* coeffs, mcsDOUBLE x)
{
    mcsUINT32 i;
    mcsDOUBLE p  = 0.0;
    mcsDOUBLE xn = 1.0;
    mcsDOUBLE coeff;

    /* iterative algorithm */
    for (i = 0; i < len; i++)
    {
        coeff = coeffs[i];
        if (coeff == 0.0)
        {
            break;
        }
        p += xn * coeff;
        xn *= x;
    }
    return p;
}

/**
 * Return the polynomial coefficients for angular diameter computation 
 *
 * @return pointer onto the structure containing polynomial coefficients, or
 * NULL if an error occured.
 *
 * @usedfiles alxAngDiamPolynomial.cfg : file containing the polynomial
 * coefficients to compute the angular diameter for bright star. The polynomial
 * coefficients are given for (B-V), (V-R), (V-K), (I-J), (I-K), (J-H), (J-K), (H-K).
 */
static alxPOLYNOMIAL_ANGULAR_DIAMETER *alxGetPolynomialForAngularDiameter(void)
{
    /*
     * Check wether the polynomial structure in which polynomial coefficients
     * will be stored to compute angular diameter is loaded into memory or not,
     * and load it if necessary.
     */
    static alxPOLYNOMIAL_ANGULAR_DIAMETER polynomial = {mcsFALSE, "alxAngDiamPolynomial.cfg", "alxAngDiamErrorPolynomial.cfg"};
    if (isTrue(polynomial.loaded))
    {
        return &polynomial;
    }

    /* 
     * Build the dynamic buffer which will contain the coefficient file for angular diameter computation
     */
    /* Find the location of the file */
    char* fileName;
    fileName = miscLocateFile(polynomial.fileName);
    if (isNull(fileName))
    {
        return NULL;
    }

    /* Load file. Comment lines start with '#' */
    miscDYN_BUF dynBuf;
    miscDynBufInit(&dynBuf);

    logInfo("Loading %s ...", fileName);

    NULL_DO(miscDynBufLoadFile(&dynBuf, fileName, "#"),
            miscDynBufDestroy(&dynBuf);
            free(fileName));

    /* For each line of the loaded file */
    mcsINT32 lineNum = 0;
    const char* pos = NULL;
    mcsSTRING1024 line;
    mcsSTRING4 color;

    while (isNotNull(pos = miscDynBufGetNextLine(&dynBuf, pos, line, sizeof (line), mcsTRUE)))
    {
        logTrace("miscDynBufGetNextLine() = '%s'", line);

        /* If the current line is not empty */
        if (isFalse(miscIsSpaceStr(line)))
        {
            /* Check if there is too many lines in file */
            if (lineNum >= alxNB_COLOR_INDEXES)
            {
                miscDynBufDestroy(&dynBuf);
                errAdd(alxERR_TOO_MANY_LINES, fileName);
                free(fileName);
                return NULL;
            }

            /* 
             * Read polynomial coefficients 
             * #Color	a0	a1	a2	a3	a4	a5	Error(skip)	DomainMin	DomainMax
             */
            if (sscanf(line, "%4s %lf %lf %lf %lf %lf %lf %*f %lf %lf",
                       color,
                       &polynomial.coeff[lineNum][0],
                       &polynomial.coeff[lineNum][1],
                       &polynomial.coeff[lineNum][2],
                       &polynomial.coeff[lineNum][3],
                       &polynomial.coeff[lineNum][4],
                       &polynomial.coeff[lineNum][5],
                       &polynomial.domainMin[lineNum],
                       &polynomial.domainMax[lineNum]) != (1 + alxNB_POLYNOMIAL_COEFF_DIAMETER + 2))
            {
                miscDynBufDestroy(&dynBuf);
                errAdd(alxERR_WRONG_FILE_FORMAT, line, fileName);
                free(fileName);
                return NULL;
            }

            if (strcmp(color, alxGetDiamLabel(lineNum)) != 0)
            {
                logError("Color index mismatch: '%s' (expected '%s')", color, alxGetDiamLabel(lineNum));
                miscDynBufDestroy(&dynBuf);
                errAdd(alxERR_WRONG_FILE_FORMAT, line, fileName);
                free(fileName);
                return NULL;
            }

            /* Next line */
            lineNum++;
        }
    }

    free(fileName);

    /* Check if there is missing line */
    if (lineNum != alxNB_COLOR_INDEXES)
    {
        miscDynBufDestroy(&dynBuf);
        errAdd(alxERR_MISSING_LINE, lineNum, alxNB_COLOR_INDEXES, fileName);
        return NULL;
    }

    /* 
     * Build the dynamic buffer which will contain the coefficient file for angular diameter error computation
     */
    /* Find the location of the file */
    fileName = miscLocateFile(polynomial.fileNameError);
    if (isNull(fileName))
    {
        miscDynBufDestroy(&dynBuf);
        return NULL;
    }

    /* Load file. Comment lines start with '#' */
    miscDynBufReset(&dynBuf);

    logInfo("Loading %s ...", fileName);

    NULL_DO(miscDynBufLoadFile(&dynBuf, fileName, "#"),
            miscDynBufDestroy(&dynBuf);
            free(fileName));

    /* For each line of the loaded file */
    lineNum = 0;
    pos = NULL;

    while (isNotNull(pos = miscDynBufGetNextLine(&dynBuf, pos, line, sizeof (line), mcsTRUE)))
    {
        logTrace("miscDynBufGetNextLine() = '%s'", line);

        /* If the current line is not empty */
        if (isFalse(miscIsSpaceStr(line)))
        {
            /* Check if there is too many lines in file */
            if (lineNum >= alxNB_COLOR_INDEXES)
            {
                miscDynBufDestroy(&dynBuf);
                errAdd(alxERR_TOO_MANY_LINES, fileName);
                free(fileName);
                return NULL;
            }

            /* 
             * Read polynomial coefficients 
             * #Color	a0	a1	a2	a3	a4	a5
             */
            if (sscanf(line, "%4s %lf %lf %lf %lf %lf %lf",
                       color,
                       &polynomial.coeffError[lineNum][0],
                       &polynomial.coeffError[lineNum][1],
                       &polynomial.coeffError[lineNum][2],
                       &polynomial.coeffError[lineNum][3],
                       &polynomial.coeffError[lineNum][4],
                       &polynomial.coeffError[lineNum][5]) != (1 + alxNB_POLYNOMIAL_COEFF_DIAMETER))
            {
                miscDynBufDestroy(&dynBuf);
                errAdd(alxERR_WRONG_FILE_FORMAT, line, fileName);
                free(fileName);
                return NULL;
            }

            if (strcmp(color, alxGetDiamLabel(lineNum)) != 0)
            {
                logError("Color index mismatch: '%s' (expected '%s')", color, alxGetDiamLabel(lineNum));
                miscDynBufDestroy(&dynBuf);
                errAdd(alxERR_WRONG_FILE_FORMAT, line, fileName);
                free(fileName);
                return NULL;
            }

            /* Next line */
            lineNum++;
        }
    }

    /* Destroy the dynamic buffer where is stored the file information */
    miscDynBufDestroy(&dynBuf);

    free(fileName);

    /* Check if there is missing line */
    if (lineNum != alxNB_COLOR_INDEXES)
    {
        errAdd(alxERR_MISSING_LINE, lineNum, alxNB_COLOR_INDEXES, fileName);
        return NULL;
    }

    /* Specify that the polynomial has been loaded */
    polynomial.loaded = mcsTRUE;

    return &polynomial;
}

/**
 * Compute am angular diameters for a given color-index based
 * on the coefficients from table. If a magnitude is not set,
 * the diameter is not computed.
 *
 * @param mA is the first input magnitude of the color index
 * @param mB is the second input magnitude of the color index
 * @param polynomial coeficients for angular diameter
 * @param band is the line corresponding to the color index A-B
 * @param diam is the structure to get back the computation 
 * @param checkDomain true to ensure mA-mB is within validity domain 
 * @param computeError true to compute diameter error
 *  
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is
 * returned.
 */
mcsCOMPL_STAT alxComputeDiameter(const char* msg,
                                 alxDATA mA,
                                 alxDATA mB,
                                 alxPOLYNOMIAL_ANGULAR_DIAMETER *polynomial,
                                 mcsINT32 band,
                                 alxDATA *diam,
                                 mcsLOGICAL checkDomain,
                                 mcsLOGICAL computeError)
{
    mcsDOUBLE a_b;

    /* V-Kc is given in COUSIN while the coefficients for V-K are are expressed
       for JOHNSON, thus the conversion (JMMC-MEM-2600-0009 Sec 2.1) */
    if (band == alxV_K_DIAM)
    {
        a_b = (mA.value - mB.value - 0.03) / 0.992;
    }
    else if (band == alxB_V_DIAM)
    {
        /* in B-V, it is the V mag that should be used to compute apparent
           diameter with formula 10^-0.2magV, thus V is given as first mag (mA)
           while the coefficients are given in B-V */
        a_b = mB.value - mA.value;
    }
    else
    {
        a_b = mA.value - mB.value;
    }

    /* update effective polynom domains */
    if (alxIsDevFlag())
    {
        if (a_b < effectiveDomainMin[band])
        {
            effectiveDomainMin[band] = a_b;
        }
        if (a_b > effectiveDomainMax[band])
        {
            effectiveDomainMax[band] = a_b;
        }
    }

    /* Check the polynom's domain */
    if (isTrue(checkDomain))
    {
        SUCCESS_COND_DO((a_b < polynomial->domainMin[band]) || (a_b > polynomial->domainMax[band]),
                        logTest("%s Color %s out of validity domain: %lf < %lf < %lf", msg,
                                alxGetDiamLabel(band), polynomial->domainMin[band], a_b, polynomial->domainMax[band]);
                        alxDATAClear((*diam)));
    }

    /* Compute the angular diameter */
    mcsDOUBLE* coeffs = polynomial->coeff[band];

    mcsDOUBLE p_a_b = alxComputePolynomial(alxNB_POLYNOMIAL_COEFF_DIAMETER, coeffs, a_b);

    /* Compute apparent diameter */
    diam->isSet = mcsTRUE;
    
    /* Note: the polynom value may be negative or very high outside the polynom's domain */
    diam->value = 9.306 * pow(10.0, -0.2 * mA.value) * p_a_b;

    /* Should compute error and confidence index (error propagation) */
    if (isTrue(computeError))
    {
        /* Compute error */
        coeffs = polynomial->coeffError[band];

        mcsDOUBLE p_err = alxComputePolynomial(alxNB_POLYNOMIAL_COEFF_DIAMETER, coeffs, a_b);

        /* TODO: remove ASAP */
        if (p_err > 75.0)
        {
            /* warning when the error is very high */
            logInfo   ("%s Color %s error high     ? %8.3lf %% (a-b = %.3lf)", msg, alxGetDiamLabel(band), p_err, a_b);
        }

        /* check if error is negative */
        if (p_err < 0.0)
        {
            logWarning("%s Color %s error negative : %8.3lf %% (a-b = %.3lf) - use %.3lf instead", msg, alxGetDiamLabel(band), p_err, a_b, MAX_DIAM_ERROR);
            /* Use arbitrary high value to consider this diameter as incorrect */
            p_err = MAX_DIAM_ERROR;
        }
        /* ensure error is not too small */
        if (p_err < MIN_DIAM_ERROR)
        {
            logWarning("%s Color %s error too small: %8.3lf %% (a-b = %.3lf) - use %.3lf instead", msg, alxGetDiamLabel(band), p_err, a_b, MIN_DIAM_ERROR);
            p_err = MIN_DIAM_ERROR;
        }
        /* ensure error is not too high */
        if (p_err > MAX_DIAM_ERROR)
        {
            logWarning("%s Color %s error too high : %8.3lf %% (a-b = %.3lf) - use %.3lf instead", msg, alxGetDiamLabel(band), p_err, a_b, MAX_DIAM_ERROR);
            p_err = MAX_DIAM_ERROR;
        }

        diam->error = diam->value * p_err * 0.01;

        /* Set confidence as the smallest confidence of the two */
        diam->confIndex = (mA.confIndex <= mB.confIndex) ? mA.confIndex : mB.confIndex;
    }

    return mcsSUCCESS;
}

mcsCOMPL_STAT alxComputeDiameterWithMagErr(alxDATA mA,
                                           alxDATA mB,
                                           alxPOLYNOMIAL_ANGULAR_DIAMETER *polynomial,
                                           mcsINT32 band,
                                           alxDATA *diam)
{
    /* If any magnitude is not available, then the diameter is not computed. */
    SUCCESS_COND_DO(alxIsNotSet(mA) || alxIsNotSet(mB),
                    alxDATAClear((*diam)));

    /** check domain and compute error */
    alxComputeDiameter("|a-b|   ", mA, mB, polynomial, band, diam, mcsTRUE, mcsTRUE);

    /* If diameter is not computed (domain check), return */
    SUCCESS_COND(isFalse(diam->isSet));

    /* If any missing magnitude error,
     * return diameter with low confidence index */
    SUCCESS_COND_DO((mA.error == 0.0) || (mB.error == 0.0),
                    diam->confIndex = alxCONFIDENCE_LOW);

    alxDATA mAe, mBe, diamMin, diamMax;
    alxDATACopy(mA, mAe);
    alxDATACopy(mB, mBe);

    /* mA+e mB-e */
    mAe.value = mA.value + mA.error;
    mBe.value = mB.value - mB.error;
    /** do not check domain and do not compute error */
    alxComputeDiameter("|a-b|min", mAe, mBe, polynomial, band, &diamMin, mcsFALSE, mcsFALSE);

    /* If diameter is not computed (domain check), 
     * return diameter with medium confidence index */
    SUCCESS_COND_DO(alxIsNotSet(diamMin),
                    diam->confIndex = alxCONFIDENCE_MEDIUM);

    /* mA-e mB+e */
    mAe.value = mA.value - mA.error;
    mBe.value = mB.value + mB.error;
    /** do not check domain and do not compute error */
    alxComputeDiameter("|a-b|max", mAe, mBe, polynomial, band, &diamMax, mcsFALSE, mcsFALSE);

    /* If diameter is not computed (domain check),
     * return diameter with medium confidence index */
    SUCCESS_COND_DO(alxIsNotSet(diamMax),
                    diam->confIndex = alxCONFIDENCE_MEDIUM);

    /* 
     * TODO: use 4 diameters: [mA-e mB-e], [mA-e mB+e], [mA+e mB-e], [mA+e mB+e]
     * and error^2 = somme(dist(diamX - diam)^2)
     */

    /* Uncertainty should encompass diamMin and diamMax */
    diam->error = alxMax(diam->error,
                         alxMax(fabs(diam->value - diamMin.value),
                                fabs(diam->value - diamMax.value))
                         );

    logDebug("Diameters %s diam=%.3lf(adj err=%.3lf), diamMin=%.3lf(%.3lf), diamMax=%.3lf(%.3lf)",
             alxGetDiamLabel(band),
             diam->value, diam->error, diamMin.value, diamMin.error, diamMax.value, diamMax.error);

    return mcsSUCCESS;
}

/*
 * Public functions definition
 */

/**
 * Compute stellar angular diameters from its photometric properties.
 * 
 * @param magnitudes B V R Ic Jc Hc Kc L M (Johnson / Cousin CIT)
 * @param diameters the structure to give back all the computed diameters
 *  
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is
 * returned.
 */
mcsCOMPL_STAT alxComputeAngularDiameters(const char* msg,
                                         alxMAGNITUDES magnitudes,
                                         alxDIAMETERS diameters)
{
    /* Get polynomial for diameter computation */
    alxPOLYNOMIAL_ANGULAR_DIAMETER *polynomial;
    polynomial = alxGetPolynomialForAngularDiameter();
    FAIL_NULL(polynomial);

    /* Compute diameters for B-V, V-R, V-K, I-J, I-K, J-H, J-K, H-K */

    alxComputeDiameterWithMagErr(magnitudes[alxV_BAND], magnitudes[alxB_BAND], polynomial, alxB_V_DIAM, &diameters[alxB_V_DIAM]);
    alxComputeDiameterWithMagErr(magnitudes[alxV_BAND], magnitudes[alxR_BAND], polynomial, alxV_R_DIAM, &diameters[alxV_R_DIAM]);
    alxComputeDiameterWithMagErr(magnitudes[alxV_BAND], magnitudes[alxK_BAND], polynomial, alxV_K_DIAM, &diameters[alxV_K_DIAM]);
    alxComputeDiameterWithMagErr(magnitudes[alxI_BAND], magnitudes[alxJ_BAND], polynomial, alxI_J_DIAM, &diameters[alxI_J_DIAM]);
    alxComputeDiameterWithMagErr(magnitudes[alxI_BAND], magnitudes[alxK_BAND], polynomial, alxI_K_DIAM, &diameters[alxI_K_DIAM]);
    alxComputeDiameterWithMagErr(magnitudes[alxJ_BAND], magnitudes[alxH_BAND], polynomial, alxJ_H_DIAM, &diameters[alxJ_H_DIAM]);
    alxComputeDiameterWithMagErr(magnitudes[alxJ_BAND], magnitudes[alxK_BAND], polynomial, alxJ_K_DIAM, &diameters[alxJ_K_DIAM]);
    alxComputeDiameterWithMagErr(magnitudes[alxH_BAND], magnitudes[alxK_BAND], polynomial, alxH_K_DIAM, &diameters[alxH_K_DIAM]);

    /* Display results */
    logTest("Diameters %s BV=%.3lf(%.3lf), VR=%.3lf(%.3lf), VK=%.3lf(%.3lf), "
            "IJ=%.3lf(%.3lf), IK=%.3lf(%.3lf), "
            "JH=%.3lf(%.3lf), JK=%.3lf(%.3lf), HK=%.3lf(%.3lf)", msg,
            diameters[alxB_V_DIAM].value, diameters[alxB_V_DIAM].error,
            diameters[alxV_R_DIAM].value, diameters[alxV_R_DIAM].error,
            diameters[alxV_K_DIAM].value, diameters[alxV_K_DIAM].error,
            diameters[alxI_J_DIAM].value, diameters[alxI_J_DIAM].error,
            diameters[alxI_K_DIAM].value, diameters[alxI_K_DIAM].error,
            diameters[alxJ_H_DIAM].value, diameters[alxJ_H_DIAM].error,
            diameters[alxJ_K_DIAM].value, diameters[alxJ_K_DIAM].error,
            diameters[alxH_K_DIAM].value, diameters[alxH_K_DIAM].error);

    return mcsSUCCESS;
}

mcsCOMPL_STAT alxComputeMeanAngularDiameter(alxDIAMETERS diameters,
                                            alxDATA     *meanDiam,
                                            alxDATA     *weightedMeanDiam,
                                            alxDATA     *weightedMeanStdDev,
                                            mcsUINT32   *nbDiameters,
                                            mcsUINT32    nbRequiredDiameters,
                                            miscDYN_BUF *diamInfo)
{
    /*
     * Only use diameters with HIGH confidence 
     * ie computed from catalog magnitudes and not interpolated magnitudes.
     */
    mcsUINT32 band;
    alxDATA   diameter;
    mcsDOUBLE diam, diamError, weight;
    mcsDOUBLE minDiamError = 99999999.0;
    mcsUINT32 nDiameters = 0;
    mcsDOUBLE sumDiameters = 0.0;
    mcsDOUBLE weightedSumDiameters = 0.0;
    mcsDOUBLE weightSum = 0.0;
    mcsDOUBLE dist = 0.0;
    mcsDOUBLE sumSquareDist = 0.0;
    mcsLOGICAL inconsistent = mcsFALSE;
    mcsSTRING32 tmp;

    for (band = 0; band < alxNB_DIAMS; band++)
    {
        diameter = diameters[band];
        /* Note: high confidence means diameter computed from catalog magnitudes */
        if (alxIsSet(diameter) && (diameter.confIndex == alxCONFIDENCE_HIGH))
        {
            nDiameters++;
            diam = diameter.value;

            diamError = diameter.error;
            if (diamError < minDiamError)
            {
                minDiamError = diamError;
            }

            sumDiameters += diam;

            /* weight = inverse variance ie (diameter error)^2 */
            /* Note: diameter error should be > 0.0 ! */
            weight = 1.0 / (diamError * diamError);
            weightSum += weight;
            weightedSumDiameters += diam * weight;
        }
    }

    /* initialize structures */
    alxDATAClear((*meanDiam));
    alxDATAClear((*weightedMeanDiam));
    alxDATAClear((*weightedMeanStdDev));

    /* update diameter count */
    *nbDiameters = nDiameters;

    /* If less than nb required diameters, can not compute mean diameter... */
    if (nDiameters < nbRequiredDiameters)
    {
        logTest("Cannot compute mean diameter (%d < %d valid diameters)", nDiameters, nbRequiredDiameters);

        /* Set diameter flag information */
        sprintf(tmp, "REQUIRED_DIAMETERS (%1d): %1d", nbRequiredDiameters, nDiameters);
        miscDynBufAppendString(diamInfo, tmp);

        return mcsSUCCESS;
    }

    /* Compute mean diameter */
    meanDiam->isSet = mcsTRUE;
    meanDiam->value = sumDiameters / nDiameters;
    /* 
     Compute error on the mean: either 20% or the best 
     error on diameter if worst than 20%
     FIXME: the spec was 10% for the bright case 
     according to JMMC-MEM-2600-0009
     */
    meanDiam->error = 0.2 * meanDiam->value;
    /* Ensure error is larger than the mininimum diameter error */
    if (meanDiam->error < minDiamError)
    {
        meanDiam->error = minDiamError;
    }

    /* Compute weighted mean diameter  */
    weightedMeanDiam->isSet = mcsTRUE;
    weightedMeanDiam->value = weightedSumDiameters / weightSum;
    /* Note: intialize to high confidence as only high confidence diameters are used */
    weightedMeanDiam->confIndex = alxCONFIDENCE_HIGH;
    /* 
     Compute error on the weighted mean: either 20% or the best 
     error on diameter if worst than 20% 
     FIXME: the spec was 10% for the bright case 
     according to JMMC-MEM-2600-0009
     */
    weightedMeanDiam->error = 0.2 * weightedMeanDiam->value;

    /* 
     Check consistency between weighted mean diameter and individual
     diameters within 20%. If inconsistency is found, the
     weighted mean diameter has a LOW confidence.
     FIXME: the spec was 10% for the bright case 
     according to JMMC-MEM-2600-000
     */
    for (band = 0; band < alxNB_DIAMS; band++)
    {
        diameter = diameters[band];
        /* Note: high confidence means diameter computed from catalog magnitudes */
        if (alxIsSet(diameter) && (diameter.confIndex == alxCONFIDENCE_HIGH))
        {
            dist = fabs(diameter.value - weightedMeanDiam->value);
            sumSquareDist += dist * dist;

            if ((dist > weightedMeanDiam->error) && (dist > diameter.error))
            {
                if (isFalse(inconsistent))
                {
                    inconsistent = mcsTRUE;

                    /* Set confidence indexes to LOW */
                    weightedMeanDiam->confIndex = alxCONFIDENCE_LOW;

                    /* Set diameter flag information */
                    miscDynBufAppendString(diamInfo, "INCONSISTENT_DIAMETER ");
                }

                /* Append each color (distance) in diameter flag information */
                sprintf(tmp, "%s (%.3lf) ", alxGetDiamLabel(band), dist);
                miscDynBufAppendString(diamInfo, tmp);
            }
        }
    }

    /* Ensure error is larger than min diameter error */
    if (weightedMeanDiam->error < minDiamError)
    {
        weightedMeanDiam->error = minDiamError;
    }

    /* stddev */
    weightedMeanStdDev->isSet = mcsTRUE;
    weightedMeanStdDev->value = sqrt(sumSquareDist / nDiameters);

    /* Propagate the weighted mean confidence to mean diameter and std dev */
    meanDiam->confIndex = weightedMeanDiam->confIndex;
    weightedMeanStdDev->confIndex = weightedMeanDiam->confIndex;

    logTest("Mean diameter = %.3lf(%.3lf) - weighted mean diameter = %.3lf(%.3lf) - stddev %.3lf - valid = %s [%s] from %i diameters",
            meanDiam->value, meanDiam->error, weightedMeanDiam->value, weightedMeanDiam->error,
            weightedMeanStdDev->value, (weightedMeanDiam->confIndex == alxCONFIDENCE_HIGH) ? "true" : "false",
            alxGetConfidenceIndex(weightedMeanDiam->confIndex), nDiameters);

    return mcsSUCCESS;
}

/**
 * Return the string literal representing the confidence index 
 * @return string literal "NO", "LOW", "MEDIUM" or "HIGH"
 */
const char* alxGetConfidenceIndex(const alxCONFIDENCE_INDEX confIndex)
{
    switch (confIndex)
    {
        case alxCONFIDENCE_HIGH:
            return "HIGH";
        case alxCONFIDENCE_MEDIUM:
            return "MEDIUM";
        case alxCONFIDENCE_LOW:
            return "LOW";
        case alxNO_CONFIDENCE:
        default:
            return "NO";
    }
}

/**
 * Return the string literal representing the diam
 * @return string literal
 */
const char* alxGetDiamLabel(const alxDIAM diam)
{
    switch (diam)
    {
        case alxB_V_DIAM:
            return "B-V";
        case alxV_R_DIAM:
            return "V-R";
        case alxV_K_DIAM:
            return "V-K";
        case alxI_J_DIAM:
            return "I-J";
        case alxI_K_DIAM:
            return "I-K";
        case alxJ_H_DIAM:
            return "J-H";
        case alxJ_K_DIAM:
            return "J-K";
        case alxH_K_DIAM:
            return "H-K";
        default:
            return "";
    }
}

/**
 * Initialize this file
 */
void alxAngularDiameterInit(void)
{
    mcsUINT32 band;
    for (band = 0; band < alxNB_COLOR_INDEXES; band++)
    {
        effectiveDomainMin[band] = +100.0;
        effectiveDomainMax[band] = -100.0;
    }

    alxGetPolynomialForAngularDiameter();
}

/**
 * Log the effective diameter polynom domains
 */
void alxShowDiameterEffectiveDomains(void)
{
    if (alxIsDevFlag())
    {
        logInfo("Effective domains for diameter polynoms:");

        mcsUINT32 band;
        for (band = 0; band < alxNB_COLOR_INDEXES; band++)
        {
            logInfo("Color %s - validity domain: %lf < color < %lf",
                    alxGetDiamLabel(band), effectiveDomainMin[band], effectiveDomainMax[band]);
        }
    }
}
/*___oOo___*/
