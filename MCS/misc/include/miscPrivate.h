#ifndef miscPrivate_H
#define miscPrivate_H
/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: miscPrivate.h,v 1.7 2005-03-03 16:11:51 gluck Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.6  2005/02/15 09:37:52  gzins
 * Added CVS log as file modification history
 *
 * gzins     24-Jun-2004  Created
 * lafrasse  08-Jul-2004  Added 'modc' like doxygen documentation tags
 * lafrasse  23-Jul-2004  Added miscDYN_BUF_MAGIC_STRUCTURE_ID from miscDynBuf.h
 * lafrasse  02-Aug-2004  Added mcs.h include
 *
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
