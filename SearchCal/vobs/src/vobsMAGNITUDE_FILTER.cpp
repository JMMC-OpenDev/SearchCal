/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * @file
 * Definition of vobsMAGNITUDE_FILTER class.
 */


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
vobsMAGNITUDE_FILTER::vobsMAGNITUDE_FILTER(const char* filterId) : vobsFILTER(filterId)
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
mcsCOMPL_STAT vobsMAGNITUDE_FILTER::SetMagnitudeValue(const char* band,
                                                      mcsDOUBLE magValue,
                                                      mcsDOUBLE magRange)
{
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
mcsCOMPL_STAT vobsMAGNITUDE_FILTER::GetMagnitudeValue(char* band,
                                                      mcsDOUBLE *magValue,
                                                      mcsDOUBLE *magRange)
{
    strcpy(band, _band);
    *magValue = _magValue;
    *magRange = _magRange;

    return mcsSUCCESS;
}

/**
 * Apply magnitude filter on a list
 *
 * @param list the list on which the filter is applied
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
 */
mcsCOMPL_STAT vobsMAGNITUDE_FILTER::Apply(vobsSTAR_LIST *list)
{
    if (IS_TRUE(IsEnabled()))
    {
        // Create the UCD corresponding to the band
        mcsSTRING256 magnitudeUcd;
        strcpy(magnitudeUcd, "PHOT_JHN_");
        strcat(magnitudeUcd, _band);

        // Create a star correponding to the reference object
        vobsSTAR referenceStar;
        FAIL(referenceStar.SetPropertyValue(magnitudeUcd, _magValue, vobsNO_CATALOG_ID));

        // Add criteria on magnitude
        vobsSTAR_COMP_CRITERIA_LIST criteriaList;
        FAIL(criteriaList.Add(magnitudeUcd, _magRange));

        mcsINT32 nCriteria = 0;
        vobsSTAR_CRITERIA_INFO* criterias = NULL;

        // log criteria:
        criteriaList.log(logTEST);

        // Get criterias:
        FAIL(criteriaList.GetCriterias(criterias, nCriteria));

        // For each star of the given star list
        vobsSTAR* starPtr = NULL;

        // For each star of the list
        // note: Remove() and GetNextStar() ensure proper list traversal:
        for (starPtr = list->GetNextStar(mcsTRUE); IS_NOT_NULL(starPtr); starPtr = list->GetNextStar(mcsFALSE))
        {
            // Get Star ID
            mcsSTRING64 starId;
            FAIL(starPtr->GetId(starId, sizeof (starId)));

            // if the star is not like the reference star (according to criteria list)
            if (IS_FALSE(referenceStar.IsMatchingCriteria(starPtr, criterias, nCriteria)))
            {
                // Remove it
                logDebug("star '%s' has been removed by the filter '%s'", starId, GetId());

                list->RemoveRef(starPtr);
            }
            else
            {
                logDebug("star '%s' valid for the filter '%s'", starId, GetId());
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
