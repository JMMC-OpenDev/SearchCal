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
 * equatorial coordinates (right ascension and declinaison).@n
 *
 * @param ra right acsension J2000 in degree
 * @param dec declinaison J2000 in degree
 * @param gLat galactic latitude in degree in range [0, 360]
 * @param gLon galactic longitude in degree in range [-90, 90]
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is
 * returned.
 */
mcsCOMPL_STAT alxComputeGalacticCoordinates(mcsDOUBLE ra,
                                            mcsDOUBLE dec,
                                            mcsDOUBLE *gLat,
                                            mcsDOUBLE *gLon)
{
    logTrace("alxComputeGalacticCoordinates()");

    mcsDOUBLE b;         /* galactic latitude */
    mcsDOUBLE sinusB;    /* galactic latitude sinus */
    mcsDOUBLE x;         /* cos(l-0.574737), l=galactic longitude */
    mcsDOUBLE y;         /* sin(l-0.574737), l=galactic longitude */

    /* Convert ra from degrees (decimal) to radians */
    ra = ra * M_PI / 180;

    /* Convert dec from degrees (decimal) to radians */
    dec = dec * M_PI / 180;

    /* Calculate galactic latitude sinus with radian parameters */
    sinusB =   (sin(dec) * cos(1.097288))
             - (cos(dec) * sin(ra-4.936838) * sin(1.097288));

    /* 
     * Compute galactic latitude in degrees (gLat)
     */
    /* Calculate galactic latitude and convert it to degrees */
    *gLat = asin(sinusB) * 180 / M_PI;

    /* Round value with 0.01 degree precision */
    *gLat = (rint(*gLat * 100)/100.0);

    /*
     * Be sure that galactic latitude is in the validity domain [-90, 90]
     */
    /* Be sure to have galactic latitude in [0, 360] */
    *gLat = fmod(*gLat, 360.0);
    /* if gLat is in ]90 - 270] => put it in [-90, 90] */
    if (*gLat > 90 && *gLat <= 270)
    {
        *gLat = 180 - *gLat;
    }
    /* if gLat is in ]270 - 360] => put it in negative value [0, -90] */
    if (*gLat > 270 && *gLat <= 360)
    {
        *gLat = *gLat - 360;
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
    x = cos(dec) * cos(ra-4.936838) / cos(b);
    y = (cos(dec) * sin(ra-4.936838) * cos(1.097288) + sin(dec) * sin(1.097288)) 
        / cos(b);

    /* Calculate galactic longitude and convert it to degrees */
    *gLon = (atan2(y, x) + 0.574737) * 180 / M_PI;

    /* Round value with 0.01 degree precision */
    *gLon = (rint(*gLon * 100)/100.0);

    /*
     * Be sure that galactic longitude is in the validity domain [0, 360[
     */
    /* Be sure to have galactic latitude in [0, 360] */
    *gLon = fmod(*gLon, 360.0);
    /* if gLon has negative value => put it in positive value */
    if (*gLon < 0)
    {
        *gLon += 360;
    }
    
    logTest("ra/dec - gLat/gLon = %f / %f radians - %.3f / %.3f degrees", ra, dec, *gLat, *gLon);

    return mcsSUCCESS;
}

/*___oOo___*/
