/*******************************************************************************
* JMMC project
*
* "@(#) $Id: sclsvrTest.cpp,v 1.1 2004-12-05 21:05:50 gzins Exp $"
*
* who       when         what
* --------  -----------  -------------------------------------------------------
* scetre    15-Sep-2004  Created
*
*
*******************************************************************************/

/**
 * \file
 * Test programm of sclsvr.
  */

static char *rcsId="@(#) $Id: sclsvrTest.cpp,v 1.1 2004-12-05 21:05:50 gzins Exp $"; 
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

    sclsvrSEARCH_CALIBRATORS searchCalibrators;
    searchCalibrators.DecodeCommand("test");
    searchCalibrators.Run();

    logInfo("Exiting ...");


    // Close MCS services
    mcsExit();
    
    // Exit from the application with SUCCESS
    exit (EXIT_SUCCESS);
}


/*___oOo___*/
