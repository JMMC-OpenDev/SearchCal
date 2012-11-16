#ifndef vobsCATALOG_META_H
#define	vobsCATALOG_META_H
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
#include <list>
#include <vector>

/*
 * Local headers
 */
#include "alx.h"

/*
 * Type declaration
 */

/** Star property ID ordered list (UCD) as const char* */
typedef std::list<const char*> vobsSTAR_PROPERTY_ID_LIST;

/** Property mask (boolean vector) */
typedef std::vector<bool> vobsSTAR_PROPERTY_MASK;

#define vobsMIXED_CATALOG_ID        "MIXED CATALOG"

/** epoch 2000 */
#define EPOCH_2000 2000.0
#define JD_2000 2451545.0 // mjd = 51544

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
    vobsCATALOG_META(const char* name,
                     const mcsDOUBLE precision,
                     const mcsDOUBLE epochFrom,
                     const mcsDOUBLE epochTo,
                     const mcsLOGICAL hasProperMotion,
                     const mcsLOGICAL multipleRows,
                     const vobsSTAR_PROPERTY_MASK* overwritePropertyMask)
    {
        // Set name
        _name = name;

        // astrometric precision of the catalog
        _precision = precision * alxDEG_IN_ARCSEC;

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
    }

    // Class destructor

    ~vobsCATALOG_META()
    {
        if (_overwritePropertyMask != NULL)
        {
            delete(_overwritePropertyMask);
        }
    }

    /**
     * Get the catalog name as string literal
     *
     * @return catalog name or NULL if not set.
     */
    inline const char* GetName() const __attribute__((always_inline))
    {
        return _name;
    }

    /**
     * Get the Epoch delta of the catalog
     *
     * @return Epoch delta of the catalog
     */
    inline const mcsDOUBLE GetEpochDelta() const __attribute__((always_inline))
    {
        return _epochTo - _epochFrom;
    }

    /**
     * Get the Epoch (from) of the catalog
     *
     * @return Epoch (from) of the catalog
     */
    inline const mcsDOUBLE GetEpochFrom() const __attribute__((always_inline))
    {
        return _epochFrom;
    }

    /**
     * Get the Epoch (to) of the catalog
     *
     * @return Epoch (to) of the catalog
     */
    inline const mcsDOUBLE GetEpochTo() const __attribute__((always_inline))
    {
        return _epochTo;
    }

    /**
     * Get the Epoch (median) of the catalog
     *
     * @return Epoch (median) of the catalog
     */
    inline const mcsDOUBLE GetEpochMedian() const __attribute__((always_inline))
    {
        return _epochMed;
    }

    /**
     * Return the flag to indicate if _epochFrom = _epochTo
     *
     * @return flag to indicate if _epochFrom = _epochTo
     */
    inline const mcsLOGICAL IsSingleEpoch() const __attribute__((always_inline))
    {
        return _singleEpoch;
    }

    /**
     * Return mcsTRUE only if the catalog's epochs are equals to 2000.0
     * @return mcsTRUE only if the catalog's epochs are equals to 2000.0
     */
    inline const mcsLOGICAL IsEpoch2000() const __attribute__((always_inline))
    {
        return ((_singleEpoch == mcsTRUE) && (_epochMed == EPOCH_2000)) ? mcsTRUE : mcsFALSE;
    }

    /**
     * Return mcsTRUE only if the catalog contains proper motion
     * @return mcsTRUE only if the catalog contains proper motion
     */
    inline const mcsLOGICAL HasProperMotion() const __attribute__((always_inline))
    {
        return _hasProperMotion;
    }

    /**
     * Return mcsTRUE only if the catalog can provide mutiple results
     * @return mcsTRUE only if the catalog can provide mutiple results
     */
    inline const mcsLOGICAL HasMultipleRows() const __attribute__((always_inline))
    {
        return _hasMultipleRows;
    }

    /**
     * Return mcsTRUE if the catalog does not contain proper motion and the catalog's epochs are NOT equals to 2000.0
     * @return mcsTRUE only if the catalog can provide mutiple results
     */
    inline const mcsLOGICAL DoPrecessEpoch() const __attribute__((always_inline))
    {
        return ((_hasProperMotion == mcsFALSE) && (IsEpoch2000() == mcsFALSE)) ? mcsTRUE : mcsFALSE;
    }

    /**
     * Return the optional overwrite star property mask that this catalog can overwrite
     * @return optional overwrite star property mask that this catalog can overwrite
     */
    inline const vobsSTAR_PROPERTY_MASK* GetOverwritePropertyMask() const __attribute__((always_inline))
    {
        return _overwritePropertyMask;
    }


private:
    // Declaration of copy constructor and assignment operator as private
    // methods, in order to hide them from the users.
    vobsCATALOG_META(const vobsCATALOG_META& property);
    vobsCATALOG_META& operator=(const vobsCATALOG_META& meta);

    // metadata members (constant):

    // Name of the catalog
    const char* _name;

    // options for the query string:
    const char* _option;

    // astrometric precision of the catalog
    mcsDOUBLE _precision;

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
};

#endif /*!vobsCATALOG_META_H*/

/*___oOo___*/
