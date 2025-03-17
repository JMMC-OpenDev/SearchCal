#ifndef vobsSTAR_PROPERTY_H
#define vobsSTAR_PROPERTY_H
/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * @file
 * vobsSTAR_PROPERTY class declaration.
 */

#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif

/*
 * System Headers
 */
#include <map>
#include <math.h>

/*
 * Local headers
 */
#include "vobsSTAR_PROPERTY_META.h"



/*
 * Type declaration
 */
/** undefined property / metadata index */
#define UNDEF_PROX_IDX 255

/**
 * Storage type contains the following layout:
 * bit 0-1 : vobsPROPERTY_STORAGE (3 values)
 * bit 2   : set flag
 * bit 3   : varchar flag (char* needed ie larger than char[8])
 */

/** bit 0-1: mask for vobsPROPERTY_STORAGE */
#define TYPE_MASK               (1 + 2)
/** bit 2: set flag */
#define FLAG_SET                4
/** bit 3: varchar flag */
#define FLAG_VARCHAR_BIT        8
/** bit 4: varchar growing flag */
#define FLAG_VARCHAR_GROW_BIT   16

inline static int alignSize(const int len, const int lg2)
{
    return (((len >> lg2) + 1) << lg2);
}

/*
 * const char* comparator used by map<const char*, ...>
 */
struct constStarPropertyMetaComparator
{

    bool operator()(const vobsSTAR_PROPERTY_META* meta1, const vobsSTAR_PROPERTY_META * meta2) const
    {
        return (meta1 == meta2) ? false : strcmp(meta1->GetId(), meta2->GetId()) < 0;
    }
} ;

/* Star property meta pointer / Catalog ID mapping */
typedef std::multimap<const vobsSTAR_PROPERTY_META*, const char*, constStarPropertyMetaComparator> vobsCATALOG_STAR_PROPERTY_CATALOG_MAPPING;

/* Star property meta pointer / Catalog ID pair */
typedef std::pair<const vobsSTAR_PROPERTY_META*, const char*> vobsCATALOG_STAR_PROPERTY_CATALOG_PAIR;

/**
 * Confidence index (4 values iso needs only 2 bits = 1 byte)
 */
typedef enum
{
    vobsCONFIDENCE_NO     = 0, /** No confidence              */
    vobsCONFIDENCE_LOW    = 1, /** Low confidence             */
    vobsCONFIDENCE_MEDIUM = 2, /** Medium confidence          */
    vobsCONFIDENCE_HIGH   = 3, /** High confidence            */
    vobsNB_CONFIDENCE_INDEX    /** number of Confidence index */
}
vobsCONFIDENCE_INDEX;

/* confidence index as label string mapping */
static const char* const vobsCONFIDENCE_STR[] = { "NO", "LOW", "MEDIUM", "HIGH" };

/* confidence index as integer string mapping */
static const char* const vobsCONFIDENCE_INT[] = { "0", "1", "2", "3" };

/**
 * Return the string literal representing the confidence index
 * @return string literal "LOW", "MEDIUM" or "HIGH"
 */
const char* vobsGetConfidenceIndex(const vobsCONFIDENCE_INDEX confIndex);

/**
 * Return the integer literal representing the confidence index
 * @return integer literal "1" (LOW), "2" (MEDIUM) or "3" (HIGH)
 */
const char* vobsGetConfidenceIndexAsInt(const vobsCONFIDENCE_INDEX confIndex);

/**
 * Storage type
 */
typedef enum
{
    vobsPROPERTY_STORAGE_FLOAT2    = 0,     /** numeric = (value/error)   */
    vobsPROPERTY_STORAGE_LONG      = 1,     /** numeric = (bool/int/long) */
    vobsPROPERTY_STORAGE_STRING    = 2,     /** string */
} vobsPROPERTY_STORAGE;

#define IS_NUM(type) \
    ((type) < vobsPROPERTY_STORAGE_STRING)
#define IS_FLOAT2(type) \
    ((type) == vobsPROPERTY_STORAGE_FLOAT2)


/* vobsSTAR_PROPERTY views declared as various structs sharing the same layout except the internal value (bool/int/long/float*2/char*) */

/* long storage */
typedef struct mcsATTRS_LOWMEM
{
    mcsINT32    header;     /* vobsSTAR_PROPERTY header[_metaIdx|_confidenceIndex|_originIndex|_storageType] */
    mcsINT64    longValue;  /* value as long (bool/int/long) */
}
vobsSTAR_PROPERTY_VIEW_LONG;

/* float*2 storage */
typedef struct mcsATTRS_LOWMEM
{
    mcsINT32    header;     /* vobsSTAR_PROPERTY header[_metaIdx|_confidenceIndex|_originIndex|_storageType] */
    mcsFLOAT    value;      /* value as float (float) */
    mcsFLOAT    error;      /* error as float (float) */
}
vobsSTAR_PROPERTY_VIEW_FLOAT2;

/* char[8] storage */
typedef struct mcsATTRS_LOWMEM
{
    mcsINT32    header;     /* vobsSTAR_PROPERTY header[_metaIdx|_confidenceIndex|_originIndex|_storageType] */
    mcsSTRING8  charValue;  /* value as char[8] (str) */
}
vobsSTAR_PROPERTY_VIEW_CHAR8;

/* char* storage */
typedef struct mcsATTRS_LOWMEM
{
    mcsINT32    header;     /* vobsSTAR_PROPERTY header[_metaIdx|_confidenceIndex|_originIndex|_storageType] */
    char*       strValue;   /* value as char* (str) */
}
vobsSTAR_PROPERTY_VIEW_VARCHAR;


/*
 * Class declaration
 */

/**
 * Star property.
 *
 * The vobsSTAR_PROPERTY ...
 *
 */
class vobsSTAR_PROPERTY
{
public:
    // Constructors
    vobsSTAR_PROPERTY();
    vobsSTAR_PROPERTY(mcsUINT8 metaIdx);

    explicit vobsSTAR_PROPERTY(const vobsSTAR_PROPERTY&);

    vobsSTAR_PROPERTY& operator=(const vobsSTAR_PROPERTY&) ;

    // Class destructor
    ~vobsSTAR_PROPERTY();

    void SetMetaIndex(mcsUINT8 metaIdx);

    // Property value setters
    mcsCOMPL_STAT SetValue(const char* value,
                           vobsORIGIN_INDEX originIndex,
                           vobsCONFIDENCE_INDEX confidenceIndex = vobsCONFIDENCE_HIGH,
                           mcsLOGICAL overwrite = mcsFALSE);

    mcsCOMPL_STAT SetValue(mcsDOUBLE value,
                           vobsORIGIN_INDEX originIndex,
                           vobsCONFIDENCE_INDEX confidenceIndex = vobsCONFIDENCE_HIGH,
                           mcsLOGICAL overwrite = mcsFALSE);

    mcsCOMPL_STAT SetValue(mcsINT64 value,
                           vobsORIGIN_INDEX originIndex,
                           vobsCONFIDENCE_INDEX confidenceIndex = vobsCONFIDENCE_HIGH,
                           mcsLOGICAL overwrite = mcsFALSE);

    mcsCOMPL_STAT SetValue(mcsINT32 value,
                           vobsORIGIN_INDEX originIndex,
                           vobsCONFIDENCE_INDEX confidenceIndex = vobsCONFIDENCE_HIGH,
                           mcsLOGICAL overwrite = mcsFALSE) {
         return SetValue((mcsINT64)value, originIndex, confidenceIndex, overwrite);
    }

    // Property error setters
    mcsCOMPL_STAT SetError(const char* error,
                           mcsLOGICAL overwrite = mcsFALSE);

    void SetError(mcsDOUBLE  error,
                  mcsLOGICAL overwrite = mcsFALSE);

    /**
     * Clear property value
     *
     * @return mcsSUCCESS
     */
    inline void ClearValue() __attribute__ ((always_inline))
    {
        SetConfidenceIndex(vobsCONFIDENCE_NO);
        SetOriginIndex(vobsORIG_NONE);
        ClearStorageValue();
    }

    /**
     * Get value as a string or NULL if not set or not a string property
     *
     * @return value as a string or NULL
     */
    inline const char* GetValue() const __attribute__ ((always_inline))
    {
        return (!IsFlagSet() || IS_NUM(GetStorageType())) ? NULL
                : IsFlagVarChar() ? (const char*) (viewAsStrVar()->strValue)
                : (const char*) (viewAsStr8()->charValue);
    }

    /**
     * Get value as a string or empty string ("") if not set or not a string property
     *
     * @return value as a string or empty string ("")
     */
    inline const char* GetValueOrBlank() const __attribute__ ((always_inline))
    {
        if (!IsFlagSet() || IS_NUM(GetStorageType()))
        {
            return "";
        }
        const char* value = IsFlagVarChar() ? (const char*) (viewAsStrVar()->strValue)
                : (const char*) (viewAsStr8()->charValue);

        if (IS_NULL(value))
        {
            return "";
        }
        return value;
    }

    /**
     * Get numerical value as a string or "" if not set or not a numerical property
     *
     * @param converted numerical value as a string or NULL
     * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
     */
    mcsCOMPL_STAT GetFormattedValue(mcsSTRING32* converted) const;

    /**
     * Get error as a string or "" if not set or not a numerical property
     *
     * @param converted error as a string or NULL
     * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
     */
    mcsCOMPL_STAT GetFormattedError(mcsSTRING32* converted) const;

    /**
     * Get value as a double.
     *
     * @param value pointer to store value.
     *
     * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
     */
    mcsCOMPL_STAT GetValue(mcsDOUBLE *value) const;

    /**
     * Get value as an integer.
     *
     * @param value pointer to store value.
     *
     * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
     */
    mcsCOMPL_STAT GetValue(mcsINT32 *value) const;

    /**
     * Get value as an long.
     *
     * @param value pointer to store value.
     *
     * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
     */
    mcsCOMPL_STAT GetValue(mcsINT64 *value) const;

    /**
     * Get value as a boolean.
     *
     * @param value pointer to store value.
     *
     * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
     */
    mcsCOMPL_STAT GetValue(mcsLOGICAL *value) const;

    /**
     * Return mcsTRUE if this boolean property is set and equals mcsTRUE
     * @return true if this boolean property is set and equals mcsTRUE; false otherwise
     */
    bool IsTrue() const
    {
        mcsLOGICAL flag;
        return ((GetValue(&flag) == mcsSUCCESS) && IS_TRUE(flag));
    }

    inline void SetOriginIndex(vobsORIGIN_INDEX originIndex) __attribute__ ((always_inline))
    {
        _originIndex = (mcsUINT8) originIndex;
    }

    /**
     * Get the origin index
     *
     * @return origin index
     */
    inline vobsORIGIN_INDEX GetOriginIndex() const __attribute__ ((always_inline))
    {
        return (vobsORIGIN_INDEX) _originIndex;
    }

    /**
     * Get value of the confidence index
     *
     * @return value of confidence index
     */
    inline void SetConfidenceIndex(vobsCONFIDENCE_INDEX confidenceIndex) __attribute__ ((always_inline))
    {
        _confidenceIndex = (mcsUINT8) confidenceIndex;
    }

    /**
     * Get value of the confidence index
     *
     * @return value of confidence index
     */
    inline vobsCONFIDENCE_INDEX GetConfidenceIndex() const __attribute__ ((always_inline))
    {
        return (vobsCONFIDENCE_INDEX) _confidenceIndex;
    }

    /**
     * Check whether the property is computed or not.
     *
     * @return mcsTRUE if the the property has been computed, mcsFALSE otherwise.
     */
    inline mcsLOGICAL IsComputed() const __attribute__ ((always_inline))
    {
        // Check whether property has been computed or not
        return isPropComputed(GetOriginIndex()) ? mcsTRUE : mcsFALSE;
    }

    /**
     * Check whether the property is set or not.
     *
     * @return mcsTRUE if the the property has been set, mcsFALSE otherwise.
     */
    inline mcsLOGICAL IsSet() const __attribute__ ((always_inline))
    {
        return IsFlagSet() ? mcsTRUE : mcsFALSE;
    }

    /**
     * Check whether the error is set or not.
     *
     * @return mcsTRUE if the the error has been set, mcsFALSE otherwise.
     */
    inline mcsLOGICAL IsErrorSet() const __attribute__ ((always_inline))
    {
        // Check if the error is not NaN
        return (!IS_FLOAT2(GetStorageType()) || !IsFlagSet() || isnan(viewAsFloat2()->error)) ? mcsFALSE : mcsTRUE;
    }

    /**
     * Get error as a double.
     *
     * @param error pointer to store value.
     *
     * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
     */
    mcsCOMPL_STAT GetError(mcsDOUBLE *error) const;

    /**
     * Get property meta data.
     *
     * @return property meta data
     */
    inline mcsUINT8 GetMetaIdx() const __attribute__ ((always_inline))
    {
        return _metaIdx;
    }

    /**
     * Get property meta data.
     *
     * @return property meta data
     */
    inline const vobsSTAR_PROPERTY_META* GetMeta() const __attribute__ ((always_inline))
    {
        return vobsSTAR_PROPERTY_META::GetPropertyMeta(_metaIdx);
    }

    /**
     * Get property id.
     *
     * @return property id
     */
    inline const char* GetId() const __attribute__ ((always_inline))
    {
        const vobsSTAR_PROPERTY_META* meta = GetMeta();
        if (IS_NULL(meta))
        {
            return "";
        }
        return meta->GetId();
    }

    /**
     * Get property name.
     *
     * @return property name
     */
    inline const char* GetName() const __attribute__ ((always_inline))
    {
        const vobsSTAR_PROPERTY_META* meta = GetMeta();
        if (IS_NULL(meta))
        {
            return "";
        }
        return meta->GetName();
    }

    /**
     * Get property type.
     *
     * @return property type
     */
    inline vobsPROPERTY_TYPE GetType() const __attribute__ ((always_inline))
    {
        const vobsSTAR_PROPERTY_META* meta = GetMeta();
        if (IS_NULL(meta))
        {
            return vobsUNDEFINED_PROPERTY;
        }
        return meta->GetType();
    }

    /**
     * Get property unit.
     *
     * @sa http://vizier.u-strasbg.fr/doc/catstd-3.2.htx
     *
     * @return property unit if present, NULL otherwise.
     */
    inline const char* GetUnit() const __attribute__ ((always_inline))
    {
        const vobsSTAR_PROPERTY_META* meta = GetMeta();
        if (IS_NULL(meta))
        {
            return "";
        }
        return meta->GetUnit();
    }

    /**
     * Get property description.
     *
     * @sa http://vizier.u-strasbg.fr/doc/catstd-3.2.htx
     *
     * @return property description if present, NULL otherwise.
     */
    inline const char* GetDescription() const __attribute__ ((always_inline))
    {
        const vobsSTAR_PROPERTY_META* meta = GetMeta();
        if (IS_NULL(meta))
        {
            return "";
        }
        return meta->GetDescription();
    }

    /**
     * Get property CDS link.
     *
     * @return property CDS link if present, NULL otherwise.
     */
    inline const char* GetLink() const __attribute__ ((always_inline))
    {
        const vobsSTAR_PROPERTY_META* meta = GetMeta();
        if (IS_NULL(meta))
        {
            return "";
        }
        return meta->GetLink();
    }

    /**
     * Get property error meta data.
     *
     * @return property error meta data or NULL
     */
    inline const vobsSTAR_PROPERTY_META* GetErrorMeta() const __attribute__ ((always_inline))
    {
        const vobsSTAR_PROPERTY_META* meta = GetMeta();
        if (IS_NULL(meta))
        {
            return NULL;
        }
        return meta->GetErrorMeta();
    }

    /**
     * Get property error id.
     *
     * @return property error id or "" if undefined property error meta data
     */
    inline const char* GetErrorId() const __attribute__ ((always_inline))
    {
        const vobsSTAR_PROPERTY_META* errorMeta = GetErrorMeta();
        if (IS_NULL(errorMeta))
        {
            return "";
        }
        return errorMeta->GetId();
    }

    /**
     * Get property error name.
     *
     * @return property error name or "" if undefined property error meta data
     */
    inline const char* GetErrorName() const __attribute__ ((always_inline))
    {
        const vobsSTAR_PROPERTY_META* errorMeta = GetErrorMeta();
        if (IS_NULL(errorMeta))
        {
            return "";
        }
        return errorMeta->GetName();
    }

    // Get the object summary as a string, including all its member's values
    const std::string GetSummaryString() const;

    void SetFlagVarCharGrow()
    {
        SetFlagVarCharGrow(true);
    }

private:

    inline vobsPROPERTY_STORAGE GetStorageType() const __attribute__ ((always_inline))
    {
        /* first 2-bits are enough for vobsPROPERTY_TYPE */
        return (vobsPROPERTY_STORAGE) (_storageType & TYPE_MASK);
    }

    inline void SetStorageType(vobsPROPERTY_TYPE type) __attribute__ ((always_inline))
    {
        SetStorageType(IsPropString(type) ? vobsPROPERTY_STORAGE_STRING
                       : (IsPropFloat(type)) ? vobsPROPERTY_STORAGE_FLOAT2
                       : vobsPROPERTY_STORAGE_LONG, false, false, false);
    }

    inline void SetStorageType(vobsPROPERTY_STORAGE type,
                               const bool set,
                               const bool varchar,
                               const bool grow) __attribute__ ((always_inline))
    {
        // set type and reset flags:
        _storageType = type;
        if (set)
        {
            _storageType |= FLAG_SET;
        }
        if (varchar)
        {
            _storageType |= FLAG_VARCHAR_BIT;
        }
        if (grow)
        {
            _storageType |= FLAG_VARCHAR_GROW_BIT;
        }
    }

    inline bool IsFlagSet() const __attribute__ ((always_inline))
    {
        return ((_storageType & FLAG_SET) != 0);
    }

    // note: varchar=true implies set=true

    inline bool IsFlagVarChar() const __attribute__ ((always_inline))
    {
        return ((_storageType & FLAG_VARCHAR_BIT) != 0);
    }

    inline bool IsFlagVarCharGrow() const __attribute__ ((always_inline))
    {
        return ((_storageType & FLAG_VARCHAR_GROW_BIT) != 0);
    }

    inline void SetFlagSet(const bool set) __attribute__ ((always_inline))
    {
        SetStorageType(GetStorageType(), set, IsFlagVarChar(), IsFlagVarCharGrow());
    }

    inline void SetFlagVarChar(const bool varchar) __attribute__ ((always_inline))
    {
        SetStorageType(GetStorageType(), IsFlagSet(), varchar, IsFlagVarCharGrow());
    }

    inline void SetFlagVarCharGrow(const bool grow) __attribute__ ((always_inline))
    {
        SetStorageType(GetStorageType(), IsFlagSet(), IsFlagVarChar(), grow);
    }

    inline void ClearStorageValue() __attribute__ ((always_inline))
    {
        vobsSTAR_PROPERTY_VIEW_FLOAT2* editValErr;
        vobsSTAR_PROPERTY_VIEW_LONG* editLong;
        vobsSTAR_PROPERTY_VIEW_CHAR8* editStr8;
        vobsSTAR_PROPERTY_VIEW_VARCHAR* editStrVar;

        switch (GetStorageType())
        {
            case vobsPROPERTY_STORAGE_FLOAT2:
                editValErr = editAsFloat2();
                editValErr->value = NAN;
                editValErr->error = NAN;
                break;
            case vobsPROPERTY_STORAGE_LONG:
                editLong = editAsLong();
                editLong->longValue = 0L;
                break;
            case vobsPROPERTY_STORAGE_STRING:
                if (IsFlagVarChar())
                {
                    editStrVar = editAsStrVar();

                    if (IS_NOT_NULL(editStrVar->strValue))
                    {
                        delete[](editStrVar->strValue);
                    }
                    // anyway:
                    SetFlagVarChar(false);
                    SetFlagVarCharGrow(false);
                }
                // anyway:
                editStr8 = editAsStr8();
                editStr8->charValue[0] = '0';
                break;
            default:
                break;
        };
        // anyway:
        SetFlagSet(false);
    }

    void copyValue(const char* value);

    /**
     * Get property format.
     *
     * @return property format
     */
    inline const char* GetFormat(void) const __attribute__ ((always_inline))
    {
        // Return property format
        return GetMeta()->GetFormat();
    }

    /**
     * Get given value as a string or "" if not set or not a numerical property
     *
     * @param value input value to format
     * @param converted numerical value as a string or NULL
     * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
     */
    mcsCOMPL_STAT GetFormattedValue(mcsDOUBLE value, mcsSTRING32* converted) const;

    /**
     * Get given value as a string or "" if not set or not a numerical property
     *
     * @param value input value to format
     * @param converted numerical value as a string or NULL
     * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
     */
    mcsCOMPL_STAT GetFormattedValue(mcsINT64 value, mcsSTRING32* converted) const;

    /* easy get special views of vobsSTAR_PROPERTY */
    inline vobsSTAR_PROPERTY_VIEW_FLOAT2* viewAsFloat2() const __attribute__ ((always_inline))
    {
        return reinterpret_cast<vobsSTAR_PROPERTY_VIEW_FLOAT2*> (const_cast<vobsSTAR_PROPERTY*> (this));
    }

    inline vobsSTAR_PROPERTY_VIEW_FLOAT2* editAsFloat2() __attribute__ ((always_inline))
    {
        return reinterpret_cast<vobsSTAR_PROPERTY_VIEW_FLOAT2*> (this);
    }

    inline vobsSTAR_PROPERTY_VIEW_LONG* viewAsLong() const __attribute__ ((always_inline))
    {
        return reinterpret_cast<vobsSTAR_PROPERTY_VIEW_LONG*> (const_cast<vobsSTAR_PROPERTY*> (this));
    }

    inline vobsSTAR_PROPERTY_VIEW_LONG* editAsLong() __attribute__ ((always_inline))
    {
        return reinterpret_cast<vobsSTAR_PROPERTY_VIEW_LONG*> (this);
    }

    inline vobsSTAR_PROPERTY_VIEW_CHAR8* viewAsStr8() const __attribute__ ((always_inline))
    {
        return reinterpret_cast<vobsSTAR_PROPERTY_VIEW_CHAR8*> (const_cast<vobsSTAR_PROPERTY*> (this));
    }

    inline vobsSTAR_PROPERTY_VIEW_CHAR8* editAsStr8() __attribute__ ((always_inline))
    {
        return reinterpret_cast<vobsSTAR_PROPERTY_VIEW_CHAR8*> (this);
    }

    inline vobsSTAR_PROPERTY_VIEW_VARCHAR* viewAsStrVar() const __attribute__ ((always_inline))
    {
        return reinterpret_cast<vobsSTAR_PROPERTY_VIEW_VARCHAR*> (const_cast<vobsSTAR_PROPERTY*> (this));
    }

    inline vobsSTAR_PROPERTY_VIEW_VARCHAR* editAsStrVar() __attribute__ ((always_inline))
    {
        return reinterpret_cast<vobsSTAR_PROPERTY_VIEW_VARCHAR*> (this);
    }

    /* Memory footprint (sizeof) = 12 bytes (4-bytes alignment) */

    // value storage type
    mcsUINT8    _storageType;                   // 1 byte (vobsPROPERTY_STORAGE + flags)
    // metadata index:
    mcsUINT8    _metaIdx;                       // 1 byte (max 255 properties)

    // data:
    // Confidence index
    mcsUINT8    _confidenceIndex;               // 1 byte (vobsCONFIDENCE_INDEX)
    // Origin index
    mcsUINT8    _originIndex;                   // 1 byte (vobsORIGIN_INDEX)

    // Value stored as 64bits field (long) (field alignment on int32)
    mcsINT64    _opaqueStorage;                 // 8 bytes

    // 2025.02: 1+1+1+1+8   = 12 bytes (12 bytes aligned)

} mcsATTRS_LOWMEM;

/**
 * vobsSTAR_PROPERTY_STATS is a class which contains statistics fields for a stra property
 * 
 */
class vobsSTAR_PROPERTY_STATS
{
public:
    // Class constructor

    /**
     * Build a vobsSTAR_PROPERTY_STATS.
     */
    vobsSTAR_PROPERTY_STATS()
    {
        Reset();
    }

    // Class destructor

    ~vobsSTAR_PROPERTY_STATS()
    {
        Reset();
    }

    void Reset()
    {
        nSamples = 0L;
        min = INFINITY;
        max = -INFINITY;
        squaredError = 0.0;
        meanValue = 0.0;
        meanValueError = 0.0;
    }

    void Add(const mcsDOUBLE x)
    {
        if (isfinite(x))
        {
            if (x < min)
            {
                min = x;
            }
            if (x > max)
            {
                max = x;
            }
            nSamples++;

            const mcsDOUBLE deltaOldMean = (x - meanValue);

            /* kahan sum for mean = (deltaOldMean / nSamples) */
            const mcsDOUBLE y = (deltaOldMean / nSamples) - meanValueError;
            const mcsDOUBLE t = meanValue + y;
            meanValueError = (t - meanValue) - y;
            meanValue = t;
            // update squared error:
            squaredError += (x - meanValue) * deltaOldMean;
        }
    }

    mcsLOGICAL IsSet()
    {
        return (nSamples != 0L) ? mcsTRUE : mcsFALSE;
    }

    mcsUINT64 NSamples()
    {
        return nSamples;
    }

    mcsDOUBLE Min()
    {
        if (IsSet())
        {
            return min;
        }
        return NAN;
    }

    mcsDOUBLE Max()
    {
        if (IsSet())
        {
            return max;
        }
        return NAN;
    }

    mcsDOUBLE Mean()
    {
        if (IsSet())
        {
            return meanValue;
        }
        return NAN;
    }

    mcsDOUBLE Variance()
    {
        if (IsSet())
        {
            return squaredError / (nSamples - 1L);
        }
        return NAN;
    }

    mcsDOUBLE Stddev()
    {
        if (IsSet())
        {
            return sqrt(Variance());
        }
        return NAN;
    }

    mcsDOUBLE RawErrorPercent()
    {
        if (IsSet())
        {
            return (100.0 * Stddev() / Mean());
        }
        return NAN;
    }

    mcsDOUBLE Total()
    {
        if (IsSet())
        {
            return Mean() * nSamples;
        }
        return NAN;
    }

    mcsCOMPL_STAT AppendToString(miscoDYN_BUF &statBuf)
    {
        mcsSTRING64 tmp;
        snprintf(tmp, sizeof (tmp) - 1, "[%lu:", nSamples);
        FAIL(statBuf.AppendString(tmp));

        snprintf(tmp, sizeof (tmp) - 1, " µ=%.4lf", Mean());
        FAIL(statBuf.AppendString(tmp));

        if (nSamples > 1)
        {
            snprintf(tmp, sizeof (tmp) - 1, " σ=%.4lf", Stddev());
            FAIL(statBuf.AppendString(tmp));
            snprintf(tmp, sizeof (tmp) - 1, " (%.2lf %%)", RawErrorPercent());
            FAIL(statBuf.AppendString(tmp));
            snprintf(tmp, sizeof (tmp) - 1, " min=%.4lf", Min());
            FAIL(statBuf.AppendString(tmp));
            snprintf(tmp, sizeof (tmp) - 1, " max=%.4lf", Max());
            FAIL(statBuf.AppendString(tmp));
        }
        FAIL(statBuf.AppendString("]"));
        return mcsSUCCESS;
    }

    mcsUINT64 nSamples;
    mcsDOUBLE min;
    mcsDOUBLE max;
    mcsDOUBLE squaredError;
    mcsDOUBLE meanValue;
    mcsDOUBLE meanValueError;

} ;

#endif /*!vobsSTAR_PROPERTY_H*/

/*___oOo___*/
