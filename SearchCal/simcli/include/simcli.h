#ifndef simcli_H
#define simcli_H
/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * @file
 * Brief description of the header file, which ends at this dot.
 */

/* The following piece of code alternates the linkage type to C for all
functions declared within the braces, which is necessary to use the
functions in C++-code.
 */
#ifdef __cplusplus
extern "C"
{
#endif


/*
 * MCS header
 */
#include "mcs.h"
#include "misc.h"

mcsCOMPL_STAT simcliGetCoordinates(char *name,
                                   char *ra, char *dec,
                                   mcsDOUBLE *pmRa, mcsDOUBLE *pmDec,
                                   char *spType, char *objTypes,
                                   char* mainId);

#ifdef __cplusplus
}
#endif

#endif /*!simcli_H*/
