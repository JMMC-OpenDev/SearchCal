/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/




/* 
 * System Headers 
 */
#include <stdlib.h>
#include <iostream>

/**
 * \namespace std
 * Export standard iostream objects (cin, cout,...).
 */
using namespace std;


/*
 * MCS Headers 
 */
#include "mcs.h"
#include "log.h"
#include "err.h"

/*
 * Local Headers 
 */
#include "vobs.h"
#include "vobsPrivate.h"
#include "vobsSTAR_COMP_CRITERIA_LIST.h"
#include "vobsSTAR.h"
/*
 * Local Variables
 */



/* 
 * Signal catching functions  
 */

/* 
 * Main
 */

int main(int argc, char *argv[])
{
    // Initialize MCS services
    if (mcsInit(argv[0]) == mcsFAILURE)
    {
        // Error handling if necessary

        // Exit from the application with mcsFAILURE
        exit(EXIT_FAILURE);
    }

    logSetStdoutLogLevel(logTEST);
    logSetPrintDate(mcsFALSE);
    logSetPrintFileLine(mcsFALSE);

    logInfo("Starting ...");

    // create stars to build property index now:
    vobsSTAR star1;
    vobsSTAR star2;

    vobsSTAR_COMP_CRITERIA_LIST *criteriaList = new vobsSTAR_COMP_CRITERIA_LIST;

    // Add criteria in the list
    logTest("we added a comparaison criteria 'ra' and a range of '0.1'");
    criteriaList->Add(vobsSTAR_POS_EQ_RA_MAIN, 0.1);
    logTest("we added a comparaison criteria 'dec' and a range of '0.2'");
    criteriaList->Add(vobsSTAR_POS_EQ_DEC_MAIN, 0.1);
    logTest("we added a comparaison criteria 'mgK' and a range of '0.0'");
    criteriaList->Add(vobsSTAR_PHOT_JHN_K, 0.0);

    // logTest all criteria

    // Get criteria informations
    int nCriteria = 0;
    vobsSTAR_CRITERIA_INFO* criterias = NULL;
    vobsSTAR_CRITERIA_INFO* criteria = NULL;

    // Get criterias:
    if (criteriaList->GetCriterias(criterias, nCriteria) == mcsFAILURE)
    {
        return mcsFALSE;
    }

    logTest("size of the criteria list created = %d", nCriteria);

    // Get each criteria of the list and check if the comparaison with all
    // this criteria gave a equality

    for (int el = 0; el < nCriteria; el++)
    {
        criteria = &criterias[el];

        if ((criteria->propCompType == vobsPROPERTY_COMP_RA_DEC))
        {
            logTest("%s = %.1lf / %.1lf", criteria->propertyId, criteria->rangeRA, criteria->rangeDEC);
        }
        else
        {
            logTest("%s = %.1lf", criteria->propertyId, criteria->range);
        }
    }

    criteriaList->Remove(vobsSTAR_PHOT_JHN_K);

    // Get criterias:
    if (criteriaList->GetCriterias(criterias, nCriteria) == mcsFAILURE)
    {
        return mcsFALSE;
    }

    for (int el = 0; el < nCriteria; el++)
    {
        criteria = &criterias[el];

        if ((criteria->propCompType == vobsPROPERTY_COMP_RA_DEC))
        {
            logTest("%s = %.1lf / %.1lf", criteria->propertyId, criteria->rangeRA, criteria->rangeDEC);
        }
        else
        {
            logTest("%s = %.1lf", criteria->propertyId, criteria->range);
        }
    }

    star1.SetPropertyValue(vobsSTAR_POS_EQ_RA_MAIN, "03 47 29.08", vobsNO_CATALOG_ID);
    star1.SetPropertyValue(vobsSTAR_POS_EQ_DEC_MAIN, "+24 06 18.5", vobsNO_CATALOG_ID);

    star2.SetPropertyValue(vobsSTAR_POS_EQ_RA_MAIN, "03 47 29.08", vobsNO_CATALOG_ID);
    star2.SetPropertyValue(vobsSTAR_POS_EQ_DEC_MAIN, "+24 06 18.5", vobsNO_CATALOG_ID);

    if (star1.IsMatchingCriteria(&star2, criterias, nCriteria) == mcsFALSE)
    {
        logTest("star1 not equal star2");
    }
    else
    {
        logTest("star1 equal star2");
    }

    delete criteriaList;
    logInfo("Exiting ...");

    // Close MCS services
    mcsExit();

    // Exit from the application with mcsSUCCESS
    exit(EXIT_SUCCESS);
}


/*___oOo___*/
