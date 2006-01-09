/*******************************************************************************
* JMMC project
*
* "@(#) $Id: vobsSTAR_PROPERTY.cpp,v 1.21 2006-01-09 16:08:46 lafrasse Exp $"
*
* History
* -------
* $Log: not supported by cvs2svn $
* Revision 1.20  2006/01/06 15:59:55  lafrasse
* Added CDS link in star property
*
* Revision 1.19  2006/01/05 09:07:39  lafrasse
* Code review
*
* Revision 1.18  2005/12/22 10:38:45  scetre
* Updated doxygen documentation
*
* Revision 1.17  2005/12/07 16:49:18  lafrasse
* Added support for 'description' attribute in VOTable column descriptors FIELD.
*
* Revision 1.16  2005/12/02 17:42:26  lafrasse
* Added property unit handling
*
* Revision 1.15  2005/11/16 10:47:55  scetre
* Updated documentation
*
* Revision 1.14  2005/11/16 10:47:54  scetre
* Updated documentation
*
* Revision 1.13  2005/11/11 16:37:51  gzins
* Added GetType() method
*
* Revision 1.12  2005/06/01 14:16:56  scetre
* Changed logExtDbg to logTrace
*
* Revision 1.11  2005/04/14 14:39:03  scetre
* Updated documentation.
* added test on method return.
*
* Revision 1.10  2005/03/04 06:34:37  gzins
* Updated GetConfidenceIndex prototype
*
* Revision 1.9  2005/02/22 14:22:25  gzins
* Fixed bug when testing convertion of float values to string
*
* Revision 1.8  2005/02/22 14:15:22  gzins
* Used format when affecting floatting value
*
* Revision 1.7  2005/02/13 15:35:02  gzins
* Added property name in message of vobsERR_INVALID_PROP_FORMAT error
*
* Revision 1.6  2005/02/11 10:43:35  gzins
* Fixed bug related to vobsCONFIDENCE_INDEX type
*
* Revision 1.5  2005/02/08 20:43:11  gzins
* Changed _isComputed by _origin
*
* Revision 1.4  2005/02/08 07:17:21  gzins
* Changed value parameter type: char* to const char*
*
* Revision 1.3  2005/01/26 08:18:33  scetre
* change history
*
* gzins     13-Dec-2004  Created
*
*******************************************************************************/

/**
 * @file
 * vobsSTAR_PROPERTY class definition.
 */

static char *rcsId="@(#) $Id: vobsSTAR_PROPERTY.cpp,v 1.21 2006-01-09 16:08:46 lafrasse Exp $"; 
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
 * Default constructor
 */
vobsSTAR_PROPERTY::vobsSTAR_PROPERTY()
{
    logTrace("vobsSTAR_PROPERTY::vobsSTAR_PROPERTY()");

    _origin = "-";
    strcpy(_value, vobsSTAR_PROP_NOT_SET);
}

/**
 * Class constructor
 * 
 * @param id     property identifier
 * @param name   property name 
 * @param type   property type
 * @param unit   property unit
 * @param format format used to set property
 * @param description property description
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
    _unit = unit;

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

    if (link != NULL)
    {
        _link = link;
    }

    if (description != NULL)
    {
        _description = description;
    }

    _confidenceIndex = vobsCONFIDENCE_LOW;

    _origin = "-";

    strcpy(_value, vobsSTAR_PROP_NOT_SET);
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
    logTrace("vobsSTAR_PROPERTY::SetValue()");

    // If value is empty, return
    if (strcmp(value, vobsSTAR_PROP_NOT_SET) == 0)
    {
        return mcsSUCCESS;
    }

    // Affect value
    if ((IsSet() == mcsFALSE) || (overwrite==mcsTRUE))
    {
        // If type of property is float
        if (_type == vobsFLOAT_PROPERTY)
        {
            // Use format to affect value
            float fValue;
            if (sscanf(value, "%f", &fValue) != 1)
            {
                errAdd(vobsERR_PROPERTY_TYPE, _id.c_str(), value, "%f");
                return (mcsFAILURE);
            }
            if (sprintf(_value, _format.c_str(), fValue) == 0)
            {
                errAdd(vobsERR_PROPERTY_TYPE, _id.c_str(), value,
                        _format.c_str());
                return (mcsFAILURE);
            }
        }
        else
        {
            // Just copy given value
            strcpy(_value, value);
        }

        _confidenceIndex = confidenceIndex;
        _origin = origin;
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
mcsCOMPL_STAT vobsSTAR_PROPERTY::SetValue(mcsFLOAT value,
                                          const char *origin,
                                          vobsCONFIDENCE_INDEX confidenceIndex,
                                          mcsLOGICAL overwrite)
{
    logTrace("vobsSTAR_PROPERTY::SetValue()");

    // Check type
    if (_type != vobsFLOAT_PROPERTY)
    {
        errAdd(vobsERR_PROPERTY_TYPE, _id.c_str(), "float", _format.c_str());
        return (mcsFAILURE);
    }

    // Affect value
    if ((IsSet() == mcsFALSE) || (overwrite==mcsTRUE))
    {
        if (sprintf(_value, _format.c_str(), value) == 0)
        {
            errAdd(vobsERR_PROPERTY_TYPE, _id.c_str(), value,
                    _format.c_str());
            return (mcsFAILURE);
        }

        _confidenceIndex = confidenceIndex;
        _origin = origin;
    }

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
mcsCOMPL_STAT vobsSTAR_PROPERTY::GetValue(mcsFLOAT *value) const
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
    // Convert property string value to integer value
    if (sscanf(_value, "%f", value) != 1)
    {
        errAdd(vobsERR_INVALID_PROP_FORMAT, _name.c_str(), _value, "float");
        return mcsFAILURE;
    }

    return mcsSUCCESS;
}

/**
 * Get property origin
 *
 * @return property origin
 */
const char   *vobsSTAR_PROPERTY::GetOrigin()
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
 * @return mcsTRUE) if the the property has been set, mcsFALSE otherwise.
 */
mcsLOGICAL vobsSTAR_PROPERTY::IsComputed(void) const
{
    logTrace("vobsSTAR_PROPERTY::IsComputed()");

    // Check whether property has been or not computed
    if (_origin != vobsSTAR_COMPUTED_PROP)
    {
        return mcsFALSE;
    }

    return mcsTRUE;
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
 * @return property unit if present, "-" otherwise.
 */
const char *vobsSTAR_PROPERTY::GetUnit(void) const
{
    logTrace("vobsSTAR_PROPERTY::GetUnit()");

    if (_unit.length() == 0)
    {
        return "-";
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


/*___oOo___*/
