#ifndef msg_H
#define msg_H
/*******************************************************************************
* JMMC project
*
* "@(#) $Id: msg.h,v 1.10 2004-11-26 13:11:28 lafrasse Exp $"
*
* who       when         what
* --------  -----------  -------------------------------------------------------
* lafrasse  10-Aug-2004  Ported from CILAS software
* lafrasse  07-Oct-2004  Added msgIsConnected
* lafrasse  19-Nov-2004  Moved all the C functions declaration to msgPrivate.h
* gzins     22-Nov-2004  Moved all the C structures declaration to msgMESSAGE.h
* lafrasse  23-Nov-2004  Added all the socket and error related headers
*
*
*******************************************************************************/

/**
 * \file
 * This header contains all the 'msg' library structures declarations.
 *
 */

#include "msgSOCKET.h"
#include "msgSOCKET_CLIENT.h"
#include "msgSOCKET_SERVER.h"
#include "msgMESSAGE.h"
#include "msgMANAGER_IF.h"

#endif /*!msg_H*/

/*___oOo___*/
