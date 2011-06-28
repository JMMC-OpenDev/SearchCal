#ifndef err_H
#define err_H
/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/* The following piece of code alternates the linkage type to C for all 
functions declared within the braces, which is necessary to use the 
functions in C++-code.
*/
#ifdef __cplusplus
extern "C" {
#endif

/************************************************************************
 * Definition of the data structures concerned by the error management
 *----------------------------------------------------------------------
 */

#include "mcs.h"

#define errSTACK_SIZE 20

typedef struct
{                    
    mcsBYTES32     timeStamp;         /* The date when the error occured      */
    mcsUINT8       sequenceNumber;    /* Number of the sequence in the stack  */

    mcsPROCNAME    procName;          /* The name of the process              */
    mcsFILE_LINE   location;          /* The location where the error occured */
                                      /* File, line, etc...                   */
    mcsMODULEID    moduleId;          /* Name of the software module          */
    mcsINT32       errorId;           /* The error identifier                 */
    char           severity;          /* The error severity                   */
    mcsLOGICAL     isErrUser;         /* TRUE if it is an error message
                                         intended to the end-user */ 
    mcsSTRING256   runTimePar;        /* Detailed information about the error */
} errERROR;

/* Max size of the error message */
#define errMSG_MAX_LEN 256

typedef struct
{
    /* The following pointer is used to know if the data structure is
     * initialized or not. When initialized, it contains pointer to itself */
    void *thisPtr;

    errERROR       stack[errSTACK_SIZE]; /* Error stack                    */
    mcsUINT8       stackSize;            /* Size of the error stack        */
    mcsLOGICAL     stackOverflow;        /* True if the stack overflows    */
    mcsLOGICAL     stackEmpty;           /* True if the stack is empty     */
} errERROR_STACK;

/* Prototypes of the public functions */
mcsCOMPL_STAT errAddInStack (const mcsMODULEID moduleId,
                             const char        *fileLine,
                             mcsINT32          errorId,
                             mcsLOGICAL        isErrUser,
                             ... );
char         *errUserGet    (void);
mcsLOGICAL    errIsInStack       (const mcsMODULEID moduleId,
                                  mcsINT32          errorId);
mcsCOMPL_STAT errResetStack      (void);
mcsCOMPL_STAT errCloseStack      (void);
mcsCOMPL_STAT errDisplayStack    (void);
mcsINT8       errGetStackSize    (void);
mcsLOGICAL    errStackIsEmpty    (void);

mcsCOMPL_STAT errPackStack    (char       *buffer,
                               mcsUINT32  bufLen);
mcsCOMPL_STAT errUnpackStack  (const char *buffer,
                               mcsUINT32  bufLen);

mcsCOMPL_STAT errInit(void);
mcsCOMPL_STAT errExit(void);


/* Convenience macro */
/**
 * Add an error message.
 *
 * Add an error message into the error stack, by calling the errAddInStack
 * function. This fonction places the error in global stack with all useful
 * information
 *
 * \param errorId error identifier
 * \param arg optional argument list associated to the error
 *
 * \sa errAddInStack
 */
#define errAdd(errorId, arg...) \
    errAddInStack(MODULE_ID, __FILE_LINE__, errorId, mcsFALSE, ##arg)

/**
 * Add an end-user oriented error message.
 *
 * Add an error message intended to the end-user; i.e. message which should be
 * understable by the end-user of the software. This error message should be
 * very simple and clear, and should not include technical information related
 * to software development such name of source file, function name or system
 * call which failed; such information should be already in error stack. The
 * user message is the message which will be displayed when error is reported to
 * user. The last end-user error message can be retreived using
 * errGetForEndUser() function.
 *
 * \param errorId error identifier
 * \param arg optional argument list associated to the error
 *
 * \sa errAddInStack, errUserGet
 */
#define errUserAdd(errorId, arg...) \
    errAddInStack(MODULE_ID, __FILE_LINE__, errorId, mcsTRUE, ##arg)

#ifdef __cplusplus
}
#endif

#endif /*!err_H*/

/*___oOo___*/

