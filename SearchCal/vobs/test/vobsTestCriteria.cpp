/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: vobsTestCriteria.cpp,v 1.6 2005-02-15 15:26:06 gzins Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.5  2005/02/13 08:54:00  gzins
 * Updated after vobs classes changes
 * Added CVS log as modifification history
 *
 * scetre    14-Dec-2004  Created
 *
 ******************************************************************************/


static char *rcsId="@(#) $Id: vobsTestCriteria.cpp,v 1.6 2005-02-15 15:26:06 gzins Exp $"; 
static void *use_rcsId = ((void)&use_rcsId,(void *) &rcsId);


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
    timlogStart(MODULE_ID, logINFO, "73", "testCriteria");
    vobsSTAR_COMP_CRITERIA_LIST *criteriaList = new vobsSTAR_COMP_CRITERIA_LIST;

    // Add criteria in the list
    printf("we added a comparaison criteria 'ra' and a range of '0.1'\n");
    criteriaList->Add(vobsSTAR_POS_EQ_RA_MAIN, 0.1);
    printf("we added a comparaison criteria 'dec' and a range of '0.2'\n");
    criteriaList->Add(vobsSTAR_POS_EQ_DEC_MAIN, 0.1);
    printf("we added a comparaison criteria 'mgK' and a range of '0.0'\n");
    criteriaList->Add(vobsSTAR_PHOT_JHN_K, 0.0);

    // printf all criteria
    mcsSTRING32 propertyId;
    mcsFLOAT range;
    int listSize=criteriaList->Size();
    printf("size of the criteria list created = %d\n", listSize);    
    for (int el = 0; el < listSize; el++)
    {
        criteriaList->GetNextCriteria(propertyId,
                                     &range,
                                     (mcsLOGICAL)(el==0));
        printf("%s = %.1f\n", propertyId, range);
    }

    criteriaList->Remove(vobsSTAR_PHOT_JHN_K);
    listSize=criteriaList->Size();
    for (int el = 0; el < listSize; el++)
    {
        criteriaList->GetNextCriteria(propertyId,
                                     &range,
                                     (mcsLOGICAL)(el==0));
        printf("%s = %.1f\n", propertyId, range);
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
        printf("star1 not equal star2\n");
    }
    else
    {
        printf("star1 equal star2\n");        
    }

    delete criteriaList;

    // Close MCS services
    mcsExit();
    
    // Exit from the application with mcsSUCCESS
    exit (EXIT_SUCCESS);
}


/*___oOo___*/
