/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * @file
 * Definition of vobsSTAR class.
 */

/*
 * System Headers
 */
#include <math.h>
#include <iostream>
#include <stdio.h>
#include <string.h>
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

/* Blanking value used for parsed RA/DEC coordinates */
#define EMPTY_COORD_DEG 1000.


/*
 * Class constructor
 */
/**
 * Build a star object.
 */
vobsSTAR::vobsSTAR()
{
    // define ra/dec to blanking value:
    _ra = EMPTY_COORD_DEG;
    _dec = EMPTY_COORD_DEG;
    
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
    // copy the parsed ra/dec:
    _ra = star._ra;
    _dec = star._dec;
    
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
    // Look for the given property
    vobsSTAR_PROPERTY* property = GetProperty(id);
    if (property == NULL)
    {
        // Return error
        return mcsFAILURE;
    }

    // Set this property value
    if (property->SetValue(value, origin, confidenceIndex, overwrite) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    return mcsSUCCESS;
}

/**
 * Set the character value of a given property.
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
                                         mcsDOUBLE value,
                                         const char *origin,
                                         vobsCONFIDENCE_INDEX confidenceIndex,
                                         mcsLOGICAL overwrite)
{
    // Look for the given property
    vobsSTAR_PROPERTY* property = GetProperty(id);
    if (property == NULL)
    {
        // Return error
        return mcsFAILURE;
    }

    // Set this property value
    if (property->SetValue(value, origin, confidenceIndex, overwrite) == mcsFAILURE)
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
    // Look for the given property
    vobsSTAR_PROPERTY* property = GetProperty(id);
    if (property == NULL)
    {
        // Return error
        return mcsFAILURE;
    }

    // Clear this property value
    if (property->ClearValue() == mcsFAILURE)
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
vobsSTAR_PROPERTY* vobsSTAR::GetProperty(const char* id)
{
    // Look for property
    PropertyMap::iterator propertyIter;
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
const char *vobsSTAR::GetPropertyValue(const char* id)
{
    // Look for property
    vobsSTAR_PROPERTY* property = GetProperty(id);
    
    return GetPropertyValue(property);
}

/**
 * Get a property character value.
 *
 * @param property property to use.
 *
 * @return pointer to the found star property value on successful completion.
 * Otherwise NULL is returned.
 */
const char* vobsSTAR::GetPropertyValue (const vobsSTAR_PROPERTY* property)
{
    if (property == NULL)
    {
        // Return error
        return NULL;
    }

    // Return the property value
    return (property->GetValue());
}

/**
 * Get a star property mcsDOUBLE value.
 *
 * @param id property id.
 * @param value pointer to store value.
 *
 * @return mcsSUCCESS on successfull completion, mcsFAILURE otherwise.
 */
mcsCOMPL_STAT vobsSTAR::GetPropertyValue(const char* id, mcsDOUBLE *value)
{
    // Look for property
    vobsSTAR_PROPERTY* property = GetProperty(id);
    
    return GetPropertyValue(property, value);
}

/**
 * Get a star property mcsDOUBLE value.
 *
 * @param property property to use.
 * @param value pointer to store value.
 *
 * @return mcsSUCCESS on successfull completion, mcsFAILURE otherwise.
 */
mcsCOMPL_STAT vobsSTAR::GetPropertyValue(const vobsSTAR_PROPERTY* property, mcsDOUBLE *value)
{
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
vobsPROPERTY_TYPE vobsSTAR::GetPropertyType(const char* id)
{
    // Look for property
    vobsSTAR_PROPERTY *property = GetProperty(id);
    
    return GetPropertyType(property);
}

/**
 * Get a star property type.
 *
 * @sa vobsSTAR_PROPERTY
 *
 * @param property property to use.
 *
 * @return property type. Otherwise vobsSTRING_PROPERTY is returned.
 */
vobsPROPERTY_TYPE vobsSTAR::GetPropertyType(const vobsSTAR_PROPERTY* property)
{
   if (property == NULL)
    {
        return vobsSTRING_PROPERTY;
    }

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
vobsCONFIDENCE_INDEX vobsSTAR::GetPropertyConfIndex(const char* id)
{
    // Look for property
    vobsSTAR_PROPERTY *property = GetProperty(id);

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
mcsLOGICAL vobsSTAR::IsPropertySet(const char* id)
{
    // Look for the property
    vobsSTAR_PROPERTY *property = GetProperty(id);
    
    return IsPropertySet(property);
}

/**
 * Check whether the property is set or not.
 *
 * @param property property to use.
 *
 * @warning If the given property is NULL, this method returns mcsFALSE.
 *
 * @return mcsTRUE if the the property has been set, mcsFALSE otherwise.
 */
mcsLOGICAL vobsSTAR::IsPropertySet(const vobsSTAR_PROPERTY* property)
{
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
mcsLOGICAL vobsSTAR::IsProperty(const char* id)
{
    // Look for property
    PropertyMap::iterator propertyIter;
    propertyIter = _propertyList.find(id);
    if (propertyIter == _propertyList.end())
    {
        return mcsFALSE;
    }

    return mcsTRUE;
}

/**
 * Return if the RA property is set
 * @return true if the RA coordinate is set
 */
mcsLOGICAL vobsSTAR::IsPropertyRaSet(void)
{
    // use cached ra coordinate:
    if (_ra != EMPTY_COORD_DEG)
    {
        return mcsTRUE;
    }

    // Check if the value is set
    if (IsPropertySet(vobsSTAR_POS_EQ_RA_MAIN) == mcsFALSE)
    {
        // if not, return error
        errAdd(vobsERR_RA_NOT_SET);
        return mcsFALSE;
    }
    
    return mcsTRUE;
}

/**
 * Get right ascension (RA) coordinate in degrees.
 *
 * @param ra pointer on an already allocated mcsDOUBLE value.
 *
 * @return mcsSUCCESS on successful completion, mcsFAILURE otherwise.
 */
mcsCOMPL_STAT vobsSTAR::GetRa(mcsDOUBLE &ra)
{
    // use cached ra coordinate:
    if (_ra != EMPTY_COORD_DEG)
    {
        ra = _ra;
        return mcsSUCCESS;
    }
    
    vobsSTAR_PROPERTY* property = GetProperty(vobsSTAR_POS_EQ_RA_MAIN);

    // Check if the value is set
    if (IsPropertySet(property) == mcsFALSE)
    {
        // if not, return error
        errAdd(vobsERR_RA_NOT_SET);
        return mcsFAILURE;
    }
    
    mcsSTRING64 raHms;
    mcsDOUBLE    hh, hm, hs;

    // RA can be given as HH:MM:SS.TT or HH MM SS.TT. 
    // Replace ':' by ' ', and remove trailing and leading pace
    strcpy(raHms, GetPropertyValue(property));
    
    if (miscReplaceChrByChr(raHms, ':', ' ') == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    if (miscTrimString(raHms, " ") == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    if (sscanf(raHms, "%lf %lf %lf", &hh, &hm, &hs) != 3)
    {
        errAdd(vobsERR_INVALID_RA_FORMAT, raHms);
        return mcsFAILURE;
    }

    // Get sign of hh which has to be propagated to hm and hs
    mcsDOUBLE sign;
    sign = (raHms[0] == '-') ? -1.0 : 1.0;

    // Convert to degrees
    ra  = (hh + sign * hm / 60.0 + sign * hs / 3600.0) * 15.0;

    // Set angle range [-180 - 180]
    if (ra > 180)
    {
        ra = -1.0 * (360 - ra);
    }
    
    // cache value:
    _ra = ra;

    return mcsSUCCESS;
}

/**
 * Return if the RA property is set
 * @return true if the RA coordinate is set
 */
mcsLOGICAL vobsSTAR::IsPropertyDecSet(void)
{
    // use cached ra coordinate:
    if (_dec != EMPTY_COORD_DEG)
    {
        return mcsTRUE;
    }

    // Check if the value is set
    if (IsPropertySet(vobsSTAR_POS_EQ_DEC_MAIN) == mcsFALSE)
    {
        // if not, return error
        errAdd(vobsERR_DEC_NOT_SET);
        return mcsFALSE;
    }
    
    return mcsTRUE;
}

/**
 * Get declinaison (DEC) coordinate in degrees.
 *
 * @param dec pointer on an already allocated mcsDOUBLE value.
 *
 * @return mcsSUCCESS on successful completion, mcsFAILURE otherwise.
 */
mcsCOMPL_STAT vobsSTAR::GetDec(mcsDOUBLE &dec)
{
    // use cached dec coordinate:
    if (_dec != EMPTY_COORD_DEG)
    {
        dec = _dec;
        return mcsSUCCESS;
    }
    
    vobsSTAR_PROPERTY* property = GetProperty(vobsSTAR_POS_EQ_DEC_MAIN);

    // Check if the value is set
    if (IsPropertySet(property) == mcsFALSE)
    {
        // if not, return error
        errAdd(vobsERR_DEC_NOT_SET);
        return mcsFAILURE;
    }

    mcsSTRING64 decDms;
    mcsDOUBLE dd,dm,ds;
    
    // DEC can be given as DD:MM:SS.TT or DD MM SS.TT. 
    // Replace ':' by ' ', and remove trailing and leading pace
    strcpy(decDms, GetPropertyValue(property));
    
    if (miscReplaceChrByChr(decDms, ':', ' ') == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    if (miscTrimString(decDms, " ") == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    if (sscanf(decDms, "%lf %lf %lf", &dd, &dm, &ds) != 3)
    {
        errAdd(vobsERR_INVALID_DEC_FORMAT, decDms);
        return mcsFAILURE;
    }

    // Get sign of hh which has to be propagated to hm and hs
    mcsDOUBLE sign;
    sign = (decDms[0] == '-') ? -1.0 : 1.0; 

    // Convert to degrees
    dec  = dd + sign * dm / 60.0 + sign * ds / 3600.0;
  
    // cache value:    
    _dec = dec;

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
    const char* propertyValue = NULL;
    vobsSTAR_PROPERTY* property = NULL;
    
    property = GetProperty(vobsSTAR_ID_HD);

    if (IsPropertySet(property) == mcsTRUE)
    {
        propertyValue = GetPropertyValue(property);
        if (propertyValue != NULL)
        {
            snprintf(starId, (maxLength - 1), "HD %s", propertyValue);
            return mcsSUCCESS;
        }
    }

    property = GetProperty(vobsSTAR_ID_HIP);

    if (IsPropertySet(property) == mcsTRUE)
    {
        propertyValue = GetPropertyValue(property);
        if (propertyValue != NULL)
        {
            snprintf(starId, (maxLength - 1), "HIP %s", propertyValue);
            return mcsSUCCESS;
        }
    }

    property = GetProperty(vobsSTAR_ID_DM);

    if (IsPropertySet(property) == mcsTRUE)
    {
        propertyValue = GetPropertyValue(property);
        if (propertyValue != NULL)
        {
            snprintf(starId, (maxLength - 1), "DM %s", propertyValue);
            return mcsSUCCESS;
        }
    }

    property = GetProperty(vobsSTAR_ID_TYC1);

    if (IsPropertySet(property) == mcsTRUE)
    {
        propertyValue = GetPropertyValue(property);
        if (propertyValue != NULL)
        {
            snprintf(starId, (maxLength - 1), "TYC1 %s", propertyValue);
            return mcsSUCCESS;
        }
    }
    
    property = GetProperty(vobsSTAR_ID_2MASS);

    if (IsPropertySet(property) == mcsTRUE)
    {
        propertyValue = GetPropertyValue(property);
        if (propertyValue != NULL)
        {
            snprintf(starId, (maxLength - 1), "2MASS %s", propertyValue);
            return mcsSUCCESS;
        }
    }

    property = GetProperty(vobsSTAR_ID_DENIS);

    if (IsPropertySet(property) == mcsTRUE)
    {
        propertyValue = GetPropertyValue(property);
        if (propertyValue != NULL)
        {
            snprintf(starId, (maxLength - 1), "DENIS %s", propertyValue);
            return mcsSUCCESS;
        }
    }

    property = GetProperty(vobsSTAR_ID_TYC2);

    if (IsPropertySet(property) == mcsTRUE)
    {
        propertyValue = GetPropertyValue(property);
        if (propertyValue != NULL)
        {
            snprintf(starId, (maxLength - 1), "TYC2 %s", propertyValue);
            return mcsSUCCESS;
        }
    }

    property = GetProperty(vobsSTAR_ID_TYC3);

    if (IsPropertySet(property) == mcsTRUE)
    {
        propertyValue = GetPropertyValue(property);
        if (propertyValue != NULL)
        {
            snprintf(starId, (maxLength - 1), "TYC3 %s", propertyValue);
            return mcsSUCCESS;
        }
    }

    property = GetProperty(vobsSTAR_POS_EQ_RA_MAIN);
    vobsSTAR_PROPERTY* propertyDec = GetProperty(vobsSTAR_POS_EQ_DEC_MAIN);

    if ((IsPropertySet(property)  == mcsTRUE) && 
        (IsPropertySet(propertyDec) == mcsTRUE))
    {
        const char* raValue  = GetPropertyValue(property);
        const char* decValue = GetPropertyValue(propertyDec);
        
        snprintf(starId, (maxLength - 1), "Coordinates-ra=%s/dec=%s", raValue, decValue);
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
    // Check if the criteria list is empty
    if (criteriaList == NULL)
    {
        mcsDOUBLE ra1, ra2, dec1, dec2;

        // Get right ascension of the star. If not set return FALSE
        if (IsPropertyRaSet() == mcsTRUE)
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

        if (star.IsPropertyRaSet() == mcsTRUE)
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
        if (IsPropertyDecSet() == mcsTRUE)
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

        if (star.IsPropertyDecSet() == mcsTRUE)
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
    
    const char* propertyId;
    vobsSTAR_PROPERTY* prop1 = NULL;
    vobsSTAR_PROPERTY* prop2 = NULL;

    mcsDOUBLE range;
    vobsPROPERTY_TYPE type;
    const char *val1Str, *val2Str;
    mcsDOUBLE val1, val2; 

    // Get the size of the criteria list
    const int listSize = criteriaList->Size();

    // Get each criteria of the list and check if the comparaison with all
    // this criteria gave a equality

    for (int el = 0; el < listSize; el++)
    {
        if (criteriaList->GetNextCriteria
            (&propertyId, &range, (mcsLOGICAL)(el==0)) == mcsFAILURE)
        {
            return mcsFALSE;
        }

        type = vobsFLOAT_PROPERTY;

        if (strcmp(propertyId, vobsSTAR_POS_EQ_RA_MAIN) == 0)
        {
            // Get right ascension of the stars. If not set return FALSE
            if (IsPropertyRaSet() == mcsTRUE)
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
            if (star.IsPropertyRaSet() == mcsTRUE)
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
            if (IsPropertyDecSet() == mcsTRUE)
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
            if (star.IsPropertyDecSet() == mcsTRUE)
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
        else 
        {
            prop1 = GetProperty(propertyId);
            prop2 = star.GetProperty(propertyId);

            // Suppose types are the same:
            type = GetPropertyType(prop1);

            // If property is a string
            if (type == vobsSTRING_PROPERTY)
            {
                // Get value of the property id
                if (IsPropertySet(prop1) == mcsTRUE)
                {
                    val1Str = GetPropertyValue(prop1);
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
                if (star.IsPropertySet(prop2) == mcsTRUE)
                {
                    val2Str = star.GetPropertyValue(prop2);
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
            }
            else
            {
                // Get value of the property id
                if (IsPropertySet(prop1) == mcsTRUE)
                {
                    if (GetPropertyValue(prop1, &val1) == mcsFAILURE)
                    {
                        errCloseStack();
                        return mcsFALSE;
                    }
                }
                else
                {
                    return mcsFALSE;
                }    
                if (star.IsPropertySet(prop2) == mcsTRUE)
                {
                    if (star.GetPropertyValue(prop2, &val2) == mcsFAILURE)
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
        }

        if (type == vobsSTRING_PROPERTY)
        {
            if (strcmp(val1Str, val2Str) != 0)
            {
                return mcsFALSE;
            }            
        } else {
            double delta = fabs(val1 - val2);

            if (delta > range)
            {
                return mcsFALSE;
            }
        }
    }
    return mcsTRUE;
}

/**
 * Update a star with the properies of another given one.
 * By default ( overwrite = mcsFALSE ) it does not modify the content if
 * the property has already been set.
 * @param star the other star.
 * @param overwrite a true flag indicates to copy the value even if it is
 * already set. (default value set to false)
 *
 * @return always mcsSUCCESS.
 *
 */
mcsCOMPL_STAT vobsSTAR::Update (vobsSTAR &star, mcsLOGICAL overwrite)
{
    const bool isLogDebug = (logGetStdoutLogLevel() >= logDEBUG);

    // For each star property
    PropertyMap::iterator propertyIter;
    for (propertyIter  = _propertyList.begin();
         propertyIter != _propertyList.end();
         propertyIter++)
    {
        // Retrieve the identifier of the current property
        const char* propertyID = propertyIter->first;

        // If the current property is not yet defined
        if (IsPropertySet(propertyID) == mcsFALSE || overwrite == mcsTRUE )
        {
            // Use the property from the given star if existing!
            if (star.IsPropertySet(propertyID) == mcsTRUE)
            {
                _propertyList[propertyID] = star._propertyList[propertyID];
                
                if (isLogDebug)
                {
                    logDebug("updated _propertyList[%s] = '%s'.", propertyID, star._propertyList[propertyID].GetSummaryString().c_str());
                }
            }
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
    PropertyMap::iterator propertyIter;
    mcsSTRING64 starId;
    mcsDOUBLE    starRa  = 0.0;
    mcsDOUBLE    starDec = 0.0;

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
 * @param id property identifier
 * @param name property name
 * @param type property type
 * @param unit property unit, vobsSTAR_PROP_NOT_SET by default or for 'NULL'.
 * @param format format used to set property (%s or %.3f by default or for 'NULL').
 * @param link link for this property (none by default or for 'NULL').
 * @param description property description (none by default or for 'NULL').
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is 
 * returned.
 */
mcsCOMPL_STAT vobsSTAR::AddProperty(const char* id, const char* name,
                                    const vobsPROPERTY_TYPE type, const char* unit,
                                    const char* format, const char* link, const char* description)
{
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
    AddProperty(vobsSTAR_ID_HD, "HD", vobsSTRING_PROPERTY, vobsSTAR_PROP_NOT_SET, "%.0f",
                "http://simbad.u-strasbg.fr/simbad/sim-id?protocol=html&amp;Ident=HD${HD}",
                "HD identifier, click to call Simbad on this object");

    AddProperty(vobsSTAR_ID_HIP, "HIP", vobsSTRING_PROPERTY, vobsSTAR_PROP_NOT_SET, "%.0f",
                "http://simbad.u-strasbg.fr/simbad/sim-id?protocol=html&amp;Ident=HIP${HIP}",
                "HIP identifier, click to call Simbad on this object");

    AddProperty(vobsSTAR_ID_DM, "DM", vobsSTRING_PROPERTY);

    AddProperty(vobsSTAR_ID_TYC1, "TYC1", vobsSTRING_PROPERTY);
    AddProperty(vobsSTAR_ID_TYC2, "TYC2", vobsSTRING_PROPERTY);
    AddProperty(vobsSTAR_ID_TYC3, "TYC3", vobsSTRING_PROPERTY);

    AddProperty(vobsSTAR_ID_CATALOG, "opt", vobsSTRING_PROPERTY);  

    AddProperty(vobsSTAR_ID_2MASS, "2MASS", vobsSTRING_PROPERTY, vobsSTAR_PROP_NOT_SET, NULL,
                "http://vizier.u-strasbg.fr/viz-bin/VizieR?-source=II/246/out&amp;-out=2MASS&amp;2MASS=${2MASS}",
                "2MASS identifier, click to call VizieR on this object");

    AddProperty(vobsSTAR_POS_EQ_RA_MAIN, "RAJ2000", vobsSTRING_PROPERTY,
                "h:m:s", NULL, "http://simbad.u-strasbg.fr/simbad/sim-coo?CooDefinedFrames=none&amp;Coord=${RAJ2000}%20${DEJ2000}&amp;CooEpoch=2000&amp;CooFrame=FK5&amp;CooEqui=2000&amp;Radius.unit=arcsec&amp;Radius=1",
                "Right Ascencion - J2000");
    AddProperty(vobsSTAR_POS_EQ_DEC_MAIN, "DEJ2000", vobsSTRING_PROPERTY,
                "d:m:s", NULL, "http://simbad.u-strasbg.fr/simbad/sim-coo?CooDefinedFrames=none&amp;Coord=${RAJ2000}%20${DEJ2000}&amp;CooEpoch=2000&amp;CooFrame=FK5&amp;CooEqui=2000&amp;Radius.unit=arcsec&amp;Radius=1",
                "Declination - J2000");

    AddProperty(vobsSTAR_ID_DENIS, "DENIS", vobsSTRING_PROPERTY, vobsSTAR_PROP_NOT_SET, "%.0f", NULL,
                "DENIS identifier");  

    AddProperty(vobsSTAR_POS_EQ_RA_OTHER, "A2RAdeg", vobsSTRING_PROPERTY, "h:m:s");
    AddProperty(vobsSTAR_POS_EQ_DEC_OTHER, "A2DEdeg", vobsSTRING_PROPERTY, "d:m:s");

    AddProperty(vobsSTAR_POS_EQ_PMRA, "pmRa", vobsFLOAT_PROPERTY, "mas/yr", NULL, NULL,
                "Proper Motion in Right Ascension");
    AddProperty(vobsSTAR_POS_EQ_PMDEC, "pmDec", vobsFLOAT_PROPERTY, "mas/yr", NULL, NULL,
                "Proper Motion in Declination");

    AddProperty(vobsSTAR_POS_PARLX_TRIG, "plx", vobsFLOAT_PROPERTY, "mas", "%.2f", NULL,
                "Trigonometric Parallaxe");
    AddProperty(vobsSTAR_POS_PARLX_TRIG_ERROR, "e_Plx", vobsFLOAT_PROPERTY, NULL, NULL, NULL,
                "Error on Parallaxe");

    AddProperty(vobsSTAR_SPECT_TYPE_MK, "SpType", vobsSTRING_PROPERTY, NULL, NULL, NULL,
                "MK Spectral Type");

    AddProperty(vobsSTAR_CODE_VARIAB_V1, "VarFlag1", vobsSTRING_PROPERTY, NULL, NULL, NULL,
                "Variability from GCVS/NSV");
    AddProperty(vobsSTAR_CODE_VARIAB_V2, "VarFlag2", vobsSTRING_PROPERTY, NULL, NULL, NULL,
                "Variability in Tycho-1");
    AddProperty(vobsSTAR_CODE_VARIAB_V3, "VarFlag3", vobsSTRING_PROPERTY, NULL, NULL, NULL,
                "Variability type among C,D,M,P,R and U");

    AddProperty(vobsSTAR_CODE_MULT_FLAG, "MultFlag", vobsSTRING_PROPERTY, NULL, NULL, NULL,
                "Multiplicity type among C,G,O,V and X");
    AddProperty(vobsSTAR_CODE_BIN_FLAG, "BinFlag", vobsSTRING_PROPERTY, NULL, NULL, NULL,
                "Multiplicity type among SB or eclipsing B");

    AddProperty(vobsSTAR_ID_SB9, "SBC9", vobsSTRING_PROPERTY, vobsSTAR_PROP_NOT_SET, "%.0f",
                "http://vizier.u-strasbg.fr/viz-bin/VizieR?-source=B/sb9&amp;-out.form=%2bH&amp;-corr=FK=Seq&amp;-out.all=1&amp;-out.max=9999&amp;Seq===%20${SBC9}",
                "SBC9 identifier, click to call VizieR on this object");

    AddProperty(vobsSTAR_ID_WDS, "WDS", vobsSTRING_PROPERTY, vobsSTAR_PROP_NOT_SET, "%.0f",
                "http://vizier.u-strasbg.fr/viz-bin/VizieR?-source=B/wds/wds&amp;-out.form=%2bH&amp;-out.all=1&amp;-out.max=9999&amp;WDS===${WDS}",
                "WDS identifier, click to call VizieR on this object");
    AddProperty(vobsSTAR_ORBIT_SEPARATION_SEP1, "sep1", vobsFLOAT_PROPERTY, "arcsec", "%.1f", NULL,
                "Angular Separation of the binary on first observation");
    AddProperty(vobsSTAR_ORBIT_SEPARATION_SEP2, "sep2", vobsFLOAT_PROPERTY, "arcsec", "%.1f", NULL,
                "Angular Separation of the binary on last observation");

    AddProperty(vobsSTAR_CODE_MISC_I, "Iflag", vobsSTRING_PROPERTY);
    AddProperty(vobsSTAR_CODE_MISC_J, "Jflag", vobsSTRING_PROPERTY);
    AddProperty(vobsSTAR_CODE_MISC_K, "Kflag", vobsSTRING_PROPERTY);
    AddProperty(vobsSTAR_CODE_QUALITY, "Qflag", vobsSTRING_PROPERTY);

    AddProperty(vobsSTAR_POS_GAL_LAT, "GLAT", vobsFLOAT_PROPERTY, "deg", "%.2f", NULL,
                "Galactic Latitude");
    AddProperty(vobsSTAR_POS_GAL_LON, "GLON", vobsFLOAT_PROPERTY, "deg", "%.2f", NULL,
                "Galactic Longitude");

    AddProperty(vobsSTAR_VELOC_HC, "RadVel", vobsSTRING_PROPERTY, "km/s", NULL, NULL,
                "Radial Velocity");
    AddProperty(vobsSTAR_VELOC_ROTAT, "RotVel", vobsSTRING_PROPERTY, "km/s", NULL, NULL,
                "Rotation Velocity (v sini)");

    AddProperty(vobsSTAR_LD_DIAM, "LD", vobsFLOAT_PROPERTY, "mas", NULL, NULL,
                "Limb-Darkened Diameter");
    AddProperty(vobsSTAR_LD_DIAM_ERROR, "e_LD", vobsFLOAT_PROPERTY, "mas", NULL, NULL,
                "Error on Limb-Darkened Diameter");

    AddProperty(vobsSTAR_UD_DIAM, "UD", vobsFLOAT_PROPERTY, "mas", NULL, NULL,
                "Uniform-Disc Diameter");
    AddProperty(vobsSTAR_UD_DIAM_ERROR, "e_UD", vobsFLOAT_PROPERTY, "mas", NULL, NULL,
                "Error on Uniform-Disc Diameter");

    AddProperty(vobsSTAR_UDDK_DIAM, "UDDK", vobsFLOAT_PROPERTY, "mas", NULL, NULL,
                "Uniform-Disc Diameter in K-band");
    AddProperty(vobsSTAR_UDDK_DIAM_ERROR, "e_UDDK", vobsFLOAT_PROPERTY, "mas", NULL, NULL,
                "Error on Uniform-Disc Diameter in K-band");

    AddProperty(vobsSTAR_DIAM12, "Dia12", vobsFLOAT_PROPERTY, "mas", NULL, NULL,
                "Angular Diameter at 12 microns");
    AddProperty(vobsSTAR_DIAM12_ERROR, "e_dia12", vobsFLOAT_PROPERTY, "mas", NULL, NULL,
                "Error on Angular Diameter at 12 microns");

    AddProperty(vobsSTAR_OBS_METHOD, "Meth", vobsSTRING_PROPERTY, NULL, NULL, NULL,
                "Method of Measurement");

    AddProperty(vobsSTAR_INST_WAVELENGTH_VALUE, "lambda", vobsFLOAT_PROPERTY, NULL, NULL, NULL,
                "Wavelength");
    AddProperty(vobsSTAR_INST_FILTER_CODE, "lambda", vobsSTRING_PROPERTY, NULL, NULL, NULL,
                "Wavelength");

    AddProperty(vobsSTAR_PHOT_FLUX_IR_MISC, "photflux", vobsSTRING_PROPERTY);

    AddProperty(vobsSTAR_UNITS, "units", vobsSTRING_PROPERTY);

    AddProperty(vobsSTAR_PHOT_JHN_B, "B", vobsFLOAT_PROPERTY, "mag", NULL, NULL,
                "Johnson's Magnitude in B-band");
    AddProperty(vobsSTAR_PHOT_PHG_B, "Bphg", vobsFLOAT_PROPERTY, "mag", NULL, NULL,
                "Photometric Magnitude in B-band");

    AddProperty(vobsSTAR_PHOT_JHN_V, "V", vobsFLOAT_PROPERTY, "mag", NULL, NULL,
                "Johnson's Magnitude in V-band");
    AddProperty(vobsSTAR_PHOT_PHG_V, "Vphg", vobsFLOAT_PROPERTY, "mag", NULL, NULL,
                "Photometric Magnitude in V-band");

    AddProperty(vobsSTAR_PHOT_JHN_R, "R", vobsFLOAT_PROPERTY, "mag", NULL, NULL,
                "Johnson's Magnitude in R-band");
    AddProperty(vobsSTAR_PHOT_PHG_R, "Rphg", vobsFLOAT_PROPERTY, "mag", NULL, NULL,
                "Photometric Magnitude in R-band");

    AddProperty(vobsSTAR_PHOT_JHN_I, "I", vobsFLOAT_PROPERTY, "mag", NULL, NULL,
                "Johnson's Magnitude in I-band");
    AddProperty(vobsSTAR_PHOT_PHG_I, "Iphg", vobsFLOAT_PROPERTY, "mag", NULL, NULL,
                "Photometric Magnitude in I-band");
    AddProperty(vobsSTAR_PHOT_COUS_I, "Icous", vobsFLOAT_PROPERTY, "mag", NULL, NULL,
                "Cousin's Magnitude in I-band");

    AddProperty(vobsSTAR_PHOT_JHN_J, "J", vobsFLOAT_PROPERTY, "mag", NULL, NULL,
                "Johnson's Magnitude in J-band");
    AddProperty(vobsSTAR_PHOT_JHN_H, "H", vobsFLOAT_PROPERTY, "mag", NULL, NULL,
                "Johnson's Magnitude in H-band");
    AddProperty(vobsSTAR_PHOT_JHN_K, "K", vobsFLOAT_PROPERTY, "mag", NULL, NULL,
                "Johnson's Magnitude in K-band");
    AddProperty(vobsSTAR_PHOT_JHN_L, "L", vobsFLOAT_PROPERTY, "mag", NULL, NULL,
                "Johnson's Magnitude in L-band");
    AddProperty(vobsSTAR_PHOT_JHN_M, "M", vobsFLOAT_PROPERTY, "mag", NULL, NULL,
                "Johnson's Magnitude in M-band");
    AddProperty(vobsSTAR_PHOT_JHN_N, "N", vobsFLOAT_PROPERTY, "mag", NULL, NULL,
                "Johnson's Magnitude in N-band");

    AddProperty(vobsSTAR_PHOT_COLOR_EXCESS, "color", vobsSTRING_PROPERTY);

    AddProperty(vobsSTAR_IR_FLUX_ORIGIN, "orig", vobsSTRING_PROPERTY, NULL, NULL, NULL,
                "Source of the IR Flux among IRAS or MSX");

    AddProperty(vobsSTAR_PHOT_FLUX_IR_09, "S09",  vobsFLOAT_PROPERTY, "Jy", NULL, NULL,
                "Mid-Infrared Flux Density at 9 microns");
    AddProperty(vobsSTAR_PHOT_FLUX_IR_09_ERROR, "e_S09", vobsFLOAT_PROPERTY, "Jy", NULL, NULL,
                "Relative Error on Mid-Infrared Flux Density at 9 microns");

    AddProperty(vobsSTAR_PHOT_FLUX_IR_12, "F12",  vobsFLOAT_PROPERTY, "Jy", NULL, NULL,
                "Mid-Infrared Flux at 12 microns");
    AddProperty(vobsSTAR_PHOT_FLUX_IR_12_ERROR, "e_F12", vobsFLOAT_PROPERTY, "Jy", NULL, NULL,
                "Relative Error on Mid-Infrared Flux at 12 microns");

    AddProperty(vobsSTAR_PHOT_FLUX_IR_18, "S18",  vobsFLOAT_PROPERTY, "Jy", NULL, NULL,
                "Mid-Infrared Flux Density at 18 microns");
    AddProperty(vobsSTAR_PHOT_FLUX_IR_18_ERROR, "e_S18", vobsFLOAT_PROPERTY, "Jy", NULL, NULL,
                "Relative Error on Mid-Infrared Flux Density at 18 microns");

    AddProperty(vobsSTAR_REF_STAR, "Calib", vobsSTRING_PROPERTY);

    AddProperty(vobsSTAR_PHYS_TEMP_EFFEC, "Teff", vobsFLOAT_PROPERTY, NULL, NULL, NULL,
                "Effective Temperature");
    AddProperty(vobsSTAR_PHYS_TEMP_EFFEC_ERROR, "e_Teff", vobsFLOAT_PROPERTY, NULL, NULL, NULL,
                "Error on Effective Temperature");

    AddProperty(vobsSTAR_PHOT_EXTINCTION_TOTAL, "A_V", vobsFLOAT_PROPERTY, NULL, NULL, NULL,
                "Visible Interstellar Absorption");

    AddProperty(vobsSTAR_CHI2_QUALITY, "Chi2", vobsFLOAT_PROPERTY, NULL, NULL, NULL,
                "Chi2 of Spectro-Photmometric Data Model Fitting");

    AddProperty(vobsSTAR_SP_TYP_PHYS_TEMP_EFFEC, "SpTyp_Teff", vobsFLOAT_PROPERTY, NULL, NULL, NULL,
                "Spectral Type from adopted Modelling Effective Temperature");

    return mcsSUCCESS;
}


/*___oOo___*/
