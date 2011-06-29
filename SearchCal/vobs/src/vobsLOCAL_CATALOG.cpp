/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * @file
 * Definition of vobsLOCAL_CATALOG class.
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


/*
 * Private methods
 */

/**
 * Load catalog.
 *
 * Build star list from catalog stars.
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is
 * returned.
 */
mcsCOMPL_STAT vobsLOCAL_CATALOG::Load(void)
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

/**
 * Set catalog option.
 *
 * This method should return an error to say that no option can be used with a
 * local catalog
 *
 * @param option the option to add in the query
 *
 * @return always mcsFAILURE
 */
mcsCOMPL_STAT vobsLOCAL_CATALOG::SetOption(string option)
{
    logTrace("vobsLOCAL_CATALOG::SetOption()");

    if (option != "")
    {
        errAdd(vobsERR_QUERY_OPTION_NOT_SUPPORTED, option.c_str()); 
    }

    return mcsFAILURE;
}
/*___oOo___*/
