#ifndef err_H
#define err_H
/*******************************************************************************
* JMMC project
*
* "@(#) $Id: err.h,v 1.1 2004-06-03 12:07:16 berezne Exp $"
*
* who       when		 what
* --------  -----------	 -------------------------------------------------------
* berezne   02-Jun-2004  created
*
*
*******************************************************************************/

/* The following piece of code alternates the linkage type to C for all 
functions declared within the braces, which is necessary to use the 
functions in C++-code.
*/

#ifdef __cplusplus
extern C {
#endif

/* insert here your declarations */
/*
Pourquoi limiter la tailles des char[]? 
On peut utiliser vsprintf mieux que ci-dessous, avec un malloc()
Ou vsnprintf()
void ascci_send_msg(const char *dest, char *format, ...)
{
    char string[ASCCIMESSAGELENGTH];
    va_list args;
    va_start(args, format);
    vsprintf(string, format, args);
    va_end(args);
    ascci_msg(dest, string);
}
*/

/*!
** \brief Sends an ASCCI_TAG_MESSAGE message to a destination task
**
** \param   const char * dest :   the final destination task
** \param   char * format     :   the printf like format of the var params
** \param   ... :                 the printf like list of parameters
**
** \return  void
** \attention The message type is ASCCI_TAG_MSG
*/

/*******************************************************************
Definitions JBz:
*/

#define ERR_API
#define MODULE_ID "ModuleJbz"
#define logMAX_LEN 64
#define logTEXT_LEN 64

typedef  char  mcsLOC_ID[64];

extern void    logPrintMsg(char * module_id, int loglevel, char *file , int line, char * functionName);
extern void    mcsGetLocalTimeStr( mcsBYTES64  timeStamp, unsigned char n);
extern char *  mcsGetFileName(char * file);
extern const char * mcsGetProcName();
extern void    logData (mcsMODULEID moduleId, mcsBYTES32 timeStamp, 
            mcsPROCNAME procName, char * log);


/************************************************************************
 * Definition of the data structures concerned by the error management
 *----------------------------------------------------------------------
 */

#include "mcs.h"

#define errSTACK_SIZE 20

typedef enum {
    errWARNING = 1,
    errSEVERE,
    errFATAL
} errSEVERITY;                         /* The error severity */

typedef struct 
{                    
  mcsBYTES32     timeStamp;            /* The date when the error occured      */
  mcsUINT8       sequenceNumber;       /* Number of the sequence in the stack  */
  
  mcsPROCNAME    procName;             /* The name of the process              */
  mcsLOC_ID      location;             /* The location where the error occured */
                                       /* File, line, etc...                   */
  mcsMODULEID    moduleId;             /* Name of the software module          */
  mcsINT32       errorId;              /* The error identifier                 */
  errSEVERITY    severity;             /* The error severity                   */
  mcsBYTES256    runTimePar;           /* Detailed information about the error */
} errSTACK_ELEM;                 

typedef struct 
{
  errSTACK_ELEM  stack[errSTACK_SIZE]; /* Error stack                   */
  mcsUINT8       stackSize;            /* Size of the error stack       */
  mcsLOGICAL     stackOverflow;        /* True if the stack overflows   */
  mcsLOGICAL     stackEmpty;           /* True if the stack is empty    */
} errERROR;                         

#ifdef __cplusplus
extern "C" {
#endif

/* Prototypes of the local functions */

#if 1    /* commente par JBz */
extern mcsCOMPL_STAT errAddInStack(errERROR          *error,
                            const char        *timeStamp,
                            const char        *procName,
                            const char        *moduleId,
                            const char        *location,
                            mcsINT32          errorId,
                            char              severity,
                                   char              *runTimePar);
    
#endif
    
/* Prototypes of the public functions */
extern ERR_API mcsCOMPL_STAT errClear           (errERROR *error);
extern ERR_API mcsCOMPL_STAT errAdd             (errERROR          *error, 
                                                 const mcsMODULEID moduleId,
                                                 mcsINT32          errorId,
                                                 const errSEVERITY severity,
                                                 const char        *file,
                                                 const int         line,
                                                 char              *format, 
                                                 ... );
extern ERR_API mcsLOGICAL    errIsInStack       (errERROR          *error,
                                                 const mcsMODULEID moduleId,
                                                 mcsINT32          errorId);
extern ERR_API mcsCOMPL_STAT errResetStack      (errERROR *error);
extern ERR_API mcsCOMPL_STAT errCloseStack      (errERROR *error);
extern ERR_API mcsCOMPL_STAT errDisplay         (errERROR *error);
extern ERR_API mcsINT8       errGetStackSize    (errERROR *error);
extern ERR_API mcsLOGICAL    errIsEmpty         (errERROR *error);

extern ERR_API mcsCOMPL_STAT errStoreExtrBuffer  (errERROR   *error,
                                                 char       *buffer,
                                                 mcsUINT32  bufLen,
                                                 mcsLOGICAL tabulate);
extern ERR_API mcsCOMPL_STAT errLoadExtrBuffer  (errERROR   *error,
                                                 char       *buffer,
                                                 mcsUINT32  bufLen);
        
extern ERR_API const char    *errGetSockErrorStr(int errNum);

#ifdef __cplusplus
}
#endif

#endif /*!err_H*/


/*___oOo___*/

