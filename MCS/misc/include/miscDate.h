#ifndef miscDate_H
#define miscDate_H
/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: miscDate.h,v 1.4 2005-02-15 09:37:52 gzins Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * lafrasse  02-Aug-2004  Forked from misc.h to isolate miscDate headers
 *                        Moved mcs.h include in from miscDate.c
 *
 ******************************************************************************/

/**
 * \file
 * This header contains ONLY the miscDate functions declarations.
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
 
mcsCOMPL_STAT miscGetUtcTimeStr  (mcsBYTES32 localTime, mcsINT32 precision);
mcsCOMPL_STAT miscGetLocalTimeStr(mcsBYTES32 localTime, mcsINT32 precision);


#ifdef __cplusplus
}
#endif

#endif /*!miscDate_H*/

/*___oOo___*/
