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
#include <stdlib.h>
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
 *   - vobsSTAR (100 max)
 *   - sclsvrCALIBRATOR (141 max) */
#define vobsSTAR_MAX_PROPERTIES (alxIsNotLowMemFlag() ? (alxIsDevFlag() ? 100 : 82) : 68)

void vobsGetXmatchColumnsFromOriginIndex(vobsORIGIN_INDEX originIndex,
                                         const char** propIdNMates, const char** propIdScore, const char** propIdSep,
                                         const char** propIdDmag, const char** propIdSep2nd)
{
    switch (originIndex)
    {
        case vobsCATALOG_ASCC_ID:
            *propIdNMates = vobsSTAR_XM_ASCC_N_MATES;
            *propIdSep = vobsSTAR_XM_ASCC_SEP;
            *propIdDmag = NULL;
            *propIdSep2nd = vobsSTAR_XM_ASCC_SEP_2ND;
            break;
        case vobsCATALOG_HIP1_ID:
        case vobsCATALOG_HIP2_ID:
            *propIdNMates = vobsSTAR_XM_HIP_N_MATES;
            *propIdSep = vobsSTAR_XM_HIP_SEP;
            *propIdDmag = NULL;
            *propIdSep2nd = NULL;
            break;
        case vobsCATALOG_MASS_ID:
            *propIdNMates = vobsSTAR_XM_2MASS_N_MATES;
            *propIdSep = vobsSTAR_XM_2MASS_SEP;
            *propIdDmag = NULL;
            *propIdSep2nd = vobsSTAR_XM_2MASS_SEP_2ND;
            break;
        case vobsCATALOG_WISE_ID:
            *propIdNMates = vobsSTAR_XM_WISE_N_MATES;
            *propIdSep = vobsSTAR_XM_WISE_SEP;
            *propIdDmag = NULL;
            *propIdSep2nd = vobsSTAR_XM_WISE_SEP_2ND;
            break;
        case vobsCATALOG_GAIA_ID:
            *propIdNMates = vobsSTAR_XM_GAIA_N_MATES;
            *propIdScore = vobsSTAR_XM_GAIA_SCORE;
            *propIdSep = vobsSTAR_XM_GAIA_SEP;
            *propIdDmag = vobsSTAR_XM_GAIA_DMAG;
            *propIdSep2nd = vobsSTAR_XM_GAIA_SEP_2ND;
            break;
        default:
            break;
    }
}

bool vobsIsMainCatalogFromOriginIndex(vobsORIGIN_INDEX originIndex)
{
    switch (originIndex)
    {
        case vobsCATALOG_ASCC_ID:
            return true;
        case vobsCATALOG_HIP1_ID:
        case vobsCATALOG_HIP2_ID:
            return false; // ignored
        case vobsCATALOG_MASS_ID:
            return true;
        case vobsCATALOG_WISE_ID:
            return false; // ignored
        case vobsCATALOG_GAIA_ID:
            return true;
        default:
            return false; // ignored
    }
}


/** Initialize static members */
vobsSTAR_PROPERTY_INDEX_MAPPING vobsSTAR::vobsSTAR_PropertyIdx;
vobsSTAR_PROPERTY_INDEX_MAPPING vobsSTAR::vobsSTAR_PropertyErrorIdx;

mcsINT32 vobsSTAR::vobsSTAR_PropertyMetaBegin = -1;
mcsINT32 vobsSTAR::vobsSTAR_PropertyMetaEnd = -1;
bool vobsSTAR::vobsSTAR_PropertyIdxInitialized = false;

mcsINT32 vobsSTAR::vobsSTAR_PropertyRAIndex = -1;
mcsINT32 vobsSTAR::vobsSTAR_PropertyDECIndex = -1;
mcsINT32 vobsSTAR::vobsSTAR_PropertyXmLogIndex = -1;
mcsINT32 vobsSTAR::vobsSTAR_PropertyXmMainFlagIndex = -1;
mcsINT32 vobsSTAR::vobsSTAR_PropertyXmAllFlagIndex = -1;
mcsINT32 vobsSTAR::vobsSTAR_PropertyTargetIdIndex = -1;
mcsINT32 vobsSTAR::vobsSTAR_PropertyPMRAIndex = -1;
mcsINT32 vobsSTAR::vobsSTAR_PropertyPMDECIndex = -1;
mcsINT32 vobsSTAR::vobsSTAR_PropertyJDIndex = -1;

/*
 * Class constructor
 */
/* macro for constructor */
#define vobsSTAR_CTOR_IMPL(nProperties) \
    ClearCache(); \
 \
    _nProps = nProperties; \
    _properties = new vobsSTAR_PROPERTY[_nProps]; /* using empty constructor */ \
 \
    /* fix meta data index: */ \
    for (mcsUINT32 p = 0; p < _nProps; p++) \
    { \
        _properties[p].SetMetaIndex(p); \
    }

/**
 * Build a star object.
 */
vobsSTAR::vobsSTAR(mcsUINT8 nProperties)
{
    vobsSTAR_CTOR_IMPL(nProperties);
}

/**
 * Build a star object.
 */
vobsSTAR::vobsSTAR()
{
    vobsSTAR_CTOR_IMPL(vobsSTAR_MAX_PROPERTIES);

    // Add all star properties
    AddProperties();
}

/**
 * Build a star object from another one (copy constructor).
 */
vobsSTAR::vobsSTAR(const vobsSTAR &star)
{
    vobsSTAR_CTOR_IMPL(vobsSTAR_MAX_PROPERTIES);

    // Uses the operator=() method to copy
    *this = star;
}

/**
 * Assignment operator
 */
vobsSTAR& vobsSTAR::operator=(const vobsSTAR& star)
{
    if (this != &star)
    {
        ClearValues();

        // copy the parsed ra/dec:
        _ra = star._ra;
        _dec = star._dec;

        // Copy (clone) the property list:
        for (mcsUINT32 p = 0, end = mcsMIN(_nProps, star._nProps); p < end; p++)
        {
            _properties[p] = star._properties[p];
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
    ClearCache();

    if (IS_NOT_NULL(_properties))
    {
        // calls destructor for all properties:
        delete[](_properties);
        _properties = NULL;
    }
}

/**
 * Clear cached values (ra, dec)
 */
void vobsSTAR::ClearCache()
{
    // define ra/dec to blanking value:
    _ra  = EMPTY_COORD_DEG;
    _dec = EMPTY_COORD_DEG;
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
    FAIL_FALSE_DO(isPropSet(property), errAdd(vobsERR_RA_NOT_SET));

    // Copy ra value to be able to fix its format:
    const char* raHms = GetPropertyValue(property);
    mcsSTRING32 raValue;
    strcpy(raValue, raHms);

    mcsINT32 status = GetRa(raValue, ra);
    FAIL(status);

    if (status == 2)
    {
        logInfo("Fixed ra format: '%s' to '%s'", raHms, raValue);

        // do fix property value:
        property->SetValue(raValue, property->GetOriginIndex(), property->GetConfidenceIndex(), mcsTRUE);
    }

    // cache value:
    _ra = ra;

    return mcsSUCCESS;
}

/**
 * Get declination (DEC) coordinate in degrees.
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
    FAIL_FALSE_DO(isPropSet(property), errAdd(vobsERR_DEC_NOT_SET));

    // Copy dec value to be able to fix its format:
    const char* decDms = GetPropertyValue(property);
    mcsSTRING32 decValue;
    strcpy(decValue, decDms);

    mcsINT32 status = GetDec(decValue, dec);
    FAIL(status);

    if (status == 2)
    {
        logInfo("Fixed dec format: '%s' to '%s'", decDms, decValue);

        // do fix property value:
        property->SetValue(decValue, property->GetOriginIndex(), property->GetConfidenceIndex(), mcsTRUE);
    }

    // cache value:
    _dec = dec;

    return mcsSUCCESS;
}

mcsCOMPL_STAT vobsSTAR::GetRaDec(mcsDOUBLE &ra, mcsDOUBLE &dec) const
{
    FAIL(GetRa(ra));
    FAIL(GetDec(dec));
    return mcsSUCCESS;
}

/**
 * Get right ascension (RA) coordinate in degrees of the reference star.
 *
 * @param ra pointer on an already allocated mcsDOUBLE value.
 *
 * @return mcsSUCCESS on successful completion, mcsFAILURE otherwise.
 */
mcsCOMPL_STAT vobsSTAR::GetRaDecRefStar(mcsDOUBLE &raRef, mcsDOUBLE &decRef) const
{
    vobsSTAR_PROPERTY* targetIdProperty = GetTargetIdProperty();

    // Check if the value is set
    FAIL_FALSE(isPropSet(targetIdProperty));

    // Parse Target identifier '016.417537-41.369444':
    const char* targetId = GetPropertyValue(targetIdProperty);

    // parse values:
    FAIL(degToRaDec(targetId, raRef, decRef));

    return mcsSUCCESS;
}

/**
 * Return the star PMRA in mas/yr DIVIDED by cos(dec)
 * @param pmRa pointer on an already allocated mcsDOUBLE value.
 * @return mcsSUCCESS on successful completion, mcsFAILURE otherwise.
 */
mcsCOMPL_STAT vobsSTAR::GetPmRa(mcsDOUBLE &pmRa) const
{
    vobsSTAR_PROPERTY* property = GetProperty(vobsSTAR::vobsSTAR_PropertyPMRAIndex);

    // Check if the value is set
    if (isNotPropSet(property))
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
 * Return the star PMDEC in mas/yr
 * @param pmDec pointer on an already allocated mcsDOUBLE value.
 * @return mcsSUCCESS on successful completion, mcsFAILURE otherwise.
 */
mcsCOMPL_STAT vobsSTAR::GetPmDec(mcsDOUBLE &pmDec) const
{
    vobsSTAR_PROPERTY* property = GetProperty(vobsSTAR::vobsSTAR_PropertyPMDECIndex);

    // Check if the value is set
    if (isNotPropSet(property))
    {
        pmDec = 0.0;
        return mcsSUCCESS;
    }

    FAIL(property->GetValue(&pmDec));

    return mcsSUCCESS;
}

mcsCOMPL_STAT vobsSTAR::GetPmRaDec(mcsDOUBLE &pmRa, mcsDOUBLE &pmDec) const
{
    FAIL(GetPmRa(pmRa));
    FAIL(GetPmDec(pmDec));
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
    if (isNotPropSet(property))
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
mcsCOMPL_STAT vobsSTAR::GetId(char* starId, mcsUINT32 maxLength) const
{
    const char* propertyValue = NULL;
    vobsSTAR_PROPERTY* property = NULL;

    --maxLength;

    // ID must be unique (not ambiguous one) to ensure stable identifiers (among releases)
    // Use following priorities:
    // 1. TYCHO ('TYC @TYC1-@TYC2-@TYC3') | GAIA DR2 ID ('Gaia DR2 @ID')
    // 2. 'HIP @ID' | 'HD @ID' | 'DM @ID' (| 'DENIS @ID') (ambiguous = double stars)
    // 3. '2MASS J@ID' | 'WISE J@ID' (less precise)
    // 4. 'RA DEC' (hms/dms) coords
    // else: '????' (no id)

    // 1. TYCHO ('TYC @TYC1-@TYC2-@TYC3')
    property = GetProperty(vobsSTAR_ID_TYC1);
    if (isPropSet(property))
    {
        const char* tyc1 = GetPropertyValue(property);
        if (IS_NOT_NULL(tyc1))
        {
            // TYC 8979-1780-1
            property = GetProperty(vobsSTAR_ID_TYC2);
            if (isPropSet(property))
            {
                const char* tyc2 = GetPropertyValue(property);
                if (IS_NOT_NULL(tyc2))
                {
                    property = GetProperty(vobsSTAR_ID_TYC3);
                    if (isPropSet(property))
                    {
                        const char* tyc3 = GetPropertyValue(property);
                        if (IS_NOT_NULL(tyc3))
                        {
                            snprintf(starId, maxLength, "TYC %s-%s-%s", tyc1, tyc2, tyc3);
                            return mcsSUCCESS;
                        }
                    }
                }
            }
        }
    }

    // 1. | GAIA DR3 ID ('Gaia DR3 @ID')
    property = GetProperty(vobsSTAR_ID_GAIA);
    if (isPropSet(property))
    {
        propertyValue = GetPropertyValue(property);
        if (IS_NOT_NULL(propertyValue))
        {
            snprintf(starId, maxLength, "Gaia DR3 %s", propertyValue);
            return mcsSUCCESS;
        }
    }

    // 2. 'HIP @ID' | 'HD @ID' | 'HD @ID' (| 'DENIS @ID') (ambiguous = double stars)
    property = GetProperty(vobsSTAR_ID_HIP);
    if (isPropSet(property))
    {
        propertyValue = GetPropertyValue(property);
        if (IS_NOT_NULL(propertyValue))
        {
            snprintf(starId, maxLength, "HIP %s", propertyValue);
            return mcsSUCCESS;
        }
    }
    property = GetProperty(vobsSTAR_ID_HD);
    if (isPropSet(property))
    {
        propertyValue = GetPropertyValue(property);
        if (IS_NOT_NULL(propertyValue))
        {
            snprintf(starId, maxLength, "HD %s", propertyValue);
            return mcsSUCCESS;
        }
    }
    property = GetProperty(vobsSTAR_ID_DM);
    if (isPropSet(property))
    {
        propertyValue = GetPropertyValue(property);
        if (IS_NOT_NULL(propertyValue))
        {
            snprintf(starId, maxLength, "DM %s", propertyValue);
            return mcsSUCCESS;
        }
    }
    if (vobsCATALOG_DENIS_ID_ENABLE)
    {
        property = GetProperty(vobsSTAR_ID_DENIS);
        if (isPropSet(property))
        {
            propertyValue = GetPropertyValue(property);
            if (IS_NOT_NULL(propertyValue))
            {
                snprintf(starId, maxLength, "DENIS %s", propertyValue);
                return mcsSUCCESS;
            }
        }
    }

    // 3. '2MASS J@ID' | 'WISE J@ID' (less precise)
    property = GetProperty(vobsSTAR_ID_2MASS);
    if (isPropSet(property))
    {
        propertyValue = GetPropertyValue(property);
        if (IS_NOT_NULL(propertyValue))
        {
            snprintf(starId, maxLength, "2MASS J%s", propertyValue);
            return mcsSUCCESS;
        }
    }
    property = GetProperty(vobsSTAR_ID_WISE);
    if (isPropSet(property))
    {
        propertyValue = GetPropertyValue(property);
        if (IS_NOT_NULL(propertyValue))
        {
            snprintf(starId, maxLength, "WISE J%s", propertyValue);
            return mcsSUCCESS;
        }
    }

    // 4. 'RA DEC' (hms/dms) coords
    property = GetProperty(vobsSTAR::vobsSTAR_PropertyRAIndex);
    vobsSTAR_PROPERTY* propertyDec = GetProperty(vobsSTAR::vobsSTAR_PropertyDECIndex);
    if (isPropSet(property) && isPropSet(propertyDec))
    {
        const char* raValue = GetPropertyValue(property);
        const char* decValue = GetPropertyValue(propertyDec);
        // HMS DMS format:
        snprintf(starId, maxLength, "%s %s", raValue, decValue);
        return mcsSUCCESS;
    }

    // else: '????' (no id)
    strncpy(starId, "????", maxLength);
    return mcsSUCCESS;
}

/**
 * Update a star with the properties of another given one.
 * By default ( overwrite = mcsFALSE ) it does not modify the content if
 * the property has already been set.
 * @param star the other star.
 * @param overwrite a true flag indicates to copy the value even if it is
 * already set. (default value set to false)
 * @param optional overwrite Property Mask
 * @param propertyUpdated integer array storing updated counts per property index (integer)
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

    const bool isPartialOverwrite = (overwrite == vobsOVERWRITE_PARTIAL);

    if (isPartialOverwrite && IS_NULL(overwritePropertyMask))
    {
        // invalid case: disable overwrite:
        overwrite = vobsOVERWRITE_NONE;
    }

    bool isPropSet;
    vobsSTAR_PROPERTY* property;
    vobsSTAR_PROPERTY* starProperty;

    // For each star property
    for (mcsINT32 idx = 0, len = NbProperties(); idx < len; idx++)
    {
        // Retrieve the properties at the current index
        property = GetProperty(idx);

        if ((idx == vobsSTAR_PropertyXmLogIndex) || (idx == vobsSTAR_PropertyXmMainFlagIndex)
                || (idx == vobsSTAR_PropertyXmAllFlagIndex) || (strcmp(property->GetId(), vobsSTAR_GROUP_SIZE) == 0))
        {
            // always overwrite (internal)
            isPropSet = mcsFALSE;
        }
        else
        {
            // Is the current property set ?
            isPropSet = IsPropertySet(property);
        }

        // If the current property is not yet defined
        if (!isPropSet || (overwrite != vobsOVERWRITE_NONE))
        {
            starProperty = star.GetProperty(idx);

            // Use the property from the given star if existing
            if (isPropSet(starProperty))
            {
                if (isPropSet)
                {
                    // Never overwrite values coming from SIMBAD or JSDC FAINT (simbad derived)
                    if ((property->GetOriginIndex() == vobsCATALOG_JSDC_FAINT_LOCAL_ID )
                            || (property->GetOriginIndex() == vobsCATALOG_SIMBAD_ID))
                    {
                        if (starProperty->GetOriginIndex() != vobsCATALOG_GAIA_ID)
                        {
                            continue;
                        }
                    }

                    // Test overwrite property mask only if the current property is already set:
                    if (isPartialOverwrite)
                    {
                        if (isLogDebug)
                        {
                            logDebug("overwritePropertyMask[%s][%d] = %s", property->GetName(), idx, (*overwritePropertyMask)[idx] ? "true" : "false");
                        }

                        if (!(*overwritePropertyMask)[idx])
                        {
                            // property should not be updated: skip it.
                            continue;
                        }
                    }
                }

                // TODO: implement better overwrite mode (check property error or scoring ...)

                // replace property by using assignment operator:
                *property = *starProperty;

                if (isLogDebug)
                {
                    logDebug("updated property[%s] = '%s'.", starProperty->GetId(), starProperty->GetSummaryString().c_str());
                }

                // statistics:
                updated = mcsTRUE;

                if (IS_NOT_NULL(propertyUpdated))
                {
                    propertyUpdated[idx]++;
                }
            }
            else if (isPropSet && isPartialOverwrite)
            {
                // Clear values (for example GAIA missing pmRA/DE)
                if (isLogDebug)
                {
                    logDebug("overwritePropertyMask[%s][%d] = %s", property->GetName(), idx, (*overwritePropertyMask)[idx] ? "true" : "false");
                }

                if (!(*overwritePropertyMask)[idx])
                {
                    // property should not be updated: skip it.
                    continue;
                }

                // clear property value:
                property->ClearValue();

                if (isLogDebug)
                {
                    logDebug("cleared property[%s].", property->GetId());
                }

                // statistics:
                updated = mcsTRUE;

                if (IS_NOT_NULL(propertyUpdated))
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
    // Get Star ID
    mcsSTRING64 starId;
    GetId(starId, sizeof (starId));

    mcsDOUBLE starRa = NAN;
    mcsDOUBLE starDec = NAN;

    if (IS_TRUE(IsPropertySet(vobsSTAR::vobsSTAR_PropertyRAIndex)))
    {
        GetRa(starRa);
    }
    if (IS_TRUE(IsPropertySet(vobsSTAR::vobsSTAR_PropertyDECIndex)))
    {
        GetDec(starDec);
    }
    printf("'%s' (%lf,%lf): ", starId, starRa, starDec);

    vobsSTAR_PROPERTY* property;
    mcsSTRING32 converted;

    if (IS_FALSE(showPropId))
    {
        for (vobsSTAR_PROPERTY_INDEX_MAPPING::iterator iter = vobsSTAR::vobsSTAR_PropertyIdx.begin();
                iter != vobsSTAR::vobsSTAR_PropertyIdx.end(); iter++)
        {
            property = GetProperty(iter->second);

            if (IS_NOT_NULL(property))
            {
                if (property->GetType() == vobsSTRING_PROPERTY)
                {
                    printf("%12s", property->GetValueOrBlank());
                }
                else
                {
                    property->GetFormattedValue(converted);
                    printf("%12s", converted);
                }
                if (IS_NOT_NULL(property->GetErrorMeta()))
                {
                    property->GetFormattedError(converted);
                    printf("%12s", converted);
                }
            }
        }
        printf("\n");
    }
    else
    {
        for (vobsSTAR_PROPERTY_INDEX_MAPPING::iterator iter = vobsSTAR::vobsSTAR_PropertyIdx.begin();
                iter != vobsSTAR::vobsSTAR_PropertyIdx.end(); iter++)
        {
            property = GetProperty(iter->second);

            if (IS_NOT_NULL(property))
            {
                if (property->GetType() == vobsSTRING_PROPERTY)
                {
                    printf("%12s = %12s\n", property->GetId(), property->GetValueOrBlank());
                }
                else
                {
                    property->GetFormattedValue(converted);
                    printf("%12s = %12s\n", property->GetId(), converted);
                }
                if (IS_NOT_NULL(property->GetErrorMeta()))
                {
                    property->GetFormattedError(converted);
                    printf("%12s = %12s\n", property->GetErrorId(), converted);
                }
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
    output[0] = '\0';
    char* outPtr = output;

    // Get Star ID
    mcsSTRING64 tmp;
    GetId(tmp, sizeof (tmp));

    mcsDOUBLE starRa = NAN;
    mcsDOUBLE starDec = NAN;

    if (IS_TRUE(IsPropertySet(vobsSTAR::vobsSTAR_PropertyRAIndex)))
    {
        GetRa(starRa);
    }
    if (IS_TRUE(IsPropertySet(vobsSTAR::vobsSTAR_PropertyDECIndex)))
    {
        GetDec(starDec);
    }
    sprintf(output, "'%s' (RA = %.9lf, DEC = %.9lf): ", tmp, starRa, starDec);

    outPtr += strlen(output);

    mcsSTRING32 converted;

    for (mcsUINT32 p = 0; p < _nProps; p++)
    {
        // vobsSTAR_PROPERTY* property = (*iter);
        vobsSTAR_PROPERTY* property = &(_properties[p]);

        if (isPropSet(property))
        {
            if (IS_NOT_NULL(strstr(property->GetId(), vobsSTAR_XM_PREFIX)))
            {
                // skip any XMATCH columns
                continue;
            }
            if (property->GetType() == vobsSTRING_PROPERTY)
            {
                snprintf(tmp, sizeof (tmp) - 1, "%s = %s%s", property->GetId(), property->GetValue(), separator);
            }
            else
            {
                property->GetFormattedValue(converted);
                snprintf(tmp, sizeof (tmp) - 1, "%s = %s%s", property->GetId(), converted, separator);
            }
            vobsStrcatFast(outPtr, tmp);
        }
    }
}

/**
 * Compare stars (i.e values)
 * @param other other vobsSTAR instance (or sub class)
 * @return 0 if equals; < 0 if this star has less properties than other; > 0 else
 */
mcsINT32 vobsSTAR::compare(const vobsSTAR& other) const
{
    mcsINT32 common = 0, lDiff = 0, rDiff = 0;
    ostringstream same, diffLeft, diffRight;

    vobsSTAR_PROPERTY* propLeft;
    vobsSTAR_PROPERTY* propRight;

    mcsLOGICAL setLeft, setRight;
    const char *val1Str, *val2Str;
    mcsDOUBLE val1, val2;

    mcsINT32 nPropsLeft = NbProperties();
    mcsINT32 nPropsRight = other.NbProperties();

    for (mcsINT32 pLeft = 0, pRight = 0; (pLeft < nPropsLeft) && (pRight < nPropsRight); pLeft++, pRight++)
    {
        propLeft = &_properties[pLeft];
        propRight = &other._properties[pRight];

        setLeft = propLeft->IsSet();
        setRight = propRight->IsSet();

        if (propLeft->GetType() == vobsSTRING_PROPERTY)
        {
            if (setLeft == setRight)
            {
                if (IS_TRUE(setLeft))
                {
                    // properties are both set; compare values as string:
                    val1Str = propLeft->GetValue();
                    val2Str = propRight->GetValue();

                    if (strcmp(val1Str, val2Str) == 0)
                    {
                        common++;
                        same << propLeft->GetId() << " = '" << val1Str << "' ";
                    }
                    else
                    {
                        lDiff++;
                        diffLeft << propLeft->GetId() << " = '" << val1Str << "' ";

                        rDiff++;
                        diffRight << propRight->GetId() << " = '" << val2Str << "' ";
                    }
                }
            }
            else
            {
                // properties are not both set:
                if (IS_TRUE(setLeft))
                {
                    lDiff++;
                    diffLeft << propLeft->GetId() << " = '" << propLeft->GetValue() << "' ";

                }
                else if (IS_TRUE(setRight))
                {
                    rDiff++;
                    diffRight << propRight->GetId() << " = '" << propRight->GetValue() << "' ";
                }
            }
        }
        else
        {
            if (setLeft == setRight)
            {
                if (IS_TRUE(setLeft))
                {
                    // properties are both set; compare values as double:
                    propLeft->GetValue(&val1);
                    propRight->GetValue(&val2);

                    if (val1 == val2)
                    {
                        common++;
                        same << propLeft->GetId() << " = '" << val1 << "' ";
                    }
                    else
                    {
                        lDiff++;
                        diffLeft << propLeft->GetId() << " = '" << val1 << "' ";

                        rDiff++;
                        diffRight << propRight->GetId() << " = '" << val2 << "' ";
                    }
                }
            }
            else
            {
                // properties are not both set:
                if (IS_TRUE(setLeft))
                {
                    propLeft->GetValue(&val1);
                    lDiff++;
                    diffLeft << propLeft->GetId() << " = '" << val1 << "' ";

                }
                else if (IS_TRUE(setRight))
                {
                    propRight->GetValue(&val2);
                    rDiff++;
                    diffRight << propRight->GetId() << " = '" << val2 << "' ";
                }
            }
        }
    }

    if ((lDiff > 0) || (rDiff > 0))
    {
        const mcsINT32 diff = lDiff - rDiff;

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

/**
 * Are both stars equals (i.e all values)
 * @param other other vobsSTAR instance (or sub class)
 * @return true if equals; false otherwise
 */
bool vobsSTAR::equals(const vobsSTAR& other) const
{
    vobsSTAR_PROPERTY* propLeft;
    vobsSTAR_PROPERTY* propRight;

    mcsLOGICAL setLeft, setRight;
    const char *val1Str, *val2Str;
    mcsDOUBLE val1, val2;

    mcsINT32 nPropsLeft = NbProperties();
    mcsINT32 nPropsRight = other.NbProperties();

    for (mcsINT32 pLeft = 0, pRight = 0; (pLeft < nPropsLeft) && (pRight < nPropsRight); pLeft++, pRight++)
    {
        propLeft = &_properties[pLeft];
        propRight = &other._properties[pRight];

        setLeft = propLeft->IsSet();
        setRight = propRight->IsSet();

        if (propLeft->GetType() == vobsSTRING_PROPERTY)
        {
            if (setLeft == setRight)
            {
                if (IS_TRUE(setLeft))
                {
                    // properties are both set; compare values as string:
                    val1Str = propLeft->GetValue();
                    val2Str = propRight->GetValue();

                    if (strcmp(val1Str, val2Str) != 0)
                    {
                        return false;
                    }
                }
            }
            else
            {
                // properties are not both set:
                return false;
            }
        }
        else
        {
            if (setLeft == setRight)
            {
                if (IS_TRUE(setLeft))
                {
                    // properties are both set; compare values as double:
                    propLeft->GetValue(&val1);
                    propRight->GetValue(&val2);

                    if (val1 != val2)
                    {
                        return false;
                    }
                }
            }
            else
            {
                // properties are not both set:
                return false;
            }
        }
    }
    return true;
}

/*
 * Private methods
 */

/**
 * Add a new property meta
 *
 * @param id property identifier (UCD)
 * @param name property name
 * @param type property type
 * @param unit property unit, "" by default or for 'NULL'.
 * @param description property description (none by default or for 'NULL').
 * @param link link for this property (none by default or for 'NULL').
 */
void vobsSTAR::AddPropertyMeta(const char* id, const char* name,
                               const vobsPROPERTY_TYPE type, const char* unit,
                               const char* description, const char* link)
{
    AddFormattedPropertyMeta(id, name, type, unit, NULL, description, link);
}

/**
 * Add a new property meta with a custom format
 *
 * @param id property identifier (UCD)
 * @param name property name
 * @param type property type
 * @param unit property unit, "" by default or for 'NULL'.
 * @param format format used to set property (%s or %.5g by default or for 'NULL').
 * @param description property description (none by default or for 'NULL').
 * @param link link for this property (none by default or for 'NULL').
 */
void vobsSTAR::AddFormattedPropertyMeta(const char* id, const char* name,
                                        const vobsPROPERTY_TYPE type, const char* unit, const char* format,
                                        const char* description, const char* link)
{
    // Create a new property from the given parameters (no format given)
    const vobsSTAR_PROPERTY_META* propertyMeta = new vobsSTAR_PROPERTY_META(id, name, type, unit, format, link, description);

    // Add the new property meta data to the internal list (copy):
    vobsSTAR_PROPERTY_META::vobsStar_PropertyMetaList.push_back(propertyMeta);
}

/**
 * Add a new error property meta to the previously added property meta
 *
 * @param id error identifier (UCD)
 * @param name error name
 * @param unit error unit, "" by default or for 'NULL'.
 * @param description error description (none by default or for 'NULL').
 */
void vobsSTAR::AddPropertyErrorMeta(const char* id, const char* name,
                                    const char* unit, const char* description)
{
    if (vobsSTAR_PROPERTY_META::vobsStar_PropertyMetaList.empty())
    {
        logError("No property meta defined; can not add the error property meta '%s'", id);
    }
    else
    {
        // Create a new error property meta data from the given parameters (no format given)
        const vobsSTAR_PROPERTY_META* errorMeta = new vobsSTAR_PROPERTY_META(id, name, vobsFLOAT_PROPERTY, unit, NULL, NULL, description, true);

        // Add the property error meta data to last property meta data:
        vobsSTAR_PROPERTY_META::vobsStar_PropertyMetaList.back()->SetErrorMeta(errorMeta);
    }
}

/**
 * Initialize the shared property index (NOT THREAD SAFE)
 */
void vobsSTAR::initializeIndex(void)
{
    // Fill the property index:
    const char* propertyId;
    const char* propertyErrorId;
    const vobsSTAR_PROPERTY_META* errorMeta;
    mcsUINT32 i = 0;

    for (vobsSTAR_PROPERTY_META_PTR_LIST::iterator iter = vobsSTAR_PROPERTY_META::vobsStar_PropertyMetaList.begin();
            iter != vobsSTAR_PROPERTY_META::vobsStar_PropertyMetaList.end(); iter++, i++)
    {
        propertyId = (*iter)->GetId();

        if (vobsSTAR::GetPropertyIndex(propertyId) == -1)
        {
            vobsSTAR::vobsSTAR_PropertyIdx.insert(vobsSTAR_PROPERTY_INDEX_PAIR(propertyId, i));
        }

        // Add the property error identifier too
        errorMeta = (*iter)->GetErrorMeta();
        if (IS_NOT_NULL(errorMeta))
        {
            propertyErrorId = errorMeta->GetId();

            if (vobsSTAR::GetPropertyErrorIndex(propertyErrorId) == -1)
            {
                vobsSTAR::vobsSTAR_PropertyErrorIdx.insert(vobsSTAR_PROPERTY_INDEX_PAIR(propertyErrorId, i));
            }
        }
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
        vobsSTAR::vobsSTAR_PropertyMetaBegin = vobsSTAR_PROPERTY_META::vobsStar_PropertyMetaList.size();

        // Add Meta data:

        /* Identifiers */
        AddPropertyMeta(vobsSTAR_ID_HD, "HD", vobsSTRING_PROPERTY, NULL,
                        "HD identifier, click to call Simbad on this object",
                        "http://simbad.u-strasbg.fr/simbad/sim-id?protocol=html&amp;Ident=HD${HD}");

        AddPropertyMeta(vobsSTAR_ID_HIP, "HIP", vobsSTRING_PROPERTY, NULL,
                        "HIP identifier, click to call Simbad on this object",
                        "http://simbad.u-strasbg.fr/simbad/sim-id?protocol=html&amp;Ident=HIP${HIP}");

        AddPropertyMeta(vobsSTAR_ID_DM, "DM", vobsSTRING_PROPERTY, NULL,
                        "DM number, click to call Simbad on this object",
                        "http://simbad.u-strasbg.fr/simbad/sim-id?protocol=html&amp;Ident=DM%20${DM}");

        AddPropertyMeta(vobsSTAR_ID_TYC1, "TYC1", vobsSTRING_PROPERTY, NULL,
                        "TYC1 number from Tycho-2, click to call Simbad on this object",
                        "http://simbad.u-strasbg.fr/simbad/sim-id?protocol=html&amp;Ident=TYC%20${TYC1}-${TYC2}-${TYC3}");
        AddPropertyMeta(vobsSTAR_ID_TYC2, "TYC2", vobsSTRING_PROPERTY, NULL,
                        "TYC2 number from Tycho-2, click to call Simbad on this object",
                        "http://simbad.u-strasbg.fr/simbad/sim-id?protocol=html&amp;Ident=TYC%20${TYC1}-${TYC2}-${TYC3}");
        AddPropertyMeta(vobsSTAR_ID_TYC3, "TYC3", vobsSTRING_PROPERTY, NULL,
                        "TYC3 number from Tycho-2, click to call Simbad on this object",
                        "http://simbad.u-strasbg.fr/simbad/sim-id?protocol=html&amp;Ident=TYC%20${TYC1}-${TYC2}-${TYC3}");

        AddPropertyMeta(vobsSTAR_ID_2MASS, "2MASS", vobsSTRING_PROPERTY, NULL,
                        "2MASS identifier, click to call VizieR on this object",
                        "http://vizier.u-strasbg.fr/viz-bin/VizieR?-source=II/246/out&amp;-out=2MASS&amp;2MASS=${2MASS}");

        if (vobsCATALOG_DENIS_ID_ENABLE && alxIsNotLowMemFlag())
        {
            AddPropertyMeta(vobsSTAR_ID_DENIS, "DENIS", vobsSTRING_PROPERTY, NULL,
                            "DENIS identifier, click to call VizieR on this object",
                            "http://vizier.u-strasbg.fr/viz-bin/VizieR?-source=B/denis/denis&amp;DENIS===${DENIS}");
        }

        AddPropertyMeta(vobsSTAR_ID_SB9, "SBC9", vobsSTRING_PROPERTY, NULL,
                        "SBC9 identifier, click to call VizieR on this object",
                        "http://vizier.u-strasbg.fr/viz-bin/VizieR?-source=B/sb9&amp;-out.form=%2bH&amp;-corr=FK=Seq&amp;-out.all=1&amp;-out.max=9999&amp;Seq===%20${SBC9}");

        AddPropertyMeta(vobsSTAR_ID_WDS, "WDS", vobsSTRING_PROPERTY, NULL,
                        "WDS identifier, click to call VizieR on this object",
                        "http://vizier.u-strasbg.fr/viz-bin/VizieR?-source=B/wds/wds&amp;-out.form=%2bH&amp;-out.all=1&amp;-out.max=9999&amp;WDS===${WDS}");

        if (alxIsNotLowMemFlag())
        {
            AddPropertyMeta(vobsSTAR_ID_AKARI, "AKARI", vobsSTRING_PROPERTY, NULL,
                            "AKARI source identifier, click to call VizieR on this object",
                            "http://vizier.u-strasbg.fr/viz-bin/VizieR?-source=II/297/irc&amp;objID=${AKARI}");
        }
        AddPropertyMeta(vobsSTAR_ID_WISE, "WISE", vobsSTRING_PROPERTY, NULL,
                        "WISE identifier, click to call VizieR on this object",
                        "http://vizier.u-strasbg.fr/viz-bin/VizieR?-source=II%2F328&amp;AllWISE=${WISE}");

        AddPropertyMeta(vobsSTAR_ID_GAIA, "GAIA", vobsSTRING_PROPERTY, NULL,
                        "GAIA DR3 identifier, click to call VizieR on this object",
                        "http://vizier.u-strasbg.fr/viz-bin/VizieR?-source=I%2F355%2Fgaiadr3&amp;Source=${GAIA}");

        /* SIMBAD Identifier (queried) */
        AddPropertyMeta(vobsSTAR_ID_SIMBAD, "SIMBAD", vobsSTRING_PROPERTY, NULL,
                        "Simbad Identifier, click to call Simbad on this object",
                        "http://simbad.u-strasbg.fr/simbad/sim-id?protocol=html&amp;Ident=${SIMBAD}");

        /* 2MASS Associated optical source (opt) 'T' for Tycho 2 or 'U' for USNO A 2.0 */
        AddPropertyMeta(vobsSTAR_2MASS_OPT_ID_CATALOG, "opt", vobsSTRING_PROPERTY, NULL,
                        "2MASS: Associated optical source (opt) 'T' for Tycho 2 or 'U' for USNO A 2.0");

        /* Crossmatch information */
        /* CDS TargetId used by internal crossmatchs (filtered in VOTable output) */
        AddPropertyMeta(vobsSTAR_ID_TARGET, "TARGET_ID", vobsSTRING_PROPERTY, "",
                        "The target identifier asked to CDS");

        /* Catalog observation date (JD) (filtered in VOTable output) */
        AddPropertyMeta(vobsSTAR_JD_DATE, "jd", vobsFLOAT_PROPERTY, "d",
                        "(jdate) Julian date of source measurement");

        AddPropertyMeta(vobsSTAR_XM_MAIN_FLAGS, "XMATCH_MAIN_FLAG", vobsINT_PROPERTY, NULL,
                        "xmatch flags for main catalogs (ASCC, GAIA, 2MASS) (internal JMMC)");
        /* xmatch informations for main catalogs */
        AddPropertyMeta(vobsSTAR_XM_SIMBAD_SEP, "XM_SIMBAD_sep", vobsFLOAT_PROPERTY, "as",
                        "Angular Separation of the first object in SIMBAD");

        if (alxIsDevFlag() && alxIsNotLowMemFlag())
        {
            /* Crossmatch log for main catalogs */
            AddPropertyMeta(vobsSTAR_XM_LOG, "XMATCH_LOG", vobsSTRING_PROPERTY, NULL,
                            "xmatch log (internal JMMC)");
            AddPropertyMeta(vobsSTAR_XM_ALL_FLAGS, "XMATCH_ALL_FLAG", vobsINT_PROPERTY, NULL,
                            "xmatch flags for all catalogs (internal JMMC)");

            AddPropertyMeta(vobsSTAR_XM_ASCC_N_MATES, "XM_ASCC_n_mates", vobsINT_PROPERTY, NULL,
                            "Number of mates within 3 as in ASCC catalog");
            AddPropertyMeta(vobsSTAR_XM_ASCC_SEP, "XM_ASCC_sep", vobsFLOAT_PROPERTY, "as",
                            "Angular Separation of the first object in ASCC catalog");
            AddPropertyMeta(vobsSTAR_XM_ASCC_SEP_2ND, "XM_ASCC_sep_2nd", vobsFLOAT_PROPERTY, "as",
                            "Angular Separation between first and second objects in ASCC catalog");

            AddPropertyMeta(vobsSTAR_XM_HIP_N_MATES, "XM_HIP_n_mates", vobsINT_PROPERTY, NULL,
                            "Number of mates within 3 as in HIP1/2 catalogs");
            AddPropertyMeta(vobsSTAR_XM_HIP_SEP, "XM_HIP_sep", vobsFLOAT_PROPERTY, "as",
                            "Angular Separation of the first object in HIP1/2 catalogs");

            AddPropertyMeta(vobsSTAR_XM_2MASS_N_MATES, "XM_2MASS_n_mates", vobsINT_PROPERTY, NULL,
                            "Number of mates within 3 as in 2MASS catalog");
            AddPropertyMeta(vobsSTAR_XM_2MASS_SEP, "XM_2MASS_sep", vobsFLOAT_PROPERTY, "as",
                            "Angular Separation of the first object in 2MASS catalog");
            AddPropertyMeta(vobsSTAR_XM_2MASS_SEP_2ND, "XM_2MASS_sep_2nd", vobsFLOAT_PROPERTY, "as",
                            "Angular Separation between first and second objects in 2MASS catalog");

            AddPropertyMeta(vobsSTAR_XM_WISE_N_MATES, "XM_WISE_n_mates", vobsINT_PROPERTY, NULL,
                            "Number of mates within 3 as in WISE catalog");
            AddPropertyMeta(vobsSTAR_XM_WISE_SEP, "XM_WISE_sep", vobsFLOAT_PROPERTY, "as",
                            "Angular Separation of the first object in WISE catalog");
            AddPropertyMeta(vobsSTAR_XM_WISE_SEP_2ND, "XM_WISE_sep_2nd", vobsFLOAT_PROPERTY, "as",
                            "Angular Separation between first and second objects in WISE catalog");

            AddPropertyMeta(vobsSTAR_XM_GAIA_N_MATES, "XM_GAIA_n_mates", vobsINT_PROPERTY, NULL,
                            "Number of mates within 3 as in GAIA catalog");

            AddPropertyMeta(vobsSTAR_XM_GAIA_SCORE, "XM_GAIA_score", vobsFLOAT_PROPERTY, NULL,
                            "Score mixing angular separation and magnitude difference of the first object in GAIA catalog");
            AddPropertyMeta(vobsSTAR_XM_GAIA_SEP, "XM_GAIA_sep", vobsFLOAT_PROPERTY, "as",
                            "Angular Separation of the first object in GAIA catalog");
            AddPropertyMeta(vobsSTAR_XM_GAIA_DMAG, "XM_GAIA_dmag", vobsFLOAT_PROPERTY, "mag",
                            "Magnitude difference in V band (Vest - Vref) derived from GAIA (G, Bp, Rp) laws");

            AddPropertyMeta(vobsSTAR_XM_GAIA_SEP_2ND, "XM_GAIA_sep_2nd", vobsFLOAT_PROPERTY, "as",
                            "Angular Separation between first and second objects in GAIA catalog");
        }

        /* Group size (ASCC / SIMBAD) */
        AddPropertyMeta(vobsSTAR_GROUP_SIZE, "GroupSize", vobsINT_PROPERTY, NULL,
                        "The number of close targets within 3 as found in the ASCC and SIMBAD catalogs");

        /* RA/DEC coordinates */
        AddPropertyMeta(vobsSTAR_POS_EQ_RA_MAIN, "RAJ2000", vobsSTRING_PROPERTY, "h:m:s",
                        "Right Ascension - J2000",
                        "http://simbad.u-strasbg.fr/simbad/sim-coo?CooDefinedFrames=none&amp;Coord=${RAJ2000}%20${DEJ2000}&amp;CooEpoch=2000&amp;CooFrame=FK5&amp;CooEqui=2000&amp;Radius.unit=arcsec&amp;Radius=1");
        AddPropertyErrorMeta(vobsSTAR_POS_EQ_RA_ERROR, "e_RAJ2000", "mas",
                             "Standard error in Right Ascension * cos(Declination) - J2000");

        AddPropertyMeta(vobsSTAR_POS_EQ_DEC_MAIN, "DEJ2000", vobsSTRING_PROPERTY, "d:m:s",
                        "Declination - J2000",
                        "http://simbad.u-strasbg.fr/simbad/sim-coo?CooDefinedFrames=none&amp;Coord=${RAJ2000}%20${DEJ2000}&amp;CooEpoch=2000&amp;CooFrame=FK5&amp;CooEqui=2000&amp;Radius.unit=arcsec&amp;Radius=1");
        AddPropertyErrorMeta(vobsSTAR_POS_EQ_DEC_ERROR, "e_DEJ2000", "mas",
                             "Standard error in Declination - J2000");

        /* Proper motion */
        AddPropertyMeta(vobsSTAR_POS_EQ_PMRA, "pmRa", vobsFLOAT_PROPERTY, "mas/yr",
                        "Proper Motion in Right Ascension * cos(Declination)");
        AddPropertyErrorMeta(vobsSTAR_POS_EQ_PMRA_ERROR, "e_pmRa", "mas/yr",
                             "Standard error in Proper Motion in Right Ascension * cos(Declination)");

        AddPropertyMeta(vobsSTAR_POS_EQ_PMDEC, "pmDec", vobsFLOAT_PROPERTY, "mas/yr",
                        "Proper Motion in Declination");
        AddPropertyErrorMeta(vobsSTAR_POS_EQ_PMDEC_ERROR, "e_pmDec", "mas/yr",
                             "Standard error in Proper Motion in Declination");

        /* Parallax */
        AddPropertyMeta(vobsSTAR_POS_PARLX_TRIG, "plx", vobsFLOAT_PROPERTY, "mas",
                        "Trigonometric Parallax");
        AddPropertyErrorMeta(vobsSTAR_POS_PARLX_TRIG_ERROR, "e_Plx", NULL,
                             "Standard error in Parallax");

        /* Spectral type */
        AddPropertyMeta(vobsSTAR_SPECT_TYPE_MK, "SpType", vobsSTRING_PROPERTY, NULL,
                        "MK Spectral Type");

        /* Object types (SIMBAD) */
        AddPropertyMeta(vobsSTAR_OBJ_TYPES, "ObjTypes", vobsSTRING_PROPERTY, NULL,
                        "SIMBAD: Object Type list (comma separated)");

        /* ASCC */
        AddPropertyMeta(vobsSTAR_CODE_VARIAB_V1, "VarFlag1", vobsSTRING_PROPERTY, NULL,
                        "ASCC: Variability from GCVS/NSV");
        AddPropertyMeta(vobsSTAR_CODE_VARIAB_V2, "VarFlag2", vobsSTRING_PROPERTY, NULL,
                        "ASCC: Variability in Tycho-1");
        AddPropertyMeta(vobsSTAR_CODE_VARIAB_V3, "VarFlag3", vobsSTRING_PROPERTY, NULL,
                        "ASCC: Variability type among C,D,M,P,R and U");

        /* binary / multiple flags (ASCC ...) */
        AddPropertyMeta(vobsSTAR_CODE_MULT_FLAG, "MultFlag", vobsSTRING_PROPERTY, NULL,
                        "Multiplicity type among C,G,O,V, X or SB (for decoded spectral binaries)");
        AddPropertyMeta(vobsSTAR_CODE_BIN_FLAG, "BinFlag", vobsSTRING_PROPERTY, NULL,
                        "Multiplicity type among SB, eclipsing B or S (for suspicious binaries in spectral type)");
        AddPropertyMeta(vobsSTAR_CODE_MULT_INDEX, "Comp", vobsSTRING_PROPERTY, NULL,
                        "Component (SBC9) or Components (WDS), when the object has more than two. "
                        "Traditionally, these have been designated in order of separation, thus AB, AC,...., "
                        "or in the cases where close pairs are observed blended, AB-C, AB-D,.... "
                        "In some instances, differing resolution limits produce situations where observations are intermixed, thus AC, AB - C, ... "
                        "There are also many instances where later observations have revealed a closer companion; these are designated Aa, Bb, etc. "
                        );

        /* WDS separation 1 and 2 */
        AddPropertyMeta(vobsSTAR_ORBIT_SEPARATION_SEP1, "sep1", vobsFLOAT_PROPERTY, "arcsec",
                        "WDS: Angular Separation of the binary on first observation");
        AddPropertyMeta(vobsSTAR_ORBIT_SEPARATION_SEP2, "sep2", vobsFLOAT_PROPERTY, "arcsec",
                        "WDS: Angular Separation of the binary on last observation");

        /* HIP / GAIA radial velocity */
        AddPropertyMeta(vobsSTAR_VELOC_HC, "RadVel", vobsFLOAT_PROPERTY, "km/s",
                        "Radial Velocity");
        AddPropertyErrorMeta(vobsSTAR_VELOC_HC_ERROR, "e_RadVel", "km/s",
                             "Radial velocity error");

        /* BSC rotational velocity */
        AddPropertyMeta(vobsSTAR_VELOC_ROTAT, "RotVel", vobsFLOAT_PROPERTY, "km/s",
                        "BSC: Rotation Velocity (vsini)");

        if (alxIsNotLowMemFlag())
        {
            /* GAIA Ag */
            AddPropertyMeta(vobsSTAR_AG_GAIA, "gaia_AG", vobsFLOAT_PROPERTY, "mag",
                            "GAIA: Extinction in G band from GSP-Phot Aeneas best library using BP/RP spectra (ag_gspphot)");
        }
        /* GAIA Distance */
        AddPropertyMeta(vobsSTAR_DIST_GAIA, "gaia_dist", vobsFLOAT_PROPERTY, "pc",
                        "GAIA: Distance from GSP-Phot Aeneas best library using BP/RP spectra (distance_gspphot)");
        AddPropertyMeta(vobsSTAR_DIST_GAIA_LOWER, "gaia_dist_lo", vobsFLOAT_PROPERTY, "pc",
                        "GAIA: Lower confidence level (16%) of distance from GSP-Phot Aeneas best library using BP/RP spectra (distancegspphotlower)");
        AddPropertyMeta(vobsSTAR_DIST_GAIA_UPPER, "gaia_dist_hi", vobsFLOAT_PROPERTY, "pc",
                        "GAIA: Upper confidence level (84%) of distance from GSP-Phot Aeneas best library using BP/RP spectra (distancegspphotupper)");

        /* GAIA Teff */
        AddPropertyMeta(vobsSTAR_TEFF_GAIA, "gaia_Teff", vobsFLOAT_PROPERTY, "K",
                        "GAIA: Effective temperature from GSP-Phot Aeneas best library using BP/RP spectra (teff_gspphot)");
        AddPropertyMeta(vobsSTAR_TEFF_GAIA_LOWER, "gaia_Teff_lo", vobsFLOAT_PROPERTY, "K",
                        "GAIA: Lower confidence level (16%) of effective temperature from GSP-Phot Aeneas best library using BP/RP spectra (teffgspphotlower)");
        AddPropertyMeta(vobsSTAR_TEFF_GAIA_UPPER, "gaia_Teff_hi", vobsFLOAT_PROPERTY, "K",
                        "GAIA: Upper confidence level (84%) of effective temperature from GSP-Phot Aeneas best library using BP/RP spectra (teffgspphotupper)");

        /* GAIA Log(g) */
        AddPropertyMeta(vobsSTAR_LOGG_GAIA, "gaia_logG", vobsFLOAT_PROPERTY, "[cm/s2]",
                        "GAIA: Surface gravity from GSP-Phot Aeneas best library using BP/RP spectra (logg_gspphot)");
        AddPropertyMeta(vobsSTAR_LOGG_GAIA_LOWER, "gaia_logG_lo", vobsFLOAT_PROPERTY, "[cm/s2]",
                        "GAIA: Lower confidence level (16%) of surface gravity from GSP-Phot Aeneas best library using BP/RP spectra (logggspphotlower)");
        AddPropertyMeta(vobsSTAR_LOGG_GAIA_UPPER, "gaia_logG_hi", vobsFLOAT_PROPERTY, "[cm/s2]",
                        "GAIA: Upper confidence level (84%) of surface gravity from GSP-Phot Aeneas best library using BP/RP spectra (logggspphotupper)");

        if (alxIsNotLowMemFlag())
        {
            /* GAIA Metallicity [Fe/H] */
            AddPropertyMeta(vobsSTAR_MH_GAIA, "gaia_M_H", vobsFLOAT_PROPERTY, NULL,
                            "GAIA: Iron abundance from GSP-Phot Aeneas best library using BP/RP spectra (mh_gspphot)");
            AddPropertyMeta(vobsSTAR_MH_GAIA_LOWER, "gaia_M_H_lo", vobsFLOAT_PROPERTY, NULL,
                            "GAIA: Lower confidence level (16%) of iron abundance from GSP-Phot Aeneas best library using BP/RP spectra (mhgspphotlower)");
            AddPropertyMeta(vobsSTAR_MH_GAIA_UPPER, "gaia_M_H_hi", vobsFLOAT_PROPERTY, NULL,
                            "GAIA: Upper confidence level (84%) of iron abundance from GSP-Phot Aeneas best library using BP/RP spectra (mhgspphotupper)");

            /* GAIA Radius (radius_gspphot) */
            AddPropertyMeta(vobsSTAR_RAD_PHOT_GAIA, "gaia_rad_phot", vobsFLOAT_PROPERTY, "Rsun",
                            "GAIA: Radius from GSP-Phot Aeneas best library using BP/RP spectra (radius_gspphot)");
            AddPropertyMeta(vobsSTAR_RAD_PHOT_GAIA_LOWER, "gaia_rad_phot_lo", vobsFLOAT_PROPERTY, "Rsun",
                            "GAIA: Lower confidence level (16%) of radius from GSP-Phot Aeneas best library using BP/RP spectra (radiusgspphotlower)");
            AddPropertyMeta(vobsSTAR_RAD_PHOT_GAIA_UPPER, "gaia_rad_phot_hi", vobsFLOAT_PROPERTY, "Rsun",
                            "GAIA: Upper confidence level (84%) of radius from GSP-Phot Aeneas best library using BP/RP spectra (radiusgspphotupper)");
        }

        /* GAIA Radius (radius_flame) */
        AddPropertyMeta(vobsSTAR_RAD_FLAME_GAIA, "gaia_rad_flame", vobsFLOAT_PROPERTY, "Rsun",
                        "GAIA: Radius of the star from FLAME using teff_gspphot and lum_flame (radius_flame)");
        AddPropertyMeta(vobsSTAR_RAD_FLAME_GAIA_LOWER, "gaia_rad_flame_lo", vobsFLOAT_PROPERTY, "Rsun",
                        "GAIA: Lower confidence level (16%) of radiusFlame (radiusflamelower)");
        AddPropertyMeta(vobsSTAR_RAD_FLAME_GAIA_UPPER, "gaia_rad_flame_hi", vobsFLOAT_PROPERTY, "Rsun",
                        "GAIA: Upper confidence level (84%) of radiusFlame (radiusflameupper)");

        /* Photometry */
        /* B */
        AddPropertyMeta(vobsSTAR_PHOT_JHN_B, "B", vobsFLOAT_PROPERTY, "mag",
                        "Johnson's Magnitude in B-band");
        AddPropertyErrorMeta(vobsSTAR_PHOT_JHN_B_ERROR, "e_B", "mag",
                             "Error on Johnson's Magnitude in B-band");

        /* GAIA Bp */
        AddPropertyMeta(vobsSTAR_PHOT_MAG_GAIA_BP, "Bp", vobsFLOAT_PROPERTY, "mag",
                        "GAIA: Integrated Bp mean magnitude (Vega)");
        AddPropertyErrorMeta(vobsSTAR_PHOT_MAG_GAIA_BP_ERROR, "e_Bp", "mag",
                             "GAIA: Standard error of BP mean magnitude (Vega)");

        if ((vobsCATALOG_DENIS_ID_ENABLE || vobsCATALOG_USNO_ID_ENABLE) && alxIsNotLowMemFlag())
        {
            AddPropertyMeta(vobsSTAR_PHOT_PHG_B, "Bphg", vobsFLOAT_PROPERTY, "mag",
                            "Photometric Magnitude in B-band");
        }

        /* V */
        AddPropertyMeta(vobsSTAR_PHOT_JHN_V, "V", vobsFLOAT_PROPERTY, "mag",
                        "Johnson's Magnitude in V-band");
        AddPropertyErrorMeta(vobsSTAR_PHOT_JHN_V_ERROR, "e_V", "mag",
                             "Error on Johnson's Magnitude in V-band");

        /* HIP1 B-V colour */
        AddPropertyMeta(vobsSTAR_PHOT_JHN_B_V, "B-V", vobsFLOAT_PROPERTY, "mag",
                        "HIP: Johnson's B-V Colour");
        AddPropertyErrorMeta(vobsSTAR_PHOT_JHN_B_V_ERROR, "e_B-V", "mag",
                             "HIP: Error on Johnson's B-V Colour");

        if (alxIsNotLowMemFlag())
        {
            /* HIP1 V-Icous colour */
            AddPropertyMeta(vobsSTAR_PHOT_COUS_V_I, "V-Icous", vobsFLOAT_PROPERTY, "mag",
                            "HIP: Cousin's V-I Colour");
            AddPropertyErrorMeta(vobsSTAR_PHOT_COUS_V_I_ERROR, "e_V-Icous", "mag",
                                 "HIP: Error on Cousin's V-I Colour");
            AddPropertyMeta(vobsSTAR_PHOT_COUS_V_I_REFER_CODE, "ref_V-Icous", vobsSTRING_PROPERTY, NULL,
                            "HIP: Source of Cousin's V-I Colour [A-T]");
        }

        /* GAIA G */
        AddPropertyMeta(vobsSTAR_PHOT_MAG_GAIA_G, "G", vobsFLOAT_PROPERTY, "mag",
                        "GAIA: G-band mean magnitude (Vega)");
        AddPropertyErrorMeta(vobsSTAR_PHOT_MAG_GAIA_G_ERROR, "e_G", "mag",
                             "GAIA: Standard error of G-band mean magnitude (Vega)");

        /* R */
        AddPropertyMeta(vobsSTAR_PHOT_JHN_R, "R", vobsFLOAT_PROPERTY, "mag",
                        "Johnson's Magnitude in R-band");
        AddPropertyErrorMeta(vobsSTAR_PHOT_JHN_R_ERROR, "e_R", "mag",
                             "Error on Johnson's Magnitude in R-band");

        if ((vobsCATALOG_DENIS_ID_ENABLE || vobsCATALOG_USNO_ID_ENABLE) && alxIsNotLowMemFlag())
        {
            AddPropertyMeta(vobsSTAR_PHOT_PHG_R, "Rphg", vobsFLOAT_PROPERTY, "mag",
                            "Photometric Magnitude in R-band");
        }

        /* GAIA Rp */
        AddPropertyMeta(vobsSTAR_PHOT_MAG_GAIA_RP, "Rp", vobsFLOAT_PROPERTY, "mag",
                        "GAIA: Integrated Rp mean magnitude (Vega)");
        AddPropertyErrorMeta(vobsSTAR_PHOT_MAG_GAIA_RP_ERROR, "e_Rp", "mag",
                             "GAIA: Standard error of Rp mean magnitude (Vega)");

        /* I */
        AddPropertyMeta(vobsSTAR_PHOT_JHN_I, "I", vobsFLOAT_PROPERTY, "mag",
                        "Johnson's Magnitude in I-band");
        AddPropertyErrorMeta(vobsSTAR_PHOT_JHN_I_ERROR, "e_I", "mag",
                             "Error on Johnson's Magnitude in I-band");

        if (vobsCATALOG_USNO_ID_ENABLE && alxIsNotLowMemFlag())
        {
            AddPropertyMeta(vobsSTAR_PHOT_PHG_I, "Iphg", vobsFLOAT_PROPERTY, "mag",
                            "USNO: Photometric Magnitude in I-band");
        }

        if (alxIsNotLowMemFlag())
        {
            AddPropertyMeta(vobsSTAR_PHOT_COUS_I, "Icous", vobsFLOAT_PROPERTY, "mag",
                            "Cousin's Magnitude in I-band");
            AddPropertyErrorMeta(vobsSTAR_PHOT_COUS_I_ERROR, "e_Icous", "mag",
                                 "Error on Cousin's Magnitude in I-band");
        }
        if (vobsCATALOG_DENIS_ID_ENABLE && alxIsNotLowMemFlag())
        {
            /* Denis IFlag */
            AddPropertyMeta(vobsSTAR_CODE_MISC_I, "Iflag", vobsSTRING_PROPERTY, NULL,
                            "DENIS: Quality flag on Cousin's Magnitude in I-band");
        }

        /* J */
        AddPropertyMeta(vobsSTAR_PHOT_JHN_J, "J", vobsFLOAT_PROPERTY, "mag",
                        "Johnson's Magnitude in J-band");
        AddPropertyErrorMeta(vobsSTAR_PHOT_JHN_J_ERROR, "e_J", "mag",
                             "Error on Johnson's Magnitude in J-band");

        /* H */
        AddPropertyMeta(vobsSTAR_PHOT_JHN_H, "H", vobsFLOAT_PROPERTY, "mag",
                        "Johnson's Magnitude in H-band");
        AddPropertyErrorMeta(vobsSTAR_PHOT_JHN_H_ERROR, "e_H", "mag",
                             "Error on Johnson's Magnitude in H-band");

        /* K */
        AddPropertyMeta(vobsSTAR_PHOT_JHN_K, "K", vobsFLOAT_PROPERTY, "mag",
                        "Johnson's Magnitude in K-band");
        AddPropertyErrorMeta(vobsSTAR_PHOT_JHN_K_ERROR, "e_K", "mag",
                             "Error on Johnson's Magnitude in K-band");

        /* 2MASS quality flag */
        AddPropertyMeta(vobsSTAR_CODE_QUALITY_2MASS, "Qflag", vobsSTRING_PROPERTY, NULL,
                        "2MASS: Quality flag [ABCDEFUX] on JHK Magnitudes");

        /* L */
        AddPropertyMeta(vobsSTAR_PHOT_JHN_L, "L", vobsFLOAT_PROPERTY, "mag",
                        "Johnson's Magnitude in L-band");
        AddPropertyErrorMeta(vobsSTAR_PHOT_JHN_L_ERROR, "e_L", "mag",
                             "Error on Johnson's Magnitude in L-band");

        /* M */
        AddPropertyMeta(vobsSTAR_PHOT_JHN_M, "M", vobsFLOAT_PROPERTY, "mag",
                        "Johnson's Magnitude in M-band");
        AddPropertyErrorMeta(vobsSTAR_PHOT_JHN_M_ERROR, "e_M", "mag",
                             "Error on Johnson's Magnitude in M-band");

        /* N */
        AddPropertyMeta(vobsSTAR_PHOT_JHN_N, "N", vobsFLOAT_PROPERTY, "mag",
                        "Johnson's Magnitude in N-band");
        AddPropertyErrorMeta(vobsSTAR_PHOT_JHN_N_ERROR, "e_N", "mag",
                             "Error on Johnson's Magnitude in N-band");

        /* WISE W4 */
        AddPropertyMeta(vobsSTAR_PHOT_FLUX_IR_25, "W4", vobsFLOAT_PROPERTY, "mag",
                        "Wise W4 magnitude (22.1um)");
        AddPropertyErrorMeta(vobsSTAR_PHOT_FLUX_IR_25_ERROR, "e_W4", "mag",
                             "Error on Wise W4 magnitude (22.1um)");

        /* WISE quality flag */
        AddPropertyMeta(vobsSTAR_CODE_QUALITY_WISE, "Qph_wise", vobsSTRING_PROPERTY, NULL,
                        "WISE: Quality flag [ABCUX] on LMN Magnitudes");

        if (alxIsNotLowMemFlag())
        {
            /* AKARI flux (9 mu) */
            AddPropertyMeta(vobsSTAR_PHOT_FLUX_IR_09, "S09", vobsFLOAT_PROPERTY, "Jy",
                            "AKARI: Mid-Infrared Flux Density at 9 microns");
            AddPropertyErrorMeta(vobsSTAR_PHOT_FLUX_IR_09_ERROR, "e_S09", "Jy",
                                 "AKARI: Relative Error on Mid-Infrared Flux Density at 9 microns");

            /* AKARI flux (12 mu) */
            AddPropertyMeta(vobsSTAR_PHOT_FLUX_IR_12, "F12", vobsFLOAT_PROPERTY, "Jy",
                            "AKARI: Mid-Infrared Flux at 12 microns");
            AddPropertyErrorMeta(vobsSTAR_PHOT_FLUX_IR_12_ERROR, "e_F12", "Jy",
                                 "AKARI: Relative Error on Mid-Infrared Flux at 12 microns");

            /* AKARI flux (18 mu) */
            AddPropertyMeta(vobsSTAR_PHOT_FLUX_IR_18, "S18", vobsFLOAT_PROPERTY, "Jy",
                            "AKARI: Mid-Infrared Flux Density at 18 microns");
            AddPropertyErrorMeta(vobsSTAR_PHOT_FLUX_IR_18_ERROR, "e_S18", "Jy",
                                 "AKARI: Relative Error on Mid-Infrared Flux Density at 18 microns");
        }
        /* MDFC */
        AddPropertyMeta(vobsSTAR_IR_FLAG, "IRFlag", vobsINT_PROPERTY, NULL, "MDFC: IR Flag (bit field): "
                        " bit 0 is set if the star shows an IR excess, identified thanks to the [K-W4] and [J-H] color indexes, and the overall MIR excess statistic X MIR computed from Gaia DR1;"
                        " bit 1 is set if the star is extended in the IR, indicated by the extent flags reported in the WISE/AllWISE and AKARI catalogues;"
                        " bit 2 is set if the star is a likely variable in the MIR, identified by the variability flags reported in the WISE/AllWISE catalogues, the MSX6C Infrared Point Source Catalogue, the IRAS PSC, and the 10-micron Catalog.");

        AddPropertyMeta(vobsSTAR_PHOT_FLUX_L_MED, "Lflux_med", vobsFLOAT_PROPERTY, "Jy",
                        "MDFC: Median flux value in band L");
        AddPropertyErrorMeta(vobsSTAR_PHOT_FLUX_L_MED_ERROR, "e_Lflux_med", "Jy",
                             "MDFC: Error on flux values in band L");
        AddPropertyMeta(vobsSTAR_PHOT_FLUX_M_MED, "Mflux_med", vobsFLOAT_PROPERTY, "Jy",
                        "MDFC: Median flux value in band M");
        AddPropertyErrorMeta(vobsSTAR_PHOT_FLUX_M_MED_ERROR, "e_Mflux_med", "Jy",
                             "MDFC: Error on flux values in band M");
        AddPropertyMeta(vobsSTAR_PHOT_FLUX_N_MED, "Nflux_med", vobsFLOAT_PROPERTY, "Jy",
                        "MDFC: Median flux value in band N");
        AddPropertyErrorMeta(vobsSTAR_PHOT_FLUX_N_MED_ERROR, "e_Nflux_med", "Jy",
                             "MDFC: Error on flux values in band N");

        // End of Meta data
        vobsSTAR::vobsSTAR_PropertyMetaEnd = vobsSTAR_PROPERTY_META::vobsStar_PropertyMetaList.size();

        logInfo("vobsSTAR has defined %d properties.", vobsSTAR::vobsSTAR_PropertyMetaEnd);

        if (vobsSTAR::vobsSTAR_PropertyMetaEnd != vobsSTAR_MAX_PROPERTIES)
        {
            logError("vobsSTAR_MAX_PROPERTIES constant is incorrect: %d < %d",
                     vobsSTAR_MAX_PROPERTIES, vobsSTAR::vobsSTAR_PropertyMetaEnd);
            exit(1);
        }

        logInfo("vobsSTAR_MAX_PROPERTIES: %d - sizeof(vobsSTAR_PROPERTY) = %d bytes - sizeof(vobsSTAR) = %d bytes - total = %d bytes",
                vobsSTAR_MAX_PROPERTIES, sizeof (vobsSTAR_PROPERTY), sizeof (vobsSTAR),
                vobsSTAR_MAX_PROPERTIES * sizeof (vobsSTAR_PROPERTY) + sizeof (vobsSTAR));

        initializeIndex();

        // Get property indexes for RA/DEC:
        vobsSTAR::vobsSTAR_PropertyRAIndex = vobsSTAR::GetPropertyIndex(vobsSTAR_POS_EQ_RA_MAIN);
        vobsSTAR::vobsSTAR_PropertyDECIndex = vobsSTAR::GetPropertyIndex(vobsSTAR_POS_EQ_DEC_MAIN);

        // Get property index for XmLog identifier:
        vobsSTAR::vobsSTAR_PropertyXmLogIndex = vobsSTAR::GetPropertyIndex(vobsSTAR_XM_LOG);
        vobsSTAR::vobsSTAR_PropertyXmMainFlagIndex = vobsSTAR::GetPropertyIndex(vobsSTAR_XM_MAIN_FLAGS);
        vobsSTAR::vobsSTAR_PropertyXmAllFlagIndex = vobsSTAR::GetPropertyIndex(vobsSTAR_XM_ALL_FLAGS);

        // Get property index for Target identifier:
        vobsSTAR::vobsSTAR_PropertyTargetIdIndex = vobsSTAR::GetPropertyIndex(vobsSTAR_ID_TARGET);

        // Get property indexes for PMRA/PMDEC:
        vobsSTAR::vobsSTAR_PropertyPMRAIndex = vobsSTAR::GetPropertyIndex(vobsSTAR_POS_EQ_PMRA);
        vobsSTAR::vobsSTAR_PropertyPMDECIndex = vobsSTAR::GetPropertyIndex(vobsSTAR_POS_EQ_PMDEC);

        // Get property indexes for JD:
        vobsSTAR::vobsSTAR_PropertyJDIndex = vobsSTAR::GetPropertyIndex(vobsSTAR_JD_DATE);

        // Dump properties into XML file:
        DumpPropertyIndexAsXML();

        vobsSTAR::vobsSTAR_PropertyIdxInitialized = true;
    }
    return mcsSUCCESS;
}

/**
 * Dump the property index
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned
 */
mcsCOMPL_STAT vobsSTAR::DumpPropertyIndexAsXML()
{
    miscoDYN_BUF xmlBuf;
    // Prepare buffer:
    FAIL(xmlBuf.Reserve(30 * 1024));

    xmlBuf.AppendLine("<?xml version=\"1.0\"?>\n\n");

    FAIL(xmlBuf.AppendString("<index>\n"));
    FAIL(xmlBuf.AppendString("  <name>vobsSTAR</name>\n"));

    DumpPropertyIndexAsXML(xmlBuf, "vobsSTAR", vobsSTAR::vobsSTAR_PropertyMetaBegin, vobsSTAR::vobsSTAR_PropertyMetaEnd);

    FAIL(xmlBuf.AppendString("</index>\n\n"));

    mcsCOMPL_STAT result = mcsSUCCESS;

    // This file will be stored in the $MCSDATA/tmp repository
    const char* fileName = "$MCSDATA/tmp/PropertyIndex_vobsSTAR.xml";

    // Resolve path
    char* resolvedPath = miscResolvePath(fileName);
    if (IS_NOT_NULL(resolvedPath))
    {
        logInfo("Saving property index XML description: %s", resolvedPath);

        result = xmlBuf.SaveInASCIIFile(resolvedPath);
        free(resolvedPath);
    }
    return result;
}

/**
 * Dump the property index into given buffer
 *
 * @param buffer buffer to append into
 * @param prefix prefix to use in <define>ID</define>
 * @param from first index
 * @param end last index
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned
 */
mcsCOMPL_STAT vobsSTAR::DumpPropertyIndexAsXML(miscoDYN_BUF& buffer, const char* prefix, const mcsINT32 from, const mcsINT32 end)
{
    const vobsSTAR_PROPERTY_META* meta;

    for (mcsINT32 i = from; i < end; i++)
    {
        meta = vobsSTAR_PROPERTY_META::GetPropertyMeta(i);

        if (IS_NOT_NULL(meta))
        {
            // full mode:
            meta->DumpAsXML(buffer, prefix, i, true);
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
    vobsSTAR::vobsSTAR_PropertyErrorIdx.clear();

    for (vobsSTAR_PROPERTY_META_PTR_LIST::iterator iter = vobsSTAR_PROPERTY_META::vobsStar_PropertyMetaList.begin();
            iter != vobsSTAR_PROPERTY_META::vobsStar_PropertyMetaList.end(); iter++)
    {
        delete(*iter);
    }
    vobsSTAR_PROPERTY_META::vobsStar_PropertyMetaList.clear();

    vobsSTAR::vobsSTAR_PropertyMetaBegin = -1;
    vobsSTAR::vobsSTAR_PropertyMetaEnd = -1;
    vobsSTAR::vobsSTAR_PropertyIdxInitialized = false;

    vobsSTAR::vobsSTAR_PropertyRAIndex = -1;
    vobsSTAR::vobsSTAR_PropertyDECIndex = -1;
    vobsSTAR::vobsSTAR_PropertyXmLogIndex = -1;
    vobsSTAR::vobsSTAR_PropertyXmMainFlagIndex = -1;
    vobsSTAR::vobsSTAR_PropertyXmAllFlagIndex = -1;
    vobsSTAR::vobsSTAR_PropertyTargetIdIndex = -1;
}

/**
 * Convert right ascension (RA) coordinate from HMS (HH MM SS.TT) into degrees [-180; 180]
 *
 * @param raHms right ascension (RA) coordinate in HMS (HH MM SS.TT)
 * @param ra pointer on an already allocated mcsDOUBLE value.
 *
 * @return 2 if the decDms parameter was fixed, mcsSUCCESS on successful completion, mcsFAILURE otherwise
 */
mcsINT32 vobsSTAR::GetRa(mcsSTRING32& raHms, mcsDOUBLE &ra)
{
    mcsDOUBLE hh, hm, other, hs = 0.0;

    mcsINT32 n = sscanf(raHms, "%lf %lf %lf %lf", &hh, &hm, &hs, &other);

    FAIL_COND_DO((n < 2) || (n > 3),
                 errAdd(vobsERR_INVALID_RA_FORMAT, raHms);
                 ra = NAN; /* reset RA anyway */
                 );

    // Get sign of hh which has to be propagated to hm and hs
    mcsDOUBLE sign = (raHms[0] == '-') ? -1.0 : 1.0;

    // Convert to degrees
    ra = (hh + sign * (hm + hs * vobsSEC_IN_MIN) * vobsMIN_IN_HOUR) * vobsHA_IN_DEG;

    // Set angle range [-180; 180]
    if (ra > 180.0)
    {
        ra -= 360.0;
    }

    if (n != 3)
    {
        // fix given raDms parameter to conform to HMS format:
        vobsSTAR::ToHms(ra, raHms);

        return 2; /* not 0 or 1 */
    }
    return mcsSUCCESS;
}

/**
 * Convert declination (DEC) coordinate from DMS (DD MM SS.TT) or (DD MM.mm) into degrees [-90; 90]
 *
 * @param decDms declination (DEC) coordinate in DMS (DD MM SS.TT) or (DD MM.mm)
 * @param dec pointer on an already allocated mcsDOUBLE value.
 *
 * @return 2 if the decDms parameter was fixed, mcsSUCCESS on successful completion, mcsFAILURE otherwise
 */
mcsINT32 vobsSTAR::GetDec(mcsSTRING32& decDms, mcsDOUBLE &dec)
{
    mcsDOUBLE dd, other, dm = 0.0, ds = 0.0;

    mcsINT32 n = sscanf(decDms, "%lf %lf %lf %lf", &dd, &dm, &ds, &other);

    FAIL_COND_DO((n < 1) || (n > 3),
                 errAdd(vobsERR_INVALID_DEC_FORMAT, decDms);
                 dec = NAN; /* reset DEC anyway */
                 );

    // Get sign of hh which has to be propagated to dm and ds
    mcsDOUBLE sign = (decDms[0] == '-') ? -1.0 : 1.0;

    // Convert to degrees
    dec = dd + sign * (dm + ds * vobsSEC_IN_MIN) * vobsMIN_IN_HOUR;

    if (n != 3)
    {
        // fix given decDms parameter to conform to DMS format:
        vobsSTAR::ToDms(dec, decDms);

        return 2; /* not 0 or 1 */
    }
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

    sprintf(raHms, "%02.0lf %02.0lf %05.4lf", fabs(hh), fabs(hm), fabs(hs));
}

/**
 * Convert declination (DEC) coordinate from degrees [-90; 90] into DMS (+/-DD MM SS.TT)
 *
 * @param dec declination (DEC) in degrees
 * @param decDms returned declination (DEC) coordinate in DMS (+/-DD MM SS.TT)
 */
void vobsSTAR::ToDms(mcsDOUBLE dec, mcsSTRING32 &decDms)
{
    mcsDOUBLE dd = trunc(dec);
    mcsDOUBLE dm = trunc((dec - dd) * 60.0);
    mcsDOUBLE ds = ((dec - dd) * 60.0 - dm) * 60.0;

    sprintf(decDms, "%c%02.0lf %02.0lf %04.3lf", (dec < 0) ? '-' : '+', fabs(dd), fabs(dm), fabs(ds));
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
 * Convert declination (DEC) coordinate from degrees [-90; 90] into degrees (+/-xx.xxxxxx)
 *
 * @param dec declination (DEC) in degrees
 * @param decDms returned declination (DEC) coordinate in degrees (+/-xx.xxxxxx)
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
    if (IS_NOT_NULL(property))
    {
        property->ClearValue();
    }
    property = GetProperty(vobsSTAR::vobsSTAR_PropertyDECIndex);
    if (IS_NOT_NULL(property))
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

mcsCOMPL_STAT vobsSTAR::PrecessRaDecJ2000ToEpoch(const mcsDOUBLE epoch, mcsDOUBLE &raEpo, mcsDOUBLE &decEpo) const
{
    mcsDOUBLE ra, dec;
    FAIL(GetRaDec(ra, dec));

    mcsDOUBLE pmRa, pmDec; // mas/yr
    FAIL(GetPmRaDec(pmRa, pmDec));

    // ra/dec coordinates are corrected from 2000 to the catalog's epoch:
    raEpo = vobsSTAR::GetPrecessedRA(ra, pmRa, EPOCH_2000, epoch);
    decEpo = vobsSTAR::GetPrecessedDEC(dec, pmDec, EPOCH_2000, epoch);

    return mcsSUCCESS;
}

mcsCOMPL_STAT vobsSTAR::CorrectRaDecEpochs(mcsDOUBLE ra, mcsDOUBLE dec, const mcsDOUBLE pmRa, const mcsDOUBLE pmDec, const mcsDOUBLE epochFrom, const mcsDOUBLE epochTo) const
{
    logDebug("CorrectRaDecToEpoch: (%.9lf %.9lf) (%.9lf %.9lf) (%.3lf)", ra, dec, pmRa, pmDec, epochFrom);

    // ra/dec coordinates are corrected from epochFrom to epochTo:
    ra = vobsSTAR::GetPrecessedRA(ra, pmRa, epochFrom, epochTo);
    dec = vobsSTAR::GetPrecessedDEC(dec, pmDec, epochFrom, epochTo);

    logDebug("CorrectRaDecToEpoch: => (%.9lf %.9lf) (%.3lf)", ra, dec, epochTo);

    SetRaDec(ra, dec);

    return mcsSUCCESS;
}

mcsDOUBLE vobsSTAR::GetPrecessedRA(const mcsDOUBLE raDeg, const mcsDOUBLE pmRa, const mcsDOUBLE epochFrom, const mcsDOUBLE epochTo)
{
    mcsDOUBLE ra = raDeg;

    const mcsDOUBLE deltaEpoch = epochTo - epochFrom;

    if ((deltaEpoch != 0.0) && (pmRa != 0.0))
    {
        ra += GetDeltaRA(pmRa, deltaEpoch);

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
            logTest("ra  (%.3lf to %.3lf): %.6lf => %.6lf", epochFrom, epochTo, raDeg, ra);
        }
    }

    return ra;
}

mcsDOUBLE vobsSTAR::GetPrecessedDEC(const mcsDOUBLE decDeg, const mcsDOUBLE pmDec, const mcsDOUBLE epochFrom, const mcsDOUBLE epochTo)
{
    mcsDOUBLE dec = decDeg;

    const mcsDOUBLE deltaEpoch = epochTo - epochFrom;

    if ((deltaEpoch != 0.0) && (pmDec != 0.0))
    {
        dec += GetDeltaDEC(pmDec, deltaEpoch);

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
            logTest("dec (%.3lf to %.3lf): %.6lf => %.6lf", epochFrom, epochTo, decDeg, dec);
        }
    }

    return dec;
}

mcsDOUBLE vobsSTAR::GetDeltaRA(const mcsDOUBLE pmRa, const mcsDOUBLE deltaEpoch)
{
    // pmRa is already pmRa / cos(dec) (see GetPmRa) because pmRA in catalogs are given in RA*cos(DE) cf ASCC (Proper Motion in RA*cos(DE)):

    // RAJ2000_ep2000 "RAJ2000+(2000-1991.25)*pmRA/cos(DEJ2000*PI/180)/1000/3600"
    return deltaEpoch * 1e-3 * alxARCSEC_IN_DEGREES * pmRa;
}

mcsDOUBLE vobsSTAR::GetDeltaDEC(const mcsDOUBLE pmDec, const mcsDOUBLE deltaEpoch)
{
    // DEJ2000_ep2000 "DEJ2000+(2000-1991.25)*pmDE/1000/3600"
    return deltaEpoch * 1e-3 * alxARCSEC_IN_DEGREES * pmDec;
}

/*___oOo___*/
