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
#define  _DEFAULT_SOURCE 1

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
alxUD_NEW_CORRECTION_TABLE* alxGetNewUDTable();
mcsUINT32 alxGetLineForUd(alxUD_CORRECTION_TABLE *udTable,
                          mcsDOUBLE teff,
                          mcsDOUBLE logg);

/*
 * Private functions definition
 */

alxUD_CORRECTION_TABLE* alxGetUDTable()
{
    static alxUD_CORRECTION_TABLE udTable = {mcsFALSE, "alxTableUDCoefficientCorrection.cfg", 0,
                                             {0.0},
        {0.0},
        {
            {0.0}
        }};

    /* Check if the structure is loaded into memory. If not load it. */
    if (IS_TRUE(udTable.loaded))
    {
        return &udTable;
    }

    /* Find the location of the file */
    char* fileName = miscLocateFile(udTable.fileName);
    if (IS_NULL(fileName))
    {
        return NULL;
    }

    /* Load file (skipping comment lines starting with '#') */
    miscDYN_BUF dynBuf;
    NULL_(miscDynBufInit(&dynBuf));

    logInfo("Loading %s ...", fileName);

    NULL_DO(miscDynBufLoadFile(&dynBuf, fileName, "#"),
            miscDynBufDestroy(&dynBuf);
            free(fileName));

    /* For each line of the loaded file */
    mcsINT32 lineNum = 0;
    const char *pos = NULL;
    mcsSTRING1024 line;
    const mcsUINT32 maxLineLength = sizeof (line) - 1;

    while (IS_NOT_NULL(pos = miscDynBufGetNextLine(&dynBuf, pos, line, maxLineLength, mcsTRUE)))
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

alxUD_NEW_CORRECTION_TABLE* alxGetNewUDTable()
{
    static alxUD_NEW_CORRECTION_TABLE udTable = {mcsFALSE, "alxNewTableUDCoefficientCorrection.cfg", 0,
                                                 {
            {0.0}},
        {
            {0.0}
        },
        {
            {0.0}
        },
        {
            {
                {0.0}
            }
        },
        {
            {
                {0.0}
            }
        }};

    /* Check if the structure is loaded into memory. If not load it. */
    if (IS_TRUE(udTable.loaded))
    {
        return &udTable;
    }

    /* Find the location of the file */
    char* fileName = miscLocateFile(udTable.fileName);
    if (IS_NULL(fileName))
    {
        return NULL;
    }

    /* Load file (skipping comment lines starting with '#') */
    miscDYN_BUF dynBuf;
    NULL_(miscDynBufInit(&dynBuf));

    logInfo("Loading %s ...", fileName);

    NULL_DO(miscDynBufLoadFile(&dynBuf, fileName, "#"),
            miscDynBufDestroy(&dynBuf);
            free(fileName));

    /* For each line of the loaded file */
    mcsINT32 lineNum = 0;
    const char *pos = NULL;
    mcsSTRING1024 line;
    const mcsUINT32 maxLineLength = sizeof (line) - 1;

    while (IS_NOT_NULL(pos = miscDynBufGetNextLine(&dynBuf, pos, line, maxLineLength, mcsTRUE)))
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
            mcsINT32 nbOfReadTokens = sscanf((line + 5), "%lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf "
                                             "%lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf",

                                             &udTable.teff[lineNum][0],
                                             &udTable.logg[lineNum][0],
                                             &udTable.mass[lineNum][0],
                                             &udTable.teff[lineNum][1],
                                             &udTable.logg[lineNum][1],
                                             &udTable.mass[lineNum][1],
                                             &udTable.teff[lineNum][2],
                                             &udTable.logg[lineNum][2],
                                             &udTable.mass[lineNum][2],
                                             &udTable.coeffNL[lineNum][alxB][0],
                                             &udTable.coeffNL[lineNum][alxV][0],
                                             &udTable.coeffNL[lineNum][alxR][0],
                                             &udTable.coeffNL[lineNum][alxI][0],
                                             &udTable.coeffNL[lineNum][alxH][0],
                                             &udTable.coeffNL[lineNum][alxK][0],
                                             &udTable.coeffNL[lineNum][alxB][1],
                                             &udTable.coeffNL[lineNum][alxV][1],
                                             &udTable.coeffNL[lineNum][alxR][1],
                                             &udTable.coeffNL[lineNum][alxI][1],
                                             &udTable.coeffNL[lineNum][alxH][1],
                                             &udTable.coeffNL[lineNum][alxK][1],
                                             &udTable.coeffCL[lineNum][alxU][0],
                                             &udTable.coeffCL[lineNum][alxB][0],
                                             &udTable.coeffCL[lineNum][alxV][0],
                                             &udTable.coeffCL[lineNum][alxR][0],
                                             &udTable.coeffCL[lineNum][alxI][0],
                                             &udTable.coeffCL[lineNum][alxJ][0],
                                             &udTable.coeffCL[lineNum][alxH][0],
                                             &udTable.coeffCL[lineNum][alxK][0],
                                             &udTable.coeffCL[lineNum][alxL][0],
                                             &udTable.coeffCL[lineNum][alxM][0],
                                             &udTable.coeffCL[lineNum][alxN][0],
                                             &udTable.coeffCL[lineNum][alxU][1],
                                             &udTable.coeffCL[lineNum][alxB][1],
                                             &udTable.coeffCL[lineNum][alxV][1],
                                             &udTable.coeffCL[lineNum][alxR][1],
                                             &udTable.coeffCL[lineNum][alxI][1],
                                             &udTable.coeffCL[lineNum][alxJ][1],
                                             &udTable.coeffCL[lineNum][alxH][1],
                                             &udTable.coeffCL[lineNum][alxK][1],
                                             &udTable.coeffCL[lineNum][alxL][1],
                                             &udTable.coeffCL[lineNum][alxM][1],
                                             &udTable.coeffCL[lineNum][alxN][1]);

            /* If parsing went wrong */
            if (nbOfReadTokens != 43)
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

    /* fill some gaps: U=B / J=H */
    mcsINT32 i = 0;
    for (i = 0; i < lineNum; ++i)
    {
        udTable.coeffNL[i][alxU][0] = udTable.coeffNL[i][alxB][0];
        udTable.coeffNL[i][alxJ][0] = udTable.coeffNL[i][alxH][0];
        udTable.coeffNL[i][alxU][1] = udTable.coeffNL[i][alxB][1];
        udTable.coeffNL[i][alxJ][1] = udTable.coeffNL[i][alxH][1];
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
    FAIL_NULL_DO(ud, 
                 errAdd(alxERR_NULL_PARAMETER, "ud"));

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
 * Compute uniform diameters from limb-darkened diameter and spectral type.
 *
 * @param ld limb-darkened diameter (milli arcseconds)
 * @param sptype_index the index corresponding to sptype, by increments of 0.25 subtype, starting at 0= sptype("O0.00")
 * @param lumclass the index corresponding to luminosity class [I,II,II,1V,V...]
 * @param ud output uniform diameters (milli arcseconds)
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
 */
mcsCOMPL_STAT alxComputeNewUDFromLDAndSP(const mcsDOUBLE ld,
                                         const mcsINT32 sptype_index,
                                         mcsINT32 lumclass,
                                         alxUNIFORM_DIAMETERS *ud)
{
    FAIL_NULL_DO(ud, 
                 errAdd(alxERR_NULL_PARAMETER, "ud"));

    /* Flush output structure before use */
    FAIL(alxFlushUNIFORM_DIAMETERS(ud));

    alxUD_NEW_CORRECTION_TABLE* udTable = alxGetNewUDTable();
    FAIL_NULL(udTable);

    if ((sptype_index < 0) || (sptype_index >= alxNB_COLORINDEX_ENTRIES))
    {
        logTest("Invalid sptype_index [%d]", sptype_index);
        return mcsSUCCESS;
    }

    if (lumclass <= 0)
    {
        lumclass = 5;
    }

    mcsINT32 classindex = (lumclass <= 3) ? 1 : 0; /* giants etc: 3 and all others dwarf */
    mcsDOUBLE coefftest = udTable->coeffNL[sptype_index][alxB][classindex];
    if (coefftest > 0.0) /* exist in Neilson & Leister */
    {
        ud->u = ld * udTable->coeffNL[sptype_index][alxU][classindex];
        ud->b = ld * udTable->coeffNL[sptype_index][alxB][classindex];
        ud->v = ld * udTable->coeffNL[sptype_index][alxV][classindex];
        ud->r = ld * udTable->coeffNL[sptype_index][alxR][classindex];
        ud->i = ld * udTable->coeffNL[sptype_index][alxI][classindex];
        ud->j = ld * udTable->coeffNL[sptype_index][alxJ][classindex];
        ud->h = ld * udTable->coeffNL[sptype_index][alxH][classindex];
        ud->k = ld * udTable->coeffNL[sptype_index][alxK][classindex];
    }
    else
    {
        coefftest = udTable->coeffCL[sptype_index][alxB][classindex];
        if (coefftest < 0.0)
        {
            return mcsSUCCESS;
        }
        ud->u = ld / computeRho(udTable->coeffCL[sptype_index][alxU][classindex]);
        ud->b = ld / computeRho(udTable->coeffCL[sptype_index][alxB][classindex]);
        ud->v = ld / computeRho(udTable->coeffCL[sptype_index][alxV][classindex]);
        ud->r = ld / computeRho(udTable->coeffCL[sptype_index][alxR][classindex]);
        ud->i = ld / computeRho(udTable->coeffCL[sptype_index][alxI][classindex]);
        ud->j = ld / computeRho(udTable->coeffCL[sptype_index][alxJ][classindex]);
        ud->h = ld / computeRho(udTable->coeffCL[sptype_index][alxH][classindex]);
        ud->k = ld / computeRho(udTable->coeffCL[sptype_index][alxK][classindex]);
    }
    coefftest =  udTable->coeffCL[sptype_index][alxL][classindex];
    if (coefftest > 0.0)
    {
        ud->l = ld / computeRho(udTable->coeffCL[sptype_index][alxL][classindex]);
        ud->m = ld / computeRho(udTable->coeffCL[sptype_index][alxM][classindex]);
        ud->n = ld / computeRho(udTable->coeffCL[sptype_index][alxN][classindex]);
    }
    ud->Teff = udTable->teff[sptype_index][classindex];
    ud->LogG = udTable->logg[sptype_index][classindex];

    /* Print results */
    logTest("Computed UD: U=%.4lf B=%.4lf V=%.4lf R=%.4lf I=%.4lf J=%.4lf H=%.4lf K=%.4lf L=%.4lf M=%.4lf N=%.4lf",
            ud->u, ud->b, ud->v, ud->r, ud->i, ud->j, ud->h, ud->k, ud->l, ud->m, ud->n);

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
    printf("\tud.u    = %lf\n", ud->u);
    printf("\tud.b    = %lf\n", ud->b);
    printf("\tud.v    = %lf\n", ud->v);
    printf("\tud.r    = %lf\n", ud->r);
    printf("\tud.i    = %lf\n", ud->i);
    printf("\tud.j    = %lf\n", ud->j);
    printf("\tud.h    = %lf\n", ud->h);
    printf("\tud.k    = %lf\n", ud->k);
    printf("\tud.l    = %lf\n", ud->l);
    printf("\tud.m    = %lf\n", ud->m);
    printf("\tud.n    = %lf\n", ud->n);

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
    FAIL_NULL_DO(ud, 
                 errAdd(alxERR_NULL_PARAMETER, "ud"));

    ud->Teff = ud->LogG = ud->u = ud->b = ud->v = ud->r = ud->i = ud->j = ud->h = ud->k = ud->l = ud->m = ud->n = NAN;

    return mcsSUCCESS;
}

/**
 * Initialize this file
 */
void alxLD2UDInit(void)
{
    alxGetUDTable();
    alxGetNewUDTable();
}

/*___oOo___*/
