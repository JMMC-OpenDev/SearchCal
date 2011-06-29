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
    logTrace("alxComputeVisibility()");
    
    mcsDOUBLE x = 15.23 * baseMax * angDiam / (1000 * wlen);

    /* Compute V */
    visibilities->vis = fabs(2 * j1f(x) / x);

    /* and its assosiated error for Diameter Uniform Disc */
    visibilities->visError  = 2 * jnf(2, x) * angDiamError / angDiam;

    /* Compute V� */
    visibilities->vis2 = pow(visibilities->vis, 2);

    /* and its assosiated error for Diameter Uniform Disc */
    visibilities->vis2Error = 
        8 * jnf(2, x) * fabs(j1f(x)/x) * angDiamError / angDiam;
    
    /* Print out result */
    logTest("Diam= %.3f(%.3f) - base = %.1f - wlen= %.3f", angDiam,
            angDiamError, baseMax, wlen);
    logTest("V   = %.6f", visibilities->vis);
    logTest("dV  = %.6f", visibilities->visError);
    logTest("V2  = %.6f", visibilities->vis2);
    logTest("dV2 = %.6f", visibilities->vis2Error);

    return mcsSUCCESS;
}


/*___oOo___*/
