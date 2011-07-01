/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * \file
 * Definition of errAddInStack function.
 */


/* 
 * System Headers
 */
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

/*
 * Common Software Headers
 */
#include "mcs.h"
#include "log.h"

/*
 * Local Headers
 */
#include "err.h"
#include "errPrivate.h"

/**
 * Add a new log to the current error stack
 *
 * It places the error identified by \em errorId into the global stack with all
 * useful information such module name or file name and line number from where
 * this function has been called.
 *
 * \param moduleId module name
 * \param fileLine file name and line number from where function has been called
 * \param errorId error identifier
 * \param isErrUser specify whether the error message is intended or not to the
 * end-user
 *
 * \return mcsSUCCESS or mcsFAILURE if an error occured.
 *
 * \warning This function should be never called directly. The convenient macros
 * errAdd and errAddForEndUser should be used instead.
 *
 * \sa errAdd, errAddForEndUser
 */
mcsCOMPL_STAT errAddInStack(const mcsMODULEID moduleId,
                            const char        *fileLine,
                            mcsINT32          errorId, 
                            mcsLOGICAL        isErrUser,
                            ...)
{
    va_list       argPtr;
    mcsCOMPL_STAT status;

    /* TODO : CLEANUP : why log so many times errors (push, close, add ...) in stederr / out / socket ...*/
    logTest("errAddInStack(%s, %d)", moduleId, errorId);
    
    /* Call the error message */
    va_start(argPtr, isErrUser);
    status = errAddInLocalStack_v(errGetThreadStack(), moduleId, 
                                  fileLine, errorId, isErrUser, argPtr);
    va_end(argPtr);

    return (status);
}

/*___oOo___*/

