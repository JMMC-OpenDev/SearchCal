/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: sclsvrDISTANCE_FILTER.cpp,v 1.1 2005-06-01 14:18:54 scetre Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 ******************************************************************************/

/**
 * \file
 *  Definition of sclsvrDISTANCE_FILTER class.
 */

static char *rcsId="@(#) $Id: sclsvrDISTANCE_FILTER.cpp,v 1.1 2005-06-01 14:18:54 scetre Exp $"; 
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
#include "sclsvrDISTANCE_FILTER.h"
#include "sclsvrPrivate.h"

#define sclsvrDISTANCE_FILTER_NAME "Filter by Distance"

/**
 * Class constructor
 */
sclsvrDISTANCE_FILTER::sclsvrDISTANCE_FILTER()
{
    strcpy(_name, sclsvrDISTANCE_FILTER_NAME);
}

/**
 * Class destructor
 */
sclsvrDISTANCE_FILTER::~sclsvrDISTANCE_FILTER()
{
}

/*
 * Public methods
 */
/**
 * Set value to the filter
 *
 * \param raRef right ascension of the science star
 * \param decRef declinaison of the science star
 * \param raRange right ascension range
 * \param decRange declinaision range
 *
 * \return always mcsSUCCESS
 */
mcsCOMPL_STAT sclsvrDISTANCE_FILTER::SetDistanceValue(mcsSTRING32 raRef,
                                                      mcsSTRING32 decRef,
                                                      mcsFLOAT raRange,
                                                      mcsFLOAT decRange)
{
    logTrace("sclsvrDISTANCE_FILTER::SetDistanceValue()");

    strcpy(_raRef, raRef);
    strcpy(_decRef, decRef);
    _raRange  = raRange;
    _decRange = decRange;
    
    return mcsSUCCESS;
}

/**
 * Get value of the filter 
 *
 * \param raRef right ascension of the science star
 * \param decRef declinaison of the science star
 * \param raRange right ascension range
 * \param decRange declinaision range
 *
 * \return always mcsSUCCESS
 */
mcsCOMPL_STAT sclsvrDISTANCE_FILTER::GetDistanceValue(mcsSTRING32 *raRef,
                                                      mcsSTRING32 *decRef,
                                                      mcsFLOAT *raRange,
                                                      mcsFLOAT *decRange)
{
    logTrace("sclsvrDISTANCE_FILTER::SetDistanceValue()");

    strcpy(*raRef, _raRef);
    strcpy(*decRef, _decRef);
    *raRange  = _raRange;
    *decRange = _decRange;

    return mcsSUCCESS;
}

/**
 *
 */
mcsCOMPL_STAT sclsvrDISTANCE_FILTER::Apply(sclsvrCALIBRATOR_LIST *list)
{
    logTrace("sclsvrDISTANCE_FILTER::Apply()");

    // create a star correponding to the science object
    sclsvrCALIBRATOR scienceStar;
    if (scienceStar.SetPropertyValue(vobsSTAR_POS_EQ_RA_MAIN,
                                     _raRef, "") == mcsFAILURE)
    {
        return mcsFAILURE;
    }
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

    // for each calibrator of the list
    sclsvrCALIBRATOR *calibrator;
    for (unsigned int el = 0; el < list->Size(); el++)
    {
        // if the calibrator is different of the science star according to the
        // criteria list
        (calibrator=(sclsvrCALIBRATOR *)list->GetNextStar((mcsLOGICAL)(el==0)));
        if (calibrator->IsSame(scienceStar, &criteriaList) != mcsTRUE)
        {
            // Remove it
            logTest("calibrator %d not in the box\n", el+1);
            list->Remove(*calibrator);
            el = el-1;
        }
        else
        {
            logTest("calibrator %d in the box\n", el+1);
            
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
