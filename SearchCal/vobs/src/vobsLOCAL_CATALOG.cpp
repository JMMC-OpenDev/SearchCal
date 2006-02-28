/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: vobsLOCAL_CATALOG.cpp,v 1.10 2006-02-28 13:18:48 scetre Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.9  2006/01/18 08:45:38  scetre
 * Added option in generic local and remote catalog
 *
 * Revision 1.8  2005/11/16 10:47:55  scetre
 * Updated documentation
 *
 * Revision 1.7  2005/11/16 10:47:54  scetre
 * Updated documentation
 *
 * Revision 1.6  2005/08/08 11:27:28  scetre
 * Moved specific treatment of midi catalog in the vobsCATALOG_MIDI class instead of vobsLOCAL_CATALOG class
 *
 * Revision 1.5  2005/06/01 14:16:55  scetre
 * Changed logExtDbg to logTrace
 *
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
 * @file
 *  Definition of vobsLOCAL_CATALOG class.
 */

static char *rcsId="@(#) $Id: vobsLOCAL_CATALOG.cpp,v 1.10 2006-02-28 13:18:48 scetre Exp $"; 
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
 * This method should return an error to say that no option can be use with a
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
        errAdd(vobsERR_NO_OPTION_IN_LOCAL_CATALOG, option.c_str()); 
    }

    return mcsFAILURE;
}
/*___oOo___*/
