/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: sclsvrMULTIPLICITY_FILTER.cpp,v 1.1 2005-06-01 14:18:54 scetre Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 ******************************************************************************/

/**
 * \file
 *  Definition of sclsvrMULTIPLICITY_FILTER class.
 */

static char *rcsId="@(#) $Id: sclsvrMULTIPLICITY_FILTER.cpp,v 1.1 2005-06-01 14:18:54 scetre Exp $"; 
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
#include "sclsvrMULTIPLICITY_FILTER.h"
#include "sclsvrPrivate.h"

/**
 * Class constructor
 */
sclsvrMULTIPLICITY_FILTER::sclsvrMULTIPLICITY_FILTER()
{
    strcpy(_name, sclsvrMULTIPLICITY_FILTER_NAME);        
}

/**
 * Class destructor
 */
sclsvrMULTIPLICITY_FILTER::~sclsvrMULTIPLICITY_FILTER()
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
mcsCOMPL_STAT sclsvrMULTIPLICITY_FILTER::Apply(sclsvrCALIBRATOR_LIST *list)
{
    logTrace("sclsvrMULTIPLICITY_FILTER::Apply()");

    if (IsEnable() == mcsTRUE)
    {
        sclsvrCALIBRATOR *calibrator;
        for (unsigned int el = 0; el < list->Size(); el++)
        {
            calibrator=
                (sclsvrCALIBRATOR *)list->GetNextStar((mcsLOGICAL)(el==0));
            // if it is not possible to get the visibility, remove the star
            if (calibrator->IsPropertySet(vobsSTAR_CODE_MULT_FLAG) == mcsTRUE)
            {
                // Remove it
                logTest("calibrator %d had multiplicity %s\n",
                        el+1, 
                        calibrator->
                        GetPropertyValue(vobsSTAR_CODE_MULT_FLAG));
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
