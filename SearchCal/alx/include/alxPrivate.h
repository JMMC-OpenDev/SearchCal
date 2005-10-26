#ifndef alxPrivate_H
#define alxPrivate_H
/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: alxPrivate.h,v 1.9 2005-10-26 11:24:01 lafrasse Exp $"
 * 
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.8  2005/06/01 14:16:07  scetre
 * Changed logExtDbg to logTrace
 *
 * Revision 1.7  2005/04/06 12:12:56  scetre
 * Changed used of float for properties to computed in alxDATA
 *
 * Revision 1.6  2005/04/04 07:22:51  scetre
 * alxDIFFERIANTIAL_MAGNITUDES become a private structure
 *
 * Revision 1.5  2005/02/22 10:15:32  gzins
 * Made alxBLANKING_VALUE definition public
 *
 * Revision 1.4  2005/02/21 19:30:56  gzins
 * Removed alxFITZ_ID definition; replaced by alxBAND
 *
 * Revision 1.3  2005/02/12 15:09:43  gzins
 * Added type definition for alxGetResearchSize function
 *
 * Revision 1.2  2005/01/21 08:10:30  gluck
 * - Updated
 * - Insert automatic rcs log in header file
 *
 * 
 * scetre    20-Sep-2004  Created
 * gzins     20-Dec-2004  Fixed wrong module name and removed unused definition
 * gzins     12-Jan-2005  Move private declaration (structures, macros, ...)
 *                        from alx.h
 *
 ******************************************************************************/

/**
 * @file
 * alxPrivate header file 
 */

/* The following piece of code alternates the linkage type to C for all 
functions declared within the braces, which is necessary to use the 
functions in C++-code.
*/
#ifdef __cplusplus
extern "C" {
#endif

/* Module name */ 
#define MODULE_ID "alx" 

/* Pi */
#ifndef M_PI 
#define M_PI 3.14159265358979323846264338327950288
#endif

/******** Macros and structure for angular diameter computation */
/*
 * Number of color indexes used to compute the angular diameter : (B-V), (V-R),
 * (V-K)
 */
#define alxNB_COLOR_INDEXES 3
    
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
    char      *fileName;
    mcsFLOAT   coeff[alxNB_COLOR_INDEXES][alxNB_POLYNOMIAL_COEFF_DIAMETER];
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

/*
 * spectral type structure.
 *
 * A spectral type is build with a code (O, B, A, F, G, K, M),
 * a number between 0 and 9, and a light class which can be I,II,III,IV,etc...
 */
typedef struct
{
    char        code;          /* Code of the spectral type */
    mcsFLOAT    quantity;      /* Quantity of the spectral subtype */
    mcsSTRING32 lightClass;    /* Luminosity class*/
} alxSPECTRAL_TYPE;

/*
 * Colum identificator of magnitude difference.
 */
typedef enum
{
    alxB_V,                 /* value of MagB - MagV */
    alxV_I,                 /* value of MagV - MagI */
    alxV_R,                 /* value of MagV - MagR */
    alxI_J,                 /* value of MagI - MagJ */
    alxJ_H,                 /* value of MagJ - MagH */
    alxJ_K,                 /* value of MagJ - MagK */
    alxK_L,                 /* value of MagK - MagL */
    alxK_M,                 /* value of MagL - MagM */
    alxNB_DIFF_MAG          /* number of differential magnitude */
} alxDIFF_MAG;

/**
 * Differential magnitudes
 */
typedef alxDATA alxDIFFERENTIAL_MAGNITUDES[alxNB_DIFF_MAG];

/*
 * Structure of the color table.
 */
typedef struct
{
    mcsLOGICAL       loaded;
    char            *fileName;
    mcsINT32         nbLines;
    alxSPECTRAL_TYPE spectralType[alxNB_SPECTRAL_TYPES];
    alxDATA         index[alxNB_SPECTRAL_TYPES][alxNB_DIFF_MAG];
} alxCOLOR_TABLE;

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
 * Fitzpatrick Id for M, B, V, R, I, J, H and K bands, used to retreive the
 * extinction ratio.
 */
typedef struct
{
    mcsLOGICAL  loaded;    
    char       *fileName;
    mcsFLOAT    rc[alxNB_BANDS];
} alxEXTINCTION_RATIO_TABLE;
/*
 * Polynomial to compute the interstellar absorbtion is made by 4 coefficients
 * which are stored in file. Polynomial coefficients is given versus the
 * galactic longitude by step of 10 degrees; i.e. there are 36 polynomial
 * coefficient sets for all longitude range [0..360°].
 */
#define alxNB_MAX_LONGITUDE_STEPS         (360/10)
#define alxNB_POLYNOMIAL_COEFF_ABSORPTION  4
/*
 * Structure of the an coefficient table for compute interstellar absorbtion
 */
typedef struct
{
    mcsLOGICAL loaded;
    char      *fileName;
    mcsINT32   nbLines;
    mcsFLOAT   gLonMin[alxNB_MAX_LONGITUDE_STEPS];
    mcsFLOAT   gLonMax[alxNB_MAX_LONGITUDE_STEPS];
    mcsFLOAT   coeff[alxNB_MAX_LONGITUDE_STEPS][alxNB_POLYNOMIAL_COEFF_ABSORPTION];
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
    char      *fileName;
    mcsFLOAT   gLonList[alxNB_GLON_STEPS];
    mcsFLOAT   gLatList[alxNB_GLAT_STEPS];
    mcsFLOAT   mag[alxNB_MAG_STEPS];
    mcsINT32   nbOfStars[alxNB_MAG_STEPS][alxNB_GLAT_STEPS][alxNB_GLON_STEPS];
} alxSTAR_POPULATION;

#ifdef __cplusplus
}
#endif

#endif /*!alxPrivate_H*/

/*___oOo___*/
