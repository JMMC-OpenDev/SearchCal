/*******************************************************************************
* JMMC project
* 
* "@(#) $Id: msgManagerProcessList.c,v 1.2 2004-11-22 14:20:03 gzins Exp $"
*
* who       when         what
* --------  -----------  -------------------------------------------------------
* lafrasse  13-Aug-2004  Ported from CILAS software
*
*
*******************************************************************************/

/**
 * \file
 * Contain the msgManagerProcessListXxx() functions definition, used to manage the
 * connected process linked list.
 * 
 */

static char *rcsId="@(#) $Id: msgManagerProcessList.c,v 1.2 2004-11-22 14:20:03 gzins Exp $"; 
static void *use_rcsId = ((void)&use_rcsId,(void *) &rcsId);


/*
 * System Headers
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/*
 * MCS Headers
 */
#include "mcs.h"
#include "log.h"
#include "err.h"


/*
 * Local Headers
 */
#include "msgMESSAGE.h"
#include "msgManager.h"
#include "msgPrivate.h"
#include "msgErrors.h"


/**
 * Try to initialize the process list structure.
 *
 * \param list the list structure
 * \param readMask the select() read mask
 *
 * \return an MCS completion status code (SUCCESS or FAILURE)
 */
mcsCOMPL_STAT   msgManagerProcessListInit (msgPROCESS_LIST    *list,
                                           fd_set             *readMask)
{
    logExtDbg("msgManagerProcessListInit()");

    /* Process list structure initialization */
    list->nbProcess = 0;
    list->header    = NULL;
    list->readMask  = readMask;

    return SUCCESS;
}


/**
 * Try to add a new process to the process list structure.
 *
 * Add the process at the head of the list.
 *
 * \param list the list structure
 * \param procName the new process name
 * \param sd the new process associated socket
 *
 * \return an MCS completion status code (SUCCESS or FAILURE)
 */
mcsCOMPL_STAT   msgManagerProcessListAdd  (msgPROCESS_LIST    *list, 
                                           mcsPROCNAME         procName,
                                           int                 sd)
{
    msgPROCESS *newProcess;
    logExtDbg("msgManagerProcessListAdd()");

    /* Try to allocate memory for the new pricess element structure */
    newProcess = malloc(sizeof(msgPROCESS));
    if (newProcess == NULL)
    {
        errAdd(msgERR_MALLOC);
        return FAILURE;
    }

    /* Initialize the new process element structure */
    newProcess->sd = sd;
    memset(newProcess->name, '\0', sizeof(mcsPROCNAME));
    strncpy(newProcess->name, procName, sizeof(mcsPROCNAME)-1);

    /* Update the select() read mask with the new process socket */
    FD_SET(sd ,list->readMask) ;

    /* Add the new process element structure at the process list head */
    newProcess->next = list->header;
    list->header = newProcess;

    /* Increment the process list stored element number */
    list->nbProcess += 1;

    return SUCCESS;
}


/**
 * Return a seeked process from the process list by its socket identifier.
 *
 * \param list the list structure
 * \param sd the seeked process associated socket identifier
 *
 * \return a process element structure address, or NULL if not found
 */
msgPROCESS     *msgManagerProcessListFind (msgPROCESS_LIST    *list,
                                           int                 sd)
{
    logExtDbg("msgManagerProcessListFind()");

    /* For each process of the process list... */
    msgPROCESS *currProcess = list->header;
    while (currProcess != NULL)
    {
        /* If the current process socket identifier is the one looked for... */
        if (currProcess->sd == sd)
        {
            /* Return the current process element address */
            return (currProcess) ;
        }

        currProcess = currProcess->next ;
    }

    /* If no process in the process list is the one searched, return NULL */
    return (msgPROCESS *)NULL;
}


/**
 * Return a seeked process from the process list by its name.
 *
 * \param list the list structure
 * \param procName the seeked process name
 *
 * \return a process element structure address, or NULL if not found
 */
msgPROCESS     *msgManagerProcessListFindByName(
                                           msgPROCESS_LIST    *list, 
                                           mcsPROCNAME         procName)
{
    logExtDbg("msgManagerProcessListFindByName()");

    /* For each process of the process list... */
    msgPROCESS *currProcess = list->header;
    while (currProcess != NULL)
    {
        /* If the current process name is the one looked for... */
        if (strcmp(currProcess->name, procName) == 0)
        {
            /* Return the current process element address */
            return (currProcess) ;
        }

        currProcess = currProcess->next ;
    }

    /* If no process in the process list is the one searched, return NULL */
    return (msgPROCESS *)NULL;
}


/**
 * Try to remove a process from the process list.
 *
 * Before removing the process, close its socket and update the select() read
 * mask accordinaly.
 *
 * \param list the list structure
 * \param process the seeked process element structure address
 *
 * \return an MCS completion status code (SUCCESS or FAILURE)
 */
mcsCOMPL_STAT   msgManagerProcessListRemove(
                                           msgPROCESS_LIST    *list, 
                                           msgPROCESS         *process)
{
    logExtDbg("msgManagerProcessListRemove()");

    /* If the process list is empty... */
    if (list->header == NULL)
    {
        errAdd(msgERR_EMPTY_PROCLIST, "Could not remove the seeked process");
        return FAILURE;
    }

    /* If the seeked process id the first of the list... */
    if (list->header == process)
    {
        /* Remove it from the process list */
        list->header = process->next;
    }
    else
    {
        /* Search the list for the seeked process */
        msgPROCESS *currProcess = list->header;
        while ((currProcess->next != process) && (currProcess->next != NULL))
        {
            currProcess = currProcess->next ;
        }

        /* If the seeked process was not found... */
        if (currProcess->next == NULL)
        {
            /* Raise an error */
            errAdd(msgERR_PROC_NOT_FOUND, process->name);

            /* Return an error code */
            return FAILURE;
        }

        /* Remove the seeked process */
        currProcess->next = currProcess->next->next ;
    }

    /* Close the seeked process socket */
    msgSocketClose(process->sd);

    /* Update the select() read mask */
    FD_CLR(process->sd ,list->readMask) ;

    /* Free the seeked process element structure memory */
    free(process);

    /* Decrement the process list stored element number */
    list->nbProcess -= 1;

    return SUCCESS;
}


/*___oOo___*/
