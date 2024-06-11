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
#include <stdlib.h>
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
vobsLOCAL_CATALOG::vobsLOCAL_CATALOG(vobsORIGIN_INDEX catalogId,
                                     const char *filename) : vobsCATALOG(catalogId), _starList("LocalCatalog")
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
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
 */
mcsCOMPL_STAT vobsLOCAL_CATALOG::Load(vobsCATALOG_STAR_PROPERTY_CATALOG_MAPPING* propertyCatalogMap)
{
    //
    // Load catalog into a buffer
    // --------------------------

    // Catalog has already been loaded
    if (IS_TRUE(_loaded))
    {
        return mcsSUCCESS;
    }

    // Catalog has not already been loaded

    // Search for file location
    char* catalogFileName = miscLocateFile(_filename);
    FAIL_NULL(catalogFileName);

    // Load catalog file
    FAIL_DO(_starList.Load(catalogFileName, GetCatalogMeta(), propertyCatalogMap, mcsFALSE, GetCatalogId()),
            free(catalogFileName));

    // Set flag indicating a correct catalog load
    _loaded = mcsTRUE;

    free(catalogFileName);

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
mcsCOMPL_STAT vobsLOCAL_CATALOG::SetOption(const char* option)
{
    if (IS_NOT_NULL(option))
    {
        errAdd(vobsERR_QUERY_OPTION_NOT_SUPPORTED, option);
    }

    return mcsFAILURE;
}

mcsCOMPL_STAT vobsLOCAL_CATALOG::Clear(void)
{
    _starList.Clear();

    _loaded = mcsFALSE;

    return mcsSUCCESS;
}
/*___oOo___*/
