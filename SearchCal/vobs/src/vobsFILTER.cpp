/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: vobsFILTER.cpp,v 1.2 2005-06-21 06:20:45 scetre Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.1  2005/06/20 11:31:53  scetre
 * Added filter class
 *
 * Revision 1.2  2005/06/07 12:36:27  scetre
 * Updated documentation
 *
 * Revision 1.1  2005/06/01 14:18:54  scetre
 * Added filters and filter list objects.
 * Changed logExtDbg to logTrace
 *
 ******************************************************************************/

/**
 * @file
 *  Definition of vobsFILTER class.
 */

static char *rcsId="@(#) $Id: vobsFILTER.cpp,v 1.2 2005-06-21 06:20:45 scetre Exp $"; 
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

/*
 * Local Headers 
 */
#include "vobsFILTER.h"
#include "vobsPrivate.h"

/**
 * Class constructor
 */
vobsFILTER::vobsFILTER()
{
    // by default, the filter is disabled
    Disabled();
}

/**
 * Class destructor
 */
vobsFILTER::~vobsFILTER()
{
}

/*
 * Public methods
 */
/**
 * Get the filter name
 *
 * @return filter name
 */
char * vobsFILTER::GetName()
{
    logTrace("vobsFILTER::GetName()");

    return _name;
}

/**
 * Say if the filter is enable or not
 *
 * @return mcsTRUE is filter is enable, otherwise mcsFALSE is returned
 */
mcsLOGICAL vobsFILTER::IsEnabled()
{
    logTrace("vobsFILTER::IsEnabled()");

    return _isEnable;
}

/**
 * Enabled the filter
 *
 * @return always mcsSUCCESS
 */
mcsCOMPL_STAT vobsFILTER::Enabled()
{
    logTrace("vobsFILTER::Enabled()");
    
    _isEnable = mcsTRUE;
    
    return mcsSUCCESS;
}

/**
 * Disabled the filter
 *
 * @return always mcsSUCCESS 
 */
mcsCOMPL_STAT vobsFILTER::Disabled()
{
    logTrace("vobsFILTER::Disabled()");

    _isEnable = mcsFALSE;
    
    return mcsSUCCESS;
}


/*___oOo___*/
