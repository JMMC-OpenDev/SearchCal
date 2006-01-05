#ifndef vobsSTAR_PROPERTY_H
#define vobsSTAR_PROPERTY_H
/*******************************************************************************
* JMMC project
*
* "@(#) $Id: vobsSTAR_PROPERTY.h,v 1.14 2006-01-05 09:07:39 lafrasse Exp $"
*
* History
* -------
* $Log: not supported by cvs2svn $
* Revision 1.13  2005/12/07 16:49:18  lafrasse
* Added support for 'description' attribute in VOTable column descriptors FIELD.
*
* Revision 1.12  2005/12/02 17:42:26  lafrasse
* Added property unit handling
*
* Revision 1.11  2005/11/16 10:47:54  scetre
* Updated documentation
*
* Revision 1.10  2005/11/16 10:47:54  scetre
* Updated documentation
*
* Revision 1.9  2005/11/11 16:38:09  gzins
* Added GetType() method
*
* Revision 1.8  2005/03/04 06:34:37  gzins
* Updated GetConfidenceIndex prototype
*
* Revision 1.7  2005/02/11 10:43:35  gzins
* Fixed bug related to vobsCONFIDENCE_INDEX type
*
* Revision 1.6  2005/02/08 20:26:16  gzins
* Changed _isComputed by _origin
*
* Revision 1.5  2005/02/08 07:16:56  gzins
* Changed value parameter type: char* to const char*
*
* Revision 1.4  2005/02/04 15:25:13  gzins
* Redefined confidence index
*
* Revision 1.3  2005/01/26 08:10:32  scetre
* change history
*
* gzins     13-Dec-2004  Created
*
*
*******************************************************************************/

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
                      const char*        unit,
                      const char*        format=NULL,
                      const char*        description=NULL);

    vobsSTAR_PROPERTY(const vobsSTAR_PROPERTY&);

    vobsSTAR_PROPERTY& operator=(const vobsSTAR_PROPERTY&);

    // Class destructor
    virtual ~vobsSTAR_PROPERTY();
    
    // Property value setting
    virtual mcsCOMPL_STAT SetValue(const char *value,
                                   const char *origin,
                                   vobsCONFIDENCE_INDEX confidenceIndex=vobsCONFIDENCE_HIGH,
                                   mcsLOGICAL overwrite=mcsFALSE);
    virtual mcsCOMPL_STAT SetValue(mcsFLOAT value,
                                   const char *origin,
                                   vobsCONFIDENCE_INDEX confidenceIndex=vobsCONFIDENCE_HIGH,
                                   mcsLOGICAL overwrite=mcsFALSE);
    
    // Get value
    virtual const char   *GetValue(void) const;
    virtual mcsCOMPL_STAT GetValue(mcsFLOAT *value) const;

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

protected:
    
private:
    string               _id;              // Identifier
    string               _name;            // Name
    vobsPROPERTY_TYPE    _type;            // Type of the value
    string               _unit;            // Unit of the value
    string               _format;          // Format to print value 
    string               _description;     // Description of the value
    mcsSTRING64          _value;           // Value
    vobsCONFIDENCE_INDEX _confidenceIndex; // Confidence index
    string               _origin;          /* Either the catalog name where the
                                            * value has been found or
                                            * vobsSTAR_COMPUTED_PROP if the
                                            * value has been calculated.
                                            */
};

#endif /*!vobsSTAR_PROPERTY_H*/

/*___oOo___*/
