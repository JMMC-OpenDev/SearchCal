/*******************************************************************************
* JMMC project
*
* "@(#) $Id: vobsSTAR_PROPERTY.cpp,v 1.1 2004-12-20 09:40:24 scetre Exp $"
*
* who       when         what
* --------  -----------  -------------------------------------------------------
* gzins     13-Dec-2004  Created
*
*******************************************************************************/

/**
 * \file
 * vobsSTAR_PROPERTY class definition.
 */

static char *rcsId="@(#) $Id: vobsSTAR_PROPERTY.cpp,v 1.1 2004-12-20 09:40:24 scetre Exp $"; 
static void *use_rcsId = ((void)&use_rcsId,(void *) &rcsId);

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

/*
 * Local Headers 
 */
#include "vobsSTAR_PROPERTY.h"
#include "vobsPrivate.h"
#include "vobsErrors.h"

/**
 * Class constructor
 * 
 */
vobsSTAR_PROPERTY::vobsSTAR_PROPERTY()
{
    //logExtDbg("vobsSTAR_PROPERTY::vobsSTAR_PROPERTY()");
    strcpy(_value, vobsSTAR_PROP_NOT_SET);
}

/**
 * Class constructor
 * 
 * \param id     property identifier
 * \param name   property name 
 * \param type   property type
 * \param format format used to set property
 */
vobsSTAR_PROPERTY::vobsSTAR_PROPERTY(char *id, char *name, 
                                     vobsPROPERTY_TYPE type, char *format)
{
    //logExtDbg("vobsSTAR_PROPERTY::vobsSTAR_PROPERTY(...)"); 
    _id     = id;
    _name   = name;
    _type   = type;
    if (format != NULL)
    {
    _format = format;
    }
    else
    {
        switch (type) 
        {
            case vobsSTRING_PROPERTY:
                _format = "%s";
                break;
            case vobsFLOAT_PROPERTY:
                _format = "%f";
                break;
        }
    }
    strcpy(_value, vobsSTAR_PROP_NOT_SET);
}

/**
 * Copy constructor
 */
vobsSTAR_PROPERTY::vobsSTAR_PROPERTY(const vobsSTAR_PROPERTY& property)
{
    //logExtDbg("vobsSTAR_PROPERTY::vobsSTAR_PROPERTY(property)"); 
    _id     = property._id;
    _name   = property._name;
    _type   = property._type;
    _format = property._format;
    strcpy(_value, property._value);
}

/**
 * Assignment operator
 */
vobsSTAR_PROPERTY &vobsSTAR_PROPERTY::operator=(const vobsSTAR_PROPERTY& property)
{
    //logExtDbg("vobsSTAR_PROPERTY::operator=()"); 
    _id     = property._id;
    _name   = property._name;
    _type   = property._type;
    _format = property._format;
    strcpy(_value, property._value);
    return *this;
}

/**
 * Class destructor
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
 * \param value property value to set (given as string)
 * \param overwrite booleen to know if it is an overwrite property 
 *
 * \return SUCCESS
 */
mcsCOMPL_STAT vobsSTAR_PROPERTY::SetValue(char *value, 
                                          mcsLOGICAL overwrite)
{
    logExtDbg("vobsSTAR_PROPERTY::SetValue()");

    // Affect value
    if ((IsSet() == mcsFALSE) || (overwrite==mcsTRUE))
    {
        strcpy(_value, value);
    }

    return SUCCESS;    
}

/**
 * Set a property value
 *
 * \param value property value to set (given as float)
 * \param overwrite booleen to know if it is an overwrite property 
 *
 * \return SUCCESS on successful completion. Otherwise FAILURE is returned.
 *
 * \b Error codes:\n
 * The possible error is :
 * \li vobsERR_PROPERTY_TYPE
 */
mcsCOMPL_STAT vobsSTAR_PROPERTY::SetValue(mcsFLOAT value, 
                                          mcsLOGICAL overwrite)
{
    logExtDbg("vobsSTAR_PROPERTY::SetValue()");

    // Check type
    if (_type != vobsFLOAT_PROPERTY)
    {
        errAdd (vobsERR_PROPERTY_TYPE, _id.c_str(), "float", _format.c_str());
        return (FAILURE);
    }

    // Affect value
    if ((IsSet() == mcsFALSE) || (overwrite==mcsTRUE))
    {
        sprintf(_value, _format.c_str(), value);
    }

    return SUCCESS;    
}

/**
 * Get value as a string.
 *
 * \return SUCCESS
 */
const char *vobsSTAR_PROPERTY::GetValue(void) const
{
    logExtDbg("vobsSTAR_PROPERTY::GetValue()");

    // Return property value
    return _value;
}

/**
 * Get value as a float.
 *
 * \param value pointer to store value.
 * 
 * \return SUCCESS on successful completion. Otherwise FAILURE is returned.
 *
 * \b Error codes:\n
 * The possible errors are :
 * \li vobsERR_PROPERTY_NOT_SET
 * \li vobsERR_PROPERTY_TYPE
 * \li vobsERR_INVALID_PROP_FORMAT
 */
mcsCOMPL_STAT vobsSTAR_PROPERTY::GetValue(mcsFLOAT *value) const
{
    logExtDbg("vobsSTAR_PROPERTY::GetValue()");

    // If value not set, return error
    if (IsSet() == mcsFALSE)
    {
        errAdd (vobsERR_PROPERTY_NOT_SET, _id.c_str());
        return (FAILURE);
    }
    
    // Check type
    if (_type != vobsFLOAT_PROPERTY)
    {
        errAdd (vobsERR_PROPERTY_TYPE, _id.c_str(), "float", _format.c_str());
        return (FAILURE);
    }

    // Get value
    // Convert property string value to integer value
    if (sscanf(_value, "%f", value) != 1)
    {
        errAdd (vobsERR_INVALID_PROP_FORMAT, _value, "float");
        return FAILURE;
    }

    return SUCCESS;
}

/**
 * Check whether the property is set or not.  
 * 
 * \return
 * True value (i.e. mcsTRUE) if the the property has been set, false (i.e.
 * mcsFALSE) otherwise.
 */
mcsLOGICAL vobsSTAR_PROPERTY::IsSet() const
{
    logExtDbg("vobsSTAR_PROPERTY::IsSet()");

    // Check if property string value is set to vobsSTAR_PROP_NOT_SET
    if (strcmp(_value, vobsSTAR_PROP_NOT_SET) == 0)
    {
        return mcsFALSE;
    }
    else
    {
        return mcsTRUE;
    }
}

/**
 * Get property id.
 *
 * \return SUCCESS
 */
const char *vobsSTAR_PROPERTY::GetId(void) const
{
    logExtDbg("vobsSTAR_PROPERTY::GetId()");

    // Return property value
    return _id.c_str();
}

/**
 * Get property name.
 *
 * \return SUCCESS
 */
const char *vobsSTAR_PROPERTY::GetName(void) const
{
    logExtDbg("vobsSTAR_PROPERTY::GetName()");

    // Return property value
    return _name.c_str();
}

/*
 * Protected methods
 */


/*
 * Private methods
 */


/*___oOo___*/
