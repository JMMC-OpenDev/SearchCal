/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: sclwsWS.cpp,v 1.8 2007-11-12 10:32:15 lafrasse Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.7  2007/10/31 11:51:17  gzins
 * Updated according to new web-service API
 * Improved error handling
 *
 * Revision 1.6  2007/07/03 17:00:03  lafrasse
 * Added support for query cancellation.
 *
 * Revision 1.5  2007/06/27 21:19:40  gzins
 * Implemented work-around related to 'semop - EIDRM' -> 'Identifier removed' bug
 *
 * Revision 1.4  2007/02/13 16:18:34  lafrasse
 * *** empty log message ***
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
 * @file
 *  Definition of sclwsWS class.
 */

static char *rcsId __attribute__ ((unused)) = "@(#) $Id: sclwsWS.cpp,v 1.8 2007-11-12 10:32:15 lafrasse Exp $"; 

/* 
 * System Headers 
 */
#include <iostream>
using namespace std;
#include <map>
#include <uuid/uuid.h>
#include <pthread.h>

/*
 * MCS Headers 
 */
#include "mcs.h"
#include "log.h"
#include "err.h"
#include "sclsvrSERVER.h"
#include "miscoDYN_BUF.h"

/*
 * Local Headers 
 */
#include "sclwsErrors.h"
#include "sclwsPrivate.h"
#include "soapH.h"

/*
 * Local Variables 
 */
/**
 * Used to store each "Communication ID"-"sclsvSERVER instance" couples
 * to ensure multiple concurrent query handling.
 */
map<string,sclsvrSERVER*> sclwsServerList;
/**
 * Used to store each "Communication ID"-"thread ID" couples
 * to ensure proper thread cancellation on ns__GetCalCancel() call.
 */
multimap<string,pthread_t> sclwsThreadList;
typedef multimap<string,pthread_t>::iterator sclwsTHREAD_ITERATOR;
typedef pair<sclwsTHREAD_ITERATOR, sclwsTHREAD_ITERATOR> sclwsTHREAD_RANGE;

/**
 * Error message string to report SOAP service failure.
 *
 * @sa Used by the sclwsReturnSoapError() macro defined in sclwsPrivate.h
 */
static char sclwsSoapErrMsg[256];

/*
 * Public methods
 */
/*
 * sclwsGETCAL Web Service
 */
/**
 * Get a unique session Id used by all this Web Service related functions
 *
 * The returned Id is used to perform asynchroneous treatment using the
 * synchroneous SOAP based web service.
 *
 * @param p_soap SOAP execution context.
 * @param jobId give-back pointer to return the communication "session" ID.
 *
 * @return a SOAP error code.
 */
int ns__GetCalOpenSession(struct soap *p_soap, char** jobId)
{
    logTrace("ns__GetCalOpenSession()");

    // Test parameters validity
    if (p_soap == NULL)
    {
        errAdd(sclwsERR_NULL_PTR, "p_soap");
        sclwsReturnSoapError();
    }
    if (jobId == NULL)
    {
        errAdd(sclwsERR_NULL_PTR, "jobId");
        sclwsReturnSoapError();
    }

    // Create a "Universally Unique Identifier" (man uuid for more informations)
    uuid_t uuidID;
    uuid_generate(uuidID);

    // Allocate SOAP-aware memory to return the generated UUID
    int jobIdLength = 37; // An UUID is 36-byte string (plus tailing '\0')
    *jobId = (char*) soap_malloc(p_soap, jobIdLength);
    if (*jobId == NULL)
    {
        errAdd(sclwsERR_ALLOC_MEM, jobIdLength);
        sclwsReturnSoapError();
    }

    // Get the string of the newly generated uuid_t structure
    uuid_unparse(uuidID, *jobId);
    logInfo("UUID = '%s'.", *jobId);

    // Create a new instance of sclsvrSERVER to perform the GETCAL query
    sclsvrSERVER* server = new sclsvrSERVER();
    if (server == NULL)
    {
        errAdd(sclwsERR_SERVER_INSTANCIATION);
        sclwsReturnSoapError();
    }

    // Associate the new sclsvrSERVER instance with the generated UUID for later
    sclwsServerList[*jobId] = server;

    return SOAP_OK;
}

/**
 * Launch a GetCal query.
 *
 * @param p_soap SOAP execution context.
 * @param jobId the communication "session" ID.
 * @param query the query to execute, in a double-quoted sclinsQuery format.
 * @param voTable give-back pointer to return the resulting VO Table.
 *
 * @return a SOAP error code.
 */
int ns__GetCalSearchCal(struct soap *p_soap,
                    char*  jobId,
                    char*  query,
                    char** voTable)
{
    logTrace("ns__GetCalSearchCal('%s')", jobId);

    // Store the thread ID iterator (used in case a Cancel occurs while this is running)
    sclwsTHREAD_ITERATOR threadIterator = 
        sclwsThreadList.insert(make_pair(jobId, pthread_self()));

    // Test parameters validity
    if (p_soap == NULL)
    {
        errAdd(sclwsERR_NULL_PTR, "p_soap");
        sclwsReturnSoapError();
    }
    if (jobId == NULL)
    {
        errAdd(sclwsERR_NULL_PTR, "jobId");
        sclwsReturnSoapError();
    }
    if (query == NULL)
    {
        errAdd(sclwsERR_NULL_PTR, "query");
        sclwsReturnSoapError();
    }
    if (voTable == NULL)
    {
        errAdd(sclwsERR_NULL_PTR, "voTable");
        sclwsReturnSoapError();
    }

    // Retrieve the sclsvrSERVER instance associated with th received UUID
    sclsvrSERVER* server = NULL;
    server = sclwsServerList[jobId];
    if (server == NULL)
    {
        errAdd(sclwsERR_WRONG_SERVER_ID, jobId);
        sclwsReturnSoapError();
    }

    // Launch the GETCAL query with the received paramters
    miscoDYN_BUF dynBuf;
    if (server->GetCal(query, dynBuf) == mcsFAILURE)
    {
        sclwsReturnSoapError();
    }

    // Allocate SOAP-aware memory to return the resulting VO Table
    char* result = NULL;
    uint resultSize = 0;
    dynBuf.GetNbStoredBytes(&resultSize);
    if (resultSize != 0)
    {
        result = dynBuf.GetBuffer();
    }
    else
    {
        logDebug("No star found.");
        result = "";
        resultSize = strlen(result);
    }
    resultSize++; // For the tailing '\0'
    *voTable = (char*) soap_malloc(p_soap, resultSize);
    strncpy(*voTable, result, resultSize);

    logDebug("Terminating query.");

    // Delete the thread ID
    sclwsThreadList.erase(threadIterator);

    return SOAP_OK;
}

/**
 * Give back the query execution state.
 *
 * @param p_soap SOAP execution context.
 * @param jobId the communication "session" ID.
 * @param status give-back pointer to return the current query status.
 *
 * @return a SOAP error code.
 */
int ns__GetCalQueryStatus(struct soap *p_soap,
                        char*  jobId,
                        char** status)
{
    logTrace("ns__GetCalQueryStatus('%s')", jobId);

    // Store the thread ID iterator (used in case a Cancel occurs while this is running)
    sclwsTHREAD_ITERATOR threadIterator = 
        sclwsThreadList.insert(make_pair(jobId, pthread_self()));

    // Test parameters validity
    if (p_soap == NULL)
    {
        errAdd(sclwsERR_NULL_PTR, "p_soap");
        sclwsReturnSoapError();
    }
    if (jobId == NULL)
    {
        errAdd(sclwsERR_NULL_PTR, "jobId");
        sclwsReturnSoapError();
    }
    if (status == NULL)
    {
        errAdd(sclwsERR_NULL_PTR, "status");
        sclwsReturnSoapError();
    }

    // Retrieve the sclsvrSERVER instance associated with th received UUID
    sclsvrSERVER* server = NULL;
    server = sclwsServerList[jobId];
    if (server == NULL)
    {
        errAdd(sclwsERR_WRONG_SERVER_ID, jobId);
        sclwsReturnSoapError();
    }

    // Allocate SOAP-aware memory to return the current catalog name
    int statusLength = 256;
    *status = (char*) soap_malloc(p_soap, statusLength);
    if (*status == NULL)
    {
        errAdd(sclwsERR_ALLOC_MEM, statusLength);
        sclwsReturnSoapError();
    }
    if (server->GetStatus(*status) == mcsFAILURE)
    {
        sclwsReturnSoapError();
    }
    logInfo("Status: '%s'.", *status);

    // Delete the thread ID
    sclwsThreadList.erase(threadIterator);

    return SOAP_OK;
}

/**
 * Abort the work done under the given ID.
 *
 * @param p_soap SOAP execution context.
 * @param jobId pointer to communication "session" ID.
 * @param isOK give-back pointer to return whether the cancellation went OK or not.
 *
 * @return a SOAP error code.
 */
int ns__GetCalCancelSession(struct soap *p_soap,
                     char*  jobId,
                     bool*  isOK)
{
    logTrace("ns__GetCalCancelSession()");

    // Test parameters validity
    if (p_soap == NULL)
    {
        errAdd(sclwsERR_NULL_PTR, "p_soap");
        *isOK = false;
        sclwsReturnSoapError();
    }
    if (jobId == NULL)
    {
        errAdd(sclwsERR_NULL_PTR, "jobId");
        *isOK = false;
        sclwsReturnSoapError();
    }
    if (isOK == NULL)
    {
        errAdd(sclwsERR_NULL_PTR, "isOK");
        *isOK = false;
        sclwsReturnSoapError();
    }

    // Retrieve the sclsvrSERVER instance associated with th received UUID
    sclsvrSERVER* server = NULL;
    server = sclwsServerList[jobId];
    if (server == NULL)
    {
        errAdd(sclwsERR_WRONG_SERVER_ID, jobId);
        *isOK = false; // Cancellation went bad
        sclwsReturnSoapError();
    }

    // For each thread launched with the current job ID
    logInfo("Killing all job threads");
    sclwsTHREAD_RANGE range = sclwsThreadList.equal_range(jobId);
    sclwsTHREAD_ITERATOR threadIterator;
    for (threadIterator = range.first; threadIterator != range.second; ++threadIterator)
    {
        // Kill the thread
        pthread_cancel(threadIterator->second);

        // Delete the thread ID
        sclwsThreadList.erase(threadIterator);
    }

    // Delete the server instance
    logInfo("Deleting server");
    delete(server);
    sclwsServerList.erase(jobId);

    *isOK = true; // Cancellation went OK

    return SOAP_OK;
}

/*___oOo___*/
