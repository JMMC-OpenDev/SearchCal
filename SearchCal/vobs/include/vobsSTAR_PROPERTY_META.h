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
 * Constant declaration
 */
#define vobsSTAR_PROP_NOT_SET  "-"   /**< Default value of empty properties */

typedef enum
{
    vobsSTRING_PROPERTY = 0,
    vobsFLOAT_PROPERTY
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
    vobsSTAR_PROPERTY_META(const char*        id,
                           const char*        name,
                           const vobsPROPERTY_TYPE   type,
                           const char*        unit        = NULL,
                           const char*        format      = NULL,
                           const char*        link        = NULL,
                           const char*        description = NULL);
    
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
        if ((_unit == NULL) || (strlen(_unit) == 0))
        {
            return vobsSTAR_PROP_NOT_SET;
        }

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
        if ((_link == NULL) || (strlen(_link) == 0))
        {
            return NULL;
        }

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
        if ((_description == NULL) || (strlen(_description) == 0))
        {
            return NULL;
        }

        // Return property description
        return _description;
    }

private:
    // Declaration of copy constructor and assignment operator as private
    // methods, in order to hide them from the users.
    vobsSTAR_PROPERTY_META(const vobsSTAR_PROPERTY_META& property);
    vobsSTAR_PROPERTY_META& operator=(const vobsSTAR_PROPERTY_META& meta);

    // metadata members (constant):
    const char*              _id;              // Identifier
    const char*              _name;            // Name
    vobsPROPERTY_TYPE        _type;            // Type of the value
    const char*              _unit;            // Unit of the value
    const char*              _format;          // Format to print value 
    const char*              _link;            // CDS link of the value
    const char*              _description;     // Description of the value
};

#endif /*!vobsSTAR_PROPERTY_META_H*/

/*___oOo___*/
