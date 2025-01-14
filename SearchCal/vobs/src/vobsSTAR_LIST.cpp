/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * @file
 * vobsSTAR_LIST class definition.
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
#include "misc.h"

/*
 * Local Headers
 */
#include "vobsSTAR_LIST.h"
#include "vobsCDATA.h"
#include "vobsVOTABLE.h"
#include "vobsPrivate.h"
#include "vobsErrors.h"

/* minimal threshold on score/distance difference ~ 0.5 */
#define MIN_SCORE_TH 0.5 /* to ensure this score difference among all proximity */
#define USE_BETTER true
#define BETTER_MIN_SCORE_TH_LO 0.01
#define BETTER_SCORE_RATIO_LO 2.0
#define BETTER_MIN_SCORE_TH_HI 0.1
#define BETTER_SCORE_RATIO_HI 1.25

/* enable/disable log matching star distance */
#define DO_LOG_STAR_MATCHING        false
#define DO_LOG_STAR_MATCHING_XM     false

/* enable/disable log star distance map */
#define DO_LOG_STAR_DIST_MAP_IDX    false
#define DO_LOG_STAR_DIST_MAP_XM     false

/* enable/disable log star index */
#define DO_LOG_STAR_INDEX           false

/* enable/disable log duplicates filter */
#define DO_LOG_DUP_FILTER           false

/* enable/disable log matching maps star distance (N case) */
#define DO_LOG_STAR_MATCHING_N      false

const char* vobsGetMatchType(vobsSTAR_MATCH_TYPE type)
{
    return vobsSTAR_MATCH_TYPE_CHAR[type];
}

mcsINT32 vobsGetMatchTypeAsFlag(vobsSTAR_MATCH_TYPE type)
{
    switch (type)
    {
        default:
            return 0; // ignore
        case vobsSTAR_MATCH_TYPE_GOOD:
            return 0; // ignore
        case vobsSTAR_MATCH_TYPE_BAD_DIST:
            return 1;
        case vobsSTAR_MATCH_TYPE_BAD_BEST:
            return 2;
        case vobsSTAR_MATCH_TYPE_GOOD_AMBIGUOUS_REF_SCORE:
            return 4; // ambiguous ref
        case vobsSTAR_MATCH_TYPE_GOOD_AMBIGUOUS_MATCH_SCORE:
            return 8; // ambiguous match
        case vobsSTAR_MATCH_TYPE_GOOD_AMBIGUOUS_REF_SCORE_BETTER:
            return 16; // ambiguous ref (better)
        case vobsSTAR_MATCH_TYPE_GOOD_AMBIGUOUS_MATCH_SCORE_BETTER:
            return 32; // ambiguous match (better)
    }
}

/**
 * Class constructor
 * @param name name of the star list
 */
vobsSTAR_LIST::vobsSTAR_LIST(const char* name)
{
    // Set name
    _name = name;

    // this list must by default free star pointers:
    SetFreeStarPointers(true);

    _starIterator = _starList.end();

    // star index is uninitialized:
    _starIndexInitialized = false;

    // define star indexes to NULL:
    _starIndex = NULL;
    _sameStarDistMap = NULL;

    // Clear catalog id / meta:
    SetCatalogMeta(vobsNO_CATALOG_ID, NULL);
}

/**
 * Destructor
 */
vobsSTAR_LIST::~vobsSTAR_LIST()
{
    Clear();

    // free star indexes:
    if (IS_NOT_NULL(_starIndex))
    {
        _starIndex->clear();
        delete(_starIndex);
    }
    if (IS_NOT_NULL(_sameStarDistMap))
    {
        _sameStarDistMap->clear();
        delete(_sameStarDistMap);
    }
}

/*
 * Public methods
 */

/**
 * Copy from a list
 * i.e. Add all elements present in the given list at the end of this list
 *
 * @param list the list to copy
 */
void vobsSTAR_LIST::Copy(const vobsSTAR_LIST& list)
{
    // Copy catalog id / meta:
    SetCatalogMeta(list.GetCatalogId(), list.GetCatalogMeta());

    const mcsUINT32 nbStars = list.Size();
    for (mcsUINT32 el = 0; el < nbStars; el++)
    {
        AddAtTail(*(list.GetNextStar((mcsLOGICAL) (el == 0))));
    }
}

/**
 * Erase (i.e de-allocate) all elements from the list.
 */
void vobsSTAR_LIST::Clear(void)
{
    // this list must now (default) free star pointers:
    ClearRefs(true);
}

void vobsSTAR_LIST::ClearRefs(const bool freeStarPtrs)
{
    if (IsFreeStarPointers())
    {
        // Deallocate all objects of the list
        for (vobsSTAR_PTR_LIST::iterator iter = _starList.begin(); iter != _starList.end(); iter++)
        {
            delete(*iter);
        }
    }

    // Clear list anyway
    _starList.clear();

    // this list must now (default) free star pointers:
    SetFreeStarPointers(freeStarPtrs);

    // Clear catalog id / meta:
    SetCatalogMeta(vobsNO_CATALOG_ID, NULL);
}

/**
 * Add the given element at the end of the list.
 *
 * @param star element to be added to the list.
 *
 * @return Always mcsSUCCESS.
 */
void vobsSTAR_LIST::AddAtTail(const vobsSTAR &star)
{
    // Put the element in the list
    vobsSTAR* newStar = new vobsSTAR(star);
    _starList.push_back(newStar);
}

/**
 * Remove the given element from the list
 *
 * This method looks for the specified @em star in the list. If found, it
 * remove it. Otherwise, nothing is done.
 *
 * The method vobsSTAR::IsSame() is used to compare the list elements with
 * the specified one.
 *
 * @warning if the list contains more than one instance of the given element,
 * only first occurence isremoved.
 *
 * @note This method does not conflict with GetNextStar(); i.e. it can be used
 * to remove the star returned by GetNextStar() method, as shown below:
 * @code
 * for (mcsUINT32 el = 0; el < starList.Size(); el++)
 * {
 *     vobsSTAR* star;
 *     star = starList.GetNextStar((mcsLOGICAL)(el==0));
 *     if ( <condition> )
 *     {
 *          // Remove star from list
 *          starList.Remove(*star);
 *
 *          // and decrease 'el' to take into account the new list size
 *          el--;
 *     }
 * }
 * @endcode

 * @param star to be removed from the list.
 *
 * @return Always mcsSUCCESS.
 */
mcsCOMPL_STAT vobsSTAR_LIST::Remove(vobsSTAR &star)
{
    // Search star in the list
    for (vobsSTAR_PTR_LIST::iterator iter = _starList.begin(); iter != _starList.end(); iter++)
    {
        // If found
        if (IS_TRUE((*iter)->IsSame(&star)))
        {
            if (IsFreeStarPointers())
            {
                // Delete star
                delete(*iter);
            }

            // If star to be deleted correspond to the one currently pointed
            // by GetNextStar method
            if (*_starIterator == *iter)
            {
                // If it is not the first star of the list
                if (iter != _starList.begin())
                {
                    // Then go back to the previous star
                    _starIterator--;
                }
                else
                {
                    // Else set current pointer to end() in order to restart scan
                    // from beginning of the list.
                    _starIterator = _starList.end();
                }
            }

            // Clear star from list
            _starList.erase(iter);

            break;
        }
    }

    return mcsSUCCESS;
}

/**
 * Remove the given star pointer from the list using pointer equality
 *
 * @note This method does not conflict with GetNextStar(); i.e. it can be used
 * to remove the star returned by GetNextStar() method, as shown below:
 * @code
 * for (mcsUINT32 el = 0; el < starList.Size(); el++)
 * {
 *     vobsSTAR* star;
 *     star = starList.GetNextStar((mcsLOGICAL)(el==0));
 *     if ( <condition> )
 *     {
 *          // Remove star from list
 *          starList.Remove(star);
 *
 *          // and decrease 'el' to take into account the new list size
 *          el--;
 *     }
 * }
 * @endcode

 * @param starPtr to be removed from the list.
 */
void vobsSTAR_LIST::RemoveRef(vobsSTAR* starPtr)
{
    // Search star pointer in the list
    for (vobsSTAR_PTR_LIST::iterator iter = _starList.begin(); iter != _starList.end(); iter++)
    {
        // compare star pointers:
        if (*iter == starPtr)
        {
            if (IsFreeStarPointers())
            {
                // Delete star
                delete(*iter);
            }

            // If star to be deleted correspond to the one currently pointed
            // by GetNextStar method
            if (*_starIterator == *iter)
            {
                // If it is not the first star of the list
                if (iter != _starList.begin())
                {
                    // Then go back to the previous star
                    _starIterator--;
                }
                else
                {
                    // Else set current pointer to end() in order to restart scan
                    // from beginning of the list.
                    _starIterator = _starList.end();
                }
            }

            // Clear star from list
            _starList.erase(iter);

            break;
        }
    }
}

/**
 * Return the star of the list corresponding to the given star.
 *
 * This method looks for the specified @em star in the list. If found, it
 * returns the pointer to this element. Otherwise, NULL is returned.
 *
 * The method vobsSTAR::IsSame() is used to compare element of the list with
 * the specified one.
 *
 * This method can be used to discover whether a star is in list or not, as
 * shown below:
 * @code
 * if (IS_NULL(starList.GetStar(star)->View()))
 * {
 *     printf ("Star not found in list !!");
 * }
 * @endcode
 *
 * @param star star to compare with
 * @return pointer to the found element of the list or NULL if element is not
 * found in list.
 */
vobsSTAR* vobsSTAR_LIST::GetStar(vobsSTAR* star)
{
    if (_starIndexInitialized)
    {
        // Use star index

        mcsDOUBLE starDec;
        NULL_DO(star->GetDec(starDec),
                logWarning("Invalid Dec coordinate for the given star !"));

        // note: add 1/100 arcsecond for floating point precision:
        vobsSTAR_PTR_DBL_MAP::iterator lower = _starIndex->lower_bound(starDec - COORDS_PRECISION);
        vobsSTAR_PTR_DBL_MAP::iterator upper = _starIndex->upper_bound(starDec + COORDS_PRECISION);

        // Search star in the star index boundaries:
        for (vobsSTAR_PTR_DBL_MAP::iterator iter = lower; iter != upper; iter++)
        {
            if (IS_TRUE(star->IsSame(iter->second)))
            {
                return iter->second;
            }
        }

        // If nothing found, return NULL pointer
        return NULL;
    }

    // Search star in the list
    for (vobsSTAR_PTR_LIST::iterator iter = _starList.begin(); iter != _starList.end(); iter++)
    {
        if (IS_TRUE(star->IsSame(*iter)))
        {
            return *iter;
        }
    }

    // If nothing found, return NULL pointer
    return NULL;
}

mcsCOMPL_STAT vobsSTAR_LIST::logNoMatch(const vobsSTAR* starRefPtr)
{
    if (doLog(logTEST))
    {
        mcsSTRING2048 dump;

        mcsDOUBLE raRef, decRef;
        FAIL(starRefPtr->GetRaDec(raRef, decRef));

        // Get ref star dump:
        starRefPtr->Dump(dump);
        logTest("- Reference star : %s", dump);

        logTest("- Candidates (%d stars) :", Size());
        mcsUINT32 i = 0;
        mcsDOUBLE ra, dec, dist;
        // star pointer on this star list:
        vobsSTAR* starListPtr;

        for (vobsSTAR_PTR_LIST::iterator iter = _starList.begin(); iter != _starList.end(); iter++)
        {
            starListPtr = *iter;
            dist = NAN;

            // Ensure star has coordinates:
            if (IS_TRUE(starListPtr->isRaDecSet()))
            {
                FAIL(starListPtr->GetRaDec(ra, dec));
                FAIL(alxComputeDistance(raRef, decRef, ra, dec, &dist));
            }

            // Get star dump:
            starListPtr->Dump(dump);
            logTest("  Star %4d [sep=%.5lf as]: %s", (++i), dist, dump);
        }
    }
    return mcsSUCCESS;
}

/**
 * Return the star of the list corresponding to the given star ie matching criteria.
 *
 * This method looks for the specified @em star in the list. If found, it
 * returns the pointer to this element. Otherwise, NULL is returned.
 *
 * The method vobsSTAR::IsSame() is used to compare element of the list with
 * the specified one.
 *
 * This method can be used to discover whether a star is in list or not, as
 * shown below:
 * @code
 * mcsINT32 nCriteria = 0;
 * vobsSTAR_CRITERIA_INFO* criterias = NULL;
 *
 * // Initialize criteria informations:
 * if (criteriaList.InitializeCriterias() == mcsFAILURE)
 * {
 *     return mcsFAILURE;
 * }
 *
 * // Get criterias:
 * if (criteriaList.GetCriterias(criterias, nCriteria) == mcsFAILURE)
 * {
 *     return mcsFAILURE;
 * }
 *
 * ...
 * if (IS_NULL(starList.MatchingCriteria(star, criterias, nCriteria)->View()))
 * {
 *     printf ("Star not found in list !!");
 * }
 * @endcode
 *
 * @param star star to compare with
 * @param criterias vobsSTAR_CRITERIA_INFO[] list of comparison criterias
 *                  given by vobsSTAR_COMP_CRITERIA_LIST.GetCriterias()
 * @param nCriteria number of criteria i.e. size of the vobsSTAR_CRITERIA_INFO array
 * @param matcher crossmatch algorithm in action
 * @param mInfo matcher information
 *
 * @return pointer to the found element of the list or NULL if element is not
 * found in list.
 */
vobsSTAR* vobsSTAR_LIST::GetStarMatchingCriteria(vobsSTAR* star,
                                                 vobsSTAR_CRITERIA_INFO* criterias, mcsUINT32 nCriteria,
                                                 vobsSTAR_MATCH matcher,
                                                 vobsSTAR_LIST_MATCH_INFO* mInfo,
                                                 mcsUINT32* noMatchs)
{
    // Assert criteria are defined:
    if (nCriteria == 0)
    {
        logWarning("GetStarMatchingCriteria: criteria are undefined !");
        return NULL;
    }

    bool useIndex = false;

    if (_starIndexInitialized && (matcher == vobsSTAR_MATCH_INDEX))
    {
        // check criteria
        // note: RA_DEC criteria is always the first one
        useIndex = ((&criterias[0])->propCompType == vobsPROPERTY_COMP_RA_DEC);
    }

    if (!useIndex)
    {
        logWarning("GetStarsMatchingTargetId: unsupported matcher !");
        return NULL;
    }
    // Use star index
    // note: RA_DEC criteria is always the first one
    mcsDOUBLE rangeDEC = (&criterias[0])->rangeDEC;
    mcsDOUBLE starDec = EMPTY_COORD_DEG;

    NULL_DO(star->GetDec(starDec),
            logWarning("Invalid Dec coordinate for the given star !"));

    // note: add +/- COORDS_PRECISION for floating point precision:
    vobsSTAR_PTR_DBL_MAP::iterator lower = _starIndex->lower_bound(starDec - rangeDEC - COORDS_PRECISION);
    vobsSTAR_PTR_DBL_MAP::iterator upper = _starIndex->upper_bound(starDec + rangeDEC + COORDS_PRECISION);

    // As several stars can be present in the [lower; upper] range,
    // an ordered distance map is used to select the closest star matching criteria:
    if (IS_NULL(_sameStarDistMap))
    {
        // create the distance map allocated until destructor is called:
        _sameStarDistMap = new vobsSTAR_PTR_MATCH_MAP();
    }
    else
    {
        _sameStarDistMap->clear();
    }

    // Search star in the star index boundaries:
    for (vobsSTAR_PTR_DBL_MAP::iterator iter = lower; iter != upper; iter++)
    {
        // reset distance:
        mcsDOUBLE distAng = NAN;

        if (IS_TRUE(star->IsMatchingCriteria(iter->second, criterias, nCriteria, &distAng, NULL, noMatchs)))
        {
            // add candidate in distance map:
            vobsSTAR_PTR_MATCH_ENTRY entry = vobsSTAR_PTR_MATCH_ENTRY(distAng, iter->second);
            _sameStarDistMap->insert(vobsSTAR_PTR_MATCH_PAIR(entry.score, entry));
        }
    }

    // get the number of stars matching criteria:
    const mcsINT32 mapSize = _sameStarDistMap->size();

    if (mapSize > 0)
    {
        // distance map is not empty
        const bool doLog = ((mapSize > 1) || DO_LOG_STAR_DIST_MAP_IDX);

        logStarMap("GetStarMatchingCriteria(useIndex)", _sameStarDistMap, doLog);

        // Use the first star (sorted by distance):
        vobsSTAR_PTR_MATCH_MAP::const_iterator iter = _sameStarDistMap->begin();
        const vobsSTAR_PTR_MATCH_ENTRY entry = iter->second;

        vobsSTAR* starListPtr = entry.starPtr;

        if (IS_NOT_NULL(mInfo))
        {
            mInfo->distAng = entry.distAng;
        }
        _sameStarDistMap->clear();

        return starListPtr;
    }
    // If nothing found, return NULL pointer
    return NULL;
}

void DumpXmatchPairMap(vobsSTAR_PTR_MATCH_MAP_MAP* xmMap, const char* labelPtr, const char* labelMap)
{
    logInfo("=====");
    mcsSTRING2048 dump;

    for (vobsSTAR_PTR_MATCH_MAP_MAP::const_iterator iter = xmMap->begin(); iter != xmMap->end(); iter++)
    {
        vobsSTAR* starPtr = iter->first;
        starPtr->Dump(dump);
        logInfo("- Score map for %s: %s", labelPtr, dump);

        vobsSTAR_PTR_MATCH_MAP* distMap = iter->second;
        if (IS_NOT_NULL(distMap))
        {
            vobsSTAR_LIST::logStarMap(labelMap, distMap, true);
        }
    }
    logInfo("=====");
}

void ClearXmatchPairMap(vobsSTAR_PTR_MATCH_MAP_MAP* xmMap)
{
    for (vobsSTAR_PTR_MATCH_MAP_MAP::const_iterator iter = xmMap->begin(); iter != xmMap->end(); iter++)
    {
        // Free internal maps:
        vobsSTAR_PTR_MATCH_MAP* distMap = iter->second;
        if (IS_NOT_NULL(distMap))
        {
            distMap->clear(); // to be sure
            delete distMap;
        }
    }
    xmMap->clear();
}

void vobsSTAR_LIST::DumpXmatchMapping(vobsSTAR_XM_PAIR_MAP* mapping)
{
    mcsSTRING2048 dumpRef;
    mcsSTRING2048 dumpStar;

    for (vobsSTAR_XM_PAIR_MAP::const_iterator iter = mapping->begin(); iter != mapping->end(); iter++)
    {
        vobsSTAR* starRefPtr = iter->first;

        if (IS_NOT_NULL(starRefPtr))
        {
            starRefPtr->Dump(dumpRef);

            const vobsSTAR_LIST_MATCH_INFO* mInfo = iter->second;
            if (IS_NOT_NULL(mInfo))
            {
                mcsDOUBLE distAng = mInfo->distAng;
                vobsSTAR* starPtr = mInfo->starPtr;

                dumpStar[0] = '\0';

                if (IS_NOT_NULL(starPtr))
                {
                    // Get star dump:
                    starPtr->Dump(dumpStar);
                }

                logInfo("Mapping Ref Star: %s <== With List Star [sep=%.5lf as] ==> %s", dumpRef, distAng, dumpStar);
            }
        }
    }
}

void vobsSTAR_LIST::ClearXmatchMapping(vobsSTAR_XM_PAIR_MAP* mapping)
{
    for (vobsSTAR_XM_PAIR_MAP::iterator iter = mapping->begin(); iter != mapping->end(); iter++)
    {
        // Free internal structs:
        vobsSTAR_LIST_MATCH_INFO* mInfo = iter->second;

        if (IS_NOT_NULL(mInfo) && (mInfo->shared == 0))
        {
            mInfo->Clear(); // to be sure
            delete mInfo;
        }
    }
    mapping->clear();
}

mcsCOMPL_STAT vobsSTAR_LIST::GetStarsMatchingCriteriaUsingDistMap(vobsSTAR_XM_PAIR_MAP* mapping,
                                                                  vobsORIGIN_INDEX originIdx, const vobsSTAR_MATCH_MODE matchMode,
                                                                  vobsSTAR_PRECESS_MODE precessMode, const mcsDOUBLE listEpoch,
                                                                  vobsSTAR_LIST* starRefList,
                                                                  vobsSTAR_CRITERIA_INFO* criterias, mcsUINT32 nCriteria,
                                                                  vobsSTAR_MATCH matcher,
                                                                  mcsDOUBLE thresholdScore,
                                                                  mcsUINT32* noMatchs)
{
    // Assert criteria are defined:
    if (nCriteria == 0)
    {
        logWarning("GetStarsMatchingCriteriaUsingDistMap: criteria are undefined !");
        return mcsFAILURE;
    }
    if (matcher != vobsSTAR_MATCH_DISTANCE_MAP)
    {
        logWarning("GetStarsMatchingCriteriaUsingDistMap: bad matcher !");
        return mcsFAILURE;
    }

    const bool isLogDebug = doLog(logDEBUG);

    // adjust criterias to use min radius (mates):
    mcsDOUBLE xmRadius = -1.0;
    if (((&criterias[0])->propCompType == vobsPROPERTY_COMP_RA_DEC) && (&criterias[0])->isRadius)
    {
        // only fix rangeRA (used by distance check for radius mode):
        xmRadius = (&criterias[0])->rangeRA;
        (&criterias[0])->rangeRA = vobsSTAR_CRITERIA_RADIUS_MATES * alxARCSEC_IN_DEGREES;
    }
    else
    {
        logWarning("GetStarsMatchingCriteriaUsingDistMap: invalid criteria on radius !");
        return mcsFAILURE;
    }

    // As several stars can be present in the [lower; upper] range,
    // an ordered distance map is used to select the closest star matching criteria:

    // create the star pointer (ref) to distance map:
    vobsSTAR_PTR_MATCH_MAP_MAP starRefDistMaps;

    // 1 - Collect all pairs (ref - star) and precess coordinates if needed
    mcsSTRING2048 dump, dump2;

    // Loop on all reference stars:
    for (vobsSTAR_PTR_LIST::const_iterator iterRef = starRefList->_starList.begin(); iterRef != starRefList->_starList.end(); iterRef++)
    {
        vobsSTAR* starRefPtr = *iterRef;

        // star original RA/DEC (degrees):
        mcsDOUBLE raOrig1, decOrig1;
        mcsDOUBLE pmRa1, pmDec1; // mas/yr

        if (precessMode != vobsSTAR_PRECESS_NONE)
        {
            FAIL(starRefPtr->GetPmRaDec(pmRa1, pmDec1));

            if (precessMode == vobsSTAR_PRECESS_BOTH)
            {
                // copy original RA/DEC:
                FAIL_DO(starRefPtr->GetRaDec(raOrig1, decOrig1),
                        starRefPtr->Dump(dump); logWarning("Failed to get Ra/Dec ! star : %s", dump));

                // correct coordinates:
                starRefPtr->CorrectRaDecEpochs(raOrig1, decOrig1, pmRa1, pmDec1, EPOCH_2000, listEpoch);
            }
        }

        // create the distance map allocated until destructor is called:
        vobsSTAR_PTR_MATCH_MAP* starRefDistMap = new vobsSTAR_PTR_MATCH_MAP();

        starRefDistMaps.insert(vobsSTAR_PTR_MATCH_MAP_PAIR(starRefPtr, starRefDistMap));


        // Loop on the list stars:
        for (vobsSTAR_PTR_LIST::const_iterator iterList = _starList.begin(); iterList != _starList.end(); iterList++)
        {
            vobsSTAR* starListPtr = *iterList;

            // correct coordinates using the reference star:
            mcsDOUBLE raOrig2, decOrig2;

            if (precessMode != vobsSTAR_PRECESS_NONE)
            {
                // copy original RA/DEC:
                FAIL_DO(starListPtr->GetRaDec(raOrig2, decOrig2),
                        starListPtr->Dump(dump); logWarning("Failed to get Ra/Dec ! star : %s", dump));

                if (precessMode == vobsSTAR_PRECESS_BOTH)
                {
                    mcsDOUBLE pmRa2, pmDec2;
                    FAIL(starListPtr->GetPmRaDec(pmRa2, pmDec2));

                    // correct coordinates:
                    starListPtr->CorrectRaDecEpochs(raOrig2, decOrig2, pmRa2, pmDec2, EPOCH_2000, listEpoch);
                }
                else
                {
                    mcsDOUBLE jdDate = starListPtr->GetJdDate();
                    mcsDOUBLE epoch = (jdDate != -1.0) ? (EPOCH_2000 + (jdDate - JD_2000) / 365.25) : listEpoch;

                    starListPtr->CorrectRaDecEpochs(raOrig2, decOrig2, pmRa1, pmDec1, epoch, EPOCH_2000);
                }
            }

            mcsDOUBLE distAng = NAN;
            mcsDOUBLE distMag = NAN;

            if (IS_TRUE(starRefPtr->IsMatchingCriteria(starListPtr, criterias, nCriteria, &distAng, &distMag, noMatchs)))
            {
                // add candidate in distance map:
                mcsDOUBLE ra1, dec1, ra2, dec2;
                FAIL_DO(starRefPtr->GetRaDec(ra1, dec1),
                        starRefPtr->Dump(dump); logWarning("Failed to get Ra/Dec ! star : %s", dump));
                FAIL_DO(starListPtr->GetRaDec(ra2, dec2),
                        starListPtr->Dump(dump); logWarning("Failed to get Ra/Dec ! star : %s", dump));

                vobsSTAR_PTR_MATCH_ENTRY entryRef = vobsSTAR_PTR_MATCH_ENTRY(distAng, distMag, starListPtr, ra1, dec1, ra2, dec2);
                starRefDistMap->insert(vobsSTAR_PTR_MATCH_PAIR(entryRef.score, entryRef));
            }

            if (precessMode != vobsSTAR_PRECESS_NONE)
            {
                // restore original RA/DEC:
                starListPtr->SetRaDec(raOrig2, decOrig2);
            }
        } // loop on list stars

        if (precessMode == vobsSTAR_PRECESS_BOTH)
        {
            // restore original RA/DEC:
            starRefPtr->SetRaDec(raOrig1, decOrig1);
        }
    } // loop on reference stars

    // anyway: restore criterias to use correct radius (xmatch):
    (&criterias[0])->rangeRA = xmRadius;

    // 2 - Collect all reverse pairs (star - ref)

    // create the star pointer (list) to distance map:
    vobsSTAR_PTR_MATCH_MAP_MAP starListDistMaps;

    // Loop on the list stars:
    for (vobsSTAR_PTR_LIST::const_iterator iterList = _starList.begin(); iterList != _starList.end(); iterList++)
    {
        vobsSTAR* starListPtr = *iterList;

        // create the distance map allocated until destructor is called:
        vobsSTAR_PTR_MATCH_MAP* starListDistMap = new vobsSTAR_PTR_MATCH_MAP(); // TO BE FREED later

        starListDistMaps.insert(vobsSTAR_PTR_MATCH_MAP_PAIR(starListPtr, starListDistMap));

        // Loop on ref pairs:
        for (vobsSTAR_PTR_MATCH_MAP_MAP::const_iterator iterRefMaps = starRefDistMaps.begin(); iterRefMaps != starRefDistMaps.end(); iterRefMaps++)
        {
            vobsSTAR* starRefPtr = iterRefMaps->first;
            vobsSTAR_PTR_MATCH_MAP* starRefDistMap = iterRefMaps->second;

            for (vobsSTAR_PTR_MATCH_MAP::const_iterator iterDistRef = starRefDistMap->begin(); iterDistRef != starRefDistMap->end(); iterDistRef++)
            {
                vobsSTAR_PTR_MATCH_ENTRY entryRef = iterDistRef->second;

                // Check if this pair corresponds to starListPtr:
                if (starListPtr == entryRef.starPtr)
                {
                    // add candidate in distance map:
                    vobsSTAR_PTR_MATCH_ENTRY entryList = vobsSTAR_PTR_MATCH_ENTRY(entryRef, starRefPtr);
                    starListDistMap->insert(vobsSTAR_PTR_MATCH_PAIR(entryList.score, entryList));
                }
            }
        }
    }

    // Process 
    // Distance maps contains all pairs within min radius (3 arcsecs)
    // Find all best pairs (ie ref stars) that satisfy criteria and avoid ambiguity

    // Loop on ref pairs:
    for (vobsSTAR_PTR_MATCH_MAP_MAP::const_iterator iterRefMaps = starRefDistMaps.begin(); iterRefMaps != starRefDistMaps.end(); iterRefMaps++)
    {
        vobsSTAR* starRefPtr = iterRefMaps->first;

        if (isLogDebug)
        {
            starRefPtr->Dump(dump);
            logDebug("GetStarsMatchingCriteriaUsingDistMap: Ref Star : %s", dump);
        }

        vobsSTAR_PTR_MATCH_MAP* starRefDistMap = iterRefMaps->second;

        // get the number of stars matching criteria:
        const mcsINT32 mapSize = starRefDistMap->size();

        if (mapSize > 0)
        {
            vobsSTAR_MATCH_TYPE type = vobsSTAR_MATCH_TYPE_NONE;
            bool doLog = false;

            // check ambiguity between 1st and 2nd matches:
            mcsDOUBLE distAngMatch12 = NAN;
            mcsDOUBLE distAngRef12 = NAN;

            // Use the first star (sorted by score):
            vobsSTAR_PTR_MATCH_MAP::const_iterator iterDistRef = starRefDistMap->begin();
            vobsSTAR_PTR_MATCH_ENTRY entryRef = iterDistRef->second;
            mcsDOUBLE distAngRef = entryRef.distAng;

            // ALWAYS check again distance criteria:
            if (distAngRef > (xmRadius * alxDEG_IN_ARCSEC))
            {
                type = vobsSTAR_MATCH_TYPE_BAD_DIST;

                starRefPtr->Dump(dump);
                logTest("GetStarsMatchingCriteriaUsingDistMap: Bad: Distance high: [sep=%.5lf as] for Ref Star: %s",
                        distAngRef, dump);

                // invalid match
                doLog = true;
            }
            else
            {
                // valid distance:
                type = vobsSTAR_MATCH_TYPE_GOOD;

                vobsSTAR* starListPtr = entryRef.starPtr;

                if (matchMode == vobsSTAR_MATCH_BEST)
                {
                    bool doLog2 = false;

                    // check if this star (list) is not closer to another reference star:
                    vobsSTAR_PTR_MATCH_MAP_MAP::const_iterator iterList = starListDistMaps.find(starListPtr);
                    vobsSTAR_PTR_MATCH_MAP* starListDistMap = iterList->second;

                    // Use the first star (sorted by score):
                    vobsSTAR_PTR_MATCH_MAP::const_iterator iterDistList = starListDistMap->begin();
                    vobsSTAR_PTR_MATCH_ENTRY entryList = iterDistList->second;
                    vobsSTAR* starRefPtrBest = entryList.starPtr;

                    if (IS_NOT_NULL(starRefPtrBest))
                    {
                        if (starRefPtrBest != starRefPtr)
                        {
                            type = vobsSTAR_MATCH_TYPE_BAD_BEST;

                            // different best match (symetric):
                            starRefPtr->Dump(dump);
                            starRefPtrBest->Dump(dump2);
                            logTest("GetStarsMatchingCriteriaUsingDistMap: Bad: Best match discarded: [sep=%.9lf as] < [sep=%.9lf as] for Ref Stars: %s > %s",
                                    distAngRef, entryList.distAng, dump, dump2);

                            // Use other matches ? not for now: ambiguity => DISCARD

                            // determine distance between ref stars:
                            if (starListDistMap->size() > 1)
                            {
                                iterDistList++;
                                // find entry corresponding to starRefPtr:
                                for (; iterDistList != starListDistMap->end(); iterDistList++)
                                {
                                    vobsSTAR_PTR_MATCH_ENTRY entryList2 = iterDistList->second;
                                    if (entryList2.starPtr == starRefPtr)
                                    {
                                        // compute real distance between matches (list 2 = ref (switched)) with epoch correction:
                                        FAIL(alxComputeDistance(entryList.ra2, entryList.de2, entryList2.ra2, entryList2.de2, &distAngRef12));
                                        break;
                                    }
                                }
                            }
                            // invalid match
                            doLog2 = true;
                        }
                        else
                        {
                            // check ambiguity between 1st and 2nd matches in starListDistMap (symetry):
                            if ((starListDistMap->size() > 1) && !isCatalogWds(originIdx) && !isCatalogSB9(originIdx))
                            {
                                iterDistList++;
                                vobsSTAR_PTR_MATCH_ENTRY entryList2 = iterDistList->second;

                                // check delta score ?
                                mcsDOUBLE deltaScore = fabs(entryList2.score - entryList.score);

                                // check absolute scores:
                                const bool isBetter = USE_BETTER && (
                                        (deltaScore >= BETTER_MIN_SCORE_TH_HI) ? (entryList2.score >= BETTER_SCORE_RATIO_LO * entryList.score) :
                                        ((deltaScore >= BETTER_MIN_SCORE_TH_LO) && (entryList2.score >= BETTER_SCORE_RATIO_LO * entryList.score))
                                        );

                                if (USE_BETTER && !isBetter)
                                {
                                    logTest("GetStarsMatchingCriteriaUsingDistMap: better ref: %s (%.5lf > %.5lf) ratio = %.2lf",
                                            isBetter ? "true" : "false", entryList2.score, entryList.score,
                                            entryList2.score / mcsMAX(1e-3, entryList.score)); // avoid div by 0
                                }

                                // check against threshold according to catalog:
                                if ((type == vobsSTAR_MATCH_TYPE_GOOD) && (deltaScore < thresholdScore))
                                {
                                    type = (USE_BETTER && !isBetter) ? vobsSTAR_MATCH_TYPE_GOOD_AMBIGUOUS_REF_SCORE
                                            : vobsSTAR_MATCH_TYPE_GOOD_AMBIGUOUS_REF_SCORE_BETTER;

                                    starRefPtr->Dump(dump);
                                    logTest("GetStarsMatchingCriteriaUsingDistMap: Bad: Ambiguous ref (1st-2nd) [d(score)=%.3lf d(sep)=%.3lf] for Ref Star: %s",
                                            deltaScore, fabs(entryList2.distAng - entryList.distAng), dump);

                                    // invalid match
                                    doLog2 = true;
                                }

                                // compute real distance between 1st / 2nd (list 2 = ref (switched)) with epoch correction:
                                FAIL(alxComputeDistance(entryList.ra2, entryList.de2, entryList2.ra2, entryList2.de2, &distAngRef12));
                            }
                        }

                        if (doLog2)
                        {
                            logStarMap("GetStarMatchingCriteriaUsingDistMap(2)", starListDistMap, true);
                        }
                    }
                    // Consider next matches ???
                } // check best match

                // check ambiguity between 1st and 2nd matches in starRefDistMap (symetry):
                if ((mapSize > 1) && !isCatalogWds(originIdx) && !isCatalogSB9(originIdx))
                {
                    iterDistRef++;
                    vobsSTAR_PTR_MATCH_ENTRY entryRef2 = iterDistRef->second;

                    // check delta score ?
                    mcsDOUBLE deltaScore = fabs(entryRef2.score - entryRef.score);

                    // check absolute scores:
                    const bool isBetter = USE_BETTER && (
                            (deltaScore >= BETTER_MIN_SCORE_TH_HI) ? (entryRef2.score >= BETTER_SCORE_RATIO_LO * entryRef.score) :
                            ((deltaScore >= BETTER_MIN_SCORE_TH_LO) && (entryRef2.score >= BETTER_SCORE_RATIO_LO * entryRef.score))
                            );

                    if (USE_BETTER && !isBetter)
                    {
                        logTest("GetStarsMatchingCriteriaUsingDistMap: better match: %s (%.5lf > %.5lf) ratio = %.2lf",
                                isBetter ? "true" : "false", entryRef2.score, entryRef.score,
                                entryRef2.score / mcsMAX(1e-3, entryRef.score)); // avoid div by 0
                    }

                    // check against threshold according to catalog:
                    if ((type == vobsSTAR_MATCH_TYPE_GOOD) && (deltaScore < thresholdScore))
                    {
                        type = (USE_BETTER && !isBetter) ? vobsSTAR_MATCH_TYPE_GOOD_AMBIGUOUS_MATCH_SCORE
                                : vobsSTAR_MATCH_TYPE_GOOD_AMBIGUOUS_MATCH_SCORE_BETTER;

                        starRefPtr->Dump(dump);
                        logTest("GetStarsMatchingCriteriaUsingDistMap: Bad: Ambiguous match (1st-2nd) [d(score)=%.3lf d(sep)=%.3lf] for Ref Star: %s",
                                deltaScore, fabs(entryRef2.distAng - entryRef.distAng), dump);

                        // invalid match
                        doLog = true;
                    }

                    // compute real distance between 1st / 2nd (list 2) with epoch correction:
                    FAIL(alxComputeDistance(entryRef.ra2, entryRef.de2, entryRef2.ra2, entryRef2.de2, &distAngMatch12));
                }
            }

            // store match result in mapping:
            vobsSTAR_LIST_MATCH_INFO* mInfo = new vobsSTAR_LIST_MATCH_INFO(); // to be freed
            mInfo->Set(type, entryRef);

            char* xmLog = &(mInfo->xm_log[0]);
            logStarMap("GetStarsMatchingCriteriaUsingDistMap", starRefDistMap, doLog || DO_LOG_STAR_DIST_MAP_XM, xmLog);

            if (!isCatalogWds(originIdx) && !isCatalogSB9(originIdx))
            {
                mInfo->nMates = mapSize;
                // separation difference between 1 and 2:
                mInfo->distAng12 = isnan(distAngRef12) ? distAngMatch12 : (isnan(distAngMatch12) ? distAngRef12 : mcsMIN(distAngRef12, distAngMatch12));
            }
            mapping->insert(vobsSTAR_XM_PAIR(starRefPtr, mInfo));
        }
    } // loop on reference stars

    if (DO_LOG_STAR_MATCHING_N)
    {
        DumpXmatchPairMap(&starRefDistMaps, "Ref Star", "List Star");
        DumpXmatchPairMap(&starListDistMaps, "List Star", "Ref Star");
    }

    // Finally: clear map:
    ClearXmatchPairMap(&starRefDistMaps);
    ClearXmatchPairMap(&starListDistMaps);

    return mcsSUCCESS;
}

/**
 * Return the star of the list corresponding to the given star ie matching criteria.
 *
 * This method looks for the specified @em star in the list. If found, it
 * returns the pointer to this element. Otherwise, NULL is returned.
 *
 * The method vobsSTAR::IsSame() is used to compare element of the list with
 * the specified one.
 *
 * This method can be used to discover whether a star is in list or not, as
 * shown below:
 * @code
 * mcsINT32 nCriteria = 0;
 * vobsSTAR_CRITERIA_INFO* criterias = NULL;
 *
 * // Initialize criteria informations:
 * if (criteriaList.InitializeCriterias() == mcsFAILURE)
 * {
 *     return mcsFAILURE;
 * }
 *
 * // Get criterias:
 * if (criteriaList.GetCriterias(criterias, nCriteria) == mcsFAILURE)
 * {
 *     return mcsFAILURE;
 * }
 *
 * ...
 * if (IS_NULL(starList.MatchingCriteria(star, criterias, nCriteria)->View()))
 * {
 *     printf ("Star not found in list !!");
 * }
 * @endcode
 *
 * @param star star to compare with
 * @param criterias vobsSTAR_CRITERIA_INFO[] list of comparison criterias
 *                  given by vobsSTAR_COMP_CRITERIA_LIST.GetCriterias()
 * @param nCriteria number of criteria i.e. size of the vobsSTAR_CRITERIA_INFO array
 * @param matcher crossmatch algorithm in action
 * @param mInfo matcher information
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
 */
mcsCOMPL_STAT vobsSTAR_LIST::GetStarMatchingCriteriaUsingDistMap(vobsSTAR_LIST_MATCH_INFO* mInfo,
                                                                 vobsORIGIN_INDEX originIdx,
                                                                 vobsSTAR_PRECESS_MODE precessMode, const mcsDOUBLE listEpoch,
                                                                 vobsSTAR* starRefPtr,
                                                                 vobsSTAR_CRITERIA_INFO* criterias, mcsUINT32 nCriteria,
                                                                 vobsSTAR_MATCH matcher,
                                                                 mcsDOUBLE thresholdScore,
                                                                 mcsUINT32* noMatchs)
{
    // Assert criteria are defined:
    if (nCriteria == 0)
    {
        logWarning("GetStarMatchingCriteriaUsingDistMap: criteria are undefined !");
        return mcsFAILURE;
    }
    if (matcher != vobsSTAR_MATCH_DISTANCE_MAP)
    {
        logWarning("GetStarMatchingCriteriaUsingDistMap: bad matcher !");
        return mcsFAILURE;
    }

    // adjust criterias to use min radius (mates):
    mcsDOUBLE xmRadius = -1.0;
    if (((&criterias[0])->propCompType == vobsPROPERTY_COMP_RA_DEC) && (&criterias[0])->isRadius)
    {
        // only fix rangeRA (used by distance check for radius mode):
        xmRadius = (&criterias[0])->rangeRA;
        (&criterias[0])->rangeRA = vobsSTAR_CRITERIA_RADIUS_MATES * alxARCSEC_IN_DEGREES;
    }
    else
    {
        logWarning("GetStarMatchingCriteriaUsingDistMap: invalid criteria on radius !");
        return mcsFAILURE;
    }

    // As several stars can be present in the [lower; upper] range,
    // an ordered distance map is used to select the closest star matching criteria:
    if (IS_NULL(_sameStarDistMap))
    {
        // create the distance map allocated until destructor is called:
        _sameStarDistMap = new vobsSTAR_PTR_MATCH_MAP();
    }
    else
    {
        _sameStarDistMap->clear();
    }

    // star original RA/DEC (degrees):
    mcsDOUBLE raOrig1, decOrig1;
    mcsDOUBLE pmRa1, pmDec1; // mas/yr
    mcsSTRING2048 dump;

    if (precessMode != vobsSTAR_PRECESS_NONE)
    {
        FAIL(starRefPtr->GetPmRaDec(pmRa1, pmDec1));

        if (precessMode == vobsSTAR_PRECESS_BOTH)
        {
            // copy original RA/DEC:
            FAIL_DO(starRefPtr->GetRaDec(raOrig1, decOrig1),
                    starRefPtr->Dump(dump); logWarning("Failed to get Ra/Dec ! star : %s", dump));

            // correct coordinates:
            starRefPtr->CorrectRaDecEpochs(raOrig1, decOrig1, pmRa1, pmDec1, EPOCH_2000, listEpoch);
        }
    }

    // Loop on the list stars:
    for (vobsSTAR_PTR_LIST::iterator iterList = _starList.begin(); iterList != _starList.end(); iterList++)
    {
        vobsSTAR* starListPtr = *iterList;

        // correct coordinates using the reference star:
        mcsDOUBLE raOrig2, decOrig2;

        if (precessMode != vobsSTAR_PRECESS_NONE)
        {
            // copy original RA/DEC:
            FAIL_DO(starListPtr->GetRaDec(raOrig2, decOrig2),
                    starListPtr->Dump(dump); logWarning("Failed to get Ra/Dec ! star : %s", dump));

            if (precessMode == vobsSTAR_PRECESS_BOTH)
            {
                mcsDOUBLE pmRa2, pmDec2;
                FAIL(starListPtr->GetPmRaDec(pmRa2, pmDec2));

                // correct coordinates:
                starListPtr->CorrectRaDecEpochs(raOrig2, decOrig2, pmRa2, pmDec2, EPOCH_2000, listEpoch);
            }
            else
            {
                mcsDOUBLE jdDate = starListPtr->GetJdDate();
                mcsDOUBLE epoch = (jdDate != -1.0) ? (EPOCH_2000 + (jdDate - JD_2000) / 365.25) : listEpoch;

                starListPtr->CorrectRaDecEpochs(raOrig2, decOrig2, pmRa1, pmDec1, epoch, EPOCH_2000);
            }
        }

        mcsDOUBLE distAng = NAN;
        mcsDOUBLE distMag = NAN;

        if (IS_TRUE(starRefPtr->IsMatchingCriteria(starListPtr, criterias, nCriteria, &distAng, &distMag, noMatchs)))
        {
            // add candidate in distance map:
            mcsDOUBLE ra1, dec1, ra2, dec2;
            FAIL_DO(starRefPtr->GetRaDec(ra1, dec1),
                    starRefPtr->Dump(dump); logWarning("Failed to get Ra/Dec ! star : %s", dump));
            FAIL_DO(starListPtr->GetRaDec(ra2, dec2),
                    starListPtr->Dump(dump); logWarning("Failed to get Ra/Dec ! star : %s", dump));

            vobsSTAR_PTR_MATCH_ENTRY entryRef = vobsSTAR_PTR_MATCH_ENTRY(distAng, distMag, starListPtr, ra1, dec1, ra2, dec2);
            _sameStarDistMap->insert(vobsSTAR_PTR_MATCH_PAIR(entryRef.score, entryRef));
        }

        if (precessMode != vobsSTAR_PRECESS_NONE)
        {
            // restore original RA/DEC:
            starListPtr->SetRaDec(raOrig2, decOrig2);
        }
    } // loop on list stars

    if (precessMode == vobsSTAR_PRECESS_BOTH)
    {
        // restore original RA/DEC:
        starRefPtr->SetRaDec(raOrig1, decOrig1);
    }

    // restore criterias to use correct radius (xmatch):
    (&criterias[0])->rangeRA = xmRadius;

    // get the number of stars matching criteria:
    mcsINT32 mapSize = _sameStarDistMap->size();

    if (mapSize > 0)
    {
        // distance map is not empty
        vobsSTAR_MATCH_TYPE type = vobsSTAR_MATCH_TYPE_NONE;
        bool doLog = false;

        // check ambiguity between 1st and 2nd matches:
        mcsDOUBLE distAngMatch12 = NAN;

        // Use the first star (sorted by score):
        vobsSTAR_PTR_MATCH_MAP::const_iterator iterDistRef = _sameStarDistMap->begin();
        vobsSTAR_PTR_MATCH_ENTRY entryRef = iterDistRef->second;
        mcsDOUBLE distAngRef = entryRef.distAng;

        // ALWAYS check again distance criteria:
        if (distAngRef > (xmRadius * alxDEG_IN_ARCSEC))
        {
            type = vobsSTAR_MATCH_TYPE_BAD_DIST;

            starRefPtr->Dump(dump);
            logTest("GetStarMatchingCriteriaUsingDistMap: Bad: Distance high: [sep=%.5lf as] for Ref Star: %s",
                    distAngRef, dump);

            // invalid match
            doLog = true;
        }
        else
        {
            // valid distance:
            type = vobsSTAR_MATCH_TYPE_GOOD;

            if ((mapSize > 1) && !isCatalogWds(originIdx) && !isCatalogSB9(originIdx))
            {
                iterDistRef++;
                vobsSTAR_PTR_MATCH_ENTRY entryRef2 = iterDistRef->second;

                // check delta score ?
                mcsDOUBLE deltaScore = fabs(entryRef2.score - entryRef.score);

                // check absolute scores:
                const bool isBetter = USE_BETTER && (
                        (deltaScore >= BETTER_MIN_SCORE_TH_HI) ? (entryRef2.score >= BETTER_SCORE_RATIO_LO * entryRef.score) :
                        ((deltaScore >= BETTER_MIN_SCORE_TH_LO) && (entryRef2.score >= BETTER_SCORE_RATIO_LO * entryRef.score))
                        );

                if (USE_BETTER && !isBetter)
                {
                    logTest("GetStarsMatchingCriteriaUsingDistMap: better match: %s (%.5lf > %.5lf) ratio = %.2lf",
                            isBetter ? "true" : "false", entryRef2.score, entryRef.score,
                            entryRef2.score / mcsMAX(1e-3, entryRef.score)); // avoid div by 0
                }

                // check against threshold according to catalog:
                if ((type == vobsSTAR_MATCH_TYPE_GOOD) && (deltaScore < thresholdScore))
                {
                    type = (USE_BETTER && !isBetter) ? vobsSTAR_MATCH_TYPE_GOOD_AMBIGUOUS_MATCH_SCORE
                            : vobsSTAR_MATCH_TYPE_GOOD_AMBIGUOUS_MATCH_SCORE_BETTER;

                    starRefPtr->Dump(dump);
                    logTest("GetStarMatchingCriteriaUsingDistMap: Bad: Ambiguous match (1st-2nd) [d(score)=%.3lf d(sep)=%.3lf] for Ref Star: %s",
                            deltaScore, fabs(entryRef2.distAng - entryRef.distAng), dump);

                    // invalid match
                    doLog = true;
                }

                // compute real distance between 1st / 2nd (list 2) with epoch correction:
                FAIL(alxComputeDistance(entryRef.ra2, entryRef.de2, entryRef2.ra2, entryRef2.de2, &distAngMatch12));
            }
        }

        // store match result:
        mInfo->Set(type, entryRef);

        char* xmLog = &(mInfo->xm_log[0]);
        logStarMap("GetStarMatchingCriteriaUsingDistMap", _sameStarDistMap, doLog || DO_LOG_STAR_DIST_MAP_XM, xmLog);

        if (!isCatalogWds(originIdx) && !isCatalogSB9(originIdx))
        {
            mInfo->nMates = mapSize;
            mInfo->distAng12 = distAngMatch12; // separation difference between 1 and 2
        }
    }

    _sameStarDistMap->clear();

    // anyway, return mcsSUCCESS
    return mcsSUCCESS;
}

/**
 * Return all star(s) of this list corresponding to the given star ie matching criteria (target Id only).
 *
 * This method looks for the specified @em star in the list. If found, it
 * returns all pointers (same reference star) in the given output list.
 *
 * The method vobsSTAR::IsSameRefStar() is used to compare element of the list with
 * the specified one.
 *
 * @param star star to compare with
 * @param criterias vobsSTAR_CRITERIA_INFO[] list of comparison criterias
 *                  given by vobsSTAR_COMP_CRITERIA_LIST.GetCriterias()
 *
 * @param outputList list to store pointers of the found elements
 */
mcsCOMPL_STAT vobsSTAR_LIST::GetStarsMatchingTargetId(vobsSTAR* star,
                                                      vobsSTAR_CRITERIA_INFO* criterias,
                                                      const mcsDOUBLE extraRadius,
                                                      vobsSTAR_LIST &outputList)
{
    // Assert criteria are defined:
    if (IS_NULL(criterias))
    {
        logWarning("GetStarsMatchingTargetId: criteria are undefined !");
        return mcsFAILURE;
    }

    bool useIndex = false;

    if (_starIndexInitialized)
    {
        // check criteria
        // note: RA_DEC criteria is always the first one
        useIndex = (((&criterias[0])->propCompType == vobsPROPERTY_COMP_RA_DEC) && (&criterias[0])->isRadius);
    }

    if (!useIndex)
    {
        logWarning("GetStarsMatchingTargetId: unsupported matcher !");
        return mcsFAILURE;
    }

    // Use star index
    mcsDOUBLE starDec = EMPTY_COORD_DEG;
    FAIL_DO(star->GetDec(starDec),
            logWarning("Invalid Dec coordinate for the given star !"));

    // note: RA_DEC criteria is always the first one

    // adjust criterias to use larger radius (mates):
    mcsDOUBLE xmRadius = (&criterias[0])->rangeRA;
    mcsDOUBLE searchRadius = xmRadius + extraRadius * alxARCSEC_IN_DEGREES; // deg

    // only fix rangeRA (used by distance check for radius mode):
    (&criterias[0])->rangeRA = searchRadius;

    if (doLog(logDEBUG))
    {
        logDebug("GetStarsMatchingTargetId: real radius: %.6lf as", searchRadius * alxDEG_IN_ARCSEC);
    }

    mcsDOUBLE rangeDEC = (&criterias[0])->rangeRA;

    // note: add +/- COORDS_PRECISION for floating point precision:
    vobsSTAR_PTR_DBL_MAP::iterator lower = _starIndex->lower_bound(starDec - rangeDEC - COORDS_PRECISION);
    vobsSTAR_PTR_DBL_MAP::iterator upper = _starIndex->upper_bound(starDec + rangeDEC + COORDS_PRECISION);

    // As several stars can be present in the [lower; upper] range,
    // an ordered distance map is used to select the closest star matching criteria:
    if (IS_NULL(_sameStarDistMap))
    {
        // create the distance map allocated until destructor is called:
        _sameStarDistMap = new vobsSTAR_PTR_MATCH_MAP();
    }
    else
    {
        _sameStarDistMap->clear();
    }

    mcsDOUBLE distAng = NAN;

    // Search star in the star index boundaries:
    for (vobsSTAR_PTR_DBL_MAP::iterator iter = lower; iter != upper; iter++)
    {
        // reset distance:
        distAng = NAN;

        if (IS_TRUE(star->IsMatchingCriteria(iter->second, criterias, 1, &distAng))) // only ra/dec criteria
        {
            // add candidate in distance map:
            vobsSTAR_PTR_MATCH_ENTRY entry = vobsSTAR_PTR_MATCH_ENTRY(distAng, iter->second);
            _sameStarDistMap->insert(vobsSTAR_PTR_MATCH_PAIR(entry.score, entry));
        }
    }

    // restore criterias to use correct radius (xmatch):
    if (xmRadius > 0.0)
    {
        (&criterias[0])->rangeRA = xmRadius;
    }

    // get the number of stars matching criteria:
    const mcsINT32 mapSize = _sameStarDistMap->size();

    if (mapSize > 0)
    {
        // distance map is not empty
        logStarMap("GetStarsMatchingTargetId()", _sameStarDistMap, DO_LOG_STAR_DIST_MAP_IDX);

        // Copy star pointers:
        for (vobsSTAR_PTR_MATCH_MAP::const_iterator iter = _sameStarDistMap->begin(); iter != _sameStarDistMap->end(); iter++)
        {
            outputList.AddRefAtTail(iter->second.starPtr);
        }

        _sameStarDistMap->clear();

        return mcsSUCCESS;
    }
    return mcsFAILURE;
}

/**
 * Return the stars of the list corresponding to the given star AND matching criteria (ra/dec/mags ...).
 *
 * This method looks for the ALL stars in the list. If found, it adds star pointers to the given list.
 *
 * @param star star to compare with
 * @param criterias vobsSTAR_CRITERIA_INFO[] list of comparison criterias
 *                  given by vobsSTAR_COMP_CRITERIA_LIST.GetCriterias()
 * @param nCriteria number of criteria i.e. size of the vobsSTAR_CRITERIA_INFO array
 * @outputList star list to add matching star pointers into
 */
mcsCOMPL_STAT vobsSTAR_LIST::GetStarsMatchingCriteria(vobsSTAR* star,
                                                      vobsSTAR_CRITERIA_INFO* criterias, mcsUINT32 nCriteria,
                                                      vobsSTAR_LIST &outputList,
                                                      mcsUINT32 maxMatches)
{
    // Assert criteria are defined:
    if (IS_NULL(criterias) || (nCriteria == 0))
    {
        logWarning("GetStarsMatchingCriteria: criteria are undefined !");
        return mcsFAILURE;
    }

    bool useIndex = false;

    if (_starIndexInitialized)
    {
        // check criteria
        // note: RA_DEC criteria is always the first one
        useIndex = ((&criterias[0])->propCompType == vobsPROPERTY_COMP_RA_DEC);
    }
    else
    {
        logWarning("GetStarsMatchingCriteria: unsupported matcher !");
        return mcsFAILURE;
    }

    mcsUINT32 i = 0;

    if (useIndex)
    {
        // Use star index
        // note: RA_DEC criteria is always the first one
        mcsDOUBLE starDec = EMPTY_COORD_DEG;
        FAIL_DO(star->GetDec(starDec),
                logWarning("Invalid Dec coordinate for the given star !"));

        mcsDOUBLE rangeDEC = (&criterias[0])->rangeDEC;

        // note: add +/- COORDS_PRECISION for floating point precision:
        vobsSTAR_PTR_DBL_MAP::iterator lower = _starIndex->lower_bound(starDec - rangeDEC - COORDS_PRECISION);
        vobsSTAR_PTR_DBL_MAP::iterator upper = _starIndex->upper_bound(starDec + rangeDEC + COORDS_PRECISION);

        // As several stars can be present in the [lower; upper] range,
        // an ordered distance map is used to select the closest star matching criteria:
        if (IS_NULL(_sameStarDistMap))
        {
            // create the distance map allocated until destructor is called:
            _sameStarDistMap = new vobsSTAR_PTR_MATCH_MAP();
        }
        else
        {
            _sameStarDistMap->clear();
        }

        mcsINT32 nStars = 0;

        // Search star in the star index boundaries:
        for (vobsSTAR_PTR_DBL_MAP::iterator iter = lower; iter != upper; iter++)
        {
            // reset distance:
            mcsDOUBLE distAng = NAN;
            vobsSTAR* starPtr = iter->second;

            if (IS_TRUE(star->IsMatchingCriteria(starPtr, criterias, nCriteria, &distAng)))
            {
                // add candidate in distance map:
                vobsSTAR_PTR_MATCH_ENTRY entry = vobsSTAR_PTR_MATCH_ENTRY(distAng, starPtr);
                _sameStarDistMap->insert(vobsSTAR_PTR_MATCH_PAIR(entry.score, entry));
            }
            nStars++;
        }

        if (nStars > 0)
        {
            // get the number of stars matching criteria:
            const mcsINT32 mapSize = _sameStarDistMap->size();

            logTest("GetStarsMatchingCriteria(useIndex): %d candidates - %d matches", nStars, mapSize);

            if (mapSize > 0)
            {
                // distance map is not empty
                if (DO_LOG_STAR_DIST_MAP_IDX)
                {
                    logStarMap("GetStarsMatchingCriteria(useIndex)", _sameStarDistMap);
                }

                // Copy star pointers (up to maxMatches):
                for (vobsSTAR_PTR_MATCH_MAP::const_iterator iter = _sameStarDistMap->begin(); iter != _sameStarDistMap->end(); iter++)
                {
                    outputList.AddRefAtTail(iter->second.starPtr);

                    if (++i == maxMatches)
                    {
                        break;
                    }
                }
            }
        }
        else
        {
            logTest("GetStarsMatchingCriteria(useIndex): 0 candidates");
        }
    }
    else
    {
        // Any other matcher mode:
        // Search star in the complete list (slow)
        for (vobsSTAR_PTR_LIST::iterator iter = _starList.begin(); iter != _starList.end(); iter++, i++)
        {
            vobsSTAR* starPtr = *iter;
            if (IS_TRUE(star->IsMatchingCriteria(starPtr, criterias, nCriteria)))
            {
                outputList.AddRefAtTail(starPtr);

                if (++i == maxMatches)
                {
                    break;
                }
            }
        }
    }
    return mcsSUCCESS;
}

/**
 * Dump the given star index in logs
 * @param operationName operation name
 * @param keyName key name
 * @param index star index to dump
 * @param isArcSec true to convert key as arcsec
 * @param doLog true to effectively log the index 
 * @param strLog optional char* buffer to dump the index (length >= 16384)
 */
void vobsSTAR_LIST::logStarIndex(const char* operationName, const char* keyName, vobsSTAR_PTR_DBL_MAP* index,
                                 const bool isArcSec, const bool doLog, char* strLog)
{
    if (IS_NULL(index) || (!doLog && IS_NULL(strLog)))
    {
        return;
    }

    char* strLog0 = NULL;

    if (doLog)
    {
        logInfo("%s: Star index [%lu stars]", operationName, index->size());
    }
    if (IS_NOT_NULL(strLog))
    {
        strLog0 = strLog;
        size_t len = strlen(strLog0);
        strLog += len;
        snprintf(strLog, 16384 - len, ": %lu stars|", index->size());
        strLog += (strlen(strLog0) - len);
    }

    mcsINT32 i = 0;
    vobsSTAR* starPtr;
    const char* unit;
    mcsDOUBLE key;
    mcsSTRING2048 dump;

    for (vobsSTAR_PTR_DBL_MAP::const_iterator iter = index->begin(); iter != index->end(); iter++)
    {
        i++;
        key = iter->first;
        starPtr = iter->second;

        if (isArcSec)
        {
            key *= alxDEG_IN_ARCSEC;
            unit = "as";
        }
        else
        {
            unit = "deg";
        }

        // Get star dump:
        starPtr->Dump(dump);

        if (doLog)
        {
            logInfo("  Star %4d (%s=%.5lf %s): %s", i, keyName, key, unit, dump);
        }
        if (IS_NOT_NULL(strLog))
        {
            size_t len = strlen(strLog0);
            snprintf(strLog, 16384 - len, "Star %2d (%s=%.5lf %s): %s|", i, keyName, key, unit, dump);
            strLog += (strlen(strLog0) - len);
        }
    }
}

/**
 * Dump the given star index in logs
 * @param operationName operation name
 * @param keyName key name
 * @param index star map to dump
 * @param isArcSec true to convert key as arcsec
 * @param doLog true to effectively log the index 
 * @param strLog optional char* buffer to dump the index (length >= 16384)
 */
void vobsSTAR_LIST::logStarMap(const char* operationName, vobsSTAR_PTR_MATCH_MAP* distMap,
                               const bool doLog, char* strLog)
{
    if (IS_NULL(distMap) || (!doLog && IS_NULL(strLog)))
    {
        return;
    }

    char* strLog0 = NULL;

    if (doLog)
    {
        logInfo("%s: Star map [%lu stars]", operationName, distMap->size());
    }
    if (IS_NOT_NULL(strLog))
    {
        strLog0 = strLog;
        const size_t len = strlen(strLog0);
        strLog += len;
        snprintf(strLog, 16384 - len, "=%lu|", distMap->size());
        strLog += (strlen(strLog0) - len);
    }

    mcsUINT32 i = 0;
    mcsSTRING2048 dump;

    for (vobsSTAR_PTR_MATCH_MAP::const_iterator iter = distMap->begin(); iter != distMap->end(); iter++)
    {
        i++;
        vobsSTAR_PTR_MATCH_ENTRY entry = iter->second;
        mcsDOUBLE score = entry.score;
        mcsDOUBLE distAng = entry.distAng;
        mcsDOUBLE distMag = entry.distMag;
        vobsSTAR* starPtr = entry.starPtr;

        // Get star dump:
        starPtr->Dump(dump);

        if (doLog)
        {
            if (distMag > 0.0)
            {
                logInfo(" [%2u: score=%.5lf sep=%.5lf dmag=%.5lf]=%s", i, score, distAng, distMag, dump);
            }
            else if (score != distAng)
            {
                logInfo(" [%2u: score=%.5lf sep=%.5lf]=%s", i, score, distAng, dump);
            }
            else
            {
                logInfo(" [%2u: sep=%.5lf]=%s", i, distAng, dump);
            }
        }
        if (IS_NOT_NULL(strLog))
        {
            size_t len = strlen(strLog0);
            if (distMag > 0.0)
            {
                snprintf(strLog, 16384 - len, "[%2u: score=%.5lf sep=%.5lf dmag=%.5lf]=%s|", i, score, distAng, distMag, dump);
            }
            else if (score != distAng)
            {
                snprintf(strLog, 16384 - len, "[%2u: score=%.5lf sep=%.5lf]=%s|", i, score, distAng, dump);
            }
            else
            {
                snprintf(strLog, 16384 - len, "[%2u: sep=%.5lf]=%s|", i, distAng, dump);
            }
            strLog += (strlen(strLog0) - len);
        }
    }
}

/**
 * Merge the specified list.
 *
 * This method merges all stars of the given list with the current one. If a
 * star is already stored in the list, it is just updated using
 * vobsSTAR::Update method, otherwise it is added to the list.
 *
 * @param list star list to merge with
 * @param criteriaList (optional) star comparison criteria
 * @param fixRaDecEpoch flag to indicate that the Ra/Dec coordinates should be corrected in the given list (wrong epoch)
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
 */
mcsCOMPL_STAT vobsSTAR_LIST::Merge(vobsSTAR_LIST &list,
                                   vobsSTAR_COMP_CRITERIA_LIST* criteriaList,
                                   mcsLOGICAL updateOnly)
{
    const mcsUINT32 nbStars = list.Size();

    const vobsCATALOG_META* thisCatalogMeta = GetCatalogMeta();
    const vobsCATALOG_META* listCatalogMeta = list.GetCatalogMeta();

    if (nbStars == 0)
    {
        // Update catalog id / meta:
        if (IS_NULL(thisCatalogMeta) && !isCatalog(GetCatalogId()))
        {
            SetCatalogMeta(list.GetCatalogId(), listCatalogMeta);
        }
        else if (GetCatalogId() != list.GetCatalogId())
        {
            SetCatalogMeta(vobsORIG_MIXED_CATALOG, NULL);
        }
        // nothing to do
        return mcsSUCCESS;
    }

    const bool isLogDebug = doLog(logDEBUG);
    const bool isLogTest = doLog(logTEST);

    // get catalog id to tell matcher to log dist map !
    vobsORIGIN_INDEX origIdx = IS_NOT_NULL(listCatalogMeta) ? listCatalogMeta->GetCatalogId() : vobsORIG_NONE;

    // size of this list:
    const mcsUINT32 currentSize = Size();

    const bool hasCriteria = IS_NOT_NULL(criteriaList);

    mcsINT32 nCriteria = 0;
    vobsSTAR_CRITERIA_INFO* criterias = NULL;

    if (hasCriteria)
    {
        if (isLogTest)
        {
            logTest("Merge: list [%s][%d stars] with criteria << input list [%s][%d stars]",
                    GetName(), currentSize, list.GetName(), nbStars);
        }

        // log criterias:
        criteriaList->log(logTEST, "Merge: ");

        // Get criterias:
        FAIL(criteriaList->GetCriterias(criterias, nCriteria));
    }
    else
    {
        logWarning("Merge: list [%s][%d stars] WITHOUT criteria << input list [%s][%d stars]; duplicated stars can occur !",
                   GetName(), currentSize, list.GetName(), nbStars);

        // Do not support such case anymore
        errAdd(vobsERR_UNKNOWN_CATALOG);
        return mcsFAILURE;
    }

    if (isLogTest)
    {
        logTest("Merge:  work list [%s] catalog id: '%s'", GetName(), GetCatalogName());
        logTest("Merge: input list [%s] catalog id: '%s'", list.GetName(), list.GetCatalogName());
    }

    // Define overwrite mode:
    vobsOVERWRITE overwrite = vobsOVERWRITE_NONE;
    const vobsSTAR_PROPERTY_MASK* overwritePropertyMask = NULL;
    // flag indicating to overwrite Ra/Dec coordinates:
    bool doOverwriteRaDec = false;

    if (IS_NOT_NULL(listCatalogMeta) && IS_NOT_NULL(listCatalogMeta->GetOverwritePropertyMask()))
    {
        overwrite = vobsOVERWRITE_PARTIAL;
        overwritePropertyMask = listCatalogMeta->GetOverwritePropertyMask();

        doOverwriteRaDec = vobsSTAR::IsRaDecOverwrites(overwritePropertyMask);

        logDebug("Merge: overwrite RA/DEC property: %s", (doOverwriteRaDec) ? "true" : "false");

        if (isLogDebug)
        {
            for (mcsUINT32 i = 0; i < overwritePropertyMask->size(); i++)
            {
                if ((*overwritePropertyMask)[i])
                {
                    logDebug("Merge: overwrite property: %d", i);
                }
            }
        }
    }

    bool isPreIndexed = _starIndexInitialized;

    if (!isPreIndexed)
    {
        FAIL(PrepareIndex());
    }

    mcsDOUBLE starDec;
    vobsSTAR_LIST_MATCH_INFO mInfo;
    mInfo.shared = 1;

    // Get the first star of the list
    vobsSTAR* starPtr = list.GetNextStar(mcsTRUE);

    const mcsINT32 propLen = starPtr->NbProperties();
    const mcsINT32 matchTypeLen = vobsSTAR_MATCH_TYPE_GOOD_AMBIGUOUS_MATCH_SCORE_BETTER + 1;

    const mcsUINT32 step = nbStars / 10;
    const bool logProgress = nbStars > 2000;

    // stats:
    mcsUINT32 lookup1 = 0;
    mcsUINT32 lookupMany = 0;
    mcsUINT32 added = 0;
    mcsUINT32 found = 0;
    mcsUINT32 updated = 0;
    mcsUINT32 skipped = 0;
    mcsUINT32 skippedTargetId = 0;
    mcsUINT32 missingTargetId = 0;
    mcsUINT32 noMatchs[nCriteria];
    mcsUINT32 propertyUpdated[propLen];
    mcsUINT32* noMatchPtr = NULL;
    mcsUINT32* propertyUpdatedPtr = NULL;
    mcsUINT32 matchTypes[matchTypeLen];

    mcsSTRING64 starId;
    mcsSTRING2048 dump;
    mcsSTRING65536 fullLog;
    mcsUINT32 maxLogLen = sizeof(fullLog) - 1;

    if (isLogTest)
    {
        // For each criteria
        for (mcsINT32 el = 0; el < nCriteria; el++)
        {
            noMatchs[el] = 0;
        }
        noMatchPtr = noMatchs;

        for (mcsINT32 el = 0; el < matchTypeLen; el++)
        {
            matchTypes[el] = 0;
        }

        // For each star property index
        for (mcsINT32 el = 0; el < propLen; el++)
        {
            propertyUpdated[el] = 0;
        }
        propertyUpdatedPtr = propertyUpdated;
    }

    // MERGE given list into this list:

    // Check that we are in secondary request cases:
    // note: hasCriteria to be sure ..
    if ((currentSize > 0) && IS_TRUE(updateOnly) && hasCriteria)
    {
        logTest("Merge: crossmatch [CLOSEST_REF_STAR]");

        // Define algorithm options:
        const bool useAllMatchingStars = IS_NOT_NULL(listCatalogMeta) ? IS_TRUE(listCatalogMeta->HasMultipleRows()) : false;

        // Define match mode:
        const vobsSTAR_MATCH_MODE matchMode = IS_NOT_NULL(listCatalogMeta) ? listCatalogMeta->GetMatchMode() : vobsSTAR_MATCH_BEST;

        logTest("Merge: xmatch mode : '%s'", (matchMode == vobsSTAR_MATCH_BEST) ? "best" : "all");

        vobsSTAR_PRECESS_MODE precessMode = vobsSTAR_PRECESS_NONE;
        mcsDOUBLE listEpoch = EPOCH_2000;

        // given list coordinates needs precession using reference star's proper motion:
        if (IS_NOT_NULL(listCatalogMeta) && IS_TRUE(listCatalogMeta->DoPrecessEpoch()))
        {
            precessMode = vobsSTAR_PRECESS_LIST;
            listEpoch = listCatalogMeta->GetEpochMedian();
        }

        // Both catalogs have PM => use pm correction to epoch 1991.25 (HIP):
        const mcsDOUBLE maxPmError = 0.1; // max(error pm) ~ 0.1 as / yr

        mcsDOUBLE maxRadiusPm = 0.0;
        mcsDOUBLE deltaEpoch = 0.0;

        if (isCatalogHip2(origIdx) || isCatalogGaia(origIdx))
        {
            precessMode = vobsSTAR_PRECESS_BOTH;
            listEpoch = EPOCH_HIP; // Hipparcos epoch (oldest) for ASCC x (HIP2 or GAIA)

            logTest("Merge: precess both lists [%s]", vobsGetOriginIndex(origIdx));

            deltaEpoch = fabs(listEpoch - EPOCH_2000); // 1991.25 - 2000 = -8.75 yr
            maxRadiusPm = deltaEpoch * maxPmError; // as

            logDebug("Merge: maxRadiusPm: %.lf as", maxRadiusPm);
        }

        mcsDOUBLE xmRadius = -1.0;
        if (((&criterias[0])->propCompType == vobsPROPERTY_COMP_RA_DEC) && (&criterias[0])->isRadius)
        {
            // only fix rangeRA (used by distance check for radius mode):
            xmRadius = (&criterias[0])->rangeRA;
        }
        else
        {
            logWarning("Merge: invalid criteria on radius !");
            return mcsFAILURE;
        }

        const mcsDOUBLE halfResolution = (IS_NOT_NULL(listCatalogMeta) ? 0.5 * listCatalogMeta->GetPrecision() : 1.0);

        // threshold in arcsec:
        const mcsDOUBLE thresholdScore = mcsMAX(isCatalogASCC(origIdx) ? 0.0 : MIN_SCORE_TH,
                                                mcsMIN(1.0, mcsMIN(halfResolution, 0.5 * xmRadius * alxDEG_IN_ARCSEC)));

        logTest("Merge: List[%s] Radius: %.2lf - 1/2 Resolution : %.2lf - Threshold: %.3lf", vobsGetOriginIndex(origIdx),
                xmRadius * alxDEG_IN_ARCSEC, halfResolution, thresholdScore);

        if (isLogTest)
        {
            if (useAllMatchingStars)
            {
                logTest("Merge: use All Matching Stars");
            }
            const vobsCATALOG_META* catalogMeta = NULL;
            if (precessMode != vobsSTAR_PRECESS_NONE)
            {
                catalogMeta = listCatalogMeta;
                logTest("Merge: precess reference star to epoch %.3lf", (precessMode == vobsSTAR_PRECESS_BOTH) ? listEpoch : EPOCH_2000);
            }
            if (IS_NOT_NULL(catalogMeta))
            {
                if (catalogMeta->IsSingleEpoch())
                {
                    logTest("Merge: precess candidate stars from epoch %.3lf", listEpoch);
                }
                else
                {
                    logTest("Merge: precess candidate stars using JD in epoch range [%.3lf %.3lf]", catalogMeta->GetEpochFrom(), catalogMeta->GetEpochTo());
                }
            }
        }

        // Secondary requests = Partial CROSS MATCH: use the reference star identifiers
        // For each given star, the query returns 1..n stars (cone search) having all the same reference star identifier
        // These stars must be processed as one sub list (correct epoch / coordinates ...) and find which star is really
        // corresponding to the initially requested star

        // First, sort list by targetId, dec/ra (in case the input list contained duplicates but not in order):
        list.Sort(vobsSTAR_ID_TARGET);

        // Create a temporary list of star having the same reference star identifier
        vobsSTAR_LIST subList("SubListMerge");
        // define the free pointer flag to avoid double frees (this list and the given list are storing same star pointers):
        subList.SetFreeStarPointers(false);

        // note: sub list has no star index created => disabled !!

        // Create a temporary list for reference stars (same reference star identifier)
        vobsSTAR_LIST subListRef("SubListRef");
        // define the free pointer flag to avoid double frees (this list and the given list are storing same star pointers):
        subListRef.SetFreeStarPointers(false);

        // Create a temporary list for reference stars (same reference star identifier)
        vobsSTAR_LIST subListRefNb("SubListRefNeighbours");
        // define the free pointer flag to avoid double frees (this list and the given list are storing same star pointers):
        subListRefNb.SetFreeStarPointers(false);

        // list of unique processed (ref) star pointers:
        vobsSTAR_PTR_SET processedRefs;

        // temporary star to perform cone search on Target Ref coords
        vobsSTAR starRef;
        // set fake coordinates (directly set by setRaDec below):
        starRef.SetPropertyValue(vobsSTAR_POS_EQ_RA_MAIN,  "00:00:00.000",  vobsORIG_COMPUTED);
        starRef.SetPropertyValue(vobsSTAR_POS_EQ_DEC_MAIN, "+00:00:00.000", vobsORIG_COMPUTED);

        // temporary star to store xmatch information (only):
        vobsSTAR starFake;

        vobsSTAR_XM_PAIR_MAP xmPairMap;

        mcsUINT32 nbSubStars, nbFilteredSubStars;

        const char* targetId = NULL;
        const char* lastTargetId = NULL;
        bool isLast = false;
        bool isSameId = false;

        mcsDOUBLE raRef, decRef;

        // For each star of the given list
        for (mcsUINT32 el = 0; el <= nbStars; el++)
        {
            if (isLogTest && logProgress && (el % step == 0))
            {
                logTest("Merge: merged stars = %d", el);
            }

            // is last ?
            isLast = (el == nbStars);

            if (isLast)
            {
                isSameId = true;
            }
            else
            {
                starPtr = list.GetNextStar((mcsLOGICAL) (el == 0));

                // Extract star identifier:
                targetId = starPtr->GetPropertyValue(starPtr->GetTargetIdProperty());

                // Is the same target Id ?
                isSameId = IS_NOT_NULL(lastTargetId) ? (strcmp(lastTargetId, targetId) == 0) : true;

                // update last target id:
                lastTargetId = targetId;

                if (isSameId)
                {
                    // Ensure star has coordinates:
                    if (IS_FALSE(starPtr->isRaDecSet()))
                    {
                        starPtr->Dump(dump);
                        logInfo("Missing Ra/Dec for star: %s", dump);
                    }
                    else
                    {
                        // same target id, add this star to the sub list:
                        subList.AddRefAtTail(starPtr);
                    }
                }
            }

            if (isLast || !isSameId)
            {
                // Filter duplicated rows in subList (brute-force):
                nbSubStars = subList.Size();

                if (nbSubStars > 1)
                {
                    if (DO_LOG_DUP_FILTER)
                    {
                        logTest("filter subList size = %d", nbSubStars);

                        for (vobsSTAR_PTR_LIST::const_iterator iter = subList._starList.begin(); iter != subList._starList.end(); iter++)
                        {
                            // Get star dump:
                            (*iter)->Dump(dump);
                            logTest("Star : %s", dump);
                        }
                    }

                    // Derived from std:list.unique() (remove contiguous equal elements)
                    // Requires the subList to be first properly sorted (ID_TARGET, DEC)
                    // ~ O(n) but sublists are very small (<10):

                    vobsSTAR_PTR_LIST::iterator first = subList._starList.begin();
                    vobsSTAR_PTR_LIST::iterator last = subList._starList.end();
                    vobsSTAR_PTR_LIST::iterator next = first;

                    while (++next != last)
                    {
                        if ((*first)->equals(**next))
                        {
                            if (DO_LOG_DUP_FILTER)
                            {
                                // Get star dump:
                                (*next)->Dump(dump);
                                logTest("erase: %s", dump);
                            }
                            subList._starList.erase(next);
                        }
                        else
                        {
                            first = next;
                        }
                        next = first;
                    }

                    nbFilteredSubStars = subList.Size();

                    if (nbFilteredSubStars < nbSubStars)
                    {
                        logTest("filtered subList size: [%d / %d]", nbFilteredSubStars, nbSubStars);
                        skipped += (nbSubStars - nbFilteredSubStars);
                        nbSubStars = nbFilteredSubStars;
                    }
                }

                if (isLogDebug && (nbSubStars > 1))
                {
                    logDebug("process subList size = %d (same targetId)", nbSubStars);
                }

                if (nbSubStars > 0)
                {
                    {
                        // Get first star in the sub list to extract the reference star:
                        vobsSTAR* subStarPtr = subList.GetNextStar(mcsTRUE);

                        // Extract star identifier:
                        targetId = subStarPtr->GetPropertyValue(subStarPtr->GetTargetIdProperty());

                        // Update reference star to use RA/DE (epoch J2000) of the reference star:
                        FAIL_DO(subStarPtr->GetRaDecRefStar(raRef, decRef),
                                // Get star dump:
                                subStarPtr->Dump(dump);
                                logWarning("Invalid reference star: %s", dump);
                                );
                        starRef.SetRaDec(raRef, decRef);
                    }

                    // Adjust extra radius to take into account pm correction:
                    mcsDOUBLE extraRadius = (precessMode == vobsSTAR_PRECESS_BOTH) ? maxRadiusPm : 0.0; // as

                    // Get All reference stars using only raRef/decRef (only Ra/Dec criteria ie same target id)
                    // (using star index and targetId) to get all identical GAIA matches (ra/dec pmra/dec)

                    // Clear sub list of matching reference stars:
                    // define the free pointer flag to avoid double frees (this list and the given list are storing same star pointers):
                    subListRef.ClearRefs(false);

                    if (GetStarsMatchingTargetId(&starRef, criterias, extraRadius, subListRef) == mcsFAILURE)
                    {
                        // BIG problem: reference star not found => skip it
                        logDebug("Reference star NOT found for targetId '%s'", targetId);

                        missingTargetId++;
                        skipped += nbSubStars;
                    }
                    else
                    {
                        if (isLogDebug)
                        {
                            if (subListRef.Size() > 1)
                            {
                                // use N-M case
                                logDebug("Many Reference stars for targetId '%s'", targetId);
                                mcsUINT32 i = 0;

                                for (vobsSTAR_PTR_LIST::iterator iterRef = subListRef._starList.begin(); iterRef != subListRef._starList.end(); iterRef++)
                                {
                                    vobsSTAR* starFoundPtr = *iterRef;
                                    // Get star dump:
                                    starFoundPtr->Dump(dump);
                                    logDebug("Reference star %4d: %s", (++i), dump);
                                }
                            }
                            else
                            {
                                // get the first ref star to compute new positions (pm) (GAIA coordinates)
                                vobsSTAR* starFoundPtr = subListRef.GetNextStar(mcsTRUE);
                                // Get star dump:
                                starFoundPtr->Dump(dump);
                                logDebug("Reference star found for targetId '%s' : %s", targetId, dump);
                            }
                        }

                        bool match = false;

                        if (useAllMatchingStars)
                        {
                            // CIO / PHOTO catalogs (only)
                            // Loop on all reference stars:
                            for (vobsSTAR_PTR_LIST::iterator iterRef = subListRef._starList.begin(); iterRef != subListRef._starList.end(); iterRef++)
                            {
                                vobsSTAR* starFoundPtr = *iterRef;

                                // skip processed refs:
                                if (processedRefs.find(starFoundPtr) != processedRefs.end())
                                {
                                    if (isLogDebug)
                                    {
                                        // Get Star ID
                                        FAIL(starFoundPtr->GetId(starId, sizeof (starId)));
                                        logDebug("Skip processed star ref : %s", starId);
                                    }
                                    match = true; // to have correct stats
                                }
                                else
                                {
                                    // Update the reference stars using all matching star:
                                    // it is imperative for catalogs giving multiple results ie stars (photometry II/225)
                                    for (vobsSTAR_PTR_LIST::iterator iterList = subList._starList.begin(); iterList != subList._starList.end(); iterList++)
                                    {
                                        vobsSTAR* subStarPtr = *iterList;

                                        if (IS_TRUE(starFoundPtr->IsMatchingCriteria(subStarPtr, criterias, nCriteria, NULL, NULL, noMatchPtr)))
                                        {
                                            match = true;
                                            found++;

                                            // Add ref in processed refs set (unique pointers):
                                            processedRefs.insert(starFoundPtr);

                                            // Anyway - clear the target identifier property (useless) before vobsSTAR::Update !
                                            subStarPtr->GetTargetIdProperty()->ClearValue();
                                            // Update the reference star:
                                            if (IS_TRUE(starFoundPtr->Update(*subStarPtr, overwrite, overwritePropertyMask, propertyUpdatedPtr)))
                                            {
                                                updated++;
                                            }
                                        }
                                        else
                                        {
                                            skipped++;
                                        }
                                    }
                                }
                            }
                        }
                        else
                        {
                            // Get all neighbours within 3 as to determine matches:
                            extraRadius += vobsSTAR_CRITERIA_RADIUS_MATES;

                            // Clear sub list of matching reference stars:
                            // define the free pointer flag to avoid double frees (this list and the given list are storing same star pointers):
                            subListRefNb.ClearRefs(false);

                            // note: no FAIL as subListRef is not empty:
                            GetStarsMatchingTargetId(&starRef, criterias, extraRadius, subListRefNb);

                            if (isLogDebug)
                            {
                                logDebug("GetStarsMatchingTargetId ref stars: %d within %.3lf as /  %d within %.3lf as",
                                         subListRef.Size(), xmRadius * alxDEG_IN_ARCSEC, subListRefNb.Size(),
                                         extraRadius + xmRadius * alxDEG_IN_ARCSEC);
                            }

                            // clear mapping anyway:
                            ClearXmatchMapping(&xmPairMap);

                            if (subListRefNb.Size() > 1)
                            {
                                lookupMany++;

                                // Try N-N approach:
                                FAIL(subList.GetStarsMatchingCriteriaUsingDistMap(&xmPairMap, origIdx, matchMode,
                                                                                  precessMode, listEpoch,
                                                                                  &subListRefNb,
                                                                                  criterias, nCriteria,
                                                                                  vobsSTAR_MATCH_DISTANCE_MAP,
                                                                                  thresholdScore, noMatchPtr));
                            }
                            else
                            {
                                lookup1++;

                                // Find in the sub list the star matching criteria with the reference star:
                                mInfo.Clear();

                                // get the first ref star to look up:
                                vobsSTAR* starFoundPtr = subListRef.GetNextStar(mcsTRUE);

                                // note: the sub list does not use star index but use distance map to discrimminate same stars:
                                FAIL(subList.GetStarMatchingCriteriaUsingDistMap(&mInfo, origIdx,
                                                                                 precessMode, listEpoch,
                                                                                 starFoundPtr,
                                                                                 criterias, nCriteria,
                                                                                 vobsSTAR_MATCH_DISTANCE_MAP,
                                                                                 thresholdScore, noMatchPtr));

                                if (mInfo.type != vobsSTAR_MATCH_TYPE_NONE)
                                {
                                    // Store match in mapping (shared):
                                    xmPairMap.insert(vobsSTAR_XM_PAIR(starFoundPtr, &mInfo));
                                }
                            }

                            // TODO: handle duplicates here: multiple stars matching criteria and same distance:
                            // mainly WDS but it happens !
                            /*
                             catalog II/7A: use best row (scoring to be defined) or all ? useAllMatchingStars as it only has 5500 entries !
                                sclsvrServer -   vobs - Test  - process subList size = 2 (same targetId)
                                sclsvrServer -   vobs - Test  - Star index [2 stars]
                                sclsvrServer -   vobs - Test  - Star    1: key = 0.000161585, star = 'Coordinates-ra=03 47 29.1/dec=+24 06 18'
                                sclsvrServer -   vobs - Test  - Star    2: key = 0.000161585, star = 'Coordinates-ra=03 47 29.1/dec=+24 06 18'
                             */

                            if (isLogDebug)
                            {
                                DumpXmatchMapping(&xmPairMap);
                            }

                            if (xmPairMap.size() <= nbSubStars)
                            {
                                skipped += (nbSubStars - xmPairMap.size());
                            }

                            // Loop on all reference stars:
                            for (vobsSTAR_PTR_LIST::iterator iterRef = subListRef._starList.begin(); iterRef != subListRef._starList.end(); iterRef++)
                            {
                                vobsSTAR* starFoundPtr = *iterRef;

                                // skip processed refs:
                                if (processedRefs.find(starFoundPtr) != processedRefs.end())
                                {
                                    if (isLogDebug)
                                    {
                                        // Get Star ID
                                        FAIL(starFoundPtr->GetId(starId, sizeof (starId)));
                                        logDebug("Skip processed star ref : %s", starId);
                                    }
                                    match = true; // to have correct stats
                                }
                                else
                                {
                                    vobsSTAR_XM_PAIR_MAP::const_iterator iterMapping = xmPairMap.find(starFoundPtr);

                                    vobsSTAR* subStarPtr = NULL;
                                    vobsSTAR_LIST_MATCH_INFO* mInfoMatch = NULL;

                                    if (iterMapping != xmPairMap.end())
                                    {
                                        mInfoMatch = iterMapping->second;
                                        subStarPtr = mInfoMatch->starPtr;
                                    }

                                    if (IS_NOT_NULL(mInfoMatch))
                                    {
                                        // Add ref in processed refs set (unique pointers):
                                        processedRefs.insert(starFoundPtr);
                                    }

                                    if (IS_NOT_NULL(subStarPtr))
                                    {
                                        match = true; // to have correct stats

                                        if (DO_LOG_STAR_MATCHING_XM)
                                        {
                                            // Get star dump:
                                            subStarPtr->Dump(dump);

                                            if (isnan(mInfoMatch->distAng12))
                                            {
                                                logTest("Matching star found for targetId '%s': sep=%.5lf as (%d mates) : %s",
                                                        targetId, mInfoMatch->distAng, mInfoMatch->nMates, dump);
                                            }
                                            else
                                            {
                                                logTest("Matching star found for targetId '%s': sep=%.5lf as (sep2nd=%.5lf as) (%d mates) : %s",
                                                        targetId, mInfoMatch->distAng, mInfoMatch->distAng12, mInfoMatch->nMates, dump);
                                            }
                                        }

                                        // store info about matches:
                                        matchTypes[mInfoMatch->type]++;

                                        // store xmatch information into columns:
                                        // TODO: use property indices instead:
                                        const char* propIdNMates = NULL;
                                        const char* propIdScore = NULL;
                                        const char* propIdSep = NULL;
                                        const char* propIdDmag = NULL;
                                        const char* propIdSep2nd = NULL;

                                        if (alxIsDevFlag() && alxIsNotLowMemFlag())
                                        {
                                            vobsGetXmatchColumnsFromOriginIndex(origIdx, &propIdNMates, &propIdScore, &propIdSep, &propIdDmag, &propIdSep2nd);
                                        }

                                        // Note: general changes on subStarPtr (not depending on ref star):
                                        if (IS_NOT_NULL(propIdNMates))
                                        {
                                            // only main catalogs:
                                            FAIL(subStarPtr->GetProperty(propIdNMates)->SetValue(mInfoMatch->nMates, origIdx, vobsCONFIDENCE_HIGH, mcsTRUE))
                                            FAIL(subStarPtr->GetProperty(propIdSep)->SetValue(mInfoMatch->distAng, origIdx, vobsCONFIDENCE_HIGH, mcsTRUE))

                                            if (IS_NOT_NULL(propIdScore))
                                            {
                                                FAIL(subStarPtr->GetProperty(propIdScore)->SetValue(mInfoMatch->score, origIdx, vobsCONFIDENCE_HIGH, mcsTRUE))
                                            }
                                            if (IS_NOT_NULL(propIdDmag) && !isnan(mInfoMatch->distMag))
                                            {
                                                FAIL(subStarPtr->GetProperty(propIdDmag)->SetValue(mInfoMatch->distMag, origIdx, vobsCONFIDENCE_HIGH, mcsTRUE))
                                            }
                                            if (IS_NOT_NULL(propIdSep2nd) && !isnan(mInfoMatch->distAng12))
                                            {
                                                FAIL(subStarPtr->GetProperty(propIdSep2nd)->SetValue(mInfoMatch->distAng12, origIdx, vobsCONFIDENCE_HIGH, mcsTRUE))
                                            }
                                        }

                                        found++;

                                        if (isLogDebug)
                                        {
                                            // Get star dump:
                                            starFoundPtr->Dump(dump);
                                            logDebug("Updating reference star found for targetId '%s' : %s", targetId, dump);
                                        }

                                        // Anyway - clear the target identifier property (useless)
                                        subStarPtr->GetTargetIdProperty()->ClearValue();

                                        // Anyway - clear the observation date property (useless)
                                        subStarPtr->GetJdDateProperty()->ClearValue();

                                        // only main catalogs:
                                        if (IS_NOT_NULL(propIdNMates))
                                        {
                                            // set group_size as max(group_size, n_mates)
                                            if (mInfoMatch->nMates > 1)
                                            {
                                                mcsINT32 refGroupSize = 0;
                                                if (isPropSet(starFoundPtr->GetGroupSizeProperty()))
                                                {
                                                    FAIL(starFoundPtr->GetGroupSizeProperty()->GetValue(&refGroupSize));
                                                }
                                                if (refGroupSize < mInfoMatch->nMates)
                                                {
                                                    FAIL(subStarPtr->GetGroupSizeProperty()->SetValue(mInfoMatch->nMates, origIdx, vobsCONFIDENCE_HIGH, mcsTRUE))
                                                }
                                            }
                                        }
                                        // Update Main Flags:
                                        if (vobsIsMainCatalogFromOriginIndex(origIdx) && (mInfoMatch->type > vobsSTAR_MATCH_TYPE_GOOD))
                                        {
                                            mcsINT32 flags = 0;
                                            if (isPropSet(starFoundPtr->GetXmMainFlagProperty()))
                                            {
                                                FAIL(starFoundPtr->GetXmMainFlagProperty()->GetValue(&flags));
                                            }
                                            flags |= vobsGetMatchTypeAsFlag(mInfoMatch->type);

                                            if (isLogDebug)
                                            {
                                                FAIL(starFoundPtr->GetId(starId, sizeof (starId)));
                                                logDebug("Merge: update main flags for '%s': %d", starId, flags);
                                            }
                                            FAIL(subStarPtr->GetXmMainFlagProperty()->SetValue(flags, vobsORIG_MIXED_CATALOG, vobsCONFIDENCE_HIGH, mcsTRUE))
                                        }
                                        if (alxIsDevFlag() && alxIsNotLowMemFlag())
                                        {
                                            // Update Log about all catalogs:
                                            if (strlen(mInfoMatch->xm_log) != 0)
                                            {
                                                const char* refLog = starFoundPtr->GetXmLogProperty()->GetValueOrBlank();
                                                char* xmLog = mInfoMatch->xm_log;
                                                snprintf(fullLog, maxLogLen, "%s[%s:%s]%s", refLog, list.GetCatalogName(),
                                                         vobsGetMatchType(mInfoMatch->type), xmLog);

                                                FAIL(subStarPtr->GetXmLogProperty()->SetValue(fullLog, vobsORIG_MIXED_CATALOG, vobsCONFIDENCE_HIGH, mcsTRUE))
                                            }
                                            // Update All Flags:
                                            if (mInfoMatch->type > vobsSTAR_MATCH_TYPE_GOOD)
                                            {
                                                mcsINT32 flags = 0;
                                                if (isPropSet(starFoundPtr->GetXmAllFlagProperty()))
                                                {
                                                    FAIL(starFoundPtr->GetXmAllFlagProperty()->GetValue(&flags));
                                                }
                                                flags |= vobsGetMatchTypeAsFlag(mInfoMatch->type);

                                                if (isLogDebug)
                                                {
                                                    FAIL(starFoundPtr->GetId(starId, sizeof (starId)));
                                                    logDebug("Merge: update all flags for '%s': %d", starId, flags);
                                                }
                                                FAIL(subStarPtr->GetXmAllFlagProperty()->SetValue(flags, vobsORIG_MIXED_CATALOG, vobsCONFIDENCE_HIGH, mcsTRUE))
                                            }
                                        }

                                        if (doOverwriteRaDec)
                                        {
                                            // Finally clear the reference star coordinates to be overriden next:
                                            // Note: this can make the star index corrupted !!
                                            starFoundPtr->ClearRaDec();
                                        }

                                        // Anyway - clear the target identifier property (useless) before vobsSTAR::Update !
                                        subStarPtr->GetTargetIdProperty()->ClearValue();
                                        // Update the reference star:
                                        if (IS_TRUE(starFoundPtr->Update(*subStarPtr, overwrite, overwritePropertyMask, propertyUpdatedPtr)))
                                        {
                                            updated++;
                                        }
                                    }
                                    else
                                    {
                                        skipped++;

                                        if (isLogTest)
                                        {
                                            // Get Star ID
                                            FAIL(starFoundPtr->GetId(starId, sizeof (starId)));
                                            logTest("No star matching all criteria for '%s':", starId);
                                            FAIL(subList.logNoMatch(starFoundPtr));
                                        }

                                        if (IS_NOT_NULL(mInfoMatch))
                                        {
                                            // store info about matches:
                                            matchTypes[mInfoMatch->type]++;

                                            // Use (empty) fake star to store xmatch information:
                                            starFake.ClearValues();
                                            subStarPtr = &starFake;

                                            // Update Main Flags:
                                            if (vobsIsMainCatalogFromOriginIndex(origIdx) && (mInfoMatch->type >= vobsSTAR_MATCH_TYPE_BAD_DIST))
                                            {
                                                mcsINT32 flags = 0;
                                                if (isPropSet(starFoundPtr->GetXmMainFlagProperty()))
                                                {
                                                    FAIL(starFoundPtr->GetXmMainFlagProperty()->GetValue(&flags));
                                                }
                                                flags |= vobsGetMatchTypeAsFlag(mInfoMatch->type);

                                                if (isLogDebug)
                                                {
                                                    FAIL(starFoundPtr->GetId(starId, sizeof (starId)));
                                                    logDebug("Merge: update flags for '%s': %d", starId, flags);
                                                }
                                                FAIL(subStarPtr->GetXmMainFlagProperty()->SetValue(flags, vobsORIG_MIXED_CATALOG, vobsCONFIDENCE_HIGH, mcsTRUE))
                                            }

                                            const char* propIdNMates = NULL;
                                            const char* propIdScore = NULL;
                                            const char* propIdSep = NULL;
                                            const char* propIdDmag = NULL;
                                            const char* propIdSep2nd = NULL;

                                            if (alxIsDevFlag() && alxIsNotLowMemFlag())
                                            {
                                                vobsGetXmatchColumnsFromOriginIndex(origIdx, &propIdNMates, &propIdScore, &propIdSep, &propIdDmag, &propIdSep2nd);

                                                // Note: general changes on subStarPtr (not depending on ref star):
                                                if (IS_NOT_NULL(propIdNMates))
                                                {
                                                    // only main catalogs:
                                                    FAIL(subStarPtr->GetProperty(propIdNMates)->SetValue(mInfoMatch->nMates, origIdx, vobsCONFIDENCE_HIGH, mcsTRUE))
                                                    FAIL(subStarPtr->GetProperty(propIdSep)->SetValue(mInfoMatch->distAng, origIdx, vobsCONFIDENCE_HIGH, mcsTRUE))

                                                    if (IS_NOT_NULL(propIdScore))
                                                    {
                                                        FAIL(subStarPtr->GetProperty(propIdScore)->SetValue(mInfoMatch->score, origIdx, vobsCONFIDENCE_HIGH, mcsTRUE))
                                                    }
                                                    if (IS_NOT_NULL(propIdDmag) && !isnan(mInfoMatch->distMag))
                                                    {
                                                        FAIL(subStarPtr->GetProperty(propIdDmag)->SetValue(mInfoMatch->distMag, origIdx, vobsCONFIDENCE_HIGH, mcsTRUE))
                                                    }
                                                    if (IS_NOT_NULL(propIdSep2nd) && !isnan(mInfoMatch->distAng12))
                                                    {
                                                        FAIL(subStarPtr->GetProperty(propIdSep2nd)->SetValue(mInfoMatch->distAng12, origIdx, vobsCONFIDENCE_HIGH, mcsTRUE))
                                                    }
                                                }
                                                // Update Log about all catalogs:
                                                if (strlen(mInfoMatch->xm_log) != 0)
                                                {
                                                    const char* refLog = starFoundPtr->GetXmLogProperty()->GetValueOrBlank();
                                                    char* xmLog = mInfoMatch->xm_log;
                                                    snprintf(fullLog, maxLogLen, "%s[%s:%s]%s", refLog, list.GetCatalogName(),
                                                             vobsGetMatchType(mInfoMatch->type), xmLog);

                                                    FAIL(subStarPtr->GetXmLogProperty()->SetValue(fullLog, vobsORIG_MIXED_CATALOG, vobsCONFIDENCE_HIGH, mcsTRUE))
                                                }
                                                // Update All Flags:
                                                if (mInfoMatch->type >= vobsSTAR_MATCH_TYPE_BAD_DIST)
                                                {
                                                    mcsINT32 flags = 0;
                                                    if (isPropSet(starFoundPtr->GetXmAllFlagProperty()))
                                                    {
                                                        FAIL(starFoundPtr->GetXmAllFlagProperty()->GetValue(&flags));
                                                    }
                                                    flags |= vobsGetMatchTypeAsFlag(mInfoMatch->type);

                                                    if (isLogDebug)
                                                    {
                                                        FAIL(starFoundPtr->GetId(starId, sizeof (starId)));
                                                        logDebug("Merge: update all flags for '%s': %d", starId, flags);
                                                    }
                                                    FAIL(subStarPtr->GetXmAllFlagProperty()->SetValue(flags, vobsORIG_MIXED_CATALOG, vobsCONFIDENCE_HIGH, mcsTRUE))
                                                }

                                                if (isLogDebug)
                                                {
                                                    FAIL(starFoundPtr->GetId(starId, sizeof (starId)));
                                                    // Get star dump:
                                                    starFoundPtr->Dump(dump);
                                                    logDebug("Updating reference star with match info '%s' : %s", starId, dump);
                                                }

                                                // Anyway - clear the target identifier property (useless) before vobsSTAR::Update !
                                                subStarPtr->GetTargetIdProperty()->ClearValue();
                                                // Update the reference star:
                                                if (IS_TRUE(starFoundPtr->Update(*subStarPtr, overwrite, NULL, propertyUpdatedPtr)))
                                                {
                                                    updated++;
                                                }
                                            }
                                        }
                                    }
                                }
                            } // Loop on all ref stars

                            if (!match)
                            {
                                skippedTargetId++;
                            }
                            // clear mapping anyway:
                            ClearXmatchMapping(&xmPairMap);

                            // clear sub lists anyway:
                            // define the free pointer flag to avoid double frees (this list and the given list are storing same star pointers):
                            subListRef.ClearRefs(false);
                            subListRefNb.ClearRefs(false);
                        }
                    }
                }
            } // process sub list

            if (!isSameId)
            {
                // Clear sub list:
                // define the free pointer flag to avoid double frees (this list and the given list are storing same star pointers):
                subList.ClearRefs(false);

                // Ensure star has coordinates:
                if (IS_FALSE(starPtr->isRaDecSet()))
                {
                    starPtr->Dump(dump);
                    logInfo("Missing Ra/Dec for star: %s", dump);
                }
                else
                {
                    // not same target id, add this star to the sub list:
                    subList.AddRefAtTail(starPtr);
                }
            }
        } // for-all stars

        // clear mapping anyway:
        ClearXmatchMapping(&xmPairMap);

        // clear sub lists anyway:
        // define the free pointer flag to avoid double frees (this list and the given list are storing same star pointers):
        subList.ClearRefs(false);
        subListRef.ClearRefs(false);
        subListRefNb.ClearRefs(false);
    }
    else
    {
        logTest("Merge: crossmatch [CLOSEST_ALL_STARS]");

        // Primary requests = Generic CROSS MATCH:
        // TODO: enhance this cross match to take into account that the given list
        // can have several stars matching criteria per star in the current list
        // i.e. do not always use the first star present in the given list !

        // For each star of the given list
        for (mcsUINT32 el = 0; el < nbStars; el++)
        {
            if (isLogTest && logProgress && (el % step == 0))
            {
                logTest("Merge: merged stars = %d", el);
            }

            starPtr = list.GetNextStar((mcsLOGICAL) (el == 0));

            // Anyway - clear the target identifier property (useless) to not use it:
            starPtr->GetTargetIdProperty()->ClearValue();

            vobsSTAR* starFoundPtr = NULL;

            // If star is in the list ?
            if (hasCriteria)
            {
                mInfo.Clear();

                // note: use star index and compute distance map:
                starFoundPtr = GetStarMatchingCriteria(starPtr, criterias, nCriteria, vobsSTAR_MATCH_INDEX, &mInfo, noMatchPtr);
            }
            else
            {
                starFoundPtr = GetStar(starPtr);
            }

            if (IS_NOT_NULL(starFoundPtr))
            {
                if (DO_LOG_STAR_MATCHING_XM)
                {
                    // Get star dump:
                    starFoundPtr->Dump(dump);
                    logTest("Matching star found: sep=%.5lf as : %s", mInfo.distAng, dump);
                }

                found++;

                // Anyway - clear the target identifier property (useless) before vobsSTAR::Update !
                starPtr->GetTargetIdProperty()->ClearValue();
                // Update the reference star:
                if (IS_TRUE(starFoundPtr->Update(*starPtr, overwrite, overwritePropertyMask, propertyUpdatedPtr)))
                {
                    updated++;
                }
            }
            else if (IS_FALSE(updateOnly) && IS_TRUE(starPtr->isRaDecSet()))
            {
                // Anyway - clear the target identifier property (useless) before AddAtTail() !
                starPtr->GetTargetIdProperty()->ClearValue();
                // Else add it to the list (copy ie clone star)
                // TODO: may optimize this star copy but using references instead ?
                AddAtTail(*starPtr);

                // Add new star in the star index:
                FAIL_DO(starPtr->GetDec(starDec),
                        logWarning("Invalid Dec coordinate for the given star !"));

                // add it also to the star index:
                _starIndex->insert(vobsSTAR_PTR_DBL_PAIR(starDec, starPtr));

                added++;
            }
            else
            {
                skipped++;
            }
        }
    }

    // END-MERGE

    // Clear targetId anyway:
    // For each star of the given list
    for (mcsUINT32 el = 0; el < nbStars; el++)
    {
        starPtr = list.GetNextStar((mcsLOGICAL) (el == 0));

        // Anyway - clear the target identifier property (useless) to not use it anymore:
        starPtr->GetTargetIdProperty()->ClearValue();
    }

    if (DO_LOG_STAR_INDEX)
    {
        logStarIndex("Merge", "dec", _starIndex);
    }

    if (!isPreIndexed)
    {
        // clear star index uninitialized:
        _starIndex->clear();
        _starIndexInitialized = false;
    }

    // Update catalog id / meta:
    if (IS_NULL(thisCatalogMeta) && !isCatalog(GetCatalogId()))
    {
        SetCatalogMeta(list.GetCatalogId(), listCatalogMeta);
    }
    else if (GetCatalogId() != list.GetCatalogId())
    {
        SetCatalogMeta(vobsORIG_MIXED_CATALOG, NULL);
    }

    if (isLogTest)
    {
        logTest("Merge:  work list [%s] catalog id: '%s'", GetName(), GetCatalogName());
        logTest("Merge: [%s] done: %d stars added / %d found / %d updated / %d skipped (%d skipped / %d missing targetId).",
                list.GetCatalogName(), added, found, updated, skipped, skippedTargetId, missingTargetId);

        if (lookup1 + lookupMany > 0)
        {
            logTest("Merge: xmatch stats: %d lookup(1) / %d lookup(N)", lookup1, lookupMany);
        }
        if (skipped > 0)
        {
            // For each criteria
            for (mcsINT32 el = 0; el < nCriteria; el++)
            {
                vobsSTAR_CRITERIA_INFO* criteria = &criterias[el];

                logTest("Merge: Criteria '%s' mismatch %d times", criteria->propertyId, noMatchs[el]);
            }
            for (mcsINT32 el = 0; el < matchTypeLen; el++)
            {
                mcsUINT32 count = matchTypes[(vobsSTAR_MATCH_TYPE) el];
                if (count > 0)
                {
                    logTest("Merge: xmatch type ['%s'] : %d times", vobsGetMatchType((vobsSTAR_MATCH_TYPE) el), count);
                }
            }
        }
        if (updated > 0)
        {
            // For each star property index
            for (mcsINT32 idx = 0; idx < propLen; idx++)
            {
                mcsUINT32 count = propertyUpdated[idx];
                if (count > 0)
                {
                    const vobsSTAR_PROPERTY_META* meta = vobsSTAR_PROPERTY_META::GetPropertyMeta(idx);
                    if (IS_NOT_NULL(meta))
                    {
                        logTest("Merge: Property '%s' [%s] updated %d times", meta->GetName(), meta->GetId(), count);
                    }
                }
            }
        }
    }
    return mcsSUCCESS;
}

mcsCOMPL_STAT vobsSTAR_LIST::PrepareIndex()
{
    logTest("Indexing star list [%s]", GetName());

    // star pointer on this list:
    vobsSTAR* starPtr;

    mcsDOUBLE starDec;

    // Prepare the star index on declination property:
    if (IS_NULL(_starIndex))
    {
        // create the star index allocated until destructor is called:
        _starIndex = new vobsSTAR_PTR_DBL_MAP();
    }
    else
    {
        _starIndex->clear();
    }
    // star index initialized:
    _starIndexInitialized = true;

    // Add existing stars into the star index:
    for (vobsSTAR_PTR_LIST::iterator iter = _starList.begin(); iter != _starList.end(); iter++)
    {
        starPtr = *iter;

        // Ensure star has coordinates:            
        if (IS_TRUE(starPtr->isRaDecSet()))
        {
            FAIL_DO(starPtr->GetDec(starDec),
                    logWarning("Invalid Dec coordinate for the given star !"));

            _starIndex->insert(vobsSTAR_PTR_DBL_PAIR(starDec, starPtr));
        }
    }

    if (DO_LOG_STAR_INDEX)
    {
        logStarIndex("PrepareIndex", "dec", _starIndex);
    }

    logTest("Indexing star list [%s] done : %d indexed stars.", GetName(), _starIndex->size());

    return mcsSUCCESS;
}

/**
 * Search in this list stars matching criteria and put star pointers in the specified list.
 *
 * @param starPtr reference star (ra/dec/mags)
 * @param criteriaList (optional) star comparison criteria
 * @param outputList star list to put star pointers
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
 */
mcsCOMPL_STAT vobsSTAR_LIST::Search(vobsSTAR* referenceStar,
                                    vobsSTAR_COMP_CRITERIA_LIST* criteriaList,
                                    vobsSTAR_LIST &outputList,
                                    mcsUINT32 maxMatches)
{
    FAIL_NULL_DO(referenceStar,
                 logWarning("Reference star is NULL"));

    const bool isLogTest = doLog(logTEST);

    // size of this list:
    const mcsUINT32 currentSize = Size();

    FAIL_COND_DO((currentSize == 0),
                 logWarning("Star list is empty"));

    const bool hasCriteria = IS_NOT_NULL(criteriaList);

    mcsINT32 nCriteria = 0;
    vobsSTAR_CRITERIA_INFO* criterias = NULL;

    if (hasCriteria)
    {
        if (isLogTest)
        {
            logTest("Search: list [%s][%d stars] with criteria",
                    GetName(), currentSize);
        }

        // log criterias:
        criteriaList->log(logTEST, "Search: ");

        // Get criterias:
        FAIL(criteriaList->GetCriterias(criterias, nCriteria));
    }
    else
    {
        logWarning("Search: list [%s][%d stars] WITHOUT criteria",
                   GetName(), currentSize);

        // Do not support such case anymore
        errAdd(vobsERR_UNKNOWN_CATALOG);
        return mcsFAILURE;
    }

    bool isPreIndexed = _starIndexInitialized;

    if (!isPreIndexed)
    {
        FAIL(PrepareIndex());
    }

    logTest("Search: crossmatch [CLOSEST_REF_STAR]");

    // Get matching star pointers using criteria:
    FAIL(GetStarsMatchingCriteria(referenceStar, criterias, nCriteria, outputList, maxMatches));

    mcsUINT32 found = outputList.Size();

    if (DO_LOG_STAR_INDEX)
    {
        logStarIndex("Search", "dec", _starIndex);
    }

    if (!isPreIndexed)
    {
        // clear star index uninitialized:
        _starIndex->clear();
        _starIndexInitialized = false;
    }

    if (isLogTest)
    {
        logTest("Search: done: %d stars found.", found);
    }

    return mcsSUCCESS;
}

/**
 * Detect (and filter in future) star duplicates in the given star list using the given criteria
 * Filter duplicates in the specified list (auto correlation)
 *
 * @param star star to compare with
 * @param criteriaList (optional) star comparison criteria
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
 */
mcsCOMPL_STAT vobsSTAR_LIST::FilterDuplicates(vobsSTAR_LIST &list,
                                              vobsSTAR_COMP_CRITERIA_LIST* criteriaList,
                                              bool doRemove)
{
    const bool isLogTest = doLog(logTEST);

    const mcsUINT32 nbStars = list.Size();

    if (nbStars == 0)
    {
        // nothing to do
        return mcsSUCCESS;
    }

    // Anyway: clear this list:
    // define the free pointer flag to avoid double frees (this list and list are storing same star pointers):
    ClearRefs(false);

    // list of unique duplicated star pointers (too close):
    vobsSTAR_PTR_SET duplicates;

    const bool hasCriteria = IS_NOT_NULL(criteriaList);

    mcsINT32 nCriteria = 0;
    vobsSTAR_CRITERIA_INFO* criterias = NULL;

    // TODO: decide which separation should be used (1.0" or 5") depends on catalog or scenario (bright, faint, prima catalog ...)???
    mcsDOUBLE filterRadius = (mcsDOUBLE) (1.0 * alxARCSEC_IN_DEGREES);

    if ((list.GetCatalogId() == vobsCATALOG_JSDC_LOCAL_ID)
            || (list.GetCatalogId() == vobsCATALOG_JSDC_FAINT_LOCAL_ID))
    {
        // 0.001 arcsec to keep duplicates:
        filterRadius = (mcsDOUBLE) (0.001 * alxARCSEC_IN_DEGREES);
    }

    mcsDOUBLE oldRadius = 0.0;

    if (hasCriteria)
    {
        if (isLogTest)
        {
            logTest("FilterDuplicates: list [%d stars] with criteria - input list [%d stars]", Size(), nbStars);
        }

        // log criterias:
        criteriaList->log(logTEST, "FilterDuplicates: ");

        // Get criterias:
        FAIL(criteriaList->GetCriterias(criterias, nCriteria));

        if (nCriteria > 0)
        {
            // note: RA_DEC criteria is always the first one
            vobsSTAR_CRITERIA_INFO* criteria = &criterias[0];

            if ((criteria->propCompType == vobsPROPERTY_COMP_RA_DEC) && (criteria->isRadius))
            {
                // keep current radius:
                oldRadius = criteria->rangeRA;

                // set it to filter radius:
                criteria->rangeRA = filterRadius;

                logTest("FilterDuplicates: filter search radius=%0.1lf arcsec", criteria->rangeRA * alxDEG_IN_ARCSEC);
            }
        }
    }
    else
    {
        logWarning("FilterDuplicates: list [%d stars] without criteria - input list [%d stars]", Size(), nbStars);

        // Do not support such case anymore
        errAdd(vobsERR_UNKNOWN_CATALOG);
        return mcsFAILURE;
    }

    // Prepare the star index on declination property:
    if (IS_NULL(_starIndex))
    {
        // create the star index allocated until destructor is called:
        _starIndex = new vobsSTAR_PTR_DBL_MAP();
    }
    else
    {
        _starIndex->clear();
    }

    // star index initialized:
    _starIndexInitialized = true;

    // star pointer on this list:
    vobsSTAR* starPtr;

    // Get the first star of the list (auto correlation):
    vobsSTAR* starFoundPtr;

    mcsDOUBLE starDec;
    vobsSTAR_LIST_MATCH_INFO mInfo;

    const mcsUINT32 step = nbStars / 10;
    const bool logProgress = nbStars > 2000;

    // stats:
    mcsUINT32 added = 0;
    mcsUINT32 found = 0;
    mcsUINT32 different = 0;

    // For each star of the given list
    for (mcsUINT32 el = 0; el < nbStars; el++)
    {
        if (isLogTest && logProgress && (el % step == 0))
        {
            logTest("FilterDuplicates: filtered stars=%d", el);
        }

        starPtr = list.GetNextStar((mcsLOGICAL) (el == 0));

        // If star is in the list ?

        if (hasCriteria)
        {
            // note: use star index and compute distance map:
            starFoundPtr = GetStarMatchingCriteria(starPtr, criterias, nCriteria, vobsSTAR_MATCH_INDEX, &mInfo);
        }
        else
        {
            starFoundPtr = GetStar(starPtr);
        }

        if (IS_NOT_NULL(starFoundPtr))
        {
            // this list has already one star matching criteria = duplicated stars
            found++;

            if (starFoundPtr->compare(*starPtr) != 0)
            {
                logWarning("FilterDuplicates: separation = %.9lf arcsec", mInfo.distAng);

                // TODO: stars are different: do something i.e. reject both / keep one but which one ...
                different++;
            }

            // Put both stars in duplicate set (unique pointers):
            duplicates.insert(starFoundPtr);
            duplicates.insert(starPtr);
        }
        else
            // Ensure star has coordinates:            
            if (IS_TRUE(starPtr->isRaDecSet()))
        {
            // Else add it to the list
            AddRefAtTail(starPtr);

            // Add new star in the star index:
            FAIL_DO(starPtr->GetDec(starDec),
                    logWarning("Invalid Dec coordinate for the given star !"));

            // add it also to the star index:
            _starIndex->insert(vobsSTAR_PTR_DBL_PAIR(starDec, starPtr));

            added++;
        }
    }

    if (DO_LOG_STAR_INDEX)
    {
        logStarIndex("FilterDuplicates", "dec", _starIndex);
    }

    // clear star index uninitialized:
    _starIndex->clear();
    _starIndexInitialized = false;

    // Anyway: clear this list
    Clear();

    if (isLogTest)
    {
        logTest("FilterDuplicates: done: %d unique stars / %d duplicates found : %d different stars.",
                added, found, different);
    }

    // Remove duplicated stars from given list:
    if (found > 0)
    {
        if (isLogTest)
        {
            logTest("FilterDuplicates: %d duplicated stars", duplicates.size());
        }

        mcsSTRING64 starId;
        mcsDOUBLE ra, dec;
        mcsSTRING16 raDeg, decDeg;

        for (vobsSTAR_PTR_SET::iterator iter = duplicates.begin(); iter != duplicates.end(); iter++)
        {
            starFoundPtr = *iter;

            if (isLogTest)
            {
                // Get Star ID
                FAIL(starFoundPtr->GetId(starId, sizeof (starId)));

                // Get Ra/Dec
                FAIL_DO(starFoundPtr->GetRaDec(ra, dec),
                        logWarning("Failed to get Ra/Dec !"));

                vobsSTAR::raToDeg(ra, raDeg);
                vobsSTAR::decToDeg(dec, decDeg);

                if (doRemove)
                {
                    logTest("FilterDuplicates: remove star '%s' (%s %s)", starId, raDeg, decDeg);
                }
                else
                {
                    logTest("FilterDuplicates: detected star '%s' (%s %s)", starId, raDeg, decDeg);
                }
            }

            if (doRemove)
            {
                list.RemoveRef(starFoundPtr);
            }
        }
    }

    if (nCriteria > 0)
    {
        // note: RA_DEC criteria is always the first one
        vobsSTAR_CRITERIA_INFO* criteria = &criterias[0];

        if ((criteria->propCompType == vobsPROPERTY_COMP_RA_DEC) && (criteria->isRadius))
        {
            // restore current radius:

            criteria->rangeRA = oldRadius;
        }
    }

    return mcsSUCCESS;
}

/**
 * vobsSTAR comparison functor
 */
class StarPropertyCompare
{
private:

    mcsINT32 _propertyIndex;
    const vobsSTAR_PROPERTY_META* _meta;
    bool _naturalOrder;
    const StarPropertyCompare* _compOther;

    // members:
    const char* _propertyId;
    vobsPROPERTY_TYPE _propertyType;
    bool _isRA;
    bool _isDEC;

public:
    // Constructor

    StarPropertyCompare(const mcsINT32 propertyIndex, const vobsSTAR_PROPERTY_META* meta, const bool reverseOrder,
                        const StarPropertyCompare* compOther)
    {
        _propertyIndex = propertyIndex;
        _meta = meta;
        _naturalOrder = !reverseOrder;
        _compOther = compOther;

        _propertyId = meta->GetId();
        _propertyType = meta->GetType();

        // is RA or DEC:
        _isRA = isPropRA(_propertyId);
        _isDEC = isPropDEC(_propertyId);

        logDebug("StarPropertyCompare: property [%d - %s]", _propertyIndex, _propertyId);
        logDebug("StarPropertyCompare: property type: %d", _propertyType);
        logDebug("StarPropertyCompare: isRA  = %d", _isRA);
        logDebug("StarPropertyCompare: isDEC = %d", _isDEC);
    }

    /**
     * Check if leftStar < rightStar
     */
    bool operator()(vobsSTAR* leftStar, vobsSTAR* rightStar) const
    {
        // Get star properties:
        vobsSTAR_PROPERTY* leftProperty = leftStar ->GetProperty(_propertyIndex);
        vobsSTAR_PROPERTY* rightProperty = rightStar->GetProperty(_propertyIndex);

        // Check properties are set
        const mcsLOGICAL isProp1Set = leftStar ->IsPropertySet(leftProperty);
        const mcsLOGICAL isProp2Set = rightStar->IsPropertySet(rightProperty);

        // If one of the properties is not set, move it at the begining
        // or at the end, according to the sorting order
        if (IS_FALSE(isProp1Set) || IS_FALSE(isProp2Set))
        {
            // If it is normal sorting order
            if (_naturalOrder)
            {
                // blank values are at the end:
                // If value of next element is not set while previous
                // one is, swap them
                return (IS_TRUE(isProp1Set) && IS_FALSE(isProp2Set));
            }
            // Else (reverse sorting order)
            // blanks values are at the beginning:
            // If value of previous element is not set while next
            // one is, swap them
            return (IS_FALSE(isProp1Set) && IS_TRUE(isProp2Set));
        }
        else
        {
            // Else (properties are set)
            // Compare element values according to property or property
            // type, and check if elements have to be swapped according
            // to the sorting order

            if ((_propertyType != vobsSTRING_PROPERTY) || _isRA || _isDEC)
            {
                mcsDOUBLE value1;
                mcsDOUBLE value2;

                if (_isRA)
                {
                    leftStar ->GetRa(value1);
                    rightStar->GetRa(value2);
                }
                else if (_isDEC)
                {
                    leftStar ->GetDec(value1);
                    rightStar->GetDec(value2);
                }
                else
                {
                    leftStar ->GetPropertyValue(leftProperty, &value1);
                    rightStar->GetPropertyValue(rightProperty, &value2);
                }

                // equals: use other comparator
                if ((value1 == value2) && IS_NOT_NULL(_compOther))
                {
                    return _compOther->operator ()(leftStar, rightStar);
                }

                if (_naturalOrder)
                {
                    return (value1 < value2);
                }
                return (value1 > value2);
            }
            else
            {
                const char* value1 = leftStar ->GetPropertyValue(leftProperty);
                const char* value2 = rightStar->GetPropertyValue(rightProperty);

                int cmp = strcmp(value1, value2);

                // equals: use other comparator
                if ((cmp == 0) && IS_NOT_NULL(_compOther))
                {
                    return _compOther->operator ()(leftStar, rightStar);
                }

                if (_naturalOrder)
                {
                    return (cmp < 0);
                }
                return (cmp > 0);
            }
        }
    }
} ;

/**
 * Sort the list.
 *
 * This method sorts the given list according to the given property Id.
 *
 * @param propertyId property id
 * @param reverseOrder indicates sorting order
 *
 * @return mcsSUCCESS on successful completion, and mcsFAILURE otherwise.
 */
mcsCOMPL_STAT vobsSTAR_LIST::Sort(const char *propertyId, mcsLOGICAL reverseOrder)
{
    // If list is empty or contains only one element, return
    if (Size() <= 1)
    {
        return mcsSUCCESS;
    }

    logInfo("Sort[%s](%d) on %s : start", GetName(), Size(), propertyId);

    // For sorting stability, always sort by declination/ascension too:
    StarPropertyCompare* compRa = NULL;
    StarPropertyCompare* compDec = NULL;
    StarPropertyCompare* comp = NULL;

    // compRa:
    {
        const char *propId = vobsSTAR_POS_EQ_RA_MAIN;

        const mcsINT32 propertyIndex = vobsSTAR::GetPropertyIndex(propId);
        FAIL_COND_DO((propertyIndex == -1),
                     errAdd(vobsERR_INVALID_PROPERTY_ID, propId));

        const vobsSTAR_PROPERTY_META* meta = vobsSTAR_PROPERTY_META::GetPropertyMeta(propertyIndex);
        FAIL_NULL_DO(meta,
                     errAdd(vobsERR_INVALID_PROPERTY_ID, propId));

        // note: compRa may be leaking if fatal error below
        compRa = new StarPropertyCompare(propertyIndex, meta, false, NULL);
    }
    // compDec(compRa):
    {
        const char *propId = vobsSTAR_POS_EQ_DEC_MAIN;

        const mcsINT32 propertyIndex = vobsSTAR::GetPropertyIndex(propId);
        FAIL_COND_DO((propertyIndex == -1),
                     errAdd(vobsERR_INVALID_PROPERTY_ID, propId));

        const vobsSTAR_PROPERTY_META* meta = vobsSTAR_PROPERTY_META::GetPropertyMeta(propertyIndex);
        FAIL_NULL_DO(meta,
                     errAdd(vobsERR_INVALID_PROPERTY_ID, propId));

        // note: compDec may be leaking if fatal error below
        compDec = new StarPropertyCompare(propertyIndex, meta, false, compRa);
    }


    if (isPropDEC(propertyId))
    {
        comp = compDec;
    }
    else
    {
        // comp(compDec(compRa)):
        const char *propId = propertyId;

        const mcsINT32 propertyIndex = vobsSTAR::GetPropertyIndex(propId);
        FAIL_COND_DO((propertyIndex == -1),
                     errAdd(vobsERR_INVALID_PROPERTY_ID, propId));

        const vobsSTAR_PROPERTY_META* meta = vobsSTAR_PROPERTY_META::GetPropertyMeta(propertyIndex);
        FAIL_NULL_DO(meta,
                     errAdd(vobsERR_INVALID_PROPERTY_ID, propId));

        comp = new StarPropertyCompare(propertyIndex, meta, IS_TRUE(reverseOrder), compDec);
    }

    // According to "The C++ Programming Language" (Stroustrup p470), yes, stl::list<>::sort is stable.
    _starList.sort(*comp);

    if (comp != compDec)
    {
        delete comp;
    }
    delete compDec;
    delete compRa;

    logInfo("Sort: done.");

    return mcsSUCCESS;
}

/**
 * Display the list contnent on the console.
 */
void vobsSTAR_LIST::Display(void) const
{
    // Display all element of the list
    for (vobsSTAR_PTR_LIST::const_iterator iter = _starList.begin(); iter != _starList.end(); iter++)
    {
        (*iter)->Display();
    }
}

/**
 * Save each star in a VOTable v1.1.
 *
 * @param command server command (SearchCal or GetStar)
 * @param header header of the VO Table
 * @param softwareVersion software version
 * @param request user request
 * @param xmlRequest user request as XML
 * @param votBuffer the buffer in which the VOTable should be written
 * @param trimColumnMode mode to trim empty columns
 * @param log optional server log for that request
 *
 * @return mcsSUCCESS on successful completion, mcsFAILURE otherwise.
 */
mcsCOMPL_STAT vobsSTAR_LIST::GetVOTable(const char* command,
                                        const char* header,
                                        const char* softwareVersion,
                                        const char* request,
                                        const char* xmlRequest,
                                        miscoDYN_BUF* votBuffer,
                                        vobsTRIM_COLUMN_MODE trimColumnMode,
                                        const char *log)
{

    vobsVOTABLE serializer;
    return (serializer.GetVotable(*this, command, NULL, header, softwareVersion, request, xmlRequest, log, trimColumnMode, votBuffer));
}

/**
 * Save each star in a VOTable v1.1.
 *
 * @param command server command (SearchCal or GetStar)
 * @param filename the path to the file in which the VOTable should be saved
 * @param header header of the VO Table
 * @param softwareVersion software version
 * @param request user request
 * @param xmlRequest user request as XML
 * @param trimColumnMode mode to trim empty columns
 * @param log optional server log for that request
 *
 * @return mcsSUCCESS on successful completion, mcsFAILURE otherwise.
 */
mcsCOMPL_STAT vobsSTAR_LIST::SaveToVOTable(const char* command,
                                           const char *filename,
                                           const char *header,
                                           const char *softwareVersion,
                                           const char *request,
                                           const char *xmlRequest,
                                           vobsTRIM_COLUMN_MODE trimColumnMode,
                                           const char *log)
{

    vobsVOTABLE serializer;
    return (serializer.Save(*this, command, filename, header, softwareVersion, request, xmlRequest, log, trimColumnMode));
}

/**
 * Save the stars of the list in a file.
 *
 * @param filename the file where to save
 * @param extendedFormat if true, each property is saved with its attributes
 * (origin and confidence index), otherwise only only property is saved.
 *
 * @return always mcsSUCCESS
 */
mcsCOMPL_STAT vobsSTAR_LIST::Save(const char *fileName,
                                  mcsLOGICAL extendedFormat)
{
    // Store list into the begining
    vobsCDATA cData;

    // use file write blocks:
    FAIL(cData.SaveBufferedToFile(fileName));

    vobsSTAR star;
    FAIL(cData.Store(star, *this, extendedFormat));

    return cData.CloseFile();
}

/**
 * Load stars from a file.
 *
 * @param filename name of file containing star list
 * @param extendedFormat if true, each property is has been saved with its
 * attributes (origin and confidence index), otherwise only only property has
 * been saved.
 * @param origin used if origin is not given in file (see above). If NULL, the
 * name of file is used as origin.
 *
 * @return always mcsSUCCESS
 */
mcsCOMPL_STAT vobsSTAR_LIST::Load(const char* filename,
                                  const vobsCATALOG_META* catalogMeta,
                                  vobsCATALOG_STAR_PROPERTY_CATALOG_MAPPING* propertyCatalogMap,
                                  mcsLOGICAL extendedFormat,
                                  vobsORIGIN_INDEX originIndex)
{
    // Load file
    logInfo("loading %s ...", filename);

    vobsCDATA cData;
    FAIL(cData.LoadFile(filename));

    // Set catalog meta data:
    if (IS_NOT_NULL(catalogMeta))
    {
        cData.SetCatalogMeta(catalogMeta);
    }

    // Set origin (if needed)
    if (IS_FALSE(extendedFormat))
    {
        // if origin is known, set catalog identifier as the catalog in which the data
        // had been got
        if (isCatalog(originIndex))
        {
            cData.SetCatalogId(originIndex);
        }
    }

    // Extract list from the CDATA
    vobsSTAR star;
    FAIL(cData.Extract(star, *this, extendedFormat, propertyCatalogMap));

    return mcsSUCCESS;
}
/*___oOo___*/
