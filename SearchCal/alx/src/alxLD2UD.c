/*******************************************************************************
 * JMMC project
 * 
 * "@(#) $Id: alxLD2UD.c,v 1.1 2010-01-08 22:29:04 lafrasse Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 ******************************************************************************/

/**
 * @file
 * Function definition for computation of unifrom diameters from limb-darkened
 * diameter and spectral type, using jmcsLD2UD.
 * 
 * @usedfiles
 * @filename jmcsLD2UD :  conversion program called using exec()
 *
 * @sa JMMC-MEM-2610-0001
 */

static char *rcsId __attribute__ ((unused)) = "@(#) $Id: alxLD2UD.c,v 1.1 2010-01-08 22:29:04 lafrasse Exp $"; 


/* Needed to preclude warnings on snprintf(), popen() and pclose() */
#define  _BSD_SOURCE 1

/*
 * System Headers
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <ctype.h>


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
#include "alx.h"
#include "alxErrors.h"
#include "alxPrivate.h"


/*
 * Public functions definition
 */
/**
 * Compute uniform diameters from limb-darkened diamter and spectral type.
 *
 * @param ld limb-darkened diameter (milli arcseconds)
 * @param sp spectral type
 * @param ud output uniform diameters (milli arcseconds)
 *
 * @todo Handle executon failures.
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is
 * returned.
 */
mcsCOMPL_STAT alxComputeUDFromLDAndSP(mcsDOUBLE ld,
                                      mcsSTRING32 sp,
                                      alxUNIFORM_DIAMETERS* ud)
{
    logTrace("alxComputeUDFromLDAndSP()");

    /* Check parameter validity */
    if (sp == NULL)
    {
        errAdd(alxERR_NULL_PARAMETER, "sp");
        return mcsFAILURE;
    }
    if (ud == NULL)
    {
        errAdd(alxERR_NULL_PARAMETER, "ud");
        return mcsFAILURE;
    }

    /* Retrieve MCSROOT file path */
    const mcsENVNAME mcsRootEnvName = "INTROOT";
    mcsSTRING256 mcsRootPath;
    mcsCOMPL_STAT status = miscGetEnvVarValue(mcsRootEnvName, mcsRootPath, sizeof(mcsRootPath));
    if (status == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    /* Forge command */
    const char* staticCommand = "%s/bin/jmcsLD2UD %f \"%s\"";
    int composedCommandLength = strlen(mcsRootPath) + strlen(staticCommand) + strlen(sp) + 10 + 1;
    char* composedCommand = (char*)malloc(composedCommandLength * sizeof(char));
    if (composedCommand == NULL)
    {
        /*errAdd(miscERR_ALLOC);*/
        return mcsFAILURE;
    }
    snprintf(composedCommand, composedCommandLength, staticCommand, mcsRootPath, ld, sp);

    /* Executing the command */
    FILE* process = popen(composedCommand, "r");

    /* Keep reading command result, until an error occurs */
    mcsSTRING1024 outputBuffer;
    int availableMemory = sizeof(outputBuffer);
    int totalReadSize = 0;
    while (feof(process) == 0)
    {
        /* While buffer is not full yet */
        if (totalReadSize < (availableMemory - 1))
        {
            /* Write the command result in the buffer */
            totalReadSize += fread(outputBuffer, 1, availableMemory, process);

            /* Write tailing '\0' accordinaly */
            outputBuffer[totalReadSize] = '\0';
        }
        else /* Once the buffer has been fulfiled entirely */
        {
            /* Keep reading the result in a temporary buffer, to count needed
               memory space for later error message */
            mcsSTRING1024 tmp;
            totalReadSize += fread(tmp, 1, sizeof(tmp), process);
        }
    }
    int pcloseStatus = pclose(process);

    /* Give back local dynamically-allocated memory */
    free(composedCommand);

    /* Buffer overflow check */
    if (totalReadSize >= availableMemory)
    {
        /*errAdd(miscERR_BUFFER_OVERFLOW, availableMemory, totalReadSize);*/
        return mcsFAILURE;
    }

    /* pclose() status check */
    if (pcloseStatus == -1)
    {
        /*errAdd(miscERR_FUNC_CALL, "pclose", strerror(errno));*/
        return mcsFAILURE;
    }
    else
    {
        /* command exec status check */
        int commandStatus = WEXITSTATUS(pcloseStatus);
        if (commandStatus != 0)
        {
            return mcsFAILURE;
        }
    }

    /* Start parsing command output */
    miscDYN_BUF parsingBuffer;
    /* Dynamic buffer initializaton */
    if (miscDynBufInit(&parsingBuffer) == mcsFAILURE)
    {
        /* Handle error */
        return mcsFAILURE;
    }
    /* Dynamic buffer filing */
    if (miscDynBufAppendString(&parsingBuffer, outputBuffer) == mcsFAILURE)
    {
        /* Handle error */
        return mcsFAILURE;
    }
    char* index = NULL;
    mcsSTRING256 currentLine;
    const mcsUINT32 lineSize = sizeof(currentLine);
    int i = 0;
    /* Parsing each line */
    while ((index = miscDynBufGetNextLine(&parsingBuffer,
                                           index,
                                           currentLine,
                                           lineSize,
                                           mcsFALSE)) != NULL)
    {
        printf("Line #%d : %s\n", i, currentLine);
        char band = 0;
        mcsDOUBLE value = 0.0;
        if (sscanf(currentLine, "UD_%c=%lf", &band, &value) != 2)
        {
            /* Handle error */
            return mcsFAILURE;
        }

        switch (tolower(band))
        {
            case 'b':
                printf("B=%f\n", value);
                ud->b = value;
                break;

            case 'i':
                printf("I=%f\n", value);
                ud->i = value;
                break;

            case 'j':
                printf("J=%f\n", value);
                ud->j = value;
                break;

            case 'h':
                printf("H=%f\n", value);
                ud->h = value;
                break;

            case 'k':
                printf("K=%f\n", value);
                ud->k = value;
                break;

            case 'l':
                printf("L=%f\n", value);
                ud->l = value;
                break;

            case 'n':
                printf("N=%f\n", value);
                ud->n = value;
                break;

            case 'r':
                printf("R=%f\n", value);
                ud->r = value;
                break;

            case 'v':
                printf("V=%f\n", value);
                ud->v = value;
                break;

            default:
                printf("???\n");
                break;
        }
        i++;
    }

    printf("Received:\n%s", outputBuffer);
    ud->b = 0;

    return mcsSUCCESS;
}

/*___oOo___*/
