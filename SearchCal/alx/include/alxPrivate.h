#ifndef alxPrivate_H
#define alxPrivate_H
/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * @file
 * alxPrivate header file
 */

/*
 * General common includes
 */
#include "math.h"

/* The following piece of code alternates the linkage type to C for all
functions declared within the braces, which is necessary to use the
functions in C++-code.
 */
#ifdef __cplusplus
extern "C"
{
#endif

/* Module name */
#define MODULE_ID "alx"

/******** Macros and structure for angular diameter computation */
/*
 * Polynomial to compute the angular diameter is made
 * by 5 coefficients (4th degree) which are stored in file.
 */
#define alxNB_POLYNOMIAL_COEFF_DIAMETER 5

/*
 * Dimension of the covariance matrix of polynomial coefficients
 */
#define alxNB_POLYNOMIAL_COEFF_COVARIANCE (alxNB_DIAMS * alxNB_POLYNOMIAL_COEFF_DIAMETER)

/*
 * Structure of the coefficient table for compute angular diameter
 */
typedef struct
{
    mcsLOGICAL loaded;
    char*      fileName;
    char*      fileNameCov;
    mcsUINT32  nbCoeff    [alxNB_DIAMS];
    mcsDOUBLE  coeff      [alxNB_DIAMS][alxNB_POLYNOMIAL_COEFF_DIAMETER];
    mcsDOUBLE  polynomCoefCovMatrix    [alxNB_POLYNOMIAL_COEFF_COVARIANCE][alxNB_POLYNOMIAL_COEFF_COVARIANCE];
} alxPOLYNOMIAL_ANGULAR_DIAMETER;

/******** Macros and structure for missing magnitudes computation */
/*
 * The maximum of line which can be found in the table is 61.
 * This represents the number of spectral types that it is possible to find in
 * the relation
 *  O5.0 to O9.75 ---->  line   1 to  20
 *  B0.0 to B9.75 ---->  line  21 to  60
 *  A0.0 to A9.75 ---->  line  61 to 100
 *  F0.0 to F9.75 ---->  line 101 to 140
 *  G0.0 to G9.75 ---->  line 141 to 180
 *  K0.0 to K9.75 ---->  line 181 to 220
 *  M0.0 to M8.00 ---->  line 221 to 252
 */
#define alxNB_SPECTRAL_TYPES 253
#define alxNB_SPECTRAL_TYPES_FOR_TEFF 71

/** the maximum number of spectral type codes (O, B, A, F, G, K, M) */
#define alxNB_SPECTRAL_TYPE_CODES 7

/*
 * Structure of the color table.
 * B-V  V-Ic  V-R  Ic-Jc  Jc-Hc  Jc-Kc  Kc-L  L-M  Mv
 */
#define alxNB_COLOR_INDEXES 9

typedef struct
{
    char      code;      /** Code of the spectral type */
    mcsUINT32 firstLine; /** First line */
} alxCOLOR_TABLE_INDEX;

typedef struct
{
    mcsLOGICAL           loaded;
    char*                fileName;
    mcsUINT32            nbLines;
    mcsDOUBLE            step;
    alxSPECTRAL_TYPE     spectralType[alxNB_SPECTRAL_TYPES];
    alxDATA              index[alxNB_SPECTRAL_TYPES][alxNB_COLOR_INDEXES];
    alxCOLOR_TABLE_INDEX lineIndex[alxNB_SPECTRAL_TYPE_CODES];
    alxDATA              absMag[alxNB_SPECTRAL_TYPES][alxL_BAND]; /* precomputed absolute magnitudes BVR Ic JHK(2MASS) */
    alxDATA              absMagError[alxNB_SPECTRAL_TYPES]; /* error on absolute magnitudes (any band) */
    mcsUINT32            absMagLineFirst; /* first line with absolute magnitudes (not blanking values) */
    mcsUINT32            absMagLineLast;  /* last  line with absolute magnitudes (not blanking values) */
} alxCOLOR_TABLE;

/*
 * Structure of the Teff/Logg table.
 */
typedef struct
{
    mcsLOGICAL       loaded;
    char*            fileName;
    mcsINT32         nbLines;
    alxSPECTRAL_TYPE spectralType[alxNB_SPECTRAL_TYPES_FOR_TEFF];
    mcsDOUBLE        teff[alxNB_SPECTRAL_TYPES_FOR_TEFF][alxNB_LUM_CLASS_FOR_TEFF];
    mcsDOUBLE        logg[alxNB_SPECTRAL_TYPES_FOR_TEFF][alxNB_LUM_CLASS_FOR_TEFF];
} alxTEFFLOGG_TABLE;

#define alxNB_UD_ENTRIES 409
#define alxNB_COLORINDEX_ENTRIES 273

typedef enum
{
    alxU, alxB, alxV, alxR, alxI, alxJ, alxH, alxK, alxL, alxM, alxN, alxNB_UD_BANDS
} alxUD_BANDS;

typedef struct
{
    mcsLOGICAL loaded;
    char*      fileName;
    mcsINT32   nbLines;
    mcsDOUBLE  logg[alxNB_UD_ENTRIES];
    mcsDOUBLE  teff[alxNB_UD_ENTRIES];
    mcsDOUBLE  coeff[alxNB_UD_ENTRIES][alxNB_UD_BANDS];

} alxUD_CORRECTION_TABLE;

typedef struct
{
    mcsLOGICAL loaded;
    char*      fileName;
    mcsINT32   nbLines;
    mcsDOUBLE  teff[alxNB_COLORINDEX_ENTRIES][3]; /* V, III, I */
    mcsDOUBLE  logg[alxNB_COLORINDEX_ENTRIES][3];
    mcsDOUBLE  mass[alxNB_COLORINDEX_ENTRIES][3];
    mcsDOUBLE  coeffNL[alxNB_COLORINDEX_ENTRIES][alxNB_UD_BANDS][2]; /* V, III */
    mcsDOUBLE  coeffCL[alxNB_COLORINDEX_ENTRIES][alxNB_UD_BANDS][2];

} alxUD_NEW_CORRECTION_TABLE;

typedef enum
{
    alx7mu,  /* column for  7 mu in akariTable */
    alx9mu,  /* column for  9 mu in akariTable */
    alx11mu, /* column for 11 mu in akariTable */
    alx15mu, /* column for 15 mu in akariTable */
    alx18mu, /* column for 18 mu in akariTable */
    alx24mu, /* column for 14 mu in akariTable */
    alxNB_AKARI_BANDS /* number of bands in akariTable */
} alxAKARI_BANDS;

#define AKARI_7MU 7.0
#define AKARI_9MU 9.0
#define AKARI_11MU 11.0
#define AKARI_15MU 15.0
#define AKARI_18MU 18.0
#define AKARI_24MU 24.0


/*
 * Structure of the Akari Correction Table.
 */
#define alxNB_AKARI_TEFF 50

typedef struct
{
    mcsLOGICAL loaded;
    char*      fileName;
    mcsINT32   nbLines;
    mcsDOUBLE  teff[alxNB_AKARI_TEFF];
    mcsDOUBLE  coeff[alxNB_AKARI_TEFF][alxNB_AKARI_BANDS];
} alxAKARI_TABLE;

/******** Macros and structure for corrected magnitudes computation */
/*
 * Structure of the extinction ratio table.
 */

/*
 * Fitzpatrick Id for M, B, V, R, I, J, H and K bands, used to retrieve the
 * extinction ratio.
 */
typedef struct
{
    mcsLOGICAL loaded;
    char*      fileName;
    mcsDOUBLE  rc[alxNB_BANDS];
    mcsDOUBLE  coeff[alxNB_BANDS];
} alxEXTINCTION_RATIO_TABLE;
/*
 * Polynomial to compute the interstellar absorbtion is made by 4 coefficients
 * which are stored in file. Polynomial coefficients is given versus the
 * galactic longitude by step of 10 degrees; i.e. there are 36 polynomial
 * coefficient sets for all longitude range [0..360 deg].
 */
#define alxNB_MAX_LONGITUDE_STEPS         (360/10)
#define alxNB_POLYNOMIAL_COEFF_ABSORPTION  4

/*
 * Structure of the an coefficient table for compute interstellar absorbtion
 */
typedef struct
{
    mcsLOGICAL loaded;
    char*      fileName;
    mcsINT32   nbLines;
    mcsDOUBLE  gLonMin[alxNB_MAX_LONGITUDE_STEPS];
    mcsDOUBLE  gLonMax[alxNB_MAX_LONGITUDE_STEPS];
    mcsDOUBLE  coeff[alxNB_MAX_LONGITUDE_STEPS][alxNB_POLYNOMIAL_COEFF_ABSORPTION];
} alxPOLYNOMIAL_INTERSTELLAR_ABSORPTION;

/*
 * Structure containing the number of star according to the magnitude and the
 * galatic coordinates.
 */
#define alxNB_MAG_STEPS 29  /* From 5.5 to 19.5 by step of 0.5 */
#define alxNB_GLON_STEPS 6  /* 0, 10, 90, 180, 270 and 350 */
#define alxNB_GLAT_STEPS 9  /* -90 -60 -30 -10 0 10 30 60 90 */

typedef struct
{
    mcsLOGICAL loaded;
    char*      fileName;
    mcsDOUBLE  gLonList[alxNB_GLON_STEPS];
    mcsDOUBLE  gLatList[alxNB_GLAT_STEPS];
    mcsDOUBLE  mag[alxNB_MAG_STEPS];
    mcsINT32   nbOfStars[alxNB_MAG_STEPS][alxNB_GLAT_STEPS][alxNB_GLON_STEPS];
} alxSTAR_POPULATION;


alxEXTINCTION_RATIO_TABLE* alxGetExtinctionRatioTable(void);


#ifdef __cplusplus
}
#endif

#endif /*!alxPrivate_H*/

/*___oOo___*/
