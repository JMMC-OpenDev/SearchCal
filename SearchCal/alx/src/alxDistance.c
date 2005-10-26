/*******************************************************************************
 * JMMC project
 * 
 * "@(#) $Id: alxDistance.c,v 1.1 2005-10-26 11:30:41 lafrasse Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 ******************************************************************************/

/**
 * @file
 * Function definition for computation of the distance between 2 ra/dec
 * coordinates.
 */

static char *rcsId="@(#) $Id: alxDistance.c,v 1.1 2005-10-26 11:30:41 lafrasse Exp $"; 
static void *use_rcsId = ((void)&use_rcsId,(void *) &rcsId);


/* 
 * System Headers
 */
#include <stdio.h>
#define __USE_MISC
#include <math.h>


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
/**
 * Compute the distance between to ra/dec coordinates.
 *
 * @param ra1 first right acsension in degree
 * @param dec1 first declinaison in degree
 * @param ra2 second right acsension in degree
 * @param dec2 second declinaison in degree
 * @param distance the already allocated distance result pointer, in arcseconds
 *
 * @return Always mcsSUCCESS
 */
mcsCOMPL_STAT alxComputeDistance(mcsFLOAT ra1,
                                 mcsFLOAT dec1,
                                 mcsFLOAT ra2,
                                 mcsFLOAT dec2,
                                 mcsFLOAT *distance)
{
    logTrace("alxComputeDistance()");

    /* Compute the cosinus of the distance */
    mcsFLOAT cosTheta = 
             (sin(dec1) * sin(dec2)) + (cos(dec1) * cos(dec2) * cos(ra1 - ra2));

    /*
     * Due to computation precision, it is possible than cosTheta is greater
     * than 1.0.
     * In this case, force it to 1.
     */
    if (cosTheta > 1.0)
    {
        cosTheta = 1.0;
    }
    if (cosTheta < -1.0)
    {
        cosTheta = -1.0;
    }
    
    /* Compute the distance theta in arcseconds */
    *distance = 3600 * acos(cosTheta) / (M_PI/180);
    
    return mcsSUCCESS;
}


/*___oOo___*/
