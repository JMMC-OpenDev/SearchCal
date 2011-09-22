/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * @file
 * Function definition for apparent magnitude computation.
 *
 * @sa JMMC-SPE-2600-0001 document.
 */



/* 
 * System Headers
 */
#include <stdio.h>


/*
 * MCS Headers 
 */
#include "mcs.h"
#include "log.h"
#include "err.h"


/* 
 * Local Headers
 */
#include "alx.h"
#include "alxPrivate.h"


/*
 * Public functions definition
 */
mcsCOMPL_STAT alxComputeApparentMagnitudes(mcsDOUBLE av,
                                           alxMAGNITUDES magnitudes)
{
    logTrace("alxComputeApparentMagnitudes()");

    /* compute B */
    if (magnitudes[alxB_BAND].isSet == mcsTRUE)
    {
        magnitudes[alxB_BAND].value = magnitudes[alxB_BAND].value + 1.32 * av;
    }
    
    /* compute V */
    if (magnitudes[alxV_BAND].isSet == mcsTRUE)
    {
        magnitudes[alxV_BAND].value = magnitudes[alxV_BAND].value + av;
    }

    /* compute R */
    if (magnitudes[alxR_BAND].isSet == mcsTRUE)
    {
        magnitudes[alxR_BAND].value = magnitudes[alxR_BAND].value + 0.748 * av;
    }

    /* compute I */
    if (magnitudes[alxI_BAND].isSet == mcsTRUE)
    {
        magnitudes[alxI_BAND].value = magnitudes[alxI_BAND].value + 0.506 * av;
    }

    /* compute J */
    if (magnitudes[alxJ_BAND].isSet == mcsTRUE)
    {
        magnitudes[alxJ_BAND].value = magnitudes[alxJ_BAND].value + 2.777 * av;
    }

    /* compute H */
    if (magnitudes[alxH_BAND].isSet == mcsTRUE)
    {
        magnitudes[alxH_BAND].value = magnitudes[alxH_BAND].value + 0.171 * av;
    }

    /* compute K */
    if (magnitudes[alxK_BAND].isSet == mcsTRUE)
    {
        magnitudes[alxK_BAND].value = magnitudes[alxK_BAND].value + 0.116 * av;
    }

    /* compute L */
    if (magnitudes[alxL_BAND].isSet == mcsTRUE)
    {
        magnitudes[alxL_BAND].value = magnitudes[alxL_BAND].value + 0.061 * av;
    }

    /* compute M */
    if (magnitudes[alxM_BAND].isSet == mcsTRUE)
    {
        magnitudes[alxM_BAND].value = magnitudes[alxM_BAND].value + 0.039 * av;
    }
    
    logTest("Apparent magnitudes: B = %0.3f, V = %0.3f, R = %0.3f, I = %0.3f, J = %0.3f, H = %0.3f, K = %0.3f, L = %0.3f, M = %0.3f", 
            magnitudes[alxB_BAND].value, 
            magnitudes[alxV_BAND].value, 
            magnitudes[alxR_BAND].value, 
            magnitudes[alxI_BAND].value, 
            magnitudes[alxJ_BAND].value, 
            magnitudes[alxH_BAND].value, 
            magnitudes[alxK_BAND].value, 
            magnitudes[alxL_BAND].value, 
            magnitudes[alxM_BAND].value);
    
    return mcsSUCCESS;
}

/*___oOo___*/
