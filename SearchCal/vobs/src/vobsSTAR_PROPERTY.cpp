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
 */
vobsSTAR_PROPERTY::vobsSTAR_PROPERTY()
{
    SetMetaIndex(UNDEF_PROX_IDX);
    // data:
    SetConfidenceIndex(vobsCONFIDENCE_NO);
    SetOriginIndex(vobsORIG_NONE);
    _value           = NULL;
    _numerical       = NAN;
    _error           = NAN;
}

/**
 * Class constructor
 *
 * @param meta property meta data
 */
vobsSTAR_PROPERTY::vobsSTAR_PROPERTY(mcsUINT8 metaIdx)
{
    SetMetaIndex(metaIdx);
    // data:
    SetConfidenceIndex(vobsCONFIDENCE_NO);
    SetOriginIndex(vobsORIG_NONE);
    _value           = NULL;
    _numerical       = NAN;
    _error           = NAN;
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
        SetMetaIndex(property._metaIdx);

        // copy raw values:
        _confidenceIndex = property._confidenceIndex;
        _originIndex     = property._originIndex;

        if (IS_NOT_NULL(property._value))
        {
            copyValue(property._value);
        }
        else
        {
            _value = NULL;
        }
        _numerical = property._numerical;
        _error     = property._error;
    }
    return *this;
}

/**
 * Destructor
 */
vobsSTAR_PROPERTY::~vobsSTAR_PROPERTY()
{
    if (IS_NOT_NULL(_value))
    {
        delete[](_value);
        _value = NULL;
    }
}

/*
 * Public methods
 */
void vobsSTAR_PROPERTY::SetMetaIndex(mcsUINT8 metaIdx)
{
    // meta data:
    _metaIdx = metaIdx;
}

/**
 * Set a property value
 *
 * @param value property value to set (given as string)
 * @param confidenceIndex confidence index
 * @param originIndex origin index
 * @param overwrite boolean to know if it is an overwrite property
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
 */
mcsCOMPL_STAT vobsSTAR_PROPERTY::SetValue(const char* value,
                                          vobsORIGIN_INDEX originIndex,
                                          vobsCONFIDENCE_INDEX confidenceIndex,
                                          mcsLOGICAL overwrite)
{
    // If the given new value is empty
    if (IS_NULL(value))
    {
        // Return immediately
        return mcsSUCCESS;
    }

    // Affect value (only if the value is not set yet, or overwritting right is granted)
    if (IS_FALSE(IsSet()) || IS_TRUE(overwrite))
    {
        // If type of property is a string
        if (GetType() == vobsSTRING_PROPERTY)
        {
            copyValue(value);

            if (doLog(logDEBUG))
            {
                logDebug("_value('%s') -> \"%s\".", GetId(), _value);
            }

            SetConfidenceIndex(confidenceIndex);
            SetOriginIndex(originIndex);
        }
        else // Value is a double
        {
            // Use the most precision format to read value
            mcsDOUBLE numerical = NAN;
            FAIL_COND_DO(sscanf(value, "%lf", &numerical) != 1, errAdd(vobsERR_PROPERTY_TYPE, GetId(), value, "%lf"));

            if (doLog(logDEBUG))
            {
                logDebug("_numerical('%s') = \"%s\" -> %lf.", GetId(), value, numerical);
            }

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
    if (IS_FALSE(IsSet()) || IS_TRUE(overwrite))
    {
        SetConfidenceIndex(confidenceIndex);
        SetOriginIndex(originIndex);
        _numerical       = value;
    }

    return mcsSUCCESS;
}

/**
 * Set a property error
 *
 * @param error property error to set (given as string)
 * @param overwrite boolean to know if it is an overwrite property
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
 */
mcsCOMPL_STAT vobsSTAR_PROPERTY::SetError(const char* error,
                                          mcsLOGICAL overwrite)
{
    // If the given new value is empty
    if (IS_NULL(error))
    {
        // Return immediately
        return mcsSUCCESS;
    }

    // Affect error (only if the error is not set yet, or overwritting right is granted)
    if (IS_FALSE(IsErrorSet()) || IS_TRUE(overwrite))
    {
        // Use the most precision format to read value
        mcsDOUBLE numerical = NAN;
        FAIL_COND_DO(sscanf(error, "%lf", &numerical) != 1, errAdd(vobsERR_PROPERTY_TYPE, GetId(), error, "%lf"));

        if (doLog(logDEBUG))
        {
            logDebug("_error('%s') = \"%s\" -> %lf.", GetErrorId(), error, numerical);
        }

        // Delegate work to double-dedicated method.
        _error = numerical;
    }

    return mcsSUCCESS;
}

/**
 * Set a property error
 *
 * @param error property error to set (given as double)
 * @param overwrite boolean to know if it is an overwrite property
 */
void vobsSTAR_PROPERTY::SetError(mcsDOUBLE  error,
                                 mcsLOGICAL overwrite)
{
    // Affect value (only if the error is not set yet, or overwritting right is granted)
    if (IS_FALSE(IsErrorSet()) || IS_TRUE(overwrite))
    {
        _error = error;
    }
}

/**
 * Get numerical value as a string or "" if not set or not a numerical property
 *
 * @param converted numerical value as a string or NULL
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
 */
mcsCOMPL_STAT vobsSTAR_PROPERTY::GetFormattedValue(mcsSTRING32& converted) const
{
    // Check type
    FAIL_COND_DO(GetType() == vobsSTRING_PROPERTY, errAdd(vobsERR_PROPERTY_TYPE, GetId(), "double", GetFormat()));

    return GetFormattedValue(_numerical, converted);
}

/**
 * Get value as a double.
 *
 * @param value pointer to store value.
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
 */
mcsCOMPL_STAT vobsSTAR_PROPERTY::GetValue(mcsDOUBLE *value) const
{
    // If value not set, return error
    FAIL_FALSE_DO(IsSet(), errAdd(vobsERR_PROPERTY_NOT_SET, GetId()));

    // Check type
    FAIL_COND_DO(GetType() == vobsSTRING_PROPERTY, errAdd(vobsERR_PROPERTY_TYPE, GetId(), "double", GetFormat()));

    // Get value
    *value = _numerical;

    return mcsSUCCESS;
}

/**
 * Get value as an integer.
 *
 * @param value pointer to store value.
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
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
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
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
 * Get error as a string or "" if not set or not a numerical property
 *
 * @param converted error as a string or NULL
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
 */
mcsCOMPL_STAT vobsSTAR_PROPERTY::GetFormattedError(mcsSTRING32& converted) const
{
    return GetFormattedValue(_error, converted);
}

/**
 * Get error as a double.
 *
 * @param error pointer to store value.
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
 */
mcsCOMPL_STAT vobsSTAR_PROPERTY::GetError(mcsDOUBLE *error) const
{
    // If error not set, return error
    FAIL_FALSE_DO(IsErrorSet(), errAdd(vobsERR_PROPERTY_NOT_SET, GetId()));

    // Get error
    *error = _error;

    return mcsSUCCESS;
}

/**
 * Get the object summary as a string, including all its member's values.
 *
 * @return the summary as a string object.
 */
const string vobsSTAR_PROPERTY::GetSummaryString(void) const
{
    ostringstream out;

    out << "vobsSTAR_PROPERTY(Id= '" << GetId();
    out << "'; Name= '" << GetName();
    out << "'; Value= '" << (IS_NULL(_value) ? "" : _value);
    out << "'; Numerical= '" << (mcsDOUBLE) _numerical;
    out << "'; Unit= '" << (IS_NULL(GetUnit()) ? "" : GetUnit());
    out << "'; Type= '" << vobsPROPERTY_TYPE_STR[GetType()];
    out << "', Origin= '" << vobsGetOriginIndex(GetOriginIndex());
    out << "'; Confidence= '" << vobsGetConfidenceIndex(GetConfidenceIndex());
    out << "'; Desc= '" << (IS_NULL(GetDescription()) ? "" : GetDescription());
    out << "'; Link= '" << (IS_NULL(GetLink()) ? "" : GetLink());

    if (IS_NOT_NULL(GetErrorMeta()))
    {
        out << "'; errorId= '" << GetErrorId();
        out << "'; errorName= '" << GetErrorName();
        out << "'; error= '" << (mcsDOUBLE) _error;
    }
    out << "')";

    return out.str();
}

/**
 * Update the value as string: allocate memory if needed; must be freed in destructor
 * @param value value to store
 */
void vobsSTAR_PROPERTY::copyValue(const char* value)
{
    const mcsUINT32 len = strlen(value);

    if (IS_NOT_NULL(_value) && (strlen(_value) < len + 1))
    {
        // resize:
        delete[](_value);
        _value = NULL;
    }

    if (IS_NULL(_value))
    {
        /* Create a new empty string */
        _value = new char[len + 1];
    }

    /* Copy str content in the new string */
    strcpy(_value, value);
}

/**
 * Get given value as a string or "" if not set or not a numerical property
 *
 * @param value input value to format
 * @param converted numerical value as a string or NULL
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
 */
mcsCOMPL_STAT vobsSTAR_PROPERTY::GetFormattedValue(mcsDOUBLE value, mcsSTRING32& converted) const
{
    converted[0] = '\0';

    // Return success if numerical value is not set
    SUCCESS_COND(isnan(value));

    // Use the custom property format by default
    const char* usedFormat = GetFormat();

    // If the value comes from a catalog
    if (IS_FALSE(IsComputed()))
    {
        // keep precision (up to 6-digits)
        usedFormat = FORMAT_DEFAULT;
    }

    // @warning Potentially loosing precision in outputed numerical values
    FAIL_COND_DO(sprintf(converted, usedFormat, value) == 0, errAdd(vobsERR_PROPERTY_TYPE, GetId(), value, usedFormat));

    return mcsSUCCESS;
}

/*___oOo___*/
