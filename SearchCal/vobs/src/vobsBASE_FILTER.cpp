/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: vobsBASE_FILTER.cpp,v 1.1 2005-11-24 08:13:50 scetre Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 ******************************************************************************/

/**
 * \file
 *  Definition of vobsBASE_FILTER class.
 */

static char *rcsId="@(#) $Id: vobsBASE_FILTER.cpp,v 1.1 2005-11-24 08:13:50 scetre Exp $"; 
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
#include "vobsBASE_FILTER.h"
#include "vobsPrivate.h"

/**
 * Class constructor
 */
vobsBASE_FILTER::vobsBASE_FILTER()
{
    // by default, the filter is disabled
    Disable();
}

/**
 * Class destructor
 */
vobsBASE_FILTER::~vobsBASE_FILTER()
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
char * vobsBASE_FILTER::GetName(void)
{
    logTrace("vobsBASE_FILTER::GetName()");

    return _name;
}

/**
 * Say if the filter is enable or not
 *
 * @return mcsTRUE is filter is enable, otherwise mcsFALSE is returned
 */
mcsLOGICAL vobsBASE_FILTER::IsEnabled(void)
{
    logTrace("vobsBASE_FILTER::IsEnabled()");

    return _isEnable;
}

/**
 * Enable the filter
 *
 * @return always mcsSUCCESS
 */
mcsCOMPL_STAT vobsBASE_FILTER::Enable(void)
{
    logTrace("vobsBASE_FILTER::Enable()");
    
    _isEnable = mcsTRUE;
    
    return mcsSUCCESS;
}

/**
 * Disable the filter
 *
 * @return always mcsSUCCESS 
 */
mcsCOMPL_STAT vobsBASE_FILTER::Disable(void)
{
    logTrace("vobsBASE_FILTER::Disable()");

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
