#ifndef sclwsWS_H
#define sclwsWS_H
/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: sclwsWS.h,v 1.1 2006-12-22 15:17:50 lafrasse Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 ******************************************************************************/

/**
 * @file
 * Declaration of sclwsWS SOAP functions.
 */

#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif


/*
 * SOAP definitions
 */
//gsoap ns service name: sclws
//gsoap ns service style: rpc
//gsoap ns service encoding: literal
//gsoap ns service location: http://localhost:8061
//gsoap ns schema namespace: urn:sclws


/*
 * sclwsGETSTAR Web Service.
 * @TODO implement
 */
/*
class sclwsGETSTAR_RESPONSE
{
    float magnitude;

    char* ra;
    char* dec;
};
int ns__GetStar(std::string, sclwsGETSTAR_RESPONSE&);
*/


/*
 * sclwsGETCAL Web Service.
 */
/* Give back an ID later used to follow a GetCal query progression. */
int ns__GetCalAsyncID(char**);

/* Launch a GetCal query. */
int ns__GetCalAsyncQuery(char*, char*, char**);

/**
 * Contains the progression status of a GetCal query.
 */
class sclwsGETCAL_TASK_STATUS
{
    char* currentCatalogName; /** current catalog name beeing queried */
    bool  lastCatalog;        /** true if last catalaog was reached */
};
int ns__GetCalStatus(char*, sclwsGETCAL_TASK_STATUS&);


#endif /*!sclwsWS_H*/

/*___oOo___*/
