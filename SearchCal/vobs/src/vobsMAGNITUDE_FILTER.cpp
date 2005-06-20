/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: vobsMAGNITUDE_FILTER.cpp,v 1.1 2005-06-20 11:31:53 scetre Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
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
 *  Definition of vobsMAGNITUDE_FILTER class.
 */

static char *rcsId="@(#) $Id: vobsMAGNITUDE_FILTER.cpp,v 1.1 2005-06-20 11:31:53 scetre Exp $"; 
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
#include "vobsMAGNITUDE_FILTER.h"
#include "vobsPrivate.h"

/**
 * Class constructor
 */
vobsMAGNITUDE_FILTER::vobsMAGNITUDE_FILTER()
{
    strcpy(_name, vobsMAGNITUDE_FILTER_NAME);    
}

/**
 * Class destructor
 */
vobsMAGNITUDE_FILTER::~vobsMAGNITUDE_FILTER()
{
}

/*
 * Public methods
 */
/**
 * Set value to the filter
 *
 * @param band magnitude band
 * @param magValue magnitude value
 * @param magRange magnitude range
 *
 * @return always mcsSUCCESS
 */
mcsCOMPL_STAT vobsMAGNITUDE_FILTER::SetMagnitudeValue(mcsSTRING32 band,
                                                        mcsFLOAT magValue,
                                                        mcsFLOAT magRange)
{
    logTrace("vobsMAGNITUDE_FILTER::SetMagnitudeValue()");

    strcpy(_band, band);
    _magValue = magValue;
    _magRange = magRange;
    
    return mcsSUCCESS;
}

/**
 * Get value of the filter 
 *  
 * @param band magnitude band
 * @param magValue magnitude value
 * @param magRange magnitude range
 *
 * @return always mcsSUCCESS
 */
mcsCOMPL_STAT vobsMAGNITUDE_FILTER::GetMagnitudeValue(mcsSTRING32 *band,
                                                        mcsFLOAT *magValue,
                                                        mcsFLOAT *magRange)
{
    logTrace("vobsMAGNITUDE_FILTER::GetMagnitudeValue()");

    strcpy(*band, _band);
    *magValue = _magValue;
    *magRange = _magRange;
    
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
mcsCOMPL_STAT vobsMAGNITUDE_FILTER::Apply(vobsSTAR_LIST *list)
{
    logTrace("vobsMAGNITUDE_FILTER::Apply()");

    if (IsEnable() == mcsTRUE)
    {
        // Create the UCD corresponding to the band
        mcsSTRING256 magnitudeUcd;
        strcpy(magnitudeUcd, "PHOT_JHN_");
        strcat(magnitudeUcd, _band);
        // create a star correponding to the science object
        vobsSTAR scienceStar;
        if (scienceStar.SetPropertyValue(magnitudeUcd,
                                         _magValue, "") == mcsFAILURE)
        {
            return mcsFAILURE;
        }
        // Create a criterialist
        vobsSTAR_COMP_CRITERIA_LIST criteriaList;
        // Add criteria on right ascension
        if (criteriaList.Add(magnitudeUcd, _magRange) == mcsFAILURE)
        {
            return mcsFAILURE;
        }
        // for each star of the list
        vobsSTAR *star;

        for (unsigned int el = 0; el < list->Size(); el++)
        {
            // if the star is different of the science star according to
            // the criteria list
            star=
                (vobsSTAR *)list->GetNextStar((mcsLOGICAL)(el==0));
            if (star->IsSame(scienceStar, &criteriaList) != mcsTRUE )
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
