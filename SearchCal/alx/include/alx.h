#ifndef alx_H
#define alx_H
/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: alx.h,v 1.12 2005-04-04 07:22:51 scetre Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.11  2005/03/30 12:46:34  scetre
 * Added structure in order to simplify the code.
 * Changed API with this structure
 *
 * Revision 1.10  2005/02/22 16:17:53  gzins
 * Added errors for B-V, V-R and V-K diameters
 *
 * Revision 1.9  2005/02/22 10:15:32  gzins
 * Made alxBLANKING_VALUE definition public
 *
 * Revision 1.8  2005/02/22 07:51:39  gzins
 * Added alxNO_CONFIDENCE in alxCONFIDENCE_INDEX enumerate
 *
 * Revision 1.7  2005/02/21 19:30:05  gzins
 * Updated alxComputeMagnitudesForBrightStar and alxComputeRealMagnitudes prototypes
 *
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

/*
 * Blanking value.
 */
#define alxBLANKING_VALUE (float)99.99

/**
 * Confidence index.
 */
typedef enum
{
    alxNO_CONFIDENCE  = -1, /** No confidence in computed value     */
    alxCONFIDENCE_LOW,      /** Low confidence in computed value    */
    alxCONFIDENCE_MEDIUM,   /** Medium confidence in computed value */
    alxCONFIDENCE_HIGH      /** High confidence in computed value   */
} alxCONFIDENCE_INDEX;

/**
 * Band for magnitude.
 */
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
    alxM_BAND,      /** M-band */
    alxNB_BANDS
} alxBAND;

/**
 * Structure of a magnitude with his value, the confidence index associated and
 * a logical to know if the magnitude is set or not
 */
typedef struct
{
    mcsFLOAT value;
    alxCONFIDENCE_INDEX confIndex;
    mcsLOGICAL isSet; 
} alxMAGNITUDE;

/**
 * Stucture of alxNB_BANDS(9) magnitudes
 */
typedef alxMAGNITUDE alxMAGNITUDES[alxNB_BANDS];

/**
 * Structure of visibilities :
 *  - visibility
 *  - visError
 *  - vis²
 *  - vis²Error
 */
typedef struct
{
    mcsFLOAT vis;
    mcsFLOAT vis2;
    mcsFLOAT visError;
    mcsFLOAT vis2Error;
} alxVISIBILITIES;

/**
 * Structure of diameters :
 *  - diam B-V
 *  - diam V-R
 *  - diam V-K
 *  - diam B-V err
 *  - diam V-R err
 *  - diam V-K err
 *  - the confidence index associated
 */
typedef struct
{
    mcsFLOAT bv;
    mcsFLOAT vr;
    mcsFLOAT vk;
    mcsFLOAT bvErr;
    mcsFLOAT vrErr;
    mcsFLOAT vkErr;
    alxCONFIDENCE_INDEX confidenceIdx;
} alxDIAMETERS;

/*
 * Pubic functions declaration
 */
mcsCOMPL_STAT alxComputeMagnitudesForBrightStar(mcsSTRING32 spType, 
                                                alxMAGNITUDES magnitudes);

mcsCOMPL_STAT alxComputeRealMagnitudes(mcsFLOAT plx,
                                       mcsFLOAT gLat,
                                       mcsFLOAT gLon,
                                       alxMAGNITUDES magnitudes);

mcsCOMPL_STAT alxComputeAngularDiameter(mcsFLOAT mgB,
                                        mcsFLOAT mgV,
                                        mcsFLOAT mgR,
                                        mcsFLOAT mgK,
                                        alxDIAMETERS *diameters);

mcsCOMPL_STAT alxComputeGalacticCoordinates(mcsFLOAT ra,
                                            mcsFLOAT dec,
                                            mcsFLOAT *gLat,
                                            mcsFLOAT *gLon);

mcsCOMPL_STAT alxComputeVisibility(mcsFLOAT angDiam,
                                   mcsFLOAT angDiamError,
                                   mcsFLOAT baseMax,
                                   mcsFLOAT wlen,
                                   alxVISIBILITIES *visibilities);

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
