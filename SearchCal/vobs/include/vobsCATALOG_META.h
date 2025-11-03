#ifndef vobsCATALOG_META_H
#define vobsCATALOG_META_H
/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * @file
 * vobsCATALOG_META_H class declaration.
 */

#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif

/*
 * System Headers
 */
#include <map>
#include <vector>
#include <stdio.h>

/*
 * Local headers
 */
#include "alx.h"
#include "misco.h"
#include "vobsSTAR.h"
#include "vobsSTAR_PROPERTY_META.h"
#include "vobsCATALOG_COLUMN.h"

/** epoch 2000 */
#define EPOCH_2000 2000.0
#define JD_2000 2451545.0 // mjd = 51544

/** epoch 1991.25 (HIP / TYCHO) */
#define EPOCH_HIP 1991.25

/** default precision for catalogs ~ 1.0 as */
#define PRECISION_DEF 1.0

/** ReplaceStringInPlace implemented in vobsSCENARIO.cpp */
void ReplaceStringInPlace(std::string& subject, const std::string& search, const std::string& replace);

/*
 * Type declaration
 */

/*
 * const char* comparator used by map<const char*, ...> defined in vobsSTAR.h
 */
struct constStringComparator;

/** Catalog column pointer map keyed by column id using char* keys and custom comparator functor */
typedef std::map<const char*, vobsCATALOG_COLUMN*, constStringComparator> vobsCATALOG_COLUMN_PTR_MAP;

/** Catalog id / Catalog column pointer pair */
typedef std::pair<const char*, vobsCATALOG_COLUMN*> vobsCATALOG_COLUMN_PAIR;

/** Catalog column pointer list */
typedef std::vector<vobsCATALOG_COLUMN*> vobsCATALOG_COLUMN_PTR_LIST;

/*
 * Class declaration
 */

/**
 * Catalog meta data.
 *
 * The vobsCATALOG_META contains catalog meta data (epoch, astrometric precision ...)
 *
 */
class vobsCATALOG_META
{
public:
    // Class constructor

    /**
     * Build a catalog meta.
     */
    vobsCATALOG_META(const char* id,
                     vobsORIGIN_INDEX catalogId,
                     const mcsDOUBLE precision = PRECISION_DEF,
                     const vobsSTAR_MATCH_MODE matchMode = vobsSTAR_MATCH_BEST,
                     const mcsDOUBLE epochFrom = EPOCH_2000,
                     const mcsDOUBLE epochTo = EPOCH_2000,
                     const mcsLOGICAL hasProperMotion = mcsFALSE,
                     const mcsLOGICAL multipleRows = mcsFALSE,
                     const vobsSTAR_PROPERTY_MASK* overwritePropertyMask = NULL,
                     const char* queryOption = NULL,
                     const mcsLOGICAL sortByDistance = mcsTRUE)

    {
        // Set ID
        _id = id;

        // Set name
        _catalogId = catalogId;

        // astrometric precision of the catalog
        _precision = precision;

        // best or all matches
        _matchMode = matchMode;

        // Epoch (from) of the catalog
        _epochFrom = epochFrom;

        // Epoch (to) of the catalog
        _epochTo = epochTo;

        if (epochTo == epochFrom)
        {
            _singleEpoch = mcsTRUE;
            _epochMed = _epochFrom;
        }
        else
        {
            _singleEpoch = mcsFALSE;
            // Epoch (median) of the catalog
            _epochMed = 0.5 * (_epochFrom + _epochTo);
        }

        // flag to indicate if the catalog contains proper motion
        _hasProperMotion = hasProperMotion;

        // flag to indicate if the catalog can provide mutiple results at same coordinates (different observations or multiple rows)
        _hasMultipleRows = multipleRows;

        // list of star properties that this catalog can overwrite
        _overwritePropertyMask = overwritePropertyMask;

        // query option
        _queryOption = queryOption;

        // sort by distance:
        _sortByDistance = sortByDistance;
    }

    // Class destructor

    ~vobsCATALOG_META()
    {
        if (IS_NOT_NULL(_overwritePropertyMask))
        {
            delete(_overwritePropertyMask);
        }
        _columnMap.clear();
        for (vobsCATALOG_COLUMN_PTR_LIST::iterator iter = _columnList.begin(); iter != _columnList.end(); iter++)
        {
            delete(*iter);
        }
        _columnList.clear();
    }

    /**
     * Get the catalog ID as string literal
     *
     * @return catalog ID or NULL if not set.
     */
    inline const char* GetId() const __attribute__ ((always_inline))
    {
        return _id;
    }

    /**
     * Return the catalog id as origin index
     */
    inline vobsORIGIN_INDEX GetCatalogId() const __attribute__ ((always_inline))
    {
        return _catalogId;
    }

    /**
     * Return the flag to indicate if _epochFrom = _epochTo
     *
     * @return flag to indicate if _epochFrom = _epochTo
     */
    inline mcsDOUBLE GetPrecision() const __attribute__ ((always_inline))
    {
        return _precision;
    }

    /**
     * Return the match mode: best or all matches
     *
     * @return vobsSTAR_MATCH_MODE
     */
    inline vobsSTAR_MATCH_MODE GetMatchMode() const __attribute__ ((always_inline))
    {
        return _matchMode;
    }

    /**
     * Get the catalog name as string literal
     *
     * @return catalog name or NULL if not set.
     */
    inline const char* GetName() const __attribute__ ((always_inline))
    {
        return vobsGetOriginIndex(_catalogId);
    }

    /**
     * Get the Epoch delta of the catalog
     *
     * @return Epoch delta of the catalog
     */
    inline mcsDOUBLE GetEpochDelta() const __attribute__ ((always_inline))
    {
        return _epochTo - _epochFrom;
    }

    /**
     * Get the Epoch (from) of the catalog
     *
     * @return Epoch (from) of the catalog
     */
    inline mcsDOUBLE GetEpochFrom() const __attribute__ ((always_inline))
    {
        return _epochFrom;
    }

    /**
     * Get the Epoch (to) of the catalog
     *
     * @return Epoch (to) of the catalog
     */
    inline mcsDOUBLE GetEpochTo() const __attribute__ ((always_inline))
    {
        return _epochTo;
    }

    /**
     * Get the Epoch (median) of the catalog
     *
     * @return Epoch (median) of the catalog
     */
    inline mcsDOUBLE GetEpochMedian() const __attribute__ ((always_inline))
    {
        return _epochMed;
    }

    /**
     * Return the flag to indicate if _epochFrom = _epochTo
     *
     * @return flag to indicate if _epochFrom = _epochTo
     */
    inline mcsLOGICAL IsSingleEpoch() const __attribute__ ((always_inline))
    {
        return _singleEpoch;
    }

    /**
     * Return mcsTRUE only if the catalog's epochs are equals to 2000.0
     * @return mcsTRUE only if the catalog's epochs are equals to 2000.0
     */
    inline mcsLOGICAL IsEpoch2000() const __attribute__ ((always_inline))
    {
        return (IS_TRUE(_singleEpoch) && (_epochMed == EPOCH_2000)) ? mcsTRUE : mcsFALSE;
    }

    /**
     * Return mcsTRUE only if the catalog contains proper motion
     * @return mcsTRUE only if the catalog contains proper motion
     */
    inline mcsLOGICAL HasProperMotion() const __attribute__ ((always_inline))
    {
        return _hasProperMotion;
    }

    /**
     * Return mcsTRUE only if the catalog can provide mutiple results
     * @return mcsTRUE only if the catalog can provide mutiple results
     */
    inline mcsLOGICAL HasMultipleRows() const __attribute__ ((always_inline))
    {
        return _hasMultipleRows;
    }

    /**
     * Return mcsTRUE if the catalog does not contain proper motion and the catalog's epochs are NOT equals to 2000.0
     * @return mcsTRUE only if the catalog can provide mutiple results
     */
    inline mcsLOGICAL DoPrecessEpoch() const __attribute__ ((always_inline))
    {
        return (IS_FALSE(_hasProperMotion) && IS_FALSE(IsEpoch2000())) ? mcsTRUE : mcsFALSE;
    }

    /**
     * Return the optional overwrite star property mask that this catalog can overwrite
     * @return optional overwrite star property mask that this catalog can overwrite
     */
    inline const vobsSTAR_PROPERTY_MASK* GetOverwritePropertyMask() const __attribute__ ((always_inline))
    {
        return _overwritePropertyMask;
    }

    /**
     * Get the query option as string literal
     *
     * @return query option or "" if not set.
     */
    inline const char* GetQueryOption() const __attribute__ ((always_inline))
    {
        if (IS_NULL(_queryOption))
        {
            return "";
        }
        return _queryOption;
    }

    /**
     * Return mcsTRUE to sort query results by distance
     * @return mcsTRUE to sort query results by distance
     */
    inline mcsLOGICAL DoSortByDistance() const __attribute__ ((always_inline))
    {
        return _sortByDistance;
    }

    /**
     * Return the catalog columns
     * @return catalog columns
     */
    inline const vobsCATALOG_COLUMN_PTR_LIST& GetColumnList(void) const __attribute__ ((always_inline))
    {
        return _columnList;
    }

    /**
     * Add a new column meta data (id, ucd, property id)
     * @param id column ID
     * @param ucd column UCD
     * @param propertyId associated star property ID
     */
    inline void AddColumnMeta(const char* id, const char* ucd, const char* propertyId) __attribute__ ((always_inline))
    {
        vobsCATALOG_COLUMN* column = new vobsCATALOG_COLUMN(id, ucd, propertyId);
        _columnList.push_back(column);
        _columnMap.insert(vobsCATALOG_COLUMN_PAIR(id, column));
    }

    /**
     * Find the column meta data for the given column ID
     * @param id column ID
     * @return column meta data or NULL if not found in the Catalog Column map
     */
    inline vobsCATALOG_COLUMN* GetColumnMeta(char* id) const __attribute__ ((always_inline))
    {
        // Look for column meta:
        vobsCATALOG_COLUMN_PTR_MAP::const_iterator iter = _columnMap.find(id);

        // If no catalog meta with the given Id was found
        if (iter == _columnMap.end())
        {
            return NULL;
        }

        return iter->second;
    }

    /**
     * Dump the catalog meta into given buffer
     *
     * @param buffer buffer to append into
     *
     * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned
     */
    mcsCOMPL_STAT DumpCatalogMetaAsXML(miscoDYN_BUF& buffer) const
    {
        mcsSTRING32 tmp;

        FAIL(buffer.AppendString("\n  <catalog>\n"));

        FAIL(buffer.AppendString("    <catalogId>"));
        sprintf(tmp, "%d", _catalogId);
        FAIL(buffer.AppendString(tmp));
        FAIL(buffer.AppendString("</catalogId>\n"));

        FAIL(buffer.AppendString("    <define>vobsCATALOG_"));
        FAIL(buffer.AppendString(_id));
        FAIL(buffer.AppendString("_ID</define>\n"));

        FAIL(buffer.AppendString("    <id>"));
        FAIL(buffer.AppendString(_id));
        FAIL(buffer.AppendString("</id>\n"));

        FAIL(buffer.AppendString("    <name>"));
        FAIL(buffer.AppendString(GetName()));
        FAIL(buffer.AppendString("</name>\n"));

        FAIL(buffer.AppendString("    <matchMode>"));
        FAIL(buffer.AppendString((_matchMode == vobsSTAR_MATCH_BEST) ? "best" : "all"));
        FAIL(buffer.AppendString("</matchMode>\n"));

        // precision is unused:
        FAIL(buffer.AppendString("    <precision>"));
        sprintf(tmp, "%.3lf", _precision);
        FAIL(buffer.AppendString(tmp));
        FAIL(buffer.AppendString("</precision><!-- arcsec -->\n"));

        FAIL(buffer.AppendString("    <epochFrom>"));
        sprintf(tmp, "%.3lf", _epochFrom);
        FAIL(buffer.AppendString(tmp));
        FAIL(buffer.AppendString("</epochFrom>\n"));

        FAIL(buffer.AppendString("    <epochTo>"));
        sprintf(tmp, "%.3lf", _epochTo);
        FAIL(buffer.AppendString(tmp));
        FAIL(buffer.AppendString("</epochTo>\n"));

        FAIL(buffer.AppendString("    <epochMed>"));
        sprintf(tmp, "%.3lf", _epochMed);
        FAIL(buffer.AppendString(tmp));
        FAIL(buffer.AppendString("</epochMed>\n"));

        if (_hasProperMotion)
        {
            FAIL(buffer.AppendString("    <properMotion>"));
            FAIL(buffer.AppendString((_hasProperMotion) ? "true" : "false"));
            FAIL(buffer.AppendString("</properMotion>\n"));
        }

        if (_hasMultipleRows)
        {
            FAIL(buffer.AppendString("    <multipleRows>"));
            FAIL(buffer.AppendString((_hasMultipleRows) ? "true" : "false"));
            FAIL(buffer.AppendString("</multipleRows>\n"));
        }

        if (IS_NOT_NULL(_queryOption))
        {
            FAIL(buffer.AppendString("    <queryOption>"));
            // encode & char by &amp;
            std::string s = std::string(_queryOption);
            ReplaceStringInPlace(s, "&", "&amp;");
            FAIL(buffer.AppendString(s.c_str()));
            FAIL(buffer.AppendString("</queryOption>\n"));
        }

        if (_sortByDistance)
        {
            FAIL(buffer.AppendString("    <sortByDistance>"));
            FAIL(buffer.AppendString((_sortByDistance) ? "true" : "false"));
            FAIL(buffer.AppendString("</sortByDistance>\n"));
        }

        if (IS_NOT_NULL(_overwritePropertyMask))
        {
            FAIL(buffer.AppendString("    <overwritePropertyMask>"));

            const vobsSTAR_PROPERTY_META* propMeta;
            for (mcsUINT32 idx = 0; idx < _overwritePropertyMask->size(); idx++)
            {
                if ((*_overwritePropertyMask)[idx])
                {
                    propMeta = vobsSTAR_PROPERTY_META::GetPropertyMeta(idx);
                    
                    if (IS_NOT_NULL(propMeta))
                    {
                        // short mode:
                        propMeta->DumpAsXML(buffer, "vobsSTAR", idx, false);
                    }
                }
            }
            FAIL(buffer.AppendString("    </overwritePropertyMask>\n"));
        }

        FAIL(buffer.AppendString("    <columns>"));
        for (vobsCATALOG_COLUMN_PTR_LIST::const_iterator iter = _columnList.begin(); iter != _columnList.end(); iter++)
        {
            (*iter)->DumpCatalogColumnAsXML(buffer);
        }
        FAIL(buffer.AppendString("    </columns>\n"));

        FAIL(buffer.AppendString("  </catalog>\n"));

        return mcsSUCCESS;
    }


private:
    // Declaration of copy constructor and assignment operator as private
    // methods, in order to hide them from the users.
    vobsCATALOG_META(const vobsCATALOG_META& meta);
    vobsCATALOG_META& operator=(const vobsCATALOG_META& meta);

    // metadata members (constant):

    // ID of the catalog
    const char* _id;

    // Name of the catalog
    vobsORIGIN_INDEX _catalogId;

    // astrometric precision of the catalog
    mcsDOUBLE _precision;

    // best or all matches
    vobsSTAR_MATCH_MODE _matchMode;

    // flag to indicate if _epochFrom = _epochTo
    mcsLOGICAL _singleEpoch;

    // Epoch (from) of the catalog
    mcsDOUBLE _epochFrom;
    // Epoch (to) of the catalog
    mcsDOUBLE _epochTo;

    // Epoch (median) of the catalog
    mcsDOUBLE _epochMed;

    // flag to indicate if the catalog contains proper motion
    mcsLOGICAL _hasProperMotion;

    // flag to indicate if the catalog can provide mutiple results at same coordinates (different observations or multiple rows)
    mcsLOGICAL _hasMultipleRows;

    // optional overwrite star property mask
    const vobsSTAR_PROPERTY_MASK* _overwritePropertyMask;

    // options for the query string:
    const char* _queryOption;

    // flag to sort query results by distance (true by default)
    mcsLOGICAL _sortByDistance;

    // Catalog Column mappings:
    vobsCATALOG_COLUMN_PTR_LIST _columnList;
    vobsCATALOG_COLUMN_PTR_MAP  _columnMap;
} ;

#endif /*!vobsCATALOG_META_H*/

/*___oOo___*/
