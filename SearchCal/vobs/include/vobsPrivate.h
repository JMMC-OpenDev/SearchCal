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

/*
 * Public methods 
 */

/**
 * Fast strcat alternative (destination and source MUST not overlap)
 * No buffer overflow checks
 * @param dest destination pointer (updated when this function returns to indicate the position of the last character)
 * @param src source buffer
 */
void vobsStrcatFast(char*& dest, const char* src);

#endif /*!vobsPrivate_H*/


/*___oOo___*/
