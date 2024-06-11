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
    (fabs(cellValue - alxBLANKING_VALUE) < 1e-2)


/** 1 arcsec in degrees. */
#define alxARCSEC_IN_DEGREES ((mcsDOUBLE)(1.0/3600.0))

/** 1 degree in arcsec. */
#define alxDEG_IN_ARCSEC     ((mcsDOUBLE)3600.0)

/** 1 degree in arcmin. */
#define alxDEG_IN_ARCMIN     ((mcsDOUBLE)60.0)

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
    alxV_BAND = 1, /** V-band */
    alxR_BAND = 2, /** R-band */
    alxI_BAND = 3, /** I-band */
    alxJ_BAND = 4, /** J-band */
    alxH_BAND = 5, /** H-band */
    alxK_BAND = 6, /** K-band */
    alxL_BAND = 7, /** L-band */
    alxM_BAND = 8, /** M-band */
    alxN_BAND = 9, /** M-band */
    alxNB_BANDS    /** number of bands */
} alxBAND;

/* band index as label string mapping */
static const char* const alxBAND_STR[] = {"B", "V", "R", "I", "J", "H", "K", "L", "M", "N", "" };

/**
 * Structure of a data with its value, the confidence index associated, and
 * a boolean flag to store wether the data is set or not
 */
typedef struct
{
    mcsDOUBLE  value; /** data value */
    mcsDOUBLE  error; /** data error */
    alxCONFIDENCE_INDEX confIndex; /** confidence index */
    mcsLOGICAL isSet;  /** mcsTRUE if the data is defined */
} alxDATA;

/** initialize (or clear) an alxData structure */
#define alxDATAClear(data)             \
    data.value = 0.0;                  \
    data.error = 0.0;                  \
    data.confIndex = alxNO_CONFIDENCE; \
    data.isSet  = mcsFALSE;

/** copy an alxData structure */
#define alxDATACopy(src, dest)         \
    dest.value = src.value;            \
    dest.error = src.error;            \
    dest.confIndex = src.confIndex;    \
    dest.isSet = src.isSet;

/** test if alxData is set */
#define alxIsSet(data) \
    IS_TRUE((data).isSet)

/** test if alxData is NOT set */
#define alxIsNotSet(data) \
    IS_FALSE((data).isSet)

/* computes the relative error for a normal distribution in percents if value is defined */
#define alxDATARelError(data) \
    alxIsSet((data)) ? 100.0 * (data).error / (data).value : 0.0

/** log(10) constant */
#define LOG_10 2.302585092994046

/* computes the relative error for a log normal distribution in percents if value is defined */
#define alxDATALogRelError(data) \
    alxIsSet((data)) ? 100.0 * (data).error / (LOG_10 * (data).value) : 0.0

/* computes 10^x */
#define alxPow10(x) \
    pow(10.0, x)

/**
 * Valid diameter conditions:
 * Note: high confidence means diameter computed from catalog magnitudes. We reject diameters with
 * negative or null errors beforehand, although this is taken into account in the alxMean() functions
 */
#define isDiameterValid(diameter) \
   (alxIsSet(diameter) && ((diameter).confIndex == alxCONFIDENCE_HIGH) && ((diameter).error > 0.0))


/** number of sigma to log individual diameter (3 sigma) */
#define LOG_RESIDUAL_THRESHOLD 3.0

/** chi2 threshold to consider diameters as inconsistent (5.0) */
#define DIAM_CHI2_THRESHOLD   5.0

#define alxNB_SED_BAND 5

/*
 * Colum identifier of magnitude difference.
 */
typedef enum
{
    alxB_V,        /** value of MagB - MagV */
    alxV_I,        /** value of MagV - MagI */
    alxV_R,        /** value of MagV - MagR */
    alxI_J,        /** value of MagI - MagJ */
    alxJ_H,        /** value of MagJ - MagH */
    alxJ_K,        /** value of MagJ - MagK */
    alxK_L,        /** value of MagK - MagL */
    alxL_M,        /** value of MagL - MagM */
    alxK_M,        /** value of MagK - MagM */
    alxNB_DIFF_MAG /** number of differential magnitude */
} alxDIFF_MAG;

/* index of the Mv (absolute magnitude in V) in color tables */
#define alxMv alxK_M

/**
 * Differential magnitudes
 */
typedef alxDATA alxDIFFERENTIAL_MAGNITUDES[alxNB_DIFF_MAG];

/*
 * Type of star.
 */
typedef enum
{
    alxSUPER_GIANT     = 0,
    alxSUB_SUPER_GIANT = 1,
    alxGIANT           = 2,
    alxSUB_GIANT       = 3,
    alxDWARF           = 4,
    alxSTAR_UNDEFINED  = 5
} alxSTAR_TYPE;

/* star type index as label string mapping */
static const char* const alxSTAR_TYPE_STR[] = {"SUPER_GIANT", "SUB_SUPER_GIANT", "GIANT", "SUB_GIANT", "DWARF", "UNDEFINED" };

/*
 * Type of star used in tables (teff/logg and color tables).
 */
typedef enum
{
    alxTABLE_DWARF           = 0,
    alxTABLE_GIANT           = 1,
    alxTABLE_SUPER_GIANT     = 2,
    alxNB_LUM_CLASS
} alxTABLE_STAR_TYPE;

/* table star type index as label string mapping */
static const char* const alxTABLE_STAR_TYPE_STR[] = {"DWARF", "GIANT", "SUPER_GIANT", "UNDEFINED" };

#define alxNB_TABLE_STAR_TYPES   alxNB_LUM_CLASS
#define alxNB_LUM_CLASS_FOR_TEFF alxNB_LUM_CLASS

/*
 * Spectral type structure:
 * A spectral type is build with a code (O, B, A, F, G, K, M),
 * a number between 0 and 9, and a luminosity class which can be I,II,III,IV,etc...
 */
typedef struct
{
    mcsLOGICAL            isSet; /** mcsTRUE if the Spectral Type is defined */
    mcsLOGICAL        isInvalid; /** mcsTRUE if the Spectral Type is not supported (bad code) */
    mcsSTRING32      origSpType; /** original spectral type */
    mcsSTRING32       ourSpType; /** spectral type as interpreted by us */
    char                   code; /** Code of the spectral type */
    mcsDOUBLE          quantity; /** Quantity of the spectral subtype */
    mcsDOUBLE     deltaQuantity; /** Quantity Uncertainty of the spectral subtype */
    mcsSTRING32 luminosityClass; /** Luminosity class */
    mcsLOGICAL         isDouble; /** mcsTRUE if Spectral Type contained a '+' */
    mcsLOGICAL isSpectralBinary; /** mcsTRUE if Spectral Type contained "SB"  */
    mcsLOGICAL       isVariable; /** mcsTRUE if Spectral Type contained "VAR" */
    mcsLOGICAL      isCorrected; /** mcsTRUE if corrected Luminosity class */
    alxSTAR_TYPE       starType; /** Parsed main  star type from Luminosity class (I,III,V) */
    alxSTAR_TYPE  otherStarType; /** Parsed other star type from Luminosity class */
} alxSPECTRAL_TYPE;

/**
 * Structure of alxNB_BANDS(9) magnitudes
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
    alxV_J_DIAM =  0, /** V-J diameter */
    alxV_H_DIAM =  1, /** V-H diameter */
    alxV_K_DIAM =  2, /** V-K diameter */
    alxNB_DIAMS =  3  /** number of diameters */
} alxDIAM;

/* color index as label string mapping */
static const char* const alxDIAM_STR[] = {"V-J", "V-H", "V-K", "" };

/* band corresponding to the first magnitude (mI) in the color index */
static const alxBAND alxDIAM_BAND_A[] = {alxV_BAND, alxV_BAND, alxV_BAND };

/* band corresponding to the second magnitude (mJ) in the color index */
static const alxBAND alxDIAM_BAND_B[] = {alxJ_BAND, alxH_BAND, alxK_BAND};

/**
 * Structure of diameters
 */
typedef alxDATA alxDIAMETERS[alxNB_DIAMS];

/**
 * Structure of covariance matrix of computed diameters
 */
typedef mcsDOUBLE alxDIAMETERS_COVARIANCE[alxNB_DIAMS][alxNB_DIAMS];

/** Structure holding uniform diameters */
typedef struct
{
    mcsDOUBLE Teff;
    mcsDOUBLE LogG;

    mcsDOUBLE u;
    mcsDOUBLE b;
    mcsDOUBLE v;
    mcsDOUBLE r;
    mcsDOUBLE i;
    mcsDOUBLE j;
    mcsDOUBLE h;
    mcsDOUBLE k;
    mcsDOUBLE l;
    mcsDOUBLE m;
    mcsDOUBLE n;

} alxUNIFORM_DIAMETERS;

/** convenience macro */
#define alxIsDevFlag() \
    IS_TRUE(alxGetDevFlag())

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

mcsCOMPL_STAT alxInitializeSpectralType(alxSPECTRAL_TYPE* spectralType);

void alxGiveIndexInTableOfSpectralCodes(alxSPECTRAL_TYPE spectralType,
                                        mcsINT32 *colorTableIndex,
                                        mcsINT32 *colorTableDelta,
                                        mcsINT32 *lumClass,
                                        mcsINT32 *lumClassDelta);

void alxFixSpType(mcsUINT32 colorTableIndex,
                  mcsUINT32 colorTableDelta,
                  alxSPECTRAL_TYPE* spectralType);

mcsCOMPL_STAT alxString2SpectralType(mcsSTRING32 spType,
                                     alxSPECTRAL_TYPE* spectralType);

mcsCOMPL_STAT alxComputeMagnitudesForBrightStar(alxSPECTRAL_TYPE* spectralType,
                                                alxMAGNITUDES magnitudes);

mcsCOMPL_STAT alxComputeMagnitudesForFaintStar(alxSPECTRAL_TYPE* spectralType,
                                               alxMAGNITUDES magnitudes);

mcsCOMPL_STAT alxComputeCorrectedMagnitudes(const char* msg,
                                            mcsDOUBLE Av,
                                            alxMAGNITUDES magnitudes,
                                            mcsLOGICAL doLog);

mcsCOMPL_STAT alxComputeApparentMagnitudes(mcsDOUBLE Av,
                                           alxMAGNITUDES magnitudes);

mcsCOMPL_STAT alxComputeAngularDiameters(const char* msg,
                                         alxMAGNITUDES magnitudes,
                                         mcsDOUBLE spTypeIndex,
                                         alxDIAMETERS diameters,
                                         alxDIAMETERS_COVARIANCE diametersCov,
                                         logLEVEL logLevel);

void alxComputeDiameterRms(alxDIAMETERS diameters,
                           alxDATA     *meanDiam,
                           mcsUINT32    nbRequiredDiameters);

mcsCOMPL_STAT alxComputeMeanAngularDiameter(alxDIAMETERS diameters,
                                            alxDIAMETERS_COVARIANCE diametersCov,
                                            mcsUINT32    nbRequiredDiameters,
                                            alxDATA     *weightedMeanDiam,
                                            alxDATA     *chi2Diam,
                                            mcsUINT32   *nbDiameters,
                                            miscDYN_BUF *diamInfo,
                                            logLEVEL logLevel);

/* unused 2017.4 */
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

/* unused 2017.4 */
mcsCOMPL_STAT alxComputeExtinctionCoefficient(mcsDOUBLE *Av,
                                              mcsDOUBLE *e_Av,
                                              mcsDOUBLE *dist,
                                              mcsDOUBLE *e_dist,
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

/* unused 2017.4 */
mcsCOMPL_STAT alxComputeUDFromLDAndSP(const mcsDOUBLE ld,
                                      const mcsDOUBLE teff,
                                      const mcsDOUBLE logg,
                                      alxUNIFORM_DIAMETERS* ud);

mcsCOMPL_STAT alxComputeNewUDFromLDAndSP(const mcsDOUBLE ld,
                                         const mcsINT32 sptype_index,
                                         const mcsINT32 lumclass,
                                         alxUNIFORM_DIAMETERS *ud);

mcsCOMPL_STAT alxComputeAvFromMagnitudes(const char* starId,
                                         mcsDOUBLE  dist_plx,
                                         mcsDOUBLE  e_dist_plx,
                                         mcsDOUBLE* Av,
                                         mcsDOUBLE* e_Av,
                                         mcsDOUBLE* dist,
                                         mcsDOUBLE* e_dist,
                                         mcsDOUBLE* chi2,
                                         mcsDOUBLE* distChi2,
                                         alxMAGNITUDES magnitudes,
                                         alxSPECTRAL_TYPE* spectralType,
                                         mcsDOUBLE minDeltaQuantity,
                                         mcsLOGICAL useLumClass,
                                         mcsLOGICAL useDeltaQuantity);

const char* alxGetConfidenceIndex(alxCONFIDENCE_INDEX confIndex);

mcsCOMPL_STAT alxShowUNIFORM_DIAMETERS(const alxUNIFORM_DIAMETERS* ud);

mcsCOMPL_STAT alxFlushUNIFORM_DIAMETERS(alxUNIFORM_DIAMETERS* ud);

void alxLogTestMagnitudes(const char* line, const char* msg, alxMAGNITUDES magnitudes);

void alxLogAngularDiameters(const char* msg, alxDIAMETERS diameters, logLEVEL logLevel);

mcsCOMPL_STAT alxSedFitting(alxDATA *magnitudes, mcsDOUBLE Av, mcsDOUBLE e_Av,
                            mcsDOUBLE *bestDiam, mcsDOUBLE *bestDiamError,
                            mcsDOUBLE *bestChi2, mcsDOUBLE *bestTeff, mcsDOUBLE *bestAv);

const char* alxGetBandLabel(const alxBAND band);

const char* alxGetConfidenceIndex(const alxCONFIDENCE_INDEX confIndex);

const char* alxGetDiamLabel(const alxDIAM color);

const char* alxGetStarTypeLabel(const alxSTAR_TYPE starType);

const char* alxGetTableStarTypeLabel(const alxTABLE_STAR_TYPE starType);


/* Mathematical Functions */

mcsINT32 alxIntMin(mcsINT32 a, mcsINT32 b);
mcsINT32 alxIntMax(mcsINT32 a, mcsINT32 b);

mcsDOUBLE alxMin(mcsDOUBLE a, mcsDOUBLE b);
mcsDOUBLE alxMax(mcsDOUBLE a, mcsDOUBLE b);

mcsDOUBLE  alxSquare(mcsDOUBLE a);

mcsDOUBLE  alxNorm(mcsDOUBLE a, mcsDOUBLE b);

mcsDOUBLE  alxTotal  (mcsUINT32 n, mcsDOUBLE x[]);
mcsDOUBLE* alxInvert(mcsUINT32 n, mcsDOUBLE x[], mcsDOUBLE y[]);

mcsDOUBLE alxMean(mcsUINT32 nbValues, mcsDOUBLE *vector);
mcsDOUBLE alxRmsDistance(mcsUINT32 nbValues, mcsDOUBLE *vector, mcsDOUBLE specialValue);

mcsDOUBLE alxMedian(mcsUINT32 n, mcsDOUBLE in[]);

mcsDOUBLE alxWeightedMean(mcsUINT32 nbValues, mcsDOUBLE *vector, mcsDOUBLE *sigma2);
mcsDOUBLE alxWeightedRmsDistance(mcsUINT32 nbValues, mcsDOUBLE *vector, mcsDOUBLE *sigma2, mcsDOUBLE specialValue);

mcsCOMPL_STAT alxInvertMatrix (mcsDOUBLE *matrix, mcsUINT32 dim);
void          alxProductMatrix(mcsDOUBLE *A, mcsDOUBLE *B, mcsDOUBLE *C, mcsUINT32 nrows,
                               mcsUINT32 ncols, mcsUINT32 mcols);

void      alxComputePows(mcsUINT32 max, mcsDOUBLE x, mcsDOUBLE* pows);
mcsDOUBLE alxComputePolynomial(mcsUINT32 len, mcsDOUBLE* coeffs, mcsDOUBLE* pows);

#ifdef __cplusplus
}
#endif

#endif /*!alx_H*/

/*___oOo___*/
