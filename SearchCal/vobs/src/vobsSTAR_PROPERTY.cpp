/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * @file
 * vobsSTAR_PROPERTY class definition.
 */

static char *rcsId __attribute__ ((unused)) ="@(#) $Id: vobsSTAR_PROPERTY.cpp,v 1.34 2011-03-03 13:09:43 lafrasse Exp $"; 


/* 
 * System Headers 
 */
#include <iostream>
#include <sstream> 
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
#include "vobsSTAR_PROPERTY.h"
#include "vobsPrivate.h"
#include "vobsErrors.h"


/**
 * Default constructor
 */
vobsSTAR_PROPERTY::vobsSTAR_PROPERTY()
{
    logTrace("vobsSTAR_PROPERTY::vobsSTAR_PROPERTY()");

    _origin = vobsSTAR_PROP_NOT_SET;
    strcpy(_value, vobsSTAR_PROP_NOT_SET);
    _numerical = FP_NAN;
}

/**
 * Class constructor
 * 
 * @param id property identifier
 * @param name property name 
 * @param type property type
 * @param unit property unit, vobsSTAR_PROP_NOT_SET by default or for 'NULL'.
 * @param format format used to set property (%s or %.3f by default or for 'NULL').
 * @param link link for this property (none by default or for 'NULL').
 * @param description property description (none by default or for 'NULL').
 */
vobsSTAR_PROPERTY::vobsSTAR_PROPERTY(const char*              id,
                                     const char*              name, 
                                     const vobsPROPERTY_TYPE  type,
                                     const char*              unit,
                                     const char*              format,
                                     const char*              link,
                                     const char*              description)
{
    logTrace("vobsSTAR_PROPERTY::vobsSTAR_PROPERTY(...)"); 

    _id   = id;
    _name = name;
    _type = type;

    _unit = vobsSTAR_PROP_NOT_SET;
    if (unit != NULL)
    {
        _unit = unit;
    }

    char* defaultFormat = "%s";
    switch (type) 
    {
        case vobsSTRING_PROPERTY:
            defaultFormat = "%s";
            break;

        case vobsFLOAT_PROPERTY:
            defaultFormat = "%.3f";
            break;
    }
    _format = defaultFormat;

    if (format != NULL)
    {
        _format = format;
    }

    if (link != NULL)
    {
        _link = link;
    }

    if (description != NULL)
    {
        _description = description;
    }

    _confidenceIndex = vobsCONFIDENCE_LOW;

    _origin = vobsSTAR_PROP_NOT_SET;

    strcpy(_value, vobsSTAR_PROP_NOT_SET);

    _numerical = FP_NAN;
}

/**
 * Copy constructor
 */
vobsSTAR_PROPERTY::vobsSTAR_PROPERTY(const vobsSTAR_PROPERTY& property)
{
    logTrace("vobsSTAR_PROPERTY::vobsSTAR_PROPERTY(property)"); 

    *this = property;
}

/**
 * Assignment operator
 */
vobsSTAR_PROPERTY &vobsSTAR_PROPERTY::operator=(const vobsSTAR_PROPERTY& property)
{
    logTrace("vobsSTAR_PROPERTY::operator=()"); 

    _id              = property._id;
    _name            = property._name;
    _type            = property._type;
    _unit            = property._unit;
    _link            = property._link;
    _description     = property._description;
    _format          = property._format;
    _confidenceIndex = property._confidenceIndex;
    _origin          = property._origin;
    _numerical       = property._numerical;

    strcpy(_value, property._value);

    return *this;
}


/**
 * Destructor
 */
vobsSTAR_PROPERTY::~vobsSTAR_PROPERTY()
{
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
    logTrace("vobsSTAR_PROPERTY::SetValue(vobsSTRING_PROPERTY)");

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
        if (_type == vobsSTRING_PROPERTY)
        {
            // Make sure we always keep the trailing \0 regardless of any buffer overflow.
            memset(_value, '\0', sizeof(_value));
            strncpy(_value, value, sizeof(_value) - 1);

            _confidenceIndex = confidenceIndex;

            _origin = origin;
        }
        else // Value is a float
        {
            // Use the most precision format to read value
            mcsDOUBLE numerical = FP_NAN;
            if (sscanf(value, "%lf", &numerical) != 1)
            {
                errAdd(vobsERR_PROPERTY_TYPE, _id.c_str(), value, "%f");
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
    logTrace("vobsSTAR_PROPERTY::SetValue(vobsFLOAT_PROPERTY)");

    // Check type
    if (_type != vobsFLOAT_PROPERTY)
    {
        errAdd(vobsERR_PROPERTY_TYPE, _id.c_str(), "float", _format.c_str());
        return (mcsFAILURE);
    }

    // Affect value (only if the value is not set yet, or overwritting right is granted)
    if ((IsSet() == mcsFALSE) || (overwrite == mcsTRUE))
    {
        _confidenceIndex = confidenceIndex;
        _origin = origin;
        _numerical = value;
        
        // Use the custom property format by default
        char* usedFormat = (char*) _format.c_str();
        // If the value comes from a catalog
        if (IsComputed() == mcsFALSE)
        {
            // Keep maximum precision
            usedFormat = "%g";
        }

        // @warning Potentially loosing precision in outputed numerical values
        if (sprintf(_value, usedFormat, value) == 0)
        {
            errAdd(vobsERR_PROPERTY_TYPE, _id.c_str(), value, usedFormat);
            return (mcsFAILURE);
        }

        logDebug("_numerical('%s') = %f -('%s')-> \"%s\".\n", _id.c_str(), _numerical, usedFormat, _value);
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
    logTrace("vobsSTAR_PROPERTY::ClearValue()");

    _confidenceIndex = vobsCONFIDENCE_LOW;

    _origin = vobsSTAR_PROP_NOT_SET;

    strcpy(_value, vobsSTAR_PROP_NOT_SET);

    _numerical = FP_NAN;

    return mcsSUCCESS;
}

/**
 * Get value as a string.
 *
 * @return mcsSUCCESS
 */
const char *vobsSTAR_PROPERTY::GetValue(void) const
{
    logTrace("vobsSTAR_PROPERTY::GetValue()");

    // Return property value
    return _value;
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
    logTrace("vobsSTAR_PROPERTY::GetValue()");

    // If value not set, return error
    if (IsSet() == mcsFALSE)
    {
        errAdd(vobsERR_PROPERTY_NOT_SET, _id.c_str());
        return (mcsFAILURE);
    }
    
    // Check type
    if (_type != vobsFLOAT_PROPERTY)
    {
        errAdd(vobsERR_PROPERTY_TYPE, _id.c_str(), "float", _format.c_str());
        return (mcsFAILURE);
    }

    // Get value
    *value = _numerical;

    return mcsSUCCESS;
}

/**
 * Get property origin
 *
 * @return property origin
 */
const char *vobsSTAR_PROPERTY::GetOrigin()
{
    return _origin.c_str();
}

/**
 * Get value of the confidence index
 *
 * @return value of confidence index
 */
vobsCONFIDENCE_INDEX vobsSTAR_PROPERTY::GetConfidenceIndex()
{
    return _confidenceIndex;
}

/**
 * Check whether the property is computed or not.  
 * 
 * @return mcsTRUE if the the property has been computed, mcsFALSE otherwise.
 */
mcsLOGICAL vobsSTAR_PROPERTY::IsComputed(void) const
{
    logTrace("vobsSTAR_PROPERTY::IsComputed()");

    // Check whether property has been computed or not
    if (_origin == vobsSTAR_COMPUTED_PROP)
    {
        return mcsTRUE;
    }

    return mcsFALSE;
}


/**
 * Check whether the property is set or not.  
 * 
 * @return mcsTRUE if the the property has been set, mcsFALSE otherwise.
 */
mcsLOGICAL vobsSTAR_PROPERTY::IsSet(void) const
{
    logTrace("vobsSTAR_PROPERTY::IsSet()");

    // Check if property string value is set to vobsSTAR_PROP_NOT_SET
    if (strcmp(_value, vobsSTAR_PROP_NOT_SET) == 0)
    {
        return mcsFALSE;
    }

    return mcsTRUE;
}

/**
 * Get property id.
 *
 * @return mcsSUCCESS
 */
const char *vobsSTAR_PROPERTY::GetId(void) const
{
    logTrace("vobsSTAR_PROPERTY::GetId()");

    // Return property id
    return _id.c_str();
}

/**
 * Get property name.
 *
 * @return mcsSUCCESS
 */
const char *vobsSTAR_PROPERTY::GetName(void) const
{
    logTrace("vobsSTAR_PROPERTY::GetName()");

    // Return property name
    return _name.c_str();
}

/**
 * Get property type.
 *
 * @return property type
 */
vobsPROPERTY_TYPE vobsSTAR_PROPERTY::GetType(void) const
{
    logTrace("vobsSTAR_PROPERTY::GetType()");

    // Return property type
    return _type;
}

/**
 * Get property unit.
 *
 * @sa http://vizier.u-strasbg.fr/doc/catstd-3.2.htx
 *
 * @return property unit if present, vobsSTAR_PROP_NOT_SET otherwise.
 */
const char *vobsSTAR_PROPERTY::GetUnit(void) const
{
    logTrace("vobsSTAR_PROPERTY::GetUnit()");

    if (_unit.length() == 0)
    {
        return vobsSTAR_PROP_NOT_SET;
    }

    // Return property unit
    return _unit.c_str();
}

/**
 * Get property description.
 *
 * @sa http://vizier.u-strasbg.fr/doc/catstd-3.2.htx
 *
 * @return property description if present, NULL otherwise.
 */
const char *vobsSTAR_PROPERTY::GetDescription(void) const
{
    logTrace("vobsSTAR_PROPERTY::GetDescription()");
    
    if (_description.length() == 0)
    {
        return NULL;
    }

    // Return property description
    return _description.c_str();
}

/**
 * Get property CDS link.
 *
 * @return property CDS link if present, NULL otherwise.
 */
const char *vobsSTAR_PROPERTY::GetLink(void) const
{
    logTrace("vobsSTAR_PROPERTY::GetLink()");
    
    if (_link.length() == 0)
    {
        return NULL;
    }

    // Return property CDS link
    return _link.c_str();
}

/**
 * Get the object summary as a string, including all its member's values.
 *
 * @return the summary as a string object.
 */
string vobsSTAR_PROPERTY::GetSummaryString(void) const
{
    stringstream numericalStream;
    numericalStream << (float)_numerical; 

    string summary = "vobsSTAR_PROPERTY(Id = '" + _id + "'; Name = '" + _name + "'; Value = '" + string(_value) + "'; Numerical = '" + numericalStream.str() + "'; Unit = '" + _unit + "'; Type = '" + (_type == vobsSTRING_PROPERTY ? "STRING" : "FLOAT") + "', Origin = '" + _origin + "'; Confidence = '" + (_confidenceIndex == vobsCONFIDENCE_LOW ? "LOW" : (_confidenceIndex == vobsCONFIDENCE_MEDIUM ? "MEDIUM" : "HIGH")) + "'; Desc = '" + _description + "'; Link = '" + _link + "')";

    return summary;
}


/*___oOo___*/
