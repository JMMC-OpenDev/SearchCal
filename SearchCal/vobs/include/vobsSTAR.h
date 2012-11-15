#ifndef vobsSTAR_H
#define vobsSTAR_H
/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * @file
 * vobsSTAR class declaration.
 */

#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif


/*
 * System headers
 */
#include "string.h"
#include "math.h"
#include <map>
#include <vector>


/*
 * Local headers
 */
#include "alx.h"
#include "vobsCATALOG_META.h"
#include "vobsSTAR_PROPERTY.h"
#include "vobsSTAR_COMP_CRITERIA_LIST.h"

/*
 * Definition of the star properties
 * TODO: use one proper data model to define properties (ID, Name, UCD, query column per catalog ...) + DOCUMENTATION
 */
/* identifiers */
#define vobsSTAR_ID_HD                          "ID_HD"
#define vobsSTAR_ID_HIP                         "ID_HIP"
#define vobsSTAR_ID_DM                          "ID_DM"
#define vobsSTAR_ID_TYC1                        "ID_TYC1"
#define vobsSTAR_ID_TYC2                        "ID_TYC2"
#define vobsSTAR_ID_TYC3                        "ID_TYC3"
#define vobsSTAR_ID_2MASS                       "ID_2MASS"
#define vobsSTAR_ID_DENIS                       "ID_DENIS"
#define vobsSTAR_ID_SB9                         "ID_SB9"
#define vobsSTAR_ID_WDS                         "ID_WDS"
#define vobsSTAR_ID_AKARI                       "ID_AKARI"

/* 2MASS Associated optical source (opt) 'T' for Tycho 2 or 'U' for USNO A 2.0 */
#define vobsSTAR_2MASS_OPT_ID_CATALOG           "ID_CATALOG"

/* RA/DEC coordinates */
#define vobsSTAR_POS_EQ_RA_MAIN                 "POS_EQ_RA_MAIN"
#define vobsSTAR_POS_EQ_DEC_MAIN                "POS_EQ_DEC_MAIN"

/* errors on RA/DEC coordinates */
#define vobsSTAR_POS_EQ_RA_ERROR                "POS_EQ_RA_ERROR"
#define vobsSTAR_POS_EQ_DEC_ERROR               "POS_EQ_DEC_ERROR"

/* given 'RA+DEC' coordinates (deg) to CDS used internally for cross matchs 
   (filtered in VOTable output) */
#define vobsSTAR_ID_TARGET                      "ID_TARGET"

/* RA/DEC OTHER (DENIS): useful ? */
#define vobsSTAR_POS_EQ_RA_OTHER                "POS_EQ_RA_OTHER"
#define vobsSTAR_POS_EQ_DEC_OTHER               "POS_EQ_DEC_OTHER"

/* Proper motion */
#define vobsSTAR_POS_EQ_PMRA                    "POS_EQ_PMRA"
#define vobsSTAR_POS_EQ_PMDEC                   "POS_EQ_PMDEC"

/* errors on RA/DEC coordinates */
#define vobsSTAR_POS_EQ_PMRA_ERROR              "POS_EQ_PMRA_ERROR"
#define vobsSTAR_POS_EQ_PMDEC_ERROR             "POS_EQ_PMDEC_ERROR"

/* 2MASS observation date (JD)
   (filtered in VOTable output) */
#define vobsSTAR_JD_DATE                        "TIME_DATE"

/* Parallax */
#define vobsSTAR_POS_PARLX_TRIG                 "POS_PARLX_TRIG"
#define vobsSTAR_POS_PARLX_TRIG_ERROR           "POS_PARLX_TRIG_ERROR"

#define vobsSTAR_SPECT_TYPE_MK                  "SPECT_TYPE_MK"

/* ASCC */
#define vobsSTAR_CODE_VARIAB_V1                 "CODE_VARIAB_V1"
#define vobsSTAR_CODE_VARIAB_V2                 "CODE_VARIAB_V2"
#define vobsSTAR_CODE_VARIAB_V3                 "VAR_CLASS"

/* binary / multiple flags (midi / ASCC ...) */
#define vobsSTAR_CODE_BIN_FLAG                  "CODE_BIN_FLAG"
#define vobsSTAR_CODE_MULT_FLAG                 "CODE_MULT_FLAG"

/* WDS separation 1 and 2 */
#define vobsSTAR_ORBIT_SEPARATION_SEP1          "ORBIT_SEPARATION_SEP1"
#define vobsSTAR_ORBIT_SEPARATION_SEP2          "ORBIT_SEPARATION_SEP2"

/* Denis IFlag */
#define vobsSTAR_CODE_MISC_I                    "CODE_MISC_I"

/* 2MASS quality flag */
#define vobsSTAR_CODE_QUALITY                   "CODE_QUALITY"

/* Hipparcos radial velocity */
#define vobsSTAR_VELOC_HC                       "VELOC_HC" 

/* BSC rotational velocity */
#define vobsSTAR_VELOC_ROTAT                    "VELOC_ROTAT"

/* Borde et Merand UD */
#define vobsSTAR_UDDK_DIAM                      "UDDK_DIAM"
#define vobsSTAR_UDDK_DIAM_ERROR                "UDDK_DIAM_ERROR"

/* CIO UCD (wavelength / IR flux) = NOT properties */
#define vobsSTAR_INST_WAVELENGTH_VALUE          "INST_WAVELENGTH_VALUE"
#define vobsSTAR_PHOT_FLUX_IR_MISC              "PHOT_FLUX_IR_MISC"

/* Johnson / photometric fluxes */
#define vobsSTAR_PHOT_JHN_B                     "PHOT_JHN_B"
#define vobsSTAR_PHOT_PHG_B                     "PHOT_PHG_B"

#define vobsSTAR_PHOT_JHN_V                     "PHOT_JHN_V"

#define vobsSTAR_PHOT_JHN_R                     "PHOT_JHN_R"
#define vobsSTAR_PHOT_PHG_R                     "PHOT_PHG_R"

#define vobsSTAR_PHOT_JHN_I                     "PHOT_JHN_I"
#define vobsSTAR_PHOT_PHG_I                     "PHOT_PHG_I"
/* Cousin flux I (denis), the others are computed (so far) */
#define vobsSTAR_PHOT_COUS_I                    "PHOT_COUS_I"

#define vobsSTAR_PHOT_JHN_J                     "PHOT_JHN_J"
#define vobsSTAR_PHOT_COUS_J                    "PHOT_COUS_J"

#define vobsSTAR_PHOT_JHN_H                     "PHOT_JHN_H"
#define vobsSTAR_PHOT_COUS_H                    "PHOT_COUS_H"

#define vobsSTAR_PHOT_JHN_K                     "PHOT_JHN_K"
#define vobsSTAR_PHOT_COUS_K                    "PHOT_COUS_K"

#define vobsSTAR_PHOT_JHN_L                     "PHOT_JHN_L"
#define vobsSTAR_PHOT_JHN_M                     "PHOT_JHN_M"
#define vobsSTAR_PHOT_JHN_N                     "PHOT_JHN_N"

/* MIDI local catalog */
#define vobsSTAR_IR_FLUX_ORIGIN                 "IR_FLUX_ORIGIN"
#define vobsSTAR_PHOT_FLUX_IR_12                "PHOT_FLUX_IR_12"
#define vobsSTAR_PHOT_FLUX_IR_12_ERROR          "PHOT_FLUX_IR_12_ERROR"
#define vobsSTAR_REF_STAR                       "REF_STAR"
#define vobsSTAR_PHYS_TEMP_EFFEC                "PHYS_TEMP_EFFEC"
#define vobsSTAR_PHYS_TEMP_EFFEC_ERROR          "PHYS_TEMP_EFFEC_ERROR"
#define vobsSTAR_DIAM12                         "DIAM12"
#define vobsSTAR_DIAM12_ERROR                   "DIAM12_ERROR"
#define vobsSTAR_PHOT_EXTINCTION_TOTAL          "PHOT_EXTINCTION_TOTAL"
#define vobsSTAR_CHI2_QUALITY                   "CHI2_QUALITY"
#define vobsSTAR_SP_TYP_PHYS_TEMP_EFFEC         "SP_TYP_PHYS_TEMP_EFFEC"

/* AKARI fluxes (9, 12, 18 mu) */
#define vobsSTAR_PHOT_FLUX_IR_09                "PHOT_FLUX_IR_9"
#define vobsSTAR_PHOT_FLUX_IR_09_ERROR          "PHOT_FLUX_IR_9_ERROR"
#define vobsSTAR_PHOT_FLUX_IR_18                "PHOT_FLUX_IR_18"
#define vobsSTAR_PHOT_FLUX_IR_18_ERROR          "PHOT_FLUX_IR_18_ERROR"


/* Blanking value used for parsed RA/DEC coordinates */
#define EMPTY_COORD_DEG 1000.

/* 
 * 1 micro degree for coordinate precision = 3.6 milli arcsec
 * related to RA/DEC coordinates expressed in degrees for CDS Vizier
 * see vobsSTAR::raToDeg() and vobsSTAR::decToDeg()
 */
#define COORDS_PRECISION 0.000001

/** utility macro to fill no match criteria information */
#define NO_MATCH(noMatchs, el)  \
    if (noMatchs != NULL)       \
    {                           \
        noMatchs[el]++;         \
    }                           \
    return mcsFALSE;

/*
 * Enumeration type definition
 */

/**
 * vobsOVERWRITE is an enumeration which define overwrite modes.
 */
typedef enum
{
    vobsOVERWRITE_NONE,
    vobsOVERWRITE_ALL,
    vobsOVERWRITE_PARTIAL,
    vobsOVERWRITE_BETTER
} vobsOVERWRITE;

/*
 * const char* comparator used by map<const char*, ...>
 */
struct constStringComparator
{

    bool operator()(const char* s1, const char* s2) const
    {
        return (s1 == s2) ? false : strcmp(s1, s2) < 0;
    }
};

/** Star property ID / index mapping keyed by property ID using const char* keys and custom comparator functor */
typedef std::map<const char*, unsigned int, constStringComparator> vobsSTAR_PROPERTY_INDEX_MAPPING;

/** Star property ID / index pair */
typedef std::pair<const char*, unsigned int> vobsSTAR_PROPERTY_INDEX_PAIR;

/** Star property meta pointer vector */
typedef std::vector<vobsSTAR_PROPERTY_META*> vobsSTAR_PROPERTY_META_PTR_LIST;

/** Star property pointer vector */
typedef std::vector<vobsSTAR_PROPERTY*> vobsSTAR_PROPERTY_PTR_LIST;

/*
 * Class declaration
 */

/**
 * Store all the propreties caracterising a star.
 *
 * It allows among oher things the following actions on star properies:
 * @li read;
 * @li update;
 * @li compare.
 *
 * @sa vobsSTAR_PROPERTY
 */
class vobsSTAR
{
public:
    // Constructors
    vobsSTAR();
    explicit vobsSTAR(const vobsSTAR& star);

    // assignement operator =
    vobsSTAR& operator=(const vobsSTAR&);

    // Destructor
    virtual ~vobsSTAR();

    // Clear cached values (ra, dec)
    void ClearCache(void);

    // Clear means free
    void Clear(void);

    // Compare stars (i.e values)
    int compare(const vobsSTAR& other) const;

    // Return the star RA and DEC coordinates (in degrees)
    mcsCOMPL_STAT GetRa(mcsDOUBLE &ra) const;
    mcsCOMPL_STAT GetDec(mcsDOUBLE &dec) const;

    // Return the star RA and DEC coordinates (in degrees)
    mcsCOMPL_STAT GetRaRefStar(mcsDOUBLE &raRef) const;
    mcsCOMPL_STAT GetDecRefStar(mcsDOUBLE &decRef) const;

    // Return the star PMRA and PMDEC (in max/yr)
    mcsCOMPL_STAT GetPmRa(mcsDOUBLE &pmRa) const;
    mcsCOMPL_STAT GetPmDec(mcsDOUBLE &pmDec) const;

    // Return the observation date (jd)
    mcsDOUBLE GetJdDate() const;

    // Return the star ID
    mcsCOMPL_STAT GetId(char* starId, const mcsUINT32 maxLength) const;

    // Update the star properties with the given star ones
    mcsLOGICAL Update(const vobsSTAR &star,
                      vobsOVERWRITE overwrite = vobsOVERWRITE_NONE,
                      const vobsSTAR_PROPERTY_MASK* overwritePropertyMask = NULL,
                      mcsUINT32* propertyUpdated = NULL);

    // Print out all star properties
    void Display(mcsLOGICAL showPropId = mcsFALSE) const;
    void Dump(char* output, const char* separator = " ") const;

    // Set the star property values
    mcsCOMPL_STAT SetPropertyValue
    (const char* propertyId,
     const char* value,
     const char* origin,
     vobsCONFIDENCE_INDEX confidenceIndex = vobsCONFIDENCE_HIGH,
     mcsLOGICAL overwrite = mcsFALSE);

    /**
     * Clear property values
     */
    inline void ClearValues(void) __attribute__((always_inline))
    {
        ClearCache();

        for (vobsSTAR_PROPERTY_PTR_LIST::iterator iter = _propertyList.begin(); iter != _propertyList.end(); iter++)
        {
            // Clear this property value
            (*iter)->ClearValue();
        }
    }

    /**
     * Set the character value of the given property.
     *
     * @param property property to use.
     * @param value property value
     * @param origin the origin of the value (catalog, computed, ...)
     * @param confidenceIndex value confidence index
     * @param overwrite booleen to know if it is an overwrite property
     *
     * @return mcsSUCCESS on successful completion, mcsFAILURE otherwise.
     */
    inline mcsCOMPL_STAT SetPropertyValue(vobsSTAR_PROPERTY* property,
                                          const char* value,
                                          const char* origin,
                                          vobsCONFIDENCE_INDEX confidenceIndex = vobsCONFIDENCE_HIGH,
                                          mcsLOGICAL overwrite = mcsFALSE
                                          ) __attribute__((always_inline))
    {
        // Set this property value
        return property->SetValue(value, origin, confidenceIndex, overwrite);
    }

    mcsCOMPL_STAT SetPropertyValue
    (const char* propertyId,
     mcsDOUBLE value,
     const char* origin,
     vobsCONFIDENCE_INDEX confidenceIndex = vobsCONFIDENCE_HIGH,
     mcsLOGICAL overwrite = mcsFALSE);

    /**
     * Set the floating value of the given property.
     *
     * @param property property to use.
     * @param value property value
     * @param origin the origin of the value (catalog, computed, ...)
     * @param confidenceIndex value confidence index
     * @param overwrite booleen to know if it is an overwrite property
     *
     * @return mcsSUCCESS on successful completion, mcsFAILURE otherwise.
     */
    inline mcsCOMPL_STAT SetPropertyValue(vobsSTAR_PROPERTY* property,
                                          mcsDOUBLE value,
                                          const char* origin,
                                          vobsCONFIDENCE_INDEX confidenceIndex = vobsCONFIDENCE_HIGH,
                                          mcsLOGICAL overwrite = mcsFALSE
                                          ) __attribute__((always_inline))
    {
        // Set this property value
        return property->SetValue(value, origin, confidenceIndex, overwrite);
    }

    mcsCOMPL_STAT ClearPropertyValue(const char* id);

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
    inline vobsSTAR_PROPERTY* GetNextProperty(mcsLOGICAL init = mcsFALSE) __attribute__((always_inline))
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

        return *_propertyListIterator;
    }

    /**
     * Get the star property at the given index.
     *
     * @param idx property index.
     * @return pointer on the found star property object on successful completion.
     * Otherwise NULL is returned.
     */
    inline vobsSTAR_PROPERTY* GetProperty(const int idx) const __attribute__((always_inline))
    {
        if ((idx < 0) || (idx >= (int) _propertyList.size()))
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
    inline vobsSTAR_PROPERTY* GetProperty(const char* id) const __attribute__((always_inline))
    {
        // Look for property
        return GetProperty(GetPropertyIndex(id));
    }

    /**
     * Get a property character value.
     *
     * @param id property id.
     *
     * @return pointer to the found star property value on successful completion.
     * Otherwise NULL is returned.
     */
    inline const char* GetPropertyValue(const char* id) const __attribute__((always_inline))
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
    inline const char* GetPropertyValue(const vobsSTAR_PROPERTY* property) const __attribute__((always_inline))
    {
        if (property == NULL)
        {
            // Return error
            return NULL;
        }

        // Return the property value
        return property->GetValue();
    }

    /**
     * Get a star property mcsDOUBLE value.
     *
     * @param id property id.
     * @param value pointer to store value.
     *
     * @return mcsSUCCESS on successfull completion, mcsFAILURE otherwise.
     */
    inline mcsCOMPL_STAT GetPropertyValue(const char* id, mcsDOUBLE* value) const __attribute__((always_inline))
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
    inline mcsCOMPL_STAT GetPropertyValue(const vobsSTAR_PROPERTY* property, mcsDOUBLE* value) const __attribute__((always_inline))
    {
        FAIL_NULL(property);

        // Return the property value
        return property->GetValue(value);
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
    inline vobsPROPERTY_TYPE GetPropertyType(const char* id) const __attribute__((always_inline))
    {
        // Look for property
        vobsSTAR_PROPERTY* property = GetProperty(id);

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
    inline vobsPROPERTY_TYPE GetPropertyType(const vobsSTAR_PROPERTY* property) const __attribute__((always_inline))
    {
        if (property == NULL)
        {
            return vobsSTRING_PROPERTY;
        }

        // Return property
        return property->GetType();
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
    inline vobsCONFIDENCE_INDEX GetPropertyConfIndex(const char* id) const __attribute__((always_inline))
    {
        // Look for property
        vobsSTAR_PROPERTY* property = GetProperty(id);

        // Return property confidence index
        return property->GetConfidenceIndex();
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
    inline mcsLOGICAL IsPropertySet(const char* id) const __attribute__((always_inline))
    {
        // Look for the property
        vobsSTAR_PROPERTY* property = GetProperty(id);

        return IsPropertySet(property);
    }

    /**
     * Check whether the property is set or not.
     *
     * @param idx property index.
     *
     * @warning If the given property id is unknown, this method returns mcsFALSE.
     *
     * @return mcsTRUE if the the property has been set, mcsFALSE otherwise.
     */
    inline mcsLOGICAL IsPropertySet(const int idx) const __attribute__((always_inline))
    {
        // Look for the property
        vobsSTAR_PROPERTY* property = GetProperty(idx);

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
    inline mcsLOGICAL IsPropertySet(const vobsSTAR_PROPERTY* property) const __attribute__((always_inline))
    {
        if (property == NULL)
        {
            return mcsFALSE;
        }

        return property->IsSet();
    }

    /**
     * Return whether a name correspond to a property.
     *
     * @param id property id.
     *
     * @return mcsTRUE) if the the property is known, mcsFALSE otherwise.
     */
    inline mcsLOGICAL IsProperty(const char* id) const __attribute__((always_inline))
    {
        // Look for property: see GetProperty(id)
        int idx = GetPropertyIndex(id);

        if ((idx < 0) || (idx >= (int) _propertyList.size()))
        {
            return mcsFALSE;
        }

        return mcsTRUE;
    }

    /**
     * Return the number of properties
     *
     * @return the number of properties of the star
     */
    inline mcsINT32 NbProperties(void) const __attribute__((always_inline))
    {
        return _propertyList.size();
    }

    /**
     * Return whether the star is the same as another given one
     * i.e. coordinates (RA/DEC) in degrees are the same (equals)
     *
     * @param star the other star.
     *
     * @return mcsTRUE if the stars are the same, mcsFALSE otherwise.
     */
    inline mcsLOGICAL IsSame(vobsSTAR* star) const __attribute__((always_inline))
    {
        // try to use first cached ra/dec coordinates for performance:

        // Get right ascension of stars. If not set return FALSE
        mcsDOUBLE ra1 = _ra;

        if ((ra1 == EMPTY_COORD_DEG) && (GetRa(ra1) == mcsFAILURE))
        {
            return mcsFALSE;
        }

        mcsDOUBLE ra2 = star->_ra;

        if ((ra2 == EMPTY_COORD_DEG) && (star->GetRa(ra2) == mcsFAILURE))
        {
            return mcsFALSE;
        }

        // Compare RA coordinates:
        if (ra1 != ra2)
        {
            return mcsFALSE;
        }

        // Get declinaison of stars. If not set return FALSE
        mcsDOUBLE dec1 = _dec;

        if ((dec1 == EMPTY_COORD_DEG) && (GetDec(dec1) == mcsFAILURE))
        {
            return mcsFALSE;
        }

        mcsDOUBLE dec2 = star->_dec;

        if ((dec2 == EMPTY_COORD_DEG) && (star->GetDec(dec2) == mcsFAILURE))
        {
            return mcsFALSE;
        }

        // Compare DEC coordinates:
        if (dec1 != dec2)
        {
            return mcsFALSE;
        }

        return mcsTRUE;
    }

    /**
     * Return whether this star is the reference star of the given star
     * i.e. coordinates (RA/DEC) in degrees are below COORDS_PRECISION
     *
     * @param star the other star having ref RA/Dec coordinates .
     *
     * @return mcsTRUE if this star is the reference star of the given star, mcsFALSE otherwise.
     */
    inline mcsLOGICAL IsSameRefStar(vobsSTAR* star) const __attribute__((always_inline))
    {
        // try to use first cached ra/dec coordinates for performance:

        // Get right ascension of stars. If not set return FALSE
        mcsDOUBLE ra1 = _ra;

        if ((ra1 == EMPTY_COORD_DEG) && (GetRa(ra1) == mcsFAILURE))
        {
            return mcsFALSE;
        }

        mcsDOUBLE ra2 = star->_raRef;

        if ((ra2 == EMPTY_COORD_DEG) && (star->GetRaRefStar(ra2) == mcsFAILURE))
        {
            return mcsFALSE;
        }

        // Compare RA coordinates using precision threshold:
        if (fabs(ra1 - ra2) > COORDS_PRECISION)
        {
            return mcsFALSE;
        }

        // Get declinaison of stars. If not set return FALSE
        mcsDOUBLE dec1 = _dec;

        if ((dec1 == EMPTY_COORD_DEG) && (GetDec(dec1) == mcsFAILURE))
        {
            return mcsFALSE;
        }

        mcsDOUBLE dec2 = star->_decRef;

        if ((dec2 == EMPTY_COORD_DEG) && (star->GetDecRefStar(dec2) == mcsFAILURE))
        {
            return mcsFALSE;
        }

        // Compare DEC coordinates using precision threshold:
        if (fabs(dec1 - dec2) > COORDS_PRECISION)
        {
            return mcsFALSE;
        }
        return mcsTRUE;
    }

    /**
     * Return whether this star and the given star are matching criteria as
     * shown below:
     * @code
     * int nCriteria = 0;
     * vobsSTAR_CRITERIA_INFO* criterias = NULL;
     * 
     * // Initialize criteria informations:
     * if (criteriaList.InitializeCriterias() == mcsFAILURE)
     * {
     *     return mcsFAILURE;
     * }
     * 
     * // Get criterias:
     * if (criteriaList.GetCriterias(criterias, nCriteria) == mcsFAILURE)
     * {
     *     return mcsFAILURE;
     * }
     * 
     * ...
     * if (star->IsMatchingCriteria(anotherStar, criterias, nCriteria) == mcsTRUE)
     * {
     *     printf ("Star is matching !!");
     * }
     * @endcode
     * 
     *
     * @param star the other star.
     * @param criterias vobsSTAR_CRITERIA_INFO[] list of comparison criterias 
     *                  given by vobsSTAR_COMP_CRITERIA_LIST.GetCriterias()
     * @param nCriteria number of criteria i.e. size of the vobsSTAR_CRITERIA_INFO array
     * @param separation (optional) returned distance in degrees if stars are matching criteria
     *
     * @return mcsTRUE if stars are matching criteria, mcsFALSE otherwise.
     */
    inline mcsLOGICAL IsMatchingCriteria(vobsSTAR* star,
                                         vobsSTAR_CRITERIA_INFO* criterias,
                                         mcsUINT32 nCriteria,
                                         mcsDOUBLE* separation = NULL,
                                         mcsUINT32* noMatchs = NULL) const __attribute__((always_inline))
    {
        // assumption: the criteria list is not NULL

        // Get criteria informations
        vobsSTAR_CRITERIA_INFO* criteria = NULL;
        mcsDOUBLE dec1, dec2, ra1, ra2;
        mcsDOUBLE delta;
        int propIndex;
        vobsSTAR_PROPERTY* prop1 = NULL;
        vobsSTAR_PROPERTY* prop2 = NULL;
        mcsDOUBLE val1, val2;
        const char *val1Str = NULL, *val2Str = NULL;
        // computed distance:
        mcsDOUBLE dist = FP_NAN;

        // Get each criteria of the list and check if the comparaison with all
        // this criteria gave a equality

        for (mcsUINT32 el = 0; el < nCriteria; el++)
        {
            criteria = &criterias[el];

            switch (criteria->propCompType)
            {
                case vobsPROPERTY_COMP_RA_DEC:
                    // try to use first cached ra/dec coordinates for performance:

                    // Get right ascension of the star. If not set return FALSE
                    ra1 = _ra;

                    if ((ra1 == EMPTY_COORD_DEG) && (GetRa(ra1) == mcsFAILURE))
                    {
                        NO_MATCH(noMatchs, el);
                    }

                    ra2 = star->_ra;

                    if ((ra2 == EMPTY_COORD_DEG) && (star->GetRa(ra2) == mcsFAILURE))
                    {
                        NO_MATCH(noMatchs, el);
                    }

                    // Get declinaison of the star. If not set return FALSE
                    dec1 = _dec;

                    if ((dec1 == EMPTY_COORD_DEG) && (GetDec(dec1) == mcsFAILURE))
                    {
                        NO_MATCH(noMatchs, el);
                    }

                    dec2 = star->_dec;

                    if ((dec2 == EMPTY_COORD_DEG) && (star->GetDec(dec2) == mcsFAILURE))
                    {
                        NO_MATCH(noMatchs, el);
                    }

                    if (criteria->isRadius)
                    {
                        // Do not test delta Ra / delta Dec as it is not safe:
                        // always use Haversine Formula (from R.W. Sinnott, "Virtues of the Haversine",
                        // Sky and Telescope, vol. 68, no. 2, 1984, p. 159):

                        // http://www.cs.nyu.edu/visual/home/proj/tiger/gisfaq.html

                        // compute angular separation using haversine formula:
                        if (alxComputeDistanceInDegrees(ra1, dec1, ra2, dec2, &dist) == mcsFAILURE)
                        {
                            NO_MATCH(noMatchs, el);
                        }

                        if (dist > criteria->rangeRA)
                        {
                            NO_MATCH(noMatchs, el);
                        }

                        // return computed distance
                        if (separation != NULL)
                        {
                            *separation = dist;
                        }
                    }
                    else
                    {
                        // Box area (MIDI):
                        delta = fabs(dec1 - dec2);
                        if (delta > criteria->rangeDEC)
                        {
                            NO_MATCH(noMatchs, el);
                        }

                        // boundary problem [-180; 180]
                        if ((ra1 >= criteria->lowerBoundRA) && (ra1 <= criteria->upperBoundRA) &&
                            (ra2 >= criteria->lowerBoundRA) && (ra2 <= criteria->upperBoundRA))
                        {
                            delta = fabs(ra1 - ra2);
                            if (delta > criteria->rangeRA)
                            {
                                NO_MATCH(noMatchs, el);
                            }
                        }
                    }
                    break;

                default:
                case vobsPROPERTY_COMP_FLOAT:
                    propIndex = criteria->propertyIndex;

                    prop1 = GetProperty(propIndex);
                    prop2 = star->GetProperty(propIndex);

                    if ((IsPropertySet(prop1) == mcsFALSE) || (GetPropertyValue(prop1, &val1) == mcsFAILURE))
                    {
                        NO_MATCH(noMatchs, el);
                    }

                    if ((star->IsPropertySet(prop2) == mcsFALSE) || (star->GetPropertyValue(prop2, &val2) == mcsFAILURE))
                    {
                        NO_MATCH(noMatchs, el);
                    }

                    delta = fabs(val1 - val2);

                    if (delta > criteria->range)
                    {
                        NO_MATCH(noMatchs, el);
                    }
                    break;

                case vobsPROPERTY_COMP_STRING:
                    propIndex = criteria->propertyIndex;

                    prop1 = GetProperty(propIndex);
                    prop2 = star->GetProperty(propIndex);

                    if (IsPropertySet(prop1) == mcsFALSE)
                    {
                        NO_MATCH(noMatchs, el);
                    }
                    val1Str = GetPropertyValue(prop1);

                    if (star->IsPropertySet(prop2) == mcsFALSE)
                    {
                        NO_MATCH(noMatchs, el);
                    }
                    val2Str = star->GetPropertyValue(prop2);

                    if (strcmp(val1Str, val2Str) != 0)
                    {
                        NO_MATCH(noMatchs, el);
                    }
                    break;
            }

        } // loop on criteria

        return mcsTRUE;
    }

    /**
     * Get the star property corresponding to the target identifier (useful for internal cross matchs).
     *
     * @return pointer on the found star property object on successful completion.
     * Otherwise NULL is returned.
     */
    inline vobsSTAR_PROPERTY* GetTargetIdProperty() const __attribute__((always_inline))
    {
        return GetProperty(vobsSTAR::vobsSTAR_PropertyTargetIdIndex);
    }

    /**
     * Get the star property corresponding to the observation date (useful for internal cross matchs).
     *
     * @return pointer on the found star property object on successful completion.
     * Otherwise NULL is returned.
     */
    inline vobsSTAR_PROPERTY* GetJdDateProperty() const __attribute__((always_inline))
    {
        return GetProperty(vobsSTAR::vobsSTAR_PropertyJDIndex);
    }

    /**
     * Find the property index (position) for the given property identifier
     * @param id property identifier
     * @return index or -1 if not found in the property index
     */
    inline static int GetPropertyIndex(const char* id) __attribute__((always_inline))
    {
        // Look for property
        vobsSTAR_PROPERTY_INDEX_MAPPING::iterator idxIter = vobsSTAR::vobsSTAR_PropertyIdx.find(id);

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
    inline static vobsSTAR_PROPERTY_META* GetPropertyMeta(const int idx) __attribute__((always_inline))
    {
        if ((idx < 0) || (idx >= (int) vobsSTAR::vobsStar_PropertyMetaList.size()))
        {
            return NULL;
        }

        return vobsSTAR::vobsStar_PropertyMetaList[idx];
    }

    /**
     * Allocate dynamically a new mask (must be freed)
     * @return 
     */
    inline static vobsSTAR_PROPERTY_MASK* GetPropertyMask(const mcsUINT32 nIds, const char* overwriteIds[]) __attribute__((always_inline))
    {
        vobsSTAR_PROPERTY_MASK* mask = new vobsSTAR_PROPERTY_MASK(vobsSTAR::vobsStar_PropertyMetaList.size(), false);

        const char* id;
        int idx;
        for (mcsUINT32 el = 0; el < nIds; el++)
        {
            id = overwriteIds[el];

            idx = GetPropertyIndex(id);

            if (idx == -1)
            {
                logPrint("vobs", logWARNING, NULL, __FILE_LINE__, "GetPropertyMask: property not found '%s'", id);
            }
            else
            {
                (*mask)[idx] = true;
            }
        }
        return mask;
    }

    /**
     * Return true if the given overwrite property mask means Ra/Dec property overwrites
     * @param overwritePropertyMask mask to test
     * @return true if Ra/Dec property overwrites
     */
    inline static bool IsRaDecOverwrites(const vobsSTAR_PROPERTY_MASK* overwritePropertyMask)
    {
        return (*overwritePropertyMask)[vobsSTAR::vobsSTAR_PropertyRAIndex] && (*overwritePropertyMask)[vobsSTAR::vobsSTAR_PropertyDECIndex];
    }

    /* Convert right ascension (RA) coordinate from HMS (HH MM SS.TT) into degrees [-180; 180] */
    static mcsCOMPL_STAT GetRa(const char* raHms, mcsDOUBLE &ra);

    /* Convert declinaison (DEC) coordinate from DMS (+/-DD MM SS.TT) into degrees [-90; 90] */
    static mcsCOMPL_STAT GetDec(const char* decDms, mcsDOUBLE &dec);

    /* Convert right ascension (RA) coordinate from degrees [-180; 180] into HMS (HH MM SS.TTT) */
    static void ToHms(mcsDOUBLE ra, mcsSTRING32 &raHms);

    /* Convert declinaison (DEC) coordinate from degrees [-90; 90] into DMS (+/-DD MM SS.TT)*/
    static void ToDms(mcsDOUBLE dec, mcsSTRING32 &decDms);

    /* Convert right ascension (RA) coordinate from degrees [-180; 180] into degrees (xxx.xxxxxx) */
    static void raToDeg(mcsDOUBLE ra, mcsSTRING16 &raDeg);

    /* Convert declinaison (DEC) coordinate from degrees [-90; 90] into degrees (+/-xx.xxxxxx) */
    static void decToDeg(mcsDOUBLE dec, mcsSTRING16 &decDeg);

    /* Convert concatenated RA/DEC 'xxx.xxxxxx(+/-)xx.xxxxxx' coordinates into degrees */
    static mcsCOMPL_STAT degToRaDec(const char* raDec, mcsDOUBLE &ra, mcsDOUBLE &dec);

    static void FreePropertyIndex(void);

    void ClearRaDec(void);
    void SetRaDec(const mcsDOUBLE ra, const mcsDOUBLE dec) const;

    mcsCOMPL_STAT PrecessRaDecToEpoch(const mcsDOUBLE epoch, mcsDOUBLE &raEpo, mcsDOUBLE &decEpo) const;
    mcsCOMPL_STAT CorrectRaDecToEpoch(const mcsDOUBLE pmRa, const mcsDOUBLE pmDec, mcsDOUBLE epoch) const;

    static mcsDOUBLE GetPrecessedRA(const mcsDOUBLE raDeg, const mcsDOUBLE pmRa, const mcsDOUBLE epochRa, const mcsDOUBLE epoch);
    static mcsDOUBLE GetPrecessedDEC(const mcsDOUBLE decDeg, const mcsDOUBLE pmDec, const mcsDOUBLE epochDec, const mcsDOUBLE epoch);

    static mcsDOUBLE GetDeltaRA(const mcsDOUBLE pmRa, const mcsDOUBLE deltaEpoch);
    static mcsDOUBLE GetDeltaDEC(const mcsDOUBLE pmDec, const mcsDOUBLE deltaEpoch);

protected:

    static vobsSTAR_PROPERTY_INDEX_MAPPING vobsSTAR_PropertyIdx;
    static vobsSTAR_PROPERTY_META_PTR_LIST vobsStar_PropertyMetaList;

    // Add a property. Should be only called by constructors.
    void AddProperty(const vobsSTAR_PROPERTY_META* meta);

    // Add a property meta data.
    static void AddPropertyMeta(const char* id,
                                const char* name,
                                const vobsPROPERTY_TYPE type,
                                const char* unit = NULL,
                                const char* format = NULL,
                                const char* link = NULL,
                                const char* description = NULL);

    static void initializeIndex(void);

    /**
     * Reserve enough space in the property list
     * @param size capacity to reserve
     */
    inline void ReserveProperties(unsigned int size) __attribute__((always_inline))
    {
        _propertyList.reserve(size);
    }

private:

    static int vobsSTAR_PropertyMetaBegin;
    static int vobsSTAR_PropertyMetaEnd;
    static bool vobsSTAR_PropertyIdxInitialized;

    // RA/DEC property indexes (read-only):
    static int vobsSTAR_PropertyRAIndex;
    static int vobsSTAR_PropertyDECIndex;
    // Target Id property index (read-only):
    static int vobsSTAR_PropertyTargetIdIndex;
    // PMRA/PMDEC property indexes (read-only):
    static int vobsSTAR_PropertyPMRAIndex;
    static int vobsSTAR_PropertyPMDECIndex;
    // JD property index (read-only):
    static int vobsSTAR_PropertyJDIndex;

    // ra/dec are mutable to be modified even by const methods
    mutable mcsDOUBLE _ra; // parsed RA
    mutable mcsDOUBLE _dec; // parsed DEC

    mutable mcsDOUBLE _raRef; // parsed RA of reference star
    mutable mcsDOUBLE _decRef; // parsed DEC of reference star

    vobsSTAR_PROPERTY_PTR_LIST _propertyList;
    vobsSTAR_PROPERTY_PTR_LIST::iterator _propertyListIterator;

    // Method to define all star properties
    mcsCOMPL_STAT AddProperties(void);
};


#endif /*!vobsSTAR_H*/
/*___oOo___*/
