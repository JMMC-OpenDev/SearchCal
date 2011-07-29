/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * @file
 *  Definition of vobsCATALOG_MIDI class.
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
#include "math.h"

/*
 * Local Headers 
 */
#include "vobsCATALOG_MIDI.h"
#include "vobsPrivate.h"
#include "vobsErrors.h"

/**
 * Class constructor
 */
vobsCATALOG_MIDI::vobsCATALOG_MIDI() : vobsLOCAL_CATALOG("MIDI", 
                                                         "vobsMidiCatalog.cfg")
{
}

/**
 * Class destructor
 */
vobsCATALOG_MIDI::~vobsCATALOG_MIDI()
{
}

/*
 * Public methods
 */
/**
 * Search for star list in catalog.
 *
 * Build star list from catalog, where each list star verifies constraints
 * specified by user request.
 *
 * @param request user request specifying search contraints
 * @param list star list provided by the search
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is
 * returned.
 */
mcsCOMPL_STAT vobsCATALOG_MIDI::Search(vobsREQUEST &request,
                                       vobsSTAR_LIST &list)
{
    logTrace("vobsCATALOG_MIDI::Search()");

    //
    // Load catalog in star list
    // -------------------------
    if (Load() == mcsFAILURE)
    {
        // Add error with specifying the catalog name
        errAdd(vobsERR_CATALOG_LOAD, GetName());
        return mcsFAILURE;
    }
    logTest("Catalog is correctly loaded in a star list");
    
    //
    // Build reference (science) object
    // --------------------------------

    // Get reference object properties
    const char *ra;          // reference object right ascension
    const char *dec;         // reference object declinaison
    mcsDOUBLE magnitude;      // reference object magnitude
    // ra
    ra = request.GetObjectRa();
    // dec
    dec = request.GetObjectDec();
    // magnitude
    magnitude = request.GetObjectMag();
    
    // Create the reference star
    vobsSTAR referenceStar;

    // Add reference star properties
    // ra
    if (referenceStar.SetPropertyValue
        (vobsSTAR_POS_EQ_RA_MAIN, ra, GetName()) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    // dec
    if (referenceStar.SetPropertyValue
        (vobsSTAR_POS_EQ_DEC_MAIN, dec, GetName()) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    // N magnitude
    if (referenceStar.SetPropertyValue
        (vobsSTAR_PHOT_JHN_N, magnitude, GetName()) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    //
    // Build constraint list
    // ---------------------
    // Aim is to set search field
    
    // Get reference object constraints
    mcsDOUBLE deltaRa;    // reference object ra constaint
    mcsDOUBLE deltaDec;    // reference object dec constaint

    // Get search area size
    if (request.GetSearchArea(deltaRa, deltaDec) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    
    // Convert minutes (arcmin) to decimal degrees
    deltaRa = deltaRa / 60.0 / 2.0;
    deltaDec = deltaDec / 60.0 / 2.0;

    // Create reference object constraint list
    vobsSTAR_COMP_CRITERIA_LIST constraintlist;
    
    // Add reference star constraints to constaint list
    // Field on sky: ra constraint
    if (constraintlist.Add(vobsSTAR_POS_EQ_RA_MAIN, deltaRa) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    // Field on sky: dec constraint
    if (constraintlist.Add(vobsSTAR_POS_EQ_DEC_MAIN, deltaDec) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    // Magnitude range constraint
    // Object magnitude conversion to IRAS flux 12mu
    mcsDOUBLE referenceStarFlux;
    mcsDOUBLE minNFlux;
    mcsDOUBLE maxNFlux;
    mcsDOUBLE diffNFlux;
    mcsDOUBLE middleNFlux;
    referenceStarFlux = 0.89 * pow(10, -0.4 * (magnitude - 4.1));

    // Calculate N magnitude range constraint depending on object flux
    // N magnitude range constraint is half N magnitude range
    minNFlux = 5;
    if (referenceStarFlux < 10)
    {
        maxNFlux = 20;
    }
    else if (referenceStarFlux >= 10 && referenceStarFlux <= 100)
    {
        maxNFlux = 50;
    }
    else
    {
        // referenceStarFlux > 100
        maxNFlux = 100;
    }

    diffNFlux = (maxNFlux - minNFlux) / 2;
    middleNFlux = (minNFlux + diffNFlux);

    // Add N flux constraint to constraint list
    if (constraintlist.Add(vobsSTAR_PHOT_FLUX_IR_12, diffNFlux) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    
    // Set flux for reference star to the middle of the flux range
    if (referenceStar.SetPropertyValue(vobsSTAR_PHOT_FLUX_IR_12, 
                                       middleNFlux, GetName()) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    //
    // Select catalog stars which verifies constraints
    // -----------------------------------------------

    const unsigned int nbStars = _starList.Size();
    for (unsigned int el = 0; el < nbStars; el++)
    {
        // Get catalog star
        vobsSTAR *midiCatalogStarPtr;
        midiCatalogStarPtr = _starList.GetNextStar((mcsLOGICAL)(el==0));
        // Compare catalog star with reference star
        if (midiCatalogStarPtr->IsSame(referenceStar, &constraintlist) == mcsTRUE)
        {
            // If Compare catalog star verifies constraint list then add it
            // to the resulting list
            list.AddAtTail(*midiCatalogStarPtr);
        }
    }
    
    // Free memory (internal loaded star list corresponding to the complete local catalog)
    Clear();

    return mcsSUCCESS;
}



/*
 * Private methods
 */

/**
 * Load MIDI catalog.
 *
 * Build star list from MIDI catalog stars.
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is
 * returned.
 */
mcsCOMPL_STAT vobsCATALOG_MIDI::Load(void)
{
    logTrace("vobsCATALOG_MIDI::Load()");
    
    if (_loaded == mcsFALSE)
    {
        //
        // Standard procedure to load catalog
        // ----------------------------------
        if (vobsLOCAL_CATALOG::Load() == mcsFAILURE)
        {
            return mcsFAILURE;
        }

        //
        // MIDI specific loading actions
        // -----------------------------

        const unsigned int nbStars = _starList.Size();

        // Compute magnitude in N band
        mcsUINT32 starIdx;

        for (starIdx = 0; starIdx < nbStars; starIdx++)
        {
            // Get star
            vobsSTAR *starPtr;
            starPtr = _starList.GetNextStar((mcsLOGICAL)(starIdx==0));

            // Get IR flux
            mcsDOUBLE flux;
            mcsDOUBLE magnitude;
            starPtr->GetPropertyValue(vobsSTAR_PHOT_FLUX_IR_12, &flux);

            // Compute magnitude
            magnitude = 4.1 - (2.5 * log10(flux/0.89));
            starPtr->SetPropertyValue(vobsSTAR_PHOT_JHN_N, magnitude, GetName());
        }

        // Re-compute diameter error in mas instead of % 
        for (starIdx = 0; starIdx < nbStars; starIdx++)
        {
            // Get star
            vobsSTAR *starPtr;
            starPtr = _starList.GetNextStar((mcsLOGICAL)(starIdx==0));

            // Get diameter and its associated error 
            mcsDOUBLE diam;
            mcsDOUBLE diamError;
            starPtr->GetPropertyValue(vobsSTAR_DIAM12, &diam);
            starPtr->GetPropertyValue(vobsSTAR_DIAM12_ERROR, &diamError);

            // Convert % to mas 
            diamError = diam * diamError / 100;

            // Rewrite diameter error
            starPtr->SetPropertyValue(vobsSTAR_DIAM12_ERROR, diamError,
                                      GetName(), vobsCONFIDENCE_HIGH, mcsTRUE);
        }

        // If log level is DEBUG or EXTDBG
        if (logIsStdoutLogLevel(logDEBUG) == mcsTRUE)
        {
            //Save star list in a file
            if (_starList.Save("$MCSDATA/tmp/catalogMIDI.dat") == mcsFAILURE)
            {
                // Ignore error (for test only)
                errCloseStack();
            }
        }
    }
  
    return mcsSUCCESS;
}

/*___oOo___*/
