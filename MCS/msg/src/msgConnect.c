/*******************************************************************************
* JMMC project
* 
* "@(#) $Id: msgConnect.c,v 1.1 2004-08-24 15:01:53 lafrasse Exp $"
*
* who       when         what
* --------  -----------  -------------------------------------------------------
* lafrasse  11-Aug-2004  Ported from CILAS software
*
*
*******************************************************************************/

/**
 * \file
 * Contain the msgConnect() function definition, used to establish the
 * connection whith the 'msgManager' communication server.
 * 
 */

static char *rcsId="@(#) $Id: msgConnect.c,v 1.1 2004-08-24 15:01:53 lafrasse Exp $"; 
static void *use_rcsId = ((void)&use_rcsId,(void *) &rcsId);


/* 
 * System Headers
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>


/*
 * MCS Headers 
 */
#include "mcs.h"
#include "log.h"
#include "err.h"
#include "misc.h"


/* 
 * Local Headers
 */
#include "msg.h"
#include "msgPrivate.h"
#include "msgErrors.h"


/**
 * Try to establish the connection with the communication server.
 *
 * The server host name is (in order) :
 * \li the msgManagerHost parameter if its value is \em not NULL
 * \li the MSG_MANAGER_HOST environment variable value, if it is defined
 * \li the local host name
 *
 * \param procName the local processus name
 * \param msgManagerHost the server host name, or NULL
 *
 * \return an MCS completion status code (SUCCESS or FAILURE)
 */
mcsCOMPL_STAT   msgConnect        (const mcsPROCNAME  procName,
                                   const char*        msgManagerHost)
{
    struct hostent    *remoteHostEnt;
    
    mcsBYTES256        hostName;
    
    int                sd;
    struct sockaddr_in addr;
    mcsINT32           nbRetry;
    mcsINT32           status;
    msgMESSAGE         msg;

    logExtDbg("msgConnect()");

    /* If a connection is already open... */
    if (msgManagerSd != -1)
    {
        /* Raise an error */
        errAdd(msgERR_PROC_ALREADY_CONNECTED);

        /* Return an error code */
        return FAILURE;
    }

    /* Try to initialize MCS services */
    if (mcsInit(procName) == FAILURE)
    {
        errAdd(msgERR_MCSINIT);
        return FAILURE;
    }

    /* Get a valid host name */
    /* If the server host name is given by parameter... */
    if (msgManagerHost != NULL)
    {
        strcpy(hostName, msgManagerHost);
    }
    else
    {
        /* If the MSG_MANAGER_HOST Env. Var. is not defined... */
        if (miscGetEnvVarValue("MSG_MANAGER_HOST", hostName,sizeof(mcsBYTES256))
            == FAILURE)
        {
            /* Try to get the local host name */
            memset(hostName, '\0', sizeof(hostName));
            if (miscGetHostName(hostName, sizeof(hostName)) == FAILURE)
            {
                return FAILURE;
            }
        }
    }

    /* Get msgManager server data */
    logTest("'msgManager' server host name is '%s'", hostName );
    remoteHostEnt = gethostbyname(hostName);
    if (remoteHostEnt == (struct hostent *) NULL)
    {
        errAdd(msgERR_GETHOSTBYNAME, strerror(errno));
        return FAILURE;
    }

    /* Try to establish the connection, retry otherwise... */
    nbRetry = 2;
    do 
    {
        /* Create the socket */
        sd = socket(AF_INET, SOCK_STREAM, 0);
        if(sd == -1)
        { 
            errAdd(msgERR_SOCKET, strerror(errno));
            return FAILURE; 
        }

        /* Initialize sockaddr_in */
        memset((char *) &addr, 0, sizeof(addr));
        addr.sin_port = htons(msgMANAGER_PORT_NUMBER);
        memcpy(&(addr.sin_addr), remoteHostEnt->h_addr,remoteHostEnt->h_length);
        addr.sin_family = AF_INET;

        /* Try to connect to msgManager */
        status = connect(sd , (struct sockaddr *)&addr, sizeof(addr));
        if (status == -1)
        {
            if (--nbRetry <= 0 )
            { 
                errAdd(msgERR_CONNECT, strerror(errno));
                return FAILURE; 
            }
            else
            {
                logWarning("Cannot connect to 'msgManager'. Trying again...");
                sleep(1);
                close(sd);
            }
        }
    } while (status == -1);


    /* Store the established connection socket */
    msgManagerSd = sd;

    /* Try to register with msgManager */
    if (msgSendCommand (msgREGISTER_CMD, "msgManager", NULL, 0) == FAILURE)
    {
        close(msgManagerSd);
        msgManagerSd = -1;
        return FAILURE;
    }
    
    /* Wait for its answer */
    if (msgReceive(&msg, 1000) == FAILURE)
    {
        close(msgManagerSd);
        msgManagerSd = -1;
        return FAILURE;
    }

    /* If the reply as ERROR... */
    if (msgGetType(&msg) == msgTYPE_ERROR_REPLY)
    {
        /* Try to put the received errors in the MCS error stack */
        if (errUnpackStack(msgGetBodyPtr(&msg), msgGetBodySize(&msg))
            == FAILURE)
        {
            return FAILURE;
        }
        close(msgManagerSd);
        msgManagerSd = -1;       
        return FAILURE;
    }
    
    logExtDbg("Connection to 'msgManager' established");

    return SUCCESS;
}


/*___oOo___*/
