#ifndef thrdSEMAPHORE_H
#define thrdSEMAPHORE_H
/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * @file
 * Brief description of the header file, which ends at this dot.
 */


/* The following piece of code alternates the linkage type to C for all
functions declared within the braces, which is necessary to use the
functions in C++-code.
*/
#ifdef __cplusplus
extern "C" {
#endif

/*
 * Unions type definition
 */
union semun {
    int              val;    /* Value for SETVAL */
    struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
    unsigned short  *array;  /* Array for GETALL, SETALL */
    struct seminfo  *__buf;  /* Buffer for IPC_INFO
                                (Linux-specific) */
};
    
/*
 * system header files
 */
#include <sys/sem.h>

/*
 * MCS header files
 */
#include "mcs.h"


/*
 * Structure type definition
 */
typedef int thrdSEMAPHORE; /**< semaphore type. */


/*
 * Public functions declaration
 */
mcsCOMPL_STAT thrdSemaphoreInit    (      thrdSEMAPHORE  *semaphore,
                                    const mcsUINT32       value);
mcsCOMPL_STAT thrdSemaphoreDestroy (const thrdSEMAPHORE   semaphore);

mcsCOMPL_STAT thrdSemaphoreGetValue(const thrdSEMAPHORE   semaphore,
                                          mcsUINT32      *value);
mcsCOMPL_STAT thrdSemaphoreSetValue(const thrdSEMAPHORE   semaphore,
                                    const mcsUINT32       value);

mcsCOMPL_STAT thrdSemaphoreWait    (const thrdSEMAPHORE   semaphore);
mcsCOMPL_STAT thrdSemaphoreSignal  (const thrdSEMAPHORE   semaphore);


#ifdef __cplusplus
};
#endif


#endif /*!thrdSEMAPHORE_H*/

/*___oOo___*/
