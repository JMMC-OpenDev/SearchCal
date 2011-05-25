/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * @file
 * Definition of vobsMAGNITUDE_FILTER class.
 */

static char *rcsId __attribute__ ((unused)) ="@(#) $Id: vobsMAGNITUDE_FILTER.cpp,v 1.9 2011-03-03 13:09:42 lafrasse Exp $"; 

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
                                                        mcsDOUBLE magValue,
                                                        mcsDOUBLE magRange)
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
                                                        mcsDOUBLE *magValue,
                                                        mcsDOUBLE *magRange)
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
