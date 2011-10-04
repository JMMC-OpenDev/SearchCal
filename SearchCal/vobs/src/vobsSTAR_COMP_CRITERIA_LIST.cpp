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
    _initialized   = false;
    _criteriaInfos = NULL;
}

/**
 * Copy Constructor
 */
vobsSTAR_COMP_CRITERIA_LIST::vobsSTAR_COMP_CRITERIA_LIST
    (const vobsSTAR_COMP_CRITERIA_LIST& criteriaList)
{
    // Ensure Internal members are undefined:
    _initialized   = false;
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
vobsSTAR_COMP_CRITERIA_LIST&vobsSTAR_COMP_CRITERIA_LIST::operator=
(const vobsSTAR_COMP_CRITERIA_LIST& criteriaList)
{
    // Clear the internal map
    Clear();
    
    // Copy in the criteria list from the given criteriaList
    _criteriaList = criteriaList._criteriaList;
    
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
    // If criteria is not a property return failure
    int propertyIndex = vobsSTAR::GetPropertyIndex(propertyId);
    if (propertyIndex == -1)
    {
        errAdd(vobsERR_INVALID_PROPERTY_ID, propertyId);
        return mcsFAILURE;
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
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is 
 * returned.
 */
mcsCOMPL_STAT vobsSTAR_COMP_CRITERIA_LIST::Remove(const char* propertyId)
{
    // check if that property is known
    // If criteria is not a property return failure
    int propertyIndex = vobsSTAR::GetPropertyIndex(propertyId);
    if (propertyIndex == -1)
    {
        errAdd(vobsERR_INVALID_PROPERTY_ID, propertyId);
        return mcsFAILURE;
    }

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
int vobsSTAR_COMP_CRITERIA_LIST::Size()
{
    return _criteriaList.size();
}

/**
 * Show criteria in logs
 */
void vobsSTAR_COMP_CRITERIA_LIST::log(logLEVEL level, const char* prefix)
{
    if (logIsStdoutLogLevel(level) == mcsTRUE)
    {
        const char* propertyId;
        mcsDOUBLE range;

        int i = 0;
        for (CriteriaList::iterator iter = _criteriaList.begin(); iter != _criteriaList.end(); iter++)
        {
            propertyId = iter->first;
            range      = iter->second;

            logPrint(MODULE_ID, level, NULL, __FILE_LINE__, "%sCriteria %d on property[%s] with range = %lf", prefix, (++i), propertyId, range);
        }
    }
}

/**
 * Prepare criteria traversal (lazily initialized)
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is 
 * returned.
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

    const bool isLogDebug = (logIsStdoutLogLevel(logDEBUG) == mcsTRUE);
    
    if (isLogDebug)
    {
        logDebug("vobsSTAR_COMP_CRITERIA_LIST::InitializeCriterias() - %d criterias", _size);
    }
    
    // Create criteria informations:
    _criteriaInfos = new vobsSTAR_CRITERIA_INFO[_size];

    // Fill criteria information:
    int i = 0;
    vobsSTAR_CRITERIA_INFO* criteria = NULL;

    const char* propertyId;
    mcsDOUBLE range;
    int propertyIndex;
    const vobsSTAR_PROPERTY_META* meta = NULL;

    for (CriteriaList::iterator iter = _criteriaList.begin(); iter != _criteriaList.end(); iter++)
    {
        propertyId = iter->first;
        range      = iter->second;

        criteria = &_criteriaInfos[i];

        criteria->propertyId = propertyId;
        criteria->range      = range;
        
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
        meta = vobsSTAR::GetPropertyMeta(propertyIndex);
        if (meta == NULL)
        {
            errAdd(vobsERR_INVALID_PROPERTY_ID, propertyId); 

            // Ensure Internal members are undefined:
            resetCriterias();
            
            return mcsFAILURE;
        }
        
        criteria->propertyIndex = propertyIndex;
        
        // is RA or DEC:
        if (strcmp(propertyId, vobsSTAR_POS_EQ_RA_MAIN)  == 0)
        {
            criteria->propCompType = vobsPROPERTY_COMP_RA;
        } 
        else if (strcmp(propertyId, vobsSTAR_POS_EQ_DEC_MAIN)  == 0)
        {
            criteria->propCompType = vobsPROPERTY_COMP_DEC;
        } else {
            criteria->propCompType = vobsPROPERTY_COMP_OTHER;
        }

        switch (criteria->propCompType)
        {
            case vobsPROPERTY_COMP_RA:
            case vobsPROPERTY_COMP_DEC:
                criteria->comparisonType = vobsFLOAT_PROPERTY;
                break;
            default:
            case vobsPROPERTY_COMP_OTHER:
                criteria->comparisonType = meta->GetType();
                break;
        }
    
        if (isLogDebug)
        {
            logDebug("vobsSTAR_COMP_CRITERIA_LIST::InitializeCriterias() - criteria %d on property [%d : %s]", 
                        i + 1, criteria->propertyIndex, criteria->propertyId);
            
            logDebug("vobsSTAR_COMP_CRITERIA_LIST::InitializeCriterias() - property comparison type  = %s", 
                        (criteria->propCompType == vobsPROPERTY_COMP_RA)  ? "RA" :
                        (criteria->propCompType == vobsPROPERTY_COMP_DEC) ? "DEC" : "OTHER"
                    );
            
            logDebug("vobsSTAR_COMP_CRITERIA_LIST::InitializeCriterias() - comparison type: %s", 
                        (criteria->comparisonType == vobsFLOAT_PROPERTY) ? "FLOAT" : "STRING"
                    );

            if (criteria->comparisonType == vobsFLOAT_PROPERTY)
            {
                logDebug("vobsSTAR_COMP_CRITERIA_LIST::InitializeCriterias() - range = %d", criteria->range);
            }
        }
        
        i++;
    }        

    _initialized = true;

    return mcsSUCCESS;
}
    
/**
 * Get all criteria informations (initialized by InitializeCriterias)
 * @param criteriaInfo returned vobsSTAR_CRITERIA_INFO[] pointer or NULL
 * @param size returned number of criteria or 0
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is 
 * returned.
 */
mcsCOMPL_STAT vobsSTAR_COMP_CRITERIA_LIST::GetCriterias(vobsSTAR_CRITERIA_INFO*& criteriaInfo, int& size)
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

        if (_criteriaInfos != NULL)
        {
            delete[] _criteriaInfos;
        }
        _criteriaInfos = NULL;
    }
}

/*___oOo___*/
