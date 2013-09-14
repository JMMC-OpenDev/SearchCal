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

/* flag to enable finding effective polynom domains */
#define alxDOMAIN_LOG mcsTRUE

/* effective polynom domains */
/* disable in concurrent context (static vars) i.e. SOAP server */
static mcsDOUBLE effectiveDomainMin[alxNB_DIAMS];
static mcsDOUBLE effectiveDomainMax[alxNB_DIAMS];

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

    /* iterative algorithm */
    for (i = 0; i < len; i++)
    {
        p += xn * coeffs[i];
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
    mcsINT32 c;

    while (isNotNull(pos = miscDynBufGetNextLine(&dynBuf, pos, line, sizeof (line), mcsTRUE)))
    {
        logTrace("miscDynBufGetNextLine()='%s'", line);

        /* If the current line is not empty */
        if (isFalse(miscIsSpaceStr(line)))
        {
            /* Check if there is too many lines in file */
            if (lineNum >= alxNB_DIAMS)
            {
                miscDynBufDestroy(&dynBuf);
                errAdd(alxERR_TOO_MANY_LINES, fileName);
                free(fileName);
                return NULL;
            }

            /* 
             * Read polynomial coefficients 
             * #Color	a0	a1	a2	ecorr	DomainMin	DomainMax
             */
            if (sscanf(line, "%4s %lf %lf %lf %lf %lf %lf",
                       color,
                       &polynomial.coeff[lineNum][0],
                       &polynomial.coeff[lineNum][1],
                       &polynomial.coeff[lineNum][2],
                       &polynomial.errCorr[lineNum],
                       &polynomial.domainMin[lineNum],
                       &polynomial.domainMax[lineNum]
                       ) != (1 + alxNB_POLYNOMIAL_COEFF_DIAMETER + 3))
            {
                miscDynBufDestroy(&dynBuf);
                errAdd(alxERR_WRONG_FILE_FORMAT, line, fileName);
                free(fileName);
                return NULL;
            }

            /* Count non zero coefficients and set nbCoeff */
            polynomial.nbCoeff[lineNum] = alxNB_POLYNOMIAL_COEFF_DIAMETER;
            for (c = 0; c < alxNB_POLYNOMIAL_COEFF_DIAMETER; c++)
            {
                if (polynomial.coeff[lineNum][c] == 0.0)
                {
                    logDebug("Color '%s' nb coeff Diam=%d", alxGetDiamLabel(lineNum), c);
                    polynomial.nbCoeff[lineNum] = c;
                    break;
                }
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
    if (lineNum != alxNB_DIAMS)
    {
        miscDynBufDestroy(&dynBuf);
        errAdd(alxERR_MISSING_LINE, lineNum, alxNB_DIAMS, fileName);
        return NULL;
    }

    /* 
     * Build the dynamic buffer which will contain the covariance matrix file for angular diameter error computation
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
        logTrace("miscDynBufGetNextLine()='%s'", line);

        /* If the current line is not empty */
        if (isFalse(miscIsSpaceStr(line)))
        {
            /* Check if there is too many lines in file */
            if (lineNum >= alxNB_DIAMS)
            {
                miscDynBufDestroy(&dynBuf);
                errAdd(alxERR_TOO_MANY_LINES, fileName);
                free(fileName);
                return NULL;
            }

            /* 
             * Read Covariance matrix coefficients(3x3)
             * #Color	a00	a01	a02	a10	a11	a12	a20	a21	a22
             */
            if (sscanf(line, "%4s %lf %lf %lf %lf %lf %lf %lf %lf %lf",
                       color,
                       &polynomial.covMatrix[lineNum][0][0],
                       &polynomial.covMatrix[lineNum][0][1],
                       &polynomial.covMatrix[lineNum][0][2],
                       &polynomial.covMatrix[lineNum][1][0],
                       &polynomial.covMatrix[lineNum][1][1],
                       &polynomial.covMatrix[lineNum][1][2],
                       &polynomial.covMatrix[lineNum][2][0],
                       &polynomial.covMatrix[lineNum][2][1],
                       &polynomial.covMatrix[lineNum][2][2]
                       ) != (1 + alxNB_POLYNOMIAL_COEFF_DIAMETER * alxNB_POLYNOMIAL_COEFF_DIAMETER))
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
    if (lineNum != alxNB_DIAMS)
    {
        errAdd(alxERR_MISSING_LINE, lineNum, alxNB_DIAMS, fileName);
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
 * @param failOnDomainCheck true to return no diameter 
 * if |mA - mB| is outside of the validity domain 
 */
void alxComputeDiameter(alxDATA mA,
                        alxDATA mB,
                        alxPOLYNOMIAL_ANGULAR_DIAMETER *polynomial,
                        mcsINT32 band,
                        alxDATA *diam,
                        mcsLOGICAL failOnDomainCheck)
{
    mcsDOUBLE a_b;

    /* no more valid */
    /* V-Kc is given in COUSIN while the coefficients for V-K are are expressed
       for JOHNSON, thus the conversion (JMMC-MEM-2600-0009 Sec 2.1) */
    /*
        if (band == alxV_K_DIAM)
        {
            a_b = (mA.value - mB.value - 0.03) / 0.992;
        }
        else
        {
            a_b = mA.value - mB.value;
        }
     */
    a_b = mA.value - mB.value;

    /* update effective polynom domains */
    if (isTrue(alxDOMAIN_LOG) && alxIsDevFlag())
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

    /* initialize the confidence */
    diam->confIndex = alxCONFIDENCE_HIGH;

    /* check if band is enabled */
    if (polynomial->domainMin[band] > polynomial->domainMax[band])
    {
        alxDATAClear((*diam));
        return;
    }

    /* Always check the polynom's domain */
    if ((a_b < polynomial->domainMin[band]) || (a_b > polynomial->domainMax[band]))
    {
        /* return no diameter */
        logTest("Color %s out of validity domain: %lf < %lf < %lf",
                alxGetDiamLabel(band), polynomial->domainMin[band], a_b, polynomial->domainMax[band]);
        alxDATAClear((*diam));
        return;
    }

    diam->isSet = mcsTRUE;

    /* 
     * TEST Alain Chelli's new diameter and error computation: TO BE VALIDATED BEFORE RELEASE
     */

    int i, j;
    int nbCoeffs = polynomial->nbCoeff[band];
    mcsDOUBLE cov = 0.0;
    mcsDOUBLE *PAR1;
    VEC_COEFF_DIAMETER *MAT1;
    PAR1 = polynomial->coeff[band];
    MAT1 = polynomial->covMatrix[band];


    /* Compute the angular diameter */
    /* FOR II=0, DEG1 DO DIAMC1=DIAMC1+COEFS(II)*(M1-M2)^II */
    mcsDOUBLE p_a_b = alxComputePolynomial(nbCoeffs, PAR1, a_b);

    /* DIAMC1=DIAMC1-0.2*M1 & DIAMC1=9.306*10.^DIAMC1 ; output diameter */
    diam->value = 9.306 * pow(10.0, p_a_b - 0.2 * mA.value);

    mcsDOUBLE varMa = mA.error * mA.error; /* EM1^2 */

    /* i start at 1 */
    for (i = 1; i < nbCoeffs; i++)
    {
        /* COV=COV-0.2*COEFS(II)*II*(M1-M2)^(II-1)*EM1^2 */
        cov += -0.2 * PAR1[i] * i * pow(a_b, (i - 1)) * varMa;
    }

    mcsDOUBLE varMb = mB.error * mB.error; /* EM2^2 */
    mcsDOUBLE sumVarMags = varMa + varMb;  /* EM1^2+EM2^2 */

    mcsDOUBLE err = 0.0;

    for (i = 0; i < nbCoeffs; i++)
    {
        for (j = 0; j < nbCoeffs; j++)
        {
            /* EDIAMC1 += MAT1(II,JJ) * (M1-M2)^(II+JJ) + COEFS(II) * COEFS(JJ) * II * JJ * (M1-M2)^(II+JJ-2) *(EM1^2+EM2^2) */
            err += MAT1[i][j] * pow(a_b, i + j) + PAR1[i] * PAR1[j] * i * j * pow(a_b, i + j - 2) * sumVarMags;
        }
    }

    /* TODO: optimize pow(a_b, n) calls by precomputing values [0; n] */

    /* EDIAMC1=EDIAMC1+2.*COV+0.04*EM1^2 & EDIAMC1=DIAMC1*SQRT(EDIAMC1)*ALOG(10.) ; error of output diameter */
    err += 2.0 * cov + 0.04 * varMa;

    err = diam->value * sqrt(err) * log(10.);

    /* Errors corrected with modelling noise */
    mcsDOUBLE errCorr = polynomial->errCorr[band]; /* relative error */

    /* EDIAMC1_COR=SQRT(EDIAMC1(B)^2+PP2(2)^2*DIAM1(B)^2) */
    diam->error = sqrt(err * err + errCorr * errCorr * diam->value * diam->value);

    logInfo("Diameter %s = %.3lf(%.3lf %.1lf%%) for magA=%.3lf(%.3lf) magB=%.3lf(%.3lf)",
            alxGetDiamLabel(band),
            diam->value, diam->error, alxDATARelError(*diam),
            mA.value, mA.error, mB.value, mB.error
            );
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

    /** check domain and compute diameter and its error */
    alxComputeDiameter(mA, mB, polynomial, band, diam, mcsTRUE);

    /* If diameter is not computed (domain check), return */
    SUCCESS_COND(isFalse(diam->isSet));

    /* Set confidence as the smallest confidence of the two magnitudes */
    diam->confIndex = (mA.confIndex <= mB.confIndex) ? mA.confIndex : mB.confIndex;

    /* If any missing magnitude error (should not happen as error = 0.1 mag if missing error),
     * return diameter with low confidence */
    if ((mA.error == 0.0) || (mB.error == 0.0))
    {
        diam->confIndex = alxCONFIDENCE_LOW;
    }

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

    /* Compute diameters for all bands */

    alxComputeDiameterWithMagErr(magnitudes[alxB_BAND], magnitudes[alxV_BAND], polynomial, alxB_V_DIAM, &diameters[alxB_V_DIAM]);
    alxComputeDiameterWithMagErr(magnitudes[alxB_BAND], magnitudes[alxI_BAND], polynomial, alxB_I_DIAM, &diameters[alxB_I_DIAM]);
    alxComputeDiameterWithMagErr(magnitudes[alxB_BAND], magnitudes[alxJ_BAND], polynomial, alxB_J_DIAM, &diameters[alxB_J_DIAM]);
    alxComputeDiameterWithMagErr(magnitudes[alxB_BAND], magnitudes[alxH_BAND], polynomial, alxB_H_DIAM, &diameters[alxB_H_DIAM]);
    alxComputeDiameterWithMagErr(magnitudes[alxB_BAND], magnitudes[alxK_BAND], polynomial, alxB_K_DIAM, &diameters[alxB_K_DIAM]);
    
    alxComputeDiameterWithMagErr(magnitudes[alxV_BAND], magnitudes[alxR_BAND], polynomial, alxV_R_DIAM, &diameters[alxV_R_DIAM]);
    alxComputeDiameterWithMagErr(magnitudes[alxV_BAND], magnitudes[alxI_BAND], polynomial, alxV_I_DIAM, &diameters[alxV_I_DIAM]);
    alxComputeDiameterWithMagErr(magnitudes[alxV_BAND], magnitudes[alxJ_BAND], polynomial, alxV_J_DIAM, &diameters[alxV_J_DIAM]);
    alxComputeDiameterWithMagErr(magnitudes[alxV_BAND], magnitudes[alxH_BAND], polynomial, alxV_H_DIAM, &diameters[alxV_H_DIAM]);
    alxComputeDiameterWithMagErr(magnitudes[alxV_BAND], magnitudes[alxK_BAND], polynomial, alxV_K_DIAM, &diameters[alxV_K_DIAM]);
    
    alxComputeDiameterWithMagErr(magnitudes[alxI_BAND], magnitudes[alxJ_BAND], polynomial, alxI_J_DIAM, &diameters[alxI_J_DIAM]);
    alxComputeDiameterWithMagErr(magnitudes[alxI_BAND], magnitudes[alxH_BAND], polynomial, alxI_H_DIAM, &diameters[alxI_H_DIAM]);
    alxComputeDiameterWithMagErr(magnitudes[alxI_BAND], magnitudes[alxK_BAND], polynomial, alxI_K_DIAM, &diameters[alxI_K_DIAM]);
    
    alxComputeDiameterWithMagErr(magnitudes[alxJ_BAND], magnitudes[alxH_BAND], polynomial, alxJ_H_DIAM, &diameters[alxJ_H_DIAM]);
    alxComputeDiameterWithMagErr(magnitudes[alxJ_BAND], magnitudes[alxK_BAND], polynomial, alxJ_K_DIAM, &diameters[alxJ_K_DIAM]);
    
    alxComputeDiameterWithMagErr(magnitudes[alxH_BAND], magnitudes[alxK_BAND], polynomial, alxH_K_DIAM, &diameters[alxH_K_DIAM]);

    /* Display results */
    alxLogTestAngularDiameters(msg, diameters);

    return mcsSUCCESS;
}

mcsCOMPL_STAT alxComputeMeanAngularDiameter(alxDIAMETERS diameters,
                                            alxDATA     *meanDiam,
                                            alxDATA     *weightedMeanDiam,
                                            alxDATA     *stddevDiam,
                                            mcsUINT32   *nbDiameters,
                                            mcsUINT32    nbRequiredDiameters,
                                            miscDYN_BUF *diamInfo)
{
    /*
     * Note: Weighted arithmetic mean
     * from http://en.wikipedia.org/wiki/Weighted_arithmetic_mean#Dealing_with_variance
     */
    /*
     * Only use diameters with HIGH confidence 
     * ie computed from catalog magnitudes and not interpolated magnitudes.
     */
    mcsUINT32 band;
    alxDATA   diameter;
    mcsDOUBLE diam, diamError, weight;
    mcsDOUBLE minDiamError = FP_INFINITE;
    mcsUINT32 nDiameters = 0;
    mcsDOUBLE sumDiameters = 0.0;
    mcsDOUBLE sumSquDistDiameters = 0.0;
    mcsDOUBLE weightedSumDiameters = 0.0;
    mcsDOUBLE weightSum = 0.0;
    mcsDOUBLE dist = 0.0;
    mcsLOGICAL inconsistent = mcsFALSE;
    mcsSTRING32 tmp;

    /* Count valid diameters */
    for (band = 0; band < alxNB_DIAMS; band++)
    {
        diameter = diameters[band];

        /* Note: high confidence means diameter computed from catalog magnitudes */
        if (alxIsSet(diameter) && (diameter.confIndex == alxCONFIDENCE_HIGH))
        {
            nDiameters++;
        }
    }

    /* initialize structures */
    alxDATAClear((*meanDiam));
    alxDATAClear((*weightedMeanDiam));
    alxDATAClear((*stddevDiam));

    /* update diameter count */
    *nbDiameters = nDiameters;

    /* if less than required diameters, can not compute mean diameter... */
    if (nDiameters < nbRequiredDiameters)
    {
        logTest("Cannot compute mean diameter (%d < %d valid diameters)", nDiameters, nbRequiredDiameters);

        /* Set diameter flag information */
        sprintf(tmp, "REQUIRED_DIAMETERS (%1d): %1d", nbRequiredDiameters, nDiameters);
        miscDynBufAppendString(diamInfo, tmp);

        return mcsSUCCESS;
    }

    /*
     * LBO: 04/07/2013: if more than 3 diameters, discard H-K diameter 
     * as it provides poor quality diameters / accuracy 
     */
    if ((nDiameters > 3) && alxIsSet(diameters[alxH_K_DIAM]))
    {
        diameters[alxH_K_DIAM].confIndex = alxCONFIDENCE_MEDIUM;
    }

    /* count diameters again */
    nDiameters = 0;

    /* compute statistics */
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
            weight = 1.0 / (diamError * diamError);
            weightSum += weight;
            weightedSumDiameters += diam * weight;
        }
    }

    /* update diameter count */
    *nbDiameters = nDiameters;

    /* Compute mean diameter */
    meanDiam->isSet = mcsTRUE;
    meanDiam->value = sumDiameters / nDiameters;
    /* 
     Compute error on the mean: either 20% or the highest error on diameter if worst than 20%
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
     Compute error on the weighted mean: either 20% or the highest error on diameter if worst than 20% 
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
            sumSquDistDiameters += dist * dist;

            /*
             * Consistency check: distance < 20% and distance < diameter error (1 sigma confidence ie 68%)
             * TODO: test confidence to 2 sigma : 95% ie distance < 2 * diameter error 
             */
            if ((dist > weightedMeanDiam->error) && (dist > /* 2.0 * */ diameter.error))
            {
                if (isFalse(inconsistent))
                {
                    inconsistent = mcsTRUE;

                    /* Set confidence to LOW */
                    weightedMeanDiam->confIndex = alxCONFIDENCE_LOW;

                    /* Set diameter flag information */
                    miscDynBufAppendString(diamInfo, "INCONSISTENT_DIAMETER ");
                }

                /* Append each color (distance relError%) in diameter flag information */
                sprintf(tmp, "%s (%.3lf %.1lf%%) ", alxGetDiamLabel(band), dist, 100.0 * dist / diameter.value);
                miscDynBufAppendString(diamInfo, tmp);
            }
        }
    }

    /* Ensure error is larger than the min diameter error */
    if (weightedMeanDiam->error < minDiamError)
    {
        weightedMeanDiam->error = minDiamError;
    }

    /* stddev of all diameters */
    stddevDiam->isSet = mcsTRUE;
    stddevDiam->value = sqrt(sumSquDistDiameters / nDiameters);

    /* stddev of the weighted mean corresponds to the diameter error rms
     * = SQRT(N / sum(weight) ) if weight = 1 / variance */
    stddevDiam->error = sqrt((1.0 / weightSum) * nDiameters);

    /* Ensure error is larger than the diameter error rms */
    if (weightedMeanDiam->error < stddevDiam->error)
    {
        weightedMeanDiam->error = stddevDiam->error;
    }

    /* Propagate the weighted mean confidence to mean diameter and std dev */
    meanDiam->confIndex = weightedMeanDiam->confIndex;
    stddevDiam->confIndex = weightedMeanDiam->confIndex;

    logTest("Diameter mean=%.3lf(%.3lf %.1lf%%) stddev=(%.3lf %.1lf%%)"
            " weighted=%.3lf(%.3lf %.1lf%%) error=(%.3lf %.1lf%%) valid=%s [%s] from %d diameters",
            meanDiam->value, meanDiam->error, alxDATARelError(*meanDiam),
            stddevDiam->value, 100.0 * stddevDiam->value / weightedMeanDiam->value,
            weightedMeanDiam->value, weightedMeanDiam->error, alxDATARelError(*weightedMeanDiam),
            stddevDiam->error, 100.0 * stddevDiam->error / weightedMeanDiam->value,
            (weightedMeanDiam->confIndex == alxCONFIDENCE_HIGH) ? "true" : "false",
            alxGetConfidenceIndex(weightedMeanDiam->confIndex), nDiameters);

    return mcsSUCCESS;
}

/**
 * Return the string literal representing the confidence index 
 * @return string literal "NO", "LOW", "MEDIUM" or "HIGH"
 */
const char* alxGetConfidenceIndex(const alxCONFIDENCE_INDEX confIndex)
{
    return alxCONFIDENCE_STR[confIndex];
}

/**
 * Return the string literal representing the diam
 * @return string literal
 */
const char* alxGetDiamLabel(const alxDIAM diam)
{
    return alxDIAM_STR[diam];
}

/**
 * Initialize this file
 */
void alxAngularDiameterInit(void)
{
    mcsUINT32 band;
    for (band = 0; band < alxNB_DIAMS; band++)
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
    if (isTrue(alxDOMAIN_LOG) && alxIsDevFlag())
    {
        logInfo("Effective domains for diameter polynoms:");

        mcsUINT32 band;
        for (band = 0; band < alxNB_DIAMS; band++)
        {
            logInfo("Color %s - validity domain: %lf < color < %lf",
                    alxGetDiamLabel(band), effectiveDomainMin[band], effectiveDomainMax[band]);
        }
    }
}
/*___oOo___*/
