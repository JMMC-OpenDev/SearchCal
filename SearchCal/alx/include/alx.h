#ifndef alx_H
#define alx_H
/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: alx.h,v 1.7 2005-02-21 19:30:05 gzins Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.6  2005/02/12 15:07:14  gzins
 * Added alxGetResearchAreaSize declaration
 *
 * Revision 1.5  2005/02/10 07:55:31  gzins
 * Updated alxComputeAngularDiameter to return diameter from B-V, V-R and V-K calibration
 *
 * Revision 1.4  2005/02/04 13:50:05  gzins
 * Re-defined confidence index
 *
 * Revision 1.3  2005/01/25 06:33:21  gzins
 * Removed wrong description for alxCONFIDENCE_INDEX
 *
 * Revision 1.2  2005/01/21 08:10:30  gluck
 * - Updated
 * - Insert automatic rcs log in header file
 *
 *
 * scetre    20-Sep-2004  Created
 * gzins     12-Jan-2005  - Update to be compliant with programming standards
 *                        - Move private declaration (structures, macros, ...)
 *                        in alxPrivate.h
 *
 ******************************************************************************/

/**
 * \file
 * Brief description of the header file, which ends at this dot.
 */

/* The following piece of code alternates the linkage type to C for all 
functions declared within the braces, which is necessary to use the 
functions in C++-code.
*/
#ifdef __cplusplus
extern "C" {
#endif

/*
 * MCS header
 */
#include "mcs.h"

/**
 * Confidence index.
 */
typedef enum
{
    alxCONFIDENCE_LOW = 0,  /** Low confidence in computed value */
    alxCONFIDENCE_MEDIUM,   /** Medium confidence in computed value */
    alxCONFIDENCE_HIGH      /** High confidence in computed value */
} alxCONFIDENCE_INDEX;

/**
 * Band for magnitude.
 */
#define alxNB_BANDS 9
typedef enum
{
    alxB_BAND = 0,  /** B-band */
    alxV_BAND,      /** V-band */
    alxR_BAND,      /** R-band */
    alxI_BAND,      /** I-band */
    alxJ_BAND,      /** J-band */
    alxH_BAND,      /** H-band */
    alxK_BAND,      /** K-band */
    alxL_BAND,      /** L-band */
    alxM_BAND       /** M-band */
} alxBAND;

/*
 * Pubic functions declaration
 */
mcsCOMPL_STAT
alxComputeMagnitudesForBrightStar(mcsSTRING32 spType, 
                                  mcsFLOAT magnitudes[alxNB_BANDS],
                                  alxCONFIDENCE_INDEX confIndexes[alxNB_BANDS]);

mcsCOMPL_STAT alxComputeRealMagnitudes(mcsFLOAT plx,
                                       mcsFLOAT gLat,
                                       mcsFLOAT gLon,
                                       mcsFLOAT magnitudes[alxNB_BANDS]);

mcsCOMPL_STAT alxComputeAngularDiameter(mcsFLOAT mgB,
                                        mcsFLOAT mgV,
                                        mcsFLOAT mgR,
                                        mcsFLOAT mgK,
                                        mcsFLOAT *diamBv,
                                        mcsFLOAT *diamVr,
                                        mcsFLOAT *diamVk,
                                        mcsFLOAT *diamError,
                                        alxCONFIDENCE_INDEX *confidenceIdx);

mcsCOMPL_STAT alxComputeGalacticCoordinates(mcsFLOAT ra,
                                            mcsFLOAT dec,
                                            mcsFLOAT *gLat,
                                            mcsFLOAT *gLon);

mcsCOMPL_STAT alxComputeVisibility(mcsFLOAT angDiam,
                                   mcsFLOAT angDiamError,
                                   mcsFLOAT baseMax,
                                   mcsFLOAT wlen,
                                   mcsFLOAT *vis,
                                   mcsFLOAT *vis2,
                                   mcsFLOAT *visError,
                                   mcsFLOAT *vis2Error);

mcsCOMPL_STAT alxGetResearchAreaSize(mcsFLOAT ra,
                                     mcsFLOAT dec,
                                     mcsFLOAT minMag,
                                     mcsFLOAT maxMag,
                                     mcsFLOAT *areaSize);

#ifdef __cplusplus
}
#endif

#endif /*!alx_H*/

/*___oOo___*/
