#ifndef alx_H
#define alx_H
/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
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


/*
 * spectral type structure.
 *
 * A spectral type is build with a code (O, B, A, F, G, K, M),
 * a number between 0 and 9, and a luminosity class which can be I,II,III,IV,etc...
 */
typedef struct
{
    char         code;              /* Code of the spectral type */
    mcsDOUBLE    quantity;          /* Quantity of the spectral subtype */
    mcsSTRING32  luminosityClass;   /* Luminosity class */
    mcsLOGICAL   isDouble;          /* mcsTRUE if Spectral Type contained a '+' */
    mcsLOGICAL   isSpectralBinary;  /* mcsTRUE if Spectral Type contained "SB"  */
    mcsLOGICAL   isVariable;        /* mcsTRUE if Spectral Type contained "VAR" */
} alxSPECTRAL_TYPE;

/**
 * Stucture of alxNB_BANDS(9) magnitudes
 */
typedef alxDATA alxMAGNITUDES[alxNB_BANDS];


/**
 * Structure of visibilities :
 *  - visibility
 *  - visError
 *  - vis2
 *  - vis2Error
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
 * Public functions declaration
 */

/* Init functions */
mcsCOMPL_STAT alxCorrectedMagnitudeInit(void);
mcsCOMPL_STAT alxAngularDiameterInit(void);
mcsCOMPL_STAT alxInterstellarAbsorptionInit(void);
mcsCOMPL_STAT alxResearchAreaInit(void);


mcsCOMPL_STAT alxComputeMagnitudesForBrightStar(mcsSTRING32 spType, 
                                                alxMAGNITUDES magnitudes);

mcsCOMPL_STAT alxComputeMagnitudesForFaintStar(mcsSTRING32 spType, 
                                               alxMAGNITUDES magnitudes); 

mcsCOMPL_STAT alxString2SpectralType(mcsSTRING32        spType,
                                     alxSPECTRAL_TYPE*  spectralType);

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

mcsCOMPL_STAT alxComputeF12FluxFromAkari(mcsDOUBLE Teff,
					 mcsDOUBLE *fnu_9,
					 mcsDOUBLE *fnu_12);

mcsCOMPL_STAT alxComputeTeffAndLoggFromSptype(const mcsSTRING32 sp,
					      mcsDOUBLE *Teff,
					      mcsDOUBLE *LogG);

mcsCOMPL_STAT alxRetrieveTeffAndLoggFromSptype(mcsSTRING32 spType,
					       mcsDOUBLE *Teff,
					       mcsDOUBLE *LogG);
mcsCOMPL_STAT alxGetUDFromLDAndSP(const mcsDOUBLE ld,
				  const mcsDOUBLE teff,
				  const mcsDOUBLE logg,
				  alxUNIFORM_DIAMETERS* ud);
#ifdef __cplusplus
}
#endif

#endif /*!alx_H*/

/*___oOo___*/
