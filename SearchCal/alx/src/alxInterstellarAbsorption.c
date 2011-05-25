/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * @file
 * Function definition for interstellar absorption computation
 *
 * @sa JMMC-MEM-2600-0008 document. 
 */

static char *rcsId __attribute__ ((unused)) ="@(#) $Id: alxInterstellarAbsorption.c,v 1.3 2011-03-03 12:59:53 lafrasse Exp $"; 


/* 
 * System Headers
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
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
#include "alxPrivate.h"
#include "alxErrors.h"


/*
 * Local Variables
 */


/*
 * Local Functions declaration
 */
static alxPOLYNOMIAL_INTERSTELLAR_ABSORPTION*
                   alxGetPolynamialForInterstellarAbsorption(void);

/* 
 * Local functions definition
 */
/**
 * Return the polynomial coefficients for interstellar absorption computation. 
 *
 * @return pointer onto structure containing polynomial coefficients, or NULL if
 * an error occured.
 *
 * @usedfiles : alxIntAbsPolynomial.cfg : configuration file containing the
 * polynomial coefficients to compute the interstellar absorption. 
 * The polynomial coefficients are given for each galactic longitude range
 */
static alxPOLYNOMIAL_INTERSTELLAR_ABSORPTION 
                                *alxGetPolynamialForInterstellarAbsorption(void)
{
    logTrace("alxGetPolynamialForInterstellarAbsorption()");

    /*
     * Check if the structure polynomial, where will be stored polynomial
     * coefficients to compute interstellar extinction, is loaded into memory.
     * If not loaded it.
     */
    static alxPOLYNOMIAL_INTERSTELLAR_ABSORPTION polynomial = 
        {mcsFALSE, "alxIntAbsPolynomial.cfg"};
    if (polynomial.loaded == mcsTRUE)
    {
        return (&polynomial);
    }

    /* 
     * Build the dynamic buffer which will contain the file of coefficient 
     * of angular diameter
     */
    /* Find the location of the file */
    char *fileName;
    fileName = miscLocateFile(polynomial.fileName);
    if (fileName == NULL)
    {
        return NULL;
    }

    /* Load file. Comment lines started with '#' */
    miscDYN_BUF dynBuf;
    miscDynBufInit(&dynBuf);
    if (miscDynBufLoadFile(&dynBuf, fileName, "#") == mcsFAILURE)
    {
        miscDynBufDestroy(&dynBuf);
        return NULL;
    }

    /* For each line */
    mcsINT32 lineNum=0;
    const char *pos = NULL;
    mcsSTRING1024 line;
    while ((pos = miscDynBufGetNextLine
            (&dynBuf, pos, line, sizeof(mcsSTRING1024), mcsTRUE)) != NULL)
    {
        logDebug("miscDynBufGetNextLine() = '%s'", line);

        /* If line is not empty */
        /* Trim line for leading and trailing characters */        
        miscTrimString (line, " ");
        if (strlen(line) != 0)
        {
            /* Check if there is to many lines in file */
            if (lineNum >= alxNB_MAX_LONGITUDE_STEPS)
            {
                miscDynBufDestroy(&dynBuf);
                errAdd(alxERR_TOO_MANY_LINES, fileName);
                return NULL;
            }

            /* Get polynomial coefficients */
            if (sscanf(line, "%lf-%lf %lf %lf %lf %lf",
                       &polynomial.gLonMin[lineNum],
                       &polynomial.gLonMax[lineNum],
                       &polynomial.coeff[lineNum][0],
                       &polynomial.coeff[lineNum][1],
                       &polynomial.coeff[lineNum][2],
                       &polynomial.coeff[lineNum][3])
                != alxNB_POLYNOMIAL_COEFF_ABSORPTION + 2)
            {
                miscDynBufDestroy(&dynBuf);
                errAdd(alxERR_WRONG_FILE_FORMAT, line, fileName);
                return NULL;
            }

            /* Next line */
            lineNum++;
        }
    }

    /* Destroy the dynamic buffr where is stored the file information */
    miscDynBufDestroy(&dynBuf);

    polynomial.nbLines = lineNum;
    polynomial.loaded  = mcsTRUE;

    return (&polynomial);
}

/*
 * Public functions definition
 */
/**
 * Compute the extinction coefficient in V band according to the galatic
 * lattitude
 *
 * @param av extinction coefficient to compute
 * @param plx paralax value
 * @param gLat galactic Lattitude value
 * @param gLon galactic Longitude value
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is
 * returned. 
 */
mcsCOMPL_STAT alxComputeExtinctionCoefficient(mcsDOUBLE* av,
                                              mcsDOUBLE  plx,
                                              mcsDOUBLE  gLat,
                                              mcsDOUBLE  gLon)
{
    logTrace("alxComputeExtinctionCoefficient()");

    /* Get polynomial for interstellar extinction computation */
    alxPOLYNOMIAL_INTERSTELLAR_ABSORPTION *polynomial;
    polynomial = alxGetPolynamialForInterstellarAbsorption();
    if (polynomial == NULL)
    {
        return mcsFAILURE;        
    }

    /* Compute distance */
    mcsDOUBLE distance;
    if (plx == 0)
    {
        errAdd(alxERR_INVALID_PARALAX_VALUE, plx);
        return mcsFAILURE;
    }
    distance = (1 / plx);
    
    /* 
     * Compute the extinction coefficient in V band according to the galatic
     * lattitude. 
     */
    
    /* If the latitude is greated than 50 degrees */
    if (fabs(gLat) > 50)
    {
        /* Set extinction coefficient to 0. */
        *av = 0;
    }
    /* If the latitude is between 10 and 50 degrees */ 
    else if ((fabs(gLat) < 50) && (fabs(gLat) > 10))
    {
        mcsDOUBLE ho = 0.120;
        *av = (0.165 * ( 1.192 - fabs(tan(gLat * M_PI / 180)))) 
            / fabs(sin(gLat * M_PI / 180)) 
            * (1 - exp(-distance * fabs(sin(gLat * M_PI / 180)) / ho));
    }
    /* If the latitude is less than 10 degrees */
    else
    {
        /* Find longitude in polynomial table */ 
        mcsINT32 i = 0;
        mcsLOGICAL found = mcsFALSE;
        while ((found == mcsFALSE) && (i < polynomial->nbLines))
        {
            /* If longitude belongs to the range */
            if (gLon >= polynomial->gLonMin[i] && 
                gLon < polynomial->gLonMax[i])
            {
                /* Stop search */
                found = mcsTRUE;
            }
            /* Else */
            else
            {
                /* Go to the next line */
                i++;
            }
        }
        /* if not found add error */
        if (found == mcsFALSE)
        {
            errAdd(alxERR_LONGITUDE_NOT_FOUND, gLon);
            return mcsFAILURE;
        }
        *av =  polynomial->coeff[i][0]*distance
            + polynomial->coeff[i][1]*distance*distance
            + polynomial->coeff[i][2]*distance*distance*distance
            + polynomial->coeff[i][3]*distance*distance*distance*distance;
    }
    /* Display results */
    logTest ("Galactic longitude          = %.3f", gLon);
    logTest ("Distance                    = %.3f", distance);
    logTest ("Extinction coefficient in V = %.3f", *av);
    
    return mcsSUCCESS;
}




/*___oOo___*/
