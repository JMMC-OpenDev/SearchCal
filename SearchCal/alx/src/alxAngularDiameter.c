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
    logTrace("alxGetPolynamialForAngularDiameter()");

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
                                 alxDATA *diam)
{
    logTrace("alxComputeDiameter()");

    /* If the magnitude are not available,
       then the diameter is not computed. */
    SUCCESS_COND_DO((mA.isSet == mcsFALSE) || (mB.isSet == mcsFALSE),
                    diam->value = 0.0;
                    diam->error = 0.0;
                    diam->confIndex = alxNO_CONFIDENCE;
                    diam->isSet = mcsFALSE);

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
    SUCCESS_COND_DO((a_b < polynomial->domainMin[band]) || (a_b > polynomial->domainMax[band]),
                    logTest("Color index %s out of validity domain: %lf < %lf < %lf", alxGetDiamLabel(band), polynomial->domainMin[band], a_b, polynomial->domainMax[band]);
                    diam->value = 0.0;
                    diam->error = 0.0;
                    diam->confIndex = alxNO_CONFIDENCE;
                    diam->isSet = mcsFALSE);

    /* Compute the angular diameter */
    mcsDOUBLE p_a_b = polynomial->coeff[band][0]
            + polynomial->coeff[band][1] * a_b
            + polynomial->coeff[band][2] * pow(a_b, 2.0)
            + polynomial->coeff[band][3] * pow(a_b, 3.0)
            + polynomial->coeff[band][4] * pow(a_b, 4.0)
            + polynomial->coeff[band][5] * pow(a_b, 5.0);

    /* Compute apparent diameter */
    diam->value = 9.306 * pow(10.0, -0.2 * mA.value) * p_a_b;

    /* Compute error */
    diam->error = diam->value * polynomial->error[band] / 100.0;

    /* Set isSet */
    diam->isSet = mcsTRUE;

    /* Set confidence as the smallest confidence of the two */
    if (mA.confIndex <= mB.confIndex)
    {
        diam->confIndex = mA.confIndex;
    }
    else
    {
        diam->confIndex = mB.confIndex;
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
mcsCOMPL_STAT alxComputeAngularDiameters(alxMAGNITUDES magnitudes,
                                         alxDIAMETERS diameters)
{
    logTrace("alxComputeAngularDiameters()");

    /* Get polynamial for diameter computation */
    alxPOLYNOMIAL_ANGULAR_DIAMETER *polynomial;
    polynomial = alxGetPolynamialForAngularDiameter();
    FAIL_NULL(polynomial);

    logTest("Compute diameters with B=%.3lf, V=%.3lf, R=%.3lf, "
            "I=%.3lf, J=%.3lf, H=%.3lf, K=%.3lf",
            magnitudes[alxB_BAND].value,
            magnitudes[alxV_BAND].value,
            magnitudes[alxR_BAND].value,
            magnitudes[alxI_BAND].value,
            magnitudes[alxJ_BAND].value,
            magnitudes[alxH_BAND].value,
            magnitudes[alxK_BAND].value);

    /* Compute diameters for B-V, V-R, V-K, I-J, I-K, J-H, J-K, H-K */

    alxComputeDiameter(magnitudes[alxV_BAND], magnitudes[alxB_BAND],
                       polynomial,
                       alxB_V_DIAM,
                       &diameters[alxB_V_DIAM]);

    alxComputeDiameter(magnitudes[alxV_BAND], magnitudes[alxR_BAND],
                       polynomial,
                       alxV_R_DIAM,
                       &diameters[alxV_R_DIAM]);

    alxComputeDiameter(magnitudes[alxV_BAND], magnitudes[alxK_BAND],
                       polynomial,
                       alxV_K_DIAM,
                       &diameters[alxV_K_DIAM]);

    alxComputeDiameter(magnitudes[alxI_BAND], magnitudes[alxJ_BAND],
                       polynomial,
                       alxI_J_DIAM,
                       &diameters[alxI_J_DIAM]);

    alxComputeDiameter(magnitudes[alxI_BAND], magnitudes[alxK_BAND],
                       polynomial,
                       alxI_K_DIAM,
                       &diameters[alxI_K_DIAM]);

    alxComputeDiameter(magnitudes[alxJ_BAND], magnitudes[alxH_BAND],
                       polynomial,
                       alxJ_H_DIAM,
                       &diameters[alxJ_H_DIAM]);

    alxComputeDiameter(magnitudes[alxJ_BAND], magnitudes[alxK_BAND],
                       polynomial,
                       alxJ_K_DIAM,
                       &diameters[alxJ_K_DIAM]);

    alxComputeDiameter(magnitudes[alxH_BAND], magnitudes[alxK_BAND],
                       polynomial,
                       alxH_K_DIAM,
                       &diameters[alxH_K_DIAM]);

    /* Display results */
    logTest("Diameters BV=%.3lf(%.3lf), VR=%.3lf(%.3lf), VK=%.3lf(%.3lf), "
            "IJ=%.3lf(%.3lf), IK=%.3lf(%.3lf), "
            "JH=%.3lf(%.3lf), JK=%.3lf(%.3lf), HK=%.3lf(%.3lf)",
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
                                            mcsUINT32 nbRequiredDiameters,
                                            mcsSTRING32 *diamInfo)
{
    logTrace("alxComputeMeanAngularDiameter()");

    mcsUINT32 nbDiameters = 0;
    mcsDOUBLE sumDiameters = 0.0;
    mcsUINT32 band;

    for (band = 0; band < alxNB_DIAMS; band++)
    {
        if (diameters[band].isSet == mcsTRUE)
        {
            sumDiameters += diameters[band].value;
            nbDiameters++;
        }
    }

    /* If less than nb required diameters, stop computation (Laurent 30/10/2012) */
    if (nbDiameters < nbRequiredDiameters)
    {
        meanDiam->value = 0.0;
        meanDiam->error = 0.0;
        meanDiam->isSet = mcsFALSE;
        meanDiam->confIndex = alxNO_CONFIDENCE;

        logTest("Cannot compute mean diameter (%d < %d valid diameters)", nbDiameters, nbRequiredDiameters);

        /* Set diameter flag information */
        sprintf(*diamInfo, "REQUIRED_DIAMETERS(%1d): %1d", nbRequiredDiameters, nbDiameters);

        return mcsSUCCESS;
    }

    /* Compute mean diameter and associated error (20%)
       FIXME: the spec was 10% for the bright case 
       according to JMMC-MEM-2600-0009 */
    meanDiam->value = sumDiameters / nbDiameters;
    meanDiam->error = 0.2 * sumDiameters / nbDiameters;
    meanDiam->isSet = mcsTRUE;
    meanDiam->confIndex = alxCONFIDENCE_HIGH;

    /* Check consistency between mean diameter and individual
       diameters. If inconsistency is found the meanDiameter is
       defined as unvalid */
    for (band = 0; band < alxNB_DIAMS; band++)
    {
        if ((diameters[band].isSet == mcsTRUE) && (fabs(diameters[band].value - meanDiam->value) > meanDiam->error))
        {
            meanDiam->isSet = mcsFALSE;
            meanDiam->confIndex = alxNO_CONFIDENCE;

            /* Set diameter flag information */
            sprintf(*diamInfo, "INCONSISTENT_DIAMETER %s", alxGetDiamLabel(band));
            break;
        }
    }

    if (meanDiam->isSet == mcsTRUE)
    {
        /* Set the confidence index of the mean diameter
           as the smallest of the used valid diameters */
        for (band = 0; band < alxNB_DIAMS; band++)
        {
            if ((diameters[band].isSet == mcsTRUE) && (diameters[band].confIndex < meanDiam->confIndex))
            {
                meanDiam->confIndex = diameters[band].confIndex;
            }
        }
    }

    logTest("Mean diameter = %.3lf(%.3lf) - isValid = %i - %s - from %i diameters",
            meanDiam->value, meanDiam->error, meanDiam->isSet,
            alxGetConfidenceIndex(meanDiam->confIndex),
            nbDiameters);

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
