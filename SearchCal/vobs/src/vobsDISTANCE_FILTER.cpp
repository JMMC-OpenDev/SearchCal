/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * @file
 * Definition of vobsDISTANCE_FILTER class.
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
 * SCALIB Headers 
 */
#include "alx.h"

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
 * @param raRef right ascension of the reference star
 * @param decRef declinaison of the reference star
 * @param distance distance to reference star
 *
 * @return always mcsSUCCESS
 */
mcsCOMPL_STAT vobsDISTANCE_FILTER::SetDistanceValue(const mcsSTRING32  raRef,
                                                    const mcsSTRING32  decRef,
                                                    const mcsDOUBLE     distance)
{
    logTrace("vobsDISTANCE_FILTER::SetDistanceValue()");

    // Copy reference star right ascension and declinaison
    strncpy(_raRef,  raRef,  sizeof(_raRef));
    strncpy(_decRef, decRef, sizeof(_decRef));

    // Copy distance to reference star
    _distance  = distance;
    
    return mcsSUCCESS;
}

/**
 * Get value of the filter 
 *
 * @param raRef right ascension of the reference star
 * @param decRef declinaison of the reference star
 * @param distance distance to reference star
 *
 * @return always mcsSUCCESS
 */
mcsCOMPL_STAT vobsDISTANCE_FILTER::GetDistanceValue(mcsSTRING32 *raRef,
                                                    mcsSTRING32 *decRef,
                                                    mcsDOUBLE    *distance)
{
    logTrace("vobsDISTANCE_FILTER::GetDistanceValue()");
    
    // Give back reference star right ascension and declinaison
    strncpy(*raRef,  _raRef,  sizeof(*raRef));
    strncpy(*decRef, _decRef, sizeof(*decRef));

    // Give back reference star distance
    *distance  = _distance;

    return mcsSUCCESS;
}

/**
 * Apply the filter on a list.
 *
 * Will go through each star of the given list, compute its separation from the
 * reference coordinates, and remove any star farther than the given distance.
 *
 * @param list star list on wich the filter is applied
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is 
 * returned.
 */
mcsCOMPL_STAT vobsDISTANCE_FILTER::Apply(vobsSTAR_LIST *list)
{
    logTrace("vobsDISTANCE_FILTER::Apply()");

    // Create a star correponding to the science object
    // (just to convert sexagesimal cooridantes in degrees !!!)
    vobsSTAR referenceStar;

    // Set right ascension property (ref) to this star
    if (referenceStar.SetPropertyValue(vobsSTAR_POS_EQ_RA_MAIN, _raRef, "") ==
        mcsFAILURE)
    {
        return mcsFAILURE;
    }

    // Set declinaison property (ref) to this star
    if (referenceStar.SetPropertyValue(vobsSTAR_POS_EQ_DEC_MAIN, _decRef, "") ==
        mcsFAILURE)
    {
        return mcsFAILURE;
    }

    // Get reference RA coordinate in degrees
    mcsDOUBLE referenceStarRA;
    referenceStar.GetRa(referenceStarRA);

    // Get reference DEC coordinate in degrees
    mcsDOUBLE referenceStarDEC;
    referenceStar.GetDec(referenceStarDEC);

    // For each star of the given star list
    vobsSTAR* currentStar;
    for (unsigned int el = 0; el < list->Size(); el++)
    {
        currentStar = list->GetNextStar((mcsLOGICAL)(el == 0));

        // Get current star ID
        // Just to log it !!!
        mcsSTRING32 starId;
        if (currentStar->GetId(starId, sizeof(starId)) == mcsFAILURE)
        {
            return mcsFAILURE;
        }

        // Get current star RA coordinate in degrees
        mcsDOUBLE currentStarRA;
        currentStar->GetRa(currentStarRA);

        // Get current star DEC coordinate in degrees
        mcsDOUBLE currentStarDEC;
        currentStar->GetDec(currentStarDEC);

        // (at last) Compute distance between refence star and the current star
        mcsDOUBLE distance;
        // Compute seperation in arcsec
        alxComputeDistance(referenceStarRA, referenceStarDEC, currentStarRA, currentStarDEC, &distance);
        // Convert separation in degrees
        distance = distance * alxARCSEC_IN_DEGREES;

        logDebug("Distance between star '%s' (RA=%f; DEC=%f) and reference star (RA=%f; DEC=%f) = %f .", starId, currentStarRA, currentStarDEC, referenceStarRA, referenceStarDEC, distance);

        // If the current star is farther than the reference distance to the reference star
        if (distance > _distance)
        {
            // Remove the current star from the given star list
            logTest("Star '%s' is farther than %f degrees of the reference star.", starId, _distance);
            list->Remove(*currentStar);
            el = el - 1;
        }
        else
        {
            logTest("Star '%s' is within %f degrees of the reference star.", starId, _distance);
        }
    }
    
    return mcsSUCCESS;    
}


/*___oOo___*/
