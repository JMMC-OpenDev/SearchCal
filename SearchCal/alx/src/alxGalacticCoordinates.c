/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * @file
 * Function definition for galactic coordinate computation.
 *
 * @sa file : JMMC-MEM-2600-0007
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
#include "alxPrivate.h"


/*
 * Public functions definition
 */

/**
 * Compute galactic coordinates (longitude and latitude).
 *
 * Galactic coordinates (longitude and latitude) are necessary to calculate
 * interstellar absorption and to determine calibrators search field size. If
 * these ones are not obtained from catalogs, they can be calculated from
 * equatorial coordinates (right ascension and declination).@n
 *
 * @param ra right ascension J2000 in degree
 * @param dec declination J2000 in degree
 * @param gLat galactic latitude in degree in range [0, 360]
 * @param gLon galactic longitude in degree in range [-90, 90]
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
 */
mcsCOMPL_STAT alxComputeGalacticCoordinates(mcsDOUBLE ra,
                                            mcsDOUBLE dec,
                                            mcsDOUBLE *gLat,
                                            mcsDOUBLE *gLon)
{
    mcsDOUBLE b; /* galactic latitude */
    mcsDOUBLE sinusB; /* galactic latitude sinus */
    mcsDOUBLE x; /* cos(l-0.574737), l=galactic longitude */
    mcsDOUBLE y; /* sin(l-0.574737), l=galactic longitude */

    /* Convert ra from degrees (decimal) to radians */
    ra *= alxDEG_IN_RAD;

    /* Convert dec from degrees (decimal) to radians */
    dec *= alxDEG_IN_RAD;

    /* Calculate galactic latitude sinus with radian parameters */
    sinusB = (sin(dec) * cos(1.097288)) - (cos(dec) * sin(ra - 4.936838) * sin(1.097288));

    /*
     * Compute galactic latitude in degrees (gLat)
     */
    /* Calculate galactic latitude and convert it to degrees */
    *gLat = asin(sinusB) * alxRAD_IN_DEG;

    /*
     * Be sure that galactic latitude is in the validity domain [-90, 90]
     */
    /* Be sure to have galactic latitude in [0, 360] */
    *gLat = fmod(*gLat, 360.0);

    /* if gLat is in ]90 - 270] => put it in [-90, 90] */
    if ((*gLat > 90.0) && (*gLat <= 270.0))
    {
        *gLat = 180.0 - *gLat;
    }

    /* if gLat is in ]270 - 360] => put it in negative value [0, -90] */
    if ((*gLat > 270.0) && (*gLat <= 360.0))
    {
        *gLat = *gLat - 360.0;
    }

    /*
     * Compute galactic longitude in degrees (gLon)
     */
    /* Calculate cos(b) */
    b = asin(sinusB);

    /* Calculate longitude with radian parameters
     * gLon = atan(y / x) + 0.574737
     *
     * where
     * x = cos(dec) cos(ra-4.936838) / cos(b)
     * y = (cos(dec) sin(ra-4.936838) cos(1.097288) +
     *      sin(dec) sin(1.097288)) / cos(b)
     */
    x = cos(dec) * cos(ra - 4.936838) / cos(b);
    y = (cos(dec) * sin(ra - 4.936838) * cos(1.097288) + sin(dec) * sin(1.097288)) / cos(b);

    /* Calculate galactic longitude and convert it to degrees */
    *gLon = (atan2(y, x) + 0.574737) * alxRAD_IN_DEG;

    /*
     * Be sure that galactic longitude is in the validity domain [0, 360[
     */
    /* Be sure to have galactic latitude in [0, 360] */
    *gLon = fmod(*gLon, 360.0);

    /* if gLon has negative value => put it in positive value */
    if (*gLon < 0.0)
    {
        *gLon += 360.0;
    }

    logTest("RA/DEC= %lf / %lf rad GLat/GLon= %.3lf / %.3lf deg", ra, dec, *gLat, *gLon);

    return mcsSUCCESS;
}

/*___oOo___*/
