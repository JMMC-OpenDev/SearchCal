/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: sclsvrCALIBRATOR.cpp,v 1.66 2006-01-09 16:11:26 lafrasse Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.65  2005/12/22 10:14:35  scetre
 * Create block to simplify the Complete() method
 * changed call to new method of alx
 *
 * Revision 1.64  2005/12/16 13:26:24  scetre
 * Added test when computing visibility Added visibility error as parameter
 *
 * Revision 1.63  2005/12/16 10:42:27  scetre
 * Added test when computing visibility
 *
 * Revision 1.62  2005/12/15 12:30:17  scetre
 * Added log
 *
 * Revision 1.61  2005/12/12 14:09:26  scetre
 * Added computing cousin magnitude as propertie of the calibrator
 *
 * Revision 1.60  2005/12/08 12:52:08  scetre
 * Merged Compute diameter and magnitude for faint and bright method
 *
 * Revision 1.59  2005/12/07 16:51:07  lafrasse
 * Added one property description for VOTable generation test
 *
 * Revision 1.58  2005/12/07 15:26:00  scetre
 * Added known unit on properties
 *
 * Revision 1.57  2005/12/07 15:11:03  scetre
 * Removed unused printf
 *
 * Revision 1.56  2005/12/07 14:52:27  scetre
 * Updated method name to compute diameter and magnitude
 *
 * Revision 1.55  2005/12/06 08:37:54  scetre
 * Prepared computing for faint calibrator
 * Added Qflag filter on faint scenario
 *
 * Revision 1.54  2005/12/02 17:45:29  lafrasse
 * Added a default "-" star property unit
 *
 * Revision 1.53  2005/11/30 10:35:21  scetre
 * Updated Filter without name
 * Updated scenario
 *
 * Revision 1.52  2005/11/29 13:06:15  scetre
 * Added conditions to compute specific properties of faint and bright star
 *
 * Revision 1.51  2005/11/14 14:19:41  lafrasse
 * Added "distance to science object" computation and sorting
 *
 * Revision 1.50  2005/10/26 11:27:24  lafrasse
 * Code review
 *
 * Revision 1.49  2005/10/24 13:09:41  lafrasse
 * Refined code documentation
 *
 * Revision 1.48  2005/06/01 14:18:54  scetre
 * Added filters and filter list objects.
 * Changed logExtDbg to logTrace
 *
 * Revision 1.47  2005/04/06 12:16:45  scetre
 * Removed unused alxNO_CONFIDENCE.
 * All actions describe in the documentation are now in the alx library
 *
 * Revision 1.46  2005/03/30 12:50:58  scetre
 * Changed call to alx funtions.
 * Updated documentation
 *
 * Revision 1.45  2005/03/07 16:06:06  gzins
 * Removed automatic sort on visibility
 *
 * Revision 1.44  2005/03/04 12:50:11  gzins
 * Added test on paralax; do not compute magnitudes and angular diameter when paralax < 1 mas
 *
 * Revision 1.43  2005/03/04 09:59:38  gzins
 * Set confidence index of computed visibility
 *
 * Revision 1.42  2005/03/03 16:48:55  scetre
 * Trace the confidence index from the computed magnitude until visibility
 *
 * Revision 1.41  2005/02/23 17:16:06  scetre
 * Updated property name
 *
 * Revision 1.40  2005/02/23 17:05:27  scetre
 * Added in compute visibility method the compute of visibility with wlen = 8 and 13 mu if it is a N observed band
 *
 * Revision 1.39  2005/02/23 12:52:45  scetre
 * Changed test if visibility is ok or not
 *
 * Revision 1.38  2005/02/23 09:06:28  scetre
 * Extract the visibility ok list with the visErr parameter and the vis instead of the only visi
 *
 * Revision 1.37  2005/02/23 08:15:19  gzins
 * Changed DIAM_ERROR to DIAM_BV_ERROR, DIAM_VR_ERROR and DIAM_VK_ERROR
 * Updated visibility computation to use diameters from catalogs, and if not found the computed diameter from V-K
 *
 * Revision 1.36  2005/02/22 16:24:18  gzins
 * Updated  alxComputeAngularDiameter API
 *
 * Revision 1.35  2005/02/22 10:20:49  gzins
 * Retrieved all known magnitude values before computing missing magnitudes
 *
 * Revision 1.34  2005/02/22 09:50:43  scetre
 * Back to compute magnitude instead of cds magnitude to compute diameter
 *
 * Revision 1.33  2005/02/22 08:10:39  gzins
 * Update calls to alxComputeRealMagnitudes() and alxComputeMagnitudesForBrightStar() functions
 *
 * Revision 1.32  2005/02/21 14:46:53  scetre
 * Use CDS magnitude instead of computed magnitude to compute diameter
 *
 * Revision 1.31  2005/02/17 15:33:54  gzins
 * Removed printf used for debug
 *
 * Revision 1.30  2005/02/16 17:33:20  gzins
 * Removed obsolete Pack and UnPack methods
 *
 * Revision 1.29  2005/02/11 10:53:17  gzins
 * Updated property IDs related to diameter
 * Fixed conflict related to confidence index types
 *
 * Revision 1.28  2005/02/10 08:19:29  gzins
 * Added properties for diameter from B-V, V-K and V- R calibration
 *
 * Revision 1.27  2005/02/08 21:36:41  gzins
 * Removed diameter error conversion (% to mas). Done at catalog level.
 *
 * Revision 1.26  2005/02/08 20:54:06  gzins
 * Added 'computed' flag when setting computed star property
 * Updated Pack() and Unpack() to store and retrieve origin and confidence index of star properties
 *
 * Revision 1.25  2005/02/08 04:39:32  gzins
 * Updated for new vobsREQUEST API and used new sclsvrREQUEST class
 *
 * Revision 1.24  2005/02/07 15:00:48  gzins
 * Added CVS log as modification history
 *
 * scetre    14-Sep-2004  Created
 *
 ******************************************************************************/

/**
 * @file
 * sclsvrCALIBRATOR class definition.
 */

static char *rcsId="@(#) $Id: sclsvrCALIBRATOR.cpp,v 1.66 2006-01-09 16:11:26 lafrasse Exp $"; 
static void *use_rcsId = ((void)&use_rcsId,(void *) &rcsId);


/* 
 * System Headers 
 */
#include <iostream>
using namespace std;
#include "gdome.h"


/*
 * MCS Headers 
 */
#include "mcs.h"
#include "log.h"
#include "err.h"


/*
 * SCALIB Headers 
 */
#include "alx.h"
#include "alxErrors.h"


/*
 * Local Headers 
 */
#include "sclsvrPrivate.h"
#include "sclsvrErrors.h"
#include "sclsvrCALIBRATOR.h"


/**
 * Class constructor.
 */
sclsvrCALIBRATOR::sclsvrCALIBRATOR()
{
    // Add all star properties 
    AddProperties();
}

/**
 * Copy Constructor.
 */
sclsvrCALIBRATOR::sclsvrCALIBRATOR(vobsSTAR &star)
{
    (*this).vobsSTAR::operator=(star); 

    // Add all star properties 
    AddProperties(); 
}

/**
 * Class destructor.
 */
sclsvrCALIBRATOR::~sclsvrCALIBRATOR()
{
}


/*
 * Public methods
 */
/**
 * Return wether the calibrator has a coherent diameter or not.
 */
mcsLOGICAL sclsvrCALIBRATOR::IsDiameterOk()
{
    // If diameter has not been computed yet
    if (IsPropertySet(sclsvrCALIBRATOR_DIAM_FLAG) == mcsFALSE)
    {
        return mcsFALSE;
    }

    // Get the flag, and test it
    const char* flag = GetPropertyValue(sclsvrCALIBRATOR_DIAM_FLAG);
    if (strcmp(flag, "OK") != 0)
    {
        return mcsFALSE;
    }

    return mcsTRUE;
}

/**
 * Complete the property of the calibrator.
 *
 * Method to complete calibrator properties by using several methods.
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is
 * returned.
 */
mcsCOMPL_STAT sclsvrCALIBRATOR::Complete(sclsvrREQUEST &request)
{
    logTrace("sclsvrCALIBRATOR::Complete()");

    mcsSTRING32 starId;
    // Get Star ID
    if (GetId(starId, sizeof(starId)) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    // If the request should return bright starts
    if ((request.IsBright() == mcsTRUE) &&
        (request.GetSearchAreaGeometry() == vobsBOX))
    {

        // Get the observed band
        const char* band = request.GetSearchBand();

        // Compute Galactic coordinates
        if (ComputeGalacticCoordinates() == mcsFAILURE)
        {
            return mcsFAILURE;
        }

        // If it is not the scenario for N band
        if (strcmp(band, "N") != 0)
        {
            // If paralax of the star if known
            if (IsPropertySet(vobsSTAR_POS_PARLX_TRIG) == mcsTRUE)
            {
                // If paralax is greater than 1 mas, compute real magnitudes,
                // missing magnitudes and the angular diameter 
                mcsFLOAT paralax;
                GetPropertyValue(vobsSTAR_POS_PARLX_TRIG, &paralax);
                if (paralax >= 1)
                {
                    char *magPropertyId[alxNB_BANDS] = 
                    {
                        vobsSTAR_PHOT_JHN_B,
                        vobsSTAR_PHOT_JHN_V,
                        vobsSTAR_PHOT_JHN_R,
                        vobsSTAR_PHOT_JHN_I,
                        vobsSTAR_PHOT_JHN_J,
                        vobsSTAR_PHOT_JHN_H,
                        vobsSTAR_PHOT_JHN_K,
                        vobsSTAR_PHOT_JHN_L,
                        vobsSTAR_PHOT_JHN_M
                    };
                    mcsFLOAT av;
                    if (ComputeExtinctionCoefficient(&av) == mcsFAILURE)
                    {
                        return mcsFAILURE;
                    }
                    // Compute Interstellar extinction
                    if (ComputeInterstellarAbsorption(magPropertyId, av) ==
                        mcsFAILURE)
                    {
                        return mcsFAILURE;
                    }

                    // Compute Missing Magnitude
                    if (ComputeMissingMagnitude(alxB_BAND, alxV_BAND) ==
                        mcsFAILURE)
                    {
                        return mcsFAILURE;
                    }
                    
                    // Compute missing apparent magnitude
                    if (ComputeApparentMagnitude(magPropertyId, av) ==
                        mcsFAILURE)
                    {
                        return mcsFAILURE;
                    }


                    alxDATA mgB, mgV, mgR, mgK;

                    // Declare the 4 properties name for B0, V0, R0, K0
                    char *starPropertyId[4] = 
                    {
                        sclsvrCALIBRATOR_BO,
                        sclsvrCALIBRATOR_VO,
                        sclsvrCALIBRATOR_RO,
                        sclsvrCALIBRATOR_KO
                    };
                    // Compute Angular Diameter
                    if (ComputeAngularDiameter(mgB, mgV, mgR, mgK,
                                               starPropertyId) == mcsFAILURE)
                    {
                        return mcsFAILURE;
                    }
                }
            }
        }
        else
        {
            SetPropertyValue(sclsvrCALIBRATOR_DIAM_FLAG, "OK", 
                             vobsSTAR_COMPUTED_PROP);
        }

        // Compute visibility and visibility error
        if (ComputeVisibility(request) == mcsFAILURE)
        {
            return mcsFAILURE;
        }

        // Compute multiplicity
        if (ComputeMultiplicity() == mcsFAILURE)
        {
            return mcsFAILURE;
        }

        // Compute distance
        if (ComputeDistance(request) == mcsFAILURE)
        {
            return mcsFAILURE;
        }
    }
    // if the researh is faint
    else
    {
        // Compute Galactic coordinates
        if (ComputeGalacticCoordinates() == mcsFAILURE)
        {
            return mcsFAILURE;
        }

        // convert in cousin magnitude
        if (ComputeCousinMagnitude() == mcsFAILURE)
        {
            return mcsFAILURE;
        }
        
        mcsFLOAT paralax;
        mcsFLOAT paralaxError;

        // If paralax of the star if known
        if (IsPropertySet(vobsSTAR_POS_PARLX_TRIG) == mcsTRUE)
        {
            GetPropertyValue(vobsSTAR_POS_PARLX_TRIG, &paralax);
            GetPropertyValue(vobsSTAR_POS_PARLX_TRIG_ERROR, &paralaxError);
        }

        // If paralax of the star if known
        // If paralax is greater than 1 mas, compute real magnitudes,
        // missing magnitudes and the angular diameter
        if ((IsPropertySet(vobsSTAR_POS_PARLX_TRIG) == mcsTRUE) &&
            (paralax >= 1) && (paralaxError/paralaxError < 0.25))
        {
            char *magPropertyId[alxNB_BANDS] = 
            {
                vobsSTAR_PHOT_PHG_B,
                vobsSTAR_PHOT_JHN_V,
                vobsSTAR_PHOT_PHG_R,
                vobsSTAR_PHOT_PHG_I,
                sclsvrCALIBRATOR_PHOT_COUS_J,
                sclsvrCALIBRATOR_PHOT_COUS_H,
                sclsvrCALIBRATOR_PHOT_COUS_K,
                vobsSTAR_PHOT_JHN_L,
                vobsSTAR_PHOT_JHN_M
            };
            // Get the extinction ratio
            mcsFLOAT av;
            if (ComputeExtinctionCoefficient(&av) == mcsFAILURE)
            {
                return mcsFAILURE;
            }
            // Compute Interstellar extinction
            if (ComputeInterstellarAbsorption(magPropertyId, av) ==
                mcsFAILURE)
            {
                return mcsFAILURE;
            }

            // Compute Missing Magnitude
            if (ComputeMissingMagnitude(alxJ_BAND, alxK_BAND, mcsFALSE) ==
                mcsFAILURE)
            {
                return mcsFAILURE;
            }

            // Compute missing apparent magnitude
            if (ComputeApparentMagnitude(magPropertyId, av) ==
                mcsFAILURE)
            {
                return mcsFAILURE;
            }

            alxDATA mgI, mgJ, mgK, mgH;

            // Declare the 4 properties name for I0, J0, K0, H0
            char *starPropertyId[4] = 
            {
                sclsvrCALIBRATOR_IO,
                sclsvrCALIBRATOR_JO,
                sclsvrCALIBRATOR_KO,
                sclsvrCALIBRATOR_HO
            };
            logInfo("Computing diameter without absorption of star %s...",
                    starId);

            // Compute Angular Diameter
            if (ComputeAngularDiameter(mgI, mgJ, mgK, mgH,
                                       starPropertyId, mcsFALSE) ==
                mcsFAILURE)
            {
                return mcsFAILURE;
            }

            // Compute visibility and visibility error
            if (ComputeVisibility(request) == mcsFAILURE)
            {
                return mcsFAILURE;
            }

        }
        // Paralax unknown
        else
        {
            // Copy the current star in a temporary star in order to compute
            // with iterstellar absorption
            sclsvrCALIBRATOR testCalibratorWithAbsorption;
            testCalibratorWithAbsorption.Update(* this);
            
            // Set no interstellar absorption (i.e av=0)
            mcsFLOAT av = 0;
            char *magPropertyId[alxNB_BANDS] = 
            {
                vobsSTAR_PHOT_PHG_B,
                vobsSTAR_PHOT_JHN_V,
                vobsSTAR_PHOT_PHG_R,
                vobsSTAR_PHOT_PHG_I,
                sclsvrCALIBRATOR_PHOT_COUS_J,
                sclsvrCALIBRATOR_PHOT_COUS_H,
                sclsvrCALIBRATOR_PHOT_COUS_K,
                vobsSTAR_PHOT_JHN_L,
                vobsSTAR_PHOT_JHN_M
            };
            // Compute Interstellar extinction
            if (ComputeInterstellarAbsorption(magPropertyId, av) ==
                mcsFAILURE)
            {
                return mcsFAILURE;
            }
            
            // Compute Missing Magnitude
            if (ComputeMissingMagnitude(alxJ_BAND, alxK_BAND, mcsFALSE) ==
                mcsFAILURE)
            {
                return mcsFAILURE;
            }

            // Compute missing apparent magnitude
            if (ComputeApparentMagnitude(magPropertyId, av) ==
                mcsFAILURE)
            {
                return mcsFAILURE;
            }

            alxDATA mgI, mgJ, mgK, mgH;

            // Declare the 4 properties name for I0, J0, K0, H0
            char *starPropertyId[4] = 
            {
                sclsvrCALIBRATOR_IO,
                sclsvrCALIBRATOR_JO,
                sclsvrCALIBRATOR_KO,
                sclsvrCALIBRATOR_HO
            };
            logInfo("Computing diameter without absorption of star %s...",
                    starId);
            // Compute Angular Diameter
            if (ComputeAngularDiameter(mgI, mgJ, mgK, mgH,
                                       starPropertyId, mcsFALSE) == mcsFAILURE)
            {
                return mcsFAILURE;
            }
            // Compute visibility and visibility error
            if (ComputeVisibility(request) == mcsFAILURE)
            {
                return mcsFAILURE;
            }

            char *magPropertyIdTemp[alxNB_BANDS] = 
            {
                vobsSTAR_PHOT_PHG_B,
                vobsSTAR_PHOT_JHN_V,
                vobsSTAR_PHOT_PHG_R,
                vobsSTAR_PHOT_PHG_I,
                sclsvrCALIBRATOR_PHOT_COUS_J,
                sclsvrCALIBRATOR_PHOT_COUS_H,
                sclsvrCALIBRATOR_PHOT_COUS_K,
                vobsSTAR_PHOT_JHN_L,
                vobsSTAR_PHOT_JHN_M
            };
            
            // Do the same with the extinction ratio fixed to 3.0
            av = 3.0;
            if (testCalibratorWithAbsorption.
                ComputeInterstellarAbsorption(magPropertyIdTemp, av) ==
                mcsFAILURE)
            {
                return mcsFAILURE;
            }
           
            // Compute Missing Magnitude
            if (testCalibratorWithAbsorption.
                ComputeMissingMagnitude(alxJ_BAND, alxK_BAND, mcsFALSE) ==
                mcsFAILURE)
            {
                return mcsFAILURE;
            }

            // Compute missing apparent magnitude
            if (testCalibratorWithAbsorption.
                ComputeApparentMagnitude(magPropertyIdTemp, av) ==
                mcsFAILURE)
            {
                return mcsFAILURE;
            }


            alxDATA magI, magJ, magK, magH;

            // Declare the 4 properties name for I0, J0, K0, H0
            char *starPropertyId2[4] = 
            {
                sclsvrCALIBRATOR_IO,
                sclsvrCALIBRATOR_JO,
                sclsvrCALIBRATOR_KO,
                sclsvrCALIBRATOR_HO
            };
            logInfo("Computing diameter with absorption of star %s...",
                    starId);
            // Compute Angular Diameter
            if (testCalibratorWithAbsorption.
                ComputeAngularDiameter(magI, magJ, magK, magH,
                                       starPropertyId2,
                                       mcsFALSE) == mcsFAILURE)
            {
                return mcsFAILURE;
            }
            // Compute visibility and visibility error
            if (testCalibratorWithAbsorption.ComputeVisibility(request) ==
                mcsFAILURE)
            {
                return mcsFAILURE;
            }

            // Get Visibility of the star
            mcsFLOAT vis2;
            mcsFLOAT vis2Err;
            if (IsPropertySet(sclsvrCALIBRATOR_VIS2) == mcsTRUE)
            {
                if (GetPropertyValue(sclsvrCALIBRATOR_VIS2, &vis2) == mcsFAILURE)
                {
                    return mcsFAILURE;
                }
            }
            if (IsPropertySet(sclsvrCALIBRATOR_VIS2_ERROR) == mcsTRUE)
            {
                if (GetPropertyValue(sclsvrCALIBRATOR_VIS2_ERROR, &vis2Err) ==
                    mcsFAILURE)
                {
                    return mcsFAILURE;
                }
            }
            // Get Visibility of the temp star            
            mcsFLOAT vis2A;
            mcsFLOAT vis2ErrA;
            if (testCalibratorWithAbsorption.IsPropertySet(sclsvrCALIBRATOR_VIS2)
                == mcsTRUE)
            {
                if (testCalibratorWithAbsorption.
                    GetPropertyValue(sclsvrCALIBRATOR_VIS2, &vis2A) ==
                    mcsFAILURE)
                {
                    return mcsFAILURE;
                }
            }
            if (testCalibratorWithAbsorption.
                IsPropertySet(sclsvrCALIBRATOR_VIS2_ERROR) == mcsTRUE)
            {
                if (testCalibratorWithAbsorption.
                    GetPropertyValue(sclsvrCALIBRATOR_VIS2_ERROR, &vis2ErrA) ==
                    mcsFAILURE)
                {
                    return mcsFAILURE;
                }
            }
            
            // compute MAX(|vis2A - vis2|; vis2Err)
            logTest("vis2 = %f", vis2);
            logTest("vis2A = %f", vis2A);
            logTest("|vis2A - vis2| = %f", fabs(vis2A - vis2));
            logTest("vis2Err = %f", vis2Err);
            mcsFLOAT visibilityErr;
            if (fabs(vis2A - vis2) < vis2Err)
            {
                visibilityErr = vis2Err;
            }
            else
            {
                visibilityErr = fabs(vis2A - vis2);
            }
            logTest("visibilityErr = %f", visibilityErr); 
            // Test of validity of the visibility
            // Get the expected visibility
            mcsFLOAT expectedVisErr = request.GetExpectedVisErr();
            if (visibilityErr > expectedVisErr)
            {
                logInfo("The star %s will be removed because the computing visibility error (%f) is higher than the expected one (%f)",
                        starId, visibilityErr, expectedVisErr);
            }
        }

        // temporary all diameter are ok....
        if (SetPropertyValue(sclsvrCALIBRATOR_DIAM_FLAG, "OK", 
                             vobsSTAR_COMPUTED_PROP,vobsCONFIDENCE_HIGH,
                             mcsTRUE) == mcsFAILURE)
        {
            return mcsFAILURE;
        }
    }
    return mcsSUCCESS;
}


/*
 * Private methods
 */
/**
 * Compute missing magnitude.
 *
 * @param firstBandId first band identifier
 * @param secondBandId
 * @param isBright
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is
 * returned.
 */
mcsCOMPL_STAT sclsvrCALIBRATOR::ComputeMissingMagnitude(alxBAND firstBandId,
                                                        alxBAND secondBandId,
                                                        mcsLOGICAL isBright)
{
    logTrace("sclsvrCALIBRATOR::ComputeMissingMagnitude()");

    mcsSTRING32 starId;
                // Get Star ID
                if (GetId(starId, sizeof(starId)) == mcsFAILURE)
                {
                    return mcsFAILURE;
                }

    alxMAGNITUDES magnitudes;

    // WARNING: Property Id lists should be defined in the same order than
    // alxBAND enumerate. In order to be able to use this enumerate as index of
    // this list.
    char* mag0PropertyId[alxNB_BANDS] = 
    {
        sclsvrCALIBRATOR_BO,
        sclsvrCALIBRATOR_VO,
        sclsvrCALIBRATOR_RO,
        sclsvrCALIBRATOR_IO,
        sclsvrCALIBRATOR_JO,
        sclsvrCALIBRATOR_HO,
        sclsvrCALIBRATOR_KO,
        sclsvrCALIBRATOR_LO,
        sclsvrCALIBRATOR_MO
    };
    
    // Get the value of the magnitude in the different band
    // if B or V is not present, return mcsFAILURE
    if (IsPropertySet(mag0PropertyId[firstBandId]) == mcsTRUE)
    {
        if (GetPropertyValue(mag0PropertyId[firstBandId], 
                             &magnitudes[firstBandId].value) ==
            mcsFAILURE)
        {       
            return mcsFAILURE;
        }
        magnitudes[firstBandId].isSet=mcsTRUE;
    }

    // Get the value of magnitude V
    if (IsPropertySet(mag0PropertyId[secondBandId]) == mcsTRUE)
    {
        if (GetPropertyValue(mag0PropertyId[secondBandId], 
                             &magnitudes[secondBandId].value) == mcsFAILURE)
        {
            return mcsFAILURE;
        }

        magnitudes[secondBandId].isSet=mcsTRUE;
    }

    // For other magnitudes
    for (int band = 0; band < alxNB_BANDS; band++)
    { 
        // Get the current value
        if (IsPropertySet(mag0PropertyId[band]) == mcsTRUE)
        {
            if (GetPropertyValue(mag0PropertyId[band],
                                 &magnitudes[band].value) == mcsFAILURE)
            {
                return mcsFAILURE;
            }

            magnitudes[band].isSet=mcsTRUE;
        }
        else
        {
            magnitudes[band].isSet=mcsFALSE;
        }
    }

    //Get the value of the Spectral Type
    mcsSTRING32 spType;
    if (isBright == mcsTRUE)
    {
        strcpy(spType, GetPropertyValue(vobsSTAR_SPECT_TYPE_MK));

        // Compute missing magnitudes
        if (alxComputeMagnitudesForBrightStar(spType, magnitudes) == mcsFAILURE)
        {
            // if error found on spectral type, reset stack and return SUCCESS
            if ((errIsInStack("alx", alxERR_SPECTRAL_TYPE_NOT_FOUND) ==
                 mcsTRUE) ||
                (errIsInStack("alx", alxERR_WRONG_SPECTRAL_TYPE_FORMAT)
                 == mcsTRUE))
            {
                logInfo("star %s removed, spType = %s can't help to compute magnitude", starId, spType);

                errResetStack();
                return mcsSUCCESS;
            }
            else
            {
                return mcsFAILURE;
            }
        }
    }
    else
    {
        if (IsPropertySet(vobsSTAR_SPECT_TYPE_MK) == mcsTRUE)
        {
            strcpy(spType, GetPropertyValue(vobsSTAR_SPECT_TYPE_MK));

            // Compute missing magnitudes
            if (alxComputeMagnitudesForFaintStar(spType, magnitudes) ==
                mcsFAILURE)
            {
                // if error found on spectral type, reset stack and
                // return SUCCESS
                if ((errIsInStack("alx", alxERR_SPECTRAL_TYPE_NOT_FOUND) ==
                     mcsTRUE) ||
                    (errIsInStack("alx", alxERR_WRONG_SPECTRAL_TYPE_FORMAT) ==
                     mcsTRUE))
                {
                    logInfo("star %s removed, spType = %s can't help to compute magnitude", starId, spType);
                    errResetStack();
                    return mcsSUCCESS;
                }
                if (errIsInStack("alx", alxERR_CANNOT_COMPUTE_RATIO) ==
                     mcsTRUE)
                {
                    logInfo("star %s removed, the ratio in magnitude completion is not ok", starId, spType);
                    errResetStack();
                    return mcsSUCCESS;
                }
                else
                {
                    return mcsFAILURE;
                }
            }
        }
        else
        {
            // Compute missing magnitudes
            if (alxComputeMagnitudesForFaintStar(NULL, magnitudes) ==
                mcsFAILURE)
            {
                if (errIsInStack("alx", alxERR_DIFFJK_NOT_IN_TABLE) ==
                     mcsTRUE)
                {
                    logInfo("star %s removed, J-K is not in the table and/or can't be interpolate", starId, spType);
                    
                    errResetStack();
                    return mcsSUCCESS;
                }
                if (errIsInStack("alx", alxERR_CANNOT_COMPUTE_RATIO) ==
                     mcsTRUE)
                {
                    logInfo("star %s removed, the ratio in magnitude completion is not ok", starId, spType);
                    errResetStack();
                    return mcsSUCCESS;
                }
                if (errIsInStack("alx", alxERR_NO_LINE_FOUND) ==
                     mcsTRUE)
                {
                    logInfo("star %s removed, no line found in color table", starId);
                    errResetStack();
                    return mcsSUCCESS;
                }

                else
                {
                    return mcsFAILURE;
                }
            }
        }
    }
  
    // For each magnitude
    for (int band = 0; band < alxNB_BANDS; band++)
    { 
        /*
         * Set the computed magnitude.
         * Please note that if a magnitude is altready set, the
         * SetPropertyValue() does nothing; i.e. existing magnitudes are not
         * overwritten.
         */
        if (magnitudes[band].isSet == mcsTRUE)
        {
            if (SetPropertyValue(mag0PropertyId[band], magnitudes[band].value,
                                 vobsSTAR_COMPUTED_PROP, 
                            (vobsCONFIDENCE_INDEX)magnitudes[band].confIndex,
                            mcsFALSE) == mcsFAILURE)
            {
                return mcsFAILURE;
            }
        }
    }
    
    return mcsSUCCESS;
}

/**
 * Compute galactic coordonates.
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is
 * returned.
 */
mcsCOMPL_STAT sclsvrCALIBRATOR::ComputeGalacticCoordinates()
{
    logTrace("sclsvrCALIBRATOR::ComputeGalacticCoordinates()");

    mcsFLOAT gLat, gLon, ra, dec;

    // Get right ascension position in degree
    if (GetRa(ra)==mcsFAILURE)
    {
        return mcsFAILURE;
    }
    
    // Get declinaison in degree
    if (GetDec(dec)==mcsFAILURE)
    {
        return mcsFAILURE;
    }
    
    // Compute galactic coordinates from the retrieved ra and dec values
    if (alxComputeGalacticCoordinates(ra, dec, &gLat, &gLon) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    
    // Set the galactic lattitude (if not yet set)
    if (SetPropertyValue(vobsSTAR_POS_GAL_LAT, gLat, vobsSTAR_COMPUTED_PROP) ==
        mcsFAILURE)
    {
        return mcsFAILURE;
    }

    // Set the galactic longitude (if not yet set)
    if (SetPropertyValue(vobsSTAR_POS_GAL_LON, gLon, vobsSTAR_COMPUTED_PROP) ==
        mcsFAILURE)
    {
        return mcsFAILURE;
    }

    return mcsSUCCESS;
}

/**
 * Compute extinction coefficient
 *
 * @param av the extinction coefficeint
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is
 * returned.
 */
mcsCOMPL_STAT sclsvrCALIBRATOR::ComputeExtinctionCoefficient(mcsFLOAT *av)
{
    logTrace("sclsvrCALIBRATOR::ComputeExtinctionCoefficient()");

    mcsFLOAT paralax, gLat, gLon;
    
    // Get the value of the paralax
    if (IsPropertySet(vobsSTAR_POS_PARLX_TRIG) == mcsTRUE)
    {
        if (GetPropertyValue(vobsSTAR_POS_PARLX_TRIG, &paralax) == mcsFAILURE)
        {
            return mcsFAILURE;
        }
    }
    else 
    {
        errAdd(sclsvrERR_MISSING_PROPERTY, vobsSTAR_POS_PARLX_TRIG,
               "interstellar absorption");
        return mcsFAILURE;
    }

    // Get the value of the galactic lattitude
    if (IsPropertySet(vobsSTAR_POS_GAL_LAT) == mcsTRUE)
    {
        if (GetPropertyValue(vobsSTAR_POS_GAL_LAT, &gLat) == mcsFAILURE)
        {
            return mcsFAILURE;
        }
    }
    else
    {
        errAdd(sclsvrERR_MISSING_PROPERTY, vobsSTAR_POS_GAL_LAT,
               "interstellar absorption");
        return mcsFAILURE;
    }

    // Get the value of the galactic longitude
    if (IsPropertySet(vobsSTAR_POS_GAL_LON) == mcsTRUE)
    {
        if (GetPropertyValue(vobsSTAR_POS_GAL_LON, &gLon) == mcsFAILURE)
        {
            return mcsFAILURE;
        }
    }
    else 
    {
        errAdd(sclsvrERR_MISSING_PROPERTY, vobsSTAR_POS_GAL_LON,
               "interstellar absorption");
        return mcsFAILURE;
    }

    // Compute Extinction ratio
    if (alxComputeExtinctionCoefficient(av, paralax, gLat, gLon) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    return mcsSUCCESS;
}

/**
 * Compute interstellar absorption.
 * 
 * @param magPropertyId list of magnitude properties
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is
 * returned.
 */
mcsCOMPL_STAT sclsvrCALIBRATOR::
ComputeInterstellarAbsorption(char *magPropertyId[alxNB_BANDS],
                              mcsFLOAT av)
{
    logTrace("sclsvrCALIBRATOR::ComputeInterstellarAbsorption()");

    // WARNING: Property Id lists should be defined in the same order than
    // alxBAND enumerate. In order to be able to use this enumerate as index of
    // this list.
    char *mag0PropertyId[alxNB_BANDS] = 
    {
        sclsvrCALIBRATOR_BO,
        sclsvrCALIBRATOR_VO,
        sclsvrCALIBRATOR_RO,
        sclsvrCALIBRATOR_IO,
        sclsvrCALIBRATOR_JO,
        sclsvrCALIBRATOR_HO,
        sclsvrCALIBRATOR_KO,
        sclsvrCALIBRATOR_LO,
        sclsvrCALIBRATOR_MO
    };

    // For each magnitude
    alxMAGNITUDES magnitudes;
    for (int band = 0; band < alxNB_BANDS; band++)
    { 
        // Get the current value
        if (IsPropertySet(magPropertyId[band]) == mcsTRUE)
        {
            if (GetPropertyValue(magPropertyId[band],
                                 &magnitudes[band].value) == mcsFAILURE)
            {
                return mcsFAILURE;
            }

            magnitudes[band].isSet=mcsTRUE;
        }
        else
        {
            magnitudes[band].isSet=mcsFALSE;
        }
    }

    // Compute corrected magnitude
    if (alxComputeCorrectedMagnitudes(av, magnitudes) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    // For each magnitude
    for (int band = 0; band < alxNB_BANDS; band++)
    { 
        // Set the corrected magnitude
        if (magnitudes[band].isSet == mcsTRUE)
        {
            if (SetPropertyValue(mag0PropertyId[band], magnitudes[band].value,
                             vobsSTAR_COMPUTED_PROP) == mcsFAILURE)
            {
                return mcsFAILURE;
            }
        }
    }

    return mcsSUCCESS;
}

/**
 * Compute apparent magnitude which are mising
 *
 * @param av the extinction coefficient
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is
 * returned.
 */
mcsCOMPL_STAT 
sclsvrCALIBRATOR::ComputeApparentMagnitude(char *magPropertyId[alxNB_BANDS], 
                                           mcsFLOAT av)
{
    logTrace("sclsvrCALIBRATOR::ComputeApparentMagnitude()");
    
    // WARNING: Property Id lists should be defined in the same order than
    // alxBAND enumerate. In order to be able to use this enumerate as index of
    // this list.
    char *mag0PropertyId[alxNB_BANDS] = 
    {
        sclsvrCALIBRATOR_BO,
        sclsvrCALIBRATOR_VO,
        sclsvrCALIBRATOR_RO,
        sclsvrCALIBRATOR_IO,
        sclsvrCALIBRATOR_JO,
        sclsvrCALIBRATOR_HO,
        sclsvrCALIBRATOR_KO,
        sclsvrCALIBRATOR_LO,
        sclsvrCALIBRATOR_MO
    };

    // For each corrected magnitude
    alxMAGNITUDES magnitudes;
    for (int band = 0; band < alxNB_BANDS; band++)
    { 
        // Get the current value
        if (IsPropertySet(mag0PropertyId[band]) == mcsTRUE)
        {
            if (GetPropertyValue(mag0PropertyId[band],
                                 &magnitudes[band].value) == mcsFAILURE)
            {
                return mcsFAILURE;
            }

            magnitudes[band].isSet=mcsTRUE;
        }
        else
        {
            magnitudes[band].isSet=mcsFALSE;
        }
    }

    // Compute apparent magnitude
    if (alxComputeApparentMagnitudes(av, magnitudes) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    // For each magnitude
    for (int band = 0; band < alxNB_BANDS; band++)
    { 
        // Set the corrected magnitude
        if (magnitudes[band].isSet == mcsTRUE)
        {
            if (SetPropertyValue(magPropertyId[band], magnitudes[band].value,
                                 vobsSTAR_COMPUTED_PROP) == mcsFAILURE)
            {
                return mcsFAILURE;
            }
        }
    }

    return mcsSUCCESS;
}


/**
 * Compute angular diameter
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is
 * returned.
 */
mcsCOMPL_STAT sclsvrCALIBRATOR::ComputeAngularDiameter(alxDATA mg1,
                                                       alxDATA mg2,
                                                       alxDATA mg3,
                                                       alxDATA mg4,
                                                       char *starPropertyId[4],
                                                       mcsLOGICAL isBright)
{
    logTrace("sclsvrCALIBRATOR::ComputeAngularDiameter()");

    mcsSTRING32 starId;
    // Get Star ID
    if (GetId(starId, sizeof(starId)) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    alxDIAMETERS diameters;
    alxDATA starProperty[4];
    
    // For each property needed to compute angular diameter
    for (int i=0; i<4; i++)
    { 
        // if the current property is affected
        if (IsPropertySet(starPropertyId[i]) == mcsTRUE)
        {
            // Get the property
            vobsSTAR_PROPERTY *property=GetProperty(starPropertyId[i]);

            // Get the property value
            if ((property->GetValue(&starProperty[i].value)) == mcsFAILURE)
            {
                return mcsFAILURE;
            }

            // Falg the property as set
            starProperty[i].isSet = mcsTRUE;

            // Get the property confidence index
            starProperty[i].confIndex =
                            (alxCONFIDENCE_INDEX)property->GetConfidenceIndex();
        }
        else
        {
            logTest("Diameter of star %s can't be computed because magnitudes are missing", starId);
            // Do nothing
            return mcsSUCCESS;
        }
    }

    mg1=starProperty[0];
    mg2=starProperty[1];
    mg3=starProperty[2];
    mg4=starProperty[3];

    if (isBright == mcsTRUE)
    {
        // Compute angular diameters for bright star
        if (alxComputeAngularDiameterForBrightStar(mg1, mg2, mg3, mg4,
                                                   &diameters) == mcsFAILURE)
        {
            return mcsFAILURE;
        }

        // Set flag according to the confidence index 
        if (diameters.areComputed == mcsFALSE)
        {
            logInfo("star %s had not a coherant diameter because |diam - meanDiam > 2*meanDiamErr|", starId);
            
            if (SetPropertyValue(sclsvrCALIBRATOR_DIAM_FLAG, "NOK",
                                 vobsSTAR_COMPUTED_PROP) == mcsFAILURE)
            {
                return mcsFAILURE;
            }
        }
        else
        {
            if (SetPropertyValue(sclsvrCALIBRATOR_DIAM_FLAG,
                                 "OK", vobsSTAR_COMPUTED_PROP) == mcsFAILURE)
            {
                return mcsFAILURE;
            }

            // Set the computed value of the angular diameter
            if (SetPropertyValue(sclsvrCALIBRATOR_DIAM_BV, diameters.bv.value,
                                 vobsSTAR_COMPUTED_PROP, 
                                 (vobsCONFIDENCE_INDEX)diameters.confidenceIdx)
                == mcsFAILURE)
            {
                return mcsFAILURE;
            }
            if (SetPropertyValue(sclsvrCALIBRATOR_DIAM_VR, diameters.vr.value,
                                 vobsSTAR_COMPUTED_PROP, 
                                 (vobsCONFIDENCE_INDEX)diameters.confidenceIdx)
                == mcsFAILURE)
            {
                return mcsFAILURE;
            }
            if (SetPropertyValue(sclsvrCALIBRATOR_DIAM_VK, diameters.vk.value,
                                 vobsSTAR_COMPUTED_PROP, 
                                 (vobsCONFIDENCE_INDEX)diameters.confidenceIdx)
                == mcsFAILURE)
            {
                return mcsFAILURE;
            }
            if (SetPropertyValue(sclsvrCALIBRATOR_DIAM_BV_ERROR, 
                                 diameters.bvErr.value,
                                 vobsSTAR_COMPUTED_PROP, 
                                 (vobsCONFIDENCE_INDEX)diameters.confidenceIdx)
                == mcsFAILURE)
            {
                return mcsFAILURE;
            }
            if (SetPropertyValue(sclsvrCALIBRATOR_DIAM_VR_ERROR,
                                 diameters.vrErr.value,
                                 vobsSTAR_COMPUTED_PROP, 
                                 (vobsCONFIDENCE_INDEX)diameters.confidenceIdx)
                == mcsFAILURE)
            {
                return mcsFAILURE;
            }
            if (SetPropertyValue(sclsvrCALIBRATOR_DIAM_VK_ERROR,
                                 diameters.vkErr.value,
                                 vobsSTAR_COMPUTED_PROP, 
                                 (vobsCONFIDENCE_INDEX)diameters.confidenceIdx)
                == mcsFAILURE)
            {
                return mcsFAILURE;
            }
        }
    }
    else
    {
        // Compute angular diameters
        if (alxComputeAngularDiameterForFaintStar(mg1, mg2, mg3, mg4,
                                                  &diameters) == mcsFAILURE)
        {
            return mcsFAILURE;
        }

        // Set flag according to the confidence index 
        if (diameters.areComputed == mcsFALSE)
        {
            logInfo("star %s had not a coherant diameter because |diam - meanDiam| > meanDiamErr", starId);
            if (SetPropertyValue(sclsvrCALIBRATOR_DIAM_FLAG, "NOK",
                                 vobsSTAR_COMPUTED_PROP) == mcsFAILURE)
            {
                return mcsFAILURE;
            }
        }
        else
        {
            if (SetPropertyValue(sclsvrCALIBRATOR_DIAM_FLAG,
                                 "OK", vobsSTAR_COMPUTED_PROP) == mcsFAILURE)
            {
                return mcsFAILURE;
            }

            // Set the computed value of the angular diameter
            if (SetPropertyValue(sclsvrCALIBRATOR_DIAM_IJ, diameters.ij.value,
                                 vobsSTAR_COMPUTED_PROP, 
                                 (vobsCONFIDENCE_INDEX)diameters.confidenceIdx)
                == mcsFAILURE)
            {
                return mcsFAILURE;
            }
            if (SetPropertyValue(sclsvrCALIBRATOR_DIAM_IK, diameters.ik.value,
                                 vobsSTAR_COMPUTED_PROP, 
                                 (vobsCONFIDENCE_INDEX)diameters.confidenceIdx)
                == mcsFAILURE)
            {
                return mcsFAILURE;
            }
            if (SetPropertyValue(sclsvrCALIBRATOR_DIAM_JK, diameters.jk.value,
                                 vobsSTAR_COMPUTED_PROP, 
                                 (vobsCONFIDENCE_INDEX)diameters.confidenceIdx)
                == mcsFAILURE)
            {
                return mcsFAILURE;
            }
            if (SetPropertyValue(sclsvrCALIBRATOR_DIAM_JH, diameters.jh.value,
                                 vobsSTAR_COMPUTED_PROP, 
                                 (vobsCONFIDENCE_INDEX)diameters.confidenceIdx)
                == mcsFAILURE)
            {
                return mcsFAILURE;
            }
            if (SetPropertyValue(sclsvrCALIBRATOR_DIAM_MEAN,
                                 diameters.mean.value,
                                 vobsSTAR_COMPUTED_PROP, 
                                 (vobsCONFIDENCE_INDEX)diameters.confidenceIdx)
                == mcsFAILURE)
            {
                return mcsFAILURE;
            }
            if (SetPropertyValue(sclsvrCALIBRATOR_DIAM_IJ_ERROR, 
                                 diameters.ijErr.value,
                                 vobsSTAR_COMPUTED_PROP, 
                                 (vobsCONFIDENCE_INDEX)diameters.confidenceIdx)
                == mcsFAILURE)
            {
                return mcsFAILURE;
            }
            if (SetPropertyValue(sclsvrCALIBRATOR_DIAM_IK_ERROR,
                                 diameters.ikErr.value,
                                 vobsSTAR_COMPUTED_PROP, 
                                 (vobsCONFIDENCE_INDEX)diameters.confidenceIdx)
                == mcsFAILURE)
            {
                return mcsFAILURE;
            }
            if (SetPropertyValue(sclsvrCALIBRATOR_DIAM_JK_ERROR,
                                 diameters.jkErr.value,
                                 vobsSTAR_COMPUTED_PROP, 
                                 (vobsCONFIDENCE_INDEX)diameters.confidenceIdx)
                == mcsFAILURE)
            {
                return mcsFAILURE;
            }
            if (SetPropertyValue(sclsvrCALIBRATOR_DIAM_JH_ERROR,
                                 diameters.jhErr.value,
                                 vobsSTAR_COMPUTED_PROP, 
                                 (vobsCONFIDENCE_INDEX)diameters.confidenceIdx)
                == mcsFAILURE)
            {
                return mcsFAILURE;
            }
            if (SetPropertyValue(sclsvrCALIBRATOR_DIAM_MEAN_ERROR,
                                 diameters.meanErr.value,
                                 vobsSTAR_COMPUTED_PROP, 
                                 (vobsCONFIDENCE_INDEX)diameters.confidenceIdx)
                == mcsFAILURE)
            {
                return mcsFAILURE;
            }
        }

    }
   
    return mcsSUCCESS;
}


/**
 * Compute visibility.
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is
 * returned.
 */
mcsCOMPL_STAT sclsvrCALIBRATOR::ComputeVisibility(sclsvrREQUEST &request)
{
    logTrace("sclsvrCALIBRATOR::ComputeVisibility()");

    mcsFLOAT diam, diamError, baseMax, wavelength;
    alxVISIBILITIES visibilities;
    vobsCONFIDENCE_INDEX confidenceIndex = vobsCONFIDENCE_HIGH;

    // Get object diameter. First look at the diameters coming from catalog
    char *diamId[4][2] = 
    {
        {vobsSTAR_UD_DIAM, vobsSTAR_UD_DIAM_ERROR},
        {vobsSTAR_LD_DIAM, vobsSTAR_LD_DIAM_ERROR},
        {vobsSTAR_UDDK_DIAM, vobsSTAR_UDDK_DIAM_ERROR},
        {vobsSTAR_DIAM12, vobsSTAR_DIAM12_ERROR}
    };

    // For each possible diameters
    mcsLOGICAL found = mcsFALSE;
    for (int i = 0; (i < 4) && (found == mcsFALSE); i++)
    {
        // If diameter and its error are set 
        if ((IsPropertySet(diamId[i][0]) == mcsTRUE) &&
            (IsPropertySet(diamId[i][1]) == mcsTRUE))
        {
            // Get values
            if (GetPropertyValue(diamId[i][0], &diam) == mcsFAILURE)
            {
                return mcsFAILURE;
            }
            if (GetPropertyValue(diamId[i][1], &diamError) == mcsFAILURE)
            {
                return mcsFAILURE;
            }
            found = mcsTRUE;

            // Set confidence index to high (value coming form catalog)
            confidenceIndex = vobsCONFIDENCE_HIGH;
        }
    }

    // If not found in catalog, use the computed one (if exist)
    if (found == mcsFALSE)
    {
        if (request.IsBright() == mcsTRUE)
        {
            // If computed diameter is OK
            if (IsDiameterOk() == mcsTRUE)
            {
                // Get V-K diameter and associated error value
                if (GetPropertyValue(sclsvrCALIBRATOR_DIAM_VK, &diam) ==
                    mcsFAILURE)
                {
                    return mcsFAILURE;
                }
                if (GetPropertyValue(sclsvrCALIBRATOR_DIAM_VK_ERROR,
                                     &diamError) == mcsFAILURE)
                {
                    return mcsFAILURE;
                }

                // Get confidence index of computed diameter
                vobsSTAR_PROPERTY *property;
                property = GetProperty(sclsvrCALIBRATOR_DIAM_VK);
                confidenceIndex = property->GetConfidenceIndex();
            }
            // Else do not compute visibility
            else
            {
                return mcsSUCCESS;
            }
        }
        // Else do not compute visibility
        else
        {
            // If computed diameter is OK
            if (IsDiameterOk() == mcsTRUE)
            {
                // Get V-K diameter and associated error value
                if (GetPropertyValue(sclsvrCALIBRATOR_DIAM_JK, &diam) ==
                    mcsFAILURE)
                {
                    return mcsFAILURE;
                }
                if (GetPropertyValue(sclsvrCALIBRATOR_DIAM_JK_ERROR,
                                     &diamError) == mcsFAILURE)
                {
                    return mcsFAILURE;
                }

                // Get confidence index of computed diameter
                vobsSTAR_PROPERTY *property;
                property = GetProperty(sclsvrCALIBRATOR_DIAM_JK);
                confidenceIndex = property->GetConfidenceIndex();
            }
            // Else do not compute visibility
            else
            {
                return mcsSUCCESS;
            }

        }
    }

    // Get value in request of the wavelength
    wavelength = request.GetObservingWlen();

    // Get value in request of the base max
    baseMax = request.GetMaxBaselineLength();
    if (alxComputeVisibility(diam, diamError, baseMax, wavelength,
                             &visibilities) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
     
    // Affect visibility property
    if (SetPropertyValue(sclsvrCALIBRATOR_VIS2, visibilities.vis2,
                         vobsSTAR_COMPUTED_PROP, confidenceIndex) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    if (SetPropertyValue(sclsvrCALIBRATOR_VIS2_ERROR, visibilities.vis2Error,
                         vobsSTAR_COMPUTED_PROP, confidenceIndex) == 
        mcsFAILURE)
    {
        return mcsFAILURE;
    }

    // If the observed band is N, computed visibility with wlen = 8 and 13 um
    if (strcmp(request.GetSearchBand(), "N") == 0)
    {
        if (alxComputeVisibility(diam, diamError, baseMax, 8,
                                 &visibilities) == mcsFAILURE)
        {
            return mcsFAILURE;
        }

        // Affect visibility property
        if (SetPropertyValue(sclsvrCALIBRATOR_VIS2_8, visibilities.vis2,
                             vobsSTAR_COMPUTED_PROP) == mcsFAILURE)
        {
            return mcsFAILURE;
        }
        if (SetPropertyValue(sclsvrCALIBRATOR_VIS2_8_ERROR, 
                             visibilities.vis2Error,
                             vobsSTAR_COMPUTED_PROP, confidenceIndex) ==
            mcsFAILURE)
        {
            return mcsFAILURE;
        }

        if (alxComputeVisibility(diam, diamError, baseMax, 13,
                                 &visibilities)==mcsFAILURE)
        {
            return mcsFAILURE;
        }

        // Affect visibility property
        if (SetPropertyValue(sclsvrCALIBRATOR_VIS2_13, visibilities.vis2,
                             vobsSTAR_COMPUTED_PROP) == mcsFAILURE)
        {
            return mcsFAILURE;
        }
        if (SetPropertyValue(sclsvrCALIBRATOR_VIS2_13_ERROR,
                             visibilities.vis2Error,
                             vobsSTAR_COMPUTED_PROP, confidenceIndex) == 
            mcsFAILURE)
        {
            return mcsFAILURE;
        }
    }

    return mcsSUCCESS;
}

/**
 * Compute multiplicity.
 *
 * @return Always mcsSUCCESS.
 */
mcsCOMPL_STAT sclsvrCALIBRATOR::ComputeMultiplicity()
{
    logTrace("sclsvrCALIBRATOR::ComputeMultiplicity()");

    //SetPropertyValue(sclsvrCALIBRATOR_MULTIPLICITY, "1.00");

    return mcsSUCCESS;
}

/**
 * Compute distance.
 *
 * This method calculate the distance between the calibrator and the science
 * object.
 *
 * @return Always mcsSUCCESS.
 */
mcsCOMPL_STAT sclsvrCALIBRATOR::ComputeDistance(sclsvrREQUEST &request)
{
    logTrace("sclsvrCALIBRATOR::ComputeDistance()");

    mcsFLOAT scienceObjectRa  = 0;
    mcsFLOAT scienceObjectDec = 0;
    mcsFLOAT calibratorRa     = 0;
    mcsFLOAT calibratorDec    = 0;
    mcsFLOAT distance         = 0;
    const char* buffer        = NULL;
    vobsSTAR scienceObject;

    // Get the science object right ascension as a C string
    buffer = request.GetObjectRa();
    if (buffer == NULL)
    {
        return mcsFAILURE;
    }
    // Convert science object right ascension from hhmmss to arcsec
    // using hidden converter embedded in vobsStar constructor
    if (scienceObject.SetPropertyValue(vobsSTAR_POS_EQ_RA_MAIN, buffer, "")
        == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    // Get the science object origin right acsension in arcsec
    if (scienceObject.GetRa(scienceObjectRa) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    // Get the science object declinaison as a C string
    buffer = request.GetObjectDec();
    if (buffer == NULL)
    {
        return mcsFAILURE;
    }
    // Convert science object science object from hhmmss to arcsec
    // using hidden converter embedded in vobsStar constructor
    if (scienceObject.SetPropertyValue(vobsSTAR_POS_EQ_DEC_MAIN, buffer, "")
        == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    // Get the science object origin declinaison in arcsec
    if (scienceObject.GetDec(scienceObjectDec) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    // Get the internal calibrator right acsension in arcsec
    if (GetRa(calibratorRa) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    // Get the internal calibrator declinaison in arcsec
    if (GetDec(calibratorDec) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    // Compute the distance in arcsec between the science object and the
    // calibrator using an alx provided function
    if( alxComputeDistance(scienceObjectRa, scienceObjectDec,
                           calibratorRa,    calibratorDec,
                           &distance) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    // Put the computed distance in the corresponding calibrator property
    if (SetPropertyValue(sclsvrCALIBRATOR_DIST, distance, "") == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    return mcsSUCCESS;
}

/**
 * Convert magnitude in cousin
 *
 * @return  mcsSUCCESS on successful completion. Otherwise mcsFAILURE is
 * returned.
 */
mcsCOMPL_STAT sclsvrCALIBRATOR::ComputeCousinMagnitude()
{
    logTrace("sclsvrCALIBRATOR::ComputeCousinMagnitude()");

    // convert J H K mag Jcous hcous Kcous
    mcsFLOAT magK;
    mcsFLOAT magJ;
    mcsFLOAT magH;
    mcsFLOAT magKcous;
    mcsFLOAT magJcous;
    mcsFLOAT magHcous;

    // if Kmag property is set
    if (IsPropertySet(vobsSTAR_PHOT_JHN_K) == mcsTRUE)
    {
        // retreive it
        if (GetPropertyValue(vobsSTAR_PHOT_JHN_K, &magK) == mcsFAILURE)
        {       
            return mcsFAILURE;
        }
    }
    // if Jmag property is set        
    if (IsPropertySet(vobsSTAR_PHOT_JHN_J) == mcsTRUE)
    {
        // retreive it
        if (GetPropertyValue(vobsSTAR_PHOT_JHN_J, &magJ) == mcsFAILURE)
        {       
            return mcsFAILURE;
        }
    }
    // if Hmag property is set
    if (IsPropertySet(vobsSTAR_PHOT_JHN_H) == mcsTRUE)
    {
        // retreive it
        if (GetPropertyValue(vobsSTAR_PHOT_JHN_H, &magH) == mcsFAILURE)
        {       
            return mcsFAILURE;
        }
    }

    // Convert magnitude
    magKcous = magK + 0.024;
    magJcous = 0.947 * magJ + 0.053 * magK + 0.036;
    magHcous = 0.975 * magH + 0.025 * magK - 0.04;

    // Set Kcous
    if (SetPropertyValue(sclsvrCALIBRATOR_PHOT_COUS_K, magKcous, 
                         vobsSTAR_COMPUTED_PROP) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    // Set Jcous
    if (SetPropertyValue(sclsvrCALIBRATOR_PHOT_COUS_J, magJcous, 
                         vobsSTAR_COMPUTED_PROP) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    // Set Hcous
    if (SetPropertyValue(sclsvrCALIBRATOR_PHOT_COUS_H, magHcous, 
                         vobsSTAR_COMPUTED_PROP) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    return mcsSUCCESS;
}

/**
 * Add all star properties 
 *
 * @return Always mcsSUCCESS.
 */
mcsCOMPL_STAT sclsvrCALIBRATOR::AddProperties(void)
{
    //logTrace("vobsSTAR::AddProperties()");

    AddProperty(sclsvrCALIBRATOR_PHOT_COUS_J, "Jcous", vobsFLOAT_PROPERTY,
                "mag", "%.3f");
    AddProperty(sclsvrCALIBRATOR_PHOT_COUS_H, "Hcous", vobsFLOAT_PROPERTY,
                "mag", "%.3f");
    AddProperty(sclsvrCALIBRATOR_PHOT_COUS_K, "Kcous", vobsFLOAT_PROPERTY,
                "mag", "%.3f");
    AddProperty(sclsvrCALIBRATOR_DIAM_BV, "diam_bv", vobsFLOAT_PROPERTY, "-", 
                "%.3f", NULL, "diameter b-v");
    AddProperty(sclsvrCALIBRATOR_DIAM_VR, "diam_vr", vobsFLOAT_PROPERTY, "-", 
                "%.3f");
    AddProperty(sclsvrCALIBRATOR_DIAM_VK, "diam_vk", vobsFLOAT_PROPERTY, "-", 
                "%.3f");
    AddProperty(sclsvrCALIBRATOR_DIAM_IJ, "diam_ij", vobsFLOAT_PROPERTY, "-", 
                "%.3f");
    AddProperty(sclsvrCALIBRATOR_DIAM_IK, "diam_ik", vobsFLOAT_PROPERTY, "-", 
                "%.3f");
    AddProperty(sclsvrCALIBRATOR_DIAM_JK, "diam_jk", vobsFLOAT_PROPERTY, "-", 
                "%.3f");
    AddProperty(sclsvrCALIBRATOR_DIAM_JH, "diam_jh", vobsFLOAT_PROPERTY, "-", 
                "%.3f");
    AddProperty(sclsvrCALIBRATOR_DIAM_MEAN, "diam_mean", vobsFLOAT_PROPERTY, 
                "-", "%.3f");
    AddProperty(sclsvrCALIBRATOR_DIAM_BV_ERROR, "e_diam_bv", 
                vobsFLOAT_PROPERTY, "-", "%.3f");
    AddProperty(sclsvrCALIBRATOR_DIAM_VR_ERROR, "e_diam_vr", 
                vobsFLOAT_PROPERTY, "-", "%.3f");
    AddProperty(sclsvrCALIBRATOR_DIAM_VK_ERROR, "e_diam_vk", 
                vobsFLOAT_PROPERTY, "-", "%.3f");
    AddProperty(sclsvrCALIBRATOR_DIAM_IJ_ERROR, "e_diam_ij", 
                vobsFLOAT_PROPERTY, "-", "%.3f");
    AddProperty(sclsvrCALIBRATOR_DIAM_IK_ERROR, "e_diam_ik", 
                vobsFLOAT_PROPERTY, "-", "%.3f");
    AddProperty(sclsvrCALIBRATOR_DIAM_JK_ERROR, "e_diam_jk", 
                vobsFLOAT_PROPERTY, "-", "%.3f");
    AddProperty(sclsvrCALIBRATOR_DIAM_JH_ERROR, "e_diam_jh", 
                vobsFLOAT_PROPERTY, "-", "%.3f");
    AddProperty(sclsvrCALIBRATOR_DIAM_MEAN_ERROR, "e_diam_mean", 
                vobsFLOAT_PROPERTY, "-", "%.3f");
    AddProperty(sclsvrCALIBRATOR_DIAM_FLAG, "diamFlag", 
                vobsSTRING_PROPERTY, "-");
    AddProperty(sclsvrCALIBRATOR_MO, "Mo", vobsFLOAT_PROPERTY, "mag", "%.3f");
    AddProperty(sclsvrCALIBRATOR_LO, "Lo", vobsFLOAT_PROPERTY, "mag", "%.3f");
    AddProperty(sclsvrCALIBRATOR_KO, "Ko", vobsFLOAT_PROPERTY, "mag", "%.3f");
    AddProperty(sclsvrCALIBRATOR_HO, "Ho", vobsFLOAT_PROPERTY, "mag", "%.3f");
    AddProperty(sclsvrCALIBRATOR_JO, "Jo", vobsFLOAT_PROPERTY, "mag", "%.3f");
    AddProperty(sclsvrCALIBRATOR_IO, "Io", vobsFLOAT_PROPERTY, "mag", "%.3f");
    AddProperty(sclsvrCALIBRATOR_RO, "Ro", vobsFLOAT_PROPERTY, "mag", "%.3f");
    AddProperty(sclsvrCALIBRATOR_VO, "Vo", vobsFLOAT_PROPERTY, "mag", "%.3f");
    AddProperty(sclsvrCALIBRATOR_BO, "Bo", vobsFLOAT_PROPERTY, "mag", "%.3f");
    AddProperty(sclsvrCALIBRATOR_VIS2, "vis2", vobsFLOAT_PROPERTY, "-", "%.3f");
    AddProperty(sclsvrCALIBRATOR_VIS2_ERROR, "vis2Err", 
                vobsFLOAT_PROPERTY, "-", "%.3f");
    AddProperty(sclsvrCALIBRATOR_VIS2_8, "vis2(8mu)", vobsFLOAT_PROPERTY, "-", "%.3f");
    AddProperty(sclsvrCALIBRATOR_VIS2_8_ERROR, "vis2Err(8mu)", 
                vobsFLOAT_PROPERTY, "-", "%.3f");
    AddProperty(sclsvrCALIBRATOR_VIS2_13, "vis2(13mu)", vobsFLOAT_PROPERTY, "-", "%.3f");
    AddProperty(sclsvrCALIBRATOR_VIS2_13_ERROR, "vis2Err(13mu)", 
                vobsFLOAT_PROPERTY, "-", "%.3f");
    AddProperty(sclsvrCALIBRATOR_VIS2_FLAG, "vis2Flag", vobsSTRING_PROPERTY, "-");
    AddProperty(sclsvrCALIBRATOR_DIST, "dist", vobsFLOAT_PROPERTY, "-", "%.3f");

    return mcsSUCCESS;
}

/*___oOo___*/
