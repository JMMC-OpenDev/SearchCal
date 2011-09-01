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
 * Local headers
 */
#include "vobsSTAR_PROPERTY_META.h"


/*
 * Constant declaration
 */
#define vobsSTAR_COMPUTED_PROP "computed"  /**< Tag for computed properties */

/*
 * Type declaration
 */
/**
 * Confidence index.
 */
typedef enum
{
    vobsCONFIDENCE_LOW = 0, /* Low confidence in property value */
    vobsCONFIDENCE_MEDIUM,  /* Medium confidence in property value */
    vobsCONFIDENCE_HIGH     /* High confidence in property value */
} vobsCONFIDENCE_INDEX;


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
    // Class constructors
    vobsSTAR_PROPERTY(const vobsSTAR_PROPERTY_META* meta);

    vobsSTAR_PROPERTY(const vobsSTAR_PROPERTY&);

    vobsSTAR_PROPERTY& operator=(const vobsSTAR_PROPERTY&);

    // Class destructor
    ~vobsSTAR_PROPERTY();
    
    // Property value setting
    mcsCOMPL_STAT SetValue(const char *value,
                                   const char *origin,
                                   vobsCONFIDENCE_INDEX confidenceIndex=vobsCONFIDENCE_HIGH,
                                   mcsLOGICAL overwrite=mcsFALSE);
    
    mcsCOMPL_STAT SetValue(mcsDOUBLE value,
                                   const char *origin,
                                   vobsCONFIDENCE_INDEX confidenceIndex=vobsCONFIDENCE_HIGH,
                                   mcsLOGICAL overwrite=mcsFALSE);
    
    mcsCOMPL_STAT ClearValue(void);
    

    /**
     * Get value as a string.
     *
     * @return mcsSUCCESS
     */
    inline const char* GetValue(void) const
    {
        // Return property value
        if (_value == NULL)
        {
            return vobsSTAR_PROP_NOT_SET;
        }
        return _value;
    }

    /**
     * Get value as a double.
     */
    mcsCOMPL_STAT GetValue(mcsDOUBLE *value) const;

    /**
     * Get property origin
     *
     * @return property origin
     */
    inline const char* GetOrigin() const
    {
        return _origin;
    }

    /**
     * Get value of the confidence index
     *
     * @return value of confidence index
     */
    inline vobsCONFIDENCE_INDEX GetConfidenceIndex() const
    {
        return _confidenceIndex;
    }

    /**
     * Check whether the property is computed or not.  
     * 
     * @return mcsTRUE if the the property has been computed, mcsFALSE otherwise.
     */
    inline mcsLOGICAL IsComputed(void) const
    {
        // Check whether property has been computed or not
        if (strcmp(_origin, vobsSTAR_COMPUTED_PROP) == 0)
        {
            return mcsTRUE;
        }

        return mcsFALSE;
    }

    /**
     * Check whether the property is set or not.  
     * 
     * @return mcsTRUE if the the property has been set, mcsFALSE otherwise.
     */
    inline mcsLOGICAL IsSet(void) const
    {
        // Check if property string value is set to vobsSTAR_PROP_NOT_SET
        if (_value == NULL)
        {
            return mcsFALSE;
        }

        return mcsTRUE;
    }

    /**
     * Get property id.
     *
     * @return property id
     */
    inline const char* GetId(void) const
    {
        return _meta->GetId();
    }

    /**
     * Get property name.
     *
     * @return property name
     */
    inline const char* GetName(void) const
    {
        // Return property name
        return _meta->GetName();
    }

    /**
     * Get property type.
     *
     * @return property type
     */
    inline const vobsPROPERTY_TYPE GetType(void) const
    {
        // Return property type
        return _meta->GetType();
    }

    /**
     * Get property unit.
     *
     * @sa http://vizier.u-strasbg.fr/doc/catstd-3.2.htx
     *
     * @return property unit if present, vobsSTAR_PROP_NOT_SET otherwise.
     */
    inline const char* GetUnit(void) const
    {
        // Return property unit
        return _meta->GetUnit();
    }

    /**
     * Get property description.
     *
     * @sa http://vizier.u-strasbg.fr/doc/catstd-3.2.htx
     *
     * @return property description if present, NULL otherwise.
     */
    inline const char* GetDescription(void) const
    {
        // Return property description
        return _meta->GetDescription();
    }

    /**
     * Get property CDS link.
     *
     * @return property CDS link if present, NULL otherwise.
     */
    inline const char* GetLink(void) const
    {
        // Return property link
        return _meta->GetLink();
    }
    
    // Get the object summary as a string, including all its member's values
    std::string GetSummaryString(void) const;

private:
    // metadata (constant):
    const vobsSTAR_PROPERTY_META* _meta;
    
    // data:
    char*                    _value;           // Value
    mcsDOUBLE                _numerical;       // Value as a true double floating point numerical (!)
    vobsCONFIDENCE_INDEX     _confidenceIndex; // Confidence index
    const char*              _origin;          /* Either the catalog name where the
                                                * value has been found or
                                                * vobsSTAR_COMPUTED_PROP if the
                                                * value has been calculated.
                                                */
    
    void copyValue(const char* value);

    /**
     * Get property format.
     *
     * @return property format
     */
    inline const char* GetFormat(void) const
    {
        // Return property format
        return _meta->GetFormat();
    }
    
};

#endif /*!vobsSTAR_PROPERTY_H*/

/*___oOo___*/
