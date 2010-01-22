#ifndef sclwsWS_H
#define sclwsWS_H
/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: sclwsWS.h,v 1.9 2010-01-22 15:41:29 lafrasse Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.8  2010/01/22 15:35:29  lafrasse
 * Log refinments.
 *
 * Revision 1.7  2007/11/12 10:32:15  lafrasse
 * Update Web Service API function name.
 * Update documentation and traces.
 * Added support for command line option parsing and handling.
 *
 * Revision 1.6  2007/10/31 11:46:56  gzins
 * Set web-service port to 8078
 *
 * Revision 1.5  2007/10/31 11:45:12  gzins
 * Updated web-service API
 *
 * Revision 1.4  2007/07/03 17:07:58  lafrasse
 * Corrected connection port number.
 *
 * Revision 1.3  2007/07/03 17:00:03  lafrasse
 * Added support for query cancellation.
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
//gsoap ns service location: http://jmmc.fr:8078
//gsoap ns schema namespace: urn:sclws

/*
 * sclwsGETCAL Web Service.
 */
/* Get a session Id used by all the Web Service related functions */
int ns__GetCalOpenSession(char**);

/* Query the server to get calibrator list */
int ns__GetCalSearchCal(char*, char*, char**);

/* Get status of the query */
int ns__GetCalQueryStatus(char*, char**);

/* Abort the given session */
int ns__GetCalCancelSession(char*, bool*);


#endif /*!sclwsWS_H*/

/*___oOo___*/
