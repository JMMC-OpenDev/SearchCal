#ifndef vobsSTAR_PROPERTY_H
#define vobsSTAR_PROPERTY_H
/*******************************************************************************
* JMMC project
*
* "@(#) $Id: vobsSTAR_PROPERTY.h,v 1.3 2005-01-26 08:10:32 scetre Exp $"
*
* History
* -------
* $Log: not supported by cvs2svn $
* gzins     13-Dec-2004  Created
*
*
*******************************************************************************/

/**
 * \file
 * vobsSTAR_PROPERTY class declaration.
 */

#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif

#define vobsSTAR_PROP_NOT_SET  "-"  /**< Default value of the empty
                                      properties */
/*
 * Class declaration
 */

/**
 * Confidence index.
 */
typedef enum
{
    vobsCONFIDENCE_VERY_LOW=0,
    vobsCONFIDENCE_2, /* id of the coefficient a1 in the table */
    vobsCONFIDENCE_3, /* id of the coefficient a2 in the table */
    vobsCONFIDENCE_4, /* id of the coefficient a3 in the table */
    vobsCONFIDENCE_HIGH  /* id of the coefficient a4 in the table */
} vobsCONFIDENCE_INDEX;

typedef enum
{
    vobsSTRING_PROPERTY = 0,
    vobsFLOAT_PROPERTY
} vobsPROPERTY_TYPE;
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
    vobsSTAR_PROPERTY(char *id, char *name, 
                      vobsPROPERTY_TYPE type, char *format=NULL);
    vobsSTAR_PROPERTY(const vobsSTAR_PROPERTY&);
    vobsSTAR_PROPERTY& operator=(const vobsSTAR_PROPERTY&);

    // Class destructor
    virtual ~vobsSTAR_PROPERTY();
    
    // Set value
    virtual mcsCOMPL_STAT SetValue(char *value,
                                   mcsINT32 confidenceIndex=vobsCONFIDENCE_HIGH,
                                   mcsFLOAT isComputed=mcsFALSE,
                                   mcsLOGICAL overwrite=mcsFALSE);
    virtual mcsCOMPL_STAT SetValue(mcsFLOAT value,
                                   mcsINT32 confidenceIndex=vobsCONFIDENCE_HIGH,
                                   mcsFLOAT isComputed=mcsFALSE,
                                   mcsLOGICAL overwrite=mcsFALSE);
    
    // Get value
    virtual const char   *GetValue(void) const;
    virtual mcsCOMPL_STAT GetValue(mcsFLOAT *value) const;

    // Get Confidence Index
    virtual mcsINT32      GetConfidenceIndex();
    
    // Is value computed?
    virtual mcsLOGICAL    IsComputed() const;
    
    // Is value set?
    virtual mcsLOGICAL    IsSet() const;

    // Get id/name
    virtual const char *GetId() const;
    virtual const char *GetName() const;
    
protected:
    
private:
    string            _id;              // Identifier
    string            _name;            // Name
    vobsPROPERTY_TYPE _type;            // Type of the value
    string            _format;          // Format to print value 
    mcsSTRING64       _value;           // Value
    mcsINT32          _confidenceIndex; // Confidence index
    mcsLOGICAL        _isComputed;      // Say if property is computed or not
};

#endif /*!vobsSTAR_PROPERTY_H*/

/*___oOo___*/
