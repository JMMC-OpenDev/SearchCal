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
#define sclsvrCALIBRATOR_PERFORM_SED_FITTING false

/* maximum number of properties (81) */
#define sclsvrCALIBRATOR_MAX_PROPERTIES 81

/* Error identifiers */
#define sclsvrCALIBRATOR_DIAM_VJ_ERROR      "DIAM_VJ_ERROR"
#define sclsvrCALIBRATOR_DIAM_VH_ERROR      "DIAM_VH_ERROR"
#define sclsvrCALIBRATOR_DIAM_VK_ERROR      "DIAM_VK_ERROR"

#define sclsvrCALIBRATOR_LD_DIAM_ERROR      "LD_DIAM_ERROR"

#define sclsvrCALIBRATOR_SEDFIT_DIAM_ERROR  "SEDFIT_DIAM_ERROR"

#define sclsvrCALIBRATOR_EXTINCTION_RATIO_ERROR "EXTINCTION_RATIO_ERROR"

#define sclsvrCALIBRATOR_AV_FIT_ERROR       "AV_FIT_ERROR"
#define sclsvrCALIBRATOR_DIST_FIT_ERROR     "DIST_FIT_ERROR"

#define sclsvrCALIBRATOR_AV_STAT_ERROR      "AV_STAT_ERROR"
#define sclsvrCALIBRATOR_DIST_STAT_ERROR    "DIST_STAT_ERROR"

#define sclsvrCALIBRATOR_VIS2_ERROR         "VIS2_ERROR"

// Same thresholds as IDL:
#define sclsvrCALIBRATOR_EMAG_MIN           0.01
#define sclsvrCALIBRATOR_EMAG_UNDEF         0.25
#define sclsvrCALIBRATOR_EMAG_MAX           0.15

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

    // Parse spectral type.
    FAIL(ParseSpectralType());
    FAIL(DefineSpectralTypeIndexes());

    // Compute diameter with SpType (bright) or without (faint: try all sptypes)
    // May fix the spectral type (min chi2)
    FAIL(ComputeAngularDiameter(msgInfo));

    FAIL(ComputeSedFitting());

    // Fill in the Teff and LogG entries using the spectral type
    FAIL(ComputeTeffLogg());

    // Compute N Band and S_12 with AKARI from Teff
    FAIL(ComputeIRFluxes());

    // Compute UD from LD and Teff (SP)
    FAIL(ComputeUDFromLDAndSP());

    if (notJSDC)
    {
        // TODO: move these last two steps into SearchCal GUI (Vis2 + distance)

        // Compute visibility and visibility error only if diameter OK
        FAIL(ComputeVisibility(request));

        // Compute distance
        FAIL(ComputeDistance(request));
    }

    if (!request.IsDiagnose())
    {
        // Final clean up:
        CleanProperties();
    }

    return mcsSUCCESS;
}

/*
 * Private methods
 */

/**
 * Clean up useless properties
 */
void sclsvrCALIBRATOR::CleanProperties()
{
    logInfo("CleanProperties");

    static const char* propIds[] = {
                                    /* vobsSTAR */
                                    /* Vizier xmatch (target / jd) */
                                    vobsSTAR_ID_TARGET,
                                    vobsSTAR_JD_DATE,

                                    /* HIP B-V V-Ic */
                                    vobsSTAR_PHOT_JHN_B_V,
                                    vobsSTAR_PHOT_COUS_V_I,
                                    vobsSTAR_PHOT_COUS_V_I_REFER_CODE,

                                    /* Icous */
                                    vobsSTAR_PHOT_COUS_I,

                                    /* 2MASS / Wise Code quality */
                                    vobsSTAR_2MASS_OPT_ID_CATALOG,
                                    vobsSTAR_CODE_QUALITY_2MASS,
                                    vobsSTAR_CODE_QUALITY_WISE,

                                    /* AKARI fluxes */
                                    vobsSTAR_ID_AKARI,
                                    vobsSTAR_PHOT_FLUX_IR_09,
                                    vobsSTAR_PHOT_FLUX_IR_12,
                                    vobsSTAR_PHOT_FLUX_IR_18,

                                    /* sclsvrCALIBRATOR */
                                    sclsvrCALIBRATOR_DIAM_COUNT,
                                    sclsvrCALIBRATOR_DIAM_FLAG_INFO,

                                    /* index/delta in color tables */
                                    sclsvrCALIBRATOR_COLOR_TABLE_INDEX,
                                    sclsvrCALIBRATOR_COLOR_TABLE_DELTA,

                                    sclsvrCALIBRATOR_COLOR_TABLE_INDEX_FIX,
                                    sclsvrCALIBRATOR_COLOR_TABLE_DELTA_FIX,

                                    sclsvrCALIBRATOR_LUM_CLASS,
                                    sclsvrCALIBRATOR_LUM_CLASS_DELTA
    };

    const mcsUINT32 propIdLen = sizeof (propIds) / sizeof (char*);

    vobsSTAR_PROPERTY* property;

    for (mcsUINT32 i = 0; i < propIdLen; i++)
    {
        property = GetProperty(propIds[i]);

        if (isPropSet(property))
        {
            property->ClearValue();
        }
    }
}

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

        alxDATAClear(magnitudes[band]);

        // Get the magnitude value
        if (isPropSet(property)
                && (!hasOrigins || (originIdxs[band] == vobsORIG_NONE) || (property->GetOriginIndex() == originIdxs[band])))
        {
            logDebug("ExtractMagnitudes[%s]: origin = %s", alxGetBandLabel((alxBAND) band), vobsGetOriginIndex(property->GetOriginIndex()))

            mcsDOUBLE mag;

            FAIL(GetPropertyValue(property, &mag));

            // check validity range [-2; 20]
            if ((mag > -2.0) && (mag < 20.0))
            {

                magnitudes[band].value = mag;
                magnitudes[band].isSet = mcsTRUE;
                magnitudes[band].confIndex = (alxCONFIDENCE_INDEX) property->GetConfidenceIndex();

                /* Extract error or default error if none */
                FAIL(GetPropertyErrorOrDefault(property, &magnitudes[band].error, defError));
            }
        }
    }
    return mcsSUCCESS;
}

/**
 * Fill the given magnitudes B to last band (M by default) and
 * fix error values (min error for all magnitudes and max error for 2MASS magnitudes)
 * @param magnitudes alxMAGNITUDES structure to fill
 * @param faint flag indicating FAINT case
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.;
 */
mcsCOMPL_STAT sclsvrCALIBRATOR::ExtractMagnitudesAndFixErrors(alxMAGNITUDES &magnitudes, mcsLOGICAL faint)
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

    // set error to the upper limit if undefined (see below):
    FAIL(ExtractMagnitudes(magnitudes, magIds, NAN, originIdxs));

    // We now have mag = {Bj, Vj, Rj, Ic, Jj, Hj, Kj, Lj, Mj, Nj}
    alxLogTestMagnitudes("Extracted magnitudes:", "", magnitudes);

    for (mcsUINT32 band = alxB_BAND; band <= alxV_BAND; band++)
    {
        if (alxIsSet(magnitudes[band]))
        {
            // Fix absent photometric errors on b and v
            if (isnan(magnitudes[band].error))
            {
                if (magnitudes[band].value < 3.0)
                {
                    // very bright stars
                    magnitudes[band].error = 0.1;
                }
                else
                {
                    // some faint stars have no e_v: put them at 0.04
                    magnitudes[band].error = 0.04;
                }
            }

            // Fix error lower limit to 0.01 mag (BV only):
            if (magnitudes[band].error < sclsvrCALIBRATOR_EMAG_MIN)
            {
                logDebug("Fix  low magnitude error[%s]: error = %.3lf => %.3lf", alxGetBandLabel((alxBAND) band),
                         magnitudes[band].error, sclsvrCALIBRATOR_EMAG_MIN);

                magnitudes[band].error = sclsvrCALIBRATOR_EMAG_MIN;
            }
        }
    }

    // normalize JHK error on max JHK error:
    mcsDOUBLE maxErr = 0.0;
    for (mcsUINT32 band = alxJ_BAND; band <= alxK_BAND; band++)
    {
        if (alxIsSet(magnitudes[band]) && !isnan(magnitudes[band].error))
        {
            maxErr = alxMax(maxErr, magnitudes[band].error);
        }
    }
    if (maxErr > 0.0)
    {
        logDebug("Fix JHK magnitude error: %.3lf", maxErr);

        for (mcsUINT32 band = alxJ_BAND; band <= alxK_BAND; band++)
        {
            if (alxIsSet(magnitudes[band]))
            {
                magnitudes[band].error = maxErr;
            }
        }
    }

    mcsDOUBLE emagUndef = sclsvrCALIBRATOR_EMAG_UNDEF;
    mcsDOUBLE emagMax   = sclsvrCALIBRATOR_EMAG_MAX;

    if (IS_TRUE(faint))
    {
        // avoid too large magnitude error to have chi2 more discrimmative:
        // ensure small error ie 0.1 mag to help chi2 selectivity:
        emagUndef = emagMax = 0.1;
    }

    // Fix error upper limit to 0.25 mag and undefined error to 0.35 mag (B..N):
    for (mcsUINT32 band = alxB_BAND; band <= alxN_BAND; band++)
    {
        if (alxIsSet(magnitudes[band]))
        {
            // Fix Undefined error:
            if (isnan(magnitudes[band].error))
            {
                logDebug("Fix undefined magnitude error[%s]: error => %.3lf", alxGetBandLabel((alxBAND) band),
                         emagUndef);

                magnitudes[band].error = emagUndef;
            }
            else if (magnitudes[band].error > emagMax)
            {
                logDebug("Fix high magnitude error[%s]: error = %.3lf => %.3lf", alxGetBandLabel((alxBAND) band),
                         magnitudes[band].error, emagMax);

                magnitudes[band].error = emagMax;
            }
        }
    }

    alxLogTestMagnitudes("Fixed magnitudes:", "(fix error)", magnitudes);

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
    if (!vobsIsDevFlag() || !sclsvrCALIBRATOR_PERFORM_SED_FITTING)
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

    /* When the Av is not known, the full range of approx 0..3 is considered as valid for the fit. */
    mcsDOUBLE Av, e_Av;
    Av = 0.5;
    e_Av = 2.0;

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

    static const bool forceFAINT = false;
    static const bool logValues = false;

    // Enforce using polynom domain:
    // TODO: externalize such values
    /*
        #FIT COLORS:  V-J V-H V-K
        #domain:       42.000000       272.00000
     */
    /* Note: it is forbidden to extrapolate polynoms: may diverge strongly ! */
    static const mcsUINT32 SPTYPE_MIN = 42;  // B0.5
    static const mcsUINT32 SPTYPE_MAX = 272; // M8

    /* max color table index for chi2 minimization */
    static const mcsUINT32 MAX_SPTYPE_INDEX = SPTYPE_MAX - SPTYPE_MIN + 1;

    // Check spectral type :
    mcsLOGICAL faint = mcsFALSE;
    mcsINT32 colorTableIndex = -1, colorTableDelta = -1;

    if (!forceFAINT)
    {
        vobsSTAR_PROPERTY* property;

        // Get index in color tables => spectral type index
        property = GetProperty(sclsvrCALIBRATOR_COLOR_TABLE_INDEX);
        if (IsPropertySet(property))
        {
            FAIL(GetPropertyValue(property, &colorTableIndex));
        }

        // Get delta in color tables => delta spectral type
        property = GetProperty(sclsvrCALIBRATOR_COLOR_TABLE_DELTA);
        if (IsPropertySet(property))
        {
            FAIL(GetPropertyValue(property, &colorTableDelta));
        }
    }

    if ((colorTableIndex == -1) || (colorTableDelta == -1))
    {
        // use FAINT approach (chi2 minimization)
        faint = mcsTRUE;

        colorTableIndex = (mcsINT32) round((SPTYPE_MAX + SPTYPE_MIN) / 2.0);
        colorTableDelta = (mcsINT32) ceil((SPTYPE_MAX - SPTYPE_MIN) / 2.0);

        logInfo("Using Faint approach with spType index in range [%u .. %u]",
                SPTYPE_MIN, SPTYPE_MAX);
    }

    mcsLOGICAL diamFlag = mcsFALSE;
    mcsUINT32 nbDiameters = 0;


    mcsUINT32 idxMin = max(SPTYPE_MIN, (mcsUINT32) (colorTableIndex - colorTableDelta));
    mcsUINT32 idxMax = min(SPTYPE_MAX, (mcsUINT32) (colorTableIndex + colorTableDelta));

    if (idxMin > idxMax)
    {
        logInfo("spectral type index = %u - delta = %u : Out of valid range [%u .. %u]",
                colorTableIndex, colorTableDelta, SPTYPE_MIN, SPTYPE_MAX);
    }
    else
    {
        // convert as double:
        mcsDOUBLE spTypeIndex = colorTableIndex;

        logInfo("spectral type index = %.1lf - delta = %d", spTypeIndex, colorTableDelta);

        // Structure to fill with diameters
        alxDIAMETERS diameters;
        alxDIAMETERS_COVARIANCE diametersCov;

        // Fill the magnitude structure
        alxMAGNITUDES mags;
        FAIL(ExtractMagnitudesAndFixErrors(mags, faint));

        // Compute diameters for spTypeIndex:
        mcsSTRING16 msg;
        sprintf(msg, "(SP %u)", (mcsUINT32) spTypeIndex);

        FAIL(alxComputeAngularDiameters(msg, mags, spTypeIndex, diameters, diametersCov));

        // average diameters:
        alxDATA meanDiam, chi2Diam;

        /* NO: may set low confidence to inconsistent diameters */
        FAIL(alxComputeMeanAngularDiameter(diameters, diametersCov, nbRequiredDiameters, &meanDiam,
                                           &chi2Diam, &nbDiameters, msgInfo.GetInternalMiscDYN_BUF()));


        /* handle uncertainty on spectral type */
        if (alxIsSet(meanDiam) && (colorTableDelta != 0))
        {
            /* sptype uncertainty */
            msgInfo.Reset();

            mcsUINT32 nSample = 0;
            mcsUINT32 sampleSpTypeIndex[MAX_SPTYPE_INDEX];
            mcsUINT32 nbDiametersSp[MAX_SPTYPE_INDEX];
            alxDIAMETERS diamsSp[MAX_SPTYPE_INDEX];

            // average diameters:
            alxDATA meanDiamSp[MAX_SPTYPE_INDEX], chi2DiamSp[MAX_SPTYPE_INDEX];

            logTest("Sampling spectral type range [%u .. %u]", idxMin, idxMax);

            mcsUINT32 index;

            for (index = idxMin; index <= idxMax; index++)
            {
                sprintf(msg, "(SP %u)", index);

                // Associate color table index to the current sample:
                sampleSpTypeIndex[nSample] = index;

                // convert as double:
                spTypeIndex = index;

                // Compute diameters for spectral type index:
                FAIL(alxComputeAngularDiameters   (msg, mags, spTypeIndex, diamsSp[nSample], diametersCov));

                FAIL(alxComputeMeanAngularDiameter(diamsSp[nSample], diametersCov, nbRequiredDiameters, &meanDiamSp[nSample],
                                                   &chi2DiamSp[nSample], &nbDiametersSp[nSample], NULL));

                if (alxIsSet(meanDiamSp[nSample]))
                {
                    // keep that sample:
                    nSample++;
                }
            }

            if (nSample != 0)
            {
                index = 0;

                mcsDOUBLE chi2, minChi2 = chi2DiamSp[0].value;

                if (nSample > 1)
                {
                    if (logValues)
                    {
                        printf("idx\tdmean\tedmean\tchi2\n");
                    }

                    /* Find minimum chi2 */
                    for (mcsUINT32 j = 1; j < nSample; j++)
                    {
                        chi2 = chi2DiamSp[j].value;

                        if (chi2 < minChi2)
                        {
                            index = j;
                            minChi2 = chi2;
                        }

                        if (logValues)
                        {
                            printf("%u\t%.4lf\t%.4lf\t%.4lf\n",
                                   sampleSpTypeIndex[j],
                                   meanDiamSp[j].value,
                                   meanDiamSp[j].error,
                                   chi2);
                        }
                    }
                }

                // Update values:
                mcsUINT32 fixedColorTableIndex = sampleSpTypeIndex[index];

                // adjust spType delta:
                mcsUINT32 colorTableIndexMin = fixedColorTableIndex;
                mcsUINT32 colorTableIndexMax = fixedColorTableIndex;

                msg[0] = '\0';
                sprintf(msg, "(SP %u) ", fixedColorTableIndex);

                // Update diameter info:
                if (faint)
                {
                    msgInfo.AppendString("[FAINT] ");
                }
                msgInfo.AppendString("MIN_CHI2 for ");
                msgInfo.AppendString(msg);

                // Copy diameters:
                for (mcsUINT32 j = 0; j < alxNB_DIAMS; j++)
                {
                    alxDATACopy(diamsSp[index][j], diameters[j]);
                }
                nbDiameters = nbDiametersSp[index];

                // Copy mean values:
                alxDATACopy(meanDiamSp[index], meanDiam);
                alxDATACopy(chi2DiamSp[index], chi2Diam);


                // Find min/max diameter where chi2 <= minChi2 + 1
                mcsDOUBLE diamMin, diamMax, bestDiam = meanDiam.value;
                diamMin = diamMax = bestDiam;
                mcsDOUBLE maxDiamErr = meanDiam.error / (bestDiam * LOG_10); // relative

                mcsDOUBLE selDiams[nSample];
                mcsUINT32 nSel = 0;

                if (nSample > 1)
                {
                    // chi2 < min_chi2 + delta
                    // where delta = 1 for 1 free parameter = (sptype index)
                    // As reduced_chi2 = chi2 / (nbDiam - 1)
                    // so chi2 / (nbDiam - 1) < (min_chi2 + 1) / (nbDiam - 1)
                    // ie reduced_chi2 < min_reduced_chi2 + 1 / (nbDiam - 1)
                    mcsDOUBLE chi2Th = alxMax(1.0, minChi2 + 1.0 / (nbDiameters - 1));
                    mcsDOUBLE diam, err;

                    // find all values below the chi2 threshold:
                    // ie chi2 < minChi2 + delta

                    for (mcsUINT32 j = 0; j < nSample; j++)
                    {
                        chi2 = chi2DiamSp[j].value;

                        if (chi2 <= chi2Th)
                        {
                            diam = meanDiamSp[j].value;
                            /* diameter is a log normal distribution */
                            selDiams[nSel++] = log10(diam);

                            err = meanDiamSp[j].error / (diam * LOG_10); // relative
                            if (err > maxDiamErr)
                            {
                                maxDiamErr = err;
                            }

                            if (diam < diamMin)
                            {
                                diamMin = diam;
                            }
                            if (diam > diamMax)
                            {
                                diamMax = diam;
                            }
                            if (sampleSpTypeIndex[j] < colorTableIndexMin)
                            {
                                colorTableIndexMin = sampleSpTypeIndex[j];
                            }
                            if (sampleSpTypeIndex[j] > colorTableIndexMax)
                            {
                                colorTableIndexMax = sampleSpTypeIndex[j];
                            }
                        }
                    }

                    // FAINT: check too large confidence area ?
                    if ((colorTableIndexMin == idxMin) || (colorTableIndexMax == idxMax))
                    {
                        logTest("Missing boundaries on confidence area (high magnitude errors or chi2 too small on the SP range)");
                    }

                    logInfo("Weighted mean diameters: %.5lf < %.5lf (%.4lf) < %.5lf - colorTableIndex: [%u to %u] - best chi2: %u == %.6lf",
                            diamMin, bestDiam, meanDiam.error, diamMax,
                            colorTableIndexMin, colorTableIndexMax,
                            fixedColorTableIndex, minChi2);
                }

                // adjust color index range:
                fixedColorTableIndex = (colorTableIndexMin + colorTableIndexMax) / 2;
                mcsUINT32 fixedColorTableDelta = (colorTableIndexMax - colorTableIndexMin) / 2;

                // Correct mean diameter:
                if (nSel > 0)
                {
                    /* diameter is a log normal distribution */
                    /* mean of sampled diameters */
                    mcsDOUBLE selDiamMean = alxMean(nSel, selDiams);
                    /* stddev of sampled diameters */
                    mcsDOUBLE selDiamErr  = alxRmsDistance(nSel, selDiams, selDiamMean); // relative

                    logTest("Diameter errors: stddev = %.4lf - max err = %.4lf (relative)", selDiamErr, maxDiamErr);

                    // variance = var(sampled diameters) + var(max mean diameter error) (relative):
                    selDiamErr = sqrt(selDiamErr * selDiamErr + maxDiamErr * maxDiamErr);

                    logTest("Fixed diameter error: %.4lf (relative)", selDiamErr);

                    /* Convert log normal diameter distribution to normal distribution */
                    selDiamMean = alxPow10(selDiamMean);
                    selDiamErr *= bestDiam * LOG_10;

                    logTest("Final Weighted mean diameter: %.4lf(%.4lf) instead of %.4lf(%.4lf)",
                            selDiamMean, selDiamErr, meanDiam.value, meanDiam.error);

                    meanDiam.value = selDiamMean;
                    meanDiam.error = selDiamErr;
                }

                miscDYN_BUF* diamInfo = msgInfo.GetInternalMiscDYN_BUF();

                if (faint)
                {
                    /* Set confidence to MEDIUM (FAINT) */
                    meanDiam.confIndex = alxCONFIDENCE_MEDIUM;
                }

                /* Check if chi2 < 5
                 * If higher i.e. inconsistency is found, the weighted mean diameter has a LOW confidence */
                if (minChi2 > DIAM_CHI2_THRESHOLD)
                {
                    /* Set confidence to LOW */
                    meanDiam.confIndex = alxCONFIDENCE_LOW;

                    if (IS_NOT_NULL(diamInfo))
                    {
                        /* Update diameter flag information */
                        miscDynBufAppendString(diamInfo, "INCONSISTENT_DIAMETERS ");
                    }
                }

                logInfo("Diameter weighted=%.4lf(%.4lf %.1lf%%) valid=%s [%s] chi2=%.4lf from %d diameters: %s",
                        meanDiam.value, meanDiam.error, alxDATALogRelError(meanDiam),
                        (meanDiam.confIndex == alxCONFIDENCE_HIGH) ? "true" : "false",
                        alxGetConfidenceIndex(meanDiam.confIndex),
                        chi2Diam.value,
                        nbDiameters,
                        msgInfo.GetBuffer());

                // Anyway update our spectral type:
                alxFixSpType(fixedColorTableIndex, fixedColorTableDelta, &_spectralType);

                if (IS_TRUE(_spectralType.isCorrected))
                {
                    // Update our decoded spectral type:
                    FAIL(SetPropertyValue(sclsvrCALIBRATOR_SP_TYPE_JMMC, _spectralType.ourSpType, vobsORIG_COMPUTED,
                                          vobsCONFIDENCE_HIGH, mcsTRUE));

                    // Set fixed index in color tables
                    FAIL(SetPropertyValue(sclsvrCALIBRATOR_COLOR_TABLE_INDEX_FIX, (mcsINT32) fixedColorTableIndex, vobsORIG_COMPUTED));
                    // Set fixed delta in color tables
                    FAIL(SetPropertyValue(sclsvrCALIBRATOR_COLOR_TABLE_DELTA_FIX, (mcsINT32) fixedColorTableDelta, vobsORIG_COMPUTED));
                }
            }
            else
            {
                logInfo("No sample in spectral type range [%u .. %u]", idxMin, idxMax);
            }
        }

        /* Write Diameters now as their confidence may have been lowered in alxComputeMeanAngularDiameter() */
        SetComputedPropWithError(sclsvrCALIBRATOR_DIAM_VJ, diameters[alxV_J_DIAM]);
        SetComputedPropWithError(sclsvrCALIBRATOR_DIAM_VH, diameters[alxV_H_DIAM]);
        SetComputedPropWithError(sclsvrCALIBRATOR_DIAM_VK, diameters[alxV_K_DIAM]);

        if alxIsSet(meanDiam)
        {
            // diamFlag OK if the mean diameter is computed:
            diamFlag = mcsTRUE;
        }

        // Write LD DIAMETER
        if (alxIsSet(meanDiam) && alxIsSet(chi2Diam))
        {
            /* diameter is a log normal distribution */
            mcsDOUBLE ldd = meanDiam.value;
            mcsDOUBLE e_ldd = meanDiam.error / (ldd * LOG_10); // relative

            // here we add 2% on relative error to take into account biases
            // unbiased variance = var(e_ldd) + var(bias) (relative):
            e_ldd = sqrt(e_ldd * e_ldd + (0.02 * 0.02));

            /* Convert log normal diameter distribution to normal distribution */
            e_ldd *= ldd * LOG_10;

            logTest("Corrected LD error=%.4lf (error=%.4lf, chi2=%.4lf)", e_ldd, meanDiam.error, chi2Diam.value);

            FAIL(SetPropertyValueAndError(sclsvrCALIBRATOR_LD_DIAM, ldd, e_ldd, vobsORIG_COMPUTED, (vobsCONFIDENCE_INDEX) meanDiam.confIndex));
        }

        // Write the chi2:
        if alxIsSet(chi2Diam)
        {
            FAIL(SetPropertyValue(sclsvrCALIBRATOR_DIAM_CHI2, chi2Diam.value, vobsORIG_COMPUTED, (vobsCONFIDENCE_INDEX) chi2Diam.confIndex));
        }
    }

    // Write DIAMETER COUNT
    FAIL(SetPropertyValue(sclsvrCALIBRATOR_DIAM_COUNT, (mcsINT32) nbDiameters, vobsORIG_COMPUTED));

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
    // If computed diameter is OK
    SUCCESS_COND_DO(IS_FALSE(IsDiameterOk()) || IS_FALSE(IsPropertySet(sclsvrCALIBRATOR_LD_DIAM)),
                    logTest("Unknown LD diameter or diameters are not OK; could not compute visibility"));

    mcsDOUBLE diam, diamError;

    // TODO FIXME: totally wrong => should use the UD diameter for the appropriate band (see Aspro2)
    // But move that code into SearchCal GUI instead.

    // Get the LD diameter and associated error value
    vobsSTAR_PROPERTY* property = GetProperty(sclsvrCALIBRATOR_LD_DIAM);
    FAIL(GetPropertyValueAndError(property, &diam, &diamError));

    // Get confidence index of computed diameter
    vobsCONFIDENCE_INDEX confidenceIndex = property->GetConfidenceIndex();

    // Get value in request of the wavelength
    mcsDOUBLE wavelength = request.GetObservingWlen();

    // Get value in request of the base max
    mcsDOUBLE baseMax = request.GetMaxBaselineLength();

    alxVISIBILITIES visibilities;
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
    FAIL(SetPropertyValue(sclsvrCALIBRATOR_SP_TYPE_JMMC, _spectralType.ourSpType, vobsORIG_COMPUTED));

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

        /* computed diameters */
        AddPropertyMeta(sclsvrCALIBRATOR_DIAM_VJ, "diam_vj", vobsFLOAT_PROPERTY, "mas",   "V-J Diameter");
        AddPropertyErrorMeta(sclsvrCALIBRATOR_DIAM_VJ_ERROR, "e_diam_vj", "mas", "Error on V-J Diameter");
        AddPropertyMeta(sclsvrCALIBRATOR_DIAM_VH, "diam_vh", vobsFLOAT_PROPERTY, "mas",   "V-H Diameter");
        AddPropertyErrorMeta(sclsvrCALIBRATOR_DIAM_VH_ERROR, "e_diam_vh", "mas", "Error on V-H Diameter");
        AddPropertyMeta(sclsvrCALIBRATOR_DIAM_VK, "diam_vk", vobsFLOAT_PROPERTY, "mas",   "V-K Diameter");
        AddPropertyErrorMeta(sclsvrCALIBRATOR_DIAM_VK_ERROR, "e_diam_vk", "mas", "Error on V-K Diameter");

        /* diameter count */
        AddPropertyMeta(sclsvrCALIBRATOR_DIAM_COUNT, "diam_count", vobsINT_PROPERTY, NULL, "Number of consistent and valid (high confidence) computed diameters");

        /* LD diameter */
        AddPropertyMeta(sclsvrCALIBRATOR_LD_DIAM, "LDD", vobsFLOAT_PROPERTY, "mas", "Limb-darkened diameter");
        AddPropertyErrorMeta(sclsvrCALIBRATOR_LD_DIAM_ERROR, "e_LDD", "mas", "Error on limb-darkened diameter");

        /* chi2 of the weighted mean diameter estimation */
        AddFormattedPropertyMeta(sclsvrCALIBRATOR_DIAM_CHI2, "diam_chi2", vobsFLOAT_PROPERTY, NULL, "%.4lf",
                                 "Reduced chi-square of the LDD diameter estimation");

        /* diameter flag (true | false) */
        AddPropertyMeta(sclsvrCALIBRATOR_DIAM_FLAG, "diamFlag", vobsBOOL_PROPERTY, NULL, "Diameter Flag (true means the LDD diameter is computed)");
        /* information about the diameter computation */
        AddPropertyMeta(sclsvrCALIBRATOR_DIAM_FLAG_INFO, "diamFlagInfo", vobsSTRING_PROPERTY, NULL, "Information related to the LDD diameter estimation");

        if (sclsvrCALIBRATOR_PERFORM_SED_FITTING)
        {
            /* Results from SED fitting */
            AddPropertyMeta(sclsvrCALIBRATOR_SEDFIT_CHI2, "chi2_SED", vobsFLOAT_PROPERTY, NULL, "Reduced chi2 of the SED fitting (experimental)");
            AddPropertyMeta(sclsvrCALIBRATOR_SEDFIT_DIAM, "diam_SED", vobsFLOAT_PROPERTY, "mas", "Diameter from SED fitting (experimental)");
            AddPropertyErrorMeta(sclsvrCALIBRATOR_SEDFIT_DIAM_ERROR, "e_diam_SED", "mas", "Diameter from SED fitting (experimental)");
            AddPropertyMeta(sclsvrCALIBRATOR_SEDFIT_TEFF, "Teff_SED", vobsFLOAT_PROPERTY, "K", "Teff from SED fitting (experimental)");
            AddPropertyMeta(sclsvrCALIBRATOR_SEDFIT_AV, "Av_SED", vobsFLOAT_PROPERTY, NULL, "Av from SED fitting (experimental)");
        }

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

        /* square visibility */
        AddPropertyMeta(sclsvrCALIBRATOR_VIS2, "vis2", vobsFLOAT_PROPERTY, NULL, "Squared Visibility");
        AddPropertyErrorMeta(sclsvrCALIBRATOR_VIS2_ERROR, "vis2Err", NULL, "Error on Squared Visibility");

        /* distance to the science object */
        AddPropertyMeta(sclsvrCALIBRATOR_DIST, "dist", vobsFLOAT_PROPERTY, "deg", "Calibrator to Science object Angular Distance");

        /* index in color tables */
        AddPropertyMeta(sclsvrCALIBRATOR_COLOR_TABLE_INDEX, "color_table_index", vobsINT_PROPERTY, NULL, "(internal) index in color tables");

        /* delta in color tables */
        AddPropertyMeta(sclsvrCALIBRATOR_COLOR_TABLE_DELTA, "color_table_delta", vobsINT_PROPERTY, NULL, "(internal) delta in color tables");

        /* fixed index in color tables */
        AddPropertyMeta(sclsvrCALIBRATOR_COLOR_TABLE_INDEX_FIX, "color_table_index_fix", vobsINT_PROPERTY, NULL, "(internal) fixed index in color tables");

        /* fixed delta in color tables */
        AddPropertyMeta(sclsvrCALIBRATOR_COLOR_TABLE_DELTA_FIX, "color_table_delta_fix", vobsINT_PROPERTY, NULL, "(internal) fixed delta in color tables");

        /* luminosity class (min) */
        AddPropertyMeta(sclsvrCALIBRATOR_LUM_CLASS, "lum_class", vobsINT_PROPERTY, NULL, "(internal) (min) luminosity class from spectral type (1,2,3,4,5)");
        /* luminosity class delta (lc_max = lum_class + lum_class_delta) */
        AddPropertyMeta(sclsvrCALIBRATOR_LUM_CLASS_DELTA, "lum_class_delta", vobsINT_PROPERTY, NULL, "(internal) luminosity class delta (lc_max = lum_class + lum_class_delta)");

        /* corrected spectral type */
        AddPropertyMeta(sclsvrCALIBRATOR_SP_TYPE_JMMC, "SpType_JMMC", vobsSTRING_PROPERTY, NULL, "Corrected spectral type by the JMMC");

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

