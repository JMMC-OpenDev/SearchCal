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
 * Class constructor
 * 
 * @param meta property meta data
 */
vobsSTAR_PROPERTY::vobsSTAR_PROPERTY(const vobsSTAR_PROPERTY_META* meta)
{
    // meta data:
    _meta            = meta;
    
    // data:
    _confidenceIndex = vobsCONFIDENCE_LOW;
    _origin          = vobsSTAR_PROP_NOT_SET;
 
    _value           = NULL;
    _numerical       = FP_NAN;
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
    _meta            = property._meta;

    // values:
    _confidenceIndex = property._confidenceIndex;
    _origin          = property._origin;

    if (property._value != NULL)
    {
        copyValue(property._value);
    }
    else
    {
        _value = NULL;
    }
    _numerical       = property._numerical;

    return *this;
}

/**
 * Destructor
 */
vobsSTAR_PROPERTY::~vobsSTAR_PROPERTY()
{
    if (_value != NULL)
    {
        delete[] _value;
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
 * @param origin either the catalog name or vobsSTAR_COMPUTED_PROP if property
 * has been computed.
 * @param overwrite booleen to know if it is an overwrite property 
 *
 * @return mcsSUCCESS
 */
mcsCOMPL_STAT vobsSTAR_PROPERTY::SetValue(const char *value,
                                          const char *origin,
                                          vobsCONFIDENCE_INDEX confidenceIndex,
                                          mcsLOGICAL overwrite)
{
    // If the given new value is empty
    if (strcmp(value, vobsSTAR_PROP_NOT_SET) == 0)
    {
        // Return immediatly
        return mcsSUCCESS;
    }

    // Affect value (only if the value is not set yet, or overwritting right is granted)
    if ((IsSet() == mcsFALSE) || (overwrite == mcsTRUE))
    {
        // If type of property is a string
        if (GetType() == vobsSTRING_PROPERTY)
        {
            copyValue(value);

            logDebug("_value('%s') -> \"%s\".", GetId(), _value);
            
            _confidenceIndex = confidenceIndex;
            _origin = origin;
        }
        else // Value is a float
        {
            // Use the most precision format to read value
            mcsDOUBLE numerical = FP_NAN;
            if (sscanf(value, "%lf", &numerical) != 1)
            {
                errAdd(vobsERR_PROPERTY_TYPE, GetId(), value, "%f");
                return (mcsFAILURE);
            }

            // Delegate work to float-dedicated method.
            return SetValue(numerical, origin, confidenceIndex, overwrite);
        }
    }

    return mcsSUCCESS;    
}

/**
 * Set a property value
 *
 * @param value property value to set (given as float)
 * @param confidenceIndex confidence index
 * @param origin either the catalog name or vobsSTAR_COMPUTED_PROP if property
 * has been computed.
 * @param overwrite booleen to know if it is an overwrite property 
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
 *
 * \b Error codes:\n
 * The possible error is :
 * \li vobsERR_PROPERTY_TYPE
 */
mcsCOMPL_STAT vobsSTAR_PROPERTY::SetValue(mcsDOUBLE value,
                                          const char *origin,
                                          vobsCONFIDENCE_INDEX confidenceIndex,
                                          mcsLOGICAL overwrite)
{
    // Check type
    if (GetType() != vobsFLOAT_PROPERTY)
    {
        errAdd(vobsERR_PROPERTY_TYPE, GetId(), "float", GetFormat());
        return (mcsFAILURE);
    }

    // Affect value (only if the value is not set yet, or overwritting right is granted)
    if ((IsSet() == mcsFALSE) || (overwrite == mcsTRUE))
    {
        _confidenceIndex = confidenceIndex;
        _origin = origin;

        _numerical = value;
        
        // Use the custom property format by default
        const char* usedFormat = GetFormat();
        // If the value comes from a catalog
        if (IsComputed() == mcsFALSE)
        {
            // Keep maximum precision
            usedFormat = "%g";
        }

        // @warning Potentially loosing precision in outputed numerical values
        mcsSTRING16 converted;
        if (sprintf(converted, usedFormat, value) == 0)
        {
            errAdd(vobsERR_PROPERTY_TYPE, GetId(), value, usedFormat);
            return (mcsFAILURE);
        }

        copyValue(converted);
        
        logDebug("_numerical('%s') = %f -('%s')-> \"%s\".", GetId(), _numerical, usedFormat, _value);
    }

    return mcsSUCCESS;    
}

/**
 * Clear property value; i.e. set to '-'
 *
 * @return mcsSUCCESS
 */
mcsCOMPL_STAT vobsSTAR_PROPERTY::ClearValue(void)
{
    _confidenceIndex = vobsCONFIDENCE_LOW;
    _origin = vobsSTAR_PROP_NOT_SET;

    if (_value != NULL)
    {
        delete[] _value;
        _value = NULL;
    }
    _numerical = FP_NAN;

    return mcsSUCCESS;
}

/**
 * Get value as a float.
 *
 * @param value pointer to store value.
 * 
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is 
 * returned.
 *
 */
mcsCOMPL_STAT vobsSTAR_PROPERTY::GetValue(mcsDOUBLE *value) const
{
    // If value not set, return error
    if (IsSet() == mcsFALSE)
    {
        errAdd(vobsERR_PROPERTY_NOT_SET, GetId());
        return (mcsFAILURE);
    }

    // Check type
    if (GetType() != vobsFLOAT_PROPERTY)
    {
        errAdd(vobsERR_PROPERTY_TYPE, GetId(), "float", GetFormat());
        return (mcsFAILURE);
    }

    // Get value
    *value = _numerical;

    return mcsSUCCESS;
}

/**
 * Get the object summary as a string, including all its member's values.
 *
 * @return the summary as a string object.
 */
string vobsSTAR_PROPERTY::GetSummaryString(void) const
{
    stringstream numericalStream;
    
    numericalStream << (double)_numerical; 

    string summary = string("vobsSTAR_PROPERTY(Id = '") + string(GetId());
    summary += "'; Name = '" + string(GetName());
    summary += "'; Value = '" + (_value == NULL ? "" : string(_value)) + "'; Numerical = '" + numericalStream.str();
    summary += "'; Unit = '" + string(GetUnit()) + "'; Type = '" +  (GetType() == vobsSTRING_PROPERTY ? "STRING" : "FLOAT");
    summary += "', Origin = '" + string(_origin) + "'; Confidence = '" + 
            (_confidenceIndex == vobsCONFIDENCE_LOW ? "LOW" : (_confidenceIndex == vobsCONFIDENCE_MEDIUM ? "MEDIUM" : "HIGH"));
    summary +=  "'; Desc = '" + (GetDescription() == NULL ? "" : string(GetDescription()));
    summary += "'; Link = '" + (GetLink() == NULL ? "" : string(GetLink())) + "')";

    return summary;
}

/**
 * Update the value as string: allocate memory if needed; must be freed in destructor
 * @param value value to store
 */
void vobsSTAR_PROPERTY::copyValue(const char* value)
{
    const unsigned int len = strlen(value);

    if (_value != NULL && strlen(_value) < len + 1) {
        // resize:
        delete[] _value;
        _value = NULL;
    }
    
    if (_value == NULL)
    {
        /* Create a new empty string */
        _value = new char[len + 1];
    }
    
    /* Copy str content in the new string */
    strcpy(_value, value);
}

/*___oOo___*/
