/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * \file
 * SearchCal SOAP webservice server.
 *
 * \b Synopsis: \n
 * \<sclwsServer\>
 * 
 * \b Details: \n
 * This daemon listen on port:
 * \li 8079 by default (beta config);
 * \li 8078 for production purpose (code editing required);
 * \li value of the 'SCLWS_PORT_NUMBER' environment variable if defined.
 * 
 * \env
 * \envvar SCLWS_PORT_NB : socket port number the server should bind on (must be greater than 1024, less than 65536).
 */

static char *rcsId __attribute__ ((unused)) = "@(#) $Id: sclwsServer.cpp,v 1.14 2011-04-13 14:34:53 lafrasse Exp $"; 

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
#include "misc.h"
#include "log.h"
#include "err.h"
#include "evh.h"


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
// SOAP execution context
struct Namespace* namespaces;
struct soap       globalSoapContext;

// Port number configuration
mcsENVNAME sclwsPortNumberEnvVarName = "SCLWS_PORT_NB";
uint       sclwsPortNumber           = 8079; // Default value for beta testing.
//uint       sclwsPortNumber           = 8078; // Default value for production purpose.


/*
 * Local Functions
 */
void* sclwsJobHandler(void* soapContextPtr)
{
    struct soap* soapContext = (struct soap*) soapContextPtr;
   
    pthread_detach(pthread_self());

    // Fulfill the received remote call
    soap_serve(soapContext);

    soap_destroy(soapContext); // Dealloc C++ data
    soap_end(soapContext);     // Dealloc data and cleanup
    soap_done(soapContext);    // Detach soap struct

    free(soapContext);

    pthread_exit(NULL);

    return NULL;
}

void sclwsExit(int returnCode)
{
    // Dealloc SOAP context
    logTest("Cleaning gSOAP ...");
    soap_done(&globalSoapContext);

    // Close MCS services
    logDebug("Cleaning MCS ...");
    mcsExit();

    /*
     * Cleanup function for the XML library (libxml2).
     * Library Clean up : must be called only once the process
     * has no more use of the XML library => main exit()
     * http://xmlsoft.org/html/libxml-parser.html
     * 
     * (valgrind check-mem)
     */
    xmlCleanupParser();
    
    // Exit from the application with SUCCESS
    printf("Exiting now.\n");
    exit (returnCode);
}


/* 
 * Signal catching functions  
 */
void sclwsSignalHandler (int signalNumber)
{
    logDebug("Received a '%d' system signal ...", signalNumber);

    if (signalNumber == SIGPIPE)
    {
        return;
    }

    sclwsExit(EXIT_SUCCESS);
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
        sclwsExit(EXIT_FAILURE);
    }
    if (signal(SIGTERM, sclwsSignalHandler) == SIG_ERR)
    {
        logError("signal(SIGTERM, ...) function error");
        sclwsExit(EXIT_FAILURE);
    }
    if (signal(SIGPIPE, sclwsSignalHandler) == SIG_ERR)
    {
        logError("signal(SIGPIPE, ...) function error");
        sclwsExit(EXIT_FAILURE);
    }

    // Initialize MCS services
    if (mcsInit(argv[0]) == mcsFAILURE)
    {
        // Exit from the application with FAILURE
        sclwsExit(EXIT_FAILURE);
    }

    // Used to handle common comand-line options (log levels, ...)
    evhTASK cliHandler;
    if (cliHandler.Init(argc, argv) == mcsFAILURE)
    {
        sclwsExit(EXIT_FAILURE);
    }

    // Try to read ENV. VAR. to get port number to bind
    mcsINT32 envPortNumber = -1;
    mcsINT32 minPortNumber = 1024;
    mcsINT32 maxPortNumber = 65536;
    if (miscGetEnvVarIntValue(sclwsPortNumberEnvVarName, &envPortNumber) == mcsSUCCESS)
    {
        // Use the guiven port only if in the right range.
        if ((envPortNumber <= minPortNumber) || (envPortNumber >= maxPortNumber))
        {
            logError("'%s' environment variable does not contain a valid port number ('%d' not in [%d, %d[)", sclwsPortNumberEnvVarName, envPortNumber, minPortNumber, maxPortNumber);
            sclwsExit(EXIT_FAILURE);
        }

        sclwsPortNumber = envPortNumber;
    }
    // else if the ENV. VAR. is not defined, do nothing (the default value is used instead).

    // @TODO : manage a "garbage collector" thread to close pending connections
    // @TODO : Each connection should have a semaphore incremented at each thread launch and decremented at each thread end, that let sclsvrSERVER instance be deallocated once equal to 0.

    logDebug("Initializing gSOAP ...");

    // SOAP Initialization
    soap_init(&globalSoapContext);
    soap_set_namespaces(&globalSoapContext, soap_namespaces);

    // Main SOAP socket creation
    if (soap_bind(&globalSoapContext, NULL, sclwsPortNumber, 100) < 0)
    {
        errAdd(sclwsERR_BIND, sclwsPortNumber);
        errCloseStack();
        sclwsExit(EXIT_FAILURE);
    }

    logTest("Listening on port '%d'.", sclwsPortNumber);

    globalSoapContext.accept_timeout = 0;
    
    // Infinite loop to receive requests
    for (uint nbOfConnection = 1; ; nbOfConnection++)
    {
        // Wait (without timeout) a new connection
        if (soap_accept(&globalSoapContext) < 0)
        {
            continue;
        }

        // Fork the SOAP context
        struct soap* forkedSoapContext = soap_copy(&globalSoapContext);

        // Start a new thread to handle the request
        pthread_t threadId;
        int status = pthread_create(&threadId, NULL, (void*(*)(void*))sclwsJobHandler, (void*)forkedSoapContext);
        if (status != 0)
        {
            // Error handling
            errAdd(sclwsERR_THREAD_CREATION);
            errCloseStack();
            sclwsExit(EXIT_FAILURE);
        }
    }

    sclwsExit(EXIT_SUCCESS);
}

/*___oOo___*/
