#ifndef sclsvrCALIBRATOR_H
#define sclsvrCALIBRATOR_H
/*******************************************************************************
* JMMC project
*
* "@(#) $Id: sclsvrCALIBRATOR.h,v 1.11 2004-12-13 13:33:48 scetre Exp $"
*
* who       when         what
* --------  -----------  -------------------------------------------------------
* scetre    13-Sep-2004  Created
*
*
*******************************************************************************/

/**
 * \file
 * sclsvrCALIBRATOR class declaration.
 */

#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif

/*
 * Local header
 */
#include"vobs.h"

/* 
 * Constants definition
 */

#define sclsvrCALIBRATOR_PROP_NOT_SET  "-"  /**< Default value of the empty
                                          properties */

/*
 * Type definition
 */

typedef mcsSTRING32 sclsvrCALIBRATOR_PROPERTY;

/*
 * Enumeration type definition
 */

/**
 * sclsvrPROPERTY_ID is an enumeration which allow correspondance between an id
 * and an PROPERTY.
 */
/* The following offset is used to have id for calibrator properties which
 * differs from the ucd id used by vobsSTAR class */
typedef enum
{
    UNKNOWN_PROP_ID = -1,
    ANGULAR_DIAMETER_ID,
    ANGULAR_DIAMETER_ERROR_ID,
    MO_ID,
    LO_ID,
    KO_ID,
    HO_ID,
    JO_ID,
    IO_ID,
    RO_ID,
    VO_ID,
    BO_ID,
    MULTIPLICITY_ID,
    VISIBILITY_ID,
    VISIBILITY_ERROR_ID,
    sclsvrNB_CALIBRATOR_PROPERTIES
} sclsvrPROPERTY_ID;

/*
 * Class declaration
 */
class sclsvrCALIBRATOR : public vobsSTAR
{
public:
    // Constructors
    sclsvrCALIBRATOR();
    sclsvrCALIBRATOR(const sclsvrCALIBRATOR &sclsvr);
    sclsvrCALIBRATOR(const vobsSTAR &star);
    
    // Destructor
    virtual ~sclsvrCALIBRATOR();
 
    // Method to set the star properties

    virtual mcsCOMPL_STAT SetProperty(char *property, char *value, 
                                      mcsLOGICAL overwrite=mcsFALSE);
    virtual mcsCOMPL_STAT SetProperty(vobsUCD_ID ucdId, char *value,
                                      mcsLOGICAL overwrite=mcsFALSE);
    virtual mcsCOMPL_STAT SetProperty(vobsUCD_ID ucdId, mcsFLOAT value,
                                      mcsLOGICAL overwrite=mcsFALSE);
    virtual mcsCOMPL_STAT SetProperty(sclsvrPROPERTY_ID id, char *value,
                                      mcsLOGICAL overwrite=mcsFALSE);
    virtual mcsCOMPL_STAT SetProperty(sclsvrPROPERTY_ID id, mcsFLOAT value,
                                      mcsLOGICAL overwrite=mcsFALSE);
    // Is property set?
    virtual mcsLOGICAL    IsPropertySet(char *ucd) const;
    virtual mcsLOGICAL    IsPropertySet(sclsvrPROPERTY_ID ucdId) const;      
    virtual mcsLOGICAL    IsPropertySet(vobsUCD_ID ucdId) const;

    // Methods to retreive the star properties
    virtual mcsCOMPL_STAT GetProperty(char *property, char *value) const;
    virtual mcsCOMPL_STAT GetProperty(sclsvrPROPERTY_ID, char *value) const; 
    virtual mcsCOMPL_STAT GetProperty(char *property, float *value) const;
    virtual mcsCOMPL_STAT GetProperty(sclsvrPROPERTY_ID id, float *value) const;
    
    virtual mcsCOMPL_STAT GetProperty(vobsUCD_ID id, char *value) const; 
    virtual mcsCOMPL_STAT GetProperty(vobsUCD_ID id, float *value) const;
    
  
    // method to complete calibrator properties
    mcsCOMPL_STAT Complete(vobsREQUEST request);

    // methods to get galactic coordonates
    mcsCOMPL_STAT GetLongitude(float &longitude);
    mcsCOMPL_STAT GetLattitude(float &lattitude);

    virtual void Display(void);    
   
    virtual mcsCOMPL_STAT Pack(miscDYN_BUF *buffer);
    virtual mcsCOMPL_STAT UnPack(char *calibratorString);

    // Say if the calibrator had coherent diameter
    virtual mcsLOGICAL HadCoherentDiameter();
    // Say if the calibrator had visibility OK
    virtual mcsLOGICAL VisibilityOk();

protected:

    // Method to obtain id from the UCD
    virtual sclsvrPROPERTY_ID Property2Id(char *ucd) const;
    
private:
    // Declaration of assignment operator as private method, in order to hide
    // them from the users.
    sclsvrCALIBRATOR& operator=(const sclsvrCALIBRATOR&);

    // Method to compute specific property
    mcsCOMPL_STAT ComputeMissingMagnitude();
    mcsCOMPL_STAT ComputeGalacticCoordinates();
    mcsCOMPL_STAT ComputeInterstellarAbsorption();
    mcsCOMPL_STAT ComputeAngularDiameter();
    mcsCOMPL_STAT ComputeVisibility(vobsREQUEST request);
    mcsCOMPL_STAT ComputeMultiplicity();

    /** Table containing the calibrator computed properties */
    sclsvrCALIBRATOR_PROPERTY _compProperties[sclsvrNB_CALIBRATOR_PROPERTIES];

    mcsLOGICAL _coherentDiameter;
    mcsLOGICAL _correctVisibility;
    
};

#endif /*!sclsvrCALIBRATOR_H*/

/*___oOo___*/
