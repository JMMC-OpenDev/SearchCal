#ifndef sclsvrCALIBRATOR_H
#define sclsvrCALIBRATOR_H
/*******************************************************************************
* JMMC project
*
* "@(#) $Id: sclsvrCALIBRATOR.h,v 1.18 2005-02-07 09:50:45 scetre Exp $"
*
* History
* -------
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

/* Definition of the calibrators properties */
#define sclsvrCALIBRATOR_MO                 "MO"
#define sclsvrCALIBRATOR_LO                 "LO"
#define sclsvrCALIBRATOR_KO                 "KO"
#define sclsvrCALIBRATOR_HO                 "HO"
#define sclsvrCALIBRATOR_JO                 "JO"
#define sclsvrCALIBRATOR_IO                 "IO"
#define sclsvrCALIBRATOR_RO                 "RO"
#define sclsvrCALIBRATOR_VO                 "VO"
#define sclsvrCALIBRATOR_BO                 "BO"
#define sclsvrCALIBRATOR_VIS                "VIS"
#define sclsvrCALIBRATOR_VIS_ERROR          "VIS_ERROR"
#define sclsvrCALIBRATOR_VIS_FLAG           "VIS_FLAG"
#define sclsvrCALIBRATOR_ANGULAR_DIAM       "ANGULAR_DIAM"
#define sclsvrCALIBRATOR_ANGULAR_DIAM_ERROR "ANGULAR_DIAM_ERROR"
#define sclsvrCALIBRATOR_ANGULAR_DIAM_FLAG  "ANGULAR_DIAM_FLAG"

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
    virtual mcsLOGICAL IsDiameterOk();
    // Say if the calibrator had visibility OK
    virtual mcsLOGICAL IsVisibilityOk();

protected:
    
private:
    // Method to define all star properties
    mcsCOMPL_STAT AddProperties(void);

    // Method to compute specific property
    mcsCOMPL_STAT ComputeMissingMagnitude();
    mcsCOMPL_STAT ComputeGalacticCoordinates();
    mcsCOMPL_STAT ComputeInterstellarAbsorption();
    mcsCOMPL_STAT ComputeAngularDiameter();
    mcsCOMPL_STAT ComputeVisibility(vobsREQUEST request);
    mcsCOMPL_STAT ComputeMultiplicity(); 
};

#endif /*!sclsvrCALIBRATOR_H*/

/*___oOo___*/
