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

/* enable/disable log matching star distance */
#define DO_LOG_STAR_MATCHING false

/* enable/disable log star distance map */
#define DO_LOG_STAR_DIST_MAP false

/* enable/disable log star index */
#define DO_LOG_STAR_INDEX false

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
    if (_starIndex != NULL)
    {
        _starIndex->clear();
        delete(_starIndex);
    }
    if (_sameStarDistMap != NULL)
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

    const unsigned int nbStars = list.Size();
    for (unsigned int el = 0; el < nbStars; el++)
    {
        AddAtTail(*(list.GetNextStar((mcsLOGICAL) (el == 0))));
    }
}

/**
 * Erase (i.e de-allocate) all elements from the list.
 *
 * @return Always mcsSUCCESS.
 */
mcsCOMPL_STAT vobsSTAR_LIST::Clear(void)
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
    SetFreeStarPointers(true);

    // Clear catalog id / meta:
    SetCatalogMeta(vobsNO_CATALOG_ID, NULL);

    return mcsSUCCESS;
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
 * for (unsigned int el = 0; el < starList.Size(); el++)
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
    logTrace("vobsSTAR_LIST::Remove()");

    // Search star in the list
    for (vobsSTAR_PTR_LIST::iterator iter = _starList.begin(); iter != _starList.end(); iter++)
    {
        // If found
        if ((*iter)->IsSame(&star) == mcsTRUE)
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
 * for (unsigned int el = 0; el < starList.Size(); el++)
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
    logTrace("vobsSTAR_LIST::RemoveRef()");

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
 * if (starList.GetStar(star)->View() == NULL)
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

        double starDec;
        NULL_DO(star->GetDec(starDec), logWarning("Invalid Dec coordinate for the given star !"));

        // note: add 1/100 arcsecond for floating point precision:
        vobsSTAR_PTR_MAP::iterator lower = _starIndex->lower_bound(starDec - COORDS_PRECISION);
        vobsSTAR_PTR_MAP::iterator upper = _starIndex->upper_bound(starDec + COORDS_PRECISION);

        // Search star in the star index boundaries:
        for (vobsSTAR_PTR_MAP::iterator iter = lower; iter != upper; iter++)
        {
            if (star->IsSame(iter->second) == mcsTRUE)
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
        if (star->IsSame(*iter) == mcsTRUE)
        {
            return *iter;
        }
    }

    // If nothing found, return NULL pointer
    return NULL;
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
 * int nCriteria = 0;
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
 * if (starList.MatchingCriteria(star, criterias, nCriteria)->View() == NULL)
 * {
 *     printf ("Star not found in list !!");
 * }
 * @endcode
 * 
 * @param star star to compare with
 * @param criterias vobsSTAR_CRITERIA_INFO[] list of comparison criterias 
 *                  given by vobsSTAR_COMP_CRITERIA_LIST.GetCriterias()
 * @param nCriteria number of criteria i.e. size of the vobsSTAR_CRITERIA_INFO array
 * @param useDistMap use distance map to discimminate same stars; only effective when the star index is not used
 *
 * @return pointer to the found element of the list or NULL if element is not
 * found in list.
 */
vobsSTAR* vobsSTAR_LIST::GetStarMatchingCriteria(vobsSTAR* star,
                                                 vobsSTAR_CRITERIA_INFO* criterias, mcsUINT32 nCriteria,
                                                 vobsSTAR_MATCH matcher,
                                                 mcsDOUBLE* separation,
                                                 mcsUINT32* noMatchs)
{
    // Assert criteria are defined:
    if (nCriteria == 0)
    {
        logWarning("GetStarMatchingCriteria: criteria are undefined !");
        return NULL;
    }

    mcsDOUBLE dist = FP_NAN;

    bool useIndex = false;

    if (_starIndexInitialized && (matcher < vobsSTAR_MATCH_DISTANCE_MAP))
    {
        // check criteria
        // note: RA_DEC criteria is always the first one
        useIndex = ((&criterias[0])->propCompType == vobsPROPERTY_COMP_RA_DEC);
    }

    if (useIndex)
    {
        // Use star index
        const bool useTargetId = (matcher == vobsSTAR_MATCH_TARGET_ID);

        // note: RA_DEC criteria is always the first one
        mcsDOUBLE rangeDEC = 0.0;
        mcsDOUBLE starDec = EMPTY_COORD_DEG;

        if (useTargetId)
        {
            // use reference star declination:
            rangeDEC = 0.0;

            NULL_DO(star->GetDecRefStar(starDec), logWarning("Invalid star Ref Dec coordinate for the given star !"));
        }
        else
        {
            rangeDEC = (&criterias[0])->rangeDEC;

            NULL_DO(star->GetDec(starDec), logWarning("Invalid Dec coordinate for the given star !"));
        }

        // note: add +/- COORDS_PRECISION for floating point precision:
        vobsSTAR_PTR_MAP::iterator lower = _starIndex->lower_bound(starDec - rangeDEC - COORDS_PRECISION);
        vobsSTAR_PTR_MAP::iterator upper = _starIndex->upper_bound(starDec + rangeDEC + COORDS_PRECISION);

        // As several stars can be present in the [lower; upper] range,
        // an ordered distance map is used to select the closest star matching criteria:
        if (_sameStarDistMap == NULL)
        {
            // create the distance map allocated until destructor is called:
            _sameStarDistMap = new vobsSTAR_PTR_MAP();
        }
        else
        {
            _sameStarDistMap->clear();
        }

        int nStars = 0;

        // Search star in the star index boundaries:
        for (vobsSTAR_PTR_MAP::iterator iter = lower; iter != upper; iter++)
        {
            // reset distance:
            dist = FP_NAN;

            if (useTargetId)
            {
                // tricky:
                if (iter->second->IsSameRefStar(star) == mcsTRUE)
                {
                    // add candidate in distance map:
                    _sameStarDistMap->insert(vobsSTAR_PTR_PAIR(0.0, iter->second));
                }
            }
            else
            {
                if (star->IsMatchingCriteria(iter->second, criterias, nCriteria, &dist, noMatchs) == mcsTRUE)
                {
                    // add candidate in distance map:
                    _sameStarDistMap->insert(vobsSTAR_PTR_PAIR(dist, iter->second));
                }
            }

            nStars++;
        }

        if (nStars > 0)
        {
            // get the number of stars matching criteria:
            const int mapSize = _sameStarDistMap->size();

            if (mapSize > 0)
            {
                // distance map is not empty

                if ((mapSize > 1) || DO_LOG_STAR_DIST_MAP)
                {
                    logStarIndex("GetStarMatchingCriteria(useIndex)", "sep", _sameStarDistMap, true);
                }

                // Use the first star (sorted by distance):
                vobsSTAR_PTR_MAP::const_iterator iter = _sameStarDistMap->begin();

                if (separation != NULL)
                {
                    *separation = iter->first;
                }

                vobsSTAR* firstStar = iter->second;

                _sameStarDistMap->clear();

                return firstStar;
            }
        }
        // If nothing found, return NULL pointer
        return NULL;
    }

    // vobsSTAR_MATCH_DISTANCE_MAP or vobsSTAR_MATCH_DISTANCE_MAP_PRECESSED modes:

    if ((matcher == vobsSTAR_MATCH_DISTANCE_MAP) || (matcher == vobsSTAR_MATCH_DISTANCE_MAP_PRECESSED))
    {
        // As several stars can be present in the [lower; upper] range,
        // an ordered distance map is used to select the closest star matching criteria:
        if (_sameStarDistMap == NULL)
        {
            // create the distance map allocated until destructor is called:
            _sameStarDistMap = new vobsSTAR_PTR_MAP();
        }
        else
        {
            _sameStarDistMap->clear();
        }

        const bool doPrecessRefStarWithList = (matcher == vobsSTAR_MATCH_DISTANCE_MAP_PRECESSED);

        // star original RA/DEC (degrees):
        mcsDOUBLE ra, dec;
        mcsDOUBLE pmRa, pmDec; // max/yr
        mcsDOUBLE jdDate, epoch = EPOCH_2000;

        if (doPrecessRefStarWithList)
        {
            // given star must be precessed

            // copy original RA/DEC:
            NULL_DO(star->GetRa(ra), logWarning("Failed to get ra !"));
            NULL_DO(star->GetDec(dec), logWarning("Failed to get dec !"));

            // has JD date ?
            jdDate = star->GetJdDate();

            if (jdDate != -1.0)
            {
                // Anyway - clear the observation date property (useless)
                star->GetJdDateProperty()->ClearValue();

                // 2MASS or Denis:
                epoch = EPOCH_2000 + (jdDate - JD_2000) / 365.25;
            }
            else
            {
                logWarning("GetStarMatchingCriteria(useDistMap): bad case : no epoch !");
            }
        }

        // star pointer on star list:
        vobsSTAR* starPtr;

        int nStars = 0;

        // Search star in the complete list (slow)
        for (vobsSTAR_PTR_LIST::iterator iter = _starList.begin(); iter != _starList.end(); iter++)
        {
            // reset distance:
            dist = FP_NAN;

            starPtr = *iter;

            if (doPrecessRefStarWithList)
            {
                // correct star coordinates using each star's proper motion in the list:
                NULL_DO(starPtr->GetPmRa(pmRa), logWarning("Failed to get pmRa !"));
                NULL_DO(starPtr->GetPmDec(pmDec), logWarning("Failed to get pmDec !"));

                // Correct from epoch to 2000 so star are then compared in J2000 / epoch 2000 coordinates
                star->CorrectRaDecToEpoch(pmRa, pmDec, epoch);
            }

            if (star->IsMatchingCriteria(starPtr, criterias, nCriteria, &dist, noMatchs) == mcsTRUE)
            {
                // add candidate in distance map:
                _sameStarDistMap->insert(vobsSTAR_PTR_PAIR(dist, starPtr));
            }

            if (doPrecessRefStarWithList)
            {
                // restore original RA/DEC:
                star->SetRaDec(ra, dec);
            }

            nStars++;
        }

        if (nStars > 0)
        {
            // get the number of stars matching criteria:
            const int mapSize = _sameStarDistMap->size();

            if (mapSize > 0)
            {
                // distance map is not empty
                if ((mapSize > 1) || DO_LOG_STAR_DIST_MAP)
                {
                    logStarIndex("GetStarMatchingCriteria(useDistMap)", "sep", _sameStarDistMap, true);
                }

                // Use the first star (sorted by distance):
                vobsSTAR_PTR_MAP::const_iterator iter = _sameStarDistMap->begin();

                if (separation != NULL)
                {
                    *separation = iter->first;
                }

                vobsSTAR* firstStar = iter->second;

                _sameStarDistMap->clear();

                return firstStar;
            }
        }

        // If nothing found, return NULL pointer
        return NULL;
    }

    // vobsSTAR_MATCH_FIRST_IN_LIST mode:

    // Search star in the complete list (slow)
    for (vobsSTAR_PTR_LIST::iterator iter = _starList.begin(); iter != _starList.end(); iter++)
    {
        if (star->IsMatchingCriteria(*iter, criterias, nCriteria, &dist, noMatchs) == mcsTRUE)
        {
            // return the first star matching criteria

            if (separation != NULL)
            {
                *separation = dist;
            }

            return *iter;
        }
    }

    // If nothing found, return NULL pointer
    return NULL;
}

/**
 * Dump the given star index in logs
 * @param operationName operation name
 * @param keyName key name
 * @param index star index to dump
 * @param isArcSec true to convert key as arcsec
 */
void vobsSTAR_LIST::logStarIndex(const char* operationName, const char* keyName, vobsSTAR_PTR_MAP* index, const bool isArcSec) const
{
    if (index != NULL)
    {
        if (doLog(logTEST))
        {
            logTest("%s: Star index [%d stars]", operationName, index->size());

            int i = 0;
            vobsSTAR* starPtr;
            const char* unit;
            double key;
            mcsSTRING2048 dump;

            for (vobsSTAR_PTR_MAP::const_iterator iter = index->begin(); iter != index->end(); iter++)
            {
                key = iter->first;
                starPtr = iter->second;

                if (isArcSec)
                {
                    key *= alxDEG_IN_ARCSEC;
                    unit = "arcsec";
                }
                else
                {
                    unit = "deg";
                }

                // Get star dump:
                starPtr->Dump(dump, "\t");

                logTest("Star %4d: %s = %.9lf %s : %s", (++i), keyName, key, unit, dump);
            }
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
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is 
 * returned if updating or adding star failed.
 */
mcsCOMPL_STAT vobsSTAR_LIST::Merge(vobsSTAR_LIST &list,
                                   vobsSTAR_COMP_CRITERIA_LIST* criteriaList,
                                   mcsLOGICAL updateOnly)
{
    const unsigned int nbStars = list.Size();

    if (nbStars == 0)
    {
        // nothing to do
        return mcsSUCCESS;
    }

    const bool isLogDebug = doLog(logDEBUG);
    const bool isLogTest = doLog(logTEST);

    // size of this list:
    const unsigned int currentSize = Size();

    const bool hasCriteria = (criteriaList != NULL);

    int nCriteria = 0;
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
        logTest("Merge:  work list [%s] catalog id: '%s'", GetName(), GetCatalogId());
        logTest("Merge: input list [%s] catalog id: '%s'", list.GetName(), list.GetCatalogId());
    }

    const vobsCATALOG_META* thisCatalogMeta = GetCatalogMeta();
    const vobsCATALOG_META* listCatalogMeta = list.GetCatalogMeta();

    // Define overwrite mode:
    vobsOVERWRITE overwrite = vobsOVERWRITE_NONE;
    const vobsSTAR_PROPERTY_MASK* overwritePropertyMask = NULL;
    // flag indicating to overwrite Ra/Dec coordinates:
    bool doOverwriteRaDec = false;

    if ((listCatalogMeta != NULL) && (listCatalogMeta->GetOverwritePropertyMask() != NULL))
    {
        overwrite = vobsOVERWRITE_PARTIAL;
        overwritePropertyMask = listCatalogMeta->GetOverwritePropertyMask();

        doOverwriteRaDec = vobsSTAR::IsRaDecOverwrites(overwritePropertyMask);

        logWarning("Merge: overwrite RA/DEC property: %s", (doOverwriteRaDec) ? "true" : "false");

        if (isLogDebug)
        {
            for (unsigned int i = 0; i < overwritePropertyMask->size(); i++)
            {
                if ((*overwritePropertyMask)[i])
                {
                    logDebug("Merge: overwrite property: %d", i);
                }
            }
        }
    }

    // star pointer on this list:
    vobsSTAR* starPtr;

    // Get the first star of the list (auto correlation):
    vobsSTAR* starFoundPtr;

    mcsDOUBLE starDec, separation = FP_NAN;

    // Prepare the star index on declination property:
    if (_starIndex == NULL)
    {
        // create the star index allocated until destructor is called:
        _starIndex = new vobsSTAR_PTR_MAP();
    }
    else
    {
        _starIndex->clear();
    }
    // star index initialized:
    _starIndexInitialized = true;

    // Add existing stars into the star index:
    if (currentSize > 0)
    {
        for (vobsSTAR_PTR_LIST::iterator iter = _starList.begin(); iter != _starList.end(); iter++)
        {
            starPtr = *iter;

            FAIL(starPtr->GetDec(starDec));

            _starIndex->insert(vobsSTAR_PTR_PAIR(starDec, starPtr));
        }
    }

    if (DO_LOG_STAR_INDEX)
    {
        logStarIndex("Merge", "dec", _starIndex);
    }

    // Get the first star of the list
    starPtr = list.GetNextStar(mcsTRUE);

    const int propLen = starPtr->NbProperties();

    const mcsUINT32 step = nbStars / 10;
    const bool logProgress = nbStars > 2000;

    // stats:
    mcsUINT32 added = 0;
    mcsUINT32 found = 0;
    mcsUINT32 updated = 0;
    mcsUINT32 skipped = 0;
    mcsUINT32 skippedTargetId = 0;
    mcsUINT32 noMatchs[nCriteria];
    mcsUINT32 propertyUpdated[propLen];
    mcsUINT32* noMatchPtr = NULL;
    mcsUINT32* propertyUpdatedPtr = NULL;
    mcsSTRING2048 dump;

    if (isLogTest)
    {
        // For each criteria
        for (int el = 0; el < nCriteria; el++)
        {
            noMatchs[el] = 0;
        }
        noMatchPtr = noMatchs;

        // For each star property index
        for (int el = 0; el < propLen; el++)
        {
            propertyUpdated[el] = 0;
        }
        propertyUpdatedPtr = propertyUpdated;
    }

    // Check that we are in secondary request cases:
    // note: hasCriteria to be sure ..
    if ((currentSize > 0) && (updateOnly == mcsTRUE) && hasCriteria)
    {
        logTest("Merge: crossmatch [CLOSEST_REF_STAR]");

        // Define algorithm options:
        const bool useAllMatchingStars = (listCatalogMeta != NULL) ? (listCatalogMeta->HasMultipleRows() == mcsTRUE) : false;

        // given list coordinates needs precession using reference star's proper motion:
        const bool doPrecessListWithRefStar = (listCatalogMeta != NULL) ? (listCatalogMeta->DoPrecessEpoch() == mcsTRUE) : false;

        // reference star coordinates needs precession using star list's proper motion (best):
        const bool doPrecessRefStarWithList = (thisCatalogMeta != NULL) ? (thisCatalogMeta->DoPrecessEpoch() == mcsTRUE) : mcsFALSE;

        const mcsDOUBLE listEpoch = (doPrecessListWithRefStar) ? (listCatalogMeta->GetEpochMedian()) : EPOCH_2000;

        if (isLogTest)
        {
            if (useAllMatchingStars)
            {
                logTest("Merge: useAllMatchingStars: true");
            }
            const vobsCATALOG_META* catalogMeta = NULL;
            if (doPrecessListWithRefStar)
            {
                catalogMeta = listCatalogMeta;
                logTest("Merge: precess candidate stars with reference star");
            }
            if (doPrecessRefStarWithList)
            {
                catalogMeta = thisCatalogMeta;
                logTest("Merge: precess reference star with candidate stars");
            }
            if (catalogMeta != NULL)
            {
                if (catalogMeta->IsSingleEpoch())
                {
                    logTest("Merge: precess to epoch %.3lf", listEpoch);
                }
                else
                {
                    logTest("Merge: precess using JD in epoch range [%.3lf %.3lf]", catalogMeta->GetEpochFrom(), catalogMeta->GetEpochTo());
                }
            }
        }

        // Secondary requests = Partial CROSS MATCH: use the reference star identifiers
        // For each given star, the query returns 1..n stars (cone search) having all the same reference star identifier
        // These stars must be processed as one sub list (correct epoch / coordinates ...) and find which star is really
        // corresponding to the initially requested star

        // Create a temporary list of star having the same reference star identifier
        vobsSTAR_LIST subList("SubListMerge");

        // note: sub list has no star index created => disabled !!

        unsigned int nbSubStars;

        // star pointer on sub list:
        vobsSTAR* subStarPtr;

        // define the free pointer flag to avoid double frees (this list and the given list are storing same star pointers):
        subList.SetFreeStarPointers(false);

        vobsSTAR_PROPERTY* targetIdProperty;
        const char* targetId = NULL;
        const char* lastTargetId = NULL;
        bool isLast = false;
        bool isSameId = false;
        bool processList = false;
        bool match = false;

        mcsDOUBLE pmRa, pmDec; // max/yr
        mcsDOUBLE jdDate, epoch;

        // For each star of the given list
        for (unsigned int el = 0; el <= nbStars; el++)
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
                targetIdProperty = starPtr->GetTargetIdProperty();
                targetId = starPtr->GetPropertyValue(targetIdProperty);

                // Is the same target Id ?
                isSameId = (lastTargetId != NULL) ? (strcmp(lastTargetId, targetId) == 0) : true;

                // update last target id:
                lastTargetId = targetId;

                if (isSameId)
                {
                    // same target id, add this star to the sub list:
                    subList.AddRefAtTail(starPtr);
                }
            }

            processList = isLast || !isSameId;

            if (processList)
            {
                nbSubStars = subList.Size();

                if (isLogTest && (nbSubStars > 1))
                {
                    logTest("process subList size = %d (same targetId)", nbSubStars);
                }

                if (nbSubStars > 0)
                {
                    // Get first star in the sub list to extract the reference star:
                    subStarPtr = subList.GetNextStar(mcsTRUE);

                    // Extract star identifier:
                    targetIdProperty = subStarPtr->GetTargetIdProperty();
                    targetId = subStarPtr->GetPropertyValue(targetIdProperty);

                    // Get reference star using only raRef/decRef (only Ra/Dec criteria):
                    // note: use star index and targetId:
                    starFoundPtr = GetStarMatchingCriteria(subStarPtr, criterias, 1, vobsSTAR_MATCH_TARGET_ID); // 1 means Ra/Dec criteria only !!

                    // If star is in the list ?
                    if (starFoundPtr == NULL)
                    {
                        // BIG problem: reference star not found => skip it
                        logWarning("Reference star NOT found for targetId '%s'", targetId);

                        skippedTargetId++;
                        skipped += nbSubStars;
                    }
                    else
                    {
                        if (isLogDebug)
                        {
                            // Get star dump:
                            starFoundPtr->Dump(dump, "\t");

                            logDebug("Reference star found for targetId '%s' : %s", targetId, dump);
                        }

                        // correct coordinates using the reference star on the complete sub list:
                        if (doPrecessListWithRefStar)
                        {
                            FAIL(starFoundPtr->GetPmRa(pmRa));
                            FAIL(starFoundPtr->GetPmDec(pmDec));

                            for (vobsSTAR_PTR_LIST::iterator iter = subList._starList.begin(); iter != subList._starList.end(); iter++)
                            {
                                subStarPtr = *iter;

                                jdDate = subStarPtr->GetJdDate();

                                if (jdDate != -1.0)
                                {
                                    // Anyway - clear the observation date property (useless)
                                    subStarPtr->GetJdDateProperty()->ClearValue();

                                    // 2MASS or Denis:
                                    epoch = EPOCH_2000 + (jdDate - JD_2000) / 365.25;
                                }
                                else
                                {
                                    // ASCC (OK) / AKARI (~OK but no date so may be out of cone search radius):
                                    epoch = listEpoch;
                                }

                                subStarPtr->CorrectRaDecToEpoch(pmRa, pmDec, epoch);
                            }

                            subStarPtr = NULL;
                        }

                        if (useAllMatchingStars)
                        {
                            match = false;

                            // Update the reference star using all matching star:
                            // it is imperative for catalogs giving multiple results ie stars (photometry II/225)
                            for (vobsSTAR_PTR_LIST::iterator iter = subList._starList.begin(); iter != subList._starList.end(); iter++)
                            {
                                subStarPtr = *iter;

                                if (starFoundPtr->IsMatchingCriteria(subStarPtr, criterias, nCriteria, NULL, noMatchPtr) == mcsTRUE)
                                {
                                    match = true;

                                    // Anyway - clear the target identifier property (useless)
                                    subStarPtr->GetTargetIdProperty()->ClearValue();

                                    // Update the reference star
                                    if (starFoundPtr->Update(*subStarPtr, overwrite, overwritePropertyMask, propertyUpdatedPtr) == mcsTRUE)
                                    {
                                        updated++;
                                    }
                                    found++;
                                }
                                else
                                {
                                    skipped++;
                                }
                            }
                            if (!match)
                            {
                                skippedTargetId++;
                            }
                        }
                        else
                        {
                            // Find in the sub list the star matching criteria with the reference star:
                            separation = FP_NAN;

                            // note: the sub list does not use star index but use distance map to discrimminate same stars:
                            subStarPtr = subList.GetStarMatchingCriteria(starFoundPtr, criterias, nCriteria,
                                                                         (doPrecessRefStarWithList) ?
                                                                         vobsSTAR_MATCH_DISTANCE_MAP_PRECESSED : vobsSTAR_MATCH_DISTANCE_MAP,
                                                                         &separation, noMatchPtr);

                            // TODO: handle duplicates here: multiple stars matching criteria and same distance:
                            // mainly WDS but it happens !
                            /*
                             catalog II/7A: use best row (scoring to be defined) or all ? useAllMatchingStars as it only has 5500 entries !
                            sclsvrServer -   vobs - Test  - process subList size = 2 (same targetId)
                            sclsvrServer -   vobs - Test  - Star index [2 stars]
                            sclsvrServer -   vobs - Test  - Star    1: key = 0.000161585, star = 'Coordinates-ra=03 47 29.1/dec=+24 06 18'
                            sclsvrServer -   vobs - Test  - Star    2: key = 0.000161585, star = 'Coordinates-ra=03 47 29.1/dec=+24 06 18'
                             */

                            skipped += (nbSubStars - 1);

                            if (subStarPtr != NULL)
                            {
                                found++;

                                if (DO_LOG_STAR_MATCHING)
                                {
                                    // Get star dump:
                                    subStarPtr->Dump(dump, "\t");

                                    logTest("Matching star found for targetId '%s': sep = %.9lf arcsec : %s", targetId, separation * alxDEG_IN_ARCSEC, dump);
                                }

                                if (doPrecessRefStarWithList || doOverwriteRaDec)
                                {
                                    // Finally clear the reference star coordinates to be overriden next:
                                    // Note: this can make the star index corrupted !!
                                    starFoundPtr->ClearRaDec();
                                }

                                // Anyway - clear the target identifier property (useless)
                                subStarPtr->GetTargetIdProperty()->ClearValue();

                                // Update the reference star
                                if (starFoundPtr->Update(*subStarPtr, overwrite, overwritePropertyMask, propertyUpdatedPtr) == mcsTRUE)
                                {
                                    updated++;
                                }
                            }
                            else
                            {
                                skippedTargetId++;
                                skipped++;

                                if (isLogTest)
                                {
                                    mcsDOUBLE raRef, decRef, ra, dec, dist;
                                    int i = 0;

                                    logTest("No star matching all criteria for targetId '%s':", targetId);

                                    FAIL(starFoundPtr->GetRa(raRef));
                                    FAIL(starFoundPtr->GetDec(decRef));

                                    // Get ref star dump:
                                    starFoundPtr->Dump(dump, "\t");

                                    logTest("Reference star : %s", dump);

                                    logTest("Candidates:");

                                    for (vobsSTAR_PTR_LIST::iterator iter = subList._starList.begin(); iter != subList._starList.end(); iter++)
                                    {
                                        subStarPtr = *iter;

                                        FAIL(subStarPtr->GetRa(ra));
                                        FAIL(subStarPtr->GetDec(dec));

                                        FAIL(alxComputeDistance(raRef, decRef, ra, dec, &dist));

                                        // Get star dump:
                                        subStarPtr->Dump(dump, "\t");

                                        logTest("Star %4d: sep = %.9lf arcsec : %s", (++i), dist, dump);
                                    }
                                }
                            }
                        }
                    }
                }
            }

            if (!isSameId)
            {
                // Clear sub list:
                subList.Clear();

                // define the free pointer flag to avoid double frees (this list and the given list are storing same star pointers):
                subList.SetFreeStarPointers(false);

                subList.AddRefAtTail(starPtr);
            }
        }

        // clear sub list anyway:
        subList.Clear();
    }
    else
    {
        logTest("Merge: crossmatch [CLOSEST_ALL_STARS]");

        // Primary requests = Generic CROSS MATCH:
        // TODO: enhance this cross match to take into account that the given list 
        // can have several stars matching criteria per star in the current list 
        // i.e. do not always use the first star present in the given list !

        // For each star of the given list
        for (unsigned int el = 0; el < nbStars; el++)
        {
            if (isLogTest && logProgress && (el % step == 0))
            {
                logTest("Merge: merged stars = %d", el);
            }

            starPtr = list.GetNextStar((mcsLOGICAL) (el == 0));

            // Anyway - clear the target identifier property (useless) to not use it:
            starPtr->GetTargetIdProperty()->ClearValue();

            // If star is in the list ?
            if (hasCriteria)
            {
                separation = FP_NAN;

                // note: use star index and compute distance map:
                starFoundPtr = GetStarMatchingCriteria(starPtr, criterias, nCriteria, vobsSTAR_MATCH_INDEX, &separation, noMatchPtr);
            }
            else
            {
                starFoundPtr = GetStar(starPtr);
            }

            if (starFoundPtr != NULL)
            {

                if (DO_LOG_STAR_MATCHING)
                {
                    // Get star dump:
                    starFoundPtr->Dump(dump, "\t");

                    logTest("Matching star found: sep = %.9lf arcsec : %s", separation * alxDEG_IN_ARCSEC, dump);
                }

                found++;

                // Update the star
                if (starFoundPtr->Update(*starPtr, overwrite, overwritePropertyMask, propertyUpdatedPtr) == mcsTRUE)
                {
                    updated++;
                }
            }
            else if (updateOnly == mcsFALSE)
            {
                // Else add it to the list
                AddAtTail(*starPtr);

                // Add new star in the star index:
                FAIL(starPtr->GetDec(starDec));

                // add it also to the star index:
                _starIndex->insert(vobsSTAR_PTR_PAIR(starDec, starPtr));

                added++;
            }
            else
            {
                skipped++;
            }
        }
    }

    if (DO_LOG_STAR_INDEX)
    {
        logStarIndex("Merge", "dec", _starIndex);
    }

    // clear star index uninitialized:
    _starIndex->clear();
    _starIndexInitialized = false;

    // Update catalog id / meta:
    if ((thisCatalogMeta == NULL) && (strcmp(GetCatalogId(), vobsNO_CATALOG_ID) == 0))
    {
        SetCatalogMeta(list.GetCatalogId(), list.GetCatalogMeta());
    }
    else if (strcmp(GetCatalogId(), list.GetCatalogId()) != 0)
    {
        SetCatalogMeta(vobsMIXED_CATALOG_ID, NULL);
    }

    if (isLogTest)
    {
        logTest("Merge:  work list [%s] catalog id: '%s'", GetName(), GetCatalogId());
    }

    if (isLogTest)
    {
        logTest("Merge: done = %d stars added / %d found / %d updated / %d skipped (%d skipped targetId).",
                added, found, updated, skipped, skippedTargetId);

        if (skipped > 0)
        {
            vobsSTAR_CRITERIA_INFO* criteria = NULL;

            // For each criteria
            for (int el = 0; el < nCriteria; el++)
            {
                criteria = &criterias[el];

                logTest("Merge: Criteria '%s' mismatch %d times", criteria->propertyId, noMatchs[el]);
            }
        }
        if (updated > 0)
        {
            const vobsSTAR_PROPERTY_META* meta;
            mcsUINT32 propUpdateCount;

            // For each star property index
            for (int idx = 0; idx < propLen; idx++)
            {
                propUpdateCount = propertyUpdated[idx];

                if (propUpdateCount > 0)
                {
                    meta = vobsSTAR::GetPropertyMeta(idx);
                    if (meta != NULL)
                    {
                        logTest("Merge: Property '%s' [%s] updated %d times", meta->GetName(), meta->GetId(), propUpdateCount);
                    }
                }
            }
        }
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
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is 
 * returned
 */
mcsCOMPL_STAT vobsSTAR_LIST::FilterDuplicates(vobsSTAR_LIST &list,
                                              vobsSTAR_COMP_CRITERIA_LIST* criteriaList,
                                              bool doRemove)
{
    const bool isLogTest = doLog(logTEST);

    const unsigned int nbStars = list.Size();

    if (nbStars == 0)
    {
        // nothing to do
        return mcsSUCCESS;
    }

    // Anyway: clear this list:
    Clear();

    // define the free pointer flag to avoid double frees (this list and list are storing same star pointers):
    SetFreeStarPointers(false);

    // list of unique duplicated star pointers (too close):
    vobsSTAR_PTR_SET duplicates;

    const bool hasCriteria = (criteriaList != NULL);

    int nCriteria = 0;
    vobsSTAR_CRITERIA_INFO* criterias = NULL;

    // TODO: decide which separation should be used (2", 5" or 10") depends on catalog or scenario (bright, faint, prima catalog ...)???
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

                // set it to 10 arcsec:
                criteria->rangeRA = (mcsDOUBLE) (10.0 * alxARCSEC_IN_DEGREES);

                logTest("FilterDuplicates: filter search radius = %0.1lf arcsec", criteria->rangeRA * alxDEG_IN_ARCSEC);
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
    if (_starIndex == NULL)
    {
        // create the star index allocated until destructor is called:
        _starIndex = new vobsSTAR_PTR_MAP();
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

    mcsDOUBLE starDec, separation;

    const mcsUINT32 step = nbStars / 10;
    const bool logProgress = nbStars > 2000;

    // stats:
    mcsUINT32 added = 0;
    mcsUINT32 found = 0;
    mcsUINT32 different = 0;

    // For each star of the given list
    for (unsigned int el = 0; el < nbStars; el++)
    {
        if (isLogTest && logProgress && (el % step == 0))
        {
            logTest("FilterDuplicates: filtered stars = %d", el);
        }

        starPtr = list.GetNextStar((mcsLOGICAL) (el == 0));

        // If star is in the list ?

        if (hasCriteria)
        {
            // note: use star index and compute distance map:
            starFoundPtr = GetStarMatchingCriteria(starPtr, criterias, nCriteria, vobsSTAR_MATCH_INDEX, &separation);
        }
        else
        {
            starFoundPtr = GetStar(starPtr);
        }

        if (starFoundPtr != NULL)
        {
            // this list has already one star matching criteria = duplicated stars
            found++;

            if (starFoundPtr->compare(*starPtr) != 0)
            {
                logWarning("FilterDuplicates: separation = %.9lf arcsec", separation * alxDEG_IN_ARCSEC);

                // TODO: stars are different: do something i.e. reject both / keep one but which one ...
                different++;
            }

            // Put both stars in duplicate set (unique pointers):
            duplicates.insert(starFoundPtr);
            duplicates.insert(starPtr);
        }
        else
        {
            // Else add it to the list
            AddRefAtTail(starPtr);

            // Add new star in the star index:
            FAIL(starPtr->GetDec(starDec));

            // add it also to the star index:
            _starIndex->insert(vobsSTAR_PTR_PAIR(starDec, starPtr));

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
        logTest("FilterDuplicates: done = %d unique stars / %d duplicates found : %d different stars.",
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
                FAIL(starFoundPtr->GetRa(ra));
                FAIL(starFoundPtr->GetDec(dec));

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
    int _propertyIndex;
    const vobsSTAR_PROPERTY_META* _meta;
    bool _reverseOrder;

    // members:
    const char* _propertyId;
    vobsPROPERTY_TYPE _propertyType;
    bool _isRA;
    bool _isDEC;

public:
    // Constructor

    StarPropertyCompare(const int propertyIndex, const vobsSTAR_PROPERTY_META* meta, const bool reverseOrder)
    {

        _propertyIndex = propertyIndex;
        _meta = meta;
        _reverseOrder = reverseOrder;

        _propertyId = meta->GetId();
        _propertyType = _meta->GetType();

        // is RA or DEC:
        _isRA = strcmp(_propertyId, vobsSTAR_POS_EQ_RA_MAIN) == 0;
        _isDEC = strcmp(_propertyId, vobsSTAR_POS_EQ_DEC_MAIN) == 0;

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
        const mcsLOGICAL isValue1Set = leftStar ->IsPropertySet(leftProperty);
        const mcsLOGICAL isValue2Set = rightStar->IsPropertySet(rightProperty);

        // If one of the properties is not set, move it at the begining
        // or at the end, according to the sorting order
        if ((isValue1Set == mcsFALSE) || (isValue2Set == mcsFALSE))
        {
            // If it is normal sorting order
            if (!_reverseOrder)
            {
                // blank values are at the end:
                // If value of next element is not set while previous
                // one is, swap them
                return ((isValue2Set == mcsFALSE) && (isValue1Set == mcsTRUE));
            }
            else
            {
                // Else (reverse sorting order)
                // blanks values are at the beginning:
                // If value of previous element is not set while next
                // one is, swap them
                return ((isValue1Set == mcsFALSE) && (isValue2Set == mcsTRUE));
            }
        }
        else
        {
            // Else (properties are set)
            // Compare element values according to property or property
            // type, and check if elements have to be swapped according
            // to the sorting order

            if ((_propertyType == vobsFLOAT_PROPERTY) || _isRA || _isDEC)
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

                if (!_reverseOrder)
                {
                    return value1 < value2;
                }
                else
                {
                    return value2 < value1;
                }
            }
            else
            {
                const char* value1 = leftStar ->GetPropertyValue(leftProperty);
                const char* value2 = rightStar->GetPropertyValue(rightProperty);

                if (!_reverseOrder)
                {
                    return strcmp(value1, value2) < 0;
                }
                else
                {
                    return strcmp(value1, value2) > 0;
                }
            }
        }
    }
};

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
    logInfo("Sort: start");

    // If list is empty or contains only one element, return
    if (Size() <= 1)
    {
        return mcsSUCCESS;
    }

    // Get property index:
    const int propertyIndex = vobsSTAR::GetPropertyIndex(propertyId);
    FAIL_COND_DO(propertyIndex == -1, errAdd(vobsERR_INVALID_PROPERTY_ID, propertyId));

    // Get property meta:
    const vobsSTAR_PROPERTY_META* meta = vobsSTAR::GetPropertyMeta(propertyIndex);
    FAIL_NULL_DO(meta, errAdd(vobsERR_INVALID_PROPERTY_ID, propertyId));

    StarPropertyCompare comp(propertyIndex, meta, reverseOrder == mcsTRUE);

    _starList.sort(comp);

    logInfo("Sort: done.");

    return mcsSUCCESS;
}

/**
 * Display the list contnent on the console.
 */
void vobsSTAR_LIST::Display(void) const
{
    logTrace("vobsSTAR_LIST::Display()");

    // Display all element of the list 
    for (vobsSTAR_PTR_LIST::const_iterator iter = _starList.begin(); iter != _starList.end(); iter++)
    {
        (*iter)->Display();
    }
}

/**
 * Save each star in a VOTable v1.1.
 *
 * @param header header of the VO Table
 * @param softwareVersion software version
 * @param request user request
 * @param xmlRequest user request as XML
 * @param buffer the buffer in which the VOTable should be written
 *
 * @return mcsSUCCESS on successful completion, mcsFAILURE otherwise. 
 */
mcsCOMPL_STAT vobsSTAR_LIST::GetVOTable(const char* header,
                                        const char* softwareVersion,
                                        const char* request,
                                        const char* xmlRequest,
                                        miscoDYN_BUF* buffer)
{
    logTrace("vobsSTAR_LIST::GetVOTable()");

    vobsVOTABLE serializer;
    return (serializer.GetVotable(*this, NULL, header, softwareVersion, request, xmlRequest, buffer));
}

/**
 * Save each star in a VOTable v1.1.
 *
 * @param filename the path to the file in which the VOTable should be saved
 * @param header header of the VO Table
 * @param softwareVersion software version
 * @param request user request
 * @param xmlRequest user request as XML
 *
 * @return mcsSUCCESS on successful completion, mcsFAILURE otherwise. 
 */
mcsCOMPL_STAT vobsSTAR_LIST::SaveToVOTable(const char *filename,
                                           const char *header,
                                           const char *softwareVersion,
                                           const char *request,
                                           const char *xmlRequest)
{
    logTrace("vobsSTAR_LIST::SaveToVOTable()");

    vobsVOTABLE serializer;
    return (serializer.Save(*this, filename, header, softwareVersion, request, xmlRequest));
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
mcsCOMPL_STAT vobsSTAR_LIST::Save(const char *filename,
                                  mcsLOGICAL extendedFormat)
{
    logTrace("vobsSTAR_LIST::Save()");

    vobsSTAR_PROPERTY_ID_LIST ucdList;

    return Save(filename, ucdList, extendedFormat);
}

/**
 * Save the stars of the list in a file.
 *
 * @param filename the file where to save
 * @param ucdList list of ucd to save
 * @param extendedFormat if true, each property is saved with its attributes
 * (origin and confidence index), otherwise only only property is saved.
 *
 * @return always mcsSUCCESS
 */
mcsCOMPL_STAT vobsSTAR_LIST::Save(const char *filename,
                                  vobsSTAR_PROPERTY_ID_LIST ucdList,
                                  mcsLOGICAL extendedFormat)
{
    logTrace("vobsSTAR_LIST::Save()");

    // Store list into the begining
    vobsCDATA cData;
    vobsSTAR star;
    FAIL(cData.Store(star, *this, ucdList, extendedFormat));

    // Save into file
    FAIL(cData.SaveInFile(filename));

    return mcsSUCCESS;
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
                                  vobsCATALOG_STAR_PROPERTY_CATALOG_MAPPING* propertyCatalogMap,
                                  mcsLOGICAL extendedFormat,
                                  const char* origin)
{
    logTrace("vobsSTAR_LIST::Load()");

    // Load file
    vobsCDATA cData;
    FAIL(cData.LoadFile(filename));

    // Set origin (if needed)
    if (extendedFormat == mcsFALSE)
    {
        // if origin is unknown, set catalog name as the file in which the data
        // had been got
        if (origin == NULL)
        {
            FAIL(cData.SetCatalogName(filename));
        }
        else
        {
            // if origin is known, set catalog name as the catalog in which the data
            // had been got
            FAIL(cData.SetCatalogName(origin));
        }
    }

    // Extract list from the CDATA
    vobsSTAR star;
    FAIL(cData.Extract(star, *this, extendedFormat, propertyCatalogMap));

    return mcsSUCCESS;
}
/*___oOo___*/
