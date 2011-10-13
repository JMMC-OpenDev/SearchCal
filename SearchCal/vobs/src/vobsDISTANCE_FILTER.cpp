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
#include <string.h>
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

    // Get reference RA coordinate in degrees
    mcsDOUBLE referenceStarRA;
    if (vobsSTAR::GetRa(_raRef, referenceStarRA) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    // Get reference DEC coordinate in degrees
    mcsDOUBLE referenceStarDEC;
    if (vobsSTAR::GetDec(_decRef, referenceStarDEC) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    mcsSTRING64 starId;
    mcsDOUBLE currentStarRA;
    mcsDOUBLE currentStarDEC;
    mcsDOUBLE distance;
    
    // For each star of the given star list
    vobsSTAR* star = NULL;

    // For each star of the list
    // note: Remove() and GetNextStar() ensure proper list traversal:
    for (star = list->GetNextStar(mcsTRUE); star != NULL; star = list->GetNextStar(mcsFALSE))
    {
        // Get the star ID (logs)
        if (star->GetId(starId, sizeof(starId)) == mcsFAILURE)
        {
            return mcsFAILURE;
        }

        // Get current star RA coordinate in degrees
        if (star->GetRa(currentStarRA) == mcsFAILURE)
        {
            return mcsFAILURE;
        }

        // Get current star DEC coordinate in degrees
        if (star->GetDec(currentStarDEC) == mcsFAILURE)
        {
            return mcsFAILURE;
        }

        // (at last) Compute distance between refence star and the current star
        // Compute separation in arcsec
        alxComputeDistance(referenceStarRA, referenceStarDEC, currentStarRA, currentStarDEC, &distance);
        
        // Convert separation in degrees
        distance *= alxARCSEC_IN_DEGREES;

        logDebug("Distance between star '%s' (RA = %lf; DEC = %lf) and reference star (RA = %lf; DEC = %lf) = %lf .", 
                starId, currentStarRA, currentStarDEC, referenceStarRA, referenceStarDEC, distance);

        // If the current star is farther than the reference distance to the reference star
        if (distance > _distance)
        {
            // Remove the current star from the given star list
            logTest("Star '%s' is farther than %lf degrees of the reference star.", starId, _distance);
            
            list->Remove(*star);
        }
        else
        {
            logTest("Star '%s' is within %lf degrees of the reference star.", starId, _distance);
        }
    }
    
    return mcsSUCCESS;    
}


/*___oOo___*/
