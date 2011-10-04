/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * @file
 * Definition of vobsBV_FILTER class.
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
#include "vobsBV_FILTER.h"
#include "vobsPrivate.h"

/**
 * Class constructor
 */
vobsBV_FILTER::vobsBV_FILTER(const char* filterId):vobsFILTER(filterId)
{
}

/**
 * Class destructor
 */
vobsBV_FILTER::~vobsBV_FILTER()
{
}

/*
 * Public methods
 */
/**
 * Set value to the filter
 *
 * @param magValue magnitude value
 *
 * @return always mcsSUCCESS
 */
mcsCOMPL_STAT vobsBV_FILTER::SetMagnitudeValue(mcsDOUBLE magValue)
{
    logTrace("vobsBV_FILTER::SetMagnitudeValue()");

    _magValue = magValue;
    
    return mcsSUCCESS;
}

/**
 * Get value of the filter 
 *  
 * @param magValue magnitude value
 *
 * @return always mcsSUCCESS
 */
mcsCOMPL_STAT vobsBV_FILTER::GetMagnitudeValue(mcsDOUBLE *magValue)
{
    logTrace("vobsBV_FILTER::GetMagnitudeValue()");

    *magValue = _magValue;
    
    return mcsSUCCESS;
}

/**
 * Apply magnitude filter on a list
 *
 * @param list the list on which the filter is applied
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is 
 * returned. 
 */
mcsCOMPL_STAT vobsBV_FILTER::Apply(vobsSTAR_LIST *list)
{
    logTrace("vobsBV_FILTER::Apply()");

    if (IsEnabled() == mcsTRUE)
    {
        vobsSTAR *star;

        for (unsigned int el = 0; el < list->Size(); el++)
        {
            star = (vobsSTAR *)list->GetNextStar((mcsLOGICAL)(el==0));
            
            mcsSTRING32 starId;
            // Get Star ID
            if (star->GetId(starId, sizeof(starId)) == mcsFAILURE)
            {
                return mcsFAILURE;
            }
            // Check if B and V magnitude are defined
            if ((star->IsPropertySet(vobsSTAR_PHOT_JHN_B) == mcsTRUE) ||
                (star->IsPropertySet(vobsSTAR_PHOT_JHN_V) == mcsTRUE))
            {
                // If OK: 
                // Get magB
                mcsDOUBLE magB;
                if (star->GetPropertyValue(vobsSTAR_PHOT_JHN_B, &magB) == mcsFAILURE)
                {
                    return mcsFAILURE;
                }
                // Get magV
                mcsDOUBLE magV;
                if (star->GetPropertyValue(vobsSTAR_PHOT_JHN_V, &magV) == mcsFAILURE)
                {
                    return mcsFAILURE;
                }
                // If B-V > magValue remove star
                if (magB - magV > _magValue)
                {
                    logTest("star %s has been removed by the filter '%s'", starId, GetId());
                    if (list->Remove(*star) == mcsFAILURE)
                    {
                        return mcsFAILURE;
                    }
                    el = el-1;
                }
            }
            // If B or V magnitude are not defined, remove star
            else 
            {
                // Remove it
                logTest("star %s has been removed by the filter '%s' : property %s is not set", starId, GetId(), "B or V");
                if (list->Remove(*star) == mcsFAILURE)
                {
                    return mcsFAILURE;
                }
                el = el-1;
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
