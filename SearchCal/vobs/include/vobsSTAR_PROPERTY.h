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
 * Constant declaration
 */
#define vobsSTAR_PROP_NOT_SET  "-"   /**< Default value of empty properties */
#define vobsSTAR_COMPUTED_PROP "computed"  /**< Tag for computed properties */


/*
 * Type declaration
 */
/**
 * Confidence index.
 */
typedef enum
{
    vobsCONFIDENCE_LOW =0, /* Low confidence in property value */
    vobsCONFIDENCE_MEDIUM, /* Medium confidence in property value */
    vobsCONFIDENCE_HIGH    /* High confidence in property value */
} vobsCONFIDENCE_INDEX;

typedef enum
{
    vobsSTRING_PROPERTY = 0,
    vobsFLOAT_PROPERTY
} vobsPROPERTY_TYPE;


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
    vobsSTAR_PROPERTY(); 

    vobsSTAR_PROPERTY(const char*        id,
                      const char*        name,
                      vobsPROPERTY_TYPE  type,
                      const char*        unit        = NULL,
                      const char*        format      = NULL,
                      const char*        link        = NULL,
                      const char*        description = NULL);

    vobsSTAR_PROPERTY(const vobsSTAR_PROPERTY&);

    vobsSTAR_PROPERTY& operator=(const vobsSTAR_PROPERTY&);

    // Class destructor
    virtual ~vobsSTAR_PROPERTY();
    
    // Property value setting
    virtual mcsCOMPL_STAT SetValue(const char *value,
                                   const char *origin,
                                   vobsCONFIDENCE_INDEX confidenceIndex=vobsCONFIDENCE_HIGH,
                                   mcsLOGICAL overwrite=mcsFALSE);
    virtual mcsCOMPL_STAT SetValue(mcsDOUBLE value,
                                   const char *origin,
                                   vobsCONFIDENCE_INDEX confidenceIndex=vobsCONFIDENCE_HIGH,
                                   mcsLOGICAL overwrite=mcsFALSE);
    virtual mcsCOMPL_STAT ClearValue(void);
    
    // Get value
    virtual const char   *GetValue(void) const;
    virtual mcsCOMPL_STAT GetValue(mcsDOUBLE *value) const;

    // Get property origin 
    virtual const char   *GetOrigin(void);

    // Get Confidence Index
    virtual vobsCONFIDENCE_INDEX GetConfidenceIndex(void);
    
    // Is value computed?
    virtual mcsLOGICAL    IsComputed(void) const;
    
    // Is value set?
    virtual mcsLOGICAL    IsSet(void) const;

    // Get id/name
    virtual const char *GetId(void) const;
    virtual const char *GetName(void) const;

    // Get property type 
    virtual vobsPROPERTY_TYPE GetType(void) const;
    
    // Get property unit
    virtual const char *GetUnit(void) const;

    // Get property description
    virtual const char *GetDescription(void) const;

    // Get property CDS link
    virtual const char *GetLink(void) const;

    // Get the object summary as a string, including all its member's values
    virtual string GetSummaryString(void) const;

protected:
    
private:
    string               _id;              // Identifier
    string               _name;            // Name
    vobsPROPERTY_TYPE    _type;            // Type of the value
    string               _unit;            // Unit of the value
    string               _format;          // Format to print value 
    string               _link;            // CDS link of the value
    string               _description;     // Description of the value
    mcsSTRING64          _value;           // Value
    mcsDOUBLE             _numerical;       // Value as a true floating point numerical (!)
    vobsCONFIDENCE_INDEX _confidenceIndex; // Confidence index
    string               _origin;          /* Either the catalog name where the
                                            * value has been found or
                                            * vobsSTAR_COMPUTED_PROP if the
                                            * value has been calculated.
                                            */
};

#endif /*!vobsSTAR_PROPERTY_H*/

/*___oOo___*/
