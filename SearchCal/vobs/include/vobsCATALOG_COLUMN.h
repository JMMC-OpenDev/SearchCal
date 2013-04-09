#ifndef vobsCATALOG_COLUMN_H
#define	vobsCATALOG_COLUMN_H
/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * @file
 * vobsCATALOG_COLUMN_H class declaration.
 */

#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif

/* 
 * System Headers 
 */

/*
 * Local headers
 */
#include "log.h"
#include "misco.h"
#include "vobsSTAR.h"


/*
 * Class declaration
 */

/**
 * Catalog column meta data. 
 * 
 * The vobsCATALOG_COLUMN contains catalog meta data (epoch, astrometric precision ...)
 *
 */
class vobsCATALOG_COLUMN
{
public:
    // Class constructor

    /**
     * Build a catalog column meta data.
     * @param id column ID
     * @param ucd column UCD
     * @param propertyId associated star property ID
     */
    vobsCATALOG_COLUMN(const char* id, const char* ucd, const char* propertyId)
    {
        // Set ID
        _id = id;

        // Set UCD (1.0)
        _ucd = ucd;

        // Set Property Id
        _propertyId = propertyId;

        if (propertyId == NULL)
        {
            _propertyIdx = -1; // undefined
        }
        else
        {
            // Set corresponding Property index:
            _propertyIdx = vobsSTAR::GetPropertyIndex(propertyId);

            if (_propertyIdx == -1)
            {
                logPrint("vobs", logWARNING, NULL, __FILE_LINE__, "Property[%s] not found by vobsSTAR::GetPropertyIndex() !", propertyId);
            }
        }
    }

    // Class destructor

    ~vobsCATALOG_COLUMN()
    {
    }

    /**
     * Get the catalog column ID as string literal
     *
     * @return catalog column ID.
     */
    inline const char* GetId() const __attribute__((always_inline))
    {
        return _id;
    }

    /**
     * Get the catalog column UCD as string literal
     *
     * @return catalog column UCD.
     */
    inline const char* GetUcd() const __attribute__((always_inline))
    {
        return _ucd;
    }

    /**
     * Get the Property ID associated to this catalog column
     *
     * @return Property ID associated to this catalog column.
     */
    inline const char* GetPropertyId() const __attribute__((always_inline))
    {
        return _propertyId;
    }

    /**
     * Get the Property index associated to this catalog column
     *
     * @return Property index associated to this catalog column or -1 if not found
     */
    inline int GetPropertyIdx() const __attribute__((always_inline))
    {
        return _propertyIdx;
    }

    /**
     * Return the property meta data associated to this catalog column
     * @return property meta (pointer) or NULL if not found
     */
    inline vobsSTAR_PROPERTY_META* GetPropertyMeta() const __attribute__((always_inline))
    {
        return vobsSTAR::GetPropertyMeta(_propertyIdx);
    }

    /**
     * Dump the catalog meta into given buffer
     * 
     * @param buffer buffer to append into
     *
     * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned 
     */
    mcsCOMPL_STAT DumpCatalogColumnAsXML(miscoDYN_BUF& buffer) const
    {
        FAIL(buffer.AppendString("\n      <column>\n"));

        FAIL(buffer.AppendString("        <id>"));
        FAIL(buffer.AppendString(_id));
        FAIL(buffer.AppendString("</id>\n"));

        FAIL(buffer.AppendString("        <ucd>"));
        FAIL(buffer.AppendString(_ucd));
        FAIL(buffer.AppendString("</ucd>\n"));

        vobsSTAR_PROPERTY_META* meta = GetPropertyMeta();
        if (meta != NULL)
        {
            // short mode:
            meta->DumpAsXML(buffer, "vobsSTAR", _propertyIdx, false);
        }

        FAIL(buffer.AppendString("      </column>\n"));

        return mcsSUCCESS;
    }


private:
    // Declaration of copy constructor and assignment operator as private
    // methods, in order to hide them from the users.
    vobsCATALOG_COLUMN(const vobsCATALOG_COLUMN& property);
    vobsCATALOG_COLUMN& operator=(const vobsCATALOG_COLUMN& meta);

    // metadata members (constant):

    // ID of the catalog column
    const char* _id;

    // UCD of the catalog column
    const char* _ucd;

    // Property ID associated to this catalog column
    const char* _propertyId;

    // Property Index (use by vobsSTAR methods)
    int _propertyIdx;
};

#endif /*!vobsCATALOG_COLUMN_H*/

/*___oOo___*/
