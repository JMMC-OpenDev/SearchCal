/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: vobsORIGIN_FILTER.cpp,v 1.3 2005-11-16 10:47:55 scetre Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.2  2005/11/16 10:47:54  scetre
 * Updated documentation
 *
 * Revision 1.1  2005/11/15 14:54:33  scetre
 * Added filter on origin and B-V
 *
 ******************************************************************************/

/**
 * @file
 *  Definition of vobsORIGIN_FILTER class.
 */

static char *rcsId="@(#) $Id: vobsORIGIN_FILTER.cpp,v 1.3 2005-11-16 10:47:55 scetre Exp $"; 
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
#include "vobsORIGIN_FILTER.h"
#include "vobsPrivate.h"

/**
 * Class constructor
 */
vobsORIGIN_FILTER::vobsORIGIN_FILTER()
{
    strcpy(_name, vobsORIGIN_FILTER_NAME);
}

/**
 * Class destructor
 */
vobsORIGIN_FILTER::~vobsORIGIN_FILTER()
{
}

/*
 * Public methods
 */
/**
 * Set value to the filter
 *
 * @param origin origin name
 * @param ucd ucd name
 *
 * @return always mcsSUCCESS
 */
mcsCOMPL_STAT vobsORIGIN_FILTER::SetOriginName(mcsSTRING32 origin,
                                               mcsSTRING32 ucd)
{
    logTrace("vobsORIGIN_FILTER::SetOriginName()");

    strcpy(_origin, origin);
    strcpy(_ucd, ucd);

    return mcsSUCCESS;
}

/**
 * Apply origin filter on a list
 *
 * @param list the list on which the filter is applied
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is 
 * returned. 
 */
mcsCOMPL_STAT vobsORIGIN_FILTER::Apply(vobsSTAR_LIST *list)
{
    logTrace("vobsORIGIN_FILTER::Apply()");

    if (IsEnabled() == mcsTRUE)
    {
        // for each star of the list
        vobsSTAR *star;

        for (unsigned int el = 0; el < list->Size(); el++)
        {
            star = (vobsSTAR *)list->GetNextStar((mcsLOGICAL)(el==0));
            // Retreive property corresponding to the UCD
            vobsSTAR_PROPERTY *property = star->GetProperty(_ucd);
            // If property doen't exist and have different origin that the
            // wanted one
            if ((property == NULL) &&
                (strcmp(property->GetOrigin(), _origin)!=0))
            {
                // Remove it
                logTest("star %d not magnitude in the range\n", el+1);
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
