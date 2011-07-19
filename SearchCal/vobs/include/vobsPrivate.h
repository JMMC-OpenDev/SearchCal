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

void vobsStrcatFast(char*& dest, const char* src);

#endif /*!vobsPrivate_H*/


/*___oOo___*/
