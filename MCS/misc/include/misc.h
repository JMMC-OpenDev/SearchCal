#ifndef misc_H
#define misc_H
/*******************************************************************************
* JMMC project
*
* "@(#) $Id: misc.h,v 1.19 2004-08-02 15:23:40 lafrasse Exp $"
*
* who       when		 what
* --------  -----------	 -------------------------------------------------------
* gzins     16-Jun-2004  Created
* lafrasse  17-Jun-2004  Added miscGetLocalTimeStr, miscStripQuotes,
*                        miscStrToUpper and miscGetExtension
* lafrasse  18-Jun-2004  Added miscYankExtension and miscResolvePath
* lafrasse  23-Jun-2004  added miscDynBuf stuff
* lafrasse  01-Jul-2004  moved miscDynBuf stuff to miscDynBuf.h
* lafrasse  08-Jul-2004  Added 'modc' like doxygen documentation tags
* lafrasse  19-Jul-2004  Added miscDynStr doxygen comment
* lafrasse  20-Jul-2004  Added miscResolvePath, miscGetValueEnvVar, and
*                        miscYankLastPath for miscFile.c
* lafrasse  22-Jul-2004  Added an include of misDynStr.h to centralize 'misc'
*                        user includes on misc.h
*                        Added error management to miscDate
* gzins     23-Jul-2004  Added miscIsSpaceStr to miscString
* lafrasse  23-Jul-2004  Added error management to miscString
* lafrasse  02-Aug-2004  Removed all functions declaration, and included
*                        separated header files instead
*                        Removed miscDynStr.h include due to null-terminated
*                        string specific functions move from miscDynStr.h to
*                        miscDynBuf.h
*
*
*******************************************************************************/
 
/**
 * \file
 * This header include all the miscDate, miscFile and miscString functions
 * declarations hedears files.
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
#include "miscString.h"


#ifdef __cplusplus
}
#endif

#endif /*!misc_H*/

/*___oOo___*/
