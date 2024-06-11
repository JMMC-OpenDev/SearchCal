#ifndef miscPrivate_H
#define miscPrivate_H
/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * \file
 * Private header file of misc module.
 */

/* The following piece of code alternates the linkage type to C for all 
functions declared within the braces, which is necessary to use the 
functions in C++-code.
*/
#ifdef __cplusplus
extern "C" {
#endif

/* 
 * MCS Headers
 */
#include "mcs.h"


/*
 * Constants definition
 */

/* Module name */ 
#define MODULE_ID "misc"


/*
 * Macro definition
 */

/**
 * Unique MCS structure identifier.
 *
 * It is meant to allow Dynamic Buffer struture initialization state test
 * (whether it has allready been initialized as a miscDYN_BUF or not).
 */
#define miscDYN_BUF_MAGIC_STRUCTURE_ID ((mcsUINT32) 2813741963u)

#ifdef __cplusplus
}
#endif

#endif /*!miscPrivate_H*/
/*___oOo___*/
