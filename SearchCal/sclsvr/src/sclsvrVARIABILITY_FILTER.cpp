/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: sclsvrVARIABILITY_FILTER.cpp,v 1.1 2005-06-01 14:18:54 scetre Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 ******************************************************************************/

/**
 * \file
 *  Definition of sclsvrVARIABILITY_FILTER class.
 */

static char *rcsId="@(#) $Id: sclsvrVARIABILITY_FILTER.cpp,v 1.1 2005-06-01 14:18:54 scetre Exp $"; 
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
#include "sclsvrVARIABILITY_FILTER.h"
#include "sclsvrPrivate.h"

/**
 * Class constructor
 */
sclsvrVARIABILITY_FILTER::sclsvrVARIABILITY_FILTER()
{
    strcpy(_name, sclsvrVARIABILITY_FILTER_NAME);        
}

/**
 * Class destructor
 */
sclsvrVARIABILITY_FILTER::~sclsvrVARIABILITY_FILTER()
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
mcsCOMPL_STAT sclsvrVARIABILITY_FILTER::Apply(sclsvrCALIBRATOR_LIST *list)
{
    logTrace("sclsvrVARIABILITY_FILTER::Apply()");

    if (IsEnable() == mcsTRUE)
    {
        sclsvrCALIBRATOR *calibrator;
        for (unsigned int el = 0; el < list->Size(); el++)
        {
            calibrator=
                (sclsvrCALIBRATOR *)list->GetNextStar((mcsLOGICAL)(el==0));
            // if it is not possible to get the visibility, remove the star
            if (calibrator->IsPropertySet(vobsSTAR_CODE_VARIAB) == mcsTRUE)
            {
                // Remove it
                logTest("calibrator %d had variability %s\n",
                        el+1, 
                        calibrator->GetPropertyValue(vobsSTAR_CODE_VARIAB));
                if (list->Remove(*calibrator) == mcsFAILURE)
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
