#ifndef msgPrivate_H
#define msgPrivate_H
/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: msgPrivate.h,v 1.9 2005-02-14 07:59:01 gzins Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.8  2005/02/04 15:57:06  lafrasse
 * Massive documentation review an refinment (also added automatic CVS log inclusion in every files)
 *
 * Revision 1.7  2005/01/24 15:39:54  gzins
 * Added CVS logs as modification history
 *
 * gzins     15-Dec-2004  Added _NAME to command name definitions
 * gzins     09-Dec-2004  Removed msgMANAGER_PORT_NUMBER definition
 * gzins     08-Dec-2004  Replaced msgMCS_ENVS with envLIST
 * gzins     06-Dec-2004  Removed no longer used C functions
 * lafrasse  10-Aug-2004  Ported from CILAS software
 *
 ******************************************************************************/

/**
 * \file
 * Private 'msg' module header file, holding the MODULE_ID definition and
 * other constants.
 */

/* The following piece of code alternates the linkage type to C for all 
functions declared within the braces, which is necessary to use the 
functions in C++-code.
*/
#ifdef __cplusplus
extern "C" {
#endif


/* 
 * Constants definition
 */
#define MODULE_ID                       "msg"

/**
 * msgManger port number for connection
 */
#define msgMANAGER_SELECT_WIDTH          32
#define msgMANAGER_MAX_LENGTH_QUEUE      32

/**
 * Private command name
 */
#define msgREGISTER_CMD_NAME             "REGISTER"
#define msgCLOSE_CMD_NAME                "CLOSE"


/*
 * Private Globals
 */


#ifdef __cplusplus
}
#endif

#endif /*!msgPrivate_H*/


/*___oOo___*/
