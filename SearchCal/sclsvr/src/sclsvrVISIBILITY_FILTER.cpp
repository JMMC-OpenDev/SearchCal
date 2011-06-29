/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * @file
 * Definition of sclsvrVISIBILITY_FILTER class.
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
#include "sclsvrVISIBILITY_FILTER.h"
#include "sclsvrPrivate.h"


/**
 * Class constructor
 */
sclsvrVISIBILITY_FILTER::sclsvrVISIBILITY_FILTER(const char *filterId):
vobsFILTER(filterId)
{
}

/**
 * Class destructor
 */
sclsvrVISIBILITY_FILTER::~sclsvrVISIBILITY_FILTER()
{
}

/*
 * Public methods
 */
/**
 * Set value to the filter.
 *
 * @param visMax maximum accuracy 
 *
 * @return always mcsSUCCESS
 */
mcsCOMPL_STAT sclsvrVISIBILITY_FILTER::SetVisibilityValue(mcsDOUBLE visMax)
{
    logTrace("sclsvrVISIBILITY_FILTER::SetVisibilityValue()");

    _visMax = visMax;
    
    return mcsSUCCESS;
}
 
/**
 * Get value from the filter
 *
 * @param visMax maximum accuracy
 *
 * @return always mcsSUCCESS
 */
mcsCOMPL_STAT sclsvrVISIBILITY_FILTER::GetVisibilityValue(mcsDOUBLE *visMax)
{
    logTrace("sclsvrVISIBILITY_FILTER::GetVisibilityValue()");

    *visMax = _visMax;

    return mcsSUCCESS;
}

/**
 * Apply visibility filtering on a given star list.
 *
 * @param list the list on which the filter is applied
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is 
 * returned. 
 */
mcsCOMPL_STAT sclsvrVISIBILITY_FILTER::Apply(vobsSTAR_LIST *list)
{
    logTrace("sclsvrVISIBILITY_FILTER::Apply()");

    // If the visibility is enabled
    if (IsEnabled() == mcsTRUE)
    {
        // for each star of the list
        vobsSTAR *star;
        mcsDOUBLE starVis, starVisError;
        for (unsigned int el = 0; el < list->Size(); el++)
        {
            star = (vobsSTAR *)list->GetNextStar((mcsLOGICAL)(el==0));
            mcsSTRING32 starId;
            // Get Star ID
            if (star->GetId(starId, sizeof(starId)) == mcsFAILURE)
            {
                return mcsFAILURE;
            }

            // if it is not possible to get the visibility or the visibility 
            // error, remove the star
            if ((star->GetPropertyValue(sclsvrCALIBRATOR_VIS2, &starVis)
                == mcsFAILURE) ||
                (star->GetPropertyValue(sclsvrCALIBRATOR_VIS2_ERROR, 
                                        &starVisError) == mcsFAILURE))
            {
                // Remove it
                logInfo("star %s has been removed by the filter '%s' : property %s is not set", starId, GetId(), "VIS2 or VIS2err");
                if (list->Remove(*star) == mcsFAILURE)
                {
                    return mcsFAILURE;
                }

                el = el-1;
            }
            // if it is possible to get the star
            else
            {
                // if the value of the star visibility error / visibility
                // is lower than this enter by the user, remove it
                if (fabs(starVisError/starVis) >= (_visMax/100.))   
                {
                    // Remove it
                    logInfo("star %s has been removed by the filter '%s'", starId, GetId());
                    if (list->Remove(*star) == mcsFAILURE)
                    {
                        return mcsFAILURE;
                    }
                    el = el-1;
                }
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
