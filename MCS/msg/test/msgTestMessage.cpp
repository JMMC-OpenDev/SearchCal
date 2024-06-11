/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * \file
 * Test program for msgMESSAGE class.
 */


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
#include "msg.h"
#include "msgPrivate.h"


/* 
 * Main
 */
int main(int argc, char *argv[])
{
    msgMESSAGE msg;

    // Initialize MCS services
    if (mcsInit(argv[0]) == mcsFAILURE)
    {
        // Exit from the application with FAILURE
        exit (EXIT_FAILURE);
    }

    // Set body
    if (msg.SetBody("OK") == mcsFAILURE)
    {
        goto errCond;
    }
    printf("Body size    = %d\n", msg.GetBodySize()); 
    printf("Message body = %s\n", msg.GetBody()); 

    if (msg.SetBodyArgs("String '%s', integer '%d', float '%.3f'",
                        "Hello World", 666, 123.456) == mcsFAILURE)
    {
        goto errCond;
    }
    printf("Body size    = %d\n", msg.GetBodySize()); 
    printf("Message body = %s\n", msg.GetBody()); 

   // Close MCS services
    mcsExit();
    
    // Exit from the application with SUCCESS
    exit (EXIT_SUCCESS);

// If an error occured, show the error stack and exit
errCond:
    errCloseStack();

    mcsExit();
    exit(EXIT_FAILURE);
}


/*___oOo___*/
