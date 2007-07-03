#ifndef sclwsWS_H
#define sclwsWS_H
/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: sclwsWS.h,v 1.3 2007-07-03 17:00:03 lafrasse Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
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
//gsoap ns service location: http://jmmc.fr:8079
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

/* Give back the name of the current catalog beeing queried. */
int ns__GetCalWaitForCurrentCatalogName(char*, char**);

/* Give back the index of the current catalog being queried. */
int ns__GetCalCurrentCatalogIndex(char*, int*);

/* Give back the total number of catalogs to be queried. */
int ns__GetCalNbOfCatalogs(char*, int*);

/* Give back the last catalogs flag. */
int ns__GetCalIsLastCatalog(char*, bool*);

/* Abort the work done under the given ID. */
int ns__GetCalCancelID(char*, bool*);


#endif /*!sclwsWS_H*/

/*___oOo___*/
