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


/* maximum number of properties in sclsvrCALIBRATOR instances */
#define sclsvrCALIBRATOR_MAX_PROPERTIES 130

/** Initialize static members */
int  sclsvrCALIBRATOR::sclsvrCALIBRATOR_PropertyMetaBegin      = -1;
int  sclsvrCALIBRATOR::sclsvrCALIBRATOR_PropertyMetaEnd        = -1;
bool sclsvrCALIBRATOR::sclsvrCALIBRATOR_PropertyIdxInitialized = false;

/** Initialize the property index used by sclsvrCALIBRATOR and vobsSTAR */
void sclsvrCalibratorBuildPropertyIndex()
{
    sclsvrCALIBRATOR initCalibrator;
}

/**
 * Class constructor.
 */
sclsvrCALIBRATOR::sclsvrCALIBRATOR()
{
    // Note: vobsSTAR() constructor adds star properties
    
    ReserveProperties(sclsvrCALIBRATOR_MAX_PROPERTIES);
    
    // Add calibrator star properties 
    AddProperties();
}

/**
 * Copy Constructor. Is it used ???
 */
sclsvrCALIBRATOR::sclsvrCALIBRATOR(vobsSTAR &star)
{
    ReserveProperties(sclsvrCALIBRATOR_MAX_PROPERTIES);
    
    (*this).vobsSTAR::operator=(star);

    // Add calibrator star properties 
    AddProperties(); 
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
mcsLOGICAL sclsvrCALIBRATOR::IsDiameterOk()
{
    // If diameter has not been computed yet
    if (IsPropertySet(sclsvrCALIBRATOR_DIAM_FLAG) == mcsFALSE)
    {
        return mcsFALSE;
    }

    // Get the flag, and test it
    const char* flag = GetPropertyValue(sclsvrCALIBRATOR_DIAM_FLAG);
    if (strcmp(flag, "OK") != 0)
    {
        return mcsFALSE;
    }

    return mcsTRUE;
}

/**
 * Compute real magnitudes, missing magnitudes and the angular diameter for
 * stars with parallax.
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is
 * returned.
 */
mcsCOMPL_STAT sclsvrCALIBRATOR::CompleteWithParallax(mcsLOGICAL isBright)
{
    logTrace("sclsvrCALIBRATOR::CompleteWithParallax()");

    // Get the extinction ratio
    if (ComputeExtinctionCoefficient() == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    // Compute Interstellar extinction
    if (ComputeInterstellarAbsorption(isBright) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    // Compute missing Magnitude
    if (ComputeMissingMagnitude(isBright) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    // Compute apparent magnitude
    if (ComputeApparentMagnitude(isBright) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    // Compute Angular Diameter
    if (ComputeAngularDiameter(isBright) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    return mcsSUCCESS;
}



/**
 * Compute real magnitudes, missing magnitudes and the angular diameter for
 * stars without parallax.
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is
 * returned.
 */
mcsCOMPL_STAT sclsvrCALIBRATOR::CompleteWithoutParallax()
{
    logTrace("sclsvrCALIBRATOR::CompleteWithoutParallax()");
    mcsLOGICAL isBright = mcsFALSE;

    // Compute Interstellar extinction
    if (ComputeInterstellarAbsorption(isBright) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    // Compute missing Magnitude
    if (ComputeMissingMagnitude(isBright) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    // Compute apparent magnitude
    if (ComputeApparentMagnitude(isBright) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    // Compute Angular Diameter
    if (ComputeAngularDiameter(isBright) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    return mcsSUCCESS;
}



/**
 * Complete the property of the calibrator.
 *
 * Method to complete calibrator properties by using several methods.
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is
 * returned.
 */
mcsCOMPL_STAT sclsvrCALIBRATOR::Complete(sclsvrREQUEST &request)
{
    mcsSTRING32 starId;
    // Get Star ID
    if (GetId(starId, sizeof(starId)) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    logTest("Complete: star '%s'", starId);

    // Check flag related to I magnitude
    // Note (2):
    // This flag is the concatenation of image and source flags, in hexadecimal
    // format.
    // For the image flag, the first two digits contain:
    // Bit 0 (0100) clouds during observation
    // Bit 1 (0200) electronic Read-Out problem
    // Bit 2 (0400) internal temperature problem
    // Bit 3 (0800) very bright star
    // Bit 4 (1000) bright star
    // Bit 5 (2000) stray light
    // Bit 6 (4000) unknown problem
    // For the source flag, the last two digits contain:
    // Bit 0 (0001) source might be a dust on mirror
    // Bit 1 (0002) source is a ghost detection of a bright star
    // Bit 2 (0004) source is saturated
    // Bit 3 (0008) source is multiple detect
    // Bit 4 (0010) reserved
    if (IsPropertySet(vobsSTAR_CODE_MISC_I) == mcsTRUE)
    {
        // Get Iflg value as string
        mcsSTRING32 IflgStr;
        strcpy(IflgStr, GetPropertyValue(vobsSTAR_CODE_MISC_I));

        // Convert it into integer; hexadecimal conversion
        int Iflg;
        sscanf(IflgStr, "%x", &Iflg);

        // Check if it is saturated or ther was clouds during observation
        if (((Iflg & 0x4) != 0) || ((Iflg & 0x100) != 0))
        {
            // Do not used I magnitude coming from Denis
            ClearPropertyValue(vobsSTAR_PHOT_COUS_I);
            ClearPropertyValue(vobsSTAR_CODE_MISC_I);
        }
    }

    // If a valid spectral type could be parsed
    if (ParseSpectralType() == mcsSUCCESS)
    {
        // Check for spectral binarity
        if (ComputeSpectralBinarity() == mcsFAILURE)
        {
            return mcsFAILURE;
        }

        // Fill in the Teff and LogG entries using the spectral type
        if (ComputeTeffLogg() == mcsFAILURE)
        {
            return mcsFAILURE;
        }
    }

    // Compute N Band and S_12 with AKARI
    if (ComputeIRFluxes() == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    // Check parallax
    mcsDOUBLE parallax;
    mcsLOGICAL parallaxIsOK = mcsFALSE;
    
    // If parallax of the star if known
    if (IsPropertySet(vobsSTAR_POS_PARLX_TRIG) == mcsTRUE)
    {
        // Check parallax
        mcsDOUBLE parallaxError = -1.0;
        GetPropertyValue(vobsSTAR_POS_PARLX_TRIG, &parallax);

        // Get error
        if (IsPropertySet(vobsSTAR_POS_PARLX_TRIG_ERROR) == mcsTRUE)
        {
            GetPropertyValue(vobsSTAR_POS_PARLX_TRIG_ERROR, &parallaxError);

            // If parallax is negative 
            if (parallax <= 0.) 
            {
                if (request.IsBright() == mcsTRUE)
                {
                    logTest("star '%s' - parallax %.2lf(%.2lf) is not valid; "
                            "could not compute diameter", starId, parallax, parallaxError);
                }
                else
                {
                    logTest("star '%s' - parallax %.2lf(%.2lf) is not valid...",
                            starId, parallax, parallaxError);
                }            
                // Clear parallax values; invalid parallax is not shown to user
                ClearPropertyValue(vobsSTAR_POS_PARLX_TRIG);
                ClearPropertyValue(vobsSTAR_POS_PARLX_TRIG_ERROR);
            }
            // If parallax is less than 1 mas 
            else if (parallax < 1.) 
            {
                if (request.IsBright() == mcsTRUE)
                {
                    logTest("star '%s' - parallax %.2lf(%.2lf) less than 1 mas; "
                            "could not compute diameter", starId, parallax, parallaxError);
                }
                else
                {
                    logTest("star '%s' - parallax %.2lf(%.2lf) less than 1 mas...",
                            starId, parallax, parallaxError);
                }            
                // Clear parallax values; invalid parallax is not shown to user
                ClearPropertyValue(vobsSTAR_POS_PARLX_TRIG);
                ClearPropertyValue(vobsSTAR_POS_PARLX_TRIG_ERROR);
            }
            // If parallax error is invalid 
            else if (parallaxError <= 0.0) 
            {
                if (request.IsBright() == mcsTRUE)
                {
                    logTest("star '%s' - parallax error %.2lf is not valid; "
                            "could not compute diameter", starId, parallaxError);
                }
                else
                {
                    logTest("star '%s' - parallax error %.2lf is not valid...", 
                            starId, parallaxError);
                }
                // Clear parallax values; invalid parallax is not shown to user
                ClearPropertyValue(vobsSTAR_POS_PARLX_TRIG);
                ClearPropertyValue(vobsSTAR_POS_PARLX_TRIG_ERROR);
            }
            // If parallax error is too high 
            else if ((parallaxError / parallax) >= 0.25)
            {
                if (request.IsBright() == mcsTRUE)
                {
                    logTest("star '%s' - parallax %.2lf(%.2lf) is not valid; "
                            "could not compute diameter", starId, parallax, parallaxError);
                }
                else
                {
                    logTest("star '%s' - parallax %.2lf(%.2lf) is not valid...", 
                            starId, parallax, parallaxError);
                }
                // Clear parallax values; invalid parallax is not shown to user
                ClearPropertyValue(vobsSTAR_POS_PARLX_TRIG);
                ClearPropertyValue(vobsSTAR_POS_PARLX_TRIG_ERROR);
            }
            // parallax OK
            else
            {
                parallaxIsOK = mcsTRUE;
                logTest("star '%s' - parallax %.2lf(%.2lf) is OK...", starId, 
                        parallax, parallaxError);
            }                
        }
        // If parallax error is unknown 
        else
        {
            if (request.IsBright() == mcsTRUE)
            {
                logTest("star '%s' - parallax error is unknown; "
                        "could not compute diameter", starId);
            }
            else
            {
                logTest("star '%s' - parallax error is unknown...", starId);
            }
            // Clear parallax value; invalid parallax is not shown to user
            ClearPropertyValue(vobsSTAR_POS_PARLX_TRIG);
        }
    }
    // parallax is unknown 
    else
    {
        if (request.IsBright() == mcsTRUE)
        {
            logTest("star '%s' - parallax is unknown; "
                    "could not compute diameter", starId); 
        }
        else
        {
            logTest("star '%s' - parallax is unknown", starId);
        }
    }

    // If the request should return bright starts
    if (request.IsBright() == mcsTRUE)
    {
        // Get the observed band
        const char* band = request.GetSearchBand();

        // Compute Galactic coordinates
        if (ComputeGalacticCoordinates() == mcsFAILURE)
        {
            return mcsFAILURE;
        }

        // If it is not the scenario for N band
        if (strcmp(band, "N") != 0)
        {
            // If parallax of the star if known
            if (parallaxIsOK == mcsTRUE)
            {
                CompleteWithParallax(mcsTRUE);
            }
        }
        else
        {
            SetPropertyValue(sclsvrCALIBRATOR_DIAM_FLAG, "OK", vobsSTAR_COMPUTED_PROP);
        }

        // Compute visibility and visibility error
        if (ComputeVisibility(request) == mcsFAILURE)
        {
            return mcsFAILURE;
        }
    }
    // If the search is faint
    else
    {
        // Compute Galactic coordinates
        if (ComputeGalacticCoordinates() == mcsFAILURE)
        {
            return mcsFAILURE;
        }

        // Convert in cousin magnitude
        if (ComputeCousinMagnitude() == mcsFAILURE)
        {
            return mcsFAILURE;
        }

        // If parallax is OK, compute real magnitudes, missing magnitudes and the
        // angular diameter
        if (parallaxIsOK == mcsTRUE)
        {
            if (CompleteWithParallax(mcsFALSE) == mcsFAILURE)
            {
                return mcsFAILURE;
            }

            // Compute visibility and visibility error
            if (ComputeVisibility(request) == mcsFAILURE)
            {
                return mcsFAILURE;
            }
        }
        // parallax unknown
        else
        {
            // Temporary stars with/without interstellar absorption
            sclsvrCALIBRATOR starWith;
            starWith.Update(* this);

            mcsLOGICAL useInterstellarAbsorption = mcsTRUE;

            // First, compute diameter and visibility without considering
            // interstellar absorption (i.e av=0)

            // Set extinction ratio property
            if (SetPropertyValue
                (sclsvrCALIBRATOR_EXTINCTION_RATIO, 0.0, vobsSTAR_COMPUTED_PROP) == mcsFAILURE)
            {
                return mcsFAILURE;
            }

            // Complete diameters without parallax
            if (CompleteWithoutParallax() == mcsFAILURE)
            {
                return mcsFAILURE;
            }

            // Compute Angular Diameter
            logTest("star '%s' - computing diameter without absorption...",
                    starId);

            // Compute visibility and visibility error
            if (ComputeVisibility(request) == mcsFAILURE)
            {
                return mcsFAILURE;
            }

            // If visibility has been computed, then compute now diameters and
            // visibility with an arbitrary interstellar absorption
            // it also depends on the useInterstellarAbsorption flag
            if ( (IsPropertySet(sclsvrCALIBRATOR_VIS2) == mcsTRUE) && 
                 (useInterstellarAbsorption == mcsTRUE))
            {
                // Do the same with the extinction ratio fixed to 3.0
                if (starWith.SetPropertyValue(sclsvrCALIBRATOR_EXTINCTION_RATIO, 3.0,
                                              vobsSTAR_COMPUTED_PROP) == mcsFAILURE)
                {
                    return mcsFAILURE;
                }

                // Complete diameters without parallax
                if (starWith.CompleteWithoutParallax() == mcsFAILURE)
                {
                    return mcsFAILURE;
                }

                // Compute Angular Diameter
                logTest("star '%s' - Computing diameter with absorption...", 
                        starId);

                // Compute visibility and visibility error
                if (starWith.ComputeVisibility(request) == mcsFAILURE)
                {
                    return mcsFAILURE;
                }

                // If visibility has been computed, compare result 
                if (starWith.IsPropertySet(sclsvrCALIBRATOR_VIS2) == mcsTRUE)
                {
                    // Get Visibility of the star (without absorption)
                    mcsDOUBLE vis2;
                    mcsDOUBLE vis2Err;
                    GetPropertyValue(sclsvrCALIBRATOR_VIS2, &vis2);
                    GetPropertyValue(sclsvrCALIBRATOR_VIS2_ERROR, &vis2Err);
                    
                    // Get Visibility of the star (with absorption)
                    mcsDOUBLE vis2A;
                    mcsDOUBLE vis2ErrA;
                    starWith.GetPropertyValue(sclsvrCALIBRATOR_VIS2, &vis2A);
                    starWith.GetPropertyValue(sclsvrCALIBRATOR_VIS2_ERROR, &vis2ErrA);

                    // Compute MAX(|vis2A - vis2|; vis2Err)
                    
                    mcsDOUBLE visibilityErr;
                    if (fabs(vis2A - vis2) < vis2Err)
                    {
                        visibilityErr = vis2Err;
                    }
                    else
                    {
                        visibilityErr = fabs(vis2A - vis2);
                    }
                    
                    logTest("vis2 / vis2A / |vis2A - vis2| = %lf / %lf / %lf", vis2, vis2A, fabs(vis2A - vis2));
                    logTest("vis2Err / visibilityErr = %lf / %lf", vis2Err, visibilityErr); 

                    // Test of validity of the visibility
                    mcsDOUBLE expectedVisErr = request.GetExpectedVisErr();
                    if (visibilityErr > expectedVisErr)
                    {
                        logTest("star '%s' - visibility error (%lf) is higher than the expected one (%lf)",
                                starId, visibilityErr, expectedVisErr);
                    }
                    else
                    {
                        logTest("star '%s' - visibility error (%lf) is lower than the expected one (%lf)",
                                starId, visibilityErr, expectedVisErr);
                        
                        Update(starWith,mcsTRUE);
                    }
                }
            }
        }
    }

    // Compute UD from LD and SP
    if (ComputeUDFromLDAndSP() == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    // Compute distance
    if (ComputeDistance(request) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    return mcsSUCCESS;
}


/*
 * Private methods
 */
/**
 * Compute missing magnitude.
 *
 * @param isBright
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is
 * returned.
 */
mcsCOMPL_STAT sclsvrCALIBRATOR::ComputeMissingMagnitude(mcsLOGICAL isBright)
{
    logTrace("sclsvrCALIBRATOR::ComputeMissingMagnitude()");

    alxMAGNITUDES magnitudes;

    // WARNING: Property Id lists should be defined in the same order than
    // alxBAND enumerate. In order to be able to use this enumerate as index of
    // this list.
    const char* mag0PropertyId[alxNB_BANDS] = 
    {
        sclsvrCALIBRATOR_BO,
        sclsvrCALIBRATOR_VO,
        sclsvrCALIBRATOR_RO,
        sclsvrCALIBRATOR_IO,
        sclsvrCALIBRATOR_JO,
        sclsvrCALIBRATOR_HO,
        sclsvrCALIBRATOR_KO,
        sclsvrCALIBRATOR_LO,
        sclsvrCALIBRATOR_MO
    };

    // For other magnitudes
    for (int band = 0; band < alxNB_BANDS; band++)
    { 
        // Get the current value
        if (IsPropertySet(mag0PropertyId[band]) == mcsTRUE)
        {
            if (GetPropertyValue(mag0PropertyId[band], &magnitudes[band].value) == mcsFAILURE)
            {
                return mcsFAILURE;
            }

            magnitudes[band].isSet     = mcsTRUE;
            magnitudes[band].confIndex = (alxCONFIDENCE_INDEX)GetPropertyConfIndex(mag0PropertyId[band]);
        }
        else
        {
            magnitudes[band].value     = 0.0;
            magnitudes[band].isSet     = mcsFALSE;
            magnitudes[band].confIndex = alxNO_CONFIDENCE;
        }
    }

    // Get the value of the Spectral Type
    mcsSTRING32 spType;
    if (isBright == mcsTRUE)
    {
        strcpy(spType, GetPropertyValue(vobsSTAR_SPECT_TYPE_MK));

        // Compute missing magnitudes
        if (alxComputeMagnitudesForBrightStar(spType, magnitudes) == mcsFAILURE)
        {
            // If error found on spectral type, reset stack and return SUCCESS
            if ((errIsInStack("alx", alxERR_SPECTRAL_TYPE_NOT_FOUND) == mcsTRUE) ||
                (errIsInStack("alx", alxERR_WRONG_SPECTRAL_TYPE_FORMAT) == mcsTRUE))
            {
                logTest("Spectral type '%s' is unknown; could not compute missing magnitudes", spType); 
                
                errResetStack();
                return mcsSUCCESS;
            }
            else
            {
                return mcsFAILURE;
            }
        }
    }
    else
    {
        if (IsPropertySet(vobsSTAR_SPECT_TYPE_MK) == mcsTRUE)
        {
            strcpy(spType, GetPropertyValue(vobsSTAR_SPECT_TYPE_MK));

            // Compute missing magnitudes
            if (alxComputeMagnitudesForFaintStar(spType, magnitudes) == mcsFAILURE)
            {
                // if error found on spectral type, reset stack and
                // return SUCCESS
                if ((errIsInStack("alx", alxERR_SPECTRAL_TYPE_NOT_FOUND) == mcsTRUE) ||
                    (errIsInStack("alx", alxERR_WRONG_SPECTRAL_TYPE_FORMAT) == mcsTRUE))
                {
                    logTest("Spectral type '%s' is unknown; could not compute missing magnitudes", spType); 
                    
                    errResetStack();
                    return mcsSUCCESS;
                }
                else
                {
                    return mcsFAILURE;
                }
            }
        }
        else
        {
            // Compute missing magnitudes
            if (alxComputeMagnitudesForFaintStar(NULL,magnitudes) == mcsFAILURE)
            {
                if ((errIsInStack("alx",alxERR_NO_LINE_FOUND) ==mcsTRUE) ||
                    (errIsInStack("alx",alxERR_DIFFJK_NOT_IN_TABLE) == mcsTRUE))
                {
                    logTest("J-K differential magnitude not found "
                            "in color table; could not compute missing magnitudes");
                    
                    errResetStack();
                    return mcsSUCCESS;
                }

                else
                {
                    return mcsFAILURE;
                }
            }
        }
    }

    // For each magnitude
    for (int band = 0; band < alxNB_BANDS; band++)
    { 
        /*
         * Set the computed magnitude.
         * Please note that if a magnitude is already set, the
         * SetPropertyValue() does nothing; i.e. existing magnitudes are not
         * overwritten.
         */
        if (magnitudes[band].isSet == mcsTRUE)
        {
            if (SetPropertyValue(mag0PropertyId[band], magnitudes[band].value,
                                 vobsSTAR_COMPUTED_PROP, 
                                 (vobsCONFIDENCE_INDEX)magnitudes[band].confIndex,
                                 mcsFALSE) == mcsFAILURE)
            {
                return mcsFAILURE;
            }
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
    if (GetRa(ra) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    // Get declinaison in degree
    if (GetDec(dec) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    // Compute galactic coordinates from the retrieved ra and dec values
    if (alxComputeGalacticCoordinates(ra, dec, &gLat, &gLon) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    // Set the galactic lattitude (if not yet set)
    if (SetPropertyValue(vobsSTAR_POS_GAL_LAT, gLat, vobsSTAR_COMPUTED_PROP) ==
        mcsFAILURE)
    {
        return mcsFAILURE;
    }

    // Set the galactic longitude (if not yet set)
    if (SetPropertyValue(vobsSTAR_POS_GAL_LON, gLon, vobsSTAR_COMPUTED_PROP) ==
        mcsFAILURE)
    {
        return mcsFAILURE;
    }

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

    // Get the value of the parallax
    if (IsPropertySet(vobsSTAR_POS_PARLX_TRIG) == mcsTRUE)
    {
        if (GetPropertyValue(vobsSTAR_POS_PARLX_TRIG, &parallax) == mcsFAILURE)
        {
            return mcsFAILURE;
        }
    }
    else 
    {
        errAdd(sclsvrERR_MISSING_PROPERTY, vobsSTAR_POS_PARLX_TRIG, "interstellar absorption");
        return mcsFAILURE;
    }

    // Get the value of the galactic lattitude
    if (IsPropertySet(vobsSTAR_POS_GAL_LAT) == mcsTRUE)
    {
        if (GetPropertyValue(vobsSTAR_POS_GAL_LAT, &gLat) == mcsFAILURE)
        {
            return mcsFAILURE;
        }
    }
    else
    {
        errAdd(sclsvrERR_MISSING_PROPERTY, vobsSTAR_POS_GAL_LAT, "interstellar absorption");
        return mcsFAILURE;
    }

    // Get the value of the galactic longitude
    if (IsPropertySet(vobsSTAR_POS_GAL_LON) == mcsTRUE)
    {
        if (GetPropertyValue(vobsSTAR_POS_GAL_LON, &gLon) == mcsFAILURE)
        {
            return mcsFAILURE;
        }
    }
    else 
    {
        errAdd(sclsvrERR_MISSING_PROPERTY, vobsSTAR_POS_GAL_LON, "interstellar absorption");
        return mcsFAILURE;
    }

    // Compute Extinction ratio
    if (alxComputeExtinctionCoefficient(&av, parallax, gLat, gLon) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    // Set extinction ratio property
    if (SetPropertyValue(sclsvrCALIBRATOR_EXTINCTION_RATIO, 
                         av, vobsSTAR_COMPUTED_PROP) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    return mcsSUCCESS;
}

/**
 * Compute interstellar absorption.
 * 
 * @param isBright true is it is for bright object
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is
 * returned.
 */
mcsCOMPL_STAT sclsvrCALIBRATOR::ComputeInterstellarAbsorption(mcsLOGICAL isBright)
{
    logTrace("sclsvrCALIBRATOR::ComputeInterstellarAbsorption()");

    // Magnitudes to be used
    const char* magPropertyId[alxNB_BANDS]; 
    if (isBright == mcsTRUE)
    {
        magPropertyId[alxB_BAND] = vobsSTAR_PHOT_JHN_B;
        magPropertyId[alxV_BAND] = vobsSTAR_PHOT_JHN_V;
        magPropertyId[alxR_BAND] = vobsSTAR_PHOT_JHN_R;
        magPropertyId[alxI_BAND] = vobsSTAR_PHOT_JHN_I;
        magPropertyId[alxJ_BAND] = vobsSTAR_PHOT_JHN_J;
        magPropertyId[alxH_BAND] = vobsSTAR_PHOT_JHN_H;
        magPropertyId[alxK_BAND] = vobsSTAR_PHOT_JHN_K;
        magPropertyId[alxL_BAND] = vobsSTAR_PHOT_JHN_L;
        magPropertyId[alxM_BAND] = vobsSTAR_PHOT_JHN_M;
    }
    else
    {
        magPropertyId[alxB_BAND] = vobsSTAR_PHOT_PHG_B;
        magPropertyId[alxV_BAND] = vobsSTAR_PHOT_JHN_V;
        magPropertyId[alxR_BAND] = vobsSTAR_PHOT_PHG_R;
        magPropertyId[alxI_BAND] = vobsSTAR_PHOT_COUS_I;
        magPropertyId[alxJ_BAND] = sclsvrCALIBRATOR_PHOT_COUS_J;
        magPropertyId[alxH_BAND] = sclsvrCALIBRATOR_PHOT_COUS_H;
        magPropertyId[alxK_BAND] = sclsvrCALIBRATOR_PHOT_COUS_K;
        magPropertyId[alxL_BAND] = vobsSTAR_PHOT_JHN_L;
        magPropertyId[alxM_BAND] = vobsSTAR_PHOT_JHN_M;
    } 
    // WARNING: Property Id lists should be defined in the same order than
    // alxBAND enumerate. In order to be able to use this enumerate as index of
    // this list.
    const char* mag0PropertyId[alxNB_BANDS] = 
    {
        sclsvrCALIBRATOR_BO,
        sclsvrCALIBRATOR_VO,
        sclsvrCALIBRATOR_RO,
        sclsvrCALIBRATOR_IO,
        sclsvrCALIBRATOR_JO,
        sclsvrCALIBRATOR_HO,
        sclsvrCALIBRATOR_KO,
        sclsvrCALIBRATOR_LO,
        sclsvrCALIBRATOR_MO
    };

    // Get the extinction ratio
    mcsDOUBLE av;
    if (GetPropertyValue(sclsvrCALIBRATOR_EXTINCTION_RATIO, &av) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    // For each magnitude
    alxMAGNITUDES magnitudes;
    for (int band = 0; band < alxNB_BANDS; band++)
    { 
        // Get the current value
        if (IsPropertySet(magPropertyId[band]) == mcsTRUE)
        {
            if (GetPropertyValue(magPropertyId[band], &magnitudes[band].value) == mcsFAILURE)
            {
                return mcsFAILURE;
            }

            magnitudes[band].isSet     = mcsTRUE;
            magnitudes[band].confIndex =  (alxCONFIDENCE_INDEX)GetPropertyConfIndex(magPropertyId[band]);
        }
        else
        {
            magnitudes[band].isSet     = mcsFALSE;
            magnitudes[band].confIndex = alxNO_CONFIDENCE;
            magnitudes[band].value     = 0.0;
        }
    }

    // Correct K magnitude  
    if (isBright == mcsTRUE)
    {
        if (IsPropertySet(vobsSTAR_PHOT_JHN_K) == mcsTRUE)
        {
            const char *origin = GetProperty(vobsSTAR_PHOT_JHN_K)->GetOrigin();

            // If coming from II/246/out, J/A+A/433/1155
            if ((strcmp(origin, vobsCATALOG_MASS_ID) == 0) ||
                (strcmp(origin, vobsCATALOG_MERAND_ID)== 0))
            {
                magnitudes[alxK_BAND].value = 1.008 * magnitudes[alxK_BAND].value + 0.005;
            }

            // If coming from J-K Denis
            if (strcmp(origin, vobsCATALOG_DENIS_JK_ID) == 0)
            {
                magnitudes[alxK_BAND].value = 1.008 * (magnitudes[alxK_BAND].value + 
                                              0.006 * (magnitudes[alxJ_BAND].value -
                                              magnitudes[alxK_BAND].value)) - 0.03;
            }
        }
    }

    // Compute corrected magnitude
    if (alxComputeCorrectedMagnitudes(av, magnitudes) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    // For each magnitude
    for (int band = 0; band < alxNB_BANDS; band++)
    { 
        // Set the corrected magnitude
        if (magnitudes[band].isSet == mcsTRUE)
        {
            if (SetPropertyValue(mag0PropertyId[band], magnitudes[band].value,
                                 vobsSTAR_COMPUTED_PROP) == mcsFAILURE)
            {
                return mcsFAILURE;
            }
        }
    }

    return mcsSUCCESS;
}

/**
 * Compute apparent magnitude which are mising
 *
 * @param av the extinction coefficient
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is
 * returned.
 */
mcsCOMPL_STAT sclsvrCALIBRATOR::ComputeApparentMagnitude(mcsLOGICAL isBright)
{
    logTrace("sclsvrCALIBRATOR::ComputeApparentMagnitude()");

    // Magnitudes to be used
    const char *magPropertyId[alxNB_BANDS]; 
    if (isBright == mcsTRUE)
    {
        magPropertyId[alxB_BAND] = vobsSTAR_PHOT_JHN_B;
        magPropertyId[alxV_BAND] = vobsSTAR_PHOT_JHN_V;
        magPropertyId[alxR_BAND] = vobsSTAR_PHOT_JHN_R;
        magPropertyId[alxI_BAND] = vobsSTAR_PHOT_JHN_I;
        magPropertyId[alxJ_BAND] = vobsSTAR_PHOT_JHN_J;
        magPropertyId[alxH_BAND] = vobsSTAR_PHOT_JHN_H;
        magPropertyId[alxK_BAND] = vobsSTAR_PHOT_JHN_K;
        magPropertyId[alxL_BAND] = vobsSTAR_PHOT_JHN_L;
        magPropertyId[alxM_BAND] = vobsSTAR_PHOT_JHN_M;
    }
    else
    {
        magPropertyId[alxB_BAND] = vobsSTAR_PHOT_PHG_B;
        magPropertyId[alxV_BAND] = vobsSTAR_PHOT_JHN_V;
        magPropertyId[alxR_BAND] = vobsSTAR_PHOT_PHG_R;
        magPropertyId[alxI_BAND] = vobsSTAR_PHOT_COUS_I;
        magPropertyId[alxJ_BAND] = sclsvrCALIBRATOR_PHOT_COUS_J;
        magPropertyId[alxH_BAND] = sclsvrCALIBRATOR_PHOT_COUS_H;
        magPropertyId[alxK_BAND] = sclsvrCALIBRATOR_PHOT_COUS_K;
        magPropertyId[alxL_BAND] = vobsSTAR_PHOT_JHN_L;
        magPropertyId[alxM_BAND] = vobsSTAR_PHOT_JHN_M;
    } 

    // WARNING: Property Id lists should be defined in the same order than
    // alxBAND enumerate. In order to be able to use this enumerate as index of
    // this list.
    const char *mag0PropertyId[alxNB_BANDS] = 
    {
        sclsvrCALIBRATOR_BO,
        sclsvrCALIBRATOR_VO,
        sclsvrCALIBRATOR_RO,
        sclsvrCALIBRATOR_IO,
        sclsvrCALIBRATOR_JO,
        sclsvrCALIBRATOR_HO,
        sclsvrCALIBRATOR_KO,
        sclsvrCALIBRATOR_LO,
        sclsvrCALIBRATOR_MO
    };

    // Get the extinction ratio
    mcsDOUBLE av;
    if (GetPropertyValue(sclsvrCALIBRATOR_EXTINCTION_RATIO, &av) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    // For each corrected magnitude
    alxMAGNITUDES magnitudes;
    for (int band = 0; band < alxNB_BANDS; band++)
    { 
        // Get the current value and the confidence index
        if (IsPropertySet(mag0PropertyId[band]) == mcsTRUE)
        {
            if (GetPropertyValue(mag0PropertyId[band], &magnitudes[band].value) == mcsFAILURE)
            {
                return mcsFAILURE;
            }

            magnitudes[band].isSet     = mcsTRUE;
            magnitudes[band].confIndex = (alxCONFIDENCE_INDEX)GetPropertyConfIndex(mag0PropertyId[band]);
        }
        else
        {
            magnitudes[band].isSet     = mcsFALSE;
            magnitudes[band].confIndex = alxNO_CONFIDENCE;
            magnitudes[band].value     = 0.0;
        }
    }

    // Compute apparent magnitude
    if (alxComputeApparentMagnitudes(av, magnitudes) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    // For each magnitude
    for (int band = 0; band < alxNB_BANDS; band++)
    { 
        // Set the magnitude magnitude
        if (magnitudes[band].isSet == mcsTRUE)
        {
            if (SetPropertyValue(magPropertyId[band], magnitudes[band].value,
                                 vobsSTAR_COMPUTED_PROP,
                                 (vobsCONFIDENCE_INDEX)magnitudes[band].confIndex) == mcsFAILURE)
            {
                return mcsFAILURE;
            }
        }
    }

    return mcsSUCCESS;
}


/**
 * Compute angular diameter
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is
 * returned.
 */
mcsCOMPL_STAT sclsvrCALIBRATOR::ComputeAngularDiameter(mcsLOGICAL isBright)
{
    logTrace("sclsvrCALIBRATOR::ComputeAngularDiameter()");

    // Declare the 4 properties name for I0, J0, K0, H0
    int nbProperties;
    const char *starPropertyId[6];
    if (isBright == mcsTRUE)
    {
        nbProperties = 4;
        starPropertyId[0] = sclsvrCALIBRATOR_BO;
        starPropertyId[1] = sclsvrCALIBRATOR_VO;
        starPropertyId[2] = sclsvrCALIBRATOR_RO;
        starPropertyId[3] = sclsvrCALIBRATOR_KO;
    }
    else
    {
        nbProperties = 4;
        starPropertyId[0] = sclsvrCALIBRATOR_IO;
        starPropertyId[1] = sclsvrCALIBRATOR_JO;
        starPropertyId[2] = sclsvrCALIBRATOR_KO;
        starPropertyId[3] = sclsvrCALIBRATOR_HO;
        starPropertyId[4] = vobsSTAR_PHOT_JHN_V;
        starPropertyId[5] = vobsSTAR_PHOT_JHN_K;
    }

    alxDIAMETERS diameters;
    alxDATA starProperty[6];

    // For each property needed to compute angular diameter
    for (int i = 0; i < nbProperties; i++)
    { 
        // If the current property is affected
        if (IsPropertySet(starPropertyId[i]) == mcsTRUE)
        {
            // Get the property
            vobsSTAR_PROPERTY *property=GetProperty(starPropertyId[i]);

            // Get the property value
            if ((property->GetValue(&starProperty[i].value)) == mcsFAILURE)
            {
                return mcsFAILURE;
            }

            // Flag the property as set
            starProperty[i].isSet = mcsTRUE;

            // Get the property confidence index
            starProperty[i].confIndex = (alxCONFIDENCE_INDEX)property->GetConfidenceIndex();
        }
        else
        {
            logWarning("Unknown '%s' property; "
                       "could not compute diameter", starPropertyId[i]);
            
            // Do nothing
            return mcsSUCCESS;
        }
    }

    // In faint case
    if (isBright == mcsFALSE)
    {
        // If V mag is coming I/280 (2mass)
        if ((IsPropertySet(starPropertyId[4]) == mcsTRUE) &&
            (strcmp(GetProperty(starPropertyId[4])->GetOrigin(), vobsCATALOG_ASCC_ID) == 0))
        {
            // Get V and K magnitude to compute diam VK 
            for (int i = 4; i < 6; i++)
            { 
                // Get the property
                vobsSTAR_PROPERTY *property = GetProperty(starPropertyId[i]);

                // Get the property value
                if ((property->GetValue(&starProperty[i].value)) == mcsFAILURE)
                {
                    return mcsFAILURE;
                }

                // Flag the property as set
                starProperty[i].isSet = mcsTRUE;

                // Get the property confidence index
                starProperty[i].confIndex =
                    (alxCONFIDENCE_INDEX)property->GetConfidenceIndex();
            }
            // Compute apparent magnitude for V and K
            // Get the extinction ratio
            mcsDOUBLE av;
            if (GetPropertyValue(sclsvrCALIBRATOR_EXTINCTION_RATIO,
                                 &av) == mcsFAILURE)
            {
                return mcsFAILURE;
            }

            // V - Correct interstellar absorption
            starProperty[4].value = starProperty[4].value + av;
            // K - Convert K2mass to Kjnk and correct interstellar absorption 
            starProperty[5].value = 1.008 * starProperty[5].value + 0.005;
            starProperty[5].value = starProperty[5].value +  0.116 * av;
        }
        else
        {
            starProperty[4].isSet = mcsFALSE;
            starProperty[5].isSet = mcsFALSE;
        }
    }

    // Set retrieved magnitudes
    alxDATA magB, magV, magR, magI, magJ, magK, magH, magKJnk;
    if (isBright == mcsTRUE)
    {
        magB=starProperty[0];
        magV=starProperty[1];
        magR=starProperty[2];
        magK=starProperty[3];
    }
    else
    {
        magI=starProperty[0];
        magJ=starProperty[1];
        magK=starProperty[2];
        magH=starProperty[3];
        magV=starProperty[4];
        magKJnk=starProperty[5];
    }

    if (isBright == mcsTRUE)
    {
        // Compute angular diameters for bright star
        if (alxComputeAngularDiameterForBrightStar(magB, magV, magR, magK,
                                                   &diameters) == mcsFAILURE)
        {
            return mcsFAILURE;
        }

        // Set flag according to the confidence index 
        if (diameters.areCoherent == mcsFALSE)
        {
            logTest("Error on diameter too high; computed diameters are not coherent");

            if (SetPropertyValue(sclsvrCALIBRATOR_DIAM_FLAG, "NOK",
                                 vobsSTAR_COMPUTED_PROP) == mcsFAILURE)
            {
                return mcsFAILURE;
            }
        }
        else
        {
            if (SetPropertyValue(sclsvrCALIBRATOR_DIAM_FLAG,
                                 "OK", vobsSTAR_COMPUTED_PROP) == mcsFAILURE)
            {
                return mcsFAILURE;
            }
        }

        // Set the computed value of the angular diameter
        if (SetPropertyValue(sclsvrCALIBRATOR_DIAM_BV, diameters.bv.value,
                             vobsSTAR_COMPUTED_PROP, 
                             (vobsCONFIDENCE_INDEX)diameters.confidenceIdx)
            == mcsFAILURE)
        {
            return mcsFAILURE;
        }
        if (SetPropertyValue(sclsvrCALIBRATOR_DIAM_VR, diameters.vr.value,
                             vobsSTAR_COMPUTED_PROP, 
                             (vobsCONFIDENCE_INDEX)diameters.confidenceIdx)
            == mcsFAILURE)
        {
            return mcsFAILURE;
        }
        if (SetPropertyValue(sclsvrCALIBRATOR_DIAM_VK, diameters.vk.value,
                             vobsSTAR_COMPUTED_PROP, 
                             (vobsCONFIDENCE_INDEX)diameters.confidenceIdx)
            == mcsFAILURE)
        {
            return mcsFAILURE;
        }
        if (SetPropertyValue(sclsvrCALIBRATOR_DIAM_BV_ERROR, 
                             diameters.bvErr.value,
                             vobsSTAR_COMPUTED_PROP, 
                             (vobsCONFIDENCE_INDEX)diameters.confidenceIdx)
            == mcsFAILURE)
        {
            return mcsFAILURE;
        }
        if (SetPropertyValue(sclsvrCALIBRATOR_DIAM_VR_ERROR,
                             diameters.vrErr.value,
                             vobsSTAR_COMPUTED_PROP, 
                             (vobsCONFIDENCE_INDEX)diameters.confidenceIdx)
            == mcsFAILURE)
        {
            return mcsFAILURE;
        }
        if (SetPropertyValue(sclsvrCALIBRATOR_DIAM_VK_ERROR,
                             diameters.vkErr.value,
                             vobsSTAR_COMPUTED_PROP, 
                             (vobsCONFIDENCE_INDEX)diameters.confidenceIdx)
            == mcsFAILURE)
        {
            return mcsFAILURE;
        }
    }
    else
    {
        // Make a short pre-processing to check that Johnson magnitutde in J, H,
        // K came from the CDS. If not, stop the angular diametre computation.
        vobsSTAR_PROPERTY *magJProp = GetProperty(vobsSTAR_PHOT_JHN_J);
        vobsSTAR_PROPERTY *magHProp = GetProperty(vobsSTAR_PHOT_JHN_H);
        vobsSTAR_PROPERTY *magKProp = GetProperty(vobsSTAR_PHOT_JHN_K);
        if ((magJProp->IsComputed() == mcsTRUE) ||
            (magHProp->IsComputed() == mcsTRUE) ||
            (magKProp->IsComputed() == mcsTRUE))
        {
            // stop the treatment
            logTest("J, H and/or K magitudes are unknown; could not compute diameter"); 
            
            if (SetPropertyValue(sclsvrCALIBRATOR_DIAM_FLAG, "NOK",
                                 vobsSTAR_COMPUTED_PROP) == mcsFAILURE)
            {
                return mcsFAILURE;
            }
        }
        else
        {
            // Check if I came from CDS. If not, do not use it for diameter
            // computation 
            vobsSTAR_PROPERTY *magIProp;
            magIProp = GetProperty(vobsSTAR_PHOT_COUS_I);
            if (strcmp(magIProp->GetOrigin(), vobsSTAR_COMPUTED_PROP) != 0)
            {
                magI.isSet = mcsTRUE;
            }
            else
            {
                magI.isSet = mcsFALSE;
            }

            // Compute angular diameters
            if (alxComputeAngularDiameterForFaintStar(magI, magJ, magK, magH,
                                                      magV, magKJnk,
                                                      &diameters) == mcsFAILURE)
            {
                return mcsFAILURE;
            }

            // Set flag according to the confidence index 
            if (diameters.areCoherent == mcsFALSE)
            {
                logTest("Error on diameter too high; computed diameters are not coherent");
                
                if (SetPropertyValue(sclsvrCALIBRATOR_DIAM_FLAG, "NOK",
                                     vobsSTAR_COMPUTED_PROP) == mcsFAILURE)
                {
                    return mcsFAILURE;
                }
            }
            else
            {
                if (SetPropertyValue(sclsvrCALIBRATOR_DIAM_FLAG, "OK",
                                     vobsSTAR_COMPUTED_PROP) == mcsFAILURE)
                {
                    return mcsFAILURE;
                }
            }

            // Save IJ and IK diameters, only if I magitude came from CDS 
            if (magI.isSet == mcsTRUE)
            {
                // Set the computed value of the angular diameter
                if (SetPropertyValue(sclsvrCALIBRATOR_DIAM_IJ,
                                     diameters.ij.value,
                                     vobsSTAR_COMPUTED_PROP, 
                                     (vobsCONFIDENCE_INDEX)diameters.confidenceIdx)
                    == mcsFAILURE)
                {
                    return mcsFAILURE;
                }
                if (SetPropertyValue(sclsvrCALIBRATOR_DIAM_IK, 
                                     diameters.ik.value,
                                     vobsSTAR_COMPUTED_PROP, 
                                     (vobsCONFIDENCE_INDEX)diameters.confidenceIdx)
                    == mcsFAILURE)
                {
                    return mcsFAILURE;
                }

                if (SetPropertyValue(sclsvrCALIBRATOR_DIAM_IJ_ERROR, 
                                     diameters.ijErr.value,
                                     vobsSTAR_COMPUTED_PROP, 
                                     (vobsCONFIDENCE_INDEX)diameters.confidenceIdx)
                    == mcsFAILURE)
                {
                    return mcsFAILURE;
                }
                if (SetPropertyValue(sclsvrCALIBRATOR_DIAM_IK_ERROR,
                                     diameters.ikErr.value,
                                     vobsSTAR_COMPUTED_PROP, 
                                     (vobsCONFIDENCE_INDEX)diameters.confidenceIdx)
                    == mcsFAILURE)
                {
                    return mcsFAILURE;
                }
            }

            // Save JK and JH diameters
            if (SetPropertyValue(sclsvrCALIBRATOR_DIAM_JK, diameters.jk.value,
                                 vobsSTAR_COMPUTED_PROP, 
                                 (vobsCONFIDENCE_INDEX)diameters.confidenceIdx)
                == mcsFAILURE)
            {
                return mcsFAILURE;
            }
            if (SetPropertyValue(sclsvrCALIBRATOR_DIAM_JH, diameters.jh.value,
                                 vobsSTAR_COMPUTED_PROP, 
                                 (vobsCONFIDENCE_INDEX)diameters.confidenceIdx)
                == mcsFAILURE)
            {
                return mcsFAILURE;
            }
            if (SetPropertyValue(sclsvrCALIBRATOR_DIAM_JK_ERROR,
                                 diameters.jkErr.value,
                                 vobsSTAR_COMPUTED_PROP, 
                                 (vobsCONFIDENCE_INDEX)diameters.confidenceIdx)
                == mcsFAILURE)
            {
                return mcsFAILURE;
            }
            if (SetPropertyValue(sclsvrCALIBRATOR_DIAM_JH_ERROR,
                                 diameters.jhErr.value,
                                 vobsSTAR_COMPUTED_PROP, 
                                 (vobsCONFIDENCE_INDEX)diameters.confidenceIdx)
                == mcsFAILURE)
            {
                return mcsFAILURE;
            }

            // Save HK diameter if I magnitude was not known
            if (magI.isSet == mcsFALSE)
            {
                if (SetPropertyValue(sclsvrCALIBRATOR_DIAM_HK, 
                                     diameters.hk.value,
                                     vobsSTAR_COMPUTED_PROP, 
                                     (vobsCONFIDENCE_INDEX)diameters.confidenceIdx)
                    == mcsFAILURE)
                {
                    return mcsFAILURE;
                }
                if (SetPropertyValue(sclsvrCALIBRATOR_DIAM_HK_ERROR,
                                     diameters.hkErr.value,
                                     vobsSTAR_COMPUTED_PROP, 
                                     (vobsCONFIDENCE_INDEX)diameters.confidenceIdx)
                    == mcsFAILURE)
                {
                    return mcsFAILURE;
                }
            }
            // Save VK diameter, only if V magitude came from CDS 
            if (magV.isSet == mcsTRUE)
            {
                // Set the computed value of the angular diameter
                if (SetPropertyValue(sclsvrCALIBRATOR_DIAM_VK,
                                     diameters.vk.value,
                                     vobsSTAR_COMPUTED_PROP, 
                                     (vobsCONFIDENCE_INDEX)diameters.confidenceIdx)
                    == mcsFAILURE)
                {
                    return mcsFAILURE;
                }
                if (SetPropertyValue(sclsvrCALIBRATOR_DIAM_VK_ERROR,
                                     diameters.vkErr.value,
                                     vobsSTAR_COMPUTED_PROP, 
                                     (vobsCONFIDENCE_INDEX)diameters.confidenceIdx)
                    == mcsFAILURE)
                {
                    return mcsFAILURE;
                }
            }
            if (SetPropertyValue(sclsvrCALIBRATOR_DIAM_MEAN,
                                 diameters.mean.value,
                                 vobsSTAR_COMPUTED_PROP, 
                                 (vobsCONFIDENCE_INDEX)diameters.confidenceIdx)
                == mcsFAILURE)
            {
                return mcsFAILURE;
            }
            if (SetPropertyValue(sclsvrCALIBRATOR_DIAM_MEAN_ERROR,
                                 diameters.meanErr.value,
                                 vobsSTAR_COMPUTED_PROP, 
                                 (vobsCONFIDENCE_INDEX)diameters.confidenceIdx)
                == mcsFAILURE)
            {
                return mcsFAILURE;
            }
        }
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

    // Compute UD only if LD are already OK
    if (IsDiameterOk() == mcsFALSE)
    {
        logTest("Skipping (diameters are not OK).");
        return mcsSUCCESS;
    }

    // Does LD diameter exist ?
    if (IsPropertySet(sclsvrCALIBRATOR_DIAM_VK) == mcsFALSE)
    {
        logTest("Skipping (LD unknown).");
        return mcsSUCCESS;
    }

    // Get LD diameter (DIAM_VK)
    mcsDOUBLE ld = FP_NAN;
    if (GetPropertyValue(sclsvrCALIBRATOR_DIAM_VK, &ld) == mcsFAILURE)
    {
        logWarning("Aborting (error while retrieving DIAM_VK).");
        return mcsSUCCESS;
    }

    // Get LD diameter confidence index (UDs will have the same one)
    vobsCONFIDENCE_INDEX ldDiameterConfidenceIndex = GetPropertyConfIndex(sclsvrCALIBRATOR_DIAM_VK);

    // Does Teff exist ?
    mcsDOUBLE Teff = FP_NAN;
    if (IsPropertySet(sclsvrCALIBRATOR_TEFF_SPTYP) == mcsFALSE)
    {
        logTest("Skipping (Teff unknown).");
        return mcsSUCCESS;
    }
    else
      {
	  if (GetPropertyValue(sclsvrCALIBRATOR_TEFF_SPTYP, &Teff) == mcsFAILURE)
          {
            logWarning("Aborting (error while retrieving Teff).");
            return mcsSUCCESS;
          }
      }

    // Does LogG exist ?
    mcsDOUBLE LogG = FP_NAN;
    if (IsPropertySet(sclsvrCALIBRATOR_LOGG_SPTYP) == mcsFALSE)
    {
        logTest("Skipping (LogG unknown).");
        return mcsSUCCESS;
    }
    else
    {
    if (GetPropertyValue(sclsvrCALIBRATOR_LOGG_SPTYP, &LogG) == mcsFAILURE)
      {
        logWarning("Aborting (error while retrieving LogG).");
        return mcsSUCCESS;
      }
    }

    // Compute UD
    logTest("Computing UDs for LD = '%lf', Teff = '%lf', LogG = '%lf' ...", ld, Teff, LogG);
    
    alxUNIFORM_DIAMETERS ud;
    if (alxGetUDFromLDAndSP(ld, Teff, LogG, &ud) == mcsFAILURE)
    {
        logWarning("Aborting (error while computing UDs).");
        errResetStack(); // To flush miscDynBufExecuteCommand() related errors
        return mcsSUCCESS;
    }

    // Set each UD_... properties accordingly
    if (SetPropertyValue(sclsvrCALIBRATOR_UD_U, ud.u, vobsSTAR_COMPUTED_PROP, ldDiameterConfidenceIndex) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    if (SetPropertyValue(sclsvrCALIBRATOR_UD_B, ud.b, vobsSTAR_COMPUTED_PROP, ldDiameterConfidenceIndex) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    if (SetPropertyValue(sclsvrCALIBRATOR_UD_V, ud.v, vobsSTAR_COMPUTED_PROP, ldDiameterConfidenceIndex) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    if (SetPropertyValue(sclsvrCALIBRATOR_UD_R, ud.r, vobsSTAR_COMPUTED_PROP, ldDiameterConfidenceIndex) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    if (SetPropertyValue(sclsvrCALIBRATOR_UD_I, ud.i, vobsSTAR_COMPUTED_PROP, ldDiameterConfidenceIndex) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    if (SetPropertyValue(sclsvrCALIBRATOR_UD_J, ud.j, vobsSTAR_COMPUTED_PROP, ldDiameterConfidenceIndex) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    if (SetPropertyValue(sclsvrCALIBRATOR_UD_H, ud.h, vobsSTAR_COMPUTED_PROP, ldDiameterConfidenceIndex) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    if (SetPropertyValue(sclsvrCALIBRATOR_UD_K, ud.k, vobsSTAR_COMPUTED_PROP, ldDiameterConfidenceIndex) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    if (SetPropertyValue(sclsvrCALIBRATOR_UD_L, ud.l, vobsSTAR_COMPUTED_PROP, ldDiameterConfidenceIndex) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    if (SetPropertyValue(sclsvrCALIBRATOR_UD_N, ud.n, vobsSTAR_COMPUTED_PROP, ldDiameterConfidenceIndex) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    return mcsSUCCESS;
}


/**
 * Compute visibility.
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is
 * returned.
 */
mcsCOMPL_STAT sclsvrCALIBRATOR::ComputeVisibility(sclsvrREQUEST &request)
{
    logTrace("sclsvrCALIBRATOR::ComputeVisibility()");

    mcsDOUBLE diam, diamError, baseMax, wavelength;
    alxVISIBILITIES visibilities;
    vobsCONFIDENCE_INDEX confidenceIndex = vobsCONFIDENCE_HIGH;

    // Get object diameter. First look at the diameters coming from catalog
    static const char *diamId[4][2] = 
    {
        {vobsSTAR_UD_DIAM, vobsSTAR_UD_DIAM_ERROR},
        {vobsSTAR_LD_DIAM, vobsSTAR_LD_DIAM_ERROR},
        {vobsSTAR_UDDK_DIAM, vobsSTAR_UDDK_DIAM_ERROR},
        {vobsSTAR_DIAM12, vobsSTAR_DIAM12_ERROR}
    };

    // For each possible diameters
    mcsLOGICAL found = mcsFALSE;
    for (int i = 0; (i < 4) && (found == mcsFALSE); i++)
    {
        // If diameter and its error are set 
        if ((IsPropertySet(diamId[i][0]) == mcsTRUE) &&
            (IsPropertySet(diamId[i][1]) == mcsTRUE))
        {
            // Get values
            if (GetPropertyValue(diamId[i][0], &diam) == mcsFAILURE)
            {
                return mcsFAILURE;
            }
            if (GetPropertyValue(diamId[i][1], &diamError) == mcsFAILURE)
            {
                return mcsFAILURE;
            }
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
            if (IsDiameterOk() == mcsTRUE)
            {
                // Get V-K diameter and associated error value
                if (GetPropertyValue(sclsvrCALIBRATOR_DIAM_VK, &diam) == mcsFAILURE)
                {
                    return mcsFAILURE;
                }
                if (GetPropertyValue(sclsvrCALIBRATOR_DIAM_VK_ERROR, &diamError) == mcsFAILURE)
                {
                    return mcsFAILURE;
                }

                // Get confidence index of computed diameter
                vobsSTAR_PROPERTY *property = GetProperty(sclsvrCALIBRATOR_DIAM_VK);
                confidenceIndex = property->GetConfidenceIndex();
            }
            // Else do not compute visibility
            else
            {
                logTest("Unknown diameter; could not compute visibility");
                return mcsSUCCESS;
            }
        }
        // Else 
        else
        {
            // If computed diameter is OK
            if (IsDiameterOk() == mcsTRUE)
            {
                // Get V-K diameter and associated error value
                if (GetPropertyValue(sclsvrCALIBRATOR_DIAM_JK, &diam) == mcsFAILURE)
                {
                    return mcsFAILURE;
                }
                if (GetPropertyValue(sclsvrCALIBRATOR_DIAM_JK_ERROR, &diamError) == mcsFAILURE)
                {
                    return mcsFAILURE;
                }

                // Get confidence index of computed diameter
                vobsSTAR_PROPERTY *property = GetProperty(sclsvrCALIBRATOR_DIAM_JK);
                confidenceIndex = property->GetConfidenceIndex();
            }
            // Else do not compute visibility
            else
            {
                logTest("unknown diameter; could not compute visibility");
                return mcsSUCCESS;
            }

        }
    }

    // Get value in request of the wavelength
    wavelength = request.GetObservingWlen();

    // Get value in request of the base max
    baseMax = request.GetMaxBaselineLength();
    if (alxComputeVisibility(diam, diamError, baseMax, wavelength,
                             &visibilities) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    // Affect visibility property
    if (SetPropertyValue(sclsvrCALIBRATOR_VIS2, visibilities.vis2,
                         vobsSTAR_COMPUTED_PROP, confidenceIndex) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    if (SetPropertyValue(sclsvrCALIBRATOR_VIS2_ERROR, visibilities.vis2Error,
                         vobsSTAR_COMPUTED_PROP, confidenceIndex) ==  mcsFAILURE)
    {
        return mcsFAILURE;
    }

    // If visibility has been computed, diameter (coming from catalog or
    // computed) must be considered as OK.
    SetPropertyValue(sclsvrCALIBRATOR_DIAM_FLAG, "OK", vobsSTAR_COMPUTED_PROP);

    // If the observed band is N, computed visibility with wlen = 8 and 13 um
    if (strcmp(request.GetSearchBand(), "N") == 0)
    {
        if (alxComputeVisibility(diam, diamError, baseMax, 8,
                                 &visibilities) == mcsFAILURE)
        {
            return mcsFAILURE;
        }

        // Affect visibility property
        if (SetPropertyValue(sclsvrCALIBRATOR_VIS2_8, visibilities.vis2,
                             vobsSTAR_COMPUTED_PROP) == mcsFAILURE)
        {
            return mcsFAILURE;
        }
        if (SetPropertyValue(sclsvrCALIBRATOR_VIS2_8_ERROR, 
                             visibilities.vis2Error,
                             vobsSTAR_COMPUTED_PROP, confidenceIndex) == mcsFAILURE)
        {
            return mcsFAILURE;
        }

        if (alxComputeVisibility(diam, diamError, baseMax, 13,
                                 &visibilities) == mcsFAILURE)
        {
            return mcsFAILURE;
        }

        // Affect visibility property
        if (SetPropertyValue(sclsvrCALIBRATOR_VIS2_13, visibilities.vis2,
                             vobsSTAR_COMPUTED_PROP) == mcsFAILURE)
        {
            return mcsFAILURE;
        }
        if (SetPropertyValue(sclsvrCALIBRATOR_VIS2_13_ERROR,
                             visibilities.vis2Error,
                             vobsSTAR_COMPUTED_PROP, confidenceIndex) == mcsFAILURE)
        {
            return mcsFAILURE;
        }
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
mcsCOMPL_STAT sclsvrCALIBRATOR::ComputeDistance(sclsvrREQUEST &request)
{
    logTrace("sclsvrCALIBRATOR::ComputeDistance()");

    mcsDOUBLE scienceObjectRa  = 0;
    mcsDOUBLE scienceObjectDec = 0;
    mcsDOUBLE calibratorRa     = 0;
    mcsDOUBLE calibratorDec    = 0;
    mcsDOUBLE distance         = 0;
    const char* buffer         = NULL;     
    vobsSTAR scienceObject;

    // Get the science object right ascension as a C string
    buffer = request.GetObjectRa(); 

    if ((buffer == NULL) || (miscIsSpaceStr(buffer) == mcsTRUE))
    {
        return mcsFAILURE;
    }
    // Convert science object right ascension from hhmmss to arcsec
    // using hidden converter embedded in vobsStar constructor
    if (scienceObject.SetPropertyValue(vobsSTAR_POS_EQ_RA_MAIN, buffer, "") == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    // Get the science object origin right acsension in arcsec
    if (scienceObject.GetRa(scienceObjectRa) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    // Get the science object declinaison as a C string
    buffer = request.GetObjectDec();

    if ((buffer == NULL) || (miscIsSpaceStr(buffer) == mcsTRUE))
    {
        return mcsFAILURE;
    }    
    // Convert science object science object from hhmmss to arcsec
    // using hidden converter embedded in vobsStar constructor
    if (scienceObject.SetPropertyValue(vobsSTAR_POS_EQ_DEC_MAIN, buffer, "") == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    // Get the science object origin declinaison in arcsec
    if (scienceObject.GetDec(scienceObjectDec) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    // Get the internal calibrator right acsension in arcsec
    if (GetRa(calibratorRa) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    // Get the internal calibrator declinaison in arcsec
    if (GetDec(calibratorDec) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    // Compute the distance in arcsec between the science object and the
    // calibrator using an alx provided function
    if (alxComputeDistance(scienceObjectRa, scienceObjectDec,
                           calibratorRa,    calibratorDec,
                           &distance) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    // Put the computed distance in the corresponding calibrator property
    if (SetPropertyValue(sclsvrCALIBRATOR_DIST, distance / 3600, vobsSTAR_COMPUTED_PROP) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

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

    mcsSTRING32 spType;
    if (IsPropertySet(vobsSTAR_SPECT_TYPE_MK) == mcsFALSE)
    {
        logTest("Spectral Type - Skipping (no SpType available).");
    	return mcsFAILURE;
    }

    strncpy(spType, GetPropertyValue(vobsSTAR_SPECT_TYPE_MK), sizeof(spType));
    if (strlen(spType) < 1)
    {
        logTest("Spectral Type - Skipping (SpType unknown).");
        return mcsFAILURE;
    }

    logTest("Parsing Spectral Type '%s'.", spType);

    /* 
     * Get each part of the spectral type XN.NLLL where X is a letter, N.N a
     * number between 0 and 9 and LLL is the light class
     */
    if (alxString2SpectralType(spType, &_spectralType) == mcsFAILURE)
    {
        logWarning("Spectral Type - Skipping (could not parse SpType '%s').", spType);
        return mcsFAILURE;
    }
    
    return mcsSUCCESS;
}
/**
 * Check wether the spectral type asses spectral binarity.
 * 
 * @return mcsSUCCESS on successfull completion, mcsFAILURE otherwise.
 */
mcsCOMPL_STAT sclsvrCALIBRATOR::ComputeSpectralBinarity()
{
    logTrace("sclsvrCALIBRATOR::ComputeSpectralBinarity()");

    if (_spectralType.isSpectralBinary == mcsTRUE)
    {
        logTest("Spectral Binarity - 'SB' found in SpType.");

        // Only store spectral binarity if none present before
        if (SetPropertyValue(vobsSTAR_CODE_BIN_FLAG, "SB", vobsSTAR_COMPUTED_PROP, vobsCONFIDENCE_HIGH, mcsFALSE) == mcsFAILURE)
        {
            return mcsFAILURE;
        }
    }

    if (_spectralType.isDouble == mcsTRUE)
    {
        logTest("Spectral Binarity - '+' found in SpType.");

        // Only store spectral binarity if none present before
        if (SetPropertyValue(vobsSTAR_CODE_MULT_FLAG, "S", vobsSTAR_COMPUTED_PROP, vobsCONFIDENCE_HIGH, mcsFALSE) == mcsFAILURE)
        {
            return mcsFAILURE;
        }
    }

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

    mcsDOUBLE Teff = FP_NAN;
    mcsDOUBLE LogG = FP_NAN;

    mcsSTRING32 spType;
    if (IsPropertySet(vobsSTAR_SPECT_TYPE_MK) == mcsFALSE)
    {
        logTest("Teff and LogG - Skipping (no SpType available).");
        return mcsSUCCESS;
    }
    strncpy(spType, GetPropertyValue(vobsSTAR_SPECT_TYPE_MK), sizeof(spType));
    if (strlen(spType) < 1)
    {
        logTest("Teff and LogG - Skipping (SpType unknown).");
        return mcsSUCCESS;
    }
    //Get Teff 
    if (alxRetrieveTeffAndLoggFromSptype(spType, &Teff, &LogG) == mcsFAILURE)
    {
        logTest("Teff and LogG - Skipping (alxRetrieveTeffAndLoggFromSptype() failed on this spectral type: '%s').",spType);
        errResetStack(); // To flush miscDynBufExecuteCommand() related errors
        return mcsSUCCESS;
    }

    // Set Teff eand LogG properties
    if (SetPropertyValue(sclsvrCALIBRATOR_TEFF_SPTYP, Teff, vobsSTAR_COMPUTED_PROP, vobsCONFIDENCE_HIGH) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    if (SetPropertyValue(sclsvrCALIBRATOR_LOGG_SPTYP, LogG, vobsSTAR_COMPUTED_PROP, vobsCONFIDENCE_HIGH) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
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
    mcsLOGICAL f12isDefined = mcsTRUE;
    mcsLOGICAL f12AlreadySet = mcsFALSE;
    mcsLOGICAL e_f12isDefined = mcsTRUE;
    mcsLOGICAL e_f12AlreadySet = mcsFALSE;
    mcsDOUBLE Teff      = FP_NAN;
    mcsDOUBLE fnu_9     = FP_NAN;
    mcsDOUBLE e_fnu_9   = FP_NAN;
    mcsDOUBLE fnu_12    = FP_NAN;
    mcsDOUBLE e_fnu_12  = FP_NAN;
    mcsDOUBLE fnu_18    = FP_NAN;
    mcsDOUBLE e_fnu_18  = FP_NAN;
    mcsDOUBLE magN      = FP_NAN;

    // initial tests of presence of data:

    // Get the value of Teff. If impossible, no possibility to go further!
    if (IsPropertySet(sclsvrCALIBRATOR_TEFF_SPTYP) == mcsTRUE)
    {
        // Retrieve it
        if (GetPropertyValue(sclsvrCALIBRATOR_TEFF_SPTYP, &Teff) == mcsFAILURE)
        {       
            return mcsFAILURE;
        }
    }
    else
    {
        logTest("IR Fluxes: Skipping (no Teff available).");
        return mcsSUCCESS;
    }

    // Get fnu_09 (vobsSTAR_PHOT_FLUX_IR_09)
    if (IsPropertySet(vobsSTAR_PHOT_FLUX_IR_09) == mcsTRUE)
    {
        // retrieve it
        if (GetPropertyValue(vobsSTAR_PHOT_FLUX_IR_09, &fnu_9) == mcsFAILURE)
        {       
            return mcsFAILURE;
        }
        has9 = mcsTRUE;
    }

    // Get fnu_18 (vobsSTAR_PHOT_FLUX_IR_18)
    if (IsPropertySet(vobsSTAR_PHOT_FLUX_IR_18) == mcsTRUE)
    {
        // retrieve it
        if (GetPropertyValue(vobsSTAR_PHOT_FLUX_IR_18, &fnu_18) == mcsFAILURE)
        {       
            return mcsFAILURE;
        }
        has18 = mcsTRUE;
    }

    // get out if no *measured* infrared fluxes
    if ( (has9 == mcsFALSE) && (has18 == mcsFALSE) )
    {
        logTest("IR Fluxes: Skipping (no 9 mu or 18 mu flux available).");
        return mcsSUCCESS;
    }

    // Get e_fnu_09 (vobsSTAR_PHOT_FLUX_IR_09_ERROR)
    if (IsPropertySet(vobsSTAR_PHOT_FLUX_IR_09_ERROR) == mcsTRUE)
    {
        // retrieve it
        if (GetPropertyValue(vobsSTAR_PHOT_FLUX_IR_09_ERROR, &e_fnu_9) == mcsFAILURE)
        {       
            return mcsFAILURE;
        }
        hase_9 = mcsTRUE;
    }

    // Get e_fnu_18 (vobsSTAR_PHOT_FLUX_IR_18_ERROR)
    if (IsPropertySet(vobsSTAR_PHOT_FLUX_IR_18_ERROR) == mcsTRUE)
    {
        // retrieve it
        if (GetPropertyValue(vobsSTAR_PHOT_FLUX_IR_18_ERROR, &e_fnu_18) == mcsFAILURE)
        {       
            return mcsFAILURE;
        }
        hase_18 = mcsTRUE;
    }


    // check presence etc of F12:
    f12isDefined = IsProperty(vobsSTAR_PHOT_FLUX_IR_12);
    if (f12isDefined) f12AlreadySet = IsPropertySet(vobsSTAR_PHOT_FLUX_IR_12);
    
    // check presence etc of e_F12:
    e_f12isDefined = IsProperty(vobsSTAR_PHOT_FLUX_IR_12_ERROR);
    if (e_f12isDefined) e_f12AlreadySet = IsPropertySet(vobsSTAR_PHOT_FLUX_IR_12_ERROR);


    // if f9 is defined, compute all fluxes from it, then fill void places.
    if (has9 == mcsTRUE)
    {
        // Compute all fluxes from 9 onwards
        if (alxComputeFluxesFromAkari09(Teff, &fnu_9, &fnu_12, &fnu_18) == mcsFAILURE)
        {
            logWarning("IR Fluxes: Skipping (akari internal error).");
            return mcsSUCCESS;
        }
        logTest("IR Fluxes: akari measured fnu_9 = %lf; computed fnu_12 = %lf, fnu_18 = %lf", fnu_9, fnu_12, fnu_18);
        
        // Store it eventually:
        if (f12isDefined && !f12AlreadySet)
        {
            if (SetPropertyValue(vobsSTAR_PHOT_FLUX_IR_12, fnu_12, vobsSTAR_COMPUTED_PROP) == mcsFAILURE)
            {
                return mcsFAILURE;
            }
        }
        // Compute Mag N:
        magN = 4.1 - 2.5 * log10(fnu_12 / 0.89);
        
        logTest("IR Fluxes: computed magN = %lf", magN);
        
        // Store it
        if (IsPropertySet(vobsSTAR_PHOT_JHN_N) == mcsFALSE)
        {
            if (SetPropertyValue(vobsSTAR_PHOT_JHN_N, magN, vobsSTAR_COMPUTED_PROP) == mcsFAILURE)
            {
                return mcsFAILURE;
            }
        }
        // store s18 if void:
        if (has18 == mcsFALSE)
        {
            if (SetPropertyValue(vobsSTAR_PHOT_FLUX_IR_18, fnu_18, vobsSTAR_COMPUTED_PROP) == mcsFAILURE)
            {
                return mcsFAILURE;
            }
        }

        // compute s_12 error etc, if s09_err is present:
        if (hase_9 == mcsTRUE)
        {
            if (alxComputeFluxesFromAkari09(Teff, &e_fnu_9, &e_fnu_12, &e_fnu_18) == mcsFAILURE)
            {
                logTest("IR Fluxes: Skipping (akari internal error).");
                return mcsSUCCESS;
            }
            // Store it eventually:
            if (e_f12isDefined && !e_f12AlreadySet)
            {
                if (SetPropertyValue(vobsSTAR_PHOT_FLUX_IR_12_ERROR, e_fnu_12, vobsSTAR_COMPUTED_PROP) == mcsFAILURE)
                {
                    return mcsFAILURE;
                }
            }
            // store e_s18 if void:
            if (hase_18 == mcsFALSE)
            {
                if (SetPropertyValue(vobsSTAR_PHOT_FLUX_IR_18_ERROR, e_fnu_18, vobsSTAR_COMPUTED_PROP) == mcsFAILURE)
                {
                    return mcsFAILURE;
                }
            }
        }
    }
    else // only s18 !
    {
        // Compute all fluxes from 18  backwards
        if (alxComputeFluxesFromAkari18(Teff, &fnu_18, &fnu_12, &fnu_9) == mcsFAILURE)
        {
            logTest("IR Fluxes: Skipping (akari internal error).");
            return mcsSUCCESS;
        }
        logTest("IR Fluxes: akari measured fnu_18 = %lf; computed fnu_12 = %lf, fnu_9 = %lf", fnu_18, fnu_12, fnu_9);
        
        // Store it eventually:
        if (f12isDefined && !f12AlreadySet)
        {
            if (SetPropertyValue(vobsSTAR_PHOT_FLUX_IR_12, fnu_12, vobsSTAR_COMPUTED_PROP) == mcsFAILURE)
            {
                return mcsFAILURE;
            }
        }
        // Compute Mag N:
        magN = 4.1 - 2.5 * log10(fnu_12 / 0.89);

        logTest("IR Fluxes: computed magN = %lf", magN);

        // Store it
        if (IsPropertySet(vobsSTAR_PHOT_JHN_N) == mcsFALSE)
        {
            if (SetPropertyValue(vobsSTAR_PHOT_JHN_N, magN, vobsSTAR_COMPUTED_PROP) == mcsFAILURE)
            {
                return mcsFAILURE;
            }
        }
        // store s9 if void:
        if (has9 == mcsFALSE)
        {
            if (SetPropertyValue(vobsSTAR_PHOT_FLUX_IR_09, fnu_9, vobsSTAR_COMPUTED_PROP) == mcsFAILURE)
            {
                return mcsFAILURE;
            }
        }

        // compute s_12 error etc, if s18_err is present:
        if (hase_18 == mcsTRUE)
        {
            if (alxComputeFluxesFromAkari18(Teff, &e_fnu_18, &e_fnu_12, &e_fnu_9) == mcsFAILURE)
            {
                logTest("IR Fluxes: Skipping (akari internal error).");
                return mcsSUCCESS;
            }
            // Store it eventually:
            if (e_f12isDefined && !e_f12AlreadySet)
            {
                if (SetPropertyValue(vobsSTAR_PHOT_FLUX_IR_12_ERROR, e_fnu_12, vobsSTAR_COMPUTED_PROP) == mcsFAILURE)
                {
                    return mcsFAILURE;
                }
            }
            // store e_s9 if void:
            if (hase_9 == mcsFALSE)
            {
                if (SetPropertyValue(vobsSTAR_PHOT_FLUX_IR_09_ERROR, e_fnu_9, vobsSTAR_COMPUTED_PROP) == mcsFAILURE)
                {
                    return mcsFAILURE;
                }
            }
        }
    }

    return mcsSUCCESS;
}

/**
 * Convert magnitude in cousin
 *
 * @return  mcsSUCCESS on successful completion. Otherwise mcsFAILURE is
 * returned.
 */
mcsCOMPL_STAT sclsvrCALIBRATOR::ComputeCousinMagnitude()
{
    logTrace("sclsvrCALIBRATOR::ComputeCousinMagnitude()");

    // convert J H K mag Jcous hcous Kcous
    mcsDOUBLE magK;
    mcsDOUBLE magJ;
    mcsDOUBLE magH;
    mcsLOGICAL magKIsSet = mcsFALSE;
    mcsLOGICAL magJIsSet = mcsFALSE;
    mcsLOGICAL magHIsSet = mcsFALSE;
    mcsDOUBLE magKcous;
    mcsDOUBLE magJcous;
    mcsDOUBLE magHcous;

    // if Kmag property is set
    if (IsPropertySet(vobsSTAR_PHOT_JHN_K) == mcsTRUE)
    {
        // retreive it
        if (GetPropertyValue(vobsSTAR_PHOT_JHN_K, &magK) == mcsFAILURE)
        {       
            return mcsFAILURE;
        }
        magKIsSet = mcsTRUE;
    }
    // if Jmag property is set        
    if (IsPropertySet(vobsSTAR_PHOT_JHN_J) == mcsTRUE)
    {
        // retreive it
        if (GetPropertyValue(vobsSTAR_PHOT_JHN_J, &magJ) == mcsFAILURE)
        {       
            return mcsFAILURE;
        }
        magJIsSet = mcsTRUE;
    }
    // if Hmag property is set
    if (IsPropertySet(vobsSTAR_PHOT_JHN_H) == mcsTRUE)
    {
        // retreive it
        if (GetPropertyValue(vobsSTAR_PHOT_JHN_H, &magH) == mcsFAILURE)
        {       
            return mcsFAILURE;
        }
        magHIsSet = mcsTRUE;
    }

    // Set Kcous
    if (magKIsSet == mcsTRUE)
    {
        magKcous = magK + 0.024;
        if (SetPropertyValue(sclsvrCALIBRATOR_PHOT_COUS_K, magKcous, 
                             vobsSTAR_COMPUTED_PROP) == mcsFAILURE)
        {
            return mcsFAILURE;
        }
    }

    // Set Jcous
    if ((magKIsSet == mcsTRUE) && (magJIsSet == mcsTRUE))
    {
        magJcous = 0.947 * magJ + 0.053 * magK + 0.036;
        if (SetPropertyValue(sclsvrCALIBRATOR_PHOT_COUS_J, magJcous, 
                             vobsSTAR_COMPUTED_PROP) == mcsFAILURE)
        {
            return mcsFAILURE;
        }
    }

    // Set Hcous
    if ((magKIsSet == mcsTRUE) && (magHIsSet == mcsTRUE))
    {
        magHcous = 0.975 * magH + 0.025 * magK - 0.004;
        if (SetPropertyValue(sclsvrCALIBRATOR_PHOT_COUS_H, magHcous, 
                             vobsSTAR_COMPUTED_PROP) == mcsFAILURE)
        {
            return mcsFAILURE;
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
        AddPropertyMeta(sclsvrCALIBRATOR_PHOT_COUS_J, "Jcous", vobsFLOAT_PROPERTY, "mag", NULL, NULL,
                    "Cousin's Magnitude in J-band");
        AddPropertyMeta(sclsvrCALIBRATOR_PHOT_COUS_H, "Hcous", vobsFLOAT_PROPERTY, "mag", NULL, NULL,
                    "Cousin's Magnitude in H-band");
        AddPropertyMeta(sclsvrCALIBRATOR_PHOT_COUS_K, "Kcous", vobsFLOAT_PROPERTY, "mag", NULL, NULL,
                    "Cousin's Magnitude in K-band");

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

        AddPropertyMeta(sclsvrCALIBRATOR_DIAM_MEAN, "diam_mean", vobsFLOAT_PROPERTY, "mas", NULL, NULL,
                    "Mean Diameter from the IR Magnitude versus Color Indices Calibrations");
        AddPropertyMeta(sclsvrCALIBRATOR_DIAM_MEAN_ERROR, "e_diam_mean", vobsFLOAT_PROPERTY, "mas", NULL, NULL,
                    "Estimated Error on Mean Diameter");

        AddPropertyMeta(sclsvrCALIBRATOR_DIAM_FLAG, "diamFlag", vobsSTRING_PROPERTY);

        AddPropertyMeta(sclsvrCALIBRATOR_TEFF_SPTYP, "Teff_SpType", vobsFLOAT_PROPERTY, NULL, NULL, NULL,
                    "Effective Temperature adopted from Spectral Type");
        AddPropertyMeta(sclsvrCALIBRATOR_LOGG_SPTYP, "logg_SpType", vobsFLOAT_PROPERTY, NULL, NULL, NULL,
                    "Gravity adopted from Spectral Type");

        AddPropertyMeta(sclsvrCALIBRATOR_UD_B, "UD_B", vobsFLOAT_PROPERTY, "mas", NULL, NULL,
                    "B-band Uniform-Disk Diameter");
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
        AddPropertyMeta(sclsvrCALIBRATOR_UD_R, "UD_R", vobsFLOAT_PROPERTY, "mas", NULL, NULL,
                    "R-band Uniform-Disk Diameter");
        AddPropertyMeta(sclsvrCALIBRATOR_UD_U, "UD_U", vobsFLOAT_PROPERTY, "mas", NULL, NULL,
                    "U-band Uniform-Disk Diameter");
        AddPropertyMeta(sclsvrCALIBRATOR_UD_V, "UD_V", vobsFLOAT_PROPERTY, "mas", NULL, NULL,
                    "V-band Uniform-Disk Diameter");

        AddPropertyMeta(sclsvrCALIBRATOR_EXTINCTION_RATIO, "Av", vobsFLOAT_PROPERTY, NULL, NULL, NULL,
                    "Visual Interstellar Absorption");

        AddPropertyMeta(sclsvrCALIBRATOR_MO, "Mo", vobsFLOAT_PROPERTY, "mag");
        AddPropertyMeta(sclsvrCALIBRATOR_LO, "Lo", vobsFLOAT_PROPERTY, "mag");
        AddPropertyMeta(sclsvrCALIBRATOR_KO, "Ko", vobsFLOAT_PROPERTY, "mag");
        AddPropertyMeta(sclsvrCALIBRATOR_HO, "Ho", vobsFLOAT_PROPERTY, "mag");
        AddPropertyMeta(sclsvrCALIBRATOR_JO, "Jo", vobsFLOAT_PROPERTY, "mag");
        AddPropertyMeta(sclsvrCALIBRATOR_IO, "Io", vobsFLOAT_PROPERTY, "mag");
        AddPropertyMeta(sclsvrCALIBRATOR_RO, "Ro", vobsFLOAT_PROPERTY, "mag");
        AddPropertyMeta(sclsvrCALIBRATOR_VO, "Vo", vobsFLOAT_PROPERTY, "mag");
        AddPropertyMeta(sclsvrCALIBRATOR_BO, "Bo", vobsFLOAT_PROPERTY, "mag");

        AddPropertyMeta(sclsvrCALIBRATOR_VIS2, "vis2", vobsFLOAT_PROPERTY, NULL, NULL, NULL,
                    "Squared Visibility");
        AddPropertyMeta(sclsvrCALIBRATOR_VIS2_ERROR, "vis2Err", vobsFLOAT_PROPERTY, NULL, NULL, NULL,
                    "Error on Squared Visibility");

        AddPropertyMeta(sclsvrCALIBRATOR_VIS2_8, "vis2(8mu)", vobsFLOAT_PROPERTY, NULL, NULL, NULL,
                    "Squared Visibility at 8 microns");
        AddPropertyMeta(sclsvrCALIBRATOR_VIS2_8_ERROR, "vis2Err(8mu)", vobsFLOAT_PROPERTY, NULL, NULL, NULL,
                    "Error on Squared Visibility at 8 microns");

        AddPropertyMeta(sclsvrCALIBRATOR_VIS2_13, "vis2(13mu)", vobsFLOAT_PROPERTY, NULL, NULL, NULL,
                    "Squared Visibility at 13 microns");
        AddPropertyMeta(sclsvrCALIBRATOR_VIS2_13_ERROR, "vis2Err(13mu)", vobsFLOAT_PROPERTY, NULL, NULL, NULL,
                    "Error on Squared Visibility at 13 microns");

        AddPropertyMeta(sclsvrCALIBRATOR_VIS2_FLAG, "vis2Flag", vobsSTRING_PROPERTY);

        AddPropertyMeta(sclsvrCALIBRATOR_DIST, "dist", vobsFLOAT_PROPERTY, "deg", NULL, NULL,
                    "Calibrator to Science object Angular Distance");
        
        sclsvrCALIBRATOR::sclsvrCALIBRATOR_PropertyMetaEnd = vobsSTAR::vobsStar_PropertyMetaList.size();

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
    sclsvrCALIBRATOR::sclsvrCALIBRATOR_PropertyMetaBegin      = -1;
    sclsvrCALIBRATOR::sclsvrCALIBRATOR_PropertyMetaEnd        = -1;
    sclsvrCALIBRATOR::sclsvrCALIBRATOR_PropertyIdxInitialized = false;
}

/*___oOo___*/
