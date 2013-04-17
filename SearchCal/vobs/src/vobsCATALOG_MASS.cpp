/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * @file
 * vobsCATALOG_MASS class definition.
 * 
 * The 2MASS catalog ["II/246/out"] is used in primary request for FAINT scenario and in secondary requests for BRIGHT scenarios 
 * to get many magnitudes
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
#include "vobsCATALOG_MASS.h"
#include "vobsPrivate.h"
#include "vobsErrors.h"

/*
 * North: 1997 June 7 - 2000 December 1 UT (mjd = 50606)
 * South: 1998 March 18 - 2001 February 15 UT (mjd = 51955)
 */

/*
 * Class constructor
 */
vobsCATALOG_MASS::vobsCATALOG_MASS() : vobsREMOTE_CATALOG(vobsCATALOG_MASS_ID)
{
}

/*
 * Class destructor
 */
vobsCATALOG_MASS::~vobsCATALOG_MASS()
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
mcsCOMPL_STAT vobsCATALOG_MASS::ProcessList(vobsSCENARIO_RUNTIME &ctx, vobsSTAR_LIST &list)
{
    const mcsUINT32 listSize = list.Size();

    if (listSize > 0)
    {
        logDebug("ProcessList: list Size = %d", listSize);

        // call parent implementation first:
        FAIL(vobsREMOTE_CATALOG::ProcessList(ctx, list));

        // keep only flux whom quality is between (A and E) (vobsSTAR_CODE_QUALITY property Qflg column)
        // ie ignore F, X or U flagged data
        static const char* fluxProperties[] = {vobsSTAR_PHOT_JHN_J, vobsSTAR_PHOT_JHN_H, vobsSTAR_PHOT_JHN_K};

        int idIdx = -1, qFlagIdx = -1, optIdx = -1;
        vobsSTAR_PROPERTY *qFlagProperty;
        vobsSTAR* star = NULL;
        const char *starId, *code;
        char ch;

        // For each star of the list
        for (star = list.GetNextStar(mcsTRUE); star != NULL; star = list.GetNextStar(mcsFALSE))
        {
            if (idIdx == -1)
            {
                idIdx = star->GetPropertyIndex(vobsSTAR_ID_2MASS);
                qFlagIdx = star->GetPropertyIndex(vobsSTAR_CODE_QUALITY);
                optIdx = star->GetPropertyIndex(vobsSTAR_2MASS_OPT_ID_CATALOG);
            }

            // Get the star ID (logs)
            starId = star->GetProperty(idIdx)->GetValue();

            // Get QFlg property:
            qFlagProperty = star->GetProperty(qFlagIdx);

            // test if property is set
            if (qFlagProperty->IsSet() == mcsTRUE)
            {
                code = qFlagProperty->GetValue();

                if (strlen(code) == 3)
                {
                    for (int i = 0; i < 3; i++)
                    {
                        ch = code[i];

                        // check quality between (A and E)
                        if ((ch < 'A') && (ch > 'E'))
                        {
                            logTest("Star '2MASS %s' - clear property %s (bad quality = '%c')", starId, fluxProperties[i], ch);

                            star->ClearPropertyValue(fluxProperties[i]);
                        }
                    }
                }
                else
                {
                    logTest("Star '2MASS %s' - invalid Qflg value '%s'", starId, code);
                }
            }
        }
    }

    return mcsSUCCESS;
}

/*___oOo___*/
