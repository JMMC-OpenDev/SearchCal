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
 * Number of color indexes used to compute the angular diameter :
 * (B-V), (V-R), (V-K), (I-J), (I-K), (J-H), (J-K), (H-K)
 */
#define alxNB_COLOR_INDEXES 8

/*
 * Polynomial to compute the angular diameter is made by 6 coefficients which
 * are stored in file.
 */
#define alxNB_POLYNOMIAL_COEFF_DIAMETER 6

/*
 * Structure of the an coefficient table for compute angular diameter
 */
typedef struct
{
    mcsLOGICAL loaded;
    char* fileName;
    mcsDOUBLE coeff[alxNB_COLOR_INDEXES][alxNB_POLYNOMIAL_COEFF_DIAMETER];
    mcsDOUBLE error[alxNB_COLOR_INDEXES];
    mcsDOUBLE domainMin[alxNB_COLOR_INDEXES];
    mcsDOUBLE domainMax[alxNB_COLOR_INDEXES];
} alxPOLYNOMIAL_ANGULAR_DIAMETER;

/******** Macros and structure for missing magnitudes computation */
/* 
 * The maximum of line which can be found in the table is 61.
 * This represents the number of spectral types that it is possible to find in
 * the relation 
 *  O5 to O9  ---->  line 1 to  5
 *  B0 to B9  ---->  line 6 to 15
 *  A0 to A9  ---->  line 16 to 25
 *  F0 to F8  ---->  line 26 to 34
 *  G0 to G9  ---->  line 35 to 44
 *  K0 to K7  ---->  line 45 to 52
 *  M0 to M8  ---->  line 53 to 61
 */
#define alxNB_SPECTRAL_TYPES 61
#define alxNB_SPECTRAL_TYPES_FOR_TEFF 71

/*
 * Colum identificator of magnitude difference.
 */
typedef enum
{
    alxB_V, /* value of MagB - MagV */
    alxV_I, /* value of MagV - MagI */
    alxV_R, /* value of MagV - MagR */
    alxI_J, /* value of MagI - MagJ */
    alxJ_H, /* value of MagJ - MagH */
    alxJ_K, /* value of MagJ - MagK */
    alxK_L, /* value of MagK - MagL */
    alxL_M, /* value of MagL - MagM */
    alxK_M, /* value of MagK - MagM */
    alxNB_DIFF_MAG /* number of differential magnitude */
} alxDIFF_MAG;

/**
 * Differential magnitudes
 */
typedef alxDATA alxDIFFERENTIAL_MAGNITUDES[alxNB_DIFF_MAG];

/*
 * Structure of the color table.
 */
#define alxNB_COLOR_INDEXES 8

typedef struct
{
    mcsLOGICAL loaded;
    char* fileName;
    mcsINT32 nbLines;
    alxSPECTRAL_TYPE spectralType[alxNB_SPECTRAL_TYPES];
    alxDATA index[alxNB_SPECTRAL_TYPES][alxNB_COLOR_INDEXES];
} alxCOLOR_TABLE;

/*
 * Structure of the Teff,Logg table.
 */
#define alxNB_LUMINOSITY_CLASSES 3

typedef struct
{
    mcsLOGICAL loaded;
    char* fileName;
    mcsINT32 nbLines;
    alxSPECTRAL_TYPE spectralType[alxNB_SPECTRAL_TYPES_FOR_TEFF];
    mcsDOUBLE teff[alxNB_SPECTRAL_TYPES_FOR_TEFF][alxNB_LUMINOSITY_CLASSES];
    mcsDOUBLE logg[alxNB_SPECTRAL_TYPES_FOR_TEFF][alxNB_LUMINOSITY_CLASSES];
} alxTEFFLOGG_TABLE;

#define alxNB_UD_BANDS 10

#define alxNB_UD_ENTRIES 409

typedef struct
{
    mcsLOGICAL loaded;
    char* fileName;
    mcsINT32 nbLines;
    mcsDOUBLE logg[alxNB_UD_ENTRIES];
    mcsDOUBLE teff[alxNB_UD_ENTRIES];
    mcsDOUBLE coeff[alxNB_UD_ENTRIES][alxNB_UD_BANDS];

} alxUD_CORRECTION_TABLE;

typedef enum
{
    alxU, alxB, alxV, alxR, alxI, alxJ, alxH, alxK, alxL, alxN, alxNBUD_BANDS
} alxUD_BANDS;

typedef enum
{
    alx7mu, /* column for 7  mu in akariTable */
    alx9mu, /* column for 9  mu in akariTable */
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
    char* fileName;
    mcsINT32 nbLines;
    mcsDOUBLE teff[alxNB_AKARI_TEFF];
    mcsDOUBLE coeff[alxNB_AKARI_TEFF][alxNB_AKARI_BANDS];
} alxAKARI_TABLE;

/*
 * Type of star.
 */
#define alxNB_STAR_TYPES 3

typedef enum
{
    alxDWARF,
    alxGIANT,
    alxSUPER_GIANT
} alxSTAR_TYPE;

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
    char* fileName;
    mcsDOUBLE rc[alxNB_BANDS];
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
    char* fileName;
    mcsINT32 nbLines;
    mcsDOUBLE gLonMin[alxNB_MAX_LONGITUDE_STEPS];
    mcsDOUBLE gLonMax[alxNB_MAX_LONGITUDE_STEPS];
    mcsDOUBLE coeff[alxNB_MAX_LONGITUDE_STEPS][alxNB_POLYNOMIAL_COEFF_ABSORPTION];
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
    char* fileName;
    mcsDOUBLE gLonList[alxNB_GLON_STEPS];
    mcsDOUBLE gLatList[alxNB_GLAT_STEPS];
    mcsDOUBLE mag[alxNB_MAG_STEPS];
    mcsINT32 nbOfStars[alxNB_MAG_STEPS][alxNB_GLAT_STEPS][alxNB_GLON_STEPS];
} alxSTAR_POPULATION;

#ifdef __cplusplus
}
#endif

#endif /*!alxPrivate_H*/

/*___oOo___*/
