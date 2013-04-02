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

/* Needed for FP_NAN support */
#define  __USE_ISOC99 1
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
 * Say if a cell value get a blanking value or not.
 *
 * If the cell value = '99.99'(alxBLANKING_VALUE), return true.
 *
 * @param cellValue the value of the cell
 *
 * @return mcsTRUE if cell value == alxBLANKING_VALUE, otherwise mcsFALSE is 
 * returned.
 */
mcsLOGICAL alxIsBlankingValue(mcsDOUBLE cellValue)
{
    if (cellValue == (mcsDOUBLE) alxBLANKING_VALUE)
    {
        return mcsTRUE;
    }

    return mcsFALSE;
}

mcsCOMPL_STAT alxLogTestMagnitudes(const char* line, alxMAGNITUDES magnitudes)
{
    /* Print out results */
    logTest("%s B=%0.2lf(%0.2lf,%s), V=%0.2lf(%0.2lf,%s), "
            "R=%0.2lf(%0.2lf,%s), I=%0.2lf(%0.2lf,%s), J=%0.2lf(%0.2lf,%s), H=%0.2lf(%0.2lf,%s), "
            "K=%0.2lf(%0.2lf,%s), L=%0.2lf(%0.2lf,%s), M=%0.2lf(%0.2lf,%s)",
	    line,
            magnitudes[alxB_BAND].value, magnitudes[alxB_BAND].error, alxGetConfidenceIndex(magnitudes[alxB_BAND].confIndex),
            magnitudes[alxV_BAND].value, magnitudes[alxV_BAND].error, alxGetConfidenceIndex(magnitudes[alxV_BAND].confIndex),
            magnitudes[alxR_BAND].value, magnitudes[alxR_BAND].error, alxGetConfidenceIndex(magnitudes[alxR_BAND].confIndex),
            magnitudes[alxI_BAND].value, magnitudes[alxI_BAND].error, alxGetConfidenceIndex(magnitudes[alxI_BAND].confIndex),
            magnitudes[alxJ_BAND].value, magnitudes[alxJ_BAND].error, alxGetConfidenceIndex(magnitudes[alxJ_BAND].confIndex),
            magnitudes[alxH_BAND].value, magnitudes[alxH_BAND].error, alxGetConfidenceIndex(magnitudes[alxH_BAND].confIndex),
            magnitudes[alxK_BAND].value, magnitudes[alxK_BAND].error, alxGetConfidenceIndex(magnitudes[alxK_BAND].confIndex),
            magnitudes[alxL_BAND].value, magnitudes[alxL_BAND].error, alxGetConfidenceIndex(magnitudes[alxL_BAND].confIndex),
            magnitudes[alxM_BAND].value, magnitudes[alxM_BAND].error, alxGetConfidenceIndex(magnitudes[alxM_BAND].confIndex));

    return mcsSUCCESS;
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
}

/*___oOo___*/
