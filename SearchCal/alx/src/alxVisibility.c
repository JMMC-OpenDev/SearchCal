/*******************************************************************************
 * JMMC project
 * 
 * "@(#) $Id: alxVisibility.c,v 1.3 2005-02-18 08:19:42 scetre Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
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
 * \file
 * Function definition for visibility computation.
 *
 * \sa JMMC-MEM-2600-0005 document 
 * 
 */

static char *rcsId="@(#) $Id: alxVisibility.c,v 1.3 2005-02-18 08:19:42 scetre Exp $"; 
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
 * \param angDiam angular diameter of the star (mas)
 * \param angDiamError relative error on the angular diameter
 * \param baseMax maximal baseline(m)
 * \param wlen wavelength (nm)
 * \param vis the computed visibility
 * \param vis2 the computed square visibility
 * \param visError error on computed visibility
 * \param vis2Error error on computed square visibility
 *
 * \return Always mcsSUCCESS
 */
mcsCOMPL_STAT alxComputeVisibility(mcsFLOAT angDiam,
                                   mcsFLOAT angDiamError,
                                   mcsFLOAT baseMax,
                                   mcsFLOAT wlen,
                                   mcsFLOAT *vis,
                                   mcsFLOAT *vis2,
                                   mcsFLOAT *visError,
                                   mcsFLOAT *vis2Error)
{
    logExtDbg("alxComputeVisibility()");
    
    mcsFLOAT x;
    x = 15.23 * baseMax * angDiam / (1000 * wlen);

    /* Compute V */
    *vis = fabs(2 * j1f(x) / x);

    /* and its assosiated error for Diameter Uniform Disc */
    *visError  = 2 * jnf(2, x) * angDiamError;

    /* Compute V² */
    *vis2 = pow(*vis, 2);

    /* and its assosiated error for Diameter Uniform Disc */
    *vis2Error = 8 * jnf(2, x) * fabs(j1f(x)/x) * angDiamError / angDiam;
    
    /* Print out result */
    logTest("V   = %.6f", *vis);
    logTest("dV  = %.6f", *visError);
    logTest("V²  = %.6f", *vis2);
    logTest("dV² = %.6f", *vis2Error);

    return mcsSUCCESS;
}




/*___oOo___*/
