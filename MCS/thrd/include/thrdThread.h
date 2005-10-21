#ifndef thrdTHREAD_H
#define thrdTHREAD_H
/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: thrdThread.h,v 1.1 2005-10-21 15:09:01 lafrasse Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 ******************************************************************************/

/**
 * @file
 * Declaration of thrdThread functions.
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
typedef void*        thrdFCT_ARG;                 /**< thread parameter type. */
typedef void*        thrdFCT_RET;                 /**< thread returned type.  */
typedef thrdFCT_RET(*thrdFCT_PTR)(thrdFCT_ARG);   /**< thread function type.  */

/**
 * A Thread structure.
 *
 * It holds all the informtations needed to manage a thread.
 */
typedef struct
{
    thrdFCT_PTR  function;      /**< a pointer on the C function the thread
                                     should execute. */

    thrdFCT_ARG  parameter;     /**< the thread parameter to be passed along its
                                     launch. */

    pthread_t    id;            /**< the thread system identifier, set after its
                                     launch. */

    thrdFCT_RET  result;        /**< the thread returned data, set after its
                                     end. */
} thrdTHREAD;



/*
 * Public functions declaration
 */
mcsCOMPL_STAT thrdThreadCreate (thrdTHREAD  *thread);

mcsCOMPL_STAT thrdThreadWait   (thrdTHREAD  *thread);



#endif /*!thrdTHREAD_H*/

/*___oOo___*/
