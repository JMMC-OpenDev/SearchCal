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
#include "vobsPrivate.h"
#include "vobsErrors.h"

/* Maximum number of properties:
 *   - vobsSTAR (65)
 *   - sclsvrCALIBRATOR (116) */
#define vobsSTAR_MAX_PROPERTIES 65

/** Initialize static members */
PropertyIndexMap vobsSTAR::vobsSTAR_PropertyIdx;
PropertyMetaList vobsSTAR::vobsStar_PropertyMetaList;

int  vobsSTAR::vobsSTAR_PropertyMetaBegin      = -1;
int  vobsSTAR::vobsSTAR_PropertyMetaEnd        = -1;
bool vobsSTAR::vobsSTAR_PropertyIdxInitialized = false;

int  vobsSTAR::vobsSTAR_PropertyRAIndex  = -1;
int  vobsSTAR::vobsSTAR_PropertyDECIndex = -1;

int  vobsSTAR::vobsSTAR_PropertyWaveLengthIndex  = -1;
int  vobsSTAR::vobsSTAR_PropertyFluxIndex        = -1;

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

        // Copy (clone) the property list:
        ReserveProperties(vobsSTAR_MAX_PROPERTIES);

        vobsSTAR_PROPERTY* propertyRef;
        for (PropertyList::const_iterator iter = star._propertyList.begin(); iter != star._propertyList.end(); iter++)
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
 * Clear property list
 */
void vobsSTAR::Clear()
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
    if (property == NULL)
    {
        // Return error
        return mcsFAILURE;
    }
    
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
    if (property == NULL)
    {
        // Return error
        return mcsFAILURE;
    }
    
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
    if (property == NULL)
    {
        // Return error
        return mcsFAILURE;
    }

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
    if (IsPropertySet(property) == mcsFALSE)
    {
        // if not, return error
        errAdd(vobsERR_RA_NOT_SET);
        return mcsFAILURE;
    }
    
    mcsSTRING32 raHms;
    strcpy(raHms, GetPropertyValue(property));
    
    if (GetRa(raHms, ra) == mcsFAILURE) {
        return mcsFAILURE;
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
    if (IsPropertySet(property) == mcsFALSE)
    {
        // if not, return error
        errAdd(vobsERR_DEC_NOT_SET);
        return mcsFAILURE;
    }

    mcsSTRING32 decDms;
    strcpy(decDms, GetPropertyValue(property));
    
    if (GetDec(decDms, dec) == mcsFAILURE) {
        return mcsFAILURE;
    }
  
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
 * Update a star with the properies of another given one.
 * By default ( overwrite = mcsFALSE ) it does not modify the content if
 * the property has already been set.
 * @param star the other star.
 * @param overwrite a true flag indicates to copy the value even if it is
 * already set. (default value set to false)
 * @param propertyUpdated integer array storing updated counts per property index (int)
 *
 * @return mcsTRUE if this star has been updated (at least one property changed)
 */
mcsLOGICAL vobsSTAR::Update(const vobsSTAR &star, mcsLOGICAL overwrite, mcsINT32* propertyUpdated)
{
    const bool isLogDebug = doLog(logDEBUG);
    mcsLOGICAL updated    = mcsFALSE;

    vobsSTAR_PROPERTY* property;
    vobsSTAR_PROPERTY* starProperty;
    
    // For each star property
    for (int idx = 0, len = NbProperties(); idx < len; idx++)
    {
        // Retrieve the properties at the current index
        property = GetProperty(idx);
        starProperty = star.GetProperty(idx);

        // If the current property is not yet defined
        if (IsPropertySet(property) == mcsFALSE || overwrite == mcsTRUE)
        {
            // Use the property from the given star if existing!
            if (star.IsPropertySet(starProperty) == mcsTRUE)
            {
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
    mcsDOUBLE   starRa  = 0.0;
    mcsDOUBLE   starDec = 0.0;

    GetId(starId, sizeof(starId));

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
        for (PropertyIndexMap::iterator iter = vobsSTAR::vobsSTAR_PropertyIdx.begin(); 
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
        for (PropertyIndexMap::iterator iter = vobsSTAR::vobsSTAR_PropertyIdx.begin(); 
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
 * Display only set star properties on the console on a single line.
 */
void vobsSTAR::Dump(const char* separator) const
{
    mcsSTRING64 starId;
    mcsDOUBLE   starRa  = 0.0;
    mcsDOUBLE   starDec = 0.0;

    GetId(starId, sizeof(starId));

    if (IsPropertySet(vobsSTAR::vobsSTAR_PropertyRAIndex) == mcsTRUE)
    {
        GetRa(starRa);
    }
    if (IsPropertySet(vobsSTAR::vobsSTAR_PropertyDECIndex) == mcsTRUE)
    {
        GetDec(starDec);
    }
    printf("%s (RA = %lf, DEC = %lf): ", starId, starRa, starDec);
    
    vobsSTAR_PROPERTY* property;
    for (PropertyList::const_iterator iter = _propertyList.begin(); iter != _propertyList.end(); iter++)
    {
        property = (*iter);

        if (property->IsSet())
        {
            printf("%s = %s%s", property->GetId(), property->GetValue(), separator);
        }
    }
    printf("\n");
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
    
    PropertyList propListLeft  = _propertyList;
    PropertyList propListRight = other._propertyList;
    
    PropertyList::const_iterator iLeft, iRight;

    vobsSTAR_PROPERTY* propLeft;
    vobsSTAR_PROPERTY* propRight;
    
    mcsLOGICAL setLeft, setRight;
    const char *val1Str, *val2Str;
    
    for (iLeft  = propListLeft.begin(), iRight = propListRight.begin();
         iLeft != propListLeft.end() && iRight != propListRight.end(); iLeft++, iRight++)
    {
        propLeft  = *iLeft;
        propRight = *iRight;
        
        setLeft  = propLeft->IsSet();
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
        } else {
            // properties are not both set:
            if (setLeft == mcsTRUE)
            {
                lDiff++;
                diffLeft << propLeft->GetId() << " = '" << propLeft->GetValue() << "' ";
                
            } else if (setRight == mcsTRUE)
            {
                rDiff++;
                diffRight << propRight->GetId() << " = '" << propRight->GetValue() << "' ";
            }
        }
    }
    
    if (lDiff > 0 || rDiff > 0)
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

        AddPropertyMeta(vobsSTAR_ID_2MASS, "2MASS", vobsSTRING_PROPERTY, vobsSTAR_PROP_NOT_SET, NULL,
                    "http://vizier.u-strasbg.fr/viz-bin/VizieR?-source=II/246/out&amp;-out=2MASS&amp;2MASS=${2MASS}",
                    "2MASS identifier, click to call VizieR on this object");

        AddPropertyMeta(vobsSTAR_ID_DENIS, "DENIS", vobsSTRING_PROPERTY, vobsSTAR_PROP_NOT_SET, "%.0lf", NULL,
                    "DENIS identifier");  

        AddPropertyMeta(vobsSTAR_ID_SB9, "SBC9", vobsSTRING_PROPERTY, vobsSTAR_PROP_NOT_SET, "%.0lf",
                    "http://vizier.u-strasbg.fr/viz-bin/VizieR?-source=B/sb9&amp;-out.form=%2bH&amp;-corr=FK=Seq&amp;-out.all=1&amp;-out.max=9999&amp;Seq===%20${SBC9}",
                    "SBC9 identifier, click to call VizieR on this object");

        AddPropertyMeta(vobsSTAR_ID_WDS, "WDS", vobsSTRING_PROPERTY, vobsSTAR_PROP_NOT_SET, "%.0lf",
                    "http://vizier.u-strasbg.fr/viz-bin/VizieR?-source=B/wds/wds&amp;-out.form=%2bH&amp;-out.all=1&amp;-out.max=9999&amp;WDS===${WDS}",
                    "WDS identifier, click to call VizieR on this object");

        /* 2MASS Associated optical source (opt) 'T' for Tycho 2 */
        AddPropertyMeta(vobsSTAR_ID_CATALOG, "opt", vobsSTRING_PROPERTY);  

        AddPropertyMeta(vobsSTAR_POS_EQ_RA_MAIN, "RAJ2000", vobsSTRING_PROPERTY,
                    "h:m:s", NULL, "http://simbad.u-strasbg.fr/simbad/sim-coo?CooDefinedFrames=none&amp;Coord=${RAJ2000}%20${DEJ2000}&amp;CooEpoch=2000&amp;CooFrame=FK5&amp;CooEqui=2000&amp;Radius.unit=arcsec&amp;Radius=1",
                    "Right Ascencion - J2000");
        AddPropertyMeta(vobsSTAR_POS_EQ_DEC_MAIN, "DEJ2000", vobsSTRING_PROPERTY,
                    "d:m:s", NULL, "http://simbad.u-strasbg.fr/simbad/sim-coo?CooDefinedFrames=none&amp;Coord=${RAJ2000}%20${DEJ2000}&amp;CooEpoch=2000&amp;CooFrame=FK5&amp;CooEqui=2000&amp;Radius.unit=arcsec&amp;Radius=1",
                    "Declination - J2000");
        
        /* RA/DEC OTHER (DENIS): useful ? */
        AddPropertyMeta(vobsSTAR_POS_EQ_RA_OTHER, "A2RAdeg", vobsSTRING_PROPERTY, "h:m:s");
        AddPropertyMeta(vobsSTAR_POS_EQ_DEC_OTHER, "A2DEdeg", vobsSTRING_PROPERTY, "d:m:s");

        AddPropertyMeta(vobsSTAR_POS_EQ_PMRA, "pmRa", vobsFLOAT_PROPERTY, "mas/yr", NULL, NULL,
                    "Proper Motion in Right Ascension");
        AddPropertyMeta(vobsSTAR_POS_EQ_PMDEC, "pmDec", vobsFLOAT_PROPERTY, "mas/yr", NULL, NULL,
                    "Proper Motion in Declination");

        AddPropertyMeta(vobsSTAR_POS_PARLX_TRIG, "plx", vobsFLOAT_PROPERTY, "mas", "%.2lf", NULL,
                    "Trigonometric Parallaxe");
        AddPropertyMeta(vobsSTAR_POS_PARLX_TRIG_ERROR, "e_Plx", vobsFLOAT_PROPERTY, NULL, NULL, NULL,
                    "Error on Parallaxe");

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
        AddPropertyMeta(vobsSTAR_CODE_BIN_FLAG, "BinFlag", vobsSTRING_PROPERTY, NULL, NULL, NULL,
                    "Multiplicity type among SB, eclipsing B or S (for suspicious binaries in spectral type)");
        AddPropertyMeta(vobsSTAR_CODE_MULT_FLAG, "MultFlag", vobsSTRING_PROPERTY, NULL, NULL, NULL,
                    "Multiplicity type among C,G,O,V, X or SB (for decoded spectral binaries)");

        /* WDS separation 1 and 2 */
        AddPropertyMeta(vobsSTAR_ORBIT_SEPARATION_SEP1, "sep1", vobsFLOAT_PROPERTY, "arcsec", "%.1lf", NULL,
                    "Angular Separation of the binary on first observation");
        AddPropertyMeta(vobsSTAR_ORBIT_SEPARATION_SEP2, "sep2", vobsFLOAT_PROPERTY, "arcsec", "%.1lf", NULL,
                    "Angular Separation of the binary on last observation");

        /* Denis IFlag */
        AddPropertyMeta(vobsSTAR_CODE_MISC_I, "Iflag", vobsSTRING_PROPERTY);
        /* 2MASS quality flag */
        AddPropertyMeta(vobsSTAR_CODE_QUALITY, "Qflag", vobsSTRING_PROPERTY);

        /* galactic positions can be computed: useless ? */
        AddPropertyMeta(vobsSTAR_POS_GAL_LAT, "GLAT", vobsFLOAT_PROPERTY, "deg", "%.2lf", NULL,
                    "Galactic Latitude");
        AddPropertyMeta(vobsSTAR_POS_GAL_LON, "GLON", vobsFLOAT_PROPERTY, "deg", "%.2lf", NULL,
                    "Galactic Longitude");

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

        /* skipped CIO UCD (wavelength / IR flux) = NOT properties */

        /* Johnson / photometric fluxes */
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
        
        /* MIDI local catalog */
        AddPropertyMeta(vobsSTAR_IR_FLUX_ORIGIN, "orig", vobsSTRING_PROPERTY, NULL, NULL, NULL,
                    "Source of the IR Flux among IRAS or MSX");

        AddPropertyMeta(vobsSTAR_PHOT_FLUX_IR_12, "F12",  vobsFLOAT_PROPERTY, "Jy", NULL, NULL,
                    "Mid-Infrared Flux at 12 microns");
        AddPropertyMeta(vobsSTAR_PHOT_FLUX_IR_12_ERROR, "e_F12", vobsFLOAT_PROPERTY, "Jy", NULL, NULL,
                    "Relative Error on Mid-Infrared Flux at 12 microns");

        AddPropertyMeta(vobsSTAR_REF_STAR, "Calib", vobsSTRING_PROPERTY);

        AddPropertyMeta(vobsSTAR_PHYS_TEMP_EFFEC, "Teff", vobsFLOAT_PROPERTY, NULL, NULL, NULL,
                    "Effective Temperature");
        AddPropertyMeta(vobsSTAR_PHYS_TEMP_EFFEC_ERROR, "e_Teff", vobsFLOAT_PROPERTY, NULL, NULL, NULL,
                    "Error on Effective Temperature");
        
        AddPropertyMeta(vobsSTAR_DIAM12, "Dia12", vobsFLOAT_PROPERTY, "mas", NULL, NULL,
                    "Angular Diameter at 12 microns");
        AddPropertyMeta(vobsSTAR_DIAM12_ERROR, "e_dia12", vobsFLOAT_PROPERTY, "mas", NULL, NULL,
                    "Error on Angular Diameter at 12 microns");

        AddPropertyMeta(vobsSTAR_PHOT_EXTINCTION_TOTAL, "A_V", vobsFLOAT_PROPERTY, NULL, NULL, NULL,
                    "Visible Interstellar Absorption");
        AddPropertyMeta(vobsSTAR_CHI2_QUALITY, "Chi2", vobsFLOAT_PROPERTY, NULL, NULL, NULL,
                    "Chi2 of Spectro-Photmometric Data Model Fitting");
        AddPropertyMeta(vobsSTAR_SP_TYP_PHYS_TEMP_EFFEC, "SpTyp_Teff", vobsFLOAT_PROPERTY, NULL, NULL, NULL,
                    "Spectral Type from adopted Modelling Effective Temperature");
        
        /* AKARI fluxes (9, 12, 18 mu) */
        AddPropertyMeta(vobsSTAR_PHOT_FLUX_IR_09, "S09",  vobsFLOAT_PROPERTY, "Jy", NULL, NULL,
                    "Mid-Infrared Flux Density at 9 microns");
        AddPropertyMeta(vobsSTAR_PHOT_FLUX_IR_09_ERROR, "e_S09", vobsFLOAT_PROPERTY, "Jy", NULL, NULL,
                    "Relative Error on Mid-Infrared Flux Density at 9 microns");

        AddPropertyMeta(vobsSTAR_PHOT_FLUX_IR_18, "S18",  vobsFLOAT_PROPERTY, "Jy", NULL, NULL,
                    "Mid-Infrared Flux Density at 18 microns");
        AddPropertyMeta(vobsSTAR_PHOT_FLUX_IR_18_ERROR, "e_S18", vobsFLOAT_PROPERTY, "Jy", NULL, NULL,
                    "Relative Error on Mid-Infrared Flux Density at 18 microns");

        // End of Meta data
        
        vobsSTAR::vobsSTAR_PropertyMetaEnd = vobsSTAR::vobsStar_PropertyMetaList.size();
        
        logTest("vobsSTAR has defined %d properties.", vobsSTAR::vobsSTAR_PropertyMetaEnd);

        initializeIndex();

        // Get property indexes for RA/DEC:
        vobsSTAR::vobsSTAR_PropertyRAIndex  = vobsSTAR::GetPropertyIndex(vobsSTAR_POS_EQ_RA_MAIN);
        vobsSTAR::vobsSTAR_PropertyDECIndex = vobsSTAR::GetPropertyIndex(vobsSTAR_POS_EQ_DEC_MAIN);

        // Get property indexes for wavelength/flux:
        vobsSTAR::vobsSTAR_PropertyWaveLengthIndex = vobsSTAR::GetPropertyIndex(vobsSTAR_INST_WAVELENGTH_VALUE);
        vobsSTAR::vobsSTAR_PropertyFluxIndex       = vobsSTAR::GetPropertyIndex(vobsSTAR_PHOT_FLUX_IR_MISC);
        
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
    
    vobsSTAR::vobsSTAR_PropertyRAIndex  = -1;
    vobsSTAR::vobsSTAR_PropertyDECIndex = -1;

    vobsSTAR::vobsSTAR_PropertyWaveLengthIndex = -1;
    vobsSTAR::vobsSTAR_PropertyFluxIndex       = -1;
}

/**
 * Convert right ascension (RA) coordinate in degrees [-180; 180]
 *
 * @param raHms right ascension (RA) coordinate in HMS (HH:MM:SS.TT or HH MM SS.TT)
 * @param ra pointer on an already allocated mcsDOUBLE value.
 *
 * @return mcsSUCCESS on successful completion, mcsFAILURE otherwise.
 */
mcsCOMPL_STAT vobsSTAR::GetRa(mcsSTRING32 raHms, mcsDOUBLE &ra)
{
    mcsDOUBLE    hh, hm, hs;

    // RA can be given as HH:MM:SS.TT or HH MM SS.TT. 
    // Replace ':' by ' ', and remove trailing and leading pace
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
    ra  = (hh + sign * (hm / 60.0 + hs / 3600.0)) * 15.0;

    // Set angle range [-180; 180]
    if (ra > 180.0)
    {
        ra -= 360.0;
    }

    return mcsSUCCESS;
}

/**
 * Convert declinaison (DEC) coordinate in degrees [-90; 90]
 *
 * @param decDms declinaison (DEC) coordinate in DMS (DD:MM:SS.TT or DD MM SS.TT)
 * @param dec pointer on an already allocated mcsDOUBLE value.
 *
 * @return mcsSUCCESS on successful completion, mcsFAILURE otherwise.
 */
mcsCOMPL_STAT vobsSTAR::GetDec(mcsSTRING32 decDms, mcsDOUBLE &dec)
{

    mcsDOUBLE dd,dm,ds;

    // DEC can be given as DD:MM:SS.TT or DD MM SS.TT. 
    // Replace ':' by ' ', and remove trailing and leading pace
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

    return mcsSUCCESS;
}

/*___oOo___*/
