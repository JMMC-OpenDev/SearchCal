#ifndef sclwsWS_H
#define sclwsWS_H
/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
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
 * sclwsGetServerStatus Web Service.
 */
/* Query the server to get star information */
int ns__GetServerStatusSearchCal(char** status);

/*
 * sclwsGETCAL Web Service.
 */
/* Get a session Id used by all the Web Service related functions */
int ns__GetCalOpenSession   (char** jobId);

/* Query the server to get calibrator list */
int ns__GetCalSearchCal     (char* jobId, char* query, char** voTable);

/* Get status of the query */
int ns__GetCalQueryStatus   (char* jobId, char** status);

/* Abort the given session */
int ns__GetCalCancelSession (char* jobId, bool* isOK);

/*
 * sclwsGETSTAR Web Service.
 */
/* Query the server to get star information */
int ns__GetStarSearchCal    (char* query, char** votable);


#endif /*!sclwsWS_H*/

/*___oOo___*/
