/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * @file
 * Definition of vobsFILTER_LIST class.
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
    // Note: filter pointers are freed elsewhere (as filters are also private class members)
}

/*
 * Public methods
 */

/**
 * Add a filter in the list
 *
 * @param filter the filter to add in the list
 * @param name the name of the filter
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
