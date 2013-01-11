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
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <vector>
#include <sstream>
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
#include "vobsCATALOG_META.h"
#include "vobsPrivate.h"
#include "vobsErrors.h"

/* hour/minute and minute/second conversions */
#define vobsSEC_IN_MIN  (1.0 / 60.0)
#define vobsMIN_IN_HOUR (1.0 / 60.0)

/* enable/disable log RA/DEC epoch precession */
#define DO_LOG_PRECESS false

/* 
 * Maximum number of properties:
 *   - vobsSTAR (72)
 *   - sclsvrCALIBRATOR (115) */
#define vobsSTAR_MAX_PROPERTIES 72

/** Initialize static members */
vobsSTAR_PROPERTY_INDEX_MAPPING vobsSTAR::vobsSTAR_PropertyIdx;
vobsSTAR_PROPERTY_META_PTR_LIST vobsSTAR::vobsStar_PropertyMetaList;

int vobsSTAR::vobsSTAR_PropertyMetaBegin = -1;
int vobsSTAR::vobsSTAR_PropertyMetaEnd = -1;
bool vobsSTAR::vobsSTAR_PropertyIdxInitialized = false;

int vobsSTAR::vobsSTAR_PropertyRAIndex = -1;
int vobsSTAR::vobsSTAR_PropertyDECIndex = -1;
int vobsSTAR::vobsSTAR_PropertyTargetIdIndex = -1;
int vobsSTAR::vobsSTAR_PropertyPMRAIndex = -1;
int vobsSTAR::vobsSTAR_PropertyPMDECIndex = -1;
int vobsSTAR::vobsSTAR_PropertyJDIndex = -1;

/*
 * Class constructor
 */

/**
 * Build a star object.
 */
vobsSTAR::vobsSTAR()
{
    ClearCache();

    ReserveProperties(vobsSTAR_MAX_PROPERTIES);

    // Add all star properties
    AddProperties();
}

/**
 * Build a star object from another one (copy constructor).
 */
vobsSTAR::vobsSTAR(const vobsSTAR &star)
{
    // Uses the operator=() method to copy
    *this = star;
}

/**
 * Assignement operator
 */
vobsSTAR& vobsSTAR::operator=(const vobsSTAR& star)
{
    if (this != &star)
    {
        Clear();

        // copy the parsed ra/dec:
        _ra = star._ra;
        _dec = star._dec;

        // copy the parsed ra/dec of the reference star: (useless ?)
        _raRef = star._raRef;
        _decRef = star._decRef;

        // Copy (clone) the property list:
        ReserveProperties(vobsSTAR_MAX_PROPERTIES);

        vobsSTAR_PROPERTY* propertyRef;
        for (vobsSTAR_PROPERTY_PTR_LIST::const_iterator iter = star._propertyList.begin(); iter != star._propertyList.end(); iter++)
        {
            propertyRef = new vobsSTAR_PROPERTY(*(*iter));
            _propertyList.push_back(propertyRef);
        }
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
 * Clear cached values (ra, dec)
 */
void vobsSTAR::ClearCache()
{
    // define ra/dec to blanking value:
    _ra = EMPTY_COORD_DEG;
    _dec = EMPTY_COORD_DEG;
    _raRef = EMPTY_COORD_DEG;
    _decRef = EMPTY_COORD_DEG;
}

/**
 * Clear property list
 */
void vobsSTAR::Clear()
{
    ClearCache();

    for (vobsSTAR_PROPERTY_PTR_LIST::iterator iter = _propertyList.begin(); iter != _propertyList.end(); iter++)
    {
        delete(*iter);
    }
    _propertyList.clear();
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
mcsCOMPL_STAT vobsSTAR::SetPropertyValue(const char* propertyId,
                                         const char* value,
                                         const char* origin,
                                         vobsCONFIDENCE_INDEX confidenceIndex,
                                         mcsLOGICAL overwrite)
{
    // Look for the given property
    vobsSTAR_PROPERTY* property = GetProperty(propertyId);

    FAIL_NULL(property);

    return SetPropertyValue(property, value, origin, confidenceIndex, overwrite);
}

/**
 * Set the floating value of a given property.
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
mcsCOMPL_STAT vobsSTAR::SetPropertyValue(const char* id,
                                         mcsDOUBLE value,
                                         const char* origin,
                                         vobsCONFIDENCE_INDEX confidenceIndex,
                                         mcsLOGICAL overwrite)
{
    // Look for the given property
    vobsSTAR_PROPERTY* property = GetProperty(id);

    FAIL_NULL(property);

    return SetPropertyValue(property, value, origin, confidenceIndex, overwrite);
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
mcsCOMPL_STAT vobsSTAR::ClearPropertyValue(const char* id)
{
    // Look for the given property
    vobsSTAR_PROPERTY* property = GetProperty(id);

    FAIL_NULL(property);

    // Clear this property value
    property->ClearValue();

    return mcsSUCCESS;
}

/**
 * Get right ascension (RA) coordinate in degrees.
 *
 * @param ra pointer on an already allocated mcsDOUBLE value.
 *
 * @return mcsSUCCESS on successful completion, mcsFAILURE otherwise.
 */
mcsCOMPL_STAT vobsSTAR::GetRa(mcsDOUBLE &ra) const
{
    // use cached ra coordinate:
    if (_ra != EMPTY_COORD_DEG)
    {
        ra = _ra;
        return mcsSUCCESS;
    }

    vobsSTAR_PROPERTY* property = GetProperty(vobsSTAR::vobsSTAR_PropertyRAIndex);

    // Check if the value is set
    FAIL_FALSE_DO(property->IsSet(), errAdd(vobsERR_RA_NOT_SET))

    FAIL(GetRa(GetPropertyValue(property), ra));

    // cache value:
    _ra = ra;

    return mcsSUCCESS;
}

/**
 * Get right ascension (RA) coordinate in degrees of the reference star.
 *
 * @param ra pointer on an already allocated mcsDOUBLE value.
 *
 * @return mcsSUCCESS on successful completion, mcsFAILURE otherwise.
 */
mcsCOMPL_STAT vobsSTAR::GetRaRefStar(mcsDOUBLE &raRef) const
{
    // use cached raRef coordinate:
    if (_raRef != EMPTY_COORD_DEG)
    {
        raRef = _raRef;
        return mcsSUCCESS;
    }

    vobsSTAR_PROPERTY* targetIdProperty = GetTargetIdProperty();

    // Check if the value is set
    FAIL_FALSE(targetIdProperty->IsSet());

    // Parse Target identifier '016.417537-41.369444':
    const char* targetId = GetPropertyValue(targetIdProperty);

    // cache values:    
    FAIL(degToRaDec(targetId, _raRef, _decRef));

    raRef = _raRef;

    return mcsSUCCESS;
}

/**
 * Get declinaison (DEC) coordinate in degrees.
 *
 * @param dec pointer on an already allocated mcsDOUBLE value.
 *
 * @return mcsSUCCESS on successful completion, mcsFAILURE otherwise.
 */
mcsCOMPL_STAT vobsSTAR::GetDec(mcsDOUBLE &dec) const
{
    // use cached dec coordinate:
    if (_dec != EMPTY_COORD_DEG)
    {
        dec = _dec;
        return mcsSUCCESS;
    }

    vobsSTAR_PROPERTY* property = GetProperty(vobsSTAR::vobsSTAR_PropertyDECIndex);

    // Check if the value is set
    FAIL_FALSE_DO(property->IsSet(), errAdd(vobsERR_DEC_NOT_SET));

    FAIL(GetDec(GetPropertyValue(property), dec));

    // cache value:    
    _dec = dec;

    return mcsSUCCESS;
}

/**
 * Get the optional declinaison (DEC) coordinate in degrees of the reference star.
 *
 * @param decRef pointer on an already allocated mcsDOUBLE value.
 *
 * @return mcsSUCCESS on successful completion, mcsFAILURE otherwise.
 */
mcsCOMPL_STAT vobsSTAR::GetDecRefStar(mcsDOUBLE &decRef) const
{
    // use cached decRef coordinate:
    if (_decRef != EMPTY_COORD_DEG)
    {
        decRef = _decRef;
        return mcsSUCCESS;
    }

    vobsSTAR_PROPERTY* targetIdProperty = GetTargetIdProperty();

    // Check if the value is set
    FAIL_FALSE(targetIdProperty->IsSet());

    // Parse Target identifier '016.417537-41.369444':
    const char* targetId = GetPropertyValue(targetIdProperty);

    // cache values:    
    FAIL(degToRaDec(targetId, _raRef, _decRef));

    decRef = _decRef;

    return mcsSUCCESS;
}

/**
 * Return the star PMRA in max/yr DIVIDED by cos(dec)
 * @param pmRa pointer on an already allocated mcsDOUBLE value.
 * @return mcsSUCCESS on successful completion, mcsFAILURE otherwise.
 */
mcsCOMPL_STAT vobsSTAR::GetPmRa(mcsDOUBLE &pmRa) const
{
    vobsSTAR_PROPERTY* property = GetProperty(vobsSTAR::vobsSTAR_PropertyPMRAIndex);

    // Check if the value is set
    if (property->IsSet() == mcsFALSE)
    {
        pmRa = 0.0;
        return mcsSUCCESS;
    }

    mcsDOUBLE pmRA, dec;

    FAIL(property->GetValue(&pmRA));
    FAIL(GetDec(dec));

    // divide by cos(dec) once for all:
    pmRA /= cos(dec * alxDEG_IN_RAD);

    pmRa = pmRA;

    return mcsSUCCESS;
}

/**
 * Return the star PMDEC in max/yr
 * @param pmDec pointer on an already allocated mcsDOUBLE value.
 * @return mcsSUCCESS on successful completion, mcsFAILURE otherwise.
 */
mcsCOMPL_STAT vobsSTAR::GetPmDec(mcsDOUBLE &pmDec) const
{
    vobsSTAR_PROPERTY* property = GetProperty(vobsSTAR::vobsSTAR_PropertyPMDECIndex);

    // Check if the value is set
    if (property->IsSet() == mcsFALSE)
    {
        pmDec = 0.0;
        return mcsSUCCESS;
    }

    FAIL(property->GetValue(&pmDec));

    return mcsSUCCESS;
}

/**
 * Return the observation date (jd)
 * @param jd pointer on an already allocated mcsDOUBLE value.
 * @return observation date (jd) or -1 if undefined
 */
mcsDOUBLE vobsSTAR::GetJdDate() const
{
    vobsSTAR_PROPERTY* property = GetJdDateProperty();

    // Check if the value is set
    if (property->IsSet() == mcsFALSE)
    {
        return -1.0;
    }

    mcsDOUBLE jd;

    FAIL(GetPropertyValue(property, &jd));

    return jd;
}

/**
 * Get star Id, based on the catalog it comes from.
 *
 * @param starId a pointer on an already allocated character buffer.
 * @param maxLength the size of the external character buffer.
 *
 * @return mcsSUCCESS on successful completion, mcsFAILURE otherwise.
 */
mcsCOMPL_STAT vobsSTAR::GetId(char* starId, const mcsUINT32 maxLength) const
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

    property = GetProperty(vobsSTAR::vobsSTAR_PropertyRAIndex);
    vobsSTAR_PROPERTY* propertyDec = GetProperty(vobsSTAR::vobsSTAR_PropertyDECIndex);

    if ((IsPropertySet(property) == mcsTRUE) && (IsPropertySet(propertyDec) == mcsTRUE))
    {
        const char* raValue = GetPropertyValue(property);
        const char* decValue = GetPropertyValue(propertyDec);

        snprintf(starId, (maxLength - 1), "Coordinates-ra=%s/dec=%s", raValue, decValue);
        return mcsSUCCESS;
    }

    // No id found
    strncpy(starId, "????", (maxLength - 1));

    return mcsSUCCESS;
}

/**
 * Update a star with the properies of another given one.
 * By default ( overwrite = mcsFALSE ) it does not modify the content if
 * the property has already been set.
 * @param star the other star.
 * @param overwrite a true flag indicates to copy the value even if it is
 * already set. (default value set to false)
 * @param optional overwrite Property Mask
 * @param propertyUpdated integer array storing updated counts per property index (int)
 *
 * @return mcsTRUE if this star has been updated (at least one property changed)
 */
mcsLOGICAL vobsSTAR::Update(const vobsSTAR &star,
                            vobsOVERWRITE overwrite,
                            const vobsSTAR_PROPERTY_MASK* overwritePropertyMask,
                            mcsUINT32* propertyUpdated)
{
    const bool isLogDebug = doLog(logDEBUG);
    mcsLOGICAL updated = mcsFALSE;

    const bool isPartialOverwrite = (overwrite >= vobsOVERWRITE_PARTIAL);

    if (isPartialOverwrite && (overwritePropertyMask == NULL))
    {
        // invalid case: disable overwrite:
        overwrite = vobsOVERWRITE_NONE;
    }

    vobsSTAR_PROPERTY* property;
    vobsSTAR_PROPERTY* starProperty;

    // For each star property
    for (int idx = 0, len = NbProperties(); idx < len; idx++)
    {
        // Retrieve the properties at the current index
        property = GetProperty(idx);

        // If the current property is not yet defined
        if ((IsPropertySet(property) == mcsFALSE) || (overwrite != vobsOVERWRITE_NONE))
        {
            starProperty = star.GetProperty(idx);

            // Use the property from the given star if existing
            if (star.IsPropertySet(starProperty) == mcsTRUE)
            {
                if (isPartialOverwrite)
                {
                    if (isLogDebug)
                    {
                        logDebug("overwritePropertyMask[%s][%d] = %s", starProperty->GetName(), idx, (*overwritePropertyMask)[idx] ? "true" : "false");
                    }

                    if (!(*overwritePropertyMask)[idx])
                    {
                        // property should not be updated: skip it.
                        continue;
                    }

                    // TODO: implement better overwrite mode (check property error or scoring ...)
                }

                // replace property by using assignement operator:
                *property = *starProperty;

                if (isLogDebug)
                {
                    logDebug("updated _propertyList[%s] = '%s'.", starProperty->GetId(), starProperty->GetSummaryString().c_str());
                }

                // statistics:
                updated = mcsTRUE;

                if (propertyUpdated != NULL)
                {
                    propertyUpdated[idx]++;
                }
            }
        }
    }

    return updated;
}

/**
 * Display all star properties on the console.
 *
 * @param showPropId displays each star property in a form \<propId\> =
 * \<value\> if mcsTRUE, otherwise all properties on a single line if mcsFALSE.
 */
void vobsSTAR::Display(mcsLOGICAL showPropId) const
{
    mcsSTRING64 starId;
    mcsDOUBLE starRa = 0.0;
    mcsDOUBLE starDec = 0.0;

    GetId(starId, sizeof (starId));

    if (IsPropertySet(vobsSTAR::vobsSTAR_PropertyRAIndex) == mcsTRUE)
    {
        GetRa(starRa);
    }
    if (IsPropertySet(vobsSTAR::vobsSTAR_PropertyDECIndex) == mcsTRUE)
    {
        GetDec(starDec);
    }
    printf("%s(%lf,%lf): ", starId, starRa, starDec);

    if (showPropId == mcsFALSE)
    {
        vobsSTAR_PROPERTY* property;
        for (vobsSTAR_PROPERTY_INDEX_MAPPING::iterator iter = vobsSTAR::vobsSTAR_PropertyIdx.begin();
             iter != vobsSTAR::vobsSTAR_PropertyIdx.end(); iter++)
        {
            property = GetProperty(iter->second);

            if (property != NULL)
            {
                printf("%12s", property->GetValue());
            }
        }
        printf("\n");
    }
    else
    {
        vobsSTAR_PROPERTY* property;
        for (vobsSTAR_PROPERTY_INDEX_MAPPING::iterator iter = vobsSTAR::vobsSTAR_PropertyIdx.begin();
             iter != vobsSTAR::vobsSTAR_PropertyIdx.end(); iter++)
        {
            property = GetProperty(iter->second);

            if (property != NULL)
            {
                printf("%12s = %12s\n", property->GetId(), property->GetValue());
            }
        }
    }
}

/**
 * Dump only set star properties to the given output char array.
 * 
 * Note: no buffer overflow checks on output buffer
 * 
 * @param output output char array
 * @param separator separator
 */
void vobsSTAR::Dump(char* output, const char* separator) const
{
    mcsSTRING64 tmp;
    mcsDOUBLE starRa = 0.0;
    mcsDOUBLE starDec = 0.0;

    output[0] = '\0';
    char* outPtr = output;

    GetId(tmp, sizeof (tmp));

    if (IsPropertySet(vobsSTAR::vobsSTAR_PropertyRAIndex) == mcsTRUE)
    {
        GetRa(starRa);
    }
    if (IsPropertySet(vobsSTAR::vobsSTAR_PropertyDECIndex) == mcsTRUE)
    {
        GetDec(starDec);
    }
    sprintf(output, "'%s' (RA = %lf, DEC = %lf): ", tmp, starRa, starDec);

    outPtr += strlen(output);

    vobsSTAR_PROPERTY* property;
    for (vobsSTAR_PROPERTY_PTR_LIST::const_iterator iter = _propertyList.begin(); iter != _propertyList.end(); iter++)
    {
        property = (*iter);

        if (property->IsSet() == mcsTRUE)
        {
            snprintf(tmp, sizeof (tmp) - 1, "%s = %s%s", property->GetId(), property->GetValue(), separator);

            vobsStrcatFast(outPtr, tmp);
        }
    }
}

/**
 * Compare stars (i.e values)
 * @param other other vobsSTAR instance (or sub class)
 * @return 0 if equals; < 0 if this star has less properties than other; > 0 else
 */
int vobsSTAR::compare(const vobsSTAR& other) const
{
    int common = 0, lDiff = 0, rDiff = 0;
    ostringstream same, diffLeft, diffRight;

    vobsSTAR_PROPERTY_PTR_LIST propListLeft = _propertyList;
    vobsSTAR_PROPERTY_PTR_LIST propListRight = other._propertyList;

    vobsSTAR_PROPERTY_PTR_LIST::const_iterator iLeft, iRight;

    vobsSTAR_PROPERTY* propLeft;
    vobsSTAR_PROPERTY* propRight;

    mcsLOGICAL setLeft, setRight;
    const char *val1Str, *val2Str;

    for (iLeft = propListLeft.begin(), iRight = propListRight.begin();
         (iLeft != propListLeft.end()) && (iRight != propListRight.end()); iLeft++, iRight++)
    {
        propLeft = *iLeft;
        propRight = *iRight;

        setLeft = propLeft->IsSet();
        setRight = propRight->IsSet();

        if (setLeft == setRight)
        {
            if (setLeft == mcsTRUE)
            {
                // properties are both set; compare values as string:
                val1Str = propLeft->GetValue();
                val2Str = propRight->GetValue();

                if (strcmp(val1Str, val2Str) == 0)
                {
                    common++;
                    same << propLeft->GetId() << " = '" << propLeft->GetValue() << "' ";
                }
                else
                {
                    lDiff++;
                    diffLeft << propLeft->GetId() << " = '" << propLeft->GetValue() << "' ";

                    rDiff++;
                    diffRight << propRight->GetId() << " = '" << propRight->GetValue() << "' ";
                }
            }
        }
        else
        {
            // properties are not both set:
            if (setLeft == mcsTRUE)
            {
                lDiff++;
                diffLeft << propLeft->GetId() << " = '" << propLeft->GetValue() << "' ";

            }
            else if (setRight == mcsTRUE)
            {
                rDiff++;
                diffRight << propRight->GetId() << " = '" << propRight->GetValue() << "' ";
            }
        }
    }

    if ((lDiff > 0) || (rDiff > 0))
    {
        const int diff = lDiff - rDiff;

        logWarning("Compare Properties[%d]: COMMON(%d) {%s} - LEFT(%d) {%s} - RIGHT(%d) {%s}",
                   diff, common, same.str().c_str(), lDiff, diffLeft.str().c_str(), rDiff, diffRight.str().c_str());

        if (diff == 0)
        {
            // which star is better: undetermined
            return -1;
        }
        return diff;
    }

    return 0;
}

/*
 * Private methods
 */

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

    for (vobsSTAR_PROPERTY_META_PTR_LIST::iterator iter = vobsSTAR::vobsStar_PropertyMetaList.begin();
         iter != vobsSTAR::vobsStar_PropertyMetaList.end(); iter++)
    {
        propertyId = (*iter)->GetId();

        if (GetPropertyIndex(propertyId) == -1)
        {
            vobsSTAR::vobsSTAR_PropertyIdx.insert(vobsSTAR_PROPERTY_INDEX_PAIR(propertyId, i));
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
        /* identifiers */
        AddPropertyMeta(vobsSTAR_ID_HD, "HD", vobsSTRING_PROPERTY, vobsSTAR_PROP_NOT_SET, "%.0lf",
                        "http://simbad.u-strasbg.fr/simbad/sim-id?protocol=html&amp;Ident=HD${HD}",
                        "HD identifier, click to call Simbad on this object");

        AddPropertyMeta(vobsSTAR_ID_HIP, "HIP", vobsSTRING_PROPERTY, vobsSTAR_PROP_NOT_SET, "%.0lf",
                        "http://simbad.u-strasbg.fr/simbad/sim-id?protocol=html&amp;Ident=HIP${HIP}",
                        "HIP identifier, click to call Simbad on this object");

        AddPropertyMeta(vobsSTAR_ID_DM, "DM", vobsSTRING_PROPERTY);

        AddPropertyMeta(vobsSTAR_ID_TYC1, "TYC1", vobsSTRING_PROPERTY);
        AddPropertyMeta(vobsSTAR_ID_TYC2, "TYC2", vobsSTRING_PROPERTY);
        AddPropertyMeta(vobsSTAR_ID_TYC3, "TYC3", vobsSTRING_PROPERTY);

        /* 2MASS Associated optical source (opt) 'T' for Tycho 2 or 'U' for USNO A 2.0 */
        AddPropertyMeta(vobsSTAR_2MASS_OPT_ID_CATALOG, "opt", vobsSTRING_PROPERTY);

        AddPropertyMeta(vobsSTAR_ID_2MASS, "2MASS", vobsSTRING_PROPERTY, vobsSTAR_PROP_NOT_SET, NULL,
                        "http://vizier.u-strasbg.fr/viz-bin/VizieR?-source=II/246/out&amp;-out=2MASS&amp;2MASS=${2MASS}",
                        "2MASS identifier, click to call VizieR on this object");

        AddPropertyMeta(vobsSTAR_ID_AKARI, "AKARI", vobsSTRING_PROPERTY, vobsSTAR_PROP_NOT_SET, NULL,
                        "http://vizier.u-strasbg.fr/viz-bin/VizieR?-source=II/297/irc&amp;objID=${AKARI}",
                        "AKARI source ID number, click to call VizieR on this object");

        /* RA/DEC coordinates */
        AddPropertyMeta(vobsSTAR_POS_EQ_RA_MAIN, "RAJ2000", vobsSTRING_PROPERTY,
                        "h:m:s", NULL, "http://simbad.u-strasbg.fr/simbad/sim-coo?CooDefinedFrames=none&amp;Coord=${RAJ2000}%20${DEJ2000}&amp;CooEpoch=2000&amp;CooFrame=FK5&amp;CooEqui=2000&amp;Radius.unit=arcsec&amp;Radius=1",
                        "Right Ascension - J2000");
        AddPropertyMeta(vobsSTAR_POS_EQ_DEC_MAIN, "DEJ2000", vobsSTRING_PROPERTY,
                        "d:m:s", NULL, "http://simbad.u-strasbg.fr/simbad/sim-coo?CooDefinedFrames=none&amp;Coord=${RAJ2000}%20${DEJ2000}&amp;CooEpoch=2000&amp;CooFrame=FK5&amp;CooEqui=2000&amp;Radius.unit=arcsec&amp;Radius=1",
                        "Declination - J2000");

        /* errors on RA/DEC coordinates */
        AddPropertyMeta(vobsSTAR_POS_EQ_RA_ERROR, "e_RAJ2000", vobsFLOAT_PROPERTY, "mas", NULL, NULL,
                        "Standard error in Right Ascension * cos(Declination) - J2000");
        AddPropertyMeta(vobsSTAR_POS_EQ_DEC_ERROR, "e_DEJ2000", vobsFLOAT_PROPERTY, "mas", NULL, NULL,
                        "Standard error in Declination - J2000");

        /* CDS TargetId used by internal crossmatchs (filtered in VOTable output) */
        AddPropertyMeta(vobsSTAR_ID_TARGET, "TARGET_ID", vobsSTRING_PROPERTY, "deg", NULL, NULL, "The target identifier (RA/DEC) asked to CDS");

        AddPropertyMeta(vobsSTAR_ID_DENIS, "DENIS", vobsSTRING_PROPERTY, vobsSTAR_PROP_NOT_SET, "%.0lf", NULL,
                        "DENIS identifier");

        /* RA/DEC OTHER (DENIS): useful ? */
        AddPropertyMeta(vobsSTAR_POS_EQ_RA_OTHER, "A2RAdeg", vobsSTRING_PROPERTY, "h:m:s");
        AddPropertyMeta(vobsSTAR_POS_EQ_DEC_OTHER, "A2DEdeg", vobsSTRING_PROPERTY, "d:m:s");

        /* Proper motion */
        AddPropertyMeta(vobsSTAR_POS_EQ_PMRA, "pmRa", vobsFLOAT_PROPERTY, "mas/yr", NULL, NULL,
                        "Proper Motion in Right Ascension * cos(Declination)");
        AddPropertyMeta(vobsSTAR_POS_EQ_PMDEC, "pmDec", vobsFLOAT_PROPERTY, "mas/yr", NULL, NULL,
                        "Proper Motion in Declination");

        /* errors on RA/DEC coordinates */
        AddPropertyMeta(vobsSTAR_POS_EQ_PMRA_ERROR, "e_pmRA", vobsFLOAT_PROPERTY, "mas/yr", NULL, NULL,
                        "Standard error in Proper Motion in Right Ascension * cos(Declination)");
        AddPropertyMeta(vobsSTAR_POS_EQ_PMDEC_ERROR, "e_pmDec", vobsFLOAT_PROPERTY, "mas/yr", NULL, NULL,
                        "Proper Motion in Proper Motion in Declination");

        /* 2MASS observation date (JD) (filtered in VOTable output) */
        AddPropertyMeta(vobsSTAR_JD_DATE, "jd", vobsFLOAT_PROPERTY, "d", NULL, NULL,
                        "(jdate) Julian date of source measurement (2MASS)");

        /* Parallax */
        AddPropertyMeta(vobsSTAR_POS_PARLX_TRIG, "plx", vobsFLOAT_PROPERTY, "mas", "%.2lf", NULL,
                        "Trigonometric Parallax");
        AddPropertyMeta(vobsSTAR_POS_PARLX_TRIG_ERROR, "e_Plx", vobsFLOAT_PROPERTY, NULL, NULL, NULL,
                        "Standard error in Parallax");

        AddPropertyMeta(vobsSTAR_SPECT_TYPE_MK, "SpType", vobsSTRING_PROPERTY, NULL, NULL, NULL,
                        "MK Spectral Type");

        /* ASCC */
        AddPropertyMeta(vobsSTAR_CODE_VARIAB_V1, "VarFlag1", vobsSTRING_PROPERTY, NULL, NULL, NULL,
                        "Variability from GCVS/NSV");
        AddPropertyMeta(vobsSTAR_CODE_VARIAB_V2, "VarFlag2", vobsSTRING_PROPERTY, NULL, NULL, NULL,
                        "Variability in Tycho-1");
        AddPropertyMeta(vobsSTAR_CODE_VARIAB_V3, "VarFlag3", vobsSTRING_PROPERTY, NULL, NULL, NULL,
                        "Variability type among C,D,M,P,R and U");

        /* binary / multiple flags (midi / ASCC ...) */
        AddPropertyMeta(vobsSTAR_CODE_MULT_FLAG, "MultFlag", vobsSTRING_PROPERTY, NULL, NULL, NULL,
                        "Multiplicity type among C,G,O,V, X or SB (for decoded spectral binaries)");
        AddPropertyMeta(vobsSTAR_CODE_BIN_FLAG, "BinFlag", vobsSTRING_PROPERTY, NULL, NULL, NULL,
                        "Multiplicity type among SB, eclipsing B or S (for suspicious binaries in spectral type)");

        AddPropertyMeta(vobsSTAR_ID_SB9, "SBC9", vobsSTRING_PROPERTY, vobsSTAR_PROP_NOT_SET, "%.0lf",
                        "http://vizier.u-strasbg.fr/viz-bin/VizieR?-source=B/sb9&amp;-out.form=%2bH&amp;-corr=FK=Seq&amp;-out.all=1&amp;-out.max=9999&amp;Seq===%20${SBC9}",
                        "SBC9 identifier, click to call VizieR on this object");

        AddPropertyMeta(vobsSTAR_ID_WDS, "WDS", vobsSTRING_PROPERTY, vobsSTAR_PROP_NOT_SET, "%.0lf",
                        "http://vizier.u-strasbg.fr/viz-bin/VizieR?-source=B/wds/wds&amp;-out.form=%2bH&amp;-out.all=1&amp;-out.max=9999&amp;WDS===${WDS}",
                        "WDS identifier, click to call VizieR on this object");

        /* WDS separation 1 and 2 */
        AddPropertyMeta(vobsSTAR_ORBIT_SEPARATION_SEP1, "sep1", vobsFLOAT_PROPERTY, "arcsec", "%.1lf", NULL,
                        "Angular Separation of the binary on first observation");
        AddPropertyMeta(vobsSTAR_ORBIT_SEPARATION_SEP2, "sep2", vobsFLOAT_PROPERTY, "arcsec", "%.1lf", NULL,
                        "Angular Separation of the binary on last observation");

        /* Denis IFlag */
        AddPropertyMeta(vobsSTAR_CODE_MISC_I, "Iflag", vobsSTRING_PROPERTY);
        /* 2MASS quality flag */
        AddPropertyMeta(vobsSTAR_CODE_QUALITY, "Qflag", vobsSTRING_PROPERTY);

        /* Hipparcos radial velocity */
        AddPropertyMeta(vobsSTAR_VELOC_HC, "RadVel", vobsSTRING_PROPERTY, "km/s", NULL, NULL,
                        "Radial Velocity");

        /* BSC rotational velocity */
        AddPropertyMeta(vobsSTAR_VELOC_ROTAT, "RotVel", vobsSTRING_PROPERTY, "km/s", NULL, NULL,
                        "Rotation Velocity (v sini)");

        /* Borde et Merand UD */
        AddPropertyMeta(vobsSTAR_UDDK_DIAM, "UDDK", vobsFLOAT_PROPERTY, "mas", NULL, NULL,
                        "Uniform-Disc Diameter in K-band");
        AddPropertyMeta(vobsSTAR_UDDK_DIAM_ERROR, "e_UDDK", vobsFLOAT_PROPERTY, "mas", NULL, NULL,
                        "Error on Uniform-Disc Diameter in K-band");

        AddPropertyMeta(vobsSTAR_DIAM12, "Dia12", vobsFLOAT_PROPERTY, "mas", NULL, NULL,
                        "Angular Diameter at 12 microns");
        AddPropertyMeta(vobsSTAR_DIAM12_ERROR, "e_dia12", vobsFLOAT_PROPERTY, "mas", NULL, NULL,
                        "Error on Angular Diameter at 12 microns");

        /* Johnson / photometric fluxes */
        AddPropertyMeta(vobsSTAR_PHOT_JHN_B, "B", vobsFLOAT_PROPERTY, "mag", NULL, NULL,
                        "Johnson's Magnitude in B-band");
        AddPropertyMeta(vobsSTAR_PHOT_PHG_B, "Bphg", vobsFLOAT_PROPERTY, "mag", NULL, NULL,
                        "Photometric Magnitude in B-band");

        AddPropertyMeta(vobsSTAR_PHOT_JHN_V, "V", vobsFLOAT_PROPERTY, "mag", NULL, NULL,
                        "Johnson's Magnitude in V-band");

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
        AddPropertyMeta(vobsSTAR_PHOT_COUS_J, "Jcous", vobsFLOAT_PROPERTY, "mag", NULL, NULL,
                        "Cousin's Magnitude in J-band");

        AddPropertyMeta(vobsSTAR_PHOT_JHN_H, "H", vobsFLOAT_PROPERTY, "mag", NULL, NULL,
                        "Johnson's Magnitude in H-band");
        AddPropertyMeta(vobsSTAR_PHOT_COUS_H, "Hcous", vobsFLOAT_PROPERTY, "mag", NULL, NULL,
                        "Cousin's Magnitude in H-band");

        AddPropertyMeta(vobsSTAR_PHOT_JHN_K, "K", vobsFLOAT_PROPERTY, "mag", NULL, NULL,
                        "Johnson's Magnitude in K-band");
        AddPropertyMeta(vobsSTAR_PHOT_COUS_K, "Kcous", vobsFLOAT_PROPERTY, "mag", NULL, NULL,
                        "Cousin's Magnitude in K-band");

        AddPropertyMeta(vobsSTAR_PHOT_JHN_L, "L", vobsFLOAT_PROPERTY, "mag", NULL, NULL,
                        "Johnson's Magnitude in L-band");

        AddPropertyMeta(vobsSTAR_PHOT_JHN_M, "M", vobsFLOAT_PROPERTY, "mag", NULL, NULL,
                        "Johnson's Magnitude in M-band");

        AddPropertyMeta(vobsSTAR_PHOT_JHN_N, "N", vobsFLOAT_PROPERTY, "mag", NULL, NULL,
                        "Johnson's Magnitude in N-band");

        /* MIDI local catalog */
        AddPropertyMeta(vobsSTAR_IR_FLUX_ORIGIN, "orig", vobsSTRING_PROPERTY, NULL, NULL, NULL,
                        "Source of the IR Flux among IRAS or MSX");

        /* AKARI flux (9 mu) */
        AddPropertyMeta(vobsSTAR_PHOT_FLUX_IR_09, "S09", vobsFLOAT_PROPERTY, "Jy", NULL, NULL,
                        "Mid-Infrared Flux Density at 9 microns");
        AddPropertyMeta(vobsSTAR_PHOT_FLUX_IR_09_ERROR, "e_S09", vobsFLOAT_PROPERTY, "Jy", NULL, NULL,
                        "Relative Error on Mid-Infrared Flux Density at 9 microns");

        AddPropertyMeta(vobsSTAR_PHOT_FLUX_IR_12, "F12", vobsFLOAT_PROPERTY, "Jy", NULL, NULL,
                        "Mid-Infrared Flux at 12 microns");
        AddPropertyMeta(vobsSTAR_PHOT_FLUX_IR_12_ERROR, "e_F12", vobsFLOAT_PROPERTY, "Jy", NULL, NULL,
                        "Relative Error on Mid-Infrared Flux at 12 microns");

        /* AKARI flux (18 mu) */
        AddPropertyMeta(vobsSTAR_PHOT_FLUX_IR_18, "S18", vobsFLOAT_PROPERTY, "Jy", NULL, NULL,
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

        // End of Meta data

        vobsSTAR::vobsSTAR_PropertyMetaEnd = vobsSTAR::vobsStar_PropertyMetaList.size();

        logTest("vobsSTAR has defined %d properties.", vobsSTAR::vobsSTAR_PropertyMetaEnd);

        if (vobsSTAR::vobsSTAR_PropertyMetaEnd != vobsSTAR_MAX_PROPERTIES)
        {
            logWarning("sclsvrCALIBRATOR_MAX_PROPERTIES constant is incorrect: %d != %d",
                       vobsSTAR_MAX_PROPERTIES, vobsSTAR::vobsSTAR_PropertyMetaEnd);
        }

        initializeIndex();

        // Get property indexes for RA/DEC:
        vobsSTAR::vobsSTAR_PropertyRAIndex = vobsSTAR::GetPropertyIndex(vobsSTAR_POS_EQ_RA_MAIN);
        vobsSTAR::vobsSTAR_PropertyDECIndex = vobsSTAR::GetPropertyIndex(vobsSTAR_POS_EQ_DEC_MAIN);

        // Get property index for Target identifier:
        vobsSTAR::vobsSTAR_PropertyTargetIdIndex = vobsSTAR::GetPropertyIndex(vobsSTAR_ID_TARGET);

        // Get property indexes for PMRA/PMDEC:
        vobsSTAR::vobsSTAR_PropertyPMRAIndex = vobsSTAR::GetPropertyIndex(vobsSTAR_POS_EQ_PMRA);
        vobsSTAR::vobsSTAR_PropertyPMDECIndex = vobsSTAR::GetPropertyIndex(vobsSTAR_POS_EQ_PMDEC);

        // Get property indexes for JD:
        vobsSTAR::vobsSTAR_PropertyJDIndex = vobsSTAR::GetPropertyIndex(vobsSTAR_JD_DATE);

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

    for (vobsSTAR_PROPERTY_META_PTR_LIST::iterator iter = vobsSTAR::vobsStar_PropertyMetaList.begin();
         iter != vobsSTAR::vobsStar_PropertyMetaList.end(); iter++)
    {
        delete(*iter);
    }
    vobsSTAR::vobsStar_PropertyMetaList.clear();

    vobsSTAR::vobsSTAR_PropertyMetaBegin = -1;
    vobsSTAR::vobsSTAR_PropertyMetaEnd = -1;
    vobsSTAR::vobsSTAR_PropertyIdxInitialized = false;

    vobsSTAR::vobsSTAR_PropertyRAIndex = -1;
    vobsSTAR::vobsSTAR_PropertyDECIndex = -1;
    vobsSTAR::vobsSTAR_PropertyTargetIdIndex = -1;
}

/**
 * Convert right ascension (RA) coordinate from HMS (HH MM SS.TT) into degrees [-180; 180]
 *
 * @param raHms right ascension (RA) coordinate in HMS (HH MM SS.TT)
 * @param ra pointer on an already allocated mcsDOUBLE value.
 *
 * @return mcsSUCCESS on successful completion, mcsFAILURE otherwise.
 */
mcsCOMPL_STAT vobsSTAR::GetRa(const char* raHms, mcsDOUBLE &ra)
{
    mcsDOUBLE hh, hm, hs;

    FAIL_COND_DO(sscanf(raHms, "%lf %lf %lf", &hh, &hm, &hs) != 3, errAdd(vobsERR_INVALID_RA_FORMAT, raHms));

    // Get sign of hh which has to be propagated to hm and hs
    mcsDOUBLE sign = (raHms[0] == '-') ? -1.0 : 1.0;

    // Convert to degrees
    ra = (hh + sign * (hm + hs * vobsSEC_IN_MIN) * vobsMIN_IN_HOUR) * vobsHA_IN_DEG;

    // Set angle range [-180; 180]
    if (ra > 180.0)
    {
        ra -= 360.0;
    }

    return mcsSUCCESS;
}

/**
 * Convert declinaison (DEC) coordinate from DMS (DD MM SS.TT) into degrees [-90; 90]
 *
 * @param decDms declinaison (DEC) coordinate in DMS (DD MM SS.TT)
 * @param dec pointer on an already allocated mcsDOUBLE value.
 *
 * @return mcsSUCCESS on successful completion, mcsFAILURE otherwise.
 */
mcsCOMPL_STAT vobsSTAR::GetDec(const char* decDms, mcsDOUBLE &dec)
{
    mcsDOUBLE dd, dm, ds;

    FAIL_COND_DO(sscanf(decDms, "%lf %lf %lf", &dd, &dm, &ds) != 3, errAdd(vobsERR_INVALID_DEC_FORMAT, decDms));

    // Get sign of hh which has to be propagated to dm and ds
    mcsDOUBLE sign = (decDms[0] == '-') ? -1.0 : 1.0;

    // Convert to degrees
    dec = dd + sign * (dm + ds * vobsSEC_IN_MIN) * vobsMIN_IN_HOUR;

    return mcsSUCCESS;
}

/**
 * Convert right ascension (RA) coordinate from degrees [-180; 180] into HMS (HH MM SS.TTT)
 *
 * @param ra right ascension (RA) in degrees
 * @param raHms returned right ascension (RA) coordinate in HMS (HH MM SS.TTT)
 */
void vobsSTAR::ToHms(mcsDOUBLE ra, mcsSTRING32 &raHms)
{
    // Be sure RA is positive [0 - 360]
    if (ra < 0.0)
    {
        ra += 360.0;
    }

    // convert ra in hour angle [0;24]:
    ra *= vobsDEG_IN_HA;

    mcsDOUBLE hh = trunc(ra);
    mcsDOUBLE hm = trunc((ra - hh) * 60.0);
    mcsDOUBLE hs = ((ra - hh) * 60.0 - hm) * 60.0;

    sprintf(raHms, "%02.0lf %02.0lf %05.2lf", fabs(hh), fabs(hm), fabs(hs));
}

/**
 * Convert declinaison (DEC) coordinate from degrees [-90; 90] into DMS (+/-DD MM SS.TT)
 *
 * @param dec declination (DEC) in degrees
 * @param decDms returned declinaison (DEC) coordinate in DMS (+/-DD MM SS.TT)
 */
void vobsSTAR::ToDms(mcsDOUBLE dec, mcsSTRING32 &decDms)
{
    mcsDOUBLE dd = trunc(dec);
    mcsDOUBLE dm = trunc((dec - dd) * 60.0);
    mcsDOUBLE ds = ((dec - dd) * 60.0 - dm) * 60.0;

    sprintf(decDms, "%c%02.0lf %02.0lf %04.1lf", (dec < 0) ? '-' : '+', fabs(dd), fabs(dm), fabs(ds));
}

/** 
 * Convert right ascension (RA) coordinate from degrees [-180; 180] into degrees (xxx.xxxxxx) 
 *
 * @param ra right ascension (RA) in degrees
 * @param raDeg returned right ascension (RA) coordinate in degrees (xxx.xxxxxx)
 */
void vobsSTAR::raToDeg(mcsDOUBLE ra, mcsSTRING16 &raDeg)
{
    // Be sure RA is positive [0 - 360]
    if (ra < 0.0)
    {
        ra += 360.0;
    }

    sprintf(raDeg, "%010.6lf", ra);
}

/** 
 * Convert declinaison (DEC) coordinate from degrees [-90; 90] into degrees (+/-xx.xxxxxx) 
 *
 * @param dec declination (DEC) in degrees
 * @param decDms returned declinaison (DEC) coordinate in degrees (+/-xx.xxxxxx)
 */
void vobsSTAR::decToDeg(mcsDOUBLE dec, mcsSTRING16 &decDeg)
{
    sprintf(decDeg, "%+09.6lf", dec);
}

/**
 * Convert concatenated RA/DEC 'xxx.xxxxxx(+/-)xx.xxxxxx' coordinates into degrees
 * 
 * @param raDec concatenated right ascension (RA) and declination in degrees
 * @param ra pointer on an already allocated mcsDOUBLE value.
 * @param dec pointer on an already allocated mcsDOUBLE value.
 */
mcsCOMPL_STAT vobsSTAR::degToRaDec(const char* raDec, mcsDOUBLE &ra, mcsDOUBLE &dec)
{
    mcsDOUBLE raDeg, decDeg;

    FAIL_COND_DO(sscanf(raDec, "%10lf%10lf", &raDeg, &decDeg) != 2, errAdd(vobsERR_INVALID_RA_FORMAT, raDec));

    // Set angle range [-180; 180]
    if (raDeg > 180.0)
    {
        raDeg -= 360.0;
    }

    ra = raDeg;
    dec = decDeg;

    return mcsSUCCESS;
}

void vobsSTAR::ClearRaDec(void)
{
    vobsSTAR_PROPERTY* property;
    property = GetProperty(vobsSTAR::vobsSTAR_PropertyRAIndex);
    if (property != NULL)
    {
        property->ClearValue();
    }
    property = GetProperty(vobsSTAR::vobsSTAR_PropertyDECIndex);
    if (property != NULL)
    {
        property->ClearValue();
    }

    // define ra/dec to blanking value:
    _ra = EMPTY_COORD_DEG;
    _dec = EMPTY_COORD_DEG;
}

void vobsSTAR::SetRaDec(const mcsDOUBLE ra, const mcsDOUBLE dec) const
{
    // fix parsed RA / DEC but not RA / DEC in sexagesimal format:
    _ra = ra;
    _dec = dec;
}

mcsCOMPL_STAT vobsSTAR::PrecessRaDecToEpoch(const mcsDOUBLE epoch, mcsDOUBLE &raEpo, mcsDOUBLE &decEpo) const
{
    mcsDOUBLE ra, dec;
    mcsDOUBLE pmRa, pmDec; // max/yr

    FAIL(GetRa(ra));
    FAIL(GetDec(dec));
    FAIL(GetPmRa(pmRa));
    FAIL(GetPmDec(pmDec));

    // ra/dec coordinates are corrected from 2000 to the catalog's epoch:
    raEpo = vobsSTAR::GetPrecessedRA(ra, pmRa, EPOCH_2000, epoch);
    decEpo = vobsSTAR::GetPrecessedDEC(dec, pmDec, EPOCH_2000, epoch);

    return mcsSUCCESS;
}

mcsCOMPL_STAT vobsSTAR::CorrectRaDecToEpoch(const mcsDOUBLE pmRa, const mcsDOUBLE pmDec, mcsDOUBLE epoch) const
{
    mcsDOUBLE ra, dec;

    FAIL(GetRa(ra))
    FAIL(GetDec(dec))

    logDebug("CorrectRaDecToEpoch: epoch %.3lf", epoch);

    // ra/dec coordinates are corrected from the catalog's epoch to 2000:
    ra = vobsSTAR::GetPrecessedRA(ra, pmRa, epoch, EPOCH_2000);
    dec = vobsSTAR::GetPrecessedDEC(dec, pmDec, epoch, EPOCH_2000);

    SetRaDec(ra, dec);

    return mcsSUCCESS;
}


// TODO: move into alx

mcsDOUBLE vobsSTAR::GetPrecessedRA(const mcsDOUBLE raDeg, const mcsDOUBLE pmRa, const mcsDOUBLE epochRa, const mcsDOUBLE epoch)
{
    mcsDOUBLE ra = raDeg;

    const mcsDOUBLE deltaEpoch = epoch - epochRa;

    if ((deltaEpoch != 0.0) && (pmRa != 0.0))
    {
        ra += GetDeltaRA(pmRa, deltaEpoch);

        // TODO: fix non linearities arround due to cos(dec) !

        // Set angle range [-180; 180]
        if (ra > 180.0)
        {
            ra -= 360.0;
        }
        else if (ra < -180.0)
        {
            ra += 360.0;
        }

        if (DO_LOG_PRECESS)
        {
            logTest("ra  (%.3lf to %.3lf): %.6lf => %.6lf", epochRa, epoch, raDeg, ra);
        }
    }

    return ra;
}

// TODO: move into alx

mcsDOUBLE vobsSTAR::GetPrecessedDEC(const mcsDOUBLE decDeg, const mcsDOUBLE pmDec, const mcsDOUBLE epochDec, const mcsDOUBLE epoch)
{
    mcsDOUBLE dec = decDeg;

    const mcsDOUBLE deltaEpoch = epoch - epochDec;

    if ((deltaEpoch != 0.0) && (pmDec != 0.0))
    {
        dec += GetDeltaDEC(pmDec, deltaEpoch);

        // TODO: fix non linearities arround due to cos(dec) !

        // Set angle range [-90; 90]
        if (dec > 90.0)
        {
            dec = 180.0 - dec;
        }
        else if (dec < -90.0)
        {
            dec = -180.0 - dec;
        }

        if (DO_LOG_PRECESS)
        {
            logTest("dec (%.3lf to %.3lf): %.6lf => %.6lf", epochDec, epoch, decDeg, dec);
        }
    }

    return dec;
}

// TODO: move into alx
// note: pmRA is given divided by cos(DEJ2000) !!

mcsDOUBLE vobsSTAR::GetDeltaRA(const mcsDOUBLE pmRa, const mcsDOUBLE deltaEpoch)
{
    // pmRA is given divided by cos(dec) because pmRA is given in RA*cos(DE) cf ASCC (Proper Motion in RA*cos(DE)):

    // RAJ2000_ep2000 "RAJ2000+(2000-1991.25)*pmRA/cos(DEJ2000*PI/180)/1000/3600"
    return deltaEpoch * 1e-3 * alxARCSEC_IN_DEGREES * pmRa;
}

mcsDOUBLE vobsSTAR::GetDeltaDEC(const mcsDOUBLE pmDec, const mcsDOUBLE deltaEpoch)
{
    // DEJ2000_ep2000 "DEJ2000+(2000-1991.25)*pmDE/1000/3600"        
    return deltaEpoch * 1e-3 * alxARCSEC_IN_DEGREES * pmDec;
}

/*___oOo___*/
