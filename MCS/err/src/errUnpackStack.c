/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * \file
 * Definition of errUnpackStack function.
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
 * Extracts the error structure from buffer
 *
 * This routines extracts the error information from buffer, and stored the
 * extracted errors in the global stack structure.
 *
 * \param buffer Pointer to buffer where the error structure has been packed
 * \param bufLen The size of buffer
 *
 * \return mcsSUCCESS, or mcsFAILURE if the buffer size is too small.
 */
mcsCOMPL_STAT errUnpackStack(const char *buffer,
                             mcsUINT32  bufLen)
{
    return (errUnpackLocalStack(errGetThreadStack(), buffer, bufLen));
}

/*___oOo___*/

