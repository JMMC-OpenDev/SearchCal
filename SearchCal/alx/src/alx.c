/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * @file
 * Definition and init of generic alx functions.
 */


/* Needed to preclude warnings on snprintf() */
#define  _BSD_SOURCE 1

/* 
 * System Headers
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
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
 * Public functions definition
 */

/**
 * Return the maximum of a and b values
 * @param a double value
 * @param b double value
 * @return maximum value
 */
mcsDOUBLE alxMax(mcsDOUBLE a, mcsDOUBLE b)
{
    return (a >= b) ? a : b;
}

/**
 * Return the minimum of a and b values
 * @param a double value
 * @param b double value
 * @return minimum value
 */
mcsDOUBLE alxMin(mcsDOUBLE a, mcsDOUBLE b)
{
    return (a <= b) ? a : b;
}

const char* alxGetBandLabel(const alxBAND band)
{
    return alxBAND_STR[band];
}

void alxLogTestMagnitudes(const char* line, const char* msg, alxMAGNITUDES magnitudes)
{
    if (doLog(logTEST))
    {
        /* Print out results */
        logTest("%s %s B=%0.3lf(%0.3lf %s) V=%0.3lf(%0.3lf %s) "
                "R=%0.3lf(%0.3lf %s) I=%0.3lf(%0.3lf %s) J=%0.3lf(%0.3lf %s) H=%0.3lf(%0.3lf %s) "
                "K=%0.3lf(%0.3lf %s) L=%0.3lf(%0.3lf %s) M=%0.3lf(%0.3lf %s)",
                line, msg,
                magnitudes[alxB_BAND].value, magnitudes[alxB_BAND].error, alxGetConfidenceIndex(magnitudes[alxB_BAND].confIndex),
                magnitudes[alxV_BAND].value, magnitudes[alxV_BAND].error, alxGetConfidenceIndex(magnitudes[alxV_BAND].confIndex),
                magnitudes[alxR_BAND].value, magnitudes[alxR_BAND].error, alxGetConfidenceIndex(magnitudes[alxR_BAND].confIndex),
                magnitudes[alxI_BAND].value, magnitudes[alxI_BAND].error, alxGetConfidenceIndex(magnitudes[alxI_BAND].confIndex),
                magnitudes[alxJ_BAND].value, magnitudes[alxJ_BAND].error, alxGetConfidenceIndex(magnitudes[alxJ_BAND].confIndex),
                magnitudes[alxH_BAND].value, magnitudes[alxH_BAND].error, alxGetConfidenceIndex(magnitudes[alxH_BAND].confIndex),
                magnitudes[alxK_BAND].value, magnitudes[alxK_BAND].error, alxGetConfidenceIndex(magnitudes[alxK_BAND].confIndex),
                magnitudes[alxL_BAND].value, magnitudes[alxL_BAND].error, alxGetConfidenceIndex(magnitudes[alxL_BAND].confIndex),
                magnitudes[alxM_BAND].value, magnitudes[alxM_BAND].error, alxGetConfidenceIndex(magnitudes[alxM_BAND].confIndex));
    }
}

void alxLogTestAngularDiameters(const char* msg, alxDIAMETERS diameters)
{
    if (doLog(logTEST))
    {
        logTest("Diameter %s B-V=%.3lf(%.3lf %.1lf%%) B-I=%.3lf(%.3lf %.1lf%%) B-J=%.3lf(%.3lf %.1lf%%) "
                "B-H=%.3lf(%.3lf %.1lf%%) B-K=%.3lf(%.3lf %.1lf%%) V-R=%.3lf(%.3lf %.1lf%%) "
                "V-I=%.3lf(%.3lf %.1lf%%) V-J=%.3lf(%.3lf %.1lf%%) V-H=%.3lf(%.3lf %.1lf%%) "
                "V-K=%.3lf(%.3lf %.1lf%%) I-J=%.3lf(%.3lf %.1lf%%) I-J=%.3lf(%.3lf %.1lf%%) "
                "I-K=%.3lf(%.3lf %.1lf%%) J-H=%.3lf(%.3lf %.1lf%%) J-K=%.3lf(%.3lf %.1lf%%) H-K=%.3lf(%.3lf %.1lf%%)", msg,
                diameters[alxB_V_DIAM].value, diameters[alxB_V_DIAM].error, alxDATARelError(diameters[alxB_V_DIAM]),
                diameters[alxB_I_DIAM].value, diameters[alxB_I_DIAM].error, alxDATARelError(diameters[alxB_I_DIAM]),
                diameters[alxB_J_DIAM].value, diameters[alxB_J_DIAM].error, alxDATARelError(diameters[alxB_J_DIAM]),
                diameters[alxB_H_DIAM].value, diameters[alxB_H_DIAM].error, alxDATARelError(diameters[alxB_H_DIAM]),
                diameters[alxB_K_DIAM].value, diameters[alxB_K_DIAM].error, alxDATARelError(diameters[alxB_K_DIAM]),
                diameters[alxV_R_DIAM].value, diameters[alxV_R_DIAM].error, alxDATARelError(diameters[alxV_R_DIAM]),
                diameters[alxV_I_DIAM].value, diameters[alxV_I_DIAM].error, alxDATARelError(diameters[alxV_I_DIAM]),
                diameters[alxV_J_DIAM].value, diameters[alxV_J_DIAM].error, alxDATARelError(diameters[alxV_J_DIAM]),
                diameters[alxV_H_DIAM].value, diameters[alxV_H_DIAM].error, alxDATARelError(diameters[alxV_H_DIAM]),
                diameters[alxV_K_DIAM].value, diameters[alxV_K_DIAM].error, alxDATARelError(diameters[alxV_K_DIAM]),
                diameters[alxI_J_DIAM].value, diameters[alxI_J_DIAM].error, alxDATARelError(diameters[alxI_J_DIAM]),
                diameters[alxI_H_DIAM].value, diameters[alxI_H_DIAM].error, alxDATARelError(diameters[alxI_H_DIAM]),
                diameters[alxI_K_DIAM].value, diameters[alxI_K_DIAM].error, alxDATARelError(diameters[alxI_K_DIAM]),
                diameters[alxJ_H_DIAM].value, diameters[alxJ_H_DIAM].error, alxDATARelError(diameters[alxJ_H_DIAM]),
                diameters[alxJ_K_DIAM].value, diameters[alxJ_K_DIAM].error, alxDATARelError(diameters[alxJ_K_DIAM]),
                diameters[alxH_K_DIAM].value, diameters[alxH_K_DIAM].error, alxDATARelError(diameters[alxH_K_DIAM])
                );
    }
}

/**
 * Initialize the alx module: preload all configuration tables
 */
void alxInit(void)
{
    alxAngularDiameterInit();
    alxMissingMagnitudeInit();
    alxInterstellarAbsorptionInit();
    alxResearchAreaInit();
    alxLD2UDInit();
    alxSedFittingInit();
}

/*___oOo___*/
