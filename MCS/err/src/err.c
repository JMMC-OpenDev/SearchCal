/*******************************************************************************
* JMMC project
*
* 
*
* who       when		 what
* --------  -----------	 -------------------------------------------------------
* berezne   02-Jun-2004  created
*
*
********************************************************************************
*   NAME
* 
*   SYNOPSIS
* 
*   DESCRIPTION
*
*   FILES
*
*   ENVIRONMENT
*
*   RETURN VALUES 
*
*   CAUTIONS 
*
*   EXAMPLES
*
*   SEE ALSO
*
*   BUGS   
*
*-----------------------------------------------------------------------------*/

#define _POSIX_SOURCE 1

static char *rcsId="@(#) $Id: err.c,v 1.1 2004-06-03 12:08:24 berezne Exp $"; 
static void *use_rcsId = ((void)&use_rcsId,(void *) &rcsId);

/* 
 * System Headers
 */

#include <string.h>
#include <stdlib.h>

#include <stdarg.h>
#include <stdio.h>
#include <time.h>
#include <sys/types.h>
#include <sys/timeb.h>

/*
 * Common Software Headers
 */
#include "mcs.h"
#include "log.h"


/*
 * Local Headers
 */
#include "err.h"


/*  commentaire JBz
  #include "errPrivate.h"
*/

/*#+*/
mcsCOMPL_STAT errClear(errERROR *error)
{
/*#-*/
    logPrint(MODULE_ID, logEXTDBG, __FILE_LINE__, "%s", "errClear()");
    
    memset(error, '\0', sizeof(errERROR));
    errResetStack(error);

    return SUCCESS;
/*#+*/
}
/*#-*/

/*#+*/
mcsCOMPL_STAT errAdd(errERROR          *error,
                     const mcsMODULEID moduleId,
                     mcsINT32          errorId,
                     const errSEVERITY severity,
                     const char        *file,
                     const int         line,
                     char              *format, ...)
{
/*#-*/
    va_list     argPtr;
    mcsBYTES64  timeStamp;
    mcsLOC_ID   location;
    char        severityStr;
    mcsBYTES256 runTimePar;

    logPrint(MODULE_ID, logEXTDBG, __FILE_LINE__, "%s", "errAdd()");

    /* Recupere la date au format UNIX */
    mcsGetLocalTimeStr(timeStamp, 3);

    /* Convertit le niveau de severite en string */
    switch (severity)
    {
        case errWARNING:
            severityStr = 'W';
            break;
        case errSEVERE:
            severityStr = 'S';
            break;
        case errFATAL:
            severityStr = 'F';
            break;
        default :
            severityStr = '?';
            break;
    }

    /* Initialise la position de l'appel dans le fichier */
    sprintf((char *)location, "%s(%d)", mcsGetFileName((char *)file), line);

    /* Renseigne le message d'erreur */
    va_start(argPtr,format);
    vsprintf(runTimePar, format, argPtr);
    va_end(argPtr);
    
    return (errAddInStack(error, timeStamp, mcsGetProcName(),
                moduleId, location, errorId, severityStr, runTimePar));
/*#+*/
}
/*#-*/

/*#+*/
mcsLOGICAL errIsInStack (errERROR          *error,
                            const mcsMODULEID moduleId,
                            mcsINT32          errorId)
{
/*#-*/
    mcsINT32 i;

    logPrint(MODULE_ID, logEXTDBG, __FILE_LINE__, "%s", "errIsInStack()");

    /*= Pour chaque message d'erreur */
    for ( i = 0; i < error->stackSize; i++)
    {
        /*= Si le nom du module logiciel correspond a celui specifie */
        if (strcmp(error->stack[i].moduleId, moduleId) == 0)
        {
            /*= Si l'identificateur d'erreur correspond a celui specifie */
            if (error->stack[i].errorId == errorId)
            {
                /*= Retourne VRAI */
                return mcsTRUE;
            }
            /*= Fin si */
        }
        /*= Fin si */
    }
    /*= Fin pour */

    /*= Retourne FAUX */
    return mcsFALSE;
/*#+*/
}
/*#-*/

/*#+*/
mcsCOMPL_STAT errResetStack(errERROR *error)
{
/*#-*/
    mcsINT32 i;

    logPrint(MODULE_ID, logEXTDBG,  __FILE_LINE__, "%s", "errResetStack()");

    /*= Re-initialise la pile d'erreurs */
    for ( i = 0; i < error->stackSize; i++)
    {
        memset((char *)error->stack[i].timeStamp, '\0',
                sizeof(error->stack[i].timeStamp));
        error->stack[i].sequenceNumber = -1;
        memset((char *)error->stack[i].procName, '\0',
                sizeof(error->stack[i].procName));
        memset((char *)error->stack[i].location, '\0',
                sizeof(error->stack[i].location));
        memset((char *)error->stack[i].moduleId, '\0',
                sizeof(error->stack[i].moduleId));
        error->stack[i].severity = -1;
        memset((char *)error->stack[i].runTimePar, '\0',
                sizeof(error->stack[i].runTimePar));
    }
    error->stackSize = 0;
    error->stackOverflow = mcsFALSE;
    error->stackEmpty = mcsTRUE;

    return SUCCESS;
/*#+*/
}
/*#-*/

/*#+*/
mcsCOMPL_STAT errCloseStack(errERROR *error)
{
/*#-*/
    mcsINT32    i;
    mcsBYTES128 tab;

    logPrint(MODULE_ID, logEXTDBG,  __FILE_LINE__, "%s", "errCloseStack()");

    memset(tab, '\0', sizeof(tab));

    /*= Pour chaque message d'erreur */
    for ( i = 0; i < error->stackSize; i++)
    {
        /*= Formate message d'erreur */
        char log[logMAX_LEN];
        char logBuf[512];

        memset(logBuf, '\0', sizeof(logBuf));

        sprintf(logBuf,"%s %d %c %s",
                error->stack[i].location,
                error->stack[i].sequenceNumber,
                error->stack[i].severity,
                error->stack[i].runTimePar);

        /* Tronque le message si necessaire */
        if (strlen(logBuf) > logTEXT_LEN)
        {
            logBuf[logTEXT_LEN] = '\0';
        }

        sprintf(log,"%s%s", tab, logBuf);

        /*= Envoyer le message dans le 'journal de bord' */
        logData (error->stack[i].moduleId, error->stack[i].timeStamp, 
            error->stack[i].procName, log);

        /* Decale le message d'erreur pour montrer la hierachie */
        strcat(tab, " ");
    }

    /*= Re-initialise la pile d'erreur */
    errResetStack(error);

    return SUCCESS;
/*#+*/
}
/*#-*/

/*#+*/
mcsCOMPL_STAT errDisplay(errERROR *error)
{
/*#-*/
    char buffer[2048];

    logPrint(MODULE_ID, logEXTDBG,  __FILE_LINE__, "%s", "errDisplay()");
    
    /*= Formater les messages d'erreur */
    errStoreExtrBuffer(error, buffer, 2048, mcsTRUE); 

    /*= Afficher les messages d'erreurs */
    printf ("%s", buffer);

    return SUCCESS;
/*#+*/
}
/*#-*/

/*#+*/
mcsLOGICAL errIsEmpty (errERROR *error)
{
/*#-*/

    logPrint(MODULE_ID, logEXTDBG,  __FILE_LINE__, "%s", "errIsEmpty()");

    /*#= Retourne l'indicateur d'etat de la pile */
    return (error->stackEmpty);
/*#+*/
}
/*#-*/

/*#+*/
mcsCOMPL_STAT errStoreExtrBuffer(errERROR *error, char *buffer,
        mcsUINT32 bufSize, mcsLOGICAL tabulate)
{
/*#-*/
    mcsINT32    i;
    mcsBYTES128 tab;
    mcsUINT32   bufLen;

    logPrint(MODULE_ID, logEXTDBG,   __FILE_LINE__, "%s", "errStoreExtrBuffer()");
    
    memset(tab, '\0', sizeof(tab));
    memset(buffer, '\0', bufSize);
    bufLen = 0;
    
    /*= Pour chaque message d'erreur */
    for ( i = 0; i < error->stackSize; i++)
    {
        /*= Formater message d'erreur */
        char log[logMAX_LEN];
        char logBuf[512];

        sprintf(logBuf,"%s %d %c %s",
                error->stack[i].location,
                error->stack[i].errorId,
                error->stack[i].severity,
                error->stack[i].runTimePar);

        /* Tronquer le message si necessaire */
        if (strlen(logBuf) > logTEXT_LEN)
        {
            logBuf[logTEXT_LEN] = '\0';
        }

        memset(log, '\0', sizeof(log));
        sprintf(log,"%s - %s %s %s%s\n",
                error->stack[i].timeStamp, error->stack[i].moduleId,
                error->stack[i].procName, tab, logBuf);

        /*= Stocker le message dans le buffer */
        if ((bufSize - bufLen) > strlen(log))
        {
            strncpy(&buffer[bufLen], log, strlen(log));
            bufLen += strlen(log);
        }
        else
        {
            logPrint(MODULE_ID, logWARNING, __FILE_LINE__, "%s %d",
                    "errStoreExtrBuffer() - Taille de buffer (%d) trop petite:",
                    bufSize);
            return FAILURE;
        }
        
        /*= Decaler le message d'erreur pour montrer la hierarchie */
        if (tabulate == mcsTRUE)
        {
            strcat(tab, " ");
        }
    }
    /*= Fin pour */

    return SUCCESS;
/*#+*/
}
/*#-*/

/*#+*/
mcsCOMPL_STAT errLoadExtrBuffer(errERROR *error, char *buffer, 
        mcsUINT32 bufLen)
{
/*#-*/
    mcsUINT32    i;
    mcsUINT32   nbErrors;
    mcsUINT32   bufPos;

    logPrint(MODULE_ID, logEXTDBG, __FILE_LINE__, "%s", "errLoadExtrBuffer()");
    
    /*= Remplacer tous les CR par '\0' */
    nbErrors = 0;
    for (i = 0; i < bufLen; i++)
    {
        if (buffer[i] == '\n')
        {
            buffer[i] = '\0';
            nbErrors += 1;
        }
    }
    
    /*= Pour chaque message d'erreur */
    bufPos = 0;
    for ( i = 0; i < nbErrors; i++)
    {
        mcsMODULEID moduleId;
        mcsINT32    errorId;
        mcsBYTES64  timeStamp;
        mcsLOC_ID   location;
        char        severityStr;
        mcsBYTES256 runTimePar;
        mcsPROCNAME  procName;

        /*= Extraire les champs de l'erreur */
        if (sscanf(&buffer[bufPos],"%s %*s %s %s %s %d %c %[^\0]",
                timeStamp, moduleId, procName, location, &errorId, &severityStr,
                runTimePar) != 7)
        {
            logPrint(MODULE_ID, logWARNING,  __FILE_LINE__, "%s", 
                    "errLoadExtrBuffer() - Format du buffer incorrect");
            return FAILURE;
        }

        bufPos += strlen(&buffer[bufPos]) + 1;
        
        /*= Ajouter l'erreur a la pile */
        if (errAddInStack(error, timeStamp, mcsGetProcName(), moduleId, 
                    location, errorId, severityStr, runTimePar) == FAILURE)
        {
            return FAILURE;
        }
    }
    /*= Fin pour */

    return SUCCESS;
/*#+*/
}
/*#-*/


/* Fonctions locales */ 

/*#+*/
mcsCOMPL_STAT errAddInStack(errERROR          *error,
                            const char        *timeStamp,
                            const char        *procName,
                            const char        *moduleId,
                            const char        *location,
                            mcsINT32          errorId,
                            char              severity,
                            char              *runTimePar)
{
/*#-*/

    mcsINT32 errNum;

    logPrint(MODULE_ID, logEXTDBG, __FILE_LINE__, "%s", "errAddInStack()");

    /*#= Verification des parametres */
    if (error  ==  NULL)
    {
        logPrint(MODULE_ID, logWARNING, __FILE_LINE__, "%s", 
                "errAddInStack() - le parametre 'error' est un pointeur NULL");
       return(FAILURE);
    }

    /*= Si la pile est remplie */
    if (error->stackSize == errSTACK_SIZE)
    {
        logPrint(MODULE_ID, logWARNING, __FILE_LINE__, "%s", 
                "errAddInStack() - la pile d'erreur est complete: "
                "log automatique des messages d'erreur");

        /*= Affichage des messages de la pile d'erreur */
        errCloseStack(error);
    }

    /*= Incremente le nombre d'erreurs dans la pile, et met a jour l'etat de la
     *= pile */
    error->stackSize++;
    error->stackEmpty = mcsFALSE;

    /*= Ajoute l'erreur a la pile */
    errNum = error->stackSize - 1;

    /* Complete les champs */
    error->stack[errNum].sequenceNumber = error->stackSize;
    strncpy((char *)error->stack[errNum].timeStamp, timeStamp,
            (sizeof(error->stack[errNum].timeStamp)-1));
    strncpy((char *)error->stack[errNum].procName, mcsGetProcName(),
            (sizeof(mcsPROCNAME)-1));
    strncpy((char *)error->stack[errNum].moduleId, moduleId,
            (sizeof(mcsMODULEID)-1));
    strncpy((char *)error->stack[errNum].location, location,
            (sizeof(mcsLOC_ID) -1));
    strcpy((char *) error->stack[errNum].moduleId, moduleId);
    error->stack[errNum].errorId = errorId;
    error->stack[errNum].severity = severity;
    strcpy((char *)error->stack[errNum].runTimePar, runTimePar);
 
    return SUCCESS;
/*#+*/
}
/*#-*/

/*___oOo___*/

