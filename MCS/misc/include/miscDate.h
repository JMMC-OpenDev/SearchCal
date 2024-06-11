#ifndef miscDate_H
#define miscDate_H
/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * @file
 * Declaration of miscDate functions.
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
 * Pubic functions declaration
 */

mcsCOMPL_STAT miscGetUtcTimeStr  (const mcsUINT32    precision,
                                        mcsSTRING32  utcTime);

mcsCOMPL_STAT miscGetLocalTimeStr(const mcsUINT32    precision,
                                        mcsSTRING32  localTime);


#ifdef __cplusplus
}
#endif

#endif /*!miscDate_H*/

/*___oOo___*/
