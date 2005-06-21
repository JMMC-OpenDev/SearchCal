/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: sclsvrVISIBILITY_FILTER.cpp,v 1.4 2005-06-21 06:21:58 scetre Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.3  2005/06/20 14:42:11  scetre
 * Changed call to mother class from sclsvrFILTER to vobsFILTER after filter removed to vobs module
 *
 * Revision 1.2  2005/06/13 10:22:47  scetre
 * Updated documentation
 *
 * Revision 1.1  2005/06/01 14:18:54  scetre
 * Added filters and filter list objects.
 * Changed logExtDbg to logTrace
 *
 ******************************************************************************/

/**
 * \file
 *  Definition of sclsvrVISIBILITY_FILTER class.
 */

static char *rcsId="@(#) $Id: sclsvrVISIBILITY_FILTER.cpp,v 1.4 2005-06-21 06:21:58 scetre Exp $"; 
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
 * \param visMax maximum accuracy 
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
 * \param visMax maximum accuracy
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
mcsCOMPL_STAT sclsvrVISIBILITY_FILTER::Apply(vobsSTAR_LIST *list)
{
    logTrace("sclsvrVISIBILITY_FILTER::Apply()");

    if (IsEnabled() == mcsTRUE)
    {
        // for each star of the list
        vobsSTAR *star;
        mcsFLOAT starVis, starVisError;
        for (unsigned int el = 0; el < list->Size(); el++)
        {
            star=
                (vobsSTAR *)list->GetNextStar((mcsLOGICAL)(el==0));
            // if it is not possible to get the visibility or the visibility 
            // error, remove the star
            if ((star->GetPropertyValue(sclsvrCALIBRATOR_VIS2,
                                              &starVis) ==
                 mcsFAILURE)||
                (star->GetPropertyValue(sclsvrCALIBRATOR_VIS2_ERROR,
                                              &starVisError) ==
                 mcsFAILURE) )
            {
                // Remove it
                logTest("star %d not visibility enough\n", el+1);
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
                if (starVisError/starVis >= _visMax)   
                {
                    // Remove it
                    logTest("star %d not visibility enough\n", el+1);
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
