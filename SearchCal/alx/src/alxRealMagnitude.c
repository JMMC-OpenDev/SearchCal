/*******************************************************************************
 * JMMC project
 * 
 * "@(#) $Id: alxRealMagnitude.c,v 1.5 2005-02-12 15:13:55 gzins Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.4  2005/01/31 13:32:37  scetre
 * changed misc...Pointer in misc...
 *
 * Revision 1.3  2005/01/26 15:49:09  scetre
 * solve memory problem with miscDYN_BUF
 *
 * Revision 1.2  2005/01/24 10:56:25  scetre
 * Changed valid format for spectral type
 *
 * Revision 1.1  2005/01/21 08:14:25  gluck
 * Creation
 *
 * 
 * scetre    28-Sep-2004  Created
 * gzins     12-Jan-2005  - Updated to be compliant with programming standards
 *                        - Improved reliability and error handling
 *
 ******************************************************************************/

/**
 * \file
 * Function definition for corrected magnitude computation (i.e taking into
 * account interstellar absorption).
 *
 * \sa JMMC-MEM-2600-0008 document.
 */

static char *rcsId="@(#) $Id: alxRealMagnitude.c,v 1.5 2005-02-12 15:13:55 gzins Exp $"; 
static void *use_rcsId = ((void)&use_rcsId,(void *) &rcsId);


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
 * Local Functions declaration
 */
static alxEXTINCTION_RATIO_TABLE *alxGetExtinctionRatioTable(void);
static alxPOLYNOMIAL_INTERSTELLAR_ABSORPTION 
        *alxGetPolynamialForInterstellarAbsorption(void);


/* 
 * Local functions definition
 */

/**
 * Return the extinction ratio for interstellar absorption computation 
 *
 * \return pointer to structure containing extinction ratio table or NULL if
 * an error occured.
 *
 * \usedfiles : alxExtinctionRatioTable.cfg : file containing the extinction
 * ratio according to the color (i.e. magnitude band)
 */
static alxEXTINCTION_RATIO_TABLE *alxGetExtinctionRatioTable(void)
{ 
    logExtDbg("alxGetExtinctionRatioTable()");

    /*
     * Check if the structure extinctionRatioTable, where will be stored
     * extinction ratio to compute interstellar extinction, is loaded into
     * memory. If not loaded it.
     */
    static alxEXTINCTION_RATIO_TABLE extinctionRatioTable = 
        {mcsFALSE, "alxExtinctionRatioTable.cfg"};
    if (extinctionRatioTable.loaded == mcsTRUE)
    {
        return (&extinctionRatioTable);
    }
    
    /*
     * Reset all extinction ratio
     */
    int i;
    for (i=0; i<alxNB_FITZ_IDS; i++)
    {
        extinctionRatioTable.rc[i] = 0.0;
    }

    /* 
     * Build the dynamic buffer which will contain the file of extinction ratio
     */
    /* Find the location of the file */
    char *fileName;
    fileName = miscLocateFile(extinctionRatioTable.fileName);
    if (fileName == NULL)
    {
        return NULL;
    }

    /* Load file where comment lines started with '#' */
    miscDYN_BUF dynBuf;
    miscDynBufInit(&dynBuf);
    if (miscDynBufLoadFile(&dynBuf, fileName, "#") == mcsFAILURE)
    {
        miscDynBufDestroy(&dynBuf);
        return NULL;
    }

    /* For each line */
    int  lineNum = 0;
    char *line = NULL;
    while ((line = miscDynBufGetNextLine(&dynBuf,
                                                line, mcsTRUE)) != NULL)
    {
        logDebug("miscDynBufGetNextLine() = '%s'", line);

        /* If line is not empty */
        miscTrimString (line, " ");
        if (strlen(line) != 0)
        {
            /* Check if there is to many lines in file */
            if (lineNum >= alxNB_FITZ_IDS)
            {
                miscDynBufDestroy(&dynBuf);
                errAdd(alxERR_TOO_MANY_LINES, fileName);
                return NULL;
            }

            /* Get extinction ratio */
            char band;
            float rc;
            if (sscanf(line, "%c %f", &band, &rc) != 2)
            {
                miscDynBufDestroy(&dynBuf);
                errAdd(alxERR_WRONG_FILE_FORMAT, line, fileName);
                return NULL;
            }
            else
            {
                alxFITZ_ID fitzId;
                switch toupper(band)
                {
                    case 'M':
                        fitzId = alxFITZ_M;
                        break;
                    case 'L':
                        fitzId = alxFITZ_L;
                        break;
                    case 'K':
                        fitzId = alxFITZ_K;
                        break;
                    case 'H':
                        fitzId = alxFITZ_H;
                        break;
                    case 'J':
                        fitzId = alxFITZ_J;
                        break;
                    case 'I':
                        fitzId = alxFITZ_I;
                        break;
                    case 'R':
                        fitzId = alxFITZ_R;
                        break;
                    case 'V':
                        fitzId = alxFITZ_V;
                        break;
                    case 'B':
                        fitzId = alxFITZ_B;
                        break;
                    default:
                        errAdd(alxERR_INVALID_BAND, band, fileName);
                        miscDynBufDestroy(&dynBuf);
                        return NULL;
                }

                /* Store read extinction ratio */
                if (extinctionRatioTable.rc[fitzId] == 0)
                {
                    extinctionRatioTable.rc[fitzId] = rc;
                }
                else
                {
                    errAdd(alxERR_DUPLICATED_LINE, line, fileName);
                    miscDynBufDestroy(&dynBuf);
                    return NULL;
                }
            }

            /* Next line */
            lineNum++;
        }
    }

    /* Destroy the dynamic buffr where is stored the file information */
    miscDynBufDestroy(&dynBuf);

    /* Check if there is missing line */
    if (lineNum != alxNB_FITZ_IDS)
    {
        errAdd(alxERR_MISSING_LINE, lineNum, alxNB_FITZ_IDS, fileName);
        return NULL;
    }

    extinctionRatioTable.loaded = mcsTRUE;

    return (&extinctionRatioTable);
}

/**
 * Return the polynomial coefficients for interstellar absorption computation 
 *
 * \return pointer to structure containing polynomial coefficients or NULL if
 * an error occured.
 *
 * \usedfiles : alxAbsIntPolynomial.cfg : file containing the polynomial
 * coefficients to compute the interstellar absorption. The polynomial
 * coefficients are given for each galactic longitude range
 */
static alxPOLYNOMIAL_INTERSTELLAR_ABSORPTION 
        *alxGetPolynamialForInterstellarAbsorption(void)
{
    logExtDbg("alxGetPolynamialForInterstellarAbsorption()");

    /*
     * Check if the structure polynomial, where will be stored polynomial
     * coefficients to compute interstellar extinction, is loaded into memory.
     * If not loaded it.
     */
    static alxPOLYNOMIAL_INTERSTELLAR_ABSORPTION polynomial = 
        {mcsFALSE, "alxAbsIntPolynomial.cfg"};
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

    /* Load file where comment lines started with '#' */
    miscDYN_BUF dynBuf;
    miscDynBufInit(&dynBuf);
    if (miscDynBufLoadFile(&dynBuf, fileName, "#") == mcsFAILURE)
    {
        miscDynBufDestroy(&dynBuf);
        return NULL;
    }

    /* For each line */
    int  lineNum=0;
    char *line=NULL;
    while ((line = miscDynBufGetNextLine(&dynBuf,
                                                line, mcsTRUE)) != NULL)
    {
        logDebug("miscDynBufGetNextLine() = '%s'", line);

        /* If line is not empty */
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
            if (sscanf(line, "%f-%f %f %f %f %f",
                       &polynomial.gLonMin[lineNum],
                       &polynomial.gLonMax[lineNum],
                       &polynomial.coeff[lineNum][0],
                       &polynomial.coeff[lineNum][1],
                       &polynomial.coeff[lineNum][2],
                       &polynomial.coeff[lineNum][3]) !=
                alxNB_POLYNOMIAL_COEFF_ABSORPTION + 2)
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
 * Compute corrected magnitudes according to the interstellar absorption
 *
 * The corrected magnitudes are computed using the galactic interstellar
 * extinction model based on galatic coordinates and distance of the star.
 *
 * \param paralax the paralax
 * \param gLat the galatic lattitude in degrees
 * \param gLon the galatic longitude in degrees
 * \param mgB corrected magnitude B
 * \param mgV corrected magnitude V
 * \param mgR corrected magnitude R
 * \param mgI corrected magnitude I
 * \param mgJ corrected magnitude J
 * \param mgH corrected magnitude H
 * \param mgK corrected magnitude K
 * \param mgL corrected magnitude L
 * \param mgM corrected magnitude M
 * 
 * \return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is
 * returned.
 */
/* WARNING: the computation of corrected magnitudes based on interstellar
 * absorption, described in JMMC-MEM-2600-0008 document (section 2.1.1), has not
 * been implemented because there is no catalog which provides this information
 * for a representative number of stars.
 */
mcsCOMPL_STAT alxComputeRealMagnitudes(mcsFLOAT paralax,
                                       mcsFLOAT gLat,
                                       mcsFLOAT gLon,
                                       mcsFLOAT *mgB,
                                       mcsFLOAT *mgV,
                                       mcsFLOAT *mgR,
                                       mcsFLOAT *mgI,
                                       mcsFLOAT *mgJ,
                                       mcsFLOAT *mgH,
                                       mcsFLOAT *mgK,
                                       mcsFLOAT *mgL,
                                       mcsFLOAT *mgM)
{
    logExtDbg("alxComputeRealMagnitudes()");

    /* Get extinction ratio table */
    alxEXTINCTION_RATIO_TABLE *extinctionRatioTable;
    extinctionRatioTable = alxGetExtinctionRatioTable();
    if (extinctionRatioTable == NULL)
    {
        return mcsFAILURE;        
    }

    /* Get polynomial for interstellar extinction computation */
    alxPOLYNOMIAL_INTERSTELLAR_ABSORPTION *polynomial;
    polynomial = alxGetPolynamialForInterstellarAbsorption();
    if (polynomial == NULL)
    {
        return mcsFAILURE;        
    }

    /* Compute distance */
    mcsFLOAT distance;
    if (paralax == 0)
    {
        errAdd(alxERR_INVALID_PARALAX_VALUE, paralax);
        return mcsFAILURE;
    }
    distance = (1 / paralax);
    
    
    /* Compute the extinction coefficient in V band according to the galatic
     * lattitude. */
    /* If the lattitude is greated than 50 degrees */
    mcsFLOAT av;
    if (fabs(gLat) > 50)
    {
        /* Set extinction coefficient to 0. */
        av = 0;
    }
    /* If the lattitude is between 10 and 50 degrees */ 
    else if ((fabs(gLat) < 50) && (fabs(gLat) > 10))
    {
        mcsFLOAT ho=0.120;
        av = (0.165 * ( 1.192 - fabs(tan(gLat * M_PI / 180)))) /
            fabs(sin(gLat * M_PI / 180)) *
            (1 - exp(-distance * fabs(sin(gLat * M_PI / 180)) / ho));
    }
    /* If the lattitude is less than 10 degrees */
    else
    {
        /* Find longitude in polynomial table */ 
        int i=0;
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
            /* End if */
        }
        if (found == mcsFALSE)
        {
            errAdd(alxERR_LONGITUDE_NOT_FOUND, gLon);
            return mcsFAILURE;
        }
        av =  polynomial->coeff[i][0]*distance
            + polynomial->coeff[i][1]*distance*distance
            + polynomial->coeff[i][2]*distance*distance*distance
            + polynomial->coeff[i][3]*distance*distance*distance*distance;
    }
    logTest ("Galactic longitude          = %.3f", gLon);
    logTest ("Distance                    = %.3f", distance);
    logTest ("Extinction coefficient in V = %.3f", av);

    /* 
     * Computed corrected magnitudes.
     * Co = C - Ac
     * where Ac = Av*Rc/Rv, with Rv=3.10
     */
    /* 
     * if the pointer of a magnitude is NULL that's mean that there is nothing
     * to compute. In this case, do nothing
     */
    if (*mgM != 0)
    {
        *mgM = *mgM - (av * extinctionRatioTable->rc[alxFITZ_M] / 3.10);
    }
    if (*mgL != 0)
    {
        *mgL = *mgL - (av * extinctionRatioTable->rc[alxFITZ_L] / 3.10);
    }
    if (*mgK != 0)
    {
        *mgK = *mgK - (av * extinctionRatioTable->rc[alxFITZ_K] / 3.10);
    }
    if (*mgH != 0)
    {
        *mgH = *mgH - (av * extinctionRatioTable->rc[alxFITZ_H] / 3.10);
    }
    if (*mgJ != 0)
    {
        *mgJ = *mgJ - (av * extinctionRatioTable->rc[alxFITZ_J] / 3.10);
    }
    if (*mgI != 0)
    {
        *mgI = *mgI - (av * extinctionRatioTable->rc[alxFITZ_I] / 3.10);
    }
    if (*mgR != 0)
    {
        *mgR = *mgR - (av * extinctionRatioTable->rc[alxFITZ_R] / 3.10);
    }
    if (*mgV != 0)
    {
        *mgV = *mgV - (av * extinctionRatioTable->rc[alxFITZ_V] / 3.10);
    }
    if (*mgB != 0)
    {
        *mgB = *mgB - (av * extinctionRatioTable->rc[alxFITZ_B] / 3.10);
    }

    /* Print out results */
    if (*mgB != 0)
    {
        logTest("B (corrected)    = %0.3f", *mgB);
    }
    if (*mgV != 0)
    {
        logTest("V (corrected)    = %0.3f", *mgV);
    }
    if (*mgR != 0)
    {
        logTest("R (corrected)    = %0.3lf", *mgR);
    }
    if (*mgI != 0)
    {
        logTest("I (corrected)    = %0.3lf", *mgI);
    }
    if (*mgJ != 0)
    {
        logTest("J (corrected)    = %0.3lf", *mgJ);
    }
    if (*mgH != 0)
    {
        logTest("H (corrected)    = %0.3lf", *mgH);
    }
    if (*mgK != 0)
    {
        logTest("K (corrected)    = %0.3lf", *mgK);
    }
    if (*mgL != 0)
    {
        logTest("L (corrected)    = %0.3lf", *mgL);
    }
    if (*mgM != 0)
    {
        logTest("M (corrected)    = %0.3lf", *mgM);
    }

    return mcsSUCCESS;
}



/*___oOo___*/
