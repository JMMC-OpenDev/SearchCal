/*******************************************************************************
* JMMC project
*
* "@(#) $Id: sclsvrSearchCalibrators.C,v 1.1 2004-11-26 13:53:56 scetre Exp $"
*
* who       when         what
* --------  -----------  -------------------------------------------------------
* scetre    04-Oct-2004  Created
*
*
*******************************************************************************/

/**
 * \file
 * Test programm of sclsvr.
 */

static char *rcsId="@(#) $Id: sclsvrSearchCalibrators.C,v 1.1 2004-11-26 13:53:56 scetre Exp $"; 
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
#include "sclsvr.h"
#include "sclsvrPrivate.h"


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

    logInfo("Starting ...");    

    logSetStdoutLogLevel(logEXTDBG);
    
    vobsSTAR star;
    star.SetProperty(DATA_LINK_ID, "Hd");
    sclsvrCALIBRATOR calib(star);
    sclsvrCALIBRATOR_LIST list;
    //calib.Display();
    calib.SetProperty(DATA_LINK_ID, "Hd2", mcsTRUE);
    calib.SetProperty(VISIBILITY_ID, 0.3198, mcsTRUE);
    //calib.Display();
    list.AddAtTail(star);
    list.AddAtTail(calib);
    list.Display();
    errDisplayStack();
    // Close MCS services
    mcsExit();
    
    // Exit from the application with SUCCESS
    exit (EXIT_SUCCESS);
}


/*___oOo___*/
