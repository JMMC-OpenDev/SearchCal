/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * @file
 * Definition of vobsVARIABILITY_FILTER class.
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
#include "vobsVARIABILITY_FILTER.h"
#include "vobsPrivate.h"

/**
 * Class constructor
 */
vobsVARIABILITY_FILTER::vobsVARIABILITY_FILTER(const char* filterId):vobsFILTER(filterId)
{
}

/**
 * Class destructor
 */
vobsVARIABILITY_FILTER::~vobsVARIABILITY_FILTER()
{
}

/*
 * Public methods
 */
/**
 * Apply variability filter on a list
 *
 * @param list the list on which the filter is applied
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is 
 * returned. 
 */
mcsCOMPL_STAT vobsVARIABILITY_FILTER::Apply(vobsSTAR_LIST *list)
{
    logTrace("vobsVARIABILITY_FILTER::Apply()");

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
            // if it is possible to get the varflag1, remove the star
            if (star->IsPropertySet(vobsSTAR_CODE_VARIAB_V1) == mcsTRUE)
            {
                logInfo("star %s has been removed by the filter '%s'",
                        starId, GetId());
                // Remove it
                logTest("star %d had variability %s",
                        el+1, 
                        star->GetPropertyValue(vobsSTAR_CODE_VARIAB_V1));
                if (list->Remove(*star) == mcsFAILURE)
                {
                    return mcsFAILURE;
                }
                el = el-1;            
            }
            // if it is possible to get the varflag2, remove the star
            else if (star->IsPropertySet(vobsSTAR_CODE_VARIAB_V2) == mcsTRUE)
            {
                logInfo("star %s has been removed by the filter '%s'",
                        starId, GetId());
                // Remove it
                logTest("star %d had variability %s",
                        el+1, 
                        star->GetPropertyValue(vobsSTAR_CODE_VARIAB_V2));
                if (list->Remove(*star) == mcsFAILURE)
                {
                    return mcsFAILURE;
                }
                el = el-1;            
            }
            // if it is possible to get the varflag3 and flag is not "C",
            // remove the star
            else if (star->IsPropertySet(vobsSTAR_CODE_VARIAB_V3) == mcsTRUE)
            {
                // Check that flag is not "C" for remove star because "C" means
                // no flag : same behaviour that if flag is not affected
                if (strcmp(star->GetPropertyValue(vobsSTAR_CODE_VARIAB_V3),
                           "C") != 0)
                {
                    logInfo("star %s has been removed by the filter '%s'",
                            starId, GetId());
                    // Remove it
                    logTest("star %d had variability %s",
                            el+1, 
                            star->GetPropertyValue(vobsSTAR_CODE_VARIAB_V3));
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
