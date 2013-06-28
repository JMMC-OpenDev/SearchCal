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
 * Private functions declaration
 */

alxUD_CORRECTION_TABLE* alxGetUDTable();

mcsINT32 alxGetLineForUd(alxUD_CORRECTION_TABLE *udTable,
                         mcsDOUBLE teff,
                         mcsDOUBLE logg);

/*
 * Private functions definition
 */

alxUD_CORRECTION_TABLE* alxGetUDTable()
{
    static alxUD_CORRECTION_TABLE udTable = {mcsFALSE, "alxTableUDCoefficientCorrection.cfg"};

    if (isTrue(udTable.loaded))
    {
        return &udTable;
    }

    /* Find the location of the file */
    char* fileName = miscLocateFile(udTable.fileName);
    if (isNull(fileName))
    {
        return NULL;
    }

    /* Load file (skipping comment lines starting with '#') */
    miscDYN_BUF dynBuf;
    miscDynBufInit(&dynBuf);

    logInfo("Loading %s ...", fileName);

    NULL_DO(miscDynBufLoadFile(&dynBuf, fileName, "#"),
            miscDynBufDestroy(&dynBuf);
            free(fileName));

    /* For each line of the loaded file */
    mcsINT32 lineNum = 0;
    const char *pos = NULL;
    mcsSTRING1024 line;

    while (isNotNull(pos = miscDynBufGetNextLine(&dynBuf, pos, line, sizeof (line), mcsTRUE)))
    {
        logTrace("miscDynBufGetNextLine() = '%s'", line);

        /* Trim line for any leading and trailing blank characters */
        miscTrimString(line, " ");

        /* If line is not empty */
        if (strlen(line) != 0)
        {
            /* Check if there are to many lines in file */
            if (lineNum >= alxNB_UD_ENTRIES)
            {
                /* Destroy the temporary dynamic buffer, raise an error and return */
                miscDynBufDestroy(&dynBuf);
                errAdd(alxERR_TOO_MANY_LINES, fileName);
                free(fileName);
                return NULL;
            }

            /* Try to read each polynomial coefficients */
            mcsINT32 nbOfReadTokens = sscanf(line, "%lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf",
                                             &udTable.logg[lineNum],
                                             &udTable.teff[lineNum],
                                             &udTable.coeff[lineNum][alxU],
                                             &udTable.coeff[lineNum][alxB],
                                             &udTable.coeff[lineNum][alxV],
                                             &udTable.coeff[lineNum][alxR],
                                             &udTable.coeff[lineNum][alxI],
                                             &udTable.coeff[lineNum][alxJ],
                                             &udTable.coeff[lineNum][alxH],
                                             &udTable.coeff[lineNum][alxK],
                                             &udTable.coeff[lineNum][alxL],
                                             &udTable.coeff[lineNum][alxN]);

            /* If parsing went wrong */
            if (nbOfReadTokens != (alxNBUD_BANDS + 2))
            {
                /* Destroy the temporary dynamic buffer, raise an error and return */
                miscDynBufDestroy(&dynBuf);
                errAdd(alxERR_WRONG_FILE_FORMAT, line, fileName);
                free(fileName);
                return NULL;
            }

            /* Next line */
            lineNum++;
        }
    }

    /* Set the total number of lines in the ud table */
    udTable.nbLines = lineNum;

    /* Mark the ud table as "loaded" */
    udTable.loaded = mcsTRUE;

    /* Destroy the temporary dynamic buffer used to parse the ud table file */
    miscDynBufDestroy(&dynBuf);
    free(fileName);

    /* Return a pointer on the freshly loaded  ud table */
    return &udTable;
}

mcsINT32 alxGetLineForUd(alxUD_CORRECTION_TABLE *udTable,
                         mcsDOUBLE teff,
                         mcsDOUBLE logg)
{
    mcsINT32 line = 0;
    mcsDOUBLE *distToUd = malloc(alxNB_UD_ENTRIES * sizeof (mcsDOUBLE));

    distToUd[0] = sqrt(pow(teff - udTable->teff[0], 2.0) + pow(logg - udTable->logg[0], 2.0));

    int i;
    for (i = 1; i < udTable->nbLines; i++)
    {
        distToUd[i] = sqrt(pow(teff - udTable->teff[i], 2.0) + pow(logg - udTable->logg[i], 2.0));

        if (distToUd[i] < distToUd[line])
        {
            line = i;
        }
    }

    free(distToUd);
    /* return the line found */
    return line;
}

/*
 * Public functions definition
 */

/**
 * Compute uniform diameters from limb-darkened diameter and spectral type.
 *
 * @param ld limb-darkened diameter (milli arcseconds)
 * @param sp spectral type
 * @param ud output uniform diameters (milli arcseconds)
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is
 * returned.
 */
mcsCOMPL_STAT alxComputeUDFromLDAndSP(const mcsDOUBLE ld,
                                      const mcsDOUBLE teff,
                                      const mcsDOUBLE logg,
                                      alxUNIFORM_DIAMETERS *ud)
{
    FAIL_NULL_DO(ud, errAdd(alxERR_NULL_PARAMETER, "ud"));

    /* Flush output structure before use */
    FAIL(alxFlushUNIFORM_DIAMETERS(ud));

    alxUD_CORRECTION_TABLE* udTable = alxGetUDTable();
    FAIL_NULL(udTable);

    ud->Teff = teff;
    ud->LogG = logg;

    mcsDOUBLE rho, value;
    mcsINT32 line = alxGetLineForUd(udTable, teff, logg);

    value = udTable->coeff[line][alxU];
    rho = sqrt((1.0 - value / 3.0) / (1.0 - 7.0 * value / 15.0));
    ud->u = ld / rho;

    value = udTable->coeff[line][alxB];
    rho = sqrt((1.0 - value / 3.0) / (1.0 - 7.0 * value / 15.0));
    ud->b = ld / rho;

    value = udTable->coeff[line][alxV];
    rho = sqrt((1.0 - value / 3.0) / (1.0 - 7.0 * value / 15.0));
    ud->v = ld / rho;

    value = udTable->coeff[line][alxR];
    rho = sqrt((1.0 - value / 3.0) / (1.0 - 7.0 * value / 15.0));
    ud->r = ld / rho;

    value = udTable->coeff[line][alxI];
    rho = sqrt((1.0 - value / 3.0) / (1.0 - 7.0 * value / 15.0));
    ud->i = ld / rho;

    value = udTable->coeff[line][alxJ];
    rho = sqrt((1.0 - value / 3.0) / (1.0 - 7.0 * value / 15.0));
    ud->j = ld / rho;

    value = udTable->coeff[line][alxH];
    rho = sqrt((1.0 - value / 3.0) / (1.0 - 7.0 * value / 15.0));
    ud->h = ld / rho;

    value = udTable->coeff[line][alxK];
    rho = sqrt((1.0 - value / 3.0) / (1.0 - 7.0 * value / 15.0));
    ud->k = ld / rho;

    value = udTable->coeff[line][alxL];
    rho = sqrt((1.0 - value / 3.0) / (1.0 - 7.0 * value / 15.0));
    ud->l = ld / rho;

    value = udTable->coeff[line][alxN];
    rho = sqrt((1.0 - value / 3.0) / (1.0 - 7.0 * value / 15.0));
    ud->n = ld / rho;

    /* Print results */
    logTest("Computed UD: U=%lf B=%lf V=%lf R=%lf I=%lf J=%lf H=%lf K=%lf L=%lf N=%lf",
            ud->u, ud->b, ud->v, ud->r, ud->i, ud->j, ud->h, ud->k, ud->l, ud->n);

    return mcsSUCCESS;
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
    /* Check parameter validity */
    FAIL_NULL_DO(ud, errAdd(alxERR_NULL_PARAMETER, "ud"));

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
    /* Check parameter validity */
    FAIL_NULL_DO(ud, errAdd(alxERR_NULL_PARAMETER, "ud"));

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

/**
 * Initialize this file
 */
void alxLD2UDInit(void)
{
    alxGetUDTable();
}

/*___oOo___*/
