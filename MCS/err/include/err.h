#ifndef err_H
#define err_H
/*******************************************************************************
* JMMC project
*
* "@(#) $Id: err.h,v 1.3 2004-06-21 17:09:47 gzins Exp $"
*
* who       when         what
* --------  -----------  -------------------------------------------------------
* berezne   02-Jun-2004  created
* gzins     16-Jun-2004  completed implementation
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
extern  mcsCOMPL_STAT errAddInStack      (errERROR          *error, 
                                          const mcsMODULEID moduleId,
                                          mcsINT32          errorId,
                                          const char        *fileLine,
                                          ... );
extern  mcsLOGICAL    errIsInStack       (errERROR          *error,
                                          const mcsMODULEID moduleId,
                                          mcsINT32          errorId);
extern  mcsCOMPL_STAT errResetStack      (errERROR *error);
extern  mcsCOMPL_STAT errCloseStack      (errERROR *error);
extern  mcsCOMPL_STAT errDisplay         (errERROR *error);
extern  mcsINT8       errGetStackSize    (errERROR *error);
extern  mcsLOGICAL    errIsEmpty         (errERROR *error);

extern  mcsCOMPL_STAT errStoreExtrBuffer (errERROR   *error,
                                          char       *buffer,
                                          mcsUINT32  bufLen,
                                          mcsLOGICAL tabulate);
extern  mcsCOMPL_STAT errLoadExtrBuffer  (errERROR   *error,
                                          char       *buffer,
                                          mcsUINT32  bufLen);
/* Convenience macro */
#define errAdd(error, errorId, arg...) \
    errAddInStack(error, MODULE_ID, errorId, __FILE_LINE__, ##arg)

#ifdef __cplusplus
}
#endif

#endif /*!err_H*/


/*___oOo___*/

