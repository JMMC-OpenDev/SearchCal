/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: sclwsWS.cpp,v 1.6 2007-07-03 17:00:03 lafrasse Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
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

static char *rcsId __attribute__ ((unused)) = "@(#) $Id: sclwsWS.cpp,v 1.6 2007-07-03 17:00:03 lafrasse Exp $"; 

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
map<string,sclsvrSERVER*> servers;
/**
 * Used to store each "Communication ID"-"thread ID" couples
 * to ensure proper thread cancellation on GetCancel call.
 */
multimap<string,pthread_t> threads;
typedef multimap<string,pthread_t>::iterator threadsIT;
typedef pair<threadsIT, threadsIT> threadsRange;


/*
 * Public methods
 */
/*
int ns__GetStar(
    struct soap *p_soap,            // contexte d'execution du service web
    std::string  starName,		    // requete
    sclwsGETSTAR_RESPONSE &response // pour retourner plusieurs valeurs
)
{
    printf("ns__GetStar('%s')\n", starName.c_str());

    response.magnitude = 1.23456789;

    response.ra = (char*) soap_malloc(p_soap, 100);
    sprintf(response.ra, "RA(%s)", starName.c_str());

    response.dec = (char*) soap_malloc(p_soap, 100);
    sprintf(response.dec, "DEC(%s)", starName.c_str());

    return SOAP_OK;
}
*/

/*
 * sclwsGETCAL Web Service
 */
/**
 * Give back an ID later used to follow a GetCal query progression.
 *
 * This is a trick to simulate asynchroneous communication with a synchrone SOAP
 * based web service.
 *
 * @param p_soap SOAP execution context.
 * @param jobID give-back pointer to return the communication "session" ID.
 *
 * @return a SOAP error code.
 */
int ns__GetCalAsyncID(struct soap *p_soap, char** jobID)
{
    logTrace("ns__GetCalAsyncID()");

    // Test parameters validity
    if (p_soap == NULL)
    {
        errAdd(sclwsERR_NULL_PTR, "p_soap");
        errCloseStack();
        return SOAP_ERR;
    }
    if (jobID == NULL)
    {
        errAdd(sclwsERR_NULL_PTR, "jobID");
        errCloseStack();
        return SOAP_ERR;
    }

    // Create a "Universally Unique Identifier" (man uuid for more informations)
    uuid_t uuidID;
    uuid_generate(uuidID);

    // Allocate SOAP-aware memory to return the generated UUID
    *jobID = (char*) soap_malloc(p_soap, 37);
    if (*jobID == NULL)
    {
        errAdd(sclwsERR_NULL_PTR, "*jobID");
        errCloseStack();
        return SOAP_ERR;
    }

    // Get the string of the newly generated uuid_t structure
    uuid_unparse(uuidID, *jobID);
    logInfo("UUID = '%s'.", *jobID);

    // Create a new instance of sclsvrSERVER to perform the GETCAL query
    sclsvrSERVER* server = new sclsvrSERVER();
    if (server == NULL)
    {
        errAdd(sclwsERR_NULL_PTR, "server");
        errCloseStack();
        return SOAP_ERR;
    }

    // Associate the new sclsvrSERVER instance with the generated UUID for later
    servers[*jobID] = server;

    return SOAP_OK;
}

/**
 * Launch a GetCal query.
 *
 * @param p_soap SOAP execution context.
 * @param jobID the communication "session" ID.
 * @param query the query to execute, in a double-quoted sclinsQuery format.
 * @param voTable give-back pointer to return the resulting VO Table.
 *
 * @return a SOAP error code.
 */
int ns__GetCalAsyncQuery(struct soap *p_soap,
                         char*  jobID,
                         char*  query,
                         char** voTable)
{
    logTrace("ns__GetCalAsyncQuery('%s')", jobID);

    // Store the thread ID iterator
    threadsIT mmit = threads.insert(make_pair(jobID, pthread_self()));

    // Test parameters validity
    if (p_soap == NULL)
    {
        errAdd(sclwsERR_NULL_PTR, "p_soap");
        errCloseStack();
        return SOAP_ERR;
    }
    if (jobID == NULL)
    {
        errAdd(sclwsERR_NULL_PTR, "jobID");
        errCloseStack();
        return SOAP_ERR;
    }
    if (query == NULL)
    {
        errAdd(sclwsERR_NULL_PTR, "query");
        errCloseStack();
        return SOAP_ERR;
    }
    if (voTable == NULL)
    {
        errAdd(sclwsERR_NULL_PTR, "voTable");
        errCloseStack();
        return SOAP_ERR;
    }

    // Retrieve the sclsvrSERVER instance associated with th received UUID
    sclsvrSERVER* server = NULL;
    server = servers[jobID];
    if (server == NULL)
    {
        errAdd(sclwsERR_NULL_PTR, "server");
        errCloseStack();
        return SOAP_ERR;
    }

    // Launch the GETCAL query with the received paramters
    miscoDYN_BUF dynBuf;
    if (server->GetCal(query, dynBuf, NULL) == mcsFAILURE)
    {
        errAdd(sclwsERR_GETCAL);
        errCloseStack();
        return SOAP_ERR;
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
        logDebug("No stars found.");
        result = "";
        resultSize = strlen(result);
    }
    resultSize++; // For the '\0'
    *voTable = (char*) soap_malloc(p_soap, resultSize);
    strncpy(*voTable, result, resultSize);

    logDebug("Terminating query.");

    // Delete the thread ID
    threads.erase(mmit);

    return SOAP_OK;
}

/**
 * Give back the name of the current catalog being queried.
 *
 * @param p_soap SOAP execution context.
 * @param jobID the communication "session" ID.
 * @param catalogName give-back pointer to return the current catalog name.
 *
 * @return a SOAP error code.
 */
int ns__GetCalWaitForCurrentCatalogName(struct soap *p_soap,
                                        char*  jobID,
                                        char** catalogName)
{
    logTrace("ns__GetCalWaitForCurrentCatalogName('%s')", jobID);

    // Store the thread ID iterator
    threadsIT mmit = threads.insert(make_pair(jobID, pthread_self()));

    // Test parameters validity
    if (p_soap == NULL)
    {
        errAdd(sclwsERR_NULL_PTR, "p_soap");
        errCloseStack();
        return SOAP_ERR;
    }
    if (jobID == NULL)
    {
        errAdd(sclwsERR_NULL_PTR, "jobID");
        errCloseStack();
        return SOAP_ERR;
    }
    if (catalogName == NULL)
    {
        errAdd(sclwsERR_NULL_PTR, "catalogName");
        errCloseStack();
        return SOAP_ERR;
    }

    // Retrieve the sclsvrSERVER instance associated with th received UUID
    sclsvrSERVER* server = NULL;
    server = servers[jobID];
    if (server == NULL)
    {
        errAdd(sclwsERR_NULL_PTR, "server");
        errCloseStack();
        return SOAP_ERR;
    }

    // Allocate SOAP-aware memory to return the current catalog name
    *catalogName = (char*) soap_malloc(p_soap, 256);
    if (*catalogName == NULL)
    {
        errAdd(sclwsERR_NULL_PTR, "*catalogName");
        errCloseStack();
        return SOAP_ERR;
    }
    if (server->WaitForCurrentCatalogName(*catalogName) == mcsFAILURE)
    {
#if 0
        errAdd(sclwsERR_CATALOG_NAME);
        errCloseStack();
        return SOAP_ERR;
#else
	strcpy(*catalogName, "Unknown");
#endif
    }
    logInfo("CatalogName = '%s'.", *catalogName);

    // Delete the thread ID
    threads.erase(mmit);

    return SOAP_OK;
}

/**
 * Give back whether the current catalog is the last one being queried.
 *
 * @param p_soap SOAP execution context.
 * @param jobID the communication "session" ID.
 * @param lastCatalog give-back pointer to return the last catalog flag.
 *
 * @return a SOAP error code.
 */
int ns__GetCalIsLastCatalog(struct soap *p_soap,
                            char*  jobID,
                            bool*  lastCatalog)
{
    logTrace("ns__GetCalIsLastCatalog('%s')", jobID);

    // Store the thread ID iterator
    threadsIT mmit = threads.insert(make_pair(jobID, pthread_self()));

    // Test parameters validity
    if (p_soap == NULL)
    {
        errAdd(sclwsERR_NULL_PTR, "p_soap");
        errCloseStack();
        return SOAP_ERR;
    }
    if (jobID == NULL)
    {
        errAdd(sclwsERR_NULL_PTR, "jobID");
        errCloseStack();
        return SOAP_ERR;
    }
    if (lastCatalog == NULL)
    {
        errAdd(sclwsERR_NULL_PTR, "lastCatalog");
        errCloseStack();
        return SOAP_ERR;
    }

    // Retrieve the sclsvrSERVER instance associated with th received UUID
    sclsvrSERVER* server = NULL;
    server = servers[jobID];
    if (server == NULL)
    {
        errAdd(sclwsERR_NULL_PTR, "server");
        errCloseStack();
        return SOAP_ERR;
    }

    // Allocate SOAP-aware memory to return the current catalog name
    *lastCatalog = server->IsLastCatalog();
    logInfo("lastCatalog = '%d'.", *lastCatalog);

    // Delete the thread ID
    threads.erase(mmit);

    return SOAP_OK;
}

/**
 * Give back the index of the current catalog being queried.
 *
 * @param p_soap SOAP execution context.
 * @param jobID the communication "session" ID.
 * @param catalogIndex give-back pointer to return the current catalog index.
 *
 * @return a SOAP error code.
 */
int ns__GetCalCurrentCatalogIndex(struct soap *p_soap,
                                  char*  jobID,
                                  int*   catalogIndex)
{
    logTrace("ns__GetCalCurrentCatalogIndex('%s')", jobID);

    // Store the thread ID iterator
    threadsIT mmit = threads.insert(make_pair(jobID, pthread_self()));

    // Test parameters validity
    if (p_soap == NULL)
    {
        errAdd(sclwsERR_NULL_PTR, "p_soap");
        errCloseStack();
        return SOAP_ERR;
    }
    if (jobID == NULL)
    {
        errAdd(sclwsERR_NULL_PTR, "jobID");
        errCloseStack();
        return SOAP_ERR;
    }
    if (catalogIndex == NULL)
    {
        errAdd(sclwsERR_NULL_PTR, "catalogIndex");
        errCloseStack();
        return SOAP_ERR;
    }

    // Retrieve the sclsvrSERVER instance associated with th received UUID
    sclsvrSERVER* server = NULL;
    server = servers[jobID];
    if (server == NULL)
    {
        errAdd(sclwsERR_NULL_PTR, "server");
        errCloseStack();
        return SOAP_ERR;
    }

    // Allocate SOAP-aware memory to return the current catalog name
    *catalogIndex = server->GetCatalogIndex();
    logInfo("CatalogIndex = '%d'.", *catalogIndex);

    // Delete the thread ID
    threads.erase(mmit);

    return SOAP_OK;
}

/**
 * Give back the total number of catalogs to be queried.
 *
 * @param p_soap SOAP execution context.
 * @param jobID the communication "session" ID.
 * @param nbOfCatalogs give-back pointer to return the number of catalogs.
 *
 * @return a SOAP error code.
 */
int ns__GetCalNbOfCatalogs(struct soap *p_soap,
                                  char*  jobID,
                                  int*   nbOfCatalogs)
{
    logTrace("ns__GetCalNbOfCatalogs('%s')", jobID);

    // Store the thread ID iterator
    threadsIT mmit = threads.insert(make_pair(jobID, pthread_self()));

    // Test parameters validity
    if (p_soap == NULL)
    {
        errAdd(sclwsERR_NULL_PTR, "p_soap");
        errCloseStack();
        return SOAP_ERR;
    }
    if (jobID == NULL)
    {
        errAdd(sclwsERR_NULL_PTR, "jobID");
        errCloseStack();
        return SOAP_ERR;
    }
    if (nbOfCatalogs == NULL)
    {
        errAdd(sclwsERR_NULL_PTR, "nbOfCatalogs");
        errCloseStack();
        return SOAP_ERR;
    }

    // Retrieve the sclsvrSERVER instance associated with th received UUID
    sclsvrSERVER* server = NULL;
    server = servers[jobID];
    if (server == NULL)
    {
        errAdd(sclwsERR_NULL_PTR, "server");
        errCloseStack();
        return SOAP_ERR;
    }

    // Allocate SOAP-aware memory to return the current catalog name
    *nbOfCatalogs = server->GetNbOfCatalogs();
    logInfo("NbOfCatalogs = '%d'.", *nbOfCatalogs);

    // Delete the thread ID
    threads.erase(mmit);

    return SOAP_OK;
}

/**
 * Abort the work done under the given ID.
 *
 * @param p_soap SOAP execution context.
 * @param jobID pointer to communication "session" ID.
 * @param isOK give-back pointer to return whether the cancellation went OK or not.
 *
 * @return a SOAP error code.
 */
int ns__GetCalCancelID(struct soap *p_soap,
                              char*  jobID,
                              bool*  isOK)
{
    logTrace("ns__GetCalCancelID()");

    // Test parameters validity
    if (p_soap == NULL)
    {
        errAdd(sclwsERR_NULL_PTR, "p_soap");
        errCloseStack();
        return SOAP_ERR;
    }
    if (jobID == NULL)
    {
        errAdd(sclwsERR_NULL_PTR, "jobID");
        errCloseStack();
        return SOAP_ERR;
    }
    if (isOK == NULL)
    {
        errAdd(sclwsERR_NULL_PTR, "isOK");
        errCloseStack();
        return SOAP_ERR;
    }

    // Retrieve the sclsvrSERVER instance associated with th received UUID
    sclsvrSERVER* server = NULL;
    server = servers[jobID];
    if (server == NULL)
    {
        errAdd(sclwsERR_NULL_PTR, "server");
        errCloseStack();
        return SOAP_ERR;
    }

    // For each thread launched with the current job ID
    logInfo("Killing all job threads");
    threadsRange range = threads.equal_range(jobID);
    for(threadsIT mmit = range.first; mmit != range.second; ++mmit)
    {
        // Kill the thread
        pthread_cancel(mmit->second);

        // Delete the thread ID
        threads.erase(mmit);
    }

    // Delete the server instance
    logInfo("Deleting server");
    delete(server);
    servers.erase(jobID);

    // Allocate SOAP-aware memory to return the current catalog name
    *isOK = true;

    return SOAP_OK;
}

/*___oOo___*/
