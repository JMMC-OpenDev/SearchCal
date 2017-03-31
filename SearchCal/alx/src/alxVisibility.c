/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * @file
 * Function definition for visibility computation.
 *
 * @sa JMMC-MEM-2600-0005 document
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
 * Compute star visibility.
 *
 * This method computes the potential calibrator visibility according to the
 * angular diameter, the length of the maximal baseline and the wavelength
 *
 * @param angDiam angular diameter of the star (mas)
 * @param angDiamError relative error on the angular diameter
 * @param baseMax maximal baseline length(m)
 * @param wlen wavelength (nm)
 * @param visibilities the computed visibilities (vis, visErr, vis2, vis2Err)
 *
 * @return Always mcsSUCCESS
 */
mcsCOMPL_STAT alxComputeVisibility(mcsDOUBLE angDiam,
                                   mcsDOUBLE angDiamError,
                                   mcsDOUBLE baseMax,
                                   mcsDOUBLE wlen,
                                   alxVISIBILITIES *visibilities)
{
    mcsDOUBLE x = 15.23 * baseMax * angDiam / (1000.0 * wlen);

    /* Compute V */
    visibilities->vis = 2.0 * fabs(j1f(x) / x);

    /* and its associated error for Diameter Uniform Disc */
    visibilities->visError = 2.0 * fabs(jnf(2.0, x) * angDiamError / angDiam);

    /* Compute V2 */
    visibilities->vis2 = visibilities->vis * visibilities->vis;

    /* and its associated error for Diameter Uniform Disc */
    /* d(Vis2) = 2 x V x dV */
    visibilities->vis2Error = 2.0 * visibilities->vis * visibilities->visError;

    /* Print out result */
    logTest("Diam=%.3lf (%.3lf) - base=%.1lf - wlen=%.3lf",
            angDiam, angDiamError, baseMax, wlen);

    logTest("V(dV), V2(dV2)= %.6lf (%.6lf - %.2lf %), %.6lf (%.6lf - %.2lf %)",
            visibilities->vis, visibilities->visError,
            100. * (visibilities->visError / visibilities->vis),
            visibilities->vis2, visibilities->vis2Error,
            100. * (visibilities->vis2Error / visibilities->vis2)
            );

    return mcsSUCCESS;
}


/*___oOo___*/
