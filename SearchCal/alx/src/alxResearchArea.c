/*******************************************************************************
* JMMC project
* 
* "@(#) $Id alxDefResearchArea.c, 2005/01/13 12:12:00 clausse Exp $"
*
* History
* -------
* $Log: not supported by cvs2svn $
* Revision 1.10  2005/12/22 10:28:31  scetre
* Updated documentation
*
* Revision 1.9  2005/12/21 15:14:08  scetre
* Added log
*
* Revision 1.8  2005/12/21 15:11:26  scetre
* Added log
*
* Revision 1.7  2005/12/19 21:13:07  gzins
* Changed longitude limit from 350 to 360
*
* Revision 1.6  2005/11/30 10:47:15  scetre
* Added new formula to compute radius
*
* Revision 1.5  2005/10/26 11:24:01  lafrasse
* Code review
*
* Revision 1.4  2005/06/01 14:16:07  scetre
* Changed logExtDbg to logTrace
*
* Revision 1.3  2005/02/22 16:18:21  gzins
* *** empty log message ***
*
* Revision 1.2  2005/02/16 15:10:57  gzins
* Updated call to miscDynBufGetNextLine()
*
* Revision 1.1  2005/02/12 14:59:11  gzins
* Created
*
* clausse     13-Jan-2005  Created
*******************************************************************************/

/**
 * @file
 * Function definition for research area determination.
 *
 * @sa JMMC-MEM-2600-0005 document.
 */

static char *rcsId __attribute__ ((unused)) ="@(#) $Id: alxResearchArea.c,v 1.11 2006-03-03 14:48:24 scetre Exp $";


/*
 * System Headers
 */
#include <stdio.h>
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
static mcsCOMPL_STAT alxGetNbOfStars(mcsFLOAT            gLon,
                                     mcsFLOAT            gLat,
                                     mcsFLOAT            minMag,
                                     mcsFLOAT            maxMag,
                                     alxSTAR_POPULATION* starPopulation,
                                     mcsINT32*           nbOfStars);


/* 
 * Local functions definition
 */
/**
 * Return the structure containing the star population in terms of magnitude and
 * galactic coordinates.
 *
 * @return pointer to structure containing star population, or NULL if an error
 * occured.
 *
 * @usedfiles alxStarPopulationInKBand.cfg : file containing the star population
 */
static alxSTAR_POPULATION *alxGetStarPopulation(void)
{
    logTrace("alxGetStarPopulation()");

    /*
     * Check if the structure, where will be stored star population information,
     * is loaded into memory. If not loaded it.
     */
    static alxSTAR_POPULATION starPopulation = 
                   {mcsFALSE,
                    "alxStarPopulationInKBand.cfg",
                    {0, 10, 90, 180, 270, 360},
                    {-90, -60, -30, -10, 0, 10, 30, 60, 90},
                   };
    if (starPopulation.loaded == mcsTRUE)
    {
        return &starPopulation;
    }

    /* 
     * Build the dynamic buffer which will contain the file of coefficient 
     * of angular diameter
     */
    /* Find the location of the file */
    char *fileName = miscLocateFile(starPopulation.fileName);
    if (fileName == NULL)
    {
        return NULL;
    }

    /* Load file where comment lines started with '#' */
    miscDYN_BUF dynBuf;
    miscDynBufInit(&dynBuf);
    logDebug("Loading %s ...", fileName);
    if (miscDynBufLoadFile(&dynBuf, fileName, "#") == mcsFAILURE)
    {
        miscDynBufDestroy(&dynBuf);
        return NULL;
    }

    /* For each line of the loaded file */
    mcsINT32  lineNum=0;
    const char *pos = NULL;
    mcsSTRING1024 line;
    while ((pos = miscDynBufGetNextLine(&dynBuf, pos, line, sizeof(line),
                                        mcsTRUE)) != NULL)
    {
        /* If line is not empty */
        miscTrimString (line, " ");
        if (strlen(line) != 0)
        {
            /* Check if there is to many lines in file */
            if (lineNum >= alxNB_MAG_STEPS)
            {
                miscDynBufDestroy(&dynBuf);
                errAdd(alxERR_TOO_MANY_LINES, fileName);
                return NULL;
            }

            /* Split line to get individual values */
            mcsINT32 nbMaxSubStrings = 1 + alxNB_GLON_STEPS * alxNB_GLAT_STEPS;
            mcsSTRING256 subStrings[nbMaxSubStrings];
            mcsUINT32 nbFoundSubStrings;
            if (miscSplitString(line, '\t', subStrings, nbMaxSubStrings,
                                &nbFoundSubStrings) == mcsFAILURE)
            {
                errAdd(alxERR_TOO_MANY_COLUMS, fileName, lineNum + 1);
                miscDynBufDestroy(&dynBuf);
                return NULL;
            }

            /* Check there is enough values in line */
            if (nbFoundSubStrings != nbMaxSubStrings)
            {
                errAdd(alxERR_MISSING_COLUMS, fileName, lineNum + 1,
                       nbMaxSubStrings, nbMaxSubStrings);
                miscDynBufDestroy(&dynBuf);
                return NULL;
            }

            /* Store values in structure */
            /* Magnitude for this line */
            if (sscanf(subStrings[0], "%f", &starPopulation.mag[lineNum]) != 1)
            {
                errAdd(alxERR_WRONG_FILE_FORMAT, line, fileName);
                miscDynBufDestroy(&dynBuf);
                return NULL;
            }

            /* and then the number of stars for each area of the sky */
            mcsINT32 gLonPos, gLatPos;
            mcsINT32 subStrIndex;
            for (gLonPos = 0; gLonPos < alxNB_GLON_STEPS; gLonPos++)
            {
                for (gLatPos = 0; gLatPos < alxNB_GLAT_STEPS; gLatPos++)
                {
                    subStrIndex = gLonPos*alxNB_GLAT_STEPS + gLatPos + 1;
                    if (sscanf(subStrings[subStrIndex], "%d", 
                               &starPopulation.nbOfStars[lineNum][gLatPos][gLonPos]) != 1)
                    {
                        errAdd(alxERR_WRONG_FILE_FORMAT, line, fileName);
                        miscDynBufDestroy(&dynBuf);
                        return NULL;
                    }
                }
            }
            /* Next line */
            lineNum++;
        }
    }

    miscDynBufDestroy(&dynBuf);

    return &starPopulation;
}

/**
 * Return the number of stars in the given sky area.
 *
 * This function determines the number of stars in the given magnitude range
 * around the galactic position into a 1 deg² solid angle circle.
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
static mcsCOMPL_STAT alxGetNbOfStars(mcsFLOAT            gLon,
                                     mcsFLOAT            gLat,
                                     mcsFLOAT            minMag,
                                     mcsFLOAT            maxMag,
                                     alxSTAR_POPULATION* starPopulation,
                                     mcsINT32*           nbOfStars)
{
    logTrace("alxGetNbOfStars()"); 

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
        if ((maxMagIdx == -1) && ((starPopulation->mag[idx] >= maxMag) ||
                                  (idx == (alxNB_MAG_STEPS -1))))
        {
            /* Save index as max magnitude index */
            maxMagIdx = idx;
        }
    }
    logDebug("Magnitude [%.1f - %.1f], range used => [%.1f - %.1f]",
             minMag, maxMag, starPopulation->mag[minMagIdx],
             starPopulation->mag[maxMagIdx]);

    /* Find indexes for longitude and lattitude */
    mcsINT32 gLatIdx = 0;
    for (idx = 0; idx < (alxNB_GLAT_STEPS - 1); idx++)
    {
        /* If longitude is in the current interval. */
        if ((starPopulation->gLatList[idx] <= gLat) && 
            (gLat < starPopulation->gLatList[idx + 1]))
        {
            /* Save index as lattitude index */
            gLatIdx = idx;
        }
    }
    logDebug("Lattitude is %.1f, range used => [%.1f - %.1f]", gLat,
             starPopulation->gLatList[gLatIdx],
             starPopulation->gLatList[gLatIdx + 1]);

    mcsINT32 gLonIdx = 0;
    for (idx = 0; idx < (alxNB_GLON_STEPS - 1); idx++)
    {
        /* If longitude is in the current interval. */
        if ((starPopulation->gLonList[idx] <= gLon) && 
            (gLon < starPopulation->gLonList[idx + 1]))
        {
            /* Save index as longitude index */
            gLonIdx = idx;
        }
    }
    logDebug("Longitude is %.1f, range used => [%.1f - %.1f]", gLon,
             starPopulation->gLonList[gLonIdx],
             starPopulation->gLonList[gLonIdx + 1]);


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
            logDebug("nbOfStarsAtLimit[lat:%d][lon:%d] = %d", 
                     i, j, nbOfStarsAtLimit[i][j]); 
        }
    }
   
    /* Compute relative distance of the given position in the selected sky 
     * area. */
    mcsFLOAT gLatDistance;
    gLatDistance = (gLat -  starPopulation->gLatList[gLatIdx]) / 
        (starPopulation->gLatList[gLatIdx + 1] -
         starPopulation->gLatList[gLatIdx]);
    logDebug("Relative position in lattitude direction = %.2f", gLatDistance); 
 
    mcsFLOAT gLonDistance;
    gLonDistance = (gLon - starPopulation->gLonList[gLonIdx]) / 
        (starPopulation->gLonList[gLonIdx + 1] -
         starPopulation->gLonList[gLonIdx]);
    logDebug("Relative position in longitude direction = %.2f", gLonDistance); 

    /* Interpolation along lattitude axis */
    mcsINT32 nbOfStarsAtHightGLatLimit, nbOfStarsAtLowGLatLimit;
    nbOfStarsAtLowGLatLimit = nbOfStarsAtLimit[0][0] + 
        gLonDistance *(nbOfStarsAtLimit[0][1] - nbOfStarsAtLimit[0][0]);
    nbOfStarsAtHightGLatLimit = nbOfStarsAtLimit[1][0] + 
        gLonDistance *(nbOfStarsAtLimit[1][1] - nbOfStarsAtLimit[1][0]);

    /* Interpolation along lattitude axis */
    *nbOfStars = nbOfStarsAtLowGLatLimit + 
        gLatDistance * (nbOfStarsAtHightGLatLimit - nbOfStarsAtLowGLatLimit);
    logTest("Number of estimated stars into 1 degree solid angle circle = %d",
            *nbOfStars); 

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
 * @param ra right acsension J2000 in degree
 * @param dec declinaison J2000 in degree
 * @param minMag minimum of the magnitude range
 * @param maxMag maximum of the magnitude range
 * @param radius estimated size of the radius 
 *
 * @return 
 * mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
 */
mcsCOMPL_STAT alxGetResearchAreaSize(mcsFLOAT  ra,
                                     mcsFLOAT  dec,
                                     mcsFLOAT  minMag,
                                     mcsFLOAT  maxMag,
                                     mcsFLOAT* radius)
{
    mcsFLOAT gLat;
    mcsFLOAT gLon;

    logTrace("alxGetResearchAreaSize()");

    logTest("Sky coordinates (in degrees) : RA = %.1f - DEC = %.1f", ra, dec);
    logTest("Magnitude range = [%.1f..%.1f]", minMag, maxMag);

    /* Get structure containing star population */
    alxSTAR_POPULATION *starPopulation;
    starPopulation = alxGetStarPopulation();
    if (starPopulation == NULL)
    {
        return mcsFAILURE;
    }
   
    /* Computes the galatic coordinate according to ra and dec */
    if (alxComputeGalacticCoordinates(ra, dec, &gLat, &gLon) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    logInfo("GLat = %f, GLon = %f", gLat, gLon);

    /* Get the number of stars for this sky area */
    mcsINT32 nbOfStars;
    if (alxGetNbOfStars(gLon, gLat, minMag, maxMag,
                        starPopulation, &nbOfStars) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    logInfo("Nb of star for this sky area = %d", nbOfStars);

    /* Compute the area size according to estimated number of stars at this sky
     * position to only have 50 stars in this area.
     * NOTE: the area of the 1 degree solid angle circle is: PI/4 */
    mcsFLOAT areaSize;
    areaSize = 50.0 * M_PI/4 / (mcsDOUBLE) nbOfStars;
    logTest("Sky research area size = %.2f (deg)", areaSize);
    /* Convert degree to arcmin */
    *radius = 60.0 * sqrt (areaSize/M_PI);
    logTest("Sky research radius = %.2f (arcmin)", *radius);

    return mcsSUCCESS;
}

/*___oOo___*/
