/*******************************************************************************
* JMMC project
*
* "@(#) $Id: vobsCATALOG.cpp,v 1.10 2005-02-11 14:22:10 gluck Exp $"
*
* History
* -------
* $Log: not supported by cvs2svn $
* Revision 1.9  2005/02/08 20:30:37  gzins
* Changed _name type; mcsSTRING to string
*
* Revision 1.8  2005/02/07 19:40:58  gzins
* Updated vobsREQUEST API
*
* Revision 1.7  2005/01/31 13:31:38  scetre
* changed misc...Pointer in misc...
*
* Revision 1.6  2005/01/26 08:11:28  scetre
* change history
*
* scetre    27-Jul-2004  Created
* gzins     09-Dec-2004  Fixed cast problem with nez mcsLOGICAL enumerate
*
******************************************************************************/


/**
 * \file
 * Definition vobsCATALOG class .
 */

static char *rcsId="@(#) $Id: vobsCATALOG.cpp,v 1.10 2005-02-11 14:22:10 gluck Exp $"; 
static void *use_rcsId = ((void)&use_rcsId,(void *) &rcsId);

/* 
 * System Headers 
 */
#include <iostream>
using namespace std;  
/**
 * Export standard iostream objects (cin, cout,...).
 */

/*
 * MCS Headers 
 */
#include "mcs.h"
#include "log.h"
#include "err.h"
#include "misc.h"

/*
 * Local Headers 
 */
#include "vobsCATALOG.h"
#include "vobsPrivate.h"
#include "vobsErrors.h"
#include "vobsSTAR.h"

/*
 * Local Variables
 */


/*
 * Class constructor
 */

/**
 * Build a catalog object.
 */
vobsCATALOG::vobsCATALOG(const char *name)
{
    // Set name
    _name = name;
}


/*
 * Class destructor
 */

/**
 * Delete a catalog object. 
 */
vobsCATALOG::~vobsCATALOG()
{
}


/*
 * Public methods
 */

/**
 * Get a catalog name in a string format
 *
 * \param name  name to get, in a string format
 *
 * \return catalog name or NULL if not set.
 */
const char *vobsCATALOG::GetName()
{
    logExtDbg("vobsCATALOG::GetName()");
    
    return _name.c_str();
}

/*___oOo___*/
