#ifndef sclsvrCALIBRATOR_H
#define sclsvrCALIBRATOR_H
/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: sclsvrCALIBRATOR.h,v 1.24 2005-02-23 17:04:15 scetre Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.23  2005/02/23 08:12:59  gzins
 * Replaced DIAM_ERROR by DIAM_BV_ERROR, DIAM_VR_ERROR and DIAM_VK_ERROR
 *
 * Revision 1.22  2005/02/16 17:33:20  gzins
 * Removed obsolete Pack and UnPack methods
 *
 * Revision 1.21  2005/02/10 08:20:21  gzins
 * Added properties for diameter from B-V, V-K and V- R calibration
 *
 * Revision 1.20  2005/02/08 04:38:50  gzins
 * Updated for new vobsREQUEST API and used new sclsvrREQUEST class
 *
 * Revision 1.19  2005/02/07 14:59:28  gzins
 * Added CVS log as modification history
 *
 * scetre    13-Sep-2004  Created
 *
 ******************************************************************************/

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
#include "vobs.h"
#include "sclsvrREQUEST.h"

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
#define sclsvrCALIBRATOR_VIS2               "VIS2"
#define sclsvrCALIBRATOR_VIS2_ERROR         "VIS2_ERROR"
#define sclsvrCALIBRATOR_VIS2_FLAG          "VIS2_FLAG"
#define sclsvrCALIBRATOR_VIS2_8             "VIS2_8"
#define sclsvrCALIBRATOR_VIS2_8_ERROR       "VIS2_8_ERROR"
#define sclsvrCALIBRATOR_VIS2_13            "VIS2_13"
#define sclsvrCALIBRATOR_VIS2_13_ERROR      "VIS2_13_ERROR"
#define sclsvrCALIBRATOR_DIAM_BV            "DIAM_BV"
#define sclsvrCALIBRATOR_DIAM_VR            "DIAM_VR"
#define sclsvrCALIBRATOR_DIAM_VK            "DIAM_VK"
#define sclsvrCALIBRATOR_DIAM_BV_ERROR      "DIAM_BV_ERROR"
#define sclsvrCALIBRATOR_DIAM_VR_ERROR      "DIAM_VR_ERROR"
#define sclsvrCALIBRATOR_DIAM_VK_ERROR      "DIAM_VK_ERROR"
#define sclsvrCALIBRATOR_DIAM_FLAG          "DIAM_FLAG"

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
    mcsCOMPL_STAT Complete(sclsvrREQUEST &request);
 
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
    mcsCOMPL_STAT ComputeVisibility(sclsvrREQUEST &request);
    mcsCOMPL_STAT ComputeMultiplicity(); 
};

#endif /*!sclsvrCALIBRATOR_H*/

/*___oOo___*/
