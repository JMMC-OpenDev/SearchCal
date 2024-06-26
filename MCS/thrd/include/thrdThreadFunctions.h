#ifndef thrdTHREAD_STRUCT_H
#define thrdTHREAD_STRUCT_H
/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * @file
 * Declaration of thrdThread functions.
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
} thrdTHREAD_STRUCT;


/*
 * Public functions declaration
 */
mcsCOMPL_STAT thrdThreadCreate (thrdTHREAD_STRUCT  *thread);

mcsCOMPL_STAT thrdThreadWait   (thrdTHREAD_STRUCT  *thread);

mcsCOMPL_STAT thrdThreadKill   (thrdTHREAD_STRUCT  *thread);


#ifdef __cplusplus
};
#endif


#endif /*!thrdTHREAD_STRUCT_H*/

/*___oOo___*/
