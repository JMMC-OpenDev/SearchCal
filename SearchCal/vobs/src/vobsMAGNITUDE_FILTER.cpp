/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: vobsMAGNITUDE_FILTER.cpp,v 1.8 2009-04-17 15:29:48 lafrasse Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.7  2009/02/19 16:56:37  lafrasse
 * Improved lisibility.
 *
 * Revision 1.6  2006/03/03 15:03:27  scetre
 * Changed rcsId to rcsId __attribute__ ((unused))
 *
 * Revision 1.5  2005/12/14 15:07:53  scetre
 * Added log information about deleted star in filters
 *
 * Revision 1.4  2005/12/13 16:30:33  lafrasse
 * Added filter Id management through additional constructor parameter
 *
 * Revision 1.3  2005/11/29 13:49:19  gzins
 * Removed filter name setting
 *
 * Revision 1.2  2005/06/21 06:20:45  scetre
 * Changed method Disable() and Enable() to Disabled() and Enabled()
 * Changed '\' in doxygen documentatiuon to '@'
 *
 * Revision 1.1  2005/06/20 11:31:53  scetre
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
 * Definition of vobsMAGNITUDE_FILTER class.
 */

static char *rcsId __attribute__ ((unused)) ="@(#) $Id: vobsMAGNITUDE_FILTER.cpp,v 1.8 2009-04-17 15:29:48 lafrasse Exp $"; 

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
vobsMAGNITUDE_FILTER::vobsMAGNITUDE_FILTER(const char* filterId):vobsFILTER(filterId)
{
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

    if (IsEnabled() == mcsTRUE)
    {
        // Create the UCD corresponding to the band
        mcsSTRING256 magnitudeUcd;
        strcpy(magnitudeUcd, "PHOT_JHN_");
        strcat(magnitudeUcd, _band);

        // Create a star correponding to the reference object
        vobsSTAR referenceStar;
        if (referenceStar.SetPropertyValue(magnitudeUcd,
                                         _magValue, "") == mcsFAILURE)
        {
            return mcsFAILURE;
        }

        // Add criteria on magnitude
        vobsSTAR_COMP_CRITERIA_LIST criteriaList;
        if (criteriaList.Add(magnitudeUcd, _magRange) == mcsFAILURE)
        {
            return mcsFAILURE;
        }

        // For each star of the given list
        vobsSTAR* star = NULL;
        mcsLOGICAL firstIteration = mcsTRUE;
        for (unsigned int el = 0; el < list->Size(); el++)
        {
            // Get first (if in the first iteration), or following ones
            firstIteration = (mcsLOGICAL) (el == 0); // computed each time as 'el' could be decreased
            star = list->GetNextStar(firstIteration);
            if (star == NULL)
            {
                return mcsFAILURE;
            }
            
            // Get Star ID
            mcsSTRING32 starID;
            if (star->GetId(starID, sizeof(starID)) == mcsFAILURE)
            {
                return mcsFAILURE;
            }

            // if the star is not like the reference star (according to criteria list)
            if (star->IsSame(referenceStar, &criteriaList) != mcsTRUE)
            {
                // Remove it
                logTest("star %s has been removed by the filter '%s'", starID, GetId());
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
