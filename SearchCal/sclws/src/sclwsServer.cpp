/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: sclwsServer.cpp,v 1.13 2011-04-13 12:56:12 lafrasse Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.12  2010/12/02 13:54:19  lafrasse
 * Commit defualt port of berta version as we do not commit from production account
 * 'sclws'.
 *
 * Revision 1.11  2010/01/22 15:41:29  lafrasse
 * Re-commited production port number (8078 instead of 8079 used for beta-testing).
 *
 * Revision 1.10  2010/01/22 15:35:29  lafrasse
 * Log refinments.
 *
 * Revision 1.9  2009/04/17 15:38:44  lafrasse
 * Added connected IP logging, plus various logging level refinments.
 * Code and documentation enhancement.
 *
 * Revision 1.8  2007/11/12 10:32:15  lafrasse
 * Update Web Service API function name.
 * Update documentation and traces.
 * Added support for command line option parsing and handling.
 *
 * Revision 1.7  2007/07/03 17:07:58  lafrasse
 * Corrected connection port number.
 *
 * Revision 1.6  2007/07/03 17:00:03  lafrasse
 * Added support for query cancellation.
 *
 * Revision 1.5  2007/07/02 13:58:47  lafrasse
 * Added signal and error handling, plus proper exit.
 *
 * Revision 1.4  2007/02/16 09:54:55  lafrasse
 * Refined log management.
 *
 * Revision 1.3  2007/02/09 17:07:46  lafrasse
 * Enhanced log and error monitoring.
 * Corrected a bug inherent to early deallocation of sclsvrSERVER _progress
 * leading to a crash on empty resulting queries.
 *
 * Revision 1.2  2007/02/04 20:56:45  lafrasse
 * Updated webservice URL port number.
 * Updated according to APIs changes in sclsvr.
 *
 * Revision 1.1  2006/12/22 15:17:50  lafrasse
 * Creation
 *
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

static char *rcsId __attribute__ ((unused)) = "@(#) $Id: sclwsServer.cpp,v 1.13 2011-04-13 12:56:12 lafrasse Exp $"; 

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
mcsENVNAME portNumberEnvVarName = "SCLWS_PORT_NB";
uint       portNumber           = 8079; // Default value for beta testing.
//uint       portNumber           = 8078; // Default value for production purpose.


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

    // Try to read ENV. VAR. to get port number to bind
    mcsINT32 envPortNumber = -1;
    mcsINT32 minPortNumber = 1024;
    mcsINT32 maxPortNumber = 65536;
    if (miscGetEnvVarIntValue(portNumberEnvVarName, &envPortNumber) == mcsSUCCESS)
    {
        // Use the guiven port only if in the right range.
        if ((envPortNumber <= minPortNumber) || (envPortNumber >= maxPortNumber))
        {
            logError("'%s' environment variable does not contain a valid port number ('%d' not in [%d, %d[)", portNumberEnvVarName, envPortNumber, minPortNumber, maxPortNumber);
            sclwsExit(EXIT_FAILURE);
        }

        portNumber = envPortNumber;
    }
    // else if the ENV. VAR. is not defined, do nothing (the default value is used instead).

    // @TODO : manage a "garbage collector" to close pending connections

    logDebug("Initializing gSOAP ...");

    // SOAP Initialization
    soap_init(&globalSoapContext);
    soap_set_namespaces(&globalSoapContext, soap_namespaces);

    // Main SOAP socket creation
    if (soap_bind(&globalSoapContext, NULL, portNumber, 100) < 0)
    {
        errAdd(sclwsERR_BIND, portNumber);
        errCloseStack();
        sclwsExit(EXIT_FAILURE);
    }

    logTest("Listening on port '%d'.", portNumber);

    // Infinite loop to receive requests
    for (uint nbOfConnection = 1; ; nbOfConnection++)
    {
        // Wait (without timeout) a new connection
        globalSoapContext.accept_timeout = 1;
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
