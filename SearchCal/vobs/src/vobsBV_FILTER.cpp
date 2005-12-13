/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: vobsBV_FILTER.cpp,v 1.7 2005-12-13 16:30:33 lafrasse Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.6  2005/11/29 13:49:19  gzins
 * Removed filter name setting
 *
 * Revision 1.5  2005/11/29 10:25:58  gzins
 * Minor change
 *
 * Revision 1.4  2005/11/16 12:53:18  scetre
 * Updated documentation
 *
 * Revision 1.3  2005/11/16 10:47:55  scetre
 * Updated documentation
 *
 * Revision 1.2  2005/11/16 10:47:54  scetre
 * Updated documentation
 *
 * Revision 1.1  2005/11/15 14:54:33  scetre
 * Added filter on origin and B-V
 *
 ******************************************************************************/

/**
 * @file
 * Definition of vobsBV_FILTER class.
 */

static char *rcsId="@(#) $Id: vobsBV_FILTER.cpp,v 1.7 2005-12-13 16:30:33 lafrasse Exp $"; 
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
mcsCOMPL_STAT vobsBV_FILTER::SetMagnitudeValue(mcsFLOAT magValue)
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
mcsCOMPL_STAT vobsBV_FILTER::GetMagnitudeValue(mcsFLOAT *magValue)
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
            star=
                (vobsSTAR *)list->GetNextStar((mcsLOGICAL)(el==0));
            // Check if B and V magnitude are defined
            if ((star->IsPropertySet(vobsSTAR_PHOT_JHN_B) == mcsTRUE) ||
                (star->IsPropertySet(vobsSTAR_PHOT_JHN_V) == mcsTRUE))
            {
                // If OK: 
                // Get magB
                mcsFLOAT magB;
                if (star->GetPropertyValue(vobsSTAR_PHOT_JHN_B, &magB) ==
                    mcsFAILURE)
                {
                    return mcsFAILURE;
                }
                // Get magV
                mcsFLOAT magV;
                if (star->GetPropertyValue(vobsSTAR_PHOT_JHN_V, &magV) ==
                    mcsFAILURE)
                {
                    return mcsFAILURE;
                }
                // If B-V > magValue remove star
                if (magB - magV > _magValue)
                {
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
