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

mcsUINT32 alxGetLineForUd(alxUD_CORRECTION_TABLE *udTable,
                          mcsDOUBLE teff,
                          mcsDOUBLE logg);

/*
 * Private functions definition
 */

alxUD_CORRECTION_TABLE* alxGetUDTable()
{
    static alxUD_CORRECTION_TABLE udTable = {mcsFALSE, "alxTableUDCoefficientCorrection.cfg"};

    /* Check if the structure is loaded into memory. If not load it. */
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
            mcsINT32 nbOfReadTokens = sscanf(line, "%lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf",
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
                                             &udTable.coeff[lineNum][alxM],
                                             &udTable.coeff[lineNum][alxN]);

            /* If parsing went wrong */
            if (nbOfReadTokens != (alxNB_UD_BANDS + 2))
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

mcsUINT32 alxGetLineForUd(alxUD_CORRECTION_TABLE *udTable,
                          mcsDOUBLE teff,
                          mcsDOUBLE logg)
{
    const mcsUINT32 len = udTable->nbLines;
    mcsUINT32 i = 0;
    mcsDOUBLE squareDistToUd    = alxSquare(teff - udTable->teff[i]) + alxSquare(logg - udTable->logg[i]);

    mcsUINT32 line = i;
    mcsDOUBLE minSquareDistToUd = squareDistToUd;

    for (i = 1; i < len; i++)
    {
        squareDistToUd = alxSquare(teff - udTable->teff[i]) + alxSquare(logg - udTable->logg[i]);

        if (squareDistToUd < minSquareDistToUd)
        {
            line = i;
            minSquareDistToUd = squareDistToUd;
        }
    }

    /* return the line found */
    return line;
}

mcsDOUBLE computeRho(mcsDOUBLE coeff)
{
    return sqrt((1.0 - coeff / 3.0) / (1.0 - 7.0 * coeff / 15.0));
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
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
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

    mcsINT32 line = alxGetLineForUd(udTable, teff, logg);

    const mcsDOUBLE* lineCoeffs = udTable->coeff[line];

    ud->u = ld / computeRho(lineCoeffs[alxU]);
    ud->b = ld / computeRho(lineCoeffs[alxB]);
    ud->v = ld / computeRho(lineCoeffs[alxV]);
    ud->r = ld / computeRho(lineCoeffs[alxR]);
    ud->i = ld / computeRho(lineCoeffs[alxI]);
    ud->j = ld / computeRho(lineCoeffs[alxJ]);
    ud->h = ld / computeRho(lineCoeffs[alxH]);
    ud->k = ld / computeRho(lineCoeffs[alxK]);
    ud->l = ld / computeRho(lineCoeffs[alxL]);
    ud->m = ld / computeRho(lineCoeffs[alxM]);
    ud->n = ld / computeRho(lineCoeffs[alxN]);

    /* Print results */
    logTest("Computed UD: U=%.4lf B=%.4lf V=%.4lf R=%.4lf I=%.4lf J=%.4lf H=%.4lf K=%.4lf L=%.4lf M=%.4lf N=%.4lf",
            ud->u, ud->b, ud->v, ud->r, ud->i, ud->j, ud->h, ud->k, ud->l, ud->m, ud->n);

    return mcsSUCCESS;
}

/**
 * Flush content of an alxUNIFORM_DIAMETERS structure.
 *
 * @param ud uniform diameters to flush.
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
 */
mcsCOMPL_STAT alxFlushUNIFORM_DIAMETERS(alxUNIFORM_DIAMETERS* ud)
{
    /* Check parameter validity */
    FAIL_NULL_DO(ud, errAdd(alxERR_NULL_PARAMETER, "ud"));

    ud->Teff = ud->LogG = ud->u = ud->b = ud->v = ud->r = ud->i = ud->j = ud->h = ud->k = ud->l = ud->m = ud->n = NAN;

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
