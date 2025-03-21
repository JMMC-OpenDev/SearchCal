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
 * Class constructor (main)
 *
 * @param meta property meta data
 */
vobsSTAR_PROPERTY::vobsSTAR_PROPERTY(mcsUINT8 metaIdx)
{
    SetMetaIndex(metaIdx);
    // data:
    SetConfidenceIndex(vobsCONFIDENCE_NO);
    SetOriginIndex(vobsORIG_NONE);
}

/**
 * Class constructor (default)
 */
vobsSTAR_PROPERTY::vobsSTAR_PROPERTY()
{
    // default constructor used by new vobsSTAR_PROPERTY[N], use long storage (unused)
    SetMetaIndex(UNDEF_PROX_IDX);
    // data:
    SetConfidenceIndex(vobsCONFIDENCE_NO);
    SetOriginIndex(vobsORIG_NONE);
}

/**
 * Copy constructor
 */
vobsSTAR_PROPERTY::vobsSTAR_PROPERTY(const vobsSTAR_PROPERTY& property)
{
    // use long storage (unused):
    SetMetaIndex(UNDEF_PROX_IDX);

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
        // Set index then storage type (set = 0):
        SetMetaIndex(property._metaIdx);

        // copy raw values:
        _confidenceIndex = property._confidenceIndex;
        _originIndex     = property._originIndex;

        if (property.IsFlagSet())
        {
            if (IS_NUM(property.GetStorageType()))
            {
                _opaqueStorage = property._opaqueStorage;
                SetFlagSet(true);
            }
            else
            {
                const char* propValue = property.GetValue();

                if (IS_NOT_NULL(propValue))
                {
                    copyValue(propValue);
                }
                else
                {
                    ClearStorageValue();
                }
            }
        }
        else
        {
            ClearStorageValue();
        }
    }
    return *this;
}

/**
 * Destructor
 */
vobsSTAR_PROPERTY::~vobsSTAR_PROPERTY()
{
    if (!IS_NUM(GetStorageType()))
    {
        // free string values:
        ClearStorageValue();
    }
}

/*
 * Public methods
 */
void vobsSTAR_PROPERTY::SetMetaIndex(const mcsUINT8 metaIdx)
{
    // update meta data:
    _metaIdx = metaIdx;

    // set type + FlagSet = 0:
    SetStorageType(GetType());
    ClearStorageValue();
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
    if (!IsFlagSet() || IS_TRUE(overwrite))
    {
        // If type of property is a string:
        if (IsPropString(GetType()))
        {
            copyValue(value);

            if (doLog(logDEBUG))
            {
                logDebug("_value('%s') -> \"%s\".", GetId(), GetValue());
            }
            SetConfidenceIndex(confidenceIndex);
            SetOriginIndex(originIndex);
        }
        else if (IsPropFloat(GetType()))
        {
            // property is a double:
            // Use the most precision format to read value
            mcsDOUBLE numerical = NAN;
            FAIL_COND_DO((sscanf(value, "%lf", &numerical) != 1),
                         errAdd(vobsERR_PROPERTY_TYPE, GetId(), value, "%lf"));

            if (doLog(logDEBUG))
            {
                logDebug("_numerical('%s') = \"%s\" -> %lf.", GetId(), value, numerical);
            }
            // Delegate to SetValue(double) method:
            return SetValue(numerical, originIndex, confidenceIndex, overwrite);
        }
        else
        {
            // property is an int/long/bool:
            // Use the (long) format to read value
            mcsINT64 numerical;
            FAIL_COND_DO((sscanf(value, "%ld", &numerical) != 1),
                         errAdd(vobsERR_PROPERTY_TYPE, GetId(), value, "%ld"));

            if (doLog(logDEBUG))
            {
                logDebug("_long('%s') = \"%s\" -> %ld.", GetId(), value, numerical);
            }
            // Delegate to SetValue(long) method:
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
    FAIL_COND_DO(!IsPropFloat(GetType()),
                 errAdd(vobsERR_PROPERTY_TYPE, GetId(), "double", GetFormat()));

    // Affect value (only if the value is not set yet, or overwritting right is granted)
    if (!IsFlagSet() || IS_TRUE(overwrite))
    {
        SetConfidenceIndex(confidenceIndex);
        SetOriginIndex(originIndex);

        editAsFloat2()->value = (mcsFLOAT) value;
        SetFlagSet(true);
    }
    return mcsSUCCESS;
}

/**
 * Set a property value
 *
 * @param value property value to set (given as long)
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
mcsCOMPL_STAT vobsSTAR_PROPERTY::SetValue(mcsINT64 value,
                                          vobsORIGIN_INDEX originIndex,
                                          vobsCONFIDENCE_INDEX confidenceIndex,
                                          mcsLOGICAL overwrite)
{
    // Check type
    FAIL_COND_DO(IsPropString(GetType()) || IsPropFloat(GetType()),
                 errAdd(vobsERR_PROPERTY_TYPE, GetId(), "long", GetFormat()));

    // Affect value (only if the value is not set yet, or overwritting right is granted)
    if (!IsFlagSet() || IS_TRUE(overwrite))
    {
        SetConfidenceIndex(confidenceIndex);
        SetOriginIndex(originIndex);
        editAsLong()->longValue = value;
        SetFlagSet(true);
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
    if (!IS_FLOAT2(GetStorageType()))
    {
        /* NOTE: only POS_EQ_RA_ERROR/POS_EQ_DEC_ERROR (unused) have the problem (String + error); ignore for now
         Warn  - 2025-03-17T15:10:30.631980 - vobsSTAR_PROPERTY.cpp:290    - _error('POS_EQ_RA_ERROR') = "5.326925" (bad type = 2).
         Warn  - 2025-03-17T15:10:30.631998 - vobsSTAR_PROPERTY.cpp:290    - _error('POS_EQ_DEC_ERROR') = "12.464516" (bad type = 2).
        */
        if (0) {
            logWarning("_error('%s') = \"%s\" (bad type = %d).", GetErrorId(), error, GetStorageType());
        }
        // Return immediately
        return mcsSUCCESS;
    }

    // Affect error (only if the error is not set yet, or overwritting right is granted)
    if (IS_FALSE(IsErrorSet()) || IS_TRUE(overwrite))
    {
        // Use the most precision format to read value
        mcsDOUBLE numerical = NAN;
        FAIL_COND_DO((sscanf(error, "%lf", &numerical) != 1),
                     errAdd(vobsERR_PROPERTY_TYPE, GetId(), error, "%lf"));

        if (doLog(logDEBUG))
        {
            logDebug("_error('%s') = \"%s\" -> %lf.", GetErrorId(), error, numerical);
        }

        editAsFloat2()->error = (mcsFLOAT) numerical;
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
    if (IS_FLOAT2(GetStorageType()))
    {
        // Affect value (only if the error is not set yet, or overwritting right is granted)
        if (IS_FALSE(IsErrorSet()) || IS_TRUE(overwrite))
        {
            editAsFloat2()->error = (mcsFLOAT) error;
        }
    }
}

/**
 * Get numerical value as a string or "" if not set or not a numerical property
 *
 * @param converted numerical value as a string or NULL
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
 */
mcsCOMPL_STAT vobsSTAR_PROPERTY::GetFormattedValue(mcsSTRING32* converted) const
{
    // Check type
    FAIL_COND_DO(IsPropString(GetType()),
                 errAdd(vobsERR_PROPERTY_TYPE, GetId(), "double", GetFormat()));

    if (IS_FLOAT2(GetStorageType()))
    {
        return GetFormattedValue(viewAsFloat2()->value, converted);
    } else {
        return GetFormattedValue(viewAsLong()->longValue, converted);
    }
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
    FAIL_FALSE_DO(IsSet(),
                  errAdd(vobsERR_PROPERTY_NOT_SET, GetId()));
    // Check type
    FAIL_COND_DO(IsPropString(GetType()),
                 errAdd(vobsERR_PROPERTY_TYPE, GetId(), "double", GetFormat()));

    mcsDOUBLE result = NAN;

    if (IS_FLOAT2(GetStorageType()))
    {
        result = viewAsFloat2()->value;
    }
    else
    {
        result = viewAsLong()->longValue;
    }
    // Get value
    *value = result;

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
    FAIL_FALSE_DO(IsSet(),
                  errAdd(vobsERR_PROPERTY_NOT_SET, GetId()));
    // Check type
    FAIL_COND_DO((GetType() != vobsINT_PROPERTY),
                 errAdd(vobsERR_PROPERTY_TYPE, GetId(), "integer", GetFormat()));

    // Get value
    *value = (mcsINT32) viewAsLong()->longValue;

    return mcsSUCCESS;
}

/**
 * Get value as an long.
 *
 * @param value pointer to store value.
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
 */
mcsCOMPL_STAT vobsSTAR_PROPERTY::GetValue(mcsINT64 *value) const
{
    // If value not set, return error
    FAIL_FALSE_DO(IsSet(),
                  errAdd(vobsERR_PROPERTY_NOT_SET, GetId()));
    // Check type
    FAIL_COND_DO((GetType() != vobsLONG_PROPERTY),
                 errAdd(vobsERR_PROPERTY_TYPE, GetId(), "long", GetFormat()));

    // Get value
    *value = viewAsLong()->longValue;

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
    FAIL_FALSE_DO(IsSet(),
                  errAdd(vobsERR_PROPERTY_NOT_SET, GetId()));
    // Check type
    FAIL_COND_DO((GetType() != vobsBOOL_PROPERTY),
                 errAdd(vobsERR_PROPERTY_TYPE, GetId(), "boolean", GetFormat()));

    // Get value
    *value = (mcsLOGICAL) viewAsLong()->longValue;

    return mcsSUCCESS;
}

/**
 * Get error as a string or "" if not set or not a numerical property
 *
 * @param converted error as a string or NULL
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
 */
mcsCOMPL_STAT vobsSTAR_PROPERTY::GetFormattedError(mcsSTRING32* converted) const
{
    // If value not set, return error
    FAIL_FALSE_DO(IsErrorSet(),
                  errAdd(vobsERR_PROPERTY_NOT_SET, GetId()));
    // Check type
    FAIL_COND_DO((GetType() != vobsFLOAT_PROPERTY),
                 errAdd(vobsERR_PROPERTY_TYPE, GetId(), "double", GetFormat()));

    return GetFormattedValue(viewAsFloat2()->error, converted);
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
    FAIL_FALSE_DO(IsErrorSet(),
                  errAdd(vobsERR_PROPERTY_NOT_SET, GetId()));
    // Check type
    FAIL_COND_DO((GetType() != vobsFLOAT_PROPERTY),
                 errAdd(vobsERR_PROPERTY_TYPE, GetId(), "double", GetFormat()));

    // Get error
    *error = viewAsFloat2()->error;

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
    char* value;

    out << "vobsSTAR_PROPERTY(Id= '" << GetId();
    out << "'; Name= '" << GetName();

    switch (GetStorageType())
    {
        case vobsPROPERTY_STORAGE_FLOAT2:
            out << "'; Value= '" << viewAsFloat2()->value;
            break;
        case vobsPROPERTY_STORAGE_LONG:
            out << "'; Value= '" << viewAsLong()->longValue;
            break;
        case vobsPROPERTY_STORAGE_STRING:
            value = IsFlagVarChar() ? (viewAsStrVar()->strValue)
                    : (viewAsStr8()->charValue);
            out << "'; Value= '" << (IS_NULL(value) ? "" : value);
            break;
        default:
            break;
    }
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
        out << "'; error= '" << viewAsFloat2()->error;
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
    /* only valid for vobsPROPERTY_STORAGE_STRING */
    const mcsUINT32 len = strlen(value); // assert (value != null)
    char* writeValue;

    if (len <= 7)
    {
        // char[8] storage so max 7 chars + '\0:
        if (IsFlagVarChar())
        {
            // free varchar storage:
            ClearStorageValue();
        }
        writeValue = editAsStr8()->charValue;
    }
    else
    {
        /* for XmLog (grow): align to 512 bytes else adaptative alignment:
         * (malloc default alignment is 16 bytes on 64bits) */
        const int alignLg2 = (IsFlagVarCharGrow() || (len >= 2048)) ? 9 /* 512 */
                : (len < 32) ? 2 /* 4 */
                : (len < 128) ? 4 /* 16 */
                : 8; /* 256 for [128-1024[ */

        mcsUINT32 aLen = alignSize(len + 1, alignLg2);

        // varchar storage:
        vobsSTAR_PROPERTY_VIEW_VARCHAR* editStrVar = editAsStrVar();

        if (IsFlagVarChar())
        {
            const mcsUINT32 sLen = (IS_NOT_NULL(editStrVar->strValue) ?
                    alignSize(strlen(editStrVar->strValue) + 1, alignLg2) : 0);

            // try reusing allocated block:
            if ((sLen != 0) && (sLen < aLen))
            {
                // free varchar storage:
                ClearStorageValue();
            }
        }
        else
        {
            // reset to allocate char*:
            SetFlagSet(false);
        }

        if (!IsFlagSet())
        {
            if (false && IsFlagVarCharGrow())
            {
                printf("copyValue: alloc[%u] for len = %u\n", aLen, len);
            }
            /* Create the char* storage with adjusted length > (len + 1) */
            writeValue = new char[aLen];
            editStrVar->strValue = writeValue;
            SetFlagVarChar(true);
        }
        else
        {
            writeValue = editStrVar->strValue;
        }
    }
    /* Anyway copy str content in the string storage */
    strncpy(writeValue, value, len + 1);
    SetFlagSet(true);
}

/**
 * Get given value as a string or "" if not set or not a numerical property
 *
 * @param value input value to format
 * @param converted numerical value as a string or NULL
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
 */
mcsCOMPL_STAT vobsSTAR_PROPERTY::GetFormattedValue(mcsDOUBLE value, mcsSTRING32* converted) const
{
    *converted[0] = '\0';

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
    FAIL_COND_DO((sprintf(*converted, usedFormat, value) == 0),
                 errAdd(vobsERR_PROPERTY_TYPE, GetId(), value, usedFormat));

    return mcsSUCCESS;
}

/**
 * Get given value as a string or "" if not set or not a numerical property
 *
 * @param value input value to format
 * @param converted numerical value as a string or NULL
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
 */
mcsCOMPL_STAT vobsSTAR_PROPERTY::GetFormattedValue(mcsINT64 value, mcsSTRING32* converted) const {
    *converted[0] = '\0';

    const char* usedFormat = "%ld";

    FAIL_COND_DO((sprintf(*converted, usedFormat, value) == 0),
                 errAdd(vobsERR_PROPERTY_TYPE, GetId(), value, usedFormat));

    return mcsSUCCESS;
}

/*___oOo___*/
