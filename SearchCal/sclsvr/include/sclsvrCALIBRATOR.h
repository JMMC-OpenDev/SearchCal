#ifndef sclsvrCALIBRATOR_H
#define sclsvrCALIBRATOR_H
/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * @file
 * sclsvrCALIBRATOR class declaration.
 */

#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif


/*
 * SCALIB header
 */
#include "vobs.h"
#include "alx.h"

/*
 * Local header
 */
#include "sclsvrREQUEST.h"


/*
 * Definition of the calibrators properties
 */

#define sclsvrCALIBRATOR_PHOT_COUS_J        "PHOT_COUS_J"
#define sclsvrCALIBRATOR_PHOT_COUS_H        "PHOT_COUS_H"
#define sclsvrCALIBRATOR_PHOT_COUS_K        "PHOT_COUS_K"

/* computed diameters */
#define sclsvrCALIBRATOR_DIAM_VJ            "DIAM_VJ"
#define sclsvrCALIBRATOR_DIAM_VH            "DIAM_VH"
#define sclsvrCALIBRATOR_DIAM_VK            "DIAM_VK"

/* diameter count used by mean / weighted mean / stddev (consistent ones) */
#define sclsvrCALIBRATOR_DIAM_COUNT         "DIAM_COUNT"

/* LD diameter */
#define sclsvrCALIBRATOR_LD_DIAM            "LD_DIAM"

/* chi2 of the weighted mean diameter estimation */
#define sclsvrCALIBRATOR_DIAM_CHI2          "DIAM_CHI2"

/* calibrator flag (bits) */
#define sclsvrCALIBRATOR_CAL_FLAG           "CAL_FLAG"

/* diameter flag (true | false) */
#define sclsvrCALIBRATOR_DIAM_FLAG          "DIAM_FLAG"

/* diameter information */
#define sclsvrCALIBRATOR_DIAM_FLAG_INFO     "DIAM_FLAG_INFO"

/* SED fitting */
#define sclsvrCALIBRATOR_SEDFIT_CHI2        "SEDFIT_CHI2"
#define sclsvrCALIBRATOR_SEDFIT_DIAM        "SEDFIT_DIAM"
#define sclsvrCALIBRATOR_SEDFIT_TEFF        "SEDFIT_TEFF"
#define sclsvrCALIBRATOR_SEDFIT_AV          "SEDFIT_AV"

/* Teff / Logg determined from spectral type */
#define sclsvrCALIBRATOR_TEFF_SPTYP         "TEFF_SPTYP"
#define sclsvrCALIBRATOR_LOGG_SPTYP         "LOGG_SPTYP"

/* uniform disk diameters */
#define sclsvrCALIBRATOR_UD_U               "UD_U"
#define sclsvrCALIBRATOR_UD_B               "UD_B"
#define sclsvrCALIBRATOR_UD_V               "UD_V"
#define sclsvrCALIBRATOR_UD_R               "UD_R"
#define sclsvrCALIBRATOR_UD_I               "UD_I"
#define sclsvrCALIBRATOR_UD_J               "UD_J"
#define sclsvrCALIBRATOR_UD_H               "UD_H"
#define sclsvrCALIBRATOR_UD_K               "UD_K"
#define sclsvrCALIBRATOR_UD_L               "UD_L"
#define sclsvrCALIBRATOR_UD_M               "UD_M"
#define sclsvrCALIBRATOR_UD_N               "UD_N"

/* extinction ratio related to interstellar absorption */
#define sclsvrCALIBRATOR_EXTINCTION_RATIO   "EXTINCTION_RATIO"

/* chi2 of the extinction ratio estimation */
#define sclsvrCALIBRATOR_AV_FIT_CHI2        "AV_FIT_CHI2"

/* distance (parsec) computed from parallax */
#define sclsvrCALIBRATOR_DIST_PLX           "DIST_PLX"

/* fitted distance (parsec) computed from photometric magnitudes and spectral type */
#define sclsvrCALIBRATOR_DIST_FIT           "DIST_FIT"

/* chi2 of the distance modulus (dist_plx vs dist_fit) */
#define sclsvrCALIBRATOR_DIST_FIT_CHI2      "DIST_FIT_CHI2"

/* square visibility */
#define sclsvrCALIBRATOR_VIS2               "VIS2"

/* distance to the science object */
#define sclsvrCALIBRATOR_DIST               "DIST"

/* corrected spectral type */
#define sclsvrCALIBRATOR_SP_TYPE_JMMC       "SPECTRAL_TYPE"

/* index in color tables */
#define sclsvrCALIBRATOR_COLOR_TABLE_INDEX  "COLOR_TABLE_INDEX"

/* delta in color tables */
#define sclsvrCALIBRATOR_COLOR_TABLE_DELTA  "COLOR_TABLE_DELTA"

/* fixed index in color tables */
#define sclsvrCALIBRATOR_COLOR_TABLE_INDEX_FIX  "COLOR_TABLE_INDEX_FIX"

/* fixed delta in color tables */
#define sclsvrCALIBRATOR_COLOR_TABLE_DELTA_FIX  "COLOR_TABLE_DELTA_FIX"

/* luminosity class (1,2,3,4,5) (min) */
#define sclsvrCALIBRATOR_LUM_CLASS          "LUM_CLASS"
/* luminosity class delta (lc_max = lum_class + lum_class_delta) */
#define sclsvrCALIBRATOR_LUM_CLASS_DELTA    "LUM_CLASS_DELTA"

/* star name (identifier) */
#define sclsvrCALIBRATOR_NAME               "NAME"

/**
 * Av method.
 */
typedef enum
{
    sclsvrAV_METHOD_UNDEFINED  = 0, /** Undefined method */
    sclsvrAV_METHOD_UNKNOWN    = 1, /** Unknown (guess in range [0;3] ) */
    sclsvrAV_METHOD_STAT       = 2, /** Statistical estimation */
    sclsvrAV_METHOD_FIT        = 3, /** Fit from photometric magnitudes and spectral type with luminosity class */
    sclsvrAV_METHOD_MIN_CHI2   = 4, /** Best chi2 from fit from photometric magnitudes and spectral type (no luminosity class) */
} sclsvrAV_METHOD;


/** Initialize the property index used by sclsvrCALIBRATOR and vobsSTAR */
void sclsvrCalibratorBuildPropertyIndex();

/*
 * Class declaration
 */
class sclsvrCALIBRATOR : public vobsSTAR
{
public:
    // Constructors
    sclsvrCALIBRATOR();
    explicit sclsvrCALIBRATOR(const sclsvrCALIBRATOR& star);

    // Conversion Construstor
    explicit sclsvrCALIBRATOR(const vobsSTAR &star);

    // assignment operator =
    sclsvrCALIBRATOR& operator=(const sclsvrCALIBRATOR&) ;

    // Destructor
    virtual ~sclsvrCALIBRATOR();

    // Complete calibrator properties
    mcsCOMPL_STAT Complete(const sclsvrREQUEST &request, miscoDYN_BUF &msgInfo);

    // Return whether the calibrator has a coherent diameter or not
    mcsLOGICAL IsDiameterOk() const;

    static void FreePropertyIndex();

protected:

private:
    alxSPECTRAL_TYPE _spectralType;

    // Define all star properties
    mcsCOMPL_STAT AddProperties(void);

    static mcsINT32 sclsvrCALIBRATOR_PropertyMetaBegin;
    static mcsINT32 sclsvrCALIBRATOR_PropertyMetaEnd;
    static bool sclsvrCALIBRATOR_PropertyIdxInitialized;

    // Compute specific property
    mcsCOMPL_STAT ComputeAngularDiameter(miscoDYN_BUF &msgInfo);
    mcsCOMPL_STAT ComputeUDFromLDAndSP();
    mcsCOMPL_STAT ComputeVisibility(const sclsvrREQUEST &request);
    mcsCOMPL_STAT ComputeDistance(const sclsvrREQUEST &request);
    mcsCOMPL_STAT ComputeTeffLogg();
    mcsCOMPL_STAT ParseSpectralType();
    mcsCOMPL_STAT DefineSpectralTypeIndexes();
    mcsCOMPL_STAT ComputeSedFitting();

    mcsCOMPL_STAT ComputeExtinctionCoefficient();
    mcsCOMPL_STAT ComputeCousinMagnitudes();
    mcsCOMPL_STAT ComputeJohnsonMagnitudes();
    mcsCOMPL_STAT ComputeMissingMagnitude();
    mcsCOMPL_STAT ComputeIRFluxes();

    mcsCOMPL_STAT DefineCalFlag(const bool bright);
    
    static mcsCOMPL_STAT DumpPropertyIndexAsXML();

    mcsCOMPL_STAT ExtractMagnitudes(alxMAGNITUDES &magnitudes,
                                    const char** magIds,
                                    mcsDOUBLE defError,
                                    const vobsORIGIN_INDEX* originIdxs = NULL);

    mcsCOMPL_STAT ExtractMagnitudesAndFixErrors(alxMAGNITUDES &magnitudes, mcsLOGICAL faint = mcsFALSE);

    void CleanProperties();
} ;

#endif /*!sclsvrCALIBRATOR_H*/

/*___oOo___*/
