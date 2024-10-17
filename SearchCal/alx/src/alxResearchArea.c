/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * @file
 * Function definition for research area determination.
 *
 * @sa JMMC-MEM-2600-0005 document.
 */


/*
 * System Headers
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
 * Local Functions declaration
 */
static alxSTAR_POPULATION *alxGetStarPopulation(void);
static mcsCOMPL_STAT alxGetNbOfStars(mcsDOUBLE gLon,
                                     mcsDOUBLE gLat,
                                     mcsDOUBLE minMag,
                                     mcsDOUBLE maxMag,
                                     alxSTAR_POPULATION* starPopulation,
                                     mcsINT32* nbOfStars);


/*
 * Local functions definition
 */

/**
 * Return the structure containing the star population in terms of magnitude and
 * galactic coordinates.
 *
 * @return pointer to structure containing star population, or NULL if an error
 * occurred.
 *
 * @usedfiles alxStarPopulationInKBand.cfg : file containing the star population
 */
static alxSTAR_POPULATION *alxGetStarPopulation(void)
{
    /*
     * Check if the structure, where will be stored star population information,
     * is loaded into memory. If not loaded it.
     */
    static alxSTAR_POPULATION starPopulation = {mcsFALSE,
                                                "alxStarPopulationInKBand.cfg",
        {0.0, 10.0, 90.0, 180.0, 270.0, 360.0},
        {-90.0, -60.0, -30.0, -10.0, 0.0, 10.0, 30.0, 60.0, 90.0},
        {0.0},
        {
            {
                {0}
            }
        }};

    /* Check if the structure is loaded into memory. If not load it. */
    if (IS_TRUE(starPopulation.loaded))
    {
        return &starPopulation;
    }

    /*
     * Build the dynamic buffer which will contain the file of coefficient
     * of angular diameter
     */
    /* Find the location of the file */
    char *fileName = miscLocateFile(starPopulation.fileName);
    if (IS_NULL(fileName))
    {
        return NULL;
    }

    /* Load file where comment lines started with '#' */
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

    while (IS_NOT_NULL(pos = miscDynBufGetNextLine(&dynBuf, pos, line, sizeof (line), mcsTRUE)))
    {
        logTrace("miscDynBufGetNextLine() = '%s'", line);

        /* If line is not empty */
        miscTrimString(line, " ");
        if (strlen(line) != 0)
        {
            /* Check if there is to many lines in file */
            if (lineNum >= alxNB_MAG_STEPS)
            {
                miscDynBufDestroy(&dynBuf);
                errAdd(alxERR_TOO_MANY_LINES, fileName);
                free(fileName);
                return NULL;
            }

            /* Split line to get individual values */
            mcsUINT32 nbMaxSubStrings = 1 + alxNB_GLON_STEPS * alxNB_GLAT_STEPS;
            mcsSTRING256 subStrings[nbMaxSubStrings];
            mcsUINT32 nbFoundSubStrings;

            NULL_DO(miscSplitString(line, '\t', subStrings, nbMaxSubStrings, &nbFoundSubStrings),
                    errAdd(alxERR_TOO_MANY_COLUMS, fileName, lineNum + 1);
                    miscDynBufDestroy(&dynBuf);
                    free(fileName));

            /* Check there is enough values in line */
            if (nbFoundSubStrings != nbMaxSubStrings)
            {
                errAdd(alxERR_MISSING_COLUMS, fileName, lineNum + 1,
                       nbMaxSubStrings, nbMaxSubStrings);
                miscDynBufDestroy(&dynBuf);
                free(fileName);
                return NULL;
            }

            /* Store values in structure */
            /* Magnitude for this line */
            if (sscanf(subStrings[0], "%lf", &starPopulation.mag[lineNum]) != 1)
            {
                errAdd(alxERR_WRONG_FILE_FORMAT, line, fileName);
                miscDynBufDestroy(&dynBuf);
                free(fileName);
                return NULL;
            }

            /* and then the number of stars for each area of the sky */
            mcsINT32 gLonPos, gLatPos;
            mcsINT32 subStrIndex;
            for (gLonPos = 0; gLonPos < alxNB_GLON_STEPS; gLonPos++)
            {
                for (gLatPos = 0; gLatPos < alxNB_GLAT_STEPS; gLatPos++)
                {
                    subStrIndex = gLonPos * alxNB_GLAT_STEPS + gLatPos + 1;
                    if (sscanf(subStrings[subStrIndex], "%d",
                               &starPopulation.nbOfStars[lineNum][gLatPos][gLonPos]) != 1)
                    {
                        errAdd(alxERR_WRONG_FILE_FORMAT, line, fileName);
                        miscDynBufDestroy(&dynBuf);
                        free(fileName);
                        return NULL;
                    }
                }
            }
            /* Next line */
            lineNum++;
        }
    }

    /* Destroy the dynamic buffer where is stored the file information */
    miscDynBufDestroy(&dynBuf);
    free(fileName);

    starPopulation.loaded = mcsTRUE;

    return &starPopulation;
}

/**
 * Return the number of stars in the given sky area.
 *
 * This function determines the number of stars in the given magnitude range
 * around the galactic position into a 1 deg solid angle circle.
 *
 * @param gLon galatic longitude in degree
 * @param gLat galatic lattitude in degree
 * @param minMag minimum of the magnitude range
 * @param maxMag maximum of the magnitude range
 * @param starPopulation structure containing the star population regarding the
 * galactic coordinates and magnitude.
 * @param nbOfStars estimated number of stars at the given galactic coordinates
 *
 * @return the estimated number of stars at the given galactic coordinates.
 */
static mcsCOMPL_STAT alxGetNbOfStars(mcsDOUBLE gLon,
                                     mcsDOUBLE gLat,
                                     mcsDOUBLE minMag,
                                     mcsDOUBLE maxMag,
                                     alxSTAR_POPULATION* starPopulation,
                                     mcsINT32* nbOfStars)
{
    /* Find indexes corresponding to magnitude range */
    mcsINT32 minMagIdx, maxMagIdx;
    mcsINT32 idx;
    minMagIdx = -1;
    maxMagIdx = -1;

    for (idx = 0; idx < alxNB_MAG_STEPS; idx++)
    {
        /* If mag min index is not already set, and the current magnitude
         * is greater than the specified one. */
        if ((minMagIdx == -1) && (starPopulation->mag[idx] >= minMag))
        {
            /* Save index as min magnitude index */
            minMagIdx = idx;
        }
        /* If mag max index is not already set, and the current magnitude
         * is greater than the specified one. */
        if ((maxMagIdx == -1) && ((starPopulation->mag[idx] >= maxMag) || (idx == (alxNB_MAG_STEPS - 1))))
        {
            /* Save index as max magnitude index */
            maxMagIdx = idx;
        }
    }
    logTest("Magnitude [%.1lf - %.1lf], range used => [%.1lf - %.1lf]",
            minMag, maxMag, starPopulation->mag[minMagIdx],
            starPopulation->mag[maxMagIdx]);

    /* Find indexes for longitude and lattitude */
    mcsINT32 gLatIdx = 0;
    for (idx = 0; idx < (alxNB_GLAT_STEPS - 1); idx++)
    {
        /* If longitude is in the current interval. */
        if ((starPopulation->gLatList[idx] <= gLat) && (gLat < starPopulation->gLatList[idx + 1]))
        {
            /* Save index as lattitude index */
            gLatIdx = idx;
        }
    }
    if (doLog(logDEBUG))
    {
        logDebug("Lattitude is %.1lf, range used => [%.1lf - %.1lf]", gLat,
                 starPopulation->gLatList[gLatIdx],
                 starPopulation->gLatList[gLatIdx + 1]);
    }

    mcsINT32 gLonIdx = 0;
    for (idx = 0; idx < (alxNB_GLON_STEPS - 1); idx++)
    {
        /* If longitude is in the current interval. */
        if ((starPopulation->gLonList[idx] <= gLon) && (gLon < starPopulation->gLonList[idx + 1]))
        {
            /* Save index as longitude index */
            gLonIdx = idx;
        }
    }
    if (doLog(logDEBUG))
    {
        logDebug("Longitude is %.1lf, range used => [%.1lf - %.1lf]", gLon,
                 starPopulation->gLonList[gLonIdx],
                 starPopulation->gLonList[gLonIdx + 1]);
    }


    /* Compute the  number of stars at the selected sky area limits */
    mcsINT32 nbOfStarsAtLimit[2][2];
    mcsINT32 i, j;
    for (i = 0; i < 2; i++)
    {
        for (j = 0; j < 2; j++)
        {
            nbOfStarsAtLimit[i][j] = 0;
            for (idx = minMagIdx; idx <= maxMagIdx; idx++)
            {
                nbOfStarsAtLimit[i][j] +=
                        starPopulation->nbOfStars[idx][gLatIdx + i][gLonIdx + j];
            }
            if (doLog(logDEBUG))
            {
                logDebug("nbOfStarsAtLimit[lat:%d][lon:%d] = %d",
                         i, j, nbOfStarsAtLimit[i][j]);
            }
        }
    }

    /* Compute relative distance of the given position in the selected sky
     * area. */
    mcsDOUBLE gLatDistance;
    gLatDistance = (gLat - starPopulation->gLatList[gLatIdx]) /
            (starPopulation->gLatList[gLatIdx + 1] -
            starPopulation->gLatList[gLatIdx]);
    logDebug("Relative position in lattitude direction = %.2lf", gLatDistance);

    mcsDOUBLE gLonDistance;
    gLonDistance = (gLon - starPopulation->gLonList[gLonIdx]) /
            (starPopulation->gLonList[gLonIdx + 1] -
            starPopulation->gLonList[gLonIdx]);
    logDebug("Relative position in longitude direction = %.2lf", gLonDistance);

    /* Interpolation along lattitude axis */
    mcsINT32 nbOfStarsAtHightGLatLimit, nbOfStarsAtLowGLatLimit;
    nbOfStarsAtLowGLatLimit = nbOfStarsAtLimit[0][0] +
            gLonDistance * (nbOfStarsAtLimit[0][1] - nbOfStarsAtLimit[0][0]);
    nbOfStarsAtHightGLatLimit = nbOfStarsAtLimit[1][0] +
            gLonDistance * (nbOfStarsAtLimit[1][1] - nbOfStarsAtLimit[1][0]);

    /* Interpolation along lattitude axis */
    *nbOfStars = nbOfStarsAtLowGLatLimit +
            gLatDistance * (nbOfStarsAtHightGLatLimit - nbOfStarsAtLowGLatLimit);

    logTest("Number of estimated stars into 1 degree solid angle circle=%d", *nbOfStars);

    return mcsSUCCESS;
}


/*
 * Public functions definition
 */

/**
 * Compute the research area size.
 *
 * This function computes the size of the area which should contain about 50
 * stars in the given magnitude range at the specified sky coordinates.
 *
 * @param ra right ascension J2000 in degree
 * @param dec declination J2000 in degree
 * @param minMag minimum of the magnitude range
 * @param maxMag maximum of the magnitude range
 * @param radius estimated size of the radius
 *
 * @warning If there is no star
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
 */
mcsCOMPL_STAT alxGetResearchAreaSize(mcsDOUBLE ra,
                                     mcsDOUBLE dec,
                                     mcsDOUBLE minMag,
                                     mcsDOUBLE maxMag,
                                     mcsDOUBLE* radius)
{
    mcsDOUBLE gLat;
    mcsDOUBLE gLon;

    logTest("Sky coordinates: RA=%.1lf - DEC=%.1lf deg", ra, dec);
    logTest("Magnitude range=%.1lf..%.1lf", minMag, maxMag);

    /* Get structure containing star population */
    alxSTAR_POPULATION *starPopulation;
    starPopulation = alxGetStarPopulation();
    FAIL_NULL(starPopulation);

    /* Computes the galatic coordinate according to ra and dec */
    FAIL(alxComputeGalacticCoordinates(ra, dec, &gLat, &gLon));

    /* Get the number of stars for this sky area */
    mcsINT32 nbOfStars;
    FAIL(alxGetNbOfStars(gLon, gLat, minMag, maxMag, starPopulation, &nbOfStars));

    logTest("Nb of star for this sky area=%d", nbOfStars);

    /* Test if there is no star found; consider that ther is at least one */
    if (nbOfStars == 0)
    {
        nbOfStars = 1;
        logTest("Consider there is at least 1 star.");
    }


    /* Compute the area size according to estimated number of stars at this sky
     * position to only have 50 stars in this area.
     * NOTE: the area of the 1 degree solid angle circle is: PI/4 */
    mcsDOUBLE areaSize;
    areaSize = 50.0 * M_PI / 4.0 / (mcsDOUBLE) nbOfStars;

    logTest("Sky research area size=%.2lf deg", areaSize);

    /* Convert degree to arcmin */
    *radius = 60.0 * sqrt(areaSize / M_PI);

    logTest("Sky research radius=%.2lf arcmin", *radius);

    return mcsSUCCESS;
}

/**
 * Initialize this file
 */
void alxResearchAreaInit(void)
{
    alxGetStarPopulation();
}

/*___oOo___*/
