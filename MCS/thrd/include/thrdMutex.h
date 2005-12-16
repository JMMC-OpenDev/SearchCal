#ifndef thrdMUTEX_H
#define thrdMUTEX_H
/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: thrdMutex.h,v 1.1 2005-12-16 15:03:57 lafrasse Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 ******************************************************************************/

/**
 * @file
 * Declaration of thrdMutex functions.
 */


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


#endif /*!thrdMUTEX_H*/

/*___oOo___*/
