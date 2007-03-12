/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: vobsVARIABILITY_FILTER.cpp,v 1.9 2007-03-12 13:51:11 scetre Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.8  2006/04/07 08:23:00  gzins
 * Removed useless \n in log messages
 *
 * Revision 1.7  2006/03/03 15:03:28  scetre
 * Changed rcsId to rcsId __attribute__ ((unused))
 *
 * Revision 1.6  2005/12/14 15:10:03  scetre
 * Added log information about deleted star in filters
 *
 * Revision 1.5  2005/12/13 16:30:33  lafrasse
 * Added filter Id management through additional constructor parameter
 *
 * Revision 1.4  2005/11/29 13:49:19  gzins
 * Removed filter name setting
 *
 * Revision 1.3  2005/11/23 08:35:10  scetre
 * changed vobsCODE_VARIAB to vobsCODE_VARIAB_V3
 *
 * Revision 1.2  2005/06/21 06:20:45  scetre
 * Changed method Disable() and Enable() to Disabled() and Enabled()
 * Changed '\' in doxygen documentatiuon to '@'
 *
 * Revision 1.1  2005/06/20 11:31:53  scetre
 * Added filter class
 *
 * Revision 1.2  2005/06/07 12:36:28  scetre
 * Updated documentation
 *
 * Revision 1.1  2005/06/01 14:18:54  scetre
 * Added filters and filter list objects.
 * Changed logExtDbg to logTrace
 *
 ******************************************************************************/

/**
 * @file
 *  Definition of vobsVARIABILITY_FILTER class.
 */

static char *rcsId __attribute__ ((unused)) ="@(#) $Id: vobsVARIABILITY_FILTER.cpp,v 1.9 2007-03-12 13:51:11 scetre Exp $"; 

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
            // if it is not possible to get the visibility, remove the star
            if (star->IsPropertySet(vobsSTAR_CODE_VARIAB_V3) == mcsTRUE)
            {
                logInfo("star %s has been removed by the filter '%s'", starId, GetId());
                // Check that flag is not "C" for remove star because "C" means
                // no flag : as beahviour that if flag is not affected
                if (strcmp(star->GetPropertyValue(vobsSTAR_CODE_VARIAB_V3),
                           "C") != 0)
                {
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
