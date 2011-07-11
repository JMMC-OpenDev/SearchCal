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
#include "timlog.h"

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
        exit (EXIT_FAILURE);
    }

    logSetStdoutLogLevel(logTEST);
    logSetPrintDate(mcsFALSE);
    logSetPrintFileLine(mcsFALSE);

    logInfo("Starting ...");
    
    timlogStart(MODULE_ID, logINFO, "73", "testCriteria");
    vobsSTAR_COMP_CRITERIA_LIST *criteriaList = new vobsSTAR_COMP_CRITERIA_LIST;

    // Add criteria in the list
    logTest("we added a comparaison criteria 'ra' and a range of '0.1'");
    criteriaList->Add(vobsSTAR_POS_EQ_RA_MAIN, 0.1);
    logTest("we added a comparaison criteria 'dec' and a range of '0.2'");
    criteriaList->Add(vobsSTAR_POS_EQ_DEC_MAIN, 0.1);
    logTest("we added a comparaison criteria 'mgK' and a range of '0.0'");
    criteriaList->Add(vobsSTAR_PHOT_JHN_K, 0.0);

    // logTest all criteria
    mcsSTRING32 propertyId;
    const char* ptr = propertyId;
    mcsDOUBLE range;
    mcsINT32 listSize = criteriaList->Size();
    logTest("size of the criteria list created = %d", listSize);    
    for (int el = 0; el < listSize; el++)
    {
        criteriaList->GetNextCriteria(&ptr,
                                      &range,
                                      (mcsLOGICAL)(el==0));
        logTest("%s = %.1f", propertyId, range);
    }

    criteriaList->Remove(vobsSTAR_PHOT_JHN_K);
    listSize = criteriaList->Size();
    for (int el = 0; el < listSize; el++)
    {
        criteriaList->GetNextCriteria(&ptr,
                                      &range,
                                      (mcsLOGICAL)(el==0));
        logTest("%s = %.1f", propertyId, range);
    }

    timlogStop("testCriteria");
    
    vobsSTAR star1;
    star1.SetPropertyValue(vobsSTAR_POS_EQ_RA_MAIN, "03 47 29.08", "");
    star1.SetPropertyValue(vobsSTAR_POS_EQ_DEC_MAIN, "+24 06 18.5", "");

    vobsSTAR star2;
    star2.SetPropertyValue(vobsSTAR_POS_EQ_RA_MAIN, "03 47 29.08", "");
    star2.SetPropertyValue(vobsSTAR_POS_EQ_DEC_MAIN, "+24 06 18.5", "");

    if (star1.IsSame(star2, criteriaList) == mcsFALSE)
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
    exit (EXIT_SUCCESS);
}


/*___oOo___*/
