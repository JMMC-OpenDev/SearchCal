/*******************************************************************************
 * JMMC project
 * 
 * "@(#) $Id: alxApparentMagnitude.c,v 1.1 2005-12-22 10:08:58 scetre Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 ******************************************************************************/

/**
 * @file
 * Function definition for apparent magnitude computation.
 *
 * @sa JMMC-SPE-2600-0001 document.
 */

static char *rcsId="@(#) $Id: alxApparentMagnitude.c,v 1.1 2005-12-22 10:08:58 scetre Exp $"; 
static void *use_rcsId = ((void)&use_rcsId,(void *) &rcsId);


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
mcsCOMPL_STAT alxComputeApparentMagnitudes(mcsFLOAT av,
                                           alxMAGNITUDES magnitudes)
{
    logTrace("alxComputeApparentMagnitudes()");

    /* compute B */
    if (magnitudes[alxB_BAND].isSet == mcsTRUE)
    {
        magnitudes[alxB_BAND].value = magnitudes[alxB_BAND].value + 1.32 * av;
    }
    logTest("Corrected magnitude[B] = %0.3f", magnitudes[alxB_BAND].value);
    
    /* compute V */
    if (magnitudes[alxV_BAND].isSet == mcsTRUE)
    {
        magnitudes[alxV_BAND].value = magnitudes[alxV_BAND].value + av;
    }
    logTest("Corrected magnitude[V] = %0.3f", magnitudes[alxV_BAND].value);

    /* compute R */
    if (magnitudes[alxR_BAND].isSet == mcsTRUE)
    {
        magnitudes[alxR_BAND].value = magnitudes[alxR_BAND].value + 0.748 * av;
    }
    logTest("Corrected magnitude[R] = %0.3f", magnitudes[alxR_BAND].value);

    /* compute I */
    if (magnitudes[alxI_BAND].isSet == mcsTRUE)
    {
        magnitudes[alxI_BAND].value = magnitudes[alxI_BAND].value + 0.506 * av;
    }
    logTest("Corrected magnitude[I] = %0.3f", magnitudes[alxI_BAND].value);

    /* compute J */
    if (magnitudes[alxJ_BAND].isSet == mcsTRUE)
    {
        magnitudes[alxJ_BAND].value = magnitudes[alxJ_BAND].value + 2.777 * av;
    }
    logTest("Corrected magnitude[J] = %0.3f", magnitudes[alxJ_BAND].value);

    /* compute H */
    if (magnitudes[alxH_BAND].isSet == mcsTRUE)
    {
        magnitudes[alxH_BAND].value = magnitudes[alxH_BAND].value + 0.171 * av;
    }
    logTest("Corrected magnitude[H] = %0.3f", magnitudes[alxH_BAND].value);

    /* compute K */
    if (magnitudes[alxK_BAND].isSet == mcsTRUE)
    {
        magnitudes[alxK_BAND].value = magnitudes[alxK_BAND].value + 0.116 * av;
    }
    logTest("Corrected magnitude[K] = %0.3f", magnitudes[alxK_BAND].value);

    /* compute L */
    if (magnitudes[alxL_BAND].isSet == mcsTRUE)
    {
        magnitudes[alxL_BAND].value = magnitudes[alxL_BAND].value + 0.061 * av;
    }
    logTest("Corrected magnitude[L] = %0.3f", magnitudes[alxL_BAND].value);

    /* compute M */
    if (magnitudes[alxM_BAND].isSet == mcsTRUE)
    {
        magnitudes[alxM_BAND].value = magnitudes[alxM_BAND].value + 0.039 * av;
    }
    logTest("Corrected magnitude[M] = %0.3f", magnitudes[alxM_BAND].value);
    
    return mcsSUCCESS;
}




/*___oOo___*/
