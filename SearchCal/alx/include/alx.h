#ifndef alx_H
#define alx_H
/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: alx.h,v 1.28 2011-03-03 12:59:53 lafrasse Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.27  2010/02/18 12:07:00  lafrasse
 * Retrieve Teff and LogG in alxComputeUDFromLDAndSP().
 *
 * Revision 1.26  2010/01/28 16:20:12  lafrasse
 * Added UD_U diameter in alxUNIFORM_DIAMETERS.
 *
 * Revision 1.25  2010/01/18 15:52:37  lafrasse
 * Added alxShowUNIFORM_DIAMETERS().
 *
 * Revision 1.24  2010/01/08 22:29:04  lafrasse
 * Added preliminary support for alxLD2UD.
 *
 * Revision 1.23  2009/10/27 09:58:58  lafrasse
 * Added a constant to define what is an arcsec in degrees.
 *
 * Revision 1.22  2007/05/11 15:30:29  gzins
 * Upadated alxComputeAngularDiameterForFaintStar prototype
 *
 * Revision 1.21  2006/07/17 08:22:20  scetre
 * Added hk diameter in alxDIAMETERS structure
 *
 * Revision 1.20  2006/04/19 12:07:16  gzins
 * Changed areComputed to areCoherent (for diameters)
 *
 * Revision 1.19  2005/12/22 10:07:53  scetre
 * Added extinction coefficient computation
 * changed realMag to CorrectedMag
 *
 * Revision 1.18  2005/12/07 14:48:33  scetre
 * Renamed function for bright and faint case
 *
 * Revision 1.17  2005/12/05 16:00:22  scetre
 * Added computing of mean diameter and associated error
 *
 * Revision 1.16  2005/12/02 12:03:14  scetre
 * Added compute missing magnitude in faint
 *
 * Revision 1.15  2005/11/30 10:47:59  scetre
 * Added computing diameter method for faint
 *
 * Revision 1.14  2005/10/26 11:24:01  lafrasse
 * Code review
 *
 * Revision 1.13  2005/04/06 12:12:56  scetre
 * Changed used of float for properties to computed in alxDATA
 *
 * Revision 1.12  2005/04/04 07:22:51  scetre
 * alxDIFFERIANTIAL_MAGNITUDES become a private structure
 *
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
 * @file
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


/** Blanking value. */
#define alxBLANKING_VALUE ((mcsDOUBLE)99.99)

/** 1 arcsec in degrees. */
#define alxARCSEC_IN_DEGREES ((mcsDOUBLE)(1.0/3600.0))


/**
 * Computed value confidence index.
 */
typedef enum
{
    alxNO_CONFIDENCE  = -1, /** No confidence     */
    alxCONFIDENCE_LOW,      /** Low confidence    */
    alxCONFIDENCE_MEDIUM,   /** Medium confidence */
    alxCONFIDENCE_HIGH      /** High confidence   */
} alxCONFIDENCE_INDEX;


/**
 * Bands for magnitude.
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
 * Structure of a magnitude with its value, the confidence index associated, and
 * a boolean flag to store wether the magnitude is set or not
 */
typedef struct
{
    mcsDOUBLE            value;
    alxCONFIDENCE_INDEX confIndex;
    mcsLOGICAL          isSet; 
} alxDATA;


/**
 * Stucture of alxNB_BANDS(9) magnitudes
 */
typedef alxDATA alxMAGNITUDES[alxNB_BANDS];


/**
 * Structure of visibilities :
 *  - visibility
 *  - visError
 *  - vis²
 *  - vis²Error
 */
typedef struct
{
    mcsDOUBLE vis;
    mcsDOUBLE vis2;
    mcsDOUBLE visError;
    mcsDOUBLE vis2Error;
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
    alxDATA bv;
    alxDATA vr;
    alxDATA vk;
    alxDATA ij;
    alxDATA ik;
    alxDATA jk;
    alxDATA jh;
    alxDATA hk;
    alxDATA mean;
    alxDATA bvErr;
    alxDATA vrErr;
    alxDATA vkErr;
    alxDATA ijErr;
    alxDATA ikErr;
    alxDATA jhErr;
    alxDATA jkErr;
    alxDATA hkErr;
    alxDATA meanErr;
    alxCONFIDENCE_INDEX confidenceIdx;
    mcsLOGICAL areCoherent;
} alxDIAMETERS;

/** Structure holding uniform diameters */
typedef struct
{
    mcsDOUBLE Teff;
    mcsDOUBLE LogG;

    mcsDOUBLE b;
    mcsDOUBLE i;
    mcsDOUBLE j;
    mcsDOUBLE h;
    mcsDOUBLE k;
    mcsDOUBLE l;
    mcsDOUBLE n;
    mcsDOUBLE r;
    mcsDOUBLE u;
    mcsDOUBLE v;

} alxUNIFORM_DIAMETERS;


/*
 * Pubic functions declaration
 */
mcsCOMPL_STAT alxComputeMagnitudesForBrightStar(mcsSTRING32 spType, 
                                                alxMAGNITUDES magnitudes);

mcsCOMPL_STAT alxComputeMagnitudesForFaintStar(mcsSTRING32 spType, 
                                               alxMAGNITUDES magnitudes); 

mcsCOMPL_STAT alxComputeCorrectedMagnitudes(mcsDOUBLE av,
                                            alxMAGNITUDES magnitudes);

mcsCOMPL_STAT alxComputeApparentMagnitudes(mcsDOUBLE av,
                                           alxMAGNITUDES magnitudes);

mcsCOMPL_STAT alxComputeAngularDiameterForBrightStar(alxDATA mgB,
                                        alxDATA mgV,
                                        alxDATA mgR,
                                        alxDATA mgK,
                                        alxDIAMETERS* diameters);

mcsCOMPL_STAT alxComputeAngularDiameterForFaintStar(alxDATA mgI,
                                             alxDATA mgJ,
                                             alxDATA mgK,
                                             alxDATA mgH,
                                             alxDATA mgV,
                                             alxDATA mgKJnk,
                                             alxDIAMETERS* diameters);

mcsCOMPL_STAT alxComputeGalacticCoordinates(mcsDOUBLE ra,
                                            mcsDOUBLE dec,
                                            mcsDOUBLE* gLat,
                                            mcsDOUBLE* gLon);

mcsCOMPL_STAT alxComputeVisibility(mcsDOUBLE angDiam,
                                   mcsDOUBLE angDiamError,
                                   mcsDOUBLE baseMax,
                                   mcsDOUBLE wlen,
                                   alxVISIBILITIES* visibilities);

mcsCOMPL_STAT alxGetResearchAreaSize(mcsDOUBLE ra,
                                     mcsDOUBLE dec,
                                     mcsDOUBLE minMag,
                                     mcsDOUBLE maxMag,
                                     mcsDOUBLE* radius);

mcsCOMPL_STAT alxComputeDistance(mcsDOUBLE ra1,
                                 mcsDOUBLE dec1,
                                 mcsDOUBLE ra2,
                                 mcsDOUBLE dec2,
                                 mcsDOUBLE* distance);

mcsCOMPL_STAT alxComputeExtinctionCoefficient(mcsDOUBLE* av,
                                              mcsDOUBLE plx,
                                              mcsDOUBLE gLat,
                                              mcsDOUBLE gLon);

mcsCOMPL_STAT alxComputeUDFromLDAndSP(const mcsDOUBLE ld,
                                      const mcsSTRING32 sp,
                                      alxUNIFORM_DIAMETERS* ud);

mcsCOMPL_STAT alxShowUNIFORM_DIAMETERS(const alxUNIFORM_DIAMETERS* ud);
mcsCOMPL_STAT alxFlushUNIFORM_DIAMETERS(alxUNIFORM_DIAMETERS* ud);

#ifdef __cplusplus
}
#endif

#endif /*!alx_H*/

/*___oOo___*/
