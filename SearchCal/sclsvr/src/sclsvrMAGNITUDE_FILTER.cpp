/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: sclsvrMAGNITUDE_FILTER.cpp,v 1.2 2005-06-07 12:36:27 scetre Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.1  2005/06/01 14:18:54  scetre
 * Added filters and filter list objects.
 * Changed logExtDbg to logTrace
 *
 ******************************************************************************/

/**
 * @file
 *  Definition of sclsvrMAGNITUDE_FILTER class.
 */

static char *rcsId="@(#) $Id: sclsvrMAGNITUDE_FILTER.cpp,v 1.2 2005-06-07 12:36:27 scetre Exp $"; 
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
#include "sclsvrMAGNITUDE_FILTER.h"
#include "sclsvrPrivate.h"

/**
 * Class constructor
 */
sclsvrMAGNITUDE_FILTER::sclsvrMAGNITUDE_FILTER()
{
    strcpy(_name, sclsvrMAGNITUDE_FILTER_NAME);    
}

/**
 * Class destructor
 */
sclsvrMAGNITUDE_FILTER::~sclsvrMAGNITUDE_FILTER()
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
mcsCOMPL_STAT sclsvrMAGNITUDE_FILTER::SetMagnitudeValue(mcsSTRING32 band,
                                                        mcsFLOAT magValue,
                                                        mcsFLOAT magRange)
{
    logTrace("sclsvrMAGNITUDE_FILTER::SetMagnitudeValue()");

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
mcsCOMPL_STAT sclsvrMAGNITUDE_FILTER::GetMagnitudeValue(mcsSTRING32 *band,
                                                        mcsFLOAT *magValue,
                                                        mcsFLOAT *magRange)
{
    logTrace("sclsvrMAGNITUDE_FILTER::GetMagnitudeValue()");

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
mcsCOMPL_STAT sclsvrMAGNITUDE_FILTER::Apply(sclsvrCALIBRATOR_LIST *list)
{
    logTrace("sclsvrMAGNITUDE_FILTER::Apply()");

    if (IsEnable() == mcsTRUE)
    {
        // Create the UCD corresponding to the band
        mcsSTRING256 magnitudeUcd;
        strcpy(magnitudeUcd, "PHOT_JHN_");
        strcat(magnitudeUcd, _band);
        // create a star correponding to the science object
        sclsvrCALIBRATOR scienceStar;
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
        // for each calibrator of the list
        sclsvrCALIBRATOR *calibrator;

        for (unsigned int el = 0; el < list->Size(); el++)
        {
            // if the calibrator is different of the science star according to
            // the criteria list
            calibrator=
                (sclsvrCALIBRATOR *)list->GetNextStar((mcsLOGICAL)(el==0));
            if (calibrator->IsSame(scienceStar, &criteriaList) != mcsTRUE )
            {
                // Remove it
                logTest("calibrator %d not magnitude in the range\n", el+1);
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
