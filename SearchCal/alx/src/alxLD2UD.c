/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
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



/* Needed to preclude warnings on snprintf(), popen() and pclose() */
#define  _BSD_SOURCE  1

/*
 * System Headers
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <ctype.h>

/* Needed for FP_NAN support */
#define  __USE_ISOC99 1
#include <math.h>


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
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is
 * returned.
 */
mcsCOMPL_STAT alxComputeTeffAndLoggFromSptype(const mcsSTRING32 sp,
					      mcsDOUBLE *Teff,
					      mcsDOUBLE *LogG)
{
    logTrace("alxComputeTeffAndLoggFromSptype()");

    /* Check parameter validity */
    if (sp == NULL)
    {
        errAdd(alxERR_NULL_PARAMETER, "sp");
        return mcsFAILURE;
    }

    /* Dynamic buffer initializaton */
    miscDYN_BUF resultBuffer;
    if (miscDynBufInit(&resultBuffer) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    /* Forge URI using hard coded URL 
     * (sptype is encoded because it can embed spaces or special characters) */
    char* encodedSp = miscUrlEncode(sp);
    const char* staticUri = "http://apps.jmmc.fr:8080/jmcs_ws/ld2ud.jsp?ld=1.0&sptype=%s";
    int composedUriLength = strlen(staticUri)-2 + strlen(encodedSp)+1;
    char* composedUri = (char*) malloc(composedUriLength * sizeof(char));
    if (composedUri == NULL)
    {
        return mcsFAILURE;
    }
    snprintf(composedUri, composedUriLength, staticUri, encodedSp);
    free(encodedSp);
    
    /* Call the web service (10 seconds timeout) */
    mcsCOMPL_STAT executionStatus = miscPerformHttpGet(composedUri, &resultBuffer, 10);

    /* Give back local dynamically-allocated memory */
    free(composedUri);
    if (executionStatus == mcsFAILURE)
    {
      return mcsFAILURE;
    }

    /* Remove any trailing or leading '\n' */
    miscTrimString(miscDynBufGetBuffer(&resultBuffer), "\n");

    /* Parsing each line that does not start with '#' */
    miscDynBufSetCommentPattern(&resultBuffer, "#");
    
    mcsCOMPL_STAT parsingWentFine = mcsSUCCESS;
    const char* index = NULL;
    mcsSTRING256 currentLine;
    const mcsUINT32 lineSize = sizeof(currentLine);
    
    while ((index = miscDynBufGetNextLine(&resultBuffer,
                                           index,
                                           currentLine,
                                           lineSize,
                                           mcsTRUE)) != NULL)
    {
        logTrace("miscDynBufGetNextLine() = '%s'", currentLine);

        char band='0';
        mcsDOUBLE value = FP_NAN;

        /* Try to read effective temperature */
        if (sscanf(currentLine, "TEFF=%lf", &value) == 1)
        {
            *Teff = value;
            continue;
        }
        /* Try to read surface gravity */
        else if (sscanf(currentLine, "LOGG=%lf", &value) == 1)
        {
            *LogG = value;
            continue;
        }
        else if (sscanf(currentLine, "UD_%c=%lf", &band, &value) == 2)
        {
	  continue;
	}
        /* Could not parse current token - stop and exit on failure */
        parsingWentFine = mcsFAILURE;
        break;
    }

    /* Parsing went fine all along */
    miscDynBufDestroy(&resultBuffer);
    return parsingWentFine;
}

/**
 * Compute uniform diameters from limb-darkened diamter and spectral type.
 *
 * @param ld limb-darkened diameter (milli arcseconds)
 * @param sp spectral type
 * @param ud output uniform diameters (milli arcseconds)
 *
 * @warning ud will be flushed on each call.
 * @sa alxFlushUNIFORM_DIAMETERS()
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

    /* Dynamic buffer initializaton */
    miscDYN_BUF resultBuffer;
    if (miscDynBufInit(&resultBuffer) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    /* Forge URI using hard coded URL 
     * (sptype is encoded because it can embed spaces or special characters) */
    char* encodedSp = miscUrlEncode(sp);
    const char* staticUri = "http://apps.jmmc.fr:8080/jmcs_ws/ld2ud.jsp?ld=%f&sptype=%s";
    int composedUriLength = strlen(staticUri) + strlen(encodedSp) + 10 + 1;
    char* composedUri = (char*) malloc(composedUriLength * sizeof(char));
    if (composedUri == NULL)
    {
        return mcsFAILURE;
    }
    snprintf(composedUri, composedUriLength, staticUri, ld, encodedSp);
    free(encodedSp);
    
    /* Call the web service (10 seconds timeout) */
    mcsCOMPL_STAT executionStatus = miscPerformHttpGet(composedUri, &resultBuffer, 10);

    /* Give back local dynamically-allocated memory */
    free(composedUri);
    if (executionStatus == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    /* Remove any trailing or leading '\n' */
    miscTrimString(miscDynBufGetBuffer(&resultBuffer), "\n");

    /* Flush output structure before use */
    if (alxFlushUNIFORM_DIAMETERS(ud) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    /* Parsing each line that does not start with '#' */
    miscDynBufSetCommentPattern(&resultBuffer, "#");
    mcsCOMPL_STAT parsingWentFine = mcsSUCCESS;
    const char* index = NULL;
    mcsSTRING256 currentLine;
    const mcsUINT32 lineSize = sizeof(currentLine);
    while ((index = miscDynBufGetNextLine(&resultBuffer,
                                           index,
                                           currentLine,
                                           lineSize,
                                           mcsTRUE)) != NULL)
    {
        logTrace("miscDynBufGetNextLine() = '%s'", currentLine);

        char band = '0';
        mcsDOUBLE value = FP_NAN;

        /* Try to read effective temperature */
        if (sscanf(currentLine, "TEFF=%lf", &value) == 1)
        {
            ud->Teff = value;
            logTest("Teff = %f", value);
            continue;
        }
        /* Try to read surface gravity */
        else if (sscanf(currentLine, "LOGG=%lf", &value) == 1)
        {
            ud->LogG = value;
            logTest("LogG = %f", value);
            continue;
        }
        /* Try to read uniform diameter */
        else if (sscanf(currentLine, "UD_%c=%lf", &band, &value) == 2)
        {
            switch (tolower(band))
            {
                case 'b':
                    logTest("UD_B = %f", value);
                    ud->b = value;
                    break;
    
                case 'i':
                    logTest("UD_I = %f", value);
                    ud->i = value;
                    break;
    
                case 'j':
                    logTest("UD_J = %f", value);
                    ud->j = value;
                    break;
    
                case 'h':
                    logTest("UD_H = %f", value);
                    ud->h = value;
                    break;
    
                case 'k':
                    logTest("UD_K = %f", value);
                    ud->k = value;
                    break;
    
                case 'l':
                    logTest("UD_L = %f", value);
                    ud->l = value;
                    break;
    
                case 'n':
                    logTest("UD_N = %f", value);
                    ud->n = value;
                    break;
    
                case 'r':
                    logTest("UD_R = %f", value);
                    ud->r = value;
                    break;
    
                case 'u':
                    logTest("UD_U = %f", value);
                    ud->u = value;
                    break;
    
                case 'v':
                    logTest("UD_V = %f", value);
                    ud->v = value;
                    break;
    
                default:
                    logWarning("Unknown band '%c'.\n", band);
                    break;
            }
            continue;
        }

        /* Could not parse current token - stop and exit on failure */
        parsingWentFine = mcsFAILURE;
        break;
    }

    /* Parsing went fine all along */
    miscDynBufDestroy(&resultBuffer);
    return parsingWentFine;
}

/**
 * Log content of an alxUNIFORM_DIAMETERS structure on STDOUT.
 *
 * @param ud uniform diameters to log.
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is
 * returned.
 */
mcsCOMPL_STAT alxShowUNIFORM_DIAMETERS(const alxUNIFORM_DIAMETERS* ud)
{
    logTrace("alxShowUNIFORM_DIAMETERS()");

    /* Check parameter validity */
    if (ud == NULL)
    {
        errAdd(alxERR_NULL_PARAMETER, "ud");
        return mcsFAILURE;
    }

    printf("alxUNIFORM_DIAMETERS structure contains:\n");
    printf("\tud.Teff = %lf\n", ud->Teff);
    printf("\tud.LogG = %lf\n", ud->LogG);
    printf("\tud.b    = %lf\n", ud->b);
    printf("\tud.i    = %lf\n", ud->i);
    printf("\tud.j    = %lf\n", ud->j);
    printf("\tud.h    = %lf\n", ud->h);
    printf("\tud.k    = %lf\n", ud->k);
    printf("\tud.l    = %lf\n", ud->l);
    printf("\tud.n    = %lf\n", ud->n);
    printf("\tud.r    = %lf\n", ud->r);
    printf("\tud.u    = %lf\n", ud->u);
    printf("\tud.v    = %lf\n", ud->v);

    return mcsSUCCESS;
}

/**
 * Flush content of an alxUNIFORM_DIAMETERS structure.
 *
 * @param ud uniform diameters to flush.
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is
 * returned.
 */
mcsCOMPL_STAT alxFlushUNIFORM_DIAMETERS(alxUNIFORM_DIAMETERS* ud)
{
    logTrace("alxFlushUNIFORM_DIAMETERS()");

    /* Check parameter validity */
    if (ud == NULL)
    {
        errAdd(alxERR_NULL_PARAMETER, "ud");
        return mcsFAILURE;
    }

    ud->Teff = FP_NAN;
    ud->LogG = FP_NAN;

    ud->b = FP_NAN;
    ud->i = FP_NAN;
    ud->j = FP_NAN;
    ud->h = FP_NAN;
    ud->k = FP_NAN;
    ud->l = FP_NAN;
    ud->n = FP_NAN;
    ud->r = FP_NAN;
    ud->u = FP_NAN;
    ud->v = FP_NAN;

    return mcsSUCCESS;
}

/*___oOo___*/
