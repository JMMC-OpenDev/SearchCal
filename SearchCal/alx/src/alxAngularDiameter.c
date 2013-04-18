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


/*
 * Local Functions declaration
 */
static alxPOLYNOMIAL_ANGULAR_DIAMETER *alxGetPolynamialForAngularDiameter(void);

const char* alxGetDiamLabel(const alxDIAM diam);

/* 
 * Local functions definition
 */

/**
 * Return the polynomial coefficients for angular diameter computation 
 *
 * @return pointer onto the structure containing polynomial coefficients, or
 * NULL if an error occured.
 *
 * @usedfiles alxAngDiamPolynomial.cfg : file containing the polynomial
 * coefficients to compute the angular diameter for bright star. The polynomial
 * coefficients are given for B-V, V-R, V-K, I-J, I-K, J-H and J-K.n
 */
static alxPOLYNOMIAL_ANGULAR_DIAMETER *alxGetPolynamialForAngularDiameter(void)
{
    /*
     * Check wether the polynomial structure in which polynomial coefficients
     * will be stored to compute angular diameter is loaded into memory or not,
     * and load it if necessary.
     */
    static alxPOLYNOMIAL_ANGULAR_DIAMETER polynomial = {mcsFALSE, "alxAngDiamPolynomial.cfg"};
    if (polynomial.loaded == mcsTRUE)
    {
        return &polynomial;
    }

    /* 
     * Build the dynamic buffer which will contain the coefficient file 
     * coefficient for angular diameter computation
     */
    /* Find the location of the file */
    char* fileName;
    fileName = miscLocateFile(polynomial.fileName);
    if (fileName == NULL)
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

    while ((pos = miscDynBufGetNextLine(&dynBuf, pos, line, sizeof (line), mcsTRUE)) != NULL)
    {
        /* use test level to see coefficient changes */
        logTrace("miscDynBufGetNextLine() = '%s'", line);

        /* If the current line is not empty */
        if (miscIsSpaceStr(line) == mcsFALSE)
        {
            /* Check if there is to many lines in file */
            if (lineNum >= alxNB_COLOR_INDEXES)
            {
                miscDynBufDestroy(&dynBuf);
                errAdd(alxERR_TOO_MANY_LINES, fileName);
                free(fileName);
                return NULL;
            }

            /* Read polynomial coefficients */
            if (sscanf(line, "%*s %lf %lf %lf %lf %lf %lf %lf %lf %lf",
                       &polynomial.coeff[lineNum][0],
                       &polynomial.coeff[lineNum][1],
                       &polynomial.coeff[lineNum][2],
                       &polynomial.coeff[lineNum][3],
                       &polynomial.coeff[lineNum][4],
                       &polynomial.coeff[lineNum][5],
                       &polynomial.error[lineNum],
                       &polynomial.domainMin[lineNum],
                       &polynomial.domainMax[lineNum]) != (alxNB_POLYNOMIAL_COEFF_DIAMETER + 1 + 2))
            {
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

    /* Check if there is missing line */
    if (lineNum != alxNB_COLOR_INDEXES)
    {
        errAdd(alxERR_MISSING_LINE, lineNum, alxNB_COLOR_INDEXES, fileName);
        free(fileName);
        return NULL;
    }

    free(fileName);

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
 *  
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is
 * returned.
 */
mcsCOMPL_STAT alxComputeDiameter(alxDATA mA,
                                 alxDATA mB,
                                 alxPOLYNOMIAL_ANGULAR_DIAMETER *polynomial,
                                 mcsINT32 band,
                                 alxDATA *diam,
                                 mcsLOGICAL checkDomain)
{
    /* If the magnitude are not available,
       then the diameter is not computed. */
    SUCCESS_COND_DO((mA.isSet == mcsFALSE) || (mB.isSet == mcsFALSE),
                    alxDATAClear((*diam)));

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

    /* LBO: Dec2012: enable validity domain checks during validation */
    /* Check the domain */
    if (checkDomain == mcsTRUE)
    {
        SUCCESS_COND_DO((a_b < polynomial->domainMin[band]) || (a_b > polynomial->domainMax[band]),
                        logTest("Color index %s out of validity domain: %lf < %lf < %lf",
                                alxGetDiamLabel(band), polynomial->domainMin[band], a_b, polynomial->domainMax[band]);
                        alxDATAClear((*diam)));
    }

    /* Compute the angular diameter */
    mcsDOUBLE* coeffs = polynomial->coeff[band];

    mcsDOUBLE p_a_b = coeffs[0]
            + coeffs[1] * a_b
            + coeffs[2] * a_b * a_b
            + coeffs[3] * a_b * a_b * a_b
            + coeffs[4] * a_b * a_b * a_b * a_b
            + coeffs[5] * a_b * a_b * a_b * a_b * a_b;

    /* Compute apparent diameter */
    diam->value = 9.306 * pow(10.0, -0.2 * mA.value) * p_a_b;

    /* Compute error */
    diam->error = diam->value * polynomial->error[band] / 100.0;

    /* Set isSet */
    diam->isSet = mcsTRUE;

    /* Set confidence as the smallest confidence of the two */
    diam->confIndex = (mA.confIndex <= mB.confIndex) ? mA.confIndex : mB.confIndex;

    return mcsSUCCESS;
}

mcsCOMPL_STAT alxComputeDiameterWithMagErr(alxDATA mA,
                                           alxDATA mB,
                                           alxPOLYNOMIAL_ANGULAR_DIAMETER *polynomial,
                                           mcsINT32 band,
                                           alxDATA *diam)
{
    /* If the magnitude are not available,
       then the diameter is not computed. */
    SUCCESS_COND_DO((mA.isSet == mcsFALSE) || (mB.isSet == mcsFALSE),
                    alxDATAClear((*diam)));
    
    alxComputeDiameter(mA, mB, polynomial, band, diam, mcsTRUE);

    /* If diameter is not computed or no mag errors, return */    
    SUCCESS_COND((diam->isSet == mcsFALSE) || ((mA.error == 0.0) && ((mB.error == 0.0))));

    alxDATA mag1, mag2, diamMin, diamMax;
    alxDATACopy(mA, mag1);
    alxDATACopy(mB, mag2);

    /* mA+e mB-e */
    mag1.value = mA.value + mA.error;
    mag2.value = mB.value - mB.error;
    alxComputeDiameter(mag1, mag2, polynomial, band, &diamMin, mcsFALSE);
    
    /* mA-e mB+e */
    mag1.value = mA.value - mA.error;
    mag2.value = mB.value + mB.error;
    alxComputeDiameter(mag1, mag2, polynomial, band, &diamMax, mcsFALSE);

    logTest("Diameters %s diam=%.3lf(%.3lf), diamMin=%.3lf(%.3lf), diamMax=%.3lf(%.3lf)",
            alxGetDiamLabel(band),
            diam->value, diam->error,
            diamMin.value, diamMin.error,
            diamMax.value, diamMax.error);

    /* LBO: enlarge error to be the largest + 1/2 diff(diamMin, diamMax) */
    diam->error = mcsMAX(mcsMAX(diam->error, diamMin.error), diamMax.error) + 0.5 * fabs(diamMin.value - diamMax.value);
    
    logTest("Adjusted diameter %s error diam=%.3lf(%.3lf)", alxGetDiamLabel(band), diam->value, diam->error);
    
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
    /* Get polynamial for diameter computation */
    alxPOLYNOMIAL_ANGULAR_DIAMETER *polynomial;
    polynomial = alxGetPolynamialForAngularDiameter();
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
                                            alxDATA *meanDiam,
                                            alxDATA *weightMeanDiam,
                                            alxDATA *meanStdDev,
                                            mcsUINT32 nbRequiredDiameters,
                                            miscDYN_BUF *diamInfo)
{
    /*
     * LBO 12/04/2013: only use diameters with HIGH confidence 
     * ie computed from catalog magnitudes not interpolated !
     */

    mcsSTRING32 tmp;
    mcsUINT32 band;
    mcsUINT32 nbDiameters = 0;
    mcsDOUBLE sumDiameters = 0.0;
    mcsDOUBLE weightSumDiameters = 0.0;
    mcsDOUBLE weightSum = 0.0;
    mcsDOUBLE diam, diamError, weight;
    mcsDOUBLE minDiameterError = 99999999.0;
    mcsLOGICAL inconsistent = mcsFALSE;
    mcsDOUBLE dist = 0.0;
    mcsDOUBLE sumSquDist = 0.0;

    for (band = 0; band < alxNB_DIAMS; band++)
    {
        if ((diameters[band].isSet == mcsTRUE) && (diameters[band].confIndex == alxCONFIDENCE_HIGH))
        {
            nbDiameters++;
            diam = diameters[band].value;
            diamError = diameters[band].error;

            if (diamError < minDiameterError)
            {
                minDiameterError = diamError;
            }

            sumDiameters += diam;

            /* weight = inverse(diameter error) or square error ? */
            weight = 1.0 / (diamError);
            weightSumDiameters += weight * diam;
            weightSum += weight;
        }
    }

    /* initialize structures */
    alxDATAClear((*meanDiam));
    alxDATAClear((*weightMeanDiam));
    alxDATAClear((*meanStdDev));

    /* If less than nb required diameters, stop computation (Laurent 30/10/2012) */
    if (nbDiameters < nbRequiredDiameters)
    {
        logTest("Cannot compute mean diameter (%d < %d valid diameters)", nbDiameters, nbRequiredDiameters);

        /* Set diameter flag information */
        sprintf(tmp, "REQUIRED_DIAMETERS(%1d): %1d", nbRequiredDiameters, nbDiameters);
        miscDynBufAppendString(diamInfo, tmp);

        return mcsSUCCESS;
    }

    /* Compute mean diameter  */
    meanDiam->value = sumDiameters / nbDiameters;
    meanDiam->isSet = mcsTRUE;
    meanDiam->confIndex = alxCONFIDENCE_HIGH;

    /* Compute error on the mean: either 20% or the best 
       error on diameter if worst than 20%
       FIXME: the spec was 10% for the bright case 
       according to JMMC-MEM-2600-0009*/
    meanDiam->error = 0.2 * meanDiam->value;

    /* Check consistency between mean diameter and individual
       diameters within 20%. If inconsistency is found the
       meanDiameter is defined as unvalid. 
       TODO: the spec was 10% for the bright case 
       according to JMMC-MEM-2600-0009 */
    for (band = 0; band < alxNB_DIAMS; band++)
    {
        if ((diameters[band].isSet == mcsTRUE) && (diameters[band].confIndex == alxCONFIDENCE_HIGH))
        {
            dist = fabs(diameters[band].value - meanDiam->value);
            sumSquDist += dist * dist;

            if ((dist > meanDiam->error) && (dist > diameters[band].error))
            {
                /* LBO: only set confidence to LOW (inconsistent but keep value) */
                /* meanDiam->isSet = mcsFALSE; */
                meanDiam->confIndex = alxCONFIDENCE_LOW;

                /* Set diameter flag information */
                if (inconsistent == mcsFALSE)
                {
                    miscDynBufAppendString(diamInfo, "INCONSISTENT_DIAMETERS ");
                    inconsistent = mcsTRUE;
                }
                /* append each band (distance) in diamInfo */
                sprintf(tmp, "%s (%.3lf) ", alxGetDiamLabel(band), dist);
                miscDynBufAppendString(diamInfo, tmp);
            }
        }
    }

    /* Ensure error is larger than min diameter error */
    if (meanDiam->error < minDiameterError)
    {
        meanDiam->error = minDiameterError;
    }

    /* Set the confidence index of the mean diameter
       as the smallest of the used valid diameters */
    /* useless as only HIGH confidence diameters are now in use*/
    /*
    if (meanDiam->confIndex > alxLOW_CONFIDENCE)
    {
        for (band = 0; band < alxNB_DIAMS; band++)
        {
            if ((diameters[band].isSet == mcsTRUE) && (diameters[band].confIndex < meanDiam->confIndex))
            {
                meanDiam->confIndex = diameters[band].confIndex;
            }
        }
    }
     */

    /* stddev */
    meanStdDev->value = sqrt(sumSquDist / nbDiameters);
    meanStdDev->isSet = mcsTRUE;
    meanStdDev->confIndex = meanDiam->confIndex;

    /* Compute weighted mean diameter  */
    weightMeanDiam->value = weightSumDiameters / weightSum;
    weightMeanDiam->isSet = mcsTRUE;
    weightMeanDiam->confIndex = meanDiam->confIndex;

    /* Compute error on the weighted mean: either 20% or the best 
       error on diameter if worst than 20% */
    weightMeanDiam->error = 0.2 * weightMeanDiam->value;

    inconsistent = mcsFALSE;

    /* Check consistency between weighted mean diameter and individual
       diameters within 20% and add it in diam information */
    for (band = 0; band < alxNB_DIAMS; band++)
    {
        if ((diameters[band].isSet == mcsTRUE) && (diameters[band].confIndex == alxCONFIDENCE_HIGH))
        {
            dist = fabs(diameters[band].value - weightMeanDiam->value);

            if ((dist > weightMeanDiam->error) && (dist > diameters[band].error))
            {
                /* Set diameter flag information */
                if (inconsistent == mcsFALSE)
                {
                    miscDynBufAppendString(diamInfo, "INCONSISTENT_DIAMETERS_WEIGHTED_MEAN ");
                    inconsistent = mcsTRUE;
                }
                /* append each band (distance) in diamInfo */
                sprintf(tmp, "%s (%.3lf) ", alxGetDiamLabel(band), dist);
                miscDynBufAppendString(diamInfo, tmp);
            }
        }
    }

    /* Ensure error is larger than min diameter error */
    if (weightMeanDiam->error < minDiameterError)
    {
        weightMeanDiam->error = minDiameterError;
    }

    logTest("Mean diameter = %.3lf(%.3lf) - weighted mean diameter = %.3lf(%.3lf) - stddev %.3lf - valid = %s [%s] from %i diameters",
            meanDiam->value, meanDiam->error, weightMeanDiam->value, weightMeanDiam->error,
            meanStdDev->value, (meanDiam->isSet == mcsTRUE) ? "true" : "false",
            alxGetConfidenceIndex(meanDiam->confIndex), nbDiameters);

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
            return "BV";
        case alxV_R_DIAM:
            return "VR";
        case alxV_K_DIAM:
            return "VK";
        case alxI_J_DIAM:
            return "IJ";
        case alxI_K_DIAM:
            return "IK";
        case alxJ_H_DIAM:
            return "JH";
        case alxJ_K_DIAM:
            return "JK";
        case alxH_K_DIAM:
            return "HK";
        default:
            return "";
    }
}

/**
 * Initialize this file
 */
void alxAngularDiameterInit(void)
{
    alxGetPolynamialForAngularDiameter();
}
/*___oOo___*/
