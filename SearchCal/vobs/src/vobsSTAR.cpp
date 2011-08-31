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
#include <vector>
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

/* Maximum number of properties:
 *   - vobsSTAR (78)
 *   - sclsvrCALIBRATOR (130) */
#define vobsSTAR_MAX_PROPERTIES 78

/** Initialize static members */
PropertyIndexMap vobsSTAR::vobsSTAR_PropertyIdx;
PropertyMetaList vobsSTAR::vobsStar_PropertyMetaList;

int  vobsSTAR::vobsSTAR_PropertyMetaBegin      = -1;
int  vobsSTAR::vobsSTAR_PropertyMetaEnd        = -1;
bool vobsSTAR::vobsSTAR_PropertyIdxInitialized = false;

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

    ReserveProperties(vobsSTAR_MAX_PROPERTIES);
    
    // Add all star properties
    AddProperties();
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
vobsSTAR&vobsSTAR::operator=(const vobsSTAR& star)
{
    Clear();

    // copy the parsed ra/dec:
    _ra = star._ra;
    _dec = star._dec;
    
    // Copy (clone) the property list:
    ReserveProperties(vobsSTAR_MAX_PROPERTIES);
    
    vobsSTAR_PROPERTY* propertyRef;
    for (PropertyList::const_iterator iter = star._propertyList.begin(); iter != star._propertyList.end(); iter++)
    {
        propertyRef = new vobsSTAR_PROPERTY(*(*iter));
        _propertyList.push_back(propertyRef);
    }

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
    Clear();
}


/*
 * Public methods
 */
/**
 * Clear property list
 */
void vobsSTAR::Clear(void)
{
    // define ra/dec to blanking value:
    _ra  = EMPTY_COORD_DEG;
    _dec = EMPTY_COORD_DEG;

    for (PropertyList::iterator iter = _propertyList.begin(); iter != _propertyList.end(); iter++)
    {
        delete(*iter);
    }
    _propertyList.clear();
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
 * Get the star property at the given index.
 *
 * @param idx property index.
 * @return pointer on the found star property object on successful completion.
 * Otherwise NULL is returned.
 */
vobsSTAR_PROPERTY* vobsSTAR::GetProperty(const int idx)
{
    if (idx < 0 || idx >= (int)_propertyList.size())
    {
        return NULL;
    }
    
    return _propertyList[idx];
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
    return GetProperty(GetPropertyIndex(id));
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
        _propertyListIterator = _propertyList.begin();
    }
    else
    {
        // Increase the iterator to the following position
        _propertyListIterator++;

        // If this reached the end of the list
        if (_propertyListIterator == _propertyList.end())
        {
            return NULL;
        }
    }

    return (*_propertyListIterator);
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
    // Look for property: see GetProperty(id)
    int idx = GetPropertyIndex(id);
    
    if (idx < 0 || idx >= (int)_propertyList.size())
    {
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
const mcsLOGICAL vobsSTAR::IsSame(vobsSTAR &star,
                            vobsSTAR_COMP_CRITERIA_LIST *criteriaList)
{
    // Check if the criteria list is empty
    if (criteriaList == NULL)
    {
        mcsDOUBLE ra1, ra2, dec1, dec2;
        
        // try to use first cached ra/dec coordinates for performance:
        
        // Get right ascension of the star. If not set return FALSE
        if (_ra != EMPTY_COORD_DEG)
        {
            ra1 = _ra;
        }
        else 
        {
            if (GetRa(ra1) == mcsFAILURE)
            {
                errCloseStack();
                return mcsFALSE;
            }
        }

        if (star._ra != EMPTY_COORD_DEG)
        {
            ra2 = star._ra;
        }
        else 
        {
            if (star.GetRa(ra2) == mcsFAILURE)
            {
                errCloseStack();
                return mcsFALSE;
            }
        }        
        
        // Get declinaison of the star. If not set return FALSE
        if (_dec != EMPTY_COORD_DEG)
        {
            dec1 = _dec;
        }
        else 
        {
            if (GetDec(dec1) == mcsFAILURE)
            {
                errCloseStack();
                return mcsFALSE;
            }
        }        
        
        if (star._dec != EMPTY_COORD_DEG)
        {
            dec2 = star._dec;
        }
        else 
        {
            if (star.GetDec(dec2) == mcsFAILURE)
            {
                errCloseStack();
                return mcsFALSE;
            }
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
    int propIndex;
    vobsSTAR_PROPERTY* prop1 = NULL;
    vobsSTAR_PROPERTY* prop2 = NULL;

    mcsDOUBLE range;
    vobsPROPERTY_TYPE type;
    const char *val1Str, *val2Str;
    mcsDOUBLE  val1, val2; 

    // Get the size of the criteria list
    const int listSize = criteriaList->Size();

    // Get each criteria of the list and check if the comparaison with all
    // this criteria gave a equality

    for (int el = 0; el < listSize; el++)
    {
        if (criteriaList->GetNextCriteria(&propertyId, &range, (mcsLOGICAL)(el==0)) == mcsFAILURE)
        {
            return mcsFALSE;
        }

        type = vobsFLOAT_PROPERTY;

        // first check pointers then strcmp:
        if (strcmp(propertyId, vobsSTAR_POS_EQ_RA_MAIN) == 0)
        {
            // try to use first cached ra/dec coordinates for performance:

            // Get right ascension of the star. If not set return FALSE
            if (_ra != EMPTY_COORD_DEG)
            {
                val1 = _ra;
            }
            else 
            {
                if (GetRa(val1) == mcsFAILURE)
                {
                    errCloseStack();
                    return mcsFALSE;
                }
            }

            if (star._ra != EMPTY_COORD_DEG)
            {
                val2 = star._ra;
            }
            else 
            {
                if (star.GetRa(val2) == mcsFAILURE)
                {
                    errCloseStack();
                    return mcsFALSE;
                }
            }        
        }
        // first check pointers then strcmp:
        else if (strcmp(propertyId, vobsSTAR_POS_EQ_DEC_MAIN) == 0)
        {
            // try to use first cached ra/dec coordinates for performance:

            // Get declinaison of the star. If not set return FALSE
            if (_dec != EMPTY_COORD_DEG)
            {
                val1 = _dec;
            }
            else 
            {
                if (GetDec(val1) == mcsFAILURE)
                {
                    errCloseStack();
                    return mcsFALSE;
                }
            }        

            if (star._dec != EMPTY_COORD_DEG)
            {
                val2 = star._dec;
            }
            else 
            {
                if (star.GetDec(val2) == mcsFAILURE)
                {
                    errCloseStack();
                    return mcsFALSE;
                }
            }        
        }
        else 
        {
            propIndex = GetPropertyIndex(propertyId);
            prop1 = GetProperty(propIndex);
            prop2 = star.GetProperty(propIndex);

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
mcsCOMPL_STAT vobsSTAR::Update(vobsSTAR &star, mcsLOGICAL overwrite)
{
    const bool isLogDebug = (logIsStdoutLogLevel(logDEBUG) == mcsTRUE);

    // For each star property
    for (int idx = 0, len = NbProperties(); idx < len; idx++)
    {
        // Retrieve the properties at the current index
        vobsSTAR_PROPERTY *property = GetProperty(idx);
        vobsSTAR_PROPERTY *starProperty = star.GetProperty(idx);

        // If the current property is not yet defined
        if (IsPropertySet(property) == mcsFALSE || overwrite == mcsTRUE)
        {
            // Use the property from the given star if existing!
            if (star.IsPropertySet(starProperty) == mcsTRUE)
            {
                // replace property by a copy of the star property:
                _propertyList[idx] = new vobsSTAR_PROPERTY(*starProperty);

                // free old property:
                delete(property);
                
                if (isLogDebug)
                {
                    logDebug("updated _propertyList[%s] = '%s'.", starProperty->GetId(), starProperty->GetSummaryString().c_str());
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
        for (PropertyList::iterator iter = _propertyList.begin(); iter != _propertyList.end(); iter++)
        {
            printf("%12s", (*iter)->GetValue());
        }
        printf("\n");
    }
    else
    {
        for (PropertyList::iterator iter = _propertyList.begin(); iter != _propertyList.end(); iter++)
        {
            printf("%12s = %12s\n", (*iter)->GetId(), (*iter)->GetValue());
        }
    }
}

/**
 * Find the property index (position) for the given property identifier
 * @param id property identifier
 * @return index or -1 if not found in the property index
 */
int vobsSTAR::GetPropertyIndex(const char* id)
{
    // Look for property
    PropertyIndexMap::iterator idxIter = vobsSTAR::vobsSTAR_PropertyIdx.find(id);

    // If no property with the given Id was found
    if (idxIter == vobsSTAR::vobsSTAR_PropertyIdx.end()) 
    {
        return -1;
    }
  
    return idxIter->second;
}

/**
 * Return the property meta data for the given index
 * @param idx property index
 * @return property meta (pointer)
 */
const vobsSTAR_PROPERTY_META* vobsSTAR::GetPropertyMeta(const int idx)
{
    if (idx < 0 || idx >= (int)vobsSTAR::vobsStar_PropertyMetaList.size())
    {
        return NULL;
    }
    
    return vobsSTAR::vobsStar_PropertyMetaList[idx];
}


/*
 * Private methods
 */


/**
 * Reserve enough space in properties
 * @param size capacity to reserve
 */    
void vobsSTAR::ReserveProperties(unsigned int size)
{
    _propertyList.reserve(size);
}

/**
 * Add a star property
 *
 * @param meta property meta data
 */
void vobsSTAR::AddProperty(const vobsSTAR_PROPERTY_META* meta)
{
    // no checks because this method is only used carefully in AddProperties()
    
    // Create a new property from the given parameters
    vobsSTAR_PROPERTY* property = new vobsSTAR_PROPERTY(meta);

    // Add the new property to the internal list (copy):
    _propertyList.push_back(property);
}

/**
 * Add one property meta
 *
 * @param id property identifier
 * @param name property name
 * @param type property type
 * @param unit property unit, vobsSTAR_PROP_NOT_SET by default or for 'NULL'.
 * @param format format used to set property (%s or %.3f by default or for 'NULL').
 * @param link link for this property (none by default or for 'NULL').
 * @param description property description (none by default or for 'NULL').
 */
void vobsSTAR::AddPropertyMeta(const char* id, const char* name,
                               const vobsPROPERTY_TYPE type, const char* unit,
                               const char* format, const char* link, const char* description)
{
    // check index and position (useless check ??)
    /*
    if (vobsSTAR::vobsSTAR_PropertyIdxInitialized)
    {
        int idx = GetPropertyIndex(id);
        if (idx != -1 && GetProperty(idx) != NULL) {
            errAdd(vobsERR_DUPLICATED_PROPERTY, id);
            return mcsFAILURE;
        }
    }
    */ 

    // Create a new property from the given parameters
    vobsSTAR_PROPERTY_META* propertyMeta = new vobsSTAR_PROPERTY_META(id, name, type, unit, format, link, description);

    // Add the new property meta data to the internal list (copy):
    vobsSTAR::vobsStar_PropertyMetaList.push_back(propertyMeta);
}

/** 
 * Initialize the shared property index (NOT THREAD SAFE)
 */
void vobsSTAR::initializeIndex(void)
{
    // Fill the property index:
    const char* propertyId;
    unsigned int i = 0;
    
    for (PropertyMetaList::iterator iter = vobsSTAR::vobsStar_PropertyMetaList.begin(); 
                                    iter != vobsSTAR::vobsStar_PropertyMetaList.end(); iter++)
    {
        propertyId = (*iter)->GetId();

        if (GetPropertyIndex(propertyId) == -1)
        {
            vobsSTAR::vobsSTAR_PropertyIdx.insert(std::pair<const char*, int>(propertyId, i));
        }
        i++;
    }
}

/**
 * Add all star properties and fix an order
 *
 * @return mcsSUCCESS
 */
mcsCOMPL_STAT vobsSTAR::AddProperties(void)
{
    if (vobsSTAR::vobsSTAR_PropertyIdxInitialized == false)
    {
        vobsSTAR::vobsSTAR_PropertyMetaBegin = vobsSTAR::vobsStar_PropertyMetaList.size();
        
        // Add Meta data:
        AddPropertyMeta(vobsSTAR_ID_HD, "HD", vobsSTRING_PROPERTY, vobsSTAR_PROP_NOT_SET, "%.0f",
                    "http://simbad.u-strasbg.fr/simbad/sim-id?protocol=html&amp;Ident=HD${HD}",
                    "HD identifier, click to call Simbad on this object");

        AddPropertyMeta(vobsSTAR_ID_HIP, "HIP", vobsSTRING_PROPERTY, vobsSTAR_PROP_NOT_SET, "%.0f",
                    "http://simbad.u-strasbg.fr/simbad/sim-id?protocol=html&amp;Ident=HIP${HIP}",
                    "HIP identifier, click to call Simbad on this object");

        AddPropertyMeta(vobsSTAR_ID_DM, "DM", vobsSTRING_PROPERTY);

        AddPropertyMeta(vobsSTAR_ID_TYC1, "TYC1", vobsSTRING_PROPERTY);
        AddPropertyMeta(vobsSTAR_ID_TYC2, "TYC2", vobsSTRING_PROPERTY);
        AddPropertyMeta(vobsSTAR_ID_TYC3, "TYC3", vobsSTRING_PROPERTY);

        AddPropertyMeta(vobsSTAR_ID_CATALOG, "opt", vobsSTRING_PROPERTY);  

        AddPropertyMeta(vobsSTAR_ID_2MASS, "2MASS", vobsSTRING_PROPERTY, vobsSTAR_PROP_NOT_SET, NULL,
                    "http://vizier.u-strasbg.fr/viz-bin/VizieR?-source=II/246/out&amp;-out=2MASS&amp;2MASS=${2MASS}",
                    "2MASS identifier, click to call VizieR on this object");

        AddPropertyMeta(vobsSTAR_POS_EQ_RA_MAIN, "RAJ2000", vobsSTRING_PROPERTY,
                    "h:m:s", NULL, "http://simbad.u-strasbg.fr/simbad/sim-coo?CooDefinedFrames=none&amp;Coord=${RAJ2000}%20${DEJ2000}&amp;CooEpoch=2000&amp;CooFrame=FK5&amp;CooEqui=2000&amp;Radius.unit=arcsec&amp;Radius=1",
                    "Right Ascencion - J2000");
        AddPropertyMeta(vobsSTAR_POS_EQ_DEC_MAIN, "DEJ2000", vobsSTRING_PROPERTY,
                    "d:m:s", NULL, "http://simbad.u-strasbg.fr/simbad/sim-coo?CooDefinedFrames=none&amp;Coord=${RAJ2000}%20${DEJ2000}&amp;CooEpoch=2000&amp;CooFrame=FK5&amp;CooEqui=2000&amp;Radius.unit=arcsec&amp;Radius=1",
                    "Declination - J2000");

        AddPropertyMeta(vobsSTAR_ID_DENIS, "DENIS", vobsSTRING_PROPERTY, vobsSTAR_PROP_NOT_SET, "%.0f", NULL,
                    "DENIS identifier");  

        AddPropertyMeta(vobsSTAR_POS_EQ_RA_OTHER, "A2RAdeg", vobsSTRING_PROPERTY, "h:m:s");
        AddPropertyMeta(vobsSTAR_POS_EQ_DEC_OTHER, "A2DEdeg", vobsSTRING_PROPERTY, "d:m:s");

        AddPropertyMeta(vobsSTAR_POS_EQ_PMRA, "pmRa", vobsFLOAT_PROPERTY, "mas/yr", NULL, NULL,
                    "Proper Motion in Right Ascension");
        AddPropertyMeta(vobsSTAR_POS_EQ_PMDEC, "pmDec", vobsFLOAT_PROPERTY, "mas/yr", NULL, NULL,
                    "Proper Motion in Declination");

        AddPropertyMeta(vobsSTAR_POS_PARLX_TRIG, "plx", vobsFLOAT_PROPERTY, "mas", "%.2f", NULL,
                    "Trigonometric Parallaxe");
        AddPropertyMeta(vobsSTAR_POS_PARLX_TRIG_ERROR, "e_Plx", vobsFLOAT_PROPERTY, NULL, NULL, NULL,
                    "Error on Parallaxe");

        AddPropertyMeta(vobsSTAR_SPECT_TYPE_MK, "SpType", vobsSTRING_PROPERTY, NULL, NULL, NULL,
                    "MK Spectral Type");

        AddPropertyMeta(vobsSTAR_CODE_VARIAB_V1, "VarFlag1", vobsSTRING_PROPERTY, NULL, NULL, NULL,
                    "Variability from GCVS/NSV");
        AddPropertyMeta(vobsSTAR_CODE_VARIAB_V2, "VarFlag2", vobsSTRING_PROPERTY, NULL, NULL, NULL,
                    "Variability in Tycho-1");
        AddPropertyMeta(vobsSTAR_CODE_VARIAB_V3, "VarFlag3", vobsSTRING_PROPERTY, NULL, NULL, NULL,
                    "Variability type among C,D,M,P,R and U");

        AddPropertyMeta(vobsSTAR_CODE_MULT_FLAG, "MultFlag", vobsSTRING_PROPERTY, NULL, NULL, NULL,
                    "Multiplicity type among C,G,O,V, X or SB (for decoded spectral binaries)");
        AddPropertyMeta(vobsSTAR_CODE_BIN_FLAG, "BinFlag", vobsSTRING_PROPERTY, NULL, NULL, NULL,
                    "Multiplicity type among SB, eclipsing B or S (for suspicious binaries in spectral tupe)");

        AddPropertyMeta(vobsSTAR_ID_SB9, "SBC9", vobsSTRING_PROPERTY, vobsSTAR_PROP_NOT_SET, "%.0f",
                    "http://vizier.u-strasbg.fr/viz-bin/VizieR?-source=B/sb9&amp;-out.form=%2bH&amp;-corr=FK=Seq&amp;-out.all=1&amp;-out.max=9999&amp;Seq===%20${SBC9}",
                    "SBC9 identifier, click to call VizieR on this object");

        AddPropertyMeta(vobsSTAR_ID_WDS, "WDS", vobsSTRING_PROPERTY, vobsSTAR_PROP_NOT_SET, "%.0f",
                    "http://vizier.u-strasbg.fr/viz-bin/VizieR?-source=B/wds/wds&amp;-out.form=%2bH&amp;-out.all=1&amp;-out.max=9999&amp;WDS===${WDS}",
                    "WDS identifier, click to call VizieR on this object");
        AddPropertyMeta(vobsSTAR_ORBIT_SEPARATION_SEP1, "sep1", vobsFLOAT_PROPERTY, "arcsec", "%.1f", NULL,
                    "Angular Separation of the binary on first observation");
        AddPropertyMeta(vobsSTAR_ORBIT_SEPARATION_SEP2, "sep2", vobsFLOAT_PROPERTY, "arcsec", "%.1f", NULL,
                    "Angular Separation of the binary on last observation");

        AddPropertyMeta(vobsSTAR_CODE_MISC_I, "Iflag", vobsSTRING_PROPERTY);
        AddPropertyMeta(vobsSTAR_CODE_MISC_J, "Jflag", vobsSTRING_PROPERTY);
        AddPropertyMeta(vobsSTAR_CODE_MISC_K, "Kflag", vobsSTRING_PROPERTY);
        AddPropertyMeta(vobsSTAR_CODE_QUALITY, "Qflag", vobsSTRING_PROPERTY);

        AddPropertyMeta(vobsSTAR_POS_GAL_LAT, "GLAT", vobsFLOAT_PROPERTY, "deg", "%.2f", NULL,
                    "Galactic Latitude");
        AddPropertyMeta(vobsSTAR_POS_GAL_LON, "GLON", vobsFLOAT_PROPERTY, "deg", "%.2f", NULL,
                    "Galactic Longitude");

        AddPropertyMeta(vobsSTAR_VELOC_HC, "RadVel", vobsSTRING_PROPERTY, "km/s", NULL, NULL,
                    "Radial Velocity");
        AddPropertyMeta(vobsSTAR_VELOC_ROTAT, "RotVel", vobsSTRING_PROPERTY, "km/s", NULL, NULL,
                    "Rotation Velocity (v sini)");

        AddPropertyMeta(vobsSTAR_LD_DIAM, "LD", vobsFLOAT_PROPERTY, "mas", NULL, NULL,
                    "Limb-Darkened Diameter");
        AddPropertyMeta(vobsSTAR_LD_DIAM_ERROR, "e_LD", vobsFLOAT_PROPERTY, "mas", NULL, NULL,
                    "Error on Limb-Darkened Diameter");

        AddPropertyMeta(vobsSTAR_UD_DIAM, "UD", vobsFLOAT_PROPERTY, "mas", NULL, NULL,
                    "Uniform-Disc Diameter");
        AddPropertyMeta(vobsSTAR_UD_DIAM_ERROR, "e_UD", vobsFLOAT_PROPERTY, "mas", NULL, NULL,
                    "Error on Uniform-Disc Diameter");

        AddPropertyMeta(vobsSTAR_UDDK_DIAM, "UDDK", vobsFLOAT_PROPERTY, "mas", NULL, NULL,
                    "Uniform-Disc Diameter in K-band");
        AddPropertyMeta(vobsSTAR_UDDK_DIAM_ERROR, "e_UDDK", vobsFLOAT_PROPERTY, "mas", NULL, NULL,
                    "Error on Uniform-Disc Diameter in K-band");

        AddPropertyMeta(vobsSTAR_DIAM12, "Dia12", vobsFLOAT_PROPERTY, "mas", NULL, NULL,
                    "Angular Diameter at 12 microns");
        AddPropertyMeta(vobsSTAR_DIAM12_ERROR, "e_dia12", vobsFLOAT_PROPERTY, "mas", NULL, NULL,
                    "Error on Angular Diameter at 12 microns");

        AddPropertyMeta(vobsSTAR_OBS_METHOD, "Meth", vobsSTRING_PROPERTY, NULL, NULL, NULL,
                    "Method of Measurement");

        AddPropertyMeta(vobsSTAR_INST_WAVELENGTH_VALUE, "lambda", vobsFLOAT_PROPERTY, NULL, NULL, NULL,
                    "Wavelength");
        AddPropertyMeta(vobsSTAR_INST_FILTER_CODE, "lambda", vobsSTRING_PROPERTY, NULL, NULL, NULL,
                    "Wavelength");

        AddPropertyMeta(vobsSTAR_PHOT_FLUX_IR_MISC, "photflux", vobsSTRING_PROPERTY);

        AddPropertyMeta(vobsSTAR_UNITS, "units", vobsSTRING_PROPERTY);

        AddPropertyMeta(vobsSTAR_PHOT_JHN_B, "B", vobsFLOAT_PROPERTY, "mag", NULL, NULL,
                    "Johnson's Magnitude in B-band");
        AddPropertyMeta(vobsSTAR_PHOT_PHG_B, "Bphg", vobsFLOAT_PROPERTY, "mag", NULL, NULL,
                    "Photometric Magnitude in B-band");

        AddPropertyMeta(vobsSTAR_PHOT_JHN_V, "V", vobsFLOAT_PROPERTY, "mag", NULL, NULL,
                    "Johnson's Magnitude in V-band");
        AddPropertyMeta(vobsSTAR_PHOT_PHG_V, "Vphg", vobsFLOAT_PROPERTY, "mag", NULL, NULL,
                    "Photometric Magnitude in V-band");

        AddPropertyMeta(vobsSTAR_PHOT_JHN_R, "R", vobsFLOAT_PROPERTY, "mag", NULL, NULL,
                    "Johnson's Magnitude in R-band");
        AddPropertyMeta(vobsSTAR_PHOT_PHG_R, "Rphg", vobsFLOAT_PROPERTY, "mag", NULL, NULL,
                    "Photometric Magnitude in R-band");

        AddPropertyMeta(vobsSTAR_PHOT_JHN_I, "I", vobsFLOAT_PROPERTY, "mag", NULL, NULL,
                    "Johnson's Magnitude in I-band");
        AddPropertyMeta(vobsSTAR_PHOT_PHG_I, "Iphg", vobsFLOAT_PROPERTY, "mag", NULL, NULL,
                    "Photometric Magnitude in I-band");
        AddPropertyMeta(vobsSTAR_PHOT_COUS_I, "Icous", vobsFLOAT_PROPERTY, "mag", NULL, NULL,
                    "Cousin's Magnitude in I-band");

        AddPropertyMeta(vobsSTAR_PHOT_JHN_J, "J", vobsFLOAT_PROPERTY, "mag", NULL, NULL,
                    "Johnson's Magnitude in J-band");
        AddPropertyMeta(vobsSTAR_PHOT_JHN_H, "H", vobsFLOAT_PROPERTY, "mag", NULL, NULL,
                    "Johnson's Magnitude in H-band");
        AddPropertyMeta(vobsSTAR_PHOT_JHN_K, "K", vobsFLOAT_PROPERTY, "mag", NULL, NULL,
                    "Johnson's Magnitude in K-band");
        AddPropertyMeta(vobsSTAR_PHOT_JHN_L, "L", vobsFLOAT_PROPERTY, "mag", NULL, NULL,
                    "Johnson's Magnitude in L-band");
        AddPropertyMeta(vobsSTAR_PHOT_JHN_M, "M", vobsFLOAT_PROPERTY, "mag", NULL, NULL,
                    "Johnson's Magnitude in M-band");
        AddPropertyMeta(vobsSTAR_PHOT_JHN_N, "N", vobsFLOAT_PROPERTY, "mag", NULL, NULL,
                    "Johnson's Magnitude in N-band");

        AddPropertyMeta(vobsSTAR_PHOT_COLOR_EXCESS, "color", vobsSTRING_PROPERTY);

        AddPropertyMeta(vobsSTAR_IR_FLUX_ORIGIN, "orig", vobsSTRING_PROPERTY, NULL, NULL, NULL,
                    "Source of the IR Flux among IRAS or MSX");

        AddPropertyMeta(vobsSTAR_PHOT_FLUX_IR_09, "S09",  vobsFLOAT_PROPERTY, "Jy", NULL, NULL,
                    "Mid-Infrared Flux Density at 9 microns");
        AddPropertyMeta(vobsSTAR_PHOT_FLUX_IR_09_ERROR, "e_S09", vobsFLOAT_PROPERTY, "Jy", NULL, NULL,
                    "Relative Error on Mid-Infrared Flux Density at 9 microns");

        AddPropertyMeta(vobsSTAR_PHOT_FLUX_IR_12, "F12",  vobsFLOAT_PROPERTY, "Jy", NULL, NULL,
                    "Mid-Infrared Flux at 12 microns");
        AddPropertyMeta(vobsSTAR_PHOT_FLUX_IR_12_ERROR, "e_F12", vobsFLOAT_PROPERTY, "Jy", NULL, NULL,
                    "Relative Error on Mid-Infrared Flux at 12 microns");

        AddPropertyMeta(vobsSTAR_PHOT_FLUX_IR_18, "S18",  vobsFLOAT_PROPERTY, "Jy", NULL, NULL,
                    "Mid-Infrared Flux Density at 18 microns");
        AddPropertyMeta(vobsSTAR_PHOT_FLUX_IR_18_ERROR, "e_S18", vobsFLOAT_PROPERTY, "Jy", NULL, NULL,
                    "Relative Error on Mid-Infrared Flux Density at 18 microns");

        AddPropertyMeta(vobsSTAR_REF_STAR, "Calib", vobsSTRING_PROPERTY);

        AddPropertyMeta(vobsSTAR_PHYS_TEMP_EFFEC, "Teff", vobsFLOAT_PROPERTY, NULL, NULL, NULL,
                    "Effective Temperature");
        AddPropertyMeta(vobsSTAR_PHYS_TEMP_EFFEC_ERROR, "e_Teff", vobsFLOAT_PROPERTY, NULL, NULL, NULL,
                    "Error on Effective Temperature");

        AddPropertyMeta(vobsSTAR_PHOT_EXTINCTION_TOTAL, "A_V", vobsFLOAT_PROPERTY, NULL, NULL, NULL,
                    "Visible Interstellar Absorption");

        AddPropertyMeta(vobsSTAR_CHI2_QUALITY, "Chi2", vobsFLOAT_PROPERTY, NULL, NULL, NULL,
                    "Chi2 of Spectro-Photmometric Data Model Fitting");

        AddPropertyMeta(vobsSTAR_SP_TYP_PHYS_TEMP_EFFEC, "SpTyp_Teff", vobsFLOAT_PROPERTY, NULL, NULL, NULL,
                    "Spectral Type from adopted Modelling Effective Temperature");

        vobsSTAR::vobsSTAR_PropertyMetaEnd = vobsSTAR::vobsStar_PropertyMetaList.size();

        initializeIndex();

        vobsSTAR::vobsSTAR_PropertyIdxInitialized = true;
    }
    
    // Add properties:
    const vobsSTAR_PROPERTY_META* meta;
    for (int i = vobsSTAR::vobsSTAR_PropertyMetaBegin; i < vobsSTAR::vobsSTAR_PropertyMetaEnd; i++)
    {
        meta = GetPropertyMeta(i);
        
        if (meta != NULL)
        {
            AddProperty(meta);
        }
    }

    return mcsSUCCESS;
}

/**
 * Clean up the property index and meta data
 */
void vobsSTAR::FreePropertyIndex()
{
    // Clear the property meta data
    vobsSTAR::vobsSTAR_PropertyIdx.clear();

    for (PropertyMetaList::iterator iter = vobsSTAR::vobsStar_PropertyMetaList.begin(); 
         iter != vobsSTAR::vobsStar_PropertyMetaList.end(); iter++)
    {
        delete(*iter);
    }
    vobsSTAR::vobsStar_PropertyMetaList.clear();

    vobsSTAR::vobsSTAR_PropertyMetaBegin      = -1;
    vobsSTAR::vobsSTAR_PropertyMetaEnd        = -1;
    vobsSTAR::vobsSTAR_PropertyIdxInitialized = false;
}

/*___oOo___*/
