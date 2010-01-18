/*******************************************************************************
 * JMMC project
 * 
 * "@(#) $Id: alxLD2UD.c,v 1.3 2010-01-18 15:52:37 lafrasse Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.2  2010/01/15 17:44:22  lafrasse
 * Added use of miscLocateExe() to properly resolve 'jmcsLD2UD' executable path.
 * Moved shell command execution code to miscDynBufExecuteCommand().
 * Added log.
 *
 * Revision 1.1  2010/01/08 22:29:04  lafrasse
 * Added preliminary support for alxLD2UD.
 *
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

static char *rcsId __attribute__ ((unused)) = "@(#) $Id: alxLD2UD.c,v 1.3 2010-01-18 15:52:37 lafrasse Exp $"; 


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
mcsCOMPL_STAT alxComputeUDFromLDAndSP(const mcsDOUBLE ld,
                                      const mcsSTRING32 sp,
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

    /* Retrieve jmcsLD2UD executable amongst MCS standard path */
    char* executablePath = miscLocateExe("jmcsLD2UD");
    if (executablePath == NULL)
    {
        return mcsFAILURE;
    }

    /* Forge command */
    const char* staticCommand = "%s %f \"%s\"";
    int composedCommandLength = strlen(staticCommand) + strlen(executablePath) + strlen(sp) + 10 + 1;
    char* composedCommand = (char*)malloc(composedCommandLength * sizeof(char));
    if (composedCommand == NULL)
    {
        /*errAdd(miscERR_ALLOC);*/
        return mcsFAILURE;
    }
    snprintf(composedCommand, composedCommandLength, staticCommand, executablePath, ld, sp);

    /* Dynamic buffer initializaton */
    miscDYN_BUF resultBuffer;
    if (miscDynBufInit(&resultBuffer) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    mcsCOMPL_STAT executionStatus = miscDynBufExecuteCommand(&resultBuffer, composedCommand);

    /* Give back local dynamically-allocated memory */
    free(composedCommand);
    if (executionStatus == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    /* Remove any trailing or leading '\n' */
    if (miscTrimString(miscDynBufGetBuffer(&resultBuffer), "\n") == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    /* Parsing each line */
    const char* index = NULL;
    mcsSTRING256 currentLine;
    const mcsUINT32 lineSize = sizeof(currentLine);
    while ((index = miscDynBufGetNextLine(&resultBuffer,
                                           index,
                                           currentLine,
                                           lineSize,
                                           mcsTRUE)) != NULL)
    {
        logDebug("Parsing line : %s\n", currentLine);
        char band = '0';
        mcsDOUBLE value = 0.0;
        if (sscanf(currentLine, "UD_%c=%lf", &band, &value) != 2)
        {
            /* Handle error */
            return mcsFAILURE;
        }

        switch (tolower(band))
        {
            case 'b':
                logTest("UD_B = %f\n", value);
                ud->b = value;
                break;

            case 'i':
                logTest("UD_I = %f\n", value);
                ud->i = value;
                break;

            case 'j':
                logTest("UD_J = %f\n", value);
                ud->j = value;
                break;

            case 'h':
                logTest("UD_H = %f\n", value);
                ud->h = value;
                break;

            case 'k':
                logTest("UD_K = %f\n", value);
                ud->k = value;
                break;

            case 'l':
                logTest("UD_L = %f\n", value);
                ud->l = value;
                break;

            case 'n':
                logTest("UD_N = %f\n", value);
                ud->n = value;
                break;

            case 'r':
                logTest("UD_R = %f\n", value);
                ud->r = value;
                break;

            case 'v':
                logTest("V = %f\n", value);
                ud->v = value;
                break;

            default:
                logTest("Unknown band '%c'.\n", band);
                break;
        }
    }

    miscDynBufDestroy(&resultBuffer);
    return mcsSUCCESS;
}

/**
 * Log content of an alxUNIFORM_DIAMETERS strucutre on STDOUT.
 *
 * @param ud uniform diameters to log.
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is
 * returned.
 */
mcsCOMPL_STAT alxShowUNIFORM_DIAMETERS(const alxUNIFORM_DIAMETERS* ud)
{
    logTrace("alxComputeUDFromLDAndSP()");

    /* Check parameter validity */
    if (ud == NULL)
    {
        errAdd(alxERR_NULL_PARAMETER, "ud");
        return mcsFAILURE;
    }

    printf("alxUNIFORM_DIAMETERS struct at %p contains:\n", ud);
    printf("\tud.b = %lf\n", ud->b);
    printf("\tud.i = %lf\n", ud->i);
    printf("\tud.j = %lf\n", ud->j);
    printf("\tud.h = %lf\n", ud->h);
    printf("\tud.k = %lf\n", ud->k);
    printf("\tud.l = %lf\n", ud->l);
    printf("\tud.n = %lf\n", ud->n);
    printf("\tud.r = %lf\n", ud->r);
    printf("\tud.v = %lf\n", ud->v);
}

/*___oOo___*/
