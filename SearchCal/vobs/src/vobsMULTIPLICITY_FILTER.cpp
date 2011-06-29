/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * @file
 * Definition of vobsMULTIPLICITY_FILTER class.
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
#include "vobsMULTIPLICITY_FILTER.h"
#include "vobsPrivate.h"

/**
 * Class constructor
 */
vobsMULTIPLICITY_FILTER::vobsMULTIPLICITY_FILTER(const char* filterId):vobsFILTER(filterId)
{
}

/**
 * Class destructor
 */
vobsMULTIPLICITY_FILTER::~vobsMULTIPLICITY_FILTER()
{
}

/*
 * Public methods
 */
/**
 * Apply multiplicity filter on a list
 *
 * @param list the list on which the filter is applied
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is 
 * returned. 
 */
mcsCOMPL_STAT vobsMULTIPLICITY_FILTER::Apply(vobsSTAR_LIST *list)
{
    logTrace("vobsMULTIPLICITY_FILTER::Apply()");

    if (IsEnabled() == mcsTRUE)
    {
        vobsSTAR *star;
        for (unsigned int el = 0; el < list->Size(); el++)
        {
            star=
                (vobsSTAR *)list->GetNextStar((mcsLOGICAL)(el==0));
            mcsSTRING32 starId;
            // Get Star ID
            if (star->GetId(starId, sizeof(starId)) == mcsFAILURE)
            {
                return mcsFAILURE;
            }

            // if it is not possible to get the visibility, remove the star
            if (star->IsPropertySet(vobsSTAR_CODE_MULT_FLAG) == mcsTRUE)
            {
                // Remove it
                logTest("star %d had multiplicity %s", el+1, 
                        star->GetPropertyValue(vobsSTAR_CODE_MULT_FLAG));
                logInfo("star %s has been removed by the filter '%s'", starId, GetId());
                
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
