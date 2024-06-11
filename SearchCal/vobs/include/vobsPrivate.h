#ifndef vobsPrivate_H
#define vobsPrivate_H
/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/*
 * General common includes
 */
#include "math.h"

#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif

#define MODULE_ID "vobs"

/* hour angle <=> degrees conversions */
#define vobsHA_IN_DEG 15.0
#define vobsDEG_IN_HA (1.0 / 15.0)

/*
 * Public methods 
 */

/* Return mcsTRUE if the development flag is enabled (env var ); mcsFALSE otherwise */
mcsLOGICAL vobsGetDevFlag();

/**
 * Fast strcat alternative (destination and source MUST not overlap)
 * No buffer overflow checks
 * @param dest destination pointer (updated when this function returns to indicate the position of the last character)
 * @param src source buffer
 */
void vobsStrcatFast(char*& dest, const char* src);

#endif /*!vobsPrivate_H*/


/*___oOo___*/
