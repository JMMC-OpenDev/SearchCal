/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * @file
 * Function definition for computation of the distance between 2 ra/dec
 * coordinates.
 */



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
#include "alxErrors.h"
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
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is
 * returned.
 */
mcsCOMPL_STAT alxComputeDistance(mcsDOUBLE  ra1,
                                 mcsDOUBLE  dec1,
                                 mcsDOUBLE  ra2,
                                 mcsDOUBLE  dec2,
                                 mcsDOUBLE* distance)
{
    logTrace("alxComputeDistance()");

    /* Check parameter validity */
    if (distance == NULL)
    {
        errAdd(alxERR_NULL_PARAMETER, "distance");
        return mcsFAILURE;
    }

    /* Convert all the given angle from degrees to rad */
    mcsDOUBLE _ra1  = ra1  / (180 / M_PI);
    mcsDOUBLE _dec1 = dec1 / (180 / M_PI);
    mcsDOUBLE _ra2  = ra2  / (180 / M_PI);
    mcsDOUBLE _dec2 = dec2 / (180 / M_PI);

    /* Compute the cosinus of the distance */
    mcsDOUBLE cosDistance = 
       (sin(_dec1) * sin(_dec2)) + (cos(_dec1) * cos(_dec2) * cos(_ra1 - _ra2));

    /*
     * Due to computation precision, it is possible that cosTheta became greater
     * than 1.0. In this case, normalize it to 1.
     */
    if (cosDistance > 1.0)
    {
        cosDistance = 1.0;
    }
    if (cosDistance < -1.0)
    {
        cosDistance = -1.0;
    }
    
    /* Compute the distance theta in arcseconds */
    *distance = (3600 * acos(cosDistance)) / (M_PI / 180);
    
    return mcsSUCCESS;
}


/*___oOo___*/
