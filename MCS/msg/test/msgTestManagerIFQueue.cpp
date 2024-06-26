/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * \file
 * Small test program of the msgMANAGER_IF message queue.
 *
 * \synopsis
 * \<msgTestManagerIFQueue\>
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
        
        // Exit from the application with FAILURE
        exit (EXIT_FAILURE);
    }

    // Set stdout log level to debug
    logSetStdoutLogLevel(logDEBUG);

    // Try to connect to the <msgManager>
    cout << "Connecting to the <msgManager> ..................... ";
    msgMANAGER_IF manager;
    if (manager.Connect(argv[0]) == mcsFAILURE)
    {
        cout << "KO !!!" << endl;
        errCloseStack();
        exit(EXIT_FAILURE);
    }
    cout << "OK" << endl;

    // Send the specified command to the specified process
    cout << "Sending a query on <sclsvrServer> .................. ";
    mcsCMD   command = "GETCAL";
    mcsINT32 commandId;
    commandId = manager.SendCommand(command, "sclsvrServer", "-objectName ETA_TAU -mag 2.96 -maxReturn 50 -diffRa 1800 -diffDec 300 -band K -minDeltaMag 1 -maxDeltaMag 5 -ra 03:47:29.08 -dec 24:06:18.50 -baseMin 46.64762 -baseMax 102.45 -wlen 0.65 -vis 0.922 -visErr 0.09");
    if (commandId == mcsFAILURE)
    {
        cout << "KO !!!" << endl;
        errCloseStack();
        exit(EXIT_FAILURE);
    }
    cout << "OK" << endl;

    // Create a filter to get only the answer of this query
    msgMESSAGE_FILTER filter(command, commandId);
    cout << "The created " << filter << endl;

    // Create an empty msgMESSAGE to store the received message
    msgMESSAGE message;
    // Wait indefinitly for the expected reply
    cout << "Waiting indefintly for the <sclsvrServer> answer ... ";
    cout.flush();
    if (manager.Receive(message, msgWAIT_FOREVER, filter) == mcsFAILURE)
    {
        cout << "KO !!!" << endl;
        errCloseStack();
        exit(EXIT_FAILURE);
    }
    cout << "OK" << endl;

    // Display the received message content
    cout << "Expected Receive : " << message << endl;

    // Get all the queued messages
    cout << manager.GetNbQueuedMessages() << " Unexpected Receive(s) : " << endl;
    while (manager.GetNbQueuedMessages() > 0)
    {
        manager.GetNextQueuedMessage(message);
        cout << message << endl;
    }

    // Disconnect from msgManager
    manager.Disconnect();

    // Print out error stack if it is not empty
    if (errStackIsEmpty() == mcsFALSE)
    {
        errCloseStack();
        exit(EXIT_FAILURE);
    }

    // Close MCS services
    mcsExit();
    
    // Exit from the application with SUCCESS
    exit (EXIT_SUCCESS);
}


/*___oOo___*/
