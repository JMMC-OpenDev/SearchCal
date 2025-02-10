/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * @file
 * Function definition for interstellar absorption computation.
 *
 * @sa JMMC-MEM-2600-0008 document.
 */


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
/* Rv coefficient = 3.1 */
static mcsDOUBLE Rv = 3.10;

/*
 * Local Functions declaration
 */
static alxPOLYNOMIAL_INTERSTELLAR_ABSORPTION* alxGetPolynomialForInterstellarAbsorption(void);

/*
 * Local functions definition
 */

/**
 * Return the polynomial coefficients for interstellar absorption computation.
 *
 * @return pointer onto structure containing polynomial coefficients, or NULL if
 * an error occurred.
 *
 * @usedfiles : alxIntAbsPolynomial.cfg : configuration file containing the
 * polynomial coefficients to compute the interstellar absorption.
 * The polynomial coefficients are given for each galactic longitude range
 */
static alxPOLYNOMIAL_INTERSTELLAR_ABSORPTION* alxGetPolynomialForInterstellarAbsorption(void)
{
    static alxPOLYNOMIAL_INTERSTELLAR_ABSORPTION polynomial = {mcsFALSE, "alxIntAbsPolynomial.cfg", 0,
        {0.0},
        {0.0},
        {
            {0.0}
        }};

    /* Check if the structure is loaded into memory. If not load it. */
    if (IS_TRUE(polynomial.loaded))
    {
        return &polynomial;
    }

    /*
     * Build the dynamic buffer which will contain the file of coefficient
     * of angular diameter
     */
    /* Find the location of the file */
    char *fileName;
    fileName = miscLocateFile(polynomial.fileName);
    if (IS_NULL(fileName))
    {
        return NULL;
    }

    /* Load file. Comment lines started with '#' */
    miscDYN_BUF dynBuf;
    NULL_(miscDynBufInit(&dynBuf));

    logInfo("Loading %s ...", fileName);

    NULL_DO(miscDynBufLoadFile(&dynBuf, fileName, "#"),
            miscDynBufDestroy(&dynBuf);
            free(fileName));

    /* For each line */
    mcsINT32 lineNum = 0;
    const char *pos = NULL;
    mcsSTRING1024 line;
    const mcsUINT32 maxLineLength = sizeof (line) - 1;

    while (IS_NOT_NULL(pos = miscDynBufGetNextLine(&dynBuf, pos, line, maxLineLength, mcsTRUE)))
    {
        logTrace("miscDynBufGetNextLine()='%s'", line);

        /* Trim line for any leading and trailing blank characters */
        TRIM_SPACE(line);
        /* If line is not empty */
        if (strlen(line) != 0)
        {
            /* Check if there is to many lines in file */
            if (lineNum >= alxNB_MAX_LONGITUDE_STEPS)
            {
                miscDynBufDestroy(&dynBuf);
                errAdd(alxERR_TOO_MANY_LINES, fileName);
                free(fileName);
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
                free(fileName);
                return NULL;
            }

            /* Next line */
            lineNum++;
        }
    }

    /* Destroy the dynamic buffer where is stored the file information */
    miscDynBufDestroy(&dynBuf);
    free(fileName);

    polynomial.nbLines = lineNum;
    polynomial.loaded = mcsTRUE;

    return &polynomial;
}

/**
 * Return the extinction ratio for interstellar absorption computation .
 *
 * @return pointer onto structure containing extinction ratio table, or NULL if
 * an error occurred.
 *
 * @usedfiles : alxExtinctionRatioTable.cfg : configuration file containing the
 * extinction ratio according to the color (i.e. magnitude band)
 */
alxEXTINCTION_RATIO_TABLE* alxGetExtinctionRatioTable(void)
{
    static alxEXTINCTION_RATIO_TABLE extinctionRatioTable = {mcsFALSE, "alxExtinctionRatioTable.cfg",
        {0.0},
        {0.0}};

    /* Check if the structure is loaded into memory. If not load it. */
    if (IS_TRUE(extinctionRatioTable.loaded))
    {
        return &extinctionRatioTable;
    }

    /*
     * Reset all extinction ratio and coefficients (Rc/Rv)
     */
    mcsUINT32 i;
    for (i = alxB_BAND; i < alxNB_BANDS; i++)
    {
        extinctionRatioTable.rc[i] = 0.0;
        extinctionRatioTable.coeff[i] = 0.0;
    }

    /*
     * Build the dynamic buffer which will contain the file of extinction ratio
     */
    /* Find the location of the file */
    char* fileName;
    fileName = miscLocateFile(extinctionRatioTable.fileName);
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
    const char *pos = NULL;
    mcsSTRING1024 line;
    const mcsUINT32 maxLineLength = sizeof (line) - 1;

    while (IS_NOT_NULL(pos = miscDynBufGetNextLine(&dynBuf, pos, line, maxLineLength, mcsTRUE)))
    {
        logTrace("miscDynBufGetNextLine()='%s'", line);

        /* Trim line for any leading and trailing blank characters */
        TRIM_SPACE(line);
        /* If line is not empty */
        if (strlen(line) != 0)
        {
            /* Check if there is to many lines in file */
            if (lineNum >= alxNB_BANDS)
            {
                miscDynBufDestroy(&dynBuf);
                errAdd(alxERR_TOO_MANY_LINES, fileName);
                free(fileName);
                return NULL;
            }

            /* Get extinction ratio */
            char band;
            mcsDOUBLE rc;
            if (sscanf(line, "%c %lf", &band, &rc) != 2)
            {
                miscDynBufDestroy(&dynBuf);
                errAdd(alxERR_WRONG_FILE_FORMAT, line, fileName);
                free(fileName);
                return NULL;
            }
            else
            {
                /* fitzpatrick identifier */
                alxBAND fitzId;
                switch (toupper(band))
                {
                    case 'B':
                        fitzId = alxB_BAND;
                        break;
                    case 'V':
                        fitzId = alxV_BAND;
                        break;
                    case 'R':
                        fitzId = alxR_BAND;
                        break;
                    case 'I':
                        fitzId = alxI_BAND;
                        break;
                    case 'J':
                        fitzId = alxJ_BAND;
                        break;
                    case 'H':
                        fitzId = alxH_BAND;
                        break;
                    case 'K':
                        fitzId = alxK_BAND;
                        break;
                    case 'L':
                        fitzId = alxL_BAND;
                        break;
                    case 'M':
                        fitzId = alxM_BAND;
                        break;
                    case 'N':
                        fitzId = alxN_BAND;
                        break;
                    default:
                        errAdd(alxERR_INVALID_BAND, band, fileName);
                        miscDynBufDestroy(&dynBuf);
                        free(fileName);
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
                    free(fileName);
                    return NULL;
                }
            }

            /* Next line */
            lineNum++;
        }
    }

    /* Destroy the dynamic buffer where is stored the file information */
    miscDynBufDestroy(&dynBuf);

    /* Check if there is missing line (except N) */
    if (lineNum != (alxNB_BANDS - 1))
    {
        errAdd(alxERR_MISSING_LINE, lineNum, alxNB_BANDS, fileName);
        free(fileName);
        return NULL;
    }
    free(fileName);


    /* Compute correction coefficients = Rc / Rv, with Rv=3.1 */
    mcsUINT32 band;
    for (band = alxB_BAND; band < alxNB_BANDS; band++)
    {
        extinctionRatioTable.coeff[band] = extinctionRatioTable.rc[band] / Rv;

        logDebug("coeff[%d] = %.3lf", band, extinctionRatioTable.coeff[band]);
    }

    extinctionRatioTable.loaded = mcsTRUE;

    return &extinctionRatioTable;
}

/**
 * Compute the extinction coefficient in V band (Av) according to the galactic
 * latitude, longitude and distance.
 *
 * @param av extinction coefficient to compute
 * @param e_av error on extinction coefficient to compute
 * @param distances distance array(nominal, min, max)
 * @param gLat galactic latitude value
 * @param gLon galactic longitude value
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
 */
mcsCOMPL_STAT alxComputeExtinctionCoefficientFromDistances(mcsDOUBLE* Av,
                                                           mcsDOUBLE* e_Av,
                                                           mcsDOUBLE distances[3],
                                                           mcsDOUBLE gLat,
                                                           mcsDOUBLE gLon)
{
    /** minimum uncertainty on Av set to 0.1 */
    static mcsDOUBLE MIN_AV_ERROR = 0.1;

    /*
     * Compute the extinction coefficient in V band according to the galactic latitude.
     */

    /* If the latitude is greater than 50 degrees */
    if (fabs(gLat) >= 50.0)
    {
        /* Set extinction coefficient to 0 and error to 0.2 */
        *Av = 0.0;
        *e_Av = MIN_AV_ERROR;
    }
    else
    {
        mcsDOUBLE Avs[3];
        mcsUINT32 n;

        /* If the latitude is between 10 and 50 degrees */
        if ((fabs(gLat) >= 10.0) && (fabs(gLat) < 50.0))
        {
            mcsDOUBLE ho = 0.120;
            mcsDOUBLE tanGLat = fabs(tan(gLat * alxDEG_IN_RAD));
            mcsDOUBLE sinGLat = fabs(sin(gLat * alxDEG_IN_RAD));

            for (n = 0; n < 3; n++)
            {
                /* ensure Av >= 0 */
                Avs[n] = alxMax(0.0, (0.165 * (1.192 - tanGLat)) / sinGLat * (1.0 - exp(-distances[n] * sinGLat / ho)));
            }
        }
        else
        {
            /* If the latitude is less than 10 degrees */

            /* Get polynomial for interstellar extinction computation */
            alxPOLYNOMIAL_INTERSTELLAR_ABSORPTION *polynomial;
            polynomial = alxGetPolynomialForInterstellarAbsorption();
            FAIL_NULL(polynomial);

            /* Find longitude in polynomial table */
            mcsINT32 i = 0;
            mcsLOGICAL found = mcsFALSE;

            while (IS_FALSE(found) && (i < polynomial->nbLines))
            {
                /* If longitude belongs to the range */
                if ((gLon >= polynomial->gLonMin[i]) && (gLon < polynomial->gLonMax[i]))
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
            FAIL_FALSE_DO(found, 
                          errAdd(alxERR_LONGITUDE_NOT_FOUND, gLon));

            mcsDOUBLE* coeffs = polynomial->coeff[i];
            mcsDOUBLE distance;

            for (n = 0; n < 3; n++)
            {
                distance = distances[n];

                /* ensure Av >= 0 */
                Avs[n] = alxMax(0.0,
                                coeffs[0] * distance
                                + coeffs[1] * distance * distance
                                + coeffs[2] * distance * distance * distance
                                + coeffs[3] * distance * distance * distance * distance);
            }
        }

        *Av = Avs[0];

        /* Uncertainty should encompass Avmin and Avmax (asymetric distribution)
         * because distance = distance +/- 1 error_distance */
        *e_Av = alxMax(fabs(Avs[0] - Avs[1]), fabs(Avs[0] - Avs[2]));

        logDebug("AVs=%.3lf [%.3lf - %.3lf] err=%.4lf", Avs[0], Avs[1], Avs[2], *e_Av);

        /* Fix minimum uncertainty on Av */
        *e_Av = alxMax(MIN_AV_ERROR, *e_Av);
    }

    /* Display results */
    logDebug("GLon/GLat=%+08.3lf,%+08.3lf dist=%.3lf Av=%.4lf (%.5lf)", gLon, gLat, distances[0], *Av, *e_Av);

    return mcsSUCCESS;
}

/*
 * Public functions definition
 */

/**
 * Compute the extinction coefficient in V band (Av) according to the galactic
 * latitude, longitude and distance.
 *
 * @param av extinction coefficient to compute
 * @param e_av error on extinction coefficient to compute
 * @param dist distance to compute
 * @param e_dist error on distance to compute
 * @param plx parallax value
 * @param e_plx error on parallax value
 * @param gLat galactic latitude value
 * @param gLon galactic Longitude value
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
 */
mcsCOMPL_STAT alxComputeExtinctionCoefficient(mcsDOUBLE *Av,
                                              mcsDOUBLE *e_Av,
                                              mcsDOUBLE *dist,
                                              mcsDOUBLE *e_dist,
                                              mcsDOUBLE plx,
                                              mcsDOUBLE e_plx,
                                              mcsDOUBLE gLat,
                                              mcsDOUBLE gLon)
{
    FAIL_COND_DO((plx == 0.0), 
                 errAdd(alxERR_INVALID_PARALAX_VALUE, plx));

    mcsDOUBLE distance, error;
    /*
     * Compute distance and its error in parsecs
     * dist = 1 / plx
     * var(dist) = dist^4 x var(plx) = e_plx^2 / plx^4
     */
    *dist   = distance = 1000.0         / plx;            /* pc */
    *e_dist = error    = 1000.0 * e_plx / alxSquare(plx); /* pc */

    /* convert into kpc */
    distance *= 1e-3;
    error    *= 1e-3;

    /* ensure 0 < distance < 1 kpc */
#define checkDistance(dist) \
    ((dist > 0.0) && (dist < 1.0))

    /* Compute distances (kpc) */
    /* see CheckParallaxes(): plx > 1.0 for polynomial approximations ie distance < 1 kpc */
    mcsDOUBLE distances[3];
    distances[0] = (checkDistance(distance)) ? distance : 0.0;
    distances[1] = (checkDistance(distance - error)) ? (distance - error) : 0.0; /* min */
    distances[2] = (checkDistance(distance + error)) ? (distance + error) : 1.0; /* max 1 kpc */

    return alxComputeExtinctionCoefficientFromDistances(Av, e_Av, distances, gLat, gLon);
}

/**
 * Compute corrected magnitudes according to the interstellar absorption and the
 * corrected ones.
 *
 * @param Av the extinction ratio
 * @param magnitudes in B, V, R, I, J, H, K, L and M bands
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
 */
mcsCOMPL_STAT alxComputeCorrectedMagnitudes(const char* msg,
                                            mcsDOUBLE Av,
                                            alxMAGNITUDES magnitudes,
                                            mcsLOGICAL doLog)
{
    /* Get extinction ratio table */
    alxEXTINCTION_RATIO_TABLE *extinctionRatioTable;
    extinctionRatioTable = alxGetExtinctionRatioTable();
    FAIL_NULL(extinctionRatioTable);

    /*
     * Computed corrected magnitudes:
     * Co = C - Ac
     * where Ac = Av * Rc/Rv, with Rv=3.1
     */
    mcsUINT32 band;

    for (band = alxB_BAND; band < alxNB_BANDS; band++)
    {
        if (alxIsSet(magnitudes[band]))
        {
            magnitudes[band].value -= Av * extinctionRatioTable->coeff[band];
        }
    }

    if (IS_TRUE(doLog))
    {
        alxLogTestMagnitudes("Corrected magnitudes:", msg, magnitudes);
    }

    return mcsSUCCESS;
}

/**
 * Compute apparent magnitudes according to the interstellar absorption.
 *
 * @param av the extinction ratio
 * @param magnitudes in B, V, R, I, J, H, K, L and M bands
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
 */
mcsCOMPL_STAT alxComputeApparentMagnitudes(mcsDOUBLE Av,
                                           alxMAGNITUDES magnitudes)
{
    /* Get extinction ratio table */
    alxEXTINCTION_RATIO_TABLE *extinctionRatioTable;
    extinctionRatioTable = alxGetExtinctionRatioTable();
    FAIL_NULL(extinctionRatioTable);

    /*
     * Computed apparent magnitudes.
     * C = Co + Ac
     * where Ac = Av * Rc/Rv, with Rv=3.10
     */
    mcsUINT32 band;
    for (band = alxB_BAND; band < alxNB_BANDS; band++)
    {
        if (alxIsSet(magnitudes[band]))
        {
            magnitudes[band].value += Av * extinctionRatioTable->coeff[band];
        }
    }

    /* Log */
    alxLogTestMagnitudes("Apparent magnitudes:", "", magnitudes);

    return mcsSUCCESS;
}

/**
 * Initialize this file
 */
void alxInterstellarAbsorptionInit(void)
{
    alxGetExtinctionRatioTable();
    alxGetPolynomialForInterstellarAbsorption();

#if 0
    /* Test all-sky Av range */
    logSetStdoutLogLevel(logDEBUG);

    mcsDOUBLE gLat, gLon;
    mcsDOUBLE Av, e_Av;
    /*
        mcsDOUBLE plx, e_plx;
     */

    /* invalid plx: HIP 85329 -0.03 1.20 */
    /*
    plx = -0.03;
    e_plx = 1.2;
    alxComputeExtinctionCoefficient(&Av, &e_Av, plx, e_plx, 0.0, 50.0);
     */
    /*
    AVs=0.000 [6.875 - 9.307] err=3.1023
    GLon/GLat=+050.000,+000.000, dist=0.000, Av=0.000 (3.1023)
     */


    /* worst JSDC (HIP2) case */
    /*
    plx = 1.25;
    e_plx = 0.58;
    alxComputeExtinctionCoefficient(&Av, &e_Av, plx, e_plx, 0.0, 50.0);
     */
    /*
    AVs=6.056 [2.939 - 9.307] err=1.0836
    GLon/GLat=+050.000,+000.000, dist=0.800, Av=6.056 (1.0836)
     */


    /* fake plx values (faint case) */
    mcsDOUBLE distances[3];
    distances[0] = 1.0 / 5.0;  /* nominal */
    distances[1] = 0.0;         /* min */
    distances[2] = 1.0;         /* max 1 kpc */

    alxComputeExtinctionCoefficientFromDistances(&Av, &e_Av, distances, 0.0, 50.0);


    // Use GLat [10; 90]
    for (gLat = 90.0; gLat >= 10.0; gLat -= 5.0)
    {
        alxComputeExtinctionCoefficientFromDistances(&Av, &e_Av, distances, gLat, 0.0);
    }

    // Use GLon [0; 360] corresponding to GLat[0;10]
    for (gLon = 5.0; gLon < 360.0; gLon += 10.0)
    {
        alxComputeExtinctionCoefficientFromDistances(&Av, &e_Av, distances, 0.0, gLon);
    }

    logSetStdoutLogLevel(logTEST);
#endif
}

/*___oOo___*/
