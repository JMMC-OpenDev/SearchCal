#ifndef msgPrivate_H
#define msgPrivate_H
/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * \file
 * Private 'msg' module header file, holding the MODULE_ID definition and
 * other constants.
 */

/* The following piece of code alternates the linkage type to C for all 
functions declared within the braces, which is necessary to use the 
functions in C++-code.
*/
#ifdef __cplusplus
extern "C" {
#endif


/* 
 * Constants definition
 */
#define MODULE_ID                       "msg"

/**
 * msgManger port number for connection
 */
#define msgMANAGER_SELECT_WIDTH          32
#define msgMANAGER_MAX_LENGTH_QUEUE      32

/**
 * Private command name
 */
#define msgREGISTER_CMD_NAME             "REGISTER"
#define msgCLOSE_CMD_NAME                "CLOSE"


/*
 * Private Globals
 */


#ifdef __cplusplus
}
#endif

#endif /*!msgPrivate_H*/


/*___oOo___*/
