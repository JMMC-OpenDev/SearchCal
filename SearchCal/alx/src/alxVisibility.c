/*******************************************************************************
 * JMMC project
 * 
 * "@(#) $Id: alxVisibility.c,v 1.9 2005-10-26 11:24:01 lafrasse Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.8  2005/06/01 14:16:07  scetre
 * Changed logExtDbg to logTrace
 *
 * Revision 1.7  2005/04/04 07:22:11  scetre
 * Updated documentation
 *
 * Revision 1.6  2005/03/30 12:48:21  scetre
 * *** empty log message ***
 *
 * Revision 1.5  2005/02/23 09:11:48  gzins
 * Added log for test
 *
 * Revision 1.4  2005/02/18 08:29:42  scetre
 * Fixed Bug in dV compute
 *
 * Revision 1.3  2005/02/18 08:19:42  scetre
 * Fixed Bug in visibility error compute
 *
 * Revision 1.2  2005/02/17 19:04:16  gzins
 * Updated formulas to compute dV and dV²
 *
 * Revision 1.1  2005/01/21 08:14:25  gluck
 * Creation
 *
 *
 * scetre    19-Oct-2004  Created
 * gzins     12-Jan-2005  - Updated to be compliant with programming standards
 *                        - Update formula to be coherent with specification 
 *                          document
 *
 ******************************************************************************/

/**
 * @file
 * Function definition for visibility computation.
 *
 * @sa JMMC-MEM-2600-0005 document 
 */

static char *rcsId="@(#) $Id: alxVisibility.c,v 1.9 2005-10-26 11:24:01 lafrasse Exp $"; 
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
mcsCOMPL_STAT alxComputeVisibility(mcsFLOAT angDiam,
                                   mcsFLOAT angDiamError,
                                   mcsFLOAT baseMax,
                                   mcsFLOAT wlen,
                                   alxVISIBILITIES *visibilities)
{
    logTrace("alxComputeVisibility()");
    
    mcsFLOAT x = 15.23 * baseMax * angDiam / (1000 * wlen);

    /* Compute V */
    visibilities->vis = fabs(2 * j1f(x) / x);

    /* and its assosiated error for Diameter Uniform Disc */
    visibilities->visError  = 2 * jnf(2, x) * angDiamError / angDiam;

    /* Compute V² */
    visibilities->vis2 = pow(visibilities->vis, 2);

    /* and its assosiated error for Diameter Uniform Disc */
    visibilities->vis2Error = 
        8 * jnf(2, x) * fabs(j1f(x)/x) * angDiamError / angDiam;
    
    /* Print out result */
    logTest("Diam= %.3f(%.3f) - base = %.1f - wlen= %.3f", angDiam,
            angDiamError, baseMax, wlen);
    logTest("V   = %.6f", visibilities->vis);
    logTest("dV  = %.6f", visibilities->visError);
    logTest("V²  = %.6f", visibilities->vis2);
    logTest("dV² = %.6f", visibilities->vis2Error);

    return mcsSUCCESS;
}


/*___oOo___*/
