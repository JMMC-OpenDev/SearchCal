/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * \file
 * Definition of errPackStack function.
 */

/* 
 * System Headers
 */
#include <stdio.h>
#include <string.h>

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
 * Packs global error structure to a buffer.
 *
 * This routines packs the error information into a buffer.
 * 
 * \param buffer Pointer to buffer where the error structure has to be packed
 * \param bufLen The size of buffer
 *
 * \return mcsSUCCESS, or mcsFAILURE if the buffer size is too small.
 */
mcsCOMPL_STAT errPackStack(char       *buffer,
                           mcsUINT32  bufLen)
{
    logTrace("errPackStack()");

    return (errPackLocalStack(errGetThreadStack(), buffer, bufLen));
}

/*___oOo___*/

