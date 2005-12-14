/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: vobsDISTANCE_FILTER.cpp,v 1.6 2005-12-14 15:07:53 scetre Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.5  2005/12/13 16:30:33  lafrasse
 * Added filter Id management through additional constructor parameter
 *
 * Revision 1.4  2005/11/29 13:49:19  gzins
 * Removed filter name setting
 *
 * Revision 1.3  2005/11/29 10:33:02  gzins
 * Minor changes
 *
 * Revision 1.2  2005/06/21 06:20:45  scetre
 * Changed method Disable() and Enable() to Disabled() and Enabled()
 * Changed '\' in doxygen documentatiuon to '@'
 *
 * Revision 1.1  2005/06/20 11:44:25  scetre
 * Added filter class
 *
 * Revision 1.2  2005/06/07 12:36:27  scetre
 * Updated documentation
 *
 * Revision 1.1  2005/06/01 14:18:54  scetre
 * Added filters and filter list objects.
 * Changed logExtDbg to logTrace
 *
 ******************************************************************************/

/**
 * @file
 * Definition of vobsDISTANCE_FILTER class.
 */

static char *rcsId="@(#) $Id: vobsDISTANCE_FILTER.cpp,v 1.6 2005-12-14 15:07:53 scetre Exp $"; 
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
#include "vobsDISTANCE_FILTER.h"
#include "vobsPrivate.h"

/**
 * Class constructor
 */
vobsDISTANCE_FILTER::vobsDISTANCE_FILTER(const char* filterId):vobsFILTER(filterId)
{
}

/**
 * Class destructor
 */
vobsDISTANCE_FILTER::~vobsDISTANCE_FILTER()
{
}

/*
 * Public methods
 */
/**
 * Set value to the filter
 *
 * @param raRef right ascension of the science star
 * @param decRef declinaison of the science star
 * @param raRange right ascension range
 * @param decRange declinaision range
 *
 * @return always mcsSUCCESS
 */
mcsCOMPL_STAT vobsDISTANCE_FILTER::SetDistanceValue(mcsSTRING32 raRef,
                                                    mcsSTRING32 decRef,
                                                    mcsFLOAT raRange,
                                                    mcsFLOAT decRange)
{
    logTrace("vobsDISTANCE_FILTER::SetDistanceValue()");

    // Copy right ascension and declinaison get in parameter
    strcpy(_raRef, raRef);
    strcpy(_decRef, decRef);
    // Copy right ascension and declinaison range get as parameter
    _raRange  = raRange;
    _decRange = decRange;
    
    return mcsSUCCESS;
}

/**
 * Get value of the filter 
 *
 * @param raRef right ascension of the science star
 * @param decRef declinaison of the science star
 * @param raRange right ascension range
 * @param decRange declinaision range
 *
 * @return always mcsSUCCESS
 */
mcsCOMPL_STAT vobsDISTANCE_FILTER::GetDistanceValue(mcsSTRING32 *raRef,
                                                    mcsSTRING32 *decRef,
                                                    mcsFLOAT *raRange,
                                                    mcsFLOAT *decRange)
{
    logTrace("vobsDISTANCE_FILTER::SetDistanceValue()");
    
    // Copy right ascension and declinaison get in parameter
    strcpy(*raRef, _raRef);
    strcpy(*decRef, _decRef);

    // Copy right ascension and declinaison range get as parameter
    *raRange  = _raRange;
    *decRange = _decRange;

    return mcsSUCCESS;
}

/**
 * Apply the filter on a list
 *
 * @param list star list on wich the filter is applied
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is 
 * returned.
 */
mcsCOMPL_STAT vobsDISTANCE_FILTER::Apply(vobsSTAR_LIST *list)
{
    logTrace("vobsDISTANCE_FILTER::Apply()");

    // create a star correponding to the science object
    vobsSTAR scienceStar;
    // Set right ascension property (ref) to this star
    if (scienceStar.SetPropertyValue(vobsSTAR_POS_EQ_RA_MAIN,
                                     _raRef, "") == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    // Set declinaison property (ref) to this star
    if (scienceStar.SetPropertyValue(vobsSTAR_POS_EQ_DEC_MAIN,
                                     _decRef, "") == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    // Create a criterialist
    vobsSTAR_COMP_CRITERIA_LIST criteriaList;
    // Add criteria on right ascension
    if (criteriaList.Add(vobsSTAR_POS_EQ_RA_MAIN, _raRange) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    // Add criteria on declinaison
    if (criteriaList.Add(vobsSTAR_POS_EQ_DEC_MAIN, _decRange) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    // for each star of the list
    vobsSTAR *star;
    for (unsigned int el = 0; el < list->Size(); el++)
    {
        (star=(vobsSTAR *)list->GetNextStar((mcsLOGICAL)(el==0)));
        mcsSTRING32 starId;
        // Get Star ID
        if (star->GetId(starId, sizeof(starId)) == mcsFAILURE)
        {
            return mcsFAILURE;
        }
        // if the star is different of the science star according to the
        // criteria list
        if (star->IsSame(scienceStar, &criteriaList) != mcsTRUE)
        {
            // Remove it
            logTest("star %s not in the box\n", starId);
            list->Remove(*star);
            el = el-1;
        }
        else
        {
            logTest("star %s in the box\n", starId);
            
        }
    }
    
    return mcsSUCCESS;    
}


/*___oOo___*/
