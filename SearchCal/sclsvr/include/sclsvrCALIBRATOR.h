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
 * TODO: use one proper data model to define properties (ID, Name, UCD, query column per catalog ...)
 */

/* galactic coordinates */
#define sclsvrCALIBRATOR_POS_GAL_LAT        "POS_GAL_LAT"
#define sclsvrCALIBRATOR_POS_GAL_LON        "POS_GAL_LON"

/* cousin fluxes (faint) */
#define sclsvrCALIBRATOR_PHOT_COUS_J        "PHOT_COUS_J"
#define sclsvrCALIBRATOR_PHOT_COUS_H        "PHOT_COUS_H"
#define sclsvrCALIBRATOR_PHOT_COUS_K        "PHOT_COUS_K"

/* computed diameters */
#define sclsvrCALIBRATOR_DIAM_BV            "DIAM_BV"
#define sclsvrCALIBRATOR_DIAM_BV_ERROR      "DIAM_BV_ERROR"
#define sclsvrCALIBRATOR_DIAM_VR            "DIAM_VR"
#define sclsvrCALIBRATOR_DIAM_VR_ERROR      "DIAM_VR_ERROR"
#define sclsvrCALIBRATOR_DIAM_VK            "DIAM_VK"
#define sclsvrCALIBRATOR_DIAM_VK_ERROR      "DIAM_VK_ERROR"
#define sclsvrCALIBRATOR_DIAM_IJ            "DIAM_IJ"
#define sclsvrCALIBRATOR_DIAM_IJ_ERROR      "DIAM_IJ_ERROR"
#define sclsvrCALIBRATOR_DIAM_IK            "DIAM_IK"
#define sclsvrCALIBRATOR_DIAM_IK_ERROR      "DIAM_IK_ERROR"
#define sclsvrCALIBRATOR_DIAM_JK            "DIAM_JK"
#define sclsvrCALIBRATOR_DIAM_JK_ERROR      "DIAM_JK_ERROR"
#define sclsvrCALIBRATOR_DIAM_JH            "DIAM_JH"
#define sclsvrCALIBRATOR_DIAM_JH_ERROR      "DIAM_JH_ERROR"
#define sclsvrCALIBRATOR_DIAM_HK            "DIAM_HK"
#define sclsvrCALIBRATOR_DIAM_HK_ERROR      "DIAM_HK_ERROR"

/* mean diameter */
#define sclsvrCALIBRATOR_DIAM_MEAN          "DIAM_MEAN"
#define sclsvrCALIBRATOR_DIAM_MEAN_ERROR    "DIAM_MEAN_ERROR"

/* diameter quality (OK | NOK) */
#define sclsvrCALIBRATOR_DIAM_FLAG          "DIAM_FLAG"

/* diameter quality information */
#define sclsvrCALIBRATOR_DIAM_FLAG_INFO     "DIAM_FLAG_INFO"

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
#define sclsvrCALIBRATOR_UD_N               "UD_N"

/* extinction ratio related to interstellar absorption (faint) */
#define sclsvrCALIBRATOR_EXTINCTION_RATIO   "EXTINCTION_RATIO"

/* square visibility */
#define sclsvrCALIBRATOR_VIS2               "VIS2"
#define sclsvrCALIBRATOR_VIS2_ERROR         "VIS2_ERROR"

/* square visibility at 8 and 13 mu (midi) */
#define sclsvrCALIBRATOR_VIS2_8             "VIS2_8"
#define sclsvrCALIBRATOR_VIS2_8_ERROR       "VIS2_8_ERROR"
#define sclsvrCALIBRATOR_VIS2_13            "VIS2_13"
#define sclsvrCALIBRATOR_VIS2_13_ERROR      "VIS2_13_ERROR"


/* distance to the science object */
#define sclsvrCALIBRATOR_DIST               "DIST"

/* corrected spectral type */
#define sclsvrCALIBRATOR_SP_TYPE            "SPECTRAL_TYPE"


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
    sclsvrCALIBRATOR& operator=(const sclsvrCALIBRATOR&);

    // Destructor
    virtual ~sclsvrCALIBRATOR();

    // Complete calibrator properties
    mcsCOMPL_STAT Complete(const sclsvrREQUEST &request);

    // Return whether the calibrator has a coherent diameter or not
    mcsLOGICAL IsDiameterOk() const;

    static void FreePropertyIndex();

protected:

private:
    alxSPECTRAL_TYPE _spectralType;

    // Define all star properties
    mcsCOMPL_STAT AddProperties(void);

    static int sclsvrCALIBRATOR_PropertyMetaBegin;
    static int sclsvrCALIBRATOR_PropertyMetaEnd;
    static bool sclsvrCALIBRATOR_PropertyIdxInitialized;

    // Compute specific property
    mcsCOMPL_STAT ComputeMissingMagnitude(mcsLOGICAL isBright);
    mcsCOMPL_STAT ComputeGalacticCoordinates();
    mcsCOMPL_STAT ComputeExtinctionCoefficient();
    mcsCOMPL_STAT ComputeInterstellarAbsorption(mcsLOGICAL isBright);
    mcsCOMPL_STAT ComputeApparentMagnitude(mcsLOGICAL isBright);
    mcsCOMPL_STAT ComputeAngularDiameter(mcsLOGICAL isBright);
    mcsCOMPL_STAT ComputeUDFromLDAndSP();
    mcsCOMPL_STAT ComputeVisibility(const sclsvrREQUEST &request);
    mcsCOMPL_STAT ComputeDistance(const sclsvrREQUEST &request);
    mcsCOMPL_STAT ComputeCousinMagnitudes();
    mcsCOMPL_STAT ComputeJohnsonMagnitudes();
    mcsCOMPL_STAT ComputeIRFluxes();
    mcsCOMPL_STAT ComputeTeffLogg();
    mcsCOMPL_STAT ComputeSpectralBinarity();
    mcsCOMPL_STAT ParseSpectralType();
    mcsCOMPL_STAT CheckParallax();
    mcsCOMPL_STAT CorrectSpectralType(mcsLOGICAL isBright);
};

#endif /*!sclsvrCALIBRATOR_H*/

/*___oOo___*/
