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


/* maximum number of properties (139) */
#define sclsvrCALIBRATOR_MAX_PROPERTIES 139

/**
 * Convenience macros
 */
#define SetComputedPropWithError(propId, propErrId, alxDATA)                                                            \
if (alxDATA.isSet == mcsTRUE)                                                                                           \
{                                                                                                                       \
    FAIL(SetPropertyValue(propId,    alxDATA.value, vobsSTAR_COMPUTED_PROP, (vobsCONFIDENCE_INDEX) alxDATA.confIndex)); \
    FAIL(SetPropertyValue(propErrId, alxDATA.error, vobsSTAR_COMPUTED_PROP, (vobsCONFIDENCE_INDEX) alxDATA.confIndex)); \
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
mcsCOMPL_STAT sclsvrCALIBRATOR::Complete(const sclsvrREQUEST &request, miscoDYN_BUF &msgInfo)
{
    // Reset the buffer that contains the diamFlagInfo string
    FAIL(msgInfo.Reset());

    mcsSTRING64 starId;

    // Get Star ID
    FAIL(GetId(starId, sizeof (starId)));

    logTest("----- Complete: star '%s'", starId);

    // Check parallax: This will set the property
    // vobsSTAR_POS_PARLX_TRIG_FLAG to 'OK' or 'NOK'
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
    if (IsParallaxOk() == mcsTRUE)
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
    if (IsParallaxOk() == mcsTRUE)
    {
        FAIL(ComputeMissingMagnitude(request.IsBright()));
    }
    else
    {
        logTest("parallax is unknown; do not compute missing magnitude");
    }

    // Compute J, H, K JOHNSON magnitude (2MASS) from COUSIN
    FAIL(ComputeJohnsonMagnitudes());

    // If N-band scenario, we don't compute diameter
    // we only use those from MIDI
    const char* band = request.GetSearchBand();
    if ( strcmp(band, "N") == 0 )
    {
        FAIL(SetPropertyValue(sclsvrCALIBRATOR_DIAM_FLAG, "OK", vobsSTAR_COMPUTED_PROP));
    }
    else
    {
        FAIL(ComputeAngularDiameter(msgInfo));
    }

    // Discard the diameter if bright and no plx
    // To be discussed 2013-04-18
    if ( request.IsBright() == mcsTRUE &&
	 IsParallaxOk() == mcsFALSE )
    {
        logTest("parallax is unknown; diameter is NOK (bright mode)", starId);

	FAIL(SetPropertyValue(sclsvrCALIBRATOR_DIAM_FLAG, "NOK", vobsSTAR_COMPUTED_PROP,
			      GetPropertyConfIndex(sclsvrCALIBRATOR_DIAM_FLAG), mcsTRUE));

	msgInfo.AppendString(" BRIGHT_PLX_NOK");
        FAIL(SetPropertyValue(sclsvrCALIBRATOR_DIAM_FLAG_INFO, msgInfo.GetBuffer(), vobsSTAR_COMPUTED_PROP,
			      GetPropertyConfIndex(sclsvrCALIBRATOR_DIAM_FLAG_INFO), mcsTRUE));
    }

    // Compute visibility and visibility error
    FAIL(ComputeVisibility(request));

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
 * Fill the given magnitudes using given property ids
 * @param magnitudes alxMAGNITUDES struct to fill
 * @param magPropertyId property ids
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is
 * returned.
 */
mcsCOMPL_STAT sclsvrCALIBRATOR::ExtractMagnitude(alxMAGNITUDES &magnitudes,
                                                 const char** magIds,
                                                 const char** magErrIds)
{
    vobsSTAR_PROPERTY* property;

    // For each magnitude
    for (int band = 0; band < alxNB_BANDS; band++)
    {
        property = GetProperty(magIds[band]);

        // Get the magnitude value
        if (IsPropertySet(property) == mcsTRUE)
        {
            FAIL(GetPropertyValue(property, &magnitudes[band].value));
            magnitudes[band].isSet = mcsTRUE;
            magnitudes[band].confIndex = (alxCONFIDENCE_INDEX) property->GetConfidenceIndex();

            property = GetProperty(magErrIds[band]);

            // Get the error value
            if (IsPropertySet(property) == mcsTRUE)
            {
                FAIL(GetPropertyValue(property, &magnitudes[band].error));
            }
            else
            {
                magnitudes[band].error = 0.0;
            }
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
    static const char* magErrIds[alxNB_BANDS] = {
                                                 vobsSTAR_PHOT_JHN_B_ERROR,
                                                 vobsSTAR_PHOT_JHN_V_ERROR,
                                                 vobsSTAR_PHOT_JHN_R_ERROR,
                                                 vobsSTAR_PHOT_COUS_I_ERROR,
                                                 vobsSTAR_PHOT_COUS_J_ERROR,
                                                 vobsSTAR_PHOT_COUS_H_ERROR,
                                                 vobsSTAR_PHOT_COUS_K_ERROR,
                                                 vobsSTAR_PHOT_JHN_L_ERROR,
                                                 vobsSTAR_PHOT_JHN_M_ERROR
    };

    alxMAGNITUDES magnitudes;
    FAIL(ExtractMagnitude(magnitudes, magIds, magErrIds));

    /* Print out results */
    alxLogTestMagnitudes("Initial magnitudes:", "", magnitudes);

    // Get the extinction ratio
    mcsDOUBLE Av, e_Av;
    FAIL(GetPropertyValue(sclsvrCALIBRATOR_EXTINCTION_RATIO, &Av));
    FAIL(GetPropertyValue(sclsvrCALIBRATOR_EXTINCTION_RATIO_ERROR, &e_Av));

    // Compute corrected magnitude
    // (remove the expected interstellar absorption)
    FAIL(alxComputeCorrectedMagnitudes("(Av)", Av, e_Av, magnitudes));

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
    FAIL(alxComputeApparentMagnitudes(Av, magnitudes));

    // Set back the computed magnitude. Already existing magnitudes are not
    // overwritten. 
    for (int band = 0; band < alxNB_BANDS; band++)
    {
        if (magnitudes[band].isSet == mcsTRUE)
        {
            // note: use SetComputedPropWithError when magnitude error is computed:
            FAIL(SetPropertyValue(magIds[band], magnitudes[band].value, vobsSTAR_COMPUTED_PROP,
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
    FAIL_FALSE_DO(IsParallaxOk(), errAdd(sclsvrERR_MISSING_PROPERTY, vobsSTAR_POS_PARLX_TRIG, "interstellar absorption"));

    mcsDOUBLE plx, e_plx, gLat, gLon;
    mcsDOUBLE Av, e_Av;
    vobsSTAR_PROPERTY* property;

    // Get the value of the parallax
    property = GetProperty(vobsSTAR_POS_PARLX_TRIG);
    FAIL(GetPropertyValue(property, &plx));

    // Get the value of the parallax error
    property = GetProperty(vobsSTAR_POS_PARLX_TRIG_ERROR);
    FAIL(GetPropertyValue(property, &e_plx));

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

    // Set extinction ratio property
    FAIL(SetPropertyValue(sclsvrCALIBRATOR_EXTINCTION_RATIO, Av, vobsSTAR_COMPUTED_PROP));

    // Set the error
    FAIL(SetPropertyValue(sclsvrCALIBRATOR_EXTINCTION_RATIO_ERROR, e_Av, vobsSTAR_COMPUTED_PROP));

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
    static const char* magErrIds[alxNB_SED_BAND] = {
                                                    vobsSTAR_PHOT_JHN_B_ERROR,
                                                    vobsSTAR_PHOT_JHN_V_ERROR,
                                                    vobsSTAR_PHOT_JHN_J_ERROR,
                                                    vobsSTAR_PHOT_JHN_H_ERROR,
                                                    vobsSTAR_PHOT_JHN_K_ERROR
    };
    alxDATA magnitudes[alxNB_SED_BAND];

    // LBO: may use ExtractMagnitudes ?
    vobsSTAR_PROPERTY* property;
    for (int band = 0; band < alxNB_SED_BAND; band++)
    {
        property = GetProperty(magIds[band]);

        if (IsPropertySet(property) == mcsFALSE)
        {
            alxDATAClear(magnitudes[band]);
            continue;
        }

        /* Extract value and confidence index */
        FAIL(GetPropertyValue(property, &magnitudes[band].value));
        magnitudes[band].isSet = mcsTRUE;
        magnitudes[band].confIndex = (alxCONFIDENCE_INDEX) property->GetConfidenceIndex();

        /* Extract error or put 0.1mag by default */
        mcsDOUBLE error = 0.1;
        property = GetProperty(magErrIds[band]);

        if (IsPropertySet(property) == mcsTRUE)
        {
            FAIL(GetPropertyValue(property, &error));
        }

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
    if (IsPropertySet(sclsvrCALIBRATOR_EXTINCTION_RATIO) == mcsTRUE)
    {
        FAIL(GetPropertyValue(sclsvrCALIBRATOR_EXTINCTION_RATIO, &Av));
        FAIL(GetPropertyValue(sclsvrCALIBRATOR_EXTINCTION_RATIO_ERROR, &e_Av));
    }
    else
    {
        Av = 0.5;
        e_Av = 2.0;
    }

    /* Perform the SED fitting */
    mcsDOUBLE bestDiam, lowerDiam, upperDiam, bestChi2, bestTeff, bestAv;
    FAIL(alxSedFitting(magnitudes, Av, e_Av, &bestDiam, &lowerDiam, &upperDiam,
                       &bestChi2, &bestTeff, &bestAv));

    /* Compute error as the maximum distance */
    mcsDOUBLE diamErr;
    diamErr = mcsMAX( fabs(upperDiam-bestDiam) , fabs(lowerDiam-bestDiam) );

    /* TODO:  define a confindence index for diameter, Teff and Av based on chi2,
       is V available, is Av known ... */

    /* Put values */
    FAIL(SetPropertyValue(sclsvrCALIBRATOR_SEDFIT_DIAM, bestDiam, vobsSTAR_COMPUTED_PROP));
    FAIL(SetPropertyValue(sclsvrCALIBRATOR_SEDFIT_DIAM_ERROR, diamErr, vobsSTAR_COMPUTED_PROP));
    FAIL(SetPropertyValue(sclsvrCALIBRATOR_SEDFIT_CHI2, bestChi2, vobsSTAR_COMPUTED_PROP));
    FAIL(SetPropertyValue(sclsvrCALIBRATOR_SEDFIT_TEFF, bestTeff, vobsSTAR_COMPUTED_PROP));
    FAIL(SetPropertyValue(sclsvrCALIBRATOR_SEDFIT_AV, bestAv, vobsSTAR_COMPUTED_PROP));

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
    // We will use these bands. PHOT_COUS bands
    // should have been prepared before. No check is done on wether
    // these magnitudes comes from computed value or directly from
    // catalogues
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
    static const char* magErrIds[alxNB_BANDS] = {
                                                 vobsSTAR_PHOT_JHN_B_ERROR,
                                                 vobsSTAR_PHOT_JHN_V_ERROR,
                                                 vobsSTAR_PHOT_JHN_R_ERROR,
                                                 vobsSTAR_PHOT_COUS_I_ERROR,
                                                 vobsSTAR_PHOT_COUS_J_ERROR,
                                                 vobsSTAR_PHOT_COUS_H_ERROR,
                                                 vobsSTAR_PHOT_COUS_K_ERROR,
                                                 vobsSTAR_PHOT_JHN_L_ERROR,
                                                 vobsSTAR_PHOT_JHN_M_ERROR
    };

    // Fill the magnitude structures
    alxMAGNITUDES magAvMin, magAvMax, magAv;
    FAIL(ExtractMagnitude(magAv,    magIds, magErrIds));
    FAIL(ExtractMagnitude(magAvMin, magIds, magErrIds));
    FAIL(ExtractMagnitude(magAvMax, magIds, magErrIds));

    // We now have mag = {Bj, Vj, Rj, Jc, Ic, Hc, Kc, Lj, Mj}
    alxLogTestMagnitudes("Extracted magnitudes:", "", magAvMin);

    // Find the Av interval to test
    mcsDOUBLE Av, e_Av, minAv, maxAv;
    if (IsPropertySet(sclsvrCALIBRATOR_EXTINCTION_RATIO) == mcsTRUE)
    {
        FAIL(GetPropertyValue(sclsvrCALIBRATOR_EXTINCTION_RATIO, &Av));
        FAIL(GetPropertyValue(sclsvrCALIBRATOR_EXTINCTION_RATIO_ERROR, &e_Av));
	minAv = mcsMAX( Av - e_Av, 0.0);
	maxAv = Av + e_Av;
    }
    else
    {
        Av = 0.05;
	minAv = 0.0;
	maxAv = 3.0;
    }
    
    // Structure to fill with diameters
    alxDIAMETERS diam, diamAv, diamAvMin, diamAvMax;

    // Compute diameter for Av
    FAIL(alxComputeCorrectedMagnitudes("(Av)  ", Av, 0.0, magAv));
    FAIL(alxComputeAngularDiameters("(Av)  ", magAv, diamAv));

    // Compute diameter for minAv
    FAIL(alxComputeCorrectedMagnitudes("(minAv)", minAv, 0.0, magAvMin));
    FAIL(alxComputeAngularDiameters("(minAv)", magAvMin, diamAvMin));

    // Compute diameter for maxAv
    FAIL(alxComputeCorrectedMagnitudes("(maxAv)", maxAv, 0.0, magAvMax));
    FAIL(alxComputeAngularDiameters("(maxAv)", magAvMax, diamAvMax));

    // Compute the final diameter and its error
    for (int band = 0; band < alxNB_DIAMS; band++)
    {
        alxDATAClear(diam[band]);

        if (diamAv[band].isSet == mcsTRUE && diamAvMin[band].isSet == mcsTRUE && diamAvMax[band].isSet == mcsTRUE)
        {
            diam[band].isSet = mcsTRUE;
            diam[band].value = diamAv[band].value;

            /* Uncertainty encompass the maximum distance+error to Avmin and Avmax */
            diam[band].error = mcsMAX( fabs(diamAvMax[band].value - diamAv[band].value) + diamAvMax[band].error,
        			       fabs(diamAvMin[band].value - diamAv[band].value) + diamAvMin[band].error );

	    /* Take the smallest confIndex */
            diam[band].confIndex = mcsMIN( diamAvMin[band].confIndex, diamAvMax[band].confIndex );
        }
    }

    logTest("Final diameters BV=%.3lf(%.3lf), VR=%.3lf(%.3lf), VK=%.3lf(%.3lf), "
            "IJ=%.3lf(%.3lf), IK=%.3lf(%.3lf), "
            "JH=%.3lf(%.3lf), JK=%.3lf(%.3lf), HK=%.3lf(%.3lf)",
            diam[alxB_V_DIAM].value, diam[alxB_V_DIAM].error,
            diam[alxV_R_DIAM].value, diam[alxV_R_DIAM].error,
            diam[alxV_K_DIAM].value, diam[alxV_K_DIAM].error,
            diam[alxI_J_DIAM].value, diam[alxI_J_DIAM].error,
            diam[alxI_K_DIAM].value, diam[alxI_K_DIAM].error,
            diam[alxJ_H_DIAM].value, diam[alxJ_H_DIAM].error,
            diam[alxJ_K_DIAM].value, diam[alxJ_K_DIAM].error,
            diam[alxH_K_DIAM].value, diam[alxH_K_DIAM].error);

    /* Write Diameters */
    SetComputedPropWithError(sclsvrCALIBRATOR_DIAM_BV, sclsvrCALIBRATOR_DIAM_BV_ERROR, diam[alxB_V_DIAM]);
    SetComputedPropWithError(sclsvrCALIBRATOR_DIAM_VR, sclsvrCALIBRATOR_DIAM_VR_ERROR, diam[alxV_R_DIAM]);
    SetComputedPropWithError(sclsvrCALIBRATOR_DIAM_VK, sclsvrCALIBRATOR_DIAM_VK_ERROR, diam[alxV_K_DIAM]);
    SetComputedPropWithError(sclsvrCALIBRATOR_DIAM_IJ, sclsvrCALIBRATOR_DIAM_IJ_ERROR, diam[alxI_J_DIAM]);
    SetComputedPropWithError(sclsvrCALIBRATOR_DIAM_IK, sclsvrCALIBRATOR_DIAM_IK_ERROR, diam[alxI_K_DIAM]);
    SetComputedPropWithError(sclsvrCALIBRATOR_DIAM_JH, sclsvrCALIBRATOR_DIAM_JH_ERROR, diam[alxJ_H_DIAM]);
    SetComputedPropWithError(sclsvrCALIBRATOR_DIAM_JK, sclsvrCALIBRATOR_DIAM_JK_ERROR, diam[alxJ_K_DIAM]);
    SetComputedPropWithError(sclsvrCALIBRATOR_DIAM_HK, sclsvrCALIBRATOR_DIAM_HK_ERROR, diam[alxH_K_DIAM]);

    // 3 diameters are required:
    const mcsUINT32 nbRequiredDiameters = 3;

    // Compute mean diameter. 
    alxDATA meanDiam, weightedMeanDiam, meanStdDev;

    FAIL(alxComputeMeanAngularDiameter(diam, &meanDiam, &weightedMeanDiam, &meanStdDev, nbRequiredDiameters, msgInfo.GetInternalMiscDYN_BUF()));

    // Write MEAN DIAMETER 
    if (meanDiam.isSet == mcsTRUE)
    {
        SetComputedPropWithError(sclsvrCALIBRATOR_DIAM_MEAN, sclsvrCALIBRATOR_DIAM_MEAN_ERROR, meanDiam);

        // meanDiam.confIndex is alxLOW_CONFIDENCE only if the mean is inconsistent with each individual (valid) diameters
        // diamFlag OK if diameters are consistent: 
        if (meanDiam.confIndex == alxCONFIDENCE_HIGH)
        {
            FAIL(SetPropertyValue(sclsvrCALIBRATOR_DIAM_FLAG, "OK", vobsSTAR_COMPUTED_PROP));
        }
        else
        {
            logTest("Computed diameters are not coherent between them; Mean diameter is not kept");
            FAIL(SetPropertyValue(sclsvrCALIBRATOR_DIAM_FLAG, "NOK", vobsSTAR_COMPUTED_PROP));
        }
    }

    // Write DIAM INFO
    mcsUINT32 storedBytes;
    FAIL(msgInfo.GetNbStoredBytes(&storedBytes));
    if (storedBytes > 0)
    {
        FAIL(SetPropertyValue(sclsvrCALIBRATOR_DIAM_FLAG_INFO, msgInfo.GetBuffer(), vobsSTAR_COMPUTED_PROP));
    }

    if (weightedMeanDiam.isSet == mcsTRUE)
    {
        SetComputedPropWithError(sclsvrCALIBRATOR_DIAM_WEIGHTED_MEAN, sclsvrCALIBRATOR_DIAM_WEIGHTED_MEAN_ERROR, weightedMeanDiam);
    }

    if (meanStdDev.isSet == mcsTRUE)
    {
        FAIL(SetPropertyValue(sclsvrCALIBRATOR_DIAM_STDDEV, meanStdDev.value, vobsSTAR_COMPUTED_PROP, (vobsCONFIDENCE_INDEX) meanStdDev.confIndex));
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
    property = GetProperty(sclsvrCALIBRATOR_DIAM_MEAN);

    // Does DIAM_MEAN exist
    SUCCESS_FALSE_DO(IsPropertySet(property), logTest("Compute UD - Skipping (DIAM_MEAN unknown)."));

    // Get DIAM_MEAN
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
    SUCCESS_DO(alxComputeUDFromLDAndSP(ld, Teff, LogG, &ud), logWarning("Aborting (error while computing UDs)."));

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
    mcsDOUBLE diam, diamError, baseMax, wavelength;
    alxVISIBILITIES visibilities;
    vobsCONFIDENCE_INDEX confidenceIndex = vobsCONFIDENCE_HIGH;

    // Get object diameter. First look at the diameters coming from catalog
    // Borde (UDDK), Merand (UDDK) and MIDI (DIAM12)
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

            // Set confidence index to high (value coming from catalog)
            confidenceIndex = vobsCONFIDENCE_HIGH;
        }
    }

    // If not found in catalog, use the computed one (if exist)
    if (found == mcsFALSE)
    {
        // If computed diameter is OK
        SUCCESS_COND_DO((IsDiameterOk() == mcsFALSE) || (IsPropertySet(sclsvrCALIBRATOR_DIAM_MEAN) == mcsFALSE),
                        logTest("Unknown mean diameter; could not compute visibility"));

        // Get mean diameter and associated error value
        property = GetProperty(sclsvrCALIBRATOR_DIAM_MEAN);
        FAIL(GetPropertyValue(property, &diam));
        FAIL(GetPropertyValue(sclsvrCALIBRATOR_DIAM_MEAN_ERROR, &diamError));

        // Get confidence index of computed diameter
        confidenceIndex = property->GetConfidenceIndex();
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
    // TODO: explain why
    FAIL(SetPropertyValue(sclsvrCALIBRATOR_DIAM_FLAG, "OK", vobsSTAR_COMPUTED_PROP));

    // Comput visibility with wlen = 8 and 13 um in case search band is N
    FAIL(alxComputeVisibility(diam, diamError, baseMax, 8.0, &visibilities));

    // Affect visibility property
    FAIL(SetPropertyValue(sclsvrCALIBRATOR_VIS2_8, visibilities.vis2, vobsSTAR_COMPUTED_PROP));
    FAIL(SetPropertyValue(sclsvrCALIBRATOR_VIS2_8_ERROR, visibilities.vis2Error, vobsSTAR_COMPUTED_PROP, confidenceIndex));

    FAIL(alxComputeVisibility(diam, diamError, baseMax, 13.0, &visibilities));

    // Affect visibility property
    FAIL(SetPropertyValue(sclsvrCALIBRATOR_VIS2_13, visibilities.vis2, vobsSTAR_COMPUTED_PROP));
    FAIL(SetPropertyValue(sclsvrCALIBRATOR_VIS2_13_ERROR, visibilities.vis2Error, vobsSTAR_COMPUTED_PROP, confidenceIndex));

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
        FAIL(GetPropertyValue(magB, &mB));
        FAIL(GetPropertyValue(magV, &mV));

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
    SUCCESS_FALSE_DO(_spectralType.isSet, logTest("Teff and LogG - Skipping (SpType unknown)."));

    mcsDOUBLE Teff = FP_NAN;
    mcsDOUBLE LogG = FP_NAN;

    //Get Teff 
    SUCCESS_DO(alxComputeTeffAndLoggFromSptype(&_spectralType, &Teff, &LogG),
               logTest("Teff and LogG - Skipping (alxComputeTeffAndLoggFromSptype() failed on this spectral type: '%s').", _spectralType.origSpType));

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
    bool plxOk = false;

    vobsSTAR_PROPERTY* property = GetProperty(vobsSTAR_POS_PARLX_TRIG);

    // If parallax of the star if known
    if (IsPropertySet(property) == mcsTRUE)
    {
        mcsDOUBLE parallax;

        // Check parallax
        FAIL(GetPropertyValue(property, &parallax));

        property = GetProperty(vobsSTAR_POS_PARLX_TRIG_ERROR);

        // Get error
        if (IsPropertySet(property) == mcsTRUE)
        {
            mcsDOUBLE parallaxError = -1.0;
            FAIL(GetPropertyValue(property, &parallaxError));

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
            else if ((parallaxError / parallax) >= 0.25)
            {
                // Note: precise such threshold 25% or 50% ...

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
    FAIL(SetPropertyValue(vobsSTAR_POS_PARLX_TRIG_FLAG, (plxOk) ? "OK" : "NOK", vobsSTAR_COMPUTED_PROP));

    return mcsSUCCESS;
}

/**
 * Return whether the parallax is OK or not.
 */
mcsLOGICAL sclsvrCALIBRATOR::IsParallaxOk() const
{
    vobsSTAR_PROPERTY* property = GetProperty(vobsSTAR_POS_PARLX_TRIG_FLAG);

    // If parallax flag has not been computed yet or is not OK, return mcsFALSE:
    if ((IsPropertySet(property) == mcsFALSE) || (strcmp(GetPropertyValue(property), "OK") != 0))
    {
        return mcsFALSE;
    }

    return mcsTRUE;
}

/**
 * Fill the J, H and K COUSIN/CIT magnitude from the JOHNSON.
 *
 * @return  mcsSUCCESS on successful completion. Otherwise mcsFAILURE is
 * returned.
 */
mcsCOMPL_STAT sclsvrCALIBRATOR::ComputeCousinMagnitudes()
{
    // Define the Cousin as NaN
    mcsDOUBLE mIc = FP_NAN;
    mcsDOUBLE eIc = FP_NAN;
    mcsDOUBLE mJc = FP_NAN;
    mcsDOUBLE eJc = FP_NAN;
    mcsDOUBLE mHc = FP_NAN;
    mcsDOUBLE eHc = FP_NAN;
    mcsDOUBLE mKc = FP_NAN;
    mcsDOUBLE eKc = FP_NAN;

    // Define the properties of the existing magnitude
    vobsSTAR_PROPERTY* magV = GetProperty(vobsSTAR_PHOT_JHN_V);
    vobsSTAR_PROPERTY* errV = GetProperty(vobsSTAR_PHOT_JHN_V_ERROR);
    vobsSTAR_PROPERTY* magJ = GetProperty(vobsSTAR_PHOT_JHN_J);
    vobsSTAR_PROPERTY* errJ = GetProperty(vobsSTAR_PHOT_JHN_J_ERROR);
    vobsSTAR_PROPERTY* magH = GetProperty(vobsSTAR_PHOT_JHN_H);
    vobsSTAR_PROPERTY* errH = GetProperty(vobsSTAR_PHOT_JHN_H_ERROR);
    vobsSTAR_PROPERTY* magK = GetProperty(vobsSTAR_PHOT_JHN_K);
    vobsSTAR_PROPERTY* errK = GetProperty(vobsSTAR_PHOT_JHN_K_ERROR);

    // Read the COUSIN Ic band
    vobsSTAR_PROPERTY* magIc = GetProperty(vobsSTAR_PHOT_COUS_I);
    if (IsPropertySet(magIc) == mcsTRUE)
    {
        FAIL(GetPropertyValue(magIc, &mIc));

        vobsSTAR_PROPERTY* errIc = GetProperty(vobsSTAR_PHOT_COUS_I_ERROR);
        if (IsPropertySet(errIc) == mcsTRUE)
        {
            FAIL(GetPropertyValue(errIc, &eIc));
        }
    }


    // Compute The COUSIN/CIT Kc band 
    if (IsPropertySet(magK) == mcsTRUE)
    {
        mcsDOUBLE mK, eK = 0.0;
        FAIL(GetPropertyValue(magK, &mK));

        if (IsPropertySet(errK) == mcsTRUE)
        {
            FAIL(GetPropertyValue(errK, &eK));
        }

        if ((strcmp(magK->GetOrigin(), vobsCATALOG_MASS_ID) == 0))
        {
            // From 2MASS 
            // see Carpenter eq.12
            mKc = mK + 0.024;
            eKc = eK;
        }
        else if ((strcmp(magK->GetOrigin(), vobsCATALOG_MERAND_ID) == 0))
        {
            // From Merand (actually 2MASS) 
            // see Carpenter eq.12
            mKc = mK + 0.024;
            eKc = eK;
        }
        else if ((IsPropertySet(magJ) == mcsTRUE) &&
                 (strcmp(magK->GetOrigin(), vobsCATALOG_DENIS_JK_ID) == 0) &&
                 (strcmp(magJ->GetOrigin(), vobsCATALOG_DENIS_JK_ID) == 0))
        {
            // From J and K coming from Denis
            // see Carpenter, eq.12 and 16
            mcsDOUBLE mJ, eJ = 0.0;
            FAIL(GetPropertyValue(magJ, &mJ));

            if (IsPropertySet(errJ) == mcsTRUE)
            {
                FAIL(GetPropertyValue(errJ, &eJ));
            }

            mKc = mK + 0.006 * (mJ - mK);
            eKc = 0.994 * eK + 0.006 * eJ;
        }
        else if ((IsPropertySet(magV) == mcsTRUE) &&
                 (strcmp(magK->GetOrigin(), vobsCATALOG_LBSI_ID) == 0))
        {
            // Assume V and K in Johnson, compute Kc from Vj and (V-K)
            // see Bessel 1988, p 1135
            // Note that this formula should be exactly
            // inverted in alxComputeDiameter to get back (V-K)j
            mcsDOUBLE mV, eV = 0.0;
            FAIL(GetPropertyValue(magV, &mV));

            if (IsPropertySet(errV) == mcsTRUE)
            {
                FAIL(GetPropertyValue(errV, &eV));
            }

            mKc = mV - (0.03 + 0.992 * (mV - mK));
            eKc = 0.992 * eK + 0.008 * eV;
        }
        else if ((IsPropertySet(magV) == mcsTRUE) &&
                 (strcmp(magK->GetOrigin(), vobsCATALOG_PHOTO_ID) == 0))
        {
            // Assume K in Johnson, compute Kc from V and (V-K)
            // Note that this formula should be exactly
            // inverted in alxComputeDiameter to get back (V-K)j
            mcsDOUBLE mV, eV = 0.0;
            FAIL(GetPropertyValue(magV, &mV));

            if (IsPropertySet(errV) == mcsTRUE)
            {
                FAIL(GetPropertyValue(errV, &eV));
            }

            mKc = mV - (0.03 + 0.992 * (mV - mK));
            eKc = 0.992 * eK + 0.008 * eV;
        }
    }


    // Compute the COUSIN/CIT Hc from Kc and (H-K)
    if ((IsPropertySet(magH) == mcsTRUE) &&
        (IsPropertySet(magK) == mcsTRUE) &&
        (mKc != FP_NAN))
    {
        mcsDOUBLE mK, eK = 0.0;
        FAIL(GetPropertyValue(magK, &mK));

        if (IsPropertySet(errK) == mcsTRUE)
        {
            FAIL(GetPropertyValue(errK, &eK));
        }

        mcsDOUBLE mH, eH = 0.0;
        FAIL(GetPropertyValue(magH, &mH));

        if (IsPropertySet(errH) == mcsTRUE)
        {
            FAIL(GetPropertyValue(errH, &eH));
        }

        if ((strcmp(magH->GetOrigin(), vobsCATALOG_MASS_ID) == 0) &&
            (strcmp(magK->GetOrigin(), vobsCATALOG_MASS_ID) == 0))
        {
            // From (H-K) 2MASS
            // see Carpenter eq.15
            mHc = mKc + ((mH - mK) - 0.028) / 1.026;
            eHc = eH; // TODO: 2s order correction
        }
        else if ((strcmp(magH->GetOrigin(), vobsCATALOG_MERAND_ID) == 0) &&
                 (strcmp(magK->GetOrigin(), vobsCATALOG_MERAND_ID) == 0))
        {
            // From (H-K) Merand (actually same as 2MASS)
            // see Carpenter eq.15
            mHc = mKc + ((mH - mK) - 0.028) / 1.026;
            eHc = eH; // TODO: 2s order correction 
        }
        else if ((strcmp(magH->GetOrigin(), vobsCATALOG_LBSI_ID) == 0) &&
                 (strcmp(magK->GetOrigin(), vobsCATALOG_LBSI_ID) == 0))
        {
            // From (H-K) LBSI, we assume LBSI in Johnson magnitude
            // see Bessel, p.1138
            mHc = mKc - 0.009 + 0.912 * (mH - mK);
            eHc = eH; // TODO: 2s order correction 
        }
        else if ((strcmp(magH->GetOrigin(), vobsCATALOG_PHOTO_ID) == 0) &&
                 (strcmp(magK->GetOrigin(), vobsCATALOG_PHOTO_ID) == 0))
        {
            // From (H-K) PHOTO, we assume PHOTO in Johnson magnitude
            // see Bessel, p.1138
            mHc = mKc - 0.009 + 0.912 * (mH - mK);
            eHc = eH; // TODO: 2s order correction
        }
    }


    // Compute the COUSIN/CIT Jc from Kc and (J-K)
    if ((IsPropertySet(magJ) == mcsTRUE) &&
        (IsPropertySet(magK) == mcsTRUE) &&
        (mKc != FP_NAN))
    {
        mcsDOUBLE mK, eK = 0.0;
        FAIL(GetPropertyValue(magK, &mK));

        if (IsPropertySet(errK) == mcsTRUE)
        {
            FAIL(GetPropertyValue(errK, &eK));
        }

        mcsDOUBLE mJ, eJ = 0.0;
        FAIL(GetPropertyValue(magJ, &mJ));

        if (IsPropertySet(errJ) == mcsTRUE)
        {
            FAIL(GetPropertyValue(errJ, &eJ));
        }

        if ((strcmp(magJ->GetOrigin(), vobsCATALOG_MASS_ID) == 0) &&
            (strcmp(magK->GetOrigin(), vobsCATALOG_MASS_ID) == 0))
        {
            // From (J-K) 2MASS
            // see Carpenter eq 14
            mJc = mKc + ((mJ - mK) + 0.013) / 1.056;
            eJc = eJ; // TODO: 2s order correction
        }
        else if ((strcmp(magJ->GetOrigin(), vobsCATALOG_MERAND_ID) == 0) &&
                 (strcmp(magK->GetOrigin(), vobsCATALOG_MERAND_ID) == 0))
        {
            // From (J-K) Merand, actually 2MASS
            // see Carpenter eq 14
            mJc = mKc + ((mJ - mK) + 0.013) / 1.056;
            eJc = eJ; // TODO: 2s order correction
        }
        else if ((strcmp(magJ->GetOrigin(), vobsCATALOG_DENIS_JK_ID) == 0) &&
                 (strcmp(magK->GetOrigin(), vobsCATALOG_DENIS_JK_ID) == 0))
        {
            // From (J-K) DENIS
            // see Carpenter eq 14 and 17
            mJc = mKc + ((0.981 * (mJ - mK) + 0.023) + 0.013) / 1.056;
            eJc = eJ; // TODO: 2s order correction
        }
        else if ((strcmp(magJ->GetOrigin(), vobsCATALOG_LBSI_ID) == 0) &&
                 (strcmp(magK->GetOrigin(), vobsCATALOG_LBSI_ID) == 0))
        {
            // From (J-K) LBSI, we assume LBSI in Johnson magnitude
            // see Bessel p.1136  This seems quite unprecise.
            mJc = mKc + 0.93 * (mJ - mK);
            eJc = eJ; // TODO: 2s order correction
        }
        else if ((strcmp(magJ->GetOrigin(), vobsCATALOG_PHOTO_ID) == 0) &&
                 (strcmp(magK->GetOrigin(), vobsCATALOG_PHOTO_ID) == 0))
        {
            // From (J-K) PHOTO, we assume in Johnson magnitude
            // see Bessel p.1136  This seems quite unprecise.
            mJc = mKc + 0.93 * (mJ - mK);
            eJc = eJ; // TODO: 2s order correction
        }
    }

    // Set the magnitudes and errors:
    if (mKc != FP_NAN)
    {
        FAIL(SetPropertyValue(vobsSTAR_PHOT_COUS_K, mKc, vobsSTAR_COMPUTED_PROP));
    }
    if (eKc != FP_NAN)
    {
        FAIL(SetPropertyValue(vobsSTAR_PHOT_COUS_K_ERROR, eKc, vobsSTAR_COMPUTED_PROP));
    }
    if (mHc != FP_NAN)
    {
        FAIL(SetPropertyValue(vobsSTAR_PHOT_COUS_H, mHc, vobsSTAR_COMPUTED_PROP));
    }
    if (eHc != FP_NAN)
    {
        FAIL(SetPropertyValue(vobsSTAR_PHOT_COUS_H_ERROR, eHc, vobsSTAR_COMPUTED_PROP));
    }
    if (mJc != FP_NAN)
    {
        FAIL(SetPropertyValue(vobsSTAR_PHOT_COUS_J, mJc, vobsSTAR_COMPUTED_PROP));
    }
    if (eJc != FP_NAN)
    {
        FAIL(SetPropertyValue(vobsSTAR_PHOT_COUS_J_ERROR, eJc, vobsSTAR_COMPUTED_PROP));
    }

    // Verbose
    logTest("Cousin magnitudes: I= %0.3lf (%0.3lf), J= %0.3lf (%0.3lf), H= %0.3lf (%0.3lf), K= %0.3lf (%0.3lf)",
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
    mcsDOUBLE mIcous = FP_NAN;
    mcsDOUBLE mJcous = FP_NAN;
    mcsDOUBLE mHcous = FP_NAN;
    mcsDOUBLE mKcous = FP_NAN;
    mcsDOUBLE mI = FP_NAN;
    mcsDOUBLE mJ = FP_NAN;
    mcsDOUBLE mH = FP_NAN;
    mcsDOUBLE mK = FP_NAN;

    vobsSTAR_PROPERTY* magI = GetProperty(vobsSTAR_PHOT_COUS_I);
    vobsSTAR_PROPERTY* magJ = GetProperty(vobsSTAR_PHOT_COUS_J);
    vobsSTAR_PROPERTY* magH = GetProperty(vobsSTAR_PHOT_COUS_H);
    vobsSTAR_PROPERTY* magK = GetProperty(vobsSTAR_PHOT_COUS_K);

    // Convert K band from COUSIN CIT to 2MASS
    if (IsPropertySet(magK) == mcsTRUE)
    {
        FAIL(GetPropertyValue(magK, &mKcous));

        // See Carpenter, 2001: 2001AJ....121.2851C, eq.12
        mK = mKcous - 0.024;

        FAIL(SetPropertyValue(vobsSTAR_PHOT_JHN_K, mK, vobsSTAR_COMPUTED_PROP, magK->GetConfidenceIndex()));
    }

    // Fill J band from COUSIN to 2MASS
    if ((IsPropertySet(magJ) == mcsTRUE) && (IsPropertySet(magK) == mcsTRUE))
    {
        FAIL(GetPropertyValue(magJ, &mJcous));
        FAIL(GetPropertyValue(magK, &mKcous));

        // See Carpenter, 2001: 2001AJ....121.2851C, eq.12 and eq.14
        mJ = 1.056 * mJcous - 0.056 * mKcous - 0.037;

        FAIL(SetPropertyValue(vobsSTAR_PHOT_JHN_J, mJ, vobsSTAR_COMPUTED_PROP,
                              min(magJ->GetConfidenceIndex(), magK->GetConfidenceIndex())));
    }

    // Fill H band from COUSIN to 2MASS
    if ((IsPropertySet(magH) == mcsTRUE) && (IsPropertySet(magK) == mcsTRUE))
    {
        FAIL(GetPropertyValue(magH, &mHcous));
        FAIL(GetPropertyValue(magK, &mKcous));

        // See Carpenter, 2001: 2001AJ....121.2851C, eq.12 and eq.15
        mH = 1.026 * mHcous - 0.026 * mKcous + 0.004;

        FAIL(SetPropertyValue(vobsSTAR_PHOT_JHN_H, mH, vobsSTAR_COMPUTED_PROP,
                              min(magH->GetConfidenceIndex(), magK->GetConfidenceIndex())));
    }

    // Fill I band from COUSIN to JOHNSON
    if ((IsPropertySet(magI) == mcsTRUE) && (IsPropertySet(magJ) == mcsTRUE))
    {
        FAIL(GetPropertyValue(magI, &mIcous));
        FAIL(GetPropertyValue(magJ, &mJcous));

        // Approximate conversion, JB. Le Bouquin
        mI = mIcous + 0.43 * (mJcous - mIcous) + 0.048;

        FAIL(SetPropertyValue(vobsSTAR_PHOT_JHN_I, mI, vobsSTAR_COMPUTED_PROP,
                              min(magI->GetConfidenceIndex(), magJ->GetConfidenceIndex())));
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

        /* weighted mean diameter */
        AddPropertyMeta(sclsvrCALIBRATOR_DIAM_WEIGHTED_MEAN, "diam_weighted_mean", vobsFLOAT_PROPERTY, "mas", NULL, NULL,
                        "Weighted mean diameter by inverse(diameter error)");
        AddPropertyMeta(sclsvrCALIBRATOR_DIAM_WEIGHTED_MEAN_ERROR, "e_diam_weighted_mean", vobsFLOAT_PROPERTY, "mas", NULL, NULL,
                        "Estimated Error on Weighted mean diameter");

        /* standard deviation on all diameters */
        AddPropertyMeta(sclsvrCALIBRATOR_DIAM_STDDEV, "diam_stddev", vobsFLOAT_PROPERTY, "mas", NULL, NULL,
                        "Standard deviation on mean diameter");

        /* diameter quality (OK | NOK) */
        AddPropertyMeta(sclsvrCALIBRATOR_DIAM_FLAG, "diamFlag", vobsSTRING_PROPERTY);

        /* information about the diameter quality */
        AddPropertyMeta(sclsvrCALIBRATOR_DIAM_FLAG_INFO, "diamFlagInfo", vobsSTRING_PROPERTY, NULL, NULL, NULL,
                        "Information related to the diamFlag value");

        /* Results from SED fitting */
        AddPropertyMeta(sclsvrCALIBRATOR_SEDFIT_CHI2, "chi2_SED", vobsFLOAT_PROPERTY, NULL, NULL, NULL,
                        "Reduced chi2 of the SED fitting (experimental)");
        AddPropertyMeta(sclsvrCALIBRATOR_SEDFIT_DIAM, "diam_SED", vobsFLOAT_PROPERTY, "mas", NULL, NULL,
                        "Diameter from SED fitting (experimental)");
        AddPropertyMeta(sclsvrCALIBRATOR_SEDFIT_DIAM_ERROR, "e_diam_SED", vobsFLOAT_PROPERTY, "mas", NULL, NULL,
                        "Diameter from SED fitting (experimental)");
        AddPropertyMeta(sclsvrCALIBRATOR_SEDFIT_TEFF, "Teff_SED", vobsFLOAT_PROPERTY, "K", NULL, NULL,
                        "Teff from SED fitting (experimental)");
        AddPropertyMeta(sclsvrCALIBRATOR_SEDFIT_AV, "Av_SED", vobsFLOAT_PROPERTY, NULL, NULL, NULL,
                        "Teff from SED fitting (experimental)");


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
        AddPropertyMeta(sclsvrCALIBRATOR_EXTINCTION_RATIO_ERROR, "e_Av", vobsFLOAT_PROPERTY, NULL, NULL, NULL,
                        "Error on Visual Interstellar Absorption");

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
 * Dump the property index
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned 
 */
mcsCOMPL_STAT sclsvrCALIBRATOR::DumpPropertyIndexAsXML()
{
    miscoDYN_BUF buffer;
    // Prepare buffer:
    FAIL(buffer.Reserve(40 * 1024));

    buffer.AppendLine("<?xml version=\"1.0\"?>\n\n");

    FAIL(buffer.AppendString("<index>\n"));
    FAIL(buffer.AppendString("  <name>sclsvrCALIBRATOR</name>\n"));

    vobsSTAR::DumpPropertyIndexAsXML(buffer, "vobsSTAR", 0, sclsvrCALIBRATOR::sclsvrCALIBRATOR_PropertyMetaBegin);
    vobsSTAR::DumpPropertyIndexAsXML(buffer, "sclsvrCALIBRATOR", sclsvrCALIBRATOR::sclsvrCALIBRATOR_PropertyMetaBegin, sclsvrCALIBRATOR::sclsvrCALIBRATOR_PropertyMetaEnd);

    FAIL(buffer.AppendString("</index>\n\n"));

    const char* fileName = "PropertyIndex_sclsvrCALIBRATOR.xml";

    logInfo("Saving property index XML description: %s", fileName);

    // Try to save the generated VOTable in the specified file as ASCII
    return buffer.SaveInASCIIFile(fileName);
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

