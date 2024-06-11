/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * @file
 * sdbEntry class test program.
 */


/* 
 * System Headers 
 */
#include <stdlib.h>
#include <iostream>
#include <unistd.h>


/**
 * @namespace std
 * Export standard iostream objects (cin, cout,...).
 */
using namespace std;


/*
 * MCS Headers 
 */
#include "mcs.h"
#include "log.h"
#include "err.h"
#include "thrd.h"


/*
 * Local Headers 
 */
#include "sdb.h"
#include "sdbPrivate.h"


/*
 * Local functions
 */
thrdFCT_RET myThreadFunction(thrdFCT_ARG param)
{
    sdbENTRY* entry = (sdbENTRY*) param;

    mcsSTRING256 message;
    int i = 1;

    while (1)
    {
        sprintf(message, "The writer is entering sleep for %d seconds", i);
        entry->Write(message);
        sleep(i);
        i++;
    }

    return NULL;
}

 

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
        // Exit from the application with FAILURE
        exit (EXIT_FAILURE);
    }

    logSetStdoutLogLevel(logINFO);

    {
        sdbENTRY entry;

        // Thread creation
        thrdTHREAD_STRUCT    myThread;
        myThread.function  = myThreadFunction;
        myThread.parameter = (thrdFCT_ARG)&entry;
        mcsSTRING256 message;

        thrdThreadCreate(&myThread);

        cout << "Reading message right away :" << endl;
        for (int i = 0; i < 5; i++)
        {
            if (entry.Read(message) == mcsFAILURE)
            {
                errCloseStack();
                exit (EXIT_FAILURE);
            }
            cout << " Reader got message = '" << message << "'." << endl;
        }

        cout << endl;

        cout << "Reading message with a 1 second timeout :" << endl;
        for (int i = 0; i < 5; i++)
        {
            if (entry.Read(message, mcsTRUE, 1000) == mcsFAILURE)
            {
                cout << " Reader timed out." << endl;
            }
            else
            {
                cout << " Redear got message = '" << message << "'." << endl;
            }
        }

        cout << endl;

        cout << "Reading message without any timeout :" << endl;
        for (int i = 0; i < 3; i++)
        {
            if (entry.Read(message, mcsTRUE) == mcsFAILURE)
            {
                errCloseStack();
                exit (EXIT_FAILURE);
            }
            cout << " Redear got message = '" << message << "'." << endl;
        }
    }

    // Close MCS services
    mcsExit();

    // Exit from the application with SUCCESS
    exit (EXIT_SUCCESS);
}


/*___oOo___*/
