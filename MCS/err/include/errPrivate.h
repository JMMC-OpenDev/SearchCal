#ifndef errPrivate_H
#define errPrivate_H
/*******************************************************************************
* JMMC project
*
* "@(#) $Id: errPrivate.h,v 1.4 2004-12-14 13:12:38 lafrasse Exp $"
*
* who       when		 what
* --------  -----------	 -------------------------------------------------------
* gzins     16-Jun-2004  Created
* lafrasse  14-Dec-2004  Moved errMSG_MAX_LEN to err.H
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

/* Local functions */
mcsCOMPL_STAT errResetLocalStack (errERROR *error);
mcsCOMPL_STAT errCloseLocalStack (errERROR *error);
mcsCOMPL_STAT errDisplayLocalStack (errERROR *error);
mcsLOGICAL    errIsInLocalStack  (errERROR          *error,
                                  const mcsMODULEID moduleId,
                                  mcsINT32          errorId);
mcsLOGICAL    errLocalStackIsEmpty (errERROR          *error);
mcsINT8       errGetLocalStackSize (errERROR *error);
mcsCOMPL_STAT errPackLocalStack (errERROR   *error,
                                 char       *buffer,
                                 mcsUINT32  bufLen);
mcsCOMPL_STAT errUnpackLocalStack (errERROR   *error,
                                   char       *buffer,
                                   mcsUINT32  bufLen);
mcsCOMPL_STAT errPushInLocalStack(errERROR   *error,
                                  const char *timeStamp,
                                  const char *procName,
                                  const char *moduleId,
                                  const char *location,
                                  mcsINT32   errorId,
                                  char       severity,
                                  char       *runTimePar);
mcsCOMPL_STAT errAddInLocalStack (errERROR          *error, 
                                  const mcsMODULEID moduleId,
                                  const char        *fileLine,
                                  mcsINT32          errorId,
                                  ... );
mcsCOMPL_STAT errAddInLocalStack_v (errERROR          *error, 
                                    const mcsMODULEID moduleId,
                                    const char        *fileLine,
                                    mcsINT32          errorId,
                                    va_list           argPtr);

#ifdef __cplusplus
}
#endif

#endif /*!errPrivate_H*/
/*___oOo___*/
