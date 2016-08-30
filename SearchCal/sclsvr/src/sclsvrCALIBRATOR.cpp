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

/** flag to enable / disable SED Fitting in development mode */
#define sclsvrCALIBRATOR_PERFORM_SED_FITTING mcsFALSE

/* maximum number of properties (108) */
#define sclsvrCALIBRATOR_MAX_PROPERTIES 108

/* Error identifiers */
#define sclsvrCALIBRATOR_DIAM_VB_ERROR      "DIAM_VB_ERROR"
#define sclsvrCALIBRATOR_DIAM_VJ_ERROR      "DIAM_VJ_ERROR"
#define sclsvrCALIBRATOR_DIAM_VH_ERROR      "DIAM_VH_ERROR"
#define sclsvrCALIBRATOR_DIAM_VK_ERROR      "DIAM_VK_ERROR"

#define sclsvrCALIBRATOR_DIAM_WEIGHTED_MEAN_ERROR "DIAM_WEIGHTED_MEAN_ERROR"
#define sclsvrCALIBRATOR_LD_DIAM_ERROR      "LD_DIAM_ERROR"

#define sclsvrCALIBRATOR_SEDFIT_DIAM_ERROR  "SEDFIT_DIAM_ERROR"

#define sclsvrCALIBRATOR_EXTINCTION_RATIO_ERROR "EXTINCTION_RATIO_ERROR"

#define sclsvrCALIBRATOR_AV_FIT_ERROR       "AV_FIT_ERROR"
#define sclsvrCALIBRATOR_DIST_FIT_ERROR     "DIST_FIT_ERROR"

#define sclsvrCALIBRATOR_AV_STAT_ERROR      "AV_STAT_ERROR"
#define sclsvrCALIBRATOR_DIST_STAT_ERROR    "DIST_STAT_ERROR"

#define sclsvrCALIBRATOR_VIS2_ERROR         "VIS2_ERROR"

// TODO: FIX sclsvrCALIBRATOR_EMAG_MIN
#define sclsvrCALIBRATOR_EMAG_MIN           0.04
#define sclsvrCALIBRATOR_EMAG_MAX_2MASS     0.50

/**
 * Convenience macros
 */
#define SetComputedPropWithError(propId, alxDATA) \
if alxIsSet(alxDATA)                              \
{                                                 \
    FAIL(SetPropertyValueAndError(propId, alxDATA.value, alxDATA.error, vobsORIG_COMPUTED, (vobsCONFIDENCE_INDEX) alxDATA.confIndex)); \
}


/** Initialize static members */
mcsINT32 sclsvrCALIBRATOR::sclsvrCALIBRATOR_PropertyMetaBegin = -1;
mcsINT32 sclsvrCALIBRATOR::sclsvrCALIBRATOR_PropertyMetaEnd = -1;
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
sclsvrCALIBRATOR::sclsvrCALIBRATOR(const sclsvrCALIBRATOR& star) : vobsSTAR()
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
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
 */
mcsCOMPL_STAT sclsvrCALIBRATOR::Complete(const sclsvrREQUEST &request, miscoDYN_BUF &msgInfo)
{
    // Reset the buffer that contains the diamFlagInfo string
    FAIL(msgInfo.Reset());

    const bool notJSDC = IS_FALSE(request.IsJSDCMode());

    // Set Star ID
    mcsSTRING64 starId;
    FAIL(GetId(starId, sizeof (starId)));
    FAIL(SetPropertyValue(sclsvrCALIBRATOR_NAME, starId, vobsORIG_COMPUTED, vobsCONFIDENCE_HIGH));

    logTest("----- Complete: star '%s'", starId);

    // Check parallax: set the property vobsSTAR_POS_PARLX_TRIG_FLAG (true | false)
    FAIL(CheckParallax());

    // Parse spectral type.
    FAIL(ParseSpectralType());
    FAIL(DefineSpectralTypeIndexes());

    // Compute Galactic coordinates:
    FAIL(ComputeGalacticCoordinates());

    if (notJSDC)
    {
        // Compute absorption coefficient Av and may correct luminosity class (ie SpType)
        FAIL(ComputeExtinctionCoefficient());
    }

    // Fill in the Teff and LogG entries using the spectral type
    FAIL(ComputeTeffLogg());

    // Compute N Band and S_12 with AKARI from Teff
    FAIL(ComputeIRFluxes());

    if (notJSDC)
    {
        FAIL(ComputeSedFitting());

        // Compute I, J, H, K COUSIN magnitude from Johnson catalogues
        FAIL(ComputeCousinMagnitudes());

        // Compute missing Magnitude (information only)
        if (isPropSet(sclsvrCALIBRATOR_EXTINCTION_RATIO))
        {
            FAIL(ComputeMissingMagnitude(request.IsBright()));
        }
        else
        {
            logTest("Av is unknown; do not compute missing magnitude");
        }

        // Compute J, H, K JOHNSON magnitude (2MASS) from COUSIN
        FAIL(ComputeJohnsonMagnitudes());
    }

    // TODO: implement FAINT approach
    // = compute diameters without SpType (chi2 minimization)

    if (IS_TRUE(_spectralType.isInvalid))
    {
        logTest("Unsupported spectral type; can not compute diameter", starId);
    }
    else
    {
        FAIL(ComputeAngularDiameter(msgInfo));
    }

    // Compute UD from LD and SP
    FAIL(ComputeUDFromLDAndSP());

    // Discard the diameter if the Spectral Type is not supported (bad code)
    if (IS_TRUE(_spectralType.isInvalid))
    {
        logTest("Unsupported spectral type; diameter flag set to NOK", starId);

        // Overwrite diamFlag and diamFlagInfo properties:
        FAIL(SetPropertyValue(sclsvrCALIBRATOR_DIAM_FLAG, mcsFALSE, vobsORIG_COMPUTED, vobsCONFIDENCE_HIGH, mcsTRUE));

        msgInfo.AppendString(" INVALID_SPTYPE");
        FAIL(SetPropertyValue(sclsvrCALIBRATOR_DIAM_FLAG_INFO, msgInfo.GetBuffer(), vobsORIG_COMPUTED, vobsCONFIDENCE_HIGH, mcsTRUE));
    }

    if (notJSDC)
    {
        // TODO: move these last two steps into SearchCal GUI (Vis2 + distance)

        // Compute visibility and visibility error only if diameter OK or (UDDK, diam12)
        FAIL(ComputeVisibility(request));

        // Compute distance
        FAIL(ComputeDistance(request));
    }

    return mcsSUCCESS;
}


/*
 * Private methods
 */

/**
 * Fill the given magnitudes B to last band (M by default) using given property identifiers
 * @param magnitudes alxMAGNITUDES structure to fill
 * @param magIds property identifiers
 * @param defError default error if none
 * @param originIdxs optional required origin indexes
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.;
 */
mcsCOMPL_STAT sclsvrCALIBRATOR::ExtractMagnitudes(alxMAGNITUDES &magnitudes,
                                                  const char** magIds,
                                                  mcsDOUBLE defError,
                                                  const vobsORIGIN_INDEX* originIdxs)
{
    const bool hasOrigins = IS_NOT_NULL(originIdxs);

    vobsSTAR_PROPERTY* property;

    // For each magnitude
    for (mcsUINT32 band = alxB_BAND; band < alxNB_BANDS; band++)
    {
        property = GetProperty(magIds[band]);

        // Get the magnitude value
        if (isPropSet(property)
                && (!hasOrigins || (originIdxs[band] == vobsORIG_NONE) || (property->GetOriginIndex() == originIdxs[band])))
        {
            logDebug("ExtractMagnitudes[%s]: origin = %s", alxGetBandLabel((alxBAND) band), vobsGetOriginIndex(property->GetOriginIndex()))

            FAIL(GetPropertyValue(property, &magnitudes[band].value));
            magnitudes[band].isSet = mcsTRUE;
            magnitudes[band].confIndex = (alxCONFIDENCE_INDEX) property->GetConfidenceIndex();

            /* Extract error or default error if none */
            FAIL(GetPropertyErrorOrDefault(property, &magnitudes[band].error, defError));
        }
        else
        {
            alxDATAClear(magnitudes[band]);
        }
    }
    return mcsSUCCESS;
}

/**
 * Fill the given magnitudes B to last band (M by default) and
 * fix error values (min error for all magnitudes and max error for 2MASS magnitudes)
 * @param magnitudes alxMAGNITUDES structure to fill
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.;
 */
mcsCOMPL_STAT sclsvrCALIBRATOR::ExtractMagnitudesAndFixErrors(alxMAGNITUDES &magnitudes)
{
    // Magnitudes to be used from catalogs ONLY
    static const char* magIds[alxNB_BANDS] = {
                                              vobsSTAR_PHOT_JHN_B,
                                              vobsSTAR_PHOT_JHN_V,
                                              vobsSTAR_PHOT_JHN_R,
                                              vobsSTAR_PHOT_COUS_I,
                                              /* (JHK 2MASS) */
                                              vobsSTAR_PHOT_JHN_J,
                                              vobsSTAR_PHOT_JHN_H,
                                              vobsSTAR_PHOT_JHN_K,
                                              vobsSTAR_PHOT_JHN_L,
                                              vobsSTAR_PHOT_JHN_M,
                                              vobsSTAR_PHOT_JHN_N
    };
    static const vobsORIGIN_INDEX originIdxs[alxNB_BANDS] = {
                                                             vobsORIG_NONE,
                                                             vobsORIG_NONE,
                                                             vobsORIG_NONE,
                                                             vobsORIG_NONE,
                                                             /* JHK 2MASS */
                                                             vobsCATALOG_MASS_ID,
                                                             vobsCATALOG_MASS_ID,
                                                             vobsCATALOG_MASS_ID,
                                                             vobsORIG_NONE,
                                                             vobsORIG_NONE,
                                                             vobsORIG_NONE
    };

    // set error to NaN if undefined (see below):
    FAIL(ExtractMagnitudes(magnitudes, magIds, NAN, originIdxs));

    // We now have mag = {Bj, Vj, Rj, Ic, Jj, Hj, Kj, Lj, Mj, Nj}
    alxLogTestMagnitudes("Extracted magnitudes:", "", magnitudes);


    // TODO: FIX sclsvrCALIBRATOR_EMAG_MIN to 1e-3
    // Fix error lower limit to 0.04 mag:
    for (mcsUINT32 band = alxB_BAND; band < alxNB_BANDS; band++)
    {
        if (alxIsSet(magnitudes[band]) && (magnitudes[band].error < sclsvrCALIBRATOR_EMAG_MIN))
        {
            logDebug("Fix  low magnitude error[%s]: error = %.3lf => %.3lf", alxGetBandLabel((alxBAND) band),
                     magnitudes[band].error, sclsvrCALIBRATOR_EMAG_MIN);

            magnitudes[band].error = sclsvrCALIBRATOR_EMAG_MIN;
        }
    }

    // Fix too big error for 2MASS (K < 6):
    if (alxIsSet(magnitudes[alxK_BAND]) && (magnitudes[alxK_BAND].value) <= 6.0)
    {
        for (mcsUINT32 band = alxJ_BAND; band <= alxK_BAND; band++)
        {
            // Get the magnitude value
            if (alxIsSet(magnitudes[band]) && (magnitudes[band].error > sclsvrCALIBRATOR_EMAG_MAX_2MASS))
            {
                logDebug("Fix high magnitude error[%s]: error = %.3lf => %.3lf", alxGetBandLabel((alxBAND) band),
                         magnitudes[band].error, sclsvrCALIBRATOR_EMAG_MAX_2MASS);

                magnitudes[band].error = sclsvrCALIBRATOR_EMAG_MAX_2MASS;
            }
        }
    }

    alxLogTestMagnitudes("Fixed magnitudes:", "(fix error)", magnitudes);

    return mcsSUCCESS;
}

/**
 * Compute missing magnitude.
 *
 * @param isBright true is it is for bright object
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
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
    FAIL(ExtractMagnitudes(magnitudes, magIds, DEF_MAG_ERROR));

    /* Print out results */
    alxLogTestMagnitudes("Initial magnitudes:", "", magnitudes);

    // Get the extinction ratio
    mcsDOUBLE Av;
    FAIL(GetPropertyValue(sclsvrCALIBRATOR_EXTINCTION_RATIO, &Av));

    // Compute corrected magnitude
    // (remove the expected interstellar absorption)
    FAIL(alxComputeCorrectedMagnitudes("(Av)", Av, magnitudes, mcsTRUE));

    // Compute missing magnitudes
    if (IS_TRUE(isBright))
    {
        FAIL(alxComputeMagnitudesForBrightStar(&_spectralType, magnitudes));
    }
    else
    {
        FAIL(alxComputeMagnitudesForFaintStar(&_spectralType, magnitudes));
    }

    // Compute apparent magnitude (apply back interstellar absorption)
    FAIL(alxComputeApparentMagnitudes(Av, magnitudes));

    // Set back the computed magnitude. Already existing magnitudes are not overwritten.
    for (mcsUINT32 band = alxB_BAND; band < alxNB_BANDS; band++)
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
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
 */
mcsCOMPL_STAT sclsvrCALIBRATOR::ComputeGalacticCoordinates()
{
    mcsDOUBLE gLat, gLon, ra, dec;

    // Get right ascension position in degree
    FAIL(GetRa(ra));

    // Get declination in degree
    FAIL(GetDec(dec));

    // Compute galactic coordinates from the retrieved ra and dec values
    FAIL(alxComputeGalacticCoordinates(ra, dec, &gLat, &gLon));

    // Set the galactic latitude
    FAIL(SetPropertyValue(sclsvrCALIBRATOR_POS_GAL_LAT, gLat, vobsORIG_COMPUTED));

    // Set the galactic longitude
    FAIL(SetPropertyValue(sclsvrCALIBRATOR_POS_GAL_LON, gLon, vobsORIG_COMPUTED));

    return mcsSUCCESS;
}

/**
 * Compute extinction coefficient.
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
 */
mcsCOMPL_STAT sclsvrCALIBRATOR::ComputeExtinctionCoefficient()
{
    mcsDOUBLE dist_plx = NAN, e_dist_plx = NAN;
    mcsDOUBLE Av_stat  = NAN, e_Av_stat  = NAN;

    // Estimate statistical Av
    if (IS_TRUE(IsParallaxOk()))
    {
        mcsDOUBLE plx, e_plx, gLat, gLon;
        vobsSTAR_PROPERTY* property;

        // Get the value of the parallax and parallax error
        property = GetProperty(vobsSTAR_POS_PARLX_TRIG);
        FAIL(GetPropertyValueAndError(property, &plx, &e_plx));

        // Get the value of the galactic latitude
        property = GetProperty(sclsvrCALIBRATOR_POS_GAL_LAT);
        FAIL_FALSE_DO(IsPropertySet(property), errAdd(sclsvrERR_MISSING_PROPERTY, sclsvrCALIBRATOR_POS_GAL_LAT, "interstellar absorption"));
        FAIL(GetPropertyValue(property, &gLat));

        // Get the value of the galactic longitude
        property = GetProperty(sclsvrCALIBRATOR_POS_GAL_LON);
        FAIL_FALSE_DO(IsPropertySet(property), errAdd(sclsvrERR_MISSING_PROPERTY, sclsvrCALIBRATOR_POS_GAL_LON, "interstellar absorption"));
        FAIL(GetPropertyValue(property, &gLon));

        // Compute Extinction ratio and distance from parallax using statistical approach
        FAIL(alxComputeExtinctionCoefficient(&Av_stat, &e_Av_stat, &dist_plx, &e_dist_plx, plx, e_plx, gLat, gLon));

        logTest("ComputeExtinctionCoefficient: (stat) Av=%.4lf (%.4lf) dist(plx)=%.4lf (%.4lf)",
                Av_stat, e_Av_stat, dist_plx, e_dist_plx);

        // Set statistical extinction ratio and error
        FAIL(SetPropertyValueAndError(sclsvrCALIBRATOR_AV_STAT, Av_stat, e_Av_stat, vobsORIG_COMPUTED));

        // Set distance computed from parallax and error
        FAIL(SetPropertyValueAndError(sclsvrCALIBRATOR_DIST_PLX, dist_plx, e_dist_plx, vobsORIG_COMPUTED));
    }

    // chi2 threshold to use low confidence:
    static mcsDOUBLE BAD_CHI2_THRESHOLD = 25.0; /* 5 sigma */

    // Fill the magnitude structure
    alxMAGNITUDES mags;
    FAIL(ExtractMagnitudesAndFixErrors(mags)); // set error to NAN if undefined

    // Get Star ID
    mcsSTRING64 starId;
    FAIL(GetId(starId, sizeof (starId)));

    mcsDOUBLE av_fit, e_av_fit, dist_fit, e_dist_fit, chi2_fit, chi2_dist;

    /* minimum value for delta on spectral quantity */
    /* 2016: =0 to disable adjusting spType at all */
    mcsDOUBLE minDeltaQuantity = 0.0;

    /* initialize fit confidence to high */
    vobsCONFIDENCE_INDEX avFitConfidence = vobsCONFIDENCE_HIGH;

    /* SpType has a precise luminosity class ? */
    mcsLOGICAL hasLumClass = (_spectralType.otherStarType != alxSTAR_UNDEFINED) ? mcsTRUE : mcsFALSE;
    bool guess             = (IS_FALSE(hasLumClass) || (_spectralType.starType != _spectralType.otherStarType));

    // compute Av from spectral type and given magnitudes
    if (alxComputeAvFromMagnitudes(starId, dist_plx, e_dist_plx, &av_fit, &e_av_fit, &dist_fit, &e_dist_fit,
                                   &chi2_fit, &chi2_dist, mags, &_spectralType, minDeltaQuantity, hasLumClass) == mcsSUCCESS)
    {
        logTest("ComputeExtinctionCoefficient: (fit) Av=%.4lf (%.4lf) distance=%.4lf (%.4lf) chi2=%.4lf chi2Dist=%.4lf",
                av_fit, e_av_fit, dist_fit, e_dist_fit, chi2_fit, chi2_dist);

        // check if chi2 is not too high (maybe wrong spectral type)
        if ((!isnan(chi2_fit) && (chi2_fit > BAD_CHI2_THRESHOLD))
                || (!isnan(chi2_dist) && (chi2_dist > BAD_CHI2_THRESHOLD)))
        {
            logInfo("ComputeExtinctionCoefficient: bad chi2 [1] Av=%.4lf (%.4lf) distance=%.4lf (%.4lf) chi2=%.4lf chi2Dist=%.4lf",
                    av_fit, e_av_fit, dist_fit, e_dist_fit, chi2_fit, chi2_dist);

            /* use low confidence for too high chi2: it will set diamFlag=false later */
            avFitConfidence = vobsCONFIDENCE_LOW;
        }

        if (IS_TRUE(_spectralType.isCorrected))
        {
            // Update our decoded spectral type using fit confidence:
            FAIL(SetPropertyValue(sclsvrCALIBRATOR_SP_TYPE, _spectralType.ourSpType, vobsORIG_COMPUTED, avFitConfidence, mcsTRUE));
        }

        // Set fitted extinction ratio and error
        FAIL(SetPropertyValueAndError(sclsvrCALIBRATOR_AV_FIT, av_fit, e_av_fit, vobsORIG_COMPUTED, avFitConfidence));

        if (!isnan(dist_fit))
        {
            // Set fitted distance and error
            FAIL(SetPropertyValueAndError(sclsvrCALIBRATOR_DIST_FIT, dist_fit, e_dist_fit, vobsORIG_COMPUTED, avFitConfidence));
        }

        if (!isnan(chi2_fit))
        {
            // Set chi2 of the fit
            FAIL(SetPropertyValue(sclsvrCALIBRATOR_AV_FIT_CHI2, chi2_fit, vobsORIG_COMPUTED, avFitConfidence));
        }

        if (!isnan(chi2_dist))
        {
            // Set chi2 of the distance modulus
            FAIL(SetPropertyValue(sclsvrCALIBRATOR_DIST_FIT_CHI2, chi2_dist, vobsORIG_COMPUTED, avFitConfidence));
        }
    }

    // Set extinction ratio and error (best)
    sclsvrAV_METHOD method;

    if (!isnan(av_fit))
    {
        method = (guess) ? sclsvrAV_METHOD_MIN_CHI2 : sclsvrAV_METHOD_FIT;
        // Set extinction ratio and error (high confidence)
        FAIL(SetPropertyValueAndError(sclsvrCALIBRATOR_EXTINCTION_RATIO, av_fit, e_av_fit, vobsORIG_COMPUTED, avFitConfidence));
    }
    else if (!isnan(Av_stat))
    {
        method = sclsvrAV_METHOD_STAT;
        // Set extinction ratio and error (medium confidence)
        FAIL(SetPropertyValueAndError(sclsvrCALIBRATOR_EXTINCTION_RATIO, Av_stat, e_Av_stat, vobsORIG_COMPUTED, vobsCONFIDENCE_MEDIUM));
    }
    else
    {
        // Unknown Av (guess)
        method = sclsvrAV_METHOD_UNKNOWN;
    }

    // Set extinction ratio and error (high confidence)
    FAIL(SetPropertyValue(sclsvrCALIBRATOR_AV_METHOD, method, vobsORIG_COMPUTED));

    return mcsSUCCESS;
}

/**
 * Compute apparent diameter by fitting the observed SED
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
 */
mcsCOMPL_STAT sclsvrCALIBRATOR::ComputeSedFitting()
{
    /* if DEV_FLAG: perform sed fitting */
    if (!vobsIsDevFlag() || IS_FALSE(sclsvrCALIBRATOR_PERFORM_SED_FITTING))
    {
        return mcsSUCCESS;
    }

    /* Extract the B V J H Ks magnitudes.
    The magnitude of the model SED are expressed in Bjohnson, Vjohnson, J2mass, H2mass, Ks2mass */
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
    for (mcsUINT32 band = 0; band < alxNB_SED_BAND; band++)
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

        /* Extract error or put 0.05mag by default */
        FAIL(GetPropertyErrorOrDefault(property, &error, 0.05));

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
    mcsDOUBLE bestDiam, errBestDiam, bestChi2, bestTeff, bestAv;

    // Ignore any failure:
    if (alxSedFitting(magnitudes, Av, e_Av, &bestDiam, &errBestDiam, &bestChi2, &bestTeff, &bestAv) == mcsSUCCESS)
    {
        /* TODO:  define a confidence index for diameter, Teff and Av based on chi2,
           is V available, is Av known ... */

        /* Put values */
        FAIL(SetPropertyValueAndError(sclsvrCALIBRATOR_SEDFIT_DIAM, bestDiam, errBestDiam, vobsORIG_COMPUTED));
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
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
 */
mcsCOMPL_STAT sclsvrCALIBRATOR::ComputeAngularDiameter(miscoDYN_BUF &msgInfo)
{
    // 3 diameters are required (some magnitudes may be invalid)
    static const mcsUINT32 nbRequiredDiameters = 3;

    // Fill the magnitude structure
    alxMAGNITUDES mags;
    FAIL(ExtractMagnitudesAndFixErrors(mags)); // set error to NAN if undefined


    // Check spectral type :
    // TODO: implement FAINT approach
    // = compute diameters without SpType (chi2 minimization)
    mcsINT32 colorTableIndex, colorTableDelta;
    {
        vobsSTAR_PROPERTY* property;

        // Get index in color tables => spectral type index
        property = GetProperty(sclsvrCALIBRATOR_COLOR_TABLE_INDEX);
        FAIL_FALSE_DO(IsPropertySet(property), errAdd(sclsvrERR_MISSING_PROPERTY, sclsvrCALIBRATOR_COLOR_TABLE_INDEX, "angular diameters"));
        FAIL(GetPropertyValue(property, &colorTableIndex));

        // Get delta in color tables => delta spectral type
        property = GetProperty(sclsvrCALIBRATOR_COLOR_TABLE_DELTA);
        FAIL_FALSE_DO(IsPropertySet(property), errAdd(sclsvrERR_MISSING_PROPERTY, sclsvrCALIBRATOR_COLOR_TABLE_DELTA, "angular diameters"));
        FAIL(GetPropertyValue(property, &colorTableDelta));
    }

    mcsDOUBLE spTypeIndex = colorTableIndex;
    mcsDOUBLE spTypeDelta = colorTableDelta;

    logInfo("spectral type index = %.1lf - delta = %.1lf", spTypeIndex, spTypeDelta)


    mcsUINT32 nbDiameters = 0;

    // Structure to fill with diameters
    alxDIAMETERS diameters;
    alxDIAMETERS_COVARIANCE diametersCov;

    // average diameters:
    alxDATA weightedMeanDiam, maxResidualsDiam, chi2Diam, maxCorrelations;

    // Compute diameters for spTypeIndex:
    FAIL(alxComputeAngularDiameters   ("(SP)   ", mags, spTypeIndex, diameters, diametersCov));

    /* may set low confidence to inconsistent diameters */
    FAIL(alxComputeMeanAngularDiameter(diameters, diametersCov, nbRequiredDiameters, &weightedMeanDiam,
                                       &maxResidualsDiam, &chi2Diam, &maxCorrelations,
                                       &nbDiameters, msgInfo.GetInternalMiscDYN_BUF()));


    /* TODO: handle uncertainty on spectral type (2016) ?
     * maybe like FAINT approach ? */
    if (false && alxIsSet(weightedMeanDiam) && (spTypeDelta != 0.0))
    {
        /* av unknown */
        msgInfo.Reset();

        mcsUINT32 nbDiametersSp;
        alxDIAMETERS diamsSpMin, diamsSpMax;

        // average diameters:
        alxDATA weightedMeanDiamSpMin, weightedMeanDiamSpMax;
        alxDATA maxResidualsSp, chi2DiamSp, maxCorrelationsSp;

        // Compute diameters for AvMin:
        FAIL(alxComputeAngularDiameters   ("(SP-DEL)", mags, spTypeIndex - spTypeDelta, diamsSpMin, diametersCov));

        /* may set low confidence to inconsistent diameters */
        FAIL(alxComputeMeanAngularDiameter(diamsSpMin, diametersCov, nbRequiredDiameters, &weightedMeanDiamSpMin,
                                           &maxResidualsSp, &chi2DiamSp, &maxCorrelationsSp, &nbDiametersSp, NULL));

        // Compute diameter for AvMax:
        FAIL(alxComputeAngularDiameters   ("(SP+DEL)", mags, spTypeIndex + spTypeDelta, diamsSpMax, diametersCov));

        /* may set low confidence to inconsistent diameters */
        FAIL(alxComputeMeanAngularDiameter(diamsSpMax, diametersCov, nbRequiredDiameters, &weightedMeanDiamSpMax,
                                           &maxResidualsSp, &chi2DiamSp, &maxCorrelationsSp, &nbDiametersSp, NULL));

        // Compute the final weighted mean diameter error
        /* ensure diameters within Sp range are computed */
        if (alxIsSet(weightedMeanDiamSpMin) && alxIsSet(weightedMeanDiamSpMax))
        {
            logInfo("weightedMeanDiams : %.5lf < %.5lf < %.5lf",
                    weightedMeanDiamSpMax.value, weightedMeanDiam.value, weightedMeanDiamSpMin.value);

            /* diameter is a log normal distribution */
            mcsDOUBLE logDiamMeanSpMin = log10(weightedMeanDiamSpMin.value);
            mcsDOUBLE logDiamMeanSpMax = log10(weightedMeanDiamSpMax.value);

            /* average diameter */
            mcsDOUBLE logDiamMean = 0.5 * (logDiamMeanSpMin + logDiamMeanSpMax);

            /* Compute error as the 1/2 (peek to peek) */
            mcsDOUBLE logDiamError = 0.5 * fabs(logDiamMeanSpMin - logDiamMeanSpMax);

            /* ensure relative error is 1% at least */
            logDiamError = alxMax(0.01, logDiamError);

            weightedMeanDiam.value = alxPow10(logDiamMean);

            /* increase error */
            weightedMeanDiam.error = alxMax(weightedMeanDiam.error,
                                            logDiamError * weightedMeanDiam.value * LOG_10); /* absolute error */

            /* Reset the confidence index */
            weightedMeanDiam.confIndex = alxCONFIDENCE_HIGH;


            /* residual (between each individual diameter) */
            /* residual = (distance between min/max diameter in each color)^2 / ( var(diamMin) + var(diamMax) ) */
            mcsUINT32  color;
            mcsUINT32  nResidual;
            mcsLOGICAL consistent = mcsTRUE;
            mcsSTRING32 tmp;

            mcsDOUBLE  relDiamError, diamErrAvMin, diamErrAvMax;
            mcsDOUBLE  residual;
            mcsDOUBLE  maxResidual = 0.0;
            mcsDOUBLE  chi2 = 0.0;

            miscDYN_BUF* diamInfo = msgInfo.GetInternalMiscDYN_BUF();

            for (nResidual = 0, color = 0; color < alxNB_DIAMS; color++)
            {
                /* ensure diameters are valid within Av range are computed */
                if (isDiameterValid(diameters[color]) && isDiameterValid(diamsSpMin[color]) && isDiameterValid(diamsSpMax[color]))
                {
                    /* correct errors to be 1% at least (typical bright case error) */

                    /* B^2 & B=EDIAM_C / (DIAM_C * ALOG(10.)) */
                    relDiamError = diamsSpMin[color].error / (diamsSpMin[color].value * LOG_10);
                    diamErrAvMin = alxMax(0.01, relDiamError);

                    /* B^2 & B=EDIAM_C / (DIAM_C * ALOG(10.)) */
                    relDiamError = diamsSpMax[color].error / (diamsSpMax[color].value * LOG_10);
                    diamErrAvMax = alxMax(0.01, relDiamError);

                    /* DIFF=ALOG10(DIAM_C_AV_0(II,*)) - ALOG10(DIAM_C_AV_3(II)) */

                    /* use variance = sum(diamVarAvMin + diamVarAvMax) because */
                    residual = alxMax(log10(diamsSpMin[color].value / weightedMeanDiam.value ) / diamErrAvMin,
                                      log10(weightedMeanDiam.value  / diamsSpMax[color].value) / diamErrAvMax);

                    if (residual > maxResidual)
                    {
                        maxResidual = residual;
                    }

                    if (residual > LOG_RESIDUAL_THRESHOLD)
                    {
                        if (IS_TRUE(consistent))
                        {
                            consistent = mcsFALSE;

                            /* Set diameter flag information */
                            miscDynBufAppendString(diamInfo, "WEAK_CONSISTENT_DIAMETER ");
                        }

                        /* Set confidence to LOW for each weak consistent diameter */
                        diameters[color].confIndex = alxCONFIDENCE_LOW;

                        /* Append each color (tolerance) in diameter flag information */
                        sprintf(tmp, "%s (%.1lf) ", alxGetDiamLabel((alxDIAM) color), residual);
                        miscDynBufAppendString(diamInfo, tmp);
                    }

                    /* update chi2 sum = RES^2 */
                    chi2 += alxSquare(residual);

                    nResidual++;
                }
            }

            if (nResidual >= nbRequiredDiameters)
            {
                chi2 /= nResidual - 1;
            }

            /* Check if max(residuals) < 5 or chi2 < 25
             * If higher i.e. inconsistency is found, the weighted mean diameter has a LOW confidence */
            if ((maxResidual > MAX_RESIDUAL_THRESHOLD) || (chi2 > DIAM_CHI2_THRESHOLD))
            {
                /* Set confidence to LOW */
                weightedMeanDiam.confIndex = alxCONFIDENCE_LOW;

                if (IS_NOT_NULL(diamInfo))
                {
                    /* Update diameter flag information */
                    miscDynBufAppendString(diamInfo, "INCONSISTENT_DIAMETERS ");
                }
            }

            /* Update max tolerance into diameter quality value */
            maxResidualsDiam.isSet = mcsTRUE;
            maxResidualsDiam.confIndex = alxCONFIDENCE_HIGH;
            maxResidualsDiam.value = maxResidual;

            /* Update chi2 */
            if (chi2 != 0.0)
            {
                chi2Diam.isSet = mcsTRUE;
                chi2Diam.confIndex = alxCONFIDENCE_HIGH;
                chi2Diam.value = chi2;
            }

            logTest("Diameter weighted=%.4lf(%.4lf %.1lf%%) valid=%s [%s] tolerance=%.2lf chi2=%.4lf from %d diameters: %s",
                    weightedMeanDiam.value, weightedMeanDiam.error, alxDATALogRelError(weightedMeanDiam),
                    (weightedMeanDiam.confIndex == alxCONFIDENCE_HIGH) ? "true" : "false",
                    alxGetConfidenceIndex(weightedMeanDiam.confIndex),
                    maxResidual, chi2,
                    nbDiameters,
                    IS_NOT_NULL(diamInfo) ? miscDynBufGetBuffer(diamInfo) : "");
        }
        else
        {
            /* Set confidence to LOW */
            weightedMeanDiam.confIndex = alxCONFIDENCE_LOW;
            chi2Diam.confIndex         = alxCONFIDENCE_LOW;
        }
    }


    /* Write Diameters now as their confidence may have been lowered in alxComputeMeanAngularDiameter() */
    SetComputedPropWithError(sclsvrCALIBRATOR_DIAM_VB, diameters[alxV_B_DIAM]);
    SetComputedPropWithError(sclsvrCALIBRATOR_DIAM_VJ, diameters[alxV_J_DIAM]);
    SetComputedPropWithError(sclsvrCALIBRATOR_DIAM_VH, diameters[alxV_H_DIAM]);
    SetComputedPropWithError(sclsvrCALIBRATOR_DIAM_VK, diameters[alxV_K_DIAM]);

    // Write DIAMETER COUNT
    FAIL(SetPropertyValue(sclsvrCALIBRATOR_DIAM_COUNT, (mcsINT32) nbDiameters, vobsORIG_COMPUTED));

    mcsLOGICAL diamFlag = mcsFALSE;

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

    // Write the max residuals:
    if alxIsSet(maxResidualsDiam)
    {
        FAIL(SetPropertyValue(sclsvrCALIBRATOR_DIAM_MAX_RESIDUALS, maxResidualsDiam.value, vobsORIG_COMPUTED, (vobsCONFIDENCE_INDEX) maxResidualsDiam.confIndex));
    }

    // Write the chi2:
    if alxIsSet(chi2Diam)
    {
        FAIL(SetPropertyValue(sclsvrCALIBRATOR_DIAM_CHI2, chi2Diam.value, vobsORIG_COMPUTED, (vobsCONFIDENCE_INDEX) chi2Diam.confIndex));
    }

    // Write the max correlations:
    if alxIsSet(maxCorrelations)
    {
        FAIL(SetPropertyValue(sclsvrCALIBRATOR_DIAM_MAX_CORRELATION, maxCorrelations.value, vobsORIG_COMPUTED, (vobsCONFIDENCE_INDEX) maxCorrelations.confIndex));
    }

    // Write LD DIAMETER
    if (alxIsSet(weightedMeanDiam) && alxIsSet(chi2Diam))
    {
        /* Corrected error = e_weightedMeanDiam x ((chi2Diam > 1.0) ? sqrt(chi2Diam) : 1.0 ) */
        mcsDOUBLE correctedError = weightedMeanDiam.error;
        if (chi2Diam.value > 1.0)
        {
            correctedError *= sqrt(chi2Diam.value);

            logTest("Corrected LD error=%.4lf (error=%.4lf, chi2=%.4lf)", correctedError, weightedMeanDiam.error, chi2Diam.value);
        }

        FAIL(SetPropertyValueAndError(sclsvrCALIBRATOR_LD_DIAM, weightedMeanDiam.value, correctedError, vobsORIG_COMPUTED, (vobsCONFIDENCE_INDEX) weightedMeanDiam.confIndex));
    }

    // Write the chi2:
    if alxIsSet(chi2Diam)
    {
        FAIL(SetPropertyValue(sclsvrCALIBRATOR_DIAM_CHI2, chi2Diam.value, vobsORIG_COMPUTED, (vobsCONFIDENCE_INDEX) chi2Diam.confIndex));
    }

    // Write the diameter flag (true | false):
    FAIL(SetPropertyValue(sclsvrCALIBRATOR_DIAM_FLAG, diamFlag, vobsORIG_COMPUTED));

    // Write DIAM INFO
    miscDynSIZE storedBytes;
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
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
 */
mcsCOMPL_STAT sclsvrCALIBRATOR::ComputeUDFromLDAndSP()
{
    // Compute UD only if LD is OK
    SUCCESS_FALSE_DO(IsDiameterOk(), logTest("Compute UD - Skipping (diameters are not OK)."));

    vobsSTAR_PROPERTY* property;
    property = GetProperty(sclsvrCALIBRATOR_LD_DIAM);

    // Does weighted mean diameter exist
    SUCCESS_FALSE_DO(IsPropertySet(property), logTest("Compute UD - Skipping (LD_DIAM unknown)."));

    // Get weighted mean diameter
    mcsDOUBLE ld = NAN;
    SUCCESS_DO(GetPropertyValue(property, &ld), logWarning("Compute UD - Aborting (error while retrieving LD_DIAM)."));

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
    logTest("Computing UDs for LD=%.4lf, Teff=%.3lf, LogG=%.3lf ...", ld, Teff, LogG);

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
    FAIL(SetPropertyValue(sclsvrCALIBRATOR_UD_M, ud.m, vobsORIG_COMPUTED, ldConfIndex));
    FAIL(SetPropertyValue(sclsvrCALIBRATOR_UD_N, ud.n, vobsORIG_COMPUTED, ldConfIndex));

    return mcsSUCCESS;
}

/**
 * Compute visibility.
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
 */
mcsCOMPL_STAT sclsvrCALIBRATOR::ComputeVisibility(const sclsvrREQUEST &request)
{
    mcsDOUBLE diam, diamError, baseMax, wavelength;
    alxVISIBILITIES visibilities;
    vobsCONFIDENCE_INDEX confidenceIndex = vobsCONFIDENCE_HIGH;

    // Get object diameter. First look at the diameters coming from catalog
    // Borde (UDDK), Merand (UDDK)
    static const mcsUINT32 nDiamId = 1;
    static const char* diamId[nDiamId] = { vobsSTAR_UDDK_DIAM };

    vobsSTAR_PROPERTY* property;

    // For each possible diameters
    mcsLOGICAL found = mcsFALSE;
    for (mcsUINT32 i = 0; (i < nDiamId) && IS_FALSE(found); i++)
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
    if (IS_FALSE(found))
    {
        // If computed diameter is OK
        SUCCESS_COND_DO(IS_FALSE(IsDiameterOk()) || IS_FALSE(IsPropertySet(sclsvrCALIBRATOR_LD_DIAM)),
                        logTest("Unknown LD diameter or diameters are not OK; could not compute visibility"));

        // FIXME: totally wrong => should use the UD diameter for the appropriate band (see Aspro2)
        // But move that code into SearchCal GUI instead.

        // Get the LD diameter and associated error value
        property = GetProperty(sclsvrCALIBRATOR_LD_DIAM);
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

    FAIL_COND(IS_NULL(ra) || IS_TRUE(miscIsSpaceStr(ra)));

    // Get science object right ascension in degrees
    mcsDOUBLE scienceObjectRa = request.GetObjectRaInDeg();

    // Get the science object declination as a C string
    const char* dec = request.GetObjectDec();

    FAIL_COND(IS_NULL(dec) || IS_TRUE(miscIsSpaceStr(dec)));

    // Get science object declination in degrees
    mcsDOUBLE scienceObjectDec = request.GetObjectDecInDeg();

    // Get the internal calibrator right ascension in arcsec
    mcsDOUBLE calibratorRa;
    FAIL(GetRa(calibratorRa));

    // Get the internal calibrator declination in arcsec
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
 * @return mcsSUCCESS on successful parsing, mcsFAILURE otherwise.
 */
mcsCOMPL_STAT sclsvrCALIBRATOR::ParseSpectralType()
{
    // initialize the spectral type structure anyway:
    FAIL(alxInitializeSpectralType(&_spectralType));

    vobsSTAR_PROPERTY* property = GetProperty(vobsSTAR_SPECT_TYPE_MK);
    SUCCESS_FALSE_DO(IsPropertySet(property), logTest("Spectral Type - Skipping (no SpType available)."));

    mcsSTRING32 spType;
    strncpy(spType, GetPropertyValue(property), sizeof (spType) - 1);
    SUCCESS_COND_DO(IS_STR_EMPTY(spType), logTest("Spectral Type - Skipping (SpType unknown)."));

    logDebug("Parsing Spectral Type '%s'.", spType);

    /*
     * Get each part of the spectral type XN.NLLL where X is a letter, N.N a
     * number between 0 and 9 and LLL is the light class
     */
    SUCCESS_DO(alxString2SpectralType(spType, &_spectralType),
               errCloseStack();
               logTest("Spectral Type - Skipping (could not parse SpType '%s').", spType));

    if (IS_TRUE(_spectralType.isSpectralBinary))
    {
        logTest("Spectral Binarity - 'SB' found in SpType.");

        // Only store spectral binarity if none present before
        FAIL(SetPropertyValue(vobsSTAR_CODE_BIN_FLAG, "SB", vobsORIG_COMPUTED));
    }

    if (IS_TRUE(_spectralType.isDouble))
    {
        logTest("Binarity - '+' found in SpType.");

        // Only store binarity if none present before
        FAIL(SetPropertyValue(vobsSTAR_CODE_MULT_FLAG, "S", vobsORIG_COMPUTED));
    }

    // Anyway, store our decoded spectral type:
    FAIL(SetPropertyValue(sclsvrCALIBRATOR_SP_TYPE, _spectralType.ourSpType, vobsORIG_COMPUTED));

    return mcsSUCCESS;
}

/**
 * Define color table indexes based on the original spectral type if available.
 *
 * @return mcsSUCCESS on successful parsing, mcsFAILURE otherwise.
 */
mcsCOMPL_STAT sclsvrCALIBRATOR::DefineSpectralTypeIndexes()
{
    mcsINT32 colorTableIndex, colorTableDelta, lumClass, deltaLumClass;

    alxGiveIndexInTableOfSpectralCodes(_spectralType,
                                       &colorTableIndex, &colorTableDelta, &lumClass, &deltaLumClass);

    /* IDL code use following integer indexes related to absolute magnitude tables */
    // Set index in color tables
    FAIL(SetPropertyValue(sclsvrCALIBRATOR_COLOR_TABLE_INDEX, colorTableIndex, vobsORIG_COMPUTED));
    // Set delta in color tables
    FAIL(SetPropertyValue(sclsvrCALIBRATOR_COLOR_TABLE_DELTA, colorTableDelta, vobsORIG_COMPUTED));
    // Set luminosity class
    FAIL(SetPropertyValue(sclsvrCALIBRATOR_LUM_CLASS, lumClass, vobsORIG_COMPUTED));
    // Set delta in luminosity class
    FAIL(SetPropertyValue(sclsvrCALIBRATOR_LUM_CLASS_DELTA, deltaLumClass, vobsORIG_COMPUTED));

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
    SUCCESS_COND_DO(IS_FALSE(has9) && IS_FALSE(has18), logTest("IR Fluxes: Skipping (no 9 mu or 18 mu flux available)."));

    property = GetProperty(vobsSTAR_PHOT_FLUX_IR_12);

    // check presence etc of F12:
    f12AlreadySet = IsPropertySet(property);

    // check presence etc of e_F12:
    e_f12AlreadySet = IsPropertyErrorSet(property);

    // if f9 is defined, compute all fluxes from it, then fill void places.
    if (IS_TRUE(has9))
    {
        // Compute all fluxes from 9 onwards
        SUCCESS_DO(alxComputeFluxesFromAkari09(Teff, &fnu_9, &fnu_12, &fnu_18), logWarning("IR Fluxes: Skipping (akari internal error)."));

        logTest("IR Fluxes: akari measured fnu_09=%.3lf computed fnu_12=%.3lf fnu_18=%.3lf", fnu_9, fnu_12, fnu_18);

        // Store it eventually:
        if (IS_FALSE(f12AlreadySet))
        {
            FAIL(SetPropertyValue(vobsSTAR_PHOT_FLUX_IR_12, fnu_12, vobsORIG_COMPUTED));
        }
        // Compute Mag N:
        magN = 4.1 - 2.5 * log10(fnu_12 / 0.89);

        logTest("IR Fluxes: computed magN=%.3lf", magN);

        // Store it if not set:
        FAIL(SetPropertyValue(vobsSTAR_PHOT_JHN_N, magN, vobsORIG_COMPUTED));

        // store s18 if void:
        if (IS_FALSE(has18))
        {
            FAIL(SetPropertyValue(vobsSTAR_PHOT_FLUX_IR_18, fnu_18, vobsORIG_COMPUTED));
        }

        // compute s_12 error etc, if s09_err is present:
        if (IS_TRUE(hasErr_9))
        {
            SUCCESS_DO(alxComputeFluxesFromAkari09(Teff, &e_fnu_9, &e_fnu_12, &e_fnu_18), logTest("IR Fluxes: Skipping (akari internal error)."));

            // Store it eventually:
            if (IS_FALSE(e_f12AlreadySet))
            {
                FAIL(SetPropertyError(vobsSTAR_PHOT_FLUX_IR_12, e_fnu_12));
            }
            // store e_s18 if void:
            if (IS_FALSE(hasErr_18))
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

        logTest("IR Fluxes: akari measured fnu_18=%.3lf computed fnu_12=%.3lf fnu_09=%.3lf", fnu_18, fnu_12, fnu_9);

        // Store it eventually:
        if (IS_FALSE(f12AlreadySet))
        {
            FAIL(SetPropertyValue(vobsSTAR_PHOT_FLUX_IR_12, fnu_12, vobsORIG_COMPUTED));
        }
        // Compute Mag N:
        magN = 4.1 - 2.5 * log10(fnu_12 / 0.89);

        logTest("IR Fluxes: computed magN=%.3lf", magN);

        // Store it if not set:
        FAIL(SetPropertyValue(vobsSTAR_PHOT_JHN_N, magN, vobsORIG_COMPUTED));

        // store s9 if void:
        if (IS_FALSE(has9))
        {
            FAIL(SetPropertyValue(vobsSTAR_PHOT_FLUX_IR_09, fnu_9, vobsORIG_COMPUTED));
        }

        // compute s_12 error etc, if s18_err is present:
        if (IS_TRUE(hasErr_18))
        {
            SUCCESS_DO(alxComputeFluxesFromAkari18(Teff, &e_fnu_18, &e_fnu_12, &e_fnu_9), logTest("IR Fluxes: Skipping (akari internal error)."));

            // Store it eventually:
            if (IS_FALSE(e_f12AlreadySet))
            {
                FAIL(SetPropertyError(vobsSTAR_PHOT_FLUX_IR_12, e_fnu_12));
            }
            // store e_s9 if void:
            if (IS_FALSE(hasErr_9))
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
            else if ((parallaxError / parallax) > 0.501)
            {
                // JSDC v2: 20/10/2013: increased threshold to 50% as parallax comes from HIP2 only (not ASCC)

                // If parallax error is too high
                logTest("parallax %.2lf(%.2lf) is not valid...", parallax, parallaxError);
            }
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
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
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

    /*
     * From http://www.astro.caltech.edu/~jmc/2mass/v3/transformations/
    2MASS - CIT
    (Ks)2MASS	= 	KCIT	+ 	(-0.019 ± 0.004)	+ 	(0.001 ± 0.005)(J-K)CIT
    (J-H)2MASS	= 	(1.087 ± 0.013)(J-H)CIT	+ 	(-0.047 ± 0.007)
    (J-Ks)2MASS	= 	(1.068 ± 0.009)(J-K)CIT	+ 	(-0.020 ± 0.007)
    (H-Ks)2MASS	= 	(1.000 ± 0.023)(H-K)CIT	+ 	(0.034 ± 0.006)
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
            eKc = alxNorm(0.994 * eK, 0.006 * eJ);
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
            eKc = alxNorm(0.992 * eK, 0.008 * eV);
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
                    eHc = alxNorm((1000.0 / 1026.0) * eH, (26.0 / 1026.0) * eK);
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
                    eJc = alxNorm((1000.0 / 1056.0) * eJ, (56.0 / 1056.0) * eK);
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
        FAIL(GetPropertyErrorOrDefault(magIc, &eIc, NAN));
    }

    logTest("Cousin magnitudes: I=%0.3lf(%0.3lf) J=%0.3lf(%0.3lf) H=%0.3lf(%0.3lf) K=%0.3lf(%0.3lf)",
            mIc, eIc, mJc, eJc, mHc, eHc, mKc, eKc);

    return mcsSUCCESS;
}

/**
 * Fill the I, J, H and K JOHNSON magnitude (actually the 2MASS system)
 * from the COUSIN/CIT magnitudes.
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
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
        AddPropertyMeta(sclsvrCALIBRATOR_DIAM_VB, "diam_vb", vobsFLOAT_PROPERTY, "mas",   "V-B Diameter");
        AddPropertyErrorMeta(sclsvrCALIBRATOR_DIAM_VB_ERROR, "e_diam_vb", "mas", "Error on V-B Diameter");

        AddPropertyMeta(sclsvrCALIBRATOR_DIAM_VJ, "diam_vj", vobsFLOAT_PROPERTY, "mas",   "V-J Diameter");
        AddPropertyErrorMeta(sclsvrCALIBRATOR_DIAM_VJ_ERROR, "e_diam_vj", "mas", "Error on V-J Diameter");
        AddPropertyMeta(sclsvrCALIBRATOR_DIAM_VH, "diam_vh", vobsFLOAT_PROPERTY, "mas",   "V-H Diameter");
        AddPropertyErrorMeta(sclsvrCALIBRATOR_DIAM_VH_ERROR, "e_diam_vh", "mas", "Error on V-H Diameter");
        AddPropertyMeta(sclsvrCALIBRATOR_DIAM_VK, "diam_vk", vobsFLOAT_PROPERTY, "mas",   "V-K Diameter");
        AddPropertyErrorMeta(sclsvrCALIBRATOR_DIAM_VK_ERROR, "e_diam_vk", "mas", "Error on V-K Diameter");

        /* diameter count used by mean / weighted mean / stddev (consistent ones) */
        AddPropertyMeta(sclsvrCALIBRATOR_DIAM_COUNT, "diam_count", vobsINT_PROPERTY, NULL, "Number of consistent and valid (high confidence) computed diameters (used by mean / weighted mean / stddev computations)");

        /* weighted mean diameter */
        AddPropertyMeta(sclsvrCALIBRATOR_DIAM_WEIGHTED_MEAN, "diam_weighted_mean", vobsFLOAT_PROPERTY, "mas", "Weighted mean diameter by inverse(diameter covariance matrix)");
        AddPropertyErrorMeta(sclsvrCALIBRATOR_DIAM_WEIGHTED_MEAN_ERROR, "e_diam_weighted_mean", "mas", "Error on weighted mean diameter");

        /* diameter max residuals (sigma) */
        AddFormattedPropertyMeta(sclsvrCALIBRATOR_DIAM_MAX_RESIDUALS, "diam_max_residuals", vobsFLOAT_PROPERTY, NULL, "%.4lf",
                                 "Max residuals between weighted mean diameter and individual diameters (expressed in sigma)");

        /* chi2 of the weighted mean diameter estimation */
        AddFormattedPropertyMeta(sclsvrCALIBRATOR_DIAM_CHI2, "diam_chi2", vobsFLOAT_PROPERTY, NULL, "%.4lf",
                                 "Reduced chi-square of the weighted mean diameter estimation");

        /* maximum of the correlation coefficients for all individual diameters */
        AddPropertyMeta(sclsvrCALIBRATOR_DIAM_MAX_CORRELATION, "diam_max_correlation", vobsFLOAT_PROPERTY, NULL,
                        "(internal) maximum of the correlation coefficients for all individual diameters");

        /* LD diameter */
        AddPropertyMeta(sclsvrCALIBRATOR_LD_DIAM, "LDD", vobsFLOAT_PROPERTY, "mas", "Limb-darkened diameter (= weighted mean diameter)");
        AddPropertyErrorMeta(sclsvrCALIBRATOR_LD_DIAM_ERROR, "e_LDD", "mas", "Error on limb-darkened diameter (= weighted mean diameter error x SQRT(chi2) if chi2 > 1)");

        /* diameter quality (true | false) */
        AddPropertyMeta(sclsvrCALIBRATOR_DIAM_FLAG, "diamFlag", vobsBOOL_PROPERTY, NULL, "Diameter Flag (true means a valid weighted mean diameter)");
        /* information about the diameter computation */
        AddPropertyMeta(sclsvrCALIBRATOR_DIAM_FLAG_INFO, "diamFlagInfo", vobsSTRING_PROPERTY, NULL, "Information related to the weighted mean diameter estimation");

        /* Results from SED fitting */
        AddPropertyMeta(sclsvrCALIBRATOR_SEDFIT_CHI2, "chi2_SED", vobsFLOAT_PROPERTY, NULL, "Reduced chi2 of the SED fitting (experimental)");
        AddPropertyMeta(sclsvrCALIBRATOR_SEDFIT_DIAM, "diam_SED", vobsFLOAT_PROPERTY, "mas", "Diameter from SED fitting (experimental)");
        AddPropertyErrorMeta(sclsvrCALIBRATOR_SEDFIT_DIAM_ERROR, "e_diam_SED", "mas", "Diameter from SED fitting (experimental)");
        AddPropertyMeta(sclsvrCALIBRATOR_SEDFIT_TEFF, "Teff_SED", vobsFLOAT_PROPERTY, "K", "Teff from SED fitting (experimental)");
        AddPropertyMeta(sclsvrCALIBRATOR_SEDFIT_AV, "Av_SED", vobsFLOAT_PROPERTY, NULL, "Av from SED fitting (experimental)");

        /* Teff / Logg determined from spectral type */
        AddPropertyMeta(sclsvrCALIBRATOR_TEFF_SPTYP, "Teff_SpType", vobsFLOAT_PROPERTY, "K", "Effective Temperature adopted from Spectral Type");
        AddPropertyMeta(sclsvrCALIBRATOR_LOGG_SPTYP, "logg_SpType", vobsFLOAT_PROPERTY, "[cm/s2]", "Gravity adopted from Spectral Type");

        /* Uniform Disk diameters */
        AddPropertyMeta(sclsvrCALIBRATOR_UD_U, "UD_U", vobsFLOAT_PROPERTY, "mas", "U-band Uniform-Disk Diameter");
        AddPropertyMeta(sclsvrCALIBRATOR_UD_B, "UD_B", vobsFLOAT_PROPERTY, "mas", "B-band Uniform-Disk Diameter");
        AddPropertyMeta(sclsvrCALIBRATOR_UD_V, "UD_V", vobsFLOAT_PROPERTY, "mas", "V-band Uniform-Disk Diameter");
        AddPropertyMeta(sclsvrCALIBRATOR_UD_R, "UD_R", vobsFLOAT_PROPERTY, "mas", "R-band Uniform-Disk Diameter");
        AddPropertyMeta(sclsvrCALIBRATOR_UD_I, "UD_I", vobsFLOAT_PROPERTY, "mas", "I-band Uniform-Disk Diameter");
        AddPropertyMeta(sclsvrCALIBRATOR_UD_J, "UD_J", vobsFLOAT_PROPERTY, "mas", "J-band Uniform-Disk Diameter");
        AddPropertyMeta(sclsvrCALIBRATOR_UD_H, "UD_H", vobsFLOAT_PROPERTY, "mas", "H-band Uniform-Disk Diameter");
        AddPropertyMeta(sclsvrCALIBRATOR_UD_K, "UD_K", vobsFLOAT_PROPERTY, "mas", "K-band Uniform-Disk Diameter");
        AddPropertyMeta(sclsvrCALIBRATOR_UD_L, "UD_L", vobsFLOAT_PROPERTY, "mas", "L-band Uniform-Disk Diameter");
        AddPropertyMeta(sclsvrCALIBRATOR_UD_M, "UD_M", vobsFLOAT_PROPERTY, "mas", "M-band Uniform-Disk Diameter");
        AddPropertyMeta(sclsvrCALIBRATOR_UD_N, "UD_N", vobsFLOAT_PROPERTY, "mas", "N-band Uniform-Disk Diameter");

        /* extinction ratio related to interstellar absorption */
        AddPropertyMeta(sclsvrCALIBRATOR_EXTINCTION_RATIO, "Av", vobsFLOAT_PROPERTY, NULL, "Visual Interstellar Absorption");
        AddPropertyErrorMeta(sclsvrCALIBRATOR_EXTINCTION_RATIO_ERROR, "e_Av", NULL, "Error on Visual Interstellar Absorption");

        /* method to compute the extinction ratio */
        AddPropertyMeta(sclsvrCALIBRATOR_AV_METHOD, "Av_method", vobsINT_PROPERTY, NULL,
                        "method to compute the extinction ratio: "
                        "0 = Undefined method,"
                        "1 = Unknown i.e. use range [0..3],"
                        "2 = Statistical estimation,"
                        "3 = Fit from photometric magnitudes and spectral type with luminosity class,"
                        "4 = Best chi2 from fit from photometric magnitudes and spectral type (no luminosity class)."
                        );

        /* fitted extinction ratio computed from photometric magnitudes and spectral type */
        AddPropertyMeta(sclsvrCALIBRATOR_AV_FIT, "Av_fit", vobsFLOAT_PROPERTY, NULL,
                        "Visual Interstellar Absorption computed from photometric magnitudes and spectral type");
        AddPropertyErrorMeta(sclsvrCALIBRATOR_AV_FIT_ERROR, "e_Av_fit", NULL,
                             "Error on Visual Interstellar Absorption computed from photometric magnitudes and spectral type");

        /* fitted distance (parsec) computed from photometric magnitudes and spectral type */
        AddPropertyMeta(sclsvrCALIBRATOR_DIST_FIT, "dist_fit", vobsFLOAT_PROPERTY, "pc",
                        "fitted distance computed from photometric magnitudes and spectral type");
        AddPropertyErrorMeta(sclsvrCALIBRATOR_DIST_FIT_ERROR, "e_dist_fit", "pc",
                             "Error on fitted distance computed from photometric magnitudes and spectral type");

        /* chi2 of the extinction ratio estimation */
        AddFormattedPropertyMeta(sclsvrCALIBRATOR_AV_FIT_CHI2, "Av_fit_chi2", vobsFLOAT_PROPERTY, NULL, "%.4lf",
                                 "Reduced chi-square of the extinction ratio estimation");

        /* chi2 of the distance modulus (dist_plx vs dist_fit) */
        AddFormattedPropertyMeta(sclsvrCALIBRATOR_DIST_FIT_CHI2, "dist_fit_chi2", vobsFLOAT_PROPERTY, NULL, "%.4lf",
                                 "chi-square of the distance modulus (dist_plx vs dist_fit)");

        /* statistical extinction ratio computed from parallax using statistical approach */
        AddPropertyMeta(sclsvrCALIBRATOR_AV_STAT, "Av_stat", vobsFLOAT_PROPERTY, NULL,
                        "Visual Interstellar Absorption computed from parallax using statistical approach");
        AddPropertyErrorMeta(sclsvrCALIBRATOR_AV_STAT_ERROR, "e_Av_stat", NULL,
                             "Error on Visual Interstellar Absorption computed from parallax using statistical approach");

        /* distance computed from parallax */
        AddPropertyMeta(sclsvrCALIBRATOR_DIST_PLX, "dist_plx", vobsFLOAT_PROPERTY, "pc",
                        "distance computed from parallax");
        AddPropertyErrorMeta(sclsvrCALIBRATOR_DIST_STAT_ERROR, "e_dist_plx", "pc",
                             "Error on distance computed from parallax");

        /* square visibility */
        AddPropertyMeta(sclsvrCALIBRATOR_VIS2, "vis2", vobsFLOAT_PROPERTY, NULL, "Squared Visibility");
        AddPropertyErrorMeta(sclsvrCALIBRATOR_VIS2_ERROR, "vis2Err", NULL, "Error on Squared Visibility");

        /* distance to the science object */
        AddPropertyMeta(sclsvrCALIBRATOR_DIST, "dist", vobsFLOAT_PROPERTY, "deg", "Calibrator to Science object Angular Distance");

        /* corrected spectral type */
        AddPropertyMeta(sclsvrCALIBRATOR_SP_TYPE, "SpType_JMMC", vobsSTRING_PROPERTY, NULL, "Corrected spectral type by the JMMC");

        /* luminosity class (min) */
        AddPropertyMeta(sclsvrCALIBRATOR_LUM_CLASS, "lum_class", vobsINT_PROPERTY, NULL, "(internal) (min) luminosity class from spectral type (1,2,3,4,5)");
        /* luminosity class delta (lc_max = lum_class + lum_class_delta) */
        AddPropertyMeta(sclsvrCALIBRATOR_LUM_CLASS_DELTA, "lum_class_delta", vobsINT_PROPERTY, NULL, "(internal) luminosity class delta (lc_max = lum_class + lum_class_delta)");

        /* index in color tables */
        AddPropertyMeta(sclsvrCALIBRATOR_COLOR_TABLE_INDEX, "color_table_index", vobsINT_PROPERTY, NULL, "(internal) line number in color tables");

        /* line delta in color tables */
        AddPropertyMeta(sclsvrCALIBRATOR_COLOR_TABLE_DELTA, "color_table_delta", vobsINT_PROPERTY, NULL, "(internal) line delta in color tables");

        /* star name (identifier) */
        AddPropertyMeta(sclsvrCALIBRATOR_NAME, "Name", vobsSTRING_PROPERTY, NULL,
                        "Star name (identifier from HIP, HD, TYC, 2MASS, DM or coordinates 'RA DE'), click to call Simbad on this object",
                        "http://simbad.u-strasbg.fr/simbad/sim-id?protocol=html&amp;Ident=${Name}");

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
        for (mcsINT32 i = sclsvrCALIBRATOR::sclsvrCALIBRATOR_PropertyMetaBegin; i < sclsvrCALIBRATOR::sclsvrCALIBRATOR_PropertyMetaEnd; i++)
        {
            meta = vobsSTAR::GetPropertyMeta(i);

            if (IS_NOT_NULL(meta))
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
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
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
    if (IS_NOT_NULL(resolvedPath))
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

