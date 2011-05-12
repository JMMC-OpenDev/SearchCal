#ifndef miscNetwork_H
#define miscNetwork_H
/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * \file
 * Definition of miscNetwork functions.
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
 * Local Headers
 */
#include "miscDynBuf.h"


/*
 * Pubic functions declaration
 */
 
mcsCOMPL_STAT miscGetHostName(char *hostName, const mcsUINT32 length);
mcsCOMPL_STAT miscGetHostByName(char *ipAddress, const char *hostName);
mcsCOMPL_STAT miscPerformHttpGet(const char *uri, miscDYN_BUF *outputBuffer, const mcsUINT32 timeout);
mcsCOMPL_STAT miscPerformHttpPost(const char *uri, const char *data, miscDYN_BUF *outputBuffer, const mcsUINT32 timeout);
char *        miscUrlEncode(const char *str);
char *        miscUrlDecode(const char *str);

#ifdef __cplusplus
}
#endif

#endif /*!miscNetwork_H*/

/*___oOo___*/
