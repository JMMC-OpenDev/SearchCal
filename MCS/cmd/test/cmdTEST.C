/*******************************************************************************
* JMMC project
*
* "@(#) $Id: cmdTEST.C,v 1.1 2004-11-19 16:29:41 mella Exp $"
*
* who       when         what
* --------  -----------  -------------------------------------------------------
* mella     16-Nov-2004  Created
*
*
*******************************************************************************/

/**
 * \file
 *  Simple test file for cmdCMD class
 */

static char *rcsId="@(#) $Id: cmdTEST.C,v 1.1 2004-11-19 16:29:41 mella Exp $"; 
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
#include "cmd.h"
#include "cmdCMD.h"
#include "cmdPARAM.h"
#include "cmdPrivate.h"


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

    logSetStdoutLogLevel(logEXTDBG);
   

    if(argc == 1)
    {
        cout<<"Simple default command"<<endl;
        cmdCMD myCmd("VALID", "-integer 1 -double 0.1 -boolean true -string \"str with blank and --\"");
        string help;
        help = myCmd.getHelp();
        cout<<help<<endl;
    }
    
    if(argc == 2)
    {
        cout<<"User command " << argv[1] <<endl;
        cmdCMD myCmd(argv[1], "-integer 1 -double 0.1 -boolean true -string \"str with blank and --\"");
        string help;
        help = myCmd.getHelp();
        cout<<help<<endl;
    }
    

    
    // Close MCS services
    mcsExit();
    
    // Exit from the application with SUCCESS
    exit (EXIT_SUCCESS);
}


/*___oOo___*/
