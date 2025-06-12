/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * @file
 * Function definition for angular diameter computation.
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


/** convenience macros */

/* log Level to dump covariance matrix and its inverse */
#define LOG_MATRIX logDEBUG


#define absError(diameter, relDiameterError) \
   (relDiameterError * diameter * LOG_10) /* absolute error */

#define relError(diameter, absDiameterError) \
   (absDiameterError / (diameter * LOG_10)) /* relative error */


/** alx dev flag */
static mcsLOGICAL alxDevFlag = mcsFALSE;

mcsLOGICAL alxGetDevFlag(void)
{
    return alxDevFlag;
}

void alxSetDevFlag(mcsLOGICAL flag)
{
    alxDevFlag = flag;
    logInfo("alxDevFlag: %s", IS_TRUE(alxDevFlag) ? "true" : "false");
}

/** low memory flag */
static mcsLOGICAL alxLowMemFlag = mcsFALSE;

mcsLOGICAL alxGetLowMemFlag(void)
{
    return alxLowMemFlag;
}

void alxSetLowMemFlag(mcsLOGICAL flag)
{
    alxLowMemFlag = flag;
    logInfo("alxLowMemFlag: %s", IS_TRUE(alxLowMemFlag) ? "true" : "false");
}

/** deprecated flag */
static mcsLOGICAL alxDeprecatedFlag = mcsFALSE;

mcsLOGICAL alxGetDeprecatedFlag(void)
{
    return alxDeprecatedFlag;
}

void alxSetDeprecatedFlag(mcsLOGICAL flag)
{
    alxDeprecatedFlag = flag;
    logInfo("alxDeprecatedFlag: %s", IS_TRUE(alxDeprecatedFlag) ? "true" : "false");
}

/*
 * Local functions definition
 */

/**
 * Return the polynomial coefficients for angular diameter computation and their covariance matrix
 *
 * @return pointer onto the structure containing polynomial coefficients, or
 * NULL if an error occurred.
 *
 * @usedfiles alxAngDiamPolynomial.cfg : file containing the polynomial coefficients to compute the angular diameters.
 * @usedfiles alxAngDiamPolynomialCovariance.cfg : file containing the covariance between polynomial coefficients to compute the angular diameter error.
 */
static alxPOLYNOMIAL_ANGULAR_DIAMETER *alxGetPolynomialForAngularDiameter(void)
{
    static alxPOLYNOMIAL_ANGULAR_DIAMETER polynomial = {mcsFALSE, "alxAngDiamPolynomial.cfg", "alxAngDiamPolynomialCovariance.cfg",
                                                        { 0},
        {
            { 0.0 }
        },
        {
            { 0.0 }
        }};

    /* Check if the structure is loaded into memory. If not load it. */
    if (IS_TRUE(polynomial.loaded))
    {
        return &polynomial;
    }

    /*
     * Build the dynamic buffer which will contain the coefficient file for angular diameter computation
     */
    /* Find the location of the file */
    char* fileName;
    fileName = miscLocateFile(polynomial.fileName);
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
    const mcsUINT32 maxLineLength = sizeof (line) - 1;
    mcsSTRING4 color;
    mcsINT32 c;
    mcsINT32 index;

    while (IS_NOT_NULL(pos = miscDynBufGetNextLine(&dynBuf, pos, line, maxLineLength, mcsTRUE)))
    {
        logTrace("miscDynBufGetNextLine()='%s'", line);

        /* If the current line is not empty */
        if (IS_FALSE(miscIsSpaceStr(line)))
        {
            /* Check if there is too many lines in file */
            if (lineNum >= alxNB_DIAMS)
            {
                miscDynBufDestroy(&dynBuf);
                errAdd(alxERR_TOO_MANY_LINES, fileName);
                free(fileName);
                return NULL;
            }

            /*
             * Read polynomial coefficients
             * #FIT COLORS:  V-J V-H V-K
             * #Color a0 a1 a2 a3 a4
             */
            if (sscanf(line, "%3s %lf %lf %lf %lf %lf",
                       color,
                       &polynomial.coeff[lineNum][0],
                       &polynomial.coeff[lineNum][1],
                       &polynomial.coeff[lineNum][2],
                       &polynomial.coeff[lineNum][3],
                       &polynomial.coeff[lineNum][4]
                       ) != (1 + alxNB_POLYNOMIAL_COEFF_DIAMETER))
            {
                miscDynBufDestroy(&dynBuf);
                errAdd(alxERR_WRONG_FILE_FORMAT, line, fileName);
                free(fileName);
                return NULL;
            }

            /* Count non zero coefficients and set nbCoeff */
            polynomial.nbCoeff[lineNum] = alxNB_POLYNOMIAL_COEFF_DIAMETER;
            for (c = 0; c < alxNB_POLYNOMIAL_COEFF_DIAMETER; c++)
            {
                if (polynomial.coeff[lineNum][c] == 0.0)
                {
                    c++;
                    logDebug("Color '%s' nb coeff Diam=%d", alxGetDiamLabel(lineNum), c);
                    polynomial.nbCoeff[lineNum] = c;
                    break;
                }
            }

            if (strcmp(color, alxGetDiamLabel(lineNum)) != 0)
            {
                logError("Color index mismatch: '%s' (expected '%s')", color, alxGetDiamLabel(lineNum));
                miscDynBufDestroy(&dynBuf);
                errAdd(alxERR_WRONG_FILE_FORMAT, line, fileName);
                free(fileName);
                return NULL;
            }

            /* Next line */
            lineNum++;
        }
    }

    /* Check if there is missing line */
    if (lineNum != alxNB_DIAMS)
    {
        miscDynBufDestroy(&dynBuf);
        errAdd(alxERR_MISSING_LINE, lineNum, alxNB_DIAMS, fileName);
        free(fileName);
        return NULL;
    }
    free(fileName);

    /*
     * Build the dynamic buffer which will contain the covariance matrix file for angular diameter error computation
     */
    /* Find the location of the file */
    fileName = miscLocateFile(polynomial.fileNameCov);
    if (IS_NULL(fileName))
    {
        miscDynBufDestroy(&dynBuf);
        return NULL;
    }

    /* Load file. Comment lines start with '#' */
    NULL_(miscDynBufReset(&dynBuf));

    logInfo("Loading %s ...", fileName);

    NULL_DO(miscDynBufLoadFile(&dynBuf, fileName, "#"),
            miscDynBufDestroy(&dynBuf);
            free(fileName));

    /* For each line of the loaded file */
    lineNum = 0;
    pos = NULL;

    mcsDOUBLE* polynomCoefCovLine;

    while (IS_NOT_NULL(pos = miscDynBufGetNextLine(&dynBuf, pos, line, maxLineLength, mcsTRUE)))
    {
        logTrace("miscDynBufGetNextLine()='%s'", line);

        /* If the current line is not empty */
        if (IS_FALSE(miscIsSpaceStr(line)))
        {
            /* Check if there is too many lines in file */
            if (lineNum >= alxNB_POLYNOMIAL_COEFF_COVARIANCE)
            {
                miscDynBufDestroy(&dynBuf);
                errAdd(alxERR_TOO_MANY_LINES, fileName);
                free(fileName);
                return NULL;
            }

            /*
             * Read the Covariance matrix of polynomial coefficients [a0ij...a1ij][a0ij...a1ij]
             *
             * MCOV_POL matrix from IDL [21 x 21] for 3 colors and 7 polynomial coefficients (a0...a6)
             *
             */
            polynomCoefCovLine = polynomial.polynomCoefCovMatrix[lineNum];

            if (sscanf(line, "%d %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf",
                       &index,
                       &polynomCoefCovLine[ 0], &polynomCoefCovLine[ 1],
                       &polynomCoefCovLine[ 2], &polynomCoefCovLine[ 3],
                       &polynomCoefCovLine[ 4], &polynomCoefCovLine[ 5],
                       &polynomCoefCovLine[ 6], &polynomCoefCovLine[ 7],
                       &polynomCoefCovLine[ 8], &polynomCoefCovLine[ 9],
                       &polynomCoefCovLine[10], &polynomCoefCovLine[11],
                       &polynomCoefCovLine[12], &polynomCoefCovLine[13],
                       &polynomCoefCovLine[14], &polynomCoefCovLine[15],
                       &polynomCoefCovLine[16], &polynomCoefCovLine[17],
                       &polynomCoefCovLine[18], &polynomCoefCovLine[19],
                       &polynomCoefCovLine[20]
                       ) != (1 + alxNB_POLYNOMIAL_COEFF_COVARIANCE))
            {
                miscDynBufDestroy(&dynBuf);
                errAdd(alxERR_WRONG_FILE_FORMAT, line, fileName);
                free(fileName);
                return NULL;
            }

            if (lineNum != index)
            {
                logError("Line index mismatch: '%d' (expected '%d')", index, lineNum);
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

    /* Check if there are missing lines */
    if (lineNum != alxNB_POLYNOMIAL_COEFF_COVARIANCE)
    {
        errAdd(alxERR_MISSING_LINE, lineNum, alxNB_POLYNOMIAL_COEFF_COVARIANCE, fileName);
        free(fileName);
        return NULL;
    }
    free(fileName);

    /* Specify that the polynomial has been loaded */
    polynomial.loaded = mcsTRUE;

    return &polynomial;
}

/**
 * Compute am angular diameters for a given color-index based
 * on the coefficients from table. If a magnitude is not set,
 * the diameter is not computed.
 *
 * @param mA is the first input magnitude of the color index
 * @param mB is the second input magnitude of the color index
 * @param polynomial coefficients for angular diameter
 * @param color is the line corresponding to the color index A-B
 * @param diam is the structure to get back the computation
 */
void alxComputeDiameter(alxDATA mI,
                        alxDATA mJ,
                        mcsDOUBLE spTypeIndex,
                        alxPOLYNOMIAL_ANGULAR_DIAMETER *polynomial,
                        mcsDOUBLE cmI,
                        mcsDOUBLE cmJ,
                        mcsUINT32 color,
                        alxDATA *diam)
{
    /*
     * Alain Chelli's new diameter and error computation (03/2015)
     */
    mcsUINT32 nbCoeffs    = polynomial->nbCoeff[color];
    mcsDOUBLE *diamCoeffs = polynomial->coeff[color];

    /* optimize pow(spTypeIndex, n) calls by pre-computing values [0; n] */
    mcsDOUBLE pows[nbCoeffs];
    alxComputePows(nbCoeffs, spTypeIndex, pows); /* pows is the serie of (sp)^[0..n] */


    /* Compute the angular diameter */
    /*
     * diameterIJ = 10^[ PIJ(sp) + 0.2 x [ CmJ . MI - CmI . MJ ] ]
     * Log10(diameterIJ) = PIJ(sp) + 0.2 x [ CmJ . MI - CmI . MJ ]
     */
    mcsDOUBLE pIJ = alxComputePolynomial(nbCoeffs, diamCoeffs, pows);
    diam->value   = alxPow10(pIJ + 0.2 * (cmJ * mI.value - cmI * mJ.value));

    /* Compute diameter error */

    /*
     * var(Log10(diameterIJ)) = term_an + term_sp + term_M where I=K and J=L
     * term_sp = 0
     * term_an = ΣΣ cov( ax_IJ, ay_IJ) . sp^x . sp^y where x € [0..4] and y € [0..4]
     * term_M = 0.04 x [ CmJ^2 . var ( MI ) + CmI^2 . var ( MJ ) ]
     */
    mcsUINT32 offsetIJ = color * alxNB_POLYNOMIAL_COEFF_DIAMETER;

    mcsDOUBLE term_an = 0.0;
    mcsUINT32 x, y;
    mcsDOUBLE* row;

    for (x = 0; x < nbCoeffs; x++)
    {
        row = polynomial->polynomCoefCovMatrix[offsetIJ + x];

        for (y = 0; y < nbCoeffs; y++)
        {
            term_an += row[offsetIJ + y] * pows[x] * pows[y];
        }
    }
    mcsDOUBLE term_M  = 0.04 * ( alxSquare(cmJ * mI.error) + alxSquare(cmI * mJ.error) );

    diam->error = term_an + term_M; /* relative variance (normal distribution) */

    /* Convert log normal diameter distribution to normal distribution */
    /* EDIAM_C(II,*)=DIAM_C(II,*)*SQRT(EDIAM_C(II,*))*ALOG(10) ; error of output diameter */
    diam->error = absError(diam->value, sqrt(diam->error));

    diam->confIndex = alxCONFIDENCE_HIGH;
    diam->isSet = mcsTRUE;

    logDebug("Diameter %s = %.4lf(%.4lf %.1lf%%) for magA=%.3lf(%.3lf) magB=%.3lf(%.3lf)",
             alxGetDiamLabel(color),
             diam->value, diam->error, alxDATALogRelError(*diam),
             mI.value, mI.error, mJ.value, mJ.error
             );
}

mcsCOMPL_STAT alxComputeDiameterWithMagErr(alxDATA mA,
                                           alxDATA mB,
                                           mcsDOUBLE spTypeIndex,
                                           alxPOLYNOMIAL_ANGULAR_DIAMETER *polynomial,
                                           mcsDOUBLE cmI,
                                           mcsDOUBLE cmJ,
                                           mcsUINT32 color,
                                           alxDATA *diam)
{
    /* If any magnitude is not available, then the diameter is not computed. */
    SUCCESS_COND_DO((alxIsNotSet(mA) || alxIsNotSet(mB)),
                    alxDATAClear((*diam)));

    /** compute diameter and its error */
    alxComputeDiameter(mA, mB, spTypeIndex, polynomial, cmI, cmJ, color, diam);

    /* If diameter is not computed (domain check), return */
    SUCCESS_COND(IS_FALSE(diam->isSet));

    /* Set confidence as the smallest confidence of the two magnitudes */
    diam->confIndex = (mA.confIndex <= mB.confIndex) ? mA.confIndex : mB.confIndex;

    /* If any missing magnitude error (should not happen as def error if missing),
     * return diameter with low confidence */
    if ((mA.error <= 0.0) || (mB.error <= 0.0))
    {
        diam->confIndex = alxCONFIDENCE_LOW;
    }

    return mcsSUCCESS;
}

/*
 * Public functions definition
 */

/**
 * Compute stellar angular diameters from its photometric magnitudes already corrected by interstellar absorption.
 *
 * @param magnitudes B V R Ic J H K L M N (Johnson / 2MASS / WISE)
 * @param spTypeIndex spectral type as double
 * @param diameters the structure to give back all the computed diameters
 * @param diametersCov the structure to give back all the covariance matrix of computed diameters
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
 */
mcsCOMPL_STAT alxComputeAngularDiameters(const char* msg,
                                         alxMAGNITUDES magnitudes,
                                         mcsDOUBLE spTypeIndex,
                                         alxDIAMETERS diameters,
                                         alxDIAMETERS_COVARIANCE diametersCov,
                                         logLEVEL logLevel)
{
    /* Get polynomial for diameter computation */
    alxPOLYNOMIAL_ANGULAR_DIAMETER *polynomial;
    polynomial = alxGetPolynomialForAngularDiameter();
    FAIL_NULL(polynomial);

    /* Get extinction ratio table CF */
    alxEXTINCTION_RATIO_TABLE *extinctionRatioTable;
    extinctionRatioTable = alxGetExtinctionRatioTable();
    FAIL_NULL(extinctionRatioTable);

    /* Compute diameters and the covariance matrix of computed diameters */
    mcsUINT32 i, j, nI, nJ, nK, nL;
    alxDATA mI, mJ;
    mcsDOUBLE varMI, varMJ;
    mcsDOUBLE term_an, term_M;
    mcsUINT32 x, y;
    mcsDOUBLE* row;
    mcsUINT32 offsetIJ, offsetKL;

    /* optimize pow(spTypeIndex, n) calls by pre-computing values [0; n] */
    mcsUINT32 nbCoeffs = alxNB_POLYNOMIAL_COEFF_DIAMETER;
    mcsDOUBLE pows[nbCoeffs];
    alxComputePows(nbCoeffs, spTypeIndex, pows); /* pows is the serie of (sp)^[0..n] */

    const mcsDOUBLE *avCoeffs = extinctionRatioTable->coeff;
    mcsDOUBLE cmI[alxNB_DIAMS];
    mcsDOUBLE cmJ[alxNB_DIAMS];

    for (i = 0; i < alxNB_DIAMS; i++) /* II */
    {
        /* NI=NBI(II) & MI=MAG_C(Z,NI) & EMI=EMAG_C(Z,NI) & NJ=NBJ(II) & MJ=MAG_C(Z,NJ) & EMJ=EMAG_C(Z,NJ) ; magnitudes & errors */
        nI = alxDIAM_BAND_A[i]; /* NI */
        nJ = alxDIAM_BAND_B[i]; /* NJ */

        /* CmI = CI / (CI – CJ) */
        cmI[i] = avCoeffs[nI] / (avCoeffs[nI] - avCoeffs[nJ]);
        /* CmJ = CJ / (CI – CJ) */
        cmJ[i] = avCoeffs[nJ] / (avCoeffs[nI] - avCoeffs[nJ]);

        mI = magnitudes[nI]; /* MI / EMI */
        mJ = magnitudes[nJ]; /* MJ / EMJ */

        alxComputeDiameterWithMagErr(mI, mJ, spTypeIndex, polynomial, cmI[i], cmJ[i], i, &diameters[i]);

        /* fill the covariance matrix */
        /*
         * Only use diameters with HIGH confidence
         * ie computed from catalog magnitudes and not interpolated magnitudes.
         */
        if (isDiameterValid(diameters[i]))
        {
            varMI = alxSquare(mI.error);   /* EMI^2 */
            varMJ = alxSquare(mJ.error);   /* EMJ^2 */

            for (j = 0; j <= i; j++) /* JJ */
            {
                /* NK=NBI(JJ) & NL=NBJ(JJ) */
                nK = alxDIAM_BAND_A[j]; /* NK */
                nL = alxDIAM_BAND_B[j]; /* NL */

                /*
                 * cov(Log10(diameterIJ), Log10(diameterKL)) = cov(Log10(diameterKL), Log10(diameterIJ))
                 * = cov ( PIJ(sp) + 0.2 x [ CmJ . MI - CmI . MJ ], PKL(sp) + 0.2 x [ CmL . MK - CmK . ML ] )
                 * = term_an + term_sp + term_M
                 *
                 * term_sp = 0
                 * term_an = ΣΣ cov( ax_IJ, ay_KL) . sp^x . sp^y where x € [0..4] and y € [0..4]
                 */
                offsetIJ = i * alxNB_POLYNOMIAL_COEFF_DIAMETER;
                offsetKL = j * alxNB_POLYNOMIAL_COEFF_DIAMETER;
                term_an = 0.0;

                for (x = 0; x < nbCoeffs; x++)
                {
                    row = polynomial->polynomCoefCovMatrix[offsetIJ + x];

                    for (y = 0; y < nbCoeffs; y++)
                    {
                        term_an += row[offsetKL + y] * pows[x] * pows[y];
                    }
                }

                /*
                 * term_M = 0.04 x [ CmJ . CmL . cov ( MI, MK ) + CmI . CmK . cov ( MJ, ML ) ]
                 */
                if (nI == nK)
                {
                    if (nJ == nL)
                    {
                        /*
                         * I = K and J = L :
                         * term_M = 0.04 x [ CmJ^2 . var ( MI ) + CmI^2 . var ( MJ ) ]
                         */
                        term_M  = 0.04 * ( alxSquare(cmJ[i]) * varMI + alxSquare(cmI[i]) * varMJ );
                    }
                    else
                    {
                        /*
                         * I = K and J ≠ L :
                         * term_M = 0.04 x [ CmJ . CmL . var ( MI ) ]
                         */
                        term_M  = 0.04 * ( cmJ[i] * cmJ[j] * varMI);
                    }
                }
                else if (nJ == nL)
                {
                    /*
                     * I ≠ K and J = L :
                     * term_M = 0.04 x [ CmI . CmK . var ( MJ ) ]
                     */
                    term_M  = 0.04 * ( cmI[i] * cmI[j] * varMJ );
                }
                else
                {
                    /*
                     * no common magnitude:
                     * term_M = 0
                     */
                    term_M = 0.0;
                }

                /* DCOV_C(JJ,II,*)=DCOV_C(II,JJ,*) */
                diametersCov[i][j] = term_an + term_M;
                diametersCov[j][i] = term_an + term_M;
            }
        } /* diameter is valid */
        else
        {
            /* fill the covariance matrix with NAN */
            for (j = 0; j <= i; j++) /* JJ */
            {
                /* DCOV_C(JJ,II,*)=DCOV_C(II,JJ,*) */
                diametersCov[j][i] = diametersCov[i][j] = NAN;
            }
        }
    }

    alxLogAngularDiameters(msg, diameters, logLevel);

    if (doLog(LOG_MATRIX))
    {
        static const mcsDOUBLE nSigma = 3.0;

        mcsDOUBLE diametersMin[alxNB_DIAMS];
        mcsDOUBLE diametersMax[alxNB_DIAMS];

        for (i = 0; i < alxNB_DIAMS; i++)
        {
            mcsDOUBLE diam = diameters[i].value;
            mcsDOUBLE relError = relError(diam, diameters[i].error);

            /* min/max diameters using 3 sigma (log-normal) */
            diametersMin[i] = alxPow10(log10(diam) - nSigma * relError);
            diametersMax[i] = alxPow10(log10(diam) + nSigma * relError);
        }

        logP(LOG_MATRIX, "Diameter %s V-J=%.4lf(%.4lf %.4lf) V-H=%.4lf(%.4lf %.4lf) V-K=%.4lf(%.4lf %.4lf)", msg,
             diameters[alxV_J_DIAM].value, diametersMin[alxV_J_DIAM], diametersMax[alxV_J_DIAM],
             diameters[alxV_H_DIAM].value, diametersMin[alxV_H_DIAM], diametersMax[alxV_H_DIAM],
             diameters[alxV_K_DIAM].value, diametersMin[alxV_K_DIAM], diametersMax[alxV_K_DIAM]
             );
    }

    return mcsSUCCESS;
}

mcsCOMPL_STAT alxComputeMeanAngularDiameter(alxDIAMETERS diameters,
                                            alxDIAMETERS_COVARIANCE diametersCov,
                                            mcsUINT32    nbRequiredDiameters,
                                            alxDATA     *weightedMeanDiam,
                                            alxDATA     *chi2Diam,
                                            mcsUINT32   *nbDiameters,
                                            miscDYN_BUF *diamInfo,
                                            logLEVEL     logLevel)
{
    /*
     * Only use diameters with HIGH confidence
     * ie computed from catalog magnitudes and not interpolated magnitudes.
     */

    /* initialize structures */
    alxDATAClear((*weightedMeanDiam));
    alxDATAClear((*chi2Diam));

    mcsUINT32   color;
    mcsSTRING32 tmp;
    alxDATA     diameter;

    /* valid diameters to compute weighted mean diameter and their errors */
    mcsUINT32 nValidDiameters;
    mcsDOUBLE validDiams[alxNB_DIAMS];
    mcsUINT32 validDiamsBand[alxNB_DIAMS];

    mcsDOUBLE residuals[alxNB_DIAMS];
    mcsDOUBLE chi2 = NAN;


    alxCONFIDENCE_INDEX weightedMeanDiamConfidence = alxCONFIDENCE_HIGH;

    /* Count only valid diameters and copy data into diameter arrays */
    for (nValidDiameters = 0, color = 0; color < alxNB_DIAMS; color++)
    {
        diameter  = diameters[color];

        if (isDiameterValid(diameter))
        {
            if (diameter.confIndex < weightedMeanDiamConfidence)
            {
                weightedMeanDiamConfidence = diameter.confIndex; /* min (all diameters) */
            }
            validDiamsBand[nValidDiameters]     = color;

            /* convert diameter and error to log(diameter) and relative error */
            validDiams[nValidDiameters]         = log10(diameter.value);                    /* ALOG10(DIAM_C) */
            nValidDiameters++;
        }
    }

    /* Set the diameter count */
    *nbDiameters = nValidDiameters;


    /* if less than required diameters, can not compute mean diameter... */
    if (nValidDiameters < nbRequiredDiameters)
    {
        logTest("Cannot compute mean diameter (%d < %d valid diameters)", nValidDiameters, nbRequiredDiameters);

        if (IS_NOT_NULL(diamInfo))
        {
            /* Set diameter flag information */
            snprintf(tmp, mcsLEN32 - 1, "REQUIRED[%1u]: %1u", nbRequiredDiameters, nValidDiameters);
            FAIL(miscDynBufAppendString(diamInfo, tmp));
        }

        return mcsSUCCESS;
    }

    mcsUINT32 i, j;

    /* Compute weighted mean diameter and its relative error */
    mcsDOUBLE icov[alxNB_DIAMS * alxNB_DIAMS];

    /* populate covariance matrix[nValidDiameters x nValidDiameters] with the covariance matrix of VALID diameters */
    for (i = 0; i < nValidDiameters; i++)
    {
        for (j = 0; j < nValidDiameters; j++)
        {
            icov[i * nValidDiameters + j] = diametersCov[validDiamsBand[i]][validDiamsBand[j]];
        }
    }

    /* invert covariance matrix */
    /* M=INVERT(DCOV_C(*,*,II),/DOUBLE) */

    if (alxInvertMatrix(icov, nValidDiameters) == mcsFAILURE)
    {
        logError("Cannot invert covariance matrix (%d diameters)", nValidDiameters);
    }
    else
    {
        mcsDOUBLE total_icov = alxTotal(nValidDiameters * nValidDiameters, icov); /* TOTAL(M) */

        /* IF (TOTAL(M) GT 0) THEN BEGIN */
        if (total_icov > 0.0)
        {
            chi2Diam->confIndex = alxCONFIDENCE_HIGH;

            mcsDOUBLE matrix_prod[nValidDiameters];

            weightedMeanDiam->isSet = mcsTRUE;
            weightedMeanDiam->confIndex = weightedMeanDiamConfidence;

            /* compute icov#diameters
             * A=TOTAL(ALOG10(DIAM_C(II,*)),1)
             * matrix_prod = M#A */
            alxProductMatrix(icov, validDiams, matrix_prod, nValidDiameters, nValidDiameters, 1);

            /* DMEAN_C(II)=TOTAL(M#A)/TOTAL(M) */
            weightedMeanDiam->value = alxTotal(nValidDiameters, matrix_prod) / total_icov;

            /* EDMEAN_C(II)=1./SQRT(TOTAL(M)) */
            const mcsDOUBLE weightedMeanDiamVariance = 1.0 / total_icov;
            /* correct error: */
            weightedMeanDiam->error = sqrt(weightedMeanDiamVariance);


            /* compute chi2 on mean diameter estimates */
            /* CHI2_DS(WWS(II))=DIF#ICOV#TRANSPOSE(DIF)/NBD */

            for (i = 0; i < nValidDiameters; i++)
            {
                /* DIF=DMEAN_C(WWS(II))-DIAM_C(WWS(II),*) */
                residuals[i] = fabs(weightedMeanDiam->value - validDiams[i]);
            }

            /* compute M=ICOV#TRANSPOSE(DIF) */
            alxProductMatrix(icov, residuals, matrix_prod, nValidDiameters, nValidDiameters, 1);

            /* compute CHI2_DS(II)=DIF#M / NBD */
            mcsDOUBLE matrix_11[1];
            alxProductMatrix(residuals, matrix_prod, matrix_11, 1, nValidDiameters, 1);
            /* reduced chi2 = chi2 / nDiameters */
            chi2 = matrix_11[0] / nValidDiameters;
            
            /* Check if chi2 < 5
             * If higher i.e. inconsistency is found, the weighted mean diameter has a LOW confidence */
            if (chi2 > DIAM_CHI2_THRESHOLD)
            {
                /* Set confidence to LOW */
                weightedMeanDiam->confIndex = alxCONFIDENCE_LOW;
                chi2Diam->confIndex = alxCONFIDENCE_LOW;

                if (IS_NOT_NULL(diamInfo))
                {
                    /* Update diameter flag information */
                    FAIL(miscDynBufAppendString(diamInfo, "INCONSISTENT"));
                }
            }
        }

    } /* matrix invert OK */

    /* Convert log normal diameter distributions to normal distributions */
    if (alxIsSet(*weightedMeanDiam))
    {
        weightedMeanDiam->value = alxPow10(weightedMeanDiam->value);
        weightedMeanDiam->error = absError(weightedMeanDiam->value, weightedMeanDiam->error);
    }

    if (!isnan(chi2))
    {
        chi2Diam->isSet = mcsTRUE;
        chi2Diam->value = chi2;
    }

    if (doLog(logLevel))
    {
        logP(logLevel,
             "Diameter weighted=%.4lf(%.4lf %.1lf%%) valid=%s [%s] chi2=%.4lf from %d diameters: %s",
             weightedMeanDiam->value, weightedMeanDiam->error, alxDATALogRelError(*weightedMeanDiam),
             (weightedMeanDiam->confIndex == alxCONFIDENCE_HIGH) ? "true" : "false",
             alxGetConfidenceIndex(weightedMeanDiam->confIndex),
             chi2, nValidDiameters,
             IS_NOT_NULL(diamInfo) ? miscDynBufGetBuffer(diamInfo) : "");
    }

    return mcsSUCCESS;
}

/**
 * Initialize this file
 */
void alxAngularDiameterInit(void)
{
    if (IS_NULL(alxGetPolynomialForAngularDiameter()))
    {
        logError("Unable to load polynomial coefficients (alxGetPolynomialForAngularDiameter)");
        errCloseStack();
        exit(1);
    }

    /* enable / disable regression tests */
    /* TESTED @ 2017/03/31: OK */
    if (0)
    {
        /* 3 diameters are required: */
        static const mcsUINT32 nbRequiredDiameters = 3;

        static const mcsDOUBLE DELTA_TH = 2e-8;
        static const mcsDOUBLE DELTA_SIG = 0.01; /* 1 sigma */

        /*
#REF STAR DATA:         573
         */
#define IDL_NS 573
#define IDL_COL_SP 1
#define IDL_COL_MAG 2
#define IDL_COL_EMAG 8
#define IDL_COL_DIAM 14
#define IDL_COL_EDIAM 17
#define IDL_COL_MEAN 20
#define IDL_COL_EMEAN 21

#define NaN NAN

        mcsUINT32 NS = IDL_NS;
        mcsDOUBLE datas[IDL_NS][IDL_COL_EMEAN + 1] = {
            {0, /* SP "K5III             " idx */ 220, /*MAG*/  2.408000000e+00,  8.700000000e-01, -8.000000000e-01, -2.095000000e+00, -2.775000000e+00, -3.044000000e+00, /*EMAG*/  1.360147000e-02,  4.000000000e-02,  2.282542000e-02,  1.940000000e-01,  1.940000000e-01,  1.940000000e-01, /*DIAM*/  1.362718968e+00,  1.350325100e+00,  1.364649481e+00, /*EDIAM*/  5.393608421e-02,  4.696724666e-02,  4.404144133e-02, /*DMEAN*/  1.359197387e+00, /*EDMEAN*/  2.766262845e-02 },
            {1, /* SP "M1.5Iab-Ib        " idx */ 246, /*MAG*/  2.925000000e+00,  1.060000000e+00, -1.840000000e+00, -2.850000000e+00, -3.725000000e+00, -4.100000000e+00, /*EMAG*/  1.612452000e-02,  4.000000000e-02,  3.104835000e-02,  1.600000000e-01,  1.600000000e-01,  1.600000000e-01, /*DIAM*/  1.593469321e+00,  1.617192442e+00,  1.642031734e+00, /*EDIAM*/  4.455635240e-02,  3.878378685e-02,  3.636472116e-02, /*DMEAN*/  1.620772301e+00, /*EDMEAN*/  2.288212648e-02 },
            {2, /* SP "M1.5Iab-Ib        " idx */ 246, /*MAG*/  2.925000000e+00,  1.060000000e+00, -1.840000000e+00, -2.850000000e+00, -3.725000000e+00, -4.100000000e+00, /*EMAG*/  1.612452000e-02,  4.000000000e-02,  3.104835000e-02,  1.600000000e-01,  1.600000000e-01,  1.600000000e-01, /*DIAM*/  1.593469321e+00,  1.617192442e+00,  1.642031734e+00, /*EDIAM*/  4.455635240e-02,  3.878378685e-02,  3.636472116e-02, /*DMEAN*/  1.620772301e+00, /*EDMEAN*/  2.288212648e-02 },
            {3, /* SP "M1III             " idx */ 244, /*MAG*/  5.541000000e+00,  4.040000000e+00,  2.250000000e+00,  1.176000000e+00,  3.250000000e-01,  1.570000000e-01, /*EMAG*/  3.224903000e-02,  4.000000000e-02,  2.039608000e-02,  2.600000000e-01,  2.600000000e-01,  2.600000000e-01, /*DIAM*/  7.098010234e-01,  7.611549882e-01,  7.546315769e-01, /*EDIAM*/  7.213259049e-02,  6.283849576e-02,  5.893167301e-02, /*DMEAN*/  7.451461325e-01, /*EDMEAN*/  3.697896716e-02 },
            {4, /* SP "A1V+DA            " idx */ 84, /*MAG*/ -1.431000000e+00, -1.440000000e+00, -1.420000000e+00, -1.391000000e+00, -1.391000000e+00, -1.390000000e+00, /*EMAG*/              NaN,  4.000000000e-02,              NaN,  2.140000000e-01,  2.140000000e-01,  2.140000000e-01, /*DIAM*/  7.985336326e-01,  7.943958963e-01,  7.820068592e-01, /*EDIAM*/  6.071313613e-02,  5.290081805e-02,  4.962104806e-02, /*DMEAN*/  7.906093284e-01, /*EDMEAN*/  3.125711247e-02 },
            {5, /* SP "F5IV-V+DQZ        " idx */ 140, /*MAG*/  8.320000000e-01,  4.000000000e-01, -9.000000000e-02, -4.980000000e-01, -6.660000000e-01, -6.580000000e-01, /*EMAG*/              NaN,  4.000000000e-02,              NaN,  3.220000000e-01,  3.220000000e-01,  3.220000000e-01, /*DIAM*/  7.662067463e-01,  7.585438901e-01,  7.401722852e-01, /*EDIAM*/  8.949010692e-02,  7.797484477e-02,  7.313066944e-02, /*DMEAN*/  7.533370204e-01, /*EDMEAN*/  4.586424020e-02 },
            {6, /* SP "M1III             " idx */ 244, /*MAG*/  5.765000000e+00,  4.220000000e+00,  2.330000000e+00,  1.088000000e+00,  1.690000000e-01, -9.900000000e-02, /*EMAG*/  3.712142000e-02,  4.000000000e-02,  2.022375000e-02,  3.500000000e-01,  3.500000000e-01,  3.500000000e-01, /*DIAM*/  7.479795954e-01,  8.062133547e-01,  8.172885121e-01, /*EDIAM*/  9.700009136e-02,  8.452108271e-02,  7.927106396e-02, /*DMEAN*/  7.953027780e-01, /*EDMEAN*/  4.970517791e-02 },
            {7, /* SP "M1III             " idx */ 244, /*MAG*/  6.797000000e+00,  5.150000000e+00,  3.130000000e+00,  1.839000000e+00,  9.960000000e-01,  8.340000000e-01, /*EMAG*/  1.300000000e-02,  4.000000000e-02,  5.024938000e-02,  3.500000000e-01,  3.500000000e-01,  3.500000000e-01, /*DIAM*/  6.115242362e-01,  6.450616013e-01,  6.306096772e-01, /*EDIAM*/  9.700009136e-02,  8.452108271e-02,  7.927106396e-02, /*DMEAN*/  6.305987995e-01, /*EDMEAN*/  4.970517791e-02 },
            {8, /* SP "M3IIIab           " idx */ 252, /*MAG*/  4.491000000e+00,  2.870000000e+00,  5.700000000e-01, -9.120000000e-01, -1.608000000e+00, -1.862000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  5.035871000e-02,  1.900000000e-01,  1.900000000e-01,  1.900000000e-01, /*DIAM*/  1.187201401e+00,  1.186128412e+00,  1.190184754e+00, /*EDIAM*/  5.286076286e-02,  4.603224089e-02,  4.316701131e-02, /*DMEAN*/  1.188003827e+00, /*EDMEAN*/  2.714127475e-02 },
            {9, /* SP "M3IIIab           " idx */ 252, /*MAG*/  4.491000000e+00,  2.870000000e+00,  5.700000000e-01, -9.120000000e-01, -1.608000000e+00, -1.862000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  5.035871000e-02,  1.900000000e-01,  1.900000000e-01,  1.900000000e-01, /*DIAM*/  1.187201401e+00,  1.186128412e+00,  1.190184754e+00, /*EDIAM*/  5.286076286e-02,  4.603224089e-02,  4.316701131e-02, /*DMEAN*/  1.188003827e+00, /*EDMEAN*/  2.714127475e-02 },
            {10, /* SP "K5III             " idx */ 220, /*MAG*/  2.408000000e+00,  8.700000000e-01, -8.000000000e-01, -2.095000000e+00, -2.775000000e+00, -3.044000000e+00, /*EMAG*/  1.360147000e-02,  4.000000000e-02,  2.282542000e-02,  1.940000000e-01,  1.940000000e-01,  1.940000000e-01, /*DIAM*/  1.362718968e+00,  1.350325100e+00,  1.364649481e+00, /*EDIAM*/  5.393608421e-02,  4.696724666e-02,  4.404144133e-02, /*DMEAN*/  1.359197387e+00, /*EDMEAN*/  2.766262845e-02 },
            {11, /* SP "B2V               " idx */ 48, /*MAG*/  1.416000000e+00,  1.640000000e+00,  1.860000000e+00,  2.149000000e+00,  2.357000000e+00,  2.375000000e+00, /*EMAG*/  1.131371000e-02,  4.000000000e-02,  2.154066000e-02,  2.820000000e-01,  2.820000000e-01,  2.820000000e-01, /*DIAM*/ -1.513736636e-01, -1.546627524e-01, -1.451335799e-01, /*EDIAM*/  7.952310608e-02,  6.954587471e-02,  6.537253368e-02, /*DMEAN*/ -1.500577927e-01, /*EDMEAN*/  4.220417784e-02 },
            {12, /* SP "B0.5Ia            " idx */ 42, /*MAG*/  1.902000000e+00,  2.070000000e+00,  2.210000000e+00,  2.469000000e+00,  2.686000000e+00,  2.679000000e+00, /*EMAG*/  2.729469000e-02,  4.000000000e-02,  2.039608000e-02,  2.940000000e-01,  2.940000000e-01,  2.940000000e-01, /*DIAM*/ -2.686504411e-01, -2.642896998e-01, -2.438624260e-01, /*EDIAM*/  8.402126680e-02,  7.359468121e-02,  6.924272932e-02, /*DMEAN*/ -2.574047076e-01, /*EDMEAN*/  4.519590777e-02 },
            {13, /* SP "A9II              " idx */ 116, /*MAG*/ -4.560000000e-01, -6.200000000e-01, -8.500000000e-01, -1.169000000e+00, -1.280000000e+00, -1.304000000e+00, /*EMAG*/  2.334524000e-02,  4.000000000e-02,  2.561250000e-02,  2.480000000e-01,  2.480000000e-01,  2.480000000e-01, /*DIAM*/  8.540521563e-01,  8.517282750e-01,  8.397966179e-01, /*EDIAM*/  6.946134615e-02,  6.051277558e-02,  5.675111234e-02, /*DMEAN*/  8.476491035e-01, /*EDMEAN*/  3.562698022e-02 },
            {14, /* SP "A1V+DA            " idx */ 84, /*MAG*/ -1.431000000e+00, -1.440000000e+00, -1.420000000e+00, -1.391000000e+00, -1.391000000e+00, -1.390000000e+00, /*EMAG*/              NaN,  4.000000000e-02,              NaN,  2.140000000e-01,  2.140000000e-01,  2.140000000e-01, /*DIAM*/  7.985336326e-01,  7.943958963e-01,  7.820068592e-01, /*EDIAM*/  6.071313613e-02,  5.290081805e-02,  4.962104806e-02, /*DMEAN*/  7.906093284e-01, /*EDMEAN*/  3.125711247e-02 },
            {15, /* SP "B1.5II            " idx */ 46, /*MAG*/  1.289000000e+00,  1.500000000e+00,  1.700000000e+00,  1.977000000e+00,  2.158000000e+00,  2.222000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  2.061553000e-02,  3.080000000e-01,  3.080000000e-01,  3.080000000e-01, /*DIAM*/ -1.339851537e-01, -1.276716638e-01, -1.273506218e-01, /*EDIAM*/  8.689860534e-02,  7.599392237e-02,  7.143101928e-02, /*DMEAN*/ -1.291986133e-01, /*EDMEAN*/  4.607522223e-02 },
            {16, /* SP "F8Ia              " idx */ 152, /*MAG*/  2.501000000e+00,  1.830000000e+00,  1.160000000e+00,  7.710000000e-01,  5.370000000e-01,  4.530000000e-01, /*EMAG*/  1.923538000e-02,  4.000000000e-02,  2.022375000e-02,  3.340000000e-01,  3.340000000e-01,  3.340000000e-01, /*DIAM*/  5.362390204e-01,  5.321081682e-01,  5.341404507e-01, /*EDIAM*/  9.273663800e-02,  8.080553285e-02,  7.578625911e-02, /*DMEAN*/  5.339885637e-01, /*EDMEAN*/  4.752779396e-02 },
            {17, /* SP "F5IV-V+DQZ        " idx */ 140, /*MAG*/  8.320000000e-01,  4.000000000e-01, -9.000000000e-02, -4.980000000e-01, -6.660000000e-01, -6.580000000e-01, /*EMAG*/              NaN,  4.000000000e-02,              NaN,  3.220000000e-01,  3.220000000e-01,  3.220000000e-01, /*DIAM*/  7.662067463e-01,  7.585438901e-01,  7.401722852e-01, /*EDIAM*/  8.949010692e-02,  7.797484477e-02,  7.313066944e-02, /*DMEAN*/  7.533370204e-01, /*EDMEAN*/  4.586424020e-02 },
            {18, /* SP "B8III             " idx */ 72, /*MAG*/  2.473000000e+00,  2.580000000e+00,  2.680000000e+00,  2.848000000e+00,  2.975000000e+00,  2.945000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  2.022375000e-02,  2.880000000e-01,  2.880000000e-01,  2.880000000e-01, /*DIAM*/ -1.101423943e-01, -1.320800314e-01, -1.307636574e-01, /*EDIAM*/  8.066500786e-02,  7.031691148e-02,  6.596857016e-02, /*DMEAN*/ -1.258174356e-01, /*EDMEAN*/  4.155739739e-02 },
            {19, /* SP "B2IV              " idx */ 48, /*MAG*/  1.822000000e+00,  1.940000000e+00,  2.040000000e+00,  2.304000000e+00,  2.458000000e+00,  2.479000000e+00, /*EMAG*/  1.000000000e-02,  4.000000000e-02,  1.166190000e-02,  3.120000000e-01,  3.120000000e-01,  3.120000000e-01, /*DIAM*/ -1.712397353e-01, -1.666549705e-01, -1.607832152e-01, /*EDIAM*/  8.768763014e-02,  7.664032027e-02,  7.201373455e-02, /*DMEAN*/ -1.655490257e-01, /*EDMEAN*/  4.625222115e-02 },
            {20, /* SP "A4V               " idx */ 96, /*MAG*/  1.315000000e+00,  1.170000000e+00,  1.010000000e+00,  1.037000000e+00,  9.370000000e-01,  9.450000000e-01, /*EMAG*/  1.303840000e-02,  4.000000000e-02,  1.220656000e-02,  2.860000000e-01,  2.860000000e-01,  2.860000000e-01, /*DIAM*/  3.550153135e-01,  3.665458529e-01,  3.492304374e-01, /*EDIAM*/  8.021585441e-02,  6.989539843e-02,  6.555588009e-02, /*DMEAN*/  3.567248581e-01, /*EDMEAN*/  4.115982201e-02 },
            {21, /* SP "M3IIIab           " idx */ 252, /*MAG*/  4.491000000e+00,  2.870000000e+00,  5.700000000e-01, -9.120000000e-01, -1.608000000e+00, -1.862000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  5.035871000e-02,  1.900000000e-01,  1.900000000e-01,  1.900000000e-01, /*DIAM*/  1.187201401e+00,  1.186128412e+00,  1.190184754e+00, /*EDIAM*/  5.286076286e-02,  4.603224089e-02,  4.316701131e-02, /*DMEAN*/  1.188003827e+00, /*EDMEAN*/  2.714127475e-02 },
            {22, /* SP "M3IIIab           " idx */ 252, /*MAG*/  4.491000000e+00,  2.870000000e+00,  5.700000000e-01, -9.120000000e-01, -1.608000000e+00, -1.862000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  5.035871000e-02,  1.900000000e-01,  1.900000000e-01,  1.900000000e-01, /*DIAM*/  1.187201401e+00,  1.186128412e+00,  1.190184754e+00, /*EDIAM*/  5.286076286e-02,  4.603224089e-02,  4.316701131e-02, /*DMEAN*/  1.188003827e+00, /*EDMEAN*/  2.714127475e-02 },
            {23, /* SP "K3III             " idx */ 212, /*MAG*/  6.272000000e+00,  4.970000000e+00,  3.720000000e+00,  2.764000000e+00,  2.192000000e+00,  1.978000000e+00, /*EMAG*/  5.608030000e-02,  4.000000000e-02,  2.022375000e-02,  3.100000000e-01,  3.100000000e-01,  3.100000000e-01, /*DIAM*/  3.212049299e-01,  3.112985550e-01,  3.259345507e-01, /*EDIAM*/  8.593447305e-02,  7.487300067e-02,  7.022021414e-02, /*DMEAN*/  3.196416057e-01, /*EDMEAN*/  4.403534956e-02 },
            {24, /* SP "M2III             " idx */ 248, /*MAG*/  7.629000000e+00,  6.000000000e+00,  3.760000000e+00,  2.252000000e+00,  1.356000000e+00,  1.097000000e+00, /*EMAG*/  1.166190000e-02,  4.000000000e-02,  5.035871000e-02,  3.240000000e-01,  3.240000000e-01,  3.240000000e-01, /*DIAM*/  5.582521616e-01,  5.969700906e-01,  5.982718464e-01, /*EDIAM*/  8.981491890e-02,  7.825711502e-02,  7.339563663e-02, /*DMEAN*/  5.873374486e-01, /*EDMEAN*/  4.603146655e-02 },
            {25, /* SP "G8Ib              " idx */ 192, /*MAG*/  4.437000000e+00,  3.060000000e+00,  1.840000000e+00,  8.860000000e-01,  2.320000000e-01,  1.250000000e-01, /*EMAG*/  1.513275000e-02,  4.000000000e-02,  1.019804000e-02,  2.220000000e-01,  2.220000000e-01,  2.220000000e-01, /*DIAM*/  6.591423576e-01,  6.831743755e-01,  6.724538976e-01, /*EDIAM*/  6.163113920e-02,  5.368074515e-02,  5.034077915e-02, /*DMEAN*/  6.726728864e-01, /*EDMEAN*/  3.160585076e-02 },
            {26, /* SP "K4III             " idx */ 216, /*MAG*/  5.838000000e+00,  4.390000000e+00,  2.880000000e+00,  1.897000000e+00,  1.220000000e+00,  1.022000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  6.007495000e-02,  3.220000000e-01,  3.220000000e-01,  3.220000000e-01, /*DIAM*/  5.226842629e-01,  5.267681165e-01,  5.320039049e-01, /*EDIAM*/  8.926017416e-02,  7.777265413e-02,  7.294021790e-02, /*DMEAN*/  5.277542496e-01, /*EDMEAN*/  4.573760424e-02 },
            {27, /* SP "M2III             " idx */ 248, /*MAG*/  7.554000000e+00,  5.970000000e+00,  3.770000000e+00,  2.450000000e+00,  1.582000000e+00,  1.444000000e+00, /*EMAG*/  1.029563000e-02,  4.000000000e-02,  3.041381000e-02,  3.180000000e-01,  3.180000000e-01,  3.180000000e-01, /*DIAM*/  5.011450179e-01,  5.412113349e-01,  5.189652759e-01, /*EDIAM*/  8.815677323e-02,  7.681140981e-02,  7.203951345e-02, /*DMEAN*/  5.219783695e-01, /*EDMEAN*/  4.518289309e-02 },
            {28, /* SP "M1III             " idx */ 244, /*MAG*/  6.614000000e+00,  4.930000000e+00,  2.990000000e+00,  1.651000000e+00,  8.140000000e-01,  5.980000000e-01, /*EMAG*/  1.802776000e-02,  4.000000000e-02,  5.035871000e-02,  3.040000000e-01,  3.040000000e-01,  3.040000000e-01, /*DIAM*/  6.466670938e-01,  6.798942866e-01,  6.782301159e-01, /*EDIAM*/  8.428684688e-02,  7.343669556e-02,  6.887348705e-02, /*DMEAN*/  6.705366614e-01, /*EDMEAN*/  4.319834334e-02 },
            {29, /* SP "K4III             " idx */ 216, /*MAG*/  5.838000000e+00,  4.390000000e+00,  2.880000000e+00,  1.897000000e+00,  1.220000000e+00,  1.022000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  6.007495000e-02,  3.220000000e-01,  3.220000000e-01,  3.220000000e-01, /*DIAM*/  5.226842629e-01,  5.267681165e-01,  5.320039049e-01, /*EDIAM*/  8.926017416e-02,  7.777265413e-02,  7.294021790e-02, /*DMEAN*/  5.277542496e-01, /*EDMEAN*/  4.573760424e-02 },
            {30, /* SP "M6III             " idx */ 264, /*MAG*/  7.212000000e+00,  5.760000000e+00,  2.170000000e+00,  2.300000000e-01, -6.520000000e-01, -8.680000000e-01, /*EMAG*/  5.385165000e-02,  4.000000000e-02,  2.004894000e-01,  2.220000000e-01,  2.220000000e-01,  2.220000000e-01, /*DIAM*/  1.057688621e+00,  1.080374284e+00,  1.053016652e+00, /*EDIAM*/  6.232669292e-02,  5.430508254e-02,  5.093736104e-02, /*DMEAN*/  1.063687087e+00, /*EDMEAN*/  3.207588928e-02 },
            {31, /* SP "M6III             " idx */ 264, /*MAG*/  7.212000000e+00,  5.760000000e+00,  2.170000000e+00,  2.300000000e-01, -6.520000000e-01, -8.680000000e-01, /*EMAG*/  5.385165000e-02,  4.000000000e-02,  2.004894000e-01,  2.220000000e-01,  2.220000000e-01,  2.220000000e-01, /*DIAM*/  1.057688621e+00,  1.080374284e+00,  1.053016652e+00, /*EDIAM*/  6.232669292e-02,  5.430508254e-02,  5.093736104e-02, /*DMEAN*/  1.063687087e+00, /*EDMEAN*/  3.207588928e-02 },
            {32, /* SP "M0                " idx */ 240, /*MAG*/  8.302000000e+00,  6.660000000e+00,  4.280000000e+00,  3.036000000e+00,  2.091000000e+00,  1.903000000e+00, /*EMAG*/  1.612452000e-02,  4.000000000e-02,  5.063596000e-02,  2.400000000e-01,  2.400000000e-01,  2.400000000e-01, /*DIAM*/  3.983891214e-01,  4.390758840e-01,  4.233859725e-01, /*EDIAM*/  6.662099861e-02,  5.803171284e-02,  5.442216807e-02, /*DMEAN*/  4.222611635e-01, /*EDMEAN*/  3.415645814e-02 },
            {33, /* SP "M1III             " idx */ 244, /*MAG*/  6.614000000e+00,  4.930000000e+00,  2.990000000e+00,  1.651000000e+00,  8.140000000e-01,  5.980000000e-01, /*EMAG*/  1.802776000e-02,  4.000000000e-02,  5.035871000e-02,  3.040000000e-01,  3.040000000e-01,  3.040000000e-01, /*DIAM*/  6.466670938e-01,  6.798942866e-01,  6.782301159e-01, /*EDIAM*/  8.428684688e-02,  7.343669556e-02,  6.887348705e-02, /*DMEAN*/  6.705366614e-01, /*EDMEAN*/  4.319834334e-02 },
            {34, /* SP "M5                " idx */ 260, /*MAG*/  1.218700000e+01,  9.240000000e+00,  5.780000000e+00,  2.624000000e+00,  1.621000000e+00,  1.105000000e+00, /*EMAG*/  2.560000000e-01,  4.000000000e-02,  9.805555000e-01,  3.100000000e-01,  3.100000000e-01,  3.100000000e-01, /*DIAM*/  6.769801123e-01,  6.744240771e-01,  6.944810495e-01, /*EDIAM*/  8.617969503e-02,  7.509441601e-02,  7.043299956e-02, /*DMEAN*/  6.829707121e-01, /*EDMEAN*/  4.421366073e-02 },
            {35, /* SP "M4III             " idx */ 256, /*MAG*/  7.911000000e+00,  6.320000000e+00,  2.900000000e+00,  8.970000000e-01, -1.640000000e-01, -5.120000000e-01, /*EMAG*/  2.842534000e-02,  4.000000000e-02,  6.264184000e-02,  3.200000000e-01,  3.200000000e-01,  3.200000000e-01, /*DIAM*/  9.424623267e-01,  9.826828889e-01,  9.796822312e-01, /*EDIAM*/  8.879866775e-02,  7.737414917e-02,  7.256930379e-02, /*DMEAN*/  9.709674786e-01, /*EDMEAN*/  4.553133643e-02 },
            {36, /* SP "M2III             " idx */ 248, /*MAG*/  6.269000000e+00,  4.680000000e+00,  2.720000000e+00,  1.530000000e+00,  7.060000000e-01,  4.900000000e-01, /*EMAG*/  4.110961000e-02,  4.000000000e-02,  2.022375000e-02,  2.800000000e-01,  2.800000000e-01,  2.800000000e-01, /*DIAM*/  6.567343060e-01,  6.993358509e-01,  7.009360815e-01, /*EDIAM*/  7.765773323e-02,  6.765703015e-02,  6.345226389e-02, /*DMEAN*/  6.888073220e-01, /*EDMEAN*/  3.981052266e-02 },
            {37, /* SP "M3IIIab           " idx */ 252, /*MAG*/  4.491000000e+00,  2.870000000e+00,  5.700000000e-01, -9.120000000e-01, -1.608000000e+00, -1.862000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  5.035871000e-02,  1.900000000e-01,  1.900000000e-01,  1.900000000e-01, /*DIAM*/  1.187201401e+00,  1.186128412e+00,  1.190184754e+00, /*EDIAM*/  5.286076286e-02,  4.603224089e-02,  4.316701131e-02, /*DMEAN*/  1.188003827e+00, /*EDMEAN*/  2.714127475e-02 },
            {38, /* SP "K4III             " idx */ 216, /*MAG*/  5.838000000e+00,  4.390000000e+00,  2.880000000e+00,  1.897000000e+00,  1.220000000e+00,  1.022000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  6.007495000e-02,  3.220000000e-01,  3.220000000e-01,  3.220000000e-01, /*DIAM*/  5.226842629e-01,  5.267681165e-01,  5.320039049e-01, /*EDIAM*/  8.926017416e-02,  7.777265413e-02,  7.294021790e-02, /*DMEAN*/  5.277542496e-01, /*EDMEAN*/  4.573760424e-02 },
            {39, /* SP "M1III             " idx */ 244, /*MAG*/  6.614000000e+00,  4.930000000e+00,  2.990000000e+00,  1.651000000e+00,  8.140000000e-01,  5.980000000e-01, /*EMAG*/  1.802776000e-02,  4.000000000e-02,  5.035871000e-02,  3.040000000e-01,  3.040000000e-01,  3.040000000e-01, /*DIAM*/  6.466670938e-01,  6.798942866e-01,  6.782301159e-01, /*EDIAM*/  8.428684688e-02,  7.343669556e-02,  6.887348705e-02, /*DMEAN*/  6.705366614e-01, /*EDMEAN*/  4.319834334e-02 },
            {40, /* SP "M2III             " idx */ 248, /*MAG*/  7.794000000e+00,  6.140000000e+00,  3.990000000e+00,  2.840000000e+00,  1.915000000e+00,  1.705000000e+00, /*EMAG*/  1.208305000e-02,  4.000000000e-02,  3.041381000e-02,  2.920000000e-01,  2.920000000e-01,  2.920000000e-01, /*DIAM*/  4.062521594e-01,  4.678883766e-01,  4.643740342e-01, /*EDIAM*/  8.097269190e-02,  7.054752395e-02,  6.616371008e-02, /*DMEAN*/  4.503639836e-01, /*EDMEAN*/  4.150666170e-02 },
            {41, /* SP "M4                " idx */ 256, /*MAG*/  9.383000000e+00,  7.820000000e+00,  5.230000000e+00,  3.714000000e+00,  2.709000000e+00,  2.443000000e+00, /*EMAG*/  2.549510000e-02,  4.000000000e-02,  4.148494000e-02,  2.620000000e-01,  2.620000000e-01,  2.620000000e-01, /*DIAM*/  2.779355311e-01,  3.514533075e-01,  3.504486452e-01, /*EDIAM*/  7.279522716e-02,  6.342070858e-02,  5.948065040e-02, /*DMEAN*/  3.318094548e-01, /*EDMEAN*/  3.734714673e-02 },
            {42, /* SP "M2III             " idx */ 248, /*MAG*/  6.269000000e+00,  4.680000000e+00,  2.720000000e+00,  1.530000000e+00,  7.060000000e-01,  4.900000000e-01, /*EMAG*/  4.110961000e-02,  4.000000000e-02,  2.022375000e-02,  2.800000000e-01,  2.800000000e-01,  2.800000000e-01, /*DIAM*/  6.567343060e-01,  6.993358509e-01,  7.009360815e-01, /*EDIAM*/  7.765773323e-02,  6.765703015e-02,  6.345226389e-02, /*DMEAN*/  6.888073220e-01, /*EDMEAN*/  3.981052266e-02 },
            {43, /* SP "M2III             " idx */ 248, /*MAG*/  6.653000000e+00,  5.030000000e+00,  2.990000000e+00,  1.717000000e+00,  8.910000000e-01,  7.010000000e-01, /*EMAG*/  1.029563000e-02,  4.000000000e-02,  5.024938000e-02,  3.080000000e-01,  3.080000000e-01,  3.080000000e-01, /*DIAM*/  6.318503770e-01,  6.691412979e-01,  6.623886357e-01, /*EDIAM*/  8.539341896e-02,  7.440205370e-02,  6.977944134e-02, /*DMEAN*/  6.567205188e-01, /*EDMEAN*/  4.376877183e-02 },
            {44, /* SP "K5III             " idx */ 220, /*MAG*/  6.779000000e+00,  5.190000000e+00,  3.590000000e+00,  2.466000000e+00,  1.778000000e+00,  1.611000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  9.008885000e-02,  3.360000000e-01,  3.360000000e-01,  3.360000000e-01, /*DIAM*/  4.320135973e-01,  4.301149697e-01,  4.248465477e-01, /*EDIAM*/  9.313857745e-02,  8.115414430e-02,  7.611221672e-02, /*DMEAN*/  4.285436488e-01, /*EDMEAN*/  4.772312286e-02 },
            {45, /* SP "M2III             " idx */ 248, /*MAG*/  1.031000000e+01,  8.529000000e+00,              NaN,  3.246000000e+00,  2.197000000e+00,  1.863000000e+00, /*EMAG*/  9.200000000e-02,  4.000000000e-02,              NaN,  2.560000000e-01,  2.560000000e-01,  2.560000000e-01, /*DIAM*/  4.773182319e-01,  4.983918790e-01,  4.913988521e-01, /*EDIAM*/  7.102962935e-02,  6.187729013e-02,  5.803046066e-02, /*DMEAN*/  4.901272061e-01, /*EDMEAN*/  3.641961516e-02 },
            {46, /* SP "M4III             " idx */ 256, /*MAG*/  7.911000000e+00,  6.320000000e+00,  2.900000000e+00,  8.970000000e-01, -1.640000000e-01, -5.120000000e-01, /*EMAG*/  2.842534000e-02,  4.000000000e-02,  6.264184000e-02,  3.200000000e-01,  3.200000000e-01,  3.200000000e-01, /*DIAM*/  9.424623267e-01,  9.826828889e-01,  9.796822312e-01, /*EDIAM*/  8.879866775e-02,  7.737414917e-02,  7.256930379e-02, /*DMEAN*/  9.709674786e-01, /*EDMEAN*/  4.553133643e-02 },
            {47, /* SP "K5III             " idx */ 220, /*MAG*/  2.408000000e+00,  8.700000000e-01, -8.000000000e-01, -2.095000000e+00, -2.775000000e+00, -3.044000000e+00, /*EMAG*/  1.360147000e-02,  4.000000000e-02,  2.282542000e-02,  1.940000000e-01,  1.940000000e-01,  1.940000000e-01, /*DIAM*/  1.362718968e+00,  1.350325100e+00,  1.364649481e+00, /*EDIAM*/  5.393608421e-02,  4.696724666e-02,  4.404144133e-02, /*DMEAN*/  1.359197387e+00, /*EDMEAN*/  2.766262845e-02 },
            {48, /* SP "K5III             " idx */ 220, /*MAG*/  2.408000000e+00,  8.700000000e-01, -8.000000000e-01, -2.095000000e+00, -2.775000000e+00, -3.044000000e+00, /*EMAG*/  1.360147000e-02,  4.000000000e-02,  2.282542000e-02,  1.940000000e-01,  1.940000000e-01,  1.940000000e-01, /*DIAM*/  1.362718968e+00,  1.350325100e+00,  1.364649481e+00, /*EDIAM*/  5.393608421e-02,  4.696724666e-02,  4.404144133e-02, /*DMEAN*/  1.359197387e+00, /*EDMEAN*/  2.766262845e-02 },
            {49, /* SP "K5III             " idx */ 220, /*MAG*/  2.408000000e+00,  8.700000000e-01, -8.000000000e-01, -2.095000000e+00, -2.775000000e+00, -3.044000000e+00, /*EMAG*/  1.360147000e-02,  4.000000000e-02,  2.282542000e-02,  1.940000000e-01,  1.940000000e-01,  1.940000000e-01, /*DIAM*/  1.362718968e+00,  1.350325100e+00,  1.364649481e+00, /*EDIAM*/  5.393608421e-02,  4.696724666e-02,  4.404144133e-02, /*DMEAN*/  1.359197387e+00, /*EDMEAN*/  2.766262845e-02 },
            {50, /* SP "K5III             " idx */ 220, /*MAG*/  2.408000000e+00,  8.700000000e-01, -8.000000000e-01, -2.095000000e+00, -2.775000000e+00, -3.044000000e+00, /*EMAG*/  1.360147000e-02,  4.000000000e-02,  2.282542000e-02,  1.940000000e-01,  1.940000000e-01,  1.940000000e-01, /*DIAM*/  1.362718968e+00,  1.350325100e+00,  1.364649481e+00, /*EDIAM*/  5.393608421e-02,  4.696724666e-02,  4.404144133e-02, /*DMEAN*/  1.359197387e+00, /*EDMEAN*/  2.766262845e-02 },
            {51, /* SP "K5III             " idx */ 220, /*MAG*/  2.408000000e+00,  8.700000000e-01, -8.000000000e-01, -2.095000000e+00, -2.775000000e+00, -3.044000000e+00, /*EMAG*/  1.360147000e-02,  4.000000000e-02,  2.282542000e-02,  1.940000000e-01,  1.940000000e-01,  1.940000000e-01, /*DIAM*/  1.362718968e+00,  1.350325100e+00,  1.364649481e+00, /*EDIAM*/  5.393608421e-02,  4.696724666e-02,  4.404144133e-02, /*DMEAN*/  1.359197387e+00, /*EDMEAN*/  2.766262845e-02 },
            {52, /* SP "M6                " idx */ 264, /*MAG*/  1.185200000e+01,  1.031700000e+01,              NaN,  2.923000000e+00,  1.635000000e+00,  1.342000000e+00, /*EMAG*/  2.280000000e-01,  4.000000000e-02,              NaN,  2.520000000e-01,  2.520000000e-01,  2.520000000e-01, /*DIAM*/  6.622171862e-01,  7.166007376e-01,  6.726896391e-01, /*EDIAM*/  7.052670092e-02,  6.145433117e-02,  5.764286028e-02, /*DMEAN*/  6.851121470e-01, /*EDMEAN*/  3.625740936e-02 },
            {53, /* SP "M6                " idx */ 264, /*MAG*/  1.031300000e+01,  9.200000000e+00,  4.580000000e+00,  1.422000000e+00,  3.630000000e-01,  1.080000000e-01, /*EMAG*/  3.671512000e-02,  4.000000000e-02,  5.314132000e-02,  2.240000000e-01,  2.240000000e-01,  2.240000000e-01, /*DIAM*/  9.919029054e-01,  9.773937375e-01,  9.225640953e-01, /*EDIAM*/  6.287251438e-02,  5.478097927e-02,  5.138371767e-02, /*DMEAN*/  9.596420761e-01, /*EDMEAN*/  3.235408095e-02 },
            {54, /* SP "K4III             " idx */ 216, /*MAG*/  5.838000000e+00,  4.390000000e+00,  2.880000000e+00,  1.897000000e+00,  1.220000000e+00,  1.022000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  6.007495000e-02,  3.220000000e-01,  3.220000000e-01,  3.220000000e-01, /*DIAM*/  5.226842629e-01,  5.267681165e-01,  5.320039049e-01, /*EDIAM*/  8.926017416e-02,  7.777265413e-02,  7.294021790e-02, /*DMEAN*/  5.277542496e-01, /*EDMEAN*/  4.573760424e-02 },
            {55, /* SP "M4III             " idx */ 256, /*MAG*/  7.911000000e+00,  6.320000000e+00,  2.900000000e+00,  8.970000000e-01, -1.640000000e-01, -5.120000000e-01, /*EMAG*/  2.842534000e-02,  4.000000000e-02,  6.264184000e-02,  3.200000000e-01,  3.200000000e-01,  3.200000000e-01, /*DIAM*/  9.424623267e-01,  9.826828889e-01,  9.796822312e-01, /*EDIAM*/  8.879866775e-02,  7.737414917e-02,  7.256930379e-02, /*DMEAN*/  9.709674786e-01, /*EDMEAN*/  4.553133643e-02 },
            {56, /* SP "M3III             " idx */ 252, /*MAG*/  8.490000000e+00,  6.870000000e+00,  4.600000000e+00,  3.157000000e+00,  2.237000000e+00,  1.982000000e+00, /*EMAG*/  1.565248000e-02,  4.000000000e-02,  6.040695000e-02,  3.040000000e-01,  3.040000000e-01,  3.040000000e-01, /*DIAM*/  3.681031744e-01,  4.235213967e-01,  4.254840124e-01, /*EDIAM*/  8.431279613e-02,  7.346097033e-02,  6.889739918e-02, /*DMEAN*/  4.097753378e-01, /*EDMEAN*/  4.322341663e-02 },
            {57, /* SP "M1III             " idx */ 244, /*MAG*/  6.797000000e+00,  5.150000000e+00,  3.130000000e+00,  1.839000000e+00,  9.960000000e-01,  8.340000000e-01, /*EMAG*/  1.300000000e-02,  4.000000000e-02,  5.024938000e-02,  3.500000000e-01,  3.500000000e-01,  3.500000000e-01, /*DIAM*/  6.115242362e-01,  6.450616013e-01,  6.306096772e-01, /*EDIAM*/  9.700009136e-02,  8.452108271e-02,  7.927106396e-02, /*DMEAN*/  6.305987995e-01, /*EDMEAN*/  4.970517791e-02 },
            {58, /* SP "M2Iab-Ib          " idx */ 248, /*MAG*/  6.380000000e+00,  4.320000000e+00,  1.780000000e+00,  3.930000000e-01, -6.090000000e-01, -9.130000000e-01, /*EMAG*/  2.915476000e-02,  4.000000000e-02,  3.195309000e-02,  2.200000000e-01,  2.200000000e-01,  2.200000000e-01, /*DIAM*/  9.437968102e-01,  1.001724960e+00,  1.008943385e+00, /*EDIAM*/  6.109348273e-02,  5.321181537e-02,  4.990136622e-02, /*DMEAN*/  9.894048670e-01, /*EDMEAN*/  3.133779163e-02 },
            {59, /* SP "K0IIICN+1         " idx */ 200, /*MAG*/  3.410000000e+00,  2.240000000e+00,  1.110000000e+00,  3.110000000e-01, -2.100000000e-01, -3.030000000e-01, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  2.000000000e-03,  1.980000000e-01,  1.980000000e-01,  1.980000000e-01, /*DIAM*/  7.699245536e-01,  7.642667240e-01,  7.562505586e-01, /*EDIAM*/  5.498990226e-02,  4.788713752e-02,  4.490495141e-02, /*DMEAN*/  7.625951365e-01, /*EDMEAN*/  2.820488865e-02 },
            {60, /* SP "M0III             " idx */ 240, /*MAG*/  3.646000000e+00,  2.070000000e+00,  3.300000000e-01, -9.570000000e-01, -1.674000000e+00, -1.846000000e+00, /*EMAG*/  1.077033000e-02,  4.000000000e-02,  3.026549000e-02,  2.060000000e-01,  2.060000000e-01,  2.060000000e-01, /*DIAM*/  1.151148061e+00,  1.158048657e+00,  1.151086713e+00, /*EDIAM*/  5.724192880e-02,  4.985124680e-02,  4.674776593e-02, /*DMEAN*/  1.153507164e+00, /*EDMEAN*/  2.935911649e-02 },
            {61, /* SP "K3IIb             " idx */ 212, /*MAG*/  3.615000000e+00,  2.100000000e+00,  7.300000000e-01, -1.800000000e-02, -8.340000000e-01, -8.610000000e-01, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  1.077033000e-02,  2.180000000e-01,  2.180000000e-01,  2.180000000e-01, /*DIAM*/  8.708477952e-01,  9.229328054e-01,  8.929199606e-01, /*EDIAM*/  6.052528622e-02,  5.271509451e-02,  4.943396538e-02, /*DMEAN*/  8.975096584e-01, /*EDMEAN*/  3.103264902e-02 },
            {62, /* SP "F5Ib              " idx */ 140, /*MAG*/  2.271000000e+00,  1.790000000e+00,  1.160000000e+00,  8.300000000e-01,  4.130000000e-01,  5.200000000e-01, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  4.019950000e-02,  1.840000000e-01,  1.840000000e-01,  1.840000000e-01, /*DIAM*/  5.053674567e-01,  5.555711244e-01,  5.101430847e-01, /*EDIAM*/  5.156484382e-02,  4.490146625e-02,  4.210463273e-02, /*DMEAN*/  5.245849158e-01, /*EDMEAN*/  2.646513615e-02 },
            {63, /* SP "K5III             " idx */ 220, /*MAG*/  2.408000000e+00,  8.700000000e-01, -8.000000000e-01, -2.095000000e+00, -2.775000000e+00, -3.044000000e+00, /*EMAG*/  1.360147000e-02,  4.000000000e-02,  2.282542000e-02,  1.940000000e-01,  1.940000000e-01,  1.940000000e-01, /*DIAM*/  1.362718968e+00,  1.350325100e+00,  1.364649481e+00, /*EDIAM*/  5.393608421e-02,  4.696724666e-02,  4.404144133e-02, /*DMEAN*/  1.359197387e+00, /*EDMEAN*/  2.766262845e-02 },
            {64, /* SP "M6III             " idx */ 264, /*MAG*/  7.212000000e+00,  5.760000000e+00,  2.170000000e+00,  2.300000000e-01, -6.520000000e-01, -8.680000000e-01, /*EMAG*/  5.385165000e-02,  4.000000000e-02,  2.004894000e-01,  2.220000000e-01,  2.220000000e-01,  2.220000000e-01, /*DIAM*/  1.057688621e+00,  1.080374284e+00,  1.053016652e+00, /*EDIAM*/  6.232669292e-02,  5.430508254e-02,  5.093736104e-02, /*DMEAN*/  1.063687087e+00, /*EDMEAN*/  3.207588928e-02 },
            {65, /* SP "M3IIIab           " idx */ 252, /*MAG*/  4.491000000e+00,  2.870000000e+00,  5.700000000e-01, -9.120000000e-01, -1.608000000e+00, -1.862000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  5.035871000e-02,  1.900000000e-01,  1.900000000e-01,  1.900000000e-01, /*DIAM*/  1.187201401e+00,  1.186128412e+00,  1.190184754e+00, /*EDIAM*/  5.286076286e-02,  4.603224089e-02,  4.316701131e-02, /*DMEAN*/  1.188003827e+00, /*EDMEAN*/  2.714127475e-02 },
            {66, /* SP "M6                " idx */ 264, /*MAG*/  1.185200000e+01,  1.031700000e+01,              NaN,  2.923000000e+00,  1.635000000e+00,  1.342000000e+00, /*EMAG*/  2.280000000e-01,  4.000000000e-02,              NaN,  2.520000000e-01,  2.520000000e-01,  2.520000000e-01, /*DIAM*/  6.622171862e-01,  7.166007376e-01,  6.726896391e-01, /*EDIAM*/  7.052670092e-02,  6.145433117e-02,  5.764286028e-02, /*DMEAN*/  6.851121470e-01, /*EDMEAN*/  3.625740936e-02 },
            {67, /* SP "M3III             " idx */ 252, /*MAG*/  9.567000000e+00,  8.045000000e+00,              NaN,  3.562000000e+00,  2.527000000e+00,  2.235000000e+00, /*EMAG*/  4.100000000e-02,  4.000000000e-02,              NaN,  2.600000000e-01,  2.600000000e-01,  2.600000000e-01, /*DIAM*/  3.462281740e-01,  4.020233419e-01,  3.991117492e-01, /*EDIAM*/  7.216291044e-02,  6.286686294e-02,  5.895961736e-02, /*DMEAN*/  3.862708261e-01, /*EDMEAN*/  3.700825419e-02 },
            {68, /* SP "M6                " idx */ 264, /*MAG*/  1.001900000e+01,  8.656000000e+00,              NaN,  2.640000000e+00,  1.695000000e+00,  1.371000000e+00, /*EMAG*/  4.300000000e-02,  4.000000000e-02,              NaN,  2.340000000e-01,  2.340000000e-01,  2.340000000e-01, /*DIAM*/  6.130064712e-01,  6.336178569e-01,  6.224808792e-01, /*EDIAM*/  6.560356984e-02,  5.716212112e-02,  5.361705954e-02, /*DMEAN*/  6.238475165e-01, /*EDMEAN*/  3.374636603e-02 },
            {69, /* SP "M2Iab-Ib          " idx */ 248, /*MAG*/  6.380000000e+00,  4.320000000e+00,  1.780000000e+00,  3.930000000e-01, -6.090000000e-01, -9.130000000e-01, /*EMAG*/  2.915476000e-02,  4.000000000e-02,  3.195309000e-02,  2.200000000e-01,  2.200000000e-01,  2.200000000e-01, /*DIAM*/  9.437968102e-01,  1.001724960e+00,  1.008943385e+00, /*EDIAM*/  6.109348273e-02,  5.321181537e-02,  4.990136622e-02, /*DMEAN*/  9.894048670e-01, /*EDMEAN*/  3.133779163e-02 },
            {70, /* SP "K0III             " idx */ 200, /*MAG*/  5.023000000e+00,  3.940000000e+00,  2.930000000e+00,  2.209000000e+00,  1.685000000e+00,  1.567000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  2.022375000e-02,  2.940000000e-01,  2.940000000e-01,  2.940000000e-01, /*DIAM*/  3.751209763e-01,  3.772239167e-01,  3.777833997e-01, /*EDIAM*/  8.150060400e-02,  7.100734635e-02,  6.659430498e-02, /*DMEAN*/  3.768928105e-01, /*EDMEAN*/  4.176836693e-02 },
            {71, /* SP "M7(III)           " idx */ 268, /*MAG*/  8.353000000e+00,  7.060000000e+00,  2.330000000e+00, -3.560000000e-01, -1.606000000e+00, -2.003000000e+00, /*EMAG*/  1.303840000e-02,  4.000000000e-02,  4.060788000e-02,  3.360000000e-01,  3.360000000e-01,  3.360000000e-01, /*DIAM*/  1.301706815e+00,  1.364367308e+00,  1.347041674e+00, /*EDIAM*/  9.397750151e-02,  8.190663576e-02,  7.683074120e-02, /*DMEAN*/  1.341146834e+00, /*EDMEAN*/  4.828344831e-02 },
            {72, /* SP "M5III             " idx */ 260, /*MAG*/  8.550000000e+00,  7.140000000e+00,  3.500000000e+00,  1.294000000e+00,  2.910000000e-01, -4.000000000e-03, /*EMAG*/  1.835756000e-02,  4.000000000e-02,  4.100000000e-02,  2.780000000e-01,  2.780000000e-01,  2.780000000e-01, /*DIAM*/  8.838551153e-01,  9.086653257e-01,  8.902401765e-01, /*EDIAM*/  7.736562459e-02,  6.740990207e-02,  6.322500281e-02, /*DMEAN*/  8.949306613e-01, /*EDMEAN*/  3.970842490e-02 },
            {73, /* SP "M1III             " idx */ 244, /*MAG*/  8.299000000e+00,  6.530000000e+00,  4.250000000e+00,  2.857000000e+00,  1.994000000e+00,  1.740000000e+00, /*EMAG*/  7.217340000e-02,  4.000000000e-02,  2.061553000e-02,  2.720000000e-01,  2.720000000e-01,  2.720000000e-01, /*DIAM*/  4.357206621e-01,  4.612172406e-01,  4.618651492e-01, /*EDIAM*/  7.544660997e-02,  6.572838302e-02,  6.164261616e-02, /*DMEAN*/  4.547948815e-01, /*EDMEAN*/  3.867459340e-02 },
            {74, /* SP "M4III             " idx */ 256, /*MAG*/  7.911000000e+00,  6.320000000e+00,  2.900000000e+00,  8.970000000e-01, -1.640000000e-01, -5.120000000e-01, /*EMAG*/  2.842534000e-02,  4.000000000e-02,  6.264184000e-02,  3.200000000e-01,  3.200000000e-01,  3.200000000e-01, /*DIAM*/  9.424623267e-01,  9.826828889e-01,  9.796822312e-01, /*EDIAM*/  8.879866775e-02,  7.737414917e-02,  7.256930379e-02, /*DMEAN*/  9.709674786e-01, /*EDMEAN*/  4.553133643e-02 },
            {75, /* SP "M3III             " idx */ 252, /*MAG*/  8.102000000e+00,  6.520000000e+00,  3.840000000e+00,  2.202000000e+00,  1.270000000e+00,  1.039000000e+00, /*EMAG*/  1.341641000e-02,  4.000000000e-02,  2.088061000e-02,  3.080000000e-01,  3.080000000e-01,  3.080000000e-01, /*DIAM*/  6.055585350e-01,  6.423696490e-01,  6.296664972e-01, /*EDIAM*/  8.541773632e-02,  7.442436321e-02,  6.980109425e-02, /*DMEAN*/  6.277376929e-01, /*EDMEAN*/  4.378874125e-02 },
            {76, /* SP "M1III             " idx */ 244, /*MAG*/  6.797000000e+00,  5.150000000e+00,  3.130000000e+00,  1.839000000e+00,  9.960000000e-01,  8.340000000e-01, /*EMAG*/  1.300000000e-02,  4.000000000e-02,  5.024938000e-02,  3.500000000e-01,  3.500000000e-01,  3.500000000e-01, /*DIAM*/  6.115242362e-01,  6.450616013e-01,  6.306096772e-01, /*EDIAM*/  9.700009136e-02,  8.452108271e-02,  7.927106396e-02, /*DMEAN*/  6.305987995e-01, /*EDMEAN*/  4.970517791e-02 },
            {77, /* SP "M2III             " idx */ 248, /*MAG*/  7.554000000e+00,  5.970000000e+00,  3.770000000e+00,  2.450000000e+00,  1.582000000e+00,  1.444000000e+00, /*EMAG*/  1.029563000e-02,  4.000000000e-02,  3.041381000e-02,  3.180000000e-01,  3.180000000e-01,  3.180000000e-01, /*DIAM*/  5.011450179e-01,  5.412113349e-01,  5.189652759e-01, /*EDIAM*/  8.815677323e-02,  7.681140981e-02,  7.203951345e-02, /*DMEAN*/  5.219783695e-01, /*EDMEAN*/  4.518289309e-02 },
            {78, /* SP "M2Iab-Ib          " idx */ 248, /*MAG*/  6.380000000e+00,  4.320000000e+00,  1.780000000e+00,  3.930000000e-01, -6.090000000e-01, -9.130000000e-01, /*EMAG*/  2.915476000e-02,  4.000000000e-02,  3.195309000e-02,  2.200000000e-01,  2.200000000e-01,  2.200000000e-01, /*DIAM*/  9.437968102e-01,  1.001724960e+00,  1.008943385e+00, /*EDIAM*/  6.109348273e-02,  5.321181537e-02,  4.990136622e-02, /*DMEAN*/  9.894048670e-01, /*EDMEAN*/  3.133779163e-02 },
            {79, /* SP "M4III             " idx */ 256, /*MAG*/  7.911000000e+00,  6.320000000e+00,  2.900000000e+00,  8.970000000e-01, -1.640000000e-01, -5.120000000e-01, /*EMAG*/  2.842534000e-02,  4.000000000e-02,  6.264184000e-02,  3.200000000e-01,  3.200000000e-01,  3.200000000e-01, /*DIAM*/  9.424623267e-01,  9.826828889e-01,  9.796822312e-01, /*EDIAM*/  8.879866775e-02,  7.737414917e-02,  7.256930379e-02, /*DMEAN*/  9.709674786e-01, /*EDMEAN*/  4.553133643e-02 },
            {80, /* SP "K5III             " idx */ 220, /*MAG*/  2.408000000e+00,  8.700000000e-01, -8.000000000e-01, -2.095000000e+00, -2.775000000e+00, -3.044000000e+00, /*EMAG*/  1.360147000e-02,  4.000000000e-02,  2.282542000e-02,  1.940000000e-01,  1.940000000e-01,  1.940000000e-01, /*DIAM*/  1.362718968e+00,  1.350325100e+00,  1.364649481e+00, /*EDIAM*/  5.393608421e-02,  4.696724666e-02,  4.404144133e-02, /*DMEAN*/  1.359197387e+00, /*EDMEAN*/  2.766262845e-02 },
            {81, /* SP "M2Iab-Ib          " idx */ 248, /*MAG*/  6.380000000e+00,  4.320000000e+00,  1.780000000e+00,  3.930000000e-01, -6.090000000e-01, -9.130000000e-01, /*EMAG*/  2.915476000e-02,  4.000000000e-02,  3.195309000e-02,  2.200000000e-01,  2.200000000e-01,  2.200000000e-01, /*DIAM*/  9.437968102e-01,  1.001724960e+00,  1.008943385e+00, /*EDIAM*/  6.109348273e-02,  5.321181537e-02,  4.990136622e-02, /*DMEAN*/  9.894048670e-01, /*EDMEAN*/  3.133779163e-02 },
            {82, /* SP "K1IIIb            " idx */ 204, /*MAG*/  3.161000000e+00,  2.010000000e+00,  8.800000000e-01, -1.960000000e-01, -7.490000000e-01, -7.830000000e-01, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  4.000000000e-03,  2.180000000e-01,  2.180000000e-01,  2.180000000e-01, /*DIAM*/  8.997484890e-01,  8.892764057e-01,  8.633493217e-01, /*EDIAM*/  6.050865738e-02,  5.270084185e-02,  4.942081708e-02, /*DMEAN*/  8.818272996e-01, /*EDMEAN*/  3.102581645e-02 },
            {83, /* SP "K0III             " idx */ 200, /*MAG*/  2.151000000e+00,  1.160000000e+00,  1.900000000e-01, -3.150000000e-01, -8.450000000e-01, -9.360000000e-01, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  1.166190000e-02,  2.320000000e-01,  2.320000000e-01,  2.320000000e-01, /*DIAM*/  8.602638407e-01,  8.729126400e-01,  8.711045749e-01, /*EDIAM*/  6.437338249e-02,  5.607183299e-02,  5.258345587e-02, /*DMEAN*/  8.688916465e-01, /*EDMEAN*/  3.300440249e-02 },
            {84, /* SP "K4III             " idx */ 216, /*MAG*/  3.535000000e+00,  2.070000000e+00,  6.100000000e-01, -4.950000000e-01, -1.225000000e+00, -1.287000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  3.014963000e-02,  2.040000000e-01,  2.040000000e-01,  2.040000000e-01, /*DIAM*/  1.006612842e+00,  1.020923766e+00,  9.935148606e-01, /*EDIAM*/  5.667740937e-02,  4.935865566e-02,  4.628502899e-02, /*DMEAN*/  1.006406897e+00, /*EDMEAN*/  2.906429508e-02 },
            {85, /* SP "K5III             " idx */ 220, /*MAG*/  3.761000000e+00,  2.240000000e+00,  7.000000000e-01, -2.450000000e-01, -1.034000000e+00, -1.162000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  3.014963000e-02,  2.120000000e-01,  2.120000000e-01,  2.120000000e-01, /*DIAM*/  9.558618194e-01,  9.868231492e-01,  9.747954610e-01, /*EDIAM*/  5.889795812e-02,  5.129563449e-02,  4.810227371e-02, /*DMEAN*/  9.739959256e-01, /*EDMEAN*/  3.020046911e-02 },
            {86, /* SP "G9III             " idx */ 196, /*MAG*/  4.060000000e+00,  3.070000000e+00,  2.130000000e+00,  1.375000000e+00,  8.370000000e-01,  8.830000000e-01, /*EMAG*/  1.811077000e-02,  4.000000000e-02,  3.006659000e-02,  2.620000000e-01,  2.620000000e-01,  2.620000000e-01, /*DIAM*/  5.318730658e-01,  5.416598657e-01,  5.053505763e-01, /*EDIAM*/  7.266401252e-02,  6.330223021e-02,  5.936652649e-02, /*DMEAN*/  5.248339379e-01, /*EDMEAN*/  3.724794250e-02 },
            {87, /* SP "M0III             " idx */ 240, /*MAG*/  3.646000000e+00,  2.070000000e+00,  3.300000000e-01, -9.570000000e-01, -1.674000000e+00, -1.846000000e+00, /*EMAG*/  1.077033000e-02,  4.000000000e-02,  3.026549000e-02,  2.060000000e-01,  2.060000000e-01,  2.060000000e-01, /*DIAM*/  1.151148061e+00,  1.158048657e+00,  1.151086713e+00, /*EDIAM*/  5.724192880e-02,  4.985124680e-02,  4.674776593e-02, /*DMEAN*/  1.153507164e+00, /*EDMEAN*/  2.935911649e-02 },
            {88, /* SP "K5III             " idx */ 220, /*MAG*/  2.408000000e+00,  8.700000000e-01, -8.000000000e-01, -2.095000000e+00, -2.775000000e+00, -3.044000000e+00, /*EMAG*/  1.360147000e-02,  4.000000000e-02,  2.282542000e-02,  1.940000000e-01,  1.940000000e-01,  1.940000000e-01, /*DIAM*/  1.362718968e+00,  1.350325100e+00,  1.364649481e+00, /*EDIAM*/  5.393608421e-02,  4.696724666e-02,  4.404144133e-02, /*DMEAN*/  1.359197387e+00, /*EDMEAN*/  2.766262845e-02 },
            {89, /* SP "M3IIIab           " idx */ 252, /*MAG*/  4.491000000e+00,  2.870000000e+00,  5.700000000e-01, -9.120000000e-01, -1.608000000e+00, -1.862000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  5.035871000e-02,  1.900000000e-01,  1.900000000e-01,  1.900000000e-01, /*DIAM*/  1.187201401e+00,  1.186128412e+00,  1.190184754e+00, /*EDIAM*/  5.286076286e-02,  4.603224089e-02,  4.316701131e-02, /*DMEAN*/  1.188003827e+00, /*EDMEAN*/  2.714127475e-02 },
            {90, /* SP "K0III             " idx */ 200, /*MAG*/  1.189000000e+00, -5.000000000e-02, -1.270000000e+00, -2.252000000e+00, -2.810000000e+00, -2.911000000e+00, /*EMAG*/  1.081665000e-02,  4.000000000e-02,  2.193171000e-02,  1.700000000e-01,  1.700000000e-01,  1.700000000e-01, /*DIAM*/  1.303487062e+00,  1.297052724e+00,  1.286206771e+00, /*EDIAM*/  4.727046129e-02,  4.115207448e-02,  3.858594226e-02, /*DMEAN*/  1.294467506e+00, /*EDMEAN*/  2.425832506e-02 },
            {91, /* SP "K5III             " idx */ 220, /*MAG*/  3.761000000e+00,  2.240000000e+00,  7.000000000e-01, -2.450000000e-01, -1.034000000e+00, -1.162000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  3.014963000e-02,  2.120000000e-01,  2.120000000e-01,  2.120000000e-01, /*DIAM*/  9.558618194e-01,  9.868231492e-01,  9.747954610e-01, /*EDIAM*/  5.889795812e-02,  5.129563449e-02,  4.810227371e-02, /*DMEAN*/  9.739959256e-01, /*EDMEAN*/  3.020046911e-02 },
            {92, /* SP "K5III             " idx */ 220, /*MAG*/  2.408000000e+00,  8.700000000e-01, -8.000000000e-01, -2.095000000e+00, -2.775000000e+00, -3.044000000e+00, /*EMAG*/  1.360147000e-02,  4.000000000e-02,  2.282542000e-02,  1.940000000e-01,  1.940000000e-01,  1.940000000e-01, /*DIAM*/  1.362718968e+00,  1.350325100e+00,  1.364649481e+00, /*EDIAM*/  5.393608421e-02,  4.696724666e-02,  4.404144133e-02, /*DMEAN*/  1.359197387e+00, /*EDMEAN*/  2.766262845e-02 },
            {93, /* SP "M5IIIv            " idx */ 260, /*MAG*/  9.482000000e+00,  8.000000000e+00,  4.820000000e+00,  2.148000000e+00,  1.292000000e+00,  1.006000000e+00, /*EMAG*/  2.773085000e-02,  4.000000000e-02,  1.205985000e-01,  3.180000000e-01,  3.180000000e-01,  3.180000000e-01, /*DIAM*/  7.135158271e-01,  7.026497585e-01,  6.842985676e-01, /*EDIAM*/  8.838455823e-02,  7.701664865e-02,  7.223602635e-02, /*DMEAN*/  6.982894846e-01, /*EDMEAN*/  4.534093134e-02 },
            {94, /* SP "G8Ib              " idx */ 192, /*MAG*/  4.437000000e+00,  3.060000000e+00,  1.840000000e+00,  8.860000000e-01,  2.320000000e-01,  1.250000000e-01, /*EMAG*/  1.513275000e-02,  4.000000000e-02,  1.019804000e-02,  2.220000000e-01,  2.220000000e-01,  2.220000000e-01, /*DIAM*/  6.591423576e-01,  6.831743755e-01,  6.724538976e-01, /*EDIAM*/  6.163113920e-02,  5.368074515e-02,  5.034077915e-02, /*DMEAN*/  6.726728864e-01, /*EDMEAN*/  3.160585076e-02 },
            {95, /* SP "M1.5IIIb          " idx */ 246, /*MAG*/  7.126000000e+00,  5.430000000e+00,  3.520000000e+00,  1.960000000e+00,  1.002000000e+00,  9.590000000e-01, /*EMAG*/  1.992486000e-02,  4.000000000e-02,  5.035871000e-02,  2.680000000e-01,  2.680000000e-01,  2.680000000e-01, /*DIAM*/  5.976835919e-01,  6.570679141e-01,  6.121266089e-01, /*EDIAM*/  7.434046294e-02,  6.476405338e-02,  6.073819787e-02, /*DMEAN*/  6.238638325e-01, /*EDMEAN*/  3.811061515e-02 },
            {96, /* SP "M1III             " idx */ 244, /*MAG*/  5.541000000e+00,  4.040000000e+00,  2.250000000e+00,  1.176000000e+00,  3.250000000e-01,  1.570000000e-01, /*EMAG*/  3.224903000e-02,  4.000000000e-02,  2.039608000e-02,  2.600000000e-01,  2.600000000e-01,  2.600000000e-01, /*DIAM*/  7.098010234e-01,  7.611549882e-01,  7.546315769e-01, /*EDIAM*/  7.213259049e-02,  6.283849576e-02,  5.893167301e-02, /*DMEAN*/  7.451461325e-01, /*EDMEAN*/  3.697896716e-02 },
            {97, /* SP "M7(III)           " idx */ 268, /*MAG*/  8.353000000e+00,  7.060000000e+00,  2.330000000e+00, -3.560000000e-01, -1.606000000e+00, -2.003000000e+00, /*EMAG*/  1.303840000e-02,  4.000000000e-02,  4.060788000e-02,  3.360000000e-01,  3.360000000e-01,  3.360000000e-01, /*DIAM*/  1.301706815e+00,  1.364367308e+00,  1.347041674e+00, /*EDIAM*/  9.397750151e-02,  8.190663576e-02,  7.683074120e-02, /*DMEAN*/  1.341146834e+00, /*EDMEAN*/  4.828344831e-02 },
            {98, /* SP "M5III             " idx */ 260, /*MAG*/  8.550000000e+00,  7.140000000e+00,  3.500000000e+00,  1.294000000e+00,  2.910000000e-01, -4.000000000e-03, /*EMAG*/  1.835756000e-02,  4.000000000e-02,  4.100000000e-02,  2.780000000e-01,  2.780000000e-01,  2.780000000e-01, /*DIAM*/  8.838551153e-01,  9.086653257e-01,  8.902401765e-01, /*EDIAM*/  7.736562459e-02,  6.740990207e-02,  6.322500281e-02, /*DMEAN*/  8.949306613e-01, /*EDMEAN*/  3.970842490e-02 },
            {99, /* SP "K5III             " idx */ 220, /*MAG*/  5.620000000e+00,  4.050000000e+00,  2.330000000e+00,  1.261000000e+00,  4.290000000e-01,  3.100000000e-01, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  2.022375000e-02,  2.280000000e-01,  2.280000000e-01,  2.280000000e-01, /*DIAM*/  6.780046724e-01,  7.085352073e-01,  6.892772086e-01, /*EDIAM*/  6.331118537e-02,  5.514494163e-02,  5.171350820e-02, /*DMEAN*/  6.929777292e-01, /*EDMEAN*/  3.245813196e-02 },
            {100, /* SP "A1V+DA            " idx */ 84, /*MAG*/ -1.431000000e+00, -1.440000000e+00, -1.420000000e+00, -1.391000000e+00, -1.391000000e+00, -1.390000000e+00, /*EMAG*/              NaN,  4.000000000e-02,              NaN,  2.140000000e-01,  2.140000000e-01,  2.140000000e-01, /*DIAM*/  7.985336326e-01,  7.943958963e-01,  7.820068592e-01, /*EDIAM*/  6.071313613e-02,  5.290081805e-02,  4.962104806e-02, /*DMEAN*/  7.906093284e-01, /*EDMEAN*/  3.125711247e-02 },
            {101, /* SP "M0III             " idx */ 240, /*MAG*/  3.646000000e+00,  2.070000000e+00,  3.300000000e-01, -9.570000000e-01, -1.674000000e+00, -1.846000000e+00, /*EMAG*/  1.077033000e-02,  4.000000000e-02,  3.026549000e-02,  2.060000000e-01,  2.060000000e-01,  2.060000000e-01, /*DIAM*/  1.151148061e+00,  1.158048657e+00,  1.151086713e+00, /*EDIAM*/  5.724192880e-02,  4.985124680e-02,  4.674776593e-02, /*DMEAN*/  1.153507164e+00, /*EDMEAN*/  2.935911649e-02 },
            {102, /* SP "K3IIb             " idx */ 212, /*MAG*/  3.615000000e+00,  2.100000000e+00,  7.300000000e-01, -1.800000000e-02, -8.340000000e-01, -8.610000000e-01, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  1.077033000e-02,  2.180000000e-01,  2.180000000e-01,  2.180000000e-01, /*DIAM*/  8.708477952e-01,  9.229328054e-01,  8.929199606e-01, /*EDIAM*/  6.052528622e-02,  5.271509451e-02,  4.943396538e-02, /*DMEAN*/  8.975096584e-01, /*EDMEAN*/  3.103264902e-02 },
            {103, /* SP "K5III             " idx */ 220, /*MAG*/  2.408000000e+00,  8.700000000e-01, -8.000000000e-01, -2.095000000e+00, -2.775000000e+00, -3.044000000e+00, /*EMAG*/  1.360147000e-02,  4.000000000e-02,  2.282542000e-02,  1.940000000e-01,  1.940000000e-01,  1.940000000e-01, /*DIAM*/  1.362718968e+00,  1.350325100e+00,  1.364649481e+00, /*EDIAM*/  5.393608421e-02,  4.696724666e-02,  4.404144133e-02, /*DMEAN*/  1.359197387e+00, /*EDMEAN*/  2.766262845e-02 },
            {104, /* SP "M3IIIab           " idx */ 252, /*MAG*/  4.491000000e+00,  2.870000000e+00,  5.700000000e-01, -9.120000000e-01, -1.608000000e+00, -1.862000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  5.035871000e-02,  1.900000000e-01,  1.900000000e-01,  1.900000000e-01, /*DIAM*/  1.187201401e+00,  1.186128412e+00,  1.190184754e+00, /*EDIAM*/  5.286076286e-02,  4.603224089e-02,  4.316701131e-02, /*DMEAN*/  1.188003827e+00, /*EDMEAN*/  2.714127475e-02 },
            {105, /* SP "K0III             " idx */ 200, /*MAG*/  2.151000000e+00,  1.160000000e+00,  1.900000000e-01, -3.150000000e-01, -8.450000000e-01, -9.360000000e-01, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  1.166190000e-02,  2.320000000e-01,  2.320000000e-01,  2.320000000e-01, /*DIAM*/  8.602638407e-01,  8.729126400e-01,  8.711045749e-01, /*EDIAM*/  6.437338249e-02,  5.607183299e-02,  5.258345587e-02, /*DMEAN*/  8.688916465e-01, /*EDMEAN*/  3.300440249e-02 },
            {106, /* SP "K7III             " idx */ 228, /*MAG*/  4.690000000e+00,  3.140000000e+00,  1.490000000e+00,  1.200000000e-01, -5.780000000e-01, -6.560000000e-01, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  3.026549000e-02,  2.200000000e-01,  2.200000000e-01,  2.200000000e-01, /*DIAM*/  9.321308283e-01,  9.238533959e-01,  8.943535337e-01, /*EDIAM*/  6.112236429e-02,  5.323589807e-02,  4.992268353e-02, /*DMEAN*/  9.144262704e-01, /*EDMEAN*/  3.133980179e-02 },
            {107, /* SP "K5III             " idx */ 220, /*MAG*/  3.761000000e+00,  2.240000000e+00,  7.000000000e-01, -2.450000000e-01, -1.034000000e+00, -1.162000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  3.014963000e-02,  2.120000000e-01,  2.120000000e-01,  2.120000000e-01, /*DIAM*/  9.558618194e-01,  9.868231492e-01,  9.747954610e-01, /*EDIAM*/  5.889795812e-02,  5.129563449e-02,  4.810227371e-02, /*DMEAN*/  9.739959256e-01, /*EDMEAN*/  3.020046911e-02 },
            {108, /* SP "M5III             " idx */ 260, /*MAG*/  5.477000000e+00,  4.080000000e+00,  9.400000000e-01, -7.380000000e-01, -1.575000000e+00, -1.837000000e+00, /*EMAG*/  1.910497000e-02,  4.000000000e-02,  4.205948000e-02,  2.300000000e-01,  2.300000000e-01,  2.300000000e-01, /*DIAM*/  1.211319406e+00,  1.232618638e+00,  1.224597846e+00, /*EDIAM*/  6.416676385e-02,  5.590138864e-02,  5.242999973e-02, /*DMEAN*/  1.223892985e+00, /*EDMEAN*/  3.296644498e-02 },
            {109, /* SP "M2.5II-IIIe       " idx */ 250, /*MAG*/  4.095000000e+00,  2.440000000e+00,  1.300000000e-01, -1.432000000e+00, -2.129000000e+00, -2.379000000e+00, /*EMAG*/  1.280625000e-02,  4.000000000e-02,  6.053098000e-02,  1.940000000e-01,  1.940000000e-01,  1.940000000e-01, /*DIAM*/  1.301637406e+00,  1.292546689e+00,  1.293600276e+00, /*EDIAM*/  5.393675517e-02,  4.696986094e-02,  4.404598389e-02, /*DMEAN*/  1.295337826e+00, /*EDMEAN*/  2.768414382e-02 },
            {110, /* SP "K4III             " idx */ 216, /*MAG*/  6.212000000e+00,  4.840000000e+00,  3.420000000e+00,  2.521000000e+00,  1.801000000e+00,  1.663000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  2.022375000e-02,  2.860000000e-01,  2.860000000e-01,  2.860000000e-01, /*DIAM*/  3.845235466e-01,  4.051650018e-01,  3.987849248e-01, /*EDIAM*/  7.931287279e-02,  6.909935835e-02,  6.480415181e-02, /*DMEAN*/  3.972527237e-01, /*EDMEAN*/  4.064616769e-02 },
            {111, /* SP "G8Ib              " idx */ 192, /*MAG*/  4.437000000e+00,  3.060000000e+00,  1.840000000e+00,  8.860000000e-01,  2.320000000e-01,  1.250000000e-01, /*EMAG*/  1.513275000e-02,  4.000000000e-02,  1.019804000e-02,  2.220000000e-01,  2.220000000e-01,  2.220000000e-01, /*DIAM*/  6.591423576e-01,  6.831743755e-01,  6.724538976e-01, /*EDIAM*/  6.163113920e-02,  5.368074515e-02,  5.034077915e-02, /*DMEAN*/  6.726728864e-01, /*EDMEAN*/  3.160585076e-02 },
            {112, /* SP "F5IV-V+DQZ        " idx */ 140, /*MAG*/  8.320000000e-01,  4.000000000e-01, -9.000000000e-02, -4.980000000e-01, -6.660000000e-01, -6.580000000e-01, /*EMAG*/              NaN,  4.000000000e-02,              NaN,  3.220000000e-01,  3.220000000e-01,  3.220000000e-01, /*DIAM*/  7.662067463e-01,  7.585438901e-01,  7.401722852e-01, /*EDIAM*/  8.949010692e-02,  7.797484477e-02,  7.313066944e-02, /*DMEAN*/  7.533370204e-01, /*EDMEAN*/  4.586424020e-02 },
            {113, /* SP "K0III             " idx */ 200, /*MAG*/  2.151000000e+00,  1.160000000e+00,  1.900000000e-01, -3.150000000e-01, -8.450000000e-01, -9.360000000e-01, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  1.166190000e-02,  2.320000000e-01,  2.320000000e-01,  2.320000000e-01, /*DIAM*/  8.602638407e-01,  8.729126400e-01,  8.711045749e-01, /*EDIAM*/  6.437338249e-02,  5.607183299e-02,  5.258345587e-02, /*DMEAN*/  8.688916465e-01, /*EDMEAN*/  3.300440249e-02 },
            {114, /* SP "K0IIICN+1         " idx */ 200, /*MAG*/  3.410000000e+00,  2.240000000e+00,  1.110000000e+00,  3.110000000e-01, -2.100000000e-01, -3.030000000e-01, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  2.000000000e-03,  1.980000000e-01,  1.980000000e-01,  1.980000000e-01, /*DIAM*/  7.699245536e-01,  7.642667240e-01,  7.562505586e-01, /*EDIAM*/  5.498990226e-02,  4.788713752e-02,  4.490495141e-02, /*DMEAN*/  7.625951365e-01, /*EDMEAN*/  2.820488865e-02 },
            {115, /* SP "M0III             " idx */ 240, /*MAG*/  3.646000000e+00,  2.070000000e+00,  3.300000000e-01, -9.570000000e-01, -1.674000000e+00, -1.846000000e+00, /*EMAG*/  1.077033000e-02,  4.000000000e-02,  3.026549000e-02,  2.060000000e-01,  2.060000000e-01,  2.060000000e-01, /*DIAM*/  1.151148061e+00,  1.158048657e+00,  1.151086713e+00, /*EDIAM*/  5.724192880e-02,  4.985124680e-02,  4.674776593e-02, /*DMEAN*/  1.153507164e+00, /*EDMEAN*/  2.935911649e-02 },
            {116, /* SP "K3-III            " idx */ 212, /*MAG*/  4.865000000e+00,  3.590000000e+00,  2.360000000e+00,  1.420000000e+00,  7.410000000e-01,  6.490000000e-01, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  2.009975000e-02,  2.420000000e-01,  2.420000000e-01,  2.420000000e-01, /*DIAM*/  5.872406481e-01,  6.044269640e-01,  5.903944087e-01, /*EDIAM*/  6.714974545e-02,  5.849273900e-02,  5.485417869e-02, /*DMEAN*/  5.944147676e-01, /*EDMEAN*/  3.442184415e-02 },
            {117, /* SP "K1IIIb            " idx */ 204, /*MAG*/  3.161000000e+00,  2.010000000e+00,  8.800000000e-01, -1.960000000e-01, -7.490000000e-01, -7.830000000e-01, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  4.000000000e-03,  2.180000000e-01,  2.180000000e-01,  2.180000000e-01, /*DIAM*/  8.997484890e-01,  8.892764057e-01,  8.633493217e-01, /*EDIAM*/  6.050865738e-02,  5.270084185e-02,  4.942081708e-02, /*DMEAN*/  8.818272996e-01, /*EDMEAN*/  3.102581645e-02 },
            {118, /* SP "K7III             " idx */ 228, /*MAG*/  4.690000000e+00,  3.140000000e+00,  1.490000000e+00,  1.200000000e-01, -5.780000000e-01, -6.560000000e-01, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  3.026549000e-02,  2.200000000e-01,  2.200000000e-01,  2.200000000e-01, /*DIAM*/  9.321308283e-01,  9.238533959e-01,  8.943535337e-01, /*EDIAM*/  6.112236429e-02,  5.323589807e-02,  4.992268353e-02, /*DMEAN*/  9.144262704e-01, /*EDMEAN*/  3.133980179e-02 },
            {119, /* SP "M0.5III           " idx */ 242, /*MAG*/  5.706000000e+00,  4.090000000e+00,  2.360000000e+00,  1.139000000e+00,  2.910000000e-01,  1.360000000e-01, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  3.014963000e-02,  2.580000000e-01,  2.580000000e-01,  2.580000000e-01, /*DIAM*/  7.252320123e-01,  7.694135040e-01,  7.582097253e-01, /*EDIAM*/  7.158441823e-02,  6.236031272e-02,  5.848294556e-02, /*DMEAN*/  7.534442113e-01, /*EDMEAN*/  3.669698517e-02 },
            {120, /* SP "K3II              " idx */ 212, /*MAG*/  4.597000000e+00,  3.160000000e+00,  1.850000000e+00,  7.240000000e-01, -1.120000000e-01, -2.300000000e-02, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  2.000000000e-03,  1.780000000e-01,  1.780000000e-01,  1.780000000e-01, /*DIAM*/  7.468656505e-01,  7.924736594e-01,  7.311535348e-01, /*EDIAM*/  4.949548106e-02,  4.309298820e-02,  4.040650933e-02, /*DMEAN*/  7.564394820e-01, /*EDMEAN*/  2.539164088e-02 },
            {121, /* SP "K5III             " idx */ 220, /*MAG*/  3.761000000e+00,  2.240000000e+00,  7.000000000e-01, -2.450000000e-01, -1.034000000e+00, -1.162000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  3.014963000e-02,  2.120000000e-01,  2.120000000e-01,  2.120000000e-01, /*DIAM*/  9.558618194e-01,  9.868231492e-01,  9.747954610e-01, /*EDIAM*/  5.889795812e-02,  5.129563449e-02,  4.810227371e-02, /*DMEAN*/  9.739959256e-01, /*EDMEAN*/  3.020046911e-02 },
            {122, /* SP "M0III             " idx */ 240, /*MAG*/  5.942000000e+00,  4.440000000e+00,  2.760000000e+00,  1.714000000e+00,  8.780000000e-01,  7.110000000e-01, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  2.009975000e-02,  2.980000000e-01,  2.980000000e-01,  2.980000000e-01, /*DIAM*/  5.938355529e-01,  6.401420349e-01,  6.347728370e-01, /*EDIAM*/  8.263719617e-02,  7.199811096e-02,  6.752379109e-02, /*DMEAN*/  6.259036159e-01, /*EDMEAN*/  4.235182537e-02 },
            {123, /* SP "K4.5Ib-II+A1.5e   " idx */ 218, /*MAG*/  5.329000000e+00,  3.720000000e+00,  2.090000000e+00,  9.820000000e-01,  2.700000000e-02, -3.800000000e-02, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  1.044031000e-02,  2.320000000e-01,  2.320000000e-01,  2.320000000e-01, /*DIAM*/  7.272835702e-01,  7.894170817e-01,  7.567892095e-01, /*EDIAM*/  6.440840357e-02,  5.610189222e-02,  5.261124328e-02, /*DMEAN*/  7.603344996e-01, /*EDMEAN*/  3.301928505e-02 },
            {124, /* SP "M2.5II-IIIe       " idx */ 250, /*MAG*/  4.095000000e+00,  2.440000000e+00,  1.300000000e-01, -1.432000000e+00, -2.129000000e+00, -2.379000000e+00, /*EMAG*/  1.280625000e-02,  4.000000000e-02,  6.053098000e-02,  1.940000000e-01,  1.940000000e-01,  1.940000000e-01, /*DIAM*/  1.301637406e+00,  1.292546689e+00,  1.293600276e+00, /*EDIAM*/  5.393675517e-02,  4.696986094e-02,  4.404598389e-02, /*DMEAN*/  1.295337826e+00, /*EDMEAN*/  2.768414382e-02 },
            {125, /* SP "M1.5Iab-Ib        " idx */ 246, /*MAG*/  2.925000000e+00,  1.060000000e+00, -1.840000000e+00, -2.850000000e+00, -3.725000000e+00, -4.100000000e+00, /*EMAG*/  1.612452000e-02,  4.000000000e-02,  3.104835000e-02,  1.600000000e-01,  1.600000000e-01,  1.600000000e-01, /*DIAM*/  1.593469321e+00,  1.617192442e+00,  1.642031734e+00, /*EDIAM*/  4.455635240e-02,  3.878378685e-02,  3.636472116e-02, /*DMEAN*/  1.620772301e+00, /*EDMEAN*/  2.288212648e-02 },
            {126, /* SP "K0IIICN+1         " idx */ 200, /*MAG*/  3.410000000e+00,  2.240000000e+00,  1.110000000e+00,  3.110000000e-01, -2.100000000e-01, -3.030000000e-01, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  2.000000000e-03,  1.980000000e-01,  1.980000000e-01,  1.980000000e-01, /*DIAM*/  7.699245536e-01,  7.642667240e-01,  7.562505586e-01, /*EDIAM*/  5.498990226e-02,  4.788713752e-02,  4.490495141e-02, /*DMEAN*/  7.625951365e-01, /*EDMEAN*/  2.820488865e-02 },
            {127, /* SP "M0III             " idx */ 240, /*MAG*/  3.646000000e+00,  2.070000000e+00,  3.300000000e-01, -9.570000000e-01, -1.674000000e+00, -1.846000000e+00, /*EMAG*/  1.077033000e-02,  4.000000000e-02,  3.026549000e-02,  2.060000000e-01,  2.060000000e-01,  2.060000000e-01, /*DIAM*/  1.151148061e+00,  1.158048657e+00,  1.151086713e+00, /*EDIAM*/  5.724192880e-02,  4.985124680e-02,  4.674776593e-02, /*DMEAN*/  1.153507164e+00, /*EDMEAN*/  2.935911649e-02 },
            {128, /* SP "K1IIIb            " idx */ 204, /*MAG*/  3.161000000e+00,  2.010000000e+00,  8.800000000e-01, -1.960000000e-01, -7.490000000e-01, -7.830000000e-01, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  4.000000000e-03,  2.180000000e-01,  2.180000000e-01,  2.180000000e-01, /*DIAM*/  8.997484890e-01,  8.892764057e-01,  8.633493217e-01, /*EDIAM*/  6.050865738e-02,  5.270084185e-02,  4.942081708e-02, /*DMEAN*/  8.818272996e-01, /*EDMEAN*/  3.102581645e-02 },
            {129, /* SP "M1.5IIIa          " idx */ 246, /*MAG*/  4.170000000e+00,  2.540000000e+00,  5.700000000e-01, -7.220000000e-01, -1.607000000e+00, -1.680000000e+00, /*EMAG*/  4.019950000e-02,  4.000000000e-02,  2.039608000e-02,  1.820000000e-01,  1.820000000e-01,  1.820000000e-01, /*DIAM*/  1.118112171e+00,  1.167278038e+00,  1.133330996e+00, /*EDIAM*/  5.061308785e-02,  4.406895746e-02,  4.132351196e-02, /*DMEAN*/  1.141078478e+00, /*EDMEAN*/  2.597652495e-02 },
            {130, /* SP "K5III             " idx */ 220, /*MAG*/  2.408000000e+00,  8.700000000e-01, -8.000000000e-01, -2.095000000e+00, -2.775000000e+00, -3.044000000e+00, /*EMAG*/  1.360147000e-02,  4.000000000e-02,  2.282542000e-02,  1.940000000e-01,  1.940000000e-01,  1.940000000e-01, /*DIAM*/  1.362718968e+00,  1.350325100e+00,  1.364649481e+00, /*EDIAM*/  5.393608421e-02,  4.696724666e-02,  4.404144133e-02, /*DMEAN*/  1.359197387e+00, /*EDMEAN*/  2.766262845e-02 },
            {131, /* SP "F5IV-V+DQZ        " idx */ 140, /*MAG*/  8.320000000e-01,  4.000000000e-01, -9.000000000e-02, -4.980000000e-01, -6.660000000e-01, -6.580000000e-01, /*EMAG*/              NaN,  4.000000000e-02,              NaN,  3.220000000e-01,  3.220000000e-01,  3.220000000e-01, /*DIAM*/  7.662067463e-01,  7.585438901e-01,  7.401722852e-01, /*EDIAM*/  8.949010692e-02,  7.797484477e-02,  7.313066944e-02, /*DMEAN*/  7.533370204e-01, /*EDMEAN*/  4.586424020e-02 },
            {132, /* SP "K0III             " idx */ 200, /*MAG*/  2.151000000e+00,  1.160000000e+00,  1.900000000e-01, -3.150000000e-01, -8.450000000e-01, -9.360000000e-01, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  1.166190000e-02,  2.320000000e-01,  2.320000000e-01,  2.320000000e-01, /*DIAM*/  8.602638407e-01,  8.729126400e-01,  8.711045749e-01, /*EDIAM*/  6.437338249e-02,  5.607183299e-02,  5.258345587e-02, /*DMEAN*/  8.688916465e-01, /*EDMEAN*/  3.300440249e-02 },
            {133, /* SP "K0III-IV          " idx */ 200, /*MAG*/  3.501000000e+00,  2.480000000e+00,  1.480000000e+00,  6.410000000e-01,  1.040000000e-01, -7.000000000e-03, /*EMAG*/  1.513275000e-02,  4.000000000e-02,  2.009975000e-02,  2.180000000e-01,  2.180000000e-01,  2.180000000e-01, /*DIAM*/  6.970138383e-01,  6.984145829e-01,  6.955790249e-01, /*EDIAM*/  6.050857208e-02,  5.270100003e-02,  4.942115649e-02, /*DMEAN*/  6.969337152e-01, /*EDMEAN*/  3.102737818e-02 },
            {134, /* SP "M2.5II-IIIe       " idx */ 250, /*MAG*/  4.095000000e+00,  2.440000000e+00,  1.300000000e-01, -1.432000000e+00, -2.129000000e+00, -2.379000000e+00, /*EMAG*/  1.280625000e-02,  4.000000000e-02,  6.053098000e-02,  1.940000000e-01,  1.940000000e-01,  1.940000000e-01, /*DIAM*/  1.301637406e+00,  1.292546689e+00,  1.293600276e+00, /*EDIAM*/  5.393675517e-02,  4.696986094e-02,  4.404598389e-02, /*DMEAN*/  1.295337826e+00, /*EDMEAN*/  2.768414382e-02 },
            {135, /* SP "M5III             " idx */ 260, /*MAG*/  9.985000000e+00,  8.420000000e+00,  4.470000000e+00,  2.159000000e+00,  1.224000000e+00,  9.010000000e-01, /*EMAG*/  3.492850000e-02,  4.000000000e-02,  4.237924000e-02,  3.200000000e-01,  3.200000000e-01,  3.200000000e-01, /*DIAM*/  7.427211847e-01,  7.363773854e-01,  7.190941886e-01, /*EDIAM*/  8.893584718e-02,  7.749726683e-02,  7.268683890e-02, /*DMEAN*/  7.312519060e-01, /*EDMEAN*/  4.562280130e-02 },
            {136, /* SP "K5III             " idx */ 220, /*MAG*/  8.736000000e+00,  6.830000000e+00,  4.910000000e+00,  2.902000000e+00,  1.947000000e+00,  1.697000000e+00, /*EMAG*/  1.746425000e-02,  4.000000000e-02,  9.027181000e-02,  2.560000000e-01,  2.560000000e-01,  2.560000000e-01, /*DIAM*/  4.372635974e-01,  4.569865654e-01,  4.484815846e-01, /*EDIAM*/  7.103892866e-02,  6.188441114e-02,  5.803592814e-02, /*DMEAN*/  4.484814046e-01, /*EDMEAN*/  3.641214744e-02 },
            {137, /* SP "K4IIIb            " idx */ 216, /*MAG*/  5.940000000e+00,  4.440000000e+00,  2.860000000e+00,  2.031000000e+00,  1.198000000e+00,  1.105000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  2.022375000e-02,  2.860000000e-01,  2.860000000e-01,  2.860000000e-01, /*DIAM*/  4.894342624e-01,  5.341377664e-01,  5.145367513e-01, /*EDIAM*/  7.931287279e-02,  6.909935835e-02,  6.480415181e-02, /*DMEAN*/  5.147304311e-01, /*EDMEAN*/  4.064616769e-02 },
            {138, /* SP "M2III             " idx */ 248, /*MAG*/  7.554000000e+00,  5.970000000e+00,  3.770000000e+00,  2.450000000e+00,  1.582000000e+00,  1.444000000e+00, /*EMAG*/  1.029563000e-02,  4.000000000e-02,  3.041381000e-02,  3.180000000e-01,  3.180000000e-01,  3.180000000e-01, /*DIAM*/  5.011450179e-01,  5.412113349e-01,  5.189652759e-01, /*EDIAM*/  8.815677323e-02,  7.681140981e-02,  7.203951345e-02, /*DMEAN*/  5.219783695e-01, /*EDMEAN*/  4.518289309e-02 },
            {139, /* SP "G8Ib              " idx */ 192, /*MAG*/  4.437000000e+00,  3.060000000e+00,  1.840000000e+00,  8.860000000e-01,  2.320000000e-01,  1.250000000e-01, /*EMAG*/  1.513275000e-02,  4.000000000e-02,  1.019804000e-02,  2.220000000e-01,  2.220000000e-01,  2.220000000e-01, /*DIAM*/  6.591423576e-01,  6.831743755e-01,  6.724538976e-01, /*EDIAM*/  6.163113920e-02,  5.368074515e-02,  5.034077915e-02, /*DMEAN*/  6.726728864e-01, /*EDMEAN*/  3.160585076e-02 },
            {140, /* SP "M2III             " idx */ 248, /*MAG*/  6.993000000e+00,  5.350000000e+00,  3.230000000e+00,  1.876000000e+00,  1.037000000e+00,  8.150000000e-01, /*EMAG*/  1.334166000e-02,  4.000000000e-02,  5.008992000e-02,  2.860000000e-01,  2.860000000e-01,  2.860000000e-01, /*DIAM*/  6.124128767e-01,  6.471179513e-01,  6.450017741e-01, /*EDIAM*/  7.931514490e-02,  6.910223050e-02,  6.480794635e-02, /*DMEAN*/  6.371971604e-01, /*EDMEAN*/  4.065854102e-02 },
            {141, /* SP "K4III             " idx */ 216, /*MAG*/  7.117000000e+00,  5.590000000e+00,  4.020000000e+00,  3.047000000e+00,  2.249000000e+00,  2.035000000e+00, /*EMAG*/  3.712142000e-02,  4.000000000e-02,  3.014963000e-02,  2.800000000e-01,  2.800000000e-01,  2.800000000e-01, /*DIAM*/  2.965235453e-01,  3.280210318e-01,  3.343177705e-01, /*EDIAM*/  7.765541262e-02,  6.765409664e-02,  6.344838827e-02, /*DMEAN*/  3.222449597e-01, /*EDMEAN*/  3.979788571e-02 },
            {142, /* SP "M0III             " idx */ 240, /*MAG*/  3.646000000e+00,  2.070000000e+00,  3.300000000e-01, -9.570000000e-01, -1.674000000e+00, -1.846000000e+00, /*EMAG*/  1.077033000e-02,  4.000000000e-02,  3.026549000e-02,  2.060000000e-01,  2.060000000e-01,  2.060000000e-01, /*DIAM*/  1.151148061e+00,  1.158048657e+00,  1.151086713e+00, /*EDIAM*/  5.724192880e-02,  4.985124680e-02,  4.674776593e-02, /*DMEAN*/  1.153507164e+00, /*EDMEAN*/  2.935911649e-02 },
            {143, /* SP "M1.5IIIa          " idx */ 246, /*MAG*/  4.170000000e+00,  2.540000000e+00,  5.700000000e-01, -7.220000000e-01, -1.607000000e+00, -1.680000000e+00, /*EMAG*/  4.019950000e-02,  4.000000000e-02,  2.039608000e-02,  1.820000000e-01,  1.820000000e-01,  1.820000000e-01, /*DIAM*/  1.118112171e+00,  1.167278038e+00,  1.133330996e+00, /*EDIAM*/  5.061308785e-02,  4.406895746e-02,  4.132351196e-02, /*DMEAN*/  1.141078478e+00, /*EDMEAN*/  2.597652495e-02 },
            {144, /* SP "K5III             " idx */ 220, /*MAG*/  2.408000000e+00,  8.700000000e-01, -8.000000000e-01, -2.095000000e+00, -2.775000000e+00, -3.044000000e+00, /*EMAG*/  1.360147000e-02,  4.000000000e-02,  2.282542000e-02,  1.940000000e-01,  1.940000000e-01,  1.940000000e-01, /*DIAM*/  1.362718968e+00,  1.350325100e+00,  1.364649481e+00, /*EDIAM*/  5.393608421e-02,  4.696724666e-02,  4.404144133e-02, /*DMEAN*/  1.359197387e+00, /*EDMEAN*/  2.766262845e-02 },
            {145, /* SP "M2Iab-Ib          " idx */ 248, /*MAG*/  6.380000000e+00,  4.320000000e+00,  1.780000000e+00,  3.930000000e-01, -6.090000000e-01, -9.130000000e-01, /*EMAG*/  2.915476000e-02,  4.000000000e-02,  3.195309000e-02,  2.200000000e-01,  2.200000000e-01,  2.200000000e-01, /*DIAM*/  9.437968102e-01,  1.001724960e+00,  1.008943385e+00, /*EDIAM*/  6.109348273e-02,  5.321181537e-02,  4.990136622e-02, /*DMEAN*/  9.894048670e-01, /*EDMEAN*/  3.133779163e-02 },
            {146, /* SP "M3IIb             " idx */ 252, /*MAG*/  6.001000000e+00,  4.300000000e+00,  1.790000000e+00,  2.450000000e-01, -6.020000000e-01, -8.130000000e-01, /*EMAG*/  1.486607000e-02,  4.000000000e-02,  3.041381000e-02,  2.200000000e-01,  2.200000000e-01,  2.200000000e-01, /*DIAM*/  9.767638977e-01,  1.002416347e+00,  9.903964296e-01, /*EDIAM*/  6.112746771e-02,  5.324300460e-02,  4.993164004e-02, /*DMEAN*/  9.909806525e-01, /*EDMEAN*/  3.136567615e-02 },
            {147, /* SP "M3IIIab           " idx */ 252, /*MAG*/  4.491000000e+00,  2.870000000e+00,  5.700000000e-01, -9.120000000e-01, -1.608000000e+00, -1.862000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  5.035871000e-02,  1.900000000e-01,  1.900000000e-01,  1.900000000e-01, /*DIAM*/  1.187201401e+00,  1.186128412e+00,  1.190184754e+00, /*EDIAM*/  5.286076286e-02,  4.603224089e-02,  4.316701131e-02, /*DMEAN*/  1.188003827e+00, /*EDMEAN*/  2.714127475e-02 },
            {148, /* SP "M4II              " idx */ 256, /*MAG*/  5.795000000e+00,  4.220000000e+00,  1.620000000e+00, -1.120000000e-01, -1.039000000e+00, -1.255000000e+00, /*EMAG*/  1.140175000e-02,  4.000000000e-02,  3.080584000e-02,  2.100000000e-01,  2.100000000e-01,  2.100000000e-01, /*DIAM*/  1.060489114e+00,  1.107157599e+00,  1.092623839e+00, /*EDIAM*/  5.847068633e-02,  5.092882050e-02,  4.776253167e-02, /*DMEAN*/  1.089237539e+00, /*EDMEAN*/  3.002701433e-02 },
            {149, /* SP "M5III             " idx */ 260, /*MAG*/  5.477000000e+00,  4.080000000e+00,  9.400000000e-01, -7.380000000e-01, -1.575000000e+00, -1.837000000e+00, /*EMAG*/  1.910497000e-02,  4.000000000e-02,  4.205948000e-02,  2.300000000e-01,  2.300000000e-01,  2.300000000e-01, /*DIAM*/  1.211319406e+00,  1.232618638e+00,  1.224597846e+00, /*EDIAM*/  6.416676385e-02,  5.590138864e-02,  5.242999973e-02, /*DMEAN*/  1.223892985e+00, /*EDMEAN*/  3.296644498e-02 },
            {150, /* SP "M2.5II-IIIe       " idx */ 250, /*MAG*/  4.095000000e+00,  2.440000000e+00,  1.300000000e-01, -1.432000000e+00, -2.129000000e+00, -2.379000000e+00, /*EMAG*/  1.280625000e-02,  4.000000000e-02,  6.053098000e-02,  1.940000000e-01,  1.940000000e-01,  1.940000000e-01, /*DIAM*/  1.301637406e+00,  1.292546689e+00,  1.293600276e+00, /*EDIAM*/  5.393675517e-02,  4.696986094e-02,  4.404598389e-02, /*DMEAN*/  1.295337826e+00, /*EDMEAN*/  2.768414382e-02 },
            {151, /* SP "M8(III)           " idx */ 272, /*MAG*/  6.954000000e+00,  5.590000000e+00,  8.900000000e-01, -2.652000000e+00, -3.732000000e+00, -4.227000000e+00, /*EMAG*/  1.700000000e-02,  4.000000000e-02,  3.440930000e-02,  2.060000000e-01,  2.060000000e-01,  2.060000000e-01, /*DIAM*/  1.802740582e+00,  1.815826689e+00,  1.814122327e+00, /*EDIAM*/  5.958092776e-02,  5.194510999e-02,  4.874308853e-02, /*DMEAN*/  1.811734430e+00, /*EDMEAN*/  3.087380509e-02 },
            {152, /* SP "M2III             " idx */ 248, /*MAG*/  7.554000000e+00,  5.970000000e+00,  3.770000000e+00,  2.450000000e+00,  1.582000000e+00,  1.444000000e+00, /*EMAG*/  1.029563000e-02,  4.000000000e-02,  3.041381000e-02,  3.180000000e-01,  3.180000000e-01,  3.180000000e-01, /*DIAM*/  5.011450179e-01,  5.412113349e-01,  5.189652759e-01, /*EDIAM*/  8.815677323e-02,  7.681140981e-02,  7.203951345e-02, /*DMEAN*/  5.219783695e-01, /*EDMEAN*/  4.518289309e-02 },
            {153, /* SP "M5IIIv            " idx */ 260, /*MAG*/  9.482000000e+00,  8.000000000e+00,  4.820000000e+00,  2.148000000e+00,  1.292000000e+00,  1.006000000e+00, /*EMAG*/  2.773085000e-02,  4.000000000e-02,  1.205985000e-01,  3.180000000e-01,  3.180000000e-01,  3.180000000e-01, /*DIAM*/  7.135158271e-01,  7.026497585e-01,  6.842985676e-01, /*EDIAM*/  8.838455823e-02,  7.701664865e-02,  7.223602635e-02, /*DMEAN*/  6.982894846e-01, /*EDMEAN*/  4.534093134e-02 },
            {154, /* SP "M4                " idx */ 256, /*MAG*/  1.020400000e+01,  8.779000000e+00,              NaN,  2.674000000e+00,  1.751000000e+00,  1.362000000e+00, /*EMAG*/  5.000000000e-02,  4.000000000e-02,              NaN,  2.280000000e-01,  2.280000000e-01,  2.280000000e-01, /*DIAM*/  6.394301793e-01,  6.221202376e-01,  6.202544886e-01, /*EDIAM*/  6.342568092e-02,  5.525022502e-02,  5.181632607e-02, /*DMEAN*/  6.259168013e-01, /*EDMEAN*/  3.255830134e-02 },
            {155, /* SP "M2Iab-Ib          " idx */ 248, /*MAG*/  6.380000000e+00,  4.320000000e+00,  1.780000000e+00,  3.930000000e-01, -6.090000000e-01, -9.130000000e-01, /*EMAG*/  2.915476000e-02,  4.000000000e-02,  3.195309000e-02,  2.200000000e-01,  2.200000000e-01,  2.200000000e-01, /*DIAM*/  9.437968102e-01,  1.001724960e+00,  1.008943385e+00, /*EDIAM*/  6.109348273e-02,  5.321181537e-02,  4.990136622e-02, /*DMEAN*/  9.894048670e-01, /*EDMEAN*/  3.133779163e-02 },
            {156, /* SP "M2Iab-Ib          " idx */ 248, /*MAG*/  6.380000000e+00,  4.320000000e+00,  1.780000000e+00,  3.930000000e-01, -6.090000000e-01, -9.130000000e-01, /*EMAG*/  2.915476000e-02,  4.000000000e-02,  3.195309000e-02,  2.200000000e-01,  2.200000000e-01,  2.200000000e-01, /*DIAM*/  9.437968102e-01,  1.001724960e+00,  1.008943385e+00, /*EDIAM*/  6.109348273e-02,  5.321181537e-02,  4.990136622e-02, /*DMEAN*/  9.894048670e-01, /*EDMEAN*/  3.133779163e-02 },
            {157, /* SP "K4III             " idx */ 216, /*MAG*/  7.556000000e+00,  5.930000000e+00,  4.250000000e+00,  3.143000000e+00,  2.287000000e+00,  2.129000000e+00, /*EMAG*/  1.581139000e-02,  4.000000000e-02,  4.031129000e-02,  2.680000000e-01,  2.680000000e-01,  2.680000000e-01, /*DIAM*/  2.960592596e-01,  3.328770630e-01,  3.219820039e-01, /*EDIAM*/  7.434092802e-02,  6.476386924e-02,  6.073711593e-02, /*DMEAN*/  3.189559536e-01, /*EDMEAN*/  3.810162090e-02 },
            {158, /* SP "M4III             " idx */ 256, /*MAG*/  7.911000000e+00,  6.320000000e+00,  2.900000000e+00,  8.970000000e-01, -1.640000000e-01, -5.120000000e-01, /*EMAG*/  2.842534000e-02,  4.000000000e-02,  6.264184000e-02,  3.200000000e-01,  3.200000000e-01,  3.200000000e-01, /*DIAM*/  9.424623267e-01,  9.826828889e-01,  9.796822312e-01, /*EDIAM*/  8.879866775e-02,  7.737414917e-02,  7.256930379e-02, /*DMEAN*/  9.709674786e-01, /*EDMEAN*/  4.553133643e-02 },
            {159, /* SP "K0III             " idx */ 200, /*MAG*/  5.893000000e+00,  4.880000000e+00,  3.890000000e+00,  3.025000000e+00,  2.578000000e+00,  2.494000000e+00, /*EMAG*/  2.517936000e-02,  4.000000000e-02,  3.014963000e-02,  2.680000000e-01,  2.680000000e-01,  2.680000000e-01, /*DIAM*/  2.214424026e-01,  2.005624354e-01,  1.927250028e-01, /*EDIAM*/  7.431633947e-02,  6.474284759e-02,  6.071775602e-02, /*DMEAN*/  2.029516171e-01, /*EDMEAN*/  3.809176902e-02 },
            {160, /* SP "M6                " idx */ 264, /*MAG*/  1.001900000e+01,  8.656000000e+00,              NaN,  2.640000000e+00,  1.695000000e+00,  1.371000000e+00, /*EMAG*/  4.300000000e-02,  4.000000000e-02,              NaN,  2.340000000e-01,  2.340000000e-01,  2.340000000e-01, /*DIAM*/  6.130064712e-01,  6.336178569e-01,  6.224808792e-01, /*EDIAM*/  6.560356984e-02,  5.716212112e-02,  5.361705954e-02, /*DMEAN*/  6.238475165e-01, /*EDMEAN*/  3.374636603e-02 },
            {161, /* SP "M6III             " idx */ 264, /*MAG*/  9.754000000e+00,  8.460000000e+00,  4.670000000e+00,  2.218000000e+00,  1.077000000e+00,  1.021000000e+00, /*EMAG*/  4.070626000e-02,  4.000000000e-02,  6.293648000e-02,  2.340000000e-01,  2.340000000e-01,  2.340000000e-01, /*DIAM*/  7.147600441e-01,  7.746233065e-01,  6.965275956e-01, /*EDIAM*/  6.560356984e-02,  5.716212112e-02,  5.361705954e-02, /*DMEAN*/  7.282690280e-01, /*EDMEAN*/  3.374636603e-02 },
            {162, /* SP "G7IIIFe-3CH1      " idx */ 188, /*MAG*/  5.211000000e+00,  4.340000000e+00,  3.420000000e+00,  2.578000000e+00,  2.154000000e+00,  2.074000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  1.044031000e-02,  2.500000000e-01,  2.500000000e-01,  2.500000000e-01, /*DIAM*/  2.818587197e-01,  2.685153906e-01,  2.611201912e-01, /*EDIAM*/  6.937625637e-02,  6.043577103e-02,  5.667798070e-02, /*DMEAN*/  2.691034305e-01, /*EDMEAN*/  3.556988892e-02 },
            {163, /* SP "K0IIICN+1         " idx */ 200, /*MAG*/  3.410000000e+00,  2.240000000e+00,  1.110000000e+00,  3.110000000e-01, -2.100000000e-01, -3.030000000e-01, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  2.000000000e-03,  1.980000000e-01,  1.980000000e-01,  1.980000000e-01, /*DIAM*/  7.699245536e-01,  7.642667240e-01,  7.562505586e-01, /*EDIAM*/  5.498990226e-02,  4.788713752e-02,  4.490495141e-02, /*DMEAN*/  7.625951365e-01, /*EDMEAN*/  2.820488865e-02 },
            {164, /* SP "G8.5IIIbFe-0.5    " idx */ 194, /*MAG*/  5.577000000e+00,  4.620000000e+00,  3.610000000e+00,  3.123000000e+00,  2.680000000e+00,  2.468000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  4.004997000e-02,  2.800000000e-01,  2.800000000e-01,  2.800000000e-01, /*DIAM*/  1.634114584e-01,  1.589319134e-01,  1.853302495e-01, /*EDIAM*/  7.764107202e-02,  6.764235505e-02,  6.343799800e-02, /*DMEAN*/  1.704738637e-01, /*EDMEAN*/  3.979562077e-02 },
            {165, /* SP "G8.5III           " idx */ 194, /*MAG*/  5.684000000e+00,  4.660000000e+00,  3.670000000e+00,  3.074000000e+00,  2.513000000e+00,  2.481000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  2.022375000e-02,  2.520000000e-01,  2.520000000e-01,  2.520000000e-01, /*DIAM*/  1.800453872e-01,  2.008696572e-01,  1.834397385e-01, /*EDIAM*/  6.990671349e-02,  6.089788747e-02,  5.711114444e-02, /*DMEAN*/  1.885701157e-01, /*EDMEAN*/  3.583812305e-02 },
            {166, /* SP "G9IIIb            " idx */ 196, /*MAG*/  5.671000000e+00,  4.680000000e+00,  3.670000000e+00,  3.019000000e+00,  2.481000000e+00,  2.311000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  3.006659000e-02,  2.180000000e-01,  2.180000000e-01,  2.180000000e-01, /*DIAM*/  2.004623465e-01,  2.114575261e-01,  2.245330539e-01, /*EDIAM*/  6.051486221e-02,  5.270678269e-02,  4.942681342e-02, /*DMEAN*/  2.137191976e-01, /*EDMEAN*/  3.103261995e-02 },
            {167, /* SP "K3-III            " idx */ 212, /*MAG*/  4.865000000e+00,  3.590000000e+00,  2.360000000e+00,  1.420000000e+00,  7.410000000e-01,  6.490000000e-01, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  2.009975000e-02,  2.420000000e-01,  2.420000000e-01,  2.420000000e-01, /*DIAM*/  5.872406481e-01,  6.044269640e-01,  5.903944087e-01, /*EDIAM*/  6.714974545e-02,  5.849273900e-02,  5.485417869e-02, /*DMEAN*/  5.944147676e-01, /*EDMEAN*/  3.442184415e-02 },
            {168, /* SP "K1IIIb            " idx */ 204, /*MAG*/  3.161000000e+00,  2.010000000e+00,  8.800000000e-01, -1.960000000e-01, -7.490000000e-01, -7.830000000e-01, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  4.000000000e-03,  2.180000000e-01,  2.180000000e-01,  2.180000000e-01, /*DIAM*/  8.997484890e-01,  8.892764057e-01,  8.633493217e-01, /*EDIAM*/  6.050865738e-02,  5.270084185e-02,  4.942081708e-02, /*DMEAN*/  8.818272996e-01, /*EDMEAN*/  3.102581645e-02 },
            {169, /* SP "K1.5III           " idx */ 206, /*MAG*/  5.632000000e+00,  4.520000000e+00,  3.480000000e+00,  2.875000000e+00,  2.347000000e+00,  2.099000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  3.006659000e-02,  3.140000000e-01,  3.140000000e-01,  3.140000000e-01, /*DIAM*/  2.459654032e-01,  2.482088949e-01,  2.795040482e-01, /*EDIAM*/  8.702999233e-02,  7.582830382e-02,  7.111644130e-02, /*DMEAN*/  2.599111268e-01, /*EDMEAN*/  4.459683339e-02 },
            {170, /* SP "K2III             " idx */ 208, /*MAG*/  5.383000000e+00,  4.350000000e+00,  3.390000000e+00,  2.775000000e+00,  2.303000000e+00,  2.169000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  1.044031000e-02,  2.740000000e-01,  2.740000000e-01,  2.740000000e-01, /*DIAM*/  2.640084441e-01,  2.541554200e-01,  2.615683150e-01, /*EDIAM*/  7.597886407e-02,  6.619226008e-02,  6.207715785e-02, /*DMEAN*/  2.596475396e-01, /*EDMEAN*/  3.894037420e-02 },
            {171, /* SP "F5Ib              " idx */ 140, /*MAG*/  2.271000000e+00,  1.790000000e+00,  1.160000000e+00,  8.300000000e-01,  4.130000000e-01,  5.200000000e-01, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  4.019950000e-02,  1.840000000e-01,  1.840000000e-01,  1.840000000e-01, /*DIAM*/  5.053674567e-01,  5.555711244e-01,  5.101430847e-01, /*EDIAM*/  5.156484382e-02,  4.490146625e-02,  4.210463273e-02, /*DMEAN*/  5.245849158e-01, /*EDMEAN*/  2.646513615e-02 },
            {172, /* SP "K0III             " idx */ 200, /*MAG*/  5.424000000e+00,  4.360000000e+00,  3.340000000e+00,  2.579000000e+00,  2.055000000e+00,  2.033000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  1.044031000e-02,  2.580000000e-01,  2.580000000e-01,  2.580000000e-01, /*DIAM*/  3.049602610e-01,  3.052861724e-01,  2.833746392e-01, /*EDIAM*/  7.155381176e-02,  6.233384672e-02,  5.845790177e-02, /*DMEAN*/  2.965931444e-01, /*EDMEAN*/  3.667817296e-02 },
            {173, /* SP "F0Ib              " idx */ 120, /*MAG*/  2.791000000e+00,  2.580000000e+00,  2.260000000e+00,  2.030000000e+00,  1.857000000e+00,  1.756000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  2.002498000e-02,  3.280000000e-01,  3.280000000e-01,  3.280000000e-01, /*DIAM*/  2.178395493e-01,  2.298437897e-01,  2.355292234e-01, /*EDIAM*/  9.134696433e-02,  7.959473442e-02,  7.465058115e-02, /*DMEAN*/  2.289312051e-01, /*EDMEAN*/  4.681972606e-02 },
            {174, /* SP "G8Ib              " idx */ 192, /*MAG*/  4.437000000e+00,  3.060000000e+00,  1.840000000e+00,  8.860000000e-01,  2.320000000e-01,  1.250000000e-01, /*EMAG*/  1.513275000e-02,  4.000000000e-02,  1.019804000e-02,  2.220000000e-01,  2.220000000e-01,  2.220000000e-01, /*DIAM*/  6.591423576e-01,  6.831743755e-01,  6.724538976e-01, /*EDIAM*/  6.163113920e-02,  5.368074515e-02,  5.034077915e-02, /*DMEAN*/  6.726728864e-01, /*EDMEAN*/  3.160585076e-02 },
            {175, /* SP "K4III_Ba0.5       " idx */ 216, /*MAG*/  5.011000000e+00,  3.530000000e+00,  2.060000000e+00,  1.191000000e+00,  2.670000000e-01,  1.900000000e-01, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  2.022375000e-02,  2.500000000e-01,  2.500000000e-01,  2.500000000e-01, /*DIAM*/  6.520592649e-01,  7.212039170e-01,  6.976681409e-01, /*EDIAM*/  6.937044661e-02,  6.042937434e-02,  5.667093567e-02, /*DMEAN*/  6.938550807e-01, /*EDMEAN*/  3.555807851e-02 },
            {176, /* SP "M2III             " idx */ 248, /*MAG*/  6.269000000e+00,  4.680000000e+00,  2.720000000e+00,  1.530000000e+00,  7.060000000e-01,  4.900000000e-01, /*EMAG*/  4.110961000e-02,  4.000000000e-02,  2.022375000e-02,  2.800000000e-01,  2.800000000e-01,  2.800000000e-01, /*DIAM*/  6.567343060e-01,  6.993358509e-01,  7.009360815e-01, /*EDIAM*/  7.765773323e-02,  6.765703015e-02,  6.345226389e-02, /*DMEAN*/  6.888073220e-01, /*EDMEAN*/  3.981052266e-02 },
            {177, /* SP "K4III             " idx */ 216, /*MAG*/  5.838000000e+00,  4.390000000e+00,  2.880000000e+00,  1.897000000e+00,  1.220000000e+00,  1.022000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  6.007495000e-02,  3.220000000e-01,  3.220000000e-01,  3.220000000e-01, /*DIAM*/  5.226842629e-01,  5.267681165e-01,  5.320039049e-01, /*EDIAM*/  8.926017416e-02,  7.777265413e-02,  7.294021790e-02, /*DMEAN*/  5.277542496e-01, /*EDMEAN*/  4.573760424e-02 },
            {178, /* SP "K0III             " idx */ 200, /*MAG*/  4.830000000e+00,  3.790000000e+00,  2.720000000e+00,  2.070000000e+00,  1.426000000e+00,  1.390000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  2.009975000e-02,  1.900000000e-01,  1.900000000e-01,  1.900000000e-01, /*DIAM*/  4.020763339e-01,  4.335196373e-01,  4.138928893e-01, /*EDIAM*/  5.278343382e-02,  4.596224078e-02,  4.309901816e-02, /*DMEAN*/  4.175820833e-01, /*EDMEAN*/  2.707662782e-02 },
            {179, /* SP "K0.5IIIb:         " idx */ 202, /*MAG*/  4.871000000e+00,  3.690000000e+00,  2.540000000e+00,  1.679000000e+00,  1.025000000e+00,  9.880000000e-01, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  3.006659000e-02,  2.620000000e-01,  2.620000000e-01,  2.620000000e-01, /*DIAM*/  5.062211009e-01,  5.283424535e-01,  5.044710536e-01, /*EDIAM*/  7.265815823e-02,  6.329677437e-02,  5.936113089e-02, /*DMEAN*/  5.131726758e-01, /*EDMEAN*/  3.724254254e-02 },
            {180, /* SP "G8III             " idx */ 192, /*MAG*/  3.784000000e+00,  2.850000000e+00,  2.020000000e+00,  1.248000000e+00,  7.330000000e-01,  6.640000000e-01, /*EMAG*/  1.513275000e-02,  4.000000000e-02,  1.019804000e-02,  2.780000000e-01,  2.780000000e-01,  2.780000000e-01, /*DIAM*/  5.428209273e-01,  5.536490817e-01,  5.449721438e-01, /*EDIAM*/  7.709401116e-02,  6.716547032e-02,  6.299075004e-02, /*DMEAN*/  5.474050435e-01, /*EDMEAN*/  3.951660736e-02 },
            {181, /* SP "K5.5III           " idx */ 222, /*MAG*/  5.570000000e+00,  4.050000000e+00,  2.450000000e+00,  1.218000000e+00,  4.380000000e-01,  4.350000000e-01, /*EMAG*/  1.236932000e-02,  4.000000000e-02,  3.014963000e-02,  2.640000000e-01,  2.640000000e-01,  2.640000000e-01, /*DIAM*/  6.923095599e-01,  7.088553662e-01,  6.635736892e-01, /*EDIAM*/  7.325288413e-02,  6.381508509e-02,  5.984710592e-02, /*DMEAN*/  6.867351453e-01, /*EDMEAN*/  3.754533192e-02 },
            {182, /* SP "G8IV              " idx */ 192, /*MAG*/  4.421000000e+00,  3.460000000e+00,  2.500000000e+00,  1.659000000e+00,  9.850000000e-01,  1.223000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  3.006659000e-02,  2.380000000e-01,  2.380000000e-01,  2.380000000e-01, /*DIAM*/  4.759012834e-01,  5.180148399e-01,  4.345122882e-01, /*EDIAM*/  6.604732657e-02,  5.753232672e-02,  5.395402398e-02, /*DMEAN*/  4.741826510e-01, /*EDMEAN*/  3.386475802e-02 },
            {183, /* SP "G7IIIFe-1         " idx */ 188, /*MAG*/  4.396000000e+00,  3.480000000e+00,  2.590000000e+00,  1.768000000e+00,  1.366000000e+00,  1.333000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  3.006659000e-02,  3.040000000e-01,  3.040000000e-01,  3.040000000e-01, /*DIAM*/  4.400194363e-01,  4.231457431e-01,  4.061931861e-01, /*EDIAM*/  8.429080363e-02,  7.344092945e-02,  6.887768881e-02, /*DMEAN*/  4.209082094e-01, /*EDMEAN*/  4.320126511e-02 },
            {184, /* SP "K3II              " idx */ 212, /*MAG*/  4.597000000e+00,  3.160000000e+00,  1.850000000e+00,  7.240000000e-01, -1.120000000e-01, -2.300000000e-02, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  2.000000000e-03,  1.780000000e-01,  1.780000000e-01,  1.780000000e-01, /*DIAM*/  7.468656505e-01,  7.924736594e-01,  7.311535348e-01, /*EDIAM*/  4.949548106e-02,  4.309298820e-02,  4.040650933e-02, /*DMEAN*/  7.564394820e-01, /*EDMEAN*/  2.539164088e-02 },
            {185, /* SP "K1III             " idx */ 204, /*MAG*/  5.102000000e+00,  4.020000000e+00,  3.020000000e+00,  2.267000000e+00,  1.911000000e+00,  1.786000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  3.014963000e-02,  2.860000000e-01,  2.860000000e-01,  2.860000000e-01, /*DIAM*/  3.723645526e-01,  3.304670588e-01,  3.348602628e-01, /*EDIAM*/  7.928989119e-02,  6.907953539e-02,  6.478574835e-02, /*DMEAN*/  3.431663378e-01, /*EDMEAN*/  4.063574028e-02 },
            {186, /* SP "K0III             " idx */ 200, /*MAG*/  5.608000000e+00,  4.350000000e+00,  3.220000000e+00,  2.213000000e+00,  1.598000000e+00,  1.514000000e+00, /*EMAG*/  1.019804000e-02,  4.000000000e-02,  3.006659000e-02,  2.660000000e-01,  2.660000000e-01,  2.660000000e-01, /*DIAM*/  4.054959768e-01,  4.151227499e-01,  4.005498234e-01, /*EDIAM*/  7.376380196e-02,  6.426102671e-02,  6.026576765e-02, /*DMEAN*/  4.068773920e-01, /*EDMEAN*/  3.780902624e-02 },
            {187, /* SP "G9III             " idx */ 196, /*MAG*/  4.750000000e+00,  3.800000000e+00,  2.950000000e+00,  2.319000000e+00,  1.823000000e+00,  1.757000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  3.006659000e-02,  2.660000000e-01,  2.660000000e-01,  2.660000000e-01, /*DIAM*/  3.266409199e-01,  3.339011077e-01,  3.267666320e-01, /*EDIAM*/  7.376896185e-02,  6.426576920e-02,  6.027040672e-02, /*DMEAN*/  3.291974076e-01, /*EDMEAN*/  3.781332794e-02 },
            {188, /* SP "K3II              " idx */ 212, /*MAG*/  4.227000000e+00,  2.720000000e+00,  1.280000000e+00,  2.760000000e-01, -5.440000000e-01, -7.200000000e-01, /*EMAG*/  1.414214000e-02,  4.000000000e-02,  2.009975000e-02,  2.440000000e-01,  2.440000000e-01,  2.440000000e-01, /*DIAM*/  8.370799376e-01,  8.785436996e-01,  8.773068217e-01, /*EDIAM*/  6.770194646e-02,  5.897431774e-02,  5.530595562e-02, /*DMEAN*/  8.672032707e-01, /*EDMEAN*/  3.470439080e-02 },
            {189, /* SP "G9.5IV            " idx */ 198, /*MAG*/  4.565000000e+00,  3.710000000e+00,  2.820000000e+00,  2.294000000e+00,  1.925000000e+00,  1.712000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  2.009975000e-02,  2.480000000e-01,  2.480000000e-01,  2.480000000e-01, /*DIAM*/  3.303011956e-01,  3.076828285e-01,  3.367330958e-01, /*EDIAM*/  6.879377396e-02,  5.992704525e-02,  5.620018356e-02, /*DMEAN*/  3.250172647e-01, /*EDMEAN*/  3.526679836e-02 },
            {190, /* SP "F8Ib              " idx */ 152, /*MAG*/  2.903000000e+00,  2.230000000e+00,  1.580000000e+00,  1.369000000e+00,  8.520000000e-01,  8.270000000e-01, /*EMAG*/  1.236932000e-02,  4.000000000e-02,  3.014963000e-02,  3.100000000e-01,  3.100000000e-01,  3.100000000e-01, /*DIAM*/  4.014354469e-01,  4.726140039e-01,  4.600236613e-01, /*EDIAM*/  8.611617019e-02,  7.503337311e-02,  7.037178049e-02, /*DMEAN*/  4.490226145e-01, /*EDMEAN*/  4.413975938e-02 },
            {191, /* SP "K0IV              " idx */ 200, /*MAG*/  4.322000000e+00,  3.410000000e+00,  2.470000000e+00,  1.914000000e+00,  1.504000000e+00,  1.393000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  4.004997000e-02,  2.760000000e-01,  2.760000000e-01,  2.760000000e-01, /*DIAM*/  4.160763341e-01,  3.990293644e-01,  4.032286555e-01, /*EDIAM*/  7.652663894e-02,  6.667022796e-02,  6.252579147e-02, /*DMEAN*/  4.051432728e-01, /*EDMEAN*/  3.922285034e-02 },
            {192, /* SP "K4.5Ib-II+A1.5e   " idx */ 218, /*MAG*/  5.329000000e+00,  3.720000000e+00,  2.090000000e+00,  9.820000000e-01,  2.700000000e-02, -3.800000000e-02, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  1.044031000e-02,  2.320000000e-01,  2.320000000e-01,  2.320000000e-01, /*DIAM*/  7.272835702e-01,  7.894170817e-01,  7.567892095e-01, /*EDIAM*/  6.440840357e-02,  5.610189222e-02,  5.261124328e-02, /*DMEAN*/  7.603344996e-01, /*EDMEAN*/  3.301928505e-02 },
            {193, /* SP "G8IIIFe-0.5       " idx */ 192, /*MAG*/  4.865000000e+00,  3.980000000e+00,  3.040000000e+00,  2.485000000e+00,  2.013000000e+00,  1.901000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  3.006659000e-02,  3.180000000e-01,  3.180000000e-01,  3.180000000e-01, /*DIAM*/  2.872048520e-01,  2.914623074e-01,  2.947604613e-01, /*EDIAM*/  8.814690478e-02,  7.680279732e-02,  7.203108435e-02, /*DMEAN*/  2.916421041e-01, /*EDMEAN*/  4.517303694e-02 },
            {194, /* SP "G2Ib              " idx */ 168, /*MAG*/  3.919000000e+00,  2.950000000e+00,  2.030000000e+00,  1.278000000e+00,  7.400000000e-01,  5.900000000e-01, /*EMAG*/  1.170470000e-02,  4.000000000e-02,  2.039608000e-02,  2.800000000e-01,  2.800000000e-01,  2.800000000e-01, /*DIAM*/  5.020347841e-01,  5.372913779e-01,  5.434858686e-01, /*EDIAM*/  7.776365917e-02,  6.775075270e-02,  6.354062897e-02, /*DMEAN*/  5.304906743e-01, /*EDMEAN*/  3.986767401e-02 },
            {195, /* SP "G9IIIbCa1         " idx */ 196, /*MAG*/  5.435000000e+00,  4.420000000e+00,  3.390000000e+00,  2.567000000e+00,  2.003000000e+00,  1.880000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  3.006659000e-02,  2.320000000e-01,  2.320000000e-01,  2.320000000e-01, /*DIAM*/  3.056052053e-01,  3.160489674e-01,  3.152264859e-01, /*EDIAM*/  6.437929501e-02,  5.607726805e-02,  5.258877264e-02, /*DMEAN*/  3.129923635e-01, /*EDMEAN*/  3.300933032e-02 },
            {196, /* SP "K2.5III           " idx */ 210, /*MAG*/  5.818000000e+00,  4.500000000e+00,  3.250000000e+00,  2.435000000e+00,  1.817000000e+00,  1.668000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  3.001666000e-02,  3.020000000e-01,  3.020000000e-01,  3.020000000e-01, /*DIAM*/  3.729599004e-01,  3.799676729e-01,  3.812844173e-01, /*EDIAM*/  8.371944609e-02,  7.294170343e-02,  6.840857144e-02, /*DMEAN*/  3.786490626e-01, /*EDMEAN*/  4.290175809e-02 },
            {197, /* SP "G8IIIa_CN0.5      " idx */ 192, /*MAG*/  5.040000000e+00,  3.970000000e+00,  2.980000000e+00,  2.030000000e+00,  1.462000000e+00,  1.508000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  3.006659000e-02,  2.380000000e-01,  2.380000000e-01,  2.380000000e-01, /*DIAM*/  4.123744967e-01,  4.239759280e-01,  3.834246962e-01, /*EDIAM*/  6.604732657e-02,  5.753232672e-02,  5.395402398e-02, /*DMEAN*/  4.050063289e-01, /*EDMEAN*/  3.386475802e-02 },
            {198, /* SP "G7III             " idx */ 188, /*MAG*/  5.479000000e+00,  4.540000000e+00,  3.610000000e+00,  3.028000000e+00,  2.659000000e+00,  2.442000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  3.014963000e-02,  3.520000000e-01,  3.520000000e-01,  3.520000000e-01, /*DIAM*/  1.726622895e-01,  1.549356224e-01,  1.831055915e-01, /*EDIAM*/  9.755637163e-02,  8.500674669e-02,  7.972682869e-02, /*DMEAN*/  1.706419883e-01, /*EDMEAN*/  4.999069024e-02 },
            {199, /* SP "G8IVk             " idx */ 192, /*MAG*/  4.794000000e+00,  3.810000000e+00,  2.850000000e+00,  2.101000000e+00,  1.501000000e+00,  1.466000000e+00, /*EMAG*/  1.063015000e-02,  4.000000000e-02,  1.220656000e-02,  2.360000000e-01,  2.360000000e-01,  2.360000000e-01, /*DIAM*/  3.804369963e-01,  4.079681457e-01,  3.887239663e-01, /*EDIAM*/  6.549521169e-02,  5.705081737e-02,  5.350231519e-02, /*DMEAN*/  3.932006422e-01, /*EDMEAN*/  3.358232724e-02 },
            {200, /* SP "G2Ia0e            " idx */ 168, /*MAG*/  5.700000000e+00,  4.510000000e+00,  3.360000000e+00,  2.269000000e+00,  1.915000000e+00,  1.670000000e+00, /*EMAG*/  1.252996000e-02,  4.000000000e-02,  1.166190000e-02,  2.620000000e-01,  2.620000000e-01,  2.620000000e-01, /*DIAM*/  3.475258532e-01,  3.181707520e-01,  3.400990043e-01, /*EDIAM*/  7.279956256e-02,  6.342218031e-02,  5.948016647e-02, /*DMEAN*/  3.344713456e-01, /*EDMEAN*/  3.732819694e-02 },
            {201, /* SP "M6                " idx */ 264, /*MAG*/  1.185200000e+01,  1.031700000e+01,              NaN,  2.923000000e+00,  1.635000000e+00,  1.342000000e+00, /*EMAG*/  2.280000000e-01,  4.000000000e-02,              NaN,  2.520000000e-01,  2.520000000e-01,  2.520000000e-01, /*DIAM*/  6.622171862e-01,  7.166007376e-01,  6.726896391e-01, /*EDIAM*/  7.052670092e-02,  6.145433117e-02,  5.764286028e-02, /*DMEAN*/  6.851121470e-01, /*EDMEAN*/  3.625740936e-02 },
            {202, /* SP "M3III             " idx */ 252, /*MAG*/  9.567000000e+00,  8.045000000e+00,              NaN,  3.562000000e+00,  2.527000000e+00,  2.235000000e+00, /*EMAG*/  4.100000000e-02,  4.000000000e-02,              NaN,  2.600000000e-01,  2.600000000e-01,  2.600000000e-01, /*DIAM*/  3.462281740e-01,  4.020233419e-01,  3.991117492e-01, /*EDIAM*/  7.216291044e-02,  6.286686294e-02,  5.895961736e-02, /*DMEAN*/  3.862708261e-01, /*EDMEAN*/  3.700825419e-02 },
            {203, /* SP "A9Ia              " idx */ 116, /*MAG*/  3.567000000e+00,  3.030000000e+00,  2.420000000e+00,  1.880000000e+00,  1.702000000e+00,  1.533000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  1.118034000e-02,  2.980000000e-01,  2.980000000e-01,  2.980000000e-01, /*DIAM*/  2.904003621e-01,  2.828800175e-01,  2.937601134e-01, /*EDIAM*/  8.316453941e-02,  7.246126250e-02,  6.795946059e-02, /*DMEAN*/  2.891230232e-01, /*EDMEAN*/  4.263623025e-02 },
            {204, /* SP "F5IV-V+DQZ        " idx */ 140, /*MAG*/  8.320000000e-01,  4.000000000e-01, -9.000000000e-02, -4.980000000e-01, -6.660000000e-01, -6.580000000e-01, /*EMAG*/              NaN,  4.000000000e-02,              NaN,  3.220000000e-01,  3.220000000e-01,  3.220000000e-01, /*DIAM*/  7.662067463e-01,  7.585438901e-01,  7.401722852e-01, /*EDIAM*/  8.949010692e-02,  7.797484477e-02,  7.313066944e-02, /*DMEAN*/  7.533370204e-01, /*EDMEAN*/  4.586424020e-02 },
            {205, /* SP "K0III             " idx */ 200, /*MAG*/  2.151000000e+00,  1.160000000e+00,  1.900000000e-01, -3.150000000e-01, -8.450000000e-01, -9.360000000e-01, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  1.166190000e-02,  2.320000000e-01,  2.320000000e-01,  2.320000000e-01, /*DIAM*/  8.602638407e-01,  8.729126400e-01,  8.711045749e-01, /*EDIAM*/  6.437338249e-02,  5.607183299e-02,  5.258345587e-02, /*DMEAN*/  8.688916465e-01, /*EDMEAN*/  3.300440249e-02 },
            {206, /* SP "G9II-III          " idx */ 196, /*MAG*/  4.088000000e+00,  3.110000000e+00,  2.150000000e+00,  1.415000000e+00,  7.580000000e-01,  6.970000000e-01, /*EMAG*/  4.909175000e-02,  4.000000000e-02,  2.022375000e-02,  2.340000000e-01,  2.340000000e-01,  2.340000000e-01, /*DIAM*/  5.238730657e-01,  5.623680372e-01,  5.484892631e-01, /*EDIAM*/  6.493145966e-02,  5.655883361e-02,  5.304053881e-02, /*DMEAN*/  5.468391754e-01, /*EDMEAN*/  3.329179356e-02 },
            {207, /* SP "K7III             " idx */ 228, /*MAG*/  4.690000000e+00,  3.140000000e+00,  1.490000000e+00,  1.200000000e-01, -5.780000000e-01, -6.560000000e-01, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  3.026549000e-02,  2.200000000e-01,  2.200000000e-01,  2.200000000e-01, /*DIAM*/  9.321308283e-01,  9.238533959e-01,  8.943535337e-01, /*EDIAM*/  6.112236429e-02,  5.323589807e-02,  4.992268353e-02, /*DMEAN*/  9.144262704e-01, /*EDMEAN*/  3.133980179e-02 },
            {208, /* SP "G1II              " idx */ 164, /*MAG*/  3.778000000e+00,  2.970000000e+00,  2.160000000e+00,  1.511000000e+00,  1.107000000e+00,  1.223000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  2.022375000e-02,  2.640000000e-01,  2.640000000e-01,  2.640000000e-01, /*DIAM*/  4.334795790e-01,  4.473114702e-01,  3.978940530e-01, /*EDIAM*/  7.337352888e-02,  6.392273612e-02,  5.994970826e-02, /*DMEAN*/  4.242772802e-01, /*EDMEAN*/  3.762192238e-02 },
            {209, /* SP "M0III             " idx */ 240, /*MAG*/  4.663000000e+00,  3.060000000e+00,  1.290000000e+00, -1.080000000e-01, -9.060000000e-01, -1.009000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  3.026549000e-02,  1.920000000e-01,  1.920000000e-01,  1.920000000e-01, /*DIAM*/  9.921748445e-01,  1.013605075e+00,  9.877071488e-01, /*EDIAM*/  5.338319637e-02,  4.648505980e-02,  4.358966794e-02, /*DMEAN*/  9.978198813e-01, /*EDMEAN*/  2.738601165e-02 },
            {210, /* SP "K1III             " idx */ 204, /*MAG*/  4.144000000e+00,  3.000000000e+00,  1.910000000e+00,  1.030000000e+00,  3.760000000e-01,  4.290000000e-01, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  3.014963000e-02,  1.980000000e-01,  1.980000000e-01,  1.980000000e-01, /*DIAM*/  6.364270565e-01,  6.587083088e-01,  6.151157414e-01, /*EDIAM*/  5.498999612e-02,  4.788696345e-02,  4.490457786e-02, /*DMEAN*/  6.357446889e-01, /*EDMEAN*/  2.820317063e-02 },
            {211, /* SP "K3III_Ba0.3       " idx */ 212, /*MAG*/  4.890000000e+00,  3.490000000e+00,  2.120000000e+00,  1.073000000e+00,  1.830000000e-01,  2.820000000e-01, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  3.006659000e-02,  1.900000000e-01,  1.900000000e-01,  1.900000000e-01, /*DIAM*/  6.756067209e-01,  7.349172384e-01,  6.708104683e-01, /*EDIAM*/  5.280259333e-02,  4.597840105e-02,  4.311370537e-02, /*DMEAN*/  6.942056675e-01, /*EDMEAN*/  2.708266758e-02 },
            {212, /* SP "M0III-IIIa_Ca1    " idx */ 240, /*MAG*/  5.433000000e+00,  3.820000000e+00,  2.030000000e+00,  8.950000000e-01, -4.900000000e-02, -1.140000000e-01, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  3.014963000e-02,  2.120000000e-01,  2.120000000e-01,  2.120000000e-01, /*DIAM*/  7.729159127e-01,  8.382042947e-01,  8.051597009e-01, /*EDIAM*/  5.889632519e-02,  5.129435093e-02,  4.810163044e-02, /*DMEAN*/  8.081366565e-01, /*EDMEAN*/  3.020519138e-02 },
            {213, /* SP "M1III             " idx */ 244, /*MAG*/  5.541000000e+00,  4.040000000e+00,  2.250000000e+00,  1.176000000e+00,  3.250000000e-01,  1.570000000e-01, /*EMAG*/  3.224903000e-02,  4.000000000e-02,  2.039608000e-02,  2.600000000e-01,  2.600000000e-01,  2.600000000e-01, /*DIAM*/  7.098010234e-01,  7.611549882e-01,  7.546315769e-01, /*EDIAM*/  7.213259049e-02,  6.283849576e-02,  5.893167301e-02, /*DMEAN*/  7.451461325e-01, /*EDMEAN*/  3.697896716e-02 },
            {214, /* SP "G0IV              " idx */ 160, /*MAG*/  3.260000000e+00,  2.680000000e+00,  2.030000000e+00,  1.793000000e+00,  1.534000000e+00,  1.485000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  2.000000000e-03,  3.060000000e-01,  3.060000000e-01,  3.060000000e-01, /*DIAM*/  3.279389762e-01,  3.302757234e-01,  3.282270133e-01, /*EDIAM*/  8.497600640e-02,  7.403939827e-02,  6.943959830e-02, /*DMEAN*/  3.288589934e-01, /*EDMEAN*/  4.355770368e-02 },
            {215, /* SP "G8IIIaBa0.5       " idx */ 192, /*MAG*/  4.446000000e+00,  3.490000000e+00,  2.600000000e+00,  1.795000000e+00,  1.268000000e+00,  1.223000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  3.006659000e-02,  3.140000000e-01,  3.140000000e-01,  3.140000000e-01, /*DIAM*/  4.405619972e-01,  4.509798195e-01,  4.353006094e-01, /*EDIAM*/  8.704140341e-02,  7.583892181e-02,  7.112692776e-02, /*DMEAN*/  4.420931149e-01, /*EDMEAN*/  4.460723749e-02 },
            {216, /* SP "K2IIIb            " idx */ 208, /*MAG*/  3.797000000e+00,  2.630000000e+00,  1.540000000e+00,  8.310000000e-01,  2.890000000e-01,  1.500000000e-01, /*EMAG*/  1.529706000e-02,  4.000000000e-02,  2.022375000e-02,  2.980000000e-01,  2.980000000e-01,  2.980000000e-01, /*DIAM*/  6.700084502e-01,  6.690814962e-01,  6.732252554e-01, /*EDIAM*/  8.261049809e-02,  7.197481580e-02,  6.750160743e-02, /*DMEAN*/  6.709517350e-01, /*EDMEAN*/  4.233442139e-02 },
            {217, /* SP "G8III-IV          " idx */ 192, /*MAG*/  3.640000000e+00,  2.730000000e+00,  1.890000000e+00,  1.081000000e+00,  5.840000000e-01,  5.790000000e-01, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  3.006659000e-02,  2.300000000e-01,  2.300000000e-01,  2.300000000e-01, /*DIAM*/  5.798298564e-01,  5.846451911e-01,  5.610524361e-01, /*EDIAM*/  6.383901841e-02,  5.560639135e-02,  5.214727684e-02, /*DMEAN*/  5.741144133e-01, /*EDMEAN*/  3.273514642e-02 },
            {218, /* SP "G0IV              " idx */ 160, /*MAG*/  3.460000000e+00,  2.810000000e+00,  2.110000000e+00,  1.495000000e+00,  1.187000000e+00,  1.278000000e+00, /*EMAG*/  1.140175000e-02,  4.000000000e-02,  3.014963000e-02,  2.980000000e-01,  2.980000000e-01,  2.980000000e-01, /*DIAM*/  4.204032632e-01,  4.193496547e-01,  3.784824885e-01, /*EDIAM*/  8.276920049e-02,  7.211528196e-02,  6.763469831e-02, /*DMEAN*/  4.035753893e-01, /*EDMEAN*/  4.242852031e-02 },
            {219, /* SP "G9III             " idx */ 196, /*MAG*/  4.060000000e+00,  3.070000000e+00,  2.130000000e+00,  1.375000000e+00,  8.370000000e-01,  8.830000000e-01, /*EMAG*/  1.811077000e-02,  4.000000000e-02,  3.006659000e-02,  2.620000000e-01,  2.620000000e-01,  2.620000000e-01, /*DIAM*/  5.318730658e-01,  5.416598657e-01,  5.053505763e-01, /*EDIAM*/  7.266401252e-02,  6.330223021e-02,  5.936652649e-02, /*DMEAN*/  5.248339379e-01, /*EDMEAN*/  3.724794250e-02 },
            {220, /* SP "K2Ib-II           " idx */ 208, /*MAG*/  3.957000000e+00,  2.380000000e+00,  9.600000000e-01, -1.170000000e-01, -7.500000000e-01, -8.600000000e-01, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  2.039608000e-02,  2.200000000e-01,  2.200000000e-01,  2.200000000e-01, /*DIAM*/  9.132048824e-01,  9.094239122e-01,  8.951960616e-01, /*EDIAM*/  6.106660810e-02,  5.318733919e-02,  4.987708591e-02, /*DMEAN*/  9.048219535e-01, /*EDMEAN*/  3.131013787e-02 },
            {221, /* SP "K1.5Ib            " idx */ 206, /*MAG*/  4.948000000e+00,  3.390000000e+00,  1.810000000e+00,  1.231000000e+00,  4.250000000e-01,  3.430000000e-01, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  2.000000000e-03,  2.400000000e-01,  2.400000000e-01,  2.400000000e-01, /*DIAM*/  6.142332658e-01,  6.652750490e-01,  6.471536887e-01, /*EDIAM*/  6.658455136e-02,  5.799997126e-02,  5.439203672e-02, /*DMEAN*/  6.447942237e-01, /*EDMEAN*/  3.413361443e-02 },
            {222, /* SP "M2V               " idx */ 248, /*MAG*/  9.650000000e+00,  8.090000000e+00,  5.610000000e+00,  5.252000000e+00,  4.476000000e+00,  4.018000000e+00, /*EMAG*/  4.472136000e-02,  4.000000000e-02,  1.711724000e-01,  2.640000000e-01,  2.640000000e-01,  2.640000000e-01, /*DIAM*/ -1.116228483e-01, -6.951240958e-02, -7.764658977e-03, /*EDIAM*/  7.323870028e-02,  6.380366577e-02,  5.983754999e-02, /*DMEAN*/ -5.628240900e-02, /*EDMEAN*/  3.754969323e-02 },
            {223, /* SP "K3V               " idx */ 212, /*MAG*/  6.708000000e+00,  5.790000000e+00,  4.730000000e+00,  4.152000000e+00,  3.657000000e+00,  3.481000000e+00, /*EMAG*/              NaN,  4.000000000e-02,              NaN,  2.640000000e-01,  2.640000000e-01,  2.640000000e-01, /*DIAM*/ -9.360619378e-06, -8.304562604e-03,  7.387100702e-03, /*EDIAM*/  7.322509479e-02,  6.379086276e-02,  5.982437162e-02, /*DMEAN*/  3.159658038e-05, /*EDMEAN*/  3.753065310e-02 },
            {224, /* SP "K8V               " idx */ 232, /*MAG*/  7.926000000e+00,  6.600000000e+00,  5.310000000e+00,  3.894000000e+00,  3.298000000e+00,  2.962000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  1.077033000e-02,  2.900000000e-01,  2.900000000e-01,  2.900000000e-01, /*DIAM*/  1.556661326e-01,  1.363303429e-01,  1.718391671e-01, /*EDIAM*/  8.044198954e-02,  7.008394412e-02,  6.572795401e-02, /*DMEAN*/  1.553413439e-01, /*EDMEAN*/  4.122613676e-02 },
            {225, /* SP "M2V               " idx */ 248, /*MAG*/  9.650000000e+00,  8.090000000e+00,  5.610000000e+00,  5.252000000e+00,  4.476000000e+00,  4.018000000e+00, /*EMAG*/  4.472136000e-02,  4.000000000e-02,  1.711724000e-01,  2.640000000e-01,  2.640000000e-01,  2.640000000e-01, /*DIAM*/ -1.116228483e-01, -6.951240958e-02, -7.764658977e-03, /*EDIAM*/  7.323870028e-02,  6.380366577e-02,  5.983754999e-02, /*DMEAN*/ -5.628240900e-02, /*EDMEAN*/  3.754969323e-02 },
            {226, /* SP "K3V               " idx */ 212, /*MAG*/  6.708000000e+00,  5.790000000e+00,  4.730000000e+00,  4.152000000e+00,  3.657000000e+00,  3.481000000e+00, /*EMAG*/              NaN,  4.000000000e-02,              NaN,  2.640000000e-01,  2.640000000e-01,  2.640000000e-01, /*DIAM*/ -9.360619378e-06, -8.304562604e-03,  7.387100702e-03, /*EDIAM*/  7.322509479e-02,  6.379086276e-02,  5.982437162e-02, /*DMEAN*/  3.159658038e-05, /*EDMEAN*/  3.753065310e-02 },
            {227, /* SP "M1V               " idx */ 244, /*MAG*/  9.444000000e+00,  7.970000000e+00,  5.890000000e+00,  4.999000000e+00,  4.149000000e+00,  4.039000000e+00, /*EMAG*/  1.421267000e-02,  4.000000000e-02,  1.486607000e-02,  3.000000000e-01,  3.000000000e-01,  3.000000000e-01, /*DIAM*/ -4.658291648e-02,  7.269613520e-04, -2.050712080e-02, /*EDIAM*/  8.318162057e-02,  7.247302224e-02,  6.796951169e-02, /*DMEAN*/ -2.000523582e-02, /*EDMEAN*/  4.263273108e-02 },
            {228, /* SP "K8V               " idx */ 232, /*MAG*/  7.926000000e+00,  6.600000000e+00,  5.310000000e+00,  3.894000000e+00,  3.298000000e+00,  2.962000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  1.077033000e-02,  2.900000000e-01,  2.900000000e-01,  2.900000000e-01, /*DIAM*/  1.556661326e-01,  1.363303429e-01,  1.718391671e-01, /*EDIAM*/  8.044198954e-02,  7.008394412e-02,  6.572795401e-02, /*DMEAN*/  1.553413439e-01, /*EDMEAN*/  4.122613676e-02 },
            {229, /* SP "M2V               " idx */ 248, /*MAG*/  8.992000000e+00,  7.490000000e+00,  5.420000000e+00,  4.320000000e+00,  3.722000000e+00,  3.501000000e+00, /*EMAG*/              NaN,  4.000000000e-02,              NaN,  3.520000000e-01,  3.520000000e-01,  3.520000000e-01, /*DIAM*/  1.002700120e-01,  8.763934373e-02,  9.345432063e-02, /*EDIAM*/  9.755408747e-02,  8.500453389e-02,  7.972490505e-02, /*DMEAN*/  9.323239676e-02, /*EDMEAN*/  4.999235031e-02 },
            {230, /* SP "M2V               " idx */ 248, /*MAG*/  8.992000000e+00,  7.490000000e+00,  5.420000000e+00,  4.320000000e+00,  3.722000000e+00,  3.501000000e+00, /*EMAG*/              NaN,  4.000000000e-02,              NaN,  3.520000000e-01,  3.520000000e-01,  3.520000000e-01, /*DIAM*/  1.002700120e-01,  8.763934373e-02,  9.345432063e-02, /*EDIAM*/  9.755408747e-02,  8.500453389e-02,  7.972490505e-02, /*DMEAN*/  9.323239676e-02, /*EDMEAN*/  4.999235031e-02 },
            {231, /* SP "M2V               " idx */ 248, /*MAG*/  8.833000000e+00,  7.350000000e+00,  5.330000000e+00,  4.338000000e+00,  3.608000000e+00,  3.465000000e+00, /*EMAG*/              NaN,  4.000000000e-02,              NaN,  2.580000000e-01,  2.580000000e-01,  2.580000000e-01, /*DIAM*/  8.453786886e-02,  1.093669705e-01,  9.792147398e-02, /*EDIAM*/  7.158186687e-02,  6.235886325e-02,  5.848221485e-02, /*DMEAN*/  9.836977546e-02, /*EDMEAN*/  3.670211186e-02 },
            {232, /* SP "M2IIIa            " idx */ 248, /*MAG*/  4.900000000e+00,  3.310000000e+00,  6.100000000e-01, -7.270000000e-01, -1.515000000e+00, -1.724000000e+00, /*EMAG*/  1.000000000e-02,  4.000000000e-02,  1.402284000e-01,  1.680000000e-01,  1.680000000e-01,  1.680000000e-01, /*DIAM*/  1.176243242e+00,  1.178635469e+00,  1.165914191e+00, /*EDIAM*/  4.676241670e-02,  4.070958050e-02,  3.817205699e-02, /*DMEAN*/  1.173006249e+00, /*EDMEAN*/  2.401331397e-02 },
            {233, /* SP "K0III             " idx */ 200, /*MAG*/  3.071000000e+00,  2.060000000e+00,  1.050000000e+00,  4.030000000e-01, -1.990000000e-01, -2.740000000e-01, /*EMAG*/  1.431782000e-02,  4.000000000e-02,  2.088061000e-02,  2.340000000e-01,  2.340000000e-01,  2.340000000e-01, /*DIAM*/  7.306388388e-01,  7.541889028e-01,  7.449585876e-01, /*EDIAM*/  6.492559743e-02,  5.655344483e-02,  5.303526733e-02, /*DMEAN*/  7.443981636e-01, /*EDMEAN*/  3.328690755e-02 },
            {234, /* SP "A1V+DA            " idx */ 84, /*MAG*/ -1.431000000e+00, -1.440000000e+00, -1.420000000e+00, -1.391000000e+00, -1.391000000e+00, -1.390000000e+00, /*EMAG*/              NaN,  4.000000000e-02,              NaN,  2.140000000e-01,  2.140000000e-01,  2.140000000e-01, /*DIAM*/  7.985336326e-01,  7.943958963e-01,  7.820068592e-01, /*EDIAM*/  6.071313613e-02,  5.290081805e-02,  4.962104806e-02, /*DMEAN*/  7.906093284e-01, /*EDMEAN*/  3.125711247e-02 },
            {235, /* SP "K0IIICN+1         " idx */ 200, /*MAG*/  3.410000000e+00,  2.240000000e+00,  1.110000000e+00,  3.110000000e-01, -2.100000000e-01, -3.030000000e-01, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  2.000000000e-03,  1.980000000e-01,  1.980000000e-01,  1.980000000e-01, /*DIAM*/  7.699245536e-01,  7.642667240e-01,  7.562505586e-01, /*EDIAM*/  5.498990226e-02,  4.788713752e-02,  4.490495141e-02, /*DMEAN*/  7.625951365e-01, /*EDMEAN*/  2.820488865e-02 },
            {236, /* SP "M0III             " idx */ 240, /*MAG*/  3.646000000e+00,  2.070000000e+00,  3.300000000e-01, -9.570000000e-01, -1.674000000e+00, -1.846000000e+00, /*EMAG*/  1.077033000e-02,  4.000000000e-02,  3.026549000e-02,  2.060000000e-01,  2.060000000e-01,  2.060000000e-01, /*DIAM*/  1.151148061e+00,  1.158048657e+00,  1.151086713e+00, /*EDIAM*/  5.724192880e-02,  4.985124680e-02,  4.674776593e-02, /*DMEAN*/  1.153507164e+00, /*EDMEAN*/  2.935911649e-02 },
            {237, /* SP "K3IIb             " idx */ 212, /*MAG*/  3.615000000e+00,  2.100000000e+00,  7.300000000e-01, -1.800000000e-02, -8.340000000e-01, -8.610000000e-01, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  1.077033000e-02,  2.180000000e-01,  2.180000000e-01,  2.180000000e-01, /*DIAM*/  8.708477952e-01,  9.229328054e-01,  8.929199606e-01, /*EDIAM*/  6.052528622e-02,  5.271509451e-02,  4.943396538e-02, /*DMEAN*/  8.975096584e-01, /*EDMEAN*/  3.103264902e-02 },
            {238, /* SP "K1IIIb            " idx */ 204, /*MAG*/  3.161000000e+00,  2.010000000e+00,  8.800000000e-01, -1.960000000e-01, -7.490000000e-01, -7.830000000e-01, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  4.000000000e-03,  2.180000000e-01,  2.180000000e-01,  2.180000000e-01, /*DIAM*/  8.997484890e-01,  8.892764057e-01,  8.633493217e-01, /*EDIAM*/  6.050865738e-02,  5.270084185e-02,  4.942081708e-02, /*DMEAN*/  8.818272996e-01, /*EDMEAN*/  3.102581645e-02 },
            {239, /* SP "K3-Ib-IIa         " idx */ 212, /*MAG*/  5.460000000e+00,  3.770000000e+00,  2.130000000e+00,  1.074000000e+00,  1.620000000e-01,  1.620000000e-01, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  3.014963000e-02,  2.240000000e-01,  2.240000000e-01,  2.240000000e-01, /*DIAM*/  6.968299355e-01,  7.515320246e-01,  7.053214177e-01, /*EDIAM*/  6.218103301e-02,  5.415926057e-02,  5.078880953e-02, /*DMEAN*/  7.190579920e-01, /*EDMEAN*/  3.187969070e-02 },
            {240, /* SP "K5III             " idx */ 220, /*MAG*/  6.114000000e+00,  4.560000000e+00,  2.890000000e+00,  1.642000000e+00,  8.000000000e-01,  7.670000000e-01, /*EMAG*/  1.581139000e-02,  4.000000000e-02,  3.041381000e-02,  2.720000000e-01,  2.720000000e-01,  2.720000000e-01, /*DIAM*/  6.117100286e-01,  6.400682802e-01,  5.992699080e-01, /*EDIAM*/  7.545683171e-02,  6.573695540e-02,  6.164996989e-02, /*DMEAN*/  6.166162189e-01, /*EDMEAN*/  3.867297093e-02 },
            {241, /* SP "M1.5IIIa          " idx */ 246, /*MAG*/  4.170000000e+00,  2.540000000e+00,  5.700000000e-01, -7.220000000e-01, -1.607000000e+00, -1.680000000e+00, /*EMAG*/  4.019950000e-02,  4.000000000e-02,  2.039608000e-02,  1.820000000e-01,  1.820000000e-01,  1.820000000e-01, /*DIAM*/  1.118112171e+00,  1.167278038e+00,  1.133330996e+00, /*EDIAM*/  5.061308785e-02,  4.406895746e-02,  4.132351196e-02, /*DMEAN*/  1.141078478e+00, /*EDMEAN*/  2.597652495e-02 },
            {242, /* SP "F5Ib              " idx */ 140, /*MAG*/  2.271000000e+00,  1.790000000e+00,  1.160000000e+00,  8.300000000e-01,  4.130000000e-01,  5.200000000e-01, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  4.019950000e-02,  1.840000000e-01,  1.840000000e-01,  1.840000000e-01, /*DIAM*/  5.053674567e-01,  5.555711244e-01,  5.101430847e-01, /*EDIAM*/  5.156484382e-02,  4.490146625e-02,  4.210463273e-02, /*DMEAN*/  5.245849158e-01, /*EDMEAN*/  2.646513615e-02 },
            {243, /* SP "M0III-IIIb        " idx */ 240, /*MAG*/  4.558000000e+00,  2.970000000e+00,  1.190000000e+00,  1.030000000e-01, -7.620000000e-01, -9.530000000e-01, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  2.039608000e-02,  2.260000000e-01,  2.260000000e-01,  2.260000000e-01, /*DIAM*/  9.268623436e-01,  9.751537130e-01,  9.726706523e-01, /*EDIAM*/  6.275788129e-02,  5.466249189e-02,  5.126142927e-02, /*DMEAN*/  9.615405813e-01, /*EDMEAN*/  3.218027675e-02 },
            {244, /* SP "K5III             " idx */ 220, /*MAG*/  2.408000000e+00,  8.700000000e-01, -8.000000000e-01, -2.095000000e+00, -2.775000000e+00, -3.044000000e+00, /*EMAG*/  1.360147000e-02,  4.000000000e-02,  2.282542000e-02,  1.940000000e-01,  1.940000000e-01,  1.940000000e-01, /*DIAM*/  1.362718968e+00,  1.350325100e+00,  1.364649481e+00, /*EDIAM*/  5.393608421e-02,  4.696724666e-02,  4.404144133e-02, /*DMEAN*/  1.359197387e+00, /*EDMEAN*/  2.766262845e-02 },
            {245, /* SP "K3II-III          " idx */ 212, /*MAG*/  4.180000000e+00,  2.690000000e+00,  1.230000000e+00, -3.100000000e-02, -6.760000000e-01, -8.440000000e-01, /*EMAG*/  4.210701000e-02,  4.000000000e-02,  3.014963000e-02,  1.880000000e-01,  1.880000000e-01,  1.880000000e-01, /*DIAM*/  9.197495817e-01,  9.091507040e-01,  9.045768951e-01, /*EDIAM*/  5.225128183e-02,  4.549741484e-02,  4.266243425e-02, /*DMEAN*/  9.101233213e-01, /*EDMEAN*/  2.680074189e-02 },
            {246, /* SP "A9Ia              " idx */ 116, /*MAG*/  3.567000000e+00,  3.030000000e+00,  2.420000000e+00,  1.880000000e+00,  1.702000000e+00,  1.533000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  1.118034000e-02,  2.980000000e-01,  2.980000000e-01,  2.980000000e-01, /*DIAM*/  2.904003621e-01,  2.828800175e-01,  2.937601134e-01, /*EDIAM*/  8.316453941e-02,  7.246126250e-02,  6.795946059e-02, /*DMEAN*/  2.891230232e-01, /*EDMEAN*/  4.263623025e-02 },
            {247, /* SP "M3IIb             " idx */ 252, /*MAG*/  6.001000000e+00,  4.300000000e+00,  1.790000000e+00,  2.450000000e-01, -6.020000000e-01, -8.130000000e-01, /*EMAG*/  1.486607000e-02,  4.000000000e-02,  3.041381000e-02,  2.200000000e-01,  2.200000000e-01,  2.200000000e-01, /*DIAM*/  9.767638977e-01,  1.002416347e+00,  9.903964296e-01, /*EDIAM*/  6.112746771e-02,  5.324300460e-02,  4.993164004e-02, /*DMEAN*/  9.909806525e-01, /*EDMEAN*/  3.136567615e-02 },
            {248, /* SP "M3IIIab           " idx */ 252, /*MAG*/  4.491000000e+00,  2.870000000e+00,  5.700000000e-01, -9.120000000e-01, -1.608000000e+00, -1.862000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  5.035871000e-02,  1.900000000e-01,  1.900000000e-01,  1.900000000e-01, /*DIAM*/  1.187201401e+00,  1.186128412e+00,  1.190184754e+00, /*EDIAM*/  5.286076286e-02,  4.603224089e-02,  4.316701131e-02, /*DMEAN*/  1.188003827e+00, /*EDMEAN*/  2.714127475e-02 },
            {249, /* SP "G8Ib              " idx */ 192, /*MAG*/  4.437000000e+00,  3.060000000e+00,  1.840000000e+00,  8.860000000e-01,  2.320000000e-01,  1.250000000e-01, /*EMAG*/  1.513275000e-02,  4.000000000e-02,  1.019804000e-02,  2.220000000e-01,  2.220000000e-01,  2.220000000e-01, /*DIAM*/  6.591423576e-01,  6.831743755e-01,  6.724538976e-01, /*EDIAM*/  6.163113920e-02,  5.368074515e-02,  5.034077915e-02, /*DMEAN*/  6.726728864e-01, /*EDMEAN*/  3.160585076e-02 },
            {250, /* SP "A1V+DA            " idx */ 84, /*MAG*/ -1.431000000e+00, -1.440000000e+00, -1.420000000e+00, -1.391000000e+00, -1.391000000e+00, -1.390000000e+00, /*EMAG*/              NaN,  4.000000000e-02,              NaN,  2.140000000e-01,  2.140000000e-01,  2.140000000e-01, /*DIAM*/  7.985336326e-01,  7.943958963e-01,  7.820068592e-01, /*EDIAM*/  6.071313613e-02,  5.290081805e-02,  4.962104806e-02, /*DMEAN*/  7.906093284e-01, /*EDMEAN*/  3.125711247e-02 },
            {251, /* SP "F5IV-V+DQZ        " idx */ 140, /*MAG*/  8.320000000e-01,  4.000000000e-01, -9.000000000e-02, -4.980000000e-01, -6.660000000e-01, -6.580000000e-01, /*EMAG*/              NaN,  4.000000000e-02,              NaN,  3.220000000e-01,  3.220000000e-01,  3.220000000e-01, /*DIAM*/  7.662067463e-01,  7.585438901e-01,  7.401722852e-01, /*EDIAM*/  8.949010692e-02,  7.797484477e-02,  7.313066944e-02, /*DMEAN*/  7.533370204e-01, /*EDMEAN*/  4.586424020e-02 },
            {252, /* SP "K0III             " idx */ 200, /*MAG*/  2.151000000e+00,  1.160000000e+00,  1.900000000e-01, -3.150000000e-01, -8.450000000e-01, -9.360000000e-01, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  1.166190000e-02,  2.320000000e-01,  2.320000000e-01,  2.320000000e-01, /*DIAM*/  8.602638407e-01,  8.729126400e-01,  8.711045749e-01, /*EDIAM*/  6.437338249e-02,  5.607183299e-02,  5.258345587e-02, /*DMEAN*/  8.688916465e-01, /*EDMEAN*/  3.300440249e-02 },
            {253, /* SP "K4III_Ba0.5       " idx */ 216, /*MAG*/  5.011000000e+00,  3.530000000e+00,  2.060000000e+00,  1.191000000e+00,  2.670000000e-01,  1.900000000e-01, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  2.022375000e-02,  2.500000000e-01,  2.500000000e-01,  2.500000000e-01, /*DIAM*/  6.520592649e-01,  7.212039170e-01,  6.976681409e-01, /*EDIAM*/  6.937044661e-02,  6.042937434e-02,  5.667093567e-02, /*DMEAN*/  6.938550807e-01, /*EDMEAN*/  3.555807851e-02 },
            {254, /* SP "M3III             " idx */ 252, /*MAG*/  6.282000000e+00,  4.740000000e+00,  2.590000000e+00,  1.381000000e+00,  3.890000000e-01,  3.050000000e-01, /*EMAG*/  1.019804000e-02,  4.000000000e-02,  2.000000000e-03,  2.460000000e-01,  2.460000000e-01,  2.460000000e-01, /*DIAM*/  6.961210364e-01,  7.814902737e-01,  7.489803676e-01, /*EDIAM*/  6.829918057e-02,  5.949755643e-02,  5.579897330e-02, /*DMEAN*/  7.463699858e-01, /*EDMEAN*/  3.503234033e-02 },
            {255, /* SP "K7III             " idx */ 228, /*MAG*/  4.690000000e+00,  3.140000000e+00,  1.490000000e+00,  1.200000000e-01, -5.780000000e-01, -6.560000000e-01, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  3.026549000e-02,  2.200000000e-01,  2.200000000e-01,  2.200000000e-01, /*DIAM*/  9.321308283e-01,  9.238533959e-01,  8.943535337e-01, /*EDIAM*/  6.112236429e-02,  5.323589807e-02,  4.992268353e-02, /*DMEAN*/  9.144262704e-01, /*EDMEAN*/  3.133980179e-02 },
            {256, /* SP "K3IIIa            " idx */ 212, /*MAG*/  3.430000000e+00,  1.990000000e+00,  6.000000000e-01, -2.560000000e-01, -9.900000000e-01, -1.127000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  2.039608000e-02,  2.080000000e-01,  2.080000000e-01,  2.080000000e-01, /*DIAM*/  9.282763675e-01,  9.560300821e-01,  9.502192315e-01, /*EDIAM*/  5.776634198e-02,  5.030857318e-02,  4.717625212e-02, /*DMEAN*/  9.464859747e-01, /*EDMEAN*/  2.962135542e-02 },
            {257, /* SP "G1II              " idx */ 164, /*MAG*/  3.778000000e+00,  2.970000000e+00,  2.160000000e+00,  1.511000000e+00,  1.107000000e+00,  1.223000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  2.022375000e-02,  2.640000000e-01,  2.640000000e-01,  2.640000000e-01, /*DIAM*/  4.334795790e-01,  4.473114702e-01,  3.978940530e-01, /*EDIAM*/  7.337352888e-02,  6.392273612e-02,  5.994970826e-02, /*DMEAN*/  4.242772802e-01, /*EDMEAN*/  3.762192238e-02 },
            {258, /* SP "M0III             " idx */ 240, /*MAG*/  4.663000000e+00,  3.060000000e+00,  1.290000000e+00, -1.080000000e-01, -9.060000000e-01, -1.009000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  3.026549000e-02,  1.920000000e-01,  1.920000000e-01,  1.920000000e-01, /*DIAM*/  9.921748445e-01,  1.013605075e+00,  9.877071488e-01, /*EDIAM*/  5.338319637e-02,  4.648505980e-02,  4.358966794e-02, /*DMEAN*/  9.978198813e-01, /*EDMEAN*/  2.738601165e-02 },
            {259, /* SP "K1III             " idx */ 204, /*MAG*/  4.144000000e+00,  3.000000000e+00,  1.910000000e+00,  1.030000000e+00,  3.760000000e-01,  4.290000000e-01, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  3.014963000e-02,  1.980000000e-01,  1.980000000e-01,  1.980000000e-01, /*DIAM*/  6.364270565e-01,  6.587083088e-01,  6.151157414e-01, /*EDIAM*/  5.498999612e-02,  4.788696345e-02,  4.490457786e-02, /*DMEAN*/  6.357446889e-01, /*EDMEAN*/  2.820317063e-02 },
            {260, /* SP "K3III_Ba0.3       " idx */ 212, /*MAG*/  4.890000000e+00,  3.490000000e+00,  2.120000000e+00,  1.073000000e+00,  1.830000000e-01,  2.820000000e-01, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  3.006659000e-02,  1.900000000e-01,  1.900000000e-01,  1.900000000e-01, /*DIAM*/  6.756067209e-01,  7.349172384e-01,  6.708104683e-01, /*EDIAM*/  5.280259333e-02,  4.597840105e-02,  4.311370537e-02, /*DMEAN*/  6.942056675e-01, /*EDMEAN*/  2.708266758e-02 },
            {261, /* SP "M0III-IIIa_Ca1    " idx */ 240, /*MAG*/  5.433000000e+00,  3.820000000e+00,  2.030000000e+00,  8.950000000e-01, -4.900000000e-02, -1.140000000e-01, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  3.014963000e-02,  2.120000000e-01,  2.120000000e-01,  2.120000000e-01, /*DIAM*/  7.729159127e-01,  8.382042947e-01,  8.051597009e-01, /*EDIAM*/  5.889632519e-02,  5.129435093e-02,  4.810163044e-02, /*DMEAN*/  8.081366565e-01, /*EDMEAN*/  3.020519138e-02 },
            {262, /* SP "M1III             " idx */ 244, /*MAG*/  5.541000000e+00,  4.040000000e+00,  2.250000000e+00,  1.176000000e+00,  3.250000000e-01,  1.570000000e-01, /*EMAG*/  3.224903000e-02,  4.000000000e-02,  2.039608000e-02,  2.600000000e-01,  2.600000000e-01,  2.600000000e-01, /*DIAM*/  7.098010234e-01,  7.611549882e-01,  7.546315769e-01, /*EDIAM*/  7.213259049e-02,  6.283849576e-02,  5.893167301e-02, /*DMEAN*/  7.451461325e-01, /*EDMEAN*/  3.697896716e-02 },
            {263, /* SP "M2III             " idx */ 248, /*MAG*/  4.961000000e+00,  3.390000000e+00,  1.150000000e+00, -1.130000000e-01, -1.010000000e+00, -1.189000000e+00, /*EMAG*/  1.431782000e-02,  4.000000000e-02,  2.022375000e-02,  1.940000000e-01,  1.940000000e-01,  1.940000000e-01, /*DIAM*/  1.012439668e+00,  1.060106284e+00,  1.046957984e+00, /*EDIAM*/  5.392383555e-02,  4.695786357e-02,  4.403423627e-02, /*DMEAN*/  1.042473660e+00, /*EDMEAN*/  2.767245635e-02 },
            {264, /* SP "G8III             " idx */ 192, /*MAG*/  3.784000000e+00,  2.850000000e+00,  2.020000000e+00,  1.248000000e+00,  7.330000000e-01,  6.640000000e-01, /*EMAG*/  1.513275000e-02,  4.000000000e-02,  1.019804000e-02,  2.780000000e-01,  2.780000000e-01,  2.780000000e-01, /*DIAM*/  5.428209273e-01,  5.536490817e-01,  5.449721438e-01, /*EDIAM*/  7.709401116e-02,  6.716547032e-02,  6.299075004e-02, /*DMEAN*/  5.474050435e-01, /*EDMEAN*/  3.951660736e-02 },
            {265, /* SP "G0IV              " idx */ 160, /*MAG*/  3.260000000e+00,  2.680000000e+00,  2.030000000e+00,  1.793000000e+00,  1.534000000e+00,  1.485000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  2.000000000e-03,  3.060000000e-01,  3.060000000e-01,  3.060000000e-01, /*DIAM*/  3.279389762e-01,  3.302757234e-01,  3.282270133e-01, /*EDIAM*/  8.497600640e-02,  7.403939827e-02,  6.943959830e-02, /*DMEAN*/  3.288589934e-01, /*EDMEAN*/  4.355770368e-02 },
            {266, /* SP "K0III             " idx */ 200, /*MAG*/  1.189000000e+00, -5.000000000e-02, -1.270000000e+00, -2.252000000e+00, -2.810000000e+00, -2.911000000e+00, /*EMAG*/  1.081665000e-02,  4.000000000e-02,  2.193171000e-02,  1.700000000e-01,  1.700000000e-01,  1.700000000e-01, /*DIAM*/  1.303487062e+00,  1.297052724e+00,  1.286206771e+00, /*EDIAM*/  4.727046129e-02,  4.115207448e-02,  3.858594226e-02, /*DMEAN*/  1.294467506e+00, /*EDMEAN*/  2.425832506e-02 },
            {267, /* SP "K4III             " idx */ 216, /*MAG*/  3.535000000e+00,  2.070000000e+00,  6.100000000e-01, -4.950000000e-01, -1.225000000e+00, -1.287000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  3.014963000e-02,  2.040000000e-01,  2.040000000e-01,  2.040000000e-01, /*DIAM*/  1.006612842e+00,  1.020923766e+00,  9.935148606e-01, /*EDIAM*/  5.667740937e-02,  4.935865566e-02,  4.628502899e-02, /*DMEAN*/  1.006406897e+00, /*EDMEAN*/  2.906429508e-02 },
            {268, /* SP "M5III             " idx */ 260, /*MAG*/  6.220000000e+00,  4.630000000e+00,  1.780000000e+00,  1.090000000e-01, -7.740000000e-01, -9.570000000e-01, /*EMAG*/  2.140093000e-02,  4.000000000e-02,  1.300000000e-02,  2.020000000e-01,  2.020000000e-01,  2.020000000e-01, /*DIAM*/  1.019114046e+00,  1.062066106e+00,  1.039926310e+00, /*EDIAM*/  5.648623755e-02,  4.920353193e-02,  4.614728763e-02, /*DMEAN*/  1.042130079e+00, /*EDMEAN*/  2.904701291e-02 },
            {269, /* SP "G8IIIaBa0.5       " idx */ 192, /*MAG*/  4.446000000e+00,  3.490000000e+00,  2.600000000e+00,  1.795000000e+00,  1.268000000e+00,  1.223000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  3.006659000e-02,  3.140000000e-01,  3.140000000e-01,  3.140000000e-01, /*DIAM*/  4.405619972e-01,  4.509798195e-01,  4.353006094e-01, /*EDIAM*/  8.704140341e-02,  7.583892181e-02,  7.112692776e-02, /*DMEAN*/  4.420931149e-01, /*EDMEAN*/  4.460723749e-02 },
            {270, /* SP "G8IV              " idx */ 192, /*MAG*/  4.421000000e+00,  3.460000000e+00,  2.500000000e+00,  1.659000000e+00,  9.850000000e-01,  1.223000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  3.006659000e-02,  2.380000000e-01,  2.380000000e-01,  2.380000000e-01, /*DIAM*/  4.759012834e-01,  5.180148399e-01,  4.345122882e-01, /*EDIAM*/  6.604732657e-02,  5.753232672e-02,  5.395402398e-02, /*DMEAN*/  4.741826510e-01, /*EDMEAN*/  3.386475802e-02 },
            {271, /* SP "K2IIIb            " idx */ 208, /*MAG*/  3.797000000e+00,  2.630000000e+00,  1.540000000e+00,  8.310000000e-01,  2.890000000e-01,  1.500000000e-01, /*EMAG*/  1.529706000e-02,  4.000000000e-02,  2.022375000e-02,  2.980000000e-01,  2.980000000e-01,  2.980000000e-01, /*DIAM*/  6.700084502e-01,  6.690814962e-01,  6.732252554e-01, /*EDIAM*/  8.261049809e-02,  7.197481580e-02,  6.750160743e-02, /*DMEAN*/  6.709517350e-01, /*EDMEAN*/  4.233442139e-02 },
            {272, /* SP "M0.5III           " idx */ 242, /*MAG*/  4.314000000e+00,  2.730000000e+00,  9.100000000e-01, -1.310000000e-01, -1.025000000e+00, -1.173000000e+00, /*EMAG*/  1.044031000e-02,  4.000000000e-02,  2.022375000e-02,  2.140000000e-01,  2.140000000e-01,  2.140000000e-01, /*DIAM*/  9.723213017e-01,  1.030798722e+00,  1.018669583e+00, /*EDIAM*/  5.944144683e-02,  5.176994608e-02,  4.854794794e-02, /*DMEAN*/  1.010732825e+00, /*EDMEAN*/  3.048534119e-02 },
            {273, /* SP "G8III-IV          " idx */ 192, /*MAG*/  3.640000000e+00,  2.730000000e+00,  1.890000000e+00,  1.081000000e+00,  5.840000000e-01,  5.790000000e-01, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  3.006659000e-02,  2.300000000e-01,  2.300000000e-01,  2.300000000e-01, /*DIAM*/  5.798298564e-01,  5.846451911e-01,  5.610524361e-01, /*EDIAM*/  6.383901841e-02,  5.560639135e-02,  5.214727684e-02, /*DMEAN*/  5.741144133e-01, /*EDMEAN*/  3.273514642e-02 },
            {274, /* SP "M6III             " idx */ 264, /*MAG*/  6.119000000e+00,  4.830000000e+00,  1.220000000e+00, -9.550000000e-01, -1.850000000e+00, -2.134000000e+00, /*EMAG*/  3.255764000e-02,  4.000000000e-02,  7.337575000e-02,  2.100000000e-01,  2.100000000e-01,  2.100000000e-01, /*DIAM*/  1.314268982e+00,  1.331027984e+00,  1.315045853e+00, /*EDIAM*/  5.905479405e-02,  5.145228141e-02,  4.826164567e-02, /*DMEAN*/  1.320362317e+00, /*EDMEAN*/  3.040880060e-02 },
            {275, /* SP "M1.5Iab-Ib        " idx */ 246, /*MAG*/  2.925000000e+00,  1.060000000e+00, -1.840000000e+00, -2.850000000e+00, -3.725000000e+00, -4.100000000e+00, /*EMAG*/  1.612452000e-02,  4.000000000e-02,  3.104835000e-02,  1.600000000e-01,  1.600000000e-01,  1.600000000e-01, /*DIAM*/  1.593469321e+00,  1.617192442e+00,  1.642031734e+00, /*EDIAM*/  4.455635240e-02,  3.878378685e-02,  3.636472116e-02, /*DMEAN*/  1.620772301e+00, /*EDMEAN*/  2.288212648e-02 },
            {276, /* SP "G0IV              " idx */ 160, /*MAG*/  3.460000000e+00,  2.810000000e+00,  2.110000000e+00,  1.495000000e+00,  1.187000000e+00,  1.278000000e+00, /*EMAG*/  1.140175000e-02,  4.000000000e-02,  3.014963000e-02,  2.980000000e-01,  2.980000000e-01,  2.980000000e-01, /*DIAM*/  4.204032632e-01,  4.193496547e-01,  3.784824885e-01, /*EDIAM*/  8.276920049e-02,  7.211528196e-02,  6.763469831e-02, /*DMEAN*/  4.035753893e-01, /*EDMEAN*/  4.242852031e-02 },
            {277, /* SP "G7IIIFe-1         " idx */ 188, /*MAG*/  4.396000000e+00,  3.480000000e+00,  2.590000000e+00,  1.768000000e+00,  1.366000000e+00,  1.333000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  3.006659000e-02,  3.040000000e-01,  3.040000000e-01,  3.040000000e-01, /*DIAM*/  4.400194363e-01,  4.231457431e-01,  4.061931861e-01, /*EDIAM*/  8.429080363e-02,  7.344092945e-02,  6.887768881e-02, /*DMEAN*/  4.209082094e-01, /*EDMEAN*/  4.320126511e-02 },
            {278, /* SP "K3II              " idx */ 212, /*MAG*/  4.597000000e+00,  3.160000000e+00,  1.850000000e+00,  7.240000000e-01, -1.120000000e-01, -2.300000000e-02, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  2.000000000e-03,  1.780000000e-01,  1.780000000e-01,  1.780000000e-01, /*DIAM*/  7.468656505e-01,  7.924736594e-01,  7.311535348e-01, /*EDIAM*/  4.949548106e-02,  4.309298820e-02,  4.040650933e-02, /*DMEAN*/  7.564394820e-01, /*EDMEAN*/  2.539164088e-02 },
            {279, /* SP "G2Ib-IIa          " idx */ 168, /*MAG*/  3.744000000e+00,  2.790000000e+00,  1.860000000e+00,  1.351000000e+00,  7.960000000e-01,  7.560000000e-01, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  3.006659000e-02,  2.880000000e-01,  2.880000000e-01,  2.880000000e-01, /*DIAM*/  4.695437122e-01,  5.171824281e-01,  5.017194446e-01, /*EDIAM*/  7.997064478e-02,  6.967511551e-02,  6.534577959e-02, /*DMEAN*/  4.986314518e-01, /*EDMEAN*/  4.099681194e-02 },
            {280, /* SP "G5IV              " idx */ 180, /*MAG*/  4.170000000e+00,  3.420000000e+00,  2.710000000e+00,  1.867000000e+00,  1.559000000e+00,  1.511000000e+00, /*EMAG*/  1.513275000e-02,  4.000000000e-02,  2.009975000e-02,  2.180000000e-01,  2.180000000e-01,  2.180000000e-01, /*DIAM*/  3.940099406e-01,  3.673294784e-01,  3.570249291e-01, /*EDIAM*/  6.059366646e-02,  5.277694155e-02,  4.949363025e-02, /*DMEAN*/  3.702608673e-01, /*EDMEAN*/  3.108221452e-02 },
            {281, /* SP "K1IIaCN+...       " idx */ 204, /*MAG*/  5.210000000e+00,  3.860000000e+00,  2.690000000e+00,  1.729000000e+00,  1.056000000e+00,  9.990000000e-01, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  3.006659000e-02,  2.440000000e-01,  2.440000000e-01,  2.440000000e-01, /*DIAM*/  5.089895546e-01,  5.301324314e-01,  5.087361778e-01, /*EDIAM*/  6.768708076e-02,  5.896157813e-02,  5.529420363e-02, /*DMEAN*/  5.161913556e-01, /*EDMEAN*/  3.469828126e-02 },
            {282, /* SP "K5III             " idx */ 220, /*MAG*/  3.761000000e+00,  2.240000000e+00,  7.000000000e-01, -2.450000000e-01, -1.034000000e+00, -1.162000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  3.014963000e-02,  2.120000000e-01,  2.120000000e-01,  2.120000000e-01, /*DIAM*/  9.558618194e-01,  9.868231492e-01,  9.747954610e-01, /*EDIAM*/  5.889795812e-02,  5.129563449e-02,  4.810227371e-02, /*DMEAN*/  9.739959256e-01, /*EDMEAN*/  3.020046911e-02 },
            {283, /* SP "M4II              " idx */ 256, /*MAG*/  5.795000000e+00,  4.220000000e+00,  1.620000000e+00, -1.120000000e-01, -1.039000000e+00, -1.255000000e+00, /*EMAG*/  1.140175000e-02,  4.000000000e-02,  3.080584000e-02,  2.100000000e-01,  2.100000000e-01,  2.100000000e-01, /*DIAM*/  1.060489114e+00,  1.107157599e+00,  1.092623839e+00, /*EDIAM*/  5.847068633e-02,  5.092882050e-02,  4.776253167e-02, /*DMEAN*/  1.089237539e+00, /*EDMEAN*/  3.002701433e-02 },
            {284, /* SP "M5III             " idx */ 260, /*MAG*/  5.477000000e+00,  4.080000000e+00,  9.400000000e-01, -7.380000000e-01, -1.575000000e+00, -1.837000000e+00, /*EMAG*/  1.910497000e-02,  4.000000000e-02,  4.205948000e-02,  2.300000000e-01,  2.300000000e-01,  2.300000000e-01, /*DIAM*/  1.211319406e+00,  1.232618638e+00,  1.224597846e+00, /*EDIAM*/  6.416676385e-02,  5.590138864e-02,  5.242999973e-02, /*DMEAN*/  1.223892985e+00, /*EDMEAN*/  3.296644498e-02 },
            {285, /* SP "G9III             " idx */ 196, /*MAG*/  4.060000000e+00,  3.070000000e+00,  2.130000000e+00,  1.375000000e+00,  8.370000000e-01,  8.830000000e-01, /*EMAG*/  1.811077000e-02,  4.000000000e-02,  3.006659000e-02,  2.620000000e-01,  2.620000000e-01,  2.620000000e-01, /*DIAM*/  5.318730658e-01,  5.416598657e-01,  5.053505763e-01, /*EDIAM*/  7.266401252e-02,  6.330223021e-02,  5.936652649e-02, /*DMEAN*/  5.248339379e-01, /*EDMEAN*/  3.724794250e-02 },
            {286, /* SP "M0III             " idx */ 240, /*MAG*/  5.942000000e+00,  4.440000000e+00,  2.760000000e+00,  1.714000000e+00,  8.780000000e-01,  7.110000000e-01, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  2.009975000e-02,  2.980000000e-01,  2.980000000e-01,  2.980000000e-01, /*DIAM*/  5.938355529e-01,  6.401420349e-01,  6.347728370e-01, /*EDIAM*/  8.263719617e-02,  7.199811096e-02,  6.752379109e-02, /*DMEAN*/  6.259036159e-01, /*EDMEAN*/  4.235182537e-02 },
            {287, /* SP "K3II              " idx */ 212, /*MAG*/  4.227000000e+00,  2.720000000e+00,  1.280000000e+00,  2.760000000e-01, -5.440000000e-01, -7.200000000e-01, /*EMAG*/  1.414214000e-02,  4.000000000e-02,  2.009975000e-02,  2.440000000e-01,  2.440000000e-01,  2.440000000e-01, /*DIAM*/  8.370799376e-01,  8.785436996e-01,  8.773068217e-01, /*EDIAM*/  6.770194646e-02,  5.897431774e-02,  5.530595562e-02, /*DMEAN*/  8.672032707e-01, /*EDMEAN*/  3.470439080e-02 },
            {288, /* SP "F8Ib              " idx */ 152, /*MAG*/  2.903000000e+00,  2.230000000e+00,  1.580000000e+00,  1.369000000e+00,  8.520000000e-01,  8.270000000e-01, /*EMAG*/  1.236932000e-02,  4.000000000e-02,  3.014963000e-02,  3.100000000e-01,  3.100000000e-01,  3.100000000e-01, /*DIAM*/  4.014354469e-01,  4.726140039e-01,  4.600236613e-01, /*EDIAM*/  8.611617019e-02,  7.503337311e-02,  7.037178049e-02, /*DMEAN*/  4.490226145e-01, /*EDMEAN*/  4.413975938e-02 },
            {289, /* SP "K0III-IV          " idx */ 200, /*MAG*/  3.501000000e+00,  2.480000000e+00,  1.480000000e+00,  6.410000000e-01,  1.040000000e-01, -7.000000000e-03, /*EMAG*/  1.513275000e-02,  4.000000000e-02,  2.009975000e-02,  2.180000000e-01,  2.180000000e-01,  2.180000000e-01, /*DIAM*/  6.970138383e-01,  6.984145829e-01,  6.955790249e-01, /*EDIAM*/  6.050857208e-02,  5.270100003e-02,  4.942115649e-02, /*DMEAN*/  6.969337152e-01, /*EDMEAN*/  3.102737818e-02 },
            {290, /* SP "K4.5Ib-II+A1.5e   " idx */ 218, /*MAG*/  5.329000000e+00,  3.720000000e+00,  2.090000000e+00,  9.820000000e-01,  2.700000000e-02, -3.800000000e-02, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  1.044031000e-02,  2.320000000e-01,  2.320000000e-01,  2.320000000e-01, /*DIAM*/  7.272835702e-01,  7.894170817e-01,  7.567892095e-01, /*EDIAM*/  6.440840357e-02,  5.610189222e-02,  5.261124328e-02, /*DMEAN*/  7.603344996e-01, /*EDMEAN*/  3.301928505e-02 },
            {291, /* SP "M1III             " idx */ 244, /*MAG*/  6.138000000e+00,  4.520000000e+00,  2.700000000e+00,  1.701000000e+00,  7.840000000e-01,  6.660000000e-01, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  3.014963000e-02,  2.920000000e-01,  2.920000000e-01,  2.920000000e-01, /*DIAM*/  6.013456646e-01,  6.702211348e-01,  6.520695316e-01, /*EDIAM*/  8.097132504e-02,  7.054578282e-02,  6.616165401e-02, /*DMEAN*/  6.450515345e-01, /*EDMEAN*/  4.150162119e-02 },
            {292, /* SP "K2Ib-II           " idx */ 208, /*MAG*/  3.957000000e+00,  2.380000000e+00,  9.600000000e-01, -1.170000000e-01, -7.500000000e-01, -8.600000000e-01, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  2.039608000e-02,  2.200000000e-01,  2.200000000e-01,  2.200000000e-01, /*DIAM*/  9.132048824e-01,  9.094239122e-01,  8.951960616e-01, /*EDIAM*/  6.106660810e-02,  5.318733919e-02,  4.987708591e-02, /*DMEAN*/  9.048219535e-01, /*EDMEAN*/  3.131013787e-02 },
            {293, /* SP "G2Ib              " idx */ 168, /*MAG*/  3.919000000e+00,  2.950000000e+00,  2.030000000e+00,  1.278000000e+00,  7.400000000e-01,  5.900000000e-01, /*EMAG*/  1.170470000e-02,  4.000000000e-02,  2.039608000e-02,  2.800000000e-01,  2.800000000e-01,  2.800000000e-01, /*DIAM*/  5.020347841e-01,  5.372913779e-01,  5.434858686e-01, /*EDIAM*/  7.776365917e-02,  6.775075270e-02,  6.354062897e-02, /*DMEAN*/  5.304906743e-01, /*EDMEAN*/  3.986767401e-02 },
            {294, /* SP "K1.5Ib            " idx */ 206, /*MAG*/  4.948000000e+00,  3.390000000e+00,  1.810000000e+00,  1.231000000e+00,  4.250000000e-01,  3.430000000e-01, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  2.000000000e-03,  2.400000000e-01,  2.400000000e-01,  2.400000000e-01, /*DIAM*/  6.142332658e-01,  6.652750490e-01,  6.471536887e-01, /*EDIAM*/  6.658455136e-02,  5.799997126e-02,  5.439203672e-02, /*DMEAN*/  6.447942237e-01, /*EDMEAN*/  3.413361443e-02 },
            {295, /* SP "G8IIIa_CN0.5      " idx */ 192, /*MAG*/  5.040000000e+00,  3.970000000e+00,  2.980000000e+00,  2.030000000e+00,  1.462000000e+00,  1.508000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  3.006659000e-02,  2.380000000e-01,  2.380000000e-01,  2.380000000e-01, /*DIAM*/  4.123744967e-01,  4.239759280e-01,  3.834246962e-01, /*EDIAM*/  6.604732657e-02,  5.753232672e-02,  5.395402398e-02, /*DMEAN*/  4.050063289e-01, /*EDMEAN*/  3.386475802e-02 },
            {296, /* SP "M2.5II-IIIe       " idx */ 250, /*MAG*/  4.095000000e+00,  2.440000000e+00,  1.300000000e-01, -1.432000000e+00, -2.129000000e+00, -2.379000000e+00, /*EMAG*/  1.280625000e-02,  4.000000000e-02,  6.053098000e-02,  1.940000000e-01,  1.940000000e-01,  1.940000000e-01, /*DIAM*/  1.301637406e+00,  1.292546689e+00,  1.293600276e+00, /*EDIAM*/  5.393675517e-02,  4.696986094e-02,  4.404598389e-02, /*DMEAN*/  1.295337826e+00, /*EDMEAN*/  2.768414382e-02 },
            {297, /* SP "K1IV(e)+DA1       " idx */ 204, /*MAG*/  6.045000000e+00,  4.760000000e+00,  3.460000000e+00,  2.435000000e+00,  1.830000000e+00,  1.765000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  2.009975000e-02,  2.820000000e-01,  2.820000000e-01,  2.820000000e-01, /*DIAM*/  3.826859813e-01,  3.805293164e-01,  3.590573434e-01, /*EDIAM*/  7.818457761e-02,  6.811573676e-02,  6.388163663e-02, /*DMEAN*/  3.726605113e-01, /*EDMEAN*/  4.007006126e-02 },
            {298, /* SP "F5IV-V+DQZ        " idx */ 140, /*MAG*/  8.320000000e-01,  4.000000000e-01, -9.000000000e-02, -4.980000000e-01, -6.660000000e-01, -6.580000000e-01, /*EMAG*/              NaN,  4.000000000e-02,              NaN,  3.220000000e-01,  3.220000000e-01,  3.220000000e-01, /*DIAM*/  7.662067463e-01,  7.585438901e-01,  7.401722852e-01, /*EDIAM*/  8.949010692e-02,  7.797484477e-02,  7.313066944e-02, /*DMEAN*/  7.533370204e-01, /*EDMEAN*/  4.586424020e-02 },
            {299, /* SP "M2III             " idx */ 248, /*MAG*/  6.001000000e+00,  4.370000000e+00,  2.020000000e+00,  6.230000000e-01, -2.380000000e-01, -3.990000000e-01, /*EMAG*/  1.208305000e-02,  4.000000000e-02,  2.061553000e-02,  2.180000000e-01,  2.180000000e-01,  2.180000000e-01, /*DIAM*/  8.839753808e-01,  9.142852704e-01,  8.939506829e-01, /*EDIAM*/  6.054174579e-02,  5.273058679e-02,  4.944991287e-02, /*DMEAN*/  8.983633917e-01, /*EDMEAN*/  3.105567272e-02 },
            {300, /* SP "K4V               " idx */ 216, /*MAG*/  6.744000000e+00,  5.720000000e+00,  4.490000000e+00,  3.663000000e+00,  3.085000000e+00,  3.048000000e+00, /*EMAG*/  1.581139000e-02,  4.000000000e-02,  8.015610000e-02,  2.580000000e-01,  2.580000000e-01,  2.580000000e-01, /*DIAM*/  1.360056857e-01,  1.317019627e-01,  1.085148475e-01, /*EDIAM*/  7.157934929e-02,  6.235568050e-02,  5.847800984e-02, /*DMEAN*/  1.237200403e-01, /*EDMEAN*/  3.668840443e-02 },
            {301, /* SP "K5V               " idx */ 220, /*MAG*/  5.746000000e+00,  4.690000000e+00,  3.540000000e+00,  2.894000000e+00,  2.349000000e+00,  2.237000000e+00, /*EMAG*/  1.612452000e-02,  4.000000000e-02,  2.009975000e-02,  2.920000000e-01,  2.920000000e-01,  2.920000000e-01, /*DIAM*/  2.751564521e-01,  2.717414265e-01,  2.700582243e-01, /*EDIAM*/  8.098084942e-02,  7.055376988e-02,  6.616850551e-02, /*DMEAN*/  2.719748545e-01, /*EDMEAN*/  4.150010916e-02 },
            {302, /* SP "K0.5V             " idx */ 202, /*MAG*/  5.250000000e+00,  4.430000000e+00,  3.540000000e+00,  3.013000000e+00,  2.594000000e+00,  2.498000000e+00, /*EMAG*/              NaN,  4.000000000e-02,              NaN,  2.380000000e-01,  2.380000000e-01,  2.380000000e-01, /*DIAM*/  1.938103819e-01,  1.803502304e-01,  1.822374722e-01, /*EDIAM*/  6.602941767e-02,  5.751600709e-02,  5.393817278e-02, /*DMEAN*/  1.846150194e-01, /*EDMEAN*/  3.385083142e-02 },
            {303, /* SP "G8.5V             " idx */ 194, /*MAG*/  4.217000000e+00,  3.490000000e+00,  2.670000000e+00,  2.149000000e+00,  1.800000000e+00,  1.794000000e+00, /*EMAG*/              NaN,  4.000000000e-02,              NaN,  3.100000000e-01,  3.100000000e-01,  3.100000000e-01, /*DIAM*/  3.462328897e-01,  3.246206318e-01,  3.081477696e-01, /*EDIAM*/  8.593101247e-02,  7.487065041e-02,  7.021855325e-02, /*DMEAN*/  3.238132688e-01, /*EDMEAN*/  4.403811166e-02 },
            {304, /* SP "A4V               " idx */ 96, /*MAG*/  1.315000000e+00,  1.170000000e+00,  1.010000000e+00,  1.037000000e+00,  9.370000000e-01,  9.450000000e-01, /*EMAG*/  1.303840000e-02,  4.000000000e-02,  1.220656000e-02,  2.860000000e-01,  2.860000000e-01,  2.860000000e-01, /*DIAM*/  3.550153135e-01,  3.665458529e-01,  3.492304374e-01, /*EDIAM*/  8.021585441e-02,  6.989539843e-02,  6.555588009e-02, /*DMEAN*/  3.567248581e-01, /*EDMEAN*/  4.115982201e-02 },
            {305, /* SP "G0IV              " idx */ 160, /*MAG*/  3.260000000e+00,  2.680000000e+00,  2.030000000e+00,  1.793000000e+00,  1.534000000e+00,  1.485000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  2.000000000e-03,  3.060000000e-01,  3.060000000e-01,  3.060000000e-01, /*DIAM*/  3.279389762e-01,  3.302757234e-01,  3.282270133e-01, /*EDIAM*/  8.497600640e-02,  7.403939827e-02,  6.943959830e-02, /*DMEAN*/  3.288589934e-01, /*EDMEAN*/  4.355770368e-02 },
            {306, /* SP "K5V               " idx */ 220, /*MAG*/  6.269000000e+00,  5.200000000e+00,  4.070000000e+00,  3.114000000e+00,  2.540000000e+00,  2.248000000e+00, /*EMAG*/              NaN,  4.000000000e-02,              NaN,  3.180000000e-01,  3.180000000e-01,  3.180000000e-01, /*DIAM*/  2.534243090e-01,  2.466986245e-01,  2.809706332e-01, /*EDIAM*/  8.816426604e-02,  7.681714643e-02,  7.204391777e-02, /*DMEAN*/  2.619195084e-01, /*EDMEAN*/  4.517687366e-02 },
            {307, /* SP "K7V               " idx */ 228, /*MAG*/  7.359000000e+00,  6.050000000e+00,  4.780000000e+00,  3.546000000e+00,  2.895000000e+00,  2.544000000e+00, /*EMAG*/  1.500000000e-02,  4.000000000e-02,  1.345362000e-02,  3.280000000e-01,  3.280000000e-01,  3.280000000e-01, /*DIAM*/  2.073093889e-01,  2.060323735e-01,  2.467330861e-01, /*EDIAM*/  9.093981077e-02,  7.923713240e-02,  7.431406887e-02, /*DMEAN*/  2.223497845e-01, /*EDMEAN*/  4.659857296e-02 },
            {308, /* SP "K0V               " idx */ 200, /*MAG*/  6.730000000e+00,  5.880000000e+00,  5.050000000e+00,  4.549000000e+00,  4.064000000e+00,  3.999000000e+00, /*EMAG*/              NaN,  4.000000000e-02,              NaN,  2.400000000e-01,  2.400000000e-01,  2.400000000e-01, /*DIAM*/ -1.235933168e-01, -1.166827055e-01, -1.215450749e-01, /*EDIAM*/  6.658237739e-02,  5.799836798e-02,  5.439077587e-02, /*DMEAN*/ -1.204020809e-01, /*EDMEAN*/  3.413452229e-02 },
            {309, /* SP "F9V               " idx */ 156, /*MAG*/  4.636000000e+00,  4.100000000e+00,  3.520000000e+00,  3.175000000e+00,  2.957000000e+00,  2.859000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  3.014963000e-02,  2.740000000e-01,  2.740000000e-01,  2.740000000e-01, /*DIAM*/  4.961846270e-02,  4.366546086e-02,  5.196899435e-02, /*EDIAM*/  7.617195387e-02,  6.636284987e-02,  6.223852306e-02, /*DMEAN*/  4.848605463e-02, /*EDMEAN*/  3.905251808e-02 },
            {310, /* SP "G3Va              " idx */ 172, /*MAG*/  6.990000000e+00,  6.270000000e+00,  5.500000000e+00,  5.386000000e+00,  4.678000000e+00,  4.601000000e+00, /*EMAG*/  1.029563000e-02,  4.000000000e-02,  1.118034000e-02,  2.680000000e-01,  2.680000000e-01,  2.680000000e-01, /*DIAM*/ -3.741032899e-01, -2.733026249e-01, -2.738434236e-01, /*EDIAM*/  7.443115543e-02,  6.484479482e-02,  6.081462984e-02, /*DMEAN*/ -2.999114277e-01, /*EDMEAN*/  3.816223627e-02 },
            {311, /* SP "G9VCN+1           " idx */ 196, /*MAG*/  7.237000000e+00,  6.420000000e+00,  5.570000000e+00,  5.023000000e+00,  4.637000000e+00,  4.491000000e+00, /*EMAG*/  1.581139000e-02,  4.000000000e-02,  2.061553000e-02,  2.600000000e-02,  2.600000000e-02,  2.600000000e-02, /*DIAM*/ -2.206090883e-01, -2.369004572e-01, -2.230289966e-01, /*EDIAM*/  8.544460884e-03,  7.175523906e-03,  6.657660720e-03, /*DMEAN*/ -2.273431776e-01, /*EDMEAN*/  4.640615332e-03 },
            {312, /* SP "K1II-III          " idx */ 204, /*MAG*/  9.035000000e+00,  7.570000000e+00,  6.100000000e+00,  5.190000000e+00,  4.323000000e+00,  3.997000000e+00, /*EMAG*/  1.835756000e-02,  4.000000000e-02,  2.193171000e-02,  2.880000000e-01,  2.880000000e-01,  2.880000000e-01, /*DIAM*/ -1.640908126e-01, -1.049959827e-01, -7.215434183e-02, /*EDIAM*/  7.984256699e-02,  6.956144699e-02,  6.523781458e-02, /*DMEAN*/ -1.075754445e-01, /*EDMEAN*/  4.091859363e-02 },
            {313, /* SP "F8.5V             " idx */ 154, /*MAG*/  5.645000000e+00,  5.070000000e+00,  4.440000000e+00,  4.174000000e+00,  3.768000000e+00,  3.748000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  2.022375000e-02,  2.800000000e-01,  2.800000000e-01,  2.800000000e-01, /*DIAM*/ -1.546877575e-01, -1.128640622e-01, -1.250096700e-01, /*EDIAM*/  7.783567355e-02,  6.781350592e-02,  6.359926244e-02, /*DMEAN*/ -1.285883731e-01, /*EDMEAN*/  3.990327170e-02 },
            {314, /* SP "G8III/IV          " idx */ 192, /*MAG*/  6.723000000e+00,  5.950000000e+00,  5.220000000e+00,  4.905000000e+00,  4.384000000e+00,  4.211000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  4.031129000e-02,  2.280000000e-01,  2.280000000e-01,  2.280000000e-01, /*DIAM*/ -2.313487271e-01, -1.992769995e-01, -1.761738523e-01, /*EDIAM*/  6.328700664e-02,  5.512495151e-02,  5.169562801e-02, /*DMEAN*/ -1.985920958e-01, /*EDMEAN*/  3.245279160e-02 },
            {315, /* SP "K2III             " idx */ 208, /*MAG*/  6.576000000e+00,  5.450000000e+00,  4.430000000e+00,  3.630000000e+00,  3.065000000e+00,  2.922000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  4.011234000e-02,  3.000000000e-01,  3.000000000e-01,  3.000000000e-01, /*DIAM*/  1.118209419e-01,  1.156962739e-01,  1.200865610e-01, /*EDIAM*/  8.316321512e-02,  7.245674827e-02,  6.795368965e-02, /*DMEAN*/  1.164053470e-01, /*EDMEAN*/  4.261731644e-02 },
            {316, /* SP "G8V               " idx */ 192, /*MAG*/  6.829000000e+00,  5.960000000e+00,  5.140000000e+00,  4.768000000e+00,  4.265000000e+00,  4.015000000e+00, /*EMAG*/  1.300000000e-02,  4.000000000e-02,  7.017834000e-02,  2.440000000e-01,  2.440000000e-01,  2.440000000e-01, /*DIAM*/ -1.926612266e-01, -1.701563765e-01, -1.315607130e-01, /*EDIAM*/  6.770381394e-02,  5.897695097e-02,  5.530923332e-02, /*DMEAN*/ -1.608840783e-01, /*EDMEAN*/  3.471215552e-02 },
            {317, /* SP "G9III             " idx */ 196, /*MAG*/  6.809000000e+00,  5.780000000e+00,  4.780000000e+00,  4.112000000e+00,  3.547000000e+00,  3.274000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  3.000000000e-03,  3.040000000e-01,  3.040000000e-01,  3.040000000e-01, /*DIAM*/ -1.760015670e-02, -3.401423770e-04,  3.553305105e-02, /*EDIAM*/  8.426885236e-02,  7.342127808e-02,  6.885887953e-02, /*DMEAN*/  9.226876827e-03, /*EDMEAN*/  4.318661728e-02 },
            {318, /* SP "G4Va              " idx */ 176, /*MAG*/  5.684000000e+00,  4.970000000e+00,  4.200000000e+00,  3.798000000e+00,  3.457000000e+00,  3.500000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  3.000000000e-03,  2.940000000e-01,  2.940000000e-01,  2.940000000e-01, /*DIAM*/ -2.807342848e-02, -2.962063127e-02, -5.568084310e-02, /*EDIAM*/  8.158430132e-02,  7.108182187e-02,  6.666521089e-02, /*DMEAN*/ -3.945051607e-02, /*EDMEAN*/  4.182093279e-02 },
            {319, /* SP "F6IV+M2           " idx */ 144, /*MAG*/  5.008000000e+00,  4.500000000e+00,  3.990000000e+00,  3.617000000e+00,  3.546000000e+00,  3.507000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  3.006659000e-02,  3.480000000e-01,  3.480000000e-01,  3.480000000e-01, /*DIAM*/ -5.440508751e-02, -8.689877654e-02, -9.190350780e-02, /*EDIAM*/  9.663683655e-02,  8.420573042e-02,  7.897551642e-02, /*DMEAN*/ -8.034934688e-02, /*EDMEAN*/  4.952243698e-02 },
            {320, /* SP "G0V               " idx */ 160, /*MAG*/  6.002000000e+00,  5.390000000e+00,  4.710000000e+00,  4.088000000e+00,  3.989000000e+00,  3.857000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  2.000000000e-03,  2.400000000e-01,  2.400000000e-01,  2.400000000e-01, /*DIAM*/ -9.919495878e-02, -1.502067740e-01, -1.372912418e-01, /*EDIAM*/  6.678466752e-02,  5.817701957e-02,  5.455971435e-02, /*DMEAN*/ -1.317791862e-01, /*EDMEAN*/  3.425150904e-02 },
            {321, /* SP "K0V+M4V           " idx */ 200, /*MAG*/  8.602000000e+00,  7.670000000e+00,  6.740000000e+00,  6.073000000e+00,  5.587000000e+00,  5.541000000e+00, /*EMAG*/  1.000000000e-02,  4.000000000e-02,  1.166190000e-02,  3.200000000e-02,  3.200000000e-02,  3.200000000e-02, /*DIAM*/ -4.079683211e-01, -4.102702585e-01, -4.234282911e-01, /*EDIAM*/  9.945197522e-03,  8.433974894e-03,  7.847507525e-03, /*DMEAN*/ -4.151814245e-01, /*EDMEAN*/  5.314116621e-03 },
            {322, /* SP "G7IV-V            " idx */ 188, /*MAG*/  6.479000000e+00,  5.730000000e+00,  4.900000000e+00,  4.554000000e+00,  4.239000000e+00,  4.076000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  7.006426000e-02,  2.520000000e-01,  2.520000000e-01,  2.520000000e-01, /*DIAM*/ -1.583377155e-01, -1.771499857e-01, -1.553615668e-01, /*EDIAM*/  6.992840423e-02,  6.091727531e-02,  5.712967486e-02, /*DMEAN*/ -1.636649425e-01, /*EDMEAN*/  3.585235542e-02 },
            {323, /* SP "G2.5IVa           " idx */ 170, /*MAG*/  6.116000000e+00,  5.450000000e+00,  4.760000000e+00,  4.655000000e+00,  4.234000000e+00,  3.911000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  1.044031000e-02,  2.960000000e-01,  2.960000000e-01,  2.960000000e-01, /*DIAM*/ -2.377667551e-01, -2.012882101e-01, -1.407930142e-01, /*EDIAM*/  8.216773291e-02,  7.159075927e-02,  6.714272889e-02, /*DMEAN*/ -1.870842325e-01, /*EDMEAN*/  4.212073318e-02 },
            {324, /* SP "K1V_Fe-2          " idx */ 204, /*MAG*/  5.874000000e+00,  5.170000000e+00,  4.340000000e+00,  4.031000000e+00,  3.597000000e+00,  3.505000000e+00, /*EMAG*/              NaN,  4.000000000e-02,              NaN,  3.440000000e-01,  3.440000000e-01,  3.440000000e-01, /*DIAM*/ -2.758188195e-02, -2.884033953e-02, -2.389156739e-02, /*EDIAM*/  9.532170949e-02,  8.305769985e-02,  7.789797238e-02, /*DMEAN*/ -2.656735504e-02, /*EDMEAN*/  4.884156149e-02 },
            {325, /* SP "K0V               " idx */ 200, /*MAG*/  6.434000000e+00,  5.630000000e+00,  4.820000000e+00,  4.596000000e+00,  4.133000000e+00,  4.014000000e+00, /*EMAG*/  1.019804000e-02,  4.000000000e-02,  2.000000000e-03,  2.620000000e-01,  2.620000000e-01,  2.620000000e-01, /*DIAM*/ -1.557986744e-01, -1.436399043e-01, -1.315085787e-01, /*EDIAM*/  7.265877416e-02,  6.329741553e-02,  5.936181678e-02, /*DMEAN*/ -1.420581576e-01, /*EDMEAN*/  3.724357549e-02 },
            {326, /* SP "G9V               " idx */ 196, /*MAG*/  5.456000000e+00,  4.670000000e+00,  3.820000000e+00,  3.423000000e+00,  3.039000000e+00,  2.900000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  2.118962000e-02,  3.240000000e-01,  3.240000000e-01,  3.240000000e-01, /*DIAM*/  8.787305915e-02,  7.643028678e-02,  9.099290589e-02, /*EDIAM*/  8.979679962e-02,  7.824107750e-02,  7.338007549e-02, /*DMEAN*/  8.514699021e-02, /*EDMEAN*/  4.601591621e-02 },
            {327, /* SP "K0III-IV          " idx */ 200, /*MAG*/  6.794000000e+00,  5.860000000e+00,  4.930000000e+00,  4.272000000e+00,  3.825000000e+00,  3.701000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  5.000000000e-03,  3.000000000e-01,  3.000000000e-01,  3.000000000e-01, /*DIAM*/ -4.845938713e-02, -5.985001982e-02, -5.463996441e-02, /*EDIAM*/  8.315880993e-02,  7.245319341e-02,  6.795059541e-02, /*DMEAN*/ -5.482002530e-02, /*EDMEAN*/  4.261703297e-02 },
            {328, /* SP "G6III             " idx */ 184, /*MAG*/  6.444000000e+00,  5.510000000e+00,  4.580000000e+00,  4.032000000e+00,  3.440000000e+00,  3.366000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  3.000000000e-03,  2.360000000e-01,  2.360000000e-01,  2.360000000e-01, /*DIAM*/ -3.785391445e-02,  3.008927066e-03, -4.240788543e-03, /*EDIAM*/  6.553271039e-02,  5.708420273e-02,  5.353411320e-02, /*DMEAN*/ -1.053528028e-02, /*EDMEAN*/  3.360596615e-02 },
            {329, /* SP "K0III             " idx */ 200, /*MAG*/  6.881000000e+00,  5.930000000e+00,  4.990000000e+00,  4.508000000e+00,  3.995000000e+00,  3.984000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  3.014963000e-02,  3.200000000e-01,  3.200000000e-01,  3.200000000e-01, /*DIAM*/ -1.084058166e-01, -9.797453401e-02, -1.168370456e-01, /*EDIAM*/  8.868684096e-02,  7.727312272e-02,  7.247193507e-02, /*DMEAN*/ -1.081148199e-01, /*EDMEAN*/  4.544642010e-02 },
            {330, /* SP "G8III             " idx */ 192, /*MAG*/  6.249000000e+00,  5.220000000e+00,  4.220000000e+00,  3.019000000e+00,  2.608000000e+00,  2.331000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  3.000000000e-03,  2.400000000e-01,  2.400000000e-01,  2.400000000e-01, /*DIAM*/  2.346155655e-01,  1.990654189e-01,  2.300451319e-01, /*EDIAM*/  6.659946563e-02,  5.801385237e-02,  5.440574682e-02, /*DMEAN*/  2.205430904e-01, /*EDMEAN*/  3.414720662e-02 },
            {331, /* SP "K1III             " idx */ 204, /*MAG*/  7.531000000e+00,  6.430000000e+00,  5.370000000e+00,  4.531000000e+00,  3.992000000e+00,  3.911000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  1.077033000e-02,  2.220000000e-01,  2.220000000e-01,  2.220000000e-01, /*DIAM*/ -6.922473971e-02, -7.216329738e-02, -8.265069235e-02, /*EDIAM*/  6.161275679e-02,  5.366385515e-02,  5.032426561e-02, /*DMEAN*/ -7.551561882e-02, /*EDMEAN*/  3.159061229e-02 },
            {332, /* SP "K2III             " idx */ 208, /*MAG*/  7.053000000e+00,  5.930000000e+00,  4.840000000e+00,  4.303000000e+00,  3.726000000e+00,  3.548000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  1.077033000e-02,  2.700000000e-01,  2.700000000e-01,  2.700000000e-01, /*DIAM*/ -3.759870322e-02, -2.396909781e-02, -8.949937229e-03, /*EDIAM*/  7.487378518e-02,  6.522862718e-02,  6.117319001e-02, /*DMEAN*/ -2.163875523e-02, /*EDMEAN*/  3.837483788e-02 },
            {333, /* SP "K1-IIIbCN-0.5     " idx */ 204, /*MAG*/  5.769000000e+00,  4.590000000e+00,  3.410000000e+00,  2.619000000e+00,  2.021000000e+00,  1.734000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  3.006659000e-02,  2.900000000e-01,  2.900000000e-01,  2.900000000e-01, /*DIAM*/  3.187038375e-01,  3.274398214e-01,  3.616047887e-01, /*EDIAM*/  8.039525512e-02,  7.004336656e-02,  6.568988754e-02, /*DMEAN*/  3.385701503e-01, /*EDMEAN*/  4.120145596e-02 },
            {334, /* SP "K3III             " idx */ 212, /*MAG*/  6.546000000e+00,  5.270000000e+00,  4.030000000e+00,  3.392000000e+00,  2.884000000e+00,  2.632000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  2.000000000e-03,  2.820000000e-01,  2.820000000e-01,  2.820000000e-01, /*DIAM*/  1.704192133e-01,  1.567304593e-01,  1.858323588e-01, /*EDIAM*/  7.819744772e-02,  6.812676457e-02,  6.389180911e-02, /*DMEAN*/  1.717461693e-01, /*EDMEAN*/  4.007535187e-02 },
            {335, /* SP "K1III             " idx */ 204, /*MAG*/  6.931000000e+00,  5.830000000e+00,  4.770000000e+00,  3.835000000e+00,  3.344000000e+00,  3.103000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  2.000000000e-03,  2.980000000e-01,  2.980000000e-01,  2.980000000e-01, /*DIAM*/  7.734669105e-02,  5.941647112e-02,  8.441500357e-02, /*EDIAM*/  8.260612591e-02,  7.197112132e-02,  6.749824395e-02, /*DMEAN*/  7.393122020e-02, /*EDMEAN*/  4.233299141e-02 },
            {336, /* SP "K2III             " idx */ 208, /*MAG*/  6.899000000e+00,  5.720000000e+00,  4.580000000e+00,  3.633000000e+00,  3.005000000e+00,  2.780000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  1.044031000e-02,  2.780000000e-01,  2.780000000e-01,  2.780000000e-01, /*DIAM*/  1.317227279e-01,  1.413071692e-01,  1.593128390e-01, /*EDIAM*/  7.708400219e-02,  6.715593168e-02,  6.298115849e-02, /*DMEAN*/  1.458693669e-01, /*EDMEAN*/  3.950595284e-02 },
            {337, /* SP "K0III             " idx */ 200, /*MAG*/  7.030000000e+00,  6.000000000e+00,  5.000000000e+00,  4.396000000e+00,  3.821000000e+00,  3.659000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  1.077033000e-02,  3.120000000e-01,  3.120000000e-01,  3.120000000e-01, /*DIAM*/ -7.203081605e-02, -5.311072011e-02, -4.145748247e-02, /*EDIAM*/  8.647551059e-02,  7.534507460e-02,  7.066333456e-02, /*DMEAN*/ -5.349096565e-02, /*EDMEAN*/  4.431457819e-02 },
            {338, /* SP "K2III             " idx */ 208, /*MAG*/  6.719000000e+00,  5.500000000e+00,  4.320000000e+00,  3.388000000e+00,  2.795000000e+00,  2.527000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  1.044031000e-02,  2.580000000e-01,  2.580000000e-01,  2.580000000e-01, /*DIAM*/  1.826423715e-01,  1.828947185e-01,  2.107799931e-01, /*EDIAM*/  7.155893136e-02,  6.233797864e-02,  5.846149845e-02, /*DMEAN*/  1.937830460e-01, /*EDMEAN*/  3.667850233e-02 },
            {339, /* SP "K4III             " idx */ 216, /*MAG*/  6.389000000e+00,  5.020000000e+00,  3.680000000e+00,  2.876000000e+00,  2.091000000e+00,  1.939000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  3.000000000e-03,  2.700000000e-01,  2.700000000e-01,  2.700000000e-01, /*DIAM*/  3.000860453e-01,  3.426280359e-01,  3.410622962e-01, /*EDIAM*/  7.489329940e-02,  6.524554478e-02,  6.118896967e-02, /*DMEAN*/  3.308723379e-01, /*EDMEAN*/  3.838430237e-02 },
            {340, /* SP "K4III             " idx */ 216, /*MAG*/  7.035000000e+00,  5.720000000e+00,  4.440000000e+00,  3.689000000e+00,  3.002000000e+00,  2.782000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  1.044031000e-02,  3.020000000e-01,  3.020000000e-01,  3.020000000e-01, /*DIAM*/  1.288092571e-01,  1.517253093e-01,  1.687046294e-01, /*EDIAM*/  8.373339700e-02,  7.295381782e-02,  6.841988976e-02, /*DMEAN*/  1.523906279e-01, /*EDMEAN*/  4.290868590e-02 },
            {341, /* SP "K5III             " idx */ 220, /*MAG*/  7.341000000e+00,  5.900000000e+00,  4.460000000e+00,  2.473000000e+00,  1.855000000e+00,  1.587000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  1.077033000e-02,  2.620000000e-01,  2.620000000e-01,  2.620000000e-01, /*DIAM*/  4.845939553e-01,  4.408231411e-01,  4.489341393e-01, /*EDIAM*/  7.269549016e-02,  6.332900994e-02,  5.939110249e-02, /*DMEAN*/  4.554706893e-01, /*EDMEAN*/  3.725985334e-02 },
            {342, /* SP "K4III:            " idx */ 216, /*MAG*/  7.154000000e+00,  5.970000000e+00,  4.830000000e+00,  4.180000000e+00,  3.593000000e+00,  3.406000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  1.077033000e-02,  3.240000000e-01,  3.240000000e-01,  3.240000000e-01, /*DIAM*/  1.210389818e-02,  1.946071590e-02,  3.407689015e-02, /*EDIAM*/  8.981291242e-02,  7.825457888e-02,  7.339228609e-02, /*DMEAN*/  2.327294302e-02, /*EDMEAN*/  4.602053767e-02 },
            {343, /* SP "K2III             " idx */ 208, /*MAG*/  7.561000000e+00,  6.270000000e+00,  5.020000000e+00,  4.125000000e+00,  3.461000000e+00,  3.177000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  1.118034000e-02,  3.840000000e-01,  3.840000000e-01,  3.840000000e-01, /*DIAM*/  3.777629790e-02,  5.398421074e-02,  8.393327583e-02, /*EDIAM*/  1.063853536e-01,  9.270315224e-02,  8.694558262e-02, /*DMEAN*/  6.149514087e-02, /*EDMEAN*/  5.450464498e-02 },
            {344, /* SP "K5III             " idx */ 220, /*MAG*/  7.153000000e+00,  5.690000000e+00,  4.220000000e+00,  3.093000000e+00,  2.340000000e+00,  2.041000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  1.077033000e-02,  2.520000000e-01,  2.520000000e-01,  2.520000000e-01, /*DIAM*/  2.968618096e-01,  3.151577695e-01,  3.406859625e-01, /*EDIAM*/  6.993466375e-02,  6.092142101e-02,  5.713254425e-02, /*DMEAN*/  3.203965862e-01, /*EDMEAN*/  3.584708420e-02 },
            {345, /* SP "K1III             " idx */ 204, /*MAG*/  6.492000000e+00,  5.350000000e+00,  4.240000000e+00,  3.777000000e+00,  3.160000000e+00,  2.876000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  1.044031000e-02,  3.020000000e-01,  3.020000000e-01,  3.020000000e-01, /*DIAM*/  5.654311931e-02,  8.400791118e-02,  1.231668290e-01, /*EDIAM*/  8.371162900e-02,  7.293504246e-02,  6.840245911e-02, /*DMEAN*/  9.219231614e-02, /*EDMEAN*/  4.289880844e-02 },
            {346, /* SP "K1III             " idx */ 204, /*MAG*/  6.913000000e+00,  5.970000000e+00,  5.040000000e+00,  3.933000000e+00,  3.557000000e+00,  3.550000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  4.000000000e-03,  2.380000000e-01,  2.380000000e-01,  2.380000000e-01, /*DIAM*/  6.097169080e-02,  1.380557550e-02, -1.305215117e-02, /*EDIAM*/  6.603017361e-02,  5.751656777e-02,  5.393861666e-02, /*DMEAN*/  1.559904346e-02, /*EDMEAN*/  3.385053644e-02 },
            {347, /* SP "K1.5IIIFe-1       " idx */ 206, /*MAG*/  5.999000000e+00,  4.820000000e+00,  3.660000000e+00,  2.949000000e+00,  2.197000000e+00,  2.085000000e+00, /*EMAG*/  1.315295000e-02,  4.000000000e-02,  3.006659000e-02,  2.340000000e-01,  2.340000000e-01,  2.340000000e-01, /*DIAM*/  2.485189746e-01,  2.967692069e-01,  2.905551432e-01, /*EDIAM*/  6.492782687e-02,  5.655508907e-02,  5.303656041e-02, /*DMEAN*/  2.816986347e-01, /*EDMEAN*/  3.328597657e-02 },
            {348, /* SP "K5III:            " idx */ 220, /*MAG*/  7.483000000e+00,  6.240000000e+00,  5.040000000e+00,  4.453000000e+00,  3.704000000e+00,  3.574000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  1.077033000e-02,  3.200000000e-01,  3.200000000e-01,  3.200000000e-01, /*DIAM*/ -3.733462395e-02,  8.784224110e-03,  8.255300660e-03, /*EDIAM*/  8.871692390e-02,  7.729900506e-02,  7.249592496e-02, /*DMEAN*/ -3.506312696e-03, /*EDMEAN*/  4.545976084e-02 },
            {349, /* SP "K1III             " idx */ 204, /*MAG*/  6.825000000e+00,  5.670000000e+00,  4.550000000e+00,  3.795000000e+00,  3.171000000e+00,  2.997000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  1.044031000e-02,  2.880000000e-01,  2.880000000e-01,  2.880000000e-01, /*DIAM*/  7.613240531e-02,  9.455265842e-02,  1.041960258e-01, /*EDIAM*/  7.984256699e-02,  6.956144699e-02,  6.523781458e-02, /*DMEAN*/  9.351548299e-02, /*EDMEAN*/  4.091859363e-02 },
            {350, /* SP "K2III             " idx */ 208, /*MAG*/  7.167000000e+00,  6.280000000e+00,  5.390000000e+00,  4.889000000e+00,  4.322000000e+00,  4.317000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  4.000000000e-03,  2.520000000e-01,  2.520000000e-01,  2.520000000e-01, /*DIAM*/ -1.729201338e-01, -1.533154032e-01, -1.737601587e-01, /*EDIAM*/  6.990173852e-02,  6.089280733e-02,  5.710578228e-02, /*DMEAN*/ -1.664800562e-01, /*EDMEAN*/  3.583050170e-02 },
            {351, /* SP "K5III             " idx */ 220, /*MAG*/  7.286000000e+00,  5.810000000e+00,  4.320000000e+00,  3.076000000e+00,  2.361000000e+00,  2.128000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  1.077033000e-02,  2.460000000e-01,  2.460000000e-01,  2.460000000e-01, /*DIAM*/  3.107814527e-01,  3.150410380e-01,  3.241531156e-01, /*EDIAM*/  6.827844117e-02,  5.947705680e-02,  5.577757332e-02, /*DMEAN*/  3.175058522e-01, /*EDMEAN*/  3.499960751e-02 },
            {352, /* SP "K0III             " idx */ 200, /*MAG*/  7.218000000e+00,  6.200000000e+00,  5.210000000e+00,  4.676000000e+00,  4.064000000e+00,  3.899000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  3.000000000e-03,  2.280000000e-01,  2.280000000e-01,  2.280000000e-01, /*DIAM*/ -1.341736741e-01, -1.034842617e-01, -9.050857809e-02, /*EDIAM*/  6.326902375e-02,  5.510865540e-02,  5.167987197e-02, /*DMEAN*/ -1.064171874e-01, /*EDMEAN*/  3.243944475e-02 },
            {353, /* SP "K4III             " idx */ 216, /*MAG*/  6.951000000e+00,  5.540000000e+00,  4.150000000e+00,  3.145000000e+00,  2.453000000e+00,  2.147000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  1.044031000e-02,  2.460000000e-01,  2.460000000e-01,  2.460000000e-01, /*DIAM*/  2.655592591e-01,  2.767447664e-01,  3.076608358e-01, /*EDIAM*/  6.826612607e-02,  5.946631014e-02,  5.576747508e-02, /*DMEAN*/  2.859627262e-01, /*EDMEAN*/  3.499300529e-02 },
            {354, /* SP "K2.5III           " idx */ 210, /*MAG*/  5.818000000e+00,  4.500000000e+00,  3.250000000e+00,  2.435000000e+00,  1.817000000e+00,  1.668000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  3.001666000e-02,  3.020000000e-01,  3.020000000e-01,  3.020000000e-01, /*DIAM*/  3.729599004e-01,  3.799676729e-01,  3.812844173e-01, /*EDIAM*/  8.371944609e-02,  7.294170343e-02,  6.840857144e-02, /*DMEAN*/  3.786490626e-01, /*EDMEAN*/  4.290175809e-02 },
            {355, /* SP "G8III/IV          " idx */ 192, /*MAG*/  5.611000000e+00,  4.680000000e+00,  3.730000000e+00,  3.162000000e+00,  2.695000000e+00,  2.590000000e+00, /*EMAG*/  3.605551000e-02,  4.000000000e-02,  2.009975000e-02,  2.720000000e-01,  2.720000000e-01,  2.720000000e-01, /*DIAM*/  1.535709214e-01,  1.558047178e-01,  1.572495103e-01, /*EDIAM*/  7.543654597e-02,  6.572018713e-02,  6.163497232e-02, /*DMEAN*/  1.557872454e-01, /*EDMEAN*/  3.866848890e-02 },
            {356, /* SP "K2IIICNII         " idx */ 208, /*MAG*/  5.781000000e+00,  4.590000000e+00,  3.470000000e+00,  2.639000000e+00,  2.127000000e+00,  1.967000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  2.009975000e-02,  2.840000000e-01,  2.840000000e-01,  2.840000000e-01, /*DIAM*/  3.200798735e-01,  3.065133974e-01,  3.135829143e-01, /*EDIAM*/  7.874181498e-02,  6.860150808e-02,  6.433721790e-02, /*DMEAN*/  3.128433520e-01, /*EDMEAN*/  4.035439624e-02 },
            {357, /* SP "G8IIIa:           " idx */ 192, /*MAG*/  4.245000000e+00,  3.330000000e+00,  2.420000000e+00,  1.879000000e+00,  1.557000000e+00,  1.439000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  2.009975000e-02,  3.120000000e-01,  3.120000000e-01,  3.120000000e-01, /*DIAM*/  4.050262823e-01,  3.746607522e-01,  3.822203166e-01, /*EDIAM*/  8.648866848e-02,  7.535699467e-02,  7.067485861e-02, /*DMEAN*/  3.855846118e-01, /*EDMEAN*/  4.432434940e-02 },
            {358, /* SP "F9V+M0-V          " idx */ 156, /*MAG*/  4.047000000e+00,  3.460000000e+00,  2.800000000e+00,  2.109000000e+00,  2.086000000e+00,  1.970000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  3.000000000e-03,  5.700000000e-01,  5.700000000e-01,  5.700000000e-01, /*DIAM*/  2.955291807e-01,  2.273930901e-01,  2.363120628e-01, /*EDIAM*/  1.579280394e-01,  1.376357090e-01,  1.290929290e-01, /*DMEAN*/  2.487589805e-01, /*EDMEAN*/  8.090155043e-02 },
            {359, /* SP "F9V               " idx */ 156, /*MAG*/  5.340000000e+00,  4.800000000e+00,  4.190000000e+00,  3.936000000e+00,  3.681000000e+00,  3.638000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  3.006659000e-02,  2.540000000e-01,  2.540000000e-01,  2.540000000e-01, /*DIAM*/ -1.072654682e-01, -1.021244246e-01, -1.059069204e-01, /*EDIAM*/  7.066230588e-02,  6.155846114e-02,  5.773167641e-02, /*DMEAN*/ -1.049564135e-01, /*EDMEAN*/  3.623405055e-02 },
            {360, /* SP "K1V_Fe-2          " idx */ 204, /*MAG*/  5.874000000e+00,  5.170000000e+00,  4.340000000e+00,  4.031000000e+00,  3.597000000e+00,  3.505000000e+00, /*EMAG*/              NaN,  4.000000000e-02,              NaN,  3.440000000e-01,  3.440000000e-01,  3.440000000e-01, /*DIAM*/ -2.758188195e-02, -2.884033953e-02, -2.389156739e-02, /*EDIAM*/  9.532170949e-02,  8.305769985e-02,  7.789797238e-02, /*DMEAN*/ -2.656735504e-02, /*EDMEAN*/  4.884156149e-02 },
            {361, /* SP "K0V               " idx */ 200, /*MAG*/  6.434000000e+00,  5.630000000e+00,  4.820000000e+00,  4.596000000e+00,  4.133000000e+00,  4.014000000e+00, /*EMAG*/  1.019804000e-02,  4.000000000e-02,  2.000000000e-03,  2.620000000e-01,  2.620000000e-01,  2.620000000e-01, /*DIAM*/ -1.557986744e-01, -1.436399043e-01, -1.315085787e-01, /*EDIAM*/  7.265877416e-02,  6.329741553e-02,  5.936181678e-02, /*DMEAN*/ -1.420581576e-01, /*EDMEAN*/  3.724357549e-02 },
            {362, /* SP "F8V               " idx */ 152, /*MAG*/  4.614000000e+00,  4.100000000e+00,  3.510000000e+00,  3.031000000e+00,  2.863000000e+00,  2.697000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  2.000000000e-03,  2.880000000e-01,  2.880000000e-01,  2.880000000e-01, /*DIAM*/  8.500687080e-02,  6.459843360e-02,  8.602365575e-02, /*EDIAM*/  8.005066687e-02,  6.974480780e-02,  6.541086483e-02, /*DMEAN*/  7.835891341e-02, /*EDMEAN*/  4.103610203e-02 },
            {363, /* SP "F9.5V             " idx */ 158, /*MAG*/  4.645000000e+00,  4.050000000e+00,  3.400000000e+00,  3.143000000e+00,  2.875000000e+00,  2.723000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  1.044031000e-02,  2.660000000e-01,  2.660000000e-01,  2.660000000e-01, /*DIAM*/  5.700833583e-02,  6.230850358e-02,  8.275216462e-02, /*EDIAM*/  7.395705748e-02,  6.443154205e-02,  6.042689011e-02, /*DMEAN*/  6.895119168e-02, /*EDMEAN*/  3.791980342e-02 },
            {364, /* SP "G5V               " idx */ 180, /*MAG*/  5.521000000e+00,  4.840000000e+00,  4.110000000e+00,  3.407000000e+00,  3.039000000e+00,  2.957000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  2.022375000e-02,  2.120000000e-01,  2.120000000e-01,  2.120000000e-01, /*DIAM*/  7.679565013e-02,  6.885476580e-02,  6.714171309e-02, /*EDIAM*/  5.894012016e-02,  5.133470158e-02,  4.814062421e-02, /*DMEAN*/  7.025896034e-02, /*EDMEAN*/  3.023675647e-02 },
            {365, /* SP "F9IV-V            " idx */ 156, /*MAG*/  4.865000000e+00,  4.290000000e+00,  3.630000000e+00,  3.194000000e+00,  2.916000000e+00,  2.835000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  2.009975000e-02,  2.600000000e-01,  2.600000000e-01,  2.600000000e-01, /*DIAM*/  5.894881999e-02,  6.139308758e-02,  6.239235217e-02, /*EDIAM*/  7.231478224e-02,  6.299944853e-02,  5.908343022e-02, /*DMEAN*/  6.114561993e-02, /*EDMEAN*/  3.707932516e-02 },
            {366, /* SP "G1V               " idx */ 164, /*MAG*/  5.320000000e+00,  4.690000000e+00,  3.990000000e+00,  3.397000000e+00,  3.154000000e+00,  3.038000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  3.000000000e-03,  2.600000000e-01,  2.600000000e-01,  2.600000000e-01, /*DIAM*/  4.353314463e-02,  2.442430433e-02,  3.239769719e-02, /*EDIAM*/  7.227101416e-02,  6.296134242e-02,  5.904785589e-02, /*DMEAN*/  3.256000071e-02, /*EDMEAN*/  3.705795456e-02 },
            {367, /* SP "F1V               " idx */ 124, /*MAG*/  4.480000000e+00,  4.160000000e+00,  3.760000000e+00,  3.221000000e+00,  3.156000000e+00,  2.978000000e+00, /*EMAG*/  1.236932000e-02,  4.000000000e-02,  3.014963000e-02,  3.160000000e-01,  3.160000000e-01,  3.160000000e-01, /*DIAM*/  1.278003866e-02, -1.586507027e-02,  4.200940972e-03, /*EDIAM*/  8.799125663e-02,  7.666864349e-02,  7.190558320e-02, /*DMEAN*/ -4.803444797e-04, /*EDMEAN*/  4.510009915e-02 },
            {368, /* SP "F7V               " idx */ 148, /*MAG*/  3.645000000e+00,  3.170000000e+00,  2.610000000e+00,  2.280000000e+00,  2.079000000e+00,  1.970000000e+00, /*EMAG*/  1.824829000e-02,  4.000000000e-02,  1.044031000e-02,  2.860000000e-01,  2.860000000e-01,  2.860000000e-01, /*DIAM*/  2.173406435e-01,  2.137354165e-01,  2.235169688e-01, /*EDIAM*/  7.952060692e-02,  6.928253951e-02,  6.497709930e-02, /*DMEAN*/  2.185215586e-01, /*EDMEAN*/  4.076396876e-02 },
            {369, /* SP "G3Va              " idx */ 172, /*MAG*/  6.046000000e+00,  5.370000000e+00,  4.630000000e+00,  4.267000000e+00,  4.040000000e+00,  3.821000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  4.000000000e-03,  3.280000000e-01,  3.280000000e-01,  3.280000000e-01, /*DIAM*/ -1.334872149e-01, -1.565088489e-01, -1.209967060e-01, /*EDIAM*/  9.099221009e-02,  7.928462224e-02,  7.435982104e-02, /*DMEAN*/ -1.365317157e-01, /*EDMEAN*/  4.663601398e-02 },
            {370, /* SP "F8V               " idx */ 152, /*MAG*/  5.361000000e+00,  4.820000000e+00,  4.240000000e+00,  4.033000000e+00,  3.758000000e+00,  3.644000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  1.019804000e-02,  2.220000000e-01,  2.220000000e-01,  2.220000000e-01, /*DIAM*/ -1.370467039e-01, -1.216194680e-01, -1.093413107e-01, /*EDIAM*/  6.188219099e-02,  5.390132334e-02,  5.054839757e-02, /*DMEAN*/ -1.208314576e-01, /*EDMEAN*/  3.174267082e-02 },
            {371, /* SP "A1IVps            " idx */ 84, /*MAG*/  2.373000000e+00,  2.340000000e+00,  2.320000000e+00,  2.269000000e+00,  2.359000000e+00,  2.285000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  3.006659000e-02,  2.440000000e-01,  2.440000000e-01,  2.440000000e-01, /*DIAM*/  7.574790754e-02,  4.563323921e-02,  4.976597238e-02, /*EDIAM*/  6.883829481e-02,  5.998461266e-02,  5.626505404e-02, /*DMEAN*/  5.514019645e-02, /*EDMEAN*/  3.539913873e-02 },
            {372, /* SP "G8V               " idx */ 192, /*MAG*/  6.033000000e+00,  5.310000000e+00,  4.530000000e+00,  3.988000000e+00,  3.648000000e+00,  3.588000000e+00, /*EMAG*/  1.431782000e-02,  4.000000000e-02,  4.011234000e-02,  2.420000000e-01,  2.420000000e-01,  2.420000000e-01, /*DIAM*/ -2.667908124e-02, -4.811746421e-02, -5.202056578e-02, /*EDIAM*/  6.715162828e-02,  5.849539392e-02,  5.485748339e-02, /*DMEAN*/ -4.403897213e-02, /*EDMEAN*/  3.442967259e-02 },
            {373, /* SP "F9V               " idx */ 156, /*MAG*/  4.108000000e+00,  3.590000000e+00,  2.980000000e+00,  2.597000000e+00,  2.363000000e+00,  2.269000000e+00, /*EMAG*/  1.552417000e-02,  4.000000000e-02,  2.039608000e-02,  2.540000000e-01,  2.540000000e-01,  2.540000000e-01, /*DIAM*/  1.704398931e-01,  1.659300541e-01,  1.720711859e-01, /*EDIAM*/  7.066230588e-02,  6.155846114e-02,  5.773167641e-02, /*DMEAN*/  1.695233731e-01, /*EDMEAN*/  3.623405055e-02 },
            {374, /* SP "K1V_Fe-1.5        " idx */ 204, /*MAG*/  7.174000000e+00,  6.420000000e+00,  5.540000000e+00,  4.937000000e+00,  4.500000000e+00,  4.373000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  2.039608000e-02,  1.960000000e-01,  1.960000000e-01,  1.960000000e-01, /*DIAM*/ -1.823675985e-01, -1.951282798e-01, -1.874536136e-01, /*EDIAM*/  5.443832045e-02,  4.740569889e-02,  4.445305807e-02, /*DMEAN*/ -1.887742167e-01, /*EDMEAN*/  2.792104432e-02 },
            {375, /* SP "G0V               " idx */ 160, /*MAG*/  4.828000000e+00,  4.240000000e+00,  3.570000000e+00,  3.213000000e+00,  2.905000000e+00,  2.848000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  1.019804000e-02,  3.100000000e-01,  3.100000000e-01,  3.100000000e-01, /*DIAM*/  5.468897208e-02,  6.387105020e-02,  6.080365164e-02, /*EDIAM*/  8.607955342e-02,  7.500156906e-02,  7.034215080e-02, /*DMEAN*/  6.026096212e-02, /*EDMEAN*/  4.412238850e-02 },
            {376, /* SP "F9.5V             " idx */ 158, /*MAG*/  4.802000000e+00,  4.230000000e+00,  3.560000000e+00,  3.232000000e+00,  2.992000000e+00,  2.923000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  4.011234000e-02,  2.740000000e-01,  2.740000000e-01,  2.740000000e-01, /*DIAM*/  4.619583567e-02,  4.150694685e-02,  4.222661657e-02, /*EDIAM*/  7.616168969e-02,  6.635393144e-02,  6.223021215e-02, /*DMEAN*/  4.301758315e-02, /*EDMEAN*/  3.904763255e-02 },
            {377, /* SP "F7V               " idx */ 148, /*MAG*/  4.537000000e+00,  4.040000000e+00,  3.450000000e+00,  3.179000000e+00,  2.980000000e+00,  2.739000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  1.200167000e-01,  3.320000000e-01,  3.320000000e-01,  3.320000000e-01, /*DIAM*/  3.531385508e-02,  3.225681454e-02,  7.237098116e-02, /*EDIAM*/  9.220306602e-02,  8.034026107e-02,  7.534970603e-02, /*DMEAN*/  4.881003249e-02, /*EDMEAN*/  4.725392459e-02 },
            {378, /* SP "F4VkF2mF1         " idx */ 136, /*MAG*/  4.834000000e+00,  4.470000000e+00,  4.060000000e+00,  3.561000000e+00,  3.462000000e+00,  3.336000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  3.006659000e-02,  3.240000000e-01,  3.240000000e-01,  3.240000000e-01, /*DIAM*/ -4.807672799e-02, -7.107476329e-02, -5.936056182e-02, /*EDIAM*/  9.007013542e-02,  7.848057401e-02,  7.360501419e-02, /*DMEAN*/ -6.044913241e-02, /*EDMEAN*/  4.616094474e-02 },
            {379, /* SP "G7Ve+K5Ve         " idx */ 188, /*MAG*/  5.260000000e+00,  4.540000000e+00,  3.720000000e+00,  2.660000000e+00,  2.253000000e+00,  1.971000000e+00, /*EMAG*/  1.552417000e-02,  4.000000000e-02,  3.026549000e-02,  6.980000000e-01,  6.980000000e-01,  6.980000000e-01, /*DIAM*/  2.745194339e-01,  2.528811491e-01,  2.896822355e-01, /*EDIAM*/  1.932609079e-01,  1.684337305e-01,  1.579805472e-01, /*DMEAN*/  2.729998069e-01, /*EDMEAN*/  9.899098852e-02 },
            {380, /* SP "kA2hA5mA7V        " idx */ 88, /*MAG*/  3.857000000e+00,  3.710000000e+00,  3.580000000e+00,  3.564000000e+00,  3.440000000e+00,  3.425000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  2.009975000e-02,  2.660000000e-01,  2.660000000e-01,  2.660000000e-01, /*DIAM*/ -1.666417289e-01, -1.486905570e-01, -1.621620381e-01, /*EDIAM*/  7.482500277e-02,  6.520000620e-02,  6.115448316e-02, /*DMEAN*/ -1.586830552e-01, /*EDMEAN*/  3.843231259e-02 },
            {381, /* SP "F6IV              " idx */ 144, /*MAG*/  4.328000000e+00,  3.850000000e+00,  3.310000000e+00,  3.149000000e+00,  2.875000000e+00,  2.703000000e+00, /*EMAG*/              NaN,  4.000000000e-02,              NaN,  3.140000000e-01,  3.140000000e-01,  3.140000000e-01, /*DIAM*/  2.521991368e-02,  4.816737333e-02,  7.294320999e-02, /*EDIAM*/  8.726014482e-02,  7.603063275e-02,  7.130701228e-02, /*DMEAN*/  5.188532413e-02, /*EDMEAN*/  4.472355163e-02 },
            {382, /* SP "G5V               " idx */ 180, /*MAG*/  6.142000000e+00,  5.490000000e+00,  4.800000000e+00,  4.667000000e+00,  4.162000000e+00,  4.186000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  2.039608000e-02,  2.920000000e-01,  2.920000000e-01,  2.920000000e-01, /*DIAM*/ -2.220436400e-01, -1.752541873e-01, -1.938728893e-01, /*EDIAM*/  8.101151934e-02,  7.058217843e-02,  6.619639027e-02, /*DMEAN*/ -1.948224795e-01, /*EDMEAN*/  4.152652372e-02 },
            {383, /* SP "F5IV-V            " idx */ 140, /*MAG*/  5.004000000e+00,  4.570000000e+00,  4.070000000e+00,  3.803000000e+00,  3.648000000e+00,  3.502000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  3.000000000e-03,  3.620000000e-01,  3.620000000e-01,  3.620000000e-01, /*DIAM*/ -1.040521952e-01, -1.101954224e-01, -9.156495344e-02, /*EDIAM*/  1.005213304e-01,  8.759227662e-02,  8.215209328e-02, /*DMEAN*/ -1.012703352e-01, /*EDMEAN*/  5.151019578e-02 },
            {384, /* SP "F2V               " idx */ 128, /*MAG*/  5.010000000e+00,  4.620000000e+00,  4.170000000e+00,  3.928000000e+00,  3.665000000e+00,  3.639000000e+00, /*EMAG*/  2.716616000e-02,  4.000000000e-02,  2.022375000e-02,  2.540000000e-01,  2.540000000e-01,  2.540000000e-01, /*DIAM*/ -1.444420131e-01, -1.175161941e-01, -1.299395032e-01, /*EDIAM*/  7.089774340e-02,  6.176383572e-02,  5.792383243e-02, /*DMEAN*/ -1.294464869e-01, /*EDMEAN*/  3.635240923e-02 },
            {385, /* SP "F6V               " idx */ 144, /*MAG*/  4.673000000e+00,  4.190000000e+00,  3.640000000e+00,  3.527000000e+00,  3.286000000e+00,  3.190000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  1.000000000e-03,  2.560000000e-01,  2.560000000e-01,  2.560000000e-01, /*DIAM*/ -5.329794463e-02, -3.696103261e-02, -2.831956524e-02, /*EDIAM*/  7.128530961e-02,  6.210145801e-02,  5.824062266e-02, /*DMEAN*/ -3.784377119e-02, /*EDMEAN*/  3.654980535e-02 },
            {386, /* SP "G7IVHdel1         " idx */ 188, /*MAG*/  5.931000000e+00,  5.170000000e+00,  4.420000000e+00,  3.553000000e+00,  3.327000000e+00,  3.037000000e+00, /*EMAG*/  1.811077000e-02,  4.000000000e-02,  7.002857000e-02,  3.200000000e-01,  3.200000000e-01,  3.200000000e-01, /*DIAM*/  7.572478803e-02,  1.976830519e-02,  6.502529780e-02, /*EDIAM*/  8.871198967e-02,  7.729573776e-02,  7.249366368e-02, /*DMEAN*/  5.220107117e-02, /*EDMEAN*/  4.546391757e-02 },
            {387, /* SP "G9V               " idx */ 196, /*MAG*/  5.456000000e+00,  4.670000000e+00,  3.820000000e+00,  3.423000000e+00,  3.039000000e+00,  2.900000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  2.118962000e-02,  3.240000000e-01,  3.240000000e-01,  3.240000000e-01, /*DIAM*/  8.787305915e-02,  7.643028678e-02,  9.099290589e-02, /*EDIAM*/  8.979679962e-02,  7.824107750e-02,  7.338007549e-02, /*DMEAN*/  8.514699021e-02, /*EDMEAN*/  4.601591621e-02 },
            {388, /* SP "F6V               " idx */ 144, /*MAG*/  4.702000000e+00,  4.200000000e+00,  3.600000000e+00,  3.358000000e+00,  3.078000000e+00,  2.961000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  3.014963000e-02,  2.860000000e-01,  2.860000000e-01,  2.860000000e-01, /*DIAM*/ -5.753301068e-03,  1.363040784e-02,  2.376072750e-02, /*EDIAM*/  7.954419734e-02,  6.930303250e-02,  6.499619737e-02, /*DMEAN*/  1.253227350e-02, /*EDMEAN*/  4.077519887e-02 },
            {389, /* SP "F7V               " idx */ 148, /*MAG*/  4.637000000e+00,  4.130000000e+00,  3.540000000e+00,  3.299000000e+00,  2.988000000e+00,  2.946000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  2.022375000e-02,  2.880000000e-01,  2.880000000e-01,  2.880000000e-01, /*DIAM*/  9.010283258e-03,  3.403891573e-02,  2.789652794e-02, /*EDIAM*/  8.007169028e-02,  6.976304881e-02,  6.542784343e-02, /*DMEAN*/  2.507080631e-02, /*EDMEAN*/  4.104593848e-02 },
            {390, /* SP "M2V               " idx */ 248, /*MAG*/  9.650000000e+00,  8.090000000e+00,  5.610000000e+00,  5.252000000e+00,  4.476000000e+00,  4.018000000e+00, /*EMAG*/  4.472136000e-02,  4.000000000e-02,  1.711724000e-01,  2.640000000e-01,  2.640000000e-01,  2.640000000e-01, /*DIAM*/ -1.116228483e-01, -6.951240958e-02, -7.764658977e-03, /*EDIAM*/  7.323870028e-02,  6.380366577e-02,  5.983754999e-02, /*DMEAN*/ -5.628240900e-02, /*EDMEAN*/  3.754969323e-02 },
            {391, /* SP "K2.5V             " idx */ 210, /*MAG*/  6.630000000e+00,  5.740000000e+00,  4.770000000e+00,  4.367000000e+00,  3.722000000e+00,  3.683000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  2.022375000e-02,  3.100000000e-01,  3.100000000e-01,  3.100000000e-01, /*DIAM*/ -6.657582048e-02, -2.846034876e-02, -4.208055254e-02, /*EDIAM*/  8.593037780e-02,  7.486945607e-02,  7.021691347e-02, /*DMEAN*/ -4.379454226e-02, /*EDMEAN*/  4.403340862e-02 },
            {392, /* SP "K3V               " idx */ 212, /*MAG*/  6.708000000e+00,  5.790000000e+00,  4.730000000e+00,  4.152000000e+00,  3.657000000e+00,  3.481000000e+00, /*EMAG*/              NaN,  4.000000000e-02,              NaN,  2.640000000e-01,  2.640000000e-01,  2.640000000e-01, /*DIAM*/ -9.360619378e-06, -8.304562604e-03,  7.387100702e-03, /*EDIAM*/  7.322509479e-02,  6.379086276e-02,  5.982437162e-02, /*DMEAN*/  3.159658038e-05, /*EDMEAN*/  3.753065310e-02 },
            {393, /* SP "M1V               " idx */ 244, /*MAG*/  9.444000000e+00,  7.970000000e+00,  5.890000000e+00,  4.999000000e+00,  4.149000000e+00,  4.039000000e+00, /*EMAG*/  1.421267000e-02,  4.000000000e-02,  1.486607000e-02,  3.000000000e-01,  3.000000000e-01,  3.000000000e-01, /*DIAM*/ -4.658291648e-02,  7.269613520e-04, -2.050712080e-02, /*EDIAM*/  8.318162057e-02,  7.247302224e-02,  6.796951169e-02, /*DMEAN*/ -2.000523582e-02, /*EDMEAN*/  4.263273108e-02 },
            {394, /* SP "M0V               " idx */ 240, /*MAG*/  9.050000000e+00,  7.640000000e+00,  5.940000000e+00,  4.889000000e+00,  3.987000000e+00,  3.988000000e+00, /*EMAG*/  1.886796000e-02,  4.000000000e-02,  3.400000000e-02,  1.880000000e-01,  1.880000000e-01,  1.880000000e-01, /*DIAM*/ -3.924481369e-02,  2.209533309e-02, -2.265053046e-02, /*EDIAM*/  5.228114338e-02,  4.552359804e-02,  4.268762057e-02, /*DMEAN*/ -1.153418938e-02, /*EDMEAN*/  2.682257717e-02 },
            {395, /* SP "K7V               " idx */ 228, /*MAG*/  9.120000000e+00,  7.700000000e+00,  5.980000000e+00,  4.779000000e+00,  4.043000000e+00,  4.136000000e+00, /*EMAG*/              NaN,  4.000000000e-02,              NaN,  2.060000000e-01,  2.060000000e-01,  2.060000000e-01, /*DIAM*/ -7.270971434e-03, -2.862571245e-03, -7.014283108e-02, /*EDIAM*/  5.726289731e-02,  4.986943682e-02,  4.676439772e-02, /*DMEAN*/ -3.046093637e-02, /*EDMEAN*/  2.936566570e-02 },
            {396, /* SP "K8V               " idx */ 232, /*MAG*/  7.926000000e+00,  6.600000000e+00,  5.310000000e+00,  3.894000000e+00,  3.298000000e+00,  2.962000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  1.077033000e-02,  2.900000000e-01,  2.900000000e-01,  2.900000000e-01, /*DIAM*/  1.556661326e-01,  1.363303429e-01,  1.718391671e-01, /*EDIAM*/  8.044198954e-02,  7.008394412e-02,  6.572795401e-02, /*DMEAN*/  1.553413439e-01, /*EDMEAN*/  4.122613676e-02 },
            {397, /* SP "M3V               " idx */ 252, /*MAG*/  9.895000000e+00,  8.460000000e+00,  6.390000000e+00,  5.181000000e+00,  4.775000000e+00,  4.415000000e+00, /*EMAG*/  3.275668000e-02,  4.000000000e-02,  2.624881000e-02,  2.060000000e-01,  2.060000000e-01,  2.060000000e-01, /*DIAM*/ -7.002183218e-02, -1.231790005e-01, -8.326782001e-02, /*EDIAM*/  5.726834467e-02,  4.987702301e-02,  4.677395900e-02, /*DMEAN*/ -9.358721038e-02, /*EDMEAN*/  2.939327760e-02 },
            {398, /* SP "K1V               " idx */ 204, /*MAG*/  6.597000000e+00,  5.770000000e+00,  4.900000000e+00,  4.446000000e+00,  4.053000000e+00,  4.039000000e+00, /*EMAG*/  1.264911000e-02,  4.000000000e-02,  2.039608000e-02,  2.760000000e-01,  2.760000000e-01,  2.760000000e-01, /*DIAM*/ -9.637652583e-02, -1.141010412e-01, -1.289572624e-01, /*EDIAM*/  7.652670639e-02,  6.667010293e-02,  6.252552319e-02, /*DMEAN*/ -1.152944882e-01, /*EDMEAN*/  3.922161493e-02 },
            {399, /* SP "M3.0Ve            " idx */ 252, /*MAG*/  1.065500000e+01,  9.150000000e+00,  7.040000000e+00,  5.335000000e+00,  4.766000000e+00,  4.548000000e+00, /*EMAG*/  3.178050000e-02,  4.000000000e-02,  1.041201000e-01,  3.300000000e-02,  3.300000000e-02,  3.300000000e-02, /*DIAM*/ -5.966468916e-02, -9.254864986e-02, -9.523132384e-02, /*EDIAM*/  1.058565018e-02,  9.019673228e-03,  8.412904845e-03, /*DMEAN*/ -8.581229889e-02, /*EDMEAN*/  5.749392420e-03 },
            {400, /* SP "K0-V              " idx */ 200, /*MAG*/  4.890000000e+00,  4.030000000e+00,  3.070000000e+00,  2.343000000e+00,  1.876000000e+00,  1.791000000e+00, /*EMAG*/              NaN,  4.000000000e-02,              NaN,  3.040000000e-01,  3.040000000e-01,  3.040000000e-01, /*DIAM*/  3.449424044e-01,  3.348581573e-01,  3.294622311e-01, /*EDIAM*/  8.426433543e-02,  7.341712701e-02,  6.885481911e-02, /*DMEAN*/  3.353807036e-01, /*EDMEAN*/  4.318285084e-02 },
            {401, /* SP "M1.0Ve            " idx */ 244, /*MAG*/  1.003300000e+01,  8.550000000e+00,  6.580000000e+00,  5.429000000e+00,  4.919000000e+00,  4.618000000e+00, /*EMAG*/  1.920937000e-02,  4.000000000e-02,  7.158911000e-02,  5.900000000e-02,  5.900000000e-02,  5.900000000e-02, /*DIAM*/ -1.210650604e-01, -1.611096169e-01, -1.362808452e-01, /*EDIAM*/  1.705675427e-02,  1.473095786e-02,  1.378246841e-02, /*DMEAN*/ -1.410196009e-01, /*EDMEAN*/  8.888717043e-03 },
            {402, /* SP "K3V               " idx */ 212, /*MAG*/  6.570000000e+00,  5.570000000e+00,  4.530000000e+00,  3.981000000e+00,  3.469000000e+00,  3.261000000e+00, /*EMAG*/              NaN,  4.000000000e-02,              NaN,  3.040000000e-01,  3.040000000e-01,  3.040000000e-01, /*DIAM*/  3.042813983e-02,  2.797559358e-02,  5.138710136e-02, /*EDIAM*/  8.427633833e-02,  7.342724510e-02,  6.886401338e-02, /*DMEAN*/  3.781246228e-02, /*EDMEAN*/  4.318663813e-02 },
            {403, /* SP "K0.5V             " idx */ 202, /*MAG*/  5.250000000e+00,  4.430000000e+00,  3.540000000e+00,  3.013000000e+00,  2.594000000e+00,  2.498000000e+00, /*EMAG*/              NaN,  4.000000000e-02,              NaN,  2.380000000e-01,  2.380000000e-01,  2.380000000e-01, /*DIAM*/  1.938103819e-01,  1.803502304e-01,  1.822374722e-01, /*EDIAM*/  6.602941767e-02,  5.751600709e-02,  5.393817278e-02, /*DMEAN*/  1.846150194e-01, /*EDMEAN*/  3.385083142e-02 },
            {404, /* SP "F5V               " idx */ 140, /*MAG*/  4.205000000e+00,  3.770000000e+00,  3.260000000e+00,  2.954000000e+00,  2.729000000e+00,  2.564000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  3.026549000e-02,  2.900000000e-01,  2.900000000e-01,  2.900000000e-01, /*DIAM*/  6.951030736e-02,  7.851275158e-02,  9.966132679e-02, /*EDIAM*/  8.067305953e-02,  7.028728905e-02,  6.591938754e-02, /*DMEAN*/  8.446099151e-02, /*EDMEAN*/  4.135223840e-02 },
            {405, /* SP "K0V               " idx */ 200, /*MAG*/  6.822000000e+00,  6.070000000e+00,  5.270000000e+00,  4.733000000e+00,  4.629000000e+00,  4.314000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  4.000000000e-03,  1.440000000e-01,  1.440000000e-01,  1.440000000e-01, /*DIAM*/ -1.599326031e-01, -2.451496335e-01, -1.878297474e-01, /*EDIAM*/  4.011296271e-02,  3.490494016e-02,  3.272409385e-02, /*DMEAN*/ -2.003551353e-01, /*EDMEAN*/  2.060139783e-02 },
            {406, /* SP "F6V               " idx */ 144, /*MAG*/  6.399000000e+00,  5.830000000e+00,  5.190000000e+00,  4.755000000e+00,  4.794000000e+00,  4.445000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  3.000000000e-03,  2.140000000e-01,  2.140000000e-01,  2.140000000e-01, /*DIAM*/ -2.672622335e-01, -3.331166790e-01, -2.692027805e-01, /*EDIAM*/  5.974368129e-02,  5.203593561e-02,  4.879808954e-02, /*DMEAN*/ -2.907680281e-01, /*EDMEAN*/  3.064689628e-02 },
            {407, /* SP "K1V               " idx */ 204, /*MAG*/  6.076000000e+00,  5.240000000e+00,  4.360000000e+00,  3.855000000e+00,  3.391000000e+00,  3.285000000e+00, /*EMAG*/              NaN,  4.000000000e-02,              NaN,  2.660000000e-01,  2.660000000e-01,  2.660000000e-01, /*DIAM*/  2.650740457e-02,  2.374331884e-02,  2.772887134e-02, /*EDIAM*/  7.376387194e-02,  6.426089699e-02,  6.026548931e-02, /*DMEAN*/  2.603271465e-02, /*EDMEAN*/  3.780774463e-02 },
            {408, /* SP "G3Va              " idx */ 172, /*MAG*/  6.990000000e+00,  6.270000000e+00,  5.500000000e+00,  5.386000000e+00,  4.678000000e+00,  4.601000000e+00, /*EMAG*/  1.029563000e-02,  4.000000000e-02,  1.118034000e-02,  2.680000000e-01,  2.680000000e-01,  2.680000000e-01, /*DIAM*/ -3.741032899e-01, -2.733026249e-01, -2.738434236e-01, /*EDIAM*/  7.443115543e-02,  6.484479482e-02,  6.081462984e-02, /*DMEAN*/ -2.999114277e-01, /*EDMEAN*/  3.816223627e-02 },
            {409, /* SP "G9VCN+1           " idx */ 196, /*MAG*/  7.237000000e+00,  6.420000000e+00,  5.570000000e+00,  5.023000000e+00,  4.637000000e+00,  4.491000000e+00, /*EMAG*/  1.581139000e-02,  4.000000000e-02,  2.061553000e-02,  2.600000000e-02,  2.600000000e-02,  2.600000000e-02, /*DIAM*/ -2.206090883e-01, -2.369004572e-01, -2.230289966e-01, /*EDIAM*/  8.544460884e-03,  7.175523906e-03,  6.657660720e-03, /*DMEAN*/ -2.273431776e-01, /*EDMEAN*/  4.640615332e-03 },
            {410, /* SP "F7V               " idx */ 148, /*MAG*/  6.231000000e+00,  5.720000000e+00,  5.140000000e+00,  4.715000000e+00,  4.642000000e+00,  4.506000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  1.044031000e-02,  2.980000000e-01,  2.980000000e-01,  2.980000000e-01, /*DIAM*/ -2.608290065e-01, -2.994007780e-01, -2.833151555e-01, /*EDIAM*/  8.282760480e-02,  7.216599182e-02,  6.768192655e-02, /*DMEAN*/ -2.829794745e-01, /*EDMEAN*/  4.245609406e-02 },
            {411, /* SP "G2V               " idx */ 168, /*MAG*/  6.609000000e+00,  5.970000000e+00,  5.270000000e+00,  5.260000000e+00,  4.595000000e+00,  4.405000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  6.000000000e-03,  2.940000000e-01,  2.940000000e-01,  2.940000000e-01, /*DIAM*/ -3.682330860e-01, -2.681483229e-01, -2.404046540e-01, /*EDIAM*/  8.162614348e-02,  7.111858673e-02,  6.669982284e-02, /*DMEAN*/ -2.834679892e-01, /*EDMEAN*/  4.184383884e-02 },
            {412, /* SP "F6V               " idx */ 144, /*MAG*/  5.617000000e+00,  5.130000000e+00,  4.570000000e+00,  4.127000000e+00,  3.942000000e+00,  3.868000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  3.000000000e-03,  2.460000000e-01,  2.460000000e-01,  2.460000000e-01, /*DIAM*/ -1.471908032e-01, -1.564474157e-01, -1.570348956e-01, /*EDIAM*/  6.853474673e-02,  5.970285614e-02,  5.599052472e-02, /*DMEAN*/ -1.542549179e-01, /*EDMEAN*/  3.514279892e-02 },
            {413, /* SP "F9IV-V+L4+L4      " idx */ 156, /*MAG*/  6.436000000e+00,  5.860000000e+00,  5.190000000e+00,  4.998000000e+00,  4.688000000e+00,  4.458000000e+00, /*EMAG*/  1.627882000e-02,  4.000000000e-02,  7.006426000e-02,  2.260000000e-01,  2.260000000e-01,  2.260000000e-01, /*DIAM*/ -3.198190428e-01, -3.013384353e-01, -2.636003534e-01, /*EDIAM*/  6.295648210e-02,  5.483836883e-02,  5.142760931e-02, /*DMEAN*/ -2.913454014e-01, /*EDMEAN*/  3.229307642e-02 },
            {414, /* SP "F8IV              " idx */ 152, /*MAG*/  5.580000000e+00,  5.040000000e+00,  4.430000000e+00,  4.341000000e+00,  3.947000000e+00,  4.008000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  2.022375000e-02,  3.060000000e-01,  3.060000000e-01,  3.060000000e-01, /*DIAM*/ -2.054038478e-01, -1.581408693e-01, -1.859252534e-01, /*EDIAM*/  8.501309849e-02,  7.407161545e-02,  6.946961293e-02, /*DMEAN*/ -1.814338093e-01, /*EDMEAN*/  4.357529967e-02 },
            {415, /* SP "G2.5V             " idx */ 170, /*MAG*/  6.544000000e+00,  5.860000000e+00,  5.120000000e+00,  4.593000000e+00,  4.045000000e+00,  4.297000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  2.061553000e-02,  3.520000000e-01,  3.520000000e-01,  3.520000000e-01, /*DIAM*/ -1.891238973e-01, -1.387823726e-01, -2.173623584e-01, /*EDIAM*/  9.762976996e-02,  8.507155757e-02,  7.978812061e-02, /*DMEAN*/ -1.828292357e-01, /*EDMEAN*/  5.003326070e-02 },
            {416, /* SP "G0V               " idx */ 160, /*MAG*/  5.999000000e+00,  5.380000000e+00,  4.680000000e+00,  4.160000000e+00,  3.905000000e+00,  3.911000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  2.022375000e-02,  2.660000000e-01,  2.660000000e-01,  2.660000000e-01, /*DIAM*/ -1.198913877e-01, -1.303546336e-01, -1.497729938e-01, /*EDIAM*/  7.394644897e-02,  6.442231345e-02,  6.041828101e-02, /*DMEAN*/ -1.352425607e-01, /*EDMEAN*/  3.791467692e-02 },
            {417, /* SP "K0V               " idx */ 200, /*MAG*/  7.199000000e+00,  6.440000000e+00,  5.640000000e+00,  4.969000000e+00,  4.637000000e+00,  4.515000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  5.000000000e-03,  2.280000000e-01,  2.280000000e-01,  2.280000000e-01, /*DIAM*/ -1.968433179e-01, -2.318188940e-01, -2.235888720e-01, /*EDIAM*/  6.326902375e-02,  5.510865540e-02,  5.167987197e-02, /*DMEAN*/ -2.194315192e-01, /*EDMEAN*/  3.243944475e-02 },
            {418, /* SP "F5V               " idx */ 140, /*MAG*/  5.430000000e+00,  4.990000000e+00,  4.480000000e+00,  4.203000000e+00,  4.059000000e+00,  3.944000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  2.000000000e-03,  3.100000000e-01,  3.100000000e-01,  3.100000000e-01, /*DIAM*/ -1.825164821e-01, -1.920242174e-01, -1.805430570e-01, /*EDIAM*/  8.618275056e-02,  7.509123501e-02,  7.042572705e-02, /*DMEAN*/ -1.850245639e-01, /*EDMEAN*/  4.417166105e-02 },
            {419, /* SP "G1.5Vb            " idx */ 166, /*MAG*/  6.633000000e+00,  5.990000000e+00,  5.380000000e+00,  5.091000000e+00,  4.724000000e+00,  4.426000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  3.026549000e-02,  2.120000000e-01,  2.120000000e-01,  2.120000000e-01, /*DIAM*/ -3.227676431e-01, -2.996051292e-01, -2.460895697e-01, /*EDIAM*/  5.904047605e-02,  5.142297345e-02,  4.822378838e-02, /*DMEAN*/ -2.846318850e-01, /*EDMEAN*/  3.029211116e-02 },
            {420, /* SP "G2V               " idx */ 168, /*MAG*/  6.349000000e+00,  5.660000000e+00,  4.920000000e+00,  4.309000000e+00,  3.908000000e+00,  3.999000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  2.022375000e-02,  2.820000000e-01,  2.820000000e-01,  2.820000000e-01, /*DIAM*/ -1.288134396e-01, -1.151989042e-01, -1.566820250e-01, /*EDIAM*/  7.831536694e-02,  6.823181376e-02,  6.399188978e-02, /*DMEAN*/ -1.350582432e-01, /*EDMEAN*/  4.014993267e-02 },
            {421, /* SP "G2.5IVa           " idx */ 170, /*MAG*/  6.116000000e+00,  5.450000000e+00,  4.760000000e+00,  4.655000000e+00,  4.234000000e+00,  3.911000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  1.044031000e-02,  2.960000000e-01,  2.960000000e-01,  2.960000000e-01, /*DIAM*/ -2.377667551e-01, -2.012882101e-01, -1.407930142e-01, /*EDIAM*/  8.216773291e-02,  7.159075927e-02,  6.714272889e-02, /*DMEAN*/ -1.870842325e-01, /*EDMEAN*/  4.212073318e-02 },
            {422, /* SP "F7V               " idx */ 148, /*MAG*/  6.136000000e+00,  5.580000000e+00,  4.950000000e+00,  4.871000000e+00,  4.599000000e+00,  4.306000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  3.000000000e-03,  2.500000000e-01,  2.500000000e-01,  2.500000000e-01, /*DIAM*/ -3.147575787e-01, -2.948015561e-01, -2.417385126e-01, /*EDIAM*/  6.960785874e-02,  6.063875196e-02,  5.686862309e-02, /*DMEAN*/ -2.791796740e-01, /*EDMEAN*/  3.569267641e-02 },
            {423, /* SP "A7V               " idx */ 108, /*MAG*/  4.690000000e+00,  4.490000000e+00,  4.270000000e+00,  4.372000000e+00,  4.204000000e+00,  4.064000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  2.022375000e-02,  2.980000000e-01,  2.980000000e-01,  2.980000000e-01, /*DIAM*/ -2.954913329e-01, -2.679448625e-01, -2.501433754e-01, /*EDIAM*/  8.330578526e-02,  7.258586115e-02,  6.807727824e-02, /*DMEAN*/ -2.681691916e-01, /*EDMEAN*/  4.271775933e-02 },
            {424, /* SP "K0III             " idx */ 200, /*MAG*/  3.059000000e+00,  2.040000000e+00,  1.040000000e+00,  3.700000000e-01, -2.190000000e-01, -2.740000000e-01, /*EMAG*/  2.137756000e-02,  4.000000000e-02,  2.039608000e-02,  3.700000000e-01,  3.700000000e-01,  3.700000000e-01, /*DIAM*/  7.382370532e-01,  7.581889029e-01,  7.444330402e-01, /*EDIAM*/  1.025105465e-01,  8.932529537e-02,  8.377727216e-02, /*DMEAN*/  7.475590797e-01, /*EDMEAN*/  5.252256625e-02 },
            {425, /* SP "M1III             " idx */ 244, /*MAG*/  4.850000000e+00,  3.260000000e+00,  1.320000000e+00,  1.610000000e-01, -8.510000000e-01, -1.025000000e+00, /*EMAG*/  5.108816000e-02,  4.000000000e-02,  2.022375000e-02,  4.280000000e-01,  4.280000000e-01,  4.280000000e-01, /*DIAM*/  9.308456695e-01,  1.012688066e+00,  1.001595084e+00, /*EDIAM*/  1.185666015e-01,  1.033226720e-01,  9.690722499e-02, /*DMEAN*/  9.868804834e-01, /*EDMEAN*/  6.074527645e-02 },
            {426, /* SP "M4III             " idx */ 256, /*MAG*/  6.093000000e+00,  4.480000000e+00,  2.060000000e+00,  6.710000000e-01, -2.310000000e-01, -4.680000000e-01, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  2.118962000e-02,  3.340000000e-01,  3.340000000e-01,  3.340000000e-01, /*DIAM*/  8.637301827e-01,  9.229552615e-01,  9.213756610e-01, /*EDIAM*/  9.266416554e-02,  8.074423179e-02,  7.573047148e-02, /*DMEAN*/  9.068176472e-01, /*EDMEAN*/  4.750877023e-02 },
            {427, /* SP "M2Iab-Ib          " idx */ 248, /*MAG*/  6.380000000e+00,  4.320000000e+00,  1.780000000e+00,  3.930000000e-01, -6.090000000e-01, -9.130000000e-01, /*EMAG*/  2.915476000e-02,  4.000000000e-02,  3.195309000e-02,  2.200000000e-01,  2.200000000e-01,  2.200000000e-01, /*DIAM*/  9.437968102e-01,  1.001724960e+00,  1.008943385e+00, /*EDIAM*/  6.109348273e-02,  5.321181537e-02,  4.990136622e-02, /*DMEAN*/  9.894048670e-01, /*EDMEAN*/  3.133779163e-02 },
            {428, /* SP "A9II              " idx */ 116, /*MAG*/ -4.560000000e-01, -6.200000000e-01, -8.500000000e-01, -1.169000000e+00, -1.280000000e+00, -1.304000000e+00, /*EMAG*/  2.334524000e-02,  4.000000000e-02,  2.561250000e-02,  2.480000000e-01,  2.480000000e-01,  2.480000000e-01, /*DIAM*/  8.540521563e-01,  8.517282750e-01,  8.397966179e-01, /*EDIAM*/  6.946134615e-02,  6.051277558e-02,  5.675111234e-02, /*DMEAN*/  8.476491035e-01, /*EDMEAN*/  3.562698022e-02 },
            {429, /* SP "K3IIIa            " idx */ 212, /*MAG*/  3.430000000e+00,  1.990000000e+00,  6.000000000e-01, -2.560000000e-01, -9.900000000e-01, -1.127000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  2.039608000e-02,  2.080000000e-01,  2.080000000e-01,  2.080000000e-01, /*DIAM*/  9.282763675e-01,  9.560300821e-01,  9.502192315e-01, /*EDIAM*/  5.776634198e-02,  5.030857318e-02,  4.717625212e-02, /*DMEAN*/  9.464859747e-01, /*EDMEAN*/  2.962135542e-02 },
            {430, /* SP "M0.5III           " idx */ 242, /*MAG*/  4.314000000e+00,  2.730000000e+00,  9.100000000e-01, -1.310000000e-01, -1.025000000e+00, -1.173000000e+00, /*EMAG*/  1.044031000e-02,  4.000000000e-02,  2.022375000e-02,  2.140000000e-01,  2.140000000e-01,  2.140000000e-01, /*DIAM*/  9.723213017e-01,  1.030798722e+00,  1.018669583e+00, /*EDIAM*/  5.944144683e-02,  5.176994608e-02,  4.854794794e-02, /*DMEAN*/  1.010732825e+00, /*EDMEAN*/  3.048534119e-02 },
            {431, /* SP "K2III_Ba1         " idx */ 208, /*MAG*/  3.357000000e+00,  1.910000000e+00,  4.600000000e-01, -3.500000000e-01, -1.024000000e+00, -1.176000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  2.000000000e-03,  2.260000000e-01,  2.260000000e-01,  2.260000000e-01, /*DIAM*/  9.416066685e-01,  9.561398662e-01,  9.543493472e-01, /*EDIAM*/  6.272272210e-02,  5.463180527e-02,  5.123220446e-02, /*DMEAN*/  9.516329512e-01, /*EDMEAN*/  3.215736868e-02 },
            {432, /* SP "K3III             " idx */ 212, /*MAG*/  4.672000000e+00,  3.120000000e+00,  1.520000000e+00,  4.090000000e-01, -4.520000000e-01, -6.330000000e-01, /*EMAG*/  9.708244000e-02,  4.000000000e-02,  2.039608000e-02,  2.320000000e-01,  2.320000000e-01,  2.320000000e-01, /*DIAM*/  8.309817232e-01,  8.728472015e-01,  8.681316391e-01, /*EDIAM*/  6.438909341e-02,  5.608508037e-02,  5.259549463e-02, /*DMEAN*/  8.600369148e-01, /*EDMEAN*/  3.300935763e-02 },
            {433, /* SP "A5V               " idx */ 100, /*MAG*/  3.990000000e+00,  3.860000000e+00,  3.720000000e+00,  3.621000000e+00,  3.652000000e+00,  3.636000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  2.000000000e-03,  2.560000000e-01,  2.560000000e-01,  2.560000000e-01, /*DIAM*/ -1.468461112e-01, -1.710262056e-01, -1.818631146e-01, /*EDIAM*/  7.197074738e-02,  6.270504816e-02,  5.881027262e-02, /*DMEAN*/ -1.689524208e-01, /*EDMEAN*/  3.693613262e-02 },
            {434, /* SP "kA1VmA3V          " idx */ 84, /*MAG*/  4.473000000e+00,  4.420000000e+00,  4.390000000e+00,  4.315000000e+00,  4.324000000e+00,  4.315000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  3.014963000e-02,  2.280000000e-01,  2.280000000e-01,  2.280000000e-01, /*DIAM*/ -3.308413842e-01, -3.426235759e-01, -3.549201650e-01, /*EDIAM*/  6.449922186e-02,  5.620171963e-02,  5.271700836e-02, /*DMEAN*/ -3.443721246e-01, /*EDMEAN*/  3.318657798e-02 },
            {435, /* SP "A2IV              " idx */ 88, /*MAG*/  3.327000000e+00,  3.330000000e+00,  3.320000000e+00,  3.115000000e+00,  3.190000000e+00,  3.082000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  3.014963000e-02,  3.080000000e-01,  3.080000000e-01,  3.080000000e-01, /*DIAM*/ -7.154351324e-02, -1.040524241e-01, -9.453429989e-02, /*EDIAM*/  8.628848633e-02,  7.519379941e-02,  7.052825902e-02, /*DMEAN*/ -9.179824029e-02, /*EDMEAN*/  4.428847389e-02 },
            {436, /* SP "B9III             " idx */ 76, /*MAG*/  3.201000000e+00,  3.250000000e+00,  3.280000000e+00,  3.115000000e+00,  3.227000000e+00,  3.122000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  3.006659000e-02,  3.380000000e-01,  3.380000000e-01,  3.380000000e-01, /*DIAM*/ -1.157618351e-01, -1.505718996e-01, -1.393686716e-01, /*EDIAM*/  9.442753922e-02,  8.230225574e-02,  7.720332652e-02, /*DMEAN*/ -1.370519007e-01, /*EDMEAN*/  4.852257276e-02 },
            {437, /* SP "A0Ia              " idx */ 80, /*MAG*/  4.318000000e+00,  4.220000000e+00,  3.970000000e+00,  3.973000000e+00,  3.864000000e+00,  3.683000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  3.006659000e-02,  2.540000000e-01,  2.540000000e-01,  2.540000000e-01, /*DIAM*/ -2.641542961e-01, -2.512842719e-01, -2.283601014e-01, /*EDIAM*/  7.152312500e-02,  6.233080994e-02,  5.846882122e-02, /*DMEAN*/ -2.456477442e-01, /*EDMEAN*/  3.679992132e-02 },
            {438, /* SP "F1V               " idx */ 124, /*MAG*/  4.832000000e+00,  4.530000000e+00,  4.180000000e+00,  3.836000000e+00,  3.760000000e+00,  3.791000000e+00, /*EMAG*/  1.403567000e-02,  4.000000000e-02,  3.001666000e-02,  2.900000000e-01,  2.900000000e-01,  2.900000000e-01, /*DIAM*/ -1.290324635e-01, -1.463164341e-01, -1.700399375e-01, /*EDIAM*/  8.084175212e-02,  7.043503441e-02,  6.605817800e-02, /*DMEAN*/ -1.511068027e-01, /*EDMEAN*/  4.144174058e-02 },
            {439, /* SP "F6V               " idx */ 144, /*MAG*/  5.586000000e+00,  5.080000000e+00,  4.500000000e+00,  4.416000000e+00,  4.201000000e+00,  3.911000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  2.000000000e-03,  2.780000000e-01,  2.780000000e-01,  2.780000000e-01, /*DIAM*/ -2.310211616e-01, -2.209921638e-01, -1.680786914e-01, /*EDIAM*/  7.734089166e-02,  6.738186298e-02,  6.319402456e-02, /*DMEAN*/ -2.028351686e-01, /*EDMEAN*/  3.964785860e-02 },
            {440, /* SP "G7V               " idx */ 188, /*MAG*/  5.449000000e+00,  4.740000000e+00,  3.990000000e+00,  3.334000000e+00,  2.974000000e+00,  2.956000000e+00, /*EMAG*/              NaN,  4.000000000e-02,              NaN,  2.360000000e-01,  2.360000000e-01,  2.360000000e-01, /*DIAM*/  1.033230027e-01,  8.719243071e-02,  7.205449499e-02, /*EDIAM*/  6.551189405e-02,  5.706570743e-02,  5.351652869e-02, /*DMEAN*/  8.546662568e-02, /*EDMEAN*/  3.359311378e-02 },
            {441, /* SP "G0V               " idx */ 160, /*MAG*/  6.002000000e+00,  5.390000000e+00,  4.710000000e+00,  4.088000000e+00,  3.989000000e+00,  3.857000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  2.000000000e-03,  2.400000000e-01,  2.400000000e-01,  2.400000000e-01, /*DIAM*/ -9.919495878e-02, -1.502067740e-01, -1.372912418e-01, /*EDIAM*/  6.678466752e-02,  5.817701957e-02,  5.455971435e-02, /*DMEAN*/ -1.317791862e-01, /*EDMEAN*/  3.425150904e-02 },
            {442, /* SP "M2V               " idx */ 248, /*MAG*/  1.122200000e+01,  9.700000000e+00,  7.460000000e+00,  6.448000000e+00,  5.865000000e+00,  5.624000000e+00, /*EMAG*/  4.341659000e-02,  4.000000000e-02,  2.907594000e-01,  2.100000000e-02,  2.100000000e-02,  2.100000000e-02, /*DIAM*/ -3.190335657e-01, -3.381972385e-01, -3.288595543e-01, /*EDIAM*/  7.632746523e-03,  6.349895293e-03,  5.880144867e-03, /*DMEAN*/ -3.300586739e-01, /*EDMEAN*/  4.265491789e-03 },
            {443, /* SP "K0III             " idx */ 200, /*MAG*/  6.881000000e+00,  5.930000000e+00,  4.990000000e+00,  4.508000000e+00,  3.995000000e+00,  3.984000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  3.014963000e-02,  3.200000000e-01,  3.200000000e-01,  3.200000000e-01, /*DIAM*/ -1.084058166e-01, -9.797453401e-02, -1.168370456e-01, /*EDIAM*/  8.868684096e-02,  7.727312272e-02,  7.247193507e-02, /*DMEAN*/ -1.081148199e-01, /*EDMEAN*/  4.544642010e-02 },
            {444, /* SP "M2.5II-IIIe       " idx */ 250, /*MAG*/  4.095000000e+00,  2.440000000e+00,  1.300000000e-01, -1.432000000e+00, -2.129000000e+00, -2.379000000e+00, /*EMAG*/  1.280625000e-02,  4.000000000e-02,  6.053098000e-02,  1.940000000e-01,  1.940000000e-01,  1.940000000e-01, /*DIAM*/  1.301637406e+00,  1.292546689e+00,  1.293600276e+00, /*EDIAM*/  5.393675517e-02,  4.696986094e-02,  4.404598389e-02, /*DMEAN*/  1.295337826e+00, /*EDMEAN*/  2.768414382e-02 },
            {445, /* SP "M6III             " idx */ 264, /*MAG*/  6.306000000e+00,  4.950000000e+00,  1.700000000e+00, -2.130000000e-01, -1.197000000e+00, -1.526000000e+00, /*EMAG*/  1.360147000e-02,  4.000000000e-02,  3.104835000e-02,  2.480000000e-01,  2.480000000e-01,  2.480000000e-01, /*DIAM*/  1.118108264e+00,  1.178444324e+00,  1.180622493e+00, /*EDIAM*/  6.943194425e-02,  6.049988752e-02,  5.674765585e-02, /*DMEAN*/  1.163506290e+00, /*EDMEAN*/  3.569890328e-02 },
            {446, /* SP "G8IIIa            " idx */ 192, /*MAG*/  3.910000000e+00,  2.990000000e+00,  2.090000000e+00,  1.519000000e+00,  1.065000000e+00,  1.024000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  2.009975000e-02,  3.000000000e-01,  3.000000000e-01,  3.000000000e-01, /*DIAM*/  4.785619977e-01,  4.793300145e-01,  4.671911208e-01, /*EDIAM*/  8.317249252e-02,  7.246558918e-02,  6.796257944e-02, /*DMEAN*/  4.743614287e-01, /*EDMEAN*/  4.262719330e-02 },
            {447, /* SP "B1Ib              " idx */ 44, /*MAG*/  3.111000000e+00,  2.840000000e+00,  2.660000000e+00,  2.602000000e+00,  2.621000000e+00,  2.603000000e+00, /*EMAG*/  2.009975000e-02,  4.000000000e-02,  3.006659000e-02,  2.560000000e-01,  2.560000000e-01,  2.560000000e-01, /*DIAM*/ -2.246439957e-01, -2.000892345e-01, -1.920912881e-01, /*EDIAM*/  7.329603027e-02,  6.423825472e-02,  6.046297195e-02, /*DMEAN*/ -2.033692311e-01, /*EDMEAN*/  3.967498285e-02 },
            {448, /* SP "B2V               " idx */ 48, /*MAG*/  1.416000000e+00,  1.640000000e+00,  1.860000000e+00,  2.149000000e+00,  2.357000000e+00,  2.375000000e+00, /*EMAG*/  1.131371000e-02,  4.000000000e-02,  2.154066000e-02,  2.820000000e-01,  2.820000000e-01,  2.820000000e-01, /*DIAM*/ -1.513736636e-01, -1.546627524e-01, -1.451335799e-01, /*EDIAM*/  7.952310608e-02,  6.954587471e-02,  6.537253368e-02, /*DMEAN*/ -1.500577927e-01, /*EDMEAN*/  4.220417784e-02 },
            {449, /* SP "B9III             " idx */ 76, /*MAG*/  3.201000000e+00,  3.250000000e+00,  3.280000000e+00,  3.115000000e+00,  3.227000000e+00,  3.122000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  3.006659000e-02,  3.380000000e-01,  3.380000000e-01,  3.380000000e-01, /*DIAM*/ -1.157618351e-01, -1.505718996e-01, -1.393686716e-01, /*EDIAM*/  9.442753922e-02,  8.230225574e-02,  7.720332652e-02, /*DMEAN*/ -1.370519007e-01, /*EDMEAN*/  4.852257276e-02 },
            {450, /* SP "B3IV              " idx */ 52, /*MAG*/  4.590000000e+00,  4.740000000e+00,  4.860000000e+00,  4.971000000e+00,  5.093000000e+00,  5.114000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  3.006659000e-02,  2.000000000e-02,  2.000000000e-02,  2.000000000e-02, /*DIAM*/ -6.586041874e-01, -6.585119003e-01, -6.588062585e-01, /*EDIAM*/  1.395031756e-02,  1.306137876e-02,  1.278157052e-02, /*DMEAN*/ -6.586590041e-01, /*EDMEAN*/  1.188582913e-02 },
            {451, /* SP "M2III             " idx */ 248, /*MAG*/  6.362000000e+00,  4.790000000e+00,  2.860000000e+00,  1.757000000e+00,  8.530000000e-01,  7.240000000e-01, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  3.014963000e-02,  2.280000000e-01,  2.280000000e-01,  2.280000000e-01, /*DIAM*/  6.023503766e-01,  6.684097804e-01,  6.508776866e-01, /*EDIAM*/  6.330075082e-02,  5.513695022e-02,  5.170737221e-02, /*DMEAN*/  6.442330186e-01, /*EDMEAN*/  3.246650884e-02 },
            {452, /* SP "K1II+(K)          " idx */ 204, /*MAG*/  4.774000000e+00,  3.560000000e+00,  2.430000000e+00,  1.694000000e+00,  1.097000000e+00,  1.025000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  2.009975000e-02,  2.660000000e-01,  2.660000000e-01,  2.660000000e-01, /*DIAM*/  4.956413401e-01,  5.078678397e-01,  4.949697542e-01, /*EDIAM*/  7.376387194e-02,  6.426089699e-02,  6.026548931e-02, /*DMEAN*/  4.995996152e-01, /*EDMEAN*/  3.780774463e-02 },
            {453, /* SP "K0III             " idx */ 200, /*MAG*/  4.665000000e+00,  3.600000000e+00,  2.550000000e+00,  1.896000000e+00,  1.401000000e+00,  1.289000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  2.009975000e-02,  3.480000000e-01,  3.480000000e-01,  3.480000000e-01, /*DIAM*/  4.356477629e-01,  4.317141898e-01,  4.317542034e-01, /*EDIAM*/  9.642756541e-02,  8.402198285e-02,  7.880262214e-02, /*DMEAN*/  4.327606514e-01, /*EDMEAN*/  4.940865533e-02 },
            {454, /* SP "K3-III            " idx */ 212, /*MAG*/  4.865000000e+00,  3.590000000e+00,  2.360000000e+00,  1.420000000e+00,  7.410000000e-01,  6.490000000e-01, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  2.009975000e-02,  2.420000000e-01,  2.420000000e-01,  2.420000000e-01, /*DIAM*/  5.872406481e-01,  6.044269640e-01,  5.903944087e-01, /*EDIAM*/  6.714974545e-02,  5.849273900e-02,  5.485417869e-02, /*DMEAN*/  5.944147676e-01, /*EDMEAN*/  3.442184415e-02 },
            {455, /* SP "K5III             " idx */ 220, /*MAG*/  6.114000000e+00,  4.560000000e+00,  2.890000000e+00,  1.642000000e+00,  8.000000000e-01,  7.670000000e-01, /*EMAG*/  1.581139000e-02,  4.000000000e-02,  3.041381000e-02,  2.720000000e-01,  2.720000000e-01,  2.720000000e-01, /*DIAM*/  6.117100286e-01,  6.400682802e-01,  5.992699080e-01, /*EDIAM*/  7.545683171e-02,  6.573695540e-02,  6.164996989e-02, /*DMEAN*/  6.166162189e-01, /*EDMEAN*/  3.867297093e-02 },
            {456, /* SP "K3IIIa_Ba0.5      " idx */ 212, /*MAG*/  6.025000000e+00,  4.470000000e+00,  2.860000000e+00,  1.673000000e+00,  8.210000000e-01,  8.770000000e-01, /*EMAG*/  2.039608000e-02,  4.000000000e-02,  1.077033000e-02,  2.660000000e-01,  2.660000000e-01,  2.660000000e-01, /*DIAM*/  5.847852910e-01,  6.214230732e-01,  5.619272550e-01, /*EDIAM*/  7.377751322e-02,  6.427258621e-02,  6.027627207e-02, /*DMEAN*/  5.884581528e-01, /*EDMEAN*/  3.781335177e-02 },
            {457, /* SP "A9Ia              " idx */ 116, /*MAG*/  3.567000000e+00,  3.030000000e+00,  2.420000000e+00,  1.880000000e+00,  1.702000000e+00,  1.533000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  1.118034000e-02,  2.980000000e-01,  2.980000000e-01,  2.980000000e-01, /*DIAM*/  2.904003621e-01,  2.828800175e-01,  2.937601134e-01, /*EDIAM*/  8.316453941e-02,  7.246126250e-02,  6.795946059e-02, /*DMEAN*/  2.891230232e-01, /*EDMEAN*/  4.263623025e-02 },
            {458, /* SP "M0III             " idx */ 240, /*MAG*/  6.341000000e+00,  4.720000000e+00,  2.820000000e+00,  1.748000000e+00,  8.650000000e-01,  6.770000000e-01, /*EMAG*/  1.044031000e-02,  4.000000000e-02,  3.014963000e-02,  2.780000000e-01,  2.780000000e-01,  2.780000000e-01, /*DIAM*/  6.059248388e-01,  6.548268600e-01,  6.498239321e-01, /*EDIAM*/  7.711261369e-02,  6.718089782e-02,  6.300493379e-02, /*DMEAN*/  6.400546913e-01, /*EDMEAN*/  3.952460224e-02 },
            {459, /* SP "K1III             " idx */ 204, /*MAG*/  5.309000000e+00,  3.990000000e+00,  2.720000000e+00,  1.841000000e+00,  1.193000000e+00,  1.087000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  2.022375000e-02,  2.940000000e-01,  2.940000000e-01,  2.940000000e-01, /*DIAM*/  4.879716972e-01,  5.024437151e-01,  4.922398272e-01, /*EDIAM*/  8.150066734e-02,  7.100722896e-02,  6.659405310e-02, /*DMEAN*/  4.946453556e-01, /*EDMEAN*/  4.176720680e-02 },
            {460, /* SP "G8Ib              " idx */ 192, /*MAG*/  4.437000000e+00,  3.060000000e+00,  1.840000000e+00,  8.860000000e-01,  2.320000000e-01,  1.250000000e-01, /*EMAG*/  1.513275000e-02,  4.000000000e-02,  1.019804000e-02,  2.220000000e-01,  2.220000000e-01,  2.220000000e-01, /*DIAM*/  6.591423576e-01,  6.831743755e-01,  6.724538976e-01, /*EDIAM*/  6.163113920e-02,  5.368074515e-02,  5.034077915e-02, /*DMEAN*/  6.726728864e-01, /*EDMEAN*/  3.160585076e-02 },
            {461, /* SP "K2III             " idx */ 208, /*MAG*/  5.671000000e+00,  4.410000000e+00,  3.160000000e+00,  2.377000000e+00,  1.821000000e+00,  1.681000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  3.014963000e-02,  3.120000000e-01,  3.120000000e-01,  3.120000000e-01, /*DIAM*/  3.787763030e-01,  3.729102855e-01,  3.735683166e-01, /*EDIAM*/  8.647974683e-02,  7.534849302e-02,  7.066631002e-02, /*DMEAN*/  3.747054750e-01, /*EDMEAN*/  4.431485080e-02 },
            {462, /* SP "G8III-IIIb        " idx */ 192, /*MAG*/  4.502000000e+00,  3.570000000e+00,  2.670000000e+00,  2.051000000e+00,  1.642000000e+00,  1.527000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  3.000000000e-03,  3.180000000e-01,  3.180000000e-01,  3.180000000e-01, /*DIAM*/  3.758477105e-01,  3.640537482e-01,  3.686144770e-01, /*EDIAM*/  8.814690478e-02,  7.680279732e-02,  7.203108435e-02, /*DMEAN*/  3.689346681e-01, /*EDMEAN*/  4.517303694e-02 },
            {463, /* SP "K0III             " idx */ 200, /*MAG*/  2.151000000e+00,  1.160000000e+00,  1.900000000e-01, -3.150000000e-01, -8.450000000e-01, -9.360000000e-01, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  1.166190000e-02,  2.320000000e-01,  2.320000000e-01,  2.320000000e-01, /*DIAM*/  8.602638407e-01,  8.729126400e-01,  8.711045749e-01, /*EDIAM*/  6.437338249e-02,  5.607183299e-02,  5.258345587e-02, /*DMEAN*/  8.688916465e-01, /*EDMEAN*/  3.300440249e-02 },
            {464, /* SP "K2III             " idx */ 208, /*MAG*/  5.642000000e+00,  4.390000000e+00,  3.120000000e+00,  2.199000000e+00,  1.553000000e+00,  1.447000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  2.022375000e-02,  3.080000000e-01,  3.080000000e-01,  3.080000000e-01, /*DIAM*/  4.265084466e-01,  4.367390803e-01,  4.259916751e-01, /*EDIAM*/  8.537419396e-02,  7.438455047e-02,  6.976207982e-02, /*DMEAN*/  4.298382095e-01, /*EDMEAN*/  4.374897577e-02 },
            {465, /* SP "K4III_Ba0.5       " idx */ 216, /*MAG*/  5.011000000e+00,  3.530000000e+00,  2.060000000e+00,  1.191000000e+00,  2.670000000e-01,  1.900000000e-01, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  2.022375000e-02,  2.500000000e-01,  2.500000000e-01,  2.500000000e-01, /*DIAM*/  6.520592649e-01,  7.212039170e-01,  6.976681409e-01, /*EDIAM*/  6.937044661e-02,  6.042937434e-02,  5.667093567e-02, /*DMEAN*/  6.938550807e-01, /*EDMEAN*/  3.555807851e-02 },
            {466, /* SP "G9II-III          " idx */ 196, /*MAG*/  4.088000000e+00,  3.110000000e+00,  2.150000000e+00,  1.415000000e+00,  7.580000000e-01,  6.970000000e-01, /*EMAG*/  4.909175000e-02,  4.000000000e-02,  2.022375000e-02,  2.340000000e-01,  2.340000000e-01,  2.340000000e-01, /*DIAM*/  5.238730657e-01,  5.623680372e-01,  5.484892631e-01, /*EDIAM*/  6.493145966e-02,  5.655883361e-02,  5.304053881e-02, /*DMEAN*/  5.468391754e-01, /*EDMEAN*/  3.329179356e-02 },
            {467, /* SP "K4.5III           " idx */ 218, /*MAG*/  5.861000000e+00,  4.320000000e+00,  2.690000000e+00,  1.449000000e+00,  6.100000000e-01,  5.860000000e-01, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  3.014963000e-02,  2.680000000e-01,  2.680000000e-01,  2.680000000e-01, /*DIAM*/  6.440960689e-01,  6.735182473e-01,  6.313585507e-01, /*EDIAM*/  7.434667705e-02,  6.476888273e-02,  6.074182237e-02, /*DMEAN*/  6.492527551e-01, /*EDMEAN*/  3.810466467e-02 },
            {468, /* SP "K2III             " idx */ 208, /*MAG*/  5.213000000e+00,  3.900000000e+00,  2.610000000e+00,  1.680000000e+00,  9.530000000e-01,  8.740000000e-01, /*EMAG*/  2.507987000e-02,  4.000000000e-02,  2.009975000e-02,  2.920000000e-01,  2.920000000e-01,  2.920000000e-01, /*DIAM*/  5.325352338e-01,  5.612760471e-01,  5.427726987e-01, /*EDIAM*/  8.095241705e-02,  7.052906415e-02,  6.614539954e-02, /*DMEAN*/  5.464805829e-01, /*EDMEAN*/  4.148578631e-02 },
            {469, /* SP "M2III             " idx */ 248, /*MAG*/  6.970000000e+00,  5.360000000e+00,  3.420000000e+00,  2.041000000e+00,  1.105000000e+00,  1.090000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  4.031129000e-02,  2.560000000e-01,  2.560000000e-01,  2.560000000e-01, /*DIAM*/  5.675110903e-01,  6.311257332e-01,  5.830382695e-01, /*EDIAM*/  7.102962935e-02,  6.187729013e-02,  5.803046066e-02, /*DMEAN*/  5.955789860e-01, /*EDMEAN*/  3.641961516e-02 },
            {470, /* SP "G1II              " idx */ 164, /*MAG*/  3.778000000e+00,  2.970000000e+00,  2.160000000e+00,  1.511000000e+00,  1.107000000e+00,  1.223000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  2.022375000e-02,  2.640000000e-01,  2.640000000e-01,  2.640000000e-01, /*DIAM*/  4.334795790e-01,  4.473114702e-01,  3.978940530e-01, /*EDIAM*/  7.337352888e-02,  6.392273612e-02,  5.994970826e-02, /*DMEAN*/  4.242772802e-01, /*EDMEAN*/  3.762192238e-02 },
            {471, /* SP "K2IIIb_CN1_Ca1    " idx */ 208, /*MAG*/  5.102000000e+00,  3.880000000e+00,  2.750000000e+00,  1.974000000e+00,  1.327000000e+00,  1.364000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  3.006659000e-02,  2.380000000e-01,  2.380000000e-01,  2.380000000e-01, /*DIAM*/  4.496245183e-01,  4.702254621e-01,  4.313712372e-01, /*EDIAM*/  6.603564329e-02,  5.752119065e-02,  5.394282562e-02, /*DMEAN*/  4.495669692e-01, /*EDMEAN*/  3.385232474e-02 },
            {472, /* SP "K0III             " idx */ 200, /*MAG*/  4.830000000e+00,  3.790000000e+00,  2.720000000e+00,  2.070000000e+00,  1.426000000e+00,  1.390000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  2.009975000e-02,  1.900000000e-01,  1.900000000e-01,  1.900000000e-01, /*DIAM*/  4.020763339e-01,  4.335196373e-01,  4.138928893e-01, /*EDIAM*/  5.278343382e-02,  4.596224078e-02,  4.309901816e-02, /*DMEAN*/  4.175820833e-01, /*EDMEAN*/  2.707662782e-02 },
            {473, /* SP "K1III             " idx */ 204, /*MAG*/  4.144000000e+00,  3.000000000e+00,  1.910000000e+00,  1.030000000e+00,  3.760000000e-01,  4.290000000e-01, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  3.014963000e-02,  1.980000000e-01,  1.980000000e-01,  1.980000000e-01, /*DIAM*/  6.364270565e-01,  6.587083088e-01,  6.151157414e-01, /*EDIAM*/  5.498999612e-02,  4.788696345e-02,  4.490457786e-02, /*DMEAN*/  6.357446889e-01, /*EDMEAN*/  2.820317063e-02 },
            {474, /* SP "M3IIb             " idx */ 252, /*MAG*/  6.217000000e+00,  4.560000000e+00,  2.290000000e+00,  1.006000000e+00,  1.550000000e-01, -7.400000000e-02, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  3.026549000e-02,  2.240000000e-01,  2.240000000e-01,  2.240000000e-01, /*DIAM*/  7.860942520e-01,  8.305175118e-01,  8.300095659e-01, /*EDIAM*/  6.223043672e-02,  5.420497527e-02,  5.083406779e-02, /*DMEAN*/  8.186937411e-01, /*EDMEAN*/  3.192949492e-02 },
            {475, /* SP "K3III_Ba0.3       " idx */ 212, /*MAG*/  4.890000000e+00,  3.490000000e+00,  2.120000000e+00,  1.073000000e+00,  1.830000000e-01,  2.820000000e-01, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  3.006659000e-02,  1.900000000e-01,  1.900000000e-01,  1.900000000e-01, /*DIAM*/  6.756067209e-01,  7.349172384e-01,  6.708104683e-01, /*EDIAM*/  5.280259333e-02,  4.597840105e-02,  4.311370537e-02, /*DMEAN*/  6.942056675e-01, /*EDMEAN*/  2.708266758e-02 },
            {476, /* SP "M0III-IIIa_Ca1    " idx */ 240, /*MAG*/  5.433000000e+00,  3.820000000e+00,  2.030000000e+00,  8.950000000e-01, -4.900000000e-02, -1.140000000e-01, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  3.014963000e-02,  2.120000000e-01,  2.120000000e-01,  2.120000000e-01, /*DIAM*/  7.729159127e-01,  8.382042947e-01,  8.051597009e-01, /*EDIAM*/  5.889632519e-02,  5.129435093e-02,  4.810163044e-02, /*DMEAN*/  8.081366565e-01, /*EDMEAN*/  3.020519138e-02 },
            {477, /* SP "M1III             " idx */ 244, /*MAG*/  5.541000000e+00,  4.040000000e+00,  2.250000000e+00,  1.176000000e+00,  3.250000000e-01,  1.570000000e-01, /*EMAG*/  3.224903000e-02,  4.000000000e-02,  2.039608000e-02,  2.600000000e-01,  2.600000000e-01,  2.600000000e-01, /*DIAM*/  7.098010234e-01,  7.611549882e-01,  7.546315769e-01, /*EDIAM*/  7.213259049e-02,  6.283849576e-02,  5.893167301e-02, /*DMEAN*/  7.451461325e-01, /*EDMEAN*/  3.697896716e-02 },
            {478, /* SP "G0V               " idx */ 160, /*MAG*/  4.828000000e+00,  4.240000000e+00,  3.570000000e+00,  3.213000000e+00,  2.905000000e+00,  2.848000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  1.019804000e-02,  3.100000000e-01,  3.100000000e-01,  3.100000000e-01, /*DIAM*/  5.468897208e-02,  6.387105020e-02,  6.080365164e-02, /*EDIAM*/  8.607955342e-02,  7.500156906e-02,  7.034215080e-02, /*DMEAN*/  6.026096212e-02, /*EDMEAN*/  4.412238850e-02 },
            {479, /* SP "G8III             " idx */ 192, /*MAG*/  3.784000000e+00,  2.850000000e+00,  2.020000000e+00,  1.248000000e+00,  7.330000000e-01,  6.640000000e-01, /*EMAG*/  1.513275000e-02,  4.000000000e-02,  1.019804000e-02,  2.780000000e-01,  2.780000000e-01,  2.780000000e-01, /*DIAM*/  5.428209273e-01,  5.536490817e-01,  5.449721438e-01, /*EDIAM*/  7.709401116e-02,  6.716547032e-02,  6.299075004e-02, /*DMEAN*/  5.474050435e-01, /*EDMEAN*/  3.951660736e-02 },
            {480, /* SP "K5III-IV          " idx */ 220, /*MAG*/  6.282000000e+00,  4.800000000e+00,  3.250000000e+00,  2.372000000e+00,  1.631000000e+00,  1.493000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  3.006659000e-02,  2.720000000e-01,  2.720000000e-01,  2.720000000e-01, /*DIAM*/  4.280850258e-01,  4.494924019e-01,  4.412991027e-01, /*EDIAM*/  7.545683171e-02,  6.573695540e-02,  6.164996989e-02, /*DMEAN*/  4.406679054e-01, /*EDMEAN*/  3.867297093e-02 },
            {481, /* SP "M2III             " idx */ 248, /*MAG*/  6.286000000e+00,  4.680000000e+00,  2.620000000e+00,  1.455000000e+00,  6.200000000e-01,  4.270000000e-01, /*EMAG*/  3.522783000e-02,  4.000000000e-02,  2.039608000e-02,  2.820000000e-01,  2.820000000e-01,  2.820000000e-01, /*DIAM*/  6.774932348e-01,  7.200829329e-01,  7.151915562e-01, /*EDIAM*/  7.821018821e-02,  6.813875289e-02,  6.390414869e-02, /*DMEAN*/  7.070074661e-01, /*EDMEAN*/  4.009318368e-02 },
            {482, /* SP "K5.5III           " idx */ 222, /*MAG*/  5.570000000e+00,  4.050000000e+00,  2.450000000e+00,  1.218000000e+00,  4.380000000e-01,  4.350000000e-01, /*EMAG*/  1.236932000e-02,  4.000000000e-02,  3.014963000e-02,  2.640000000e-01,  2.640000000e-01,  2.640000000e-01, /*DIAM*/  6.923095599e-01,  7.088553662e-01,  6.635736892e-01, /*EDIAM*/  7.325288413e-02,  6.381508509e-02,  5.984710592e-02, /*DMEAN*/  6.867351453e-01, /*EDMEAN*/  3.754533192e-02 },
            {483, /* SP "M3III             " idx */ 252, /*MAG*/  6.152000000e+00,  4.580000000e+00,  2.230000000e+00,  8.560000000e-01, -5.400000000e-02, -2.400000000e-01, /*EMAG*/  1.077033000e-02,  4.000000000e-02,  3.026549000e-02,  2.300000000e-01,  2.300000000e-01,  2.300000000e-01, /*DIAM*/  8.291478241e-01,  8.817626487e-01,  8.680971577e-01, /*EDIAM*/  6.388516098e-02,  5.564812669e-02,  5.218788358e-02, /*DMEAN*/  8.626234290e-01, /*EDMEAN*/  3.277542960e-02 },
            {484, /* SP "K5III             " idx */ 220, /*MAG*/  6.371000000e+00,  4.760000000e+00,  3.130000000e+00,  1.045000000e+00,  1.260000000e-01, -9.000000000e-03, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  1.118034000e-02,  2.320000000e-01,  2.320000000e-01,  2.320000000e-01, /*DIAM*/  7.923082456e-01,  8.109165318e-01,  7.801166260e-01, /*EDIAM*/  6.441482128e-02,  5.610749631e-02,  5.261651447e-02, /*DMEAN*/  7.939439979e-01, /*EDMEAN*/  3.302276999e-02 },
            {485, /* SP "K3III             " idx */ 212, /*MAG*/  4.868000000e+00,  3.570000000e+00,  2.350000000e+00,  1.501000000e+00,  7.620000000e-01,  7.560000000e-01, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  3.006659000e-02,  2.640000000e-01,  2.640000000e-01,  2.640000000e-01, /*DIAM*/  5.632852906e-01,  5.985359134e-01,  5.656571820e-01, /*EDIAM*/  7.322509479e-02,  6.379086276e-02,  5.982437162e-02, /*DMEAN*/  5.763898810e-01, /*EDMEAN*/  3.753065310e-02 },
            {486, /* SP "M3III             " idx */ 252, /*MAG*/  6.472000000e+00,  4.800000000e+00,  2.670000000e+00,  1.477000000e+00,  5.360000000e-01,  3.810000000e-01, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  3.041381000e-02,  2.760000000e-01,  2.760000000e-01,  2.760000000e-01, /*DIAM*/  6.741567503e-01,  7.485019463e-01,  7.333599294e-01, /*EDIAM*/  7.657999629e-02,  6.671850507e-02,  6.257267853e-02, /*DMEAN*/  7.230846875e-01, /*EDMEAN*/  3.926750706e-02 },
            {487, /* SP "G8IIIaBa0.5       " idx */ 192, /*MAG*/  4.446000000e+00,  3.490000000e+00,  2.600000000e+00,  1.795000000e+00,  1.268000000e+00,  1.223000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  3.006659000e-02,  3.140000000e-01,  3.140000000e-01,  3.140000000e-01, /*DIAM*/  4.405619972e-01,  4.509798195e-01,  4.353006094e-01, /*EDIAM*/  8.704140341e-02,  7.583892181e-02,  7.112692776e-02, /*DMEAN*/  4.420931149e-01, /*EDMEAN*/  4.460723749e-02 },
            {488, /* SP "K4III             " idx */ 216, /*MAG*/  6.306000000e+00,  4.800000000e+00,  3.260000000e+00,  2.256000000e+00,  1.512000000e+00,  1.339000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  3.006659000e-02,  2.880000000e-01,  2.880000000e-01,  2.880000000e-01, /*DIAM*/  4.548003333e-01,  4.732350417e-01,  4.710476996e-01, /*EDIAM*/  7.986538955e-02,  6.958113267e-02,  6.525609055e-02, /*DMEAN*/  4.675474338e-01, /*EDMEAN*/  4.092894898e-02 },
            {489, /* SP "K4III             " idx */ 216, /*MAG*/  6.389000000e+00,  5.020000000e+00,  3.680000000e+00,  2.876000000e+00,  2.091000000e+00,  1.939000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  3.000000000e-03,  2.700000000e-01,  2.700000000e-01,  2.700000000e-01, /*DIAM*/  3.000860453e-01,  3.426280359e-01,  3.410622962e-01, /*EDIAM*/  7.489329940e-02,  6.524554478e-02,  6.118896967e-02, /*DMEAN*/  3.308723379e-01, /*EDMEAN*/  3.838430237e-02 },
            {490, /* SP "K2III             " idx */ 208, /*MAG*/  4.456000000e+00,  3.290000000e+00,  2.220000000e+00,  1.293000000e+00,  7.240000000e-01,  6.710000000e-01, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  3.006659000e-02,  2.200000000e-01,  2.200000000e-01,  2.200000000e-01, /*DIAM*/  5.928120205e-01,  5.913616507e-01,  5.726778087e-01, /*EDIAM*/  6.106660810e-02,  5.318733919e-02,  4.987708591e-02, /*DMEAN*/  5.843987209e-01, /*EDMEAN*/  3.131013787e-02 },
            {491, /* SP "K2IIIb            " idx */ 208, /*MAG*/  3.797000000e+00,  2.630000000e+00,  1.540000000e+00,  8.310000000e-01,  2.890000000e-01,  1.500000000e-01, /*EMAG*/  1.529706000e-02,  4.000000000e-02,  2.022375000e-02,  2.980000000e-01,  2.980000000e-01,  2.980000000e-01, /*DIAM*/  6.700084502e-01,  6.690814962e-01,  6.732252554e-01, /*EDIAM*/  8.261049809e-02,  7.197481580e-02,  6.750160743e-02, /*DMEAN*/  6.709517350e-01, /*EDMEAN*/  4.233442139e-02 },
            {492, /* SP "K2III             " idx */ 208, /*MAG*/  5.371000000e+00,  4.140000000e+00,  2.970000000e+00,  2.104000000e+00,  1.488000000e+00,  1.349000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  3.006659000e-02,  2.600000000e-01,  2.600000000e-01,  2.600000000e-01, /*DIAM*/  4.336066609e-01,  4.421087301e-01,  4.415975147e-01, /*EDIAM*/  7.211136498e-02,  6.281972594e-02,  5.891342378e-02, /*DMEAN*/  4.396817048e-01, /*EDMEAN*/  3.696119492e-02 },
            {493, /* SP "G8III-IV          " idx */ 192, /*MAG*/  3.640000000e+00,  2.730000000e+00,  1.890000000e+00,  1.081000000e+00,  5.840000000e-01,  5.790000000e-01, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  3.006659000e-02,  2.300000000e-01,  2.300000000e-01,  2.300000000e-01, /*DIAM*/  5.798298564e-01,  5.846451911e-01,  5.610524361e-01, /*EDIAM*/  6.383901841e-02,  5.560639135e-02,  5.214727684e-02, /*DMEAN*/  5.741144133e-01, /*EDMEAN*/  3.273514642e-02 },
            {494, /* SP "G7IIIFe-1         " idx */ 188, /*MAG*/  4.396000000e+00,  3.480000000e+00,  2.590000000e+00,  1.768000000e+00,  1.366000000e+00,  1.333000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  3.006659000e-02,  3.040000000e-01,  3.040000000e-01,  3.040000000e-01, /*DIAM*/  4.400194363e-01,  4.231457431e-01,  4.061931861e-01, /*EDIAM*/  8.429080363e-02,  7.344092945e-02,  6.887768881e-02, /*DMEAN*/  4.209082094e-01, /*EDMEAN*/  4.320126511e-02 },
            {495, /* SP "K3II              " idx */ 212, /*MAG*/  4.597000000e+00,  3.160000000e+00,  1.850000000e+00,  7.240000000e-01, -1.120000000e-01, -2.300000000e-02, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  2.000000000e-03,  1.780000000e-01,  1.780000000e-01,  1.780000000e-01, /*DIAM*/  7.468656505e-01,  7.924736594e-01,  7.311535348e-01, /*EDIAM*/  4.949548106e-02,  4.309298820e-02,  4.040650933e-02, /*DMEAN*/  7.564394820e-01, /*EDMEAN*/  2.539164088e-02 },
            {496, /* SP "K2III             " idx */ 208, /*MAG*/  3.928000000e+00,  2.760000000e+00,  1.660000000e+00,  1.184000000e+00,  4.670000000e-01,  4.370000000e-01, /*EMAG*/  2.209072000e-02,  4.000000000e-02,  2.009975000e-02,  2.360000000e-01,  2.360000000e-01,  2.360000000e-01, /*DIAM*/  5.822852346e-01,  6.315017291e-01,  6.116997071e-01, /*EDIAM*/  6.548342990e-02,  5.703958729e-02,  5.349102227e-02, /*DMEAN*/  6.108389441e-01, /*EDMEAN*/  3.356978936e-02 },
            {497, /* SP "K1III             " idx */ 204, /*MAG*/  4.307000000e+00,  3.320000000e+00,  2.370000000e+00,  1.732000000e+00,  1.289000000e+00,  1.225000000e+00, /*EMAG*/  3.512834000e-02,  4.000000000e-02,  2.022375000e-02,  3.060000000e-01,  3.060000000e-01,  3.060000000e-01, /*DIAM*/  4.666949111e-01,  4.516499400e-01,  4.434077096e-01, /*EDIAM*/  8.481717486e-02,  7.389899125e-02,  6.930669761e-02, /*DMEAN*/  4.523542751e-01, /*EDMEAN*/  4.346465663e-02 },
            {498, /* SP "K1.5IIIFe-1       " idx */ 206, /*MAG*/  5.999000000e+00,  4.820000000e+00,  3.660000000e+00,  2.949000000e+00,  2.197000000e+00,  2.085000000e+00, /*EMAG*/  1.315295000e-02,  4.000000000e-02,  3.006659000e-02,  2.340000000e-01,  2.340000000e-01,  2.340000000e-01, /*DIAM*/  2.485189746e-01,  2.967692069e-01,  2.905551432e-01, /*EDIAM*/  6.492782687e-02,  5.655508907e-02,  5.303656041e-02, /*DMEAN*/  2.816986347e-01, /*EDMEAN*/  3.328597657e-02 },
            {499, /* SP "G0Ib/II           " idx */ 160, /*MAG*/  5.307000000e+00,  4.220000000e+00,  3.130000000e+00,  2.210000000e+00,  1.714000000e+00,  1.623000000e+00, /*EMAG*/  3.811824000e-02,  4.000000000e-02,  2.022375000e-02,  2.920000000e-01,  2.920000000e-01,  2.920000000e-01, /*DIAM*/  3.307693333e-01,  3.503691089e-01,  3.374678893e-01, /*EDIAM*/  8.111436360e-02,  7.067240390e-02,  6.628121369e-02, /*DMEAN*/  3.401682634e-01, /*EDMEAN*/  4.158180571e-02 },
            {500, /* SP "K0III             " idx */ 200, /*MAG*/  5.971000000e+00,  4.820000000e+00,  3.720000000e+00,  3.260000000e+00,  2.547000000e+00,  2.321000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  3.006659000e-02,  2.860000000e-01,  2.860000000e-01,  2.860000000e-01, /*DIAM*/  1.517906159e-01,  2.055663265e-01,  2.302943464e-01, /*EDIAM*/  7.928982609e-02,  6.907965606e-02,  6.478600726e-02, /*DMEAN*/  2.011937488e-01, /*EDMEAN*/  4.063693271e-02 },
            {501, /* SP "K1III             " idx */ 204, /*MAG*/  5.099000000e+00,  4.020000000e+00,  2.940000000e+00,  2.245000000e+00,  1.756000000e+00,  1.636000000e+00, /*EMAG*/  3.805260000e-02,  4.000000000e-02,  2.009975000e-02,  3.480000000e-01,  3.480000000e-01,  3.480000000e-01, /*DIAM*/  3.784538384e-01,  3.678600555e-01,  3.688018691e-01, /*EDIAM*/  9.642761894e-02,  8.402188364e-02,  7.880240928e-02, /*DMEAN*/  3.710058571e-01, /*EDMEAN*/  4.940767460e-02 },
            {502, /* SP "K2III             " idx */ 208, /*MAG*/  6.156000000e+00,  5.000000000e+00,  3.880000000e+00,  2.886000000e+00,  2.392000000e+00,  2.145000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  2.000000000e-03,  3.140000000e-01,  3.140000000e-01,  3.140000000e-01, /*DIAM*/  2.831959444e-01,  2.594939414e-01,  2.840792642e-01, /*EDIAM*/  8.703253843e-02,  7.583047420e-02,  7.111843351e-02, /*DMEAN*/  2.753584047e-01, /*EDMEAN*/  4.459779914e-02 },
            {503, /* SP "G9III             " idx */ 196, /*MAG*/  4.750000000e+00,  3.800000000e+00,  2.950000000e+00,  2.319000000e+00,  1.823000000e+00,  1.757000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  3.006659000e-02,  2.660000000e-01,  2.660000000e-01,  2.660000000e-01, /*DIAM*/  3.266409199e-01,  3.339011077e-01,  3.267666320e-01, /*EDIAM*/  7.376896185e-02,  6.426576920e-02,  6.027040672e-02, /*DMEAN*/  3.291974076e-01, /*EDMEAN*/  3.781332794e-02 },
            {504, /* SP "M0III             " idx */ 240, /*MAG*/  5.942000000e+00,  4.440000000e+00,  2.760000000e+00,  1.714000000e+00,  8.780000000e-01,  7.110000000e-01, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  2.009975000e-02,  2.980000000e-01,  2.980000000e-01,  2.980000000e-01, /*DIAM*/  5.938355529e-01,  6.401420349e-01,  6.347728370e-01, /*EDIAM*/  8.263719617e-02,  7.199811096e-02,  6.752379109e-02, /*DMEAN*/  6.259036159e-01, /*EDMEAN*/  4.235182537e-02 },
            {505, /* SP "K3II              " idx */ 212, /*MAG*/  4.227000000e+00,  2.720000000e+00,  1.280000000e+00,  2.760000000e-01, -5.440000000e-01, -7.200000000e-01, /*EMAG*/  1.414214000e-02,  4.000000000e-02,  2.009975000e-02,  2.440000000e-01,  2.440000000e-01,  2.440000000e-01, /*DIAM*/  8.370799376e-01,  8.785436996e-01,  8.773068217e-01, /*EDIAM*/  6.770194646e-02,  5.897431774e-02,  5.530595562e-02, /*DMEAN*/  8.672032707e-01, /*EDMEAN*/  3.470439080e-02 },
            {506, /* SP "K0.5IIIb          " idx */ 202, /*MAG*/  5.188000000e+00,  4.080000000e+00,  3.030000000e+00,  2.372000000e+00,  1.931000000e+00,  1.752000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  3.006659000e-02,  2.700000000e-01,  2.700000000e-01,  2.700000000e-01, /*DIAM*/  3.443550270e-01,  3.258599602e-01,  3.418433140e-01, /*EDIAM*/  7.486829450e-02,  6.522405616e-02,  6.116908715e-02, /*DMEAN*/  3.369816858e-01, /*EDMEAN*/  3.837352055e-02 },
            {507, /* SP "G0Ib              " idx */ 160, /*MAG*/  3.728000000e+00,  2.900000000e+00,  2.080000000e+00,  1.548000000e+00,  1.265000000e+00,  1.212000000e+00, /*EMAG*/  1.019804000e-02,  4.000000000e-02,  2.009975000e-02,  2.840000000e-01,  2.840000000e-01,  2.840000000e-01, /*DIAM*/  4.126443346e-01,  4.042445961e-01,  3.957817588e-01, /*EDIAM*/  7.890829856e-02,  6.874886685e-02,  6.447684093e-02, /*DMEAN*/  4.031206391e-01, /*EDMEAN*/  4.045310119e-02 },
            {508, /* SP "K1III             " idx */ 204, /*MAG*/  5.658000000e+00,  4.550000000e+00,  3.480000000e+00,  2.462000000e+00,  2.002000000e+00,  1.722000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  3.006659000e-02,  2.080000000e-01,  2.080000000e-01,  2.080000000e-01, /*DIAM*/  3.590877667e-01,  3.303736736e-01,  3.632690223e-01, /*EDIAM*/  5.774891872e-02,  5.029363855e-02,  4.716247441e-02, /*DMEAN*/  3.508144810e-01, /*EDMEAN*/  2.961419723e-02 },
            {509, /* SP "G2Ib              " idx */ 168, /*MAG*/  3.919000000e+00,  2.950000000e+00,  2.030000000e+00,  1.278000000e+00,  7.400000000e-01,  5.900000000e-01, /*EMAG*/  1.170470000e-02,  4.000000000e-02,  2.039608000e-02,  2.800000000e-01,  2.800000000e-01,  2.800000000e-01, /*DIAM*/  5.020347841e-01,  5.372913779e-01,  5.434858686e-01, /*EDIAM*/  7.776365917e-02,  6.775075270e-02,  6.354062897e-02, /*DMEAN*/  5.304906743e-01, /*EDMEAN*/  3.986767401e-02 },
            {510, /* SP "K3II-III          " idx */ 212, /*MAG*/  5.587000000e+00,  4.140000000e+00,  2.810000000e+00,  1.812000000e+00,  1.112000000e+00,  1.012000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  3.014963000e-02,  2.520000000e-01,  2.520000000e-01,  2.520000000e-01, /*DIAM*/  5.209727900e-01,  5.376098424e-01,  5.227082763e-01, /*EDIAM*/  6.991096726e-02,  6.090077697e-02,  5.711318651e-02, /*DMEAN*/  5.273998489e-01, /*EDMEAN*/  3.583472893e-02 },
            {511, /* SP "G9IIIbCa1         " idx */ 196, /*MAG*/  5.435000000e+00,  4.420000000e+00,  3.390000000e+00,  2.567000000e+00,  2.003000000e+00,  1.880000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  3.006659000e-02,  2.320000000e-01,  2.320000000e-01,  2.320000000e-01, /*DIAM*/  3.056052053e-01,  3.160489674e-01,  3.152264859e-01, /*EDIAM*/  6.437929501e-02,  5.607726805e-02,  5.258877264e-02, /*DMEAN*/  3.129923635e-01, /*EDMEAN*/  3.300933032e-02 },
            {512, /* SP "M1III             " idx */ 244, /*MAG*/  6.099000000e+00,  4.540000000e+00,  2.750000000e+00,  1.584000000e+00,  7.730000000e-01,  6.080000000e-01, /*EMAG*/  3.313608000e-02,  4.000000000e-02,  2.022375000e-02,  3.220000000e-01,  3.220000000e-01,  3.220000000e-01, /*DIAM*/  6.352653080e-01,  6.736997340e-01,  6.657191668e-01, /*EDIAM*/  8.926095314e-02,  7.777362666e-02,  7.294172424e-02, /*DMEAN*/  6.604962339e-01, /*EDMEAN*/  4.574402758e-02 },
            {513, /* SP "K1IV(e)+DA1       " idx */ 204, /*MAG*/  6.045000000e+00,  4.760000000e+00,  3.460000000e+00,  2.435000000e+00,  1.830000000e+00,  1.765000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  2.009975000e-02,  2.820000000e-01,  2.820000000e-01,  2.820000000e-01, /*DIAM*/  3.826859813e-01,  3.805293164e-01,  3.590573434e-01, /*EDIAM*/  7.818457761e-02,  6.811573676e-02,  6.388163663e-02, /*DMEAN*/  3.726605113e-01, /*EDMEAN*/  4.007006126e-02 },
            {514, /* SP "M1III             " idx */ 244, /*MAG*/  5.765000000e+00,  4.220000000e+00,  2.330000000e+00,  1.088000000e+00,  1.690000000e-01, -9.900000000e-02, /*EMAG*/  3.712142000e-02,  4.000000000e-02,  2.022375000e-02,  3.500000000e-01,  3.500000000e-01,  3.500000000e-01, /*DIAM*/  7.479795954e-01,  8.062133547e-01,  8.172885121e-01, /*EDIAM*/  9.700009136e-02,  8.452108271e-02,  7.927106396e-02, /*DMEAN*/  7.953027780e-01, /*EDMEAN*/  4.970517791e-02 },
            {515, /* SP "G8IV              " idx */ 192, /*MAG*/  4.616000000e+00,  3.700000000e+00,  2.730000000e+00,  2.021000000e+00,  1.486000000e+00,  1.393000000e+00, /*EMAG*/              NaN,  4.000000000e-02,              NaN,  2.800000000e-01,  2.800000000e-01,  2.800000000e-01, /*DIAM*/  3.941334250e-01,  4.070498578e-01,  4.023517038e-01, /*EDIAM*/  7.764653041e-02,  6.764725218e-02,  6.344269388e-02, /*DMEAN*/  4.018216622e-01, /*EDMEAN*/  3.979933627e-02 },
            {516, /* SP "G8IVk             " idx */ 192, /*MAG*/  4.794000000e+00,  3.810000000e+00,  2.850000000e+00,  2.101000000e+00,  1.501000000e+00,  1.466000000e+00, /*EMAG*/  1.063015000e-02,  4.000000000e-02,  1.220656000e-02,  2.360000000e-01,  2.360000000e-01,  2.360000000e-01, /*DIAM*/  3.804369963e-01,  4.079681457e-01,  3.887239663e-01, /*EDIAM*/  6.549521169e-02,  5.705081737e-02,  5.350231519e-02, /*DMEAN*/  3.932006422e-01, /*EDMEAN*/  3.358232724e-02 },
            {517, /* SP "M2.5III           " idx */ 250, /*MAG*/  7.985000000e+00,  6.370000000e+00,  4.420000000e+00,  2.825000000e+00,  1.948000000e+00,  1.674000000e+00, /*EMAG*/  2.942788000e-02,  4.000000000e-02,  5.024938000e-02,  2.680000000e-01,  2.680000000e-01,  2.680000000e-01, /*DIAM*/  4.251284647e-01,  4.710836420e-01,  4.797681475e-01, /*EDIAM*/  7.435272368e-02,  6.477563261e-02,  6.074968150e-02, /*DMEAN*/  4.624614432e-01, /*EDMEAN*/  3.812330658e-02 },
            {518, /* SP "K2IIICNIVp        " idx */ 208, /*MAG*/  7.546000000e+00,  6.180000000e+00,  4.850000000e+00,  3.844000000e+00,  3.015000000e+00,  2.877000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  8.000000000e-03,  2.480000000e-01,  2.480000000e-01,  2.480000000e-01, /*DIAM*/  1.086423704e-01,  1.578674808e-01,  1.494515250e-01, /*EDIAM*/  6.879703764e-02,  5.992942145e-02,  5.620202375e-02, /*DMEAN*/  1.416740444e-01, /*EDMEAN*/  3.526523531e-02 },
            {519, /* SP "K2IIICNIVp        " idx */ 208, /*MAG*/  7.546000000e+00,  6.180000000e+00,  4.850000000e+00,  3.844000000e+00,  3.015000000e+00,  2.877000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  8.000000000e-03,  2.480000000e-01,  2.480000000e-01,  2.480000000e-01, /*DIAM*/  1.086423704e-01,  1.578674808e-01,  1.494515250e-01, /*EDIAM*/  6.879703764e-02,  5.992942145e-02,  5.620202375e-02, /*DMEAN*/  1.416740444e-01, /*EDMEAN*/  3.526523531e-02 },
            {520, /* SP "M3IIIe            " idx */ 252, /*MAG*/  9.638000000e+00,  8.480000000e+00,  7.360000000e+00,  5.001000000e+00,  4.371000000e+00,  3.851000000e+00, /*EMAG*/  3.689173000e-02,  4.000000000e-02,  3.605551000e-02,  2.460000000e-01,  2.460000000e-01,  2.460000000e-01, /*DIAM*/ -1.866468855e-02, -2.489106130e-02,  4.487816730e-02, /*EDIAM*/  6.829918057e-02,  5.949755643e-02,  5.579897330e-02, /*DMEAN*/  4.150561153e-03, /*EDMEAN*/  3.503234033e-02 },
            {521, /* SP "G8.5IIIbFe-0.5    " idx */ 194, /*MAG*/  6.098000000e+00,  5.170000000e+00,  4.250000000e+00,  3.654000000e+00,  3.155000000e+00,  2.999000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  2.000000000e-03,  3.320000000e-01,  3.320000000e-01,  3.320000000e-01, /*DIAM*/  5.867038540e-02,  6.702529720e-02,  7.962951798e-02, /*EDIAM*/  9.201186448e-02,  8.017243913e-02,  7.519185111e-02, /*DMEAN*/  6.978567849e-02, /*EDMEAN*/  4.715042857e-02 },
            {522, /* SP "G8.5IIIbFe-0.5    " idx */ 194, /*MAG*/  6.098000000e+00,  5.170000000e+00,  4.250000000e+00,  3.654000000e+00,  3.155000000e+00,  2.999000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  2.000000000e-03,  3.320000000e-01,  3.320000000e-01,  3.320000000e-01, /*DIAM*/  5.867038540e-02,  6.702529720e-02,  7.962951798e-02, /*EDIAM*/  9.201186448e-02,  8.017243913e-02,  7.519185111e-02, /*DMEAN*/  6.978567849e-02, /*EDMEAN*/  4.715042857e-02 },
            {523, /* SP "M3Ia-Iab          " idx */ 252, /*MAG*/  1.050500000e+01,  8.803000000e+00,              NaN,  2.495000000e+00,  1.534000000e+00,  9.890000000e-01, /*EMAG*/  9.000000000e-02,  4.000000000e-02,              NaN,  3.300000000e-01,  3.300000000e-01,  3.300000000e-01, /*DIAM*/  6.997621079e-01,  6.728435794e-01,  7.009716077e-01, /*EDIAM*/  9.149585930e-02,  7.972371048e-02,  7.477202875e-02, /*DMEAN*/  6.909420332e-01, /*EDMEAN*/  4.689875517e-02 },
            {524, /* SP "M2Iab             " idx */ 248, /*MAG*/  9.199000000e+00,  7.186000000e+00,              NaN,  2.799000000e+00,  1.951000000e+00,  1.567000000e+00, /*EMAG*/  2.800000000e-02,  4.000000000e-02,              NaN,  2.940000000e-01,  2.940000000e-01,  2.940000000e-01, /*DIAM*/  4.979182322e-01,  5.023459647e-01,  5.230864438e-01, /*EDIAM*/  8.152523622e-02,  7.102930814e-02,  6.661564853e-02, /*DMEAN*/  5.093321716e-01, /*EDMEAN*/  4.178939026e-02 },
            {525, /* SP "K5III             " idx */ 220, /*MAG*/  7.900000000e+00,  6.080000000e+00,  4.250000000e+00,  2.766000000e+00,  1.833000000e+00,  1.637000000e+00, /*EMAG*/  2.061553000e-02,  4.000000000e-02,  9.013878000e-02,  3.040000000e-01,  3.040000000e-01,  3.040000000e-01, /*DIAM*/  4.173171685e-01,  4.535546588e-01,  4.423501976e-01, /*EDIAM*/  8.429599666e-02,  7.344436824e-02,  6.888006880e-02, /*DMEAN*/  4.396615031e-01, /*EDMEAN*/  4.319689066e-02 },
            {526, /* SP "F9V               " idx */ 156, /*MAG*/  4.636000000e+00,  4.100000000e+00,  3.520000000e+00,  3.175000000e+00,  2.957000000e+00,  2.859000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  3.014963000e-02,  2.740000000e-01,  2.740000000e-01,  2.740000000e-01, /*DIAM*/  4.961846270e-02,  4.366546086e-02,  5.196899435e-02, /*EDIAM*/  7.617195387e-02,  6.636284987e-02,  6.223852306e-02, /*DMEAN*/  4.848605463e-02, /*EDMEAN*/  3.905251808e-02 },
            {527, /* SP "K1V               " idx */ 204, /*MAG*/  6.076000000e+00,  5.240000000e+00,  4.360000000e+00,  3.855000000e+00,  3.391000000e+00,  3.285000000e+00, /*EMAG*/              NaN,  4.000000000e-02,              NaN,  2.660000000e-01,  2.660000000e-01,  2.660000000e-01, /*DIAM*/  2.650740457e-02,  2.374331884e-02,  2.772887134e-02, /*EDIAM*/  7.376387194e-02,  6.426089699e-02,  6.026548931e-02, /*DMEAN*/  2.603271465e-02, /*EDMEAN*/  3.780774463e-02 },
            {528, /* SP "G8.5V             " idx */ 194, /*MAG*/  4.217000000e+00,  3.490000000e+00,  2.670000000e+00,  2.149000000e+00,  1.800000000e+00,  1.794000000e+00, /*EMAG*/              NaN,  4.000000000e-02,              NaN,  3.100000000e-01,  3.100000000e-01,  3.100000000e-01, /*DIAM*/  3.462328897e-01,  3.246206318e-01,  3.081477696e-01, /*EDIAM*/  8.593101247e-02,  7.487065041e-02,  7.021855325e-02, /*DMEAN*/  3.238132688e-01, /*EDMEAN*/  4.403811166e-02 },
            {529, /* SP "G0V               " idx */ 160, /*MAG*/  5.447000000e+00,  4.840000000e+00,  4.080000000e+00,  3.525000000e+00,  3.287000000e+00,  3.076000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  7.017834000e-02,  2.760000000e-01,  2.760000000e-01,  2.760000000e-01, /*DIAM*/  1.440325719e-02, -3.537511121e-03,  2.497883359e-02, /*EDIAM*/  7.670270728e-02,  6.682570024e-02,  6.267280534e-02, /*DMEAN*/  1.236196512e-02, /*EDMEAN*/  3.932470280e-02 },
            {530, /* SP "F8V               " idx */ 152, /*MAG*/  4.614000000e+00,  4.100000000e+00,  3.510000000e+00,  3.031000000e+00,  2.863000000e+00,  2.697000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  2.000000000e-03,  2.880000000e-01,  2.880000000e-01,  2.880000000e-01, /*DIAM*/  8.500687080e-02,  6.459843360e-02,  8.602365575e-02, /*EDIAM*/  8.005066687e-02,  6.974480780e-02,  6.541086483e-02, /*DMEAN*/  7.835891341e-02, /*EDMEAN*/  4.103610203e-02 },
            {531, /* SP "F9.5V             " idx */ 158, /*MAG*/  4.645000000e+00,  4.050000000e+00,  3.400000000e+00,  3.143000000e+00,  2.875000000e+00,  2.723000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  1.044031000e-02,  2.660000000e-01,  2.660000000e-01,  2.660000000e-01, /*DIAM*/  5.700833583e-02,  6.230850358e-02,  8.275216462e-02, /*EDIAM*/  7.395705748e-02,  6.443154205e-02,  6.042689011e-02, /*DMEAN*/  6.895119168e-02, /*EDMEAN*/  3.791980342e-02 },
            {532, /* SP "F9IV-V            " idx */ 156, /*MAG*/  4.865000000e+00,  4.290000000e+00,  3.630000000e+00,  3.194000000e+00,  2.916000000e+00,  2.835000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  2.009975000e-02,  2.600000000e-01,  2.600000000e-01,  2.600000000e-01, /*DIAM*/  5.894881999e-02,  6.139308758e-02,  6.239235217e-02, /*EDIAM*/  7.231478224e-02,  6.299944853e-02,  5.908343022e-02, /*DMEAN*/  6.114561993e-02, /*EDMEAN*/  3.707932516e-02 },
            {533, /* SP "G1V               " idx */ 164, /*MAG*/  5.320000000e+00,  4.690000000e+00,  3.990000000e+00,  3.397000000e+00,  3.154000000e+00,  3.038000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  3.000000000e-03,  2.600000000e-01,  2.600000000e-01,  2.600000000e-01, /*DIAM*/  4.353314463e-02,  2.442430433e-02,  3.239769719e-02, /*EDIAM*/  7.227101416e-02,  6.296134242e-02,  5.904785589e-02, /*DMEAN*/  3.256000071e-02, /*EDMEAN*/  3.705795456e-02 },
            {534, /* SP "F2V               " idx */ 128, /*MAG*/  4.047000000e+00,  3.710000000e+00,  3.320000000e+00,  3.063000000e+00,  2.985000000e+00,  2.993000000e+00, /*EMAG*/  1.216553000e-02,  4.000000000e-02,  2.009975000e-02,  2.540000000e-01,  2.540000000e-01,  2.540000000e-01, /*DIAM*/  2.510263226e-02,  8.997426440e-03, -7.676727689e-03, /*EDIAM*/  7.089774340e-02,  6.176383572e-02,  5.792383243e-02, /*DMEAN*/  6.663731639e-03, /*EDMEAN*/  3.635240923e-02 },
            {535, /* SP "K8V               " idx */ 232, /*MAG*/  7.926000000e+00,  6.600000000e+00,  5.310000000e+00,  3.894000000e+00,  3.298000000e+00,  2.962000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  1.077033000e-02,  2.900000000e-01,  2.900000000e-01,  2.900000000e-01, /*DIAM*/  1.556661326e-01,  1.363303429e-01,  1.718391671e-01, /*EDIAM*/  8.044198954e-02,  7.008394412e-02,  6.572795401e-02, /*DMEAN*/  1.553413439e-01, /*EDMEAN*/  4.122613676e-02 },
            {536, /* SP "G1-V_Fe-0.5       " idx */ 164, /*MAG*/  5.654000000e+00,  5.030000000e+00,  4.340000000e+00,  3.960000000e+00,  3.736000000e+00,  3.750000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  3.000000000e-03,  3.400000000e-01,  3.400000000e-01,  3.400000000e-01, /*DIAM*/ -8.619007159e-02, -1.019570205e-01, -1.197774876e-01, /*EDIAM*/  9.434190651e-02,  8.220518799e-02,  7.709940851e-02, /*DMEAN*/ -1.048233142e-01, /*EDMEAN*/  4.835084383e-02 },
            {537, /* SP "A1IVps            " idx */ 84, /*MAG*/  2.373000000e+00,  2.340000000e+00,  2.320000000e+00,  2.269000000e+00,  2.359000000e+00,  2.285000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  3.006659000e-02,  2.440000000e-01,  2.440000000e-01,  2.440000000e-01, /*DIAM*/  7.574790754e-02,  4.563323921e-02,  4.976597238e-02, /*EDIAM*/  6.883829481e-02,  5.998461266e-02,  5.626505404e-02, /*DMEAN*/  5.514019645e-02, /*EDMEAN*/  3.539913873e-02 },
            {538, /* SP "F9V               " idx */ 156, /*MAG*/  4.108000000e+00,  3.590000000e+00,  2.980000000e+00,  2.597000000e+00,  2.363000000e+00,  2.269000000e+00, /*EMAG*/  1.552417000e-02,  4.000000000e-02,  2.039608000e-02,  2.540000000e-01,  2.540000000e-01,  2.540000000e-01, /*DIAM*/  1.704398931e-01,  1.659300541e-01,  1.720711859e-01, /*EDIAM*/  7.066230588e-02,  6.155846114e-02,  5.773167641e-02, /*DMEAN*/  1.695233731e-01, /*EDMEAN*/  3.623405055e-02 },
            {539, /* SP "F2V               " idx */ 128, /*MAG*/  4.688000000e+00,  4.300000000e+00,  3.860000000e+00,  3.609000000e+00,  3.372000000e+00,  3.372000000e+00, /*EMAG*/  1.414214000e-02,  4.000000000e-02,  2.009975000e-02,  3.020000000e-01,  3.020000000e-01,  3.020000000e-01, /*DIAM*/ -8.071879789e-02, -6.002981194e-02, -7.793220319e-02, /*EDIAM*/  8.408963375e-02,  7.326658200e-02,  6.871415145e-02, /*DMEAN*/ -7.248031994e-02, /*EDMEAN*/  4.310181548e-02 },
            {540, /* SP "F9.5V             " idx */ 158, /*MAG*/  4.802000000e+00,  4.230000000e+00,  3.560000000e+00,  3.232000000e+00,  2.992000000e+00,  2.923000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  4.011234000e-02,  2.740000000e-01,  2.740000000e-01,  2.740000000e-01, /*DIAM*/  4.619583567e-02,  4.150694685e-02,  4.222661657e-02, /*EDIAM*/  7.616168969e-02,  6.635393144e-02,  6.223021215e-02, /*DMEAN*/  4.301758315e-02, /*EDMEAN*/  3.904763255e-02 },
            {541, /* SP "G7V               " idx */ 188, /*MAG*/  5.449000000e+00,  4.740000000e+00,  3.990000000e+00,  3.334000000e+00,  2.974000000e+00,  2.956000000e+00, /*EMAG*/              NaN,  4.000000000e-02,              NaN,  2.360000000e-01,  2.360000000e-01,  2.360000000e-01, /*DIAM*/  1.033230027e-01,  8.719243071e-02,  7.205449499e-02, /*EDIAM*/  6.551189405e-02,  5.706570743e-02,  5.351652869e-02, /*DMEAN*/  8.546662568e-02, /*EDMEAN*/  3.359311378e-02 },
            {542, /* SP "G4Va              " idx */ 176, /*MAG*/  5.684000000e+00,  4.970000000e+00,  4.200000000e+00,  3.798000000e+00,  3.457000000e+00,  3.500000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  3.000000000e-03,  2.940000000e-01,  2.940000000e-01,  2.940000000e-01, /*DIAM*/ -2.807342848e-02, -2.962063127e-02, -5.568084310e-02, /*EDIAM*/  8.158430132e-02,  7.108182187e-02,  6.666521089e-02, /*DMEAN*/ -3.945051607e-02, /*EDMEAN*/  4.182093279e-02 },
            {543, /* SP "F6IV+M2           " idx */ 144, /*MAG*/  5.008000000e+00,  4.500000000e+00,  3.990000000e+00,  3.617000000e+00,  3.546000000e+00,  3.507000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  3.006659000e-02,  3.480000000e-01,  3.480000000e-01,  3.480000000e-01, /*DIAM*/ -5.440508751e-02, -8.689877654e-02, -9.190350780e-02, /*EDIAM*/  9.663683655e-02,  8.420573042e-02,  7.897551642e-02, /*DMEAN*/ -8.034934688e-02, /*EDMEAN*/  4.952243698e-02 },
            {544, /* SP "K4V               " idx */ 216, /*MAG*/  6.744000000e+00,  5.720000000e+00,  4.490000000e+00,  3.663000000e+00,  3.085000000e+00,  3.048000000e+00, /*EMAG*/  1.581139000e-02,  4.000000000e-02,  8.015610000e-02,  2.580000000e-01,  2.580000000e-01,  2.580000000e-01, /*DIAM*/  1.360056857e-01,  1.317019627e-01,  1.085148475e-01, /*EDIAM*/  7.157934929e-02,  6.235568050e-02,  5.847800984e-02, /*DMEAN*/  1.237200403e-01, /*EDMEAN*/  3.668840443e-02 },
            {545, /* SP "K0III-IV          " idx */ 200, /*MAG*/  5.786000000e+00,  4.790000000e+00,  3.820000000e+00,  3.035000000e+00,  2.575000000e+00,  2.423000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  3.006659000e-02,  2.420000000e-01,  2.420000000e-01,  2.420000000e-01, /*DIAM*/  2.117638310e-01,  1.975741085e-01,  2.064257329e-01, /*EDIAM*/  6.713468059e-02,  5.848003703e-02,  5.484263574e-02, /*DMEAN*/  2.047663308e-01, /*EDMEAN*/  3.441709237e-02 },
            {546, /* SP "F6IV              " idx */ 144, /*MAG*/  4.328000000e+00,  3.850000000e+00,  3.310000000e+00,  3.149000000e+00,  2.875000000e+00,  2.703000000e+00, /*EMAG*/              NaN,  4.000000000e-02,              NaN,  3.140000000e-01,  3.140000000e-01,  3.140000000e-01, /*DIAM*/  2.521991368e-02,  4.816737333e-02,  7.294320999e-02, /*EDIAM*/  8.726014482e-02,  7.603063275e-02,  7.130701228e-02, /*DMEAN*/  5.188532413e-02, /*EDMEAN*/  4.472355163e-02 },
            {547, /* SP "K0-V              " idx */ 200, /*MAG*/  4.890000000e+00,  4.030000000e+00,  3.070000000e+00,  2.343000000e+00,  1.876000000e+00,  1.791000000e+00, /*EMAG*/              NaN,  4.000000000e-02,              NaN,  3.040000000e-01,  3.040000000e-01,  3.040000000e-01, /*DIAM*/  3.449424044e-01,  3.348581573e-01,  3.294622311e-01, /*EDIAM*/  8.426433543e-02,  7.341712701e-02,  6.885481911e-02, /*DMEAN*/  3.353807036e-01, /*EDMEAN*/  4.318285084e-02 },
            {548, /* SP "K5V               " idx */ 220, /*MAG*/  6.269000000e+00,  5.200000000e+00,  4.070000000e+00,  3.114000000e+00,  2.540000000e+00,  2.248000000e+00, /*EMAG*/              NaN,  4.000000000e-02,              NaN,  3.180000000e-01,  3.180000000e-01,  3.180000000e-01, /*DIAM*/  2.534243090e-01,  2.466986245e-01,  2.809706332e-01, /*EDIAM*/  8.816426604e-02,  7.681714643e-02,  7.204391777e-02, /*DMEAN*/  2.619195084e-01, /*EDMEAN*/  4.517687366e-02 },
            {549, /* SP "F5V               " idx */ 140, /*MAG*/  4.205000000e+00,  3.770000000e+00,  3.260000000e+00,  2.954000000e+00,  2.729000000e+00,  2.564000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  3.026549000e-02,  2.900000000e-01,  2.900000000e-01,  2.900000000e-01, /*DIAM*/  6.951030736e-02,  7.851275158e-02,  9.966132679e-02, /*EDIAM*/  8.067305953e-02,  7.028728905e-02,  6.591938754e-02, /*DMEAN*/  8.446099151e-02, /*EDMEAN*/  4.135223840e-02 },
            {550, /* SP "A4V               " idx */ 96, /*MAG*/  1.315000000e+00,  1.170000000e+00,  1.010000000e+00,  1.037000000e+00,  9.370000000e-01,  9.450000000e-01, /*EMAG*/  1.303840000e-02,  4.000000000e-02,  1.220656000e-02,  2.860000000e-01,  2.860000000e-01,  2.860000000e-01, /*DIAM*/  3.550153135e-01,  3.665458529e-01,  3.492304374e-01, /*EDIAM*/  8.021585441e-02,  6.989539843e-02,  6.555588009e-02, /*DMEAN*/  3.567248581e-01, /*EDMEAN*/  4.115982201e-02 },
            {551, /* SP "F7V               " idx */ 148, /*MAG*/  4.637000000e+00,  4.130000000e+00,  3.540000000e+00,  3.299000000e+00,  2.988000000e+00,  2.946000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  2.022375000e-02,  2.880000000e-01,  2.880000000e-01,  2.880000000e-01, /*DIAM*/  9.010283258e-03,  3.403891573e-02,  2.789652794e-02, /*EDIAM*/  8.007169028e-02,  6.976304881e-02,  6.542784343e-02, /*DMEAN*/  2.507080631e-02, /*EDMEAN*/  4.104593848e-02 },
            {552, /* SP "G8IV              " idx */ 192, /*MAG*/  4.616000000e+00,  3.700000000e+00,  2.730000000e+00,  2.021000000e+00,  1.486000000e+00,  1.393000000e+00, /*EMAG*/              NaN,  4.000000000e-02,              NaN,  2.800000000e-01,  2.800000000e-01,  2.800000000e-01, /*DIAM*/  3.941334250e-01,  4.070498578e-01,  4.023517038e-01, /*EDIAM*/  7.764653041e-02,  6.764725218e-02,  6.344269388e-02, /*DMEAN*/  4.018216622e-01, /*EDMEAN*/  3.979933627e-02 },
            {553, /* SP "A0V               " idx */ 80, /*MAG*/  4.796000000e+00,  4.780000000e+00,  4.700000000e+00,  4.900000000e+00,  4.712000000e+00,  4.296000000e+00, /*EMAG*/  2.915476000e-02,  4.000000000e-02,  2.022375000e-02,  2.060000000e-01,  2.060000000e-01,  2.060000000e-01, /*DIAM*/ -4.777346564e-01, -4.327628738e-01, -3.523528040e-01, /*EDIAM*/  5.851496667e-02,  5.099108955e-02,  4.783382780e-02, /*DMEAN*/ -4.129197188e-01, /*EDMEAN*/  3.017545465e-02 },
            {554, /* SP "A7V               " idx */ 108, /*MAG*/  5.418000000e+00,  5.110000000e+00,  4.740000000e+00,  4.338000000e+00,  4.264000000e+00,  4.313000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  3.014963000e-02,  2.220000000e-01,  2.220000000e-01,  2.220000000e-01, /*DIAM*/ -2.384734749e-01, -2.568475861e-01, -2.901944709e-01, /*EDIAM*/  6.254769076e-02,  5.448554198e-02,  5.109828358e-02, /*DMEAN*/ -2.651437057e-01, /*EDMEAN*/  3.210438584e-02 },
            {555, /* SP "G4Va              " idx */ 176, /*MAG*/  5.684000000e+00,  4.970000000e+00,  4.200000000e+00,  3.798000000e+00,  3.457000000e+00,  3.500000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  3.000000000e-03,  2.940000000e-01,  2.940000000e-01,  2.940000000e-01, /*DIAM*/ -2.807342848e-02, -2.962063127e-02, -5.568084310e-02, /*EDIAM*/  8.158430132e-02,  7.108182187e-02,  6.666521089e-02, /*DMEAN*/ -3.945051607e-02, /*EDMEAN*/  4.182093279e-02 },
            {556, /* SP "K0V               " idx */ 200, /*MAG*/  6.730000000e+00,  5.880000000e+00,  5.050000000e+00,  4.549000000e+00,  4.064000000e+00,  3.999000000e+00, /*EMAG*/              NaN,  4.000000000e-02,              NaN,  2.400000000e-01,  2.400000000e-01,  2.400000000e-01, /*DIAM*/ -1.235933168e-01, -1.166827055e-01, -1.215450749e-01, /*EDIAM*/  6.658237739e-02,  5.799836798e-02,  5.439077587e-02, /*DMEAN*/ -1.204020809e-01, /*EDMEAN*/  3.413452229e-02 },
            {557, /* SP "F9V               " idx */ 156, /*MAG*/  4.636000000e+00,  4.100000000e+00,  3.520000000e+00,  3.175000000e+00,  2.957000000e+00,  2.859000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  3.014963000e-02,  2.740000000e-01,  2.740000000e-01,  2.740000000e-01, /*DIAM*/  4.961846270e-02,  4.366546086e-02,  5.196899435e-02, /*EDIAM*/  7.617195387e-02,  6.636284987e-02,  6.223852306e-02, /*DMEAN*/  4.848605463e-02, /*EDMEAN*/  3.905251808e-02 },
            {558, /* SP "F8.5V             " idx */ 154, /*MAG*/  5.645000000e+00,  5.070000000e+00,  4.440000000e+00,  4.174000000e+00,  3.768000000e+00,  3.748000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  2.022375000e-02,  2.800000000e-01,  2.800000000e-01,  2.800000000e-01, /*DIAM*/ -1.546877575e-01, -1.128640622e-01, -1.250096700e-01, /*EDIAM*/  7.783567355e-02,  6.781350592e-02,  6.359926244e-02, /*DMEAN*/ -1.285883731e-01, /*EDMEAN*/  3.990327170e-02 },
            {559, /* SP "G8V               " idx */ 192, /*MAG*/  6.829000000e+00,  5.960000000e+00,  5.140000000e+00,  4.768000000e+00,  4.265000000e+00,  4.015000000e+00, /*EMAG*/  1.300000000e-02,  4.000000000e-02,  7.017834000e-02,  2.440000000e-01,  2.440000000e-01,  2.440000000e-01, /*DIAM*/ -1.926612266e-01, -1.701563765e-01, -1.315607130e-01, /*EDIAM*/  6.770381394e-02,  5.897695097e-02,  5.530923332e-02, /*DMEAN*/ -1.608840783e-01, /*EDMEAN*/  3.471215552e-02 },
            {560, /* SP "K1III             " idx */ 204, /*MAG*/  6.913000000e+00,  5.970000000e+00,  5.040000000e+00,  3.933000000e+00,  3.557000000e+00,  3.550000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  4.000000000e-03,  2.380000000e-01,  2.380000000e-01,  2.380000000e-01, /*DIAM*/  6.097169080e-02,  1.380557550e-02, -1.305215117e-02, /*EDIAM*/  6.603017361e-02,  5.751656777e-02,  5.393861666e-02, /*DMEAN*/  1.559904346e-02, /*EDMEAN*/  3.385053644e-02 },
            {561, /* SP "K1.5IIIFe-1       " idx */ 206, /*MAG*/  5.999000000e+00,  4.820000000e+00,  3.660000000e+00,  2.949000000e+00,  2.197000000e+00,  2.085000000e+00, /*EMAG*/  1.315295000e-02,  4.000000000e-02,  3.006659000e-02,  2.340000000e-01,  2.340000000e-01,  2.340000000e-01, /*DIAM*/  2.485189746e-01,  2.967692069e-01,  2.905551432e-01, /*EDIAM*/  6.492782687e-02,  5.655508907e-02,  5.303656041e-02, /*DMEAN*/  2.816986347e-01, /*EDMEAN*/  3.328597657e-02 },
            {562, /* SP "G8                " idx */ 192, /*MAG*/  7.080000000e+00,  6.040000000e+00,  5.030000000e+00,  4.580000000e+00,  3.929000000e+00,  3.814000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  1.077033000e-02,  3.540000000e-01,  3.540000000e-01,  3.540000000e-01, /*DIAM*/ -1.344826543e-01, -8.579839862e-02, -8.397677064e-02, /*EDIAM*/  9.809808318e-02,  8.547879896e-02,  8.016946142e-02, /*DMEAN*/ -9.784069038e-02, /*EDMEAN*/  5.026646238e-02 },
            {563, /* SP "G7IV-V            " idx */ 188, /*MAG*/  6.479000000e+00,  5.730000000e+00,  4.900000000e+00,  4.554000000e+00,  4.239000000e+00,  4.076000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  7.006426000e-02,  2.520000000e-01,  2.520000000e-01,  2.520000000e-01, /*DIAM*/ -1.583377155e-01, -1.771499857e-01, -1.553615668e-01, /*EDIAM*/  6.992840423e-02,  6.091727531e-02,  5.712967486e-02, /*DMEAN*/ -1.636649425e-01, /*EDMEAN*/  3.585235542e-02 },
            {564, /* SP "G2.5IVa           " idx */ 170, /*MAG*/  6.116000000e+00,  5.450000000e+00,  4.760000000e+00,  4.655000000e+00,  4.234000000e+00,  3.911000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  1.044031000e-02,  2.960000000e-01,  2.960000000e-01,  2.960000000e-01, /*DIAM*/ -2.377667551e-01, -2.012882101e-01, -1.407930142e-01, /*EDIAM*/  8.216773291e-02,  7.159075927e-02,  6.714272889e-02, /*DMEAN*/ -1.870842325e-01, /*EDMEAN*/  4.212073318e-02 },
            {565, /* SP "G8III             " idx */ 192, /*MAG*/  6.249000000e+00,  5.220000000e+00,  4.220000000e+00,  3.019000000e+00,  2.608000000e+00,  2.331000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  3.000000000e-03,  2.400000000e-01,  2.400000000e-01,  2.400000000e-01, /*DIAM*/  2.346155655e-01,  1.990654189e-01,  2.300451319e-01, /*EDIAM*/  6.659946563e-02,  5.801385237e-02,  5.440574682e-02, /*DMEAN*/  2.205430904e-01, /*EDMEAN*/  3.414720662e-02 },
            {566, /* SP "K0III             " idx */ 200, /*MAG*/  7.127000000e+00,  6.190000000e+00,  5.300000000e+00,  4.952000000e+00,  4.326000000e+00,  4.225000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  3.041381000e-02,  2.640000000e-01,  2.640000000e-01,  2.640000000e-01, /*DIAM*/ -2.113343896e-01, -1.671029397e-01, -1.645377763e-01, /*EDIAM*/  7.321128007e-02,  6.377921594e-02,  5.981378783e-02, /*DMEAN*/ -1.776775435e-01, /*EDMEAN*/  3.752629498e-02 },
            {567, /* SP "F5III             " idx */ 140, /*MAG*/  5.602000000e+00,  5.160000000e+00,  4.650000000e+00,  4.389000000e+00,  4.218000000e+00,  4.069000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  3.000000000e-03,  3.420000000e-01,  3.420000000e-01,  3.420000000e-01, /*DIAM*/ -2.209450541e-01, -2.233705214e-01, -2.043605756e-01, /*EDIAM*/  9.500454946e-02,  8.278261886e-02,  7.764051805e-02, /*DMEAN*/ -2.152703181e-01, /*EDMEAN*/  4.868651572e-02 },
            {568, /* SP "G5V               " idx */ 180, /*MAG*/  7.070000000e+00,  6.110000000e+00,  5.160000000e+00,  4.262000000e+00,  3.848000000e+00,  3.893000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  4.000000000e-03,  2.740000000e-01,  2.740000000e-01,  2.740000000e-01, /*DIAM*/ -6.233828052e-02, -7.393122854e-02, -1.112816472e-01, /*EDIAM*/  7.604183207e-02,  6.624885143e-02,  6.213148745e-02, /*DMEAN*/ -8.556628245e-02, /*EDMEAN*/  3.898377160e-02 },
            {569, /* SP "G9III             " idx */ 196, /*MAG*/  6.886000000e+00,  5.870000000e+00,  4.880000000e+00,  4.301000000e+00,  3.766000000e+00,  3.657000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  4.000000000e-03,  2.420000000e-01,  2.420000000e-01,  2.420000000e-01, /*DIAM*/ -6.300194309e-02, -4.946076568e-02, -4.876622028e-02, /*EDIAM*/  6.714034994e-02,  5.848524830e-02,  5.484773351e-02, /*DMEAN*/ -5.273260326e-02, /*EDMEAN*/  3.442181796e-02 },
            {570, /* SP "F5V               " idx */ 140, /*MAG*/  5.430000000e+00,  4.990000000e+00,  4.480000000e+00,  4.203000000e+00,  4.059000000e+00,  3.944000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  2.000000000e-03,  3.100000000e-01,  3.100000000e-01,  3.100000000e-01, /*DIAM*/ -1.825164821e-01, -1.920242174e-01, -1.805430570e-01, /*EDIAM*/  8.618275056e-02,  7.509123501e-02,  7.042572705e-02, /*DMEAN*/ -1.850245639e-01, /*EDMEAN*/  4.417166105e-02 },
            {571, /* SP "F5V               " idx */ 140, /*MAG*/  5.479000000e+00,  5.040000000e+00,  4.530000000e+00,  4.136000000e+00,  4.027000000e+00,  3.960000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  3.000000000e-03,  2.560000000e-01,  2.560000000e-01,  2.560000000e-01, /*DIAM*/ -1.601325532e-01, -1.822421161e-01, -1.828496263e-01, /*EDIAM*/  7.131583534e-02,  6.212803542e-02,  5.826544406e-02, /*DMEAN*/ -1.766918293e-01, /*EDMEAN*/  3.656477081e-02 },
            {572, /* SP "K0                " idx */ 200, /*MAG*/  7.294000000e+00,  6.210000000e+00,  5.160000000e+00,  4.397000000e+00,  3.822000000e+00,  3.666000000e+00, /*EMAG*/  9.999999776e-03,  4.000000000e-02,  1.077033000e-02,  2.960000000e-01,  2.960000000e-01,  2.960000000e-01, /*DIAM*/ -5.618260153e-02, -4.469048652e-02, -3.752317584e-02, /*EDIAM*/  8.205332804e-02,  7.148928807e-02,  6.704639561e-02, /*DMEAN*/ -4.488584988e-02, /*EDMEAN*/  4.205124729e-02 },
        };
#undef NaN

        logInfo("check %d diameters:", NS);

        mcsUINT32 i, j;

        alxMAGNITUDES mags;

        for (i = alxB_BAND; i < alxNB_BANDS; i++)
        {
            mags[alxB_BAND].isSet = mcsFALSE;
        }

        mcsDOUBLE spTypeIndex;

        mcsUINT32 nbDiameters = 0;

        alxDIAMETERS diameters;
        alxDIAMETERS_COVARIANCE diametersCov;

        alxDATA weightedMeanDiam, chi2Diam;

        mcsDOUBLE delta, diam, err;
        mcsUINT32 NB = 0;
        mcsUINT32 bad = 0;

        for (i = 0; i < NS; i++)
        {
            spTypeIndex = datas[i][IDL_COL_SP]; /* before 201608: +20 to shift O5 from 0 to 20 */

            mags[alxB_BAND].isSet = mcsTRUE;
            mags[alxB_BAND].confIndex = alxCONFIDENCE_HIGH;
            mags[alxB_BAND].value = datas[i][IDL_COL_MAG + 0];
            mags[alxB_BAND].error = datas[i][IDL_COL_EMAG + 0];

            mags[alxV_BAND].isSet = mcsTRUE;
            mags[alxV_BAND].confIndex = alxCONFIDENCE_HIGH;
            mags[alxV_BAND].value = datas[i][IDL_COL_MAG + 1];
            mags[alxV_BAND].error = datas[i][IDL_COL_EMAG + 1];

            /* skip Ic */

            mags[alxJ_BAND].isSet = mcsTRUE;
            mags[alxJ_BAND].confIndex = alxCONFIDENCE_HIGH;
            mags[alxJ_BAND].value = datas[i][IDL_COL_MAG + 3];
            mags[alxJ_BAND].error = datas[i][IDL_COL_EMAG + 3];

            mags[alxH_BAND].isSet = mcsTRUE;
            mags[alxH_BAND].confIndex = alxCONFIDENCE_HIGH;
            mags[alxH_BAND].value = datas[i][IDL_COL_MAG + 4];
            mags[alxH_BAND].error = datas[i][IDL_COL_EMAG + 4];

            mags[alxK_BAND].isSet = mcsTRUE;
            mags[alxK_BAND].confIndex = alxCONFIDENCE_HIGH;
            mags[alxK_BAND].value = datas[i][IDL_COL_MAG + 5];
            mags[alxK_BAND].error = datas[i][IDL_COL_EMAG + 5];

            bad = 0;

            if (alxComputeAngularDiameters   ("(SP)   ", mags, spTypeIndex, diameters, diametersCov, logTEST) == mcsFAILURE)
            {
                logInfo("alxComputeAngularDiameters : fail");
            }

            for (j = 0; j < alxNB_DIAMS; j++)
            {
                /* diam (log10): */
                diam = log10(diameters[j].value);
                delta = fabs(datas[i][IDL_COL_DIAM + j] - diam);

                if (delta > DELTA_TH)
                {
                    logInfo("diam: %.9lf %.9lf", datas[i][IDL_COL_DIAM + j], diam);
                    bad = 1;
                }

                /* rel error: */
                err = relError(diameters[j].value, diameters[j].error);
                delta = fabs(datas[i][IDL_COL_EDIAM + j] - err);

                if (delta > DELTA_TH)
                {
                    logInfo("err : %.9lf %.9lf", datas[i][IDL_COL_EDIAM + j], err);
                    bad = 1;
                }

                delta = fabs(datas[i][IDL_COL_DIAM + j] - diam) / alxNorm(datas[i][IDL_COL_EDIAM + j], err);

                if (delta > DELTA_SIG)
                {
                    logInfo("res : %.9lf sigma", delta);
                    bad = 1;
                }
            }

            /* may set low confidence to inconsistent diameters */
            if (alxComputeMeanAngularDiameter(diameters, diametersCov, nbRequiredDiameters, &weightedMeanDiam,
                                              &chi2Diam, &nbDiameters, NULL, logTEST) == mcsFAILURE)
            {
                logInfo("alxComputeMeanAngularDiameter : fail");
            }

            if (alxIsSet(weightedMeanDiam))
            {
                /* diam (log10): */
                diam = log10(weightedMeanDiam.value);
                delta = fabs(datas[i][IDL_COL_MEAN] - diam);

                if (delta > DELTA_TH)
                {
                    logInfo("Mean: %.9lf %.9lf", datas[i][IDL_COL_MEAN], diam);
                    bad = 1;
                }

                if (1)
                {
                    /* rel error: */
                    err = relError(weightedMeanDiam.value, weightedMeanDiam.error);
                    delta = fabs(datas[i][IDL_COL_EMEAN] - err);

                    if (delta > DELTA_TH)
                    {
                        logInfo("errM: %.9lf %.9lf", i, datas[i][IDL_COL_EMEAN], err);
                        bad = 1;
                    }
                    /* Note: E_DMEAN uses DCOV_OB (includes E_DIAM_I^2 term) so is different ! */
                    delta = fabs(datas[i][IDL_COL_MEAN] - diam) / alxNorm(datas[i][IDL_COL_EMEAN], err);

                    if (delta > DELTA_SIG)
                    {
                        logInfo("resM: %.9lf sigma", delta);
                        bad = 1;
                    }
                }

                if (bad == 1)
                {
                    NB++;
                }
            }
        }
        logInfo("check %u diameters: done : %u diffs.", NS, NB);
    }
}

/*___oOo___*/
