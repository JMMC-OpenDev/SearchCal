#ifndef vobsCATALOG_COLUMN_H
#define vobsCATALOG_COLUMN_H
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

        // By default:
        _isError = false;

        if (IS_NULL(propertyId))
        {
            _propertyIdx = -1;
            // CIO catalog: lambda and F(IR) columns has specific mapping rules (see CDATA.h)
            logPrint("vobs", logDEBUG, NULL, __FILE_LINE__, "Column [%s ] has no property identifier !", id);
        }
        else
        {
            // Set corresponding Property index:
            _propertyIdx = vobsSTAR::GetPropertyIndex(propertyId);

            if (_propertyIdx == -1)
            {
                // try property error index:
                _propertyIdx = vobsSTAR::GetPropertyErrorIndex(propertyId);
                _isError     = true;
            }
            if (_propertyIdx == -1)
            {
                /* use logPrint instead of logP because MODULE_ID is undefined in header files */
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
    inline const char* GetId() const __attribute__ ((always_inline))
    {
        return _id;
    }

    /**
     * Get the catalog column UCD as string literal
     *
     * @return catalog column UCD.
     */
    inline const char* GetUcd() const __attribute__ ((always_inline))
    {
        return _ucd;
    }

    /**
     * Get the Property ID associated to this catalog column
     *
     * @return Property ID associated to this catalog column.
     */
    inline const char* GetPropertyId() const __attribute__ ((always_inline))
    {
        return _propertyId;
    }

    /**
     * Get the Property index associated to this catalog column
     *
     * @return Property index associated to this catalog column or -1 if not found
     */
    inline mcsINT32 GetPropertyIdx() const __attribute__ ((always_inline))
    {
        return _propertyIdx;
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

        FAIL(buffer.AppendString("        <isError>"));
        FAIL(buffer.AppendString(_isError ? "true" : "false"));
        FAIL(buffer.AppendString("</isError>\n"));

        const vobsSTAR_PROPERTY_META* meta = (_isError) ? vobsSTAR_PROPERTY_META::GetPropertyErrorMeta(_propertyIdx)
                : vobsSTAR_PROPERTY_META::GetPropertyMeta(_propertyIdx);
        
        if (IS_NOT_NULL(meta))
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

    // Property Index (used by vobsSTAR methods)
    mcsINT32 _propertyIdx;

    // flag indicating if the value is property error or a property value
    bool _isError;
} ;

#endif /*!vobsCATALOG_COLUMN_H*/

/*___oOo___*/
