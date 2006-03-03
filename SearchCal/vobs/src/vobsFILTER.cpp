/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: vobsFILTER.cpp,v 1.15 2006-03-03 15:03:27 scetre Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.14  2005/12/22 10:38:45  scetre
 * Updated doxygen documentation
 *
 * Revision 1.13  2005/12/14 15:07:53  scetre
 * Added log information about deleted star in filters
 *
 * Revision 1.12  2005/12/13 16:30:33  lafrasse
 * Added filter Id management through additional constructor parameter
 *
 * Revision 1.11  2005/11/29 13:53:40  gzins
 * Removed GetName method
 *
 * Revision 1.10  2005/11/29 10:33:22  gzins
 * Changed vobsBASE_FILTER to vobsFILTER
 *
 * Revision 1.1  2005/11/24 08:13:50  scetre
 * Changed mother class of filter from vobsFILTER to vobsFILTER
 *
 ******************************************************************************/

/**
 * @file
 * Definition of vobsFILTER class.
 */

static char *rcsId __attribute__ ((unused)) ="@(#) $Id: vobsFILTER.cpp,v 1.15 2006-03-03 15:03:27 scetre Exp $"; 

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
 * 
 * @param filterId a pointer on an already allocated character buffer.
 */
vobsFILTER::vobsFILTER(const char* filterId)
{
    // Store the filter Id
    strcpy(_id, filterId);

    // Disable the filter by default
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
 * Return whether the filter is enable or not
 *
 * @return mcsTRUE if the filter is enable, mcsFALSE otherwise.
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


/**
 * Get the filter Id.
 *
 * @return mcsSUCCESS on successful completion, mcsFAILURE otherwise.
 */
const char * vobsFILTER::GetId(void)
{
    logTrace("vobsFILTER::GetId()");

    return _id;
}


/*
 * Protected methods
 */


/*
 * Private methods
 */


/*___oOo___*/
