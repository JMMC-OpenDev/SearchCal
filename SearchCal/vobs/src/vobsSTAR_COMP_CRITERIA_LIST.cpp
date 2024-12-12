/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * @file
 * vobsSTAR_COMP_CRITERIA_LIST class definition.
 */

/*
 * System Headers
 */
#include <iostream>
#include <string.h>
using namespace std;
#include <map>

/*
 * MCS Headers
 */
#include "mcs.h"
#include "log.h"
#include "err.h"

/*
 * Local Headers
 */
#include "vobsSTAR_COMP_CRITERIA_LIST.h"
#include "vobsSTAR.h"
#include "vobsPrivate.h"
#include "vobsErrors.h"

/**
 * Class constructor
 */
vobsSTAR_COMP_CRITERIA_LIST::vobsSTAR_COMP_CRITERIA_LIST()
{
    // Ensure Internal members are undefined:
    _initialized = false;
    _criteriaInfos = NULL;
}

/**
 * Copy Constructor
 */
vobsSTAR_COMP_CRITERIA_LIST::vobsSTAR_COMP_CRITERIA_LIST(const vobsSTAR_COMP_CRITERIA_LIST& criteriaList)
{
    // Ensure Internal members are undefined:
    _initialized = false;
    _criteriaInfos = NULL;

    // Uses the operator=() method to copy
    *this = criteriaList;
}

/**
 * Class destructor
 */
vobsSTAR_COMP_CRITERIA_LIST::~vobsSTAR_COMP_CRITERIA_LIST()
{
    // note: criteria list contains char* keys but these are in fact literals

    // Clear the internal map
    Clear();
}

/**
 * Assignment operator
 */
vobsSTAR_COMP_CRITERIA_LIST&vobsSTAR_COMP_CRITERIA_LIST::operator=(const vobsSTAR_COMP_CRITERIA_LIST& criteriaList)
{
    if (this != &criteriaList)
    {
        // Clear the internal map
        Clear();

        // Copy in the criteria list from the given criteriaList
        _criteriaList = criteriaList._criteriaList;
    }
    return *this;
}

/*
 * Public methods
 */

/**
 * Clear the criteria list
 *
 * @return always mcsSUCCESS
 */
mcsCOMPL_STAT vobsSTAR_COMP_CRITERIA_LIST::Clear()
{
    _criteriaList.clear();

    // Ensure Internal members are undefined:
    resetCriterias();

    return mcsSUCCESS;
}

/**
 * Add a criteria in the list
 *
 * @param propertyId the identifier of property.
 * @param range the range value of the criteria used when comparing double
 * floating property. It is ignored when comparing string.
 *
 * @return
 * Always mcsSUCCESS.
 */
mcsCOMPL_STAT vobsSTAR_COMP_CRITERIA_LIST::Add(const char* propertyId,
                                               mcsDOUBLE range)
{
    // check if that property is known
    if (!isCompGaiaMags(propertyId))
    {
        // If criteria is not a property return failure
        mcsINT32 propertyIndex = vobsSTAR::GetPropertyIndex(propertyId);
        FAIL_COND_DO(propertyIndex == -1, errAdd(vobsERR_INVALID_PROPERTY_ID, propertyId));
    }
    // Put criteria in the list
    _criteriaList[propertyId] = range;

    // Ensure Internal members are undefined:
    resetCriterias();

    return mcsSUCCESS;
}

/**
 * Method to remove a criteria from the list of criteria
 *
 * @param propertyId the id of the criteria to removed
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
 */
mcsCOMPL_STAT vobsSTAR_COMP_CRITERIA_LIST::Remove(const char* propertyId)
{
    // check if that property is known
    // If criteria is not a property return failure
    mcsINT32 propertyIndex = vobsSTAR::GetPropertyIndex(propertyId);
    FAIL_COND_DO(propertyIndex == -1, errAdd(vobsERR_INVALID_PROPERTY_ID, propertyId));

    // Remove criteria from the list
    _criteriaList.erase(propertyId);

    // Ensure Internal members are undefined:
    resetCriterias();

    return mcsSUCCESS;
}

/**
 * Get the size of the list
 *
 * Note: this function uses the map::size()
 *
 * @return the number of element in the list
 **/
mcsINT32 vobsSTAR_COMP_CRITERIA_LIST::Size()
{
    return _criteriaList.size();
}

/**
 * Show criteria in logs:
 * TODO: use vobsSTAR_CRITERIA_INFO instead of _criteriaList
 */
void vobsSTAR_COMP_CRITERIA_LIST::log(logLEVEL level, const char* prefix)
{
    if (doLog(level))
    {
        const char* propertyId;
        mcsDOUBLE range;

        mcsINT32 i = 0;
        for (vobsSTAR_CRITERIA_MAP::iterator iter = _criteriaList.begin(); iter != _criteriaList.end(); iter++)
        {
            propertyId = iter->first;
            range = iter->second;

            logP(level, "%sCriteria %d on property[%s] with range = %.9lf", prefix, (++i), propertyId, range);
        }
    }
}

/**
 * Prepare criteria traversal (lazily initialized)
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
 */
mcsCOMPL_STAT vobsSTAR_COMP_CRITERIA_LIST::InitializeCriterias()
{
    if (_initialized)
    {
        return mcsSUCCESS;
    }

    // Reset Internal members:
    resetCriterias();

    // cache list size:
    _size = _criteriaList.size();

    const bool isLogDebug = doLog(logDEBUG);

    // Create criteria informations:
    _criteriaInfos = new vobsSTAR_CRITERIA_INFO[_size];

    // Fill criteria information:
    vobsSTAR_CRITERIA_INFO* criteria = NULL;

    const char* propertyId;
    mcsDOUBLE range;
    mcsINT32 propertyIndex;
    mcsINT32 otherPropertyIndex;
    const vobsSTAR_PROPERTY_META* meta = NULL;

    /*
     * Notes:
     * - criterias RA and DEC are merged into one single vobsSTAR_CRITERIA_INFO to perform accurate separation computation
     * - criteria RA  is always at position 0 in _criteriaList
     * - criteria DEC is always at position 1 in _criteriaList
     */

    // TODO: check that RA and DEC criteria are always defined

    mcsINT32 i = 0; // correct size

    for (vobsSTAR_CRITERIA_MAP::iterator iter = _criteriaList.begin(); iter != _criteriaList.end(); iter++)
    {
        propertyId = iter->first;
        range = iter->second;

        criteria = &_criteriaInfos[i];

        if (isCompGaiaMags(propertyId))
        {
            // GAIA only (hard-coded):
            criteria->propCompType = vobsPROPERTY_COMP_GAIA_MAGS;
            criteria->propertyId = vobsSTAR_PHOT_JHN_V;
            criteria->otherPropertyId = vobsSTAR_PHOT_GAIA_V;
            // fix:
            propertyId = criteria->propertyId;
        }
        else
        {
            // Ensure propCompType is always set (not vobsPROPERTY_COMP_GAIA_MAGS)
            criteria->propCompType = vobsPROPERTY_COMP_FLOAT;
            criteria->propertyId = propertyId;
            criteria->otherPropertyId = propertyId;
        }

        // Get property index:
        propertyIndex = vobsSTAR::GetPropertyIndex(propertyId);
        if (propertyIndex == -1)
        {
            errAdd(vobsERR_INVALID_PROPERTY_ID, propertyId);

            // Ensure Internal members are undefined:
            resetCriterias();

            return mcsFAILURE;
        }

        // Get property meta:
        meta = vobsSTAR_PROPERTY_META::GetPropertyMeta(propertyIndex);
        if (IS_NULL(meta))
        {
            errAdd(vobsERR_INVALID_PROPERTY_ID, propertyId);

            // Ensure Internal members are undefined:
            resetCriterias();

            return mcsFAILURE;
        }

        // is RA or DEC:
        if (isPropRA(propertyId))
        {
            criteria->propertyIndex = -1; // undefined and useless
            criteria->otherPropertyIndex = -1; // undefined and useless
            criteria->propCompType = vobsPROPERTY_COMP_RA_DEC;
            criteria->propertyId = "RA/DEC"; // undefined and useless            
            criteria->otherPropertyId = "RA/DEC"; // undefined and useless

            criteria->range = NAN;
            criteria->rangeRA = range;
            criteria->rangeDEC = NAN;

            // box or circular area ?
            criteria->isRadius = false;

            // note: i is not incremented to define DEC next
        }
        else if (isPropDEC(propertyId))
        {
            criteria->propertyIndex = -1; // undefined and useless
            criteria->otherPropertyIndex = -1; // undefined and useless
            criteria->propCompType = vobsPROPERTY_COMP_RA_DEC;
            criteria->propertyId = "RA/DEC"; // undefined and useless                  
            criteria->otherPropertyId = "RA/DEC"; // undefined and useless
            criteria->rangeDEC = range;

            // box or circular area ?
            criteria->isRadius = (criteria->rangeRA == criteria->rangeDEC);
            i++;
        }
        else
        {
            criteria->propertyIndex = propertyIndex;
            criteria->range = range;

            if (criteria->otherPropertyId != propertyId)
            {
                // Get other property index:
                otherPropertyIndex = vobsSTAR::GetPropertyIndex(criteria->otherPropertyId);
                if (otherPropertyIndex == -1)
                {
                    errAdd(vobsERR_INVALID_PROPERTY_ID, criteria->otherPropertyId);

                    // Ensure Internal members are undefined:
                    resetCriterias();

                    return mcsFAILURE;
                }
                criteria->otherPropertyIndex = otherPropertyIndex;

                // TODO: check type is FLOAT (only supported ?)
            }
            else
            {
                criteria->otherPropertyIndex = propertyIndex;
            }

            if (criteria->propCompType != vobsPROPERTY_COMP_GAIA_MAGS)
            {
                switch (meta->GetType())
                {
                    case vobsSTRING_PROPERTY:
                        criteria->propCompType = vobsPROPERTY_COMP_STRING;
                        break;
                    default:
                    case vobsFLOAT_PROPERTY:
                    case vobsINT_PROPERTY:
                    case vobsBOOL_PROPERTY:
                        criteria->propCompType = vobsPROPERTY_COMP_FLOAT;
                }
            }
            i++;
        }
    }

    // fix size:
    _size = i;
    _initialized = true;

    if (isLogDebug)
    {
        for (i = 0; i < _size; i++)
        {
            criteria = &_criteriaInfos[i];

            if (criteria->propCompType == vobsPROPERTY_COMP_RA_DEC)
            {
                // ra/dec criteria
                logDebug("InitializeCriterias: criteria %d on RA/DEC using %s area", i + 1, criteria->isRadius ? "CIRCULAR" : "BOX");
                logDebug("InitializeCriterias: range RA / DEC = %.9lf / %.9lf", criteria->rangeRA, criteria->rangeDEC);
            }
            else
            {
                // other criteria
                logDebug("InitializeCriterias: criteria %d on Property [%d : '%s'] vs <> [%d : '%s']", i + 1, criteria->propertyIndex,
                         criteria->propertyId, criteria->otherPropertyIndex, criteria->otherPropertyId);

                if (criteria->propCompType == vobsPROPERTY_COMP_GAIA_MAGS)
                {
                    logDebug("InitializeCriterias: comparison type: COMP_GAIA_MAGS");
                    logDebug("InitializeCriterias: range = %.9lf", criteria->range);
                }
                else
                    if (criteria->propCompType == vobsPROPERTY_COMP_STRING)
                {
                    logDebug("InitializeCriterias: comparison type: STRING");
                }
                else
                {
                    logDebug("InitializeCriterias: comparison type: FLOAT");
                    logDebug("InitializeCriterias: range = %.9lf", criteria->range);
                }
            }
        }
    }

    return mcsSUCCESS;
}

/**
 * Get all criteria informations (initialized by InitializeCriterias)
 * @param criteriaInfo returned vobsSTAR_CRITERIA_INFO[] pointer or NULL
 * @param size returned number of criteria or 0
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
 */
mcsCOMPL_STAT vobsSTAR_COMP_CRITERIA_LIST::GetCriterias(vobsSTAR_CRITERIA_INFO*& criteriaInfo, mcsINT32& size)
{
    if (!_initialized)
    {
        if (InitializeCriterias() == mcsFAILURE)
        {
            criteriaInfo = NULL;
            size = 0;

            return mcsFAILURE;
        }
    }

    criteriaInfo = _criteriaInfos;
    size = _size;

    return mcsSUCCESS;
}

/**
 * Reset internal criteria informations
 */
void vobsSTAR_COMP_CRITERIA_LIST::resetCriterias()
{
    if (_initialized)
    {
        _initialized = false;
        _size = 0;

        if (IS_NOT_NULL(_criteriaInfos))
        {
            delete[](_criteriaInfos);
        }
        _criteriaInfos = NULL;
    }
}

/*___oOo___*/
