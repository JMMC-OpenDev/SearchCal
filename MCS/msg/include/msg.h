#ifndef msg_H
#define msg_H
/*******************************************************************************
* JMMC project
*
* "@(#) $Id: msg.h,v 1.9 2004-11-22 14:30:27 gzins Exp $"
*
* who       when         what
* --------  -----------  -------------------------------------------------------
* lafrasse  10-Aug-2004  Ported from CILAS software
* lafrasse  07-Oct-2004  Added msgIsConnected
* lafrasse  19-Nov-2004  Moved all the C functions declaration to msgPrivate.h
*
*
*******************************************************************************/

/**
 * \file
 * This header contains all the 'msg' library structures declarations.
 *
 */

/* The following piece of code alternates the linkage type to C for all 
functions declared within the braces, which is necessary to use the 
functions in C++-code.
*/
#ifdef __cplusplus
extern "C" {
#endif

#include "msgMESSAGE.h"
#include "msgMANAGER_IF.h"

#ifdef __cplusplus
}
#endif

#endif /*!msg_H*/

/*___oOo___*/
