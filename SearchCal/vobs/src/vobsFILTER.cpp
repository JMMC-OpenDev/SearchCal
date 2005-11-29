/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: vobsFILTER.cpp,v 1.10 2005-11-29 10:33:22 gzins Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.1  2005/11/24 08:13:50  scetre
 * Changed mother class of filter from vobsFILTER to vobsFILTER
 *
 ******************************************************************************/

/**
 * \file
 *  Definition of vobsFILTER class.
 */

static char *rcsId="@(#) $Id: vobsFILTER.cpp,v 1.10 2005-11-29 10:33:22 gzins Exp $"; 
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
    Disable();
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
char * vobsFILTER::GetName(void)
{
    logTrace("vobsFILTER::GetName()");

    return _name;
}

/**
 * Say if the filter is enable or not
 *
 * @return mcsTRUE is filter is enable, otherwise mcsFALSE is returned
 */
mcsLOGICAL vobsFILTER::IsEnabled(void)
{
    logTrace("vobsFILTER::IsEnabled()");

    return _isEnable;
}

/**
 * Enable the filter
 *
 * @return always mcsSUCCESS
 */
mcsCOMPL_STAT vobsFILTER::Enable(void)
{
    logTrace("vobsFILTER::Enable()");
    
    _isEnable = mcsTRUE;
    
    return mcsSUCCESS;
}

/**
 * Disable the filter
 *
 * @return always mcsSUCCESS 
 */
mcsCOMPL_STAT vobsFILTER::Disable(void)
{
    logTrace("vobsFILTER::Disable()");

    _isEnable = mcsFALSE;
    
    return mcsSUCCESS;
}


/*
 * Protected methods
 */


/*
 * Private methods
 */


/*___oOo___*/
