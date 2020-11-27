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
vobsDISTANCE_FILTER::vobsDISTANCE_FILTER(const char* filterId) : vobsFILTER(filterId)
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
 * @param decRef declination of the reference star
 * @param separation distance to reference star
 *
 * @return always mcsSUCCESS
 */
mcsCOMPL_STAT vobsDISTANCE_FILTER::SetDistanceValue(const mcsSTRING32 raRef,
                                                    const mcsSTRING32 decRef,
                                                    const mcsDOUBLE separation)
{
    // Copy reference star right ascension and declination
    strncpy(_raRef, raRef, sizeof (_raRef));
    strncpy(_decRef, decRef, sizeof (_decRef));

    // Copy distance to reference star
    _separation = separation;

    return mcsSUCCESS;
}

/**
 * Get value of the filter
 *
 * @param raRef right ascension of the reference star
 * @param decRef declination of the reference star
 * @param separation distance to reference star
 *
 * @return always mcsSUCCESS
 */
mcsCOMPL_STAT vobsDISTANCE_FILTER::GetDistanceValue(mcsSTRING32* raRef,
                                                    mcsSTRING32* decRef,
                                                    mcsDOUBLE* separation)
{
    // Give back reference star right ascension and declination
    strncpy(*raRef, _raRef, sizeof (*raRef));
    strncpy(*decRef, _decRef, sizeof (*decRef));

    // Give back reference star distance
    *separation = _separation;

    return mcsSUCCESS;
}

/**
 * Apply the filter on a list.
 *
 * Will go through each star of the given list, compute its separation from the
 * reference coordinates, and remove any star farther than the given distance.
 *
 * @param list star list on which the filter is applied
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
 */
mcsCOMPL_STAT vobsDISTANCE_FILTER::Apply(vobsSTAR_LIST* list)
{
    // Get reference RA coordinate in degrees
    mcsDOUBLE referenceStarRA;
    FAIL(vobsSTAR::GetRa(_raRef, referenceStarRA));

    // Get reference DEC coordinate in degrees
    mcsDOUBLE referenceStarDEC;
    FAIL(vobsSTAR::GetDec(_decRef, referenceStarDEC));

    mcsDOUBLE currentStarRA;
    mcsDOUBLE currentStarDEC;
    mcsDOUBLE separation;

    // For each star of the given star list
    vobsSTAR* starPtr = NULL;

    // For each star of the list
    // note: Remove() and GetNextStar() ensure proper list traversal:
    for (starPtr = list->GetNextStar(mcsTRUE); IS_NOT_NULL(starPtr); starPtr = list->GetNextStar(mcsFALSE))
    {
        // Get Star ID
        mcsSTRING64 starId;
        FAIL(starPtr->GetId(starId, sizeof (starId)));

        // Get current star RA/DEC coordinates in degrees
        FAIL(starPtr->GetRaDec(currentStarRA, currentStarDEC));

        // (at last) Compute distance between refence star and the current star
        // Compute separation in degrees
        alxComputeDistanceInDegrees(referenceStarRA, referenceStarDEC, currentStarRA, currentStarDEC, &separation);

        logDebug("Distance between star '%s' (RA = %lf; DEC = %lf) and reference star (RA = %lf; DEC = %lf) = %lf .",
                 starId, currentStarRA, currentStarDEC, referenceStarRA, referenceStarDEC, separation);

        // If the current star is farther than the reference distance to the reference star
        if (separation > _separation)
        {
            // Remove the current star from the given star list
            logDebug("star '%s' has been removed by the distance filter : too far from the reference star: %lf > %lf deg", starId, separation, _separation);

            list->RemoveRef(starPtr);
        }
        else
        {
            logDebug("star '%s' valid for the distance filter: close to the reference star: %lf < %lf deg", starId, separation, _separation);
        }
    }

    return mcsSUCCESS;
}


/*___oOo___*/
