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
vobsCATALOG_DENIS::vobsCATALOG_DENIS() : vobsREMOTE_CATALOG(vobsCATALOG_DENIS_ID, 1.0, 1995.5, 2002.5)
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

                if (((iFlag & 0x4) != 0) || ((iFlag & 0x100) != 0))
                {
                    logTest("Star 'DENIS %s' - discard I Cousin magnitude (saturated or clouds - Iflg = '%s')", starId, code);

                    magIcousProperty->ClearValue();
                    iFlagProperty->ClearValue();
                }
            }
        }
    }

    return mcsSUCCESS;
}

/*
 * Private methods
 */

/**
 * Build the specific part of the asking.
 *
 * Build the specific part of the asking. This is the part of the asking
 * which is write specificaly for each catalog.
 *
 * @return always mcsSUCCESS 
 */
mcsCOMPL_STAT vobsCATALOG_DENIS::WriteQuerySpecificPart(void)
{
    // SECONDARY REQUEST: cone search arround given star coordinates for the FAINT scenario 

    // Get the identifier DENIS (ID_MAIN) stored in the 'vobsSTAR_ID_DENIS' property
    miscDynBufAppendString(&_query, "&-out=DENIS");

    // Get the Julian date of source measurement (TIME_DATE) stored in the 'vobsSTAR_JD_DATE' property
    miscDynBufAppendString(&_query, "&-out=ObsJD");

    // A2RAdeg / A2DEdeg = USNOA2.0 nearest match: TODO what use = query USNO catalog ?

    // Get the RA  USNOA2.0 nearest match A2RAdeg (POS_EQ_RA_OTHER)  stored in the 'vobsSTAR_POS_EQ_RA_OTHER' property
    miscDynBufAppendString(&_query, "&-out=A2RAdeg");

    // Get the DEC USNOA2.0 nearest match A2DEdeg (POS_EQ_DEC_OTHER) stored in the 'vobsSTAR_POS_EQ_DEC_OTHER' property
    miscDynBufAppendString(&_query, "&-out=A2DEdeg");

    // Get the cousin magnitude Imag at 0.82 mu (PHOT_COUS_I) stored in the 'vobsSTAR_PHOT_COUS_I' property
    miscDynBufAppendString(&_query, "&-out=Imag");

    // Get the quality flag Iflg (CODE_MISC) stored in the 'vobsSTAR_CODE_MISC_I' property
    miscDynBufAppendString(&_query, "&-out=Iflg");

    // TODO: decide if photographic blue and red magnitudes should be removed definitely (Denis, USNO, 2MASS ...)
    // because useless for computations only used by GUI (display)

    // Get the photometric magnitude Bmag (PHOT_PHG_B) stored in the 'vobsSTAR_PHOT_PHG_B' property
    miscDynBufAppendString(&_query, "&-out=Bmag");

    // Get the photometric magnitude Rmag (PHOT_PHG_R) stored in the 'vobsSTAR_PHOT_PHG_R' property
    miscDynBufAppendString(&_query, "&-out=Rmag");

    return mcsSUCCESS;
}

/*___oOo___*/
