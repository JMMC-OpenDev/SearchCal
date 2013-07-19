/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * @file
 * vobsSTAR_PROPERTY class definition.
 */

/* 
 * System Headers 
 */
#include <iostream>
#include <sstream> 
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
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
#include "vobsSTAR_PROPERTY_META.h"
#include "vobsSTAR_PROPERTY.h"
#include "vobsPrivate.h"
#include "vobsErrors.h"

/**
 * Return the string literal representing the confidence index 
 * @return string literal "LOW", "MEDIUM" or "HIGH"
 */
const char* vobsGetConfidenceIndex(const vobsCONFIDENCE_INDEX confIndex)
{
    return vobsCONFIDENCE_STR[confIndex];
}

/**
 * Return the integer literal representing the confidence index 
 * @return integer literal "1" (LOW), "2" (MEDIUM) or "3" (HIGH)
 */
const char* vobsGetConfidenceIndexAsInt(const vobsCONFIDENCE_INDEX confIndex)
{
    return vobsCONFIDENCE_INT[confIndex];
}

/**
 * Class constructor
 * 
 * @param meta property meta data
 */
vobsSTAR_PROPERTY::vobsSTAR_PROPERTY(const vobsSTAR_PROPERTY_META* meta)
{
    // meta data:
    _meta = meta;

    // data:
    _confidenceIndex = vobsCONFIDENCE_NO;
    _originIndex = vobsORIG_NONE;

    _value = NULL;
    _numerical = FP_NAN;
}

/**
 * Copy constructor
 */
vobsSTAR_PROPERTY::vobsSTAR_PROPERTY(const vobsSTAR_PROPERTY& property)
{
    // Initialize _value to NULL
    _value = NULL;

    // Uses the operator=() method to copy
    *this = property;
}

/**
 * Assignment operator
 */
vobsSTAR_PROPERTY &vobsSTAR_PROPERTY::operator=(const vobsSTAR_PROPERTY& property)
{
    if (this != &property)
    {
        _meta = property._meta;

        // values:
        _confidenceIndex = property._confidenceIndex;
        _originIndex = property._originIndex;

        if (isNotNull(property._value))
        {
            copyValue(property._value);
        }
        else
        {
            _value = NULL;
        }
        _numerical = property._numerical;
    }
    return *this;
}

/**
 * Destructor
 */
vobsSTAR_PROPERTY::~vobsSTAR_PROPERTY()
{
    if (isNotNull(_value))
    {
        delete[](_value);
    }
}


/*
 * Public methods
 */

/**
 * Set a property value
 *
 * @param value property value to set (given as string)
 * @param confidenceIndex confidence index
 * @param originIndex origin index
 * @param overwrite boolean to know if it is an overwrite property 
 *
 * @return mcsSUCCESS
 */
mcsCOMPL_STAT vobsSTAR_PROPERTY::SetValue(const char *value,
                                          vobsORIGIN_INDEX originIndex,
                                          vobsCONFIDENCE_INDEX confidenceIndex,
                                          mcsLOGICAL overwrite)
{
    // If the given new value is empty
    if (isNull(value))
    {
        // Return immediately
        return mcsSUCCESS;
    }

    // Affect value (only if the value is not set yet, or overwritting right is granted)
    if (isFalse(IsSet()) || isTrue(overwrite))
    {
        // If type of property is a string
        if (GetType() == vobsSTRING_PROPERTY)
        {
            copyValue(value);

            if (doLog(logDEBUG))
            {
                logDebug("_value('%s') -> \"%s\".", GetId(), _value);
            }

            _confidenceIndex = confidenceIndex;
            _originIndex = originIndex;
        }
        else // Value is a double
        {
            // Use the most precision format to read value
            mcsDOUBLE numerical = FP_NAN;
            FAIL_COND_DO(sscanf(value, "%lf", &numerical) != 1, errAdd(vobsERR_PROPERTY_TYPE, GetId(), value, "%lf"));

            // Delegate work to double-dedicated method.
            return SetValue(numerical, originIndex, confidenceIndex, overwrite);
        }
    }

    return mcsSUCCESS;
}

/**
 * Set a property value
 *
 * @param value property value to set (given as double)
 * @param confidenceIndex confidence index
 * @param originIndex origin index
 * @param overwrite boolean to know if it is an overwrite property 
 * @param originalValue the original value given a string 
 *        to avoid redundant parsing string / converting to string (loosing precision)
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
 *
 * \b Error codes:\n
 * The possible error is :
 * \li vobsERR_PROPERTY_TYPE
 */
mcsCOMPL_STAT vobsSTAR_PROPERTY::SetValue(mcsDOUBLE value,
                                          vobsORIGIN_INDEX originIndex,
                                          vobsCONFIDENCE_INDEX confidenceIndex,
                                          mcsLOGICAL overwrite)
{
    // Check type
    FAIL_COND_DO(GetType() == vobsSTRING_PROPERTY, errAdd(vobsERR_PROPERTY_TYPE, GetId(), "double", GetFormat()));

    // Affect value (only if the value is not set yet, or overwritting right is granted)
    if (isFalse(IsSet()) || isTrue(overwrite))
    {
        _confidenceIndex = confidenceIndex;
        _originIndex = originIndex;

        _numerical = value;
        
        // Use the custom property format by default
        const char* usedFormat = GetFormat();

        // If the value comes from a catalog
        if (isFalse(IsComputed()))
        {
            // keep precision (up to 6-digits)
            usedFormat = FORMAT_DEFAULT;
        }

        // @warning Potentially loosing precision in outputed numerical values
        mcsSTRING32 converted;
        FAIL_COND_DO(sprintf(converted, usedFormat, value) == 0, errAdd(vobsERR_PROPERTY_TYPE, GetId(), value, usedFormat));

        copyValue(converted);

        if (doLog(logDEBUG))
        {
            logDebug("_numerical('%s') = %lf -('%s')-> \"%s\".", GetId(), _numerical, usedFormat, _value);
        }
    }

    return mcsSUCCESS;
}

/**
 * Get value as a double.
 *
 * @param value pointer to store value.
 * 
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is 
 * returned.
 */
mcsCOMPL_STAT vobsSTAR_PROPERTY::GetValue(mcsDOUBLE *value) const
{
    // If value not set, return error
    FAIL_FALSE_DO(IsSet(), errAdd(vobsERR_PROPERTY_NOT_SET, GetId()));

    // Check type
    FAIL_COND_DO(GetType() != vobsFLOAT_PROPERTY, errAdd(vobsERR_PROPERTY_TYPE, GetId(), "double", GetFormat()));

    // Get value
    *value = _numerical;

    return mcsSUCCESS;
}

/**
 * Get value as an integer.
 *
 * @param value pointer to store value.
 * 
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is 
 * returned.
 */
mcsCOMPL_STAT vobsSTAR_PROPERTY::GetValue(mcsINT32 *value) const
{
    // If value not set, return error
    FAIL_FALSE_DO(IsSet(), errAdd(vobsERR_PROPERTY_NOT_SET, GetId()));

    // Check type
    FAIL_COND_DO(GetType() != vobsINT_PROPERTY, errAdd(vobsERR_PROPERTY_TYPE, GetId(), "integer", GetFormat()));

    // Get value
    *value = (mcsINT32) _numerical;

    return mcsSUCCESS;
}

/**
 * Get value as a boolean.
 *
 * @param value pointer to store value.
 * 
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is 
 * returned.
 */
mcsCOMPL_STAT vobsSTAR_PROPERTY::GetValue(mcsLOGICAL *value) const
{
    // If value not set, return error
    FAIL_FALSE_DO(IsSet(), errAdd(vobsERR_PROPERTY_NOT_SET, GetId()));

    // Check type
    FAIL_COND_DO(GetType() != vobsBOOL_PROPERTY, errAdd(vobsERR_PROPERTY_TYPE, GetId(), "boolean", GetFormat()));

    // Get value
    *value = (mcsLOGICAL) _numerical;

    return mcsSUCCESS;
}

/**
 * Get the object summary as a string, including all its member's values.
 *
 * @return the summary as a string object.
 */
const string vobsSTAR_PROPERTY::GetSummaryString(void) const
{
    // TODO: use C++ streams only !
    stringstream numericalStream;

    numericalStream << (double) _numerical;

    string summary = string("vobsSTAR_PROPERTY(Id = '") + string(GetId());
    summary += "'; Name = '" + string(GetName());
    summary += "'; Value = '" + (isNull(_value) ? "" : string(_value)) + "'; Numerical = '" + numericalStream.str();
    summary += "'; Unit = '" + string(GetUnit()) + "'; Type = '" + (GetType() == vobsSTRING_PROPERTY ? "STRING" : "FLOAT");
    summary += "', Origin = '" + string(vobsGetOriginIndex(GetOriginIndex()));
    summary += "'; Confidence = '" + string(vobsGetConfidenceIndex(GetConfidenceIndex()));
    summary += "'; Desc = '" + (isNull(GetDescription()) ? "" : string(GetDescription()));
    summary += "'; Link = '" + (isNull(GetLink()) ? "" : string(GetLink())) + "')";

    return summary;
}

/**
 * Update the value as string: allocate memory if needed; must be freed in destructor
 * @param value value to store
 */
void vobsSTAR_PROPERTY::copyValue(const char* value)
{
    const mcsUINT32 len = strlen(value);

    if (isNotNull(_value) && (strlen(_value) < len + 1))
    {
        // resize:
        delete[](_value);
        _value = NULL;
    }

    if (isNull(_value))
    {
        /* Create a new empty string */
        _value = new char[len + 1];
    }

    /* Copy str content in the new string */
    strcpy(_value, value);
}

/*___oOo___*/
