#ifndef sclsvrCALIBRATOR_H
#define sclsvrCALIBRATOR_H
/*******************************************************************************
* JMMC project
*
* "@(#) $Id: sclsvrCALIBRATOR.h,v 1.14 2004-12-21 08:45:08 scetre Exp $"
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
 * Definition of the calibrators properties
 */
#define sclsvrCALIBRATOR_ANGULAR_DIAMETER       "ANGULAR_DIAMETER"
#define sclsvrCALIBRATOR_ANGULAR_DIAMETER_ERROR "ANGULAR_DIAMETER_ERROR"
#define sclsvrCALIBRATOR_MO                     "MO"
#define sclsvrCALIBRATOR_LO                     "LO"
#define sclsvrCALIBRATOR_KO                     "KO"
#define sclsvrCALIBRATOR_HO                     "HO"
#define sclsvrCALIBRATOR_JO                     "JO"
#define sclsvrCALIBRATOR_IO                     "IO"
#define sclsvrCALIBRATOR_RO                     "RO"
#define sclsvrCALIBRATOR_VO                     "VO"
#define sclsvrCALIBRATOR_BO                     "BO"
#define sclsvrCALIBRATOR_MULTIPLICITY           "MULTIPLICITY"
#define sclsvrCALIBRATOR_VISIBILITY             "VISIBILITY"
#define sclsvrCALIBRATOR_VISIBILITY_ERROR       "VISIBILITY_ERROR"

/*
 * Class declaration
 */
class sclsvrCALIBRATOR : public vobsSTAR
{
public:
    // Constructors
    sclsvrCALIBRATOR();
    // Copy Construstor
    sclsvrCALIBRATOR(vobsSTAR &star);
     
    // Destructor
    virtual ~sclsvrCALIBRATOR();    
  
    // method to complete calibrator properties
    mcsCOMPL_STAT Complete(vobsREQUEST request);
 
    virtual mcsCOMPL_STAT Pack(miscDYN_BUF *buffer);
    virtual mcsCOMPL_STAT UnPack(char *calibratorString);

    // Say if the calibrator had coherent diameter
    virtual mcsLOGICAL HadCoherentDiameter();
    // Say if the calibrator had visibility OK
    virtual mcsLOGICAL VisibilityOk();

protected:
    
private:
    // Declaration of assignment operator as private method, in order to hide
    // them from the users.
    sclsvrCALIBRATOR& operator=(const sclsvrCALIBRATOR&);
    
    // Method to define all star properties
    mcsCOMPL_STAT AddProperties(void);

    // Method to compute specific property
    mcsCOMPL_STAT ComputeMissingMagnitude();
    mcsCOMPL_STAT ComputeGalacticCoordinates();
    mcsCOMPL_STAT ComputeInterstellarAbsorption();
    mcsCOMPL_STAT ComputeAngularDiameter();
    mcsCOMPL_STAT ComputeVisibility(vobsREQUEST request);
    mcsCOMPL_STAT ComputeMultiplicity(); 

    mcsLOGICAL _coherentDiameter;
    mcsLOGICAL _correctVisibility;
    
};

#endif /*!sclsvrCALIBRATOR_H*/

/*___oOo___*/
