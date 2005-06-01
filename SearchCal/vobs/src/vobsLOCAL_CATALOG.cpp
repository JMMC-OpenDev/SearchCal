/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: vobsLOCAL_CATALOG.cpp,v 1.5 2005-06-01 14:16:55 scetre Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.4  2005/03/04 11:08:18  gzins
 * Updated diffRa/diffDec as bos size for search
 *
 * Revision 1.3  2005/02/24 17:01:35  scetre
 * Set _loaded flag to true when catalog has been loaded
 *
 * Revision 1.2  2005/02/13 15:58:08  gzins
 * Updated Load() method to use new vobsCDATA class methods
 *
 * Revision 1.1  2005/02/11 14:14:31  gluck
 * Added vobsLOCAL_CATALOG  and vobsREMOTE_CATALOG classes to have a more coherent and homogenous inheritance tree
 *
 ******************************************************************************/

/**
 * \file
 *  Definition of vobsLOCAL_CATALOG class.
 */

static char *rcsId="@(#) $Id: vobsLOCAL_CATALOG.cpp,v 1.5 2005-06-01 14:16:55 scetre Exp $"; 
static void *use_rcsId = ((void)&use_rcsId,(void *) &rcsId);

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
#include "vobsLOCAL_CATALOG.h"
#include "vobsPrivate.h"
#include "vobsErrors.h"
#include "vobsCDATA.h"
#include "vobsPARSER.h"

/**
 * Class constructor
 */
vobsLOCAL_CATALOG::vobsLOCAL_CATALOG(const char *name, 
                                     const char *filename) : vobsCATALOG(name)
{
    // Set local catalog filename
    _filename = filename;

    // Initialize load flag
    _loaded = mcsFALSE;
}

/**
 * Class destructor
 */
vobsLOCAL_CATALOG::~vobsLOCAL_CATALOG()
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
 * \param request user request specifying search contraints
 * \param list star list provided by the search
 *
 * \return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is
 * returned.
 */
mcsCOMPL_STAT vobsLOCAL_CATALOG::Search(vobsREQUEST &request,
                                       vobsSTAR_LIST &list)
{
    logTrace("vobsLOCAL_CATALOG::Search()");

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
    mcsFLOAT magnitude;      // reference object magnitude
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
    mcsFLOAT diffRa;    // reference object ra constaint
    mcsFLOAT diffDec;   // reference object dec constaint
    // ra constraint
    diffRa = request.GetDeltaRa();
    // dec constraint
    diffDec = request.GetDeltaDec();
    
    // Convert minutes (arcmin) to decimal degrees
    diffRa = diffRa / 60.0 / 2.0;
    diffDec = diffDec / 60.0 / 2.0;

    // Create reference object constraint list
    vobsSTAR_COMP_CRITERIA_LIST constraintlist;
    
    // Add reference star constraints to constaint list
    // Field on sky: ra constraint
    if (constraintlist.Add("POS_EQ_RA_MAIN", diffRa) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    // Field on sky: dec constraint
    if (constraintlist.Add("POS_EQ_DEC_MAIN", diffDec) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    // Magnitude range constraint
    // Object magnitude conversion to IRAS flux 12mu
    mcsFLOAT referenceStarFlux;
    mcsFLOAT minNFlux;
    mcsFLOAT maxNFlux;
    mcsFLOAT diffNFlux;
    mcsFLOAT middleNFlux;
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
    if (constraintlist.Add("PHOT_FLUX_IR_12", diffNFlux) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    
    // Set flux for reference star to the middle of the flux range
    if (referenceStar.SetPropertyValue("PHOT_FLUX_IR_12", 
                                       middleNFlux, GetName()) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    //
    // Select catalog stars which verifies constraints
    // -----------------------------------------------

    for (mcsUINT32 i=0; i<_starList.Size(); i++)
    {
        // Get catalog star
        vobsSTAR *midiCatalogStarPtr;
        midiCatalogStarPtr = _starList.GetNextStar((mcsLOGICAL)(i==0));
        // Compare catalog star with reference star
        if (midiCatalogStarPtr->IsSame(referenceStar, &constraintlist) == 
                                                                        mcsTRUE)
        {
            // If Compare catalog star verifies constraint list then add it
            // to the resulting list
            list.AddAtTail(*midiCatalogStarPtr);
        }
    }

    return mcsSUCCESS;
}


/*
 * Private methods
 */

/**
 * Load catalog.
 *
 * Build star list from catalog stars.
 *
 * \return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is
 * returned.
 */
mcsCOMPL_STAT vobsLOCAL_CATALOG::Load()
{
    logTrace("vobsLOCAL_CATALOG::Load()");
    
    //
    // Load catalog into a buffer
    // --------------------------
    logDebug("Load catalog ...");

    // Catalog has already been loaded
    if (_loaded == mcsTRUE)
    {
        return mcsSUCCESS;
    }

    // Catalog has not already been loaded
    
    // Search for file location
    const char *catalogFileName;
    catalogFileName = miscLocateFile(_filename);
    if (catalogFileName == NULL)
    {
        return mcsFAILURE;
    }

    // Load catalog file 
    if (_starList.Load(catalogFileName, mcsFALSE, GetName()) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
 
    // Set flag indicating a correct catalog load
    _loaded = mcsTRUE;
  
    return mcsSUCCESS;
}

/*___oOo___*/
