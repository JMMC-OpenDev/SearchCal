#ifndef errPrivate_H
#define errPrivate_H
/*******************************************************************************
* JMMC project
*
* "@(#) $Id: errPrivate.h,v 1.2 2004-06-23 13:04:48 gzins Exp $"
*
* who       when		 what
* --------  -----------	 -------------------------------------------------------
* gzins     16-Jun-2004  created
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

#include <stdarg.h>
/* Module name */
#define MODULE_ID "err"

/* Max size of the error message */
#define errMSG_MAX_LEN 128

/* Local functions */
extern  mcsCOMPL_STAT errResetLocalStack (errERROR *error);
extern  mcsCOMPL_STAT errCloseLocalStack (errERROR *error);
extern  mcsCOMPL_STAT errDisplayLocalStack (errERROR *error);
extern  mcsLOGICAL    errIsInLocalStack  (errERROR          *error,
                                          const mcsMODULEID moduleId,
                                          mcsINT32          errorId);
extern  mcsLOGICAL    errLocalStackIsEmpty (errERROR          *error);
extern  mcsINT8       errGetLocalStackSize (errERROR *error);
extern  mcsCOMPL_STAT errPackLocalStack (errERROR   *error,
                                         char       *buffer,
                                         mcsUINT32  bufLen);
extern  mcsCOMPL_STAT errUnpackLocalStack (errERROR   *error,
                                           char       *buffer,
                                           mcsUINT32  bufLen);
extern mcsCOMPL_STAT errPushInLocalStack(errERROR   *error,
                                         const char *timeStamp,
                                         const char *procName,
                                         const char *moduleId,
                                         const char *location,
                                         mcsINT32   errorId,
                                         char       severity,
                                         char       *runTimePar);
extern  mcsCOMPL_STAT errAddInLocalStack (errERROR          *error, 
                                          const mcsMODULEID moduleId,
                                          const char        *fileLine,
                                          mcsINT32          errorId,
                                          ... );
extern  mcsCOMPL_STAT errAddInLocalStack_v (errERROR          *error, 
                                          const mcsMODULEID moduleId,
                                          const char        *fileLine,
                                          mcsINT32          errorId,
                                          va_list           argPtr);

#ifdef __cplusplus
}
#endif

#endif /*!errPrivate_H*/
/*___oOo___*/
