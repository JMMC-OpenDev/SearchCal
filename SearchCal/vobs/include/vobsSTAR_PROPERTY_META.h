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
#define vobsSTAR_VAL_NOT_SET "-"   /**< Default value of empty properties */
#define vobsSTAR_NO_ORIGIN    ""    /**< Undefined origin ('') */

/* convenience macros */
#define isValueSet(value) \
    (strcmp(value, vobsSTAR_VAL_NOT_SET) != 0)

#define hasOrigin(origin) \
    (strcmp(origin, vobsSTAR_NO_ORIGIN) != 0)

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
            if (isValueSet(_unit))
            {
                FAIL(buffer.AppendString("    <unit>"));
                FAIL(buffer.AppendString(_unit));
                FAIL(buffer.AppendString("</unit>\n"));
            }

            FAIL(buffer.AppendString("    <format>"));
            FAIL(buffer.AppendString(_format));
            FAIL(buffer.AppendString("</format>\n"));

            if (_link != NULL)
            {
                FAIL(buffer.AppendString("    <link>"));
                FAIL(buffer.AppendString(_link));
                FAIL(buffer.AppendString("</link>\n"));
            }

            if (_description != NULL)
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
};

#endif /*!vobsSTAR_PROPERTY_META_H*/

/*___oOo___*/
