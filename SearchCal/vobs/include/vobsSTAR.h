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
#include "misco.h"
#include "vobsSTAR_PROPERTY.h"
#include "vobsSTAR_COMP_CRITERIA_LIST.h"

/*
 * Definition of the star properties
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
#define vobsSTAR_ID_WISE                        "ID_WISE"
#define vobsSTAR_ID_GAIA                        "ID_GAIA"
/* SIMBAD Identifier (queried) */
#define vobsSTAR_ID_SIMBAD                      "ID_SIMBAD"

/* 2MASS Associated optical source (opt) 'T' for Tycho 2 or 'U' for USNO A 2.0 */
#define vobsSTAR_2MASS_OPT_ID_CATALOG           "ID_CATALOG"

/* Crossmatch information */
/* given 'RA+DEC' coordinates (deg) to CDS used internally for cross matchs */
#define vobsSTAR_ID_TARGET                      "ID_TARGET"
/* observation date (JD) (2MASS, DENIS ...) */
#define vobsSTAR_JD_DATE                        "TIME_DATE"

/* prefix for specific xmatch columns */
#define vobsSTAR_XM_PREFIX                      "XMATCH_"

/* full xmatch log */
#define vobsSTAR_XM_LOG                         "XMATCH_LOG"
/* bit mask on match types (OR) */
#define vobsSTAR_XM_MAIN_FLAGS                  "XMATCH_MAIN_FLAG"
/* bit mask on match types (OR) */
#define vobsSTAR_XM_ALL_FLAGS                   "XMATCH_ALL_FLAG"

/* xmatch informations for main catalogs */
#define vobsSTAR_XM_SIMBAD_SEP                  "XMATCH_SIMBAD_SEP"

#define vobsSTAR_XM_ASCC_N_MATES                "XMATCH_ASCC_N_MATES"
#define vobsSTAR_XM_ASCC_SEP                    "XMATCH_ASCC_SEP"
#define vobsSTAR_XM_ASCC_SEP_2ND                "XMATCH_ASCC_SEP_2ND"

#define vobsSTAR_XM_HIP_N_MATES                 "XMATCH_HIP_N_MATES"
#define vobsSTAR_XM_HIP_SEP                     "XMATCH_HIP_SEP"
#define vobsSTAR_XM_HIP_SEP_2ND                 "XMATCH_HIP_SEP_2ND"

#define vobsSTAR_XM_2MASS_N_MATES               "XMATCH_2MASS_N_MATES"
#define vobsSTAR_XM_2MASS_SEP                   "XMATCH_2MASS_SEP"
#define vobsSTAR_XM_2MASS_SEP_2ND               "XMATCH_2MASS_SEP_2ND"

#define vobsSTAR_XM_WISE_N_MATES                "XMATCH_WISE_N_MATES"
#define vobsSTAR_XM_WISE_SEP                    "XMATCH_WISE_SEP"
#define vobsSTAR_XM_WISE_SEP_2ND                "XMATCH_WISE_SEP_2ND"

#define vobsSTAR_XM_GAIA_N_MATES                "XMATCH_GAIA_N_MATES"
#define vobsSTAR_XM_GAIA_SCORE                  "XMATCH_GAIA_SCORE"
#define vobsSTAR_XM_GAIA_SEP                    "XMATCH_GAIA_SEP"
#define vobsSTAR_XM_GAIA_DMAG                   "XMATCH_GAIA_DMAG"
#define vobsSTAR_XM_GAIA_SEP_2ND                "XMATCH_GAIA_SEP_2ND"

/* Group Size (ASCC / SIMBAD) for JSDC */
#define vobsSTAR_GROUP_SIZE                     "GROUP_SIZE"

/* RA/DEC coordinates */
#define vobsSTAR_POS_EQ_RA_MAIN                 "POS_EQ_RA_MAIN"
#define vobsSTAR_POS_EQ_RA_ERROR                "POS_EQ_RA_ERROR"
#define vobsSTAR_POS_EQ_DEC_MAIN                "POS_EQ_DEC_MAIN"
#define vobsSTAR_POS_EQ_DEC_ERROR               "POS_EQ_DEC_ERROR"

/* Proper motion */
#define vobsSTAR_POS_EQ_PMRA                    "POS_EQ_PMRA"
#define vobsSTAR_POS_EQ_PMRA_ERROR              "POS_EQ_PMRA_ERROR"
#define vobsSTAR_POS_EQ_PMDEC                   "POS_EQ_PMDEC"
#define vobsSTAR_POS_EQ_PMDEC_ERROR             "POS_EQ_PMDEC_ERROR"

/* Parallax */
#define vobsSTAR_POS_PARLX_TRIG                 "POS_PARLX_TRIG"
#define vobsSTAR_POS_PARLX_TRIG_ERROR           "POS_PARLX_TRIG_ERROR"

/* Spectral type (SIMBAD or ASCC) */
#define vobsSTAR_SPECT_TYPE_MK                  "SPECT_TYPE_MK"
/* Object types (SIMBAD) */
#define vobsSTAR_OBJ_TYPES                      "OBJ_TYPES"

/* ASCC */
#define vobsSTAR_CODE_VARIAB_V1                 "CODE_VARIAB_V1"
#define vobsSTAR_CODE_VARIAB_V2                 "CODE_VARIAB_V2"
#define vobsSTAR_CODE_VARIAB_V3                 "VAR_CLASS"

/* binary flag (SPType) */
#define vobsSTAR_CODE_BIN_FLAG                  "CODE_BIN_FLAG"
/* multiplicty flag (ASCC or SPType) */
#define vobsSTAR_CODE_MULT_FLAG                 "CODE_MULT_FLAG"

/* define column Comp(onent) for SB9/WDS Comp: CODE_MULT_INDEX */
#define vobsSTAR_CODE_MULT_INDEX                 "CODE_MULT_INDEX"

/* WDS separation 1 and 2 */
#define vobsSTAR_ORBIT_SEPARATION_SEP1          "ORBIT_SEPARATION_SEP1"
#define vobsSTAR_ORBIT_SEPARATION_SEP2          "ORBIT_SEPARATION_SEP2"

/* HIP / GAIA radial velocity */
#define vobsSTAR_VELOC_HC                       "VELOC_HC"
#define vobsSTAR_VELOC_HC_ERROR                 "VELOC_HC_ERROR"

/* BSC rotational velocity */
#define vobsSTAR_VELOC_ROTAT                    "VELOC_ROTAT"

/* GAIA Teff */
#define vobsSTAR_TEFF_GAIA                      "TEFF_GAIA"
#define vobsSTAR_TEFF_GAIA_LOWER                "TEFF_GAIA_LOWER"
#define vobsSTAR_TEFF_GAIA_UPPER                "TEFF_GAIA_UPPER"

/* GAIA Distance */
#define vobsSTAR_DIST_GAIA                      "DIST_GAIA"
#define vobsSTAR_DIST_GAIA_LOWER                "DIST_GAIA_LOWER"
#define vobsSTAR_DIST_GAIA_UPPER                "DIST_GAIA_UPPER"

/* Photometry */
/* B */
#define vobsSTAR_PHOT_JHN_B                     "PHOT_JHN_B"
#define vobsSTAR_PHOT_JHN_B_ERROR               "PHOT_JHN_B_ERROR"
#define vobsSTAR_PHOT_PHG_B                     "PHOT_PHG_B"

/* GAIA Bp */
#define vobsSTAR_PHOT_MAG_GAIA_BP               "PHOT_MAG_Bp"
#define vobsSTAR_PHOT_MAG_GAIA_BP_ERROR         "PHOT_MAG_Bp_ERROR"

/* Johnson B-V (HIP1) */
#define vobsSTAR_PHOT_JHN_B_V                   "PHOT_JHN_B-V"
#define vobsSTAR_PHOT_JHN_B_V_ERROR             "PHOT_JHN_B-V_ERROR"

/* V */
#define vobsSTAR_PHOT_JHN_V                     "PHOT_JHN_V"
#define vobsSTAR_PHOT_JHN_V_ERROR               "PHOT_JHN_V_ERROR"

/* Johnson V-I (HIP1) */
#define vobsSTAR_PHOT_COUS_V_I                  "PHOT_COUS_V-I"
#define vobsSTAR_PHOT_COUS_V_I_ERROR            "PHOT_COUS_V-I_ERROR"
#define vobsSTAR_PHOT_COUS_V_I_REFER_CODE       "PHOT_COUS_V-I_REFER_CODE"

/* GAIA G */
#define vobsSTAR_PHOT_MAG_GAIA_G                "PHOT_MAG_G"
#define vobsSTAR_PHOT_MAG_GAIA_G_ERROR          "PHOT_MAG_G_ERROR"

/* R */
#define vobsSTAR_PHOT_JHN_R                     "PHOT_JHN_R"
#define vobsSTAR_PHOT_JHN_R_ERROR               "PHOT_JHN_R_ERROR"
#define vobsSTAR_PHOT_PHG_R                     "PHOT_PHG_R"

/* GAIA Rp */
#define vobsSTAR_PHOT_MAG_GAIA_RP               "PHOT_MAG_Rp"
#define vobsSTAR_PHOT_MAG_GAIA_RP_ERROR         "PHOT_MAG_Rp_ERROR"

/* I */
#define vobsSTAR_PHOT_JHN_I                     "PHOT_JHN_I"
#define vobsSTAR_PHOT_JHN_I_ERROR               "PHOT_JHN_I_ERROR"
#define vobsSTAR_PHOT_PHG_I                     "PHOT_PHG_I"
/* Cousin flux I (denis) or computed from HIP */
#define vobsSTAR_PHOT_COUS_I                    "PHOT_COUS_I"
#define vobsSTAR_PHOT_COUS_I_ERROR              "PHOT_COUS_I_ERROR"
/* Denis IFlag */
#define vobsSTAR_CODE_MISC_I                    "CODE_MISC_I"

/* J */
#define vobsSTAR_PHOT_JHN_J                     "PHOT_JHN_J"
#define vobsSTAR_PHOT_JHN_J_ERROR               "PHOT_JHN_J_ERROR"
/* H */
#define vobsSTAR_PHOT_JHN_H                     "PHOT_JHN_H"
#define vobsSTAR_PHOT_JHN_H_ERROR               "PHOT_JHN_H_ERROR"
/* K */
#define vobsSTAR_PHOT_JHN_K                     "PHOT_JHN_K"
#define vobsSTAR_PHOT_JHN_K_ERROR               "PHOT_JHN_K_ERROR"

/* 2MASS quality flag */
#define vobsSTAR_CODE_QUALITY_2MASS             "CODE_QUALITY"

/* L */
#define vobsSTAR_PHOT_JHN_L                     "PHOT_JHN_L"
#define vobsSTAR_PHOT_JHN_L_ERROR               "PHOT_JHN_L_ERROR"
/* M */
#define vobsSTAR_PHOT_JHN_M                     "PHOT_JHN_M"
#define vobsSTAR_PHOT_JHN_M_ERROR               "PHOT_JHN_M_ERROR"
/* N */
#define vobsSTAR_PHOT_JHN_N                     "PHOT_JHN_N"
#define vobsSTAR_PHOT_JHN_N_ERROR               "PHOT_JHN_N_ERROR"

/* WISE quality flag */
#define vobsSTAR_CODE_QUALITY_WISE              "CODE_QUALITY_WISE"

/* AKARI fluxes (9, 12, 18 mu) */
#define vobsSTAR_PHOT_FLUX_IR_09                "PHOT_FLUX_IR_9"
#define vobsSTAR_PHOT_FLUX_IR_09_ERROR          "PHOT_FLUX_IR_9_ERROR"
#define vobsSTAR_PHOT_FLUX_IR_12                "PHOT_FLUX_IR_12"
#define vobsSTAR_PHOT_FLUX_IR_12_ERROR          "PHOT_FLUX_IR_12_ERROR"
#define vobsSTAR_PHOT_FLUX_IR_18                "PHOT_FLUX_IR_18"
#define vobsSTAR_PHOT_FLUX_IR_18_ERROR          "PHOT_FLUX_IR_18_ERROR"

/* CIO UCDs (wavelength / IR flux) = NOT properties */
#define vobsSTAR_INST_WAVELENGTH_VALUE          "INST_WAVELENGTH_VALUE"
#define vobsSTAR_PHOT_FLUX_IR_MISC              "PHOT_FLUX_IR_MISC"

/* MDFC */
#define vobsSTAR_IR_FLAG                        "IR_FLAG"
#define vobsSTAR_PHOT_FLUX_L_MED                "PHOT_FLUX_L"
#define vobsSTAR_PHOT_FLUX_L_MED_ERROR          "PHOT_FLUX_L_ERROR"
#define vobsSTAR_PHOT_FLUX_M_MED                "PHOT_FLUX_M"
#define vobsSTAR_PHOT_FLUX_M_MED_ERROR          "PHOT_FLUX_M_ERROR"
#define vobsSTAR_PHOT_FLUX_N_MED                "PHOT_FLUX_N"
#define vobsSTAR_PHOT_FLUX_N_MED_ERROR          "PHOT_FLUX_N_ERROR"

/* convenience macros */
#define isPropRA(propertyID) \
    (strcmp(propertyID, vobsSTAR_POS_EQ_RA_MAIN) == 0)

#define isPropDEC(propertyID) \
    (strcmp(propertyID, vobsSTAR_POS_EQ_DEC_MAIN) == 0)

#define isPropSet(propPtr) \
    IS_TRUE(vobsSTAR::IsPropertySet(propPtr))

#define isNotPropSet(propPtr) \
    IS_FALSE(vobsSTAR::IsPropertySet(propPtr))

#define isErrorSet(propPtr) \
    IS_TRUE(vobsSTAR::IsPropertyErrorSet(propPtr))

static void vobsGetXmatchColumnsFromOriginIndex(vobsORIGIN_INDEX originIndex,
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

static bool vobsIsMainCatalogFromOriginIndex(vobsORIGIN_INDEX originIndex)
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

/* Blanking value used for parsed RA/DEC coordinates */
#define EMPTY_COORD_DEG  1000.0

/* Default magnitude error (0.1 mag) when undefined in catalog */
#define DEF_MAG_ERROR    0.1

/*
 * 1 micro degree for coordinate precision = 3.6 milli arcsec = 0.0036 as
 * related to RA/DEC coordinates expressed in degrees (6 digits) for CDS Vizier
 * see vobsSTAR::raToDeg() and vobsSTAR::decToDeg()
 */
#define COORDS_PRECISION 0.000001

/** utility macro to fill no match criteria information */
#define NO_MATCH(noMatchs, el)  \
    if (IS_NOT_NULL(noMatchs))    \
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

/**
 * vobsOVERWRITE is an enumeration which define overwrite modes.
 */
typedef enum
{
    vobsSTAR_MATCH_ALL,
    vobsSTAR_MATCH_BEST
} vobsSTAR_MATCH_MODE;

/*
 * const char* comparator used by map<const char*, ...>
 */
struct constStringComparator
{

    bool operator()(const char* s1, const char* s2) const
    {
        return (s1 == s2) ? false : strcmp(s1, s2) < 0;
    }
} ;

/*
 * Type declaration
 */

/** Property mask (boolean vector) */
typedef std::vector<bool> vobsSTAR_PROPERTY_MASK;

/** Star property ID / index pair */
typedef std::pair<const char*, mcsUINT32> vobsSTAR_PROPERTY_INDEX_PAIR;

/** Star property ID / index mapping keyed by property ID using const char* keys and custom comparator functor */
typedef std::map<const char*, mcsUINT32, constStringComparator> vobsSTAR_PROPERTY_INDEX_MAPPING;

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
    vobsSTAR(mcsUINT8 nProperties);
    vobsSTAR();
    explicit vobsSTAR(const vobsSTAR& star);

    // assignment operator =
    vobsSTAR& operator=(const vobsSTAR&) ;

    // Destructor
    virtual ~vobsSTAR();

    // Clear means free
    void Clear(void);

    // Clear cached values (ra, dec)
    void ClearCache(void);

    // Compare stars (i.e values)
    mcsINT32 compare(const vobsSTAR& other) const;
    bool equals(const vobsSTAR& other) const;

    // Return the star RA and DEC coordinates (in degrees)
    mcsCOMPL_STAT GetRa(mcsDOUBLE &ra) const;
    mcsCOMPL_STAT GetDec(mcsDOUBLE &dec) const;

    mcsCOMPL_STAT GetRaDec(mcsDOUBLE &ra, mcsDOUBLE &dec) const;

    // Return the star RA and DEC coordinates of the reference star (in degrees)
    mcsCOMPL_STAT GetRaDecRefStar(mcsDOUBLE &raRef, mcsDOUBLE &decRef) const;

    // Return the star PMRA and PMDEC (in max/yr)
    mcsCOMPL_STAT GetPmRa(mcsDOUBLE &pmRa) const;
    mcsCOMPL_STAT GetPmDec(mcsDOUBLE &pmDec) const;
    mcsCOMPL_STAT GetPmRaDec(mcsDOUBLE &pmRa, mcsDOUBLE &pmDec) const;

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

    /**
     * Find the property index (position) for the given property identifier
     * @param id property identifier
     * @return index or -1 if not found in the property index
     */
    inline static mcsINT32 GetPropertyIndex(const char* id) __attribute__ ((always_inline))
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
     * Find the property index (position) for the given property error identifier
     * @param id property error identifier
     * @return index or -1 if not found in the property error index
     */
    inline static mcsINT32 GetPropertyErrorIndex(const char* id) __attribute__ ((always_inline))
    {
        // Look for property
        vobsSTAR_PROPERTY_INDEX_MAPPING::iterator idxIter = vobsSTAR::vobsSTAR_PropertyErrorIdx.find(id);

        // If no property with the given Id was found
        if (idxIter == vobsSTAR::vobsSTAR_PropertyErrorIdx.end())
        {
            return -1;
        }

        return idxIter->second;
    }

    inline mcsLOGICAL isRaDecSet(void) const __attribute__ ((always_inline))
    {
        if (IS_TRUE(IsPropertySet(vobsSTAR::vobsSTAR_PropertyRAIndex))
                && IS_TRUE(IsPropertySet(vobsSTAR::vobsSTAR_PropertyDECIndex)))
        {
            return mcsTRUE;
        }
        return mcsFALSE;
    }

    /**
     * Clear property values
     */
    inline void ClearValues(void) __attribute__ ((always_inline))
    {
        ClearCache();

        for (mcsUINT32 p = 0; p < _nProps; p++)
        {
            _properties[p].ClearValue();
        }
    }

    // Set the star property values

    /**
     * Set the value as string of a given property.
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
    inline mcsCOMPL_STAT SetPropertyValue(const char* propertyId,
                                          const char* value,
                                          vobsORIGIN_INDEX originIndex,
                                          vobsCONFIDENCE_INDEX confidenceIndex = vobsCONFIDENCE_HIGH,
                                          mcsLOGICAL overwrite = mcsFALSE) __attribute__ ((always_inline))
    {
        // Look for the given property
        vobsSTAR_PROPERTY* property = GetProperty(propertyId);

        FAIL_NULL(property);

        return SetPropertyValue(property, value, originIndex, confidenceIndex, overwrite);
    }

    /**
     * Set the value as string of the given property.
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
                                          vobsORIGIN_INDEX originIndex,
                                          vobsCONFIDENCE_INDEX confidenceIndex = vobsCONFIDENCE_HIGH,
                                          mcsLOGICAL overwrite = mcsFALSE) __attribute__ ((always_inline))
    {
        // Set this property value
        return property->SetValue(value, originIndex, confidenceIndex, overwrite);
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
    inline mcsCOMPL_STAT SetPropertyValue(const char* id,
                                          mcsDOUBLE value,
                                          vobsORIGIN_INDEX originIndex,
                                          vobsCONFIDENCE_INDEX confidenceIndex = vobsCONFIDENCE_HIGH,
                                          mcsLOGICAL overwrite = mcsFALSE) __attribute__ ((always_inline))
    {
        // Look for the given property
        vobsSTAR_PROPERTY* property = GetProperty(id);

        FAIL_NULL(property);

        return SetPropertyValue(property, value, originIndex, confidenceIndex, overwrite);
    }

    inline mcsCOMPL_STAT SetPropertyValue(const char* propertyId,
                                          mcsINT32 value,
                                          vobsORIGIN_INDEX originIndex,
                                          vobsCONFIDENCE_INDEX confidenceIndex = vobsCONFIDENCE_HIGH,
                                          mcsLOGICAL overwrite = mcsFALSE) __attribute__ ((always_inline))
    {
        return SetPropertyValue(propertyId, (mcsDOUBLE) value, originIndex, confidenceIndex, overwrite);
    }

    inline mcsCOMPL_STAT SetPropertyValue(const char* propertyId,
                                          mcsLOGICAL value,
                                          vobsORIGIN_INDEX originIndex,
                                          vobsCONFIDENCE_INDEX confidenceIndex = vobsCONFIDENCE_HIGH,
                                          mcsLOGICAL overwrite = mcsFALSE) __attribute__ ((always_inline))
    {
        return SetPropertyValue(propertyId, (mcsDOUBLE) value, originIndex, confidenceIndex, overwrite);
    }

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
                                          vobsORIGIN_INDEX originIndex,
                                          vobsCONFIDENCE_INDEX confidenceIndex = vobsCONFIDENCE_HIGH,
                                          mcsLOGICAL overwrite = mcsFALSE) __attribute__ ((always_inline))
    {
        // Set this property value
        return property->SetValue(value, originIndex, confidenceIndex, overwrite);
    }

    /**
     * Set the floating error of the given property.
     *
     * @param id property id
     * @param error property error to set (given as string)
     * @param overwrite boolean to know if it is an overwrite property
     *
     * @return mcsSUCCESS on successful completion, mcsFAILURE otherwise.
     */
    inline mcsCOMPL_STAT SetPropertyError(const char* id,
                                          mcsDOUBLE error,
                                          mcsLOGICAL overwrite = mcsFALSE) __attribute__ ((always_inline))
    {
        // Look for the given property
        vobsSTAR_PROPERTY* property = GetProperty(id);

        FAIL_NULL(property);

        // Set this property error
        property->SetError(error, overwrite);

        return mcsSUCCESS;
    }

    /**
     * Set the error as string of the given property.
     *
     * @param property property to use.
     * @param error property error to set (given as string)
     * @param overwrite boolean to know if it is an overwrite property
     *
     * @return mcsSUCCESS on successful completion, mcsFAILURE otherwise.
     */
    inline mcsCOMPL_STAT SetPropertyError(vobsSTAR_PROPERTY* property,
                                          const char* error,
                                          mcsLOGICAL overwrite = mcsFALSE) __attribute__ ((always_inline))
    {
        // Set this property error
        return property->SetError(error, overwrite);
    }

    /**
     * Set the floating error of the given property.
     *
     * @param property property to use.
     * @param error property error to set (given as string)
     * @param overwrite boolean to know if it is an overwrite property
     *
     * @return mcsSUCCESS on successful completion, mcsFAILURE otherwise.
     */
    inline mcsCOMPL_STAT SetPropertyError(vobsSTAR_PROPERTY* property,
                                          mcsDOUBLE error,
                                          mcsLOGICAL overwrite = mcsFALSE) __attribute__ ((always_inline))
    {
        // Set this property error
        property->SetError(error, overwrite);

        return mcsSUCCESS;
    }

    /**
     * Set the floating value and error of the given property.
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
    inline mcsCOMPL_STAT SetPropertyValueAndError(const char* id,
                                                  mcsDOUBLE value,
                                                  mcsDOUBLE error,
                                                  vobsORIGIN_INDEX originIndex,
                                                  vobsCONFIDENCE_INDEX confidenceIndex = vobsCONFIDENCE_HIGH,
                                                  mcsLOGICAL overwrite = mcsFALSE) __attribute__ ((always_inline))
    {
        // Look for the given property
        vobsSTAR_PROPERTY* property = GetProperty(id);

        FAIL_NULL(property);

        return SetPropertyValueAndError(property, value, error, originIndex, confidenceIndex, overwrite);
    }

    /**
     * Set the floating value and error of the given property.
     *
     * @param property property to use.
     * @param value property value
     * @param error property error to set (given as string)
     * @param origin the origin of the value (catalog, computed, ...)
     * @param confidenceIndex value confidence index
     * @param overwrite booleen to know if it is an overwrite property
     *
     * @return mcsSUCCESS on successful completion, mcsFAILURE otherwise.
     */
    inline mcsCOMPL_STAT SetPropertyValueAndError(vobsSTAR_PROPERTY* property,
                                                  mcsDOUBLE value,
                                                  mcsDOUBLE error,
                                                  vobsORIGIN_INDEX originIndex,
                                                  vobsCONFIDENCE_INDEX confidenceIndex = vobsCONFIDENCE_HIGH,
                                                  mcsLOGICAL overwrite = mcsFALSE) __attribute__ ((always_inline))
    {
        // Set this property value
        FAIL(property->SetValue(value, originIndex, confidenceIndex, overwrite));
        // Set this property error
        property->SetError(error, overwrite);

        return mcsSUCCESS;
    }

    /**
     * Clear the value of a given property.
     *
     * @param id property id
     *
     * @return mcsSUCCESS on successful completion, mcsFAILURE otherwise.
     *
     * @b Error codes:@n
     * The possible errors are :
     * @li vobsERR_INVALID_PROPERTY_ID
     */
    inline mcsCOMPL_STAT ClearPropertyValue(const char* id) __attribute__ ((always_inline))
    {
        // Look for the given property
        vobsSTAR_PROPERTY* property = GetProperty(id);

        FAIL_NULL(property);

        ClearPropertyValue(property);

        return mcsSUCCESS;
    }

    /**
     * Clear the value of a given property.
     *
     * @param property property to use.
     *
     * @return mcsSUCCESS on successful completion, mcsFAILURE otherwise.
     *
     * @b Error codes:@n
     * The possible errors are :
     * @li vobsERR_INVALID_PROPERTY_ID
     */
    inline void ClearPropertyValue(vobsSTAR_PROPERTY* property) __attribute__ ((always_inline))
    {
        // Clear this property value
        property->ClearValue();
    }

    /**
     * Get the star property at the given index.
     *
     * @param idx property index.
     * @return pointer on the found star property object on successful completion.
     * Otherwise NULL is returned.
     */
    inline vobsSTAR_PROPERTY* GetProperty(const mcsINT32 idx) const __attribute__ ((always_inline))
    {
        if ((idx < 0) || (idx >= (mcsINT32) _nProps))
        {
            return NULL;
        }

        return &_properties[idx];
    }

    /**
     * Get the star property corresponding to the given property ID (UCD).
     *
     * @param id property id (UCD).
     *
     * @return pointer on the found star property object on successful completion.
     * Otherwise NULL is returned.
     */
    inline vobsSTAR_PROPERTY* GetProperty(const char* id) const __attribute__ ((always_inline))
    {
        // Look for property
        return GetProperty(vobsSTAR::GetPropertyIndex(id));
    }

    /**
     * Get the star property corresponding to the given property error ID (UCD).
     *
     * @param id property error id (UCD).
     *
     * @return pointer on the found star property object on successful completion.
     * Otherwise NULL is returned.
     */
    inline vobsSTAR_PROPERTY* GetPropertyError(const char* id) const __attribute__ ((always_inline))
    {
        // Look for property
        return GetProperty(vobsSTAR::GetPropertyErrorIndex(id));
    }

    /**
     * Get a property string value.
     *
     * @param id property id.
     *
     * @return pointer to the found star property value on successful completion.
     * Otherwise NULL is returned.
     */
    inline const char* GetPropertyValue(const char* id) const __attribute__ ((always_inline))
    {
        // Look for property
        vobsSTAR_PROPERTY* property = GetProperty(id);

        return GetPropertyValue(property);
    }

    /**
     * Get a property string value.
     *
     * @param property property to use.
     *
     * @return pointer to the found star property value on successful completion.
     * Otherwise NULL is returned.
     */
    inline const char* GetPropertyValue(const vobsSTAR_PROPERTY* property) const __attribute__ ((always_inline))
    {
        if (IS_NULL(property))
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
     * @return mcsSUCCESS on successful completion, mcsFAILURE otherwise.
     */
    inline mcsCOMPL_STAT GetPropertyValue(const char* id, mcsDOUBLE* value) const __attribute__ ((always_inline))
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
     * @return mcsSUCCESS on successful completion, mcsFAILURE otherwise.
     */
    inline mcsCOMPL_STAT GetPropertyValue(const vobsSTAR_PROPERTY* property, mcsDOUBLE* value) const __attribute__ ((always_inline))
    {
        FAIL_NULL(property);

        // Return the property value
        return property->GetValue(value);
    }

    /**
     * Get a star property mcsINT32 value.
     *
     * @param property property to use.
     * @param value pointer to store value.
     *
     * @return mcsSUCCESS on successful completion, mcsFAILURE otherwise.
     */
    inline mcsCOMPL_STAT GetPropertyValue(const vobsSTAR_PROPERTY* property, mcsINT32* value) const __attribute__ ((always_inline))
    {
        FAIL_NULL(property);

        // Return the property value
        return property->GetValue(value);
    }

    /**
     * Get a star property mcsDOUBLE value and error.
     *
     * @param id property id.
     * @param value pointer to store value.
     * @param error pointer to store value.
     *
     * @return mcsSUCCESS on successful completion, mcsFAILURE otherwise.
     */
    inline mcsCOMPL_STAT GetPropertyValueAndError(const char* id, mcsDOUBLE* value, mcsDOUBLE* error) const __attribute__ ((always_inline))
    {
        // Look for property
        vobsSTAR_PROPERTY* property = GetProperty(id);

        return GetPropertyValueAndError(property, value, error);
    }

    /**
     * Get a star property mcsDOUBLE value and error.
     *
     * @param property property to use.
     * @param value pointer to store value.
     * @param error pointer to store value.
     *
     * @return mcsSUCCESS on successful completion, mcsFAILURE otherwise.
     */
    inline mcsCOMPL_STAT GetPropertyValueAndError(const vobsSTAR_PROPERTY* property, mcsDOUBLE* value, mcsDOUBLE* error) const __attribute__ ((always_inline))
    {
        FAIL_NULL(property);

        // Get the property value
        FAIL(property->GetValue(value));

        // Return the property error
        return property->GetError(error);
    }

    /**
     * Get a star property mcsDOUBLE error.
     *
     * @param property property to use.
     * @param error pointer to store value.
     *
     * @return mcsSUCCESS on successful completion, mcsFAILURE otherwise.
     */
    inline mcsCOMPL_STAT GetPropertyError(const vobsSTAR_PROPERTY* property, mcsDOUBLE* error) const __attribute__ ((always_inline))
    {
        FAIL_NULL(property);

        // Return the property error
        return property->GetError(error);
    }

    /**
     * Get a star property mcsDOUBLE error if set or the default value
     *
     * @param property property to use.
     * @param value pointer to store value.
     * @param def default value.
     *
     * @return mcsSUCCESS on successful completion, mcsFAILURE otherwise.
     */
    inline mcsCOMPL_STAT GetPropertyErrorOrDefault(const vobsSTAR_PROPERTY* property, mcsDOUBLE* value, mcsDOUBLE def) const __attribute__ ((always_inline))
    {
        if (IS_NOT_NULL(property) && IS_TRUE(property->IsErrorSet()))
        {
            return property->GetError(value);
        }

        *value = def;

        return mcsSUCCESS;
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
    inline vobsPROPERTY_TYPE GetPropertyType(const char* id) const __attribute__ ((always_inline))
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
    inline vobsPROPERTY_TYPE GetPropertyType(const vobsSTAR_PROPERTY* property) const __attribute__ ((always_inline))
    {
        if (IS_NULL(property))
        {
            return vobsSTRING_PROPERTY;
        }

        // Return property
        return property->GetType();
    }

    /**
     * Get a star property origin index.
     *
     * @sa vobsSTAR_PROPERTY
     *
     * @param id property id.
     *
     * @return property origin index.
     */
    inline vobsORIGIN_INDEX GetPropertyOrigIndex(const char* id) const __attribute__ ((always_inline))
    {
        // Look for property
        vobsSTAR_PROPERTY* property = GetProperty(id);

        // Return property confidence index
        return property->GetOriginIndex();
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
    inline vobsCONFIDENCE_INDEX GetPropertyConfIndex(const char* id) const __attribute__ ((always_inline))
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
    inline mcsLOGICAL IsPropertySet(const char* id) const __attribute__ ((always_inline))
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
    inline mcsLOGICAL IsPropertySet(const mcsINT32 idx) const __attribute__ ((always_inline))
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
    inline static mcsLOGICAL IsPropertySet(const vobsSTAR_PROPERTY* property) __attribute__ ((always_inline))
    {
        return IS_NULL(property) ? mcsFALSE : property->IsSet();
    }

    /**
     * Check whether the property error is set or not.
     *
     * @param property property to use.
     *
     * @warning If the given property is NULL, this method returns mcsFALSE.
     *
     * @return mcsTRUE if the the property error has been set, mcsFALSE otherwise.
     */
    inline static mcsLOGICAL IsPropertyErrorSet(const vobsSTAR_PROPERTY* property) __attribute__ ((always_inline))
    {
        return IS_NULL(property) ? mcsFALSE : property->IsErrorSet();
    }

    /**
     * Return the number of properties
     *
     * @return the number of properties of the star
     */
    inline mcsINT32 NbProperties(void) const __attribute__ ((always_inline))
    {
        return _nProps;
    }

    /**
     * Return whether the star is the same as another given one
     * i.e. coordinates (RA/DEC) in degrees are the same (equals)
     *
     * @param star the other star.
     *
     * @return mcsTRUE if the stars are the same, mcsFALSE otherwise.
     */
    inline mcsLOGICAL IsSame(vobsSTAR* star) const __attribute__ ((always_inline))
    {
        // always check RA/DEC are defined:
        if (IS_FALSE(isRaDecSet()) || IS_FALSE(star->isRaDecSet()))
        {
            return mcsFALSE;
        }
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

        // Get declination of stars. If not set return FALSE
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

    inline mcsLOGICAL IsMatchingGaiaMags(const vobsSTAR* star,
                                         mcsDOUBLE ref_Vmag, mcsDOUBLE ref_e_Vmag,
                                         mcsDOUBLE star_Gmag,
                                         mcsDOUBLE nSigma,
                                         mcsDOUBLE* distance) const __attribute__ ((always_inline))
    {
        // use simpler check if (Bp or Rp) is missing
        bool simple = false;

        mcsDOUBLE star_BPmag, star_RPmag;
        vobsSTAR_PROPERTY* property = star->GetProperty(vobsSTAR_PHOT_MAG_GAIA_BP); // Bp

        // Get the magnitude value
        if (isPropSet(property))
        {
            if (star->GetPropertyValue(property, &star_BPmag) == mcsFAILURE)
            {
                return mcsFALSE;
            }
        }
        else
        {
            // Bp required, use simpler approach:
            simple = true;
        }

        property = star->GetProperty(vobsSTAR_PHOT_MAG_GAIA_RP); // Rp

        // Get the magnitude value
        if (isPropSet(property))
        {
            if (star->GetPropertyValue(property, &star_RPmag) == mcsFAILURE)
            {
                return mcsFALSE;
            }
        }
        else
        {
            // Rp required, use simpler approach:
            simple = true;
        }

        mcsDOUBLE dist = NAN;
        mcsLOGICAL doMatch = mcsFALSE;

        if (!simple)
        {
            // Compute V_est from (G-V)=f(Bp-Rp) law:
            mcsDOUBLE BP_RP = star_BPmag - star_RPmag;

            // Check validity range:
            if ((BP_RP >= -0.3) && (BP_RP <= 4.5))
            {
                /* use logPrint instead of logP because MODULE_ID is undefined in header files */
                logPrint("vobs", logDEBUG, NULL, __FILE_LINE__,
                         "IsMatchingGaiaMags: ref V = %.3f (%.3f) star [Bp G Rp] = [%.3f %.3f %.3f]",
                         ref_Vmag, ref_e_Vmag, star_BPmag, star_Gmag, star_RPmag);

                mcsDOUBLE star_V_est = star_Gmag + 0.015; // offset

                if (BP_RP <= 2.5)
                {
                    // Published polynomial law:
                    star_V_est += (0.01760 + 0.006860 * BP_RP + 0.1732 * BP_RP * BP_RP); // sigma ~ 0.06
                }
                else
                {
                    // Extrapolated law on TYCHO / GAIA xmatch:
                    star_V_est += (0.28 + 0.134 * BP_RP * BP_RP);
                }

                // Check consistency using n * sigma:
                dist = (star_V_est - ref_Vmag);
                mcsDOUBLE threshold = mcsMIN(0.5, mcsMAX(0.15, ref_e_Vmag)); // use 0.15 mag as min uncertainty, 0.5 mag at max
                threshold *= nSigma;

                if (fabs(dist) <= threshold)
                {
                    doMatch = mcsTRUE;
                }

                logPrint("vobs", logDEBUG, NULL, __FILE_LINE__,
                         "IsMatchingGaiaMags: V_ref = %.3f, V_est = %.3f, dist = %.3f, th = %.3f : %s",
                         ref_Vmag, star_V_est, dist, threshold,
                         (doMatch == mcsTRUE) ? "True" : "False"
                         );
            }
            else
            {
                logPrint("vobs", logDEBUG, NULL, __FILE_LINE__, "IsMatchingGaiaMags: invalid range for Bp-Rp = ", BP_RP);
            }
        }

        if (simple || !doMatch)
        {
            // Check consistency on -1.0 < (G-V) < 0.2:
            dist = (star_Gmag - ref_Vmag); // G - V
            mcsDOUBLE threshold_low = mcsMIN(0.3, mcsMAX(0.1, ref_e_Vmag)); // use 0.1 mag as min uncertainty, 0.3 mag at max
            threshold_low *= nSigma; // 0.5..1.5 mags
            mcsDOUBLE threshold_up = mcsMAX(0.3, ref_e_Vmag); // use 0.3 mag as min uncertainty, e_V at max

            if ((dist >= -threshold_low) && (dist <= threshold_up))
            {
                doMatch = mcsTRUE;
            }

            logPrint("vobs", logDEBUG, NULL, __FILE_LINE__,
                     "IsMatchingGaiaMags: V_ref = %.3f, G = %.3f, dist = %.3f, th_low = %.3f, th_up = %.3f : %s",
                     ref_Vmag, star_Gmag, dist, -threshold_low, threshold_up,
                     (doMatch == mcsTRUE) ? "True" : "False"
                     );
        }

        if (distance != NULL)
        {
            *distance = fabs(dist);
        }
        return doMatch;
    }

    /**
     * Return whether this star and the given star are matching criteria as
     * shown below:
     * @code
     * mcsINT32 nCriteria = 0;
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
     * if (IS_TRUE(star->IsMatchingCriteria(anotherStar, criterias, nCriteria)))
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
                                         mcsDOUBLE* distAng = NULL,
                                         mcsDOUBLE* distMag = NULL,
                                         mcsUINT32* noMatchs = NULL) const __attribute__ ((always_inline))
    {
        // assumption: the criteria list is not NULL

        // Get criteria informations
        vobsSTAR_CRITERIA_INFO* criteria = NULL;
        mcsDOUBLE dec1, dec2, ra1, ra2;
        mcsDOUBLE delta;
        mcsINT32 propIndex, otherPropIndex;
        vobsSTAR_PROPERTY* prop1 = NULL;
        vobsSTAR_PROPERTY* prop2 = NULL;
        mcsDOUBLE val1, val2, eVal1;
        const char *val1Str = NULL, *val2Str = NULL;
        // computed distance:
        mcsDOUBLE dist = NAN;
        const vobsSTAR* starGaia = NULL;
        const vobsSTAR* starRef = NULL;

        // Get each criteria of the list and check if the comparaison with all
        // this criteria gave a equality

        for (mcsUINT32 el = 0; el < nCriteria; el++)
        {
            criteria = &criterias[el];

            switch (criteria->propCompType)
            {
                case vobsPROPERTY_COMP_RA_DEC:
                    // always check RA/DEC are defined:
                    if (IS_FALSE(isRaDecSet()) || IS_FALSE(star->isRaDecSet()))
                    {
                        NO_MATCH(noMatchs, el);
                    }
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

                    // Get declination of the star. If not set return FALSE
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
                        if (IS_NOT_NULL(distAng))
                        {
                            *distAng = dist;
                        }
                    }
                    else
                    {
                        // Box area:
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

                        if (IS_NOT_NULL(distAng))
                        {
                            // Update separation
                            // compute angular separation using haversine formula:
                            if (alxComputeDistanceInDegrees(ra1, dec1, ra2, dec2, &dist) == mcsFAILURE)
                            {
                                NO_MATCH(noMatchs, el);
                            }

                            // return computed distance
                            *distAng = dist;
                        }
                    }
                    break;

                default:
                case vobsPROPERTY_COMP_FLOAT:
                    propIndex = criteria->propertyIndex;
                    prop1 = GetProperty(propIndex);
                    otherPropIndex = criteria->otherPropertyIndex;
                    prop2 = star->GetProperty(otherPropIndex);

                    /* note: if both property not set, it does NOT match criteria */
                    if (isNotPropSet(prop1) || (GetPropertyValue(prop1, &val1) == mcsFAILURE))
                    {
                        NO_MATCH(noMatchs, el);
                    }

                    if (isNotPropSet(prop2) || (star->GetPropertyValue(prop2, &val2) == mcsFAILURE))
                    {
                        NO_MATCH(noMatchs, el);
                    }

                    delta = fabs(val1 - val2);

                    if (delta > criteria->range)
                    {
                        NO_MATCH(noMatchs, el);
                    }
                    break;

                case vobsPROPERTY_COMP_GAIA_MAGS:
                    propIndex = criteria->propertyIndex;
                    otherPropIndex = criteria->otherPropertyIndex;

                    // For symetry, identify which star comes from GAIA(G defined but no V = not already merged):
                    prop1 = star->GetProperty(otherPropIndex); // PHOT_MAG_G
                    if (isPropSet(prop1))
                    {
                        prop1 = star->GetProperty(propIndex); // PHOT_JHN_V

                        if (isNotPropSet(prop1))
                        {
                            starGaia = star;
                            starRef = this;
                        }
                    }

                    prop1 = this->GetProperty(otherPropIndex); // PHOT_MAG_G
                    if (isPropSet(prop1))
                    {
                        prop1 = this->GetProperty(propIndex); // PHOT_JHN_V

                        if (isNotPropSet(prop1))
                        {
                            starGaia = this;
                            starRef = star;
                        }
                    }

                    if (IS_NULL(starGaia))
                    {
                        NO_MATCH(noMatchs, el);
                    }

                    // check (reentrance) ie G but not V !
                    prop1 = starRef->GetProperty(propIndex); // PHOT_JHN_V
                    prop2 = starGaia->GetProperty(otherPropIndex); // PHOT_MAG_G

                    /* note: if both property not set, it does NOT match criteria */

                    if (isNotPropSet(prop1) || (starRef->GetPropertyValueAndError(prop1, &val1, &eVal1) == mcsFAILURE))
                    {
                        NO_MATCH(noMatchs, el);
                    }

                    if (isNotPropSet(prop2) || (starGaia->GetPropertyValue(prop2, &val2) == mcsFAILURE))
                    {
                        NO_MATCH(noMatchs, el);
                    }

                    if (IsMatchingGaiaMags(starGaia, val1, eVal1, val2, criteria->range, &dist) == mcsFALSE)
                    {
                        NO_MATCH(noMatchs, el);
                    }

                    // return computed distance on magnitudes
                    if (IS_NOT_NULL(distMag))
                    {
                        *distMag = dist;
                    }
                    break;

                case vobsPROPERTY_COMP_STRING:
                    propIndex = criteria->propertyIndex;

                    prop1 = GetProperty(propIndex);
                    prop2 = star->GetProperty(propIndex);

                    /* note: if both property not set, it does match criteria */

                    val1Str = (isPropSet(prop1)) ? GetPropertyValue(prop1) : "";
                    val2Str = (isPropSet(prop2)) ? star->GetPropertyValue(prop2) : "";

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
     * Get the star property corresponding to the xmatch log identifier (useful for internal cross matchs).
     *
     * @return pointer on the found star property object on successful completion.
     * Otherwise NULL is returned.
     */
    inline vobsSTAR_PROPERTY* GetXmLogProperty() const __attribute__ ((always_inline))
    {
        return GetProperty(vobsSTAR::vobsSTAR_PropertyXmLogIndex);
    }

    inline vobsSTAR_PROPERTY* GetXmMainFlagProperty() const __attribute__ ((always_inline))
    {
        return GetProperty(vobsSTAR::vobsSTAR_PropertyXmMainFlagIndex);
    }

    inline vobsSTAR_PROPERTY* GetXmAllFlagProperty() const __attribute__ ((always_inline))
    {
        return GetProperty(vobsSTAR::vobsSTAR_PropertyXmAllFlagIndex);
    }

    /**
     * Get the star property corresponding to the target identifier (useful for internal cross matchs).
     *
     * @return pointer on the found star property object on successful completion.
     * Otherwise NULL is returned.
     */
    inline vobsSTAR_PROPERTY* GetTargetIdProperty() const __attribute__ ((always_inline))
    {
        return GetProperty(vobsSTAR::vobsSTAR_PropertyTargetIdIndex);
    }

    /**
     * Get the star property corresponding to the observation date (useful for internal cross matchs).
     *
     * @return pointer on the found star property object on successful completion.
     * Otherwise NULL is returned.
     */
    inline vobsSTAR_PROPERTY* GetJdDateProperty() const __attribute__ ((always_inline))
    {
        return GetProperty(vobsSTAR::vobsSTAR_PropertyJDIndex);
    }

    /**
     * Allocate dynamically a new mask (must be freed)
     * @return
     */
    inline static vobsSTAR_PROPERTY_MASK* GetPropertyMask(const mcsUINT32 nIds, const char* overwriteIds[]) __attribute__ ((always_inline))
    {
        vobsSTAR_PROPERTY_MASK* mask = new vobsSTAR_PROPERTY_MASK(vobsSTAR_PROPERTY_META::vobsStar_PropertyMetaList.size(), false);

        const char* id;
        mcsINT32 idx;
        for (mcsUINT32 el = 0; el < nIds; el++)
        {
            id = overwriteIds[el];

            idx = vobsSTAR::GetPropertyIndex(id);

            if (idx == -1)
            {
                /* use logPrint instead of logP because MODULE_ID is undefined in header files */
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
    static mcsINT32 GetRa(mcsSTRING32& raHms, mcsDOUBLE &ra);

    /* Convert declination (DEC) coordinate from DMS (+/-DD MM SS.TT) into degrees [-90; 90] */
    static mcsINT32 GetDec(mcsSTRING32& decDms, mcsDOUBLE &dec);

    /* Convert right ascension (RA) coordinate from degrees [-180; 180] into HMS (HH MM SS.TTT) */
    static void ToHms(mcsDOUBLE ra, mcsSTRING32 &raHms);

    /* Convert declination (DEC) coordinate from degrees [-90; 90] into DMS (+/-DD MM SS.TT)*/
    static void ToDms(mcsDOUBLE dec, mcsSTRING32 &decDms);

    /* Convert right ascension (RA) coordinate from degrees [-180; 180] into degrees (xxx.xxxxxx) */
    static void raToDeg(mcsDOUBLE ra, mcsSTRING16 &raDeg);

    /* Convert declination (DEC) coordinate from degrees [-90; 90] into degrees (+/-xx.xxxxxx) */
    static void decToDeg(mcsDOUBLE dec, mcsSTRING16 &decDeg);

    /* Convert concatenated RA/DEC 'xxx.xxxxxx(+/-)xx.xxxxxx' coordinates into degrees */
    static mcsCOMPL_STAT degToRaDec(const char* raDec, mcsDOUBLE &ra, mcsDOUBLE &dec);

    static void FreePropertyIndex(void);

    void ClearRaDec(void);
    void SetRaDec(const mcsDOUBLE ra, const mcsDOUBLE dec) const;

    mcsCOMPL_STAT PrecessRaDecJ2000ToEpoch(const mcsDOUBLE epoch, mcsDOUBLE &raEpo, mcsDOUBLE &decEpo) const;
    mcsCOMPL_STAT CorrectRaDecEpochs(mcsDOUBLE ra, mcsDOUBLE dec, const mcsDOUBLE pmRa, const mcsDOUBLE pmDec, const mcsDOUBLE epochFrom, const mcsDOUBLE epochTo) const;

    static mcsDOUBLE GetPrecessedRA(const mcsDOUBLE raDeg, const mcsDOUBLE pmRa, const mcsDOUBLE epochFrom, const mcsDOUBLE epochTo);
    static mcsDOUBLE GetPrecessedDEC(const mcsDOUBLE decDeg, const mcsDOUBLE pmDec, const mcsDOUBLE epochFrom, const mcsDOUBLE epochTo);

    static mcsDOUBLE GetDeltaRA(const mcsDOUBLE pmRa, const mcsDOUBLE deltaEpoch);
    static mcsDOUBLE GetDeltaDEC(const mcsDOUBLE pmDec, const mcsDOUBLE deltaEpoch);

protected:

    // Add a property meta data.
    static void AddPropertyMeta(const char* id,
                                const char* name,
                                const vobsPROPERTY_TYPE type,
                                const char* unit,
                                const char* description,
                                const char* link = NULL);

    // Add a property meta data with a custom format.
    static void AddFormattedPropertyMeta(const char* id,
                                         const char* name,
                                         const vobsPROPERTY_TYPE type,
                                         const char* unit,
                                         const char* format,
                                         const char* description,
                                         const char* link = NULL);

    // Add an property error meta data.
    static void AddPropertyErrorMeta(const char* id, const char* name,
                                     const char* unit, const char* description = NULL);

    static void initializeIndex(void);

    static mcsCOMPL_STAT DumpPropertyIndexAsXML(miscoDYN_BUF& buffer, const char* name, const mcsINT32 from, const mcsINT32 end);

    // Method to define all star properties
    mcsCOMPL_STAT AddProperties(void);

private:
    static vobsSTAR_PROPERTY_INDEX_MAPPING vobsSTAR_PropertyIdx;
    static vobsSTAR_PROPERTY_INDEX_MAPPING vobsSTAR_PropertyErrorIdx;

    static mcsINT32 vobsSTAR_PropertyMetaBegin;
    static mcsINT32 vobsSTAR_PropertyMetaEnd;
    static bool vobsSTAR_PropertyIdxInitialized;

    // RA/DEC property indexes (read-only):
    static mcsINT32 vobsSTAR_PropertyRAIndex;
    static mcsINT32 vobsSTAR_PropertyDECIndex;
    // XmLog property index:
    static mcsINT32 vobsSTAR_PropertyXmLogIndex;
    // Xm Main flag property index (read-only):
    static mcsINT32 vobsSTAR_PropertyXmMainFlagIndex;
    // Xm All flag property index (read-only):
    static mcsINT32 vobsSTAR_PropertyXmAllFlagIndex;
    // Target Id property index (read-only):
    static mcsINT32 vobsSTAR_PropertyTargetIdIndex;
    // PMRA/PMDEC property indexes (read-only):
    static mcsINT32 vobsSTAR_PropertyPMRAIndex;
    static mcsINT32 vobsSTAR_PropertyPMDECIndex;
    // JD property index (read-only):
    static mcsINT32 vobsSTAR_PropertyJDIndex;

    /* Memory footprint (sizeof) = 48 bytes (8-bytes alignment) */

    // ra/dec are mutable to be modified even by const methods
    mutable mcsDOUBLE _ra;     // parsed RA     // 8 bytes
    mutable mcsDOUBLE _dec;    // parsed DEC    // 8 bytes

    mcsUINT8 _nProps;                           // 1 byte (max 255 properties)
    vobsSTAR_PROPERTY* _properties;             // 8 bytes

    static mcsCOMPL_STAT DumpPropertyIndexAsXML();

} ;


#endif /*!vobsSTAR_H*/
/*___oOo___*/
