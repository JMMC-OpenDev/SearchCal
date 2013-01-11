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


/* maximum number of properties (115) */
#define sclsvrCALIBRATOR_MAX_PROPERTIES 115

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
 * Assignement operator
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

    // If diameter has not been computed yet or is not OK, return mcsFALSE:
    if ((IsPropertySet(property) == mcsFALSE) || (strcmp(GetPropertyValue(property), "OK") != 0))
    {
        return mcsFALSE;
    }

    return mcsTRUE;
}

/**
 * Complete the property of the calibrator.
 *
 * Method to complete calibrator properties by using several methods.
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is
 * returned.
 */
mcsCOMPL_STAT sclsvrCALIBRATOR::Complete(const sclsvrREQUEST &request)
{
    mcsSTRING64 starId;

    // Get Star ID
    FAIL(GetId(starId, sizeof (starId)));

    logTest("----- Complete: star '%s'", starId);

    // Check parallax. This will remove the property
    // vobsSTAR_POS_PARLX_TRIG of if parallax is not OK
    FAIL(CheckParallax());

    // Parse spectral type. Use Johnson B-V to access luminosity class
    // if unknow by comparing with colorTables
    FAIL(ParseSpectralType());

    // Fill in the Teff and LogG entries using the spectral type
    FAIL(ComputeTeffLogg())

    // Compute Galactic coordinates:
    FAIL(ComputeGalacticCoordinates());

    // Compute N Band and S_12 with AKARI from Teff
    FAIL(ComputeIRFluxes());

    // Compute J, H, K COUSIN magnitude from Johnson catalogues
    FAIL(ComputeCousinMagnitudes());

    // If parallax is OK, we compute absorption coefficient Av, which
    // is set in sclsvrCALIBRATOR_EXTINCTION_RATIO
    // We also compute missing magnitude
    // since the relation color-index / spectral type has high confidence
    // when the distance is known
    if (IsPropertySet(vobsSTAR_POS_PARLX_TRIG) == mcsTRUE)
    {
        FAIL(ComputeExtinctionCoefficient());

        // Compute missing Magnitude. All values (including computed ones)
        // will be used later to compute a diameter
        FAIL(ComputeMissingMagnitude(request.IsBright()));
    }
    else
    {
        logTest("parallax is unknown; could not compute Av and missing magnitude");
    }

    // Compute J, H, K JOHNSON magnitude (2MASS) from COUSIN
    FAIL(ComputeJohnsonMagnitudes());

    // If the request should return bright stars
    if (request.IsBright() == mcsTRUE)
    {
        // Get the observed band
        const char* band = request.GetSearchBand();

        // If it is the scenario for N band, we don't compute diameter
        // but we only use those from catalogs.
        if (strcmp(band, "N") == 0)
        {
            FAIL(SetPropertyValue(sclsvrCALIBRATOR_DIAM_FLAG, "OK", vobsSTAR_COMPUTED_PROP));
        }
        else
        {
            // If the parallax is known, then extinction ratio Av is also known
            // in sclsvrCALIBRATOR_EXTINCTION_RATIO.

            // We compute the diameter with Av, Bj, Vj, Rj, Kc
            if (IsPropertySet(vobsSTAR_POS_PARLX_TRIG) == mcsTRUE)
            {
                FAIL(ComputeAngularDiameter(mcsTRUE));
            }
            else
            {
                // In bright we don't compute the diameter if the
                // parallax (so the Av) is not known
                logTest("parallax is unknown; could not compute diameter (bright)", starId);
            }
        }

        // Compute visibility and visibility error
        FAIL(ComputeVisibility(request));
    }
    else
    {
        // If the search is faint
        // If the parallax is known, then extinction ratio Av is also known
        // in sclsvrCALIBRATOR_EXTINCTION_RATIO.

        // We compute the diameter with Av, Vj, Ic, Jc, Hc, Kc
        if (IsPropertySet(vobsSTAR_POS_PARLX_TRIG) == mcsTRUE)
        {
            // Compute Angular Diameter
            FAIL(ComputeAngularDiameter(mcsFALSE));

            // Compute visibility and visibility error
            FAIL(ComputeVisibility(request));
        }
        else
        {
            // Parallax is unknown so Av is unknown.
            // We first compute diameter and visibility without considering
            // interstellar absorption (i.e av=0) and then with (i.e. av=3)
            // See JMMC-MEM-2600-0003

            mcsLOGICAL vis2Compatible = mcsFALSE;

            // Compute Angular Diameter without absorption i.e. av=0
            logTest("Computing diameter without absorption...", starId);

            // Temporary stars with/without interstellar absorption
            sclsvrCALIBRATOR starWith(*this);

            // Set extinction ratio property
            FAIL(SetPropertyValue(sclsvrCALIBRATOR_EXTINCTION_RATIO, 0.0, vobsSTAR_COMPUTED_PROP));

            // Compute Angular Diameter
            FAIL(ComputeAngularDiameter(mcsFALSE));

            // Compute visibility and visibility error
            FAIL(ComputeVisibility(request));

            // If visibility has been computed, then compute now diameters and
            // visibility with an arbitrary interstellar absorption
            if (IsPropertySet(sclsvrCALIBRATOR_VIS2) == mcsTRUE)
            {
                // Compute Angular Diameter
                logTest("Computing diameter with absorption...", starId);

                // Do the same with absorption i.e. av=3
                FAIL(starWith.SetPropertyValue(sclsvrCALIBRATOR_EXTINCTION_RATIO, 3.0, vobsSTAR_COMPUTED_PROP));

                // Compute Angular Diameter
                FAIL(starWith.ComputeAngularDiameter(mcsFALSE));

                // Compute visibility and visibility error
                FAIL(starWith.ComputeVisibility(request));

                // If visibility has been computed, compare result 
                if (starWith.IsPropertySet(sclsvrCALIBRATOR_VIS2) == mcsTRUE)
                {
                    // Get Visibility of the star (without absorption)
                    mcsDOUBLE vis2, vis2Err;
                    GetPropertyValue(sclsvrCALIBRATOR_VIS2, &vis2);
                    GetPropertyValue(sclsvrCALIBRATOR_VIS2_ERROR, &vis2Err);

                    // Get Visibility of the star (with absorption)
                    mcsDOUBLE vis2A, vis2ErrA;
                    starWith.GetPropertyValue(sclsvrCALIBRATOR_VIS2, &vis2A);
                    starWith.GetPropertyValue(sclsvrCALIBRATOR_VIS2_ERROR, &vis2ErrA);

                    const mcsDOUBLE vis2Diff = fabs(vis2A - vis2);

                    // Compute MAX(|vis2A - vis2|; vis2Err)
                    const mcsDOUBLE visibilityErr = (vis2Diff < vis2Err) ? vis2Err : vis2Diff;

                    logTest("vis2, vis2A, |vis2A - vis2| = %lf / %lf / %lf", vis2, vis2A, vis2Diff);
                    logTest("vis2Err, visibilityErr = %lf / %lf", vis2Err, visibilityErr);

                    // Test of validity of the visibility
                    const mcsDOUBLE expectedVisErr = request.GetExpectedVisErr();

                    if (visibilityErr > expectedVisErr)
                    {
                        logTest("star '%s' - visibility error (%lf) is higher than the expected one (%lf)",
                                starId, visibilityErr, expectedVisErr);
                    }
                    else
                    {
                        logDebug("star '%s' - visibility error (%lf) is better than the expected one (%lf)",
                                 starId, visibilityErr, expectedVisErr);

                        vis2Compatible = mcsTRUE;

                        Update(starWith, vobsOVERWRITE_ALL);
                    }
                }
            }

            if (vis2Compatible == mcsFALSE)
            {
                // visibility computed with/without interstellar absorption are not compatible: 
                logTest("star '%s' - visibilities computed with/without interstellar absorption are not compatible", starId);

                // Overwrite DIAM_FLAG property = "NOK":
                FAIL(SetPropertyValue(sclsvrCALIBRATOR_DIAM_FLAG, "NOK", vobsSTAR_COMPUTED_PROP, vobsCONFIDENCE_HIGH, mcsTRUE));
                FAIL(SetPropertyValue(sclsvrCALIBRATOR_DIAM_FLAG_INFO, "INCONSISTENT_VIS2_AV", vobsSTAR_COMPUTED_PROP));

                // If visibility has been computed, then clear values (to filter such stars on GUI):
                if (IsPropertySet(sclsvrCALIBRATOR_VIS2) == mcsTRUE)
                {
                    // Reset vis2 (to filter such stars on GUI):
                    ClearPropertyValue(sclsvrCALIBRATOR_VIS2);
                    ClearPropertyValue(sclsvrCALIBRATOR_VIS2_ERROR);
                }
            }
        }
    }

    // Compute UD from LD and SP
    FAIL(ComputeUDFromLDAndSP());

    // Compute distance
    FAIL(ComputeDistance(request));

    return mcsSUCCESS;
}


/*
 * Private methods
 */

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
    const char* magPropertyId[alxNB_BANDS] = {
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

    vobsSTAR_PROPERTY* property;

    // For each magnitude
    alxMAGNITUDES magnitudes;
    for (int band = 0; band < alxNB_BANDS; band++)
    {
        property = GetProperty(magPropertyId[band]);

        // Get the current value
        if (IsPropertySet(property) == mcsTRUE)
        {
            FAIL(GetPropertyValue(property, &magnitudes[band].value));

            magnitudes[band].isSet = mcsTRUE;
            magnitudes[band].confIndex = (alxCONFIDENCE_INDEX) property->GetConfidenceIndex();
        }
        else
        {
            magnitudes[band].isSet = mcsFALSE;
            magnitudes[band].confIndex = alxNO_CONFIDENCE;
            magnitudes[band].value = 0.0;
        }
    }

    /* Print out results */
    logTest("Initial magnitudes:   B = %0.3lf (%s), V = %0.3lf (%s), "
            "R = %0.3lf (%s), I = %0.3lf (%s), J = %0.3lf (%s), H = %0.3lf (%s), "
            "K = %0.3lf (%s), L = %0.3lf (%s), M = %0.3lf (%s)",
            magnitudes[alxB_BAND].value, alxGetConfidenceIndex(magnitudes[alxB_BAND].confIndex),
            magnitudes[alxV_BAND].value, alxGetConfidenceIndex(magnitudes[alxV_BAND].confIndex),
            magnitudes[alxR_BAND].value, alxGetConfidenceIndex(magnitudes[alxR_BAND].confIndex),
            magnitudes[alxI_BAND].value, alxGetConfidenceIndex(magnitudes[alxI_BAND].confIndex),
            magnitudes[alxJ_BAND].value, alxGetConfidenceIndex(magnitudes[alxJ_BAND].confIndex),
            magnitudes[alxH_BAND].value, alxGetConfidenceIndex(magnitudes[alxH_BAND].confIndex),
            magnitudes[alxK_BAND].value, alxGetConfidenceIndex(magnitudes[alxK_BAND].confIndex),
            magnitudes[alxL_BAND].value, alxGetConfidenceIndex(magnitudes[alxL_BAND].confIndex),
            magnitudes[alxM_BAND].value, alxGetConfidenceIndex(magnitudes[alxM_BAND].confIndex));

    // Get the extinction ratio
    mcsDOUBLE av;
    FAIL(GetPropertyValue(sclsvrCALIBRATOR_EXTINCTION_RATIO, &av));

    // Compute corrected magnitude
    // (remove the expected interstellar absorption)
    FAIL(alxComputeCorrectedMagnitudes(av, magnitudes));

    // Compute missing magnitudes
    if (isBright == mcsTRUE)
    {
        FAIL(alxComputeMagnitudesForBrightStar(&_spectralType, magnitudes));
    }
    else
    {
        FAIL(alxComputeMagnitudesForFaintStar(&_spectralType, magnitudes));
    }

    // Compute apparent magnitude (apply back interstellar absorption)
    FAIL(alxComputeApparentMagnitudes(av, magnitudes));

    // Set back the computed magnitude. Already existing magnitudes are not
    // overwritten. 
    for (int band = 0; band < alxNB_BANDS; band++)
    {
        if (magnitudes[band].isSet == mcsTRUE)
        {
            FAIL(SetPropertyValue(magPropertyId[band], magnitudes[band].value, vobsSTAR_COMPUTED_PROP,
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
    logTrace("sclsvrCALIBRATOR::ComputeGalacticCoordinates()");

    mcsDOUBLE gLat, gLon, ra, dec;

    // Get right ascension position in degree
    FAIL(GetRa(ra));

    // Get declinaison in degree
    FAIL(GetDec(dec));

    // Compute galactic coordinates from the retrieved ra and dec values
    FAIL(alxComputeGalacticCoordinates(ra, dec, &gLat, &gLon));

    // Set the galactic lattitude
    FAIL(SetPropertyValue(sclsvrCALIBRATOR_POS_GAL_LAT, gLat, vobsSTAR_COMPUTED_PROP));

    // Set the galactic longitude
    FAIL(SetPropertyValue(sclsvrCALIBRATOR_POS_GAL_LON, gLon, vobsSTAR_COMPUTED_PROP));

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
    logTrace("sclsvrCALIBRATOR::ComputeExtinctionCoefficient()");

    mcsDOUBLE parallax, gLat, gLon;
    mcsDOUBLE av;
    vobsSTAR_PROPERTY* property;

    // Get the value of the parallax
    property = GetProperty(vobsSTAR_POS_PARLX_TRIG);
    FAIL_FALSE_DO(IsPropertySet(property), errAdd(sclsvrERR_MISSING_PROPERTY, vobsSTAR_POS_PARLX_TRIG, "interstellar absorption"));
    FAIL(GetPropertyValue(property, &parallax));

    // Get the value of the galactic lattitude
    property = GetProperty(sclsvrCALIBRATOR_POS_GAL_LAT);
    FAIL_FALSE_DO(IsPropertySet(property), errAdd(sclsvrERR_MISSING_PROPERTY, sclsvrCALIBRATOR_POS_GAL_LAT, "interstellar absorption"));
    FAIL(GetPropertyValue(property, &gLat));

    // Get the value of the galactic longitude
    property = GetProperty(sclsvrCALIBRATOR_POS_GAL_LON);
    FAIL_FALSE_DO(IsPropertySet(property), errAdd(sclsvrERR_MISSING_PROPERTY, sclsvrCALIBRATOR_POS_GAL_LON, "interstellar absorption"));
    FAIL(GetPropertyValue(property, &gLon));

    // Compute Extinction ratio
    FAIL(alxComputeExtinctionCoefficient(&av, parallax, gLat, gLon));

    // Set extinction ratio property
    FAIL(SetPropertyValue(sclsvrCALIBRATOR_EXTINCTION_RATIO, av, vobsSTAR_COMPUTED_PROP));

    return mcsSUCCESS;
}

/**
 * Compute angular diameter
 * 
 * @param isBright true is it is for bright object
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is
 * returned.
 */
mcsCOMPL_STAT sclsvrCALIBRATOR::ComputeAngularDiameter(mcsLOGICAL isBright)
{
    logTrace("sclsvrCALIBRATOR::ComputeAngularDiameter()");

    // We will use these bands. PHOT_COUS bands
    // should have been prepared before. No check is done on wether
    // these magnitudes comes from computed value or directly from
    // catalogues
    const char* magPropertyId[alxNB_BANDS] = {
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

    vobsSTAR_PROPERTY* property;

    // Fill the magnitude structure
    alxMAGNITUDES mag;
    for (int band = 0; band < alxNB_BANDS; band++)
    {
        property = GetProperty(magPropertyId[band]);

        // Get the current value
        if (IsPropertySet(property) == mcsTRUE)
        {
            FAIL(GetPropertyValue(property, &mag[band].value));

            mag[band].isSet = mcsTRUE;
            mag[band].confIndex = (alxCONFIDENCE_INDEX) property->GetConfidenceIndex();
        }
        else
        {
            mag[band].isSet = mcsFALSE;
            mag[band].confIndex = alxNO_CONFIDENCE;
            mag[band].value = 0.0;
        }
    }

    // We now have mag = {Bj, Vj, Rj, Jc, Ic, Hc, Kc, Lj, Mj}
    logTest("Extracted magnitudes: B = %0.3lf (%s), V = %0.3lf (%s), "
            "R = %0.3lf (%s), I = %0.3lf (%s), J = %0.3lf (%s), H = %0.3lf (%s), "
            "K = %0.3lf (%s), L = %0.3lf (%s), M = %0.3lf (%s)",
            mag[alxB_BAND].value, alxGetConfidenceIndex(mag[alxB_BAND].confIndex),
            mag[alxV_BAND].value, alxGetConfidenceIndex(mag[alxV_BAND].confIndex),
            mag[alxR_BAND].value, alxGetConfidenceIndex(mag[alxR_BAND].confIndex),
            mag[alxI_BAND].value, alxGetConfidenceIndex(mag[alxI_BAND].confIndex),
            mag[alxJ_BAND].value, alxGetConfidenceIndex(mag[alxJ_BAND].confIndex),
            mag[alxH_BAND].value, alxGetConfidenceIndex(mag[alxH_BAND].confIndex),
            mag[alxK_BAND].value, alxGetConfidenceIndex(mag[alxK_BAND].confIndex),
            mag[alxL_BAND].value, alxGetConfidenceIndex(mag[alxL_BAND].confIndex),
            mag[alxM_BAND].value, alxGetConfidenceIndex(mag[alxM_BAND].confIndex));


    // Get the extinction ratio
    mcsDOUBLE av;
    FAIL(GetPropertyValue(sclsvrCALIBRATOR_EXTINCTION_RATIO, &av));

    // Correct interstellar absorption
    FAIL(alxComputeCorrectedMagnitudes(av, mag));

    // Compute diameters independently for each color index
    alxDIAMETERS diam;
    FAIL(alxComputeAngularDiameters(mag, diam));

    /* Write BV Diameter */
    if (diam[alxB_V_DIAM].isSet == mcsTRUE)
    {
        FAIL(SetPropertyValue(sclsvrCALIBRATOR_DIAM_BV, diam[alxB_V_DIAM].value, vobsSTAR_COMPUTED_PROP,
                              (vobsCONFIDENCE_INDEX) diam[alxB_V_DIAM].confIndex));
        FAIL(SetPropertyValue(sclsvrCALIBRATOR_DIAM_BV_ERROR, diam[alxB_V_DIAM].error, vobsSTAR_COMPUTED_PROP,
                              (vobsCONFIDENCE_INDEX) diam[alxB_V_DIAM].confIndex));
    }

    /* Write VR Diameter */
    if (diam[alxV_R_DIAM].isSet == mcsTRUE)
    {
        FAIL(SetPropertyValue(sclsvrCALIBRATOR_DIAM_VR, diam[alxV_R_DIAM].value, vobsSTAR_COMPUTED_PROP,
                              (vobsCONFIDENCE_INDEX) diam[alxV_R_DIAM].confIndex));
        FAIL(SetPropertyValue(sclsvrCALIBRATOR_DIAM_VR_ERROR, diam[alxV_R_DIAM].error, vobsSTAR_COMPUTED_PROP,
                              (vobsCONFIDENCE_INDEX) diam[alxV_R_DIAM].confIndex));
    }

    /* Write VK Diameter */
    if (diam[alxV_K_DIAM].isSet == mcsTRUE)
    {
        FAIL(SetPropertyValue(sclsvrCALIBRATOR_DIAM_VK, diam[alxV_K_DIAM].value, vobsSTAR_COMPUTED_PROP,
                              (vobsCONFIDENCE_INDEX) diam[alxV_K_DIAM].confIndex));
        FAIL(SetPropertyValue(sclsvrCALIBRATOR_DIAM_VK_ERROR, diam[alxV_K_DIAM].error, vobsSTAR_COMPUTED_PROP,
                              (vobsCONFIDENCE_INDEX) diam[alxV_K_DIAM].confIndex));
    }

    /* Write IJ Diameter */
    if (diam[alxI_J_DIAM].isSet == mcsTRUE)
    {
        FAIL(SetPropertyValue(sclsvrCALIBRATOR_DIAM_IJ, diam[alxI_J_DIAM].value, vobsSTAR_COMPUTED_PROP,
                              (vobsCONFIDENCE_INDEX) diam[alxI_J_DIAM].confIndex));
        FAIL(SetPropertyValue(sclsvrCALIBRATOR_DIAM_IJ_ERROR, diam[alxI_J_DIAM].error, vobsSTAR_COMPUTED_PROP,
                              (vobsCONFIDENCE_INDEX) diam[alxI_J_DIAM].confIndex));
    }

    /* Write IK Diameter */
    if (diam[alxI_K_DIAM].isSet == mcsTRUE)
    {
        FAIL(SetPropertyValue(sclsvrCALIBRATOR_DIAM_IK, diam[alxI_K_DIAM].value, vobsSTAR_COMPUTED_PROP,
                              (vobsCONFIDENCE_INDEX) diam[alxI_K_DIAM].confIndex));
        FAIL(SetPropertyValue(sclsvrCALIBRATOR_DIAM_IK_ERROR, diam[alxI_K_DIAM].error, vobsSTAR_COMPUTED_PROP,
                              (vobsCONFIDENCE_INDEX) diam[alxI_K_DIAM].confIndex));
    }

    /* Write JH Diameter */
    if (diam[alxJ_H_DIAM].isSet == mcsTRUE)
    {
        FAIL(SetPropertyValue(sclsvrCALIBRATOR_DIAM_JH, diam[alxJ_H_DIAM].value, vobsSTAR_COMPUTED_PROP,
                              (vobsCONFIDENCE_INDEX) diam[alxJ_H_DIAM].confIndex));
        FAIL(SetPropertyValue(sclsvrCALIBRATOR_DIAM_JH_ERROR, diam[alxJ_H_DIAM].error, vobsSTAR_COMPUTED_PROP,
                              (vobsCONFIDENCE_INDEX) diam[alxJ_H_DIAM].confIndex));
    }

    /* Write JK Diameter */
    if (diam[alxJ_K_DIAM].isSet == mcsTRUE)
    {
        FAIL(SetPropertyValue(sclsvrCALIBRATOR_DIAM_JK, diam[alxJ_K_DIAM].value, vobsSTAR_COMPUTED_PROP,
                              (vobsCONFIDENCE_INDEX) diam[alxJ_K_DIAM].confIndex));
        FAIL(SetPropertyValue(sclsvrCALIBRATOR_DIAM_JK_ERROR, diam[alxJ_K_DIAM].error, vobsSTAR_COMPUTED_PROP,
                              (vobsCONFIDENCE_INDEX) diam[alxJ_K_DIAM].confIndex));
    }

    /* Write HK Diameter */
    if (diam[alxH_K_DIAM].isSet == mcsTRUE)
    {
        FAIL(SetPropertyValue(sclsvrCALIBRATOR_DIAM_HK, diam[alxH_K_DIAM].value, vobsSTAR_COMPUTED_PROP,
                              (vobsCONFIDENCE_INDEX) diam[alxH_K_DIAM].confIndex));
        FAIL(SetPropertyValue(sclsvrCALIBRATOR_DIAM_HK_ERROR, diam[alxH_K_DIAM].error, vobsSTAR_COMPUTED_PROP,
                              (vobsCONFIDENCE_INDEX) diam[alxH_K_DIAM].confIndex));
    }

    // Tweak the diameter to implement the different
    // specification between BRIGHT and FAINT
    // FIXME: discuss new specifications to use all colors
    // in both faint and bright

    const mcsUINT32 nbRequiredDiameters = 3; // 3 diameters is enough in any case (bright or faint)

    if (isBright == mcsTRUE)
    {
        // Bright: remove IR diameters
        diam[alxI_J_DIAM].isSet = mcsFALSE;
        diam[alxI_K_DIAM].isSet = mcsFALSE;
        diam[alxJ_H_DIAM].isSet = mcsFALSE;
        diam[alxJ_K_DIAM].isSet = mcsFALSE;
        diam[alxH_K_DIAM].isSet = mcsFALSE;

        // Bright: check the we have all B, V, R and Kc bands
        if ((mag[alxB_BAND].isSet == mcsFALSE) || (mag[alxV_BAND].isSet == mcsFALSE) ||
            (mag[alxR_BAND].isSet == mcsFALSE) || (mag[alxK_BAND].isSet == mcsFALSE))
        {
            logTest("B, V, R and/or Kc magitudes are unknown; could not compute diameter (bright case)");

            diam[alxB_V_DIAM].isSet = mcsFALSE;
            diam[alxV_R_DIAM].isSet = mcsFALSE;
            diam[alxV_K_DIAM].isSet = mcsFALSE;
        }
    }
    else
    {
        // Faint: remove Visible diameters
        diam[alxB_V_DIAM].isSet = mcsFALSE;
        diam[alxV_R_DIAM].isSet = mcsFALSE;

        // Faint: check that we have Jc, Hc, Kc. Note that Ic and Vj are optional
        if ((mag[alxJ_BAND].isSet != mcsTRUE) || (mag[alxH_BAND].isSet != mcsTRUE) || (mag[alxK_BAND].isSet != mcsTRUE))
        {
            logTest("J, H and/or K magitudes are unknown; could not compute diameter (faint case)");

            diam[alxV_K_DIAM].isSet = mcsFALSE;
            diam[alxI_J_DIAM].isSet = mcsFALSE;
            diam[alxI_K_DIAM].isSet = mcsFALSE;
            diam[alxJ_H_DIAM].isSet = mcsFALSE;
            diam[alxJ_K_DIAM].isSet = mcsFALSE;
            diam[alxH_K_DIAM].isSet = mcsFALSE;
        }
    }

    // Compute mean diameter. meanDiam.isSet is true if the mean
    // is consistent with each individual (valid) diameters
    alxDATA meanDiam;
    mcsSTRING32 diamInfo;
    FAIL(alxComputeMeanAngularDiameter(diam, &meanDiam, nbRequiredDiameters, &diamInfo));

    // Write MEAN DIAMETER 
    if (meanDiam.isSet == mcsTRUE)
    {
        FAIL(SetPropertyValue(sclsvrCALIBRATOR_DIAM_FLAG, "OK", vobsSTAR_COMPUTED_PROP));
        FAIL(SetPropertyValue(sclsvrCALIBRATOR_DIAM_MEAN, meanDiam.value, vobsSTAR_COMPUTED_PROP,
                              (vobsCONFIDENCE_INDEX) meanDiam.confIndex));
        FAIL(SetPropertyValue(sclsvrCALIBRATOR_DIAM_MEAN_ERROR, meanDiam.error, vobsSTAR_COMPUTED_PROP,
                              (vobsCONFIDENCE_INDEX) meanDiam.confIndex));
    }
    else
    {
        logTest("Computed diameters are not coherent between them; Mean diameter is not kept");

        FAIL(SetPropertyValue(sclsvrCALIBRATOR_DIAM_FLAG, "NOK", vobsSTAR_COMPUTED_PROP));
        FAIL(SetPropertyValue(sclsvrCALIBRATOR_DIAM_FLAG_INFO, diamInfo, vobsSTAR_COMPUTED_PROP));
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
    logTrace("sclsvrCALIBRATOR::ComputeUDFromLDAndSP()");

    // Nov 2012: LBO: in bright case, if UDDK is defined should use such diameter instead of computed or missing ones (diam NOK) ?

    // Compute UD only if LD is OK
    SUCCESS_FALSE_DO(IsDiameterOk(), logTest("Compute UD - Skipping (diameters are not OK)."));

    vobsSTAR_PROPERTY* property;

    // Does diameter from VK exist ?
    property = GetProperty(sclsvrCALIBRATOR_DIAM_VK);

    if (IsPropertySet(property) == mcsFALSE)
    {
        // use mean diameter instead (defined because diameters are OK) (main case in faint scenario):
        property = GetProperty(sclsvrCALIBRATOR_DIAM_MEAN);
    }

    // Does LD diameter exist ?
    SUCCESS_FALSE_DO(IsPropertySet(property), logTest("Compute UD - Skipping (LD unknown)."));

    // Get LD diameter (DIAM_VK or DIAM_MEAN)
    mcsDOUBLE ld = FP_NAN;
    SUCCESS_DO(GetPropertyValue(property, &ld), logWarning("Compute UD - Aborting (error while retrieving DIAM_VK)."));

    // Get LD diameter confidence index (UDs will have the same one)
    vobsCONFIDENCE_INDEX ldDiameterConfidenceIndex = property->GetConfidenceIndex();

    // Does Teff exist ?
    property = GetProperty(sclsvrCALIBRATOR_TEFF_SPTYP);
    SUCCESS_FALSE_DO(IsPropertySet(property), logTest("Compute UD - Skipping (Teff unknown)."));

    mcsDOUBLE Teff = FP_NAN;
    SUCCESS_DO(GetPropertyValue(property, &Teff), logWarning("Compute UD - Aborting (error while retrieving Teff)."))

    // Does LogG exist ?
    property = GetProperty(sclsvrCALIBRATOR_LOGG_SPTYP);
    SUCCESS_FALSE_DO(IsPropertySet(property), logTest("Compute UD - Skipping (LogG unknown)."));

    mcsDOUBLE LogG = FP_NAN;
    SUCCESS_DO(GetPropertyValue(property, &LogG), logWarning("Compute UD - Aborting (error while retrieving LogG)."));

    // Compute UD
    logTest("Computing UDs for LD = '%lf', Teff = '%lf', LogG = '%lf' ...", ld, Teff, LogG);

    alxUNIFORM_DIAMETERS ud;
    SUCCESS_DO(alxGetUDFromLDAndSP(ld, Teff, LogG, &ud), logWarning("Aborting (error while computing UDs)."));

    // Set each UD_ properties accordingly:
    FAIL(SetPropertyValue(sclsvrCALIBRATOR_UD_U, ud.u, vobsSTAR_COMPUTED_PROP, ldDiameterConfidenceIndex));
    FAIL(SetPropertyValue(sclsvrCALIBRATOR_UD_B, ud.b, vobsSTAR_COMPUTED_PROP, ldDiameterConfidenceIndex));
    FAIL(SetPropertyValue(sclsvrCALIBRATOR_UD_V, ud.v, vobsSTAR_COMPUTED_PROP, ldDiameterConfidenceIndex));
    FAIL(SetPropertyValue(sclsvrCALIBRATOR_UD_R, ud.r, vobsSTAR_COMPUTED_PROP, ldDiameterConfidenceIndex));
    FAIL(SetPropertyValue(sclsvrCALIBRATOR_UD_I, ud.i, vobsSTAR_COMPUTED_PROP, ldDiameterConfidenceIndex));
    FAIL(SetPropertyValue(sclsvrCALIBRATOR_UD_J, ud.j, vobsSTAR_COMPUTED_PROP, ldDiameterConfidenceIndex));
    FAIL(SetPropertyValue(sclsvrCALIBRATOR_UD_H, ud.h, vobsSTAR_COMPUTED_PROP, ldDiameterConfidenceIndex));
    FAIL(SetPropertyValue(sclsvrCALIBRATOR_UD_K, ud.k, vobsSTAR_COMPUTED_PROP, ldDiameterConfidenceIndex));
    FAIL(SetPropertyValue(sclsvrCALIBRATOR_UD_L, ud.l, vobsSTAR_COMPUTED_PROP, ldDiameterConfidenceIndex));
    FAIL(SetPropertyValue(sclsvrCALIBRATOR_UD_N, ud.n, vobsSTAR_COMPUTED_PROP, ldDiameterConfidenceIndex));

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
    logTrace("sclsvrCALIBRATOR::ComputeVisibility()");

    mcsDOUBLE diam, diamError, baseMax, wavelength;
    alxVISIBILITIES visibilities;
    vobsCONFIDENCE_INDEX confidenceIndex = vobsCONFIDENCE_HIGH;

    // Get object diameter. First look at the diameters coming from catalog
    static const int nDiamId = 2;
    static const char* diamId[nDiamId][2] = {
        {vobsSTAR_UDDK_DIAM, vobsSTAR_UDDK_DIAM_ERROR},
        {vobsSTAR_DIAM12, vobsSTAR_DIAM12_ERROR}
    };

    vobsSTAR_PROPERTY* property;
    vobsSTAR_PROPERTY* propErr;

    // For each possible diameters
    mcsLOGICAL found = mcsFALSE;
    for (int i = 0; (i < nDiamId) && (found == mcsFALSE); i++)
    {
        property = GetProperty(diamId[i][0]);
        propErr = GetProperty(diamId[i][1]);

        // If diameter and its error are set 
        if ((IsPropertySet(property) == mcsTRUE) && (IsPropertySet(propErr) == mcsTRUE))
        {
            // Get values
            FAIL(GetPropertyValue(property, &diam));
            FAIL(GetPropertyValue(propErr, &diamError));
            found = mcsTRUE;

            // Set confidence index too high (value coming form catalog)
            confidenceIndex = vobsCONFIDENCE_HIGH;
        }
    }

    // If not found in catalog, use the computed one (if exist)
    if (found == mcsFALSE)
    {
        if (request.IsBright() == mcsTRUE)
        {
            // If computed diameter is OK
            SUCCESS_COND_DO((IsDiameterOk() == mcsFALSE) || (IsPropertySet(sclsvrCALIBRATOR_DIAM_VK) == mcsFALSE),
                            logTest("Unknown VK diameter; could not compute visibility"));

            // Bright case: diam VK is surely defined (required to have DIAM_FLAG = 'OK'):
            property = GetProperty(sclsvrCALIBRATOR_DIAM_VK);

            // Get V-K diameter and associated error value
            FAIL(GetPropertyValue(property, &diam));
            FAIL(GetPropertyValue(sclsvrCALIBRATOR_DIAM_VK_ERROR, &diamError));

            // Get confidence index of computed diameter
            confidenceIndex = property->GetConfidenceIndex();
        }
        else
        {
            // If computed diameter is OK
            SUCCESS_COND_DO((IsDiameterOk() == mcsFALSE) || (IsPropertySet(sclsvrCALIBRATOR_DIAM_JK) == mcsFALSE),
                            logTest("Unknown JK diameter; could not compute visibility"));

            // TODO: IsDiameterOk == true but diam JK may be undefined as only 3 diameters are required
            property = GetProperty(sclsvrCALIBRATOR_DIAM_JK);

            // Get J-K diameter and associated error value
            FAIL(GetPropertyValue(property, &diam));
            FAIL(GetPropertyValue(sclsvrCALIBRATOR_DIAM_JK_ERROR, &diamError));

            // LBO: Jan 2013: Proposed fix to use mean diameter when diam JK is undefined:

            /*
            SUCCESS_COND_DO(IsDiameterOk() == mcsFALSE,
                            logTest("Unknown diameter; could not compute visibility"));

            // Does diameter from JK exist ( may be undefined as only 3 diameters are required) ?
            property = GetProperty(sclsvrCALIBRATOR_DIAM_JK);

            if (IsPropertySet(property) == mcsTRUE)
            {
                // Get J-K diameter and associated error value
                FAIL(GetPropertyValue(sclsvrCALIBRATOR_DIAM_JK_ERROR, &diamError));
            }
            else
            {
                // use mean diameter instead (defined because diameters are OK) (main case in faint scenario):
                property = GetProperty(sclsvrCALIBRATOR_DIAM_MEAN);
                FAIL(GetPropertyValue(sclsvrCALIBRATOR_DIAM_MEAN_ERROR, &diamError));
            }

            FAIL(GetPropertyValue(property, &diam));

             */

            // Get confidence index of computed diameter
            confidenceIndex = property->GetConfidenceIndex();
        }
    }

    // Get value in request of the wavelength
    wavelength = request.GetObservingWlen();

    // Get value in request of the base max
    baseMax = request.GetMaxBaselineLength();

    FAIL(alxComputeVisibility(diam, diamError, baseMax, wavelength, &visibilities));

    // Affect visibility property
    FAIL(SetPropertyValue(sclsvrCALIBRATOR_VIS2, visibilities.vis2, vobsSTAR_COMPUTED_PROP, confidenceIndex));
    FAIL(SetPropertyValue(sclsvrCALIBRATOR_VIS2_ERROR, visibilities.vis2Error, vobsSTAR_COMPUTED_PROP, confidenceIndex));

    // If visibility has been computed, diameter (coming from catalog or computed) must be considered as OK.
    FAIL(SetPropertyValue(sclsvrCALIBRATOR_DIAM_FLAG, "OK", vobsSTAR_COMPUTED_PROP));

    // If the observed band is N, computed visibility with wlen = 8 and 13 um
    if (strcmp(request.GetSearchBand(), "N") == 0)
    {
        FAIL(alxComputeVisibility(diam, diamError, baseMax, 8.0, &visibilities));

        // Affect visibility property
        FAIL(SetPropertyValue(sclsvrCALIBRATOR_VIS2_8, visibilities.vis2, vobsSTAR_COMPUTED_PROP));
        FAIL(SetPropertyValue(sclsvrCALIBRATOR_VIS2_8_ERROR, visibilities.vis2Error, vobsSTAR_COMPUTED_PROP, confidenceIndex));

        FAIL(alxComputeVisibility(diam, diamError, baseMax, 13.0, &visibilities));

        // Affect visibility property
        FAIL(SetPropertyValue(sclsvrCALIBRATOR_VIS2_13, visibilities.vis2, vobsSTAR_COMPUTED_PROP));
        FAIL(SetPropertyValue(sclsvrCALIBRATOR_VIS2_13_ERROR, visibilities.vis2Error, vobsSTAR_COMPUTED_PROP, confidenceIndex));
    }

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
    logTrace("sclsvrCALIBRATOR::ComputeDistance()");

    // Get the science object right ascension as a C string
    const char* ra = request.GetObjectRa();

    FAIL_COND((ra == NULL) || (miscIsSpaceStr(ra) == mcsTRUE));

    // Get science object right ascension in degrees
    mcsDOUBLE scienceObjectRa = request.GetObjectRaInDeg();

    // Get the science object declinaison as a C string
    const char* dec = request.GetObjectDec();

    FAIL_COND((dec == NULL) || (miscIsSpaceStr(dec) == mcsTRUE));

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
    FAIL(SetPropertyValue(sclsvrCALIBRATOR_DIST, separation, vobsSTAR_COMPUTED_PROP));

    return mcsSUCCESS;
}

/**
 * Parse spectral type if available.
 * 
 * @return mcsSUCCESS on successfull parsing, mcsFAILURE otherwise.
 */
mcsCOMPL_STAT sclsvrCALIBRATOR::ParseSpectralType()
{
    logTrace("sclsvrCALIBRATOR::ParseSpectralType()");

    // initialize the spectral type structure anyway:
    alxInitializeSpectralType(&_spectralType);

    vobsSTAR_PROPERTY* property = GetProperty(vobsSTAR_SPECT_TYPE_MK);

    SUCCESS_FALSE_DO(IsPropertySet(property), logTest("Spectral Type - Skipping (no SpType available)."));

    mcsSTRING32 spType;
    strncpy(spType, GetPropertyValue(property), sizeof (spType) - 1);

    SUCCESS_COND_DO(strlen(spType) == 0, logTest("Spectral Type - Skipping (SpType unknown)."));

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

    if ((IsPropertySet(magB) == mcsTRUE) && (IsPropertySet(magV) == mcsTRUE))
    {
        mcsDOUBLE mV, mB;
        GetPropertyValue(magB, &mB);
        GetPropertyValue(magV, &mV);

        FAIL(alxCorrectSpectralType(&_spectralType, mB - mV));
    }

    if (_spectralType.isSpectralBinary == mcsTRUE)
    {
        logTest("Spectral Binarity - 'SB' found in SpType.");

        // Only store spectral binarity if none present before
        FAIL(SetPropertyValue(vobsSTAR_CODE_BIN_FLAG, "SB", vobsSTAR_COMPUTED_PROP, vobsCONFIDENCE_HIGH, mcsFALSE));
    }

    if (_spectralType.isDouble == mcsTRUE)
    {
        logTest("Spectral Binarity - '+' found in SpType.");

        // Only store spectral binarity if none present before
        FAIL(SetPropertyValue(vobsSTAR_CODE_MULT_FLAG, "S", vobsSTAR_COMPUTED_PROP, vobsCONFIDENCE_HIGH, mcsFALSE));
    }

    // Anyway, store our decoded spectral type:
    FAIL(SetPropertyValue(sclsvrCALIBRATOR_SP_TYPE, _spectralType.ourSpType, vobsSTAR_COMPUTED_PROP, vobsCONFIDENCE_HIGH, mcsFALSE));

    return mcsSUCCESS;
}

/**
 * Compute Teff and Log(g) from the SpType and Tables
 * 
 * @return Always mcsSUCCESS.
 */
mcsCOMPL_STAT sclsvrCALIBRATOR::ComputeTeffLogg()
{
    logTrace("sclsvrCALIBRATOR::ComputeTeffLogg()");

    SUCCESS_FALSE_DO(_spectralType.isSet, logTest("Teff and LogG - Skipping (SpType unknown)."));

    mcsDOUBLE Teff = FP_NAN;
    mcsDOUBLE LogG = FP_NAN;

    //Get Teff 
    SUCCESS_DO(alxRetrieveTeffAndLoggFromSptype(&_spectralType, &Teff, &LogG),
               logTest("Teff and LogG - Skipping (alxRetrieveTeffAndLoggFromSptype() failed on this spectral type: '%s').", _spectralType.origSpType));

    // Set Teff eand LogG properties
    FAIL(SetPropertyValue(sclsvrCALIBRATOR_TEFF_SPTYP, Teff, vobsSTAR_COMPUTED_PROP, vobsCONFIDENCE_HIGH));
    FAIL(SetPropertyValue(sclsvrCALIBRATOR_LOGG_SPTYP, LogG, vobsSTAR_COMPUTED_PROP, vobsCONFIDENCE_HIGH));

    return mcsSUCCESS;
}

/**
 * Compute Infrared Fluxes and N band using Akari
 * 
 * @return Always mcsSUCCESS.
 */
mcsCOMPL_STAT sclsvrCALIBRATOR::ComputeIRFluxes()
{
    logTrace("sclsvrCALIBRATOR::ComputeIRFluxes()");

    mcsLOGICAL has9 = mcsFALSE;
    mcsLOGICAL has18 = mcsFALSE;
    mcsLOGICAL hase_9 = mcsFALSE;
    mcsLOGICAL hase_18 = mcsFALSE;
    mcsLOGICAL f12AlreadySet = mcsFALSE;
    mcsLOGICAL e_f12AlreadySet = mcsFALSE;
    mcsDOUBLE Teff = FP_NAN;
    mcsDOUBLE fnu_9 = FP_NAN;
    mcsDOUBLE e_fnu_9 = FP_NAN;
    mcsDOUBLE fnu_12 = FP_NAN;
    mcsDOUBLE e_fnu_12 = FP_NAN;
    mcsDOUBLE fnu_18 = FP_NAN;
    mcsDOUBLE e_fnu_18 = FP_NAN;
    mcsDOUBLE magN = FP_NAN;

    // initial tests of presence of data:

    vobsSTAR_PROPERTY* property = GetProperty(sclsvrCALIBRATOR_TEFF_SPTYP);

    // Get the value of Teff. If impossible, no possibility to go further!
    SUCCESS_FALSE_DO(IsPropertySet(property), logTest("IR Fluxes: Skipping (no Teff available)."));

    // Retrieve it
    FAIL(GetPropertyValue(property, &Teff));

    property = GetProperty(vobsSTAR_PHOT_FLUX_IR_09);

    // Get fnu_09 (vobsSTAR_PHOT_FLUX_IR_09)
    if (IsPropertySet(property) == mcsTRUE)
    {
        // retrieve it
        FAIL(GetPropertyValue(property, &fnu_9));
        has9 = mcsTRUE;
    }

    property = GetProperty(vobsSTAR_PHOT_FLUX_IR_18);

    // Get fnu_18 (vobsSTAR_PHOT_FLUX_IR_18)
    if (IsPropertySet(property) == mcsTRUE)
    {
        // retrieve it
        FAIL(GetPropertyValue(property, &fnu_18));
        has18 = mcsTRUE;
    }

    // get out if no *measured* infrared fluxes
    SUCCESS_COND_DO((has9 == mcsFALSE) && (has18 == mcsFALSE), logTest("IR Fluxes: Skipping (no 9 mu or 18 mu flux available)."));

    property = GetProperty(vobsSTAR_PHOT_FLUX_IR_09_ERROR);

    // Get e_fnu_09 (vobsSTAR_PHOT_FLUX_IR_09_ERROR)
    if (IsPropertySet(property) == mcsTRUE)
    {
        // retrieve it
        FAIL(GetPropertyValue(property, &e_fnu_9));
        hase_9 = mcsTRUE;
    }

    property = GetProperty(vobsSTAR_PHOT_FLUX_IR_18_ERROR);

    // Get e_fnu_18 (vobsSTAR_PHOT_FLUX_IR_18_ERROR)
    if (IsPropertySet(property) == mcsTRUE)
    {
        // retrieve it
        FAIL(GetPropertyValue(property, &e_fnu_18));
        hase_18 = mcsTRUE;
    }

    // check presence etc of F12:
    f12AlreadySet = IsPropertySet(vobsSTAR_PHOT_FLUX_IR_12);

    // check presence etc of e_F12:
    e_f12AlreadySet = IsPropertySet(vobsSTAR_PHOT_FLUX_IR_12_ERROR);

    // if f9 is defined, compute all fluxes from it, then fill void places.
    if (has9 == mcsTRUE)
    {
        // Compute all fluxes from 9 onwards
        SUCCESS_DO(alxComputeFluxesFromAkari09(Teff, &fnu_9, &fnu_12, &fnu_18), logWarning("IR Fluxes: Skipping (akari internal error)."));

        logTest("IR Fluxes: akari measured fnu_9 = %lf; computed fnu_12 = %lf, fnu_18 = %lf", fnu_9, fnu_12, fnu_18);

        // Store it eventually:
        if (!f12AlreadySet)
        {
            FAIL(SetPropertyValue(vobsSTAR_PHOT_FLUX_IR_12, fnu_12, vobsSTAR_COMPUTED_PROP));
        }
        // Compute Mag N:
        magN = 4.1 - 2.5 * log10(fnu_12 / 0.89);

        logTest("IR Fluxes: computed magN = %lf", magN);

        // Store it if not set:
        FAIL(SetPropertyValue(vobsSTAR_PHOT_JHN_N, magN, vobsSTAR_COMPUTED_PROP));

        // store s18 if void:
        if (has18 == mcsFALSE)
        {
            FAIL(SetPropertyValue(vobsSTAR_PHOT_FLUX_IR_18, fnu_18, vobsSTAR_COMPUTED_PROP));
        }

        // compute s_12 error etc, if s09_err is present:
        if (hase_9 == mcsTRUE)
        {
            SUCCESS_DO(alxComputeFluxesFromAkari09(Teff, &e_fnu_9, &e_fnu_12, &e_fnu_18), logTest("IR Fluxes: Skipping (akari internal error)."));

            // Store it eventually:
            if (!e_f12AlreadySet)
            {
                FAIL(SetPropertyValue(vobsSTAR_PHOT_FLUX_IR_12_ERROR, e_fnu_12, vobsSTAR_COMPUTED_PROP));
            }
            // store e_s18 if void:
            if (hase_18 == mcsFALSE)
            {
                FAIL(SetPropertyValue(vobsSTAR_PHOT_FLUX_IR_18_ERROR, e_fnu_18, vobsSTAR_COMPUTED_PROP));
            }
        }
    }
    else
    {
        // only s18 !

        // Compute all fluxes from 18  backwards
        SUCCESS_DO(alxComputeFluxesFromAkari18(Teff, &fnu_18, &fnu_12, &fnu_9), logTest("IR Fluxes: Skipping (akari internal error)."));

        logTest("IR Fluxes: akari measured fnu_18 = %lf; computed fnu_12 = %lf, fnu_9 = %lf", fnu_18, fnu_12, fnu_9);

        // Store it eventually:
        if (!f12AlreadySet)
        {
            FAIL(SetPropertyValue(vobsSTAR_PHOT_FLUX_IR_12, fnu_12, vobsSTAR_COMPUTED_PROP));
        }
        // Compute Mag N:
        magN = 4.1 - 2.5 * log10(fnu_12 / 0.89);

        logTest("IR Fluxes: computed magN = %lf", magN);

        // Store it if not set:
        FAIL(SetPropertyValue(vobsSTAR_PHOT_JHN_N, magN, vobsSTAR_COMPUTED_PROP));

        // store s9 if void:
        if (has9 == mcsFALSE)
        {
            FAIL(SetPropertyValue(vobsSTAR_PHOT_FLUX_IR_09, fnu_9, vobsSTAR_COMPUTED_PROP));
        }

        // compute s_12 error etc, if s18_err is present:
        if (hase_18 == mcsTRUE)
        {
            SUCCESS_DO(alxComputeFluxesFromAkari18(Teff, &e_fnu_18, &e_fnu_12, &e_fnu_9), logTest("IR Fluxes: Skipping (akari internal error)."));

            // Store it eventually:
            if (!e_f12AlreadySet)
            {
                FAIL(SetPropertyValue(vobsSTAR_PHOT_FLUX_IR_12_ERROR, e_fnu_12, vobsSTAR_COMPUTED_PROP));
            }
            // store e_s9 if void:
            if (hase_9 == mcsFALSE)
            {
                FAIL(SetPropertyValue(vobsSTAR_PHOT_FLUX_IR_09_ERROR, e_fnu_9, vobsSTAR_COMPUTED_PROP));
            }
        }
    }

    return mcsSUCCESS;
}

mcsCOMPL_STAT sclsvrCALIBRATOR::CheckParallax()
{
    logTrace("sclsvrCALIBRATOR::CheckParallax()");

    mcsDOUBLE parallax;
    vobsSTAR_PROPERTY* property = GetProperty(vobsSTAR_POS_PARLX_TRIG);

    // If parallax of the star if known
    if (IsPropertySet(property) == mcsTRUE)
    {
        // Check parallax
        mcsDOUBLE parallaxError = -1.0;
        GetPropertyValue(property, &parallax);

        property = GetProperty(vobsSTAR_POS_PARLX_TRIG_ERROR);

        // Get error
        if (IsPropertySet(property) == mcsTRUE)
        {
            GetPropertyValue(property, &parallaxError);

            // If parallax is negative 
            if (parallax <= 0.0)
            {
                logTest("parallax %.2lf(%.2lf) is not valid...", parallax, parallaxError);

                // Clear parallax values; invalid parallax is not shown to user
                ClearPropertyValue(vobsSTAR_POS_PARLX_TRIG);
                ClearPropertyValue(vobsSTAR_POS_PARLX_TRIG_ERROR);
            }
            else if (parallax < 1.0)
            {
                // If parallax is less than 1 mas 
                logTest("parallax %.2lf(%.2lf) less than 1 mas...", parallax, parallaxError);

                // Clear parallax values; invalid parallax is not shown to user
                ClearPropertyValue(vobsSTAR_POS_PARLX_TRIG);
                ClearPropertyValue(vobsSTAR_POS_PARLX_TRIG_ERROR);
            }
            else if (parallaxError <= 0.0)
            {
                // If parallax error is invalid 
                logTest("parallax error %.2lf is not valid...", parallaxError);

                // Clear parallax values; invalid parallax is not shown to user
                ClearPropertyValue(vobsSTAR_POS_PARLX_TRIG);
                ClearPropertyValue(vobsSTAR_POS_PARLX_TRIG_ERROR);
            }
            else if ((parallaxError / parallax) >= 0.25)
            {
                // Note: precise such threshold 25% or 50% ...

                // If parallax error is too high 
                logTest("parallax %.2lf(%.2lf) is not valid...", parallax, parallaxError);
                // Clear parallax values; invalid parallax is not shown to user
                ClearPropertyValue(vobsSTAR_POS_PARLX_TRIG);
                ClearPropertyValue(vobsSTAR_POS_PARLX_TRIG_ERROR);
            }
            else
            {
                // parallax OK
                logTest("parallax %.2lf(%.2lf) is OK...", parallax, parallaxError);
            }
        }
        else
        {
            // If parallax error is unknown 
            logTest("parallax error is unknown...");

            // Clear parallax value; invalid parallax is not shown to user
            ClearPropertyValue(vobsSTAR_POS_PARLX_TRIG);
        }
    }

    return mcsSUCCESS;
}

/**
 * Fill the J, H and K COUSIN/CIT magnitude from the JOHNSON.
 *
 * @return  mcsSUCCESS on successful completion. Otherwise mcsFAILURE is
 * returned.
 */
mcsCOMPL_STAT sclsvrCALIBRATOR::ComputeCousinMagnitudes()
{
    logTrace("sclsvrCALIBRATOR::ComputeCousinMagnitudes()");

    // Define the Cousin as NaN
    mcsDOUBLE mIc = FP_NAN;
    mcsDOUBLE mJc = FP_NAN;
    mcsDOUBLE mHc = FP_NAN;
    mcsDOUBLE mKc = FP_NAN;


    // Define the prperties of the existing magnitude
    vobsSTAR_PROPERTY* magV = GetProperty(vobsSTAR_PHOT_JHN_V);
    vobsSTAR_PROPERTY* magJ = GetProperty(vobsSTAR_PHOT_JHN_J);
    vobsSTAR_PROPERTY* magH = GetProperty(vobsSTAR_PHOT_JHN_H);
    vobsSTAR_PROPERTY* magK = GetProperty(vobsSTAR_PHOT_JHN_K);


    // Read the COUSIN Ic band
    vobsSTAR_PROPERTY* magIc = GetProperty(vobsSTAR_PHOT_COUS_I);
    if (IsPropertySet(magIc) == mcsTRUE)
    {
        FAIL(GetPropertyValue(magIc, &mIc));
    }

    // Compute The COUSIN/CIT Kc band 
    if (IsPropertySet(magK) == mcsTRUE)
    {
        mcsDOUBLE mK;
        FAIL(GetPropertyValue(magK, &mK));

        if ((strcmp(magK->GetOrigin(), vobsCATALOG_MASS_ID) == 0))
        {
            // From 2MASS 
            // see Carpenter eq.12
            mKc = mK + 0.024;
        }
        else if ((strcmp(magK->GetOrigin(), vobsCATALOG_MERAND_ID) == 0))
        {
            // From Merand (actually 2MASS) 
            // see Carpenter eq.12
            mKc = mK + 0.024;
        }
        else if ((IsPropertySet(magJ) == mcsTRUE) &&
                 (strcmp(magK->GetOrigin(), vobsCATALOG_DENIS_JK_ID) == 0) &&
                 (strcmp(magJ->GetOrigin(), vobsCATALOG_DENIS_JK_ID) == 0))
        {
            // From J and K coming from Denis
            // see Carpenter, eq.12 and 16
            mcsDOUBLE mJ;
            FAIL(GetPropertyValue(magJ, &mJ));

            mKc = mK + 0.006 * (mJ - mK);
        }
        else if ((IsPropertySet(magV) == mcsTRUE) &&
                 (strcmp(magK->GetOrigin(), vobsCATALOG_LBSI_ID) == 0))
        {
            // Assume V and K in Johnson, compute Kc from Vj and (V-K)
            // see Bessel 1988, p 1135
            // Note that this formula should be exactly
            // inverted in alxComputeDiameter to get back (V-K)j
            mcsDOUBLE mV;
            FAIL(GetPropertyValue(magV, &mV));

            mKc = mV - (0.03 + 0.992 * (mV - mK));

        }
        else if ((IsPropertySet(magV) == mcsTRUE) &&
                 (strcmp(magK->GetOrigin(), vobsCATALOG_PHOTO_ID) == 0))
        {
            // Assume K in Johnson, compute Kc from V and (V-K)
            // Note that this formula should be exactly
            // inverted in alxComputeDiameter to get back (V-K)j
            mcsDOUBLE mV;
            FAIL(GetPropertyValue(magV, &mV));

            mKc = mV - (0.03 + 0.992 * (mV - mK));
        }
    }

    // Compute the COUSIN/CIT Hc from Kc and (H-K)
    if ((IsPropertySet(magH) == mcsTRUE) &&
        (IsPropertySet(magK) == mcsTRUE) &&
        (mKc != FP_NAN))
    {
        mcsDOUBLE mK;
        FAIL(GetPropertyValue(magK, &mK));

        mcsDOUBLE mH;
        FAIL(GetPropertyValue(magH, &mH));

        if ((strcmp(magH->GetOrigin(), vobsCATALOG_MASS_ID) == 0) &&
            (strcmp(magK->GetOrigin(), vobsCATALOG_MASS_ID) == 0))
        {
            // From (H-K) 2MASS
            // see Carpenter eq.15
            mHc = mKc + ((mH - mK) - 0.028) / 1.026;
        }
        else if ((strcmp(magH->GetOrigin(), vobsCATALOG_MERAND_ID) == 0) &&
                 (strcmp(magK->GetOrigin(), vobsCATALOG_MERAND_ID) == 0))
        {
            // From (H-K) Merand (actually same as 2MASS)
            // see Carpenter eq.15
            mHc = mKc + ((mH - mK) - 0.028) / 1.026;
        }
        else if ((strcmp(magH->GetOrigin(), vobsCATALOG_LBSI_ID) == 0) &&
                 (strcmp(magK->GetOrigin(), vobsCATALOG_LBSI_ID) == 0))
        {
            // From (H-K) LBSI, we assume LBSI in Johnson magnitude
            // see Bessel, p.1138
            mHc = mKc - 0.009 + 0.912 * (mH - mK);
        }
        else if ((strcmp(magH->GetOrigin(), vobsCATALOG_PHOTO_ID) == 0) &&
                 (strcmp(magK->GetOrigin(), vobsCATALOG_PHOTO_ID) == 0))
        {
            // From (H-K) PHOTO, we assume PHOTO in Johnson magnitude
            // see Bessel, p.1138
            mHc = mKc - 0.009 + 0.912 * (mH - mK);
        }
    }


    // Compute the COUSIN/CIT Jc from Kc and (J-K)
    if ((IsPropertySet(magJ) == mcsTRUE) &&
        (IsPropertySet(magK) == mcsTRUE) &&
        (mKc != FP_NAN))
    {
        mcsDOUBLE mK;
        FAIL(GetPropertyValue(magK, &mK));

        mcsDOUBLE mJ;
        FAIL(GetPropertyValue(magJ, &mJ));

        if ((strcmp(magJ->GetOrigin(), vobsCATALOG_MASS_ID) == 0) &&
            (strcmp(magK->GetOrigin(), vobsCATALOG_MASS_ID) == 0))
        {
            // From (J-K) 2MASS
            // see Carpenter eq 14
            mJc = mKc + ((mJ - mK) + 0.013) / 1.056;
        }
        else if ((strcmp(magJ->GetOrigin(), vobsCATALOG_MERAND_ID) == 0) &&
                 (strcmp(magK->GetOrigin(), vobsCATALOG_MERAND_ID) == 0))
        {
            // From (J-K) Merand, actually 2MASS
            // see Carpenter eq 14
            mJc = mKc + ((mJ - mK) + 0.013) / 1.056;
        }
        else if ((strcmp(magJ->GetOrigin(), vobsCATALOG_DENIS_JK_ID) == 0) &&
                 (strcmp(magK->GetOrigin(), vobsCATALOG_DENIS_JK_ID) == 0))
        {
            // From (J-K) DENIS
            // see Carpenter eq 14 and 17
            mJc = mKc + ((0.981 * (mJ - mK) + 0.023) + 0.013) / 1.056;
        }
        else if ((strcmp(magJ->GetOrigin(), vobsCATALOG_LBSI_ID) == 0) &&
                 (strcmp(magK->GetOrigin(), vobsCATALOG_LBSI_ID) == 0))
        {
            // From (J-K) LBSI, we assume LBSI in Johnson magnitude
            // see Bessel p.1136  This seems quite unprecise.
            mJc = mKc + 0.93 * (mJ - mK);
        }
        else if ((strcmp(magJ->GetOrigin(), vobsCATALOG_PHOTO_ID) == 0) &&
                 (strcmp(magK->GetOrigin(), vobsCATALOG_PHOTO_ID) == 0))
        {
            // From (J-K) PHOTO, we assume in Johnson magnitude
            // see Bessel p.1136  This seems quite unprecise.
            mJc = mKc + 0.93 * (mJ - mK);
        }

    }

    // Set the magnitudes 
    if (mKc != FP_NAN)
    {
        FAIL(SetPropertyValue(vobsSTAR_PHOT_COUS_K, mKc, vobsSTAR_COMPUTED_PROP));
    }
    if (mHc != FP_NAN)
    {
        FAIL(SetPropertyValue(vobsSTAR_PHOT_COUS_H, mHc, vobsSTAR_COMPUTED_PROP));
    }
    if (mJc != FP_NAN)
    {
        FAIL(SetPropertyValue(vobsSTAR_PHOT_COUS_J, mJc, vobsSTAR_COMPUTED_PROP));
    }

    // Verbose
    logTest("Cousin magnitudes: I = %0.3lf, J = %0.3lf, H = %0.3lf, K = %0.3lf",
            mIc, mJc, mHc, mKc);

    return mcsSUCCESS;
}

/**
 * Fill the J, H and K JOHNSON magnitude (actually the 2MASS system)
 * from the COUSIN/CIT magnitudes.
 *
 * @return  mcsSUCCESS on successful completion. Otherwise mcsFAILURE is
 * returned.
 */
mcsCOMPL_STAT sclsvrCALIBRATOR::ComputeJohnsonMagnitudes()
{
    logTrace("sclsvrCALIBRATOR::ComputeJohnsonMagnitudes()");

    // Define the Cousin as NaN
    /*    mcsDOUBLE mIcous = FP_NAN; */
    mcsDOUBLE mJcous = FP_NAN;
    mcsDOUBLE mHcous = FP_NAN;
    mcsDOUBLE mKcous = FP_NAN;
    mcsDOUBLE mI = FP_NAN;
    mcsDOUBLE mJ = FP_NAN;
    mcsDOUBLE mH = FP_NAN;
    mcsDOUBLE mK = FP_NAN;

    // TODO: convert also Icous => I johnson
    /*    vobsSTAR_PROPERTY* magI = GetProperty(vobsSTAR_PHOT_COUS_I); */
    vobsSTAR_PROPERTY* magJ = GetProperty(vobsSTAR_PHOT_COUS_J);
    vobsSTAR_PROPERTY* magH = GetProperty(vobsSTAR_PHOT_COUS_H);
    vobsSTAR_PROPERTY* magK = GetProperty(vobsSTAR_PHOT_COUS_K);

    // Convert K band from COUSIN CIT to 2MASS
    if (IsPropertySet(magK) == mcsTRUE)
    {
        FAIL(GetPropertyValue(magK, &mKcous));

        // See Carpenter, 2001: 2001AJ....121.2851C, eq.12
        mK = mKcous - 0.024;

        FAIL(SetPropertyValue(vobsSTAR_PHOT_JHN_K, mK, vobsSTAR_COMPUTED_PROP));
    }

    // Fill J band from COUSIN to 2MASS
    if ((IsPropertySet(magJ) == mcsTRUE) && (IsPropertySet(magK) == mcsTRUE))
    {
        FAIL(GetPropertyValue(magJ, &mJcous));
        FAIL(GetPropertyValue(magK, &mKcous));

        // See Carpenter, 2001: 2001AJ....121.2851C, eq.12 and eq.14
        mJ = 1.056 * mJcous - 0.056 * mKcous - 0.037;

        FAIL(SetPropertyValue(vobsSTAR_PHOT_JHN_J, mJ, vobsSTAR_COMPUTED_PROP));
    }

    // Fill H band from COUSIN to 2MASS
    if ((IsPropertySet(magH) == mcsTRUE) && (IsPropertySet(magK) == mcsTRUE))
    {
        FAIL(GetPropertyValue(magH, &mHcous));
        FAIL(GetPropertyValue(magK, &mKcous));

        // See Carpenter, 2001: 2001AJ....121.2851C, eq.12 and eq.15
        mH = 1.026 * mHcous - 0.026 * mKcous + 0.004;

        FAIL(SetPropertyValue(vobsSTAR_PHOT_JHN_H, mH, vobsSTAR_COMPUTED_PROP));
    }

    // Verbose
    logTest("Johnson magnitudes: I = %0.3lf, J = %0.3lf, H = %0.3lf, K = %0.3lf",
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
        AddPropertyMeta(sclsvrCALIBRATOR_POS_GAL_LAT, "GLAT", vobsFLOAT_PROPERTY, "deg", "%.2lf", NULL,
                        "Galactic Latitude");
        AddPropertyMeta(sclsvrCALIBRATOR_POS_GAL_LON, "GLON", vobsFLOAT_PROPERTY, "deg", "%.2lf", NULL,
                        "Galactic Longitude");

        /* computed diameters */
        AddPropertyMeta(sclsvrCALIBRATOR_DIAM_BV, "diam_bv", vobsFLOAT_PROPERTY, "mas", NULL, NULL,
                        "B-V Diameter");
        AddPropertyMeta(sclsvrCALIBRATOR_DIAM_BV_ERROR, "e_diam_bv", vobsFLOAT_PROPERTY, "mas", NULL, NULL,
                        "Error on B-V Diameter");

        AddPropertyMeta(sclsvrCALIBRATOR_DIAM_VR, "diam_vr", vobsFLOAT_PROPERTY, "mas", NULL, NULL,
                        "V-R Diameter");
        AddPropertyMeta(sclsvrCALIBRATOR_DIAM_VR_ERROR, "e_diam_vr", vobsFLOAT_PROPERTY, "mas", NULL, NULL,
                        "Error on V-R Diameter");

        AddPropertyMeta(sclsvrCALIBRATOR_DIAM_VK, "diam_vk", vobsFLOAT_PROPERTY, "mas", NULL, NULL,
                        "V-K Diameter");
        AddPropertyMeta(sclsvrCALIBRATOR_DIAM_VK_ERROR, "e_diam_vk", vobsFLOAT_PROPERTY, "mas", NULL, NULL,
                        "Error on V-K Diameter");

        AddPropertyMeta(sclsvrCALIBRATOR_DIAM_IJ, "diam_ij", vobsFLOAT_PROPERTY, "mas", NULL, NULL,
                        "I-J Diameter");
        AddPropertyMeta(sclsvrCALIBRATOR_DIAM_IJ_ERROR, "e_diam_ij", vobsFLOAT_PROPERTY, "mas", NULL, NULL,
                        "Error on I-J Diameter");

        AddPropertyMeta(sclsvrCALIBRATOR_DIAM_IK, "diam_ik", vobsFLOAT_PROPERTY, "mas", NULL, NULL,
                        "I-K Diameter");
        AddPropertyMeta(sclsvrCALIBRATOR_DIAM_IK_ERROR, "e_diam_ik", vobsFLOAT_PROPERTY, "mas", NULL, NULL,
                        "Error on I-K Diameter");

        AddPropertyMeta(sclsvrCALIBRATOR_DIAM_JK, "diam_jk", vobsFLOAT_PROPERTY, "mas", NULL, NULL,
                        "J-K Diameter");
        AddPropertyMeta(sclsvrCALIBRATOR_DIAM_JK_ERROR, "e_diam_jk", vobsFLOAT_PROPERTY, "mas", NULL, NULL,
                        "Error on J-K Diameter");

        AddPropertyMeta(sclsvrCALIBRATOR_DIAM_JH, "diam_jh", vobsFLOAT_PROPERTY, "mas", NULL, NULL,
                        "J-H Diameter");
        AddPropertyMeta(sclsvrCALIBRATOR_DIAM_JH_ERROR, "e_diam_jh", vobsFLOAT_PROPERTY, "mas", NULL, NULL,
                        "Error on J-H Diameter");

        AddPropertyMeta(sclsvrCALIBRATOR_DIAM_HK, "diam_hk", vobsFLOAT_PROPERTY, "mas", NULL, NULL,
                        "H-K Diameter");
        AddPropertyMeta(sclsvrCALIBRATOR_DIAM_HK_ERROR, "e_diam_hk", vobsFLOAT_PROPERTY, "mas", NULL, NULL,
                        "Error on H-K Diameter");

        /* mean diameter */
        AddPropertyMeta(sclsvrCALIBRATOR_DIAM_MEAN, "diam_mean", vobsFLOAT_PROPERTY, "mas", NULL, NULL,
                        "Mean Diameter from the IR Magnitude versus Color Indices Calibrations");
        AddPropertyMeta(sclsvrCALIBRATOR_DIAM_MEAN_ERROR, "e_diam_mean", vobsFLOAT_PROPERTY, "mas", NULL, NULL,
                        "Estimated Error on Mean Diameter");

        /* diameter quality (OK | NOK) */
        AddPropertyMeta(sclsvrCALIBRATOR_DIAM_FLAG, "diamFlag", vobsSTRING_PROPERTY);

        /* information about the diameter quality */
        AddPropertyMeta(sclsvrCALIBRATOR_DIAM_FLAG_INFO, "diamFlagInfo", vobsSTRING_PROPERTY, NULL, NULL, NULL,
                        "Information related to the diamFlag value");

        /* Teff / Logg determined from spectral type */
        AddPropertyMeta(sclsvrCALIBRATOR_TEFF_SPTYP, "Teff_SpType", vobsFLOAT_PROPERTY, NULL, NULL, NULL,
                        "Effective Temperature adopted from Spectral Type");
        AddPropertyMeta(sclsvrCALIBRATOR_LOGG_SPTYP, "logg_SpType", vobsFLOAT_PROPERTY, NULL, NULL, NULL,
                        "Gravity adopted from Spectral Type");

        /* uniform disk diameters */
        AddPropertyMeta(sclsvrCALIBRATOR_UD_U, "UD_U", vobsFLOAT_PROPERTY, "mas", NULL, NULL,
                        "U-band Uniform-Disk Diameter");
        AddPropertyMeta(sclsvrCALIBRATOR_UD_B, "UD_B", vobsFLOAT_PROPERTY, "mas", NULL, NULL,
                        "B-band Uniform-Disk Diameter");
        AddPropertyMeta(sclsvrCALIBRATOR_UD_V, "UD_V", vobsFLOAT_PROPERTY, "mas", NULL, NULL,
                        "V-band Uniform-Disk Diameter");
        AddPropertyMeta(sclsvrCALIBRATOR_UD_R, "UD_R", vobsFLOAT_PROPERTY, "mas", NULL, NULL,
                        "R-band Uniform-Disk Diameter");
        AddPropertyMeta(sclsvrCALIBRATOR_UD_I, "UD_I", vobsFLOAT_PROPERTY, "mas", NULL, NULL,
                        "I-band Uniform-Disk Diameter");
        AddPropertyMeta(sclsvrCALIBRATOR_UD_J, "UD_J", vobsFLOAT_PROPERTY, "mas", NULL, NULL,
                        "J-band Uniform-Disk Diameter");
        AddPropertyMeta(sclsvrCALIBRATOR_UD_H, "UD_H", vobsFLOAT_PROPERTY, "mas", NULL, NULL,
                        "H-band Uniform-Disk Diameter");
        AddPropertyMeta(sclsvrCALIBRATOR_UD_K, "UD_K", vobsFLOAT_PROPERTY, "mas", NULL, NULL,
                        "K-band Uniform-Disk Diameter");
        AddPropertyMeta(sclsvrCALIBRATOR_UD_L, "UD_L", vobsFLOAT_PROPERTY, "mas", NULL, NULL,
                        "L-band Uniform-Disk Diameter");
        AddPropertyMeta(sclsvrCALIBRATOR_UD_N, "UD_N", vobsFLOAT_PROPERTY, "mas", NULL, NULL,
                        "N-band Uniform-Disk Diameter");

        /* extinction ratio related to interstellar absorption (faint) */
        AddPropertyMeta(sclsvrCALIBRATOR_EXTINCTION_RATIO, "Av", vobsFLOAT_PROPERTY, NULL, NULL, NULL,
                        "Visual Interstellar Absorption");

        /* square visibility */
        AddPropertyMeta(sclsvrCALIBRATOR_VIS2, "vis2", vobsFLOAT_PROPERTY, NULL, NULL, NULL,
                        "Squared Visibility");
        AddPropertyMeta(sclsvrCALIBRATOR_VIS2_ERROR, "vis2Err", vobsFLOAT_PROPERTY, NULL, NULL, NULL,
                        "Error on Squared Visibility");

        /* square visibility at 8 and 13 mu (midi) */
        AddPropertyMeta(sclsvrCALIBRATOR_VIS2_8, "vis2(8mu)", vobsFLOAT_PROPERTY, NULL, NULL, NULL,
                        "Squared Visibility at 8 microns");
        AddPropertyMeta(sclsvrCALIBRATOR_VIS2_8_ERROR, "vis2Err(8mu)", vobsFLOAT_PROPERTY, NULL, NULL, NULL,
                        "Error on Squared Visibility at 8 microns");

        AddPropertyMeta(sclsvrCALIBRATOR_VIS2_13, "vis2(13mu)", vobsFLOAT_PROPERTY, NULL, NULL, NULL,
                        "Squared Visibility at 13 microns");
        AddPropertyMeta(sclsvrCALIBRATOR_VIS2_13_ERROR, "vis2Err(13mu)", vobsFLOAT_PROPERTY, NULL, NULL, NULL,
                        "Error on Squared Visibility at 13 microns");

        /* distance to the science object */
        AddPropertyMeta(sclsvrCALIBRATOR_DIST, "dist", vobsFLOAT_PROPERTY, "deg", NULL, NULL,
                        "Calibrator to Science object Angular Distance");

        /* corrected spectral type */
        AddPropertyMeta(sclsvrCALIBRATOR_SP_TYPE, "SpType_JMMC", vobsSTRING_PROPERTY, NULL, NULL, NULL,
                        "Corrected spectral type by the JMMC");

        // End of Meta data

        sclsvrCALIBRATOR::sclsvrCALIBRATOR_PropertyMetaEnd = vobsSTAR::vobsStar_PropertyMetaList.size();

        logTest("sclsvrCALIBRATOR has defined %d properties.", sclsvrCALIBRATOR::sclsvrCALIBRATOR_PropertyMetaEnd);

        if (sclsvrCALIBRATOR::sclsvrCALIBRATOR_PropertyMetaEnd != sclsvrCALIBRATOR_MAX_PROPERTIES)
        {
            logWarning("sclsvrCALIBRATOR_MAX_PROPERTIES constant is incorrect: %d != %d",
                       sclsvrCALIBRATOR_MAX_PROPERTIES, sclsvrCALIBRATOR::sclsvrCALIBRATOR_PropertyMetaEnd);
        }

        initializeIndex();

        sclsvrCALIBRATOR::sclsvrCALIBRATOR_PropertyIdxInitialized = true;
    }

    // Add properties only if missing:
    if (NbProperties() <= sclsvrCALIBRATOR::sclsvrCALIBRATOR_PropertyMetaBegin)
    {
        const vobsSTAR_PROPERTY_META* meta;
        for (int i = sclsvrCALIBRATOR::sclsvrCALIBRATOR_PropertyMetaBegin; i < sclsvrCALIBRATOR::sclsvrCALIBRATOR_PropertyMetaEnd; i++)
        {
            meta = GetPropertyMeta(i);

            if (meta != NULL)
            {
                AddProperty(meta);
            }
        }
    }

    return mcsSUCCESS;
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
