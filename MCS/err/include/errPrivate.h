#ifndef errPrivate_H
#define errPrivate_H
/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

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
mcsCOMPL_STAT errResetLocalStack (errERROR_STACK *error);
mcsCOMPL_STAT errCloseLocalStack (errERROR_STACK *error);
mcsCOMPL_STAT errDisplayLocalStack (errERROR_STACK *error);
mcsLOGICAL    errIsInLocalStack (errERROR_STACK    *error,
                                 const mcsMODULEID moduleId,
                                 mcsINT32          errorId);
mcsLOGICAL    errLocalStackIsEmpty (errERROR_STACK *error);
mcsINT8       errGetLocalStackSize (errERROR_STACK *error);
mcsCOMPL_STAT errPackLocalStack (errERROR_STACK *error,
                                 char           *buffer,
                                 mcsUINT32      bufLen);
mcsCOMPL_STAT errUnpackLocalStack (errERROR_STACK *error,
                                   const char     *buffer,
                                   mcsUINT32      bufLen);
mcsCOMPL_STAT errPushInLocalStack(errERROR_STACK *error,
                                  const char     *timeStamp,
                                  const char     *procName,
                                  const char     *moduleId,
                                  const char     *location,
                                  mcsINT32       errorId,
                                  mcsLOGICAL     isErrUser,
                                  char           severity,
                                  const char     *runTimePar);
mcsCOMPL_STAT errAddInLocalStack (errERROR_STACK    *error, 
                                  const mcsMODULEID moduleId,
                                  const char        *fileLine,
                                  mcsINT32          errorId,
                                  mcsLOGICAL        isErrUser,
                                  ... );
mcsCOMPL_STAT errAddInLocalStack_v (errERROR_STACK    *error, 
                                    const mcsMODULEID moduleId,
                                    const char        *fileLine,
                                    mcsINT32          errorId,
                                    mcsLOGICAL        isErrUser,
                                    va_list           argPtr);
char         *errUserGetInLocalStack (errERROR_STACK *error);

errERROR_STACK* errGetThreadStack();

#ifdef __cplusplus
}
#endif

#endif /*!errPrivate_H*/
/*___oOo___*/
