/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: sclwsWS.cpp,v 1.2 2007-02-04 20:56:45 lafrasse Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.1  2006/12/22 15:17:50  lafrasse
 * Creation
 *
 ******************************************************************************/

/**
 * @file
 *  Definition of sclwsWS class.
 */

static char *rcsId __attribute__ ((unused)) = "@(#) $Id: sclwsWS.cpp,v 1.2 2007-02-04 20:56:45 lafrasse Exp $"; 

/* 
 * System Headers 
 */
#include <iostream>
using namespace std;
#include <map>
#include <uuid/uuid.h>

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
#include "sclwsPrivate.h"
#include "soapH.h"

/*
 * Local Variables 
 */
/**
 * Used to store each "Communication ID"-"sclsvSERVER instance" couples
 * to ensure multiple concurrent query handling.
 */
map<string,sclsvrSERVER*> tasks;

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
 * @param taskID give-back pointer to return the communication "session" ID.
 *
 * @return a SOAP error code.
 */
int ns__GetCalAsyncID(struct soap *p_soap, char** taskID)
{
    logTrace("ns__GetCalAsyncID()");

    // Create a "Universally Unique Identifier" (man uuid for more informations)
    uuid_t uuidID;
    uuid_generate(uuidID);

    // Allocate SOAP-aware memory to return the generated UUID
    *taskID = (char*) soap_malloc(p_soap, 37);
    uuid_unparse(uuidID, *taskID);
    logInfo("UUID = '%s'.", *taskID);

    // Create a new instance of sclsvrSERVER to perform the GETCAL query
    sclsvrSERVER* server = new sclsvrSERVER();

    // Associate the new sclsvrSERVER instance with the generated UUID for later
    tasks[*taskID] = server;

    return SOAP_OK;
}

/**
 * Launch a GetCal query.
 *
 * @param p_soap SOAP execution context.
 * @param taskID the communication "session" ID.
 * @param query the query to execute, in a double-quoted sclinsQuery format.
 * @param voTable give-back pointer to return the resulting VO Table.
 *
 * @return a SOAP error code.
 */
int ns__GetCalAsyncQuery(struct soap *p_soap,
                         char*  taskID,
                         char*  query,
                         char** voTable)
{
    logTrace("ns__GetCalAsyncQuery('%s')", taskID);

    // Retrieve the sclsvrSERVER instance associated with th received UUID
    sclsvrSERVER* server = tasks[taskID];

    // Launch the GETCAL query with the received paramters
    miscoDYN_BUF dynBuf;
    server->GetCal(query, dynBuf, NULL);

    // Allocate SOAP-aware memory to return the resulting VO Table
    uint resultSize;
    dynBuf.GetNbStoredBytes(&resultSize);
    resultSize++; // For the '\0'
    *voTable = (char*) soap_malloc(p_soap, resultSize);
    strncpy(*voTable, dynBuf.GetBuffer(), resultSize);

    // Deallocate & remove the sclsvrSERVER instance as the GETCAL query is over
    delete(server);
    tasks.erase(taskID);

    return SOAP_OK;
}

/**
 * Give back the name of the current catalog being queried.
 *
 * @param p_soap SOAP execution context.
 * @param taskID the communication "session" ID.
 * @param catalogName give-back pointer to return the current catalog name.
 *
 * @return a SOAP error code.
 */
int ns__GetCalWaitForCurrentCatalogName(struct soap *p_soap,
                                        char*  taskID,
                                        char** catalogName)
{
    logTrace("ns__GetCalWaitForCurrentCatalogName('%s')", taskID);

    // Retrieve the sclsvrSERVER instance associated with th received UUID
    sclsvrSERVER* server = tasks[taskID];

    // Allocate SOAP-aware memory to return the current catalog name
    *catalogName = (char*) soap_malloc(p_soap, 256);
    server->WaitForCurrentCatalogName(*catalogName);
    logInfo("CatalogName = '%s'.", *catalogName);

    return SOAP_OK;
}

/**
 * Give back whether the current catalog is the last one being queried.
 *
 * @param p_soap SOAP execution context.
 * @param taskID the communication "session" ID.
 * @param lastCatalog give-back pointer to return the last catalog flag.
 *
 * @return a SOAP error code.
 */
int ns__GetCalIsLastCatalog(struct soap *p_soap,
                            char*  taskID,
                            bool*  lastCatalog)
{
    logTrace("ns__GetCalIsLastCatalog('%s')", taskID);

    // Retrieve the sclsvrSERVER instance associated with th received UUID
    sclsvrSERVER* server = tasks[taskID];

    // Allocate SOAP-aware memory to return the current catalog name
    *lastCatalog = server->IsLastCatalog();
    logInfo("lastCatalog = '%d'.", *lastCatalog);

    return SOAP_OK;
}

/**
 * Give back the index of the current catalog being queried.
 *
 * @param p_soap SOAP execution context.
 * @param taskID the communication "session" ID.
 * @param catalogIndex give-back pointer to return the current catalog index.
 *
 * @return a SOAP error code.
 */
int ns__GetCalCurrentCatalogIndex(struct soap *p_soap,
                                  char*  taskID,
                                  int*   catalogIndex)
{
    logTrace("ns__GetCalCurrentCatalogIndex('%s')", taskID);

    // Retrieve the sclsvrSERVER instance associated with th received UUID
    sclsvrSERVER* server = tasks[taskID];

    // Allocate SOAP-aware memory to return the current catalog name
    *catalogIndex = server->GetCatalogIndex();
    logInfo("CatalogIndex = '%d'.", *catalogIndex);

    return SOAP_OK;
}

/**
 * Give back the total number of catalogs to be queried.
 *
 * @param p_soap SOAP execution context.
 * @param taskID the communication "session" ID.
 * @param nbOfCatalogs give-back pointer to return the number of catalogs.
 *
 * @return a SOAP error code.
 */
int ns__GetCalNbOfCatalogs(struct soap *p_soap,
                                  char*  taskID,
                                  int*   nbOfCatalogs)
{
    logTrace("ns__GetCalNbOfCatalogs('%s')", taskID);

    // Retrieve the sclsvrSERVER instance associated with th received UUID
    sclsvrSERVER* server = tasks[taskID];

    // Allocate SOAP-aware memory to return the current catalog name
    *nbOfCatalogs = server->GetNbOfCatalogs();
    logInfo("NbOfCatalogs = '%d'.", *nbOfCatalogs);

    return SOAP_OK;
}

/*___oOo___*/
