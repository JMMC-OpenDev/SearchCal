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
 * @param ra1 first right ascension in degree
 * @param dec1 first declination in degree
 * @param ra2 second right ascension in degree
 * @param dec2 second declination in degree
 * @param distance the already allocated distance result pointer, in arcsec
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
 */
mcsCOMPL_STAT alxComputeDistance(mcsDOUBLE ra1,
                                 mcsDOUBLE dec1,
                                 mcsDOUBLE ra2,
                                 mcsDOUBLE dec2,
                                 mcsDOUBLE* distance)
{
    FAIL(alxComputeDistanceInDegrees(ra1, dec1, ra2, dec2, distance));

    *distance *= alxDEG_IN_ARCSEC;

    return mcsSUCCESS;
}

/**
 * Compute the distance between to ra/dec coordinates.
 *
 * @param ra1 first right ascension in degree
 * @param dec1 first declination in degree
 * @param ra2 second right ascension in degree
 * @param dec2 second declination in degree
 * @param distance the already allocated distance result pointer, in degrees
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
 */
mcsCOMPL_STAT alxComputeDistanceInDegrees(mcsDOUBLE ra1,
                                          mcsDOUBLE dec1,
                                          mcsDOUBLE ra2,
                                          mcsDOUBLE dec2,
                                          mcsDOUBLE* distance)
{
    /* Check parameter validity */
    FAIL_NULL_DO(distance, 
                 errAdd(alxERR_NULL_PARAMETER, "distance"));

    /* Convert all the given angle from degrees to rad */
    ra1 *= alxDEG_IN_RAD;
    dec1 *= alxDEG_IN_RAD;
    ra2 *= alxDEG_IN_RAD;
    dec2 *= alxDEG_IN_RAD;

    /*
     * This implementation derives from Bob Chamberlain's contribution
     * to the comp.infosystems.gis FAQ; he cites
     * R.W.Sinnott, "Virtues of the Haversine", Sky and Telescope vol.68,
     * no.2, 1984, p159.
     */

    /* haversine formula: better precision than cosinus law */
    mcsDOUBLE sd2 = sin(0.5 * (dec2 - dec1));
    mcsDOUBLE sr2 = sin(0.5 * (ra2 - ra1));

    mcsDOUBLE angle = sd2 * sd2 + sr2 * sr2 * cos(dec1) * cos(dec2);

    /* check angle ranges [0;1] */
    *distance = (angle <= 0.0) ? 0.0 :
            ((angle < 1.0) ? 2.0 * asin(sqrt(angle)) * alxRAD_IN_DEG : 180.0);

    return mcsSUCCESS;
}


/*___oOo___*/
