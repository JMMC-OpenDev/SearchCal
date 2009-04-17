/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: sclwsWS.cpp,v 1.9 2009-04-17 15:38:44 lafrasse Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.8  2007/11/12 10:32:15  lafrasse
 * Update Web Service API function name.
 * Update documentation and traces.
 * Added support for command line option parsing and handling.
 *
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

static char *rcsId __attribute__ ((unused)) = "@(#) $Id: sclwsWS.cpp,v 1.9 2009-04-17 15:38:44 lafrasse Exp $"; 

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
 * @param soapContext SOAP execution context.
 * @param jobId give-back pointer to return the communication "session" ID.
 *
 * @return a SOAP error code.
 */
int ns__GetCalOpenSession(struct soap* soapContext, char** jobId)
{
    logTrace("ns__GetCalOpenSession()");

    // Test parameters validity
    if (soapContext == NULL)
    {
        errAdd(sclwsERR_NULL_PTR, "soapContext");
        sclwsReturnSoapError();
    }
    if (jobId == NULL)
    {
        errAdd(sclwsERR_NULL_PTR, "jobId");
        sclwsReturnSoapError();
    }

    // Compute connection IP and log it
    mcsSTRING16 conectionIP;
    // @WARNING : IP address computing code is probably endian-ness dependant !
    unsigned long ipAddress = soapContext->ip;
    long firstByte  = (long)((ipAddress >> 24) & 0xFF);
    long secondByte = (long)((ipAddress >> 16) & 0xFF);
    long thirdByte  = (long)((ipAddress >> 8)  & 0xFF);
    long forthByte  = (long)((ipAddress >> 0)  & 0xFF);
    snprintf(conectionIP, sizeof(conectionIP), "%ld.%ld.%ld.%ld", firstByte, secondByte, thirdByte, forthByte);
    logInfo("Accepted connection from IP address '%s'.", conectionIP);
    
    // Create a "Universally Unique Identifier" (man uuid for more informations)
    uuid_t uuidID;
    uuid_generate(uuidID);

    // Allocate SOAP-aware memory to return the generated UUID
    int jobIdLength = 37; // An UUID is 36-byte string (plus tailing '\0')
    *jobId = (char*) soap_malloc(soapContext, jobIdLength);
    if (*jobId == NULL)
    {
        errAdd(sclwsERR_ALLOC_MEM, jobIdLength);
        sclwsReturnSoapError();
    }

    // Get the string of the newly generated uuid_t structure
    uuid_unparse(uuidID, *jobId);
    logTest("\tSession '%s': uniq identifier generated.", *jobId);

    // Create a new instance of sclsvrSERVER to perform the GETCAL query
    sclsvrSERVER* server = new sclsvrSERVER();
    if (server == NULL)
    {
        errAdd(sclwsERR_SERVER_INSTANCIATION);
        sclwsReturnSoapError();
    }

    // Associate the new sclsvrSERVER instance with the generated UUID for later
    sclwsServerList[*jobId] = server;

    logDebug("\tSession '%s': server instanciated.", *jobId);
    
    return SOAP_OK;
}

/**
 * Launch a GetCal query.
 *
 * @param soapContext SOAP execution context.
 * @param jobId the communication "session" ID.
 * @param query the query to execute, in a double-quoted sclinsQuery format.
 * @param voTable give-back pointer to return the resulting VO Table.
 *
 * @return a SOAP error code.
 */
int ns__GetCalSearchCal(struct soap* soapContext,
                    char*  jobId,
                    char*  query,
                    char** voTable)
{
    logTrace("ns__GetCalSearchCal('%s')", jobId);

    // Store the thread ID iterator (used in case a Cancel occurs while this is running)
    sclwsTHREAD_ITERATOR threadIterator = 
        sclwsThreadList.insert(make_pair(jobId, pthread_self()));

    // Test parameters validity
    if (soapContext == NULL)
    {
        errAdd(sclwsERR_NULL_PTR, "soapContext");
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

    // Retrieve the sclsvrSERVER instance associated with the received UUID
    sclsvrSERVER* server = sclwsServerList[jobId];
    if (server == NULL)
    {
        errAdd(sclwsERR_WRONG_SERVER_ID, jobId);
        sclwsReturnSoapError();
    }

    logTest("\tSession '%s': launching query.", jobId);

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
        logDebug("\tSession '%s': resulting VOTable ('%d' bytes) =\n%s", jobId , resultSize, result);
    }
    else
    {
        logDebug("\tSession '%s': no star found.", jobId);
        result = "";
        resultSize = strlen(result);
    }
    resultSize++; // For the trailing '\0'
    *voTable = (char*) soap_malloc(soapContext, resultSize);
    strncpy(*voTable, result, resultSize);

    logTest("\tSession '%s': terminating query.", jobId);

    // Delete the thread ID
    sclwsThreadList.erase(threadIterator);

    return SOAP_OK;
}

/**
 * Give back the query execution state.
 *
 * @param soapContext SOAP execution context.
 * @param jobId the communication "session" ID.
 * @param status give-back pointer to return the current query status.
 *
 * @return a SOAP error code.
 */
int ns__GetCalQueryStatus(struct soap* soapContext,
                        char*  jobId,
                        char** status)
{
    logTrace("ns__GetCalQueryStatus('%s')", jobId);

    // Store the thread ID iterator (used in case a Cancel occurs while this is running)
    sclwsTHREAD_ITERATOR threadIterator = 
        sclwsThreadList.insert(make_pair(jobId, pthread_self()));

    // Test parameters validity
    if (soapContext == NULL)
    {
        errAdd(sclwsERR_NULL_PTR, "soapContext");
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
    sclsvrSERVER* server = sclwsServerList[jobId];
    if (server == NULL)
    {
        errAdd(sclwsERR_WRONG_SERVER_ID, jobId);
        sclwsReturnSoapError();
    }

    // Allocate SOAP-aware memory to return the current catalog name
    int statusLength = 256; // Should be enough !
    *status = (char*) soap_malloc(soapContext, statusLength);
    if (*status == NULL)
    {
        errAdd(sclwsERR_ALLOC_MEM, statusLength);
        sclwsReturnSoapError();
    }
    if (server->GetStatus(*status) == mcsFAILURE)
    {
        sclwsReturnSoapError();
    }

    logTest("\tSession '%s': query status = '%s'.", jobId, *status);

    // Delete the thread ID
    sclwsThreadList.erase(threadIterator);

    return SOAP_OK;
}

/**
 * Abort the work done under the given ID.
 *
 * @param soapContext SOAP execution context.
 * @param jobId pointer to communication "session" ID.
 * @param isOK give-back pointer to return whether the cancellation went OK or not.
 *
 * @return a SOAP error code.
 */
int ns__GetCalCancelSession(struct soap* soapContext,
                     char*  jobId,
                     bool*  isOK)
{
    logTrace("ns__GetCalCancelSession()");

    // Test parameters validity
    if (soapContext == NULL)
    {
        *isOK = false;
        errAdd(sclwsERR_NULL_PTR, "soapContext");
        sclwsReturnSoapError();
    }
    if (jobId == NULL)
    {
        *isOK = false;
        errAdd(sclwsERR_NULL_PTR, "jobId");
        sclwsReturnSoapError();
    }
    if (isOK == NULL)
    {
        *isOK = false;
        errAdd(sclwsERR_NULL_PTR, "isOK");
        sclwsReturnSoapError();
    }

    logTest("\tSession '%s': cancelling query.", jobId);

    // Retrieve the sclsvrSERVER instance associated with th received UUID
    sclsvrSERVER* server = sclwsServerList[jobId];
    if (server == NULL)
    {
        *isOK = false; // Cancellation went bad
        errAdd(sclwsERR_WRONG_SERVER_ID, jobId);
        logWarning("\tSession '%s': cancelling FAILED !", jobId);
        sclwsReturnSoapError();
    }

    // For each thread launched with the current job ID
    logDebug("\tSession '%s': killing all associated threads,", jobId);
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
    logDebug("\tSession '%s': deleting associated server.", jobId);
    delete(server);
    sclwsServerList.erase(jobId);

    *isOK = true; // Cancellation succesfully completed

    logTest("\tSession '%s': cancelling done.", jobId);

    return SOAP_OK;
}

/*___oOo___*/
