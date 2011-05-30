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

    // @TODO : manage a "garbage collector" thread to close pending connections
    // @TODO : Each connection should have a semaphore incremented at each thread launch and decremented at each thread end, that let sclsvrSERVER instance be deallocated once equal to 0.

    logDebug("Initializing gSOAP ...");

    // SOAP Initialization
    soap_init(&globalSoapContext);
    
    // no socket timeout:
    globalSoapContext.accept_timeout = 0;
    
    // reuse server port:
    globalSoapContext.bind_flags = SO_REUSEADDR;
    
    soap_set_namespaces(&globalSoapContext, soap_namespaces);

    mcsUINT16 portNumber = sclwsGetServerPortNumber();

    // Main SOAP socket creation
    SOAP_SOCKET m = soap_bind(&globalSoapContext, NULL, portNumber, 100);
    if ((m < 0) || (!soap_valid_socket(m)))
    {
        errAdd(sclwsERR_BIND, portNumber);
        errCloseStack();
        sclwsExit(EXIT_FAILURE);
    }

    logInfo("Listening on port '%d'.", portNumber);
    
    // Infinite loop to receive requests
    for (uint nbOfConnection = 1; ; nbOfConnection++)
    {
        // Wait (without timeout) a new connection
        SOAP_SOCKET s = soap_accept(&globalSoapContext);
        
        if ((s < 0) || (!soap_valid_socket(s)))
        {
           continue; // retry
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
