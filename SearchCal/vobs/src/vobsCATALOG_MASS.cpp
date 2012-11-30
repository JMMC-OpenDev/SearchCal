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
vobsCATALOG_MASS::vobsCATALOG_MASS() : vobsREMOTE_CATALOG(vobsCATALOG_MASS_ID, 1.0, 1997.43, 2001.13)
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
mcsCOMPL_STAT vobsCATALOG_MASS::ProcessList(vobsSTAR_LIST &list)
{
    const mcsUINT32 listSize = list.Size();

    if (listSize > 0)
    {
        logDebug("ProcessList: list Size = %d", listSize);

        // call parent implementation first:
        FAIL(vobsREMOTE_CATALOG::ProcessList(list));

        // keep only flux whom quality is between (A and E) (vobsSTAR_CODE_QUALITY property Qflg column)
        // ie ignore F, X or U flagged data
        static const char* fluxProperties[] = {vobsSTAR_PHOT_JHN_J, vobsSTAR_PHOT_JHN_H, vobsSTAR_PHOT_JHN_K};

        int idIdx = -1, qFlagIdx = -1, optIdx = -1, magPhgBIdx = -1, magPhgRIdx = -1, magJhnBIdx = -1, magJhnVIdx = -1;
        vobsSTAR_PROPERTY *qFlagProperty, *optProperty;
        vobsSTAR_PROPERTY *magPhgBProperty, *magPhgRProperty, *magJhnBProperty, *magJhnVProperty;
        vobsSTAR* star = NULL;
        const char *starId, *code, *opt;
        char ch;
        mcsDOUBLE mag;

        // For each star of the list
        for (star = list.GetNextStar(mcsTRUE); star != NULL; star = list.GetNextStar(mcsFALSE))
        {
            if (idIdx == -1)
            {
                idIdx = star->GetPropertyIndex(vobsSTAR_ID_2MASS);
                qFlagIdx = star->GetPropertyIndex(vobsSTAR_CODE_QUALITY);
                optIdx = star->GetPropertyIndex(vobsSTAR_2MASS_OPT_ID_CATALOG);

                magPhgBIdx = star->GetPropertyIndex(vobsSTAR_PHOT_PHG_B);
                magPhgRIdx = star->GetPropertyIndex(vobsSTAR_PHOT_PHG_R);

                magJhnBIdx = star->GetPropertyIndex(vobsSTAR_PHOT_JHN_B);
                magJhnVIdx = star->GetPropertyIndex(vobsSTAR_PHOT_JHN_V);
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

            // Dec2012: do not get anymore these values (USNO is better and ASCC too)
            if (false)
            {
                // filter Rmag/Bmag according to Associated optical source (opt):
                optProperty = star->GetProperty(optIdx);

                // test if property is set
                if (optProperty->IsSet() == mcsTRUE)
                {
                    opt = optProperty->GetValue();

                    if (strlen(opt) == 1)
                    {
                        ch = opt[0];

                        /* 
                         * http://www.ipac.caltech.edu/2mass/releases/allsky/doc/sec4_4f.html
                         * 
                         * The optical magnitudes listed for the optical associations in the PSC are derived from the Tycho 2 and USNO-A2.0 values. 
                         * In the case of Tycho 2 associations, the optical magnitudes listed are Johnson B and V magnitudes and are derived from 
                         * the Tycho blue (BT) and visual (VT) magnitudes using the transformations given by (Høg et al. 2000, "Guide to the Tycho-2 Catalog"):
                         * 
                         * V = VT - 0.090 * (BT - VT)
                         * B-V = 0.850 * (BT - VT)
                         * 
                         * For USNO-A2.0 associations, the optical magnitudes are the photographic blue and red magnitudes taken explicitly from the USNO-A2.0 Catalog.      
                         */

                        // TODO: decide if photographic blue and red magnitudes should be removed definitely (Denis, USNO, 2MASS ...)
                        // because useless for computations only used by GUI (display)

                        if (ch == 'T')
                        {
                            // move Tycho magnitudes into proper properties:

                            // vobsSTAR_PHOT_PHG_B => vobsSTAR_PHOT_JHN_B
                            magPhgBProperty = star->GetProperty(magPhgBIdx);
                            magJhnBProperty = star->GetProperty(magJhnBIdx);

                            if ((magPhgBProperty->IsSet() == mcsTRUE) && (magJhnBProperty->IsSet() == mcsFALSE))
                            {
                                FAIL(magPhgBProperty->GetValue(&mag));
                                FAIL(magJhnBProperty->SetValue(mag, magPhgBProperty->GetOrigin()));
                            }
                            magPhgBProperty->ClearValue();

                            // vobsSTAR_PHOT_PHG_R => vobsSTAR_PHOT_JHN_V
                            magPhgRProperty = star->GetProperty(magPhgRIdx);
                            magJhnVProperty = star->GetProperty(magJhnVIdx);

                            if ((magPhgRProperty->IsSet() == mcsTRUE) && (magJhnVProperty->IsSet() == mcsFALSE))
                            {
                                FAIL(magPhgRProperty->GetValue(&mag));
                                FAIL(magJhnVProperty->SetValue(mag, magPhgRProperty->GetOrigin()));
                            }
                            magPhgRProperty->ClearValue();
                        }
                    }
                    else
                    {
                        logTest("Star '2MASS %s' - invalid opt value '%s'", starId, opt);
                    }
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
mcsCOMPL_STAT vobsCATALOG_MASS::WriteQuerySpecificPart(void)
{
    // SECONDARY REQUEST: cone search arround given star coordinates for BRIGHT scenarios

    // Get the identifier 2MASS (ID_MAIN) stored in the 'vobsSTAR_ID_2MASS' property
    miscDynBufAppendString(&_query, "&-out=2MASS");

    // Get the Julian date of source measurement (TIME_DATE) stored in the 'vobsSTAR_JD_DATE' property
    miscDynBufAppendString(&_query, "&-out=JD");

    // Get the associated optical source opt (ID_CATALOG) stored in the 'vobsSTAR_2MASS_OPT_ID_CATALOG' property
    miscDynBufAppendString(&_query, "&-out=opt");

    // Get the quality flag Qflg (CODE_QUALITY) stored in the 'vobsSTAR_CODE_QUALITY' property
    miscDynBufAppendString(&_query, "&-out=Qflg");

    // Get the johnson magnitude Jmag (PHOT_JHN_J) stored in the 'vobsSTAR_PHOT_JHN_J' property
    miscDynBufAppendString(&_query, "&-out=Jmag");

    // Get the johnson magnitude Hmag (PHOT_JHN_H) stored in the 'vobsSTAR_PHOT_JHN_H' property
    miscDynBufAppendString(&_query, "&-out=Hmag");

    // Get the johnson magnitude Kmag (PHOT_JHN_K) stored in the 'vobsSTAR_PHOT_JHN_K' property
    miscDynBufAppendString(&_query, "&-out=Kmag");

    /* 
     * The optical magnitudes listed for the optical associations in the PSC are derived from the Tycho 2 and USNO-A2.0 values. 
     * In the case of Tycho 2 associations, the optical magnitudes listed are Johnson B and V magnitudes and are derived from 
     * the Tycho blue (BT) and visual (VT) magnitudes using the transformations given by (Høg et al. 2000, "Guide to the Tycho-2 Catalog").
     * 
     * For USNO-A2.0 associations, the optical magnitudes are the photographic blue and red magnitudes taken explicitly from the USNO-A2.0 Catalog.      
     */

    // TODO: decide if photographic blue and red magnitudes should be removed definitely (Denis, USNO, 2MASS ...)
    // because useless for computations only used by GUI (display)

    // Dec2012: do not get anymore these values (USNO is better and ASCC too)

    // Get the photometric magnitude Bmag (PHOT_PHG_B) stored in the 'vobsSTAR_PHOT_PHG_B' property
    //    miscDynBufAppendString(&_query, "&-out=Bmag");

    // Get the photometric magnitude Rmag (PHOT_PHG_R) stored in the 'vobsSTAR_PHOT_PHG_R' property
    //    miscDynBufAppendString(&_query, "&-out=Rmag");

    return mcsSUCCESS;
}

/*___oOo___*/
