/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * @file
 * Definition of vobsORIGIN_FILTER class.
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
#include "vobsORIGIN_FILTER.h"
#include "vobsPrivate.h"

/**
 * Class constructor
 */
vobsORIGIN_FILTER::vobsORIGIN_FILTER(const char* filterId) : vobsFILTER(filterId)
{
}

/**
 * Class destructor
 */
vobsORIGIN_FILTER::~vobsORIGIN_FILTER()
{
}

/*
 * Public methods
 */

/**
 * Set value to the filter
 *
 * @param propId property id on which filter has to be applied
 * @param catalogId the catalog identifier to have
 *
 * @return always mcsSUCCESS
 */
mcsCOMPL_STAT vobsORIGIN_FILTER::SetCriteria(const char* propId, vobsORIGIN_INDEX originIndex)
{
    strcpy(_propId, propId);
    _originIndex = originIndex;

    return mcsSUCCESS;
}

/**
 * Apply origin filter on a list
 *
 * @param list the list on which the filter is applied
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
 */
mcsCOMPL_STAT vobsORIGIN_FILTER::Apply(vobsSTAR_LIST *list)
{
    if (IS_TRUE(IsEnabled()))
    {
        // For each star of the given star list
        vobsSTAR* starPtr = NULL;

        // For each star of the list
        // note: Remove() and GetNextStar() ensure proper list traversal:
        for (starPtr = list->GetNextStar(mcsTRUE); IS_NOT_NULL(starPtr); starPtr = list->GetNextStar(mcsFALSE))
        {
            // Get Star ID
            mcsSTRING64 starId;
            FAIL(starPtr->GetId(starId, sizeof (starId)));

            // Get property:
            vobsSTAR_PROPERTY* property = starPtr->GetProperty(_propId);

            // If property doen't exist or have different origin that the wanted one
            if (IS_NULL(property) || (property->GetOriginIndex() != _originIndex))
            {
                // Remove it
                logDebug("star '%s' has been removed by the filter '%s'", starId, GetId());

                list->RemoveRef(starPtr);
            }
            else
            {
                logDebug("star '%s' valid for the filter '%s'", starId, GetId());
            }
        }
    }

    return mcsSUCCESS;
}
/*
 * Protected methods
 */


/*
 * Private methods
 */


/*___oOo___*/
