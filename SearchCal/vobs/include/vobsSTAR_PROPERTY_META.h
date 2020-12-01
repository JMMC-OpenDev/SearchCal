#ifndef vobsSTAR_PROPERTY_META_H
#define vobsSTAR_PROPERTY_META_H
/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * @file
 * vobsSTAR_PROPERTY_META class declaration.
 */

#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif

/*
 * System Headers
 */
#include <stdio.h>

/*
 * Local headers
 */
#include "misco.h"

/*
 * Constant declaration
 */

/** printf format to have enough precision (up to 6-digits) like 1.123456e-5 (scientific notation) */
#define FORMAT_DEFAULT          "%.7lg"

/** printf format to keep maximum precision (fixed 15-digits) */
#define FORMAT_MAX_PRECISION    "%.15lf"


/* Enable/Disable the DENIS catalog */
#define vobsCATALOG_DENIS_ID_ENABLE false
/* Enable/Disable the USNO catalog (faint scenario) */
#define vobsCATALOG_USNO_ID_ENABLE false

/**
 * Origin index (Provenance) (1 byte)
 * Note: integer values must not change in future:
 *       to have a fixed catalog mapping (forward compatibility issue):
 *       only additions AT END to the vobsORIGIN_INDEX enum are allowed.
 */
typedef enum
{
    vobsORIG_NONE             = 0,      /** No catalog / origin                         */
    vobsORIG_MIXED_CATALOG    = 1,      /** Mixed catalog origin (merge star list)      */
    vobsORIG_COMPUTED         = 2,      /** Computed value                              */
    vobsCATALOG_AKARI_ID      = 3,      /** AKARI catalog [II/297/irc]                  */
    vobsCATALOG_ASCC_ID       = 4,      /** ASCC catalog [I/280]                        */
    vobsCATALOG_JSDC_LOCAL_ID = 5,      /** JSDC BRIGHT LOCAL catalog [I/280B]          */
    vobsCATALOG_BSC_ID        = 6,      /** BSC catalog [V/50/catalog]                  */
    vobsCATALOG_CIO_ID        = 7,      /** CIO catalog [II/225/catalog]                */
    vobsCATALOG_DENIS_ID      = 8,      /** Denis catalog [B/denis]                     */
    vobsCATALOG_DENIS_JK_ID   = 9,      /** Denis JK catalog [J/A+A/413/1037/table1]    */
    vobsCATALOG_HIC_ID        = 10,     /** HIC catalog [I/196/main]                    */
    vobsCATALOG_HIP1_ID       = 11,     /** HIP catalog [I/239/hip_main]                */
    vobsCATALOG_HIP2_ID       = 12,     /** HIP2 catalog [I/311/hip2]                   */
    vobsCATALOG_LBSI_ID       = 13,     /** LBSI catalog [J/A+A/393/183/catalog] (deprecated) */
    vobsCATALOG_MASS_ID       = 14,     /** 2MASS catalog [II/246/out]                  */
    vobsCATALOG_MERAND_ID     = 15,     /** Merand catalog [J/A+A/433/1155] (deprecated) */
    vobsCATALOG_MIDI_ID       = 16,     /** MIDI local catalog [MIDI] (deprecated)      */
    vobsCATALOG_PHOTO_ID      = 17,     /** PHOTO catalog [II/7A/catalog]               */
    vobsCATALOG_SBSC_ID       = 18,     /** SBSC catalog [V/36B/bsc4s]                  */
    vobsCATALOG_SB9_ID        = 19,     /** SB9 catalog [B/sb9/main]                    */
    vobsCATALOG_USNO_ID       = 20,     /** USNO catalog [I/284]                        */
    vobsCATALOG_WDS_ID        = 21,     /** WDS catalog [B/wds/wds]                     */
    vobsCATALOG_JSDC_FAINT_LOCAL_ID = 22, /** JSDC FAINT LOCAL catalog [I/280F]         */
    vobsCATALOG_SIMBAD_ID     = 23,     /** SIMBAD database [SIMBAD]                    */
    vobsCATALOG_WISE_ID       = 24,     /** WISE catalog [II/328/allwise]               */
    vobsCATALOG_GAIA_ID       = 25,     /** GAIA catalog [I/345/gaia2]                  */
    vobsCATALOG_GAIA_DIST_ID  = 26,     /** GAIA distance catalog [I/347/gaia2dis]      */
    vobsCATALOG_MDFC_ID       = 27,     /** MDFC v10 catalog [II/361]                   */
    vobsNB_ORIGIN_INDEX                 /** number of Origin index                      */
}
vobsORIGIN_INDEX;

/* vobsNO_CATALOG_ID is an alias for vobsORIG_NONE */
#define vobsNO_CATALOG_ID       vobsORIG_NONE

/* origin index as label string mapping */
static const char* const vobsORIGIN_STR[] = {"NO CATALOG", "MIXED CATALOG", "computed",
                                             "II/297/irc", "I/280", "I/280B", "V/50/catalog" , "II/225/catalog",
                                             "B/denis", "J/A+A/413/1037/table1", "I/196/main", "I/239/hip_main",
                                             "I/311/hip2", "J/A+A/393/183/catalog", "II/246/out", "J/A+A/433/1155",
                                             "MIDI", "II/7A/catalog", "V/36B/bsc4s", "B/sb9/main", "I/284", "B/wds/wds",
                                             "I/280B", "SIMBAD", "II/328/allwise", "I/345/gaia2", "I/347/gaia2dis",
                                             "II/361"};

/* origin index as integer string mapping */
static const char* const vobsORIGIN_INT[] = {"0", "1", "2",
                                             "3", "4", "5", "6", "7",
                                             "8", "9", "10", "11",
                                             "12", "13", "14", "15",
                                             "16", "17", "18", "19", "20", "21",
                                             "22", "23", "24", "25", "26", "27"};

/**
 * Return the string literal representing the origin index
 * @return string literal "NO CATALOG", "MIXED CATALOG", "computed", "II/297/irc" ... "B/wds/wds"
 */
static const char* vobsGetOriginIndex(vobsORIGIN_INDEX originIndex)
{
    return vobsORIGIN_STR[originIndex];
}

/**
 * Return the integer literal representing the origin index
 * @return integer literal "0" (NO CATALOG), "1" (MIXED CATALOG), "2" (computed), "3" (II/297/irc) ... "21" (B/wds/wds)
 */
static const char* vobsGetOriginIndexAsInt(vobsORIGIN_INDEX originIndex)
{
    return vobsORIGIN_INT[originIndex];
}

/* convenience macros */
#define isPropComputed(originIdx) \
    (originIdx == vobsORIG_COMPUTED)

#define isCatalog(catalogId) \
    (catalogId != vobsORIG_NONE)

#define hasOrigin(catalogId) \
    (catalogId != vobsORIG_NONE)

#define isCatalogASCC(catalogId) \
    (catalogId == vobsCATALOG_ASCC_ID)

#define isCatalogCio(catalogId) \
    (catalogId == vobsCATALOG_CIO_ID)

#define isCatalogDenis(catalogId) \
    (catalogId == vobsCATALOG_DENIS_ID)

#define isCatalogDenisJK(catalogId) \
    (catalogId == vobsCATALOG_DENIS_JK_ID)

#define isCatalogHip1(catalogId) \
    (catalogId == vobsCATALOG_HIP1_ID)

#define isCatalogHip2(catalogId) \
    (catalogId == vobsCATALOG_HIP2_ID)

#define isCatalog2Mass(catalogId) \
    (catalogId == vobsCATALOG_MASS_ID)

#define isCatalogPhoto(catalogId) \
    (catalogId == vobsCATALOG_PHOTO_ID)

#define isCatalogSB9(catalogId) \
    (catalogId == vobsCATALOG_SB9_ID)

#define isCatalogWds(catalogId) \
    (catalogId == vobsCATALOG_WDS_ID)

#define isCatalogWise(catalogId) \
    (catalogId == vobsCATALOG_WISE_ID)

#define isCatalogGaia(catalogId) \
    (catalogId == vobsCATALOG_GAIA_ID)

typedef enum
{
    vobsSTRING_PROPERTY = 0,
    vobsFLOAT_PROPERTY  = 1,
    vobsINT_PROPERTY    = 2,
    vobsBOOL_PROPERTY   = 3
} vobsPROPERTY_TYPE;

/* property type as label string mapping */
static const char* const vobsPROPERTY_TYPE_STR[] = {"STRING", "FLOAT", "INT", "BOOL"};

/*
 * Class declaration
 */

/**
 * Star meta data.
 *
 * The vobsSTAR_PROPERTY_META ...
 *
 */
class vobsSTAR_PROPERTY_META
{
public:
    // Class constructors
    vobsSTAR_PROPERTY_META(const char* id,
                           const char* name,
                           const vobsPROPERTY_TYPE type,
                           const char* unit,
                           const char* format,
                           const char* link = NULL,
                           const char* description = NULL,
                           const bool  isError = false);

    // Class destructor
    ~vobsSTAR_PROPERTY_META();

    /**
     * Get property id.
     *
     * @return property id
     */
    inline const char* GetId(void) const __attribute__ ((always_inline))
    {
        return _id;
    }

    /**
     * Get property name.
     *
     * @return property name
     */
    inline const char* GetName(void) const __attribute__ ((always_inline))
    {
        return _name;
    }

    /**
     * Get property type.
     *
     * @return property type
     */
    inline vobsPROPERTY_TYPE GetType(void) const __attribute__ ((always_inline))
    {
        return _type;
    }

    /**
     * Get property unit.
     *
     * @sa http://vizier.u-strasbg.fr/doc/catstd-3.2.htx
     *
     * @return property unit if present, NULL otherwise.
     */
    inline const char* GetUnit(void) const __attribute__ ((always_inline))
    {
        return _unit;
    }

    /**
     * Get property format.
     *
     * @return property format
     */
    inline const char* GetFormat(void) const __attribute__ ((always_inline))
    {
        return _format;
    }

    /**
     * Get property CDS link.
     *
     * @return property CDS link if present, NULL otherwise.
     */
    inline const char* GetLink(void) const __attribute__ ((always_inline))
    {
        return _link;
    }

    /**
     * Get property description.
     *
     * @sa http://vizier.u-strasbg.fr/doc/catstd-3.2.htx
     *
     * @return property description if present, NULL otherwise.
     */
    inline const char* GetDescription(void) const __attribute__ ((always_inline))
    {
        return _description;
    }

    /**
     * Get the flag to indicate if this meta data describes a property error (true) or a property value (false)
     *
     * @return true for a property error; false otherwise
     */
    inline bool IsError(void) const __attribute__ ((always_inline))
    {
        return _isError;
    }

    /**
     * Get the optional error meta data.
     *
     * @return optional error meta data if present, NULL otherwise.
     */
    inline const vobsSTAR_PROPERTY_META* GetErrorMeta(void) const __attribute__ ((always_inline))
    {
        return _errorMeta;
    }

    /**
     * Define the optional error meta data.
     *
     * @param errorMeta error meta data to set.
     */
    inline void SetErrorMeta(const vobsSTAR_PROPERTY_META* errorMeta) const __attribute__ ((always_inline))
    {
        _errorMeta = errorMeta;
    }

    /**
     * Dump the property meta into given buffer
     *
     * @param buffer buffer to append into
     * @param prefix prefix to use in <define>ID</define>
     * @param idx property index
     * @param full true to give full details; false to have only main ones
     *
     * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned
     */
    mcsCOMPL_STAT DumpAsXML(miscoDYN_BUF& buffer, const char* prefix, const mcsINT32 idx, const bool full) const
    {
        mcsSTRING4 tmp;

        FAIL(buffer.AppendString("\n  <property>\n"));

        FAIL(buffer.AppendString("    <index>"));
        sprintf(tmp, "%d", idx);
        FAIL(buffer.AppendString(tmp));
        FAIL(buffer.AppendString("</index>\n"));

        FAIL(buffer.AppendString("    <define>"));
        FAIL(buffer.AppendString(prefix));
        FAIL(buffer.AppendString("_"));
        FAIL(buffer.AppendString(_id));
        FAIL(buffer.AppendString("</define>\n"));

        FAIL(buffer.AppendString("    <id>"));
        FAIL(buffer.AppendString(_id));
        FAIL(buffer.AppendString("</id>\n"));

        FAIL(buffer.AppendString("    <name>"));
        FAIL(buffer.AppendString(_name));
        FAIL(buffer.AppendString("</name>\n"));

        if (full)
        {
            FAIL(buffer.AppendString("    <type>"));
            switch (_type)
            {
                case vobsSTRING_PROPERTY:
                    FAIL(buffer.AppendString("STRING"));
                    break;
                case vobsFLOAT_PROPERTY:
                    FAIL(buffer.AppendString("FLOAT"));
                    break;
                case vobsINT_PROPERTY:
                    FAIL(buffer.AppendString("INTEGER"));
                    break;
                case vobsBOOL_PROPERTY:
                    FAIL(buffer.AppendString("BOOLEAN"));
                    break;
                default:
                    FAIL(buffer.AppendString("UNDEFINED"));
            }
            FAIL(buffer.AppendString("</type>\n"));

            // If the unit exists
            if (IS_NOT_NULL(_unit))
            {
                FAIL(buffer.AppendString("    <unit>"));
                FAIL(buffer.AppendString(_unit));
                FAIL(buffer.AppendString("</unit>\n"));
            }

            FAIL(buffer.AppendString("    <format>"));
            FAIL(buffer.AppendString(_format));
            FAIL(buffer.AppendString("</format>\n"));

            if (IS_NOT_NULL(_link))
            {
                FAIL(buffer.AppendString("    <link>"));
                FAIL(buffer.AppendString(_link));
                FAIL(buffer.AppendString("</link>\n"));
            }

            if (IS_NOT_NULL(_description))
            {
                FAIL(buffer.AppendString("    <description>"));
                FAIL(buffer.AppendString(_description));
                FAIL(buffer.AppendString("</description>\n"));
            }

            FAIL(buffer.AppendString("        <isError>"));
            FAIL(buffer.AppendString(_isError ? "true" : "false"));
            FAIL(buffer.AppendString("</isError>\n"));

            // error meta data:
            if (IS_NOT_NULL(_errorMeta))
            {
                FAIL(buffer.AppendString("    <error>"));
                _errorMeta->DumpAsXML(buffer, prefix, idx, full);
                FAIL(buffer.AppendString("    </error>\n"));
            }
        }

        FAIL(buffer.AppendString("  </property>\n"));

        return mcsSUCCESS;
    }


private:
    // Declaration of copy constructor and assignment operator as private
    // methods, in order to hide them from the users.
    vobsSTAR_PROPERTY_META(const vobsSTAR_PROPERTY_META& property);
    vobsSTAR_PROPERTY_META& operator=(const vobsSTAR_PROPERTY_META& meta);

    // metadata (constant):
    const char* _id;            // Identifier
    const char* _name;          // Name
    vobsPROPERTY_TYPE _type;    // Type of the value
    const char* _unit;          // Unit of the value
    const char* _format;        // Format to print value
    const char* _link;          // CDS link of the value
    const char* _description;   // Description of the value
    bool        _isError;       // flag to indicate if this meta data describes a property error or a property value
    // error metadata (constant but mutable to be modified even by const methods
    mutable const vobsSTAR_PROPERTY_META* _errorMeta;
} ;

#endif /*!vobsSTAR_PROPERTY_META_H*/

/*___oOo___*/
