/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: vobsFILTER.cpp,v 1.12 2005-12-13 16:30:33 lafrasse Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
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

static char *rcsId="@(#) $Id: vobsFILTER.cpp,v 1.12 2005-12-13 16:30:33 lafrasse Exp $"; 
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
 * 
 * @param filterId a pointer on an already allocated character buffer.
 */
vobsFILTER::vobsFILTER(const char* filterId)
{
    // Store the filter Id
    strncpy(_id, filterId, (sizeof(filterId) - 1));

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
 * @param filterId a pointer on an already allocated character buffer.
 * @param maxLength the size of the external character buffer.
 *
 * @return mcsSUCCESS on successful completion, mcsFAILURE otherwise.
 */
mcsCOMPL_STAT vobsFILTER::GetId(char* filterId, const mcsUINT32 maxLength)
{
    logTrace("vobsFILTER::GetId()");

    // Verify parameter validity
    if (filterId == NULL)
    {
        return mcsFAILURE;
    }

    strncpy(filterId, _id, (maxLength - 1));
    return mcsSUCCESS;
}


/*
 * Protected methods
 */


/*
 * Private methods
 */


/*___oOo___*/
