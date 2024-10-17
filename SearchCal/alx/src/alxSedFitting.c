/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * @file
 * Function definition for SED fitting
 *
 * @sa JMMC-MEM-2600-0009 document.
 */


/*
 * System Headers
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
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
#include "alxPrivate.h"
#include "alxErrors.h"

/*
 * SED models
 */
#define alxNB_SED_MODEL 180600

/*
 * Structure of the an coefficient table for compute angular diameter
 */
typedef struct
{
    mcsLOGICAL loaded;
    char* fileName;
    mcsDOUBLE Teff[alxNB_SED_MODEL];
    mcsDOUBLE Logg[alxNB_SED_MODEL];
    mcsDOUBLE Av[alxNB_SED_MODEL];
    mcsDOUBLE Flux[alxNB_SED_MODEL][alxNB_SED_BAND];
} alxSED_MODEL;

/*
 * Local Functions declaration
 */
static alxSED_MODEL *alxGetSedModel(void);

/*
 * Public Functions Definition
 */

/**
 * Compute fit of the Spectral Energy Distribution B V J H Ks.
 *
 * The diameter and the chi2 is computed for all models.
 * The best fit is returned (bestDiam, bestChi2, bestTeff, bestAv)
 * The uncertainty on the diameter is estimated by exploring
 * all models satisfying   chi2 < ( bestChi2 + 2 ).
 *
 * Note that the ZeroPoint of the spectral band and their typical error
 * are hardcoded: it is mandatory to use Johnson and 2MASS photometry.
 *
 * The fit not performed if only one or two photometries are available.
 *
 * @return SUCESS
 *
 * @usedfiles alxSedModel.cfg
 * This file contains the flux in the bands for the Kurucz models
 * of temperature from 3500K to 40000K, for classes I, III and V,
 * and for Av from 0 to 2.
 * Fluxes are given in W/m2/m in B,V,J,H,Ks (Johnson/2MASS).
 */
mcsCOMPL_STAT alxSedFitting(alxDATA *magnitudes, mcsDOUBLE Av, mcsDOUBLE e_Av,
                            mcsDOUBLE *bestDiam, mcsDOUBLE *bestDiamError,
                            mcsDOUBLE *bestChi2, mcsDOUBLE *bestTeff,  mcsDOUBLE *bestAv)
{
    /* Get the SED of the models */
    alxSED_MODEL *sedModel;
    sedModel = alxGetSedModel();
    FAIL_NULL(sedModel);

    static mcsDOUBLE fluxRef = 2.06265e+08;

    /* Fill the zero points Bj, Vj, J2mass, H2mass, Ks2mass.
       ZP in W/m2/m and relative error are hardcoded */
    static mcsDOUBLE zeroPoint[alxNB_SED_BAND] = {0.0630823, 0.0361871, 0.00313311, 0.00111137, 0.000428856};

    /* Convert magnitudes into fluxes. Maybe this could go on the sclsvr side */
    mcsDOUBLE fluxData = 0.0, fluxErr = 0.0;
    mcsUINT32 i, n, b, bestModelIndex = 0, bestResultIndex = 0, nbFree = 0;

    /* fast arrays (traversal) */
    mcsDOUBLE mag[alxNB_SED_BAND], invMagErr[alxNB_SED_BAND];
    mcsUINT32 bandIdx[alxNB_SED_BAND];

    for (b = 0; b < alxNB_SED_BAND; b++)
    {
        if (alxIsSet(magnitudes[b]))
        {
            /* fill fast arrays */
            /* Fluxes (W/m2/m). */
            mag[nbFree] = zeroPoint[b] * alxPow10(-0.4 * magnitudes[b].value);

            /* Compute the variance (sig2) of flux */
            fluxErr = 1.0 - alxPow10(-0.4 * magnitudes[b].error);

            /* logDebug("flux= %.3lf pm (%.1lf%%) (W/m2/m)", mag[nbFree], fluxErr * 100.0); */

            invMagErr[nbFree] = fluxErr * mag[nbFree];
            /* store inverse of flux variance */
            invMagErr[nbFree] = 1.0 / (invMagErr[nbFree] * invMagErr[nbFree]);

            /* Total flux weighted by variance */
            fluxData += mag[nbFree] * invMagErr[nbFree];

            bandIdx[nbFree] = b;

            nbFree++;
        }
    }

    /* Check the number of available magnitudes */
    if (nbFree < 3)
    {
        logInfo("Skip SED fitting (%d less than 3 magnitudes available)", nbFree);
        return mcsFAILURE;
    }

    /* Build the map of chi2. (large arrays but only small part used) */
    mcsDOUBLE mapChi2[alxNB_SED_MODEL], mapFluxRatio[alxNB_SED_MODEL];
    mcsDOUBLE fluxModel, fluxRatio, diffDataModel, chi2, best_chi2, chi2Threshold;
    mcsDOUBLE *ptrFlux;

    const mcsLOGICAL hasAv   = (e_Av > 0.0) ? mcsTRUE : mcsFALSE;
    const mcsDOUBLE invAvErr = (IS_TRUE(hasAv)) ? 1.0 / e_Av : NAN;
    best_chi2 = 1e99;

    /* Optimization: only keep data (chi2, flux ratio) which chi2 < best(chi2) + 2 */
    chi2Threshold = best_chi2 + 2.0; /* moving threshold */

    /* Loop on models */
    for (n = 0, i = 0; i < alxNB_SED_MODEL; i++)
    {
        ptrFlux = sedModel->Flux[i];

        /* Compute the flux of the model weighted by the variance */
        fluxModel = 0.0;
        for (b = 0; b < nbFree; b++)
        {
            fluxModel += ptrFlux[bandIdx[b]] * invMagErr[b];
        }
        /* adjust flux model with data */
        fluxRatio = fluxData / fluxModel;

        /* Compute chi2 for the photometry */
        chi2 = 0.0;
        for (b = 0; b < nbFree; b++)
        {
            diffDataModel = mag[b] - (fluxRatio * ptrFlux[bandIdx[b]]);
            chi2 += diffDataModel * diffDataModel * invMagErr[b];
        }

        if (IS_TRUE(hasAv))
        {
            /* Add the chi2 contribution of the Av */
            diffDataModel = (Av - sedModel->Av[i]) * invAvErr;
            chi2 += diffDataModel * diffDataModel;
        }

        /* Look for the best chi2 */
        if (chi2 <= best_chi2)
        {
            bestModelIndex = i;
            best_chi2      = chi2;
            chi2Threshold  = best_chi2 + 2.0; /* moving threshold */

            /* Keep chi2 and flux ratio and not the diameter (faster because less amount of data) */
            bestResultIndex = n;
            mapChi2[n]      = chi2;
            mapFluxRatio[n] = fluxRatio;
            n++;
        }
        else if (chi2 <= chi2Threshold)
        {
            /* Keep chi2 and flux ratio and not the diameter (faster because less amount of data) */
            mapChi2[n]      = chi2;
            mapFluxRatio[n] = fluxRatio;
            n++;
        }
    }
    /* End loop on models */

    /* Found the parameter of the best fitting model */
    *bestTeff = sedModel->Teff[bestModelIndex];
    *bestAv   = sedModel->Av[bestModelIndex];

    /* Compute uncertainty on bestDiam as the peek to peek of all the models that fit the data within 2 sigma */
    mcsDOUBLE minFluxRatio, maxFluxRatio;
    maxFluxRatio = 0.0;
    minFluxRatio = 1e99;

    for (i = 0; i < n; i++)
    {
        if (mapChi2[i] <= chi2Threshold)
        {
            if (mapFluxRatio[i] > maxFluxRatio)
            {
                maxFluxRatio = mapFluxRatio[i];
            }
            if (mapFluxRatio[i] < minFluxRatio)
            {
                minFluxRatio = mapFluxRatio[i];
            }
        }
    }

    /* Compute the apparent diameter in mas */
    /* Convert flux ratio into diameter at the end (faster) */
    *bestDiam  = fluxRef * sqrt(mapFluxRatio[bestResultIndex]);
    mcsDOUBLE upperDiam = fluxRef * sqrt(maxFluxRatio);
    mcsDOUBLE lowerDiam = fluxRef * sqrt(minFluxRatio);

    /* Compute error as the maximum distance */
    *bestDiamError = alxMax(fabs(upperDiam - *bestDiam), fabs(*bestDiam - lowerDiam));

    /* Compute reduced chi2 */
    *bestChi2 = best_chi2 / (nbFree - 2.0);

    logInfo("SED fitting: chi2=%.3lf with diam=%.3lf(%.5lf) av=%.3lf", *bestChi2, *bestDiam, *bestDiamError, *bestAv);

    return mcsSUCCESS;
}

/*
 * Private Functions Definition
 */

static alxSED_MODEL * alxGetSedModel(void)
{
    static alxSED_MODEL sedModel = {mcsFALSE, "alxSedModel.cfg",
        {0.0},
        {0.0},
        {0.0},
        {
            {0.0}
        }};

    /* Check if the structure is loaded into memory. If not load it. */
    if (IS_TRUE(sedModel.loaded))
    {
        return &sedModel;
    }

    /* Find the location of the file */
    char* fileName;
    fileName = miscLocateFile(sedModel.fileName);
    if (IS_NULL(fileName))
    {
        return NULL;
    }

    /* Load file. Comment lines start with '#' */
    miscDYN_BUF dynBuf;
    NULL_(miscDynBufInit(&dynBuf));

    logInfo("Loading %s ...", fileName);

    NULL_DO(miscDynBufLoadFile(&dynBuf, fileName, "#"),
            miscDynBufDestroy(&dynBuf);
            free(fileName));

    /* For each line of the loaded file */
    mcsINT32 lineNum = 0;
    const char* pos = NULL;
    mcsSTRING1024 line;

    while (IS_NOT_NULL(pos = miscDynBufGetNextLine(&dynBuf, pos, line, sizeof (line), mcsTRUE)))
    {
        logTrace("miscDynBufGetNextLine() = '%s'", line);

        /* If the current line is not empty */
        if (IS_FALSE(miscIsSpaceStr(line)))
        {
            /* Check if there is too many lines in file */
            if (lineNum >= alxNB_SED_MODEL)
            {
                miscDynBufDestroy(&dynBuf);
                errAdd(alxERR_TOO_MANY_LINES, fileName);
                free(fileName);
                return NULL;
            }

            /* Read polynomial coefficients */
            if (sscanf(line, "%lf %lf %lf %lf %lf %lf %lf %lf",
                       &sedModel.Logg[lineNum],
                       &sedModel.Teff[lineNum],
                       &sedModel.Av[lineNum],
                       &sedModel.Flux[lineNum][0],
                       &sedModel.Flux[lineNum][1],
                       &sedModel.Flux[lineNum][2],
                       &sedModel.Flux[lineNum][3],
                       &sedModel.Flux[lineNum][4]
                       ) != (alxNB_SED_BAND + 3))
            {
                miscDynBufDestroy(&dynBuf);
                errAdd(alxERR_WRONG_FILE_FORMAT, line, fileName);
                free(fileName);
                return NULL;
            }

            /* Log what has been read */
            logDebug("%lf %lf %lf - %lf %lf %lf %lf %lf",
                     sedModel.Logg[lineNum], sedModel.Teff[lineNum], sedModel.Av[lineNum],
                     sedModel.Flux[lineNum][0],
                     sedModel.Flux[lineNum][1],
                     sedModel.Flux[lineNum][2],
                     sedModel.Flux[lineNum][3],
                     sedModel.Flux[lineNum][4]);

            /* Next line */
            lineNum++;
        }
    }

    /* Destroy the dynamic buffer where is stored the file information */
    miscDynBufDestroy(&dynBuf);

    free(fileName);

    /* Specify that the models have been loaded */
    sedModel.loaded = mcsTRUE;

    return &sedModel;
}

/**
 * Initialize this file
 */
void alxSedFittingInit(void)
{
    alxGetSedModel();
}
