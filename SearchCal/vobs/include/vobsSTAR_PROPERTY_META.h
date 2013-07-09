#ifndef vobsSTAR_PROPERTY_META_H
#define	vobsSTAR_PROPERTY_META_H
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

/**
 * Origin index (Provenance) (... values iso needs only ... bits)
 */
typedef enum
{
    vobsORIG_NONE             = 0,      /** No catalog / origin                         */
    vobsORIG_MIXED_CATALOG    = 1,      /** Mixed catalog origin (merge star list)      */
    vobsORIG_COMPUTED         = 2,      /** Computed value                              */
    vobsCATALOG_AKARI_ID      = 3,      /** AKARI catalog [II/297/irc]                  */
    vobsCATALOG_ASCC_ID       = 4,      /** ASCC catalog [I/280]                        */
    vobsCATALOG_ASCC_LOCAL_ID = 5,      /** ASCC LOCAL catalog [I/280B]                 */
    vobsCATALOG_BSC_ID        = 6,      /** BSC catalog [V/50/catalog]                  */
    vobsCATALOG_CIO_ID        = 7,      /** CIO catalog [II/225/catalog]                */
    vobsCATALOG_DENIS_ID      = 8,      /** Denis catalog [B/denis]                     */
    vobsCATALOG_DENIS_JK_ID   = 9,      /** Denis JK catalog [J/A+A/413/1037/table1]    */
    vobsCATALOG_HIC_ID        = 10,     /** HIC catalog [I/196/main]                    */
    vobsCATALOG_HIP1_ID       = 11,     /** HIP catalog [I/239/hip_main]                */
    vobsCATALOG_HIP2_ID       = 12,     /** HIP2 catalog [I/311/hip2]                   */
    vobsCATALOG_LBSI_ID       = 13,     /** LBSI catalog [J/A+A/393/183/catalog]        */
    vobsCATALOG_MASS_ID       = 14,     /** 2MASS catalog [II/246/out]                  */
    vobsCATALOG_MERAND_ID     = 15,     /** Merand catalog [J/A+A/433/1155]             */
    vobsCATALOG_MIDI_ID       = 16,     /** MIDI local catalog [MIDI]                   */
    vobsCATALOG_PHOTO_ID      = 17,     /** PHOTO catalog [II/7A/catalog]               */
    vobsCATALOG_SBSC_ID       = 18,     /** SBSC catalog [V/36B/bsc4s]                  */
    vobsCATALOG_SB9_ID        = 19,     /** SB9 catalog [B/sb9/main]                    */
    vobsCATALOG_USNO_ID       = 20,     /** USNO catalog [I/284]                        */
    vobsCATALOG_WDS_ID        = 21,     /** WDS catalog [B/wds/wds]                     */
    vobsNB_ORIGIN_INDEX                 /** number of Origin index                      */
} vobsORIGIN_INDEX;

/* vobsNO_CATALOG_ID is an alias for vobsORIG_NONE */
#define vobsNO_CATALOG_ID       vobsORIG_NONE

/* confidence index as label string mapping */
static const char* const vobsORIGIN_STR[] = {"NO CATALOG", "MIXED CATALOG", "computed",
                                             "II/297/irc", "I/280", "I/280B", "V/50/catalog" , "II/225/catalog",
                                             "B/denis", "J/A+A/413/1037/table1", "I/196/main", "I/239/hip_main",
                                             "I/311/hip2", "J/A+A/393/183/catalog", "II/246/out", "J/A+A/433/1155",
                                             "MIDI", "II/7A/catalog", "V/36B/bsc4s", "B/sb9/main", "I/284", "B/wds/wds"};

/* confidence index as integer string mapping */
static const char* const vobsORIGIN_INT[] = {"0", "1", "2",
                                             "3", "4", "5", "6", "7",
                                             "8", "9", "10", "11",
                                             "12", "13", "14", "15",
                                             "16", "17", "18", "19", "20", "21"};

/**
 * Return the string literal representing the origin index 
 * @return string literal "NO CATALOG", "computed", "II/297/irc" ... "B/wds/wds"
 */
const char* vobsGetOriginIndex(vobsORIGIN_INDEX originIndex);

/**
 * Return the integer literal representing the origin index 
 * @return integer literal "0" (NO CATALOG), "1" (computed), "10" (II/297/irc) ... "28" (B/wds/wds)
 */
const char* vobsGetOriginIndexAsInt(vobsORIGIN_INDEX originIndex);


/* convenience macros */
#define isPropComputed(catalogId) \
    (catalogId == vobsORIG_COMPUTED)

#define isCatalog(catalogId) \
    (catalogId != vobsORIG_NONE)

#define hasOrigin(catalogId) \
    (catalogId != vobsORIG_NONE)

#define isCatalogCio(catalogId) \
    (catalogId == vobsCATALOG_CIO_ID)

#define isCatalogDenis(catalogId) \
    (catalogId == vobsCATALOG_DENIS_ID)

#define isCatalogDenisJK(catalogId) \
    (catalogId == vobsCATALOG_DENIS_JK_ID)

#define isCatalogHip1(catalogId) \
    (catalogId == vobsCATALOG_HIP1_ID)

#define isCatalogLBSI(catalogId) \
    (catalogId == vobsCATALOG_LBSI_ID)

#define isCatalog2Mass(catalogId) \
    (catalogId == vobsCATALOG_MASS_ID)

#define isCatalogMerand(catalogId) \
    (catalogId == vobsCATALOG_MERAND_ID)

#define isCatalogPhoto(catalogId) \
    (catalogId == vobsCATALOG_PHOTO_ID)


typedef enum
{
    vobsSTRING_PROPERTY = 0,
    vobsFLOAT_PROPERTY  = 1,
    vobsINT_PROPERTY    = 2,
    vobsBOOL_PROPERTY   = 3
} vobsPROPERTY_TYPE;

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
                           const char* unit = NULL,
                           const char* format = NULL,
                           const char* link = NULL,
                           const char* description = NULL);

    // Class destructor
    ~vobsSTAR_PROPERTY_META();

    /**
     * Get property id.
     *
     * @return property id
     */
    inline const char* GetId(void) const __attribute__((always_inline))
    {
        // Return property id
        return _id;
    }

    /**
     * Get property name.
     *
     * @return property name
     */
    inline const char* GetName(void) const __attribute__((always_inline))
    {
        // Return property name
        return _name;
    }

    /**
     * Get property type.
     *
     * @return property type
     */
    inline const vobsPROPERTY_TYPE GetType(void) const __attribute__((always_inline))
    {
        // Return property type
        return _type;
    }

    /**
     * Get property unit.
     *
     * @sa http://vizier.u-strasbg.fr/doc/catstd-3.2.htx
     *
     * @return property unit if present, vobsSTAR_PROP_NOT_SET otherwise.
     */
    inline const char* GetUnit(void) const __attribute__((always_inline))
    {
        // Return property unit
        return _unit;
    }

    /**
     * Get property format.
     *
     * @return property format
     */
    inline const char* GetFormat(void) const __attribute__((always_inline))
    {
        // Return property format
        return _format;
    }

    /**
     * Get property CDS link.
     *
     * @return property CDS link if present, NULL otherwise.
     */
    inline const char* GetLink(void) const __attribute__((always_inline))
    {
        // Return property CDS link
        return _link;
    }

    /**
     * Get property description.
     *
     * @sa http://vizier.u-strasbg.fr/doc/catstd-3.2.htx
     *
     * @return property description if present, NULL otherwise.
     */
    inline const char* GetDescription(void) const __attribute__((always_inline))
    {
        // Return property description
        return _description;
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
    mcsCOMPL_STAT DumpAsXML(miscoDYN_BUF& buffer, const char* prefix, const int idx, const bool full) const
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

            // If the unit exists (not the default vobsSTAR_PROP_NOT_SET)
            if (isNotNull(_unit))
            {
                FAIL(buffer.AppendString("    <unit>"));
                FAIL(buffer.AppendString(_unit));
                FAIL(buffer.AppendString("</unit>\n"));
            }

            FAIL(buffer.AppendString("    <format>"));
            FAIL(buffer.AppendString(_format));
            FAIL(buffer.AppendString("</format>\n"));

            if (isNotNull(_link))
            {
                FAIL(buffer.AppendString("    <link>"));
                FAIL(buffer.AppendString(_link));
                FAIL(buffer.AppendString("</link>\n"));
            }

            if (isNotNull(_description))
            {
                FAIL(buffer.AppendString("    <description>"));
                FAIL(buffer.AppendString(_description));
                FAIL(buffer.AppendString("</description>\n"));
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

    // metadata members (constant):
    const char* _id; // Identifier
    const char* _name; // Name
    vobsPROPERTY_TYPE _type; // Type of the value
    const char* _unit; // Unit of the value
    const char* _format; // Format to print value 
    const char* _link; // CDS link of the value
    const char* _description; // Description of the value
} ;

#endif /*!vobsSTAR_PROPERTY_META_H*/

/*___oOo___*/
