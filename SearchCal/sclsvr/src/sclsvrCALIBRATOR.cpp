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

/* maximum number of properties (141) */
#define sclsvrCALIBRATOR_MAX_PROPERTIES  (alxIsDevFlag() ? 141 : 123)

/* Error identifiers */
#define sclsvrCALIBRATOR_PHOT_COUS_J_ERROR  "PHOT_COUS_J_ERROR"
#define sclsvrCALIBRATOR_PHOT_COUS_H_ERROR  "PHOT_COUS_H_ERROR"
#define sclsvrCALIBRATOR_PHOT_COUS_K_ERROR  "PHOT_COUS_K_ERROR"

#define sclsvrCALIBRATOR_DIAM_VJ_ERROR      "DIAM_VJ_ERROR"
#define sclsvrCALIBRATOR_DIAM_VH_ERROR      "DIAM_VH_ERROR"
#define sclsvrCALIBRATOR_DIAM_VK_ERROR      "DIAM_VK_ERROR"

#define sclsvrCALIBRATOR_LD_DIAM_ERROR      "LD_DIAM_ERROR"
#define sclsvrCALIBRATOR_LD_DIAM_ERROR_REL  "LD_DIAM_ERROR_REL"

#define sclsvrCALIBRATOR_SEDFIT_DIAM_ERROR  "SEDFIT_DIAM_ERROR"

#define sclsvrCALIBRATOR_EXTINCTION_RATIO_ERROR "EXTINCTION_RATIO_ERROR"

#define sclsvrCALIBRATOR_DIST_FIT_ERROR     "DIST_FIT_ERROR"

#define sclsvrCALIBRATOR_DIST_PLX_ERROR     "DIST_PLX_ERROR"

#define sclsvrCALIBRATOR_VIS2_ERROR         "VIS2_ERROR"

// Same thresholds as IDL:
#define sclsvrCALIBRATOR_EMAG_MIN           0.01
#define sclsvrCALIBRATOR_EMAG_MAX           0.35

/**
 * Convenience macros
 */
#define SetComputedPropWithError(propId, alxDATA) \
if (alxIsSet(alxDATA))                            \
{                                                 \
    FAIL(SetPropertyValueAndError(propId, alxDATA.value, alxDATA.error, vobsORIG_COMPUTED, (vobsCONFIDENCE_INDEX) alxDATA.confIndex)); \
}

#define IsMagInValidRange(mag) \
  ((mag >= -2.0) && (mag <= 20.0))

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
sclsvrCALIBRATOR::sclsvrCALIBRATOR() : vobsSTAR(sclsvrCALIBRATOR_MAX_PROPERTIES)
{
    // Note: vobsSTAR() explicit constructor adds star properties
    this->vobsSTAR::AddProperties();

    // Add calibrator star properties
    AddProperties();
}

/**
 * Conversion Constructor.
 */
sclsvrCALIBRATOR::sclsvrCALIBRATOR(const vobsSTAR &star) : vobsSTAR(sclsvrCALIBRATOR_MAX_PROPERTIES)
{
    // apply vobsSTAR assignment operator between this and given star:
    // note: this includes copy of calibrator properties
    this->vobsSTAR::operator=(star);

    // Add calibrator star properties
    AddProperties();
}

/**
 * Copy Constructor.
 */
sclsvrCALIBRATOR::sclsvrCALIBRATOR(const sclsvrCALIBRATOR& star) : vobsSTAR(sclsvrCALIBRATOR_MAX_PROPERTIES)
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

    // Compute UD from LD and Teff (SP)
    FAIL(ComputeUDFromLDAndSP());

    // Define CalFlag
    FAIL(DefineCalFlag(IS_TRUE(request.IsBright())));

    // Compute absorption coefficient Av and may correct luminosity class (ie SpType)
    FAIL(ComputeExtinctionCoefficient());

    FAIL(ComputeSedFitting());

    // Compute I, J, H, K COUSIN magnitude from Johnson catalogues
    FAIL(ComputeCousinMagnitudes());

    // Compute missing Magnitude (information only)
    if (isPropSet(sclsvrCALIBRATOR_EXTINCTION_RATIO))
    {
        FAIL(ComputeMissingMagnitude());
    }
    else
    {
        logTest("Av is unknown; do not compute missing magnitude");
    }

    // Compute J, H, K JOHNSON magnitude (2MASS) from COUSIN
    FAIL(ComputeJohnsonMagnitudes());

    // Fill in the Teff and LogG entries using the spectral type
    FAIL(ComputeTeffLogg());

    // Compute N Band and S_12 with AKARI from Teff
    FAIL(ComputeIRFluxes());

    if (IS_FALSE(request.IsJSDCMode()))
    {
        // Compute visibility and visibility error only if diameter OK
        FAIL(ComputeVisibility(request));

        // Compute distance
        FAIL(ComputeDistance(request));
    }

    if (IS_FALSE(request.IsDiagnose()) && IS_FALSE(request.IsJSDCMode()))
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
 * Define the CalFlag property
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.;
 */
mcsCOMPL_STAT sclsvrCALIBRATOR::DefineCalFlag(const bool bright)
{
    mcsINT32 calFlag = 0;

    /* bit 0: chi2 > 5 */
    if (isPropSet(sclsvrCALIBRATOR_DIAM_CHI2))
    {
        mcsDOUBLE chi2;
        FAIL(GetPropertyValue(sclsvrCALIBRATOR_DIAM_CHI2, &chi2));

        /* Check if chi2 > 5 */
        if (chi2 > DIAM_CHI2_THRESHOLD)
        {
            logTest("DefineCalFlag: bit 0");
            calFlag |= 1;
        }
        else
        {
            if (bright)
            {
                // Bright mode:
                // check color table index & delta:
                mcsINT32 colorTableIndex = -1, colorTableDelta = -1;

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
                // discard faint approach: no sp_type or high uncertainty (more than 1 SP class)
                if ((colorTableIndex <= 0) || (colorTableDelta > 20))
                {
                    logTest("DefineCalFlag: bit 0 (bright)");
                    calFlag |= 1;
                }
            }
        }
    }

    /* bit 1: binary (SBC9 or WDS) */
    if (isPropSet(vobsSTAR_ID_SB9))
    {
        logTest("DefineCalFlag: bit 1 (SB9)");
        calFlag |= 2;
    }
    else if (isPropSet(vobsSTAR_ID_WDS))
    {
        mcsDOUBLE sep1 = 1e9, sep2 = 1e9;

        if (isPropSet(vobsSTAR_ORBIT_SEPARATION_SEP1))
        {
            FAIL(GetPropertyValue(vobsSTAR_ORBIT_SEPARATION_SEP1, &sep1));
        }
        if (isPropSet(vobsSTAR_ORBIT_SEPARATION_SEP2))
        {
            FAIL(GetPropertyValue(vobsSTAR_ORBIT_SEPARATION_SEP2, &sep2));
        }
        // discard negative values:
        if (sep1 < 0.0) {
            sep1 = 1e9;
        }
        if (sep2 < 0.0) {
            sep2 = 1e9;
        }
        mcsDOUBLE minSep = alxMin(sep1, sep2);
        if (minSep <= 2.0)
        {
            logTest("DefineCalFlag: bit 1 (WDS) - sep = %.3lf", minSep);
            calFlag |= 2;
        }
    }

    /* Generated from ObjectTypes_2017.ods */
    static const char* const FILTER_SIMBAD_OBJTYPES[]
            = { ",EB?,", ",Sy?,", ",CV?,", ",No?,", ",pr?,", ",TT?,", ",C*?,",
               ",S*?,", ",OH?,", ",CH?,", ",WR?,", ",Ae?,", ",RR?,", ",Ce?,", ",LP?,", ",Mi?,", ",sv?,", ",pA?,", ",WD?,", ",N*?,",
               ",BH?,", ",SN?,", ",BD?,", ",EB*,", ",Al*,", ",bL*,", ",WU*,", ",EP*,", ",SB*,", ",El*,", ",Sy*,", ",CV*,", ",NL*,",
               ",No*,", ",DN*,", ",Ae*,", ",C*,", ",S*,", ",pA*,", ",WD*,", ",ZZ*,", ",BD*,", ",N*,", ",OH*,", ",CH*,", ",pr*,",
               ",TT*,", ",WR*,", ",Ir*,", ",Or*,", ",RI*,", ",Er*,", ",FU*,", ",RC*,", ",RC?,", ",Psr,", ",BY*,", ",RS*,", ",Pu*,",
               ",RR*,", ",Ce*,", ",dS*,", ",RV*,", ",WV*,", ",bC*,", ",cC*,", ",gD*,", ",SX*,", ",LP*,", ",Mi*,", ",sr*,", ",SN*," };
    static const int N_OBJTYPES = sizeof (FILTER_SIMBAD_OBJTYPES) / sizeof (char*);

    /* bit 2: bad object type */
    if (isPropSet(vobsSTAR_OBJ_TYPES))
    {
        const char* objTypes = GetPropertyValue(vobsSTAR_OBJ_TYPES);

        for (int i = 0; i < N_OBJTYPES; i++)
        {
            const char* otype = FILTER_SIMBAD_OBJTYPES[i];

            if (IS_NOT_NULL(strstr(objTypes, otype)))
            {
                logTest("DefineCalFlag: bit 2 (OTYPES) - '%s' found in objTypes [%s]", otype, objTypes);
                calFlag |= 4;
                break;
            }
        }
    }

    if (isPropSet(GetXmMainFlagProperty()))
    {
        mcsINT32 flags = 0;
        FAIL(GetXmMainFlagProperty()->GetValue(&flags));

        if (flags != 0)
        {
            logTest("DefineCalFlag: bit 3 (XMATCH_MAIN_FLAG)");
            calFlag |= 8;
        }
    }


    // Set CalFlag property
    FAIL(SetPropertyValue(sclsvrCALIBRATOR_CAL_FLAG, calFlag, vobsORIG_COMPUTED, vobsCONFIDENCE_HIGH));

    return mcsSUCCESS;
}

/**
 * Clean up useless properties
 */
void sclsvrCALIBRATOR::CleanProperties()
{
    logDebug("CleanProperties");

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
                                    sclsvrCALIBRATOR_PHOT_COUS_J,
                                    sclsvrCALIBRATOR_PHOT_COUS_H,
                                    sclsvrCALIBRATOR_PHOT_COUS_K,

                                    sclsvrCALIBRATOR_DIAM_VJ,
                                    sclsvrCALIBRATOR_DIAM_VH,
                                    sclsvrCALIBRATOR_DIAM_VK,

                                    sclsvrCALIBRATOR_DIAM_COUNT,
                                    sclsvrCALIBRATOR_DIAM_FLAG_INFO,

                                    /* av */
                                    sclsvrCALIBRATOR_EXTINCTION_RATIO,
                                    sclsvrCALIBRATOR_AV_FIT_CHI2,
                                    sclsvrCALIBRATOR_DIST_PLX,
                                    sclsvrCALIBRATOR_DIST_FIT,
                                    sclsvrCALIBRATOR_DIST_FIT_CHI2,

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

            // check validity range
            if (IsMagInValidRange(mag))
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
                // very bright stars
                magnitudes[band].error = (magnitudes[band].value <= 3.0) ? 0.1 : 0.04;
            }
            else if (magnitudes[band].error < sclsvrCALIBRATOR_EMAG_MIN)
            {
                // Fix error lower limit to 0.01 mag (BV only):
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
            if (alxIsSet(magnitudes[band]) && !isnan(magnitudes[band].error))
            {
                magnitudes[band].error = maxErr;
            }
        }
    }

    mcsDOUBLE emagMax   = sclsvrCALIBRATOR_EMAG_MAX;

    if (IS_TRUE(faint))
    {
        // avoid too large magnitude error to have chi2 more discrimmative:
        emagMax = 0.25;
    }

    // Fix error (upper limit) for magnitudes (B..N):
    for (mcsUINT32 band = alxB_BAND; band <= alxN_BAND; band++)
    {
        if (alxIsSet(magnitudes[band]))
        {
            if (!isnan(magnitudes[band].error) && magnitudes[band].error > emagMax)
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
 * Compute missing magnitude.
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
 */
mcsCOMPL_STAT sclsvrCALIBRATOR::ComputeMissingMagnitude()
{
    logTest("sclsvrCALIBRATOR::ComputeMissingMagnitude()");

    // Magnitudes to be used
    // PHOT_COUS bands should have been prepared before.
    static const char* magIds[alxNB_BANDS] = {
                                              vobsSTAR_PHOT_JHN_B,
                                              vobsSTAR_PHOT_JHN_V,
                                              vobsSTAR_PHOT_JHN_R,
                                              vobsSTAR_PHOT_COUS_I,
                                              sclsvrCALIBRATOR_PHOT_COUS_J,
                                              sclsvrCALIBRATOR_PHOT_COUS_H,
                                              sclsvrCALIBRATOR_PHOT_COUS_K,
                                              vobsSTAR_PHOT_JHN_L,
                                              vobsSTAR_PHOT_JHN_M,
                                              vobsSTAR_PHOT_JHN_N
    };

    alxMAGNITUDES magnitudes;
    FAIL(ExtractMagnitudes(magnitudes, magIds, DEF_MAG_ERROR));

    /* Print out results */
    alxLogTestMagnitudes("Initial magnitudes:", "", magnitudes);

    // Note: do not check confidence on Av

    // Get the extinction ratio
    mcsDOUBLE Av;
    FAIL(GetPropertyValue(sclsvrCALIBRATOR_EXTINCTION_RATIO, &Av));

    // Compute corrected magnitude
    // (remove the expected interstellar absorption)
    FAIL(alxComputeCorrectedMagnitudes("(Av)", Av, magnitudes, mcsTRUE));

    // Compute missing magnitudes
    // TRY both methods to be sure to get missing magnitudes
    FAIL(alxComputeMagnitudesForBrightStar(&_spectralType, magnitudes));
    FAIL(alxComputeMagnitudesForFaintStar(&_spectralType, magnitudes));

    // Compute apparent magnitude (apply back interstellar absorption)
    FAIL(alxComputeApparentMagnitudes(Av, magnitudes));

    // Set back the computed magnitude. Already existing magnitudes are not overwritten.
    for (mcsUINT32 band = alxB_BAND; band < alxNB_BANDS; band++)
    {
        // check validity range
        if (alxIsSet(magnitudes[band]) && IsMagInValidRange(magnitudes[band].value))
        {
            FAIL(SetPropertyValue(magIds[band], magnitudes[band].value, vobsORIG_COMPUTED,
                                  (vobsCONFIDENCE_INDEX) magnitudes[band].confIndex, mcsFALSE));
        }
    }

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

    // Compute distance from parallax:
    vobsSTAR_PROPERTY* property = GetProperty(vobsSTAR_POS_PARLX_TRIG);

    // If parallax of the star if known
    if (isPropSet(property))
    {
        mcsDOUBLE plx;

        // Check parallax
        FAIL(GetPropertyValue(property, &plx));

        // Get error
        if (isErrorSet(property))
        {
            mcsDOUBLE e_plx = -1.0;
            FAIL(GetPropertyError(property, &e_plx));

            // If parallax is negative
            if (plx <= 0.0)
            {
                logTest("parallax %.2lf(%.2lf) is not valid...", plx, e_plx);
            }
            else if (plx < 1.0)
            {
                // If parallax is less than 1 mas
                logTest("parallax %.2lf(%.2lf) less than 1 mas...", plx, e_plx);
            }
            else if (e_plx <= 0.0)
            {
                // If parallax error is invalid
                logTest("parallax error %.2lf is not valid...", e_plx);
            }
            else if ((e_plx / plx) > 1.001)
            {
                // If parallax error is too high > 100%
                logTest("parallax %.2lf(%.2lf) is not valid...", plx, e_plx);
            }
            else
            {
                // parallax OK
                logTest("parallax %.2lf(%.2lf) is valid...", plx, e_plx);

                /*
                 * Compute distance and its error in parsecs
                 * dist = 1 / plx
                 * var(dist) = dist^4 x var(plx) = e_plx^2 / plx^4
                 */
                dist_plx   = 1000.0         / plx;            /* pc */
                e_dist_plx = 1000.0 * e_plx / alxSquare(plx); /* pc */

                logTest("Dist(plx)=%.4lf (%.4lf)", dist_plx, e_dist_plx);

                // Set distance computed from parallax and error
                FAIL(SetPropertyValueAndError(sclsvrCALIBRATOR_DIST_PLX, dist_plx, e_dist_plx, vobsORIG_COMPUTED));
            }
        }
        else
        {
            // If parallax error is unknown
            logDebug("parallax error is unknown...");
        }
    }

    // chi2 threshold to use low confidence:
    static mcsDOUBLE BAD_CHI2_THRESHOLD = 5.0; /* ~2 sigma */

    // Fill the magnitude structure
    alxMAGNITUDES mags;
    FAIL(ExtractMagnitudesAndFixErrors(mags));

    // Get Star ID
    mcsSTRING64 starId;
    FAIL(GetId(starId, sizeof (starId)));

    mcsDOUBLE av_fit, e_av_fit, dist_fit, e_dist_fit, chi2_fit, chi2_dist;

    /* minimum value for delta on spectral quantity */
    /* 2017: disable adjusting spType at all */
    mcsLOGICAL useDeltaQuantity = mcsFALSE;
    mcsDOUBLE minDeltaQuantity = 0.0;

    /* initialize fit confidence to high */
    vobsCONFIDENCE_INDEX avFitConfidence = vobsCONFIDENCE_HIGH;
    vobsCONFIDENCE_INDEX distFitConfidence = vobsCONFIDENCE_HIGH;

    /* SpType has a precise luminosity class ? */
    mcsLOGICAL hasLumClass = (_spectralType.otherStarType != alxSTAR_UNDEFINED) ? mcsTRUE : mcsFALSE;

    // compute Av from spectral type and given magnitudes
    if (alxComputeAvFromMagnitudes(starId, dist_plx, e_dist_plx, &av_fit, &e_av_fit, &dist_fit, &e_dist_fit,
                                   &chi2_fit, &chi2_dist, mags, &_spectralType,
                                   minDeltaQuantity, hasLumClass, useDeltaQuantity) == mcsSUCCESS)
    {
        logTest("ComputeExtinctionCoefficient: (fit) Av=%.4lf (%.4lf) distance=%.4lf (%.4lf) chi2=%.4lf chi2Dist=%.4lf",
                av_fit, e_av_fit, dist_fit, e_dist_fit, chi2_fit, chi2_dist);

        // check if chi2 is not too high (maybe wrong spectral type)
        if (!isnan(chi2_fit) && (chi2_fit > BAD_CHI2_THRESHOLD))
        {
            logInfo("ComputeExtinctionCoefficient: bad chi2 for Av=%.4lf (%.4lf) chi2=%.4lf",
                    av_fit, e_av_fit, chi2_fit);

            /* use low confidence for high chi2 */
            avFitConfidence = vobsCONFIDENCE_LOW;
        }
        if (!isnan(chi2_dist) && (chi2_dist > BAD_CHI2_THRESHOLD))
        {
            logInfo("ComputeExtinctionCoefficient: bad chi2 for distance=%.4lf (%.4lf) chi2Dist=%.4lf",
                    dist_fit, e_dist_fit, chi2_dist);

            /* use low confidence for high chi2 */
            distFitConfidence = vobsCONFIDENCE_LOW;
        }
        /*
                if (IS_TRUE(_spectralType.isCorrected))
                {
                    // Update our decoded spectral type:
                    FAIL(SetPropertyValue(sclsvrCALIBRATOR_SP_TYPE_JMMC, _spectralType.ourSpType, vobsORIG_COMPUTED, vobsCONFIDENCE_HIGH, mcsTRUE));
                }
         */
        // Set extinction ratio and error (best)
        if (!isnan(av_fit))
        {
            // Set extinction ratio and error
            FAIL(SetPropertyValueAndError(sclsvrCALIBRATOR_EXTINCTION_RATIO, av_fit, e_av_fit, vobsORIG_COMPUTED, avFitConfidence));
        }

        if (!isnan(chi2_fit))
        {
            // Set chi2 of the fit
            FAIL(SetPropertyValue(sclsvrCALIBRATOR_AV_FIT_CHI2, chi2_fit, vobsORIG_COMPUTED, avFitConfidence));
        }

        if (!isnan(dist_fit))
        {
            // Set fitted distance and error
            FAIL(SetPropertyValueAndError(sclsvrCALIBRATOR_DIST_FIT, dist_fit, e_dist_fit, vobsORIG_COMPUTED, distFitConfidence));
        }

        if (!isnan(chi2_dist))
        {
            // Set chi2 of the distance modulus
            FAIL(SetPropertyValue(sclsvrCALIBRATOR_DIST_FIT_CHI2, chi2_dist, vobsORIG_COMPUTED, distFitConfidence));
        }
    }

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
    if (!alxIsDevFlag() || !sclsvrCALIBRATOR_PERFORM_SED_FITTING)
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

    // Check confidence on Av:
    if (isPropSet(sclsvrCALIBRATOR_EXTINCTION_RATIO)
            && (GetPropertyConfIndex(sclsvrCALIBRATOR_EXTINCTION_RATIO) == vobsCONFIDENCE_HIGH))
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

    static const bool forceFAINT = false;
    static const bool forceNoDelta = false && !forceFAINT;
    static const bool logValues = false;

    // Enforce using polynom domain:
    // TODO: externalize such values
    /*
        #FIT COLORS:  V-J V-H V-K
        #domain:       42.000000       272.00000
     */
    /* Note: it is forbidden to extrapolate polynoms: may diverge strongly ! */
    static const mcsUINT32 SPTYPE_MIN = 40;  // B0
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
        if (forceNoDelta)
        {
            colorTableDelta = 0;
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

        FAIL(alxComputeAngularDiameters(msg, mags, spTypeIndex, diameters, diametersCov, logTEST));

        // average diameters:
        alxDATA meanDiam, chi2Diam;

        /* NOTE: may set low confidence to inconsistent diameters */
        FAIL(alxComputeMeanAngularDiameter(diameters, diametersCov, nbRequiredDiameters, &meanDiam,
                                           &chi2Diam, &nbDiameters, msgInfo.GetInternalMiscDYN_BUF(), logTEST));


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
                FAIL(alxComputeAngularDiameters   (msg, mags, spTypeIndex, diamsSp[nSample], diametersCov, logDEBUG));

                FAIL(alxComputeMeanAngularDiameter(diamsSp[nSample], diametersCov, nbRequiredDiameters, &meanDiamSp[nSample],
                                                   &chi2DiamSp[nSample], &nbDiametersSp[nSample], NULL, logDEBUG));

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
                    // As reduced_chi2 = chi2 / nbDiam
                    // so chi2 / nbDiam < (min_chi2 + 1) / nbDiam
                    // ie reduced_chi2 < min_reduced_chi2 + 1 / nbDiam
                    mcsDOUBLE chi2Th = alxMax(1.0, minChi2 + 1.0 / nbDiameters);
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
                        // high magnitude errors or chi2 too small on the SP range:
                        logTest("Missing boundaries on the confidence area for chi2 threshold = %.6lf", chi2Th);
                    }

                    logInfo("Weighted mean diameters: %.5lf < %.5lf (%.4lf) < %.5lf - colorTableIndex: [%u to %u] - best chi2: %u == %.6lf",
                            diamMin, bestDiam, meanDiam.error, diamMax,
                            colorTableIndexMin, colorTableIndexMax,
                            fixedColorTableIndex, minChi2);

                    /* fix min chi2 as chi2 threshold */
                    chi2Diam.value = minChi2 = chi2Th;
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

                    logDebug("Sampled diameters: stddev = %.4lf - max err = %.4lf (relative)", selDiamErr, maxDiamErr);

                    // variance = var(sampled diameters) + var(max diameter error) (relative):
                    selDiamErr = sqrt(selDiamErr * selDiamErr + maxDiamErr * maxDiamErr);

                    logDebug("Fixed diameter error: %.4lf (relative)", selDiamErr);

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

        if (alxIsSet(meanDiam))
        {
            // diamFlag OK if the mean diameter is computed:
            diamFlag = mcsTRUE;
        }

        // Write LD DIAMETER
        if (alxIsSet(meanDiam) && alxIsSet(chi2Diam))
        {
            /* diameter is a log normal distribution */
            mcsDOUBLE ldd = meanDiam.value;
            mcsDOUBLE e_ldd = meanDiam.error / ldd; // relative but no log(10)

            // here we add 2% on relative error to take into account biases
            // unbiased variance = var(e_ldd) + var(bias) (relative):
            e_ldd = sqrt((e_ldd * e_ldd) + (0.02 * 0.02));

            // set relative LDD error:
            mcsDOUBLE e_ldd_rel = 100. * e_ldd;

            FAIL(SetPropertyValue(sclsvrCALIBRATOR_LD_DIAM_ERROR_REL, e_ldd_rel, vobsORIG_COMPUTED, (vobsCONFIDENCE_INDEX) meanDiam.confIndex));

            e_ldd *= ldd;

            logTest("Corrected LD error=%.4lf (%.1lf %)(error=%.4lf, chi2=%.4lf)", e_ldd, e_ldd_rel, meanDiam.error, chi2Diam.value);

            FAIL(SetPropertyValueAndError(sclsvrCALIBRATOR_LD_DIAM, ldd, e_ldd, vobsORIG_COMPUTED, (vobsCONFIDENCE_INDEX) meanDiam.confIndex));
        }

        // Write the chi2:
        if (alxIsSet(chi2Diam))
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

    // Does LDD exist
    SUCCESS_FALSE_DO(IsPropertySet(property), logTest("Compute UD - Skipping (LD_DIAM unknown)."));

    mcsDOUBLE ld = NAN;
    SUCCESS_DO(GetPropertyValue(property, &ld), logWarning("Compute UD - Aborting (error while retrieving LD_DIAM)."));

    // Get LD diameter confidence index (UDs will have the same one)
    vobsCONFIDENCE_INDEX ldConfIndex = property->GetConfidenceIndex();

    // Get index in color tables => spectral type index
    mcsINT32 colorTableIndex = -1;
    mcsINT32 lumClass = -1;

    // use Fixed (faint or unprecise):
    property = GetProperty(sclsvrCALIBRATOR_COLOR_TABLE_INDEX_FIX);
    if (IsPropertySet(property))
    {
        FAIL(GetPropertyValue(property, &colorTableIndex));
    }
    else
    {
        property = GetProperty(sclsvrCALIBRATOR_COLOR_TABLE_INDEX);
        if (IsPropertySet(property))
        {
            FAIL(GetPropertyValue(property, &colorTableIndex));
        }
    }
    SUCCESS_COND_DO((colorTableIndex == -1), logWarning("Compute UD - Aborting (no color table index)."));

    // Get luminosity class
    property = GetProperty(sclsvrCALIBRATOR_LUM_CLASS);
    if (IsPropertySet(property))
    {
        FAIL(GetPropertyValue(property, &lumClass));
    }

    // Compute UD
    logTest("Computing UDs for LD=%.4lf at idx=%d lc=%d", ld, colorTableIndex, lumClass);

    alxUNIFORM_DIAMETERS ud;

    SUCCESS_DO(alxComputeNewUDFromLDAndSP(ld, colorTableIndex, lumClass, &ud), logWarning("Aborting (error while computing UDs)."));

    // Set Teff eand LogG properties
    FAIL(SetPropertyValue(sclsvrCALIBRATOR_TEFF_SPTYP, ud.Teff, vobsORIG_COMPUTED));
    FAIL(SetPropertyValue(sclsvrCALIBRATOR_LOGG_SPTYP, ud.LogG, vobsORIG_COMPUTED));

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

    // Get value in request of the wavelength
    mcsDOUBLE wavelength = request.GetObservingWlen();

    // Get value in request of the base max
    mcsDOUBLE baseMax = request.GetMaxBaselineLength();

    if ((wavelength > 0.0) && (baseMax > 0.0))
    {
        mcsDOUBLE diam, diamError;

        // TODO FIXME: should use the UD diameter for the appropriate band (see Aspro2)
        // But move that code into SearchCal GUI instead.

        // Get the LD diameter and associated error value
        vobsSTAR_PROPERTY* property = GetProperty(sclsvrCALIBRATOR_LD_DIAM);
        FAIL(GetPropertyValueAndError(property, &diam, &diamError));

        // Get confidence index of computed diameter
        vobsCONFIDENCE_INDEX confidenceIndex = property->GetConfidenceIndex();

        alxVISIBILITIES visibilities;
        FAIL(alxComputeVisibility(diam, diamError, baseMax, wavelength, &visibilities));

        // Affect visibility property
        FAIL(SetPropertyValueAndError(sclsvrCALIBRATOR_VIS2, visibilities.vis2, visibilities.vis2Error, vobsORIG_COMPUTED, confidenceIndex));
    }

    return mcsSUCCESS;
}

/**
 * Compute distance.
 *
 * This method calculate the distance in degree between the calibrator and the
 * science object.
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
 */
mcsCOMPL_STAT sclsvrCALIBRATOR::ComputeDistance(const sclsvrREQUEST &request)
{
    SUCCESS_COND(IS_FALSE(request.hasObjectRaDec()));

    // Get science object right ascension in degrees
    mcsDOUBLE scienceObjectRa = request.GetObjectRaInDeg();

    // Get science object declination in degrees
    mcsDOUBLE scienceObjectDec = request.GetObjectDecInDeg();

    // Get the internal calibrator right ascension / declination in degrees
    mcsDOUBLE calibratorRa, calibratorDec;
    FAIL(GetRaDec(calibratorRa, calibratorDec));

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
        if (isCatalog2Mass(oriK))
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
        else if (isPropSet(magV) && isCatalogPhoto(oriK))
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

                if (isCatalog2Mass(oriH) && isCatalog2Mass(oriK))
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
                else if (isCatalogPhoto(oriH) && isCatalogPhoto(oriK))
                {
                    // From (H-K) LBSI / PHOTO, we assume H and K in Johnson magnitude
                    // see Bessel, p.1138
                    mHc = mKc - 0.009 + 0.912 * (mH - mK);
                    eHc = eH;
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

                if (isCatalog2Mass(oriJ) && isCatalog2Mass(oriK))
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
                    eJc = eJ;
                    oriJc = oriJ;
                }
                else if (isCatalogPhoto(oriJ) && isCatalogPhoto(oriK))
                {
                    // From (J-K) LBSI / PHOTO, we assume H and K in Johnson magnitude
                    // see Bessel p.1136  This seems quite unprecise.
                    mJc = mKc + 0.93 * (mJ - mK);
                    eJc = eJ;
                    oriJc = oriJ;
                }
                else
                {
                    logInfo("Jc not computed: unsupported origins J (%s) K (%s)", vobsGetOriginIndex(oriJ), vobsGetOriginIndex(oriK));
                }
            }

            // Set the magnitudes and errors:
            FAIL(SetPropertyValueAndError(sclsvrCALIBRATOR_PHOT_COUS_K, mKc, eKc, oriKc));

            if (!isnan(mHc))
            {
                FAIL(SetPropertyValueAndError(sclsvrCALIBRATOR_PHOT_COUS_H, mHc, eHc, oriHc));
            }
            if (!isnan(mJc))
            {
                FAIL(SetPropertyValueAndError(sclsvrCALIBRATOR_PHOT_COUS_J, mJc, eJc, oriJc));
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
    vobsSTAR_PROPERTY* magJ = GetProperty(sclsvrCALIBRATOR_PHOT_COUS_J);
    vobsSTAR_PROPERTY* magH = GetProperty(sclsvrCALIBRATOR_PHOT_COUS_H);
    vobsSTAR_PROPERTY* magK = GetProperty(sclsvrCALIBRATOR_PHOT_COUS_K);

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
        sclsvrCALIBRATOR::sclsvrCALIBRATOR_PropertyMetaBegin = vobsSTAR_PROPERTY_META::vobsStar_PropertyMetaList.size();

        // Add Meta data:
        AddPropertyMeta(sclsvrCALIBRATOR_PHOT_COUS_J, "Jcous", vobsFLOAT_PROPERTY, "mag",
                        "Cousin's Magnitude in J-band");
        AddPropertyErrorMeta(sclsvrCALIBRATOR_PHOT_COUS_J_ERROR, "e_Jcous", "mag",
                             "Error on Cousin's Magnitude in J-band");
        AddPropertyMeta(sclsvrCALIBRATOR_PHOT_COUS_H, "Hcous", vobsFLOAT_PROPERTY, "mag",
                        "Cousin's Magnitude in H-band");
        AddPropertyErrorMeta(sclsvrCALIBRATOR_PHOT_COUS_H_ERROR, "e_Hcous", "mag",
                             "Error on Cousin's Magnitude in H-band");
        AddPropertyMeta(sclsvrCALIBRATOR_PHOT_COUS_K, "Kcous", vobsFLOAT_PROPERTY, "mag",
                        "Cousin's Magnitude in K-band");
        AddPropertyErrorMeta(sclsvrCALIBRATOR_PHOT_COUS_K_ERROR, "e_Kcous", "mag",
                             "Error on Cousin's Magnitude in K-band");

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

        /* Relative error on LD diameter */
        AddPropertyMeta(sclsvrCALIBRATOR_LD_DIAM_ERROR_REL, "e_LDD_rel", vobsFLOAT_PROPERTY, "%", "Relative error on limb-darkened diameter");

        /* chi2 of the weighted mean diameter estimation */
        AddFormattedPropertyMeta(sclsvrCALIBRATOR_DIAM_CHI2, "diam_chi2", vobsFLOAT_PROPERTY, NULL, "%.4lf",
                                 "Reduced chi-square of the LDD diameter estimation");

        /* calibrator flag (bits) */
        AddPropertyMeta(sclsvrCALIBRATOR_CAL_FLAG, "CalFlag", vobsINT_PROPERTY, NULL, "Calibrator Flag (bit field): "
                        " bit 0 is set if LDD_CHI is above 5;"
                        " bit 1 is set if the star is a known double in WDS (Cat. B/wds/wds) with separation inferior to 1 arcsec;"
                        " bit 2 is set if the star is, according to Simbad's OTYPEs, one of the codes which signals a possible binarity or pulsating stars;"
                        " bit 3 is set if the star has neighbours within 0.5 as (ASCC / GAIA) or 1.0 as (2MASS)");

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

        /* extinction ratio related to interstellar absorption */
        AddPropertyMeta(sclsvrCALIBRATOR_EXTINCTION_RATIO, "Av", vobsFLOAT_PROPERTY, NULL, "Visual Interstellar Absorption computed from photometric magnitudes and (possible) spectral type");
        AddPropertyErrorMeta(sclsvrCALIBRATOR_EXTINCTION_RATIO_ERROR, "e_Av", NULL, "Error on Visual Interstellar Absorption");

        /* chi2 of the extinction ratio estimation */
        AddFormattedPropertyMeta(sclsvrCALIBRATOR_AV_FIT_CHI2, "Av_fit_chi2", vobsFLOAT_PROPERTY, NULL, "%.4lf",
                                 "Reduced chi-square of the extinction ratio estimation");

        /* distance computed from parallax */
        AddPropertyMeta(sclsvrCALIBRATOR_DIST_PLX, "dist_plx", vobsFLOAT_PROPERTY, "pc",
                        "Distance computed from parallax");
        AddPropertyErrorMeta(sclsvrCALIBRATOR_DIST_PLX_ERROR, "e_dist_plx", "pc",
                             "Error on distance computed from parallax");

        /* fitted distance (parsec) computed from photometric magnitudes and spectral type */
        AddPropertyMeta(sclsvrCALIBRATOR_DIST_FIT, "dist_fit", vobsFLOAT_PROPERTY, "pc",
                        "Fitted distance computed from photometric magnitudes and (possible) spectral type");
        AddPropertyErrorMeta(sclsvrCALIBRATOR_DIST_FIT_ERROR, "e_dist_fit", "pc",
                             "Error on the fitted distance");

        /* chi2 of the distance modulus (dist_plx vs dist_fit) */
        AddFormattedPropertyMeta(sclsvrCALIBRATOR_DIST_FIT_CHI2, "dist_fit_chi2", vobsFLOAT_PROPERTY, NULL, "%.4lf",
                                 "Chi-square of the distance modulus (dist_plx vs dist_fit)");

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
        sclsvrCALIBRATOR::sclsvrCALIBRATOR_PropertyMetaEnd = vobsSTAR_PROPERTY_META::vobsStar_PropertyMetaList.size();

        logInfo("sclsvrCALIBRATOR has defined %d properties.", sclsvrCALIBRATOR::sclsvrCALIBRATOR_PropertyMetaEnd);

        if (sclsvrCALIBRATOR::sclsvrCALIBRATOR_PropertyMetaEnd != sclsvrCALIBRATOR_MAX_PROPERTIES)
        {
            logError("sclsvrCALIBRATOR_MAX_PROPERTIES constant is incorrect: %d < %d",
                     sclsvrCALIBRATOR_MAX_PROPERTIES, sclsvrCALIBRATOR::sclsvrCALIBRATOR_PropertyMetaEnd);
            exit(1);
        }

        logInfo("sclsvrCALIBRATOR_MAX_PROPERTIES: %d - sizeof(vobsSTAR_PROPERTY) = %d bytes - sizeof(sclsvrCALIBRATOR) = %d bytes - total = %d bytes",
                sclsvrCALIBRATOR_MAX_PROPERTIES, sizeof (vobsSTAR_PROPERTY), sizeof (sclsvrCALIBRATOR),
                sclsvrCALIBRATOR_MAX_PROPERTIES * sizeof (vobsSTAR_PROPERTY) + sizeof (sclsvrCALIBRATOR));

        initializeIndex();

        // Dump all properties (vobsSTAR and sclsvrCALIBRATOR) into XML file:
        DumpPropertyIndexAsXML();

        sclsvrCALIBRATOR::sclsvrCALIBRATOR_PropertyIdxInitialized = true;
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

