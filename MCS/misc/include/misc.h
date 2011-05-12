#ifndef misc_H
#define misc_H
/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/
 
/**
 * \file
 * This header include all the miscDate, miscDynBuf, miscFile, miscNetwork and
 * miscString headers files.
 */

/* The following piece of code alternates the linkage type to C for all 
functions declared within the braces, which is necessary to use the 
functions in C++-code.
*/
#ifdef __cplusplus
extern "C" {
#endif


/*
 * Local Headers 
 */
#include "miscDate.h"
#include "miscDynBuf.h"
#include "miscFile.h"
#include "miscNetwork.h"
#include "miscString.h"
#include "miscHash.h"

#ifdef __cplusplus
}
#endif

#endif /*!misc_H*/

/*___oOo___*/
