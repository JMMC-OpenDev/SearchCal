/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: vobsFILTER_LIST.cpp,v 1.9 2005-12-13 16:30:33 lafrasse Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.8  2005/11/29 14:05:18  gzins
 * Removed GetNextFilter() method.
 * Associated name to the filters added into the list
 *
 * Revision 1.7  2005/11/29 10:33:40  gzins
 * Changed vobsBASE_FILTER to vobsFILTER
 *
 * Revision 1.6  2005/11/24 08:14:23  scetre
 * Changed mother class of filter from vobsFILTER to vobsFILTER
 *
 * Revision 1.5  2005/11/16 10:47:55  scetre
 * Updated documentation
 *
 * Revision 1.4  2005/11/16 10:47:54  scetre
 * Updated documentation
 *
 * Revision 1.3  2005/11/05 15:52:36  gzins
 * Change Enabled and Disabled to Enable and Disable
 *
 * Revision 1.2  2005/06/21 06:20:45  scetre
 * Changed method Disable() and Enable() to Disabled() and Enabled()
 * Changed '\' in doxygen documentatiuon to '@'
 *
 * Revision 1.1  2005/06/20 11:31:53  scetre
 * Added filter class
 *
 * Revision 1.1  2005/06/01 14:18:54  scetre
 * Added filters and filter list objects.
 * Changed logExtDbg to logTrace
 *
 ******************************************************************************/

/**
 * @file
 *  Definition of vobsFILTER_LIST class.
 */

static char *rcsId="@(#) $Id: vobsFILTER_LIST.cpp,v 1.9 2005-12-13 16:30:33 lafrasse Exp $"; 
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
#include "vobsFILTER_LIST.h"
#include "vobsPrivate.h"

/**
 * Class constructor
 */
vobsFILTER_LIST::vobsFILTER_LIST(const char* filterId):vobsFILTER(filterId)
{
}

/**
 * Class destructor
 */
vobsFILTER_LIST::~vobsFILTER_LIST()
{
}

/*
 * Public methods
 */

/**
 * Add a filter in the list
 *
 * @param filter the filter to add in the list
 *
 * @return always mcsSUCCESS
 */
mcsCOMPL_STAT vobsFILTER_LIST::Add(vobsFILTER *filter, string name)
{
    logTrace("vobsFILTER_LIST::Add()");

    // Add the filter into the list
    _filterList[name] = filter;

    return mcsSUCCESS;
}

/**
 * Reset the list of filter
 *
 * this method remove all the filter of the list
 * 
 * @return always mcsSUCCESS
 */
mcsCOMPL_STAT vobsFILTER_LIST::Reset(void)
{
    logTrace(" vobsFILTER_LIST::Reset()");

    // Disable all filters  
    map<string, vobsFILTER *>::const_iterator iter;

    for (iter=_filterList.begin(); iter != _filterList.end(); ++iter) 
    {
        (iter->second)->Disable(); 
    }
    return mcsSUCCESS;
}

/**
 * Returns the number of elements (filters) currently stored in the list.
 * @return 
 * The numbers of filters in the list.
 */
mcsUINT32 vobsFILTER_LIST::Size(void) 
{
    return _filterList.size();
}

/**
 * Get the filter which have corresponding name
 *
 * @param name the name of the wanted filter
 *
 * @return pointer to the element of the list or NULL if the element has not
 * been found.
 */
vobsFILTER *vobsFILTER_LIST::GetFilter(string name)
{
    logTrace("vobsFILTER_LIST::GetFilter()");
    map<string, vobsFILTER *>::const_iterator iter;

    // Look for filter
    iter = _filterList.find(name);

    // If not found 
    if (iter == _filterList.end())
    {
        // Return NULL
        return NULL;
    }

    // return filter
    // If it has not been found, NULL will be return
    return (iter->second);
}

/**
 * Apply all filter on a list
 *
 * @param list the list on which the filter will be apply
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is 
 * returned
 */
mcsCOMPL_STAT vobsFILTER_LIST::Apply(vobsSTAR_LIST *list)
{
    logTrace("vobsFILTER_LIST::Apply()");
    vobsFILTER * filter;
    map<string, vobsFILTER *>::const_iterator iter;

    // For each filter in list 
    for (iter=_filterList.begin(); iter != _filterList.end(); ++iter) 
    {
        filter = iter->second; 
        // If it is enabled
        if (filter->IsEnabled() == mcsTRUE)
        {
            // Apply it
            if (filter->Apply(list) == mcsFAILURE)
            {
                return mcsFAILURE;
            }
        }
    }

    return mcsSUCCESS;
}

/*___oOo___*/
