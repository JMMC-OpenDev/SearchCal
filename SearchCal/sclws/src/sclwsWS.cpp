/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: sclwsWS.cpp,v 1.7 2007-10-31 11:51:17 gzins Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
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

static char *rcsId __attribute__ ((unused)) = "@(#) $Id: sclwsWS.cpp,v 1.7 2007-10-31 11:51:17 gzins Exp $"; 

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
 * to ensure proper thread cancellation on GetCancel call.
 */
multimap<string,pthread_t> sclwsThreadList;
typedef multimap<string,pthread_t>::iterator sclwsTHREAD_IT;
typedef pair<sclwsTHREAD_IT, sclwsTHREAD_IT> sclwsTHREAD_RANGE;

/* Error message string to report SOAP service failure */
static char sclwsSoapErrMsg[256];

/*
 * Public methods
 */
/*
 * sclwsGETCAL Web Service
 */
/**
 * Get an ID used by all Web Service related functions
 *
 * The returned Id is used to perform asynchroneous treatment using the
 * synchroneous SOAP based web service.
 *
 * @param p_soap SOAP execution context.
 * @param jobId give-back pointer to return the communication "session" ID.
 *
 * @return a SOAP error code.
 */
int ns__GetCalGetId(struct soap *p_soap, char** jobId)
{
    logTrace("ns__GetCalAsyncID()");

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
    int jobIdLen = 37;
    *jobId = (char*) soap_malloc(p_soap, jobIdLen);
    if (*jobId == NULL)
    {
        errAdd(sclwsERR_ALLOC_MEM, jobIdLen);
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
int ns__GetCalQuery(struct soap *p_soap,
                    char*  jobId,
                    char*  query,
                    char** voTable)
{
    logTrace("ns__GetCalAsyncQuery('%s')", jobId);

    // Store the thread ID iterator
    sclwsTHREAD_IT mmit = 
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
    resultSize++; // For the '\0'
    *voTable = (char*) soap_malloc(p_soap, resultSize);
    strncpy(*voTable, result, resultSize);

    logDebug("Terminating query.");

    // Delete the thread ID
    sclwsThreadList.erase(mmit);

    return SOAP_OK;
}

/**
 * Give back the name of the current catalog being queried.
 *
 * @param p_soap SOAP execution context.
 * @param jobId the communication "session" ID.
 * @param catalogName give-back pointer to return the current catalog name.
 *
 * @return a SOAP error code.
 */
int ns__GetCalGetStatus(struct soap *p_soap,
                        char*  jobId,
                        char** status)
{
    logTrace("ns__GetCalGetStatus('%s')", jobId);

    // Store the thread ID iterator
    sclwsTHREAD_IT mmit = 
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
    int statusLen = 256;
    *status = (char*) soap_malloc(p_soap, statusLen);
    if (*status == NULL)
    {
        errAdd(sclwsERR_ALLOC_MEM, statusLen);
        sclwsReturnSoapError();
    }
    if (server->GetStatus(*status) == mcsFAILURE)
    {
        sclwsReturnSoapError();
    }
    logInfo("Status: '%s'.", *status);

    // Delete the thread ID
    sclwsThreadList.erase(mmit);

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
int ns__GetCalCancel(struct soap *p_soap,
                     char*  jobId,
                     bool*  isOK)
{
    logTrace("ns__GetCalCancel()");


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
        *isOK = false;
        sclwsReturnSoapError();
    }

    // For each thread launched with the current job ID
    logInfo("Killing all job threads");
    sclwsTHREAD_RANGE range = sclwsThreadList.equal_range(jobId);
    for (sclwsTHREAD_IT mmit = range.first; mmit != range.second; ++mmit)
    {
        // Kill the thread
        pthread_cancel(mmit->second);

        // Delete the thread ID
        sclwsThreadList.erase(mmit);
    }

    // Delete the server instance
    logInfo("Deleting server");
    delete(server);
    sclwsServerList.erase(jobId);

    // Allocate SOAP-aware memory to return the current catalog name
    *isOK = true;

    return SOAP_OK;
}

/*___oOo___*/
