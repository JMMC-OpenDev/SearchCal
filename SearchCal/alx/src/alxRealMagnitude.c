/*******************************************************************************
 * JMMC project
 * 
 * "@(#) $Id: alxRealMagnitude.c,v 1.12 2005-04-04 07:22:11 scetre Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.11  2005/03/30 12:48:15  scetre
 * Added structure in order to simplify the code.
 * Changed API with this structure.
 * Share the code of the main function in sevral function in order to simplify the code
 *
 * Revision 1.10  2005/02/22 16:18:13  gzins
 * Updated misDynBufGetNextLine API
 *
 * Revision 1.9  2005/02/22 10:16:43  gzins
 * Removed \n in logTest message
 *
 * Revision 1.8  2005/02/22 08:08:59  gzins
 * Changed printf to logTest
 *
 * Revision 1.7  2005/02/21 19:35:45  gzins
 * Updated alxComputeRealMagnitudes to have API compatible with alxComputeRealMagnitudes and handle blanking values.
 *
 * Revision 1.6  2005/02/16 15:10:57  gzins
 * Updated call to miscDynBufGetNextLine()
 *
 * Revision 1.5  2005/02/12 15:13:55  gzins
 * Removed call to miscResolvePath; done by miscLocateFile
 *
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

static char *rcsId="@(#) $Id: alxRealMagnitude.c,v 1.12 2005-04-04 07:22:11 scetre Exp $"; 
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
static mcsCOMPL_STAT
alxComputeExtinctionCoefficient(mcsFLOAT *av,
                                mcsFLOAT paralax,
                                mcsFLOAT gLat,
                                mcsFLOAT gLon,
                                alxPOLYNOMIAL_INTERSTELLAR_ABSORPTION
                                    *polynomial);

/* 
 * Local functions definition
 */

/**
 * Return the extinction ratio for interstellar absorption computation 
 *
 * \return pointer onto structure containing extinction ratio table or NULL if
 * an error occured.
 *
 * \usedfiles : alxExtinctionRatioTable.cfg : configuration file containing the
 * extinction ratio according to the color (i.e. magnitude band)
 */
static alxEXTINCTION_RATIO_TABLE *alxGetExtinctionRatioTable(void)
{ 
    logExtDbg("alxGetExtinctionRatioTable()");

    /*
     * Check if the structure extinctionRatioTable, where will be stored
     * extinction ratio to compute interstellar extinction, is loaded into
     * memory. If not load it.
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
    for (i=0; i<alxNB_BANDS; i++)
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

    /* Load file. Comment lines start with '#' */
    miscDYN_BUF dynBuf;
    /* initialize the dynamic buffer */
    miscDynBufInit(&dynBuf);
    if (miscDynBufLoadFile(&dynBuf, fileName, "#") == mcsFAILURE)
    {
        /* If Load of the file failed, destroy dynamic buffer */
        miscDynBufDestroy(&dynBuf);
        return NULL;
    }

    /* For each line */
    mcsINT32  lineNum = 0;
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
            if (lineNum >= alxNB_BANDS)
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
                /* fitzpatrick identifier */
                alxBAND fitzId;
                switch toupper(band)
                {
                    case 'M':
                        fitzId = alxM_BAND;
                        break;
                    case 'L':
                        fitzId = alxL_BAND;
                        break;
                    case 'K':
                        fitzId = alxK_BAND;
                        break;
                    case 'H':
                        fitzId = alxH_BAND;
                        break;
                    case 'J':
                        fitzId = alxJ_BAND;
                        break;
                    case 'I':
                        fitzId = alxI_BAND;
                        break;
                    case 'R':
                        fitzId = alxR_BAND;
                        break;
                    case 'V':
                        fitzId = alxV_BAND;
                        break;
                    case 'B':
                        fitzId = alxB_BAND;
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
    if (lineNum != alxNB_BANDS)
    {
        errAdd(alxERR_MISSING_LINE, lineNum, alxNB_BANDS, fileName);
        return NULL;
    }

    extinctionRatioTable.loaded = mcsTRUE;

    return (&extinctionRatioTable);
}

/**
 * Return the polynomial coefficients for interstellar absorption computation 
 *
 * \return pointer onto structure containing polynomial coefficients or NULL if
 * an error occured.
 *
 * \usedfiles : alxAbsIntPolynomial.cfg : configuration file containing the
 * polynomial coefficients to compute the interstellar absorption. 
 * The polynomial coefficients are given for each galactic longitude range
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

    /* Load file. Comment lines started with '#' */
    miscDYN_BUF dynBuf;
    /* initialize the dynamic buffer */
    miscDynBufInit(&dynBuf);
    if (miscDynBufLoadFile(&dynBuf, fileName, "#") == mcsFAILURE)
    {
        /* If Load of the file failed, destroy dynamic buffer */
        miscDynBufDestroy(&dynBuf);
        return NULL;
    }

    /* For each line */
    mcsINT32  lineNum=0;
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

/**
 * Compute the extinction coefficient in V band according to the galatic
 * lattitude
 *
 * \param av extinction coefficient to compute
 * \param paralax paralax value
 * \param gLat galactic Lattitude value
 * \param gLon galactic Longitude value
 * \param polynomial polynomial interstellar absorption structure
 *
 * \return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is
 * returned. 
 */
static mcsCOMPL_STAT
    alxComputeExtinctionCoefficient(mcsFLOAT *av,
                                    mcsFLOAT paralax,
                                    mcsFLOAT gLat,
                                    mcsFLOAT gLon,
                                    alxPOLYNOMIAL_INTERSTELLAR_ABSORPTION
                                        *polynomial)
{
    logExtDbg("alxComputeExtinctionCoefficient()");

    /* Compute distance */
    mcsFLOAT distance;
    if (paralax == 0)
    {
        errAdd(alxERR_INVALID_PARALAX_VALUE, paralax);
        return mcsFAILURE;
    }
    distance = (1 / paralax);
    
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
        mcsFLOAT ho = 0.120;
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
 * \param magnitudes computed magnitudes in B, V, R, I, J, H, K, L and M bands
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
                                       alxMAGNITUDES magnitudes)
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

    /* Declare extinction coefficient */ 
    mcsFLOAT av; 
   
    /* Compute extinction coefficient */
    if (alxComputeExtinctionCoefficient(&av, paralax, gLat, gLon, polynomial) ==
        mcsFAILURE)
    {
        return mcsFAILURE;
    }
    
    /* 
     * Computed corrected magnitudes.
     * Co = C - Ac
     * where Ac = Av*Rc/Rv, with Rv=3.10
     */
    /* 
     * If the pointer of a magnitude is NULL its means that there is nothing
     * to compute. In this case, do nothing
     */
    int band;
    for (band = alxB_BAND; band <= alxM_BAND; band++)
    {
        if (magnitudes[band].isSet == mcsTRUE)
        {
            magnitudes[band].value = magnitudes[band].value 
                - (av * extinctionRatioTable->rc[band] / 3.10);
        }
        
        logTest("Corrected magnitude[%d] = %0.3f",
                band, magnitudes[band].value); 
    }

    return mcsSUCCESS;
}

/*___oOo___*/
