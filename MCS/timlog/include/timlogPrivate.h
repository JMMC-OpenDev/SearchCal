#ifndef timlogPrivate_H
#define timlogPrivate_H
/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: timlogPrivate.h,v 1.5 2005-02-15 10:30:55 gzins Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.4  2005/02/15 10:27:46  gzins
 * Added CVS log as file modification history
 *
 * sccmgr    17-Dec-2004  Created
 * gzins     20-Dec-2004  Added moduleName and fileLine fields to timlogENTRY
 *
 ******************************************************************************/

/**
 * \file
 * Private declaration for timer log module.
 */

/* The following piece of code alternates the linkage type to C for all 
functions declared within the braces, which is necessary to use the 
functions in C++-code.
*/
#ifdef __cplusplus
extern "C" {
#endif


/*
 * Module name
 */
#define MODULE_ID "timlog"

#include <sys/time.h>
 
/* Structure to hold one actionName<->startTime relation. */ 
typedef struct 
{
    mcsMODULEID    moduleName;
    mcsSTRING128   fileLine;
    mcsSTRING64    actionName;
    logLEVEL       level;
    struct timeval startTime;
} timlogENTRY;


#ifdef __cplusplus
}
#endif


#endif /*!timlogPrivate_H*/

/*___oOo___*/
