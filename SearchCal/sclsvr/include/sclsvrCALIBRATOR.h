#ifndef sclsvrCALIBRATOR_H
#define sclsvrCALIBRATOR_H
/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: sclsvrCALIBRATOR.h,v 1.33 2005-12-14 09:01:39 scetre Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.32  2005/12/12 14:09:26  scetre
 * Added computing cousin magnitude as propertie of the calibrator
 *
 * Revision 1.31  2005/12/08 12:52:08  scetre
 * Merged Compute diameter and magnitude for faint and bright method
 *
 * Revision 1.30  2005/12/07 14:52:27  scetre
 * Updated method name to compute diameter and magnitude
 *
 * Revision 1.29  2005/12/05 16:01:36  scetre
 * Added magnitude faint and diameter faint computing
 * Added mean diam and mean diam error as calibrator properties
 *
 * Revision 1.28  2005/11/30 10:54:45  scetre
 * Added compute faint diameter
 *
 * Revision 1.27  2005/11/14 14:19:41  lafrasse
 * Added "distance to science object" computation and sorting
 *
 * Revision 1.26  2005/10/26 11:27:24  lafrasse
 * Code review
 *
 * Revision 1.25  2005/03/07 16:05:52  gzins
 * Removed automatic sort on visibility
 *
 * Revision 1.24  2005/02/23 17:04:15  scetre
 * Added vis and visErr 8mu and 13mu
 *
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
 * @file
 * sclsvrCALIBRATOR class declaration.
 */

#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif


/*
 * SCALIB header
 */
#include "vobs.h"
#include "alx.h"

/*
 * Local header
 */
#include "sclsvrREQUEST.h"


/* 
 * Constants definition
 */
/* Definition of the calibrators properties */
#define sclsvrCALIBRATOR_PHOT_COUS_J        "PHOT_COUS_J"
#define sclsvrCALIBRATOR_PHOT_COUS_H        "PHOT_COUS_H"
#define sclsvrCALIBRATOR_PHOT_COUS_K        "PHOT_COUS_K"
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
#define sclsvrCALIBRATOR_DIAM_IJ            "DIAM_IJ"
#define sclsvrCALIBRATOR_DIAM_IK            "DIAM_IK"
#define sclsvrCALIBRATOR_DIAM_JK            "DIAM_JK"
#define sclsvrCALIBRATOR_DIAM_JH            "DIAM_JH"
#define sclsvrCALIBRATOR_DIAM_MEAN          "DIAM_MEAN"
#define sclsvrCALIBRATOR_DIAM_BV_ERROR      "DIAM_BV_ERROR"
#define sclsvrCALIBRATOR_DIAM_VR_ERROR      "DIAM_VR_ERROR"
#define sclsvrCALIBRATOR_DIAM_VK_ERROR      "DIAM_VK_ERROR"
#define sclsvrCALIBRATOR_DIAM_IJ_ERROR      "DIAM_IJ_ERROR"
#define sclsvrCALIBRATOR_DIAM_IK_ERROR      "DIAM_IK_ERROR"
#define sclsvrCALIBRATOR_DIAM_JK_ERROR      "DIAM_JK_ERROR"
#define sclsvrCALIBRATOR_DIAM_JH_ERROR      "DIAM_JH_ERROR"
#define sclsvrCALIBRATOR_DIAM_MEAN_ERROR    "DIAM_MEAN_ERROR"
#define sclsvrCALIBRATOR_DIAM_FLAG          "DIAM_FLAG"
#define sclsvrCALIBRATOR_DIST               "DIST"

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
  
    // Complete calibrator properties
    mcsCOMPL_STAT Complete(sclsvrREQUEST &request);
 
    // Return wether the calibrator has a coherent diameter or not
    virtual mcsLOGICAL IsDiameterOk();

protected:
    
private:
    // Define all star properties
    mcsCOMPL_STAT AddProperties(void);

    // Compute specific property
    mcsCOMPL_STAT ComputeMissingMagnitude(alxBAND firstBand,
                                          alxBAND secondBand,
                                          mcsLOGICAL isBright=mcsTRUE);
    mcsCOMPL_STAT ComputeGalacticCoordinates();
    mcsCOMPL_STAT 
        ComputeInterstellarAbsorption(char *magPropertyId[alxNB_BANDS]);
    mcsCOMPL_STAT ComputeAngularDiameter(alxDATA mg1,
                                         alxDATA mg2,
                                         alxDATA mg3,
                                         alxDATA mg4,
                                         char *starPropertyId[4],
                                         mcsLOGICAL isBright=mcsTRUE);
    mcsCOMPL_STAT ComputeVisibility(sclsvrREQUEST &request);
    mcsCOMPL_STAT ComputeMultiplicity(); 
    mcsCOMPL_STAT ComputeDistance(sclsvrREQUEST &request); 
};

#endif /*!sclsvrCALIBRATOR_H*/

/*___oOo___*/
