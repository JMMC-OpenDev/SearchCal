/*******************************************************************************
* JMMC project
*
* "@(#) $Id: vobsTestCriteria.cpp,v 1.1 2004-12-14 14:45:16 scetre Exp $"
*
* who       when         what
* --------  -----------  -------------------------------------------------------
* scetre    14-Dec-2004  Created
*
*
*******************************************************************************/


static char *rcsId="@(#) $Id: vobsTestCriteria.cpp,v 1.1 2004-12-14 14:45:16 scetre Exp $"; 
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


/*
 * Local Headers 
 */
#include "vobs.h"
#include "vobsPrivate.h"
#include "vobsSTAR_COMP_CRITERIA_LIST.h"

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
    if (mcsInit(argv[0]) == FAILURE)
    {
        // Error handling if necessary
        
        // Exit from the application with FAILURE
        exit (EXIT_FAILURE);
    }

    vobsSTAR_COMP_CRITERIA_LIST criteriaList;

    // Add criteria in the list
    printf("we added a comparaison criteria 'ra' and a range of '0.1'\n");
    criteriaList.Add("ra", 0.1);
    printf("we added a comparaison criteria 'dec' and a range of '0.2'\n");
    criteriaList.Add("dec", 0.2);
    printf("we added a comparaison criteria 'mgK' and a range of '0.0'\n");
    criteriaList.Add("mgK", 0.0);

    // printf all criteria
    mcsSTRING32 propertyId;
    mcsFLOAT range;
    int listSize=criteriaList.Size();
    printf("size of the criteria list created = %d\n", listSize);    
    for (int el = 0; el < listSize; el++)
    {
        criteriaList.GetNextCriteria(propertyId,
                                     &range,
                                     (mcsLOGICAL)(el==0));
        printf("%s = %.1f\n", propertyId, range);
    }

    
    
    // Close MCS services
    mcsExit();
    
    // Exit from the application with SUCCESS
    exit (EXIT_SUCCESS);
}


/*___oOo___*/
