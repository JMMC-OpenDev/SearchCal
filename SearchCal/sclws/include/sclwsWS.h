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
 * Note: For compatibility issue, paramater are given anonymous instead of named !
 */

/*
 * sclwsGETCAL Web Service.
 */
/* Get a session Id used by all the Web Service related functions */
int ns__GetCalOpenSession   (char**);

/* Query the server to get calibrator list */
int ns__GetCalSearchCal     (char*, char*, char**);

/* Get status of the query */
int ns__GetCalQueryStatus   (char*, char**);

/* Abort the given session */
int ns__GetCalCancelSession (char*, bool*);


/*
 * sclwsGETSTAR Web Service.
 */
/* Query the server to get star information */
int ns__GetStar             (char *query, char **output);


/*
 * sclwsGetServerStatus Web Service.
 */
/* Query the server to get star information */
int ns__GetServerStatus     (char** status);

#endif /*!sclwsWS_H*/

/*___oOo___*/
