/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: vobsCATALOG_MIDI.cpp,v 1.18 2005-03-04 16:05:51 scetre Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.17  2005/02/24 17:03:18  scetre
 * Computed MagN and diameter error only once; when catalog is loaded.
 *
 * Revision 1.16  2005/02/23 07:51:29  gzins
 * Changed vobsSTAR_UD_DIAM/vobsSTAR_UD_DIAM_ERROR to vobsSTAR_DIAM12/vobsSTAR_DIAM12_ERROR
 *
 * Revision 1.15  2005/02/17 17:56:55  gzins
 * Used decimal logarithm instead of neperian logarithm when computing mag N
 *
 * Revision 1.14  2005/02/13 15:59:08  gzins
 * Fixed wrong class name in logExtDbg()
 *
 * Revision 1.13  2005/02/11 14:22:10  gluck
 * - Updated to fit with vobsLOCAL_CATALOG and vobsREMOTE_CATALOG classes introduction
 * - Changed catalog naming: suppressed SetName() method in general catalog and put it in initialisation list of specialised catalog
 * - Updated some comments
 *
 * Revision 1.10  2005/02/10 10:46:42  gzins
 * Changed column name to parameter name
 *
 * Revision 1.9  2005/02/08 21:23:28  gzins
 * Convert diameter error expressed in % to mas
 *
 * Revision 1.8  2005/02/08 20:30:37  gzins
 * Changed _name type; mcsSTRING to string
 *
 * Revision 1.7  2005/02/07 19:40:58  gzins
 * Updated vobsREQUEST API
 *
 * Revision 1.6  2005/02/07 09:47:08  gzins
 * Renamed vobsCDATA method to be compliant with programming standards; method
 * name starts with capital
 *
 * Revision 1.5  2005/02/07 09:13:43  gzins
 * Added initialisation of _loaded
 * Removed printf
 * Supported RA and DEC expressed with ':' as separator instead of ' '
 *
 * Revision 1.4  2005/02/04 15:10:25  gluck
 * Update documentation
 *
 * Revision 1.3  2005/02/04 10:49:43  gzins
 * Fixed wrong MIDI catalog name
 *
 * Revision 1.2  2005/02/04 10:42:32  gzins
 * Added RA/DEC delta conversion (minutes (dm) to decimal degrees)
 *
 * Revision 1.1  2005/02/04 09:54:42  gzins
 * Created
 *
 ******************************************************************************/

/**
 * \file
 *  Definition of vobsCATALOG_MIDI class.
 */

static char *rcsId="@(#) $Id: vobsCATALOG_MIDI.cpp,v 1.18 2005-03-04 16:05:51 scetre Exp $"; 
static void *use_rcsId = ((void)&use_rcsId,(void *) &rcsId);

/* 
 * System Headers 
 */
#include <iostream>
using namespace std;

/*
 * MCS Headers 
 */
#include "mcs.h"
#include "log.h"
#include "err.h"
#include "math.h"

/*
 * Local Headers 
 */
#include "vobsCATALOG_MIDI.h"
#include "vobsPrivate.h"
#include "vobsErrors.h"

/**
 * Class constructor
 */
vobsCATALOG_MIDI::vobsCATALOG_MIDI() : vobsLOCAL_CATALOG("MIDI", 
                                                         "vobsMidiCatalog.cfg")
{
}

/**
 * Class destructor
 */
vobsCATALOG_MIDI::~vobsCATALOG_MIDI()
{
}

/*
 * Public methods
 */




/*
 * Private methods
 */

/**
 * Load MIDI catalog.
 *
 * Build star list from MIDI catalog stars.
 *
 * \return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is
 * returned.
 */
mcsCOMPL_STAT vobsCATALOG_MIDI::Load()
{
    logExtDbg("vobsCATALOG_MIDI::Load()");
    
    if (_loaded == mcsFALSE)
    {
        //
        // Standard procedure to load catalog
        // ----------------------------------
        if (vobsLOCAL_CATALOG::Load() == mcsFAILURE)
        {
            return mcsFAILURE;
        }

        //
        // MIDI specific loading actions
        // -----------------------------

        // Compute magnitude in N band
        mcsUINT32 starIdx;
        for (starIdx = 0; starIdx < _starList.Size(); starIdx++)
        {
            // Get star
            vobsSTAR *starPtr;
            starPtr = _starList.GetNextStar((mcsLOGICAL)(starIdx==0));

            // Get IR flux
            mcsFLOAT flux;
            mcsFLOAT magnitude;
            starPtr->GetPropertyValue(vobsSTAR_PHOT_FLUX_IR_12, &flux);

            // Compute magnitude
            magnitude = 4.1 - (2.5 * log10(flux/0.89));
            starPtr->SetPropertyValue(vobsSTAR_PHOT_JHN_N, magnitude, GetName());
        }

        // Re-compute diameter error in mas instead of % 
        for (starIdx = 0; starIdx < _starList.Size(); starIdx++)
        {
            // Get star
            vobsSTAR *starPtr;
            starPtr = _starList.GetNextStar((mcsLOGICAL)(starIdx==0));

            // Get diameter and its associated error 
            mcsFLOAT diam;
            mcsFLOAT diamError;
            starPtr->GetPropertyValue(vobsSTAR_DIAM12, &diam);
            starPtr->GetPropertyValue(vobsSTAR_DIAM12_ERROR, &diamError);

            // Convert % to mas 
            diamError = diam * diamError / 100;

            // Rewrite diameter error
            starPtr->SetPropertyValue(vobsSTAR_DIAM12_ERROR, diamError,
                                      GetName(), vobsCONFIDENCE_HIGH, mcsTRUE);
        }

        // If log level is DEBUG or EXTDBG
        if (logGetStdoutLogLevel() >= logDEBUG)
        {
            vobsSTAR_PROPERTY_ID_LIST ucdList;
            //Save star list in a file
            if (_starList.Save("$MCSDATA/tmp/catalogMIDI.dat", ucdList) ==
                mcsFAILURE)
            {
                // Ignore error (for test only)
                errCloseStack();
            }
        }
    }
  
    return mcsSUCCESS;
}

/*___oOo___*/
