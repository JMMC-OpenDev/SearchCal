/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: sclsvrCALIBRATOR.cpp,v 1.100 2010-02-02 10:16:50 lafrasse Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.99  2010/01/28 17:36:40  lafrasse
 * Removed 'DIAM_VK' error while computing UD diameter (prop. not set).
 * Enhanced log and output.
 *
 * Revision 1.98  2010/01/28 16:45:49  lafrasse
 * Added UD_U property.
 *
 * Revision 1.97  2010/01/28 16:34:06  lafrasse
 * Removed no more needed UD diameter structure initialization.
 *
 * Revision 1.96  2010/01/22 15:49:05  mella
 * factorization of diameters computation to be refined
 *
 * Revision 1.95  2010/01/22 15:35:29  lafrasse
 * Log refinments.
 *
 * Revision 1.94  2010/01/11 17:21:42  lafrasse
 * Added UD diameters columns, and a way to compute them.
 *
 * Revision 1.93  2009/04/17 15:28:10  lafrasse
 * Updated log level from Info to Test to clean sclws log output.
 *
 * Revision 1.92  2007/11/14 15:52:03  gzins
 * Set DIAM_FLAG to OK when diameter is coming from catalog
 *
 * Revision 1.91  2007/05/11 15:49:57  gzins
 * Ignored I magnitude from DENIS catalog when saturated (faint case)
 * Computed V-K diameter when V is coming from I/280 (faint case)
 * Added test on parallax error (Perr/P must be less than 25%) (bright case)
 * Corrected K magnitude when read from 2mass, Merand or J-K Denis catalogs
 *
 * Revision 1.90  2006/08/25 06:19:33  gzins
 * Fixed bug when computing Cousin magnitudes; they were computed even if Johnson magnitudes were not set.
 * Fixed test on I magnitude to know whether it came from CDS or it is computed
 *
 * Revision 1.89  2006/08/23 13:53:33  gzins
 * Minor change in logged message
 *
 * Revision 1.88  2006/08/23 12:06:48  gzins
 * Removed useless parameters in ComputeMissingMagnitude
 * Fixed bug when computing diameters; case I unknown was not properly handled
 * No longer set diameter IJ and IK when I is unknown
 * No longer set HK diameter when I is known
 *
 * Revision 1.87  2006/07/17 09:12:20  scetre
 * Added HK diameter
 * Modify the angular diameter coputation if magI is unknown
 *
 * Revision 1.86  2006/07/13 13:30:56  gzins
 * Fixed potential problem with tat; uninitialised magnitude value
 *
 * Revision 1.85  2006/04/19 12:06:02  gzins
 * Changed areComputed to areCoherent (for diameters)
 * Set diameter properties even if not coherent
 *
 * Revision 1.84  2006/04/14 06:46:38  gzins
 * Minor changes related to log messages
 *
 * Revision 1.83  2006/04/11 08:39:41  gzins
 * Minor change in comment
 *
 * Revision 1.82  2006/04/11 07:42:51  gzins
 * Added parallax error in log messages
 *
 * Revision 1.81  2006/04/10 14:50:42  gzins
 * Clear parallax and its associated error when is invalid
 *
 * Revision 1.80  2006/04/10 12:05:41  gzins
 * Removed IsSuitable() and ComputeMultiplicity() (not needed)
 * Added some log message when completing star properties
 * Stopped computation sequence for faint star when computation step fails
 *
 * Revision 1.79  2006/04/07 08:36:33  gzins
 * Removed specific handling of 'alxERR_CANNOT_COMPUTE_RATIO' error
 *
 * Revision 1.78  2006/04/07 06:44:34  gzins
 * Set confidence index to the magnitudes used for computation
 *
 * Revision 1.77  2006/04/05 15:18:03  gzins
 * Added 'computed' when storing distance to science object
 *
 * Revision 1.76  2006/04/03 08:58:24  swmgr
 * Fixed bug in copy constructor
 *
 * Revision 1.75  2006/03/28 13:49:18  gzins
 * Added IsSuitable method
 *
 * Revision 1.74  2006/03/28 10:04:33  gzins
 * Fixed Hcous magnitude computation
 *
 * Revision 1.73  2006/03/27 08:49:57  scetre
 * Changed phg to cous for I mag
 *
 * Revision 1.72  2006/03/22 10:45:00  scetre
 * Fixed bug with parallax test in faint case
 *
 * Revision 1.71  2006/03/07 07:53:14  scetre
 * Added distance computing for faint stars
 *
 * Revision 1.70  2006/03/03 15:25:23  scetre
 * Changed rcsId to rcsId __attribute__ ((unused))
 *
 * Revision 1.69  2006/01/23 14:12:25  scetre
 * Added Av parameter as a property of a calibrator
 *
 * Revision 1.68  2006/01/18 16:01:09  scetre
 * Use the confidence index of corrected magnitude in the computed apparent magnitudes
 *
 * Revision 1.67  2006/01/18 08:53:03  scetre
 * Convert distance from arcsec to degree
 *
 * Revision 1.66  2006/01/09 16:11:26  lafrasse
 * Updated property description to reflect vobsSTAR API change
 *
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
 * Added test on parallax; do not compute magnitudes and angular diameter when parallax < 1 mas
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

 static char *rcsId __attribute__ ((unused))="@(#) $Id: sclsvrCALIBRATOR.cpp,v 1.100 2010-02-02 10:16:50 lafrasse Exp $"; 


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
 * Return whether the calibrator has a coherent diameter or not.
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
 * Compute real magnitudes, missing magnitudes and the angular diameter for
 * stars with parallax.
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is
 * returned.
 */
mcsCOMPL_STAT sclsvrCALIBRATOR::CompleteWithParallax(mcsLOGICAL isBright)
{
    logTrace("sclsvrCALIBRATOR::CompleteWithParallax()");

    // Get the extinction ratio
    if (ComputeExtinctionCoefficient() == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    // Compute Interstellar extinction
    if (ComputeInterstellarAbsorption(isBright) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    // Compute missing Magnitude
    if (ComputeMissingMagnitude(isBright) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    // Compute apparent magnitude
    if (ComputeApparentMagnitude(isBright) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    // Compute Angular Diameter
    if (ComputeAngularDiameter(isBright) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    return mcsSUCCESS;
}



/**
 * Compute real magnitudes, missing magnitudes and the angular diameter for
 * stars without parallax.
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is
 * returned.
 */
mcsCOMPL_STAT sclsvrCALIBRATOR::CompleteWithoutParallax()
{
    logTrace("sclsvrCALIBRATOR::CompleteWithoutParallax()");
    mcsLOGICAL isBright = mcsFALSE;

    // Compute Interstellar extinction
    if (ComputeInterstellarAbsorption(isBright) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    // Compute missing Magnitude
    if (ComputeMissingMagnitude(isBright) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    // Compute apparent magnitude
    if (ComputeApparentMagnitude(isBright) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    // Compute Angular Diameter
    if (ComputeAngularDiameter(isBright) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    return mcsSUCCESS;
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

    // Check flag related to I magnitude
    // Note (2):
    // This flag is the concatenation of image and source flags, in hexadecimal
    // format.
    // For the image flag, the first two digits contain:
    // Bit 0 (0100) clouds during observation
    // Bit 1 (0200) electronic Read-Out problem
    // Bit 2 (0400) internal temperature problem
    // Bit 3 (0800) very bright star
    // Bit 4 (1000) bright star
    // Bit 5 (2000) stray light
    // Bit 6 (4000) unknown problem
    // For the source flag, the last two digits contain:
    // Bit 0 (0001) source might be a dust on mirror
    // Bit 1 (0002) source is a ghost detection of a bright star
    // Bit 2 (0004) source is saturated
    // Bit 3 (0008) source is multiple detect
    // Bit 4 (0010) reserved
    if (IsPropertySet(vobsSTAR_CODE_MISC_I) == mcsTRUE)
    {
        // Get Iflg value as string
        mcsSTRING32 IflgStr;
        strcpy(IflgStr, GetPropertyValue(vobsSTAR_CODE_MISC_I));

        // Convert it into integer; hexadecimal conversion
        int Iflg;
        sscanf(IflgStr, "%x", &Iflg);

        // Check if it is saturated or ther was clouds during observation
        if (((Iflg & 0x4) != 0) || ((Iflg & 0x100) != 0))
        {
            // Do not used I magnitude coming from Denis
            ClearPropertyValue(vobsSTAR_PHOT_COUS_I);
            ClearPropertyValue(vobsSTAR_CODE_MISC_I);
        }
    }

    // Check parallax
    mcsFLOAT parallax;
    mcsLOGICAL parallaxIsOK = mcsFALSE;
    // If parallax of the star if known
    if (IsPropertySet(vobsSTAR_POS_PARLX_TRIG) == mcsTRUE)
    {
        // Check parallax
        mcsFLOAT parallaxError  = -1.0;
        GetPropertyValue(vobsSTAR_POS_PARLX_TRIG, &parallax);

        // Get error
        if (IsPropertySet(vobsSTAR_POS_PARLX_TRIG_ERROR) == mcsTRUE)
        {
            GetPropertyValue(vobsSTAR_POS_PARLX_TRIG_ERROR, &parallaxError);

            // If parallax is negative 
            if (parallax <= 0) 
            {
                if (request.IsBright() == mcsTRUE)
                {
                    logTest("star %s - parallax %.2f(%.2f) is not valid; "
                            "could not compute diameter", starId, 
                            parallax, parallaxError);
                }
                else
                {
                    logTest("star %s - parallax %.2f(%.2f) is not valid...",
                            starId, parallax, parallaxError);
                }            
                // Clear parallax values; invalid parallax is not shown to user
                ClearPropertyValue(vobsSTAR_POS_PARLX_TRIG);
                ClearPropertyValue(vobsSTAR_POS_PARLX_TRIG_ERROR);
            }
            // If parallax is less than 1 mas 
            else if (parallax < 1) 
            {
                if (request.IsBright() == mcsTRUE)
                {
                    logTest("star %s - parallax %.2f(%.2f) less than 1 mas; "
                            "could not compute diameter", starId, 
                            parallax, parallaxError);
                }
                else
                {
                    logTest("star %s - parallax %.2f(%.2f) less than 1 mas...",
                            starId, parallax, parallaxError);
                }            
                // Clear parallax values; invalid parallax is not shown to user
                ClearPropertyValue(vobsSTAR_POS_PARLX_TRIG);
                ClearPropertyValue(vobsSTAR_POS_PARLX_TRIG_ERROR);
            }
            // If parallax error is invalid 
            else if (parallaxError <= 0.0) 
            {
                if (request.IsBright() == mcsTRUE)
                {
                    logTest("star %s - parallax error %.2f is not valid; "
                            "could not compute diameter", starId, 
                            parallaxError);
                }
                else
                {
                    logTest("star %s - parallax error %.2f is not valid...", 
                            starId, parallaxError);
                }
                // Clear parallax values; invalid parallax is not shown to user
                ClearPropertyValue(vobsSTAR_POS_PARLX_TRIG);
                ClearPropertyValue(vobsSTAR_POS_PARLX_TRIG_ERROR);
            }
            // If parallax error is too high 
            else if ((parallaxError / parallax) >= 0.25)
            {
                if (request.IsBright() == mcsTRUE)
                {
                    logTest("star %s - parallax %.2f(%.2f) is not valid; "
                            "could not compute diameter", starId, 
                            parallax, parallaxError);
                }
                else
                {
                    logTest("star %s - parallax %.2f(%.2f) is not valid...", 
                            starId, parallax, parallaxError);
                }
                // Clear parallax values; invalid parallax is not shown to user
                ClearPropertyValue(vobsSTAR_POS_PARLX_TRIG);
                ClearPropertyValue(vobsSTAR_POS_PARLX_TRIG_ERROR);
            }
            // parallax OK
            else
            {
                parallaxIsOK = mcsTRUE;
                logTest("star %s - parallax %.2f(%.2f) is OK...", starId, 
                        parallax, parallaxError);
            }                
        }
        // If parallax error is unknown 
        else
        {
            if (request.IsBright() == mcsTRUE)
            {
                logTest("star %s - parallax error is unknown; "
                        "could not compute diameter", starId);
            }
            else
            {
                logTest("star %s - parallax error is unknown...", starId);
            }
            // Clear parallax value; invalid parallax is not shown to user
            ClearPropertyValue(vobsSTAR_POS_PARLX_TRIG);
        }
    }
    // parallax is unknown 
    else
    {
        if (request.IsBright() == mcsTRUE)
        {
            logTest("star %s - parallax is unknown; "
                    "could not compute diameter", starId); 
        }
        else
        {
            logTest("star %s - parallax is unknown", starId);
        }
    }

    // If the request should return bright starts
    if (request.IsBright() == mcsTRUE)
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
            // If parallax of the star if known
            if (parallaxIsOK == mcsTRUE)
            {
                CompleteWithParallax(mcsTRUE);
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
    }
    // If the researh is faint
    else
    {
        // Compute Galactic coordinates
        if (ComputeGalacticCoordinates() == mcsFAILURE)
        {
            return mcsFAILURE;
        }

        // Convert in cousin magnitude
        if (ComputeCousinMagnitude() == mcsFAILURE)
        {
            return mcsFAILURE;
        }

        // If parallax is OK, compute real magnitudes, missing magnitudes and the
        // angular diameter
        if (parallaxIsOK == mcsTRUE)
        {
            if (CompleteWithParallax(mcsFALSE) == mcsFAILURE)
            {
                return mcsFAILURE;
            }

            // Compute visibility and visibility error
            if (ComputeVisibility(request) == mcsFAILURE)
            {
                return mcsFAILURE;
            }
        }
        // parallax unknown
        else
        {
            // Temporary stars with/without interstellar absorption
            sclsvrCALIBRATOR starWith;
            starWith.Update(* this);

            mcsLOGICAL useInterstellarAbsorption = mcsTRUE;

            // First, compute diameter and visibility without considering
            // interstellar absorption (i.e av=0)

            // Set extinction ratio property
            if (SetPropertyValue
                (sclsvrCALIBRATOR_EXTINCTION_RATIO, 0.0, 
                 vobsSTAR_COMPUTED_PROP) == mcsFAILURE)
            {
                return mcsFAILURE;
            }

            // Complete diameters without parallax
            if (CompleteWithoutParallax() == mcsFAILURE)
            {
                return mcsFAILURE;
            }

            // Compute Angular Diameter
            logTest("star %s - computing diameter without absorption...",
                    starId);

            // Compute visibility and visibility error
            if (ComputeVisibility(request) == mcsFAILURE)
            {
                return mcsFAILURE;
            }

            // If visibility has been computed, then compute now diameters and
            // visibility with an arbitrary interstellar absorption
            // it also depends on the useInterstellarAbsorption flag
            if ( (IsPropertySet(sclsvrCALIBRATOR_VIS2) == mcsTRUE) && 
                 (useInterstellarAbsorption == mcsTRUE))
            {
                // Do the same with the extinction ratio fixed to 3.0
                if (starWith.
                    SetPropertyValue(sclsvrCALIBRATOR_EXTINCTION_RATIO, 3.0,
                                     vobsSTAR_COMPUTED_PROP) == mcsFAILURE)
                {
                    return mcsFAILURE;
                }

                // Complete diameters without parallax
                if (starWith.CompleteWithoutParallax() == mcsFAILURE)
                {
                    return mcsFAILURE;
                }

                // Compute Angular Diameter
                logTest("star %s - Computing diameter with absorption...", 
                        starId);

                // Compute visibility and visibility error
                if (starWith.ComputeVisibility(request) == mcsFAILURE)
                {
                    return mcsFAILURE;
                }

                // If visibility has been computed, compare result 
                if (starWith.IsPropertySet(sclsvrCALIBRATOR_VIS2) == mcsTRUE)
                {
                    // Get Visibility of the star (without absorption)
                    mcsFLOAT vis2;
                    mcsFLOAT vis2Err;
                    GetPropertyValue(sclsvrCALIBRATOR_VIS2, &vis2);
                    GetPropertyValue(sclsvrCALIBRATOR_VIS2_ERROR,
                                     &vis2Err);
                    // Get Visibility of the star (with absorption)
                    mcsFLOAT vis2A;
                    mcsFLOAT vis2ErrA;
                    starWith.GetPropertyValue(sclsvrCALIBRATOR_VIS2, &vis2A);
                    starWith.GetPropertyValue(sclsvrCALIBRATOR_VIS2_ERROR, 
                                              &vis2ErrA);

                    // Compute MAX(|vis2A - vis2|; vis2Err)
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
                    mcsFLOAT expectedVisErr = request.GetExpectedVisErr();
                    if (visibilityErr > expectedVisErr)
                    {
                        logTest("star %s - visibility error (%f) is higher than the expected one (%f)",
                                starId, visibilityErr, expectedVisErr);
                    }
                    else
                    {
                        logTest("star %s - visibility error (%f) is lower than the expected one (%f)",
                                starId, visibilityErr, expectedVisErr);
                        Update(starWith,mcsTRUE);
                    }
                }
            }
        }
    }

    // Compute UD from LD and SP
    if (ComputeUDFromLDAndSP() == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    // Compute distance
    if (ComputeDistance(request) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    return mcsSUCCESS;
}


/*
 * Private methods
 */
/**
 * Compute missing magnitude.
 *
 * @param isBright
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is
 * returned.
 */
mcsCOMPL_STAT sclsvrCALIBRATOR::ComputeMissingMagnitude(mcsLOGICAL isBright)
{
    logTrace("sclsvrCALIBRATOR::ComputeMissingMagnitude()");

    // Get Star ID
    mcsSTRING32 starId;
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

            magnitudes[band].isSet     = mcsTRUE;
            magnitudes[band].confIndex =
                (alxCONFIDENCE_INDEX)GetPropertyConfIndex(mag0PropertyId[band]);
        }
        else
        {
            magnitudes[band].value     = 0.0;
            magnitudes[band].isSet     = mcsFALSE;
            magnitudes[band].confIndex = alxNO_CONFIDENCE;
        }
    }

    // Get the value of the Spectral Type
    mcsSTRING32 spType;
    if (isBright == mcsTRUE)
    {
        strcpy(spType, GetPropertyValue(vobsSTAR_SPECT_TYPE_MK));

        // Compute missing magnitudes
        if (alxComputeMagnitudesForBrightStar(spType, magnitudes) == mcsFAILURE)
        {
            // If error found on spectral type, reset stack and return SUCCESS
            if ((errIsInStack("alx", 
                              alxERR_SPECTRAL_TYPE_NOT_FOUND) == mcsTRUE) ||
                (errIsInStack("alx", 
                              alxERR_WRONG_SPECTRAL_TYPE_FORMAT) == mcsTRUE))
            {
                logTest("star %s - spectral type '%s' is unknown; "
                        "could not compute missing magnitudes", starId, spType); 
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
                    logTest("star %s - spectral type '%s' is unknown; "
                            "could not compute missing magnitudes",
                            starId, spType); 
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
            if (alxComputeMagnitudesForFaintStar(NULL,magnitudes) == mcsFAILURE)
            {
                if ((errIsInStack("alx",alxERR_NO_LINE_FOUND) ==mcsTRUE) ||
                    (errIsInStack("alx",alxERR_DIFFJK_NOT_IN_TABLE) == mcsTRUE))
                {
                    logTest("star %s - J-K differential magnitude not found "
                            "in color table; could not compute missing magnitudes",
                            starId);
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
         * Please note that if a magnitude is already set, the
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
 * Compute galactic coordinates.
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
 * Compute extinction coefficient.
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is
 * returned.
 */
mcsCOMPL_STAT sclsvrCALIBRATOR::ComputeExtinctionCoefficient()
{
    logTrace("sclsvrCALIBRATOR::ComputeExtinctionCoefficient()");

    mcsFLOAT parallax, gLat, gLon;
    mcsFLOAT av;

    // Get the value of the parallax
    if (IsPropertySet(vobsSTAR_POS_PARLX_TRIG) == mcsTRUE)
    {
        if (GetPropertyValue(vobsSTAR_POS_PARLX_TRIG, &parallax) == mcsFAILURE)
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
    if (alxComputeExtinctionCoefficient(&av, parallax, gLat, gLon) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    // Set extinction ratio property
    if (SetPropertyValue(sclsvrCALIBRATOR_EXTINCTION_RATIO, 
                         av,
                         vobsSTAR_COMPUTED_PROP) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    return mcsSUCCESS;
}

/**
 * Compute interstellar absorption.
 * 
 * @param isBright true is it is for bright object
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is
 * returned.
 */
mcsCOMPL_STAT sclsvrCALIBRATOR::
ComputeInterstellarAbsorption(mcsLOGICAL isBright)
{
    logTrace("sclsvrCALIBRATOR::ComputeInterstellarAbsorption()");

    // Magnitudes to be used
    char *magPropertyId[alxNB_BANDS]; 
    if (isBright == mcsTRUE)
    {
        magPropertyId[alxB_BAND] = vobsSTAR_PHOT_JHN_B;
        magPropertyId[alxV_BAND] = vobsSTAR_PHOT_JHN_V;
        magPropertyId[alxR_BAND] = vobsSTAR_PHOT_JHN_R;
        magPropertyId[alxI_BAND] = vobsSTAR_PHOT_JHN_I;
        magPropertyId[alxJ_BAND] = vobsSTAR_PHOT_JHN_J;
        magPropertyId[alxH_BAND] = vobsSTAR_PHOT_JHN_H;
        magPropertyId[alxK_BAND] = vobsSTAR_PHOT_JHN_K;
        magPropertyId[alxL_BAND] = vobsSTAR_PHOT_JHN_L;
        magPropertyId[alxM_BAND] = vobsSTAR_PHOT_JHN_M;
    }
    else
    {
        magPropertyId[alxB_BAND] = vobsSTAR_PHOT_PHG_B;
        magPropertyId[alxV_BAND] = vobsSTAR_PHOT_JHN_V;
        magPropertyId[alxR_BAND] = vobsSTAR_PHOT_PHG_R;
        magPropertyId[alxI_BAND] = vobsSTAR_PHOT_COUS_I;
        magPropertyId[alxJ_BAND] = sclsvrCALIBRATOR_PHOT_COUS_J;
        magPropertyId[alxH_BAND] = sclsvrCALIBRATOR_PHOT_COUS_H;
        magPropertyId[alxK_BAND] = sclsvrCALIBRATOR_PHOT_COUS_K;
        magPropertyId[alxL_BAND] = vobsSTAR_PHOT_JHN_L;
        magPropertyId[alxM_BAND] = vobsSTAR_PHOT_JHN_M;
    } 
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

    // Get the extinction ratio
    mcsFLOAT av;
    if (GetPropertyValue(sclsvrCALIBRATOR_EXTINCTION_RATIO, &av) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

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

            magnitudes[band].isSet     = mcsTRUE;
            magnitudes[band].confIndex = 
                (alxCONFIDENCE_INDEX)GetPropertyConfIndex(magPropertyId[band]);
        }
        else
        {
            magnitudes[band].isSet     = mcsFALSE;
            magnitudes[band].confIndex = alxNO_CONFIDENCE;
            magnitudes[band].value     = 0.0;
        }
    }

    // Correct K magnitude  
    if (isBright == mcsTRUE)
    {
        if (IsPropertySet(vobsSTAR_PHOT_JHN_K) == mcsTRUE)
        {
            const char *origin = GetProperty(vobsSTAR_PHOT_JHN_K)->GetOrigin();

            // If coming from II/246/out, J/A+A/433/1155
            if ((strcmp(origin, "II/246/out") == 0) ||
                (strcmp(origin, "J/A+A/433/1155")== 0))
            {
                magnitudes[alxK_BAND].value = 
                    1.008 * magnitudes[alxK_BAND].value + 0.005;
            }

            // If coming from J-K Denis
            if (strcmp(origin, "J/A+A/413/1037") == 0)
            {
                magnitudes[alxK_BAND].value = 
                    1.008 * (magnitudes[alxK_BAND].value + 
                             0.006 * (magnitudes[alxJ_BAND].value -
                                      magnitudes[alxK_BAND].value)) - 0.03;
            }
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
sclsvrCALIBRATOR::ComputeApparentMagnitude(mcsLOGICAL isBright)
{
    logTrace("sclsvrCALIBRATOR::ComputeApparentMagnitude()");

    // Magnitudes to be used
    char *magPropertyId[alxNB_BANDS]; 
    if (isBright == mcsTRUE)
    {
        magPropertyId[alxB_BAND] = vobsSTAR_PHOT_JHN_B;
        magPropertyId[alxV_BAND] = vobsSTAR_PHOT_JHN_V;
        magPropertyId[alxR_BAND] = vobsSTAR_PHOT_JHN_R;
        magPropertyId[alxI_BAND] = vobsSTAR_PHOT_JHN_I;
        magPropertyId[alxJ_BAND] = vobsSTAR_PHOT_JHN_J;
        magPropertyId[alxH_BAND] = vobsSTAR_PHOT_JHN_H;
        magPropertyId[alxK_BAND] = vobsSTAR_PHOT_JHN_K;
        magPropertyId[alxL_BAND] = vobsSTAR_PHOT_JHN_L;
        magPropertyId[alxM_BAND] = vobsSTAR_PHOT_JHN_M;
    }
    else
    {
        magPropertyId[alxB_BAND] = vobsSTAR_PHOT_PHG_B;
        magPropertyId[alxV_BAND] = vobsSTAR_PHOT_JHN_V;
        magPropertyId[alxR_BAND] = vobsSTAR_PHOT_PHG_R;
        magPropertyId[alxI_BAND] = vobsSTAR_PHOT_COUS_I;
        magPropertyId[alxJ_BAND] = sclsvrCALIBRATOR_PHOT_COUS_J;
        magPropertyId[alxH_BAND] = sclsvrCALIBRATOR_PHOT_COUS_H;
        magPropertyId[alxK_BAND] = sclsvrCALIBRATOR_PHOT_COUS_K;
        magPropertyId[alxL_BAND] = vobsSTAR_PHOT_JHN_L;
        magPropertyId[alxM_BAND] = vobsSTAR_PHOT_JHN_M;
    } 

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

    // Get the extinction ratio
    mcsFLOAT av;
    if (GetPropertyValue(sclsvrCALIBRATOR_EXTINCTION_RATIO, &av) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    // For each corrected magnitude
    alxMAGNITUDES magnitudes;
    for (int band = 0; band < alxNB_BANDS; band++)
    { 
        // Get the current value and the confidence index
        if (IsPropertySet(mag0PropertyId[band]) == mcsTRUE)
        {
            if (GetPropertyValue(mag0PropertyId[band],
                                 &magnitudes[band].value) == mcsFAILURE)
            {
                return mcsFAILURE;
            }

            magnitudes[band].isSet     = mcsTRUE;
            magnitudes[band].confIndex =
                (alxCONFIDENCE_INDEX)GetPropertyConfIndex(mag0PropertyId[band]);
        }
        else
        {
            magnitudes[band].isSet     = mcsFALSE;
            magnitudes[band].confIndex = alxNO_CONFIDENCE;
            magnitudes[band].value     = 0.0;
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
        // Set the magnitude magnitude
        if (magnitudes[band].isSet == mcsTRUE)
        {
            if (SetPropertyValue(magPropertyId[band], magnitudes[band].value,
                                 vobsSTAR_COMPUTED_PROP,
                                 (vobsCONFIDENCE_INDEX)magnitudes[band].confIndex) == mcsFAILURE)
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
mcsCOMPL_STAT sclsvrCALIBRATOR::ComputeAngularDiameter(mcsLOGICAL isBright)
{
    logTrace("sclsvrCALIBRATOR::ComputeAngularDiameter()");

    mcsSTRING32 starId;
    // Get Star ID
    if (GetId(starId, sizeof(starId)) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    // Declare the 4 properties name for I0, J0, K0, H0
    int nbProperties;
    char *starPropertyId[6];
    if (isBright == mcsTRUE)
    {
        nbProperties = 4;
        starPropertyId[0] = sclsvrCALIBRATOR_BO;
        starPropertyId[1] = sclsvrCALIBRATOR_VO;
        starPropertyId[2] = sclsvrCALIBRATOR_RO;
        starPropertyId[3] = sclsvrCALIBRATOR_KO;
    }
    else
    {
        nbProperties = 4;
        starPropertyId[0] = sclsvrCALIBRATOR_IO;
        starPropertyId[1] = sclsvrCALIBRATOR_JO;
        starPropertyId[2] = sclsvrCALIBRATOR_KO;
        starPropertyId[3] = sclsvrCALIBRATOR_HO;
        starPropertyId[4] = vobsSTAR_PHOT_JHN_V;
        starPropertyId[5] = vobsSTAR_PHOT_JHN_K;
    }

    alxDIAMETERS diameters;
    alxDATA starProperty[6];

    // For each property needed to compute angular diameter
    for (int i = 0; i < nbProperties; i++)
    { 
        // If the current property is affected
        if (IsPropertySet(starPropertyId[i]) == mcsTRUE)
        {
            // Get the property
            vobsSTAR_PROPERTY *property=GetProperty(starPropertyId[i]);

            // Get the property value
            if ((property->GetValue(&starProperty[i].value)) == mcsFAILURE)
            {
                return mcsFAILURE;
            }

            // Flag the property as set
            starProperty[i].isSet = mcsTRUE;

            // Get the property confidence index
            starProperty[i].confIndex =
                (alxCONFIDENCE_INDEX)property->GetConfidenceIndex();
        }
        else
        {
            logTest("star %s - unknown %s property; "
                    "could not compute diameter", starId, starPropertyId[i]);
            // Do nothing
            return mcsSUCCESS;
        }
    }

    // In faint case
    if (isBright == mcsFALSE)
    {
        // If V mag is coming I/280 (2mass)
        if ((IsPropertySet(starPropertyId[4]) == mcsTRUE) &&
            (strcmp(GetProperty(starPropertyId[4])->GetOrigin(), "I/280") == 0))
        {
            // Get V and K magnitude to compute diam VK 
            for (int i = 4; i < 6; i++)
            { 
                // Get the property
                vobsSTAR_PROPERTY *property=GetProperty(starPropertyId[i]);

                // Get the property value
                if ((property->GetValue(&starProperty[i].value)) == mcsFAILURE)
                {
                    return mcsFAILURE;
                }

                // Flag the property as set
                starProperty[i].isSet = mcsTRUE;

                // Get the property confidence index
                starProperty[i].confIndex =
                    (alxCONFIDENCE_INDEX)property->GetConfidenceIndex();
            }
            // Compute apparent magnitude for V and K
            // Get the extinction ratio
            mcsFLOAT av;
            if (GetPropertyValue(sclsvrCALIBRATOR_EXTINCTION_RATIO,
                                 &av) == mcsFAILURE)
            {
                return mcsFAILURE;
            }

            // V - Correct interstellar absorption
            starProperty[4].value = starProperty[4].value + av;
            // K - Convert K2mass to Kjnk and correct interstellar absorption 
            starProperty[5].value = 1.008 * starProperty[5].value + 0.005;
            starProperty[5].value = starProperty[5].value +  0.116 * av;
        }
        else
        {
            starProperty[4].isSet = mcsFALSE;
            starProperty[5].isSet = mcsFALSE;
        }
    }

    // Set retrieved magnitudes
    alxDATA magB, magV, magR, magI, magJ, magK, magH, magKJnk;
    if (isBright == mcsTRUE)
    {
        magB=starProperty[0];
        magV=starProperty[1];
        magR=starProperty[2];
        magK=starProperty[3];
    }
    else
    {
        magI=starProperty[0];
        magJ=starProperty[1];
        magK=starProperty[2];
        magH=starProperty[3];
        magV=starProperty[4];
        magKJnk=starProperty[5];
    }

    if (isBright == mcsTRUE)
    {
        // Compute angular diameters for bright star
        if (alxComputeAngularDiameterForBrightStar(magB, magV, magR, magK,
                                                   &diameters) == mcsFAILURE)
        {
            return mcsFAILURE;
        }

        // Set flag according to the confidence index 
        if (diameters.areCoherent == mcsFALSE)
        {
            logTest("star %s - error on diameter too high; "
                    "computed diameters are not coherent", starId);

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
    else
    {
        // Make a short pre-processing to check that Johnson magnitutde in J, H,
        // K came from the CDS. If not, stop the angular diametre computation.
        vobsSTAR_PROPERTY *magJProp = GetProperty(vobsSTAR_PHOT_JHN_J);
        vobsSTAR_PROPERTY *magHProp = GetProperty(vobsSTAR_PHOT_JHN_H);
        vobsSTAR_PROPERTY *magKProp = GetProperty(vobsSTAR_PHOT_JHN_K);
        if ((magJProp->IsComputed() == mcsTRUE) ||
            (magHProp->IsComputed() == mcsTRUE) ||
            (magKProp->IsComputed() == mcsTRUE))
        {
            // stop the treatment
            logTest("star %s - J, H and/or K magitudes are unknown; "
                    "could not compute diameter", starId); 
            if (SetPropertyValue(sclsvrCALIBRATOR_DIAM_FLAG, "NOK",
                                 vobsSTAR_COMPUTED_PROP) == mcsFAILURE)
            {
                return mcsFAILURE;
            }
        }
        else
        {
            // Check if I came from CDS. If not, do not use it for diameter
            // computation 
            vobsSTAR_PROPERTY *magIProp;
            magIProp = GetProperty(vobsSTAR_PHOT_COUS_I);
            if (strcmp(magIProp->GetOrigin(), vobsSTAR_COMPUTED_PROP) != 0)
            {
                magI.isSet = mcsTRUE;
            }
            else
            {
                magI.isSet = mcsFALSE;
            }

            // Compute angular diameters
            if (alxComputeAngularDiameterForFaintStar(magI, magJ, magK, magH,
                                                      magV, magKJnk,
                                                      &diameters) == mcsFAILURE)
            {
                return mcsFAILURE;
            }

            // Set flag according to the confidence index 
            if (diameters.areCoherent == mcsFALSE)
            {
                logTest("star %s - error on diameter too high; "
                        "computed diameters are not coherent", starId);
                if (SetPropertyValue(sclsvrCALIBRATOR_DIAM_FLAG, "NOK",
                                     vobsSTAR_COMPUTED_PROP) == mcsFAILURE)
                {
                    return mcsFAILURE;
                }
            }
            else
            {
                if (SetPropertyValue(sclsvrCALIBRATOR_DIAM_FLAG, "OK",
                                     vobsSTAR_COMPUTED_PROP) == mcsFAILURE)
                {
                    return mcsFAILURE;
                }
            }

            // Save IJ and IK diameters, only if I magitude came from CDS 
            if (magI.isSet == mcsTRUE)
            {
                // Set the computed value of the angular diameter
                if (SetPropertyValue(sclsvrCALIBRATOR_DIAM_IJ,
                                     diameters.ij.value,
                                     vobsSTAR_COMPUTED_PROP, 
                                     (vobsCONFIDENCE_INDEX)diameters.confidenceIdx)
                    == mcsFAILURE)
                {
                    return mcsFAILURE;
                }
                if (SetPropertyValue(sclsvrCALIBRATOR_DIAM_IK, 
                                     diameters.ik.value,
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
            }

            // Save JK and JH diameters
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

            // Save HK diameter if I magnitude was not known
            if (magI.isSet == mcsFALSE)
            {
                if (SetPropertyValue(sclsvrCALIBRATOR_DIAM_HK, 
                                     diameters.hk.value,
                                     vobsSTAR_COMPUTED_PROP, 
                                     (vobsCONFIDENCE_INDEX)diameters.confidenceIdx)
                    == mcsFAILURE)
                {
                    return mcsFAILURE;
                }
                if (SetPropertyValue(sclsvrCALIBRATOR_DIAM_HK_ERROR,
                                     diameters.hkErr.value,
                                     vobsSTAR_COMPUTED_PROP, 
                                     (vobsCONFIDENCE_INDEX)diameters.confidenceIdx)
                    == mcsFAILURE)
                {
                    return mcsFAILURE;
                }
            }
            // Save VK diameter, only if V magitude came from CDS 
            if (magV.isSet == mcsTRUE)
            {
                // Set the computed value of the angular diameter
                if (SetPropertyValue(sclsvrCALIBRATOR_DIAM_VK,
                                     diameters.vk.value,
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
            if (SetPropertyValue(sclsvrCALIBRATOR_DIAM_MEAN,
                                 diameters.mean.value,
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
 * Compute UD from LD and SP.
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is
 * returned.
 */
mcsCOMPL_STAT sclsvrCALIBRATOR::ComputeUDFromLDAndSP()
{
    logTrace("sclsvrCALIBRATOR::ComputeUDFromLDAndSP()");

    // Compute UD only if LD are already OK
    if (IsDiameterOk() == mcsFALSE)
    {
        logTest("Skipping (diameters are not OK).");
        return mcsSUCCESS;
    }

    // Get the value of the Spectral Type
    mcsSTRING32 spType;
    strncpy(spType, GetPropertyValue(vobsSTAR_SPECT_TYPE_MK), sizeof(spType));
    if (strlen(spType) < 1)
    {
        logTest("Skipping (SpType unknown).");
        return mcsSUCCESS;
    }

    // Does LD diameter exist ?
    if (IsPropertySet(sclsvrCALIBRATOR_DIAM_VK) == mcsFALSE)
    {
        logTest("Skipping (LD unknown).");
        return mcsSUCCESS;
    }

    // Get LD diameter (DIAM_VK)
    mcsFLOAT ld = FP_NAN;
    if (GetPropertyValue(sclsvrCALIBRATOR_DIAM_VK, &ld) == mcsFAILURE)
    {
        logError("Aborting (error while retrieving DIAM_VK).");
        return mcsSUCCESS;
    }

    // Compute UD
    logTest("Computing UDs for LD='%f' and SP='%s'...", ld, spType);
    alxUNIFORM_DIAMETERS ud;
    if (alxComputeUDFromLDAndSP(ld, spType, &ud) == mcsFAILURE)
    {
        logError("Aborting (error while computing).");
        return mcsFAILURE;
    }

    // Set each UD_* property accordinaly
    if (SetPropertyValue(sclsvrCALIBRATOR_UD_B, ud.b, vobsSTAR_COMPUTED_PROP, vobsCONFIDENCE_HIGH) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    if (SetPropertyValue(sclsvrCALIBRATOR_UD_I, ud.i, vobsSTAR_COMPUTED_PROP, vobsCONFIDENCE_HIGH) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    if (SetPropertyValue(sclsvrCALIBRATOR_UD_J, ud.j, vobsSTAR_COMPUTED_PROP, vobsCONFIDENCE_HIGH) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    if (SetPropertyValue(sclsvrCALIBRATOR_UD_H, ud.h, vobsSTAR_COMPUTED_PROP, vobsCONFIDENCE_HIGH) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    if (SetPropertyValue(sclsvrCALIBRATOR_UD_K, ud.k, vobsSTAR_COMPUTED_PROP, vobsCONFIDENCE_HIGH) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    if (SetPropertyValue(sclsvrCALIBRATOR_UD_L, ud.l, vobsSTAR_COMPUTED_PROP, vobsCONFIDENCE_HIGH) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    if (SetPropertyValue(sclsvrCALIBRATOR_UD_N, ud.n, vobsSTAR_COMPUTED_PROP, vobsCONFIDENCE_HIGH) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    if (SetPropertyValue(sclsvrCALIBRATOR_UD_R, ud.r, vobsSTAR_COMPUTED_PROP, vobsCONFIDENCE_HIGH) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    if (SetPropertyValue(sclsvrCALIBRATOR_UD_U, ud.u, vobsSTAR_COMPUTED_PROP, vobsCONFIDENCE_HIGH) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    if (SetPropertyValue(sclsvrCALIBRATOR_UD_V, ud.v, vobsSTAR_COMPUTED_PROP, vobsCONFIDENCE_HIGH) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    if (logGetStdoutLogLevel() >= logTEST)
    {
        alxShowUNIFORM_DIAMETERS(&ud);
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

    // Get Star ID
    mcsSTRING32 starId;
    if (GetId(starId, sizeof(starId)) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

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

            // Set confidence index too high (value coming form catalog)
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
                logTest("star %s - unknown diameter; "
                        "could not compute visibility", starId);
                return mcsSUCCESS;
            }
        }
        // Else 
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
                logTest("star %s - unknown diameter; "
                        "could not compute visibility", starId);
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

    // If visibility has been computed, diameter (coming from catalog or
    // computed) must be considered as OK.
    SetPropertyValue(sclsvrCALIBRATOR_DIAM_FLAG, "OK", vobsSTAR_COMPUTED_PROP);

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
 * Compute distance.
 *
 * This method calculate the distance in degree between the calibrator and the
 * science object.
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
    if (SetPropertyValue(sclsvrCALIBRATOR_DIST, 
                         distance/3600, vobsSTAR_COMPUTED_PROP) ==
        mcsFAILURE)
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
    mcsLOGICAL magKIsSet = mcsFALSE;
    mcsLOGICAL magJIsSet = mcsFALSE;
    mcsLOGICAL magHIsSet = mcsFALSE;
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
        magKIsSet = mcsTRUE;
    }
    // if Jmag property is set        
    if (IsPropertySet(vobsSTAR_PHOT_JHN_J) == mcsTRUE)
    {
        // retreive it
        if (GetPropertyValue(vobsSTAR_PHOT_JHN_J, &magJ) == mcsFAILURE)
        {       
            return mcsFAILURE;
        }
        magJIsSet = mcsTRUE;
    }
    // if Hmag property is set
    if (IsPropertySet(vobsSTAR_PHOT_JHN_H) == mcsTRUE)
    {
        // retreive it
        if (GetPropertyValue(vobsSTAR_PHOT_JHN_H, &magH) == mcsFAILURE)
        {       
            return mcsFAILURE;
        }
        magHIsSet = mcsTRUE;
    }

    // Set Kcous
    if (magKIsSet == mcsTRUE)
    {
        magKcous = magK + 0.024;
        if (SetPropertyValue(sclsvrCALIBRATOR_PHOT_COUS_K, magKcous, 
                             vobsSTAR_COMPUTED_PROP) == mcsFAILURE)
        {
            return mcsFAILURE;
        }
    }

    // Set Jcous
    if ((magKIsSet == mcsTRUE) && (magJIsSet == mcsTRUE))
    {
        magJcous = 0.947 * magJ + 0.053 * magK + 0.036;
        if (SetPropertyValue(sclsvrCALIBRATOR_PHOT_COUS_J, magJcous, 
                             vobsSTAR_COMPUTED_PROP) == mcsFAILURE)
        {
            return mcsFAILURE;
        }
    }

    // Set Hcous
    if ((magKIsSet == mcsTRUE) && (magHIsSet == mcsTRUE))
    {
        magHcous = 0.975 * magH + 0.025 * magK - 0.004;
        if (SetPropertyValue(sclsvrCALIBRATOR_PHOT_COUS_H, magHcous, 
                             vobsSTAR_COMPUTED_PROP) == mcsFAILURE)
        {
            return mcsFAILURE;
        }
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

    AddProperty(sclsvrCALIBRATOR_PHOT_COUS_J, "Jcous", vobsFLOAT_PROPERTY, "mag");
    AddProperty(sclsvrCALIBRATOR_PHOT_COUS_H, "Hcous", vobsFLOAT_PROPERTY, "mag");
    AddProperty(sclsvrCALIBRATOR_PHOT_COUS_K, "Kcous", vobsFLOAT_PROPERTY, "mag");

    AddProperty(sclsvrCALIBRATOR_DIAM_BV, "diam_bv", vobsFLOAT_PROPERTY, NULL, NULL, NULL, "diameter b-v");
    AddProperty(sclsvrCALIBRATOR_DIAM_VR, "diam_vr", vobsFLOAT_PROPERTY);
    AddProperty(sclsvrCALIBRATOR_DIAM_VK, "diam_vk", vobsFLOAT_PROPERTY);
    AddProperty(sclsvrCALIBRATOR_DIAM_IJ, "diam_ij", vobsFLOAT_PROPERTY);
    AddProperty(sclsvrCALIBRATOR_DIAM_IK, "diam_ik", vobsFLOAT_PROPERTY);
    AddProperty(sclsvrCALIBRATOR_DIAM_JK, "diam_jk", vobsFLOAT_PROPERTY);
    AddProperty(sclsvrCALIBRATOR_DIAM_JH, "diam_jh", vobsFLOAT_PROPERTY);
    AddProperty(sclsvrCALIBRATOR_DIAM_HK, "diam_hk", vobsFLOAT_PROPERTY);

    AddProperty(sclsvrCALIBRATOR_DIAM_MEAN, "diam_mean", vobsFLOAT_PROPERTY);

    AddProperty(sclsvrCALIBRATOR_DIAM_BV_ERROR, "e_diam_bv", vobsFLOAT_PROPERTY);
    AddProperty(sclsvrCALIBRATOR_DIAM_VR_ERROR, "e_diam_vr", vobsFLOAT_PROPERTY);
    AddProperty(sclsvrCALIBRATOR_DIAM_VK_ERROR, "e_diam_vk", vobsFLOAT_PROPERTY);
    AddProperty(sclsvrCALIBRATOR_DIAM_IJ_ERROR, "e_diam_ij", vobsFLOAT_PROPERTY);
    AddProperty(sclsvrCALIBRATOR_DIAM_IK_ERROR, "e_diam_ik", vobsFLOAT_PROPERTY);
    AddProperty(sclsvrCALIBRATOR_DIAM_JK_ERROR, "e_diam_jk", vobsFLOAT_PROPERTY);
    AddProperty(sclsvrCALIBRATOR_DIAM_JH_ERROR, "e_diam_jh", vobsFLOAT_PROPERTY);
    AddProperty(sclsvrCALIBRATOR_DIAM_HK_ERROR, "e_diam_hk", vobsFLOAT_PROPERTY);

    AddProperty(sclsvrCALIBRATOR_DIAM_MEAN_ERROR, "e_diam_mean", vobsFLOAT_PROPERTY);

    AddProperty(sclsvrCALIBRATOR_DIAM_FLAG, "diamFlag", vobsSTRING_PROPERTY);

    AddProperty(sclsvrCALIBRATOR_UD_B, "UD_B", vobsFLOAT_PROPERTY);
    AddProperty(sclsvrCALIBRATOR_UD_I, "UD_I", vobsFLOAT_PROPERTY);
    AddProperty(sclsvrCALIBRATOR_UD_J, "UD_J", vobsFLOAT_PROPERTY);
    AddProperty(sclsvrCALIBRATOR_UD_H, "UD_H", vobsFLOAT_PROPERTY);
    AddProperty(sclsvrCALIBRATOR_UD_K, "UD_K", vobsFLOAT_PROPERTY);
    AddProperty(sclsvrCALIBRATOR_UD_L, "UD_L", vobsFLOAT_PROPERTY);
    AddProperty(sclsvrCALIBRATOR_UD_N, "UD_N", vobsFLOAT_PROPERTY);
    AddProperty(sclsvrCALIBRATOR_UD_R, "UD_R", vobsFLOAT_PROPERTY);
    AddProperty(sclsvrCALIBRATOR_UD_U, "UD_U", vobsFLOAT_PROPERTY);
    AddProperty(sclsvrCALIBRATOR_UD_V, "UD_V", vobsFLOAT_PROPERTY);

    AddProperty(sclsvrCALIBRATOR_EXTINCTION_RATIO, "Av", vobsFLOAT_PROPERTY);

    AddProperty(sclsvrCALIBRATOR_MO, "Mo", vobsFLOAT_PROPERTY, "mag");
    AddProperty(sclsvrCALIBRATOR_LO, "Lo", vobsFLOAT_PROPERTY, "mag");
    AddProperty(sclsvrCALIBRATOR_KO, "Ko", vobsFLOAT_PROPERTY, "mag");
    AddProperty(sclsvrCALIBRATOR_HO, "Ho", vobsFLOAT_PROPERTY, "mag");
    AddProperty(sclsvrCALIBRATOR_JO, "Jo", vobsFLOAT_PROPERTY, "mag");
    AddProperty(sclsvrCALIBRATOR_IO, "Io", vobsFLOAT_PROPERTY, "mag");
    AddProperty(sclsvrCALIBRATOR_RO, "Ro", vobsFLOAT_PROPERTY, "mag");
    AddProperty(sclsvrCALIBRATOR_VO, "Vo", vobsFLOAT_PROPERTY, "mag");
    AddProperty(sclsvrCALIBRATOR_BO, "Bo", vobsFLOAT_PROPERTY, "mag");

    AddProperty(sclsvrCALIBRATOR_VIS2, "vis2", vobsFLOAT_PROPERTY);
    AddProperty(sclsvrCALIBRATOR_VIS2_ERROR, "vis2Err", vobsFLOAT_PROPERTY);

    AddProperty(sclsvrCALIBRATOR_VIS2_8, "vis2(8mu)", vobsFLOAT_PROPERTY);
    AddProperty(sclsvrCALIBRATOR_VIS2_8_ERROR, "vis2Err(8mu)", vobsFLOAT_PROPERTY);

    AddProperty(sclsvrCALIBRATOR_VIS2_13, "vis2(13mu)", vobsFLOAT_PROPERTY);
    AddProperty(sclsvrCALIBRATOR_VIS2_13_ERROR, "vis2Err(13mu)", vobsFLOAT_PROPERTY);

    AddProperty(sclsvrCALIBRATOR_VIS2_FLAG, "vis2Flag", vobsSTRING_PROPERTY);

    AddProperty(sclsvrCALIBRATOR_DIST, "dist", vobsFLOAT_PROPERTY, "deg");

    return mcsSUCCESS;
}

/*___oOo___*/
