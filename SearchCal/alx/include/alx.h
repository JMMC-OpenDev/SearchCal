#ifndef alx_H
#define alx_H
/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: alx.h,v 1.5 2005-02-10 07:55:31 gzins Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
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

/*
 * Pubic functions declaration
 */
mcsCOMPL_STAT alxComputeMagnitudesForBrightStar(mcsSTRING32 spType,
                                                mcsFLOAT mgB,
                                                mcsFLOAT mgV,
                                                mcsFLOAT *mgR,
                                                mcsFLOAT *mgI,
                                                mcsFLOAT *mgJ,
                                                mcsFLOAT *mgH,
                                                mcsFLOAT *mgK,
                                                mcsFLOAT *mgL,
                                                mcsFLOAT *mgM,
                                                alxCONFIDENCE_INDEX *confIdx);

mcsCOMPL_STAT alxComputeRealMagnitudes(mcsFLOAT plx,
                                       mcsFLOAT gLat,
                                       mcsFLOAT gLon,
                                       mcsFLOAT *mgM,
                                       mcsFLOAT *mgL,
                                       mcsFLOAT *mgK,
                                       mcsFLOAT *mgH,
                                       mcsFLOAT *mgJ,
                                       mcsFLOAT *mgI,
                                       mcsFLOAT *mgR,
                                       mcsFLOAT *mgV,
                                       mcsFLOAT *mgB);

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
#ifdef __cplusplus
}
#endif

#endif /*!alx_H*/

/*___oOo___*/
