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
 * Constants definition
 */
/* Definition of the calibrators properties */
#define sclsvrCALIBRATOR_PHOT_COUS_J        "PHOT_COUS_J"
#define sclsvrCALIBRATOR_PHOT_COUS_H        "PHOT_COUS_H"
#define sclsvrCALIBRATOR_PHOT_COUS_K        "PHOT_COUS_K"
#define sclsvrCALIBRATOR_EXTINCTION_RATIO   "EXTINCTION_RATIO"
#define sclsvrCALIBRATOR_MO                 "MO"
#define sclsvrCALIBRATOR_LO                 "LO"
#define sclsvrCALIBRATOR_KO                 "KO"
#define sclsvrCALIBRATOR_HO                 "HO"
#define sclsvrCALIBRATOR_JO                 "JO"
#define sclsvrCALIBRATOR_IO                 "IO"
#define sclsvrCALIBRATOR_RO                 "RO"
#define sclsvrCALIBRATOR_VO                 "VO"
#define sclsvrCALIBRATOR_BO                 "BO"
#define sclsvrCALIBRATOR_VIS2               "VIS2"
#define sclsvrCALIBRATOR_VIS2_ERROR         "VIS2_ERROR"
#define sclsvrCALIBRATOR_VIS2_FLAG          "VIS2_FLAG"
#define sclsvrCALIBRATOR_VIS2_8             "VIS2_8"
#define sclsvrCALIBRATOR_VIS2_8_ERROR       "VIS2_8_ERROR"
#define sclsvrCALIBRATOR_VIS2_13            "VIS2_13"
#define sclsvrCALIBRATOR_VIS2_13_ERROR      "VIS2_13_ERROR"
#define sclsvrCALIBRATOR_DIAM_BV            "DIAM_BV"
#define sclsvrCALIBRATOR_DIAM_VR            "DIAM_VR"
#define sclsvrCALIBRATOR_DIAM_VK            "DIAM_VK"
#define sclsvrCALIBRATOR_DIAM_IJ            "DIAM_IJ"
#define sclsvrCALIBRATOR_DIAM_IK            "DIAM_IK"
#define sclsvrCALIBRATOR_DIAM_JK            "DIAM_JK"
#define sclsvrCALIBRATOR_DIAM_JH            "DIAM_JH"
#define sclsvrCALIBRATOR_DIAM_HK            "DIAM_HK"
#define sclsvrCALIBRATOR_DIAM_MEAN          "DIAM_MEAN"
#define sclsvrCALIBRATOR_DIAM_BV_ERROR      "DIAM_BV_ERROR"
#define sclsvrCALIBRATOR_DIAM_VR_ERROR      "DIAM_VR_ERROR"
#define sclsvrCALIBRATOR_DIAM_VK_ERROR      "DIAM_VK_ERROR"
#define sclsvrCALIBRATOR_DIAM_IJ_ERROR      "DIAM_IJ_ERROR"
#define sclsvrCALIBRATOR_DIAM_IK_ERROR      "DIAM_IK_ERROR"
#define sclsvrCALIBRATOR_DIAM_JK_ERROR      "DIAM_JK_ERROR"
#define sclsvrCALIBRATOR_DIAM_JH_ERROR      "DIAM_JH_ERROR"
#define sclsvrCALIBRATOR_DIAM_HK_ERROR      "DIAM_HK_ERROR"
#define sclsvrCALIBRATOR_DIAM_MEAN_ERROR    "DIAM_MEAN_ERROR"
#define sclsvrCALIBRATOR_DIAM_FLAG          "DIAM_FLAG"
#define sclsvrCALIBRATOR_TEFF_SPTYP         "TEFF_SPTYP"
#define sclsvrCALIBRATOR_LOGG_SPTYP         "LOGG_SPTYP"
#define sclsvrCALIBRATOR_UD_B               "UD_B"
#define sclsvrCALIBRATOR_UD_I               "UD_I"
#define sclsvrCALIBRATOR_UD_J               "UD_J"
#define sclsvrCALIBRATOR_UD_H               "UD_H"
#define sclsvrCALIBRATOR_UD_K               "UD_K"
#define sclsvrCALIBRATOR_UD_L               "UD_L"
#define sclsvrCALIBRATOR_UD_N               "UD_N"
#define sclsvrCALIBRATOR_UD_R               "UD_R"
#define sclsvrCALIBRATOR_UD_U               "UD_U"
#define sclsvrCALIBRATOR_UD_V               "UD_V"
#define sclsvrCALIBRATOR_DIST               "DIST"

/*
 * Class declaration
 */
class sclsvrCALIBRATOR : public vobsSTAR
{
public:
    // Constructors
    sclsvrCALIBRATOR();
    // Copy Construstor
    sclsvrCALIBRATOR(vobsSTAR &star);
     
    // Destructor
    virtual ~sclsvrCALIBRATOR();    
  
    // Complete calibrator properties
    mcsCOMPL_STAT Complete(sclsvrREQUEST &request);
    mcsCOMPL_STAT CompleteWithParallax(mcsLOGICAL isBright);
    mcsCOMPL_STAT CompleteWithoutParallax();

     // Return whether the calibrator has a coherent diameter or not
     virtual mcsLOGICAL IsDiameterOk();

protected:
    
private:
    // Define all star properties
    mcsCOMPL_STAT AddProperties(void);

    // Compute specific property
    mcsCOMPL_STAT ComputeMissingMagnitude(mcsLOGICAL isBright=mcsTRUE);
    mcsCOMPL_STAT ComputeGalacticCoordinates();
    mcsCOMPL_STAT ComputeExtinctionCoefficient();
    mcsCOMPL_STAT ComputeInterstellarAbsorption(mcsLOGICAL isBright=mcsTRUE);
    mcsCOMPL_STAT ComputeApparentMagnitude(mcsLOGICAL isBright=mcsTRUE);     
    mcsCOMPL_STAT ComputeAngularDiameter(mcsLOGICAL isBright=mcsTRUE);
    mcsCOMPL_STAT ComputeUDFromLDAndSP();
    mcsCOMPL_STAT ComputeVisibility(sclsvrREQUEST &request);
    mcsCOMPL_STAT ComputeDistance(sclsvrREQUEST &request); 
    mcsCOMPL_STAT ComputeCousinMagnitude(); 
    mcsCOMPL_STAT ComputeIRFluxes(); 
    mcsCOMPL_STAT ComputeTeffLogg(); 
};

#endif /*!sclsvrCALIBRATOR_H*/

/*___oOo___*/
