#ifndef err_H
#define err_H
/*******************************************************************************
* JMMC project
*
* "@(#) $Id: err.h,v 1.6 2004-12-14 13:12:38 lafrasse Exp $"
*
* who       when         what
* --------  -----------  -------------------------------------------------------
* berezne   02-Jun-2004  created
* gzins     16-Jun-2004  completed implementation
* lafrasse  14-Dec-2004  Added errMSG_MAX_LEN from errPrivate.H
*
*
*******************************************************************************/

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
                                      /* File, line, etc...                 */
    mcsMODULEID    moduleId;          /* Name of the software module          */
    mcsINT32       errorId;           /* The error identifier                 */
    char           severity;          /* The error severity                   */
    mcsBYTES256    runTimePar;        /* Detailed information about the error */
} errSTACK_ELEM;

/* Max size of the error message */
#define errMSG_MAX_LEN 256

typedef struct
{
    /* The following pointer is used to know if the data structure is
     * initialized or not. When initialized, it contains pointer to itself */
    void *thisPtr;

    errSTACK_ELEM  stack[errSTACK_SIZE]; /* Error stack                   */
    mcsUINT8       stackSize;            /* Size of the error stack       */
    mcsLOGICAL     stackOverflow;        /* True if the stack overflows   */
    mcsLOGICAL     stackEmpty;           /* True if the stack is empty    */
} errERROR;

/* Prototypes of the public functions */
mcsCOMPL_STAT errAddInStack (const mcsMODULEID moduleId,
                             const char        *fileLine,
                             mcsINT32          errorId,
                             ... );
mcsLOGICAL    errIsInStack       (const mcsMODULEID moduleId,
                                  mcsINT32          errorId);
mcsCOMPL_STAT errResetStack      (void);
mcsCOMPL_STAT errCloseStack      (void);
mcsCOMPL_STAT errDisplayStack    (void);
mcsINT8       errGetStackSize    (void);
mcsLOGICAL    errStackIsEmpty    (void);

mcsCOMPL_STAT errPackStack    (char       *buffer,
                               mcsUINT32  bufLen);
mcsCOMPL_STAT errUnpackStack  (char       *buffer,
                               mcsUINT32  bufLen);
/* Convenience macro */
#define errAdd(errorId, arg...) \
    errAddInStack(MODULE_ID, __FILE_LINE__, errorId, ##arg)

/* Global variable */
extern errERROR errGlobalStack;

#ifdef __cplusplus
}
#endif

#endif /*!err_H*/


/*___oOo___*/

