/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * @file
 * sclsvrCALIBRATOR class definition.
 */

/* 
 * System Headers 
 */
#include <iostream>
using namespace std;

#include <math.h>

/*
 * MCS Headers 
 */
#include "mcs.h"
#include "log.h"
#include "err.h"


/*
 * SCALIB Headers 
 */
#include "alx.h"
#include "alxErrors.h"


/*
 * Local Headers 
 */
#include "sclsvrPrivate.h"
#include "sclsvrErrors.h"
#include "sclsvrCALIBRATOR.h"


/* maximum number of properties (107) */
#define sclsvrCALIBRATOR_MAX_PROPERTIES 107

/* Error identifiers */

#define sclsvrCALIBRATOR_DIAM_BV_ERROR      "DIAM_BV_ERROR"
#define sclsvrCALIBRATOR_DIAM_BI_ERROR      "DIAM_BI_ERROR"
#define sclsvrCALIBRATOR_DIAM_BJ_ERROR      "DIAM_BJ_ERROR"
#define sclsvrCALIBRATOR_DIAM_BH_ERROR      "DIAM_BH_ERROR"
#define sclsvrCALIBRATOR_DIAM_BK_ERROR      "DIAM_BK_ERROR"
#define sclsvrCALIBRATOR_DIAM_VR_ERROR      "DIAM_VR_ERROR"
#define sclsvrCALIBRATOR_DIAM_VI_ERROR      "DIAM_VI_ERROR"
#define sclsvrCALIBRATOR_DIAM_VJ_ERROR      "DIAM_VJ_ERROR"
#define sclsvrCALIBRATOR_DIAM_VH_ERROR      "DIAM_VH_ERROR"
#define sclsvrCALIBRATOR_DIAM_VK_ERROR      "DIAM_VK_ERROR"
#define sclsvrCALIBRATOR_DIAM_IJ_ERROR      "DIAM_IJ_ERROR"
#define sclsvrCALIBRATOR_DIAM_IH_ERROR      "DIAM_IH_ERROR"
#define sclsvrCALIBRATOR_DIAM_IK_ERROR      "DIAM_IK_ERROR"
#define sclsvrCALIBRATOR_DIAM_JH_ERROR      "DIAM_JH_ERROR"
#define sclsvrCALIBRATOR_DIAM_JK_ERROR      "DIAM_JK_ERROR"
#define sclsvrCALIBRATOR_DIAM_HK_ERROR      "DIAM_HK_ERROR"

#define sclsvrCALIBRATOR_DIAM_MEAN_ERROR    "DIAM_MEAN_ERROR"
#define sclsvrCALIBRATOR_DIAM_MEDIAN_ERROR  "DIAM_MEDIAN_ERROR"
#define sclsvrCALIBRATOR_DIAM_WEIGHTED_MEAN_ERROR "DIAM_WEIGHTED_MEAN_ERROR"

#define sclsvrCALIBRATOR_SEDFIT_DIAM_ERROR  "SEDFIT_DIAM_ERROR"

#define sclsvrCALIBRATOR_EXTINCTION_RATIO_ERROR "EXTINCTION_RATIO_ERROR"

#define sclsvrCALIBRATOR_VIS2_ERROR         "VIS2_ERROR"
#define sclsvrCALIBRATOR_VIS2_8_ERROR       "VIS2_8_ERROR"
#define sclsvrCALIBRATOR_VIS2_13_ERROR      "VIS2_13_ERROR"

/**
 * Convenience macros
 */
#define SetComputedPropWithError(propId, alxDATA)                                                            \
if alxIsSet(alxDATA)                                                                                                    \
{                                                                                                                       \
    FAIL(SetPropertyValueAndError(propId, alxDATA.value, alxDATA.error, vobsORIG_COMPUTED, (vobsCONFIDENCE_INDEX) alxDATA.confIndex)); \
}


/** Initialize static members */
int sclsvrCALIBRATOR::sclsvrCALIBRATOR_PropertyMetaBegin = -1;
int sclsvrCALIBRATOR::sclsvrCALIBRATOR_PropertyMetaEnd = -1;
bool sclsvrCALIBRATOR::sclsvrCALIBRATOR_PropertyIdxInitialized = false;

/** Initialize the property index used by sclsvrCALIBRATOR and vobsSTAR */
void sclsvrCalibratorBuildPropertyIndex()
{
    sclsvrCALIBRATOR initCalibrator;
}

/**
 * Class constructor.
 */
sclsvrCALIBRATOR::sclsvrCALIBRATOR() : vobsSTAR()
{
    // Note: vobsSTAR() explicit constructor adds star properties

    ReserveProperties(sclsvrCALIBRATOR_MAX_PROPERTIES);

    // Add calibrator star properties 
    AddProperties();
}

/**
 * Conversion Constructor.
 */
sclsvrCALIBRATOR::sclsvrCALIBRATOR(const vobsSTAR &star)
{
    ReserveProperties(sclsvrCALIBRATOR_MAX_PROPERTIES);

    // apply vobsSTAR assignment operator between this and given star:
    // note: this includes copy of calibrator properties
    this->vobsSTAR::operator=(star);

    // Add calibrator star properties 
    AddProperties();
}

/**
 * Copy Constructor.
 */
sclsvrCALIBRATOR::sclsvrCALIBRATOR(const sclsvrCALIBRATOR& star)
{
    // Uses the operator=() method to copy
    *this = star;
}

/**
 * Assignment operator
 */
sclsvrCALIBRATOR& sclsvrCALIBRATOR::operator=(const sclsvrCALIBRATOR& star)
{
    if (this != &star)
    {
        ReserveProperties(sclsvrCALIBRATOR_MAX_PROPERTIES);

        // apply vobsSTAR assignment operator between this and given star:
        // note: this includes copy of calibrator properties
        this->vobsSTAR::operator=(star);

        // Copy spectral type
        _spectralType = star._spectralType;
    }
    return *this;
}

/**
 * Class destructor.
 */
sclsvrCALIBRATOR::~sclsvrCALIBRATOR()
{
}


/*
 * Public methods
 */

/**
 * Return whether the calibrator has a coherent diameter or not.
 */
mcsLOGICAL sclsvrCALIBRATOR::IsDiameterOk() const
{
    vobsSTAR_PROPERTY* property = GetProperty(sclsvrCALIBRATOR_DIAM_FLAG);

    if (isPropSet(property) && property->IsTrue())
    {
        return mcsTRUE;
    }

    // If diameter has not been computed yet or is not OK, return mcsFALSE
    return mcsFALSE;
}

/**
 * Complete the property of the calibrator.
 *
 * Method to complete calibrator properties by using several methods.
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is
 * returned.
 */
mcsCOMPL_STAT sclsvrCALIBRATOR::Complete(const sclsvrREQUEST &request, miscoDYN_BUF &msgInfo)
{
    // Reset the buffer that contains the diamFlagInfo string
    FAIL(msgInfo.Reset());

    mcsSTRING64 starId;

    // Get Star ID
    FAIL(GetId(starId, sizeof (starId)));

    logTest("----- Complete: star '%s'", starId);

    // Check parallax: set the property vobsSTAR_POS_PARLX_TRIG_FLAG (true | false)
    FAIL(CheckParallax());

    // Parse spectral type. Use Johnson B-V to access luminosity class
    // if unknow by comparing with colorTables
    FAIL(ParseSpectralType());

    // Fill in the Teff and LogG entries using the spectral type
    FAIL(ComputeTeffLogg());

    // Compute Galactic coordinates:
    FAIL(ComputeGalacticCoordinates());

    // Compute N Band and S_12 with AKARI from Teff
    FAIL(ComputeIRFluxes());

    // If parallax is OK, we compute absorption coefficient Av
    if (isTrue(IsParallaxOk()))
    {
        FAIL(ComputeExtinctionCoefficient());
    }

    FAIL(ComputeSedFitting());

    // Compute I, J, H, K COUSIN magnitude from Johnson catalogues
    FAIL(ComputeCousinMagnitudes());

    // Compute missing Magnitude
    // TODO: refine how to compute missing magnitude in FAINT/BRIGHT
    // TODO: refine if/how to us the computed magnitudes for
    // diameter computation.
    // Idea: a computed magnitude should be used for diameter if
    // it comes from a cataogues magnitude + color taken from 
    // a high confidence SpType (catalogue, plx known, V known...)
    if (isTrue(IsParallaxOk()))
    {
        FAIL(ComputeMissingMagnitude(request.IsBright()));
    }
    else
    {
        logTest("parallax is unknown; do not compute missing magnitude");
    }

    // Compute J, H, K JOHNSON magnitude (2MASS) from COUSIN
    FAIL(ComputeJohnsonMagnitudes());

    // If N-band scenario, we don't compute diameter ie only use those from MIDI
    if (strcmp(request.GetSearchBand(), "N") != 0)
    {
        FAIL(ComputeAngularDiameter(msgInfo));
    }

    // Compute UD from LD and SP
    FAIL(ComputeUDFromLDAndSP());

    /* LBO: TODO remove dead code ASAP */
#ifdef CHECK_PLX_OK_IN_BRIGHT
    // Discard the diameter if bright and no plx
    if ((strcmp(request.GetSearchBand(), "N") != 0) && isTrue(request.IsBright()) && isFalse(IsParallaxOk()))
    {
        logTest("parallax is unknown; diameter flag set to NOK (bright mode)", starId);

        // Overwrite diamFlag and diamFlagInfo properties:
        FAIL(SetPropertyValue(sclsvrCALIBRATOR_DIAM_FLAG, mcsFALSE, vobsORIG_COMPUTED, vobsCONFIDENCE_HIGH, mcsTRUE));

        msgInfo.AppendString(" BRIGHT_PLX_NOK");
        FAIL(SetPropertyValue(sclsvrCALIBRATOR_DIAM_FLAG_INFO, msgInfo.GetBuffer(), vobsORIG_COMPUTED, vobsCONFIDENCE_HIGH, mcsTRUE));
    }
#endif

    // TODO: move the two last steps into SearchCal GUI (Vis2 + distance)

    // Compute visibility and visibility error only if diameter OK or (UDDK, diam12)
    FAIL(ComputeVisibility(request));

    // Compute distance
    FAIL(ComputeDistance(request));

    return mcsSUCCESS;
}


/*
 * Private methods
 */

/**
 * Fill the given magnitudes using given property ids
 * @param magnitudes alxMAGNITUDES struct to fill
 * @param magPropertyId property ids
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is
 * returned.
 */
mcsCOMPL_STAT sclsvrCALIBRATOR::ExtractMagnitude(alxMAGNITUDES &magnitudes,
                                                 const char** magIds)
{
    vobsSTAR_PROPERTY* property;

    // For each magnitude
    for (int band = 0; band < alxNB_BANDS; band++)
    {
        property = GetProperty(magIds[band]);

        // Get the magnitude value
        if (isPropSet(property))
        {
            FAIL(GetPropertyValue(property, &magnitudes[band].value));
            magnitudes[band].isSet = mcsTRUE;
            magnitudes[band].confIndex = (alxCONFIDENCE_INDEX) property->GetConfidenceIndex();

            /* Extract error or put 0.1mag by default */
            FAIL(GetPropertyErrorOrDefault(property, &magnitudes[band].error, MIN_MAG_ERROR));
        }
        else
        {
            alxDATAClear(magnitudes[band]);
        }
    }
    return mcsSUCCESS;
}

/**
 * Compute missing magnitude.
 *
 * @param isBright true is it is for bright object
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is
 * returned.
 */
mcsCOMPL_STAT sclsvrCALIBRATOR::ComputeMissingMagnitude(mcsLOGICAL isBright)
{
    logTest("sclsvrCALIBRATOR::ComputeMissingMagnitude()");

    // Magnitudes to be used
    // PHOT_COUS bands should have been prepared before.
    static const char* magIds[alxNB_BANDS] = {
                                              vobsSTAR_PHOT_JHN_B,
                                              vobsSTAR_PHOT_JHN_V,
                                              vobsSTAR_PHOT_JHN_R,
                                              vobsSTAR_PHOT_COUS_I,
                                              vobsSTAR_PHOT_COUS_J,
                                              vobsSTAR_PHOT_COUS_H,
                                              vobsSTAR_PHOT_COUS_K,
                                              vobsSTAR_PHOT_JHN_L,
                                              vobsSTAR_PHOT_JHN_M
    };

    alxMAGNITUDES magnitudes;
    FAIL(ExtractMagnitude(magnitudes, magIds));

    /* Print out results */
    alxLogTestMagnitudes("Initial magnitudes:", "", magnitudes);

    // Get the extinction ratio
    mcsDOUBLE Av;
    FAIL(GetPropertyValue(sclsvrCALIBRATOR_EXTINCTION_RATIO, &Av));

    // Compute corrected magnitude
    // (remove the expected interstellar absorption)
    FAIL(alxComputeCorrectedMagnitudes("(Av)", Av, magnitudes));

    // Compute missing magnitudes
    if (isTrue(isBright))
    {
        FAIL(alxComputeMagnitudesForBrightStar(&_spectralType, magnitudes));
    }
    else
    {
        FAIL(alxComputeMagnitudesForFaintStar(&_spectralType, magnitudes));
    }

    // Compute apparent magnitude (apply back interstellar absorption)
    FAIL(alxComputeApparentMagnitudes(Av, magnitudes));

    // Set back the computed magnitude. Already existing magnitudes are not
    // overwritten. 
    for (int band = 0; band < alxNB_BANDS; band++)
    {
        if alxIsSet(magnitudes[band])
        {
            // note: use SetComputedPropWithError when magnitude error is computed:
            FAIL(SetPropertyValue(magIds[band], magnitudes[band].value, vobsORIG_COMPUTED,
                                  (vobsCONFIDENCE_INDEX) magnitudes[band].confIndex, mcsFALSE));
        }
    }

    return mcsSUCCESS;
}

/**
 * Compute galactic coordinates.
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is
 * returned.
 */
mcsCOMPL_STAT sclsvrCALIBRATOR::ComputeGalacticCoordinates()
{
    mcsDOUBLE gLat, gLon, ra, dec;

    // Get right ascension position in degree
    FAIL(GetRa(ra));

    // Get declinaison in degree
    FAIL(GetDec(dec));

    // Compute galactic coordinates from the retrieved ra and dec values
    FAIL(alxComputeGalacticCoordinates(ra, dec, &gLat, &gLon));

    // Set the galactic lattitude
    FAIL(SetPropertyValue(sclsvrCALIBRATOR_POS_GAL_LAT, gLat, vobsORIG_COMPUTED));

    // Set the galactic longitude
    FAIL(SetPropertyValue(sclsvrCALIBRATOR_POS_GAL_LON, gLon, vobsORIG_COMPUTED));

    return mcsSUCCESS;
}

/**
 * Compute extinction coefficient.
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is
 * returned.
 */
mcsCOMPL_STAT sclsvrCALIBRATOR::ComputeExtinctionCoefficient()
{
    FAIL_FALSE_DO(IsParallaxOk(), errAdd(sclsvrERR_MISSING_PROPERTY, vobsSTAR_POS_PARLX_TRIG, "interstellar absorption"));

    mcsDOUBLE plx, e_plx, gLat, gLon;
    mcsDOUBLE Av, e_Av;
    vobsSTAR_PROPERTY* property;

    // Get the value of the parallax and parallax error
    property = GetProperty(vobsSTAR_POS_PARLX_TRIG);
    FAIL(GetPropertyValueAndError(property, &plx, &e_plx));

    // Get the value of the galactic lattitude
    property = GetProperty(sclsvrCALIBRATOR_POS_GAL_LAT);
    FAIL_FALSE_DO(IsPropertySet(property), errAdd(sclsvrERR_MISSING_PROPERTY, sclsvrCALIBRATOR_POS_GAL_LAT, "interstellar absorption"));
    FAIL(GetPropertyValue(property, &gLat));

    // Get the value of the galactic longitude
    property = GetProperty(sclsvrCALIBRATOR_POS_GAL_LON);
    FAIL_FALSE_DO(IsPropertySet(property), errAdd(sclsvrERR_MISSING_PROPERTY, sclsvrCALIBRATOR_POS_GAL_LON, "interstellar absorption"));
    FAIL(GetPropertyValue(property, &gLon));

    // Compute Extinction ratio
    FAIL(alxComputeExtinctionCoefficient(&Av, &e_Av, plx, e_plx, gLat, gLon));

    // Set extinction ratio and error
    FAIL(SetPropertyValueAndError(sclsvrCALIBRATOR_EXTINCTION_RATIO, Av, e_Av, vobsORIG_COMPUTED));

    return mcsSUCCESS;
}

/**
 * Compute apparent diameter by fitting the observed SED
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is
 * returned.
 */
mcsCOMPL_STAT sclsvrCALIBRATOR::ComputeSedFitting()
{
    /*
     * if DEV_FLAG: perform sed fitting
     */
    if (!vobsIsDevFlag())
    {
        return mcsSUCCESS;
    }

    /* Extract the B V J H Ks magnitudes.
       The magnitude of the model SED are expressed in
       Bjohnson, Vjohnson, J2mass, H2mass, Ks2mass */
    static const char* magIds[alxNB_SED_BAND] = {
                                                 vobsSTAR_PHOT_JHN_B,
                                                 vobsSTAR_PHOT_JHN_V,
                                                 vobsSTAR_PHOT_JHN_J,
                                                 vobsSTAR_PHOT_JHN_H,
                                                 vobsSTAR_PHOT_JHN_K
    };
    alxDATA magnitudes[alxNB_SED_BAND];

    // LBO: may use ExtractMagnitudes ?
    mcsDOUBLE error;
    vobsSTAR_PROPERTY* property;
    for (int band = 0; band < alxNB_SED_BAND; band++)
    {
        property = GetProperty(magIds[band]);

        if (isNotPropSet(property))
        {
            alxDATAClear(magnitudes[band]);
            continue;
        }

        /* Extract value and confidence index */
        FAIL(GetPropertyValue(property, &magnitudes[band].value));
        magnitudes[band].isSet = mcsTRUE;
        magnitudes[band].confIndex = (alxCONFIDENCE_INDEX) property->GetConfidenceIndex();

        /* Extract error or put 0.1mag by default */
        FAIL(GetPropertyErrorOrDefault(property, &error, MIN_MAG_ERROR));

        /* Error cannot be more precise than an threshold of 0.05mag */
        error = (error > 0.05 ? error : 0.05);

        /* Hack to deal with the (too?) large error
           associated with bright stars in 2MASS */
        if ((band > 1) && (magnitudes[band].value < 6.0))
        {
            error = 0.05;
        }

        magnitudes[band].error = error;
    }

    /* Extract the extinction ratio with its uncertainty.
       When the Av is not known, the full range of approx 0..3
       is considered as valid for the fit. */
    mcsDOUBLE Av, e_Av;
    if (isPropSet(sclsvrCALIBRATOR_EXTINCTION_RATIO))
    {
        FAIL(GetPropertyValueAndError(sclsvrCALIBRATOR_EXTINCTION_RATIO, &Av, &e_Av));
    }
    else
    {
        Av = 0.5;
        e_Av = 2.0;
    }

    /* Perform the SED fitting */
    mcsDOUBLE bestDiam, lowerDiam, upperDiam, bestChi2, bestTeff, bestAv;

    // Ignore any failure:
    if (alxSedFitting(magnitudes, Av, e_Av, &bestDiam, &lowerDiam, &upperDiam,
                      &bestChi2, &bestTeff, &bestAv) == mcsSUCCESS)
    {
        /* Compute error as the maximum distance */
        mcsDOUBLE diamErr;
        diamErr = alxMax(fabs(upperDiam - bestDiam), fabs(lowerDiam - bestDiam));

        /* TODO:  define a confindence index for diameter, Teff and Av based on chi2,
           is V available, is Av known ... */

        /* Put values */
        FAIL(SetPropertyValueAndError(sclsvrCALIBRATOR_SEDFIT_DIAM, bestDiam, diamErr, vobsORIG_COMPUTED));
        FAIL(SetPropertyValue(sclsvrCALIBRATOR_SEDFIT_CHI2, bestChi2, vobsORIG_COMPUTED));
        FAIL(SetPropertyValue(sclsvrCALIBRATOR_SEDFIT_TEFF, bestTeff, vobsORIG_COMPUTED));
        FAIL(SetPropertyValue(sclsvrCALIBRATOR_SEDFIT_AV, bestAv, vobsORIG_COMPUTED));
    }

    return mcsSUCCESS;
}

/**
 * Compute angular diameter
 * 
 * @param isBright true is it is for bright object
 * @param buffer temporary buffer to write information messages
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is
 * returned.
 */
mcsCOMPL_STAT sclsvrCALIBRATOR::ComputeAngularDiameter(miscoDYN_BUF &msgInfo)
{
    // We will use these bands. PHOT_COUS bands should have been prepared before. 
    // Note: confidence index is high if magnitude comes directly from catalogues, 
    // medium or low if computed value
    static const char* magIds[alxNB_BANDS] = {
                                              vobsSTAR_PHOT_JHN_B,
                                              vobsSTAR_PHOT_JHN_V,
                                              vobsSTAR_PHOT_JHN_R,
                                              vobsSTAR_PHOT_COUS_I,
                                              /* old polynoms (JHK CIT) */
                                              /*
        vobsSTAR_PHOT_COUS_J,
        vobsSTAR_PHOT_COUS_H,
        vobsSTAR_PHOT_COUS_K,
         */
                                              /* new polynom fits (alain chelli) (JHK 2MASS) 18/09/2013 */
                                              vobsSTAR_PHOT_JHN_J,
                                              vobsSTAR_PHOT_JHN_H,
                                              vobsSTAR_PHOT_JHN_K,
                                              vobsSTAR_PHOT_JHN_L,
                                              vobsSTAR_PHOT_JHN_M
    };

    // Fill the magnitude structures
    alxMAGNITUDES magAvMin, magAvMax, magAv;
    FAIL(ExtractMagnitude(magAv, magIds));

    // We now have mag = {Bj, Vj, Rj, Jc, Ic, Hc, Kc, Lj, Mj}
    alxLogTestMagnitudes("Extracted magnitudes:", "", magAv);

    // Copy magnitudes:
    for (int band = 0; band < alxNB_BANDS; band++)
    {
        alxDATACopy(magAv[band], magAvMin[band]);
        alxDATACopy(magAv[band], magAvMax[band]);
    }


    /*
     * TODO: new approach (LBO) to handle e_Av error => propagate e_Av into magnitude error
     * 
     * magCorr = magObs - rc(band) / 3.1 x Av
     * 
     * var(magCorr) = var(magObs) + (rc(band) / 3.1)^2 x var(Av)
     */


    // Find the Av range to use:
    mcsDOUBLE Av, AvMin, AvMax;

#ifdef USE_AV_0
    /* use Av = 0 */
    Av = AvMin = AvMax = 0.0;
#else
    if (isPropSet(sclsvrCALIBRATOR_EXTINCTION_RATIO))
    {
        mcsDOUBLE e_Av;
        FAIL(GetPropertyValueAndError(sclsvrCALIBRATOR_EXTINCTION_RATIO, &Av, &e_Av));

        /* ensure Av >= 0 */
        AvMin = alxMax(0.0, Av - e_Av);
        AvMax = Av + e_Av;
    }
    else
    {
        Av = 0.2;
        AvMin = 0.0;
        AvMax = 3.0;
    }
#endif

    // Structure to fill with diameters
    alxDIAMETERS diameters, diamsAv, diamsAvMin, diamsAvMax;

    // e_Av is not propagated to corrected magnitudes => 0.0
    // Compute diameters for Av:
    FAIL(alxComputeCorrectedMagnitudes("(Av)   ", Av,    magAv));
    FAIL(alxComputeAngularDiameters   ("(Av)   ", magAv, diamsAv));

    if (AvMin != Av)
    {
        // Compute diameters for AvMin:
        FAIL(alxComputeCorrectedMagnitudes("(minAv)", AvMin,    magAvMin));
        FAIL(alxComputeAngularDiameters   ("(minAv)", magAvMin, diamsAvMin));
    }
    else
    {
        // Copy diamsAv => diamsAvMin:
        for (int band = 0; band < alxNB_DIAMS; band++)
        {
            alxDATACopy(diamsAv[band], diamsAvMin[band]);
        }
    }

    if (AvMax != Av)
    {
        // Compute diameter for AvMax:
        FAIL(alxComputeCorrectedMagnitudes("(maxAv)", AvMax,    magAvMax));
        FAIL(alxComputeAngularDiameters   ("(maxAv)", magAvMax, diamsAvMax));
    }
    else
    {
        // Copy diamsAv => diamsAvMax:
        for (int band = 0; band < alxNB_DIAMS; band++)
        {
            alxDATACopy(diamsAv[band], diamsAvMax[band]);
        }
    }

    // Compute the final diameter and its error
    for (int band = 0; band < alxNB_DIAMS; band++)
    {
        alxDATAClear(diameters[band]);

        /* ensure diameters within Av range are computed */
        if (alxIsSet(diamsAv[band]) && alxIsSet(diamsAvMin[band]) && alxIsSet(diamsAvMax[band]))
        {
            diameters[band].isSet = mcsTRUE;
            diameters[band].value = diamsAv[band].value;

            /* Uncertainty encompass diamAvmin and diamAvmax */
            diameters[band].error = alxMax(diamsAv[band].error,
                                           alxMax(fabs(diamsAv[band].value - diamsAvMin[band].value),
                                                  fabs(diamsAv[band].value - diamsAvMax[band].value))
                                           );

            /* Take the smallest confidence index */
            diameters[band].confIndex = min(diamsAvMin[band].confIndex, diamsAvMax[band].confIndex);
        }
    }

    /* Display results */
    alxLogTestAngularDiameters("(final)", diameters);

    // 3 diameters are required:
    const mcsUINT32 nbRequiredDiameters = 3;

    // Compute mean diameter:
    mcsUINT32 nbDiameters = 0;
    alxDATA meanDiam, medianDiam, weightedMeanDiam, stddevDiam, qualityDiam;

    /* may set low confidence to inconsistent diameters */
    FAIL(alxComputeMeanAngularDiameter(diameters, &meanDiam, &weightedMeanDiam,
                                       &medianDiam, &stddevDiam, &qualityDiam, &nbDiameters,
                                       nbRequiredDiameters, msgInfo.GetInternalMiscDYN_BUF()));

    /* Write Diameters now as their confidence may have been lowered in alxComputeMeanAngularDiameter() */
    SetComputedPropWithError(sclsvrCALIBRATOR_DIAM_BV, diameters[alxB_V_DIAM]);
    SetComputedPropWithError(sclsvrCALIBRATOR_DIAM_BI, diameters[alxB_I_DIAM]);
    SetComputedPropWithError(sclsvrCALIBRATOR_DIAM_BJ, diameters[alxB_J_DIAM]);
    SetComputedPropWithError(sclsvrCALIBRATOR_DIAM_BH, diameters[alxB_H_DIAM]);
    SetComputedPropWithError(sclsvrCALIBRATOR_DIAM_BK, diameters[alxB_K_DIAM]);

    SetComputedPropWithError(sclsvrCALIBRATOR_DIAM_VR, diameters[alxV_R_DIAM]);
    SetComputedPropWithError(sclsvrCALIBRATOR_DIAM_VI, diameters[alxV_I_DIAM]);
    SetComputedPropWithError(sclsvrCALIBRATOR_DIAM_VJ, diameters[alxV_J_DIAM]);
    SetComputedPropWithError(sclsvrCALIBRATOR_DIAM_VH, diameters[alxV_H_DIAM]);
    SetComputedPropWithError(sclsvrCALIBRATOR_DIAM_VK, diameters[alxV_K_DIAM]);

    SetComputedPropWithError(sclsvrCALIBRATOR_DIAM_IJ, diameters[alxI_J_DIAM]);
    SetComputedPropWithError(sclsvrCALIBRATOR_DIAM_IH, diameters[alxI_H_DIAM]);
    SetComputedPropWithError(sclsvrCALIBRATOR_DIAM_IK, diameters[alxI_K_DIAM]);

    SetComputedPropWithError(sclsvrCALIBRATOR_DIAM_JH, diameters[alxJ_H_DIAM]);
    SetComputedPropWithError(sclsvrCALIBRATOR_DIAM_JK, diameters[alxJ_K_DIAM]);

    SetComputedPropWithError(sclsvrCALIBRATOR_DIAM_HK, diameters[alxH_K_DIAM]);

    // Write DIAMETER COUNT
    FAIL(SetPropertyValue(sclsvrCALIBRATOR_DIAM_COUNT, (mcsINT32) nbDiameters, vobsORIG_COMPUTED));

    mcsLOGICAL diamFlag = mcsFALSE;

    // Write MEAN DIAMETER 
    if alxIsSet(meanDiam)
    {
        SetComputedPropWithError(sclsvrCALIBRATOR_DIAM_MEAN, meanDiam);
    }

    // Write MEDIAN DIAMETER 
    if alxIsSet(medianDiam)
    {
        SetComputedPropWithError(sclsvrCALIBRATOR_DIAM_MEDIAN, medianDiam);
    }

    // Write WEIGHTED MEAN DIAMETER
    if alxIsSet(weightedMeanDiam)
    {
        SetComputedPropWithError(sclsvrCALIBRATOR_DIAM_WEIGHTED_MEAN, weightedMeanDiam);

        // confidence index is alxLOW_CONFIDENCE when individual diameters are inconsistent with the weighted mean
        if (weightedMeanDiam.confIndex == alxCONFIDENCE_HIGH)
        {
            // diamFlag OK if diameters are consistent: 
            diamFlag = mcsTRUE;
        }
        else
        {
            logTest("Computed diameters are not consistent between them; Weighted mean diameter is not kept");
        }
    }

    // Write DIAMETER STDDEV
    if alxIsSet(stddevDiam)
    {
        FAIL(SetPropertyValue(sclsvrCALIBRATOR_DIAM_STDDEV, stddevDiam.value, vobsORIG_COMPUTED, (vobsCONFIDENCE_INDEX) stddevDiam.confIndex));
    }

    // Write the diameter flag (true | false):
    FAIL(SetPropertyValue(sclsvrCALIBRATOR_DIAM_FLAG, diamFlag, vobsORIG_COMPUTED));

    // Write the diameter quality (0.0 to 10.0):
    if alxIsSet(qualityDiam)
    {
        FAIL(SetPropertyValue(sclsvrCALIBRATOR_DIAM_QUALITY, qualityDiam.value, vobsORIG_COMPUTED));
    }

    // Write DIAM INFO
    mcsUINT32 storedBytes;
    FAIL(msgInfo.GetNbStoredBytes(&storedBytes));
    if (storedBytes > 0)
    {
        FAIL(SetPropertyValue(sclsvrCALIBRATOR_DIAM_FLAG_INFO, msgInfo.GetBuffer(), vobsORIG_COMPUTED));
    }

    return mcsSUCCESS;
}

/**
 * Compute UD from LD and SP.
 *
 * @return mcsSUCCESS on successful completion or computation cancellation.
 * Otherwise mcsFAILURE is returned.
 */
mcsCOMPL_STAT sclsvrCALIBRATOR::ComputeUDFromLDAndSP()
{
    // Compute UD only if LD is OK
    SUCCESS_FALSE_DO(IsDiameterOk(), logTest("Compute UD - Skipping (diameters are not OK)."));

    vobsSTAR_PROPERTY* property;
    property = GetProperty(sclsvrCALIBRATOR_DIAM_WEIGHTED_MEAN);

    // Does weighted mean diameter exist
    SUCCESS_FALSE_DO(IsPropertySet(property), logTest("Compute UD - Skipping (DIAM_WEIGHTED_MEAN unknown)."));

    // Get weighted mean diameter
    mcsDOUBLE ld = NAN;
    SUCCESS_DO(GetPropertyValue(property, &ld), logWarning("Compute UD - Aborting (error while retrieving DIAM_WEIGHTED_MEAN)."));

    // Get LD diameter confidence index (UDs will have the same one)
    vobsCONFIDENCE_INDEX ldConfIndex = property->GetConfidenceIndex();

    // Does Teff exist ?
    property = GetProperty(sclsvrCALIBRATOR_TEFF_SPTYP);
    SUCCESS_FALSE_DO(IsPropertySet(property), logTest("Compute UD - Skipping (Teff unknown)."));

    mcsDOUBLE Teff = NAN;
    SUCCESS_DO(GetPropertyValue(property, &Teff), logWarning("Compute UD - Aborting (error while retrieving Teff)."))

    // Does LogG exist ?
    property = GetProperty(sclsvrCALIBRATOR_LOGG_SPTYP);
    SUCCESS_FALSE_DO(IsPropertySet(property), logTest("Compute UD - Skipping (LogG unknown)."));

    mcsDOUBLE LogG = NAN;
    SUCCESS_DO(GetPropertyValue(property, &LogG), logWarning("Compute UD - Aborting (error while retrieving LogG)."));

    // Compute UD
    logTest("Computing UDs for LD=%lf, Teff=%lf, LogG=%lf ...", ld, Teff, LogG);

    alxUNIFORM_DIAMETERS ud;
    SUCCESS_DO(alxComputeUDFromLDAndSP(ld, Teff, LogG, &ud), logWarning("Aborting (error while computing UDs)."));

    // Set each UD_ properties accordingly:
    FAIL(SetPropertyValue(sclsvrCALIBRATOR_UD_U, ud.u, vobsORIG_COMPUTED, ldConfIndex));
    FAIL(SetPropertyValue(sclsvrCALIBRATOR_UD_B, ud.b, vobsORIG_COMPUTED, ldConfIndex));
    FAIL(SetPropertyValue(sclsvrCALIBRATOR_UD_V, ud.v, vobsORIG_COMPUTED, ldConfIndex));
    FAIL(SetPropertyValue(sclsvrCALIBRATOR_UD_R, ud.r, vobsORIG_COMPUTED, ldConfIndex));
    FAIL(SetPropertyValue(sclsvrCALIBRATOR_UD_I, ud.i, vobsORIG_COMPUTED, ldConfIndex));
    FAIL(SetPropertyValue(sclsvrCALIBRATOR_UD_J, ud.j, vobsORIG_COMPUTED, ldConfIndex));
    FAIL(SetPropertyValue(sclsvrCALIBRATOR_UD_H, ud.h, vobsORIG_COMPUTED, ldConfIndex));
    FAIL(SetPropertyValue(sclsvrCALIBRATOR_UD_K, ud.k, vobsORIG_COMPUTED, ldConfIndex));
    FAIL(SetPropertyValue(sclsvrCALIBRATOR_UD_L, ud.l, vobsORIG_COMPUTED, ldConfIndex));
    FAIL(SetPropertyValue(sclsvrCALIBRATOR_UD_N, ud.n, vobsORIG_COMPUTED, ldConfIndex));

    return mcsSUCCESS;
}

/**
 * Compute visibility.
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is
 * returned.
 */
mcsCOMPL_STAT sclsvrCALIBRATOR::ComputeVisibility(const sclsvrREQUEST &request)
{
    mcsDOUBLE diam, diamError, baseMax, wavelength;
    alxVISIBILITIES visibilities;
    vobsCONFIDENCE_INDEX confidenceIndex = vobsCONFIDENCE_HIGH;

    // Get object diameter. First look at the diameters coming from catalog
    // Borde (UDDK), Merand (UDDK) and MIDI (DIAM12)
    static const int nDiamId = 2;
    static const char* diamId[nDiamId] = { vobsSTAR_UDDK_DIAM, vobsSTAR_DIAM12 };

    vobsSTAR_PROPERTY* property;

    // For each possible diameters
    mcsLOGICAL found = mcsFALSE;
    for (int i = 0; (i < nDiamId) && isFalse(found); i++)
    {
        property = GetProperty(diamId[i]);

        // If diameter and its error are set 
        if (isPropSet(property) && isErrorSet(property))
        {
            // Get values
            FAIL(GetPropertyValueAndError(property, &diam, &diamError));
            found = mcsTRUE;

            // Set confidence index to high (value coming from catalog)
            confidenceIndex = vobsCONFIDENCE_HIGH;
        }
    }

    // If not found in catalog, use the computed one (if exist)
    if (isFalse(found))
    {
        // If computed diameter is OK
        SUCCESS_COND_DO(isFalse(IsDiameterOk()) || isFalse(IsPropertySet(sclsvrCALIBRATOR_DIAM_WEIGHTED_MEAN)),
                        logTest("Unknown mean diameter; could not compute visibility"));

        // FIXME: totally wrong => should use the UD diameter for the appropriate band (see Aspro2)
        // But move that code into SearchCal GUI instead.

        // Get weighted mean diameter and associated error value
        property = GetProperty(sclsvrCALIBRATOR_DIAM_WEIGHTED_MEAN);
        FAIL(GetPropertyValueAndError(property, &diam, &diamError));

        // Get confidence index of computed diameter
        confidenceIndex = property->GetConfidenceIndex();
    }

    // Get value in request of the wavelength
    wavelength = request.GetObservingWlen();

    // Get value in request of the base max
    baseMax = request.GetMaxBaselineLength();

    FAIL(alxComputeVisibility(diam, diamError, baseMax, wavelength, &visibilities));

    // Affect visibility property
    FAIL(SetPropertyValueAndError(sclsvrCALIBRATOR_VIS2, visibilities.vis2, visibilities.vis2Error, vobsORIG_COMPUTED, confidenceIndex));

    return mcsSUCCESS;
}

/**
 * Compute distance.
 *
 * This method calculate the distance in degree between the calibrator and the
 * science object.
 *
 * @return Always mcsSUCCESS.
 */
mcsCOMPL_STAT sclsvrCALIBRATOR::ComputeDistance(const sclsvrREQUEST &request)
{
    // Get the science object right ascension as a C string
    const char* ra = request.GetObjectRa();

    FAIL_COND(isNull(ra) || isTrue(miscIsSpaceStr(ra)));

    // Get science object right ascension in degrees
    mcsDOUBLE scienceObjectRa = request.GetObjectRaInDeg();

    // Get the science object declinaison as a C string
    const char* dec = request.GetObjectDec();

    FAIL_COND(isNull(dec) || isTrue(miscIsSpaceStr(dec)));

    // Get science object declination in degrees
    mcsDOUBLE scienceObjectDec = request.GetObjectDecInDeg();

    // Get the internal calibrator right acsension in arcsec
    mcsDOUBLE calibratorRa;
    FAIL(GetRa(calibratorRa));

    // Get the internal calibrator declinaison in arcsec
    mcsDOUBLE calibratorDec;
    FAIL(GetDec(calibratorDec));

    // Compute the separation in deg between the science object and the
    // calibrator using an alx provided function
    mcsDOUBLE separation;
    FAIL(alxComputeDistanceInDegrees(scienceObjectRa, scienceObjectDec, calibratorRa, calibratorDec, &separation));

    // Put the computed distance in the corresponding calibrator property
    FAIL(SetPropertyValue(sclsvrCALIBRATOR_DIST, separation, vobsORIG_COMPUTED));

    return mcsSUCCESS;
}

/**
 * Parse spectral type if available.
 * 
 * @return mcsSUCCESS on successfull parsing, mcsFAILURE otherwise.
 */
mcsCOMPL_STAT sclsvrCALIBRATOR::ParseSpectralType()
{
    // initialize the spectral type structure anyway:
    alxInitializeSpectralType(&_spectralType);

    vobsSTAR_PROPERTY* property = GetProperty(vobsSTAR_SPECT_TYPE_MK);

    SUCCESS_FALSE_DO(IsPropertySet(property), logTest("Spectral Type - Skipping (no SpType available)."));

    mcsSTRING32 spType;
    strncpy(spType, GetPropertyValue(property), sizeof (spType) - 1);

    SUCCESS_COND_DO(isStrEmpty(spType), logTest("Spectral Type - Skipping (SpType unknown)."));

    logDebug("Parsing Spectral Type '%s'.", spType);

    /* 
     * Get each part of the spectral type XN.NLLL where X is a letter, N.N a
     * number between 0 and 9 and LLL is the light class
     */
    SUCCESS_DO(alxString2SpectralType(spType, &_spectralType),
               errCloseStack();
               logTest("Spectral Type - Skipping (could not parse SpType '%s').", spType));

    // Check and correct luminosity class using B-V magnitudes:
    vobsSTAR_PROPERTY* magB = GetProperty(vobsSTAR_PHOT_JHN_B);
    vobsSTAR_PROPERTY* magV = GetProperty(vobsSTAR_PHOT_JHN_V);

    if (isPropSet(magB) && isPropSet(magV))
    {
        mcsDOUBLE mV, mB;
        FAIL(GetPropertyValue(magB, &mB));
        FAIL(GetPropertyValue(magV, &mV));

        FAIL(alxCorrectSpectralType(&_spectralType, mB - mV));
    }

    if (isTrue(_spectralType.isSpectralBinary))
    {
        logTest("Spectral Binarity - 'SB' found in SpType.");

        // Only store spectral binarity if none present before
        FAIL(SetPropertyValue(vobsSTAR_CODE_BIN_FLAG, "SB", vobsORIG_COMPUTED));
    }

    if (isTrue(_spectralType.isDouble))
    {
        logTest("Spectral Binarity - '+' found in SpType.");

        // Only store spectral binarity if none present before
        FAIL(SetPropertyValue(vobsSTAR_CODE_MULT_FLAG, "S", vobsORIG_COMPUTED));
    }

    // Anyway, store our decoded spectral type:
    FAIL(SetPropertyValue(sclsvrCALIBRATOR_SP_TYPE, _spectralType.ourSpType, vobsORIG_COMPUTED));

    return mcsSUCCESS;
}

/**
 * Compute Teff and Log(g) from the SpType and Tables
 * 
 * @return Always mcsSUCCESS.
 */
mcsCOMPL_STAT sclsvrCALIBRATOR::ComputeTeffLogg()
{
    SUCCESS_FALSE_DO(_spectralType.isSet, logTest("Teff and LogG - Skipping (SpType unknown)."));

    mcsDOUBLE Teff = NAN;
    mcsDOUBLE LogG = NAN;

    //Get Teff 
    SUCCESS_DO(alxComputeTeffAndLoggFromSptype(&_spectralType, &Teff, &LogG),
               logTest("Teff and LogG - Skipping (alxComputeTeffAndLoggFromSptype() failed on this spectral type: '%s').", _spectralType.origSpType));

    // Set Teff eand LogG properties
    FAIL(SetPropertyValue(sclsvrCALIBRATOR_TEFF_SPTYP, Teff, vobsORIG_COMPUTED));
    FAIL(SetPropertyValue(sclsvrCALIBRATOR_LOGG_SPTYP, LogG, vobsORIG_COMPUTED));

    return mcsSUCCESS;
}

/**
 * Compute Infrared Fluxes and N band using Akari
 * 
 * @return Always mcsSUCCESS.
 */
mcsCOMPL_STAT sclsvrCALIBRATOR::ComputeIRFluxes()
{
    mcsLOGICAL has9 = mcsFALSE;
    mcsLOGICAL has18 = mcsFALSE;
    mcsLOGICAL hasErr_9 = mcsFALSE;
    mcsLOGICAL hasErr_18 = mcsFALSE;
    mcsLOGICAL f12AlreadySet = mcsFALSE;
    mcsLOGICAL e_f12AlreadySet = mcsFALSE;
    mcsDOUBLE Teff = NAN;
    mcsDOUBLE fnu_9 = NAN;
    mcsDOUBLE e_fnu_9 = NAN;
    mcsDOUBLE fnu_12 = NAN;
    mcsDOUBLE e_fnu_12 = NAN;
    mcsDOUBLE fnu_18 = NAN;
    mcsDOUBLE e_fnu_18 = NAN;
    mcsDOUBLE magN = NAN;

    // initial tests of presence of data:

    vobsSTAR_PROPERTY* property = GetProperty(sclsvrCALIBRATOR_TEFF_SPTYP);

    // Get the value of Teff. If impossible, no possibility to go further!
    SUCCESS_FALSE_DO(IsPropertySet(property), logTest("IR Fluxes: Skipping (no Teff available)."));

    // Retrieve it
    FAIL(GetPropertyValue(property, &Teff));

    property = GetProperty(vobsSTAR_PHOT_FLUX_IR_09);

    // Get fnu_09 (vobsSTAR_PHOT_FLUX_IR_09)
    if (isPropSet(property))
    {
        // retrieve it
        FAIL(GetPropertyValue(property, &fnu_9));
        has9 = mcsTRUE;
    }

    // Get e_fnu_09 (vobsSTAR_PHOT_FLUX_IR_09)
    if (isErrorSet(property))
    {
        // retrieve it
        FAIL(GetPropertyError(property, &e_fnu_9));
        hasErr_9 = mcsTRUE;
    }

    property = GetProperty(vobsSTAR_PHOT_FLUX_IR_18);

    // Get fnu_18 (vobsSTAR_PHOT_FLUX_IR_18)
    if (isPropSet(property))
    {
        // retrieve it
        FAIL(GetPropertyValue(property, &fnu_18));
        has18 = mcsTRUE;
    }

    // Get e_fnu_18 (vobsSTAR_PHOT_FLUX_IR_18)
    if (isErrorSet(property))
    {
        // retrieve it
        FAIL(GetPropertyError(property, &e_fnu_18));
        hasErr_18 = mcsTRUE;
    }

    // get out if no *measured* infrared fluxes
    SUCCESS_COND_DO(isFalse(has9) && isFalse(has18), logTest("IR Fluxes: Skipping (no 9 mu or 18 mu flux available)."));

    property = GetProperty(vobsSTAR_PHOT_FLUX_IR_12);

    // check presence etc of F12:
    f12AlreadySet = IsPropertySet(property);

    // check presence etc of e_F12:
    e_f12AlreadySet = IsPropertyErrorSet(property);

    // if f9 is defined, compute all fluxes from it, then fill void places.
    if (isTrue(has9))
    {
        // Compute all fluxes from 9 onwards
        SUCCESS_DO(alxComputeFluxesFromAkari09(Teff, &fnu_9, &fnu_12, &fnu_18), logWarning("IR Fluxes: Skipping (akari internal error)."));

        logTest("IR Fluxes: akari measured fnu_09=%lf computed fnu_12=%lf fnu_18=%lf", fnu_9, fnu_12, fnu_18);

        // Store it eventually:
        if (isFalse(f12AlreadySet))
        {
            FAIL(SetPropertyValue(vobsSTAR_PHOT_FLUX_IR_12, fnu_12, vobsORIG_COMPUTED));
        }
        // Compute Mag N:
        magN = 4.1 - 2.5 * log10(fnu_12 / 0.89);

        logTest("IR Fluxes: computed magN=%lf", magN);

        // Store it if not set:
        FAIL(SetPropertyValue(vobsSTAR_PHOT_JHN_N, magN, vobsORIG_COMPUTED));

        // store s18 if void:
        if (isFalse(has18))
        {
            FAIL(SetPropertyValue(vobsSTAR_PHOT_FLUX_IR_18, fnu_18, vobsORIG_COMPUTED));
        }

        // compute s_12 error etc, if s09_err is present:
        if (isTrue(hasErr_9))
        {
            SUCCESS_DO(alxComputeFluxesFromAkari09(Teff, &e_fnu_9, &e_fnu_12, &e_fnu_18), logTest("IR Fluxes: Skipping (akari internal error)."));

            // Store it eventually:
            if (isFalse(e_f12AlreadySet))
            {
                FAIL(SetPropertyError(vobsSTAR_PHOT_FLUX_IR_12, e_fnu_12));
            }
            // store e_s18 if void:
            if (isFalse(hasErr_18))
            {
                FAIL(SetPropertyError(vobsSTAR_PHOT_FLUX_IR_18, e_fnu_18));
            }
        }
    }
    else
    {
        // only s18 !

        // Compute all fluxes from 18  backwards
        SUCCESS_DO(alxComputeFluxesFromAkari18(Teff, &fnu_18, &fnu_12, &fnu_9), logTest("IR Fluxes: Skipping (akari internal error)."));

        logTest("IR Fluxes: akari measured fnu_18=%lf computed fnu_12=%lf fnu_09=%lf", fnu_18, fnu_12, fnu_9);

        // Store it eventually:
        if (isFalse(f12AlreadySet))
        {
            FAIL(SetPropertyValue(vobsSTAR_PHOT_FLUX_IR_12, fnu_12, vobsORIG_COMPUTED));
        }
        // Compute Mag N:
        magN = 4.1 - 2.5 * log10(fnu_12 / 0.89);

        logTest("IR Fluxes: computed magN=%lf", magN);

        // Store it if not set:
        FAIL(SetPropertyValue(vobsSTAR_PHOT_JHN_N, magN, vobsORIG_COMPUTED));

        // store s9 if void:
        if (isFalse(has9))
        {
            FAIL(SetPropertyValue(vobsSTAR_PHOT_FLUX_IR_09, fnu_9, vobsORIG_COMPUTED));
        }

        // compute s_12 error etc, if s18_err is present:
        if (isTrue(hasErr_18))
        {
            SUCCESS_DO(alxComputeFluxesFromAkari18(Teff, &e_fnu_18, &e_fnu_12, &e_fnu_9), logTest("IR Fluxes: Skipping (akari internal error)."));

            // Store it eventually:
            if (isFalse(e_f12AlreadySet))
            {
                FAIL(SetPropertyError(vobsSTAR_PHOT_FLUX_IR_12, e_fnu_12));
            }
            // store e_s9 if void:
            if (isFalse(hasErr_9))
            {
                FAIL(SetPropertyError(vobsSTAR_PHOT_FLUX_IR_09, e_fnu_9));
            }
        }
    }

    return mcsSUCCESS;
}

mcsCOMPL_STAT sclsvrCALIBRATOR::CheckParallax()
{
    bool plxOk = false;

    vobsSTAR_PROPERTY* property = GetProperty(vobsSTAR_POS_PARLX_TRIG);

    // If parallax of the star if known
    if (isPropSet(property))
    {
        mcsDOUBLE parallax;

        // Check parallax
        FAIL(GetPropertyValue(property, &parallax));

        // Get error
        if (isErrorSet(property))
        {
            mcsDOUBLE parallaxError = -1.0;
            FAIL(GetPropertyError(property, &parallaxError));

            // If parallax is negative 
            if (parallax <= 0.0)
            {
                logTest("parallax %.2lf(%.2lf) is not valid...", parallax, parallaxError);
            }
            else if (parallax < 1.0)
            {
                // If parallax is less than 1 mas 
                logTest("parallax %.2lf(%.2lf) less than 1 mas...", parallax, parallaxError);
            }
            else if (parallaxError <= 0.0)
            {
                // If parallax error is invalid 
                logTest("parallax error %.2lf is not valid...", parallaxError);
            }
#ifdef CHECK_PARALLAX_ERROR_25P
            else if ((parallaxError / parallax) >= 0.25)
            {
                // Note: precise such threshold 25% or 50% ...

                // If parallax error is too high 
                logTest("parallax %.2lf(%.2lf) is not valid...", parallax, parallaxError);
            }
#endif            
            else
            {
                // parallax OK
                logTest("parallax %.2lf(%.2lf) is valid...", parallax, parallaxError);
                plxOk = true;
            }
        }
        else
        {
            // If parallax error is unknown 
            logTest("parallax error is unknown...");
        }
    }

    // Set parallax flag:
    FAIL(SetPropertyValue(vobsSTAR_POS_PARLX_TRIG_FLAG, (plxOk) ? mcsTRUE : mcsFALSE, vobsORIG_COMPUTED));

    return mcsSUCCESS;
}

/**
 * Return whether the parallax is OK or not.
 */
mcsLOGICAL sclsvrCALIBRATOR::IsParallaxOk() const
{
    vobsSTAR_PROPERTY* property = GetProperty(vobsSTAR_POS_PARLX_TRIG_FLAG);

    if (isPropSet(property) && property->IsTrue())
    {
        return mcsTRUE;
    }

    // If parallax flag has not been computed yet or is not OK, return mcsFALSE
    return mcsFALSE;
}

/**
 * Fill the J, H and K COUSIN/CIT magnitude from the JOHNSON.
 *
 * @return  mcsSUCCESS on successful completion. Otherwise mcsFAILURE is
 * returned.
 */
mcsCOMPL_STAT sclsvrCALIBRATOR::ComputeCousinMagnitudes()
{
    /*
     * Note: this method performs color conversions 
     * so both magnitudes (ie color) MUST come from the same catalog (same origin) !
     * 
     * Few mixed cases ie magnitudes coming from different catalogs that are impossible to convert:
     * 
     * vobsCATALOG_CIO_ID          "II/225/catalog" (missing case)
     * vobsCATALOG_DENIS_JK_ID     "J/A+A/413/1037/table1"
     * vobsCATALOG_LBSI_ID         "J/A+A/393/183/catalog"
     * vobsCATALOG_MASS_ID         "II/246/out"
     * vobsCATALOG_PHOTO_ID        "II/7A/catalog"
     * 
     * JSDC scenario:
     * Hc not computed: unsupported case = origins H (II/246/out) K (II/7A/catalog)
     * Hc not computed: unsupported case = origins H (II/246/out) K (II/7A/catalog)
     * Hc not computed: unsupported case = origins H (II/246/out) K (II/7A/catalog)
     * Hc not computed: unsupported case = origins H (II/246/out) K (II/7A/catalog)
     * 
     * Jc not computed: unsupported case = origins J (II/225/catalog) K (II/246/out)
     * Jc not computed: unsupported case = origins J (II/225/catalog) K (II/246/out)
     * Jc not computed: unsupported case = origins J (II/246/out) K (II/7A/catalog)
     * Jc not computed: unsupported case = origins J (II/246/out) K (II/7A/catalog)
     * Jc not computed: unsupported case = origins J (II/246/out) K (II/7A/catalog)
     * Jc not computed: unsupported case = origins J (J/A+A/413/1037/table1) K (II/246/out)
     * 
     * Kc not computed: unsupported case = origins J (II/246/out) K (II/225/catalog)
     * Kc not computed: unsupported case = origins J (II/246/out) K (II/225/catalog)
     * Kc not computed: unsupported case = origins J (II/246/out) K (J/A+A/413/1037/table1)
     * Kc not computed: unsupported case = origins J (II/7A/catalog) K (J/A+A/413/1037/table1)
     */

    // Define the Cousin magnitudes and errors to NaN
    mcsDOUBLE mJc = NAN;
    mcsDOUBLE mHc = NAN;
    mcsDOUBLE mKc = NAN;
    mcsDOUBLE eJc = NAN;
    mcsDOUBLE eHc = NAN;
    mcsDOUBLE eKc = NAN;

    vobsSTAR_PROPERTY* magK = GetProperty(vobsSTAR_PHOT_JHN_K);

    // check if the K magnitude is defined:
    if (isPropSet(magK))
    {
        mcsDOUBLE mK;
        FAIL(GetPropertyValue(magK, &mK));

        // Define the properties of the existing magnitude (V, J, H, K)
        vobsSTAR_PROPERTY* magV = GetProperty(vobsSTAR_PHOT_JHN_V);
        vobsSTAR_PROPERTY* magJ = GetProperty(vobsSTAR_PHOT_JHN_J);
        vobsSTAR_PROPERTY* magH = GetProperty(vobsSTAR_PHOT_JHN_H);

        // Origin for catalog magnitudes:
        vobsORIGIN_INDEX oriJ = magJ->GetOriginIndex();
        vobsORIGIN_INDEX oriH = magH->GetOriginIndex();
        vobsORIGIN_INDEX oriK = magK->GetOriginIndex();

        // Derived origin for Cousin/CIT magnitudes:
        vobsORIGIN_INDEX oriJc = vobsORIG_COMPUTED;
        vobsORIGIN_INDEX oriHc = vobsORIG_COMPUTED;
        vobsORIGIN_INDEX oriKc = vobsORIG_COMPUTED;

        // Get errors once:
        mcsDOUBLE eV, eJ, eH, eK;
        // Use NaN to avoid using undefined error:
        FAIL(GetPropertyErrorOrDefault(magV, &eV, NAN));
        FAIL(GetPropertyErrorOrDefault(magJ, &eJ, NAN));
        FAIL(GetPropertyErrorOrDefault(magH, &eH, NAN));
        FAIL(GetPropertyErrorOrDefault(magK, &eK, NAN));


        // Compute The COUSIN/CIT Kc band 
        if (isCatalog2Mass(oriK) || isCatalogMerand(oriK))
        {
            // From 2MASS or Merand (actually 2MASS)
            // see Carpenter eq.12
            mKc = mK + 0.024;
            eKc = eK;
            oriKc = oriK;
        }
        else if (isPropSet(magJ) && isCatalogDenisJK(oriJ) && isCatalogDenisJK(oriK))
        {
            // From J and K coming from Denis JK
            // see Carpenter, eq.12 and 16
            mcsDOUBLE mJ;
            FAIL(GetPropertyValue(magJ, &mJ));

            mKc = mK + 0.006 * (mJ - mK);
            eKc = 0.994 * eK + 0.006 * eJ;
            oriKc = oriK;
        }
        else if (isPropSet(magV) && (isCatalogLBSI(oriK) || isCatalogPhoto(oriK)))
        {
            // Assume V and K in Johnson, compute Kc from Vj and (V-K)
            // see Bessel 1988, p 1135
            // Note that this formula should be exactly
            // inverted in alxComputeDiameter to get back (V-K)j
            mcsDOUBLE mV;
            FAIL(GetPropertyValue(magV, &mV));

            mKc = mV - (0.03 + 0.992 * (mV - mK));
            eKc = 0.992 * eK + 0.008 * eV;
            oriKc = oriK;
        }
        else
        {
            logInfo("Kc not computed: unsupported origins J (%s) K (%s)", vobsGetOriginIndex(oriJ), vobsGetOriginIndex(oriK));
        }


        // check if the Kc magnitude is defined:
        if (!isnan(mKc))
        {
            // Compute the COUSIN/CIT Hc from Kc and (H-K)
            if (isPropSet(magH))
            {
                mcsDOUBLE mH;
                FAIL(GetPropertyValue(magH, &mH));

                if ((isCatalog2Mass(oriH) || isCatalogMerand(oriH))
                    && (isCatalog2Mass(oriK) || isCatalogMerand(oriK)))
                {
                    // From (H-K) 2MASS or Merand (actually 2MASS)
                    // see Carpenter eq.15
                    mHc = mKc + ((mH - mK) - 0.028) / 1.026;

                    /*
                     * As mKc = mK + 0.024, previous equation can be rewritten as:
                     * mHc = (1000 / 1026) * mH + (26 / 1026) mK + 0.024 - 28 / 1026
                     */
                    eHc = (1000.0 / 1026.0) * eH + (26.0 / 1026.0) * eK;
                    oriHc = oriH;
                }
                else if ((isCatalogLBSI(oriH) || isCatalogPhoto(oriH))
                         && (isCatalogLBSI(oriK) || isCatalogPhoto(oriK)))
                {
                    // From (H-K) LBSI / PHOTO, we assume H and K in Johnson magnitude
                    // see Bessel, p.1138
                    mHc = mKc - 0.009 + 0.912 * (mH - mK);
                    eHc = eH; // TODO: 2s order correction 
                    oriHc = oriH;
                }
                else
                {
                    logInfo("Hc not computed: unsupported origins H (%s) K (%s)", vobsGetOriginIndex(oriH), vobsGetOriginIndex(oriK));
                }
            }


            // Compute the COUSIN/CIT Jc from Kc and (J-K)
            if (isPropSet(magJ))
            {
                mcsDOUBLE mJ;
                FAIL(GetPropertyValue(magJ, &mJ));

                if ((isCatalog2Mass(oriJ) || isCatalogMerand(oriJ))
                    && (isCatalog2Mass(oriK) || isCatalogMerand(oriK)))
                {
                    // From (J-K) 2MASS or Merand (actually 2MASS)
                    // see Carpenter eq 14
                    mJc = mKc + ((mJ - mK) + 0.013) / 1.056;

                    /*
                     * As mKc = mK + 0.024, previous equation can be rewritten as:
                     * mJc = (1000 / 1056) * mJ + (56 / 1056) mK + 0.024 + 13 / 1056
                     */
                    eJc = (1000.0 / 1056.0) * eJ + (56.0 / 1056.0) * eK;
                    oriJc = oriJ;
                }
                else if (isCatalogDenisJK(oriJ) && isCatalogDenisJK(oriK))
                {
                    // From (J-K) DENIS
                    // see Carpenter eq 14 and 17
                    mJc = mKc + ((0.981 * (mJ - mK) + 0.023) + 0.013) / 1.056;
                    eJc = eJ; // TODO: 2s order correction
                    oriJc = oriJ;
                }
                else if ((isCatalogLBSI(oriJ) || isCatalogPhoto(oriJ))
                         && (isCatalogLBSI(oriK) || isCatalogPhoto(oriK)))
                {
                    // From (J-K) LBSI / PHOTO, we assume H and K in Johnson magnitude
                    // see Bessel p.1136  This seems quite unprecise.
                    mJc = mKc + 0.93 * (mJ - mK);
                    eJc = eJ; // TODO: 2s order correction
                    oriJc = oriJ;
                }
                else
                {
                    logInfo("Jc not computed: unsupported origins J (%s) K (%s)", vobsGetOriginIndex(oriJ), vobsGetOriginIndex(oriK));
                }
            }

            // Set the magnitudes and errors:
            FAIL(SetPropertyValueAndError(vobsSTAR_PHOT_COUS_K, mKc, eKc, oriKc));

            if (!isnan(mHc))
            {
                FAIL(SetPropertyValueAndError(vobsSTAR_PHOT_COUS_H, mHc, eHc, oriHc));
            }
            if (!isnan(mJc))
            {
                FAIL(SetPropertyValueAndError(vobsSTAR_PHOT_COUS_J, mJc, eJc, oriJc));
            }
        } // Kc defined

    } //  K defined

    // Read the COUSIN Ic band
    mcsDOUBLE mIc = NAN;
    mcsDOUBLE eIc = NAN;
    vobsSTAR_PROPERTY* magIc = GetProperty(vobsSTAR_PHOT_COUS_I);
    if (isPropSet(magIc))
    {
        FAIL(GetPropertyValue(magIc, &mIc));
        FAIL(GetPropertyErrorOrDefault(magIc, &eIc, MIN_MAG_ERROR));
    }

    logTest("Cousin magnitudes: I=%0.3lf(%0.3lf) J=%0.3lf(%0.3lf) H=%0.3lf(%0.3lf) K=%0.3lf(%0.3lf)",
            mIc, eIc, mJc, eJc, mHc, eHc, mKc, eKc);

    return mcsSUCCESS;
}

/**
 * Fill the I, J, H and K JOHNSON magnitude (actually the 2MASS system)
 * from the COUSIN/CIT magnitudes.
 *
 * @return  mcsSUCCESS on successful completion. Otherwise mcsFAILURE is
 * returned.
 */
mcsCOMPL_STAT sclsvrCALIBRATOR::ComputeJohnsonMagnitudes()
{
    // Define the Cousin as NaN
    mcsDOUBLE mIcous = NAN;
    mcsDOUBLE mJcous = NAN;
    mcsDOUBLE mHcous = NAN;
    mcsDOUBLE mKcous = NAN;
    mcsDOUBLE mI = NAN;
    mcsDOUBLE mJ = NAN;
    mcsDOUBLE mH = NAN;
    mcsDOUBLE mK = NAN;

    vobsSTAR_PROPERTY* magI = GetProperty(vobsSTAR_PHOT_COUS_I);
    vobsSTAR_PROPERTY* magJ = GetProperty(vobsSTAR_PHOT_COUS_J);
    vobsSTAR_PROPERTY* magH = GetProperty(vobsSTAR_PHOT_COUS_H);
    vobsSTAR_PROPERTY* magK = GetProperty(vobsSTAR_PHOT_COUS_K);

    // Convert K band from COUSIN CIT to 2MASS
    if (isPropSet(magK))
    {
        FAIL(GetPropertyValue(magK, &mKcous));

        // See Carpenter, 2001: 2001AJ....121.2851C, eq.12
        mK = mKcous - 0.024;

        FAIL(SetPropertyValue(vobsSTAR_PHOT_JHN_K, mK, vobsORIG_COMPUTED, magK->GetConfidenceIndex()));
    }

    // Fill J band from COUSIN to 2MASS
    if (isPropSet(magJ) && isPropSet(magK))
    {
        FAIL(GetPropertyValue(magJ, &mJcous));
        FAIL(GetPropertyValue(magK, &mKcous));

        // See Carpenter, 2001: 2001AJ....121.2851C, eq.12 and eq.14
        mJ = 1.056 * mJcous - 0.056 * mKcous - 0.037;

        FAIL(SetPropertyValue(vobsSTAR_PHOT_JHN_J, mJ, vobsORIG_COMPUTED,
                              min(magJ->GetConfidenceIndex(), magK->GetConfidenceIndex())));
    }

    // Fill H band from COUSIN to 2MASS
    if (isPropSet(magH) && isPropSet(magK))
    {
        FAIL(GetPropertyValue(magH, &mHcous));
        FAIL(GetPropertyValue(magK, &mKcous));

        // See Carpenter, 2001: 2001AJ....121.2851C, eq.12 and eq.15
        mH = 1.026 * mHcous - 0.026 * mKcous + 0.004;

        FAIL(SetPropertyValue(vobsSTAR_PHOT_JHN_H, mH, vobsORIG_COMPUTED,
                              min(magH->GetConfidenceIndex(), magK->GetConfidenceIndex())));
    }

    // Fill I band from COUSIN to JOHNSON
    if (isPropSet(magI) && isPropSet(magJ))
    {
        FAIL(GetPropertyValue(magI, &mIcous));
        FAIL(GetPropertyValue(magJ, &mJcous));

        // Approximate conversion, JB. Le Bouquin
        mI = mIcous + 0.43 * (mJcous - mIcous) + 0.048;

        FAIL(SetPropertyValue(vobsSTAR_PHOT_JHN_I, mI, vobsORIG_COMPUTED,
                              min(magI->GetConfidenceIndex(), magJ->GetConfidenceIndex())));
    }

    logTest("Johnson magnitudes: I=%0.3lf J=%0.3lf H=%0.3lf K=%0.3lf",
            mI, mJ, mH, mK);

    return mcsSUCCESS;
}

/**
 * Add all star properties 
 *
 * @return Always mcsSUCCESS.
 */
mcsCOMPL_STAT sclsvrCALIBRATOR::AddProperties(void)
{
    if (sclsvrCALIBRATOR::sclsvrCALIBRATOR_PropertyIdxInitialized == false)
    {
        sclsvrCALIBRATOR::sclsvrCALIBRATOR_PropertyMetaBegin = vobsSTAR::vobsStar_PropertyMetaList.size();

        // Add Meta data:

        /* computed galactic positions */
        AddPropertyMeta(sclsvrCALIBRATOR_POS_GAL_LAT, "GLAT", vobsFLOAT_PROPERTY, "deg", "Galactic Latitude");
        AddPropertyMeta(sclsvrCALIBRATOR_POS_GAL_LON, "GLON", vobsFLOAT_PROPERTY, "deg", "Galactic Longitude");

        /* computed diameters */
        AddPropertyMeta(sclsvrCALIBRATOR_DIAM_BV, "diam_bv", vobsFLOAT_PROPERTY, "mas",   "B-V Diameter");
        AddPropertyErrorMeta(sclsvrCALIBRATOR_DIAM_BV_ERROR, "e_diam_bv", "mas", "Error on B-V Diameter");
        AddPropertyMeta(sclsvrCALIBRATOR_DIAM_BI, "diam_bi", vobsFLOAT_PROPERTY, "mas",   "B-I Diameter");
        AddPropertyErrorMeta(sclsvrCALIBRATOR_DIAM_BI_ERROR, "e_diam_bi", "mas", "Error on B-I Diameter");
        AddPropertyMeta(sclsvrCALIBRATOR_DIAM_BJ, "diam_bj", vobsFLOAT_PROPERTY, "mas",   "B-J Diameter");
        AddPropertyErrorMeta(sclsvrCALIBRATOR_DIAM_BJ_ERROR, "e_diam_bj", "mas", "Error on B-J Diameter");
        AddPropertyMeta(sclsvrCALIBRATOR_DIAM_BH, "diam_bh", vobsFLOAT_PROPERTY, "mas",   "B-H Diameter");
        AddPropertyErrorMeta(sclsvrCALIBRATOR_DIAM_BH_ERROR, "e_diam_bh", "mas", "Error on B-H Diameter");
        AddPropertyMeta(sclsvrCALIBRATOR_DIAM_BK, "diam_bk", vobsFLOAT_PROPERTY, "mas",   "B-K Diameter");
        AddPropertyErrorMeta(sclsvrCALIBRATOR_DIAM_BK_ERROR, "e_diam_bk", "mas", "Error on B-K Diameter");

        AddPropertyMeta(sclsvrCALIBRATOR_DIAM_VR, "diam_vr", vobsFLOAT_PROPERTY, "mas",   "V-R Diameter");
        AddPropertyErrorMeta(sclsvrCALIBRATOR_DIAM_VR_ERROR, "e_diam_vr", "mas", "Error on V-R Diameter");
        AddPropertyMeta(sclsvrCALIBRATOR_DIAM_VI, "diam_vi", vobsFLOAT_PROPERTY, "mas",   "V-I Diameter");
        AddPropertyErrorMeta(sclsvrCALIBRATOR_DIAM_VI_ERROR, "e_diam_vi", "mas", "Error on V-I Diameter");
        AddPropertyMeta(sclsvrCALIBRATOR_DIAM_VJ, "diam_vj", vobsFLOAT_PROPERTY, "mas",   "V-J Diameter");
        AddPropertyErrorMeta(sclsvrCALIBRATOR_DIAM_VJ_ERROR, "e_diam_vj", "mas", "Error on V-J Diameter");
        AddPropertyMeta(sclsvrCALIBRATOR_DIAM_VH, "diam_vh", vobsFLOAT_PROPERTY, "mas",   "V-H Diameter");
        AddPropertyErrorMeta(sclsvrCALIBRATOR_DIAM_VH_ERROR, "e_diam_vh", "mas", "Error on V-H Diameter");
        AddPropertyMeta(sclsvrCALIBRATOR_DIAM_VK, "diam_vk", vobsFLOAT_PROPERTY, "mas",   "V-K Diameter");
        AddPropertyErrorMeta(sclsvrCALIBRATOR_DIAM_VK_ERROR, "e_diam_vk", "mas", "Error on V-K Diameter");

        AddPropertyMeta(sclsvrCALIBRATOR_DIAM_IJ, "diam_ij", vobsFLOAT_PROPERTY, "mas",   "I-J Diameter");
        AddPropertyErrorMeta(sclsvrCALIBRATOR_DIAM_IJ_ERROR, "e_diam_ij", "mas", "Error on I-J Diameter");
        AddPropertyMeta(sclsvrCALIBRATOR_DIAM_IH, "diam_ih", vobsFLOAT_PROPERTY, "mas",   "I-H Diameter");
        AddPropertyErrorMeta(sclsvrCALIBRATOR_DIAM_IH_ERROR, "e_diam_ih", "mas", "Error on I-H Diameter");
        AddPropertyMeta(sclsvrCALIBRATOR_DIAM_IK, "diam_ik", vobsFLOAT_PROPERTY, "mas",   "I-K Diameter");
        AddPropertyErrorMeta(sclsvrCALIBRATOR_DIAM_IK_ERROR, "e_diam_ik", "mas", "Error on I-K Diameter");

        AddPropertyMeta(sclsvrCALIBRATOR_DIAM_JH, "diam_jh", vobsFLOAT_PROPERTY, "mas",   "J-H Diameter");
        AddPropertyErrorMeta(sclsvrCALIBRATOR_DIAM_JH_ERROR, "e_diam_jh", "mas", "Error on J-H Diameter");
        AddPropertyMeta(sclsvrCALIBRATOR_DIAM_JK, "diam_jk", vobsFLOAT_PROPERTY, "mas",   "J-K Diameter");
        AddPropertyErrorMeta(sclsvrCALIBRATOR_DIAM_JK_ERROR, "e_diam_jk", "mas", "Error on J-K Diameter");

        AddPropertyMeta(sclsvrCALIBRATOR_DIAM_HK, "diam_hk", vobsFLOAT_PROPERTY, "mas",   "H-K Diameter");
        AddPropertyErrorMeta(sclsvrCALIBRATOR_DIAM_HK_ERROR, "e_diam_hk", "mas", "Error on H-K Diameter");

        /* diameter count used by mean / weighted mean / stddev (consistent ones) */
        AddPropertyMeta(sclsvrCALIBRATOR_DIAM_COUNT, "diam_count", vobsINT_PROPERTY, NULL, "Number of consistent and valid (high confidence) computed diameters (used by mean / weighted mean / stddev computations)");

        /* mean diameter */
        AddPropertyMeta(sclsvrCALIBRATOR_DIAM_MEAN, "diam_mean", vobsFLOAT_PROPERTY, "mas", "Mean Diameter from the IR Magnitude versus Color Indices Calibrations");
        AddPropertyErrorMeta(sclsvrCALIBRATOR_DIAM_MEAN_ERROR, "e_diam_mean", "mas", "Estimated Error on Mean Diameter");

        /* median diameter */
        AddPropertyMeta(sclsvrCALIBRATOR_DIAM_MEDIAN, "diam_median", vobsFLOAT_PROPERTY, "mas", "Median Diameter from the IR Magnitude versus Color Indices Calibrations");
        AddPropertyErrorMeta(sclsvrCALIBRATOR_DIAM_MEDIAN_ERROR, "e_diam_median", "mas", "Estimated Error on Median Diameter");

        /* standard deviation on all diameters */
        AddPropertyMeta(sclsvrCALIBRATOR_DIAM_STDDEV, "diam_stddev", vobsFLOAT_PROPERTY, "mas", "Standard deviation of all diameters");

        /* weighted mean diameter */
        AddPropertyMeta(sclsvrCALIBRATOR_DIAM_WEIGHTED_MEAN, "diam_weighted_mean", vobsFLOAT_PROPERTY, "mas", "Weighted mean diameter by inverse(diameter error)");
        AddPropertyErrorMeta(sclsvrCALIBRATOR_DIAM_WEIGHTED_MEAN_ERROR, "e_diam_weighted_mean", "mas", "Estimated Error on Weighted mean diameter");

        /* diameter quality (true | false) */
        AddPropertyMeta(sclsvrCALIBRATOR_DIAM_FLAG, "diamFlag", vobsBOOL_PROPERTY, NULL, "Diameter Flag (true means valid diameter)");
        /* diameter quality (1 to 10 sigma) */
        AddFormattedPropertyMeta(sclsvrCALIBRATOR_DIAM_QUALITY, "diamQuality", vobsFLOAT_PROPERTY, NULL, "%.1lf", "Diameter Quality (1 to 10 sigma)");
        /* information about the diameter computation */
        AddPropertyMeta(sclsvrCALIBRATOR_DIAM_FLAG_INFO, "diamFlagInfo", vobsSTRING_PROPERTY, NULL, "Information related to the diamFlag value");

        /* Results from SED fitting */
        AddPropertyMeta(sclsvrCALIBRATOR_SEDFIT_CHI2, "chi2_SED", vobsFLOAT_PROPERTY, NULL, "Reduced chi2 of the SED fitting (experimental)");
        AddPropertyMeta(sclsvrCALIBRATOR_SEDFIT_DIAM, "diam_SED", vobsFLOAT_PROPERTY, "mas", "Diameter from SED fitting (experimental)");
        AddPropertyErrorMeta(sclsvrCALIBRATOR_SEDFIT_DIAM_ERROR, "e_diam_SED", "mas", "Diameter from SED fitting (experimental)");
        AddPropertyMeta(sclsvrCALIBRATOR_SEDFIT_TEFF, "Teff_SED", vobsFLOAT_PROPERTY, "K", "Teff from SED fitting (experimental)");
        AddPropertyMeta(sclsvrCALIBRATOR_SEDFIT_AV, "Av_SED", vobsFLOAT_PROPERTY, NULL, "Teff from SED fitting (experimental)");

        /* Teff / Logg determined from spectral type */
        AddPropertyMeta(sclsvrCALIBRATOR_TEFF_SPTYP, "Teff_SpType", vobsFLOAT_PROPERTY, NULL, "Effective Temperature adopted from Spectral Type");
        AddPropertyMeta(sclsvrCALIBRATOR_LOGG_SPTYP, "logg_SpType", vobsFLOAT_PROPERTY, NULL, "Gravity adopted from Spectral Type");

        /* uniform disk diameters */
        AddPropertyMeta(sclsvrCALIBRATOR_UD_U, "UD_U", vobsFLOAT_PROPERTY, "mas", "U-band Uniform-Disk Diameter");
        AddPropertyMeta(sclsvrCALIBRATOR_UD_B, "UD_B", vobsFLOAT_PROPERTY, "mas", "B-band Uniform-Disk Diameter");
        AddPropertyMeta(sclsvrCALIBRATOR_UD_V, "UD_V", vobsFLOAT_PROPERTY, "mas", "V-band Uniform-Disk Diameter");
        AddPropertyMeta(sclsvrCALIBRATOR_UD_R, "UD_R", vobsFLOAT_PROPERTY, "mas", "R-band Uniform-Disk Diameter");
        AddPropertyMeta(sclsvrCALIBRATOR_UD_I, "UD_I", vobsFLOAT_PROPERTY, "mas", "I-band Uniform-Disk Diameter");
        AddPropertyMeta(sclsvrCALIBRATOR_UD_J, "UD_J", vobsFLOAT_PROPERTY, "mas", "J-band Uniform-Disk Diameter");
        AddPropertyMeta(sclsvrCALIBRATOR_UD_H, "UD_H", vobsFLOAT_PROPERTY, "mas", "H-band Uniform-Disk Diameter");
        AddPropertyMeta(sclsvrCALIBRATOR_UD_K, "UD_K", vobsFLOAT_PROPERTY, "mas", "K-band Uniform-Disk Diameter");
        AddPropertyMeta(sclsvrCALIBRATOR_UD_L, "UD_L", vobsFLOAT_PROPERTY, "mas", "L-band Uniform-Disk Diameter");
        AddPropertyMeta(sclsvrCALIBRATOR_UD_N, "UD_N", vobsFLOAT_PROPERTY, "mas", "N-band Uniform-Disk Diameter");

        /* extinction ratio related to interstellar absorption (faint) */
        AddPropertyMeta(sclsvrCALIBRATOR_EXTINCTION_RATIO, "Av", vobsFLOAT_PROPERTY, NULL, "Visual Interstellar Absorption");
        AddPropertyErrorMeta(sclsvrCALIBRATOR_EXTINCTION_RATIO_ERROR, "e_Av", NULL, "Error on Visual Interstellar Absorption");

        /* square visibility */
        AddPropertyMeta(sclsvrCALIBRATOR_VIS2, "vis2", vobsFLOAT_PROPERTY, NULL, "Squared Visibility");
        AddPropertyErrorMeta(sclsvrCALIBRATOR_VIS2_ERROR, "vis2Err", NULL, "Error on Squared Visibility");

        /* distance to the science object */
        AddPropertyMeta(sclsvrCALIBRATOR_DIST, "dist", vobsFLOAT_PROPERTY, "deg", "Calibrator to Science object Angular Distance");

        /* corrected spectral type */
        AddPropertyMeta(sclsvrCALIBRATOR_SP_TYPE, "SpType_JMMC", vobsSTRING_PROPERTY, NULL, "Corrected spectral type by the JMMC");

        // End of Meta data

        sclsvrCALIBRATOR::sclsvrCALIBRATOR_PropertyMetaEnd = vobsSTAR::vobsStar_PropertyMetaList.size();

        logInfo("sclsvrCALIBRATOR has defined %d properties.", sclsvrCALIBRATOR::sclsvrCALIBRATOR_PropertyMetaEnd);

        if (sclsvrCALIBRATOR::sclsvrCALIBRATOR_PropertyMetaEnd != sclsvrCALIBRATOR_MAX_PROPERTIES)
        {
            logWarning("sclsvrCALIBRATOR_MAX_PROPERTIES constant is incorrect: %d != %d",
                       sclsvrCALIBRATOR_MAX_PROPERTIES, sclsvrCALIBRATOR::sclsvrCALIBRATOR_PropertyMetaEnd);
        }

        initializeIndex();

        // Dump all properties (vobsSTAR and sclsvrCALIBRATOR) into XML file:
        DumpPropertyIndexAsXML();

        sclsvrCALIBRATOR::sclsvrCALIBRATOR_PropertyIdxInitialized = true;
    }

    // Add properties only if missing:
    if (NbProperties() <= sclsvrCALIBRATOR::sclsvrCALIBRATOR_PropertyMetaBegin)
    {
        const vobsSTAR_PROPERTY_META* meta;
        for (int i = sclsvrCALIBRATOR::sclsvrCALIBRATOR_PropertyMetaBegin; i < sclsvrCALIBRATOR::sclsvrCALIBRATOR_PropertyMetaEnd; i++)
        {
            meta = vobsSTAR::GetPropertyMeta(i);

            if (isNotNull(meta))
            {
                AddProperty(meta);
            }
        }
    }

    return mcsSUCCESS;
}

/**
 * Dump the property index
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned 
 */
mcsCOMPL_STAT sclsvrCALIBRATOR::DumpPropertyIndexAsXML()
{
    miscoDYN_BUF xmlBuf;
    // Prepare buffer:
    FAIL(xmlBuf.Reserve(40 * 1024));

    xmlBuf.AppendLine("<?xml version=\"1.0\"?>\n\n");

    FAIL(xmlBuf.AppendString("<index>\n"));
    FAIL(xmlBuf.AppendString("  <name>sclsvrCALIBRATOR</name>\n"));

    vobsSTAR::DumpPropertyIndexAsXML(xmlBuf, "vobsSTAR", 0, sclsvrCALIBRATOR::sclsvrCALIBRATOR_PropertyMetaBegin);
    vobsSTAR::DumpPropertyIndexAsXML(xmlBuf, "sclsvrCALIBRATOR", sclsvrCALIBRATOR::sclsvrCALIBRATOR_PropertyMetaBegin, sclsvrCALIBRATOR::sclsvrCALIBRATOR_PropertyMetaEnd);

    FAIL(xmlBuf.AppendString("</index>\n\n"));

    mcsCOMPL_STAT result = mcsSUCCESS;

    // This file will be stored in the $MCSDATA/tmp repository
    const char* fileName = "$MCSDATA/tmp/PropertyIndex_sclsvrCALIBRATOR.xml";

    // Resolve path
    char* resolvedPath = miscResolvePath(fileName);
    if (isNotNull(resolvedPath))
    {
        logInfo("Saving property index XML description: %s", resolvedPath);

        result = xmlBuf.SaveInASCIIFile(resolvedPath);
        free(resolvedPath);
    }
    return result;
}

/**
 * Clean up the property index
 */
void sclsvrCALIBRATOR::FreePropertyIndex()
{
    sclsvrCALIBRATOR::sclsvrCALIBRATOR_PropertyMetaBegin = -1;
    sclsvrCALIBRATOR::sclsvrCALIBRATOR_PropertyMetaEnd = -1;
    sclsvrCALIBRATOR::sclsvrCALIBRATOR_PropertyIdxInitialized = false;
}

/*___oOo___*/

