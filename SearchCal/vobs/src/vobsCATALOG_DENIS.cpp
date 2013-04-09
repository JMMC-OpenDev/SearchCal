/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * @file
 * vobsCATALOG_DENIS class definition.
 * 
 * The DENIS catalog ["B/denis"] is used in secondary requests for the FAINT scenario
 * to get cousin I magnitude, photometric R and B magnitudes and USNO coordinates
 */


/* 
 * System Headers 
 */
#include <iostream>
#include <stdio.h>
using namespace std;


/*
 * MCS Headers 
 */
#include "mcs.h"
#include "log.h"
#include "err.h"
#include "misc.h"

/*
 * Local Headers 
 */
#include "vobsCATALOG_DENIS.h"
#include "vobsPrivate.h"
#include "vobsErrors.h"

/*
 * Class constructor
 */
vobsCATALOG_DENIS::vobsCATALOG_DENIS() : vobsREMOTE_CATALOG(vobsCATALOG_DENIS_ID)
{
}

/*
 * Class destructor
 */
vobsCATALOG_DENIS::~vobsCATALOG_DENIS()
{
}

/*
 * Protected methods
 */

/**
 * Method to process the output star list from the catalog
 * 
 * @param list a vobsSTAR_LIST as the result of the search
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
 */
mcsCOMPL_STAT vobsCATALOG_DENIS::ProcessList(vobsSTAR_LIST &list)
{
    const mcsUINT32 listSize = list.Size();

    if (listSize > 0)
    {
        logDebug("ProcessList: list Size = %d", listSize);

        // call parent implementation first:
        FAIL(vobsREMOTE_CATALOG::ProcessList(list));


        // Check flag related to I magnitude
        // Note (2):
        // This flag is the concatenation of image and source flags, in hexadecimal format.
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

        int idIdx = -1, iFlagIdx = -1, magICousIdx = -1;
        vobsSTAR_PROPERTY *iFlagProperty, *magIcousProperty;
        vobsSTAR* star = NULL;
        const char *starId, *code;
        int iFlag;

        // For each star of the list
        for (star = list.GetNextStar(mcsTRUE); star != NULL; star = list.GetNextStar(mcsFALSE))
        {
            if (idIdx == -1)
            {
                idIdx = star->GetPropertyIndex(vobsSTAR_ID_DENIS);
                iFlagIdx = star->GetPropertyIndex(vobsSTAR_CODE_MISC_I);
                magICousIdx = star->GetPropertyIndex(vobsSTAR_PHOT_COUS_I);
            }

            // Get the star ID (logs)
            starId = star->GetProperty(idIdx)->GetValue();

            // Get Imag property:
            magIcousProperty = star->GetProperty(magICousIdx);

            // Get I Flag property:
            iFlagProperty = star->GetProperty(iFlagIdx);

            // test if property is set
            if ((magIcousProperty->IsSet() == mcsTRUE) && (iFlagProperty->IsSet() == mcsTRUE))
            {
                // Check if it is saturated or there was clouds during observation

                // Get Iflg value as string
                code = iFlagProperty->GetValue();

                // Convert it into integer; hexadecimal conversion
                sscanf(code, "%x", &iFlag);

                // discard all flagged observation
                if (iFlag != 0)
                {
                    logTest("Star 'DENIS %s' - discard I Cousin magnitude (saturated or clouds - Iflg = '%s')", starId, code);

                    magIcousProperty->ClearValue();
                    // iFlagProperty->ClearValue(); // Keep value
                }
            }
        }
    }

    return mcsSUCCESS;
}

/*___oOo___*/
