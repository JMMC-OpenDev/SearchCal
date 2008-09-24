/*******************************************************************************
* JMMC project
*
* "@(#) $Id: vobsSTAR.cpp,v 1.75 2008-09-24 11:34:59 lafrasse Exp $"
*
* History
* -------
* $Log: not supported by cvs2svn $
* Revision 1.74  2008/03/10 07:50:22  lafrasse
* Minor modifications on comments and log traces.
*
* Revision 1.73  2007/10/31 11:21:12  gzins
* Updated IsSame() to support string star property comparison
*
* Revision 1.72  2007/09/12 10:13:52  lafrasse
* Changed catalog identifier property types from FLOAT to STRING.
*
* Revision 1.71  2006/08/22 14:45:08  gzins
* Improved error handling in GetId() method.
* Check RA/DEC is set, before calling GetRa()/GetDec() method
*
* Revision 1.70  2006/04/10 14:51:57  gzins
* Added ClearPropertyValue
*
* Revision 1.69  2006/03/22 10:42:18  scetre
* Added TYC2 and TYC3 properties
*
* Revision 1.68  2006/03/03 15:03:28  scetre
* Changed rcsId to rcsId __attribute__ ((unused))
*
* Revision 1.67  2006/02/21 16:32:00  scetre
* Updated documentation
*
* Revision 1.66  2006/01/18 15:59:45  scetre
* Added GetPropertyConfIndex() method
*
* Revision 1.65  2006/01/18 09:07:03  scetre
* Removed unused U magnitude
*
* Revision 1.64  2006/01/09 16:09:49  lafrasse
* Updated property links
*
* Revision 1.63  2006/01/06 15:59:55  lafrasse
* Added CDS link in star property
*
* Revision 1.62  2006/01/05 09:10:04  gzins
* Moved star coordinates in front of property list; workaround for Aladin bug
*
* Revision 1.61  2006/01/05 09:07:39  lafrasse
* Code review
*
* Revision 1.60  2005/12/22 10:38:45  scetre
* Updated doxygen documentation
*
* Revision 1.59  2005/12/14 15:09:56  scetre
* Added return of coordinates when getting star id
*
* Revision 1.58  2005/12/13 15:37:24  lafrasse
* Added star Id management with the new GetId() method
*
* Revision 1.57  2005/12/12 14:05:55  scetre
* Moved computed cousin magnitude to other module
*
* Revision 1.56  2005/12/07 16:49:18  lafrasse
* Added support for 'description' attribute in VOTable column descriptors FIELD.
*
* Revision 1.55  2005/12/07 15:25:28  scetre
* Added known unit of properties
*
* Revision 1.54  2005/12/02 17:43:29  lafrasse
* Added property unit handling
*
* Revision 1.53  2005/12/02 12:02:46  scetre
* Added J H K cous
*
* Revision 1.52  2005/11/24 15:13:01  scetre
* Added CODE_MISC Kflg
*
* Revision 1.51  2005/11/24 13:17:23  scetre
* Added new properties for faint research
*
* Revision 1.50  2005/11/24 08:14:14  scetre
* Added GetPropertyType() method
*
* Revision 1.49  2005/11/23 10:22:20  scetre
* Generalized filter
*
* Revision 1.48  2005/11/23 08:32:47  scetre
* Added properties for faint K
*
* Revision 1.47  2005/11/16 10:47:55  scetre
* Updated documentation
*
* Revision 1.46  2005/11/16 10:47:54  scetre
* Updated documentation
*
* Revision 1.45  2005/11/15 14:57:56  scetre
* Added new scenario structure
* Added possibility to query merand and borde as primary catalog
*
* Revision 1.44  2005/09/06 12:37:41  scetre
* Added INST_WAVELENGTH_VALUE as UCD for the wavelength in order to get magnitude according to the flux in II/225 catalog
*
* Revision 1.43  2005/06/01 14:16:56  scetre
* Changed logExtDbg to logTrace
*
* Revision 1.42  2005/04/14 14:39:03  scetre
* Updated documentation.
* added test on method return.
*
* Revision 1.41  2005/03/30 12:49:26  scetre
* Updated documentation
*
* Revision 1.40  2005/03/06 10:44:03  gzins
* Removed GetSpectralClass and GetLuminosityClass; not applicable due to the complexity of the spectral type format
*
* Revision 1.39  2005/03/04 06:36:18  gzins
* Fixed bug related to RA/DEC conversion; -00:xx was seen as positive.
*
* Revision 1.38  2005/02/24 16:59:14  scetre
* Improved debug message when comparing star properties (show HD number)
*
* Revision 1.37  2005/02/23 17:15:32  scetre
* Updated property name
*
* Revision 1.36  2005/02/23 07:52:13  gzins
* Added vobsSTAR_DIAM12 and vobsSTAR_DIAM12_ERROR properties
*
* Revision 1.35  2005/02/22 15:42:42  gzins
* Set format for paralax to "%.2f"
*
* Revision 1.34  2005/02/21 11:07:47  scetre
* changed glat and glon format in %0.2f
*
* Revision 1.33  2005/02/16 17:01:37  scetre
* Changed HD HIP and DM number format from string to float
*
* Revision 1.32  2005/02/15 15:41:47  gzins
* Fixed wrong mcsmcsSUCCESS and mcsmcsFAILURE
*
* Revision 1.31  2005/02/15 15:19:30  gzins
* Changed SUCCESS/FAILURE to mcsSUCCESS/mcsFAILURE
*
* Revision 1.30  2005/02/13 15:25:59  gzins
* Added showPropId parameter to Display() method
*
* Revision 1.29  2005/02/11 10:44:00  gzins
* Added UD, LD and UDDK diameter star properties
*
* Revision 1.28  2005/02/11 10:32:10  gzins
* Added UD, LD and UDDK diameter properties
*
* Revision 1.27  2005/02/10 08:07:38  scetre
* changed parser and hd, hip, dm number id in order to get all of them even if
* they have the same UCD
*
* Revision 1.26  2005/02/08 20:43:11  gzins
* Changed _isComputed by _origin
*
* Revision 1.25  2005/02/08 07:22:27  gzins
* Changed value parameter type: char* to const char*
* Accepted RA and DEC with either ':' or ' ' as separator
*
* Revision 1.24  2005/02/07 09:14:00  gzins
* Fixed wrong type for isComputed parameter; mcsFLOAT instead of mcsLOGICAL
*
* Revision 1.23  2005/02/04 14:31:30  scetre
* add getLuminosityClass method
*
* Revision 1.22  2005/02/04 09:50:12  gzins
* Fixed major bug in RA and DEC convertion (sign was not correctly handled)
* Merged IRAS and MSX IR flux properties
*
* Revision 1.21  2005/02/04 08:09:28  gzins
* Added star properties for MIDI catalog
*
* Revision 1.20  2005/02/04 07:44:43  gzins
* Limited number of logged messages for test
*
* Revision 1.19  2005/01/27 13:44:41  scetre
* update documentation in IsSame method
*
* Revision 1.18  2005/01/26 14:11:42  scetre
* change assignement operator and pass list as private member of the class
* vobsSTAR
*
* Revision 1.17  2005/01/26 08:50:25  scetre
* upated documentation
*
* Revision 1.16  2005/01/26 08:17:54  scetre
* Change History, add documentation, add checking method on property.
* Change problem (vobsSTAR_POS_EQ_RA_MAIN -> ...DEC_MAIN). Update IsSame Method
*
* scetre    22-Jul-2004  Created
*
*
*******************************************************************************/

/**
 * @file
 * Definition of vobsSTAR class.
 */


static char *rcsId __attribute__ ((unused)) ="@(#) $Id: vobsSTAR.cpp,v 1.75 2008-09-24 11:34:59 lafrasse Exp $"; 

/*
 * System Headers
 */
#include <math.h>
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
#include "vobsSTAR.h"
#include "vobsPrivate.h"
#include "vobsErrors.h"

/*
 * Class constructor
 */
/**
 * Build a star object.
 */
vobsSTAR::vobsSTAR()
{
    // Add all star properties
    AddProperties();

    _propertyListIterator  = _propertyList.begin();
    _propertyOrderIterator = _propertyOrder.begin();
}

/**
 * Build a star object from another one (copy constructor).
 */
vobsSTAR::vobsSTAR(vobsSTAR &star)
{
    // Uses the operator=() method to copy
    *this = star;
}

/**
 * Assignement operator
 */
vobsSTAR&vobsSTAR::operator=(const vobsSTAR&star)
{
    logTrace("vobsSTAR::operator=()");
   
    // Clear the 2 internal maps
    _propertyList.erase(_propertyList.begin(), _propertyList.end());
    _propertyOrder.erase(_propertyOrder.begin(), _propertyOrder.end());
    
    // Copy in the 2 maps from the given star
    _propertyList  = star._propertyList;
    _propertyOrder = star._propertyOrder;

    return *this;
}


/*
 * Class destructor
 */
/**
 * Delete a star object.
 */
vobsSTAR::~vobsSTAR()
{
}


/*
 * Public methods
 */
/**
 * Set the charater value of a given property.
 *
 * @param id property id
 * @param value property value
 * @param origin the origin of the value (catalog, computed, ...)
 * @param confidenceIndex value confidence index
 * @param overwrite booleen to know if it is an overwrite property
 *
 * @return mcsSUCCESS on successful completion, mcsFAILURE otherwise.
 *
 * @b Error codes:@n
 * The possible errors are :
 * @li vobsERR_INVALID_PROPERTY_ID
 */
mcsCOMPL_STAT vobsSTAR::SetPropertyValue(const char *id,
                                         const char *value,
                                         const char *origin,
                                         vobsCONFIDENCE_INDEX confidenceIndex,
                                         mcsLOGICAL overwrite)
{
    logTrace("vobsSTAR::SetPropertyValue(char*)");

    // Look for the given property
    map<string, vobsSTAR_PROPERTY>::iterator propertyIter;
    propertyIter = _propertyList.find(id);

    // If no property with the given Id was found
    if (propertyIter == _propertyList.end())
    {
        // Raise an error
        errAdd(vobsERR_INVALID_PROPERTY_ID, id);
        return mcsFAILURE;
    }

    // Set this property value
    if (propertyIter->second.SetValue(value, origin, confidenceIndex, overwrite)
        == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    return mcsSUCCESS;
}

/**
 * Set the charater value of a given property.
 *
 * @param id property id
 * @param value property value
 * @param origin the origin of the value (catalog, computed, ...)
 * @param confidenceIndex value confidence index
 * @param overwrite booleen to know if it is an overwrite property
 *
 * @return mcsSUCCESS on successful completion, mcsFAILURE otherwise.
 *
 * @b Error codes:@n
 * The possible errors are :
 * @li vobsERR_INVALID_PROPERTY_ID
 */
mcsCOMPL_STAT vobsSTAR::SetPropertyValue(const char *id,
                                         mcsFLOAT value,
                                         const char *origin,
                                         vobsCONFIDENCE_INDEX confidenceIndex,
                                         mcsLOGICAL overwrite)
{
    logTrace("vobsSTAR::SetPropertyValue(float)");

    
    // Look for the given property
    map<string, vobsSTAR_PROPERTY>::iterator propertyIter;
    propertyIter = _propertyList.find(id);

    // If no property with the given Id was found
    if (propertyIter == _propertyList.end())
    {
        // Raise an error
        errAdd(vobsERR_INVALID_PROPERTY_ID, id);
        return mcsFAILURE;
    }

    // Set this property value
    if (propertyIter->second.SetValue(value, origin, confidenceIndex, overwrite) 
        == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    return mcsSUCCESS;
}

/**
 * Clear the charater value of a given property.
 *
 * @param id property id
 *
 * @return mcsSUCCESS on successful completion, mcsFAILURE otherwise.
 *
 * @b Error codes:@n
 * The possible errors are :
 * @li vobsERR_INVALID_PROPERTY_ID
 */
mcsCOMPL_STAT vobsSTAR::ClearPropertyValue(const char *id)
{
    logTrace("vobsSTAR::ClearPropertyValue()");

    
    // Look for the given property
    map<string, vobsSTAR_PROPERTY>::iterator propertyIter;
    propertyIter = _propertyList.find(id);

    // If no property with the given Id was found
    if (propertyIter == _propertyList.end())
    {
        // Raise an error
        errAdd(vobsERR_INVALID_PROPERTY_ID, id);
        return mcsFAILURE;
    }

    // Set this property value
    if (propertyIter->second.ClearValue() == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    return mcsSUCCESS;
}


/**
 * Get the star property corresponding to the given UCD.
 *
 * @param id property id.
 *
 * @return pointer on the found star property object on successful completion.
 * Otherwise NULL is returned.
 *
 * @b Error codes:@n
 * The possible errors are :
 * @li vobsERR_INVALID_PROPERTY_ID
 */
vobsSTAR_PROPERTY* vobsSTAR::GetProperty(char *id)
{
    logTrace("vobsSTAR::GetProperty()");

    // Look for property
    map<string, vobsSTAR_PROPERTY> ::iterator propertyIter;
    propertyIter = _propertyList.find(id);

    // If no property with the given Id was found
    if (propertyIter == _propertyList.end())
    {
        // Raise an error
        errAdd(vobsERR_INVALID_PROPERTY_ID, id);
        return NULL;
    }

    // Return a pointer on the found property
    return &propertyIter->second;
}

/**
 * Return the next property in the list.
 *
 * This method returns a pointer on the next element of the list.
 *
 * @param init if mcsTRUE, returns the first element of the list.
 *
 * This method can be used to move forward in the list, as shown below:
 * @code
 * ...
 * for (unsigned int el = 0; el < star.NbProperties(); el++)
 * {
 *     printf("%s",star.GetNextProperty((mcsLOGICAL)(el==0))->GetName());
 * }
 * ...
 * @endcode
 *
 * @return pointer to the next element of the list, or NULL if the end of the
 * list is reached.
 */
vobsSTAR_PROPERTY *vobsSTAR::GetNextProperty(mcsLOGICAL init)
{
    logTrace("vobsSTAR::GetNextProperty()");

    // if the logical value of the parameter, init is mcsTRUE, the wanted value
    // is the first
    if (init == mcsTRUE)
    {
        _propertyOrderIterator=_propertyOrder.begin();
    }
    else
    {
        // Increase the iterator to the following position
        _propertyOrderIterator++;

        // If this reached the end of the list
        if ( _propertyOrderIterator == _propertyOrder.end())
        {
            return NULL;
        }
    }

    // Once the iterator is put on the desired property
    _propertyListIterator = _propertyList.find(_propertyOrderIterator->second);

    // Return the value of the property
    return &(_propertyListIterator->second);
}


/**
 * Get a property character value.
 *
 * @param id property id.
 *
 * @return pointer to the found star property value on successful completion.
 * Otherwise NULL is returned.
 */
const char *vobsSTAR::GetPropertyValue(char *id)
{
    logTrace("(char*)vobsSTAR::GetPropertyValue()");

    // Look for property
    vobsSTAR_PROPERTY* property = GetProperty(id);
    if (property == NULL)
    {
        // Return error
        return NULL;
    }

    // Return the property value
    return (property->GetValue());
}

/**
 * Get a star property float value.
 *
 * @param id property id.
 * @param value pointer to store value.
 *
 * @return mcsSUCCESS on successfull completion, mcsFAILURE otherwise.
 */
mcsCOMPL_STAT vobsSTAR::GetPropertyValue(char *id, mcsFLOAT *value)
{
    logTrace("vobsSTAR::GetPropertyValue(float*)");

    // Look for property
    vobsSTAR_PROPERTY* property = GetProperty(id);
    if (property == NULL)
    {
        // Return error
        return mcsFAILURE;
    }

    // Return the property value
    return (property->GetValue(value));
}

/**
 * Get a star property type.
 *
 * @sa vobsSTAR_PROPERTY
 *
 * @param id property id.
 *
 * @return property type.
 */
vobsPROPERTY_TYPE vobsSTAR::GetPropertyType(char *id)
{
    logTrace("vobsSTAR::GetPropertyType()");

    // Look for property
    vobsSTAR_PROPERTY *property;
    property = GetProperty(id);

    // Return property
    return (property->GetType());
}

/**
 * Get a star property confidence index.
 *
 * @sa vobsSTAR_PROPERTY
 *
 * @param id property id.
 *
 * @return property confidence index.
 */
vobsCONFIDENCE_INDEX vobsSTAR::GetPropertyConfIndex(char *id)
{
    logTrace("vobsSTAR::GetPropertyConfIndex()");

    // Look for property
    vobsSTAR_PROPERTY *property;
    property = GetProperty(id);

    // Return property confidence index
    return (property->GetConfidenceIndex());
}

/**
 * Check whether the property is set or not.
 *
 * @param id property id.
 *
 * @warning If the given property id is unknown, this method returns mcsFALSE.
 *
 * @return mcsTRUE if the the property has been set, mcsFALSE otherwise.
 */
mcsLOGICAL vobsSTAR::IsPropertySet(char *id)
{
    logTrace("vobsSTAR::IsPropertySet()");

    // Look for the property
    vobsSTAR_PROPERTY *property = GetProperty(id);
    if (property == NULL)
    {
        return mcsFALSE;
    }

    return (property->IsSet());
}

/**
 * Return whether a name correspond to a property.
 *
 * @param id property id.
 *
 * @return mcsTRUE) if the the property is known, mcsFALSE otherwise.
 */
mcsLOGICAL vobsSTAR::IsProperty(char *id)
{
    logTrace("vobsSTAR::IsProperty()");

    // Look for property
    map<string, vobsSTAR_PROPERTY>::iterator propertyIter;
    propertyIter = _propertyList.find(id);
    if (propertyIter == _propertyList.end())
    {
        return mcsFALSE;
    }

    return mcsTRUE;
}


/**
 * Get right ascension (RA) coordinate in arcseconds.
 *
 * @param ra pointer on an already allocated float value.
 *
 * @return mcsSUCCESS on successful completion, mcsFAILURE otherwise.
 */
mcsCOMPL_STAT vobsSTAR::GetRa(float &ra)
{
    logTrace("vobsSTAR::GetRa()");

    mcsSTRING64 raHms;
    mcsFLOAT    hh, hm, hs;

    // Check if the value is set
    if (IsPropertySet(vobsSTAR_POS_EQ_RA_MAIN) == mcsFALSE)
    {
        // if not, return error
        errAdd(vobsERR_RA_NOT_SET);
        return mcsFAILURE;
    }

    // RA can be given as HH:MM:SS.TT or HH MM SS.TT. 
    // Replace ':' by ' ', and remove trailing and leading pace
    strcpy(raHms, GetPropertyValue(vobsSTAR_POS_EQ_RA_MAIN));
    if (miscReplaceChrByChr(raHms, ':', ' ') == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    if (miscTrimString(raHms, " ") == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    if (sscanf(raHms, "%f %f %f", &hh, &hm, &hs) != 3)
    {
        errAdd(vobsERR_INVALID_RA_FORMAT, raHms);
        return mcsFAILURE;
    }

    // Get sign of hh which has to be propagated to hm and hs
    mcsFLOAT sign;
    sign = (raHms[0] == '-') ? -1.0 : 1.0;

    // Convert to degrees
    ra  = (hh + sign*hm/60.0 + sign*hs/3600.0) * 15.0;

    // Set angle range [-180 - 180]
    if (ra > 180)
    {
        ra = -1.0 * (360 - ra);
    }

    return mcsSUCCESS;
}

/**
 * Get declinaison (DEC) coordinate in arcseconds.
 *
 * @param dec pointer on an already allocated float value.
 *
 * @return mcsSUCCESS on successful completion, mcsFAILURE otherwise.
 */
mcsCOMPL_STAT vobsSTAR::GetDec(float &dec)
{
    logTrace("vobsSTAR::GetDec()");

    mcsSTRING64 decDms;
    float dd,dm,ds;

    // Check if the value is set
    if (IsPropertySet(vobsSTAR_POS_EQ_DEC_MAIN) == mcsFALSE)
    {
        // if not, return error
        errAdd(vobsERR_DEC_NOT_SET);
        return mcsFAILURE;
    }

    // DEC can be given as DD:MM:SS.TT or DD MM SS.TT. 
    // Replace ':' by ' ', and remove trailing and leading pace
    strcpy(decDms, GetPropertyValue(vobsSTAR_POS_EQ_DEC_MAIN));
    if (miscReplaceChrByChr(decDms, ':', ' ') == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    if (miscTrimString(decDms, " ") == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    if (sscanf(decDms, "%f %f %f", &dd, &dm, &ds) != 3)
    {
        errAdd(vobsERR_INVALID_DEC_FORMAT, decDms);
        return mcsFAILURE;
    }

    // Get sign of hh which has to be propagated to hm and hs
    mcsFLOAT sign;
    sign = (decDms[0] == '-') ? -1.0 : 1.0; 

    // Convert to degrees
    dec  = dd + sign*dm/60.0 + sign*ds/3600.0;

    return mcsSUCCESS;
}

/**
 * Get star Id, based on the catalog it comes from.
 *
 * @param starId a pointer on an already allocated character buffer.
 * @param maxLength the size of the external character buffer.
 *
 * @return mcsSUCCESS on successful completion, mcsFAILURE otherwise.
 */
mcsCOMPL_STAT vobsSTAR::GetId(char* starId, const mcsUINT32 maxLength)
{
    logTrace("vobsSTAR::GetId()");

    const char* propertyValue = NULL;

    if (IsPropertySet(vobsSTAR_ID_HD) == mcsTRUE)
    {
        propertyValue = GetPropertyValue(vobsSTAR_ID_HD);
        if (propertyValue != NULL)
        {
            snprintf(starId, (maxLength - 1), "HD %s", propertyValue);
            return mcsSUCCESS;
        }
    }

    if (IsPropertySet(vobsSTAR_ID_HIP) == mcsTRUE)
    {
        propertyValue = GetPropertyValue(vobsSTAR_ID_HIP);
        if (propertyValue != NULL)
        {
            snprintf(starId, (maxLength - 1), "HIP %s", propertyValue);
            return mcsSUCCESS;
        }
    }

    if (IsPropertySet(vobsSTAR_ID_DM) == mcsTRUE)
    {
        propertyValue = GetPropertyValue(vobsSTAR_ID_DM);
        if (propertyValue != NULL)
        {
            snprintf(starId, (maxLength - 1), "DM %s", propertyValue);
            return mcsSUCCESS;
        }
    }

    if (IsPropertySet(vobsSTAR_ID_TYC1) == mcsTRUE)
    {
        propertyValue = GetPropertyValue(vobsSTAR_ID_TYC1);
        if (propertyValue != NULL)
        {
            snprintf(starId, (maxLength - 1), "TYC1 %s", propertyValue);
            return mcsSUCCESS;
        }
    }
    
    if (IsPropertySet(vobsSTAR_ID_2MASS) == mcsTRUE)
    {
        propertyValue = GetPropertyValue(vobsSTAR_ID_2MASS);
        if (propertyValue != NULL)
        {
            snprintf(starId, (maxLength - 1), "2MASS %s", propertyValue);
            return mcsSUCCESS;
        }
    }

    if (IsPropertySet(vobsSTAR_ID_DENIS) == mcsTRUE)
    {
        propertyValue = GetPropertyValue(vobsSTAR_ID_DENIS);
        if (propertyValue != NULL)
        {
            snprintf(starId, (maxLength - 1), "DENIS %s", propertyValue);
            return mcsSUCCESS;
        }
    }

    if (IsPropertySet(vobsSTAR_ID_TYC2) == mcsTRUE)
    {
        propertyValue = GetPropertyValue(vobsSTAR_ID_TYC2);
        if (propertyValue != NULL)
        {
            snprintf(starId, (maxLength - 1), "TYC2 %s", propertyValue);
            return mcsSUCCESS;
        }
    }

    if (IsPropertySet(vobsSTAR_ID_TYC3) == mcsTRUE)
    {
        propertyValue = GetPropertyValue(vobsSTAR_ID_TYC3);
        if (propertyValue != NULL)
        {
            snprintf(starId, (maxLength - 1), "TYC3 %s", propertyValue);
            return mcsSUCCESS;
        }
    }

    const char* raValue  = NULL;
    const char* decValue = NULL;
    if ((IsPropertySet(vobsSTAR_POS_EQ_RA_MAIN)  == mcsTRUE) && 
        (IsPropertySet(vobsSTAR_POS_EQ_DEC_MAIN) == mcsTRUE))
    {
        raValue  = GetPropertyValue(vobsSTAR_POS_EQ_RA_MAIN);
        decValue = GetPropertyValue(vobsSTAR_POS_EQ_DEC_MAIN);
        snprintf(starId, (maxLength - 1), "Coordinates-ra=%s/dec=%s",
                 raValue, decValue);
        return mcsSUCCESS;        
    }
    
    // No id found
    strncpy(starId, "????", (maxLength - 1));

    return mcsSUCCESS;
}

/**
 * Return whether the star is the same as another given one.
 *
 * @param star the other star.
 * @param criteriaList the list of comparison criterias to be used (the
 * comparison will be based on the stars coordinates if NULL is given as value).
 *
 * @return mcsTRUE if the stars are the same, mcsFALSE otherwise.
 */
mcsLOGICAL vobsSTAR::IsSame(vobsSTAR &star,
                            vobsSTAR_COMP_CRITERIA_LIST *criteriaList)
{
    logTrace("vobsSTAR::IsSame(0x%x)", criteriaList);

    // Check if the criteria list is empty
    if (criteriaList == NULL)
    {
        mcsFLOAT ra1, ra2, dec1, dec2;

        // Get right ascension of the star. If not set return FALSE
        if (IsPropertySet(vobsSTAR_POS_EQ_RA_MAIN) == mcsTRUE)
        {
            if (GetRa(ra1) == mcsFAILURE)
            {
                errCloseStack();
                return mcsFALSE;
            }
        }
        else
        {
            return mcsFALSE;
        }

        if (star.IsPropertySet(vobsSTAR_POS_EQ_RA_MAIN) == mcsTRUE)
        {
            if (star.GetRa(ra2) == mcsFAILURE)
            {
                errCloseStack();
                return mcsFALSE;
            }
        }
        else
        {
            return mcsFALSE;
        }

        // Get declinaison of the star. If not set return FALSE
        if (IsPropertySet(vobsSTAR_POS_EQ_DEC_MAIN) == mcsTRUE)
        {
            if (GetDec(dec1) == mcsFAILURE)
            {
                errCloseStack();
                return mcsFALSE;
            }
        }
        else
        {
            return mcsFALSE;
        }

        if (star.IsPropertySet(vobsSTAR_POS_EQ_DEC_MAIN) == mcsTRUE)
        {
            if (star.GetDec(dec2) == mcsFAILURE)
            {
                errCloseStack();
                return mcsFALSE;
            }
        }
        else
        {
            return mcsFALSE;
        }

        // Compare coordinates
        if ((ra1 == ra2) && (dec1==dec2))
        {
            return mcsTRUE;
        }
        else
        {
            return mcsFALSE;
        }
    }
    // if criteria list is not empty
    else
    {
        const char *hd;
        mcsSTRING64 propertyId;
        mcsFLOAT range;
        // Get the size of the criteria list
        int listSize=criteriaList->Size();
        mcsFLOAT val1, val2;    
        // Get each criteria of the list and check if the comparaison with all
        // this criteria gave a equality
        hd = GetPropertyValue(vobsSTAR_ID_HD);
        for (int el = 0; el < listSize; el++)
        {
            if (criteriaList->GetNextCriteria
                (propertyId, &range, (mcsLOGICAL)(el==0)) == mcsFAILURE)
            {
                return mcsFALSE;
            }
            logDebug("%s: %s delta is in +/- %.3f?", hd, propertyId, range);
            if (strcmp(propertyId, vobsSTAR_POS_EQ_RA_MAIN) == 0)
            {
                // Get right ascension of the stars. If not set return FALSE
                if (IsPropertySet(vobsSTAR_POS_EQ_RA_MAIN) == mcsTRUE)
                {
                    if (GetRa(val1) == mcsFAILURE)
                    {
                        errCloseStack();
                        return mcsFALSE;
                    }
                }
                else
                {
                    return mcsFALSE;
                }
                if (star.IsPropertySet(vobsSTAR_POS_EQ_RA_MAIN) == mcsTRUE)
                {
                    if (star.GetRa(val2) == mcsFAILURE)
                    {
                        errCloseStack();
                        return mcsFALSE;
                    }
                }
                else
                {
                    return mcsFALSE;
                }
            }
            else if(strcmp(propertyId, vobsSTAR_POS_EQ_DEC_MAIN) == 0)
            {
                // Get declinaison of the stars. If not set return FALSE
                if (IsPropertySet(vobsSTAR_POS_EQ_DEC_MAIN) == mcsTRUE)
                {
                    if (GetDec(val1) == mcsFAILURE)
                    {
                        errCloseStack();
                        return mcsFALSE;
                    }
                }
                else
                {
                    return mcsFALSE;
                }
                if (star.IsPropertySet(vobsSTAR_POS_EQ_DEC_MAIN) == mcsTRUE)
                {
                    if (star.GetDec(val2) == mcsFAILURE)
                    {
                        errCloseStack();
                        return mcsFALSE;
                    }
                }
                else
                {
                    return mcsFALSE;
                }
            }
            // If property is a sting
            else if (GetProperty(propertyId)->GetType() == vobsSTRING_PROPERTY)
            {
                const char *val1Str, *val2Str;
                // Get value of the property id
                if (IsPropertySet(propertyId) == mcsTRUE)
                {
                    val1Str = GetPropertyValue(propertyId);
                    if (val1Str == NULL)
                    {
                        errCloseStack();
                        return mcsFALSE;
                    }
                }
                else
                {
                    return mcsFALSE;
                }    
                if (star.IsPropertySet(propertyId) == mcsTRUE)
                {
                    val2Str = star.GetPropertyValue(propertyId);
                    if (val2Str == NULL)
                    {
                        errCloseStack();
                        return mcsFALSE;
                    } 
                }
                else
                {
                    return mcsFALSE;
                }    
                if (strcmp(val1Str, val2Str) == 0)
                {
                    return mcsTRUE;
                }
                else
                {
                    return mcsFALSE;
                }
            }
            else
            {
                // Get value of the property id
                if (IsPropertySet(propertyId) == mcsTRUE)
                {
                    if (GetPropertyValue(propertyId, &val1) == mcsFAILURE)
                    {
                        errCloseStack();
                        return mcsFALSE;
                    }
                }
                else
                {
                    return mcsFALSE;
                }    
                if (star.IsPropertySet(propertyId) == mcsTRUE)
                {
                    if (star.GetPropertyValue(propertyId, &val2) == mcsFAILURE)
                    {
                        errCloseStack();
                        return mcsFALSE;
                    } 
                }
                else
                {
                    return mcsFALSE;
                }    
            }
            logDebug("%s: %s delta = (%.3f - %.3f) %.3f", 
                     hd, propertyId, val1, val2, fabs(val1-val2));
            if (fabs(val1-val2) > range)
            {
                return mcsFALSE;
            }
        }
        return mcsTRUE;
    }
}

/**
 * Update a star with the properies of another given one.
 *
 * @param star the other star.
 *
 * @return always mcsSUCCESS.
 *
 */
mcsCOMPL_STAT vobsSTAR::Update (vobsSTAR &star)
{
    logTrace("vobsSTAR::Update()");

    // For each star property
    map<string, vobsSTAR_PROPERTY > ::iterator propertyIter;
    for (propertyIter  = _propertyList.begin();
         propertyIter != _propertyList.end();
         propertyIter++)
    {
        /*
         * If the current property can be filled with the one coming from the
         * given star
         */
        if (IsPropertySet((char *)(*propertyIter).first.c_str()) == mcsFALSE)
        {
            // Copy the property form the given star inside
            _propertyList[(*propertyIter).first] =
                star._propertyList[(*propertyIter).first];
        }
    }

    return mcsSUCCESS;
}

/**
 * Return the number of properties
 *
 * @return the number of properties of the star
 */
mcsINT32 vobsSTAR::NbProperties()
{
    return _propertyList.size();
}

/**
 * Display all star properties on the console.
 *
 * @param showPropId displays each star property in a form \<propId\> =
 * \<value\> if mcsTRUE, otherwise all properties on a single line if mcsFALSE.
 */
void vobsSTAR::Display(mcsLOGICAL showPropId)
{
    logTrace("vobsSTAR::Display()");

    map<string, vobsSTAR_PROPERTY > ::iterator propertyIter;
    mcsSTRING64 starId;
    mcsFLOAT    starRa  = 0.0;
    mcsFLOAT    starDec = 0.0;

    GetId(starId, sizeof(starId));

    if (IsPropertySet(vobsSTAR_POS_EQ_RA_MAIN) == mcsTRUE)
    {
        GetRa(starRa);
    }
    if (IsPropertySet(vobsSTAR_POS_EQ_DEC_MAIN) == mcsTRUE)
    {
        GetDec(starDec);
    }
    printf("%s(%f,%f): ", starId, starRa, starDec);

    if (showPropId == mcsFALSE)
    {
        for (propertyIter  = _propertyList.begin();
             propertyIter != _propertyList.end();
             propertyIter++)
        {
            printf("%12s", (*propertyIter).second.GetValue());
        }
        printf("\n");
    }
    else
    {
        for (propertyIter  = _propertyList.begin();
             propertyIter != _propertyList.end();
             propertyIter++)
        {
            printf("%12s = %12s\n", (*propertyIter).second.GetId(),
                   (*propertyIter).second.GetValue());
        }
    }
}


/*
 * Protected methods
 */
/**
 * Add a star property
 *
 * @param id     property identifier
 * @param name   property name
 * @param type   property type
 * @param unit   property unit
 * @param format format used to set property
 * @param link link for the VOTable
 * @param description property description
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is 
 * returned.
 */
mcsCOMPL_STAT vobsSTAR::AddProperty(char *id, char *name,
                                    vobsPROPERTY_TYPE type, char *unit,
                                    char *format, char *link, char *description)
{
    logTrace("vobsSTAR::AddProperty()");

    // Verify that the desired property does not already exist
    if (_propertyList.find(id) != _propertyList.end())
    {
        errAdd(vobsERR_DUPLICATED_PROPERTY, id);
        return mcsFAILURE;
    }

    // Create a new property from the given parameters
    vobsSTAR_PROPERTY property(id, name, type, unit, format, link, description);

    // Add the new property to the internal list
    _propertyList[id] = property;
    _propertyOrder[_propertyList.size()-1] = id;

    return mcsSUCCESS;
}

/**
 * Add all star properties and fix an order
 *
 * @return mcsSUCCESS
 */
mcsCOMPL_STAT vobsSTAR::AddProperties(void)
{
    logTrace("vobsSTAR::AddProperties()");

    AddProperty(vobsSTAR_ID_HD, "HD", vobsSTRING_PROPERTY, "-", "%.0f",
                "http://simbad.u-strasbg.fr/simbad/sim-id?protocol=html&amp;Ident=HD${HD}&amp;NbIdent=1&amp;Radius=1&amp;Radius.unit=arcsec",
                "HD identifier, click to call Simbad on this object");
    AddProperty(vobsSTAR_ID_HIP, "HIP", vobsSTRING_PROPERTY, "-");  
    AddProperty(vobsSTAR_ID_DM, "DM", vobsSTRING_PROPERTY, "-");  
    AddProperty(vobsSTAR_ID_TYC1, "TYC1", vobsSTRING_PROPERTY, "-");  
    AddProperty(vobsSTAR_ID_TYC2, "TYC2", vobsSTRING_PROPERTY, "-");  
    AddProperty(vobsSTAR_ID_TYC3, "TYC3", vobsSTRING_PROPERTY, "-");  
    AddProperty(vobsSTAR_ID_CATALOG, "opt", vobsSTRING_PROPERTY, "-");    
    AddProperty(vobsSTAR_ID_2MASS, "2MASS", vobsSTRING_PROPERTY, "-", NULL,
                "http://cdsweb.u-strasbg.fr/viz-bin/VizieR-4?-source=II/246/out&amp;-out=2MASS&amp;2MASS=${2MASS}&amp;-out=Hmag&amp;-out=e_Hmag&amp;-out=Kmag&amp;-out=e_Kmag&amp;-out=Qflg&amp;-out=Rflg&amp;-out=Bflg&amp;-out=Cflg&amp;-out=Xflg&amp;-out=Aflg-meta.ucd=0",
                "2MASS identifier, click to call Vizier on this object");
    AddProperty(vobsSTAR_POS_EQ_RA_MAIN, "RAJ2000", vobsSTRING_PROPERTY,
                "h:m:s", NULL, "http://simbad.u-strasbg.fr/simbad/sim-id?protocol=html&amp;Ident=${RAJ2000}%20${DEJ2000}&amp;NbIdent=1&amp;Radius=1&amp;Radius.unit=arcsec",
                "Click to call Simbad based on those coordinates");
    AddProperty(vobsSTAR_POS_EQ_DEC_MAIN, "DEJ2000", vobsSTRING_PROPERTY,
                "d:m:s", NULL, "http://simbad.u-strasbg.fr/simbad/sim-id?protocol=html&amp;Ident=${RAJ2000}%20${DEJ2000}&amp;NbIdent=1&amp;Radius=1&amp;Radius.unit=arcsec",
                "Click to call Simbad based on those coordinates");
    AddProperty(vobsSTAR_ID_DENIS, "DENIS", vobsSTRING_PROPERTY, "-");    
    AddProperty(vobsSTAR_POS_EQ_RA_OTHER, "A2RAdeg", vobsSTRING_PROPERTY, 
                "h:m:s");
    AddProperty(vobsSTAR_POS_EQ_DEC_OTHER, "A2DEdeg", vobsSTRING_PROPERTY,
                "d:m:s");
    AddProperty(vobsSTAR_POS_EQ_PMDEC, "pmDec", vobsSTRING_PROPERTY, "mas/yr");
    AddProperty(vobsSTAR_POS_EQ_PMRA, "pmRa", vobsSTRING_PROPERTY, "mas/yr");
    AddProperty(vobsSTAR_POS_PARLX_TRIG, "plx", vobsFLOAT_PROPERTY, "mas",
                "%.2f");
    AddProperty(vobsSTAR_SPECT_TYPE_MK, "SpType", vobsSTRING_PROPERTY, "-");
    AddProperty(vobsSTAR_CODE_VARIAB_V1, "VarFlag1", vobsSTRING_PROPERTY, "-");
    AddProperty(vobsSTAR_CODE_VARIAB_V2, "VarFlag2", vobsSTRING_PROPERTY, "-");
    AddProperty(vobsSTAR_CODE_VARIAB_V3, "VarFlag3", vobsSTRING_PROPERTY, "-");
    AddProperty(vobsSTAR_CODE_MULT_FLAG, "MultFlag", vobsSTRING_PROPERTY, "-");
    AddProperty(vobsSTAR_CODE_MISC_I, "Iflag", vobsSTRING_PROPERTY, "-");
    AddProperty(vobsSTAR_CODE_MISC_J, "Jflag", vobsSTRING_PROPERTY, "-");
    AddProperty(vobsSTAR_CODE_MISC_K, "Kflag", vobsSTRING_PROPERTY, "-");
    AddProperty(vobsSTAR_CODE_QUALITY, "Qflag", vobsSTRING_PROPERTY, "-");
    AddProperty(vobsSTAR_POS_GAL_LAT, "GLAT", vobsFLOAT_PROPERTY, "deg",
                "%.2f");
    AddProperty(vobsSTAR_POS_GAL_LON, "GLON", vobsFLOAT_PROPERTY, "deg",
                "%.2f");
    AddProperty(vobsSTAR_VELOC_HC, "RadVel", vobsSTRING_PROPERTY, "-");
    AddProperty(vobsSTAR_LD_DIAM, "LD", vobsFLOAT_PROPERTY, "-", "%.3f");
    AddProperty(vobsSTAR_LD_DIAM_ERROR, "e_LD", vobsFLOAT_PROPERTY, "-",
                "%.3f");
    AddProperty(vobsSTAR_UD_DIAM, "UD", vobsFLOAT_PROPERTY, "-", "%.3f");
    AddProperty(vobsSTAR_UD_DIAM_ERROR, "e_UD", vobsFLOAT_PROPERTY, "-",
                "%.3f");
    AddProperty(vobsSTAR_UDDK_DIAM, "UDDK", vobsFLOAT_PROPERTY, "-", "%.3f");
    AddProperty(vobsSTAR_UDDK_DIAM_ERROR, "e_UDDK", vobsFLOAT_PROPERTY, "-",
                "%.3f");
    AddProperty(vobsSTAR_DIAM12, "Dia12", vobsFLOAT_PROPERTY, "-", "%.3f");
    AddProperty(vobsSTAR_DIAM12_ERROR, "e_dia12", vobsFLOAT_PROPERTY, "-",
                "%.3f");
    AddProperty(vobsSTAR_OBS_METHOD, "Meth", vobsSTRING_PROPERTY, "-");
    AddProperty(vobsSTAR_INST_WAVELENGTH_VALUE, "lambda", vobsFLOAT_PROPERTY,
                "-");
    AddProperty(vobsSTAR_INST_FILTER_CODE, "lambda", vobsSTRING_PROPERTY, "-");
    AddProperty(vobsSTAR_PHOT_FLUX_IR_MISC, "photflux", vobsSTRING_PROPERTY,
                "-");
    AddProperty(vobsSTAR_UNITS, "units", vobsSTRING_PROPERTY, "-");
    AddProperty(vobsSTAR_PHOT_JHN_B, "B", vobsFLOAT_PROPERTY, "mag", "%.3f");
    AddProperty(vobsSTAR_PHOT_PHG_B, "Bphg", vobsFLOAT_PROPERTY, "mag", "%.3f");
    AddProperty(vobsSTAR_PHOT_JHN_V, "V", vobsFLOAT_PROPERTY, "mag", "%.3f");
    AddProperty(vobsSTAR_PHOT_PHG_V, "Vphg", vobsFLOAT_PROPERTY, "mag", "%.3f");
    AddProperty(vobsSTAR_PHOT_JHN_R, "R", vobsFLOAT_PROPERTY, "mag", "%.3f");
    AddProperty(vobsSTAR_PHOT_PHG_R, "Rphg", vobsFLOAT_PROPERTY, "mag", "%.3f");
    AddProperty(vobsSTAR_PHOT_JHN_I, "I", vobsFLOAT_PROPERTY, "mag", "%.3f");
    AddProperty(vobsSTAR_PHOT_PHG_I, "Iphg", vobsFLOAT_PROPERTY, "mag", "%.3f");
    AddProperty(vobsSTAR_PHOT_COUS_I, "Icous", vobsFLOAT_PROPERTY, "mag",
                "%.3f");
    AddProperty(vobsSTAR_PHOT_JHN_J, "J", vobsFLOAT_PROPERTY, "mag", "%.3f");
    AddProperty(vobsSTAR_PHOT_JHN_H, "H", vobsFLOAT_PROPERTY, "mag", "%.3f");
    AddProperty(vobsSTAR_PHOT_JHN_K, "K", vobsFLOAT_PROPERTY, "mag", "%.3f");
    AddProperty(vobsSTAR_PHOT_JHN_L, "L", vobsFLOAT_PROPERTY, "mag", "%.3f");
    AddProperty(vobsSTAR_PHOT_JHN_M, "M", vobsFLOAT_PROPERTY, "mag", "%.3f");
    AddProperty(vobsSTAR_PHOT_JHN_N, "N", vobsFLOAT_PROPERTY, "mag", "%.3f");
    AddProperty(vobsSTAR_VELOC_ROTAT, "RotVel", vobsSTRING_PROPERTY, "-");
    AddProperty(vobsSTAR_PHOT_COLOR_EXCESS, "color", vobsSTRING_PROPERTY, "-");
    AddProperty(vobsSTAR_IR_FLUX_ORIGIN, "orig", vobsSTRING_PROPERTY, "-");
    AddProperty(vobsSTAR_POS_PARLX_TRIG_ERROR, "e_Plx",  vobsFLOAT_PROPERTY, "-", 
                "%.3f");
    AddProperty(vobsSTAR_PHOT_FLUX_IR_12, "F12",  vobsFLOAT_PROPERTY, "-", 
                "%.3f");
    AddProperty(vobsSTAR_PHOT_FLUX_IR_12_ERROR, "e_F12",  
                vobsFLOAT_PROPERTY, "-", "%.3f");
    AddProperty(vobsSTAR_REF_STAR, "Calib", vobsSTRING_PROPERTY, "-");
    AddProperty(vobsSTAR_PHYS_TEMP_EFFEC, "Teff", vobsFLOAT_PROPERTY, "-", 
                "%.3f");
    AddProperty(vobsSTAR_PHYS_TEMP_EFFEC_ERROR, "e_Teff", vobsFLOAT_PROPERTY, "-", "%.3f");
    AddProperty(vobsSTAR_PHOT_EXTINCTION_TOTAL, "A_V", vobsFLOAT_PROPERTY, "-", 
                "%.3f");
    AddProperty(vobsSTAR_CHI2_QUALITY, "Chi2", vobsFLOAT_PROPERTY, "-", "%.3f");
    AddProperty(vobsSTAR_SP_TYP_PHYS_TEMP_EFFEC, "SpTyp_Teff", 
                vobsFLOAT_PROPERTY, "-", "%.3f");


    return mcsSUCCESS;
}


/*___oOo___*/
