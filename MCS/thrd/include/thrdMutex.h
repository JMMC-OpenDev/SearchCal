#ifndef thrdMUTEX_H
#define thrdMUTEX_H
/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: thrdMutex.h,v 1.2 2005-12-16 17:18:32 lafrasse Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.1  2005/12/16 15:03:57  lafrasse
 * Added mutex support
 *
 ******************************************************************************/

/**
 * @file
 * Declaration of thrdMutex functions.
 */


/* The following piece of code alternates the linkage type to C for all
functions declared within the braces, which is necessary to use the
functions in C++-code.
*/
#ifdef __cplusplus
extern "C" {
#endif


/*
 * System header
 */
#include <pthread.h>


/*
 * MCS header
 */
#include "mcs.h"


/*
 * Structure type definition
 */
typedef  pthread_mutex_t thrdMUTEX; /**< mutex type. */


/*
 * Public functions declaration
 */
mcsCOMPL_STAT thrdMutexInit     (thrdMUTEX *mutex);
mcsCOMPL_STAT thrdMutexDestroy  (thrdMUTEX *mutex);

mcsCOMPL_STAT thrdMutexLock     (thrdMUTEX *mutex);

mcsCOMPL_STAT thrdMutexUnlock   (thrdMUTEX *mutex);

#ifdef __cplusplus
};
#endif


#endif /*!thrdMUTEX_H*/

/*___oOo___*/
