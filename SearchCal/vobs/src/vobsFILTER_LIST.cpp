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
vobsFILTER_LIST::vobsFILTER_LIST(const char* filterId) : vobsFILTER(filterId)
{
    // Enable the filter by default
    Enable();
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
mcsCOMPL_STAT vobsFILTER_LIST::Add(vobsFILTER* filter, const char* name)
{
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
    // Disable all filters
    for (vobsFILTER_PTR_MAP::const_iterator iter = _filterList.begin(); iter != _filterList.end(); ++iter)
    {
        (iter->second)->Disable();
    }
    return mcsSUCCESS;
}

/**
 * Get the filter which have corresponding name
 *
 * @param name the name of the wanted filter
 *
 * @return pointer to the element of the list or NULL if the element has not
 * been found.
 */
vobsFILTER* vobsFILTER_LIST::GetFilter(const char* name)
{
    // Look for filter
    vobsFILTER_PTR_MAP::const_iterator iter = _filterList.find(name);

    // If not found
    if (iter == _filterList.end())
    {
        return NULL;
    }

    // return filter
    // If it has not been found, NULL will be return
    return iter->second;
}

/**
 * Apply all filter on a list
 *
 * @param list the list on which the filter will be apply
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
 */
mcsCOMPL_STAT vobsFILTER_LIST::Apply(vobsSTAR_LIST* list)
{
    if (IS_TRUE(IsEnabled()))
    {
        vobsFILTER* filter;

        // For each filter in list
        for (vobsFILTER_PTR_MAP::const_iterator iter = _filterList.begin(); iter != _filterList.end(); ++iter)
        {
            filter = iter->second;

            // If it is enabled
            if (IS_TRUE(filter->IsEnabled()))
            {
                // Apply it
                FAIL(filter->Apply(list));
            }
        }
    }
    return mcsSUCCESS;
}

mcsCOMPL_STAT vobsFILTER_LIST::DumpAsXML(miscoDYN_BUF& buffer) const
{
    if (IS_TRUE(IsEnabled()))
    {
        vobsFILTER* filter;

        // For each filter in list
        for (vobsFILTER_PTR_MAP::const_iterator iter = _filterList.begin(); iter != _filterList.end(); ++iter)
        {
            filter = iter->second;

            // If it is enabled
            if (IS_TRUE(filter->IsEnabled()))
            {
                // Dump it
                FAIL(filter->DumpAsXML(buffer));
            }
        }
    }
    return mcsSUCCESS;
}

/*___oOo___*/
