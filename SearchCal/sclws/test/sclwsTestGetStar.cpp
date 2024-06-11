/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * @file
 * Test program implementing a clinet to web service
 */


/* 
 * System Headers 
 */
#include <stdlib.h>
#include <iostream>
#include <signal.h>

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
#include "timlog.h"

/*
 * Local Headers 
 */
#include "soapH.h"
#include "soap.nsmap"
#include "sclwsPrivate.h"
#include "sclwsErrors.h"

/*
 * Local Variables
 */
struct Namespace *namespaces;
mcsSTRING256 sclwsURL;


void sclwsThreadStats(mcsUINT32 *threadCreated, mcsUINT32 *threadJoined)
{
// Not implemented function
}

/* 
 * Signal catching functions  
 */
void sclwsSignalHandler (int signalNumber)
{
    logInfo("Received a '%d' system signal ...", signalNumber);

    if (signalNumber == SIGPIPE)
    {
        return;
    }

    // Close MCS services
    mcsExit();
    
    // Exit from the application with SUCCESS
    exit (EXIT_SUCCESS);
}


/* 
 * Main
 */
int main(int argc, char *argv[])
{
    /* Init system signal trapping */
    if (signal(SIGINT, sclwsSignalHandler) == SIG_ERR)
    {
        logError("signal(SIGINT, ...) function error");
        exit (EXIT_FAILURE);
    }
    if (signal(SIGTERM, sclwsSignalHandler) == SIG_ERR)
    {
        logError("signal(SIGTERM, ...) function error");
        exit (EXIT_FAILURE);
    }
    if (signal(SIGPIPE, sclwsSignalHandler) == SIG_ERR)
    {
        logError("signal(SIGPIPE, ...) function error");
        exit (EXIT_FAILURE);
    }

    // Initialize MCS services
    if (mcsInit(argv[0]) == mcsFAILURE)
    {
        // Error handling if necessary

        // Exit from the application with FAILURE
        exit (EXIT_FAILURE);
    }

    // Set stdout log level 
    logSetStdoutLogLevel(logINFO);

    // Composing server address
    char* url = "http://jmmc.fr";
    mcsUINT16 portNumber = sclwsGetServerPortNumber();
    snprintf(sclwsURL, sizeof(sclwsURL), "%s:%d", url, portNumber);

    char* query = argv[1];
    char*  result  = NULL;

    // Define a SOAP context for main()
    struct soap v_soap;
    soap_init(&v_soap);
    soap_set_namespaces(&v_soap, soap_namespaces);

    // Start timer log
    timlogInfoStart("GETSTAR");

    // Launch the GETSTAR query
    soap_call_ns__GetStar(&v_soap, sclwsURL, "", query, &result);

    // Stop timer log
    timlogStop("GETSTAR");

    // Check completion status
    if (v_soap.error)
    {
	    soap_print_fault(&v_soap, stderr);
	    exit (EXIT_FAILURE);
    }

    if (strlen(result) == 0)
    {
	logWarning("No star found");
    }
    else
    {
	// Output the received VO Table on stdout
	cout << result << endl;
    }

    // Dealloc SOAP context
    soap_end(&v_soap);
    logInfo("Connection closed.");

    // Close MCS services
    mcsExit();
    
    // Exit from the application with SUCCESS
    exit (EXIT_SUCCESS);
}


/*___oOo___*/
