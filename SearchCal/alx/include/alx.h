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
extern "C"
{
#endif


/*
 * MCS header
 */
#include "mcs.h"
#include "misc.h"


/** Blanking value. */
#define alxBLANKING_VALUE    ((mcsDOUBLE)99.99)


/**
 * Return true if a cell value is a blanking value or not.
 * @param cellValue the value of the cell
 * @return true if cell value == '99.99'(alxBLANKING_VALUE); otherwise false
 */
#define alxIsBlankingValue(cellValue) \
    (cellValue == alxBLANKING_VALUE)

#define alxIsNotBlankingValue(cellValue) \
    (cellValue != alxBLANKING_VALUE)


/** 1 arcsec in degrees. */
#define alxARCSEC_IN_DEGREES ((mcsDOUBLE)(1.0/3600.0))

/** 1 degree in arcsecs. */
#define alxDEG_IN_ARCSEC     ((mcsDOUBLE)3600.0)

/* radians <=> degrees conversions */
#define alxRAD_IN_DEG        (180.0 / M_PI)
#define alxDEG_IN_RAD        (M_PI / 180.0)


/** value not found in table */
#define alxNOT_FOUND -1

/**
 * Computed value confidence index.
 */
typedef enum
{
    alxNO_CONFIDENCE     = 0, /** No confidence     */
    alxCONFIDENCE_LOW    = 1, /** Low confidence    */
    alxCONFIDENCE_MEDIUM = 2, /** Medium confidence */
    alxCONFIDENCE_HIGH   = 3  /** High confidence   */
} alxCONFIDENCE_INDEX;

/* confidence index as label string mapping */
static const char* const alxCONFIDENCE_STR[] = { "NO", "LOW", "MEDIUM", "HIGH" };

/**
 * Bands for magnitude.
 */
typedef enum
{
    alxB_BAND = 0, /** B-band */
    alxV_BAND,     /** V-band */
    alxR_BAND,     /** R-band */
    alxI_BAND,     /** I-band */
    alxJ_BAND,     /** J-band */
    alxH_BAND,     /** H-band */
    alxK_BAND,     /** K-band */
    alxL_BAND,     /** L-band */
    alxM_BAND,     /** M-band */
    alxNB_BANDS    /** number of bands */
} alxBAND;

/**
 * Structure of a data with its value, the confidence index associated, and
 * a boolean flag to store wether the data is set or not
 */
typedef struct
{
    mcsDOUBLE  value; /** data value */
    mcsDOUBLE  error; /** data error */
    alxCONFIDENCE_INDEX confIndex; /** confidence index */
    mcsLOGICAL isSet; /** mcsTRUE if the data is defined */
} alxDATA;

/** initialize (or clear) an alxData structure */
#define alxDATAClear(data)             \
    data.value = 0.0;                  \
    data.error = 0.0;                  \
    data.confIndex = alxNO_CONFIDENCE; \
    data.isSet = mcsFALSE;

/** copy an alxData structure */
#define alxDATACopy(src, dest)         \
    dest.value = src.value;            \
    dest.error = src.error;            \
    dest.confIndex = src.confIndex;    \
    dest.isSet = src.isSet;

/** test if alxData is set */
#define alxIsSet(data) \
    isTrue(data.isSet)

/** test if alxData is NOT set */
#define alxIsNotSet(data) \
    isFalse(data.isSet)

/* computes the relative error in percents if value is defined */
#define alxDATARelError(data) \
    alxIsSet((data)) ? 100.0 * (data).error / (data).value : 0.0


#define alxNB_SED_BAND 5

/*
 * Spectral type structure:
 * A spectral type is build with a code (O, B, A, F, G, K, M),
 * a number between 0 and 9, and a luminosity class which can be I,II,III,IV,etc...
 */
typedef struct
{
    mcsLOGICAL            isSet; /** mcsTRUE if the Spectral Type is defined */
    mcsSTRING32      origSpType; /** original spectral type */
    mcsSTRING32       ourSpType; /** spectral type as interpreted by us */
    char                   code; /** Code of the spectral type */
    mcsDOUBLE          quantity; /** Quantity of the spectral subtype */
    mcsSTRING32 luminosityClass; /** Luminosity class */
    mcsLOGICAL         isDouble; /** mcsTRUE if Spectral Type contained a '+' */
    mcsLOGICAL isSpectralBinary; /** mcsTRUE if Spectral Type contained "SB"  */
    mcsLOGICAL       isVariable; /** mcsTRUE if Spectral Type contained "VAR" */
} alxSPECTRAL_TYPE;

/**
 * Stucture of alxNB_BANDS(9) magnitudes
 */
typedef alxDATA alxMAGNITUDES[alxNB_BANDS];

/** 
 * Structure of visibilities.
 */
typedef struct
{
    mcsDOUBLE vis;       /** visibility value */
    mcsDOUBLE visError;  /** visibility error */
    mcsDOUBLE vis2;      /** square visibility value */
    mcsDOUBLE vis2Error; /** square visibility error */
} alxVISIBILITIES;

/**
 * Color indexes for diameters.
 */
typedef enum
{
    alxB_V_DIAM = 0, /** B-V diameter */
    alxV_R_DIAM = 1, /** V-R diameter */
    alxV_K_DIAM = 2, /** V-K diameter */
    alxI_J_DIAM = 3, /** I-J diameter */
    alxI_K_DIAM = 4, /** I-K diameter */
    alxJ_H_DIAM = 5, /** J-H diameter */
    alxJ_K_DIAM = 6, /** J-K diameter */
    alxH_K_DIAM = 7, /** H-K diameter */
    alxNB_DIAMS = 8  /** number of diameters */
} alxDIAM;

/* color index as label string mapping */
static const char* const alxDIAM_STR[] = { "B-V", "V-R", "V-K", "I-J", "I-K", "J-H", "J-K", "H-K", "" };

/**
 * Stucture of diameters
 */
typedef alxDATA alxDIAMETERS[alxNB_DIAMS];

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

/** convenience macro */
#define alxIsDevFlag() \
    isTrue(alxGetDevFlag())

mcsLOGICAL alxGetDevFlag(void);
void       alxSetDevFlag(mcsLOGICAL flag);

/*
 * Public functions declaration
 */

/* Init functions */
void alxMissingMagnitudeInit(void);
void alxAngularDiameterInit(void);
void alxInterstellarAbsorptionInit(void);
void alxResearchAreaInit(void);
void alxLD2UDInit(void);
void alxSedFittingInit(void);

void alxInit(void);

mcsDOUBLE alxMin(mcsDOUBLE a, mcsDOUBLE b);
mcsDOUBLE alxMax(mcsDOUBLE a, mcsDOUBLE b);

mcsCOMPL_STAT alxInitializeSpectralType(alxSPECTRAL_TYPE* spectralType);

mcsCOMPL_STAT alxString2SpectralType(mcsSTRING32 spType,
                                     alxSPECTRAL_TYPE* spectralType);

mcsCOMPL_STAT alxCorrectSpectralType(alxSPECTRAL_TYPE* spectralType,
                                     mcsDOUBLE diffBV);

mcsCOMPL_STAT alxComputeMagnitudesForBrightStar(alxSPECTRAL_TYPE* spectralType,
                                                alxMAGNITUDES magnitudes);

mcsCOMPL_STAT alxComputeMagnitudesForFaintStar(alxSPECTRAL_TYPE* spectralType,
                                               alxMAGNITUDES magnitudes);

mcsCOMPL_STAT alxComputeCorrectedMagnitudes(const char* msg,
                                            mcsDOUBLE Av,
                                            alxMAGNITUDES magnitudes);

mcsCOMPL_STAT alxComputeApparentMagnitudes(mcsDOUBLE Av,
                                           alxMAGNITUDES magnitudes);

mcsCOMPL_STAT alxComputeAngularDiameters(const char* msg,
                                         alxMAGNITUDES magnitudes,
                                         alxDIAMETERS diameters);

mcsCOMPL_STAT alxComputeMeanAngularDiameter(alxDIAMETERS diameters,
                                            alxDATA     *meanDiam,
                                            alxDATA     *weightedMeanDiam,
                                            alxDATA     *stddevDiam,
                                            mcsUINT32   *nbDiameters,
                                            mcsUINT32    nbRequiredDiameters,
                                            miscDYN_BUF *diamInfo);

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

mcsCOMPL_STAT alxComputeDistanceInDegrees(mcsDOUBLE ra1,
                                          mcsDOUBLE dec1,
                                          mcsDOUBLE ra2,
                                          mcsDOUBLE dec2,
                                          mcsDOUBLE* distance);

mcsCOMPL_STAT alxComputeExtinctionCoefficient(mcsDOUBLE* Av,
                                              mcsDOUBLE* e_Av,
                                              mcsDOUBLE plx,
                                              mcsDOUBLE e_plx,
                                              mcsDOUBLE gLat,
                                              mcsDOUBLE gLon);

mcsCOMPL_STAT alxComputeFluxesFromAkari09(mcsDOUBLE Teff,
                                          mcsDOUBLE *fnu_9,
                                          mcsDOUBLE *fnu_12,
                                          mcsDOUBLE *fnu_18);

mcsCOMPL_STAT alxComputeFluxesFromAkari18(mcsDOUBLE Teff,
                                          mcsDOUBLE *fnu_18,
                                          mcsDOUBLE *fnu_12,
                                          mcsDOUBLE *fnu_9);

mcsCOMPL_STAT alxComputeTeffAndLoggFromSptype(alxSPECTRAL_TYPE* spectralType,
                                              mcsDOUBLE* Teff,
                                              mcsDOUBLE* LogG);

mcsCOMPL_STAT alxComputeUDFromLDAndSP(const mcsDOUBLE ld,
                                      const mcsDOUBLE teff,
                                      const mcsDOUBLE logg,
                                      alxUNIFORM_DIAMETERS* ud);

const char* alxGetConfidenceIndex(alxCONFIDENCE_INDEX confIndex);

mcsCOMPL_STAT alxShowUNIFORM_DIAMETERS(const alxUNIFORM_DIAMETERS* ud);

mcsCOMPL_STAT alxFlushUNIFORM_DIAMETERS(alxUNIFORM_DIAMETERS* ud);

void alxShowDiameterEffectiveDomains(void);

void alxLogTestMagnitudes(const char* line, const char* msg, alxMAGNITUDES magnitudes);

void alxLogTestAngularDiameters(const char* msg, alxDIAMETERS diameters);

mcsCOMPL_STAT alxSedFitting(alxDATA *magnitudes, mcsDOUBLE Av, mcsDOUBLE e_Av,
                            mcsDOUBLE *bestDiam, mcsDOUBLE *lowerDiam, mcsDOUBLE *upperDiam,
                            mcsDOUBLE *bestChi2, mcsDOUBLE *bestTeff, mcsDOUBLE *bestAv);


#ifdef __cplusplus
}
#endif

#endif /*!alx_H*/

/*___oOo___*/
