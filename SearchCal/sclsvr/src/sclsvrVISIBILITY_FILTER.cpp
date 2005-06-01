/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: sclsvrVISIBILITY_FILTER.cpp,v 1.1 2005-06-01 14:18:54 scetre Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 ******************************************************************************/

/**
 * \file
 *  Definition of sclsvrVISIBILITY_FILTER class.
 */

static char *rcsId="@(#) $Id: sclsvrVISIBILITY_FILTER.cpp,v 1.1 2005-06-01 14:18:54 scetre Exp $"; 
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
#include "sclsvrVISIBILITY_FILTER.h"
#include "sclsvrPrivate.h"

/**
 * Class constructor
 */
sclsvrVISIBILITY_FILTER::sclsvrVISIBILITY_FILTER()
{
    strcpy(_name, sclsvrVISIBILITY_FILTER_NAME);    
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
 * Set value to the filter
 *
 * \param 
 *
 * \return always mcsSUCCESS
 */
mcsCOMPL_STAT sclsvrVISIBILITY_FILTER::SetVisibilityValue(mcsFLOAT visMax)
{
    logTrace("sclsvrVISIBILITY_FILTER::SetVisibilityValue()");

    _visMax = visMax;
    
    return mcsSUCCESS;
}
 
/**
 * Get value to the filter
 *
 * \param 
 *
 * \return always mcsSUCCESS
 */
mcsCOMPL_STAT sclsvrVISIBILITY_FILTER::GetVisibilityValue(mcsFLOAT *visMax)
{
    logTrace("sclsvrVISIBILITY_FILTER::GetVisibilityValue()");

    *visMax = _visMax;

    return mcsSUCCESS;
}

/**
 * Apply visibility filter on a list
 *
 * @param list the list on which the filter is applied
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is 
 * returned. 
 */
mcsCOMPL_STAT sclsvrVISIBILITY_FILTER::Apply(sclsvrCALIBRATOR_LIST *list)
{
    logTrace("sclsvrMAGNITUDE_FILTER::Apply()");

    if (IsEnable() == mcsTRUE)
    {
        // for each calibrator of the list
        sclsvrCALIBRATOR *calibrator;
        mcsFLOAT calibratorVis, calibratorVisError;
        for (unsigned int el = 0; el < list->Size(); el++)
        {
            calibrator=
                (sclsvrCALIBRATOR *)list->GetNextStar((mcsLOGICAL)(el==0));
            // if it is not possible to get the visibility or the visibility 
            // error, remove the star
            if ((calibrator->GetPropertyValue(sclsvrCALIBRATOR_VIS2,
                                              &calibratorVis) ==
                 mcsFAILURE)||
                (calibrator->GetPropertyValue(sclsvrCALIBRATOR_VIS2_ERROR,
                                              &calibratorVisError) ==
                 mcsFAILURE) )
            {
                // Remove it
                logTest("calibrator %d not visibility enough\n", el+1);
                if (list->Remove(*calibrator) == mcsFAILURE)
                {
                    return mcsFAILURE;
                }
                el = el-1;            
            }
            // if it is possible to get the star
            else
            {
                // if the value of the calibrator visibility error / visibility
                // is lower than this enter by the user, remove it
                if (calibratorVisError/calibratorVis >= _visMax)   
                {
                    // Remove it
                    logTest("calibrator %d not visibility enough\n", el+1);
                    if (list->Remove(*calibrator) == mcsFAILURE)
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
