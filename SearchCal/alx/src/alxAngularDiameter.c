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

/* enable/disable checks for high correlations */
#define CHECK_HIGH_CORRELATION  mcsTRUE

/* enable/disable discarding redundant color bands (high correlation) */
#define FILTER_HIGH_CORRELATION mcsFALSE

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
        { 0 },
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
    miscDynBufInit(&dynBuf);

    logInfo("Loading %s ...", fileName);

    NULL_DO(miscDynBufLoadFile(&dynBuf, fileName, "#"),
            miscDynBufDestroy(&dynBuf);
            free(fileName));

    /* For each line of the loaded file */
    mcsINT32 lineNum = 0;
    const char* pos = NULL;
    mcsSTRING1024 line;
    mcsSTRING4 color;
    mcsINT32 c;
    mcsINT32 index;

    while (IS_NOT_NULL(pos = miscDynBufGetNextLine(&dynBuf, pos, line, sizeof (line), mcsTRUE)))
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
             * #FIT COLORS:  V-B V-J V-H V-K
             * #Color a0 a1 a2 a3 a4
             */
            if (sscanf(line, "%3s %lf %lf %lf %lf %lf %lf %lf",
                       color,
                       &polynomial.coeff[lineNum][0],
                       &polynomial.coeff[lineNum][1],
                       &polynomial.coeff[lineNum][2],
                       &polynomial.coeff[lineNum][3],
                       &polynomial.coeff[lineNum][4],
                       &polynomial.coeff[lineNum][5],
                       &polynomial.coeff[lineNum][6]
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

    free(fileName);

    /* Check if there is missing line */
    if (lineNum != alxNB_DIAMS)
    {
        miscDynBufDestroy(&dynBuf);
        errAdd(alxERR_MISSING_LINE, lineNum, alxNB_DIAMS, fileName);
        return NULL;
    }

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
    miscDynBufReset(&dynBuf);

    logInfo("Loading %s ...", fileName);

    NULL_DO(miscDynBufLoadFile(&dynBuf, fileName, "#"),
            miscDynBufDestroy(&dynBuf);
            free(fileName));

    /* For each line of the loaded file */
    lineNum = 0;
    pos = NULL;

    mcsDOUBLE* polynomCoefCovLine;

    while (IS_NOT_NULL(pos = miscDynBufGetNextLine(&dynBuf, pos, line, sizeof (line), mcsTRUE)))
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

    free(fileName);

    /* Check if there are missing lines */
    if (lineNum != alxNB_POLYNOMIAL_COEFF_COVARIANCE)
    {
        errAdd(alxERR_MISSING_LINE, lineNum, alxNB_POLYNOMIAL_COEFF_COVARIANCE, fileName);
        return NULL;
    }

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
    SUCCESS_COND_DO(alxIsNotSet(mA) || alxIsNotSet(mB),
                    alxDATAClear((*diam)));

    /** compute diameter and its error */
    alxComputeDiameter(mA, mB, spTypeIndex, polynomial, cmI, cmJ, color, diam);

    /* If diameter is not computed (domain check), return */
    SUCCESS_COND(IS_FALSE(diam->isSet));

    /* Set confidence as the smallest confidence of the two magnitudes */
    diam->confIndex = (mA.confIndex <= mB.confIndex) ? mA.confIndex : mB.confIndex;

    /* If any missing magnitude error (should not happen as error = 0.1 mag if missing error),
     * return diameter with low confidence */
    if ((mA.error <= 0.0) || (mB.error <= 0.0))
    {
        diam->confIndex = alxCONFIDENCE_LOW;
    }

    return mcsSUCCESS;
}

void logMatrix(const char* label, alxDIAMETERS_COVARIANCE matrix)
{
    if (doLog(LOG_MATRIX))
    {
        mcsUINT32 i;

        logP(LOG_MATRIX, "%s matrix of diameters [%d][%d]:", label, alxNB_DIAMS, alxNB_DIAMS);

        logP(LOG_MATRIX, "[%3s][%15s %15s %15s %15s %15s]",
             "Col",
             alxGetDiamLabel(0), alxGetDiamLabel(1), alxGetDiamLabel(2), alxGetDiamLabel(3), alxGetDiamLabel(4)
             );

        /* log covariance the matrix */
        for (i = 0; i < alxNB_DIAMS; i++) /* II */
        {
            logP(LOG_MATRIX, "[%s][%15lg %15lg %15lg %15lg %15lg]",
                 alxGetDiamLabel(i),
                 matrix[i][0], matrix[i][1], matrix[i][2], matrix[i][3], matrix[i][4]
                 );
        }
    }
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
                                         alxDIAMETERS_COVARIANCE diametersCov)
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
                     * no common mangitude:
                     * term_M = 0
                     */
                    term_M = 0.0;
                }

                /* DCOV_C(JJ,II,*)=DCOV_C(II,JJ,*) */
                diametersCov[j][i] = diametersCov[i][j] = term_an + term_M;
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

    alxLogTestAngularDiameters(msg, diameters);

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
                                            alxDATA     *maxResidualDiam,
                                            alxDATA     *chi2Diam,
                                            alxDATA     *maxCorrelation,
                                            mcsUINT32   *nbDiameters,
                                            miscDYN_BUF *diamInfo)
{
    /*
     * Only use diameters with HIGH confidence
     * ie computed from catalog magnitudes and not interpolated magnitudes.
     */

    /* initialize structures */
    alxDATAClear((*weightedMeanDiam));
    alxDATAClear((*maxResidualDiam));
    alxDATAClear((*chi2Diam));
    alxDATAClear((*maxCorrelation));

    mcsUINT32   color;
    mcsLOGICAL  consistent = mcsTRUE;
    mcsSTRING32 tmp;
    alxDATA     diameter;
    mcsDOUBLE   diamRelError;

    /* valid diameters to compute weighted mean diameter and their errors */
    mcsUINT32 nValidDiameters;
    mcsDOUBLE validDiams[alxNB_DIAMS];
    mcsUINT32 validDiamsBand[alxNB_DIAMS];
    mcsDOUBLE validDiamsVariance[alxNB_DIAMS];

    /* residual (diameter vs weighted mean diameter) */
    mcsDOUBLE residual;
    mcsDOUBLE residuals[alxNB_DIAMS];
    mcsDOUBLE maxResidual = 0.0;
    mcsDOUBLE chi2 = NAN;


    /* check correlation in covariance matrix */
    static const mcsDOUBLE THRESHOLD_CORRELATION = 0.99;


    /* Count only valid diameters and copy data into diameter arrays */
    for (nValidDiameters = 0, color = 0; color < alxNB_DIAMS; color++)
    {
        diameter  = diameters[color];

        if (isDiameterValid(diameter))
        {
            validDiamsBand[nValidDiameters]     = color;

            /* convert diameter and error to log(diameter) and relative error */
            validDiams[nValidDiameters]         = log10(diameter.value);                    /* ALOG10(DIAM_C) */
            diamRelError                        = relError(diameter.value, diameter.error); /* B=EDIAM_C / (DIAM_C * ALOG(10.)) */
            validDiamsVariance[nValidDiameters] = alxSquare(diamRelError);                  /* B^2 */
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
            sprintf(tmp, "REQUIRED_DIAMETERS (%1u): %1u", nbRequiredDiameters, nValidDiameters);
            miscDynBufAppendString(diamInfo, tmp);
        }

        return mcsSUCCESS;
    }

    /* Compute correlations */
    mcsDOUBLE maxCor = 0.0; /* max(correlation coefficients) */
    mcsUINT32 nThCor = 0;
    mcsUINT32 i, j;

    if (IS_TRUE(CHECK_HIGH_CORRELATION))
    {
        mcsDOUBLE maxCors[alxNB_DIAMS];
        alxDIAMETERS_COVARIANCE diamCorrelations;
        mcsDOUBLE max;

        for (i = 0; i < alxNB_DIAMS; i++) /* II */
        {
            for (j = 0; j < alxNB_DIAMS; j++)
            {
                diamCorrelations[i][j] = (i < nValidDiameters) ? ( (j < nValidDiameters) ?
                        (diametersCov[i][j] / sqrt(diametersCov[i][i] * diametersCov[j][j])) : NAN ) : NAN;
            }

            /* check max(correlation) out of diagonal (1) */
            max = 0.0;
            for (j = 0; j < alxNB_DIAMS; j++)
            {
                if (i != j)
                {
                    if (!isnan(diamCorrelations[i][j]) && (diamCorrelations[i][j] > max))
                    {
                        max = diamCorrelations[i][j];
                    }
                }
            }
            maxCors[i] = max;
            if (maxCors[i] > maxCor)
            {
                maxCor = maxCors[i];
                if (maxCor > THRESHOLD_CORRELATION)
                {
                    nThCor++;
                }
            }
        }

        if (maxCor > THRESHOLD_CORRELATION)
        {
            logP(LOG_MATRIX, "Max Correlation=%.6lf from [%15.6lf %15.6lf %15.6lf]",
                 maxCor, maxCors[0], maxCors[1], maxCors[2]
                 );
        }

        /* Set the maximum correlation */
        maxCorrelation->isSet     = mcsTRUE;
        maxCorrelation->confIndex = alxCONFIDENCE_HIGH;
        maxCorrelation->value     = maxCor;

        if (IS_TRUE(FILTER_HIGH_CORRELATION) && (nThCor != 0))
        {
            /* at least 2 colors are too much correlated => eliminate redundant colors */

            logMatrix("Covariance",  diametersCov);
            logMatrix("Correlation", diamCorrelations);


            mcsUINT32 nBands = 0;
            mcsUINT32 uncorBands[alxNB_DIAMS];

            /** preferred bands when discarding correlated colors */
            static const mcsUINT32 preferedBands[alxNB_DIAMS] = { alxV_K_DIAM, alxV_H_DIAM, alxV_J_DIAM };

            nThCor = 0;
            for (j = 0; j < alxNB_DIAMS; j++)
            {
                /* use preferred bands first */
                i = preferedBands[j];
                uncorBands[nBands] = i;

                if (maxCors[i] > THRESHOLD_CORRELATION)
                {
                    if (nThCor != 0)
                    {
                        /* discard correlated color */
                        logTest("remove correlated color: %s", alxGetDiamLabel(i));
                    }
                    else
                    {
                        logTest("keep first correlated color: %s", alxGetDiamLabel(i));
                        nBands++;
                    }
                    nThCor++;
                }
                else
                {
                    nBands++;
                }
            }

            /* extract again valid diameters */
            for (nValidDiameters = 0, i = 0; i < nBands; i++)
            {
                color = uncorBands[i];
                diameter  = diameters[color];

                if (isDiameterValid(diameter))
                {
                    validDiamsBand[nValidDiameters]     = color;

                    /* convert diameter and error to log(diameter) and relative error */
                    validDiams[nValidDiameters]         = log10(diameter.value);                    /* ALOG10(DIAM_C) */
                    diamRelError                        = relError(diameter.value, diameter.error); /* B=EDIAM_C / (DIAM_C * ALOG(10.)) */
                    validDiamsVariance[nValidDiameters] = alxSquare(diamRelError);                  /* B^2 */

                    nValidDiameters++;
                }
            }

            /* recompute max correlations on remaining colors */
            for (i = 0; i < alxNB_DIAMS; i++)
            {
                maxCors[i] = 0.0;
            }

            maxCor = 0.0;
            mcsUINT32 k, l;
            for (k = 0; k < nValidDiameters; k++) /* II */
            {
                i = validDiamsBand[k];

                for (l = 0; l < nValidDiameters; l++)
                {
                    j = validDiamsBand[l];
                    diamCorrelations[k][l] = diametersCov[i][j] / sqrt(diametersCov[i][i] * diametersCov[j][j]);
                }

                /* check max(correlation) out of diagonal (1) */
                max = 0.0;
                for (l = 0; l < nValidDiameters; l++)
                {
                    j = validDiamsBand[l];
                    if (i != j)
                    {
                        if (diamCorrelations[k][l] > max)
                        {
                            max = diamCorrelations[k][l];
                        }
                    }
                }
                maxCors[i] = max;
                if (maxCors[i] > maxCor)
                {
                    maxCor = maxCors[i];
                }
            }

            logP(LOG_MATRIX, "Corrected max Correlation=%.6lf from [%15.6lf %15.6lf %15.6lf %15.6lf %15.6lf]",
                 maxCor, maxCors[0], maxCors[1], maxCors[2], maxCors[3], maxCors[4]
                 );

            /* Update the maximum correlation */
            maxCorrelation->value = maxCor;

            /* Update the diameter count */
            *nbDiameters = nValidDiameters;

            /* if less than required diameters, can not compute mean diameter... */
            if (nValidDiameters < nbRequiredDiameters)
            {
                logTest("Cannot compute mean diameter (%d < %d valid diameters)", nValidDiameters, nbRequiredDiameters);

                if (IS_NOT_NULL(diamInfo))
                {
                    /* Set diameter flag information */
                    sprintf(tmp, "REQUIRED_DIAMETERS (%1u): %1u", nbRequiredDiameters, nValidDiameters);
                    miscDynBufAppendString(diamInfo, tmp);
                }

                return mcsSUCCESS;
            }
        }

    } /* check high correlations */


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
        if (nThCor != 0)
        {
            if (doLog(LOG_MATRIX))
            {
                /* log inverse of covariance matrix */
                alxDIAMETERS_COVARIANCE diamWeights;
                for (i = 0; i < alxNB_DIAMS; i++)
                {
                    for (j = 0; j < alxNB_DIAMS; j++)
                    {
                        diamWeights[i][j] = NAN;
                    }
                }
                for (i = 0; i < nValidDiameters; i++)
                {
                    for (j = 0; j < nValidDiameters; j++)
                    {
                        diamWeights[validDiamsBand[i]][validDiamsBand[j]] = icov[i * nValidDiameters + j];
                    }
                }
                logMatrix("Inverse of Covariance",  diamWeights);
            }
        }


        mcsDOUBLE total_icov = alxTotal(nValidDiameters * nValidDiameters, icov); /* TOTAL(M) */

        /* IF (TOTAL(M) GT 0) THEN BEGIN */
        if (total_icov > 0.0)
        {
            mcsDOUBLE matrix_prod[nValidDiameters];

            weightedMeanDiam->isSet = mcsTRUE;
            weightedMeanDiam->confIndex = alxCONFIDENCE_HIGH;

            /* compute icov#diameters
             * A=TOTAL(ALOG10(DIAM_C(II,*)),1)
             * matrix_prod = M#A */
            alxProductMatrix(icov, validDiams, matrix_prod, nValidDiameters, nValidDiameters, 1);

            /* Convert log normal diameter distribution to normal distribution */
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
            chi2 = matrix_11[0] / nValidDiameters;


            /* Compute residuals and find the max residual (number of sigma) on all diameters */
            for (i = 0; i < nValidDiameters; i++)
            {
                /* RES_C(WWS(II),*)=DIF/SQRT((EDMEAN_C(WWS(II))^2+EDIAM_C(WWS(II),*)^2)) */
                residual = residuals[i] /= sqrt(weightedMeanDiamVariance + validDiamsVariance[i]);

                if (residual > maxResidual)
                {
                    maxResidual = residual;
                }
            }

            if (IS_NOT_NULL(diamInfo) && (maxResidual > LOG_RESIDUAL_THRESHOLD))
            {
                /* Report high tolerance between weighted mean diameter and individual diameters in diameter flag information */
                for (i = 0; i < nValidDiameters; i++)
                {
                    residual = residuals[i];
                    color    = validDiamsBand[i];

                    if (residual > LOG_RESIDUAL_THRESHOLD)
                    {
                        if (IS_TRUE(consistent))
                        {
                            consistent = mcsFALSE;

                            /* Set diameter flag information */
                            miscDynBufAppendString(diamInfo, "WEAK_CONSISTENT_DIAMETER ");
                        }

                        /* Set confidence to LOW for each weak consistent diameter */
                        diameters[color].confIndex = alxCONFIDENCE_LOW;

                        /* Append each color (tolerance) in diameter flag information */
                        sprintf(tmp, "%s (%.1lf) ", alxGetDiamLabel(color), residual);
                        miscDynBufAppendString(diamInfo, tmp);
                    }
                }
            }

            /* Check if max(residuals) < 5 or chi2 < 9
             * If higher i.e. inconsistency is found, the weighted mean diameter has a LOW confidence */
            if ((maxResidual > MAX_RESIDUAL_THRESHOLD) || (chi2 > DIAM_CHI2_THRESHOLD))
            {
                /* Set confidence to LOW */
                weightedMeanDiam->confIndex = alxCONFIDENCE_LOW;

                if (IS_NOT_NULL(diamInfo))
                {
                    /* Update diameter flag information */
                    miscDynBufAppendString(diamInfo, "INCONSISTENT_DIAMETERS ");
                }
            }

            /* Store max tolerance into diameter quality value */
            maxResidualDiam->isSet = mcsTRUE;
            maxResidualDiam->confIndex = alxCONFIDENCE_HIGH;
            maxResidualDiam->value = maxResidual;
        } /* ENDIF */

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
        chi2Diam->confIndex = alxCONFIDENCE_HIGH;
        chi2Diam->value = chi2;
    }

    logTest("Diameter weighted=%.4lf(%.4lf %.1lf%%) valid=%s [%s] tolerance=%.2lf chi2=%.4lf from %d diameters: %s",
            weightedMeanDiam->value, weightedMeanDiam->error, alxDATALogRelError(*weightedMeanDiam),
            (weightedMeanDiam->confIndex == alxCONFIDENCE_HIGH) ? "true" : "false",
            alxGetConfidenceIndex(weightedMeanDiam->confIndex),
            maxResidual, chi2, nValidDiameters,
            IS_NOT_NULL(diamInfo) ? miscDynBufGetBuffer(diamInfo) : "");

    return mcsSUCCESS;
}


#define copyMag(mag, band, offset) \
mag[band].isSet = mcsTRUE; \
mag[band].value = ROW[I_MAG  + offset]; \
mag[band].error = ROW[I_EMAG + offset]; \
mag[band].confIndex = alxCONFIDENCE_HIGH;

#define copyDiam(diams, band, offset) \
diams[band].isSet = mcsTRUE; \
diams[band].value = ROW[I_DIAM  + offset]; \
diams[band].error = ROW[I_EDIAM + offset]; \
diams[band].confIndex = alxCONFIDENCE_HIGH;

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

    mcsUINT32 NS = 486;
    /* #REF STAR DATA:         486 */
    mcsDOUBLE datas[486][2 + 6 + 6 + 4 + 4 + 2] = {
        {0, /* SP */ 52, /*MAG*/  3.050000000e-01,  1.800000000e-01,  1.500000000e-01,  2.060000000e-01,  1.760000000e-01,  2.130000000e-01, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.280000000e-01,  3.520000000e-01,  3.960000000e-01, /*DIAM*/  5.009266810e-01,  4.361052408e-01,  4.475947109e-01,  4.313266962e-01, /*EDIAM*/  4.162711671e-02,  6.361723219e-02,  8.517156134e-02,  8.977810293e-02, /*DMEAN*/  4.714529490e-01, /*EDMEAN*/  2.980056647e-02 },
        {1, /* SP */ 28, /*MAG*/  1.416000000e+00,  1.640000000e+00,  1.860000000e+00,  2.149000000e+00,  2.357000000e+00,  2.375000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.820000000e-01,  1.680000000e-01,  2.560000000e-01, /*DIAM*/ -1.140781490e-01, -1.071086832e-01, -1.167481210e-01, -1.199025758e-01, /*EDIAM*/  4.215539991e-02,  7.901896679e-02,  4.166170703e-02,  5.857814044e-02, /*DMEAN*/ -1.153717462e-01, /*EDMEAN*/  2.489066739e-02 },
        {2, /* SP */ 20, /*MAG*/  1.506000000e+00,  1.690000000e+00,  1.850000000e+00,  2.191000000e+00,  2.408000000e+00,  2.273000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  3.240000000e-01,  1.800000000e-01,  2.820000000e-01, /*DIAM*/ -1.358710475e-01, -1.724819212e-01, -1.706514356e-01, -1.414776412e-01, /*EDIAM*/  4.291333793e-02,  9.126865275e-02,  4.539151159e-02,  6.494940460e-02, /*DMEAN*/ -1.520456152e-01, /*EDMEAN*/  2.692087006e-02 },
        {3, /* SP */ 100, /*MAG*/ -4.560000000e-01, -6.200000000e-01, -8.500000000e-01, -1.169000000e+00, -1.280000000e+00, -1.304000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  1.130000000e-01,  1.900000000e-01,  2.480000000e-01, /*DIAM*/  8.027307302e-01,  8.500159581e-01,  8.573381735e-01,  8.435342667e-01, /*EDIAM*/  4.141222888e-02,  3.178180073e-02,  4.605456211e-02,  5.623460019e-02, /*DMEAN*/  8.380936089e-01, /*EDMEAN*/  1.993308232e-02 },
        {4, /* SP */ 64, /*MAG*/ -1.431000000e+00, -1.440000000e+00, -1.420000000e+00, -1.391000000e+00, -1.391000000e+00, -1.390000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  1.090000000e-01,  1.840000000e-01,  2.140000000e-01, /*DIAM*/  7.976485391e-01,  7.895688542e-01,  7.926104850e-01,  7.847986050e-01, /*EDIAM*/  4.156156435e-02,  3.112209404e-02,  4.482744732e-02,  4.871344905e-02, /*DMEAN*/  7.913889618e-01, /*EDMEAN*/  1.930752922e-02 },
        {5, /* SP */ 28, /*MAG*/  1.289000000e+00,  1.500000000e+00,  1.700000000e+00,  1.977000000e+00,  2.158000000e+00,  2.222000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  3.020000000e-01,  2.860000000e-01,  3.080000000e-01, /*DIAM*/ -7.801814849e-02, -7.025153982e-02, -7.451465730e-02, -8.896096947e-02, /*EDIAM*/  4.215539991e-02,  8.449145380e-02,  6.966739660e-02,  7.023556511e-02, /*DMEAN*/ -7.833965339e-02, /*EDMEAN*/  2.974025824e-02 },
        {6, /* SP */ 52, /*MAG*/  1.273000000e+00,  1.360000000e+00,  1.460000000e+00,  1.665000000e+00,  1.658000000e+00,  1.640000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  3.140000000e-01,  1.860000000e-01,  2.120000000e-01, /*DIAM*/  1.334866755e-01,  1.228820219e-01,  1.387386752e-01,  1.394361809e-01, /*EDIAM*/  4.162711671e-02,  8.728180183e-02,  4.534908398e-02,  4.829181471e-02, /*DMEAN*/  1.357171016e-01, /*EDMEAN*/  2.442537046e-02 },
        {7, /* SP */ 60, /*MAG*/  2.900000000e-02,  3.000000000e-02,  4.000000000e-02, -1.770000000e-01, -2.900000000e-02,  1.290000000e-01, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.060000000e-01,  1.460000000e-01,  1.860000000e-01, /*DIAM*/  4.835040149e-01,  5.557847016e-01,  5.143860913e-01,  4.696410310e-01, /*EDIAM*/  4.158344239e-02,  5.754727116e-02,  3.579414408e-02,  4.243305745e-02, /*DMEAN*/  5.007332117e-01, /*EDMEAN*/  2.083208076e-02 },
        {8, /* SP */ 28, /*MAG*/  1.822000000e+00,  1.940000000e+00,  2.040000000e+00,  2.304000000e+00,  2.458000000e+00,  2.479000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  3.120000000e-01,  2.180000000e-01,  2.820000000e-01, /*DIAM*/ -1.083581489e-01, -1.269747550e-01, -1.287403390e-01, -1.355522110e-01, /*EDIAM*/  4.215539991e-02,  8.723068951e-02,  5.346908145e-02,  6.440189743e-02, /*DMEAN*/ -1.207748487e-01, /*EDMEAN*/  2.767564446e-02 },
        {9, /* SP */ 44, /*MAG*/  1.660000000e+00,  1.730000000e+00,  1.780000000e+00,  2.021000000e+00,  2.027000000e+00,  2.016000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  3.500000000e-01,  2.280000000e-01,  2.440000000e-01, /*DIAM*/  3.624183978e-02,  2.141028245e-02,  3.712766800e-02,  3.616552862e-02, /*EDIAM*/  4.168798539e-02,  9.724487672e-02,  5.542889297e-02,  5.552166016e-02, /*DMEAN*/  3.523947735e-02, /*EDMEAN*/  2.703604389e-02 },
        {10, /* SP */ 76, /*MAG*/  1.315000000e+00,  1.170000000e+00,  1.010000000e+00,  1.037000000e+00,  9.370000000e-01,  9.450000000e-01, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.340000000e-01,  2.560000000e-01,  2.860000000e-01, /*DIAM*/  3.952827907e-01,  3.436178144e-01,  3.642054831e-01,  3.498999950e-01, /*EDIAM*/  4.149458958e-02,  6.514702869e-02,  6.202308459e-02,  6.489767592e-02, /*DMEAN*/  3.717682351e-01, /*EDMEAN*/  2.707546299e-02 },
        {11, /* SP */ 228, /*MAG*/  5.356000000e+00,  3.730000000e+00,  1.660000000e+00,  4.230000000e-01, -4.300000000e-01, -6.680000000e-01, /*EMAG*/  4.428318000e-02,  3.999999911e-02,  3.999999911e-02,  1.820000000e-01,  2.360000000e-01,  3.260000000e-01, /*DIAM*/  9.800736497e-01,  8.964066212e-01,  9.424313875e-01,  9.446490824e-01, /*EDIAM*/  4.295994481e-02,  5.059835885e-02,  5.704748360e-02,  7.382614381e-02, /*DMEAN*/  9.445549928e-01, /*EDMEAN*/  2.589794562e-02 },
        {12, /* SP */ 0, /*MAG*/  1.941000000e+00,  2.210000000e+00,  2.430000000e+00,  2.790000000e+00,  2.955000000e+00,  2.968000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  3.020000000e-01,  2.560000000e-01,  2.680000000e-01, /*DIAM*/ -3.722525769e-01, -4.969469469e-01, -4.187978449e-01, -4.345122276e-01, /*EDIAM*/  4.936630489e-02,  9.213932707e-02,  6.882374385e-02,  6.669970974e-02, /*DMEAN*/ -4.108693122e-01, /*EDMEAN*/  3.379769588e-02 },
        {13, /* SP */ 224, /*MAG*/  6.797000000e+00,  5.150000000e+00,  3.130000000e+00,  1.839000000e+00,  9.960000000e-01,  8.340000000e-01, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  5.024938000e-02,  3.000000000e-01,  3.120000000e-01,  3.500000000e-01, /*DIAM*/  6.682455641e-01,  6.124400374e-01,  6.481132398e-01,  6.347502884e-01, /*EDIAM*/  4.126750894e-02,  8.316472474e-02,  7.534453082e-02,  7.924584100e-02, /*DMEAN*/  6.522448988e-01, /*EDMEAN*/  3.012143758e-02 },
        {14, /* SP */ 194, /*MAG*/  6.299000000e+00,  4.770000000e+00,  3.150000000e+00,  2.160000000e+00,  1.390000000e+00,  1.247000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  4.031129000e-02,  2.660000000e-01,  2.440000000e-01,  2.640000000e-01, /*DIAM*/  5.107315488e-01,  4.632391815e-01,  4.896251564e-01,  4.844581124e-01, /*EDIAM*/  4.123086059e-02,  7.377820551e-02,  5.895503883e-02,  5.980145742e-02, /*DMEAN*/  4.942071152e-01, /*EDMEAN*/  2.681344018e-02 },
        {15, /* SP */ 232, /*MAG*/  4.491000000e+00,  2.870000000e+00,  5.700000000e-01, -9.120000000e-01, -1.608000000e+00, -1.862000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  5.035871000e-02,  1.880000000e-01,  1.900000000e-01,  1.800000000e-01, /*DIAM*/  1.195608009e+00,  1.199869035e+00,  1.200683158e+00,  1.199790111e+00, /*EDIAM*/  4.140373314e-02,  5.229214929e-02,  4.601931072e-02,  4.087339370e-02, /*DMEAN*/  1.198756934e+00, /*EDMEAN*/  2.175318215e-02 },
        {16, /* SP */ 232, /*MAG*/  4.491000000e+00,  2.870000000e+00,  5.700000000e-01, -9.120000000e-01, -1.608000000e+00, -1.862000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  5.035871000e-02,  1.880000000e-01,  1.900000000e-01,  1.800000000e-01, /*DIAM*/  1.195608009e+00,  1.199869035e+00,  1.200683158e+00,  1.199790111e+00, /*EDIAM*/  4.140373314e-02,  5.229214929e-02,  4.601931072e-02,  4.087339370e-02, /*DMEAN*/  1.198756934e+00, /*EDMEAN*/  2.175318215e-02 },
        {17, /* SP */ 200, /*MAG*/  2.408000000e+00,  8.700000000e-01, -8.000000000e-01, -2.095000000e+00, -2.775000000e+00, -3.044000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  1.940000000e-01,  1.700000000e-01,  1.400000000e-01, /*DIAM*/  1.311522700e+00,  1.349936934e+00,  1.340728556e+00,  1.361062830e+00, /*EDIAM*/  4.123976885e-02,  5.391790149e-02,  4.115293056e-02,  3.180987386e-02, /*DMEAN*/  1.342580427e+00, /*EDMEAN*/  1.951298048e-02 },
        {18, /* SP */ 44, /*MAG*/  2.920000000e-01,  4.500000000e-01,  6.200000000e-01,  8.150000000e-01,  8.650000000e-01,  8.800000000e-01, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.540000000e-01,  3.200000000e-01,  3.300000000e-01, /*DIAM*/  2.376818428e-01,  2.569281431e-01,  2.646607453e-01,  2.595815903e-01, /*EDIAM*/  4.168798539e-02,  7.081237658e-02,  7.750183075e-02,  7.490102155e-02, /*DMEAN*/  2.485705898e-01, /*EDMEAN*/  2.942037734e-02 },
        {19, /* SP */ 52, /*MAG*/  3.050000000e-01,  1.800000000e-01,  1.500000000e-01,  2.060000000e-01,  1.760000000e-01,  2.130000000e-01, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.280000000e-01,  3.520000000e-01,  3.960000000e-01, /*DIAM*/  5.009266810e-01,  4.361052408e-01,  4.475947109e-01,  4.313266962e-01, /*EDIAM*/  4.162711671e-02,  6.361723219e-02,  8.517156134e-02,  8.977810293e-02, /*DMEAN*/  4.714529490e-01, /*EDMEAN*/  2.980056647e-02 },
        {20, /* SP */ 28, /*MAG*/  1.416000000e+00,  1.640000000e+00,  1.860000000e+00,  2.149000000e+00,  2.357000000e+00,  2.375000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.820000000e-01,  1.680000000e-01,  2.560000000e-01, /*DIAM*/ -1.140781490e-01, -1.071086832e-01, -1.167481210e-01, -1.199025758e-01, /*EDIAM*/  4.215539991e-02,  7.901896679e-02,  4.166170703e-02,  5.857814044e-02, /*DMEAN*/ -1.153717462e-01, /*EDMEAN*/  2.489066739e-02 },
        {21, /* SP */ 20, /*MAG*/  1.506000000e+00,  1.690000000e+00,  1.850000000e+00,  2.191000000e+00,  2.408000000e+00,  2.273000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  3.240000000e-01,  1.800000000e-01,  2.820000000e-01, /*DIAM*/ -1.358710475e-01, -1.724819212e-01, -1.706514356e-01, -1.414776412e-01, /*EDIAM*/  4.291333793e-02,  9.126865275e-02,  4.539151159e-02,  6.494940460e-02, /*DMEAN*/ -1.520456152e-01, /*EDMEAN*/  2.692087006e-02 },
        {22, /* SP */ 100, /*MAG*/ -4.560000000e-01, -6.200000000e-01, -8.500000000e-01, -1.169000000e+00, -1.280000000e+00, -1.304000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  1.130000000e-01,  1.900000000e-01,  2.480000000e-01, /*DIAM*/  8.027307302e-01,  8.500159581e-01,  8.573381735e-01,  8.435342667e-01, /*EDIAM*/  4.141222888e-02,  3.178180073e-02,  4.605456211e-02,  5.623460019e-02, /*DMEAN*/  8.380936089e-01, /*EDMEAN*/  1.993308232e-02 },
        {23, /* SP */ 64, /*MAG*/ -1.431000000e+00, -1.440000000e+00, -1.420000000e+00, -1.391000000e+00, -1.391000000e+00, -1.390000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  1.090000000e-01,  1.840000000e-01,  2.140000000e-01, /*DIAM*/  7.976485391e-01,  7.895688542e-01,  7.926104850e-01,  7.847986050e-01, /*EDIAM*/  4.156156435e-02,  3.112209404e-02,  4.482744732e-02,  4.871344905e-02, /*DMEAN*/  7.913889618e-01, /*EDMEAN*/  1.930752922e-02 },
        {24, /* SP */ 28, /*MAG*/  1.289000000e+00,  1.500000000e+00,  1.700000000e+00,  1.977000000e+00,  2.158000000e+00,  2.222000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  3.020000000e-01,  2.860000000e-01,  3.080000000e-01, /*DIAM*/ -7.801814849e-02, -7.025153982e-02, -7.451465730e-02, -8.896096947e-02, /*EDIAM*/  4.215539991e-02,  8.449145380e-02,  6.966739660e-02,  7.023556511e-02, /*DMEAN*/ -7.833965339e-02, /*EDMEAN*/  2.974025824e-02 },
        {25, /* SP */ 132, /*MAG*/  2.501000000e+00,  1.830000000e+00,  1.160000000e+00,  7.710000000e-01,  5.370000000e-01,  4.530000000e-01, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.220000000e-01,  3.000000000e-01,  3.340000000e-01, /*DIAM*/  6.127969733e-01,  5.344738537e-01,  5.389775895e-01,  5.335620656e-01, /*EDIAM*/  4.136756490e-02,  6.166023735e-02,  7.247329207e-02,  7.562825984e-02, /*DMEAN*/  5.728998848e-01, /*EDMEAN*/  2.813962653e-02 },
        {26, /* SP */ 40, /*MAG*/  2.367000000e+00,  2.450000000e+00,  2.440000000e+00,  2.439000000e+00,  2.492000000e+00,  2.442000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.540000000e-01,  2.360000000e-01,  2.680000000e-01, /*DIAM*/ -1.335783546e-01, -5.736300733e-02, -6.104832029e-02, -5.721752691e-02, /*EDIAM*/  4.174031488e-02,  7.086777287e-02,  5.738218826e-02,  6.095753869e-02, /*DMEAN*/ -9.102187079e-02, /*EDMEAN*/  2.684960851e-02 },
        {27, /* SP */ 0, /*MAG*/  1.941000000e+00,  2.210000000e+00,  2.430000000e+00,  2.790000000e+00,  2.955000000e+00,  2.968000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  3.020000000e-01,  2.560000000e-01,  2.680000000e-01, /*DIAM*/ -3.722525769e-01, -4.969469469e-01, -4.187978449e-01, -4.345122276e-01, /*EDIAM*/  4.936630489e-02,  9.213932707e-02,  6.882374385e-02,  6.669970974e-02, /*DMEAN*/ -4.108693122e-01, /*EDMEAN*/  3.379769588e-02 },
        {28, /* SP */ 68, /*MAG*/  1.740000000e+00,  1.670000000e+00,  1.650000000e+00,  1.493000000e+00,  1.500000000e+00,  1.487000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  3.180000000e-01,  2.160000000e-01,  2.400000000e-01, /*DIAM*/  2.263688811e-01,  2.396394471e-01,  2.328072279e-01,  2.246544920e-01, /*EDIAM*/  4.153908537e-02,  8.833362367e-02,  5.246506015e-02,  5.455369292e-02, /*DMEAN*/  2.288499203e-01, /*EDMEAN*/  2.623154697e-02 },
        {29, /* SP */ 52, /*MAG*/  1.273000000e+00,  1.360000000e+00,  1.460000000e+00,  1.665000000e+00,  1.658000000e+00,  1.640000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  3.140000000e-01,  1.860000000e-01,  2.120000000e-01, /*DIAM*/  1.334866755e-01,  1.228820219e-01,  1.387386752e-01,  1.394361809e-01, /*EDIAM*/  4.162711671e-02,  8.728180183e-02,  4.534908398e-02,  4.829181471e-02, /*DMEAN*/  1.357171016e-01, /*EDMEAN*/  2.442537046e-02 },
        {30, /* SP */ 72, /*MAG*/  2.230000000e+00,  2.140000000e+00,  2.040000000e+00,  1.854000000e+00,  1.925000000e+00,  1.883000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.740000000e-01,  1.940000000e-01,  1.920000000e-01, /*DIAM*/  1.565585764e-01,  1.843273289e-01,  1.581667400e-01,  1.556674817e-01, /*EDIAM*/  4.151649521e-02,  7.618533422e-02,  4.717485240e-02,  4.373626256e-02, /*DMEAN*/  1.595229189e-01, /*EDMEAN*/  2.366295973e-02 },
        {31, /* SP */ 52, /*MAG*/  2.473000000e+00,  2.580000000e+00,  2.680000000e+00,  2.848000000e+00,  2.975000000e+00,  2.945000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.880000000e-01,  2.200000000e-01,  2.760000000e-01, /*DIAM*/ -1.229133283e-01, -1.108769102e-01, -1.286621070e-01, -1.237973997e-01, /*EDIAM*/  4.162711671e-02,  8.011871221e-02,  5.347823271e-02,  6.269946566e-02, /*DMEAN*/ -1.231438335e-01, /*EDMEAN*/  2.690855893e-02 },
        {32, /* SP */ 80, /*MAG*/  2.235000000e+00,  2.080000000e+00,  1.910000000e+00,  1.833000000e+00,  1.718000000e+00,  1.683000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.800000000e-01,  1.760000000e-01,  2.060000000e-01, /*DIAM*/  2.288983364e-01,  2.000435609e-01,  2.202679223e-01,  2.135655476e-01, /*EDIAM*/  4.147421938e-02,  7.779492832e-02,  4.281407926e-02,  4.684608903e-02, /*DMEAN*/  2.195585467e-01, /*EDMEAN*/  2.348652408e-02 },
        {33, /* SP */ 58, /*MAG*/  1.759000000e+00,  1.790000000e+00,  1.780000000e+00,  1.733000000e+00,  1.768000000e+00,  1.771000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.740000000e-01,  2.280000000e-01,  2.540000000e-01, /*DIAM*/  1.055641495e-01,  1.564836817e-01,  1.479603782e-01,  1.389324321e-01, /*EDIAM*/  4.159416852e-02,  7.624473637e-02,  5.537808129e-02,  5.773167452e-02, /*DMEAN*/  1.291295899e-01, /*EDMEAN*/  2.650891299e-02 },
        {34, /* SP */ 60, /*MAG*/  2.900000000e-02,  3.000000000e-02,  4.000000000e-02, -1.770000000e-01, -2.900000000e-02,  1.290000000e-01, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.060000000e-01,  1.460000000e-01,  1.860000000e-01, /*DIAM*/  4.835040149e-01,  5.557847016e-01,  5.143860913e-01,  4.696410310e-01, /*EDIAM*/  4.158344239e-02,  5.754727116e-02,  3.579414408e-02,  4.243305745e-02, /*DMEAN*/  5.007332117e-01, /*EDMEAN*/  2.083208076e-02 },
        {35, /* SP */ 28, /*MAG*/  1.822000000e+00,  1.940000000e+00,  2.040000000e+00,  2.304000000e+00,  2.458000000e+00,  2.479000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  3.120000000e-01,  2.180000000e-01,  2.820000000e-01, /*DIAM*/ -1.083581489e-01, -1.269747550e-01, -1.287403390e-01, -1.355522110e-01, /*EDIAM*/  4.215539991e-02,  8.723068951e-02,  5.346908145e-02,  6.440189743e-02, /*DMEAN*/ -1.207748487e-01, /*EDMEAN*/  2.767564446e-02 },
        {36, /* SP */ 44, /*MAG*/  1.660000000e+00,  1.730000000e+00,  1.780000000e+00,  2.021000000e+00,  2.027000000e+00,  2.016000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  3.500000000e-01,  2.280000000e-01,  2.440000000e-01, /*DIAM*/  3.624183978e-02,  2.141028245e-02,  3.712766800e-02,  3.616552862e-02, /*EDIAM*/  4.168798539e-02,  9.724487672e-02,  5.542889297e-02,  5.552166016e-02, /*DMEAN*/  3.523947735e-02, /*EDMEAN*/  2.703604389e-02 },
        {37, /* SP */ 76, /*MAG*/  1.315000000e+00,  1.170000000e+00,  1.010000000e+00,  1.037000000e+00,  9.370000000e-01,  9.450000000e-01, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.340000000e-01,  2.560000000e-01,  2.860000000e-01, /*DIAM*/  3.952827907e-01,  3.436178144e-01,  3.642054831e-01,  3.498999950e-01, /*EDIAM*/  4.149458958e-02,  6.514702869e-02,  6.202308459e-02,  6.489767592e-02, /*DMEAN*/  3.717682351e-01, /*EDMEAN*/  2.707546299e-02 },
        {38, /* SP */ 232, /*MAG*/  4.491000000e+00,  2.870000000e+00,  5.700000000e-01, -9.120000000e-01, -1.608000000e+00, -1.862000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  5.035871000e-02,  1.880000000e-01,  1.900000000e-01,  1.800000000e-01, /*DIAM*/  1.195608009e+00,  1.199869035e+00,  1.200683158e+00,  1.199790111e+00, /*EDIAM*/  4.140373314e-02,  5.229214929e-02,  4.601931072e-02,  4.087339370e-02, /*DMEAN*/  1.198756934e+00, /*EDMEAN*/  2.175318215e-02 },
        {39, /* SP */ 232, /*MAG*/  4.491000000e+00,  2.870000000e+00,  5.700000000e-01, -9.120000000e-01, -1.608000000e+00, -1.862000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  5.035871000e-02,  1.880000000e-01,  1.900000000e-01,  1.800000000e-01, /*DIAM*/  1.195608009e+00,  1.199869035e+00,  1.200683158e+00,  1.199790111e+00, /*EDIAM*/  4.140373314e-02,  5.229214929e-02,  4.601931072e-02,  4.087339370e-02, /*DMEAN*/  1.198756934e+00, /*EDMEAN*/  2.175318215e-02 },
        {40, /* SP */ 192, /*MAG*/  6.272000000e+00,  4.970000000e+00,  3.720000000e+00,  2.764000000e+00,  2.192000000e+00,  1.978000000e+00, /*EMAG*/  5.608030000e-02,  3.999999911e-02,  3.999999911e-02,  3.100000000e-01,  2.460000000e-01,  2.520000000e-01, /*DIAM*/  3.263528487e-01,  3.084758736e-01,  3.022322061e-01,  3.217360959e-01, /*EDIAM*/  4.789148529e-02,  8.593216101e-02,  5.943397618e-02,  5.708681742e-02, /*DMEAN*/  3.171989393e-01, /*EDMEAN*/  2.892353915e-02 },
        {41, /* SP */ 232, /*MAG*/  7.629000000e+00,  6.000000000e+00,  3.760000000e+00,  2.252000000e+00,  1.356000000e+00,  1.097000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  5.035871000e-02,  2.540000000e-01,  2.740000000e-01,  3.240000000e-01, /*DIAM*/  5.745679998e-01,  5.644583110e-01,  6.147298416e-01,  6.124835324e-01, /*EDIAM*/  4.140373314e-02,  7.049358426e-02,  6.622801175e-02,  7.339366871e-02, /*DMEAN*/  5.861822607e-01, /*EDMEAN*/  2.835772041e-02 },
        {42, /* SP */ 196, /*MAG*/  5.838000000e+00,  4.390000000e+00,  2.880000000e+00,  1.897000000e+00,  1.220000000e+00,  1.022000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  6.007495000e-02,  3.220000000e-01,  2.500000000e-01,  3.220000000e-01, /*DIAM*/  5.408401866e-01,  5.097389996e-01,  5.172385341e-01,  5.280244280e-01, /*EDIAM*/  4.123418936e-02,  8.925424381e-02,  6.040295872e-02,  7.291568422e-02, /*DMEAN*/  5.299353882e-01, /*EDMEAN*/  2.867700340e-02 },
        {43, /* SP */ 228, /*MAG*/  7.554000000e+00,  5.970000000e+00,  3.770000000e+00,  2.450000000e+00,  1.582000000e+00,  1.444000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  3.180000000e-01,  2.300000000e-01,  2.380000000e-01, /*DIAM*/  5.060336426e-01,  5.073619726e-01,  5.494352726e-01,  5.256125798e-01, /*EDIAM*/  4.131329595e-02,  8.814588109e-02,  5.560297097e-02,  5.393578812e-02, /*DMEAN*/  5.210615124e-01, /*EDMEAN*/  2.643609112e-02 },
        {44, /* SP */ 224, /*MAG*/  6.614000000e+00,  4.930000000e+00,  2.990000000e+00,  1.651000000e+00,  8.140000000e-01,  5.980000000e-01, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  5.035871000e-02,  2.660000000e-01,  2.900000000e-01,  3.040000000e-01, /*DIAM*/  7.351855651e-01,  6.475828950e-01,  6.829459251e-01,  6.823707271e-01, /*EDIAM*/  4.126750894e-02,  7.377047006e-02,  7.004290063e-02,  6.884445477e-02, /*DMEAN*/  7.031538706e-01, /*EDMEAN*/  2.852212604e-02 },
        {45, /* SP */ 196, /*MAG*/  5.838000000e+00,  4.390000000e+00,  2.880000000e+00,  1.897000000e+00,  1.220000000e+00,  1.022000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  6.007495000e-02,  3.220000000e-01,  2.500000000e-01,  3.220000000e-01, /*DIAM*/  5.408401866e-01,  5.097389996e-01,  5.172385341e-01,  5.280244280e-01, /*EDIAM*/  4.123418936e-02,  8.925424381e-02,  6.040295872e-02,  7.291568422e-02, /*DMEAN*/  5.299353882e-01, /*EDMEAN*/  2.867700340e-02 },
        {46, /* SP */ 224, /*MAG*/  6.614000000e+00,  4.930000000e+00,  2.990000000e+00,  1.651000000e+00,  8.140000000e-01,  5.980000000e-01, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  5.035871000e-02,  2.660000000e-01,  2.900000000e-01,  3.040000000e-01, /*DIAM*/  7.351855651e-01,  6.475828950e-01,  6.829459251e-01,  6.823707271e-01, /*EDIAM*/  4.126750894e-02,  7.377047006e-02,  7.004290063e-02,  6.884445477e-02, /*DMEAN*/  7.031538706e-01, /*EDMEAN*/  2.852212604e-02 },
        {47, /* SP */ 228, /*MAG*/  6.269000000e+00,  4.680000000e+00,  2.720000000e+00,  1.530000000e+00,  7.060000000e-01,  4.900000000e-01, /*EMAG*/  4.110961000e-02,  3.999999911e-02,  3.999999911e-02,  2.800000000e-01,  2.140000000e-01,  2.280000000e-01, /*DIAM*/  7.671336465e-01,  6.629512606e-01,  7.075597886e-01,  7.075833854e-01, /*EDIAM*/  4.172989890e-02,  7.764536829e-02,  5.175174550e-02,  5.167659521e-02, /*DMEAN*/  7.257889945e-01, /*EDMEAN*/  2.544561310e-02 },
        {48, /* SP */ 176, /*MAG*/  4.671000000e+00,  3.520000000e+00,  2.430000000e+00,  1.603000000e+00,  1.040000000e+00,  9.760000000e-01, /*EMAG*/  7.111259000e-02,  3.999999911e-02,  3.999999911e-02,  3.120000000e-01,  2.660000000e-01,  2.860000000e-01, /*DIAM*/  5.140610978e-01,  4.938580148e-01,  5.065973011e-01,  4.922878322e-01, /*EDIAM*/  5.502096106e-02,  8.646763926e-02,  6.423706447e-02,  6.475716038e-02, /*DMEAN*/  5.038548203e-01, /*EDMEAN*/  3.215125178e-02 },
        {49, /* SP */ 240, /*MAG*/  7.246000000e+00,  5.780000000e+00,  3.010000000e+00,  1.429000000e+00,  5.260000000e-01,  2.550000000e-01, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.420000000e-01,  2.120000000e-01,  2.580000000e-01, /*DIAM*/  6.297925197e-01,  7.715596430e-01,  8.270439327e-01,  8.129450483e-01, /*EDIAM*/  4.182650566e-02,  6.738538408e-02,  5.151649382e-02,  5.861934585e-02, /*DMEAN*/  7.374677816e-01, /*EDMEAN*/  2.568952846e-02 },
        {50, /* SP */ 194, /*MAG*/  6.299000000e+00,  4.770000000e+00,  3.150000000e+00,  2.160000000e+00,  1.390000000e+00,  1.247000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  4.031129000e-02,  2.660000000e-01,  2.440000000e-01,  2.640000000e-01, /*DIAM*/  5.107315488e-01,  4.632391815e-01,  4.896251564e-01,  4.844581124e-01, /*EDIAM*/  4.123086059e-02,  7.377820551e-02,  5.895503883e-02,  5.980145742e-02, /*DMEAN*/  4.942071152e-01, /*EDMEAN*/  2.681344018e-02 },
        {51, /* SP */ 232, /*MAG*/  4.491000000e+00,  2.870000000e+00,  5.700000000e-01, -9.120000000e-01, -1.608000000e+00, -1.862000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  5.035871000e-02,  1.880000000e-01,  1.900000000e-01,  1.800000000e-01, /*DIAM*/  1.195608009e+00,  1.199869035e+00,  1.200683158e+00,  1.199790111e+00, /*EDIAM*/  4.140373314e-02,  5.229214929e-02,  4.601931072e-02,  4.087339370e-02, /*DMEAN*/  1.198756934e+00, /*EDMEAN*/  2.175318215e-02 },
        {52, /* SP */ 196, /*MAG*/  5.838000000e+00,  4.390000000e+00,  2.880000000e+00,  1.897000000e+00,  1.220000000e+00,  1.022000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  6.007495000e-02,  3.220000000e-01,  2.500000000e-01,  3.220000000e-01, /*DIAM*/  5.408401866e-01,  5.097389996e-01,  5.172385341e-01,  5.280244280e-01, /*EDIAM*/  4.123418936e-02,  8.925424381e-02,  6.040295872e-02,  7.291568422e-02, /*DMEAN*/  5.299353882e-01, /*EDMEAN*/  2.867700340e-02 },
        {53, /* SP */ 224, /*MAG*/  6.614000000e+00,  4.930000000e+00,  2.990000000e+00,  1.651000000e+00,  8.140000000e-01,  5.980000000e-01, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  5.035871000e-02,  2.660000000e-01,  2.900000000e-01,  3.040000000e-01, /*DIAM*/  7.351855651e-01,  6.475828950e-01,  6.829459251e-01,  6.823707271e-01, /*EDIAM*/  4.126750894e-02,  7.377047006e-02,  7.004290063e-02,  6.884445477e-02, /*DMEAN*/  7.031538706e-01, /*EDMEAN*/  2.852212604e-02 },
        {54, /* SP */ 228, /*MAG*/  7.794000000e+00,  6.140000000e+00,  3.990000000e+00,  2.840000000e+00,  1.915000000e+00,  1.705000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.920000000e-01,  2.360000000e-01,  2.720000000e-01, /*DIAM*/  5.154336427e-01,  4.124691140e-01,  4.761123144e-01,  4.710213381e-01, /*EDIAM*/  4.131329595e-02,  8.096083325e-02,  5.704748360e-02,  6.161900964e-02, /*DMEAN*/  4.852321326e-01, /*EDMEAN*/  2.714646044e-02 },
        {55, /* SP */ 228, /*MAG*/  6.269000000e+00,  4.680000000e+00,  2.720000000e+00,  1.530000000e+00,  7.060000000e-01,  4.900000000e-01, /*EMAG*/  4.110961000e-02,  3.999999911e-02,  3.999999911e-02,  2.800000000e-01,  2.140000000e-01,  2.280000000e-01, /*DIAM*/  7.671336465e-01,  6.629512606e-01,  7.075597886e-01,  7.075833854e-01, /*EDIAM*/  4.172989890e-02,  7.764536829e-02,  5.175174550e-02,  5.167659521e-02, /*DMEAN*/  7.257889945e-01, /*EDMEAN*/  2.544561310e-02 },
        {56, /* SP */ 226, /*MAG*/  6.653000000e+00,  5.030000000e+00,  2.990000000e+00,  1.717000000e+00,  8.910000000e-01,  7.010000000e-01, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  5.024938000e-02,  2.960000000e-01,  2.620000000e-01,  3.080000000e-01, /*DIAM*/  6.970915736e-01,  6.376581696e-01,  6.728481669e-01,  6.653353571e-01, /*EDIAM*/  4.128610059e-02,  8.206103029e-02,  6.330099083e-02,  6.975154414e-02, /*DMEAN*/  6.790497975e-01, /*EDMEAN*/  2.848975487e-02 },
        {57, /* SP */ 200, /*MAG*/  6.779000000e+00,  5.190000000e+00,  3.590000000e+00,  2.466000000e+00,  1.778000000e+00,  1.611000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  9.008885000e-02,  3.360000000e-01,  2.500000000e-01,  2.860000000e-01, /*DIAM*/  4.791426880e-01,  4.192315633e-01,  4.205184256e-01,  4.212598966e-01, /*EDIAM*/  4.123976885e-02,  9.312804911e-02,  6.040742751e-02,  6.477957501e-02, /*DMEAN*/  4.489509309e-01, /*EDMEAN*/  2.821165178e-02 },
        {58, /* SP */ 200, /*MAG*/  2.408000000e+00,  8.700000000e-01, -8.000000000e-01, -2.095000000e+00, -2.775000000e+00, -3.044000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  1.940000000e-01,  1.700000000e-01,  1.400000000e-01, /*DIAM*/  1.311522700e+00,  1.349936934e+00,  1.340728556e+00,  1.361062830e+00, /*EDIAM*/  4.123976885e-02,  5.391790149e-02,  4.115293056e-02,  3.180987386e-02, /*DMEAN*/  1.342580427e+00, /*EDMEAN*/  1.951298048e-02 },
        {59, /* SP */ 200, /*MAG*/  2.408000000e+00,  8.700000000e-01, -8.000000000e-01, -2.095000000e+00, -2.775000000e+00, -3.044000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  1.940000000e-01,  1.700000000e-01,  1.400000000e-01, /*DIAM*/  1.311522700e+00,  1.349936934e+00,  1.340728556e+00,  1.361062830e+00, /*EDIAM*/  4.123976885e-02,  5.391790149e-02,  4.115293056e-02,  3.180987386e-02, /*DMEAN*/  1.342580427e+00, /*EDMEAN*/  1.951298048e-02 },
        {60, /* SP */ 200, /*MAG*/  2.408000000e+00,  8.700000000e-01, -8.000000000e-01, -2.095000000e+00, -2.775000000e+00, -3.044000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  1.940000000e-01,  1.700000000e-01,  1.400000000e-01, /*DIAM*/  1.311522700e+00,  1.349936934e+00,  1.340728556e+00,  1.361062830e+00, /*EDIAM*/  4.123976885e-02,  5.391790149e-02,  4.115293056e-02,  3.180987386e-02, /*DMEAN*/  1.342580427e+00, /*EDMEAN*/  1.951298048e-02 },
        {61, /* SP */ 200, /*MAG*/  2.408000000e+00,  8.700000000e-01, -8.000000000e-01, -2.095000000e+00, -2.775000000e+00, -3.044000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  1.940000000e-01,  1.700000000e-01,  1.400000000e-01, /*DIAM*/  1.311522700e+00,  1.349936934e+00,  1.340728556e+00,  1.361062830e+00, /*EDIAM*/  4.123976885e-02,  5.391790149e-02,  4.115293056e-02,  3.180987386e-02, /*DMEAN*/  1.342580427e+00, /*EDMEAN*/  1.951298048e-02 },
        {62, /* SP */ 200, /*MAG*/  2.408000000e+00,  8.700000000e-01, -8.000000000e-01, -2.095000000e+00, -2.775000000e+00, -3.044000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  1.940000000e-01,  1.700000000e-01,  1.400000000e-01, /*DIAM*/  1.311522700e+00,  1.349936934e+00,  1.340728556e+00,  1.361062830e+00, /*EDIAM*/  4.123976885e-02,  5.391790149e-02,  4.115293056e-02,  3.180987386e-02, /*DMEAN*/  1.342580427e+00, /*EDMEAN*/  1.951298048e-02 },
        {63, /* SP */ 196, /*MAG*/  5.838000000e+00,  4.390000000e+00,  2.880000000e+00,  1.897000000e+00,  1.220000000e+00,  1.022000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  6.007495000e-02,  3.220000000e-01,  2.500000000e-01,  3.220000000e-01, /*DIAM*/  5.408401866e-01,  5.097389996e-01,  5.172385341e-01,  5.280244280e-01, /*EDIAM*/  4.123418936e-02,  8.925424381e-02,  6.040295872e-02,  7.291568422e-02, /*DMEAN*/  5.299353882e-01, /*EDMEAN*/  2.867700340e-02 },
        {64, /* SP */ 234, /*MAG*/  8.540000000e+00,  6.770000000e+00,  3.850000000e+00,  1.919000000e+00,  9.880000000e-01,  7.420000000e-01, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  5.063596000e-02,  2.900000000e-01,  2.120000000e-01,  2.440000000e-01, /*DIAM*/  5.336031164e-01,  7.153005489e-01,  7.402910711e-01,  7.169049646e-01, /*EDIAM*/  4.147319769e-02,  8.045871305e-02,  5.133952327e-02,  5.533996999e-02, /*DMEAN*/  6.481257802e-01, /*EDMEAN*/  2.586149509e-02 },
        {65, /* SP */ 232, /*MAG*/  8.490000000e+00,  6.870000000e+00,  4.600000000e+00,  3.157000000e+00,  2.237000000e+00,  1.982000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  6.040695000e-02,  2.780000000e-01,  2.520000000e-01,  3.040000000e-01, /*DIAM*/  3.949879971e-01,  3.807708083e-01,  4.380761425e-01,  4.350893692e-01, /*EDIAM*/  4.140373314e-02,  7.712007595e-02,  6.093166854e-02,  6.887336509e-02, /*DMEAN*/  4.091146758e-01, /*EDMEAN*/  2.798966009e-02 },
        {66, /* SP */ 224, /*MAG*/  6.797000000e+00,  5.150000000e+00,  3.130000000e+00,  1.839000000e+00,  9.960000000e-01,  8.340000000e-01, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  5.024938000e-02,  3.000000000e-01,  3.120000000e-01,  3.500000000e-01, /*DIAM*/  6.682455641e-01,  6.124400374e-01,  6.481132398e-01,  6.347502884e-01, /*EDIAM*/  4.126750894e-02,  8.316472474e-02,  7.534453082e-02,  7.924584100e-02, /*DMEAN*/  6.522448988e-01, /*EDMEAN*/  3.012143758e-02 },
        {67, /* SP */ 180, /*MAG*/  3.410000000e+00,  2.240000000e+00,  1.110000000e+00,  3.110000000e-01, -2.100000000e-01, -3.030000000e-01, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  1.980000000e-01,  1.600000000e-01,  1.700000000e-01, /*DIAM*/  7.810956764e-01,  7.594122244e-01,  7.582559465e-01,  7.521798920e-01, /*EDIAM*/  4.121196542e-02,  5.498112280e-02,  3.871183635e-02,  3.854235465e-02, /*DMEAN*/  7.628889880e-01, /*EDMEAN*/  2.054840746e-02 },
        {68, /* SP */ 220, /*MAG*/  3.646000000e+00,  2.070000000e+00,  3.300000000e-01, -9.570000000e-01, -1.674000000e+00, -1.846000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.060000000e-01,  1.480000000e-01,  1.600000000e-01, /*DIAM*/  1.204764089e+00,  1.147798187e+00,  1.156974353e+00,  1.153131106e+00, /*EDIAM*/  4.124821477e-02,  5.720936880e-02,  3.586084689e-02,  3.630933919e-02, /*DMEAN*/  1.166809703e+00, /*EDMEAN*/  1.982906048e-02 },
        {69, /* SP */ 120, /*MAG*/  2.271000000e+00,  1.790000000e+00,  1.160000000e+00,  8.300000000e-01,  4.130000000e-01,  5.200000000e-01, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  4.019950000e-02,  1.840000000e-01,  1.620000000e-01,  1.640000000e-01, /*DIAM*/  5.155310634e-01,  5.023688449e-01,  5.620735163e-01,  5.098462848e-01, /*EDIAM*/  4.139308516e-02,  5.119623122e-02,  3.928372230e-02,  3.723091686e-02, /*DMEAN*/  5.243397714e-01, /*EDMEAN*/  2.023142224e-02 },
        {70, /* SP */ 68, /*MAG*/  1.342000000e+00,  1.250000000e+00,  1.090000000e+00,  1.139000000e+00,  9.020000000e-01,  1.010000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.540000000e-01,  1.880000000e-01,  2.020000000e-01, /*DIAM*/  3.240088826e-01,  3.053715909e-01,  3.597488640e-01,  3.215523037e-01, /*EDIAM*/  4.153908537e-02,  7.069850149e-02,  4.576279082e-02,  4.600136834e-02, /*DMEAN*/  3.309230626e-01, /*EDMEAN*/  2.361431929e-02 },
        {71, /* SP */ 200, /*MAG*/  2.408000000e+00,  8.700000000e-01, -8.000000000e-01, -2.095000000e+00, -2.775000000e+00, -3.044000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  1.940000000e-01,  1.700000000e-01,  1.400000000e-01, /*DIAM*/  1.311522700e+00,  1.349936934e+00,  1.340728556e+00,  1.361062830e+00, /*EDIAM*/  4.123976885e-02,  5.391790149e-02,  4.115293056e-02,  3.180987386e-02, /*DMEAN*/  1.342580427e+00, /*EDMEAN*/  1.951298048e-02 },
        {72, /* SP */ 232, /*MAG*/  4.491000000e+00,  2.870000000e+00,  5.700000000e-01, -9.120000000e-01, -1.608000000e+00, -1.862000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  5.035871000e-02,  1.880000000e-01,  1.900000000e-01,  1.800000000e-01, /*DIAM*/  1.195608009e+00,  1.199869035e+00,  1.200683158e+00,  1.199790111e+00, /*EDIAM*/  4.140373314e-02,  5.229214929e-02,  4.601931072e-02,  4.087339370e-02, /*DMEAN*/  1.198756934e+00, /*EDMEAN*/  2.175318215e-02 },
        {73, /* SP */ 192, /*MAG*/  5.797000000e+00,  4.450000000e+00,  3.080000000e+00,  2.195000000e+00,  1.495000000e+00,  1.357000000e+00, /*EMAG*/  4.809366000e-02,  3.999999911e-02,  3.999999911e-02,  3.080000000e-01,  2.600000000e-01,  3.080000000e-01, /*DIAM*/  4.582528507e-01,  4.260383753e-01,  4.489325974e-01,  4.485901124e-01, /*EDIAM*/  4.442717189e-02,  8.537942333e-02,  6.280669359e-02,  6.974655643e-02, /*DMEAN*/  4.502687811e-01, /*EDMEAN*/  2.964406230e-02 },
        {74, /* SP */ 180, /*MAG*/  5.023000000e+00,  3.940000000e+00,  2.930000000e+00,  2.209000000e+00,  1.685000000e+00,  1.567000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.940000000e-01,  1.780000000e-01,  2.040000000e-01, /*DIAM*/  3.871556705e-01,  3.646086471e-01,  3.712131392e-01,  3.737127331e-01, /*EDIAM*/  4.121196542e-02,  8.149468060e-02,  4.304288901e-02,  4.622327028e-02, /*DMEAN*/  3.767744494e-01, /*EDMEAN*/  2.345121994e-02 },
        {75, /* SP */ 224, /*MAG*/  8.299000000e+00,  6.530000000e+00,  4.250000000e+00,  2.857000000e+00,  1.994000000e+00,  1.740000000e+00, /*EMAG*/  7.217340000e-02,  3.999999911e-02,  3.999999911e-02,  2.720000000e-01,  2.360000000e-01,  2.640000000e-01, /*DIAM*/  4.678855611e-01,  4.366364633e-01,  4.642688791e-01,  4.660057604e-01, /*EDIAM*/  5.559052763e-02,  7.542798153e-02,  5.703435318e-02,  5.980185660e-02, /*DMEAN*/  4.613129230e-01, /*EDMEAN*/  2.996442940e-02 },
        {76, /* SP */ 234, /*MAG*/  8.540000000e+00,  6.770000000e+00,  3.850000000e+00,  1.919000000e+00,  9.880000000e-01,  7.420000000e-01, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  5.063596000e-02,  2.900000000e-01,  2.120000000e-01,  2.440000000e-01, /*DIAM*/  5.336031164e-01,  7.153005489e-01,  7.402910711e-01,  7.169049646e-01, /*EDIAM*/  4.147319769e-02,  8.045871305e-02,  5.133952327e-02,  5.533996999e-02, /*DMEAN*/  6.481257802e-01, /*EDMEAN*/  2.586149509e-02 },
        {77, /* SP */ 232, /*MAG*/  8.102000000e+00,  6.520000000e+00,  3.840000000e+00,  2.202000000e+00,  1.270000000e+00,  1.039000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  3.080000000e-01,  2.120000000e-01,  2.240000000e-01, /*DIAM*/  4.414279978e-01,  6.182261690e-01,  6.569243948e-01,  6.392718540e-01, /*EDIAM*/  4.140373314e-02,  8.540676621e-02,  5.130780459e-02,  5.080148869e-02, /*DMEAN*/  5.632919958e-01, /*EDMEAN*/  2.546518442e-02 },
        {78, /* SP */ 224, /*MAG*/  6.797000000e+00,  5.150000000e+00,  3.130000000e+00,  1.839000000e+00,  9.960000000e-01,  8.340000000e-01, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  5.024938000e-02,  3.000000000e-01,  3.120000000e-01,  3.500000000e-01, /*DIAM*/  6.682455641e-01,  6.124400374e-01,  6.481132398e-01,  6.347502884e-01, /*EDIAM*/  4.126750894e-02,  8.316472474e-02,  7.534453082e-02,  7.924584100e-02, /*DMEAN*/  6.522448988e-01, /*EDMEAN*/  3.012143758e-02 },
        {79, /* SP */ 232, /*MAG*/  6.543000000e+00,  4.930000000e+00,  2.370000000e+00,  8.460000000e-01, -1.230000000e-01, -3.650000000e-01, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  5.141984000e-02,  2.060000000e-01,  2.780000000e-01,  3.120000000e-01, /*DIAM*/  7.786480028e-01,  8.714583156e-01,  9.273991070e-01,  9.151842668e-01, /*EDIAM*/  4.140373314e-02,  5.725198108e-02,  6.719117060e-02,  7.068140600e-02, /*DMEAN*/  8.456778850e-01, /*EDMEAN*/  2.703666586e-02 },
        {80, /* SP */ 228, /*MAG*/  7.554000000e+00,  5.970000000e+00,  3.770000000e+00,  2.450000000e+00,  1.582000000e+00,  1.444000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  3.180000000e-01,  2.300000000e-01,  2.380000000e-01, /*DIAM*/  5.060336426e-01,  5.073619726e-01,  5.494352726e-01,  5.256125798e-01, /*EDIAM*/  4.131329595e-02,  8.814588109e-02,  5.560297097e-02,  5.393578812e-02, /*DMEAN*/  5.210615124e-01, /*EDMEAN*/  2.643609112e-02 },
        {81, /* SP */ 228, /*MAG*/  6.380000000e+00,  4.320000000e+00,  1.780000000e+00,  3.930000000e-01, -6.090000000e-01, -9.130000000e-01, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.200000000e-01,  1.860000000e-01,  1.620000000e-01, /*DIAM*/  1.131153652e+00,  9.500137649e-01,  1.009948898e+00,  1.015590689e+00, /*EDIAM*/  4.131329595e-02,  6.107776452e-02,  4.501568571e-02,  3.677696588e-02, /*DMEAN*/  1.039369435e+00, /*EDMEAN*/  2.143176939e-02 },
        {82, /* SP */ 200, /*MAG*/  2.408000000e+00,  8.700000000e-01, -8.000000000e-01, -2.095000000e+00, -2.775000000e+00, -3.044000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  1.940000000e-01,  1.700000000e-01,  1.400000000e-01, /*DIAM*/  1.311522700e+00,  1.349936934e+00,  1.340728556e+00,  1.361062830e+00, /*EDIAM*/  4.123976885e-02,  5.391790149e-02,  4.115293056e-02,  3.180987386e-02, /*DMEAN*/  1.342580427e+00, /*EDMEAN*/  1.951298048e-02 },
        {83, /* SP */ 188, /*MAG*/  3.161000000e+00,  2.010000000e+00,  8.800000000e-01, -1.960000000e-01, -7.490000000e-01, -7.830000000e-01, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.180000000e-01,  1.560000000e-01,  1.760000000e-01, /*DIAM*/  8.193742439e-01,  8.944534343e-01,  8.856415365e-01,  8.637797692e-01, /*EDIAM*/  4.122057722e-02,  6.051369180e-02,  3.776350066e-02,  3.991055356e-02, /*DMEAN*/  8.621116294e-01, /*EDMEAN*/  2.088674756e-02 },
        {84, /* SP */ 196, /*MAG*/  3.535000000e+00,  2.070000000e+00,  6.100000000e-01, -4.950000000e-01, -1.225000000e+00, -1.287000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  1.940000000e-01,  1.840000000e-01,  2.040000000e-01, /*DIAM*/  1.015380194e+00,  9.936675782e-01,  1.011394183e+00,  9.895353837e-01, /*EDIAM*/  4.123418936e-02,  5.391067583e-02,  4.451358359e-02,  4.624635692e-02, /*DMEAN*/  1.004215105e+00, /*EDMEAN*/  2.238375638e-02 },
        {85, /* SP */ 200, /*MAG*/  3.761000000e+00,  2.240000000e+00,  7.000000000e-01, -2.450000000e-01, -1.034000000e+00, -1.162000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.120000000e-01,  1.800000000e-01,  1.600000000e-01, /*DIAM*/  1.026982696e+00,  9.430797854e-01,  9.772266051e-01,  9.712088099e-01, /*EDIAM*/  4.123976885e-02,  5.888130766e-02,  4.355755898e-02,  3.631909640e-02, /*DMEAN*/  9.846287324e-01, /*EDMEAN*/  2.104265438e-02 },
        {86, /* SP */ 176, /*MAG*/  4.060000000e+00,  3.070000000e+00,  2.130000000e+00,  1.375000000e+00,  8.370000000e-01,  8.830000000e-01, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.620000000e-01,  1.660000000e-01,  1.980000000e-01, /*DIAM*/  5.042410977e-01,  5.224115867e-01,  5.370097529e-01,  5.015068105e-01, /*EDIAM*/  4.121207570e-02,  7.264940585e-02,  4.015214051e-02,  4.486332293e-02, /*DMEAN*/  5.159828087e-01, /*EDMEAN*/  2.248837934e-02 },
        {87, /* SP */ 220, /*MAG*/  3.646000000e+00,  2.070000000e+00,  3.300000000e-01, -9.570000000e-01, -1.674000000e+00, -1.846000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.060000000e-01,  1.480000000e-01,  1.600000000e-01, /*DIAM*/  1.204764089e+00,  1.147798187e+00,  1.156974353e+00,  1.153131106e+00, /*EDIAM*/  4.124821477e-02,  5.720936880e-02,  3.586084689e-02,  3.630933919e-02, /*DMEAN*/  1.166809703e+00, /*EDMEAN*/  1.982906048e-02 },
        {88, /* SP */ 200, /*MAG*/  2.408000000e+00,  8.700000000e-01, -8.000000000e-01, -2.095000000e+00, -2.775000000e+00, -3.044000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  1.940000000e-01,  1.700000000e-01,  1.400000000e-01, /*DIAM*/  1.311522700e+00,  1.349936934e+00,  1.340728556e+00,  1.361062830e+00, /*EDIAM*/  4.123976885e-02,  5.391790149e-02,  4.115293056e-02,  3.180987386e-02, /*DMEAN*/  1.342580427e+00, /*EDMEAN*/  1.951298048e-02 },
        {89, /* SP */ 232, /*MAG*/  4.491000000e+00,  2.870000000e+00,  5.700000000e-01, -9.120000000e-01, -1.608000000e+00, -1.862000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  5.035871000e-02,  1.880000000e-01,  1.900000000e-01,  1.800000000e-01, /*DIAM*/  1.195608009e+00,  1.199869035e+00,  1.200683158e+00,  1.199790111e+00, /*EDIAM*/  4.140373314e-02,  5.229214929e-02,  4.601931072e-02,  4.087339370e-02, /*DMEAN*/  1.198756934e+00, /*EDMEAN*/  2.175318215e-02 },
        {90, /* SP */ 200, /*MAG*/  3.761000000e+00,  2.240000000e+00,  7.000000000e-01, -2.450000000e-01, -1.034000000e+00, -1.162000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.120000000e-01,  1.800000000e-01,  1.600000000e-01, /*DIAM*/  1.026982696e+00,  9.430797854e-01,  9.772266051e-01,  9.712088099e-01, /*EDIAM*/  4.123976885e-02,  5.888130766e-02,  4.355755898e-02,  3.631909640e-02, /*DMEAN*/  9.846287324e-01, /*EDMEAN*/  2.104265438e-02 },
        {91, /* SP */ 200, /*MAG*/  2.408000000e+00,  8.700000000e-01, -8.000000000e-01, -2.095000000e+00, -2.775000000e+00, -3.044000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  1.940000000e-01,  1.700000000e-01,  1.400000000e-01, /*DIAM*/  1.311522700e+00,  1.349936934e+00,  1.340728556e+00,  1.361062830e+00, /*EDIAM*/  4.123976885e-02,  5.391790149e-02,  4.115293056e-02,  3.180987386e-02, /*DMEAN*/  1.342580427e+00, /*EDMEAN*/  1.951298048e-02 },
        {92, /* SP */ 172, /*MAG*/  4.437000000e+00,  3.060000000e+00,  1.840000000e+00,  8.860000000e-01,  2.320000000e-01,  1.250000000e-01, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.220000000e-01,  1.540000000e-01,  1.720000000e-01, /*DIAM*/  7.484260096e-01,  6.507950237e-01,  6.799614749e-01,  6.688949766e-01, /*EDIAM*/  4.121588091e-02,  6.160103013e-02,  3.726501435e-02,  3.898617076e-02, /*DMEAN*/  6.921446152e-01, /*EDMEAN*/  2.070239555e-02 },
        {93, /* SP */ 226, /*MAG*/  7.126000000e+00,  5.430000000e+00,  3.520000000e+00,  1.960000000e+00,  1.002000000e+00,  9.590000000e-01, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  5.035871000e-02,  2.680000000e-01,  1.880000000e-01,  1.820000000e-01, /*DIAM*/  6.623515731e-01,  6.011135262e-01,  6.625680111e-01,  6.174667433e-01, /*EDIAM*/  4.128610059e-02,  7.432484350e-02,  4.548605940e-02,  4.128075225e-02, /*DMEAN*/  6.422550002e-01, /*EDMEAN*/  2.286750553e-02 },
        {94, /* SP */ 224, /*MAG*/  5.541000000e+00,  4.040000000e+00,  2.250000000e+00,  1.176000000e+00,  3.250000000e-01,  1.570000000e-01, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.240000000e-01,  2.140000000e-01,  2.600000000e-01, /*DIAM*/  7.997255660e-01,  7.107168245e-01,  7.642066267e-01,  7.587721881e-01, /*EDIAM*/  4.126750894e-02,  6.217267618e-02,  5.173727108e-02,  5.889774034e-02, /*DMEAN*/  7.679092292e-01, /*EDMEAN*/  2.520772210e-02 },
        {95, /* SP */ 200, /*MAG*/  5.620000000e+00,  4.050000000e+00,  2.330000000e+00,  1.261000000e+00,  4.290000000e-01,  3.100000000e-01, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.280000000e-01,  2.080000000e-01,  2.260000000e-01, /*DIAM*/  6.953626912e-01,  6.652226384e-01,  6.989386632e-01,  6.856905575e-01, /*EDIAM*/  4.123976885e-02,  6.329569586e-02,  5.029450477e-02,  5.121996538e-02, /*DMEAN*/  6.892704147e-01, /*EDMEAN*/  2.436812234e-02 },
        {96, /* SP */ 220, /*MAG*/  3.646000000e+00,  2.070000000e+00,  3.300000000e-01, -9.570000000e-01, -1.674000000e+00, -1.846000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.060000000e-01,  1.480000000e-01,  1.600000000e-01, /*DIAM*/  1.204764089e+00,  1.147798187e+00,  1.156974353e+00,  1.153131106e+00, /*EDIAM*/  4.124821477e-02,  5.720936880e-02,  3.586084689e-02,  3.630933919e-02, /*DMEAN*/  1.166809703e+00, /*EDMEAN*/  1.982906048e-02 },
        {97, /* SP */ 236, /*MAG*/  4.848000000e+00,  3.320000000e+00,  5.600000000e-01, -7.790000000e-01, -1.675000000e+00, -1.904000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  6.067125000e-02,  1.820000000e-01,  1.580000000e-01,  1.520000000e-01, /*DIAM*/  1.100779971e+00,  1.196385014e+00,  1.245629920e+00,  1.228885646e+00, /*EDIAM*/  4.156379099e-02,  5.073369883e-02,  3.843683177e-02,  3.464882788e-02, /*DMEAN*/  1.196550783e+00, /*EDMEAN*/  1.966125840e-02 },
        {98, /* SP */ 200, /*MAG*/  2.408000000e+00,  8.700000000e-01, -8.000000000e-01, -2.095000000e+00, -2.775000000e+00, -3.044000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  1.940000000e-01,  1.700000000e-01,  1.400000000e-01, /*DIAM*/  1.311522700e+00,  1.349936934e+00,  1.340728556e+00,  1.361062830e+00, /*EDIAM*/  4.123976885e-02,  5.391790149e-02,  4.115293056e-02,  3.180987386e-02, /*DMEAN*/  1.342580427e+00, /*EDMEAN*/  1.951298048e-02 },
        {99, /* SP */ 232, /*MAG*/  4.491000000e+00,  2.870000000e+00,  5.700000000e-01, -9.120000000e-01, -1.608000000e+00, -1.862000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  5.035871000e-02,  1.880000000e-01,  1.900000000e-01,  1.800000000e-01, /*DIAM*/  1.195608009e+00,  1.199869035e+00,  1.200683158e+00,  1.199790111e+00, /*EDIAM*/  4.140373314e-02,  5.229214929e-02,  4.601931072e-02,  4.087339370e-02, /*DMEAN*/  1.198756934e+00, /*EDMEAN*/  2.175318215e-02 },
        {100, /* SP */ 200, /*MAG*/  3.761000000e+00,  2.240000000e+00,  7.000000000e-01, -2.450000000e-01, -1.034000000e+00, -1.162000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.120000000e-01,  1.800000000e-01,  1.600000000e-01, /*DIAM*/  1.026982696e+00,  9.430797854e-01,  9.772266051e-01,  9.712088099e-01, /*EDIAM*/  4.123976885e-02,  5.888130766e-02,  4.355755898e-02,  3.631909640e-02, /*DMEAN*/  9.846287324e-01, /*EDMEAN*/  2.104265438e-02 },
        {101, /* SP */ 230, /*MAG*/  4.095000000e+00,  2.440000000e+00,  1.300000000e-01, -1.432000000e+00, -2.129000000e+00, -2.379000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  6.053098000e-02,  1.940000000e-01,  1.800000000e-01,  1.540000000e-01, /*DIAM*/  1.278622113e+00,  1.310928683e+00,  1.303784020e+00,  1.301667562e+00, /*EDIAM*/  4.135154739e-02,  5.392079872e-02,  4.359066989e-02,  3.498948922e-02, /*DMEAN*/  1.297360004e+00, /*EDMEAN*/  2.052092339e-02 },
        {102, /* SP */ 172, /*MAG*/  4.437000000e+00,  3.060000000e+00,  1.840000000e+00,  8.860000000e-01,  2.320000000e-01,  1.250000000e-01, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.220000000e-01,  1.540000000e-01,  1.720000000e-01, /*DIAM*/  7.484260096e-01,  6.507950237e-01,  6.799614749e-01,  6.688949766e-01, /*EDIAM*/  4.121588091e-02,  6.160103013e-02,  3.726501435e-02,  3.898617076e-02, /*DMEAN*/  6.921446152e-01, /*EDMEAN*/  2.070239555e-02 },
        {103, /* SP */ 180, /*MAG*/  3.410000000e+00,  2.240000000e+00,  1.110000000e+00,  3.110000000e-01, -2.100000000e-01, -3.030000000e-01, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  1.980000000e-01,  1.600000000e-01,  1.700000000e-01, /*DIAM*/  7.810956764e-01,  7.594122244e-01,  7.582559465e-01,  7.521798920e-01, /*EDIAM*/  4.121196542e-02,  5.498112280e-02,  3.871183635e-02,  3.854235465e-02, /*DMEAN*/  7.628889880e-01, /*EDMEAN*/  2.054840746e-02 },
        {104, /* SP */ 220, /*MAG*/  3.646000000e+00,  2.070000000e+00,  3.300000000e-01, -9.570000000e-01, -1.674000000e+00, -1.846000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.060000000e-01,  1.480000000e-01,  1.600000000e-01, /*DIAM*/  1.204764089e+00,  1.147798187e+00,  1.156974353e+00,  1.153131106e+00, /*EDIAM*/  4.124821477e-02,  5.720936880e-02,  3.586084689e-02,  3.630933919e-02, /*DMEAN*/  1.166809703e+00, /*EDMEAN*/  1.982906048e-02 },
        {105, /* SP */ 188, /*MAG*/  3.161000000e+00,  2.010000000e+00,  8.800000000e-01, -1.960000000e-01, -7.490000000e-01, -7.830000000e-01, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.180000000e-01,  1.560000000e-01,  1.760000000e-01, /*DIAM*/  8.193742439e-01,  8.944534343e-01,  8.856415365e-01,  8.637797692e-01, /*EDIAM*/  4.122057722e-02,  6.051369180e-02,  3.776350066e-02,  3.991055356e-02, /*DMEAN*/  8.621116294e-01, /*EDMEAN*/  2.088674756e-02 },
        {106, /* SP */ 198, /*MAG*/  5.800000000e+00,  4.250000000e+00,  2.640000000e+00,  1.263000000e+00,  4.530000000e-01,  3.750000000e-01, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.980000000e-01,  1.660000000e-01,  1.780000000e-01, /*DIAM*/  6.371374352e-01,  6.772873487e-01,  6.988891900e-01,  6.734571266e-01, /*EDIAM*/  4.123720484e-02,  8.262434132e-02,  4.018823352e-02,  4.037832817e-02, /*DMEAN*/  6.705019722e-01, /*EDMEAN*/  2.209892035e-02 },
        {107, /* SP */ 208, /*MAG*/  4.690000000e+00,  3.140000000e+00,  1.490000000e+00,  1.200000000e-01, -5.780000000e-01, -6.560000000e-01, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.200000000e-01,  1.700000000e-01,  1.720000000e-01, /*DIAM*/  8.967572525e-01,  9.211325408e-01,  9.156301924e-01,  8.921898173e-01, /*EDIAM*/  4.124396433e-02,  6.109164872e-02,  4.115794873e-02,  3.902886317e-02, /*DMEAN*/  9.037331893e-01, /*EDMEAN*/  2.133520844e-02 },
        {108, /* SP */ 192, /*MAG*/  4.597000000e+00,  3.160000000e+00,  1.850000000e+00,  7.240000000e-01, -1.120000000e-01, -2.300000000e-02, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  1.780000000e-01,  1.780000000e-01,  1.600000000e-01, /*DIAM*/  7.720528553e-01,  7.341365942e-01,  7.834073106e-01,  7.269550800e-01, /*EDIAM*/  4.122737356e-02,  4.949146678e-02,  4.306271908e-02,  3.630654260e-02, /*DMEAN*/  7.533914444e-01, /*EDMEAN*/  2.038396054e-02 },
        {109, /* SP */ 200, /*MAG*/  3.761000000e+00,  2.240000000e+00,  7.000000000e-01, -2.450000000e-01, -1.034000000e+00, -1.162000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.120000000e-01,  1.800000000e-01,  1.600000000e-01, /*DIAM*/  1.026982696e+00,  9.430797854e-01,  9.772266051e-01,  9.712088099e-01, /*EDIAM*/  4.123976885e-02,  5.888130766e-02,  4.355755898e-02,  3.631909640e-02, /*DMEAN*/  9.846287324e-01, /*EDMEAN*/  2.104265438e-02 },
        {110, /* SP */ 220, /*MAG*/  5.942000000e+00,  4.440000000e+00,  2.760000000e+00,  1.714000000e+00,  8.780000000e-01,  7.110000000e-01, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.980000000e-01,  1.700000000e-01,  2.120000000e-01, /*DIAM*/  6.848840808e-01,  5.904856783e-01,  6.390677308e-01,  6.368172299e-01, /*EDIAM*/  4.124821477e-02,  8.261464553e-02,  4.114805030e-02,  4.805006684e-02, /*DMEAN*/  6.500610312e-01, /*EDMEAN*/  2.337720631e-02 },
        {111, /* SP */ 230, /*MAG*/  4.095000000e+00,  2.440000000e+00,  1.300000000e-01, -1.432000000e+00, -2.129000000e+00, -2.379000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  6.053098000e-02,  1.940000000e-01,  1.800000000e-01,  1.540000000e-01, /*DIAM*/  1.278622113e+00,  1.310928683e+00,  1.303784020e+00,  1.301667562e+00, /*EDIAM*/  4.135154739e-02,  5.392079872e-02,  4.359066989e-02,  3.498948922e-02, /*DMEAN*/  1.297360004e+00, /*EDMEAN*/  2.052092339e-02 },
        {112, /* SP */ 226, /*MAG*/  2.925000000e+00,  1.060000000e+00, -1.840000000e+00, -2.850000000e+00, -3.725000000e+00, -4.100000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  9.300000000e-02,  1.460000000e-01,  1.600000000e-01, /*DIAM*/  1.641131588e+00,  1.596899255e+00,  1.622692539e+00,  1.647371868e+00, /*EDIAM*/  4.128610059e-02,  2.615960185e-02,  3.539156707e-02,  3.631612330e-02, /*DMEAN*/  1.620665351e+00, /*EDMEAN*/  1.609651798e-02 },
        {113, /* SP */ 192, /*MAG*/  4.538000000e+00,  3.270000000e+00,  2.040000000e+00,  9.860000000e-01,  4.030000000e-01,  4.670000000e-01, /*EMAG*/  4.011234000e-02,  3.999999911e-02,  3.999999911e-02,  2.400000000e-01,  1.720000000e-01,  1.900000000e-01, /*DIAM*/  6.452728534e-01,  6.700651647e-01,  6.637030286e-01,  6.189696769e-01, /*EDIAM*/  4.126930928e-02,  6.659458382e-02,  4.161945968e-02,  4.307876786e-02, /*DMEAN*/  6.464843288e-01, /*EDMEAN*/  2.228668460e-02 },
        {114, /* SP */ 180, /*MAG*/  3.410000000e+00,  2.240000000e+00,  1.110000000e+00,  3.110000000e-01, -2.100000000e-01, -3.030000000e-01, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  1.980000000e-01,  1.600000000e-01,  1.700000000e-01, /*DIAM*/  7.810956764e-01,  7.594122244e-01,  7.582559465e-01,  7.521798920e-01, /*EDIAM*/  4.121196542e-02,  5.498112280e-02,  3.871183635e-02,  3.854235465e-02, /*DMEAN*/  7.628889880e-01, /*EDMEAN*/  2.054840746e-02 },
        {115, /* SP */ 220, /*MAG*/  3.646000000e+00,  2.070000000e+00,  3.300000000e-01, -9.570000000e-01, -1.674000000e+00, -1.846000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.060000000e-01,  1.480000000e-01,  1.600000000e-01, /*DIAM*/  1.204764089e+00,  1.147798187e+00,  1.156974353e+00,  1.153131106e+00, /*EDIAM*/  4.124821477e-02,  5.720936880e-02,  3.586084689e-02,  3.630933919e-02, /*DMEAN*/  1.166809703e+00, /*EDMEAN*/  1.982906048e-02 },
        {116, /* SP */ 188, /*MAG*/  3.161000000e+00,  2.010000000e+00,  8.800000000e-01, -1.960000000e-01, -7.490000000e-01, -7.830000000e-01, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.180000000e-01,  1.560000000e-01,  1.760000000e-01, /*DIAM*/  8.193742439e-01,  8.944534343e-01,  8.856415365e-01,  8.637797692e-01, /*EDIAM*/  4.122057722e-02,  6.051369180e-02,  3.776350066e-02,  3.991055356e-02, /*DMEAN*/  8.621116294e-01, /*EDMEAN*/  2.088674756e-02 },
        {117, /* SP */ 200, /*MAG*/  2.408000000e+00,  8.700000000e-01, -8.000000000e-01, -2.095000000e+00, -2.775000000e+00, -3.044000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  1.940000000e-01,  1.700000000e-01,  1.400000000e-01, /*DIAM*/  1.311522700e+00,  1.349936934e+00,  1.340728556e+00,  1.361062830e+00, /*EDIAM*/  4.123976885e-02,  5.391790149e-02,  4.115293056e-02,  3.180987386e-02, /*DMEAN*/  1.342580427e+00, /*EDMEAN*/  1.951298048e-02 },
        {118, /* SP */ 180, /*MAG*/  3.501000000e+00,  2.480000000e+00,  1.480000000e+00,  6.410000000e-01,  1.040000000e-01, -7.000000000e-03, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.180000000e-01,  1.600000000e-01,  2.040000000e-01, /*DIAM*/  6.407156743e-01,  6.865015090e-01,  6.924038055e-01,  6.915083582e-01, /*EDIAM*/  4.121196542e-02,  6.050059346e-02,  3.871183635e-02,  4.622327028e-02, /*DMEAN*/  6.756306648e-01, /*EDMEAN*/  2.186827014e-02 },
        {119, /* SP */ 230, /*MAG*/  4.095000000e+00,  2.440000000e+00,  1.300000000e-01, -1.432000000e+00, -2.129000000e+00, -2.379000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  6.053098000e-02,  1.940000000e-01,  1.800000000e-01,  1.540000000e-01, /*DIAM*/  1.278622113e+00,  1.310928683e+00,  1.303784020e+00,  1.301667562e+00, /*EDIAM*/  4.135154739e-02,  5.392079872e-02,  4.359066989e-02,  3.498948922e-02, /*DMEAN*/  1.297360004e+00, /*EDMEAN*/  2.052092339e-02 },
        {120, /* SP */ 196, /*MAG*/  5.940000000e+00,  4.440000000e+00,  2.860000000e+00,  2.031000000e+00,  1.198000000e+00,  1.105000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.860000000e-01,  1.680000000e-01,  1.740000000e-01, /*DIAM*/  5.630801869e-01,  4.764889991e-01,  5.246081840e-01,  5.105572744e-01, /*EDIAM*/  4.123418936e-02,  7.930619861e-02,  4.066547858e-02,  3.947278945e-02, /*DMEAN*/  5.279811205e-01, /*EDMEAN*/  2.194761748e-02 },
        {121, /* SP */ 228, /*MAG*/  7.554000000e+00,  5.970000000e+00,  3.770000000e+00,  2.450000000e+00,  1.582000000e+00,  1.444000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  3.180000000e-01,  2.300000000e-01,  2.380000000e-01, /*DIAM*/  5.060336426e-01,  5.073619726e-01,  5.494352726e-01,  5.256125798e-01, /*EDIAM*/  4.131329595e-02,  8.814588109e-02,  5.560297097e-02,  5.393578812e-02, /*DMEAN*/  5.210615124e-01, /*EDMEAN*/  2.643609112e-02 },
        {122, /* SP */ 172, /*MAG*/  4.437000000e+00,  3.060000000e+00,  1.840000000e+00,  8.860000000e-01,  2.320000000e-01,  1.250000000e-01, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.220000000e-01,  1.540000000e-01,  1.720000000e-01, /*DIAM*/  7.484260096e-01,  6.507950237e-01,  6.799614749e-01,  6.688949766e-01, /*EDIAM*/  4.121588091e-02,  6.160103013e-02,  3.726501435e-02,  3.898617076e-02, /*DMEAN*/  6.921446152e-01, /*EDMEAN*/  2.070239555e-02 },
        {123, /* SP */ 228, /*MAG*/  6.993000000e+00,  5.350000000e+00,  3.230000000e+00,  1.876000000e+00,  1.037000000e+00,  8.150000000e-01, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  5.008992000e-02,  2.860000000e-01,  2.580000000e-01,  2.780000000e-01, /*DIAM*/  6.666136450e-01,  6.186298314e-01,  6.553418890e-01,  6.516490780e-01, /*EDIAM*/  4.131329595e-02,  7.930303838e-02,  6.234520388e-02,  6.297513103e-02, /*DMEAN*/  6.551432671e-01, /*EDMEAN*/  2.774088331e-02 },
        {124, /* SP */ 220, /*MAG*/  3.646000000e+00,  2.070000000e+00,  3.300000000e-01, -9.570000000e-01, -1.674000000e+00, -1.846000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.060000000e-01,  1.480000000e-01,  1.600000000e-01, /*DIAM*/  1.204764089e+00,  1.147798187e+00,  1.156974353e+00,  1.153131106e+00, /*EDIAM*/  4.124821477e-02,  5.720936880e-02,  3.586084689e-02,  3.630933919e-02, /*DMEAN*/  1.166809703e+00, /*EDMEAN*/  1.982906048e-02 },
        {125, /* SP */ 236, /*MAG*/  4.848000000e+00,  3.320000000e+00,  5.600000000e-01, -7.790000000e-01, -1.675000000e+00, -1.904000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  6.067125000e-02,  1.820000000e-01,  1.580000000e-01,  1.520000000e-01, /*DIAM*/  1.100779971e+00,  1.196385014e+00,  1.245629920e+00,  1.228885646e+00, /*EDIAM*/  4.156379099e-02,  5.073369883e-02,  3.843683177e-02,  3.464882788e-02, /*DMEAN*/  1.196550783e+00, /*EDMEAN*/  1.966125840e-02 },
        {126, /* SP */ 200, /*MAG*/  2.408000000e+00,  8.700000000e-01, -8.000000000e-01, -2.095000000e+00, -2.775000000e+00, -3.044000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  1.940000000e-01,  1.700000000e-01,  1.400000000e-01, /*DIAM*/  1.311522700e+00,  1.349936934e+00,  1.340728556e+00,  1.361062830e+00, /*EDIAM*/  4.123976885e-02,  5.391790149e-02,  4.115293056e-02,  3.180987386e-02, /*DMEAN*/  1.342580427e+00, /*EDMEAN*/  1.951298048e-02 },
        {127, /* SP */ 228, /*MAG*/  6.380000000e+00,  4.320000000e+00,  1.780000000e+00,  3.930000000e-01, -6.090000000e-01, -9.130000000e-01, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.200000000e-01,  1.860000000e-01,  1.620000000e-01, /*DIAM*/  1.131153652e+00,  9.500137649e-01,  1.009948898e+00,  1.015590689e+00, /*EDIAM*/  4.131329595e-02,  6.107776452e-02,  4.501568571e-02,  3.677696588e-02, /*DMEAN*/  1.039369435e+00, /*EDMEAN*/  2.143176939e-02 },
        {128, /* SP */ 232, /*MAG*/  6.001000000e+00,  4.300000000e+00,  1.790000000e+00,  2.450000000e-01, -6.020000000e-01, -8.130000000e-01, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.200000000e-01,  1.740000000e-01,  1.680000000e-01, /*DIAM*/  9.592080055e-01,  9.894315317e-01,  1.016971093e+00,  1.000001786e+00, /*EDIAM*/  4.140373314e-02,  6.111213746e-02,  4.217611568e-02,  3.816827302e-02, /*DMEAN*/  9.913544718e-01, /*EDMEAN*/  2.136809310e-02 },
        {129, /* SP */ 232, /*MAG*/  4.491000000e+00,  2.870000000e+00,  5.700000000e-01, -9.120000000e-01, -1.608000000e+00, -1.862000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  5.035871000e-02,  1.880000000e-01,  1.900000000e-01,  1.800000000e-01, /*DIAM*/  1.195608009e+00,  1.199869035e+00,  1.200683158e+00,  1.199790111e+00, /*EDIAM*/  4.140373314e-02,  5.229214929e-02,  4.601931072e-02,  4.087339370e-02, /*DMEAN*/  1.198756934e+00, /*EDMEAN*/  2.175318215e-02 },
        {130, /* SP */ 236, /*MAG*/  5.795000000e+00,  4.220000000e+00,  1.620000000e+00, -1.120000000e-01, -1.039000000e+00, -1.255000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.100000000e-01,  1.660000000e-01,  1.760000000e-01, /*DIAM*/  9.499199685e-01,  1.080876083e+00,  1.129318634e+00,  1.105681265e+00, /*EDIAM*/  4.156379099e-02,  5.843614616e-02,  4.035148573e-02,  4.004496314e-02, /*DMEAN*/  1.064497920e+00, /*EDMEAN*/  2.132189579e-02 },
        {131, /* SP */ 230, /*MAG*/  4.095000000e+00,  2.440000000e+00,  1.300000000e-01, -1.432000000e+00, -2.129000000e+00, -2.379000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  6.053098000e-02,  1.940000000e-01,  1.800000000e-01,  1.540000000e-01, /*DIAM*/  1.278622113e+00,  1.310928683e+00,  1.303784020e+00,  1.301667562e+00, /*EDIAM*/  4.135154739e-02,  5.392079872e-02,  4.359066989e-02,  3.498948922e-02, /*DMEAN*/  1.297360004e+00, /*EDMEAN*/  2.052092339e-02 },
        {132, /* SP */ 230, /*MAG*/  4.095000000e+00,  2.440000000e+00,  1.300000000e-01, -1.432000000e+00, -2.129000000e+00, -2.379000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  6.053098000e-02,  1.940000000e-01,  1.800000000e-01,  1.540000000e-01, /*DIAM*/  1.278622113e+00,  1.310928683e+00,  1.303784020e+00,  1.301667562e+00, /*EDIAM*/  4.135154739e-02,  5.392079872e-02,  4.359066989e-02,  3.498948922e-02, /*DMEAN*/  1.297360004e+00, /*EDMEAN*/  2.052092339e-02 },
        {133, /* SP */ 120, /*MAG*/  4.848000000e+00,  4.070000000e+00,  3.260000000e+00,  2.865000000e+00,  2.608000000e+00,  2.354000000e+00, /*EMAG*/  5.661272000e-02,  3.999999911e-02,  4.459821000e-02,  2.000000000e-01,  1.900000000e-01,  2.160000000e-01, /*DIAM*/  2.436710593e-01,  1.141813391e-01,  1.265793464e-01,  1.547659875e-01, /*EDIAM*/  4.827369406e-02,  5.560348872e-02,  4.600893712e-02,  4.896817113e-02, /*DMEAN*/  1.628487954e-01, /*EDMEAN*/  2.425150237e-02 },
        {134, /* SP */ 228, /*MAG*/  7.554000000e+00,  5.970000000e+00,  3.770000000e+00,  2.450000000e+00,  1.582000000e+00,  1.444000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  3.180000000e-01,  2.300000000e-01,  2.380000000e-01, /*DIAM*/  5.060336426e-01,  5.073619726e-01,  5.494352726e-01,  5.256125798e-01, /*EDIAM*/  4.131329595e-02,  8.814588109e-02,  5.560297097e-02,  5.393578812e-02, /*DMEAN*/  5.210615124e-01, /*EDMEAN*/  2.643609112e-02 },
        {135, /* SP */ 180, /*MAG*/  5.893000000e+00,  4.880000000e+00,  3.890000000e+00,  3.025000000e+00,  2.578000000e+00,  2.494000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.680000000e-01,  2.100000000e-01,  2.320000000e-01, /*DIAM*/  1.557556670e-01,  2.109300733e-01,  1.945516580e-01,  1.886543361e-01, /*EDIAM*/  4.121196542e-02,  7.430984340e-02,  5.074733235e-02,  5.255147946e-02, /*DMEAN*/  1.799433916e-01, /*EDMEAN*/  2.514680470e-02 },
        {136, /* SP */ 108, /*MAG*/  2.660000000e+00,  2.280000000e+00,  1.880000000e+00,  1.714000000e+00,  1.585000000e+00,  1.454000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  3.100000000e-01,  1.740000000e-01,  2.020000000e-01, /*DIAM*/  3.592850947e-01,  2.831674664e-01,  2.923266633e-01,  3.027029326e-01, /*EDIAM*/  4.140392777e-02,  8.597300367e-02,  4.218750461e-02,  4.582860043e-02, /*DMEAN*/  3.170917457e-01, /*EDMEAN*/  2.341582153e-02 },
        {137, /* SP */ 172, /*MAG*/  5.211000000e+00,  4.340000000e+00,  3.420000000e+00,  2.578000000e+00,  2.154000000e+00,  2.074000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.340000000e-01,  1.980000000e-01,  2.500000000e-01, /*DIAM*/  1.787060011e-01,  2.807593039e-01,  2.690820913e-01,  2.615154085e-01, /*EDIAM*/  4.121588091e-02,  6.491454875e-02,  4.785468345e-02,  5.661508964e-02, /*DMEAN*/  2.350789906e-01, /*EDMEAN*/  2.466643877e-02 },
        {138, /* SP */ 180, /*MAG*/  3.410000000e+00,  2.240000000e+00,  1.110000000e+00,  3.110000000e-01, -2.100000000e-01, -3.030000000e-01, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  1.980000000e-01,  1.600000000e-01,  1.700000000e-01, /*DIAM*/  7.810956764e-01,  7.594122244e-01,  7.582559465e-01,  7.521798920e-01, /*EDIAM*/  4.121196542e-02,  5.498112280e-02,  3.871183635e-02,  3.854235465e-02, /*DMEAN*/  7.628889880e-01, /*EDMEAN*/  2.054840746e-02 },
        {139, /* SP */ 172, /*MAG*/  5.577000000e+00,  4.620000000e+00,  3.610000000e+00,  3.123000000e+00,  2.680000000e+00,  2.468000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  4.004997000e-02,  2.380000000e-01,  1.880000000e-01,  2.800000000e-01, /*DIAM*/  1.760260011e-01,  1.514110877e-01,  1.537357861e-01,  1.797197869e-01, /*EDIAM*/  4.121588091e-02,  6.601923161e-02,  4.544691145e-02,  6.339851259e-02, /*DMEAN*/  1.660412744e-01, /*EDMEAN*/  2.484790193e-02 },
        {140, /* SP */ 174, /*MAG*/  5.684000000e+00,  4.660000000e+00,  3.670000000e+00,  3.074000000e+00,  2.513000000e+00,  2.481000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.260000000e-01,  2.180000000e-01,  2.520000000e-01, /*DIAM*/  2.082718961e-01,  1.711361868e-01,  1.969317877e-01,  1.797323105e-01, /*EDIAM*/  4.121350068e-02,  6.270540904e-02,  5.267137716e-02,  5.706816799e-02, /*DMEAN*/  1.935912912e-01, /*EDMEAN*/  2.518483998e-02 },
        {141, /* SP */ 176, /*MAG*/  5.671000000e+00,  4.680000000e+00,  3.670000000e+00,  3.019000000e+00,  2.481000000e+00,  2.311000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.000000000e-01,  1.640000000e-01,  2.180000000e-01, /*DIAM*/  1.828610929e-01,  1.910008674e-01,  2.068074134e-01,  2.206892881e-01, /*EDIAM*/  4.121207570e-02,  5.552936052e-02,  3.967093276e-02,  4.938333517e-02, /*DMEAN*/  1.995482088e-01, /*EDMEAN*/  2.206344723e-02 },
        {142, /* SP */ 192, /*MAG*/  4.865000000e+00,  3.590000000e+00,  2.360000000e+00,  1.420000000e+00,  7.410000000e-01,  6.490000000e-01, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.420000000e-01,  1.540000000e-01,  2.040000000e-01, /*DIAM*/  5.856128526e-01,  5.745115918e-01,  5.953606152e-01,  5.861959538e-01, /*EDIAM*/  4.122737356e-02,  6.714678661e-02,  3.729164026e-02,  4.624075505e-02, /*DMEAN*/  5.879620099e-01, /*EDMEAN*/  2.188740635e-02 },
        {143, /* SP */ 192, /*MAG*/  5.797000000e+00,  4.450000000e+00,  3.080000000e+00,  2.195000000e+00,  1.495000000e+00,  1.357000000e+00, /*EMAG*/  4.809366000e-02,  3.999999911e-02,  3.999999911e-02,  3.080000000e-01,  2.600000000e-01,  3.080000000e-01, /*DIAM*/  4.582528507e-01,  4.260383753e-01,  4.489325974e-01,  4.485901124e-01, /*EDIAM*/  4.442717189e-02,  8.537942333e-02,  6.280669359e-02,  6.974655643e-02, /*DMEAN*/  4.502687811e-01, /*EDMEAN*/  2.964406230e-02 },
        {144, /* SP */ 188, /*MAG*/  3.161000000e+00,  2.010000000e+00,  8.800000000e-01, -1.960000000e-01, -7.490000000e-01, -7.830000000e-01, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.180000000e-01,  1.560000000e-01,  1.760000000e-01, /*DIAM*/  8.193742439e-01,  8.944534343e-01,  8.856415365e-01,  8.637797692e-01, /*EDIAM*/  4.122057722e-02,  6.051369180e-02,  3.776350066e-02,  3.991055356e-02, /*DMEAN*/  8.621116294e-01, /*EDMEAN*/  2.088674756e-02 },
        {145, /* SP */ 186, /*MAG*/  5.632000000e+00,  4.520000000e+00,  3.480000000e+00,  2.875000000e+00,  2.347000000e+00,  2.099000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  3.140000000e-01,  2.160000000e-01,  2.780000000e-01, /*DIAM*/  2.914006234e-01,  2.341129447e-01,  2.404207538e-01,  2.752451214e-01, /*EDIAM*/  4.121758890e-02,  8.702918198e-02,  5.219940072e-02,  6.295716352e-02, /*DMEAN*/  2.687614571e-01, /*EDMEAN*/  2.682729849e-02 },
        {146, /* SP */ 188, /*MAG*/  5.383000000e+00,  4.350000000e+00,  3.390000000e+00,  2.775000000e+00,  2.303000000e+00,  2.169000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.740000000e-01,  1.800000000e-01,  2.480000000e-01, /*DIAM*/  2.782142358e-01,  2.518016390e-01,  2.458749900e-01,  2.572980083e-01, /*EDIAM*/  4.122057722e-02,  7.597811991e-02,  4.353619698e-02,  5.617766542e-02, /*DMEAN*/  2.607699679e-01, /*EDMEAN*/  2.444599129e-02 },
        {147, /* SP */ 120, /*MAG*/  2.271000000e+00,  1.790000000e+00,  1.160000000e+00,  8.300000000e-01,  4.130000000e-01,  5.200000000e-01, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  4.019950000e-02,  1.840000000e-01,  1.620000000e-01,  1.640000000e-01, /*DIAM*/  5.155310634e-01,  5.023688449e-01,  5.620735163e-01,  5.098462848e-01, /*EDIAM*/  4.139308516e-02,  5.119623122e-02,  3.928372230e-02,  3.723091686e-02, /*DMEAN*/  5.243397714e-01, /*EDMEAN*/  2.023142224e-02 },
        {148, /* SP */ 180, /*MAG*/  5.424000000e+00,  4.360000000e+00,  3.340000000e+00,  2.579000000e+00,  2.055000000e+00,  2.033000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.580000000e-01,  2.260000000e-01,  2.440000000e-01, /*DIAM*/  2.913756691e-01,  2.944479317e-01,  2.992753949e-01,  2.793039725e-01, /*EDIAM*/  4.121196542e-02,  7.154706486e-02,  5.460118552e-02,  5.526409924e-02, /*DMEAN*/  2.909363630e-01, /*EDMEAN*/  2.577610433e-02 },
        {149, /* SP */ 100, /*MAG*/  2.791000000e+00,  2.580000000e+00,  2.260000000e+00,  2.030000000e+00,  1.857000000e+00,  1.756000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  3.280000000e-01,  2.240000000e-01,  2.260000000e-01, /*DIAM*/  1.918707211e-01,  2.102927343e-01,  2.325366078e-01,  2.352130898e-01, /*EDIAM*/  4.141222888e-02,  9.096093276e-02,  5.422393229e-02,  5.126793889e-02, /*DMEAN*/  2.145097159e-01, /*EDMEAN*/  2.605368591e-02 },
        {150, /* SP */ 172, /*MAG*/  4.437000000e+00,  3.060000000e+00,  1.840000000e+00,  8.860000000e-01,  2.320000000e-01,  1.250000000e-01, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.220000000e-01,  1.540000000e-01,  1.720000000e-01, /*DIAM*/  7.484260096e-01,  6.507950237e-01,  6.799614749e-01,  6.688949766e-01, /*EDIAM*/  4.121588091e-02,  6.160103013e-02,  3.726501435e-02,  3.898617076e-02, /*DMEAN*/  6.921446152e-01, /*EDMEAN*/  2.070239555e-02 },
        {151, /* SP */ 196, /*MAG*/  5.011000000e+00,  3.530000000e+00,  2.060000000e+00,  1.191000000e+00,  2.670000000e-01,  1.900000000e-01, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.500000000e-01,  1.600000000e-01,  2.020000000e-01, /*DIAM*/  7.333001895e-01,  6.391140015e-01,  7.116743346e-01,  6.936886640e-01, /*EDIAM*/  4.123418936e-02,  6.936281577e-02,  3.874230774e-02,  4.579464982e-02, /*DMEAN*/  7.062452775e-01, /*EDMEAN*/  2.222353274e-02 },
        {152, /* SP */ 228, /*MAG*/  6.269000000e+00,  4.680000000e+00,  2.720000000e+00,  1.530000000e+00,  7.060000000e-01,  4.900000000e-01, /*EMAG*/  4.110961000e-02,  3.999999911e-02,  3.999999911e-02,  2.800000000e-01,  2.140000000e-01,  2.280000000e-01, /*DIAM*/  7.671336465e-01,  6.629512606e-01,  7.075597886e-01,  7.075833854e-01, /*EDIAM*/  4.172989890e-02,  7.764536829e-02,  5.175174550e-02,  5.167659521e-02, /*DMEAN*/  7.257889945e-01, /*EDMEAN*/  2.544561310e-02 },
        {153, /* SP */ 196, /*MAG*/  5.838000000e+00,  4.390000000e+00,  2.880000000e+00,  1.897000000e+00,  1.220000000e+00,  1.022000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  6.007495000e-02,  3.220000000e-01,  2.500000000e-01,  3.220000000e-01, /*DIAM*/  5.408401866e-01,  5.097389996e-01,  5.172385341e-01,  5.280244280e-01, /*EDIAM*/  4.123418936e-02,  8.925424381e-02,  6.040295872e-02,  7.291568422e-02, /*DMEAN*/  5.299353882e-01, /*EDMEAN*/  2.867700340e-02 },
        {154, /* SP */ 194, /*MAG*/  6.299000000e+00,  4.770000000e+00,  3.150000000e+00,  2.160000000e+00,  1.390000000e+00,  1.247000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  4.031129000e-02,  2.660000000e-01,  2.440000000e-01,  2.640000000e-01, /*DIAM*/  5.107315488e-01,  4.632391815e-01,  4.896251564e-01,  4.844581124e-01, /*EDIAM*/  4.123086059e-02,  7.377820551e-02,  5.895503883e-02,  5.980145742e-02, /*DMEAN*/  4.942071152e-01, /*EDMEAN*/  2.681344018e-02 },
        {155, /* SP */ 182, /*MAG*/  4.871000000e+00,  3.690000000e+00,  2.540000000e+00,  1.679000000e+00,  1.025000000e+00,  9.880000000e-01, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.620000000e-01,  1.620000000e-01,  1.900000000e-01, /*DIAM*/  4.981860068e-01,  4.952234663e-01,  5.216968305e-01,  5.003147338e-01, /*EDIAM*/  4.121316064e-02,  7.265426221e-02,  3.919550493e-02,  4.306269048e-02, /*DMEAN*/  5.061507280e-01, /*EDMEAN*/  2.206543502e-02 },
        {156, /* SP */ 172, /*MAG*/  3.784000000e+00,  2.850000000e+00,  2.020000000e+00,  1.248000000e+00,  7.330000000e-01,  6.640000000e-01, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.760000000e-01,  2.440000000e-01,  2.780000000e-01, /*DIAM*/  5.157660061e-01,  5.344735934e-01,  5.504361811e-01,  5.414132229e-01, /*EDIAM*/  4.121588091e-02,  7.651726541e-02,  5.893536422e-02,  6.294625154e-02, /*DMEAN*/  5.307954676e-01, /*EDMEAN*/  2.722721514e-02 },
        {157, /* SP */ 202, /*MAG*/  5.570000000e+00,  4.050000000e+00,  2.450000000e+00,  1.218000000e+00,  4.380000000e-01,  4.350000000e-01, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.640000000e-01,  1.800000000e-01,  1.820000000e-01, /*DIAM*/  6.709960806e-01,  6.797757539e-01,  6.993982570e-01,  6.602574786e-01, /*EDIAM*/  4.124177049e-02,  7.323617043e-02,  4.355984099e-02,  4.128494080e-02, /*DMEAN*/  6.764563844e-01, /*EDMEAN*/  2.255063423e-02 },
        {158, /* SP */ 172, /*MAG*/  4.421000000e+00,  3.460000000e+00,  2.500000000e+00,  1.659000000e+00,  9.850000000e-01,  1.223000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.380000000e-01,  1.920000000e-01,  1.960000000e-01, /*DIAM*/  4.105060046e-01,  4.675539495e-01,  5.148019393e-01,  4.309533673e-01, /*EDIAM*/  4.121588091e-02,  6.601923161e-02,  4.640996193e-02,  4.440870073e-02, /*DMEAN*/  4.503248694e-01, /*EDMEAN*/  2.314559663e-02 },
        {159, /* SP */ 170, /*MAG*/  4.396000000e+00,  3.480000000e+00,  2.590000000e+00,  1.768000000e+00,  1.366000000e+00,  1.333000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  3.040000000e-01,  1.920000000e-01,  2.200000000e-01, /*DIAM*/  3.802127545e-01,  4.358485630e-01,  4.225226513e-01,  4.047449260e-01, /*EDIAM*/  4.121922010e-02,  8.425632516e-02,  4.641071796e-02,  4.983235186e-02, /*DMEAN*/  4.034883951e-01, /*EDMEAN*/  2.455148321e-02 },
        {160, /* SP */ 192, /*MAG*/  4.597000000e+00,  3.160000000e+00,  1.850000000e+00,  7.240000000e-01, -1.120000000e-01, -2.300000000e-02, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  1.780000000e-01,  1.780000000e-01,  1.600000000e-01, /*DIAM*/  7.720528553e-01,  7.341365942e-01,  7.834073106e-01,  7.269550800e-01, /*EDIAM*/  4.122737356e-02,  4.949146678e-02,  4.306271908e-02,  3.630654260e-02, /*DMEAN*/  7.533914444e-01, /*EDMEAN*/  2.038396054e-02 },
        {161, /* SP */ 184, /*MAG*/  5.102000000e+00,  4.020000000e+00,  3.020000000e+00,  2.267000000e+00,  1.911000000e+00,  1.786000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.860000000e-01,  2.040000000e-01,  2.180000000e-01, /*DIAM*/  3.715495565e-01,  3.609177326e-01,  3.232264676e-01,  3.306402791e-01, /*EDIAM*/  4.121507289e-02,  7.928805486e-02,  4.930690713e-02,  4.939206465e-02, /*DMEAN*/  3.474028543e-01, /*EDMEAN*/  2.476348322e-02 },
        {162, /* SP */ 180, /*MAG*/  5.608000000e+00,  4.350000000e+00,  3.220000000e+00,  2.213000000e+00,  1.598000000e+00,  1.514000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.660000000e-01,  1.920000000e-01,  2.160000000e-01, /*DIAM*/  4.136556709e-01,  3.949836475e-01,  4.091119725e-01,  3.964791567e-01, /*EDIAM*/  4.121196542e-02,  7.375725723e-02,  4.641295320e-02,  4.893512045e-02, /*DMEAN*/  4.060423706e-01, /*EDMEAN*/  2.409263556e-02 },
        {163, /* SP */ 176, /*MAG*/  4.750000000e+00,  3.800000000e+00,  2.950000000e+00,  2.319000000e+00,  1.823000000e+00,  1.757000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.660000000e-01,  1.740000000e-01,  1.860000000e-01, /*DIAM*/  3.334410951e-01,  3.171794407e-01,  3.292509950e-01,  3.229228662e-01, /*EDIAM*/  4.121207570e-02,  7.375457400e-02,  4.207726525e-02,  4.215188575e-02, /*DMEAN*/  3.275673688e-01, /*EDMEAN*/  2.247004753e-02 },
        {164, /* SP */ 192, /*MAG*/  4.227000000e+00,  2.720000000e+00,  1.280000000e+00,  2.760000000e-01, -5.440000000e-01, -7.200000000e-01, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  1.680000000e-01,  2.240000000e-01,  2.440000000e-01, /*DIAM*/  9.034528573e-01,  8.243508813e-01,  8.694773508e-01,  8.731083669e-01, /*EDIAM*/  4.122737356e-02,  4.673685422e-02,  5.413517939e-02,  5.527872445e-02, /*DMEAN*/  8.699650664e-01, /*EDMEAN*/  2.353830738e-02 },
        {165, /* SP */ 178, /*MAG*/  4.565000000e+00,  3.710000000e+00,  2.820000000e+00,  2.294000000e+00,  1.925000000e+00,  1.712000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.480000000e-01,  2.020000000e-01,  2.300000000e-01, /*DIAM*/  2.919649803e-01,  3.203031821e-01,  3.023395340e-01,  3.327674506e-01, /*EDIAM*/  4.121158010e-02,  6.878297174e-02,  4.881921389e-02,  5.209745065e-02, /*DMEAN*/  3.077631262e-01, /*EDMEAN*/  2.458005071e-02 },
        {166, /* SP */ 132, /*MAG*/  2.903000000e+00,  2.230000000e+00,  1.580000000e+00,  1.369000000e+00,  8.520000000e-01,  8.270000000e-01, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  3.100000000e-01,  2.020000000e-01,  2.240000000e-01, /*DIAM*/  5.340369722e-01,  3.996702802e-01,  4.794834252e-01,  4.594452762e-01, /*EDIAM*/  4.136756490e-02,  8.595681518e-02,  4.887954081e-02,  5.076306536e-02, /*DMEAN*/  4.884378028e-01, /*EDMEAN*/  2.513312601e-02 },
        {167, /* SP */ 180, /*MAG*/  4.322000000e+00,  3.410000000e+00,  2.470000000e+00,  1.914000000e+00,  1.504000000e+00,  1.393000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  4.004997000e-02,  2.760000000e-01,  1.700000000e-01,  2.380000000e-01, /*DIAM*/  3.871356705e-01,  4.055640048e-01,  3.930185870e-01,  3.991579889e-01, /*EDIAM*/  4.121196542e-02,  7.652033051e-02,  4.111768088e-02,  5.390775526e-02, /*DMEAN*/  3.934352904e-01, /*EDMEAN*/  2.378744775e-02 },
        {168, /* SP */ 198, /*MAG*/  5.329000000e+00,  3.720000000e+00,  2.090000000e+00,  9.820000000e-01,  2.700000000e-02, -3.800000000e-02, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.320000000e-01,  1.780000000e-01,  2.020000000e-01, /*DIAM*/  7.797174373e-01,  7.143677064e-01,  7.797996970e-01,  7.529826752e-01, /*EDIAM*/  4.123720484e-02,  6.439683562e-02,  4.307366331e-02,  4.579701510e-02, /*DMEAN*/  7.643527285e-01, /*EDMEAN*/  2.278318496e-02 },
        {169, /* SP */ 160, /*MAG*/  4.865000000e+00,  3.980000000e+00,  3.040000000e+00,  2.485000000e+00,  2.013000000e+00,  1.901000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.580000000e-01,  2.560000000e-01,  3.180000000e-01, /*DIAM*/  2.726941474e-01,  2.608768270e-01,  2.819887601e-01,  2.809655193e-01, /*EDIAM*/  4.124903710e-02,  7.155451864e-02,  6.183631213e-02,  7.199301126e-02, /*DMEAN*/  2.740074497e-01, /*EDMEAN*/  2.789177174e-02 },
        {170, /* SP */ 148, /*MAG*/  3.919000000e+00,  2.950000000e+00,  2.030000000e+00,  1.278000000e+00,  7.400000000e-01,  5.900000000e-01, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.800000000e-01,  2.160000000e-01,  2.140000000e-01, /*DIAM*/  5.493425963e-01,  4.993702509e-01,  5.418756727e-01,  5.419760992e-01, /*EDIAM*/  4.130274461e-02,  7.765108435e-02,  5.222215055e-02,  4.848807511e-02, /*DMEAN*/  5.401289599e-01, /*EDMEAN*/  2.497391622e-02 },
        {171, /* SP */ 174, /*MAG*/  5.435000000e+00,  4.420000000e+00,  3.390000000e+00,  2.567000000e+00,  2.003000000e+00,  1.880000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.320000000e-01,  1.700000000e-01,  2.180000000e-01, /*DIAM*/  2.506918967e-01,  2.930379744e-01,  3.100679761e-01,  3.094184438e-01, /*EDIAM*/  4.121350068e-02,  6.436220559e-02,  4.111413142e-02,  4.938194668e-02, /*DMEAN*/  2.880692615e-01, /*EDMEAN*/  2.285276637e-02 },
        {172, /* SP */ 120, /*MAG*/  4.848000000e+00,  4.070000000e+00,  3.260000000e+00,  2.865000000e+00,  2.608000000e+00,  2.354000000e+00, /*EMAG*/  5.661272000e-02,  3.999999911e-02,  4.459821000e-02,  2.000000000e-01,  1.900000000e-01,  2.160000000e-01, /*DIAM*/  2.436710593e-01,  1.141813391e-01,  1.265793464e-01,  1.547659875e-01, /*EDIAM*/  4.827369406e-02,  5.560348872e-02,  4.600893712e-02,  4.896817113e-02, /*DMEAN*/  1.628487954e-01, /*EDMEAN*/  2.425150237e-02 },
        {173, /* SP */ 190, /*MAG*/  5.818000000e+00,  4.500000000e+00,  3.250000000e+00,  2.435000000e+00,  1.817000000e+00,  1.668000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  3.020000000e-01,  2.260000000e-01,  2.160000000e-01, /*DIAM*/  4.272861514e-01,  3.604581026e-01,  3.712583586e-01,  3.770332467e-01, /*EDIAM*/  4.122389129e-02,  8.371824248e-02,  5.461372476e-02,  4.894874008e-02, /*DMEAN*/  3.942541272e-01, /*EDMEAN*/  2.549169879e-02 },
        {174, /* SP */ 172, /*MAG*/  5.040000000e+00,  3.970000000e+00,  2.980000000e+00,  2.030000000e+00,  1.462000000e+00,  1.508000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.380000000e-01,  2.040000000e-01,  2.380000000e-01, /*DIAM*/  3.760860040e-01,  4.040271629e-01,  4.207630274e-01,  3.798657753e-01, /*EDIAM*/  4.121588091e-02,  6.601923161e-02,  4.929956626e-02,  5.390206578e-02, /*DMEAN*/  3.925747790e-01, /*EDMEAN*/  2.468300877e-02 },
        {175, /* SP */ 172, /*MAG*/  4.443000000e+00,  3.510000000e+00,  2.620000000e+00,  1.700000000e+00,  1.243000000e+00,  1.181000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.620000000e-01,  1.700000000e-01,  1.560000000e-01, /*DIAM*/  3.831460041e-01,  4.600450209e-01,  4.546229501e-01,  4.417708857e-01, /*EDIAM*/  4.121588091e-02,  7.264890158e-02,  4.111430435e-02,  3.537253612e-02, /*DMEAN*/  4.303562606e-01, /*EDMEAN*/  2.103212875e-02 },
        {176, /* SP */ 168, /*MAG*/  5.479000000e+00,  4.540000000e+00,  3.610000000e+00,  3.028000000e+00,  2.659000000e+00,  2.442000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.520000000e-01,  2.200000000e-01,  3.520000000e-01, /*DIAM*/  1.843622176e-01,  1.654444137e-01,  1.531869989e-01,  1.798715903e-01, /*EDIAM*/  4.122350083e-02,  6.988799623e-02,  5.315510438e-02,  7.968154677e-02, /*DMEAN*/  1.724354641e-01, /*EDMEAN*/  2.713263995e-02 },
        {177, /* SP */ 148, /*MAG*/  5.700000000e+00,  4.510000000e+00,  3.360000000e+00,  2.269000000e+00,  1.915000000e+00,  1.670000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.620000000e-01,  1.640000000e-01,  2.100000000e-01, /*DIAM*/  3.743625937e-01,  3.448613200e-01,  3.227550468e-01,  3.385892348e-01, /*EDIAM*/  4.130274461e-02,  7.267929913e-02,  3.971317831e-02,  4.758419666e-02, /*DMEAN*/  3.455073954e-01, /*EDMEAN*/  2.276081298e-02 },
        {178, /* SP */ 192, /*MAG*/  4.538000000e+00,  3.270000000e+00,  2.040000000e+00,  9.860000000e-01,  4.030000000e-01,  4.670000000e-01, /*EMAG*/  4.011234000e-02,  3.999999911e-02,  3.999999911e-02,  2.400000000e-01,  1.720000000e-01,  1.900000000e-01, /*DIAM*/  6.452728534e-01,  6.700651647e-01,  6.637030286e-01,  6.189696769e-01, /*EDIAM*/  4.126930928e-02,  6.659458382e-02,  4.161945968e-02,  4.307876786e-02, /*DMEAN*/  6.464843288e-01, /*EDMEAN*/  2.228668460e-02 },
        {179, /* SP */ 176, /*MAG*/  4.088000000e+00,  3.110000000e+00,  2.150000000e+00,  1.415000000e+00,  7.580000000e-01,  6.970000000e-01, /*EMAG*/  4.909175000e-02,  3.999999911e-02,  3.999999911e-02,  2.340000000e-01,  1.600000000e-01,  1.900000000e-01, /*DIAM*/  4.888010974e-01,  5.144115865e-01,  5.577179244e-01,  5.446454973e-01, /*EDIAM*/  4.483078376e-02,  6.491511309e-02,  3.870861305e-02,  4.305564365e-02, /*DMEAN*/  5.304820342e-01, /*EDMEAN*/  2.225747080e-02 },
        {180, /* SP */ 208, /*MAG*/  4.690000000e+00,  3.140000000e+00,  1.490000000e+00,  1.200000000e-01, -5.780000000e-01, -6.560000000e-01, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.200000000e-01,  1.700000000e-01,  1.720000000e-01, /*DIAM*/  8.967572525e-01,  9.211325408e-01,  9.156301924e-01,  8.921898173e-01, /*EDIAM*/  4.124396433e-02,  6.109164872e-02,  4.115794873e-02,  3.902886317e-02, /*DMEAN*/  9.037331893e-01, /*EDMEAN*/  2.133520844e-02 },
        {181, /* SP */ 144, /*MAG*/  3.778000000e+00,  2.970000000e+00,  2.160000000e+00,  1.511000000e+00,  1.107000000e+00,  1.223000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.640000000e-01,  1.820000000e-01,  2.280000000e-01, /*DIAM*/  4.519676188e-01,  4.313133444e-01,  4.527532575e-01,  3.966735460e-01, /*EDIAM*/  4.132123941e-02,  7.323790269e-02,  4.404950524e-02,  5.165577578e-02, /*DMEAN*/  4.376716919e-01, /*EDMEAN*/  2.403569640e-02 },
        {182, /* SP */ 220, /*MAG*/  4.663000000e+00,  3.060000000e+00,  1.290000000e+00, -1.080000000e-01, -9.060000000e-01, -1.009000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  1.920000000e-01,  1.560000000e-01,  1.700000000e-01, /*DIAM*/  1.023504086e+00,  9.888249699e-01,  1.012530771e+00,  9.897515417e-01, /*EDIAM*/  4.124821477e-02,  5.334828133e-02,  3.778281129e-02,  3.856595508e-02, /*DMEAN*/  1.005390650e+00, /*EDMEAN*/  2.031383255e-02 },
        {183, /* SP */ 184, /*MAG*/  4.144000000e+00,  3.000000000e+00,  1.910000000e+00,  1.030000000e+00,  3.760000000e-01,  4.290000000e-01, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  1.800000000e-01,  1.700000000e-01,  1.980000000e-01, /*DIAM*/  6.139895601e-01,  6.249802365e-01,  6.514677176e-01,  6.108957577e-01, /*EDIAM*/  4.121507289e-02,  5.002351234e-02,  4.112310640e-02,  4.487293174e-02, /*DMEAN*/  6.258732627e-01, /*EDMEAN*/  2.141081688e-02 },
        {184, /* SP */ 220, /*MAG*/  5.433000000e+00,  3.820000000e+00,  2.030000000e+00,  8.950000000e-01, -4.900000000e-02, -1.140000000e-01, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.120000000e-01,  1.960000000e-01,  1.740000000e-01, /*DIAM*/  8.777040837e-01,  7.695660381e-01,  8.371299905e-01,  8.072040937e-01, /*EDIAM*/  4.124821477e-02,  5.886468029e-02,  4.740238146e-02,  3.946879886e-02, /*DMEAN*/  8.300623899e-01, /*EDMEAN*/  2.208266016e-02 },
        {185, /* SP */ 224, /*MAG*/  5.541000000e+00,  4.040000000e+00,  2.250000000e+00,  1.176000000e+00,  3.250000000e-01,  1.570000000e-01, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.240000000e-01,  2.140000000e-01,  2.600000000e-01, /*DIAM*/  7.997255660e-01,  7.107168245e-01,  7.642066267e-01,  7.587721881e-01, /*EDIAM*/  4.126750894e-02,  6.217267618e-02,  5.173727108e-02,  5.889774034e-02, /*DMEAN*/  7.679092292e-01, /*EDMEAN*/  2.520772210e-02 },
        {186, /* SP */ 140, /*MAG*/  3.260000000e+00,  2.680000000e+00,  2.030000000e+00,  1.793000000e+00,  1.534000000e+00,  1.485000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  3.060000000e-01,  1.540000000e-01,  1.700000000e-01, /*DIAM*/  3.748930407e-01,  3.260927728e-01,  3.363906703e-01,  3.272603329e-01, /*EDIAM*/  4.133861416e-02,  8.484480339e-02,  3.732992507e-02,  3.855872229e-02, /*DMEAN*/  3.438984320e-01, /*EDMEAN*/  2.131837350e-02 },
        {187, /* SP */ 172, /*MAG*/  4.446000000e+00,  3.490000000e+00,  2.600000000e+00,  1.795000000e+00,  1.268000000e+00,  1.223000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  3.140000000e-01,  1.180000000e-01,  1.660000000e-01, /*DIAM*/  4.014060044e-01,  4.322146633e-01,  4.477669189e-01,  4.317416884e-01, /*EDIAM*/  4.121588091e-02,  8.702008678e-02,  2.861458696e-02,  3.763090451e-02, /*DMEAN*/  4.319252429e-01, /*EDMEAN*/  1.900005920e-02 },
        {188, /* SP */ 188, /*MAG*/  3.797000000e+00,  2.630000000e+00,  1.540000000e+00,  8.310000000e-01,  2.890000000e-01,  1.500000000e-01, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.100000000e-01,  2.440000000e-01,  2.980000000e-01, /*DIAM*/  7.052942422e-01,  6.578016451e-01,  6.608010662e-01,  6.689549488e-01, /*EDIAM*/  4.122057722e-02,  5.830602992e-02,  5.894653790e-02,  6.748132796e-02, /*DMEAN*/  6.804263447e-01, /*EDMEAN*/  2.623452368e-02 },
        {189, /* SP */ 172, /*MAG*/  3.640000000e+00,  2.730000000e+00,  1.890000000e+00,  1.081000000e+00,  5.840000000e-01,  5.790000000e-01, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.300000000e-01,  1.520000000e-01,  2.080000000e-01, /*DIAM*/  5.248860063e-01,  5.714825225e-01,  5.814322905e-01,  5.574935151e-01, /*EDIAM*/  4.121588091e-02,  6.380995116e-02,  3.678400978e-02,  4.712066766e-02, /*DMEAN*/  5.580101868e-01, /*EDMEAN*/  2.172866824e-02 },
        {190, /* SP */ 140, /*MAG*/  3.460000000e+00,  2.810000000e+00,  2.110000000e+00,  1.495000000e+00,  1.187000000e+00,  1.278000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.980000000e-01,  1.900000000e-01,  2.120000000e-01, /*DIAM*/  3.922930409e-01,  4.185570598e-01,  4.254646016e-01,  3.775158081e-01, /*EDIAM*/  4.133861416e-02,  8.263449370e-02,  4.598110892e-02,  4.804426186e-02, /*DMEAN*/  4.003491468e-01, /*EDMEAN*/  2.424484347e-02 },
        {191, /* SP */ 176, /*MAG*/  4.060000000e+00,  3.070000000e+00,  2.130000000e+00,  1.375000000e+00,  8.370000000e-01,  8.830000000e-01, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.620000000e-01,  1.660000000e-01,  1.980000000e-01, /*DIAM*/  5.042410977e-01,  5.224115867e-01,  5.370097529e-01,  5.015068105e-01, /*EDIAM*/  4.121207570e-02,  7.264940585e-02,  4.015214051e-02,  4.486332293e-02, /*DMEAN*/  5.159828087e-01, /*EDMEAN*/  2.248837934e-02 },
        {192, /* SP */ 188, /*MAG*/  3.957000000e+00,  2.380000000e+00,  9.600000000e-01, -1.170000000e-01, -7.500000000e-01, -8.600000000e-01, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  1.660000000e-01,  1.920000000e-01,  2.200000000e-01, /*DIAM*/  1.009494247e+00,  9.009980773e-01,  9.011434822e-01,  8.909257550e-01, /*EDIAM*/  4.122057722e-02,  4.617522958e-02,  4.642415040e-02,  4.984963707e-02, /*DMEAN*/  9.333396913e-01, /*EDMEAN*/  2.221654260e-02 },
        {193, /* SP */ 228, /*MAG*/  9.650000000e+00,  8.090000000e+00,  5.610000000e+00,  5.252000000e+00,  4.476000000e+00,  4.018000000e+00, /*EMAG*/  4.472136000e-02,  3.999999911e-02,  1.711724000e-01,  2.640000000e-01,  2.000000000e-01,  3.999999911e-02, /*DIAM*/  6.715363606e-02, -1.054058937e-01, -6.128847186e-02, -1.117355064e-03, /*EDIAM*/  4.313407541e-02,  7.322558914e-02,  4.838306021e-02,  9.525582923e-03, /*DMEAN*/ -5.800551036e-04, /*EDMEAN*/  8.887988734e-03 },
        {194, /* SP */ 192, /*MAG*/  6.708000000e+00,  5.790000000e+00,  4.730000000e+00,  4.152000000e+00,  3.657000000e+00,  3.481000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.640000000e-01,  2.440000000e-01,  2.080000000e-01, /*DIAM*/ -7.572715730e-02, -1.273841694e-02, -1.737091149e-02,  3.188645875e-03, /*EDIAM*/  4.122737356e-02,  7.322238145e-02,  5.895220348e-02,  4.714432536e-02, /*DMEAN*/ -3.376456724e-02, /*EDMEAN*/  2.522877332e-02 },
        {195, /* SP */ 212, /*MAG*/  7.926000000e+00,  6.600000000e+00,  5.310000000e+00,  3.894000000e+00,  3.298000000e+00,  2.962000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.900000000e-01,  2.600000000e-01,  2.880000000e-01, /*DIAM*/  8.746045695e-02,  1.464605568e-01,  1.297187181e-01,  1.707678557e-01, /*EDIAM*/  4.124302509e-02,  8.041559913e-02,  6.281798601e-02,  6.523134331e-02, /*DMEAN*/  1.193418817e-01, /*EDMEAN*/  2.800554938e-02 },
        {196, /* SP */ 228, /*MAG*/  8.992000000e+00,  7.490000000e+00,  5.420000000e+00,  4.320000000e+00,  3.722000000e+00,  3.501000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  4.049057000e-02,  2.820000000e-01,  2.240000000e-01,  3.520000000e-01, /*DIAM*/  1.511936373e-01,  1.064869666e-01,  9.586328145e-02,  1.001016245e-01, /*EDIAM*/  4.131329595e-02,  7.819791063e-02,  5.415861631e-02,  7.970494738e-02, /*DMEAN*/  1.238812517e-01, /*EDMEAN*/  2.778764978e-02 },
        {197, /* SP */ 228, /*MAG*/  8.992000000e+00,  7.490000000e+00,  5.420000000e+00,  4.320000000e+00,  3.722000000e+00,  3.501000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  4.049057000e-02,  2.820000000e-01,  2.240000000e-01,  3.520000000e-01, /*DIAM*/  1.511936373e-01,  1.064869666e-01,  9.586328145e-02,  1.001016245e-01, /*EDIAM*/  4.131329595e-02,  7.819791063e-02,  5.415861631e-02,  7.970494738e-02, /*DMEAN*/  1.238812517e-01, /*EDMEAN*/  2.778764978e-02 },
        {198, /* SP */ 228, /*MAG*/  9.650000000e+00,  8.090000000e+00,  5.610000000e+00,  5.252000000e+00,  4.476000000e+00,  4.018000000e+00, /*EMAG*/  4.472136000e-02,  3.999999911e-02,  1.711724000e-01,  2.640000000e-01,  2.000000000e-01,  3.999999911e-02, /*DIAM*/  6.715363606e-02, -1.054058937e-01, -6.128847186e-02, -1.117355064e-03, /*EDIAM*/  4.313407541e-02,  7.322558914e-02,  4.838306021e-02,  9.525582923e-03, /*DMEAN*/ -5.800551036e-04, /*EDMEAN*/  8.887988734e-03 },
        {199, /* SP */ 192, /*MAG*/  6.708000000e+00,  5.790000000e+00,  4.730000000e+00,  4.152000000e+00,  3.657000000e+00,  3.481000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.640000000e-01,  2.440000000e-01,  2.080000000e-01, /*DIAM*/ -7.572715730e-02, -1.273841694e-02, -1.737091149e-02,  3.188645875e-03, /*EDIAM*/  4.122737356e-02,  7.322238145e-02,  5.895220348e-02,  4.714432536e-02, /*DMEAN*/ -3.376456724e-02, /*EDMEAN*/  2.522877332e-02 },
        {200, /* SP */ 226, /*MAG*/  9.444000000e+00,  7.970000000e+00,  5.890000000e+00,  4.999000000e+00,  4.149000000e+00,  4.039000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  3.000000000e-01,  2.120000000e-01,  2.600000000e-01, /*DIAM*/  1.671156343e-02, -4.500255488e-02,  8.132203710e-03, -1.272304714e-02, /*EDIAM*/  4.128610059e-02,  8.316640980e-02,  5.126103841e-02,  5.890094761e-02, /*DMEAN*/  1.949496245e-03, /*EDMEAN*/  2.623702547e-02 },
        {201, /* SP */ 212, /*MAG*/  7.926000000e+00,  6.600000000e+00,  5.310000000e+00,  3.894000000e+00,  3.298000000e+00,  2.962000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.900000000e-01,  2.600000000e-01,  2.880000000e-01, /*DIAM*/  8.746045695e-02,  1.464605568e-01,  1.297187181e-01,  1.707678557e-01, /*EDIAM*/  4.124302509e-02,  8.041559913e-02,  6.281798601e-02,  6.523134331e-02, /*DMEAN*/  1.193418817e-01, /*EDMEAN*/  2.800554938e-02 },
        {202, /* SP */ 228, /*MAG*/  8.992000000e+00,  7.490000000e+00,  5.420000000e+00,  4.320000000e+00,  3.722000000e+00,  3.501000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  4.049057000e-02,  2.820000000e-01,  2.240000000e-01,  3.520000000e-01, /*DIAM*/  1.511936373e-01,  1.064869666e-01,  9.586328145e-02,  1.001016245e-01, /*EDIAM*/  4.131329595e-02,  7.819791063e-02,  5.415861631e-02,  7.970494738e-02, /*DMEAN*/  1.238812517e-01, /*EDMEAN*/  2.778764978e-02 },
        {203, /* SP */ 228, /*MAG*/  8.992000000e+00,  7.490000000e+00,  5.420000000e+00,  4.320000000e+00,  3.722000000e+00,  3.501000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  4.049057000e-02,  2.820000000e-01,  2.240000000e-01,  3.520000000e-01, /*DIAM*/  1.511936373e-01,  1.064869666e-01,  9.586328145e-02,  1.001016245e-01, /*EDIAM*/  4.131329595e-02,  7.819791063e-02,  5.415861631e-02,  7.970494738e-02, /*DMEAN*/  1.238812517e-01, /*EDMEAN*/  2.778764978e-02 },
        {204, /* SP */ 232, /*MAG*/  4.900000000e+00,  3.310000000e+00,  6.100000000e-01, -7.270000000e-01, -1.515000000e+00, -1.724000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  1.402284000e-01,  1.680000000e-01,  1.640000000e-01,  1.500000000e-01, /*DIAM*/  1.088388007e+00,  1.182449392e+00,  1.196395220e+00,  1.180125877e+00, /*EDIAM*/  4.140373314e-02,  4.678678583e-02,  3.977576384e-02,  3.411363330e-02, /*DMEAN*/  1.162089377e+00, /*EDMEAN*/  1.941511087e-02 },
        {205, /* SP */ 180, /*MAG*/  3.071000000e+00,  2.060000000e+00,  1.050000000e+00,  4.030000000e-01, -1.990000000e-01, -2.740000000e-01, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  1.840000000e-01,  1.940000000e-01,  2.340000000e-01, /*DIAM*/  7.185156754e-01,  7.201265095e-01,  7.481781254e-01,  7.408879210e-01, /*EDIAM*/  4.121196542e-02,  5.111958352e-02,  4.689447748e-02,  5.300356349e-02, /*DMEAN*/  7.306263046e-01, /*EDMEAN*/  2.311518235e-02 },
        {206, /* SP */ 64, /*MAG*/ -1.431000000e+00, -1.440000000e+00, -1.420000000e+00, -1.391000000e+00, -1.391000000e+00, -1.390000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  1.090000000e-01,  1.840000000e-01,  2.140000000e-01, /*DIAM*/  7.976485391e-01,  7.895688542e-01,  7.926104850e-01,  7.847986050e-01, /*EDIAM*/  4.156156435e-02,  3.112209404e-02,  4.482744732e-02,  4.871344905e-02, /*DMEAN*/  7.913889618e-01, /*EDMEAN*/  1.930752922e-02 },
        {207, /* SP */ 192, /*MAG*/  4.538000000e+00,  3.270000000e+00,  2.040000000e+00,  9.860000000e-01,  4.030000000e-01,  4.670000000e-01, /*EMAG*/  4.011234000e-02,  3.999999911e-02,  3.999999911e-02,  2.400000000e-01,  1.720000000e-01,  1.900000000e-01, /*DIAM*/  6.452728534e-01,  6.700651647e-01,  6.637030286e-01,  6.189696769e-01, /*EDIAM*/  4.126930928e-02,  6.659458382e-02,  4.161945968e-02,  4.307876786e-02, /*DMEAN*/  6.464843288e-01, /*EDMEAN*/  2.228668460e-02 },
        {208, /* SP */ 180, /*MAG*/  3.410000000e+00,  2.240000000e+00,  1.110000000e+00,  3.110000000e-01, -2.100000000e-01, -3.030000000e-01, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  1.980000000e-01,  1.600000000e-01,  1.700000000e-01, /*DIAM*/  7.810956764e-01,  7.594122244e-01,  7.582559465e-01,  7.521798920e-01, /*EDIAM*/  4.121196542e-02,  5.498112280e-02,  3.871183635e-02,  3.854235465e-02, /*DMEAN*/  7.628889880e-01, /*EDMEAN*/  2.054840746e-02 },
        {209, /* SP */ 220, /*MAG*/  3.646000000e+00,  2.070000000e+00,  3.300000000e-01, -9.570000000e-01, -1.674000000e+00, -1.846000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.060000000e-01,  1.480000000e-01,  1.600000000e-01, /*DIAM*/  1.204764089e+00,  1.147798187e+00,  1.156974353e+00,  1.153131106e+00, /*EDIAM*/  4.124821477e-02,  5.720936880e-02,  3.586084689e-02,  3.630933919e-02, /*DMEAN*/  1.166809703e+00, /*EDMEAN*/  1.982906048e-02 },
        {210, /* SP */ 192, /*MAG*/  3.615000000e+00,  2.100000000e+00,  7.300000000e-01, -1.800000000e-02, -8.340000000e-01, -8.610000000e-01, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.180000000e-01,  1.700000000e-01,  1.820000000e-01, /*DIAM*/  1.032412859e+00,  8.581187389e-01,  9.138664565e-01,  8.887215058e-01, /*EDIAM*/  4.122737356e-02,  6.052200352e-02,  4.113843897e-02,  4.127232403e-02, /*DMEAN*/  9.346657252e-01, /*EDMEAN*/  2.165618777e-02 },
        {211, /* SP */ 188, /*MAG*/  3.161000000e+00,  2.010000000e+00,  8.800000000e-01, -1.960000000e-01, -7.490000000e-01, -7.830000000e-01, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.180000000e-01,  1.560000000e-01,  1.760000000e-01, /*DIAM*/  8.193742439e-01,  8.944534343e-01,  8.856415365e-01,  8.637797692e-01, /*EDIAM*/  4.122057722e-02,  6.051369180e-02,  3.776350066e-02,  3.991055356e-02, /*DMEAN*/  8.621116294e-01, /*EDMEAN*/  2.088674756e-02 },
        {212, /* SP */ 192, /*MAG*/  5.460000000e+00,  3.770000000e+00,  2.130000000e+00,  1.074000000e+00,  1.620000000e-01,  1.620000000e-01, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.240000000e-01,  2.000000000e-01,  1.700000000e-01, /*DIAM*/  8.069128559e-01,  6.841008792e-01,  7.424656757e-01,  7.011229628e-01, /*EDIAM*/  4.122737356e-02,  6.217783773e-02,  4.835682415e-02,  3.856332214e-02, /*DMEAN*/  7.407186971e-01, /*EDMEAN*/  2.218668977e-02 },
        {213, /* SP */ 200, /*MAG*/  6.114000000e+00,  4.560000000e+00,  2.890000000e+00,  1.642000000e+00,  8.000000000e-01,  7.670000000e-01, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.720000000e-01,  1.700000000e-01,  1.800000000e-01, /*DIAM*/  5.834426895e-01,  5.989279945e-01,  6.304717361e-01,  5.956832569e-01, /*EDIAM*/  4.123976885e-02,  7.544383589e-02,  4.115293056e-02,  4.083193125e-02, /*DMEAN*/  6.025896488e-01, /*EDMEAN*/  2.216455910e-02 },
        {214, /* SP */ 236, /*MAG*/  4.848000000e+00,  3.320000000e+00,  5.600000000e-01, -7.790000000e-01, -1.675000000e+00, -1.904000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  6.067125000e-02,  1.820000000e-01,  1.580000000e-01,  1.520000000e-01, /*DIAM*/  1.100779971e+00,  1.196385014e+00,  1.245629920e+00,  1.228885646e+00, /*EDIAM*/  4.156379099e-02,  5.073369883e-02,  3.843683177e-02,  3.464882788e-02, /*DMEAN*/  1.196550783e+00, /*EDMEAN*/  1.966125840e-02 },
        {215, /* SP */ 120, /*MAG*/  2.271000000e+00,  1.790000000e+00,  1.160000000e+00,  8.300000000e-01,  4.130000000e-01,  5.200000000e-01, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  4.019950000e-02,  1.840000000e-01,  1.620000000e-01,  1.640000000e-01, /*DIAM*/  5.155310634e-01,  5.023688449e-01,  5.620735163e-01,  5.098462848e-01, /*EDIAM*/  4.139308516e-02,  5.119623122e-02,  3.928372230e-02,  3.723091686e-02, /*DMEAN*/  5.243397714e-01, /*EDMEAN*/  2.023142224e-02 },
        {216, /* SP */ 224, /*MAG*/  4.558000000e+00,  2.970000000e+00,  1.190000000e+00,  1.030000000e-01, -7.620000000e-01, -9.530000000e-01, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  1.620000000e-01,  1.940000000e-01,  2.260000000e-01, /*DIAM*/  1.067665570e+00,  9.255471849e-01,  9.823077973e-01,  9.818232863e-01, /*EDIAM*/  4.126750894e-02,  4.507774670e-02,  4.692376402e-02,  5.121416884e-02, /*DMEAN*/  9.946445884e-01, /*EDMEAN*/  2.226442250e-02 },
        {217, /* SP */ 200, /*MAG*/  2.408000000e+00,  8.700000000e-01, -8.000000000e-01, -2.095000000e+00, -2.775000000e+00, -3.044000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  1.940000000e-01,  1.700000000e-01,  1.400000000e-01, /*DIAM*/  1.311522700e+00,  1.349936934e+00,  1.340728556e+00,  1.361062830e+00, /*EDIAM*/  4.123976885e-02,  5.391790149e-02,  4.115293056e-02,  3.180987386e-02, /*DMEAN*/  1.342580427e+00, /*EDMEAN*/  1.951298048e-02 },
        {218, /* SP */ 192, /*MAG*/  4.180000000e+00,  2.690000000e+00,  1.230000000e+00, -3.100000000e-02, -6.760000000e-01, -8.440000000e-01, /*EMAG*/  4.210701000e-02,  3.999999911e-02,  3.999999911e-02,  1.880000000e-01,  1.520000000e-01,  1.580000000e-01, /*DIAM*/  8.989128572e-01,  9.070205253e-01,  9.000843551e-01,  9.003784403e-01, /*EDIAM*/  4.202615457e-02,  5.224747928e-02,  3.681098361e-02,  3.585527736e-02, /*DMEAN*/  9.009339728e-01, /*EDMEAN*/  1.975112914e-02 },
        {219, /* SP */ 188, /*MAG*/  5.839000000e+00,  4.470000000e+00,  3.150000000e+00,  2.158000000e+00,  1.504000000e+00,  1.342000000e+00, /*EMAG*/  6.606815000e-02,  3.999999911e-02,  3.999999911e-02,  2.760000000e-01,  2.200000000e-01,  2.840000000e-01, /*DIAM*/  4.625342385e-01,  4.317927131e-01,  4.435792731e-01,  4.475826827e-01, /*EDIAM*/  5.255475393e-02,  7.653068710e-02,  5.316566447e-02,  6.431594067e-02, /*DMEAN*/  4.488134716e-01, /*EDMEAN*/  2.936163115e-02 },
        {220, /* SP */ 92, /*MAG*/  3.567000000e+00,  3.030000000e+00,  2.420000000e+00,  1.880000000e+00,  1.702000000e+00,  1.533000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.980000000e-01,  1.780000000e-01,  2.140000000e-01, /*DIAM*/  2.964760453e-01,  2.770353183e-01,  2.801238279e-01,  2.889855862e-01, /*EDIAM*/  4.142845504e-02,  8.270207072e-02,  4.321359662e-02,  4.859076893e-02, /*DMEAN*/  2.877868866e-01, /*EDMEAN*/  2.389615093e-02 },
        {221, /* SP */ 232, /*MAG*/  6.001000000e+00,  4.300000000e+00,  1.790000000e+00,  2.450000000e-01, -6.020000000e-01, -8.130000000e-01, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.200000000e-01,  1.740000000e-01,  1.680000000e-01, /*DIAM*/  9.592080055e-01,  9.894315317e-01,  1.016971093e+00,  1.000001786e+00, /*EDIAM*/  4.140373314e-02,  6.111213746e-02,  4.217611568e-02,  3.816827302e-02, /*DMEAN*/  9.913544718e-01, /*EDMEAN*/  2.136809310e-02 },
        {222, /* SP */ 232, /*MAG*/  4.491000000e+00,  2.870000000e+00,  5.700000000e-01, -9.120000000e-01, -1.608000000e+00, -1.862000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  5.035871000e-02,  1.880000000e-01,  1.900000000e-01,  1.800000000e-01, /*DIAM*/  1.195608009e+00,  1.199869035e+00,  1.200683158e+00,  1.199790111e+00, /*EDIAM*/  4.140373314e-02,  5.229214929e-02,  4.601931072e-02,  4.087339370e-02, /*DMEAN*/  1.198756934e+00, /*EDMEAN*/  2.175318215e-02 },
        {223, /* SP */ 172, /*MAG*/  4.437000000e+00,  3.060000000e+00,  1.840000000e+00,  8.860000000e-01,  2.320000000e-01,  1.250000000e-01, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.220000000e-01,  1.540000000e-01,  1.720000000e-01, /*DIAM*/  7.484260096e-01,  6.507950237e-01,  6.799614749e-01,  6.688949766e-01, /*EDIAM*/  4.121588091e-02,  6.160103013e-02,  3.726501435e-02,  3.898617076e-02, /*DMEAN*/  6.921446152e-01, /*EDMEAN*/  2.070239555e-02 },
        {224, /* SP */ 64, /*MAG*/ -1.431000000e+00, -1.440000000e+00, -1.420000000e+00, -1.391000000e+00, -1.391000000e+00, -1.390000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  1.090000000e-01,  1.840000000e-01,  2.140000000e-01, /*DIAM*/  7.976485391e-01,  7.895688542e-01,  7.926104850e-01,  7.847986050e-01, /*EDIAM*/  4.156156435e-02,  3.112209404e-02,  4.482744732e-02,  4.871344905e-02, /*DMEAN*/  7.913889618e-01, /*EDMEAN*/  1.930752922e-02 },
        {225, /* SP */ 196, /*MAG*/  5.011000000e+00,  3.530000000e+00,  2.060000000e+00,  1.191000000e+00,  2.670000000e-01,  1.900000000e-01, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.500000000e-01,  1.600000000e-01,  2.020000000e-01, /*DIAM*/  7.333001895e-01,  6.391140015e-01,  7.116743346e-01,  6.936886640e-01, /*EDIAM*/  4.123418936e-02,  6.936281577e-02,  3.874230774e-02,  4.579464982e-02, /*DMEAN*/  7.062452775e-01, /*EDMEAN*/  2.222353274e-02 },
        {226, /* SP */ 232, /*MAG*/  6.282000000e+00,  4.740000000e+00,  2.590000000e+00,  1.381000000e+00,  3.890000000e-01,  3.050000000e-01, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.460000000e-01,  1.720000000e-01,  2.300000000e-01, /*DIAM*/  7.726280027e-01,  7.087886703e-01,  7.960450194e-01,  7.585857244e-01, /*EDIAM*/  4.140373314e-02,  6.828546040e-02,  4.169593359e-02,  5.215615014e-02, /*DMEAN*/  7.694143771e-01, /*EDMEAN*/  2.347353073e-02 },
        {227, /* SP */ 208, /*MAG*/  4.690000000e+00,  3.140000000e+00,  1.490000000e+00,  1.200000000e-01, -5.780000000e-01, -6.560000000e-01, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.200000000e-01,  1.700000000e-01,  1.720000000e-01, /*DIAM*/  8.967572525e-01,  9.211325408e-01,  9.156301924e-01,  8.921898173e-01, /*EDIAM*/  4.124396433e-02,  6.109164872e-02,  4.115794873e-02,  3.902886317e-02, /*DMEAN*/  9.037331893e-01, /*EDMEAN*/  2.133520844e-02 },
        {228, /* SP */ 192, /*MAG*/  3.430000000e+00,  1.990000000e+00,  6.000000000e-01, -2.560000000e-01, -9.900000000e-01, -1.127000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  1.470000000e-01,  1.920000000e-01,  2.080000000e-01, /*DIAM*/  1.007912859e+00,  9.155473112e-01,  9.469637332e-01,  9.460207767e-01, /*EDIAM*/  4.122737356e-02,  4.095798103e-02,  4.643134399e-02,  4.714432536e-02, /*DMEAN*/  9.556823490e-01, /*EDMEAN*/  2.123585910e-02 },
        {229, /* SP */ 144, /*MAG*/  3.778000000e+00,  2.970000000e+00,  2.160000000e+00,  1.511000000e+00,  1.107000000e+00,  1.223000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.640000000e-01,  1.820000000e-01,  2.280000000e-01, /*DIAM*/  4.519676188e-01,  4.313133444e-01,  4.527532575e-01,  3.966735460e-01, /*EDIAM*/  4.132123941e-02,  7.323790269e-02,  4.404950524e-02,  5.165577578e-02, /*DMEAN*/  4.376716919e-01, /*EDMEAN*/  2.403569640e-02 },
        {230, /* SP */ 220, /*MAG*/  4.663000000e+00,  3.060000000e+00,  1.290000000e+00, -1.080000000e-01, -9.060000000e-01, -1.009000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  1.920000000e-01,  1.560000000e-01,  1.700000000e-01, /*DIAM*/  1.023504086e+00,  9.888249699e-01,  1.012530771e+00,  9.897515417e-01, /*EDIAM*/  4.124821477e-02,  5.334828133e-02,  3.778281129e-02,  3.856595508e-02, /*DMEAN*/  1.005390650e+00, /*EDMEAN*/  2.031383255e-02 },
        {231, /* SP */ 184, /*MAG*/  4.144000000e+00,  3.000000000e+00,  1.910000000e+00,  1.030000000e+00,  3.760000000e-01,  4.290000000e-01, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  1.800000000e-01,  1.700000000e-01,  1.980000000e-01, /*DIAM*/  6.139895601e-01,  6.249802365e-01,  6.514677176e-01,  6.108957577e-01, /*EDIAM*/  4.121507289e-02,  5.002351234e-02,  4.112310640e-02,  4.487293174e-02, /*DMEAN*/  6.258732627e-01, /*EDMEAN*/  2.141081688e-02 },
        {232, /* SP */ 192, /*MAG*/  4.890000000e+00,  3.490000000e+00,  2.120000000e+00,  1.073000000e+00,  1.830000000e-01,  2.820000000e-01, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  1.900000000e-01,  1.760000000e-01,  1.820000000e-01, /*DIAM*/  6.831128540e-01,  6.628776646e-01,  7.258508895e-01,  6.666120134e-01, /*EDIAM*/  4.122737356e-02,  5.279883048e-02,  4.258160084e-02,  4.127232403e-02, /*DMEAN*/  6.861947918e-01, /*EDMEAN*/  2.138668426e-02 },
        {233, /* SP */ 220, /*MAG*/  5.433000000e+00,  3.820000000e+00,  2.030000000e+00,  8.950000000e-01, -4.900000000e-02, -1.140000000e-01, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.120000000e-01,  1.960000000e-01,  1.740000000e-01, /*DIAM*/  8.777040837e-01,  7.695660381e-01,  8.371299905e-01,  8.072040937e-01, /*EDIAM*/  4.124821477e-02,  5.886468029e-02,  4.740238146e-02,  3.946879886e-02, /*DMEAN*/  8.300623899e-01, /*EDMEAN*/  2.208266016e-02 },
        {234, /* SP */ 224, /*MAG*/  5.541000000e+00,  4.040000000e+00,  2.250000000e+00,  1.176000000e+00,  3.250000000e-01,  1.570000000e-01, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.240000000e-01,  2.140000000e-01,  2.600000000e-01, /*DIAM*/  7.997255660e-01,  7.107168245e-01,  7.642066267e-01,  7.587721881e-01, /*EDIAM*/  4.126750894e-02,  6.217267618e-02,  5.173727108e-02,  5.889774034e-02, /*DMEAN*/  7.679092292e-01, /*EDMEAN*/  2.520772210e-02 },
        {235, /* SP */ 232, /*MAG*/  4.961000000e+00,  3.390000000e+00,  1.150000000e+00, -1.130000000e-01, -1.010000000e+00, -1.189000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  1.530000000e-01,  1.740000000e-01,  1.940000000e-01, /*DIAM*/  1.060608007e+00,  1.018645818e+00,  1.077866035e+00,  1.061169670e+00, /*EDIAM*/  4.140373314e-02,  4.266291821e-02,  4.217611568e-02,  4.403095609e-02, /*DMEAN*/  1.054546697e+00, /*EDMEAN*/  2.072595409e-02 },
        {236, /* SP */ 172, /*MAG*/  3.784000000e+00,  2.850000000e+00,  2.020000000e+00,  1.248000000e+00,  7.330000000e-01,  6.640000000e-01, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.760000000e-01,  2.440000000e-01,  2.780000000e-01, /*DIAM*/  5.157660061e-01,  5.344735934e-01,  5.504361811e-01,  5.414132229e-01, /*EDIAM*/  4.121588091e-02,  7.651726541e-02,  5.893536422e-02,  6.294625154e-02, /*DMEAN*/  5.307954676e-01, /*EDMEAN*/  2.722721514e-02 },
        {237, /* SP */ 140, /*MAG*/  3.260000000e+00,  2.680000000e+00,  2.030000000e+00,  1.793000000e+00,  1.534000000e+00,  1.485000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  3.060000000e-01,  1.540000000e-01,  1.700000000e-01, /*DIAM*/  3.748930407e-01,  3.260927728e-01,  3.363906703e-01,  3.272603329e-01, /*EDIAM*/  4.133861416e-02,  8.484480339e-02,  3.732992507e-02,  3.855872229e-02, /*DMEAN*/  3.438984320e-01, /*EDMEAN*/  2.131837350e-02 },
        {238, /* SP */ 196, /*MAG*/  3.535000000e+00,  2.070000000e+00,  6.100000000e-01, -4.950000000e-01, -1.225000000e+00, -1.287000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  1.940000000e-01,  1.840000000e-01,  2.040000000e-01, /*DIAM*/  1.015380194e+00,  9.936675782e-01,  1.011394183e+00,  9.895353837e-01, /*EDIAM*/  4.123418936e-02,  5.391067583e-02,  4.451358359e-02,  4.624635692e-02, /*DMEAN*/  1.004215105e+00, /*EDMEAN*/  2.238375638e-02 },
        {239, /* SP */ 240, /*MAG*/  6.220000000e+00,  4.630000000e+00,  1.780000000e+00,  1.090000000e-01, -7.740000000e-01, -9.570000000e-01, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.020000000e-01,  1.820000000e-01,  2.000000000e-01, /*DIAM*/  9.366725243e-01,  1.048613219e+00,  1.093230707e+00,  1.056974249e+00, /*EDIAM*/  4.182650566e-02,  5.639317803e-02,  4.433998670e-02,  4.556437690e-02, /*DMEAN*/  1.027354650e+00, /*EDMEAN*/  2.262488780e-02 },
        {240, /* SP */ 172, /*MAG*/  4.446000000e+00,  3.490000000e+00,  2.600000000e+00,  1.795000000e+00,  1.268000000e+00,  1.223000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  3.140000000e-01,  1.180000000e-01,  1.660000000e-01, /*DIAM*/  4.014060044e-01,  4.322146633e-01,  4.477669189e-01,  4.317416884e-01, /*EDIAM*/  4.121588091e-02,  8.702008678e-02,  2.861458696e-02,  3.763090451e-02, /*DMEAN*/  4.319252429e-01, /*EDMEAN*/  1.900005920e-02 },
        {241, /* SP */ 172, /*MAG*/  4.421000000e+00,  3.460000000e+00,  2.500000000e+00,  1.659000000e+00,  9.850000000e-01,  1.223000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.380000000e-01,  1.920000000e-01,  1.960000000e-01, /*DIAM*/  4.105060046e-01,  4.675539495e-01,  5.148019393e-01,  4.309533673e-01, /*EDIAM*/  4.121588091e-02,  6.601923161e-02,  4.640996193e-02,  4.440870073e-02, /*DMEAN*/  4.503248694e-01, /*EDMEAN*/  2.314559663e-02 },
        {242, /* SP */ 188, /*MAG*/  3.797000000e+00,  2.630000000e+00,  1.540000000e+00,  8.310000000e-01,  2.890000000e-01,  1.500000000e-01, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.100000000e-01,  2.440000000e-01,  2.980000000e-01, /*DIAM*/  7.052942422e-01,  6.578016451e-01,  6.608010662e-01,  6.689549488e-01, /*EDIAM*/  4.122057722e-02,  5.830602992e-02,  5.894653790e-02,  6.748132796e-02, /*DMEAN*/  6.804263447e-01, /*EDMEAN*/  2.623452368e-02 },
        {243, /* SP */ 222, /*MAG*/  4.314000000e+00,  2.730000000e+00,  9.100000000e-01, -1.310000000e-01, -1.025000000e+00, -1.173000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  1.660000000e-01,  1.800000000e-01,  2.140000000e-01, /*DIAM*/  1.094806029e+00,  9.709817598e-01,  1.031663416e+00,  1.021713262e+00, /*EDIAM*/  4.125545403e-02,  4.618023288e-02,  4.355319174e-02,  4.850163712e-02, /*DMEAN*/  1.034567669e+00, /*EDMEAN*/  2.174621206e-02 },
        {244, /* SP */ 172, /*MAG*/  3.640000000e+00,  2.730000000e+00,  1.890000000e+00,  1.081000000e+00,  5.840000000e-01,  5.790000000e-01, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.300000000e-01,  1.520000000e-01,  2.080000000e-01, /*DIAM*/  5.248860063e-01,  5.714825225e-01,  5.814322905e-01,  5.574935151e-01, /*EDIAM*/  4.121588091e-02,  6.380995116e-02,  3.678400978e-02,  4.712066766e-02, /*DMEAN*/  5.580101868e-01, /*EDMEAN*/  2.172866824e-02 },
        {245, /* SP */ 226, /*MAG*/  2.925000000e+00,  1.060000000e+00, -1.840000000e+00, -2.850000000e+00, -3.725000000e+00, -4.100000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  9.300000000e-02,  1.460000000e-01,  1.600000000e-01, /*DIAM*/  1.641131588e+00,  1.596899255e+00,  1.622692539e+00,  1.647371868e+00, /*EDIAM*/  4.128610059e-02,  2.615960185e-02,  3.539156707e-02,  3.631612330e-02, /*DMEAN*/  1.620665351e+00, /*EDMEAN*/  1.609651798e-02 },
        {246, /* SP */ 140, /*MAG*/  3.460000000e+00,  2.810000000e+00,  2.110000000e+00,  1.495000000e+00,  1.187000000e+00,  1.278000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.980000000e-01,  1.900000000e-01,  2.120000000e-01, /*DIAM*/  3.922930409e-01,  4.185570598e-01,  4.254646016e-01,  3.775158081e-01, /*EDIAM*/  4.133861416e-02,  8.263449370e-02,  4.598110892e-02,  4.804426186e-02, /*DMEAN*/  4.003491468e-01, /*EDMEAN*/  2.424484347e-02 },
        {247, /* SP */ 170, /*MAG*/  4.396000000e+00,  3.480000000e+00,  2.590000000e+00,  1.768000000e+00,  1.366000000e+00,  1.333000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  3.040000000e-01,  1.920000000e-01,  2.200000000e-01, /*DIAM*/  3.802127545e-01,  4.358485630e-01,  4.225226513e-01,  4.047449260e-01, /*EDIAM*/  4.121922010e-02,  8.425632516e-02,  4.641071796e-02,  4.983235186e-02, /*DMEAN*/  4.034883951e-01, /*EDMEAN*/  2.455148321e-02 },
        {248, /* SP */ 192, /*MAG*/  4.597000000e+00,  3.160000000e+00,  1.850000000e+00,  7.240000000e-01, -1.120000000e-01, -2.300000000e-02, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  1.780000000e-01,  1.780000000e-01,  1.600000000e-01, /*DIAM*/  7.720528553e-01,  7.341365942e-01,  7.834073106e-01,  7.269550800e-01, /*EDIAM*/  4.122737356e-02,  4.949146678e-02,  4.306271908e-02,  3.630654260e-02, /*DMEAN*/  7.533914444e-01, /*EDMEAN*/  2.038396054e-02 },
        {249, /* SP */ 148, /*MAG*/  3.744000000e+00,  2.790000000e+00,  1.860000000e+00,  1.351000000e+00,  7.960000000e-01,  7.560000000e-01, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.880000000e-01,  1.520000000e-01,  2.220000000e-01, /*DIAM*/  5.720425966e-01,  4.668791789e-01,  5.217667230e-01,  5.002096752e-01, /*EDIAM*/  4.130274461e-02,  7.986118106e-02,  3.682994855e-02,  5.029596483e-02, /*DMEAN*/  5.288284438e-01, /*EDMEAN*/  2.261277708e-02 },
        {250, /* SP */ 160, /*MAG*/  4.170000000e+00,  3.420000000e+00,  2.710000000e+00,  1.867000000e+00,  1.559000000e+00,  1.511000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.180000000e-01,  1.840000000e-01,  1.980000000e-01, /*DIAM*/  3.009941478e-01,  3.889304004e-01,  3.684167769e-01,  3.544983671e-01, /*EDIAM*/  4.124903710e-02,  6.050940799e-02,  4.449694803e-02,  4.486201073e-02, /*DMEAN*/  3.459751346e-01, /*EDMEAN*/  2.266474836e-02 },
        {251, /* SP */ 184, /*MAG*/  5.210000000e+00,  3.860000000e+00,  2.690000000e+00,  1.729000000e+00,  1.056000000e+00,  9.990000000e-01, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.440000000e-01,  1.660000000e-01,  1.900000000e-01, /*DIAM*/  5.697095594e-01,  4.975427346e-01,  5.228918402e-01,  5.045161941e-01, /*EDIAM*/  4.121507289e-02,  6.768492963e-02,  4.016080347e-02,  4.306565578e-02, /*DMEAN*/  5.295759919e-01, /*EDMEAN*/  2.206989796e-02 },
        {252, /* SP */ 200, /*MAG*/  3.761000000e+00,  2.240000000e+00,  7.000000000e-01, -2.450000000e-01, -1.034000000e+00, -1.162000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.120000000e-01,  1.800000000e-01,  1.600000000e-01, /*DIAM*/  1.026982696e+00,  9.430797854e-01,  9.772266051e-01,  9.712088099e-01, /*EDIAM*/  4.123976885e-02,  5.888130766e-02,  4.355755898e-02,  3.631909640e-02, /*DMEAN*/  9.846287324e-01, /*EDMEAN*/  2.104265438e-02 },
        {253, /* SP */ 60, /*MAG*/  2.900000000e-02,  3.000000000e-02,  4.000000000e-02, -1.770000000e-01, -2.900000000e-02,  1.290000000e-01, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.060000000e-01,  1.460000000e-01,  1.860000000e-01, /*DIAM*/  4.835040149e-01,  5.557847016e-01,  5.143860913e-01,  4.696410310e-01, /*EDIAM*/  4.158344239e-02,  5.754727116e-02,  3.579414408e-02,  4.243305745e-02, /*DMEAN*/  5.007332117e-01, /*EDMEAN*/  2.083208076e-02 },
        {254, /* SP */ 236, /*MAG*/  5.795000000e+00,  4.220000000e+00,  1.620000000e+00, -1.120000000e-01, -1.039000000e+00, -1.255000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.100000000e-01,  1.660000000e-01,  1.760000000e-01, /*DIAM*/  9.499199685e-01,  1.080876083e+00,  1.129318634e+00,  1.105681265e+00, /*EDIAM*/  4.156379099e-02,  5.843614616e-02,  4.035148573e-02,  4.004496314e-02, /*DMEAN*/  1.064497920e+00, /*EDMEAN*/  2.132189579e-02 },
        {255, /* SP */ 176, /*MAG*/  4.060000000e+00,  3.070000000e+00,  2.130000000e+00,  1.375000000e+00,  8.370000000e-01,  8.830000000e-01, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.620000000e-01,  1.660000000e-01,  1.980000000e-01, /*DIAM*/  5.042410977e-01,  5.224115867e-01,  5.370097529e-01,  5.015068105e-01, /*EDIAM*/  4.121207570e-02,  7.264940585e-02,  4.015214051e-02,  4.486332293e-02, /*DMEAN*/  5.159828087e-01, /*EDMEAN*/  2.248837934e-02 },
        {256, /* SP */ 220, /*MAG*/  5.942000000e+00,  4.440000000e+00,  2.760000000e+00,  1.714000000e+00,  8.780000000e-01,  7.110000000e-01, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.980000000e-01,  1.700000000e-01,  2.120000000e-01, /*DIAM*/  6.848840808e-01,  5.904856783e-01,  6.390677308e-01,  6.368172299e-01, /*EDIAM*/  4.124821477e-02,  8.261464553e-02,  4.114805030e-02,  4.805006684e-02, /*DMEAN*/  6.500610312e-01, /*EDMEAN*/  2.337720631e-02 },
        {257, /* SP */ 192, /*MAG*/  4.227000000e+00,  2.720000000e+00,  1.280000000e+00,  2.760000000e-01, -5.440000000e-01, -7.200000000e-01, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  1.680000000e-01,  2.240000000e-01,  2.440000000e-01, /*DIAM*/  9.034528573e-01,  8.243508813e-01,  8.694773508e-01,  8.731083669e-01, /*EDIAM*/  4.122737356e-02,  4.673685422e-02,  5.413517939e-02,  5.527872445e-02, /*DMEAN*/  8.699650664e-01, /*EDMEAN*/  2.353830738e-02 },
        {258, /* SP */ 132, /*MAG*/  2.903000000e+00,  2.230000000e+00,  1.580000000e+00,  1.369000000e+00,  8.520000000e-01,  8.270000000e-01, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  3.100000000e-01,  2.020000000e-01,  2.240000000e-01, /*DIAM*/  5.340369722e-01,  3.996702802e-01,  4.794834252e-01,  4.594452762e-01, /*EDIAM*/  4.136756490e-02,  8.595681518e-02,  4.887954081e-02,  5.076306536e-02, /*DMEAN*/  4.884378028e-01, /*EDMEAN*/  2.513312601e-02 },
        {259, /* SP */ 68, /*MAG*/  1.342000000e+00,  1.250000000e+00,  1.090000000e+00,  1.139000000e+00,  9.020000000e-01,  1.010000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.540000000e-01,  1.880000000e-01,  2.020000000e-01, /*DIAM*/  3.240088826e-01,  3.053715909e-01,  3.597488640e-01,  3.215523037e-01, /*EDIAM*/  4.153908537e-02,  7.069850149e-02,  4.576279082e-02,  4.600136834e-02, /*DMEAN*/  3.309230626e-01, /*EDMEAN*/  2.361431929e-02 },
        {260, /* SP */ 180, /*MAG*/  3.501000000e+00,  2.480000000e+00,  1.480000000e+00,  6.410000000e-01,  1.040000000e-01, -7.000000000e-03, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.180000000e-01,  1.600000000e-01,  2.040000000e-01, /*DIAM*/  6.407156743e-01,  6.865015090e-01,  6.924038055e-01,  6.915083582e-01, /*EDIAM*/  4.121196542e-02,  6.050059346e-02,  3.871183635e-02,  4.622327028e-02, /*DMEAN*/  6.756306648e-01, /*EDMEAN*/  2.186827014e-02 },
        {261, /* SP */ 198, /*MAG*/  5.329000000e+00,  3.720000000e+00,  2.090000000e+00,  9.820000000e-01,  2.700000000e-02, -3.800000000e-02, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.320000000e-01,  1.780000000e-01,  2.020000000e-01, /*DIAM*/  7.797174373e-01,  7.143677064e-01,  7.797996970e-01,  7.529826752e-01, /*EDIAM*/  4.123720484e-02,  6.439683562e-02,  4.307366331e-02,  4.579701510e-02, /*DMEAN*/  7.643527285e-01, /*EDMEAN*/  2.278318496e-02 },
        {262, /* SP */ 172, /*MAG*/  4.200000000e+00,  3.210000000e+00,  2.240000000e+00,  1.625000000e+00,  9.910000000e-01,  1.160000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  1.840000000e-01,  1.800000000e-01,  1.940000000e-01, /*DIAM*/  4.784860056e-01,  4.577682351e-01,  5.030431843e-01,  4.386394988e-01, /*EDIAM*/  4.121588091e-02,  5.111499529e-02,  4.352106700e-02,  4.395674659e-02, /*DMEAN*/  4.709086854e-01, /*EDMEAN*/  2.173464666e-02 },
        {263, /* SP */ 224, /*MAG*/  6.138000000e+00,  4.520000000e+00,  2.700000000e+00,  1.701000000e+00,  7.840000000e-01,  6.660000000e-01, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.920000000e-01,  1.660000000e-01,  1.820000000e-01, /*DIAM*/  7.762655657e-01,  6.022614658e-01,  6.732727733e-01,  6.562101428e-01, /*EDIAM*/  4.126750894e-02,  8.095396790e-02,  4.018944573e-02,  4.127617587e-02, /*DMEAN*/  6.945542605e-01, /*EDMEAN*/  2.221826843e-02 },
        {264, /* SP */ 188, /*MAG*/  3.957000000e+00,  2.380000000e+00,  9.600000000e-01, -1.170000000e-01, -7.500000000e-01, -8.600000000e-01, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  1.660000000e-01,  1.920000000e-01,  2.200000000e-01, /*DIAM*/  1.009494247e+00,  9.009980773e-01,  9.011434822e-01,  8.909257550e-01, /*EDIAM*/  4.122057722e-02,  4.617522958e-02,  4.642415040e-02,  4.984963707e-02, /*DMEAN*/  9.333396913e-01, /*EDMEAN*/  2.221654260e-02 },
        {265, /* SP */ 148, /*MAG*/  3.919000000e+00,  2.950000000e+00,  2.030000000e+00,  1.278000000e+00,  7.400000000e-01,  5.900000000e-01, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.800000000e-01,  2.160000000e-01,  2.140000000e-01, /*DIAM*/  5.493425963e-01,  4.993702509e-01,  5.418756727e-01,  5.419760992e-01, /*EDIAM*/  4.130274461e-02,  7.765108435e-02,  5.222215055e-02,  4.848807511e-02, /*DMEAN*/  5.401289599e-01, /*EDMEAN*/  2.497391622e-02 },
        {266, /* SP */ 186, /*MAG*/  4.948000000e+00,  3.390000000e+00,  1.810000000e+00,  1.231000000e+00,  4.250000000e-01,  3.430000000e-01, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.400000000e-01,  1.720000000e-01,  1.700000000e-01, /*DIAM*/  7.939206309e-01,  6.023808074e-01,  6.574869079e-01,  6.428947620e-01, /*EDIAM*/  4.121758890e-02,  6.658349217e-02,  4.160769697e-02,  3.855221882e-02, /*DMEAN*/  6.869777112e-01, /*EDMEAN*/  2.154286667e-02 },
        {267, /* SP */ 172, /*MAG*/  5.040000000e+00,  3.970000000e+00,  2.980000000e+00,  2.030000000e+00,  1.462000000e+00,  1.508000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.380000000e-01,  2.040000000e-01,  2.380000000e-01, /*DIAM*/  3.760860040e-01,  4.040271629e-01,  4.207630274e-01,  3.798657753e-01, /*EDIAM*/  4.121588091e-02,  6.601923161e-02,  4.929956626e-02,  5.390206578e-02, /*DMEAN*/  3.925747790e-01, /*EDMEAN*/  2.468300877e-02 },
        {268, /* SP */ 172, /*MAG*/  4.443000000e+00,  3.510000000e+00,  2.620000000e+00,  1.700000000e+00,  1.243000000e+00,  1.181000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.620000000e-01,  1.700000000e-01,  1.560000000e-01, /*DIAM*/  3.831460041e-01,  4.600450209e-01,  4.546229501e-01,  4.417708857e-01, /*EDIAM*/  4.121588091e-02,  7.264890158e-02,  4.111430435e-02,  3.537253612e-02, /*DMEAN*/  4.303562606e-01, /*EDMEAN*/  2.103212875e-02 },
        {269, /* SP */ 228, /*MAG*/  5.356000000e+00,  3.730000000e+00,  1.660000000e+00,  4.230000000e-01, -4.300000000e-01, -6.680000000e-01, /*EMAG*/  4.428318000e-02,  3.999999911e-02,  3.999999911e-02,  1.820000000e-01,  2.360000000e-01,  3.260000000e-01, /*DIAM*/  9.800736497e-01,  8.964066212e-01,  9.424313875e-01,  9.446490824e-01, /*EDIAM*/  4.295994481e-02,  5.059835885e-02,  5.704748360e-02,  7.382614381e-02, /*DMEAN*/  9.445549928e-01, /*EDMEAN*/  2.589794562e-02 },
        {270, /* SP */ 230, /*MAG*/  4.095000000e+00,  2.440000000e+00,  1.300000000e-01, -1.432000000e+00, -2.129000000e+00, -2.379000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  6.053098000e-02,  1.940000000e-01,  1.800000000e-01,  1.540000000e-01, /*DIAM*/  1.278622113e+00,  1.310928683e+00,  1.303784020e+00,  1.301667562e+00, /*EDIAM*/  4.135154739e-02,  5.392079872e-02,  4.359066989e-02,  3.498948922e-02, /*DMEAN*/  1.297360004e+00, /*EDMEAN*/  2.052092339e-02 },
        {271, /* SP */ 184, /*MAG*/  6.045000000e+00,  4.760000000e+00,  3.460000000e+00,  2.435000000e+00,  1.830000000e+00,  1.765000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.820000000e-01,  1.960000000e-01,  2.200000000e-01, /*DIAM*/  3.494095561e-01,  3.712391613e-01,  3.732887252e-01,  3.548373597e-01, /*EDIAM*/  4.121507289e-02,  7.818271531e-02,  4.738073031e-02,  4.984404196e-02, /*DMEAN*/  3.596075675e-01, /*EDMEAN*/  2.451174879e-02 },
        {272, /* SP */ 232, /*MAG*/  6.001000000e+00,  4.370000000e+00,  2.020000000e+00,  6.230000000e-01, -2.380000000e-01, -3.990000000e-01, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  1.640000000e-01,  1.900000000e-01,  2.180000000e-01, /*DIAM*/  9.018080047e-01,  8.901815302e-01,  9.320450215e-01,  9.081623689e-01, /*EDIAM*/  4.140373314e-02,  4.568659333e-02,  4.601931072e-02,  4.944699195e-02, /*DMEAN*/  9.073985467e-01, /*EDMEAN*/  2.211842201e-02 },
        {273, /* SP */ 196, /*MAG*/  6.744000000e+00,  5.720000000e+00,  4.490000000e+00,  3.663000000e+00,  3.085000000e+00,  3.048000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  8.015610000e-02,  2.580000000e-01,  1.960000000e-01,  2.240000000e-01, /*DIAM*/  1.196017871e-02,  1.230604224e-01,  1.221723803e-01,  1.045353706e-01, /*EDIAM*/  4.123418936e-02,  7.157195395e-02,  4.740092342e-02,  5.076425833e-02, /*DMEAN*/  7.736702525e-02, /*EDMEAN*/  2.437262862e-02 },
        {274, /* SP */ 200, /*MAG*/  5.746000000e+00,  4.690000000e+00,  3.540000000e+00,  2.894000000e+00,  2.349000000e+00,  2.237000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.920000000e-01,  2.140000000e-01,  2.400000000e-01, /*DIAM*/  2.486826845e-01,  2.623744181e-01,  2.621448823e-01,  2.664715731e-01, /*EDIAM*/  4.123976885e-02,  8.096874023e-02,  5.173874390e-02,  5.438311411e-02, /*DMEAN*/  2.576524902e-01, /*EDMEAN*/  2.575484486e-02 },
        {275, /* SP */ 72, /*MAG*/  2.230000000e+00,  2.140000000e+00,  2.040000000e+00,  1.854000000e+00,  1.925000000e+00,  1.883000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.740000000e-01,  1.940000000e-01,  1.920000000e-01, /*DIAM*/  1.565585764e-01,  1.843273289e-01,  1.581667400e-01,  1.556674817e-01, /*EDIAM*/  4.151649521e-02,  7.618533422e-02,  4.717485240e-02,  4.373626256e-02, /*DMEAN*/  1.595229189e-01, /*EDMEAN*/  2.366295973e-02 },
        {276, /* SP */ 76, /*MAG*/  1.315000000e+00,  1.170000000e+00,  1.010000000e+00,  1.037000000e+00,  9.370000000e-01,  9.450000000e-01, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.340000000e-01,  2.560000000e-01,  2.860000000e-01, /*DIAM*/  3.952827907e-01,  3.436178144e-01,  3.642054831e-01,  3.498999950e-01, /*EDIAM*/  4.149458958e-02,  6.514702869e-02,  6.202308459e-02,  6.489767592e-02, /*DMEAN*/  3.717682351e-01, /*EDMEAN*/  2.707546299e-02 },
        {277, /* SP */ 184, /*MAG*/  4.435000000e+00,  3.520000000e+00,  2.580000000e+00,  2.060000000e+00,  1.729000000e+00,  1.619000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  3.620000000e-01,  2.800000000e-01,  2.920000000e-01, /*DIAM*/  3.680095564e-01,  3.798195186e-01,  3.465105147e-01,  3.552899145e-01, /*EDIAM*/  4.121507289e-02,  1.002970646e-01,  6.761635153e-02,  6.612048624e-02, /*DMEAN*/  3.623951325e-01, /*EDMEAN*/  2.922535626e-02 },
        {278, /* SP */ 140, /*MAG*/  3.260000000e+00,  2.680000000e+00,  2.030000000e+00,  1.793000000e+00,  1.534000000e+00,  1.485000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  3.060000000e-01,  1.540000000e-01,  1.700000000e-01, /*DIAM*/  3.748930407e-01,  3.260927728e-01,  3.363906703e-01,  3.272603329e-01, /*EDIAM*/  4.133861416e-02,  8.484480339e-02,  3.732992507e-02,  3.855872229e-02, /*DMEAN*/  3.438984320e-01, /*EDMEAN*/  2.131837350e-02 },
        {279, /* SP */ 208, /*MAG*/  7.359000000e+00,  6.050000000e+00,  4.780000000e+00,  3.546000000e+00,  2.895000000e+00,  2.544000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.780000000e-01,  2.180000000e-01,  3.280000000e-01, /*DIAM*/  1.653372416e-01,  1.963111015e-01,  1.978091700e-01,  2.445693696e-01, /*EDIAM*/  4.124396433e-02,  7.710384111e-02,  5.270558712e-02,  7.427627555e-02, /*DMEAN*/  1.892295861e-01, /*EDMEAN*/  2.724023994e-02 },
        {280, /* SP */ 180, /*MAG*/  6.730000000e+00,  5.880000000e+00,  5.050000000e+00,  4.549000000e+00,  4.064000000e+00,  3.999000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.060000000e-01,  2.400000000e-01,  3.999999911e-02, /*DIAM*/ -1.453043374e-01, -1.341056461e-01, -1.226934829e-01, -1.256157415e-01, /*EDIAM*/  4.121196542e-02,  5.718853426e-02,  5.797396946e-02,  9.365552128e-03, /*DMEAN*/ -1.270261014e-01, /*EDMEAN*/  8.711809673e-03 },
        {281, /* SP */ 136, /*MAG*/  4.636000000e+00,  4.100000000e+00,  3.520000000e+00,  3.175000000e+00,  2.957000000e+00,  2.859000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.100000000e-01,  1.780000000e-01,  2.740000000e-01, /*DIAM*/  6.958391622e-02,  4.790669673e-02,  5.025831776e-02,  5.121699945e-02, /*EDIAM*/  4.135420953e-02,  5.834660926e-02,  4.310316425e-02,  6.205991749e-02, /*DMEAN*/  5.687743199e-02, /*EDMEAN*/  2.387435999e-02 },
        {282, /* SP */ 176, /*MAG*/  7.237000000e+00,  6.420000000e+00,  5.570000000e+00,  5.023000000e+00,  4.637000000e+00,  4.491000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  3.999999911e-02, /*DIAM*/ -2.730189139e-01, -2.300705674e-01, -2.415505700e-01, -2.268727624e-01, /*EDIAM*/  4.121207570e-02,  1.190265538e-02,  1.007847081e-02,  9.345506325e-03, /*DMEAN*/ -2.348583284e-01, /*EDMEAN*/  5.752170286e-03 },
        {283, /* SP */ 184, /*MAG*/  9.035000000e+00,  7.570000000e+00,  6.100000000e+00,  5.190000000e+00,  4.323000000e+00,  3.997000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.880000000e-01,  2.400000000e-01,  3.999999911e-02, /*DIAM*/ -1.009904506e-01, -1.755376326e-01, -1.122365739e-01, -7.637432551e-02, /*EDIAM*/  4.121507289e-02,  7.984074337e-02,  5.797781760e-02,  9.391525255e-03, /*DMEAN*/ -8.004559327e-02, /*EDMEAN*/  8.787485063e-03 },
        {284, /* SP */ 132, /*MAG*/  5.645000000e+00,  5.070000000e+00,  4.440000000e+00,  4.174000000e+00,  3.768000000e+00,  3.748000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.800000000e-01,  2.380000000e-01,  2.360000000e-01, /*DIAM*/ -9.472303720e-02, -1.586422281e-01, -1.068512139e-01, -1.268831997e-01, /*EDIAM*/  4.136756490e-02,  7.766962878e-02,  5.754202869e-02,  5.347448736e-02, /*DMEAN*/ -1.128989168e-01, /*EDMEAN*/  2.622787397e-02 },
        {285, /* SP */ 188, /*MAG*/  6.576000000e+00,  5.450000000e+00,  4.430000000e+00,  3.630000000e+00,  3.065000000e+00,  2.922000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  4.011234000e-02,  2.520000000e-01,  2.060000000e-01,  3.000000000e-01, /*DIAM*/  1.158742334e-01,  9.961413675e-02,  1.074158440e-01,  1.158162544e-01, /*EDIAM*/  4.122057722e-02,  6.990092966e-02,  4.979445515e-02,  6.793354514e-02, /*DMEAN*/  1.110833929e-01, /*EDMEAN*/  2.606126361e-02 },
        {286, /* SP */ 176, /*MAG*/  6.809000000e+00,  5.780000000e+00,  4.780000000e+00,  4.112000000e+00,  3.547000000e+00,  3.274000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.260000000e-01,  1.940000000e-01,  3.040000000e-01, /*DIAM*/ -1.357891004e-02, -2.706163582e-02, -4.990255163e-03,  3.168928524e-02, /*EDIAM*/  4.121207570e-02,  6.270602757e-02,  4.689181666e-02,  6.882767756e-02, /*DMEAN*/ -7.154736101e-03, /*EDMEAN*/  2.517910484e-02 },
        {287, /* SP */ 156, /*MAG*/  5.684000000e+00,  4.970000000e+00,  4.200000000e+00,  3.798000000e+00,  3.457000000e+00,  3.500000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.860000000e-01,  2.060000000e-01,  2.940000000e-01, /*DIAM*/ -2.544822395e-02, -3.221464587e-02, -2.723654944e-02, -5.785177151e-02, /*EDIAM*/  4.126573137e-02,  7.929621373e-02,  4.979974466e-02,  6.656734783e-02, /*DMEAN*/ -3.197926544e-02, /*EDMEAN*/  2.645559599e-02 },
        {288, /* SP */ 124, /*MAG*/  5.008000000e+00,  4.500000000e+00,  3.990000000e+00,  3.617000000e+00,  3.546000000e+00,  3.507000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.840000000e-01,  2.420000000e-01,  3.480000000e-01, /*DIAM*/ -1.312531287e-02, -5.682413775e-02, -8.008054364e-02, -9.225678701e-02, /*EDIAM*/  4.138687277e-02,  7.877885256e-02,  5.851289262e-02,  7.879896627e-02, /*DMEAN*/ -4.579529632e-02, /*EDMEAN*/  2.837917163e-02 },
        {289, /* SP */ 140, /*MAG*/  6.002000000e+00,  5.390000000e+00,  4.710000000e+00,  4.088000000e+00,  3.989000000e+00,  3.857000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.400000000e-01,  3.999999911e-02,  3.999999911e-02, /*DIAM*/ -1.472669671e-01, -1.010411622e-01, -1.440918271e-01, -1.382579222e-01, /*EDIAM*/  4.133861416e-02,  6.661764641e-02,  1.031452738e-02,  9.432684095e-03, /*DMEAN*/ -1.408599985e-01, /*EDMEAN*/  6.728822772e-03 },
        {290, /* SP */ 186, /*MAG*/  8.602000000e+00,  7.670000000e+00,  6.740000000e+00,  6.073000000e+00,  5.587000000e+00,  5.541000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  3.999999911e-02, /*DIAM*/ -4.501993877e-01, -4.091727791e-01, -4.112913181e-01, -4.208278816e-01, /*EDIAM*/  4.121758890e-02,  1.196590778e-02,  1.012687643e-02,  9.406064006e-03, /*DMEAN*/ -4.163837947e-01, /*EDMEAN*/  5.787274474e-03 },
        {291, /* SP */ 168, /*MAG*/  6.479000000e+00,  5.730000000e+00,  4.900000000e+00,  4.554000000e+00,  4.239000000e+00,  4.076000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  7.006426000e-02,  2.260000000e-01,  2.520000000e-01,  3.999999911e-02, /*DIAM*/ -1.714377877e-01, -1.655555912e-01, -1.788986092e-01, -1.585955681e-01, /*EDIAM*/  4.122350083e-02,  6.270740655e-02,  6.086465359e-02,  9.327753710e-03, /*DMEAN*/ -1.599809748e-01, /*EDMEAN*/  8.707668999e-03 },
        {292, /* SP */ 150, /*MAG*/  6.116000000e+00,  5.450000000e+00,  4.760000000e+00,  4.655000000e+00,  4.234000000e+00,  3.911000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.960000000e-01,  2.700000000e-01,  3.999999911e-02, /*DIAM*/ -1.417481703e-01, -2.407436089e-01, -1.971970567e-01, -1.424590613e-01, /*EDIAM*/  4.129332637e-02,  8.206853521e-02,  6.522379621e-02,  9.381015195e-03, /*DMEAN*/ -1.445719534e-01, /*EDMEAN*/  8.808085476e-03 },
        {293, /* SP */ 180, /*MAG*/  6.434000000e+00,  5.630000000e+00,  4.820000000e+00,  4.596000000e+00,  4.133000000e+00,  4.014000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.380000000e-01,  2.000000000e-01,  2.620000000e-01, /*DIAM*/ -1.238243371e-01, -1.663110037e-01, -1.496506818e-01, -1.355792453e-01, /*EDIAM*/  4.121196542e-02,  6.602278408e-02,  4.833916591e-02,  5.933349352e-02, /*DMEAN*/ -1.395566359e-01, /*EDMEAN*/  2.502652613e-02 },
        {294, /* SP */ 176, /*MAG*/  5.456000000e+00,  4.670000000e+00,  3.820000000e+00,  3.423000000e+00,  3.039000000e+00,  2.900000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.700000000e-01,  2.140000000e-01,  3.240000000e-01, /*DIAM*/  5.776109103e-02,  7.841158004e-02,  7.178017400e-02,  8.714914008e-02, /*EDIAM*/  4.121207570e-02,  7.485980052e-02,  5.170829778e-02,  7.335079677e-02, /*DMEAN*/  6.853989890e-02, /*EDMEAN*/  2.691781664e-02 },
        {295, /* SP */ 180, /*MAG*/  6.794000000e+00,  5.860000000e+00,  4.930000000e+00,  4.272000000e+00,  3.825000000e+00,  3.701000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.540000000e-01,  2.520000000e-01,  3.000000000e-01, /*DIAM*/ -8.922433660e-02, -5.897171639e-02, -6.586079727e-02, -5.871063106e-02, /*EDIAM*/  4.121196542e-02,  7.044206510e-02,  6.086533740e-02,  6.792585353e-02, /*DMEAN*/ -7.438239764e-02, /*EDMEAN*/  2.744724213e-02 },
        {296, /* SP */ 164, /*MAG*/  6.444000000e+00,  5.510000000e+00,  4.580000000e+00,  4.032000000e+00,  3.440000000e+00,  3.366000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.360000000e-01,  1.860000000e-01,  2.040000000e-01, /*DIAM*/ -8.226781477e-03, -4.396991954e-02,  2.706983623e-03, -7.125894078e-03, /*EDIAM*/  4.123472009e-02,  6.547281775e-02,  4.497199853e-02,  4.621612345e-02, /*DMEAN*/ -9.688757431e-03, /*EDMEAN*/  2.317597152e-02 },
        {297, /* SP */ 184, /*MAG*/  6.881000000e+00,  5.930000000e+00,  4.990000000e+00,  4.508000000e+00,  3.995000000e+00,  3.984000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  3.200000000e-01,  2.260000000e-01,  2.940000000e-01, /*DIAM*/ -9.167045044e-02, -1.126983459e-01, -1.007501924e-01, -1.165276108e-01, /*EDIAM*/  4.121507289e-02,  8.868525741e-02,  5.460527135e-02,  6.657271812e-02, /*DMEAN*/ -1.005063659e-01, /*EDMEAN*/  2.749554429e-02 },
        {298, /* SP */ 172, /*MAG*/  6.249000000e+00,  5.220000000e+00,  4.220000000e+00,  3.019000000e+00,  2.608000000e+00,  2.331000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  1.800000000e-01,  1.840000000e-01,  2.400000000e-01, /*DIAM*/  1.006659999e-01,  2.262682317e-01,  1.958525183e-01,  2.264862109e-01, /*EDIAM*/  4.121588091e-02,  5.001197960e-02,  4.448394462e-02,  5.435422044e-02, /*DMEAN*/  1.765342349e-01, /*EDMEAN*/  2.278466620e-02 },
        {299, /* SP */ 184, /*MAG*/  7.531000000e+00,  6.430000000e+00,  5.370000000e+00,  4.531000000e+00,  3.992000000e+00,  3.911000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.220000000e-01,  1.440000000e-01,  3.999999911e-02, /*DIAM*/ -9.867045055e-02, -8.067155972e-02, -7.940388858e-02, -8.687067603e-02, /*EDIAM*/  4.121507289e-02,  6.161039358e-02,  3.487078280e-02,  9.391525255e-03, /*DMEAN*/ -8.704051472e-02, /*EDMEAN*/  8.574824535e-03 },
        {300, /* SP */ 188, /*MAG*/  7.053000000e+00,  5.930000000e+00,  4.840000000e+00,  4.303000000e+00,  3.726000000e+00,  3.548000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.700000000e-01,  2.500000000e-01,  2.220000000e-01, /*DIAM*/  1.801423191e-02, -4.980550833e-02, -3.224952778e-02, -1.322024389e-02, /*EDIAM*/  4.122057722e-02,  7.487303004e-02,  6.039202598e-02,  5.030157449e-02, /*DMEAN*/ -8.540115761e-03, /*EDMEAN*/  2.589719023e-02 },
        {301, /* SP */ 184, /*MAG*/  5.769000000e+00,  4.590000000e+00,  3.410000000e+00,  2.619000000e+00,  2.021000000e+00,  1.734000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.900000000e-01,  1.860000000e-01,  2.180000000e-01, /*DIAM*/  3.176895557e-01,  3.072570175e-01,  3.201992302e-01,  3.573848050e-01, /*EDIAM*/  4.121507289e-02,  8.039344403e-02,  4.497346557e-02,  4.939206465e-02, /*DMEAN*/  3.271134357e-01, /*EDMEAN*/  2.415285440e-02 },
        {302, /* SP */ 192, /*MAG*/  6.546000000e+00,  5.270000000e+00,  4.030000000e+00,  3.392000000e+00,  2.884000000e+00,  2.632000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.700000000e-01,  2.140000000e-01,  2.820000000e-01, /*DIAM*/  2.502328476e-01,  1.576901570e-01,  1.476641104e-01,  1.816339040e-01, /*EDIAM*/  4.122737356e-02,  7.487974787e-02,  5.172721806e-02,  6.386823875e-02, /*DMEAN*/  1.987749480e-01, /*EDMEAN*/  2.634613099e-02 },
        {303, /* SP */ 184, /*MAG*/  6.931000000e+00,  5.830000000e+00,  4.770000000e+00,  3.835000000e+00,  3.344000000e+00,  3.103000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.580000000e-01,  2.320000000e-01,  2.980000000e-01, /*DIAM*/  2.132955124e-02,  6.589987103e-02,  5.217587992e-02,  8.019501989e-02, /*EDIAM*/  4.121507289e-02,  7.155184902e-02,  5.605057627e-02,  6.747719486e-02, /*DMEAN*/  4.498441381e-02, /*EDMEAN*/  2.697039044e-02 },
        {304, /* SP */ 188, /*MAG*/  6.899000000e+00,  5.720000000e+00,  4.580000000e+00,  3.633000000e+00,  3.005000000e+00,  2.780000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.560000000e-01,  2.200000000e-01,  2.780000000e-01, /*DIAM*/  9.473423305e-02,  1.195159228e-01,  1.330267393e-01,  1.550425323e-01, /*EDIAM*/  4.122057722e-02,  7.100571919e-02,  5.316566447e-02,  6.295942298e-02, /*DMEAN*/  1.186568548e-01, /*EDMEAN*/  2.626535604e-02 },
        {305, /* SP */ 180, /*MAG*/  7.030000000e+00,  6.000000000e+00,  5.000000000e+00,  4.396000000e+00,  3.821000000e+00,  3.659000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.320000000e-01,  2.360000000e-01,  3.120000000e-01, /*DIAM*/ -5.770433613e-02, -8.254314531e-02, -5.912149755e-02, -4.552814911e-02, /*EDIAM*/  4.121196542e-02,  6.436588295e-02,  5.701026075e-02,  7.063954284e-02, /*DMEAN*/ -6.079615224e-02, /*EDMEAN*/  2.677140341e-02 },
        {306, /* SP */ 188, /*MAG*/  6.719000000e+00,  5.500000000e+00,  4.320000000e+00,  3.388000000e+00,  2.795000000e+00,  2.527000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.340000000e-01,  1.840000000e-01,  2.580000000e-01, /*DIAM*/  1.635342341e-01,  1.704355664e-01,  1.746142885e-01,  2.065096864e-01, /*EDIAM*/  4.122057722e-02,  6.493036886e-02,  4.449874721e-02,  5.843808192e-02, /*DMEAN*/  1.755167319e-01, /*EDMEAN*/  2.428227762e-02 },
        {307, /* SP */ 196, /*MAG*/  6.389000000e+00,  5.020000000e+00,  3.680000000e+00,  2.876000000e+00,  2.091000000e+00,  1.939000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.300000000e-01,  1.940000000e-01,  2.700000000e-01, /*DIAM*/  3.658601840e-01,  2.871407820e-01,  3.330984535e-01,  3.370828193e-01, /*EDIAM*/  4.123418936e-02,  6.384153072e-02,  4.691963506e-02,  6.115972228e-02, /*DMEAN*/  3.390010488e-01, /*EDMEAN*/  2.481411359e-02 },
        {308, /* SP */ 196, /*MAG*/  7.035000000e+00,  5.720000000e+00,  4.440000000e+00,  3.689000000e+00,  3.002000000e+00,  2.782000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  3.020000000e-01,  2.160000000e-01,  2.620000000e-01, /*DIAM*/  1.923801814e-01,  1.158639937e-01,  1.421957269e-01,  1.647251525e-01, /*EDIAM*/  4.123418936e-02,  8.372707520e-02,  5.221502723e-02,  5.935148074e-02, /*DMEAN*/  1.654979803e-01, /*EDMEAN*/  2.641439896e-02 },
        {309, /* SP */ 200, /*MAG*/  7.341000000e+00,  5.900000000e+00,  4.460000000e+00,  2.473000000e+00,  1.855000000e+00,  1.587000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.620000000e-01,  2.120000000e-01,  2.380000000e-01, /*DIAM*/  2.453826845e-01,  4.718119212e-01,  4.312265969e-01,  4.453474882e-01, /*EDIAM*/  4.123976885e-02,  7.268200060e-02,  5.125731060e-02,  5.393120170e-02, /*DMEAN*/  3.659293626e-01, /*EDMEAN*/  2.530120743e-02 },
        {310, /* SP */ 196, /*MAG*/  7.154000000e+00,  5.970000000e+00,  4.830000000e+00,  4.180000000e+00,  3.593000000e+00,  3.406000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.600000000e-01,  2.160000000e-01,  3.240000000e-01, /*DIAM*/  6.116017944e-02, -8.413651929e-04,  9.931133523e-03,  3.009741324e-02, /*EDIAM*/  4.123418936e-02,  7.212428782e-02,  5.221502723e-02,  7.336790359e-02, /*DMEAN*/  3.385332713e-02, /*EDMEAN*/  2.685185694e-02 },
        {311, /* SP */ 200, /*MAG*/  7.153000000e+00,  5.690000000e+00,  4.220000000e+00,  3.093000000e+00,  2.340000000e+00,  2.041000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  1.860000000e-01,  1.960000000e-01,  2.520000000e-01, /*DIAM*/  3.010226853e-01,  2.840797755e-01,  3.055612254e-01,  3.370993114e-01, /*EDIAM*/  4.123976885e-02,  5.171309987e-02,  4.740661787e-02,  5.709480230e-02, /*DMEAN*/  3.047132059e-01, /*EDMEAN*/  2.357729872e-02 },
        {312, /* SP */ 184, /*MAG*/  6.492000000e+00,  5.350000000e+00,  4.240000000e+00,  3.777000000e+00,  3.160000000e+00,  2.876000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  3.020000000e-01,  2.260000000e-01,  2.860000000e-01, /*DIAM*/  1.427495531e-01,  4.509629929e-02,  7.676731997e-02,  1.189468453e-01, /*EDIAM*/  4.121507289e-02,  8.370988966e-02,  5.460527135e-02,  6.476381766e-02, /*DMEAN*/  1.107142663e-01, /*EDMEAN*/  2.718177924e-02 },
        {313, /* SP */ 184, /*MAG*/  6.913000000e+00,  5.970000000e+00,  5.040000000e+00,  3.933000000e+00,  3.557000000e+00,  3.550000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.300000000e-01,  1.980000000e-01,  2.380000000e-01, /*DIAM*/ -1.046304506e-01,  4.952487079e-02,  6.564984296e-03, -1.727213485e-02, /*EDIAM*/  4.121507289e-02,  6.381899051e-02,  4.786224593e-02,  5.391227371e-02, /*DMEAN*/ -3.301738300e-02, /*EDMEAN*/  2.435452653e-02 },
        {314, /* SP */ 186, /*MAG*/  5.999000000e+00,  4.820000000e+00,  3.660000000e+00,  2.949000000e+00,  2.197000000e+00,  2.085000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.340000000e-01,  1.960000000e-01,  2.160000000e-01, /*DIAM*/  2.729406231e-01,  2.366665162e-01,  2.889810659e-01,  2.862962165e-01, /*EDIAM*/  4.121758890e-02,  6.492674065e-02,  4.738370932e-02,  4.894289006e-02, /*DMEAN*/  2.751766384e-01, /*EDMEAN*/  2.382205470e-02 },
        {315, /* SP */ 200, /*MAG*/  7.483000000e+00,  6.240000000e+00,  5.040000000e+00,  4.453000000e+00,  3.704000000e+00,  3.574000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  1.980000000e-01,  2.060000000e-01,  3.200000000e-01, /*DIAM*/  5.462268165e-02, -5.011665801e-02, -8.123200267e-04,  4.668649531e-03, /*EDIAM*/  4.123976885e-02,  5.502058795e-02,  4.981313341e-02,  7.246618504e-02, /*DMEAN*/  1.085775331e-02, /*EDMEAN*/  2.511929769e-02 },
        {316, /* SP */ 184, /*MAG*/  6.825000000e+00,  5.670000000e+00,  4.550000000e+00,  3.795000000e+00,  3.171000000e+00,  2.997000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.880000000e-01,  2.560000000e-01,  2.700000000e-01, /*DIAM*/  8.680955222e-02,  6.468558530e-02,  8.731206721e-02,  9.997604208e-02, /*EDIAM*/  4.121507289e-02,  7.984074337e-02,  6.183280925e-02,  6.114625088e-02, /*DMEAN*/  8.684804902e-02, /*EDMEAN*/  2.751365169e-02 },
        {317, /* SP */ 188, /*MAG*/  7.167000000e+00,  6.280000000e+00,  5.390000000e+00,  4.889000000e+00,  4.322000000e+00,  4.317000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.460000000e-01,  3.999999911e-02,  2.520000000e-01, /*DIAM*/ -1.983057713e-01, -1.851269389e-01, -1.615958332e-01, -1.780304653e-01, /*EDIAM*/  4.122057722e-02,  6.824388838e-02,  1.014222059e-02,  5.708180960e-02, /*DMEAN*/ -1.654082611e-01, /*EDMEAN*/  9.267227057e-03 },
        {318, /* SP */ 200, /*MAG*/  7.286000000e+00,  5.810000000e+00,  4.320000000e+00,  3.076000000e+00,  2.361000000e+00,  2.128000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  1.980000000e-01,  1.660000000e-01,  2.460000000e-01, /*DIAM*/  2.850826851e-01,  2.979994186e-01,  3.054444939e-01,  3.205664645e-01, /*EDIAM*/  4.123976885e-02,  5.502058795e-02,  4.019134172e-02,  5.573891391e-02, /*DMEAN*/  3.000182316e-01, /*EDMEAN*/  2.264198036e-02 },
        {319, /* SP */ 180, /*MAG*/  7.218000000e+00,  6.200000000e+00,  5.210000000e+00,  4.676000000e+00,  4.064000000e+00,  3.899000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.280000000e-01,  2.260000000e-01,  2.200000000e-01, /*DIAM*/ -1.051443368e-01, -1.446860034e-01, -1.094950392e-01, -9.457924473e-02, /*EDIAM*/  4.121196542e-02,  6.326139329e-02,  5.460118552e-02,  4.983915468e-02, /*DMEAN*/ -1.097850443e-01, /*EDMEAN*/  2.466102536e-02 },
        {320, /* SP */ 196, /*MAG*/  6.951000000e+00,  5.540000000e+00,  4.150000000e+00,  3.145000000e+00,  2.453000000e+00,  2.147000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.460000000e-01,  1.840000000e-01,  2.320000000e-01, /*DIAM*/  2.879001828e-01,  2.526139957e-01,  2.672151840e-01,  3.036813589e-01, /*EDIAM*/  4.123418936e-02,  6.825837177e-02,  4.451358359e-02,  5.257178717e-02, /*DMEAN*/  2.804589645e-01, /*EDMEAN*/  2.396426450e-02 },
        {321, /* SP */ 190, /*MAG*/  5.818000000e+00,  4.500000000e+00,  3.250000000e+00,  2.435000000e+00,  1.817000000e+00,  1.668000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  3.020000000e-01,  2.260000000e-01,  2.160000000e-01, /*DIAM*/  4.272861514e-01,  3.604581026e-01,  3.712583586e-01,  3.770332467e-01, /*EDIAM*/  4.122389129e-02,  8.371824248e-02,  5.461372476e-02,  4.894874008e-02, /*DMEAN*/  3.942541272e-01, /*EDMEAN*/  2.549169879e-02 },
        {322, /* SP */ 174, /*MAG*/  5.611000000e+00,  4.680000000e+00,  3.730000000e+00,  3.162000000e+00,  2.695000000e+00,  2.590000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.720000000e-01,  2.120000000e-01,  2.400000000e-01, /*DIAM*/  1.466118952e-01,  1.483147579e-01,  1.538500750e-01,  1.555936240e-01, /*EDIAM*/  4.121350068e-02,  7.541192054e-02,  5.122616517e-02,  5.435515107e-02, /*DMEAN*/  1.506682347e-01, /*EDMEAN*/  2.545592604e-02 },
        {323, /* SP */ 190, /*MAG*/  5.781000000e+00,  4.590000000e+00,  3.470000000e+00,  2.639000000e+00,  2.127000000e+00,  1.967000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.740000000e-01,  2.340000000e-01,  2.840000000e-01, /*DIAM*/  3.305461499e-01,  3.109045304e-01,  3.001844276e-01,  3.117412749e-01, /*EDIAM*/  4.122389129e-02,  7.598139722e-02,  5.654053450e-02,  6.431818086e-02, /*DMEAN*/  3.173557922e-01, /*EDMEAN*/  2.704940391e-02 },
        {324, /* SP */ 172, /*MAG*/  4.245000000e+00,  3.330000000e+00,  2.420000000e+00,  1.879000000e+00,  1.557000000e+00,  1.439000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.920000000e-01,  2.580000000e-01,  3.120000000e-01, /*DIAM*/  4.079860045e-01,  3.966789485e-01,  3.714478516e-01,  3.786613957e-01, /*EDIAM*/  4.121588091e-02,  8.093903004e-02,  6.230891778e-02,  7.063520108e-02, /*DMEAN*/  3.938800041e-01, /*EDMEAN*/  2.837121680e-02 },
        {325, /* SP */ 188, /*MAG*/  6.674000000e+00,  5.450000000e+00,  4.290000000e+00,  3.566000000e+00,  2.953000000e+00,  2.804000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  4.004997000e-02,  2.940000000e-01,  2.320000000e-01,  2.680000000e-01, /*DIAM*/  1.766342343e-01,  1.173284227e-01,  1.344352996e-01,  1.425169847e-01, /*EDIAM*/  4.122057722e-02,  8.150440511e-02,  5.605586871e-02,  6.069867408e-02, /*DMEAN*/  1.528790770e-01, /*EDMEAN*/  2.693790673e-02 },
        {326, /* SP */ 136, /*MAG*/  5.340000000e+00,  4.800000000e+00,  4.190000000e+00,  3.936000000e+00,  3.681000000e+00,  3.638000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.500000000e-01,  2.220000000e-01,  2.540000000e-01, /*DIAM*/ -6.793608583e-02, -1.089772342e-01, -9.553156769e-02, -1.066589153e-01, /*EDIAM*/  4.135420953e-02,  6.938268431e-02,  5.368602185e-02,  5.753908295e-02, /*DMEAN*/ -8.846544449e-02, /*EDMEAN*/  2.582547245e-02 },
        {327, /* SP */ 180, /*MAG*/  6.434000000e+00,  5.630000000e+00,  4.820000000e+00,  4.596000000e+00,  4.133000000e+00,  4.014000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.380000000e-01,  2.000000000e-01,  2.620000000e-01, /*DIAM*/ -1.238243371e-01, -1.663110037e-01, -1.496506818e-01, -1.355792453e-01, /*EDIAM*/  4.121196542e-02,  6.602278408e-02,  4.833916591e-02,  5.933349352e-02, /*DMEAN*/ -1.395566359e-01, /*EDMEAN*/  2.502652613e-02 },
        {328, /* SP */ 132, /*MAG*/  4.614000000e+00,  4.100000000e+00,  3.510000000e+00,  3.031000000e+00,  2.863000000e+00,  2.697000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.440000000e-01,  2.060000000e-01,  2.880000000e-01, /*DIAM*/  6.145696513e-02,  8.324170408e-02,  7.146785487e-02,  8.544527062e-02, /*EDIAM*/  4.136756490e-02,  6.773030320e-02,  4.984165555e-02,  6.522773839e-02, /*DMEAN*/  7.139693598e-02, /*EDMEAN*/  2.582138850e-02 },
        {329, /* SP */ 138, /*MAG*/  4.645000000e+00,  4.050000000e+00,  3.400000000e+00,  3.143000000e+00,  2.875000000e+00,  2.723000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.460000000e-01,  2.060000000e-01,  2.660000000e-01, /*DIAM*/  1.132270233e-01,  5.525280975e-02,  6.868730567e-02,  8.189788201e-02, /*EDIAM*/  4.134666816e-02,  6.827629836e-02,  4.983308726e-02,  6.024993273e-02, /*DMEAN*/  8.696869260e-02, /*EDMEAN*/  2.549219823e-02 },
        {330, /* SP */ 160, /*MAG*/  5.521000000e+00,  4.840000000e+00,  4.110000000e+00,  3.407000000e+00,  3.039000000e+00,  2.957000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  1.920000000e-01,  1.820000000e-01,  2.120000000e-01, /*DIAM*/ -2.578585703e-02,  7.171610992e-02,  6.994206428e-02,  6.461515108e-02, /*EDIAM*/  4.124903710e-02,  5.333593853e-02,  4.401563994e-02,  4.802599628e-02, /*DMEAN*/  3.837901799e-02, /*EDMEAN*/  2.247003576e-02 },
        {331, /* SP */ 132, /*MAG*/  4.865000000e+00,  4.290000000e+00,  3.630000000e+00,  3.194000000e+00,  2.916000000e+00,  2.835000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.460000000e-01,  2.000000000e-01,  2.600000000e-01, /*DIAM*/  6.127696512e-02,  5.271491792e-02,  6.651843846e-02,  5.921169359e-02, /*EDIAM*/  4.136756490e-02,  6.828229708e-02,  4.839852623e-02,  5.889841115e-02, /*DMEAN*/  6.112350986e-02, /*EDMEAN*/  2.518267835e-02 },
        {332, /* SP */ 146, /*MAG*/  5.320000000e+00,  4.690000000e+00,  3.990000000e+00,  3.397000000e+00,  3.154000000e+00,  3.038000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.600000000e-01,  2.040000000e-01,  2.580000000e-01, /*DIAM*/ -5.605089039e-03,  4.389217688e-02,  3.056627453e-02,  3.246183371e-02, /*EDIAM*/  4.131208422e-02,  7.213022436e-02,  4.933774324e-02,  5.843492030e-02, /*DMEAN*/  1.821170533e-02, /*EDMEAN*/  2.546696016e-02 },
        {333, /* SP */ 72, /*MAG*/  3.686000000e+00,  3.580000000e+00,  3.460000000e+00,  3.540000000e+00,  3.495000000e+00,  3.535000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  3.300000000e-01,  2.840000000e-01,  2.620000000e-01, /*DIAM*/ -1.215214278e-01, -1.717619621e-01, -1.611951324e-01, -1.803033262e-01, /*EDIAM*/  4.151649521e-02,  9.162681348e-02,  6.876915663e-02,  5.949825780e-02, /*DMEAN*/ -1.473947409e-01, /*EDMEAN*/  2.852564686e-02 },
        {334, /* SP */ 100, /*MAG*/  4.480000000e+00,  4.160000000e+00,  3.760000000e+00,  3.221000000e+00,  3.156000000e+00,  2.978000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.320000000e-01,  2.460000000e-01,  3.160000000e-01, /*DIAM*/ -5.654928265e-02,  1.962374015e-03, -1.567351262e-02,  2.203855581e-04, /*EDIAM*/  4.141222888e-02,  6.446177125e-02,  5.951482765e-02,  7.159614651e-02, /*DMEAN*/ -2.847088207e-02, /*EDMEAN*/  2.717770824e-02 },
        {335, /* SP */ 100, /*MAG*/  4.010000000e+00,  3.650000000e+00,  3.240000000e+00,  3.222000000e+00,  3.025000000e+00,  2.864000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.800000000e-01,  2.380000000e-01,  2.920000000e-01, /*DIAM*/  7.025071924e-02, -3.747512657e-02, -5.105419077e-03,  1.261454633e-02, /*EDIAM*/  4.141222888e-02,  7.770518271e-02,  5.759051443e-02,  6.617308112e-02, /*DMEAN*/  2.827436926e-02, /*EDMEAN*/  2.747392168e-02 },
        {336, /* SP */ 172, /*MAG*/  6.170000000e+00,  5.400000000e+00,  4.620000000e+00,  4.059000000e+00,  3.718000000e+00,  3.690000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  4.100000000e-01,  3.380000000e-01,  3.999999911e-02, /*DIAM*/ -9.591400299e-02, -4.776748671e-02, -6.450546228e-02, -7.629481549e-02, /*EDIAM*/  4.121588091e-02,  1.135659274e-01,  8.159252281e-02,  9.332747982e-03, /*DMEAN*/ -7.725224820e-02, /*EDMEAN*/  8.815843540e-03 },
        {337, /* SP */ 152, /*MAG*/  6.046000000e+00,  5.370000000e+00,  4.630000000e+00,  4.267000000e+00,  4.040000000e+00,  3.821000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  3.280000000e-01,  2.580000000e-01,  3.999999911e-02, /*DIAM*/ -1.227554424e-01, -1.368159058e-01, -1.529506419e-01, -1.228256980e-01, /*EDIAM*/  4.128394110e-02,  9.090931111e-02,  6.232979479e-02,  9.371210334e-03, /*DMEAN*/ -1.235581373e-01, /*EDMEAN*/  8.801072232e-03 },
        {338, /* SP */ 132, /*MAG*/  5.361000000e+00,  4.820000000e+00,  4.240000000e+00,  4.033000000e+00,  3.758000000e+00,  3.644000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.180000000e-01,  1.980000000e-01,  2.220000000e-01, /*DIAM*/ -6.580303677e-02, -1.388118707e-01, -1.147500467e-01, -1.099196958e-01, /*EDIAM*/  4.136756490e-02,  6.055700363e-02,  4.791754132e-02,  5.031120266e-02, /*DMEAN*/ -1.002658715e-01, /*EDMEAN*/  2.382588568e-02 },
        {339, /* SP */ 64, /*MAG*/  2.373000000e+00,  2.340000000e+00,  2.320000000e+00,  2.269000000e+00,  2.359000000e+00,  2.285000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.440000000e-01,  1.640000000e-01,  2.440000000e-01, /*DIAM*/  5.652852802e-02,  6.678312919e-02,  4.384782796e-02,  5.255771815e-02, /*EDIAM*/  4.156156435e-02,  6.796653595e-02,  4.005538042e-02,  5.546627818e-02, /*DMEAN*/  5.262006343e-02, /*EDMEAN*/  2.349124803e-02 },
        {340, /* SP */ 76, /*MAG*/  2.688000000e+00,  2.560000000e+00,  2.440000000e+00,  2.238000000e+00,  2.191000000e+00,  2.144000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.580000000e-01,  1.880000000e-01,  2.060000000e-01, /*DIAM*/  1.067427864e-01,  1.179303110e-01,  1.190148180e-01,  1.151189696e-01, /*EDIAM*/  4.149458958e-02,  7.175479163e-02,  4.571337157e-02,  4.686597214e-02, /*DMEAN*/  1.136118133e-01, /*EDMEAN*/  2.375113154e-02 },
        {341, /* SP */ 172, /*MAG*/  6.033000000e+00,  5.310000000e+00,  4.530000000e+00,  3.988000000e+00,  3.648000000e+00,  3.588000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  4.011234000e-02,  2.420000000e-01,  2.280000000e-01,  3.999999911e-02, /*DIAM*/ -1.070540032e-01, -3.502641509e-02, -5.133036481e-02, -5.557948671e-02, /*EDIAM*/  4.121588091e-02,  6.712399553e-02,  5.508045650e-02,  9.332747982e-03, /*DMEAN*/ -5.842902456e-02, /*EDMEAN*/  8.702626806e-03 },
        {342, /* SP */ 136, /*MAG*/  4.108000000e+00,  3.590000000e+00,  2.980000000e+00,  2.597000000e+00,  2.363000000e+00,  2.269000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.520000000e-01,  2.300000000e-01,  2.540000000e-01, /*DIAM*/  1.604239176e-01,  1.687281271e-01,  1.725229110e-01,  1.713191910e-01, /*EDIAM*/  4.135420953e-02,  6.993478143e-02,  5.561148534e-02,  5.753908295e-02, /*DMEAN*/  1.666623592e-01, /*EDMEAN*/  2.607616695e-02 },
        {343, /* SP */ 140, /*MAG*/  4.828000000e+00,  4.240000000e+00,  3.570000000e+00,  3.213000000e+00,  2.905000000e+00,  2.848000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.180000000e-01,  1.980000000e-01,  3.100000000e-01, /*DIAM*/  6.785303611e-02,  5.284276868e-02,  6.998599710e-02,  5.983697122e-02, /*EDIAM*/  4.133861416e-02,  6.054737921e-02,  4.790524751e-02,  7.019624188e-02, /*DMEAN*/  6.463811265e-02, /*EDMEAN*/  2.528427803e-02 },
        {344, /* SP */ 140, /*MAG*/  4.802000000e+00,  4.230000000e+00,  3.560000000e+00,  3.232000000e+00,  2.992000000e+00,  2.923000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  4.011234000e-02,  2.340000000e-01,  1.920000000e-01,  2.740000000e-01, /*DIAM*/  5.993303600e-02,  4.681598288e-02,  4.858521857e-02,  4.260339432e-02, /*EDIAM*/  4.133861416e-02,  6.496176595e-02,  4.646209829e-02,  6.205685401e-02, /*DMEAN*/  5.169484575e-02, /*EDMEAN*/  2.490602662e-02 },
        {345, /* SP */ 128, /*MAG*/  4.537000000e+00,  4.040000000e+00,  3.450000000e+00,  3.179000000e+00,  2.980000000e+00,  2.739000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  1.200167000e-01,  2.440000000e-01,  2.160000000e-01,  3.320000000e-01, /*DIAM*/  6.784054924e-02,  3.331028707e-02,  3.920017932e-02,  7.192496582e-02, /*EDIAM*/  4.137844895e-02,  6.773317144e-02,  5.225203904e-02,  7.517850455e-02, /*DMEAN*/  5.497021396e-02, /*EDMEAN*/  2.671934357e-02 },
        {346, /* SP */ 108, /*MAG*/  4.834000000e+00,  4.470000000e+00,  4.060000000e+00,  3.561000000e+00,  3.462000000e+00,  3.336000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.500000000e-01,  2.460000000e-01,  3.240000000e-01, /*DIAM*/ -8.863491196e-02, -5.989503870e-02, -7.016361443e-02, -6.560364225e-02, /*EDIAM*/  4.140392777e-02,  6.940641379e-02,  5.949562832e-02,  7.338909961e-02, /*DMEAN*/ -7.638077994e-02, /*EDMEAN*/  2.765003168e-02 },
        {347, /* SP */ 172, /*MAG*/  5.260000000e+00,  4.540000000e+00,  3.720000000e+00,  2.660000000e+00,  2.253000000e+00,  1.971000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  4.480000000e-01,  6.980000000e-01,  6.000000000e-01, /*DIAM*/  4.508599911e-02,  2.734200181e-01,  2.534478498e-01,  2.900774527e-01, /*EDIAM*/  4.121588091e-02,  1.240766822e-01,  1.684141288e-01,  1.357854895e-01, /*DMEAN*/  9.385085517e-02, /*EDMEAN*/  3.638144789e-02 },
        {348, /* SP */ 68, /*MAG*/  3.857000000e+00,  3.710000000e+00,  3.580000000e+00,  3.564000000e+00,  3.440000000e+00,  3.425000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.580000000e-01,  2.340000000e-01,  2.660000000e-01, /*DIAM*/ -1.338911243e-01, -1.769409163e-01, -1.510682642e-01, -1.602652217e-01, /*EDIAM*/  4.153908537e-02,  7.179955479e-02,  5.678024457e-02,  6.041284352e-02, /*DMEAN*/ -1.492456851e-01, /*EDMEAN*/  2.666487692e-02 },
        {349, /* SP */ 124, /*MAG*/  4.328000000e+00,  3.850000000e+00,  3.310000000e+00,  3.149000000e+00,  2.875000000e+00,  2.703000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.420000000e-01,  2.240000000e-01,  3.140000000e-01, /*DIAM*/  9.827468879e-02,  2.280086344e-02,  5.498560624e-02,  7.258993077e-02, /*EDIAM*/  4.138687277e-02,  6.718357092e-02,  5.418102613e-02,  7.111142590e-02, /*DMEAN*/  7.108342401e-02, /*EDMEAN*/  2.673805609e-02 },
        {350, /* SP */ 148, /*MAG*/  6.142000000e+00,  5.490000000e+00,  4.800000000e+00,  4.667000000e+00,  4.162000000e+00,  4.186000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.600000000e-01,  1.780000000e-01,  2.920000000e-01, /*DIAM*/ -1.551974142e-01, -2.436208316e-01, -1.789025482e-01, -2.049728171e-01, /*EDIAM*/  4.130274461e-02,  7.212696679e-02,  4.307893186e-02,  6.612043693e-02, /*DMEAN*/ -1.817048401e-01, /*EDMEAN*/  2.490837357e-02 },
        {351, /* SP */ 120, /*MAG*/  5.004000000e+00,  4.570000000e+00,  4.070000000e+00,  3.803000000e+00,  3.648000000e+00,  3.502000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.640000000e-01,  2.480000000e-01,  3.620000000e-01, /*DIAM*/ -6.960894534e-02, -1.070508070e-01, -1.036930306e-01, -9.186175341e-02, /*EDIAM*/  4.139308516e-02,  7.325823036e-02,  5.996094678e-02,  8.196749101e-02, /*DMEAN*/ -8.612363181e-02, /*EDMEAN*/  2.837366207e-02 },
        {352, /* SP */ 108, /*MAG*/  5.010000000e+00,  4.620000000e+00,  4.170000000e+00,  3.928000000e+00,  3.665000000e+00,  3.639000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.540000000e-01,  1.920000000e-01,  2.380000000e-01, /*DIAM*/ -1.025149122e-01, -1.499575400e-01, -1.129496073e-01, -1.302240812e-01, /*EDIAM*/  4.140392777e-02,  7.051025954e-02,  4.651023511e-02,  5.395638567e-02, /*DMEAN*/ -1.174499708e-01, /*EDMEAN*/  2.458515073e-02 },
        {353, /* SP */ 124, /*MAG*/  4.673000000e+00,  4.190000000e+00,  3.640000000e+00,  3.527000000e+00,  3.286000000e+00,  3.190000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.420000000e-01,  1.760000000e-01,  2.560000000e-01, /*DIAM*/  3.337468783e-02, -5.571699487e-02, -3.014279970e-02, -2.867284446e-02, /*EDIAM*/  4.138687277e-02,  6.718357092e-02,  4.263987190e-02,  5.800099155e-02, /*DMEAN*/ -1.046548078e-02, /*EDMEAN*/  2.408614163e-02 },
        {354, /* SP */ 60, /*MAG*/  3.004000000e+00,  2.990000000e+00,  3.000000000e+00,  3.084000000e+00,  3.048000000e+00,  2.876000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  6.007495000e-02,  3.300000000e-01,  2.800000000e-01,  3.600000000e-01, /*DIAM*/ -9.919599380e-02, -1.195278084e-01, -1.058395989e-01, -7.416189682e-02, /*EDIAM*/  4.158344239e-02,  9.167070080e-02,  6.784883718e-02,  8.163888005e-02, /*DMEAN*/ -9.939680136e-02, /*EDMEAN*/  3.020976758e-02 },
        {355, /* SP */ 172, /*MAG*/  5.931000000e+00,  5.170000000e+00,  4.420000000e+00,  3.553000000e+00,  3.327000000e+00,  3.037000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  7.002857000e-02,  2.060000000e-01,  2.140000000e-01,  3.200000000e-01, /*DIAM*/ -5.549400239e-02,  7.462537226e-02,  2.033500591e-02,  6.542051509e-02, /*EDIAM*/  4.121588091e-02,  5.718443297e-02,  5.170802597e-02,  7.244450395e-02, /*DMEAN*/  6.605406030e-03, /*EDMEAN*/  2.558466960e-02 },
        {356, /* SP */ 176, /*MAG*/  5.456000000e+00,  4.670000000e+00,  3.820000000e+00,  3.423000000e+00,  3.039000000e+00,  2.900000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.700000000e-01,  2.140000000e-01,  3.240000000e-01, /*DIAM*/  5.776109103e-02,  7.841158004e-02,  7.178017400e-02,  8.714914008e-02, /*EDIAM*/  4.121207570e-02,  7.485980052e-02,  5.170829778e-02,  7.335079677e-02, /*DMEAN*/  6.853989890e-02, /*EDMEAN*/  2.691781664e-02 },
        {357, /* SP */ 116, /*MAG*/  4.885000000e+00,  4.490000000e+00,  4.050000000e+00,  3.878000000e+00,  3.716000000e+00,  3.537000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.840000000e-01,  2.340000000e-01,  2.960000000e-01, /*DIAM*/ -7.530701953e-02, -1.380062556e-01, -1.255170975e-01, -1.045881972e-01, /*EDIAM*/  4.139755035e-02,  7.878328660e-02,  5.659587761e-02,  6.704945548e-02, /*DMEAN*/ -1.005288146e-01, /*EDMEAN*/  2.746086211e-02 },
        {358, /* SP */ 64, /*MAG*/  3.606000000e+00,  3.520000000e+00,  3.430000000e+00,  3.455000000e+00,  3.390000000e+00,  3.377000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.680000000e-01,  2.060000000e-01,  2.640000000e-01, /*DIAM*/ -1.466114750e-01, -1.708775886e-01, -1.562066497e-01, -1.635298763e-01, /*EDIAM*/  4.156156435e-02,  7.457102483e-02,  5.008841476e-02,  5.997261648e-02, /*DMEAN*/ -1.555440278e-01, /*EDMEAN*/  2.593263810e-02 },
        {359, /* SP */ 64, /*MAG*/  3.791000000e+00,  3.760000000e+00,  3.710000000e+00,  3.830000000e+00,  3.867000000e+00,  3.851000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.380000000e-01,  2.080000000e-01,  2.740000000e-01, /*DIAM*/ -2.287114762e-01, -2.562436613e-01, -2.613817486e-01, -2.644787829e-01, /*EDIAM*/  4.156156435e-02,  6.631661131e-02,  5.056716566e-02,  6.222683117e-02, /*DMEAN*/ -2.479341880e-01, /*EDMEAN*/  2.572217507e-02 },
        {360, /* SP */ 128, /*MAG*/  4.702000000e+00,  4.200000000e+00,  3.600000000e+00,  3.358000000e+00,  3.078000000e+00,  2.961000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.540000000e-01,  2.140000000e-01,  2.860000000e-01, /*DIAM*/  3.894054881e-02, -3.948642052e-03,  2.215737751e-02,  2.589576806e-02, /*EDIAM*/  4.137844895e-02,  7.049327596e-02,  5.177088235e-02,  6.477849011e-02, /*DMEAN*/  2.604465181e-02, /*EDMEAN*/  2.623576522e-02 },
        {361, /* SP */ 128, /*MAG*/  4.637000000e+00,  4.130000000e+00,  3.540000000e+00,  3.299000000e+00,  2.988000000e+00,  2.946000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.860000000e-01,  2.400000000e-01,  2.880000000e-01, /*DIAM*/  5.604054906e-02,  7.006715254e-03,  4.098228051e-02,  2.745051261e-02, /*EDIAM*/  4.137844895e-02,  7.932924087e-02,  5.802764674e-02,  6.523060664e-02, /*DMEAN*/  4.114348446e-02, /*EDMEAN*/  2.750992688e-02 },
        {362, /* SP */ 228, /*MAG*/  9.650000000e+00,  8.090000000e+00,  5.610000000e+00,  5.252000000e+00,  4.476000000e+00,  4.018000000e+00, /*EMAG*/  4.472136000e-02,  3.999999911e-02,  1.711724000e-01,  2.640000000e-01,  2.000000000e-01,  3.999999911e-02, /*DIAM*/  6.715363606e-02, -1.054058937e-01, -6.128847186e-02, -1.117355064e-03, /*EDIAM*/  4.313407541e-02,  7.322558914e-02,  4.838306021e-02,  9.525582923e-03, /*DMEAN*/ -5.800551036e-04, /*EDMEAN*/  8.887988734e-03 },
        {363, /* SP */ 190, /*MAG*/  6.630000000e+00,  5.740000000e+00,  4.770000000e+00,  4.367000000e+00,  3.722000000e+00,  3.683000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  3.100000000e-01,  2.300000000e-01,  2.680000000e-01, /*DIAM*/ -8.607385628e-02, -7.907761822e-02, -3.716966302e-02, -4.633172309e-02, /*EDIAM*/  4.122389129e-02,  8.592920516e-02,  5.557710229e-02,  6.070104777e-02, /*DMEAN*/ -6.535207863e-02, /*EDMEAN*/  2.704821613e-02 },
        {364, /* SP */ 192, /*MAG*/  6.708000000e+00,  5.790000000e+00,  4.730000000e+00,  4.152000000e+00,  3.657000000e+00,  3.481000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.640000000e-01,  2.440000000e-01,  2.080000000e-01, /*DIAM*/ -7.572715730e-02, -1.273841694e-02, -1.737091149e-02,  3.188645875e-03, /*EDIAM*/  4.122737356e-02,  7.322238145e-02,  5.895220348e-02,  4.714432536e-02, /*DMEAN*/ -3.376456724e-02, /*EDMEAN*/  2.522877332e-02 },
        {365, /* SP */ 226, /*MAG*/  9.444000000e+00,  7.970000000e+00,  5.890000000e+00,  4.999000000e+00,  4.149000000e+00,  4.039000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  3.000000000e-01,  2.120000000e-01,  2.600000000e-01, /*DIAM*/  1.671156343e-02, -4.500255488e-02,  8.132203710e-03, -1.272304714e-02, /*EDIAM*/  4.128610059e-02,  8.316640980e-02,  5.126103841e-02,  5.890094761e-02, /*DMEAN*/  1.949496245e-03, /*EDMEAN*/  2.623702547e-02 },
        {366, /* SP */ 208, /*MAG*/  9.120000000e+00,  7.700000000e+00,  5.980000000e+00,  4.779000000e+00,  4.043000000e+00,  4.136000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  4.701026000e-02,  1.740000000e-01,  2.060000000e-01,  3.999999911e-02, /*DIAM*/ -9.584276233e-02, -1.826925889e-02, -1.108577472e-02, -7.230654748e-02, /*EDIAM*/  4.124396433e-02,  4.841181596e-02,  4.981727924e-02,  9.509512854e-03, /*DMEAN*/ -7.012275547e-02, /*EDMEAN*/  8.758009298e-03 },
        {367, /* SP */ 212, /*MAG*/  7.926000000e+00,  6.600000000e+00,  5.310000000e+00,  3.894000000e+00,  3.298000000e+00,  2.962000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.900000000e-01,  2.600000000e-01,  2.880000000e-01, /*DIAM*/  8.746045695e-02,  1.464605568e-01,  1.297187181e-01,  1.707678557e-01, /*EDIAM*/  4.124302509e-02,  8.041559913e-02,  6.281798601e-02,  6.523134331e-02, /*DMEAN*/  1.193418817e-01, /*EDMEAN*/  2.800554938e-02 },
        {368, /* SP */ 228, /*MAG*/  8.992000000e+00,  7.490000000e+00,  5.420000000e+00,  4.320000000e+00,  3.722000000e+00,  3.501000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  4.049057000e-02,  2.820000000e-01,  2.240000000e-01,  3.520000000e-01, /*DIAM*/  1.511936373e-01,  1.064869666e-01,  9.586328145e-02,  1.001016245e-01, /*EDIAM*/  4.131329595e-02,  7.819791063e-02,  5.415861631e-02,  7.970494738e-02, /*DMEAN*/  1.238812517e-01, /*EDMEAN*/  2.778764978e-02 },
        {369, /* SP */ 228, /*MAG*/  8.992000000e+00,  7.490000000e+00,  5.420000000e+00,  4.320000000e+00,  3.722000000e+00,  3.501000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  4.049057000e-02,  2.820000000e-01,  2.240000000e-01,  3.520000000e-01, /*DIAM*/  1.511936373e-01,  1.064869666e-01,  9.586328145e-02,  1.001016245e-01, /*EDIAM*/  4.131329595e-02,  7.819791063e-02,  5.415861631e-02,  7.970494738e-02, /*DMEAN*/  1.238812517e-01, /*EDMEAN*/  2.778764978e-02 },
        {370, /* SP */ 228, /*MAG*/  1.031100000e+01,  8.820000000e+00,  6.810000000e+00,  5.538000000e+00,  5.002000000e+00,  4.769000000e+00, /*EMAG*/  4.150614000e-02,  4.145793000e-02,  4.264692000e-02,  3.999999911e-02,  3.999999911e-02,  3.999999911e-02, /*DIAM*/ -1.216263668e-01, -1.285130369e-01, -1.580744655e-01, -1.518691821e-01, /*EDIAM*/  4.282325891e-02,  1.207601289e-02,  1.030886544e-02,  9.529886351e-03, /*DMEAN*/ -1.465834962e-01, /*EDMEAN*/  5.909627024e-03 },
        {371, /* SP */ 188, /*MAG*/  6.597000000e+00,  5.770000000e+00,  4.900000000e+00,  4.446000000e+00,  4.053000000e+00,  4.039000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.760000000e-01,  2.080000000e-01,  2.340000000e-01, /*DIAM*/ -1.335057703e-01, -1.016715805e-01, -1.177359104e-01, -1.285268150e-01, /*EDIAM*/  4.122057722e-02,  7.653068710e-02,  5.027600534e-02,  5.301342019e-02, /*DMEAN*/ -1.245989073e-01, /*EDMEAN*/  2.523255455e-02 },
        {372, /* SP */ 234, /*MAG*/  1.065500000e+01,  9.150000000e+00,  7.040000000e+00,  5.335000000e+00,  4.766000000e+00,  4.548000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  1.041201000e-01,  3.999999911e-02,  3.999999911e-02,  3.999999911e-02, /*DIAM*/ -1.066968931e-01, -4.744946242e-02, -7.296964139e-02, -8.176658015e-02, /*EDIAM*/  4.147319769e-02,  1.237951053e-02,  1.063766398e-02,  9.803324954e-03, /*DMEAN*/ -7.180641674e-02, /*EDMEAN*/  6.140955302e-03 },
        {373, /* SP */ 228, /*MAG*/  1.003300000e+01,  8.550000000e+00,  6.580000000e+00,  5.429000000e+00,  4.919000000e+00,  4.618000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  7.158911000e-02,  3.999999911e-02,  5.900000000e-02,  3.999999911e-02, /*DIAM*/ -7.258636602e-02, -1.190755368e-01, -1.491873059e-01, -1.247961890e-01, /*EDIAM*/  4.131329595e-02,  1.204698605e-02,  1.468139650e-02,  9.525582923e-03, /*DMEAN*/ -1.247211285e-01, /*EDMEAN*/  6.443957782e-03 },
        {374, /* SP */ 192, /*MAG*/  6.570000000e+00,  5.570000000e+00,  4.530000000e+00,  3.981000000e+00,  3.469000000e+00,  3.261000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.580000000e-01,  2.260000000e-01,  3.040000000e-01, /*DIAM*/  1.911284412e-02,  1.769908352e-02,  1.890924469e-02,  4.718864653e-02, /*EDIAM*/  4.122737356e-02,  7.156517024e-02,  5.461681921e-02,  6.884214544e-02, /*DMEAN*/  2.321481130e-02, /*EDMEAN*/  2.688767845e-02 },
        {375, /* SP */ 108, /*MAG*/  6.176000000e+00,  5.780000000e+00,  5.320000000e+00,  4.990000000e+00,  4.878000000e+00,  4.715000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  3.999999911e-02, /*DIAM*/ -3.307949156e-01, -3.548325431e-01, -3.577356031e-01, -3.432167851e-01, /*EDIAM*/  4.140392777e-02,  1.231589128e-02,  1.052923871e-02,  9.641527066e-03, /*DMEAN*/ -3.500434188e-01, /*EDMEAN*/  6.089618363e-03 },
        {376, /* SP */ 124, /*MAG*/  5.232000000e+00,  4.780000000e+00,  4.250000000e+00,  4.037000000e+00,  3.944000000e+00,  4.020000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.920000000e-01,  2.300000000e-01,  3.140000000e-01, /*DIAM*/ -1.038453142e-01, -1.515741392e-01, -1.645474710e-01, -2.009794164e-01, /*EDIAM*/  4.138687277e-02,  8.098844368e-02,  5.562479890e-02,  7.111142590e-02, /*DMEAN*/ -1.402643374e-01, /*EDMEAN*/  2.770288353e-02 },
        {377, /* SP */ 180, /*MAG*/  8.383000000e+00,  7.540000000e+00,  6.660000000e+00,  6.088000000e+00,  5.751000000e+00,  5.670000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  3.999999911e-02, /*DIAM*/ -4.816443425e-01, -4.326145791e-01, -4.612071067e-01, -4.601047976e-01, /*EDIAM*/  4.121196542e-02,  1.191927063e-02,  1.009084352e-02,  9.365552128e-03, /*DMEAN*/ -4.548751822e-01, /*EDMEAN*/  5.761461068e-03 },
        {378, /* SP */ 140, /*MAG*/  7.769000000e+00,  7.200000000e+00,  6.560000000e+00,  6.145000000e+00,  5.923000000e+00,  5.829000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  3.999999911e-02, /*DIAM*/ -5.359269729e-01, -5.314072400e-01, -5.360062298e-01, -5.369148625e-01, /*EDIAM*/  4.133861416e-02,  1.215451901e-02,  1.031452738e-02,  9.432684095e-03, /*DMEAN*/ -5.352432058e-01, /*EDMEAN*/  5.916130621e-03 },
        {379, /* SP */ 108, /*MAG*/  7.008000000e+00,  6.570000000e+00,  6.060000000e+00,  5.748000000e+00,  5.560000000e+00,  5.513000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  3.999999911e-02, /*DIAM*/ -4.627549175e-01, -5.039754025e-01, -4.896811304e-01, -5.030270064e-01, /*EDIAM*/  4.140392777e-02,  1.231589128e-02,  1.052923871e-02,  9.641527066e-03, /*DMEAN*/ -4.968216032e-01, /*EDMEAN*/  6.089618363e-03 },
        {380, /* SP */ 120, /*MAG*/  8.040000000e+00,  7.530000000e+00,  6.950000000e+00,  6.544000000e+00,  6.346000000e+00,  6.283000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  3.999999911e-02, /*DIAM*/ -6.144889535e-01, -6.384347435e-01, -6.324868128e-01, -6.433581120e-01, /*EDIAM*/  4.139308516e-02,  1.224382636e-02,  1.043788258e-02,  9.536736181e-03, /*DMEAN*/ -6.371466632e-01, /*EDMEAN*/  6.015198487e-03 },
        {381, /* SP */ 140, /*MAG*/  6.400000000e+00,  5.800000000e+00,  5.160000000e+00,  4.689000000e+00,  4.430000000e+00,  4.388000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  7.011419000e-02,  2.060000000e-01,  2.040000000e-01,  3.999999911e-02, /*DIAM*/ -2.367069684e-01, -2.359072356e-01, -2.335704277e-01, -2.476374859e-01, /*EDIAM*/  4.133861416e-02,  5.723802731e-02,  4.934864987e-02,  9.432684095e-03, /*DMEAN*/ -2.462136540e-01, /*EDMEAN*/  8.741320159e-03 },
        {382, /* SP */ 120, /*MAG*/  4.205000000e+00,  3.770000000e+00,  3.260000000e+00,  2.954000000e+00,  2.729000000e+00,  2.564000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.220000000e-01,  1.840000000e-01,  2.900000000e-01, /*DIAM*/  9.101105706e-02,  6.651169554e-02,  8.501514342e-02,  9.936452681e-02, /*EDIAM*/  4.139308516e-02,  6.166845435e-02,  4.456706472e-02,  6.568918332e-02, /*DMEAN*/  8.620307190e-02, /*EDMEAN*/  2.459997583e-02 },
        {383, /* SP */ 80, /*MAG*/  6.229000000e+00,  5.970000000e+00,  5.680000000e+00,  5.383000000e+00,  5.280000000e+00,  5.240000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  3.999999911e-02, /*DIAM*/ -4.846216743e-01, -4.838493064e-01, -4.786036834e-01, -4.890840979e-01, /*EDIAM*/  4.147421938e-02,  1.297603672e-02,  1.110789160e-02,  1.018301141e-02, /*DMEAN*/ -4.842073358e-01, /*EDMEAN*/  6.494929166e-03 },
        {384, /* SP */ 180, /*MAG*/  6.822000000e+00,  6.070000000e+00,  5.270000000e+00,  4.733000000e+00,  4.629000000e+00,  4.314000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  1.440000000e-01,  4.200000000e-02, /*DIAM*/ -2.440643389e-01, -1.704449323e-01, -2.511604109e-01, -1.919004141e-01, /*EDIAM*/  4.121196542e-02,  1.191927063e-02,  3.486438433e-02,  9.803601777e-03, /*DMEAN*/ -1.895301828e-01, /*EDMEAN*/  7.044253896e-03 },
        {385, /* SP */ 124, /*MAG*/  6.399000000e+00,  5.830000000e+00,  5.190000000e+00,  4.755000000e+00,  4.794000000e+00,  4.445000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.140000000e-01,  3.999999911e-02, /*DIAM*/ -2.413053163e-01, -2.696812838e-01, -3.262984461e-01, -2.695560597e-01, /*EDIAM*/  4.138687277e-02,  1.223116358e-02,  5.177519709e-02,  9.513108529e-03, /*DMEAN*/ -2.688655964e-01, /*EDMEAN*/  7.137613577e-03 },
        {386, /* SP */ 184, /*MAG*/  6.076000000e+00,  5.240000000e+00,  4.360000000e+00,  3.855000000e+00,  3.391000000e+00,  3.285000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.400000000e-01,  2.260000000e-01,  2.660000000e-01, /*DIAM*/ -2.497044945e-02,  1.506058456e-02,  1.650272764e-02,  2.350888766e-02, /*EDIAM*/  4.121507289e-02,  6.658026810e-02,  5.460527135e-02,  6.024191308e-02, /*DMEAN*/  2.946588784e-04, /*EDMEAN*/  2.595065457e-02 },
        {387, /* SP */ 160, /*MAG*/  6.990000000e+00,  6.270000000e+00,  5.500000000e+00,  5.386000000e+00,  4.678000000e+00,  4.601000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.680000000e-01,  3.999999911e-02,  3.999999911e-02, /*DIAM*/ -2.876058609e-01, -3.662392537e-01, -2.664781742e-01, -2.698082116e-01, /*EDIAM*/  4.124903710e-02,  7.431702007e-02,  1.013432383e-02,  9.339205043e-03, /*DMEAN*/ -2.700050056e-01, /*EDMEAN*/  6.616701732e-03 },
        {388, /* SP */ 176, /*MAG*/  7.237000000e+00,  6.420000000e+00,  5.570000000e+00,  5.023000000e+00,  4.637000000e+00,  4.491000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  3.999999911e-02, /*DIAM*/ -2.730189139e-01, -2.300705674e-01, -2.415505700e-01, -2.268727624e-01, /*EDIAM*/  4.121207570e-02,  1.190265538e-02,  1.007847081e-02,  9.345506325e-03, /*DMEAN*/ -2.348583284e-01, /*EDMEAN*/  5.752170286e-03 },
        {389, /* SP */ 128, /*MAG*/  6.231000000e+00,  5.720000000e+00,  5.140000000e+00,  4.715000000e+00,  4.642000000e+00,  4.506000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.980000000e-01,  1.180000000e-01,  3.999999911e-02, /*DIAM*/ -2.594794556e-01, -2.628325745e-01, -2.924574132e-01, -2.837611709e-01, /*EDIAM*/  4.137844895e-02,  8.264389662e-02,  2.872801589e-02,  9.492178293e-03, /*DMEAN*/ -2.827760396e-01, /*EDMEAN*/  8.589343889e-03 },
        {390, /* SP */ 148, /*MAG*/  6.902000000e+00,  6.200000000e+00,  5.370000000e+00,  4.909000000e+00,  4.579000000e+00,  4.417000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  7.017834000e-02,  3.999999911e-02,  7.600000000e-02,  3.999999911e-02, /*DIAM*/ -2.661974159e-01, -2.560851175e-01, -2.502177243e-01, -2.385859563e-01, /*EDIAM*/  4.130274461e-02,  1.208377251e-02,  1.865393048e-02,  9.391178095e-03, /*DMEAN*/ -2.472148063e-01, /*EDMEAN*/  6.643379146e-03 },
        {391, /* SP */ 156, /*MAG*/  6.609000000e+00,  5.970000000e+00,  5.270000000e+00,  5.260000000e+00,  4.595000000e+00,  4.405000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.940000000e-01,  7.600000000e-02,  3.999999911e-02, /*DIAM*/ -2.719482276e-01, -3.600896508e-01, -2.605283817e-01, -2.363554238e-01, /*EDIAM*/  4.126573137e-02,  8.150681838e-02,  1.861266685e-02,  9.353381524e-03, /*DMEAN*/ -2.441579661e-01, /*EDMEAN*/  7.990931083e-03 },
        {392, /* SP */ 124, /*MAG*/  5.617000000e+00,  5.130000000e+00,  4.570000000e+00,  4.127000000e+00,  3.942000000e+00,  3.868000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.220000000e-01,  1.920000000e-01,  2.460000000e-01, /*DIAM*/ -1.521453149e-01, -1.496098534e-01, -1.496291828e-01, -1.573881748e-01, /*EDIAM*/  4.138687277e-02,  6.166594149e-02,  4.648478945e-02,  5.574122129e-02, /*DMEAN*/ -1.520356785e-01, /*EDMEAN*/  2.423294339e-02 },
        {393, /* SP */ 136, /*MAG*/  6.436000000e+00,  5.860000000e+00,  5.190000000e+00,  4.998000000e+00,  4.688000000e+00,  4.458000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  7.006426000e-02,  2.180000000e-01,  2.260000000e-01,  3.999999911e-02, /*DIAM*/ -2.576160887e-01, -3.215308088e-01, -2.947455784e-01, -2.643523483e-01, /*EDIAM*/  4.135420953e-02,  6.055279169e-02,  5.464871393e-02,  9.452817440e-03, /*DMEAN*/ -2.659254476e-01, /*EDMEAN*/  8.796357658e-03 },
        {394, /* SP */ 132, /*MAG*/  5.580000000e+00,  5.040000000e+00,  4.430000000e+00,  4.341000000e+00,  3.947000000e+00,  4.008000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  3.060000000e-01,  2.840000000e-01,  2.840000000e-01, /*DIAM*/ -1.104230374e-01, -2.071690145e-01, -1.512714481e-01, -1.865036385e-01, /*EDIAM*/  4.136756490e-02,  8.485167189e-02,  6.861898136e-02,  6.432347116e-02, /*DMEAN*/ -1.448466022e-01, /*EDMEAN*/  2.867054230e-02 },
        {395, /* SP */ 150, /*MAG*/  6.544000000e+00,  5.860000000e+00,  5.120000000e+00,  4.593000000e+00,  4.045000000e+00,  4.297000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  3.520000000e-01,  3.480000000e-01,  3.999999911e-02, /*DIAM*/ -2.125881713e-01, -1.921007510e-01, -1.346912192e-01, -2.190284055e-01, /*EDIAM*/  4.129332637e-02,  9.754629733e-02,  8.402129804e-02,  9.381015195e-03, /*DMEAN*/ -2.174431449e-01, /*EDMEAN*/  8.856057585e-03 },
        {396, /* SP */ 140, /*MAG*/  5.999000000e+00,  5.380000000e+00,  4.680000000e+00,  4.160000000e+00,  3.905000000e+00,  3.911000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.660000000e-01,  2.460000000e-01,  2.340000000e-01, /*DIAM*/ -1.409269670e-01, -1.217375911e-01, -1.242396867e-01, -1.507396743e-01, /*EDIAM*/  4.133861416e-02,  7.379563885e-02,  5.945800532e-02,  5.301546666e-02, /*DMEAN*/ -1.374828589e-01, /*EDMEAN*/  2.616439564e-02 },
        {397, /* SP */ 180, /*MAG*/  7.199000000e+00,  6.440000000e+00,  5.640000000e+00,  4.969000000e+00,  4.637000000e+00,  4.515000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.280000000e-01,  3.999999911e-02,  3.999999911e-02, /*DIAM*/ -3.137243399e-01, -2.073556472e-01, -2.378296714e-01, -2.276595387e-01, /*EDIAM*/  4.121196542e-02,  6.326139329e-02,  1.009084352e-02,  9.365552128e-03, /*DMEAN*/ -2.362200818e-01, /*EDMEAN*/  6.590376972e-03 },
        {398, /* SP */ 120, /*MAG*/  5.430000000e+00,  4.990000000e+00,  4.480000000e+00,  4.203000000e+00,  4.059000000e+00,  3.944000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.580000000e-01,  2.360000000e-01,  3.100000000e-01, /*DIAM*/ -1.498889465e-01, -1.855150939e-01, -1.855218256e-01, -1.808398569e-01, /*EDIAM*/  4.139308516e-02,  7.160184888e-02,  5.707270890e-02,  7.021029968e-02, /*DMEAN*/ -1.685681544e-01, /*EDMEAN*/  2.733365008e-02 },
        {399, /* SP */ 146, /*MAG*/  6.633000000e+00,  5.990000000e+00,  5.380000000e+00,  5.091000000e+00,  4.724000000e+00,  4.426000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.120000000e-01,  1.940000000e-01,  3.999999911e-02, /*DIAM*/ -2.575450928e-01, -3.251614000e-01, -2.945699171e-01, -2.474505792e-01, /*EDIAM*/  4.131208422e-02,  5.888196355e-02,  4.693165280e-02,  9.401544313e-03, /*DMEAN*/ -2.514538866e-01, /*EDMEAN*/  8.704332224e-03 },
        {400, /* SP */ 152, /*MAG*/  6.911000000e+00,  6.250000000e+00,  5.640000000e+00,  4.993000000e+00,  4.695000000e+00,  4.651000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  3.999999911e-02, /*DIAM*/ -3.080554451e-01, -2.701909078e-01, -2.746704881e-01, -2.875118318e-01, /*EDIAM*/  4.128394110e-02,  1.204299113e-02,  1.020536860e-02,  9.371210334e-03, /*DMEAN*/ -2.802286992e-01, /*EDMEAN*/  5.837804293e-03 },
        {401, /* SP */ 150, /*MAG*/  6.349000000e+00,  5.660000000e+00,  4.920000000e+00,  4.309000000e+00,  3.908000000e+00,  3.999000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.820000000e-01,  2.480000000e-01,  3.999999911e-02, /*DIAM*/ -1.694881707e-01, -1.288507501e-01, -1.098896624e-01, -1.568532221e-01, /*EDIAM*/  4.129332637e-02,  7.820047111e-02,  5.992416328e-02,  9.381015195e-03, /*DMEAN*/ -1.562960756e-01, /*EDMEAN*/  8.787869313e-03 },
        {402, /* SP */ 140, /*MAG*/  6.547000000e+00,  5.960000000e+00,  5.300000000e+00,  4.793000000e+00,  4.598000000e+00,  4.559000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  3.999999911e-02, /*DIAM*/ -2.767669690e-01, -2.524072359e-01, -2.675003893e-01, -2.821265375e-01, /*EDIAM*/  4.133861416e-02,  1.215451901e-02,  1.031452738e-02,  9.432684095e-03, /*DMEAN*/ -2.699656765e-01, /*EDMEAN*/  5.916130621e-03 },
        {403, /* SP */ 150, /*MAG*/  6.116000000e+00,  5.450000000e+00,  4.760000000e+00,  4.655000000e+00,  4.234000000e+00,  3.911000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.960000000e-01,  2.700000000e-01,  3.999999911e-02, /*DIAM*/ -1.417481703e-01, -2.407436089e-01, -1.971970567e-01, -1.424590613e-01, /*EDIAM*/  4.129332637e-02,  8.206853521e-02,  6.522379621e-02,  9.381015195e-03, /*DMEAN*/ -1.445719534e-01, /*EDMEAN*/  8.808085476e-03 },
        {404, /* SP */ 128, /*MAG*/  6.136000000e+00,  5.580000000e+00,  4.950000000e+00,  4.871000000e+00,  4.599000000e+00,  4.306000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.500000000e-01,  1.900000000e-01,  3.999999911e-02, /*DIAM*/ -2.035794548e-01, -3.167611467e-01, -2.878581913e-01, -2.421845279e-01, /*EDIAM*/  4.137844895e-02,  6.938916004e-02,  4.599918126e-02,  9.492178293e-03, /*DMEAN*/ -2.425605832e-01, /*EDMEAN*/  8.808494914e-03 },
        {405, /* SP */ 88, /*MAG*/  4.690000000e+00,  4.490000000e+00,  4.270000000e+00,  4.372000000e+00,  4.204000000e+00,  4.064000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.980000000e-01,  2.440000000e-01,  3.999999911e-02, /*DIAM*/ -2.101110002e-01, -3.057960463e-01, -2.682399403e-01, -2.503041065e-01, /*EDIAM*/  4.144078776e-02,  8.271942746e-02,  5.908028406e-02,  9.996094871e-03, /*DMEAN*/ -2.486623641e-01, /*EDMEAN*/  9.343945916e-03 },
        {406, /* SP */ 180, /*MAG*/  3.059000000e+00,  2.040000000e+00,  1.040000000e+00,  3.700000000e-01, -2.190000000e-01, -2.740000000e-01, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  1.980000000e-01,  2.800000000e-01,  3.700000000e-01, /*DIAM*/  7.274756756e-01,  7.277247239e-01,  7.521781254e-01,  7.403623736e-01, /*EDIAM*/  4.121196542e-02,  5.498112280e-02,  6.761305196e-02,  8.375720561e-02, /*DMEAN*/  7.330894973e-01, /*EDMEAN*/  2.730646278e-02 },
        {407, /* SP */ 228, /*MAG*/  4.850000000e+00,  3.260000000e+00,  1.320000000e+00,  1.610000000e-01, -8.510000000e-01, -1.025000000e+00, /*EMAG*/  5.108816000e-02,  3.999999911e-02,  3.999999911e-02,  2.340000000e-01,  3.380000000e-01,  4.280000000e-01, /*DIAM*/  1.051753651e+00,  9.328351932e-01,  1.024610377e+00,  1.013079740e+00, /*EDIAM*/  4.577152473e-02,  6.494173216e-02,  8.162023641e-02,  9.689221061e-02, /*DMEAN*/  1.014045669e+00, /*EDMEAN*/  3.153907513e-02 },
        {408, /* SP */ 236, /*MAG*/  5.980000000e+00,  4.480000000e+00,  2.060000000e+00,  6.710000000e-01, -2.310000000e-01, -4.680000000e-01, /*EMAG*/  1.401749000e-01,  3.999999911e-02,  3.999999911e-02,  2.020000000e-01,  2.540000000e-01,  3.340000000e-01, /*DIAM*/  8.514199670e-01,  8.841171519e-01,  9.451162971e-01,  9.344330872e-01, /*EDIAM*/  9.308912498e-02,  5.623411358e-02,  6.147589256e-02,  7.569287785e-02, /*DMEAN*/  9.079709457e-01, /*EDMEAN*/  3.370974891e-02 },
        {409, /* SP */ 232, /*MAG*/  6.483000000e+00,  4.710000000e+00,  2.080000000e+00,  4.830000000e-01, -4.760000000e-01, -6.620000000e-01, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  1.880000000e-01,  1.920000000e-01,  1.640000000e-01, /*DIAM*/  9.218480050e-01,  9.550386740e-01,  1.003484711e+00,  9.766076254e-01, /*EDIAM*/  4.140373314e-02,  5.229214929e-02,  4.649990493e-02,  3.726689606e-02, /*DMEAN*/  9.629968556e-01, /*EDMEAN*/  2.118284359e-02 },
        {410, /* SP */ 100, /*MAG*/ -4.560000000e-01, -6.200000000e-01, -8.500000000e-01, -1.169000000e+00, -1.280000000e+00, -1.304000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  1.130000000e-01,  1.900000000e-01,  2.480000000e-01, /*DIAM*/  8.027307302e-01,  8.500159581e-01,  8.573381735e-01,  8.435342667e-01, /*EDIAM*/  4.141222888e-02,  3.178180073e-02,  4.605456211e-02,  5.623460019e-02, /*DMEAN*/  8.380936089e-01, /*EDMEAN*/  1.993308232e-02 },
        {411, /* SP */ 192, /*MAG*/  3.430000000e+00,  1.990000000e+00,  6.000000000e-01, -2.560000000e-01, -9.900000000e-01, -1.127000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  1.470000000e-01,  1.920000000e-01,  2.080000000e-01, /*DIAM*/  1.007912859e+00,  9.155473112e-01,  9.469637332e-01,  9.460207767e-01, /*EDIAM*/  4.122737356e-02,  4.095798103e-02,  4.643134399e-02,  4.714432536e-02, /*DMEAN*/  9.556823490e-01, /*EDMEAN*/  2.123585910e-02 },
        {412, /* SP */ 222, /*MAG*/  4.314000000e+00,  2.730000000e+00,  9.100000000e-01, -1.310000000e-01, -1.025000000e+00, -1.173000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  1.660000000e-01,  1.800000000e-01,  2.140000000e-01, /*DIAM*/  1.094806029e+00,  9.709817598e-01,  1.031663416e+00,  1.021713262e+00, /*EDIAM*/  4.125545403e-02,  4.618023288e-02,  4.355319174e-02,  4.850163712e-02, /*DMEAN*/  1.034567669e+00, /*EDMEAN*/  2.174621206e-02 },
        {413, /* SP */ 188, /*MAG*/  3.357000000e+00,  1.910000000e+00,  4.600000000e-01, -3.500000000e-01, -1.024000000e+00, -1.176000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  1.430000000e-01,  1.920000000e-01,  2.260000000e-01, /*DIAM*/  1.022894247e+00,  9.293998634e-01,  9.478594362e-01,  9.500790405e-01, /*EDIAM*/  4.122057722e-02,  3.984573016e-02,  4.642415040e-02,  5.120548204e-02, /*DMEAN*/  9.649666352e-01, /*EDMEAN*/  2.139304853e-02 },
        {414, /* SP */ 192, /*MAG*/  4.672000000e+00,  3.120000000e+00,  1.520000000e+00,  4.090000000e-01, -4.520000000e-01, -6.330000000e-01, /*EMAG*/  9.708244000e-02,  3.999999911e-02,  3.999999911e-02,  1.700000000e-01,  2.100000000e-01,  2.320000000e-01, /*DIAM*/  8.513528565e-01,  8.182526669e-01,  8.637808526e-01,  8.639331842e-01, /*EDIAM*/  6.861214434e-02,  4.728765299e-02,  5.076415291e-02,  5.256685938e-02, /*DMEAN*/  8.474315560e-01, /*EDMEAN*/  2.635525138e-02 },
        {415, /* SP */ 28, /*MAG*/  3.494000000e+00,  3.690000000e+00,  3.920000000e+00,  4.135000000e+00,  4.248000000e+00,  4.247000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  3.000000000e-01,  2.540000000e-01,  3.999999911e-02, /*DIAM*/ -5.067181549e-01, -4.993944034e-01, -4.883901498e-01, -4.896252090e-01, /*EDIAM*/  4.215539991e-02,  8.394383199e-02,  6.203115123e-02,  1.256116043e-02, /*DMEAN*/ -4.912308903e-01, /*EDMEAN*/  1.170097941e-02 },
        {416, /* SP */ 80, /*MAG*/  3.990000000e+00,  3.860000000e+00,  3.720000000e+00,  3.621000000e+00,  3.652000000e+00,  3.636000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.560000000e-01,  1.820000000e-01,  2.140000000e-01, /*DIAM*/ -1.426016692e-01, -1.581707301e-01, -1.728838345e-01, -1.815804437e-01, /*EDIAM*/  4.147421938e-02,  7.117960093e-02,  4.424994119e-02,  4.864764047e-02, /*DMEAN*/ -1.627488215e-01, /*EDMEAN*/  2.371828708e-02 },
        {417, /* SP */ 32, /*MAG*/  3.032000000e+00,  3.180000000e+00,  3.350000000e+00,  3.611000000e+00,  3.761000000e+00,  3.857000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.620000000e-01,  2.380000000e-01,  2.920000000e-01, /*DIAM*/ -3.564522421e-01, -3.686199753e-01, -3.722957749e-01, -3.943640222e-01, /*EDIAM*/  4.195292001e-02,  7.331176720e-02,  5.803974539e-02,  6.650396282e-02, /*DMEAN*/ -3.684473924e-01, /*EDMEAN*/  2.765515867e-02 },
        {418, /* SP */ 124, /*MAG*/  5.232000000e+00,  4.780000000e+00,  4.250000000e+00,  4.037000000e+00,  3.944000000e+00,  4.020000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.920000000e-01,  2.300000000e-01,  3.140000000e-01, /*DIAM*/ -1.038453142e-01, -1.515741392e-01, -1.645474710e-01, -2.009794164e-01, /*EDIAM*/  4.138687277e-02,  8.098844368e-02,  5.562479890e-02,  7.111142590e-02, /*DMEAN*/ -1.402643374e-01, /*EDMEAN*/  2.770288353e-02 },
        {419, /* SP */ 64, /*MAG*/  4.473000000e+00,  4.420000000e+00,  4.390000000e+00,  4.315000000e+00,  4.324000000e+00,  4.315000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.280000000e-01,  1.760000000e-01,  3.999999911e-02, /*DIAM*/ -3.470714780e-01, -3.398061626e-01, -3.444089872e-01, -3.521284193e-01, /*EDIAM*/  4.156156435e-02,  6.356798564e-02,  4.291720279e-02,  1.049289839e-02, /*DMEAN*/ -3.511163227e-01, /*EDMEAN*/  9.632915330e-03 },
        {420, /* SP */ 68, /*MAG*/  3.327000000e+00,  3.330000000e+00,  3.320000000e+00,  3.115000000e+00,  3.190000000e+00,  3.082000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.760000000e-01,  2.400000000e-01,  3.080000000e-01, /*DIAM*/ -1.508911245e-01, -8.184270058e-02, -1.064301312e-01, -9.263748351e-02, /*EDIAM*/  4.153908537e-02,  7.675646265e-02,  5.821953916e-02,  6.988616581e-02, /*DMEAN*/ -1.215427029e-01, /*EDMEAN*/  2.781780466e-02 },
        {421, /* SP */ 56, /*MAG*/  3.201000000e+00,  3.250000000e+00,  3.280000000e+00,  3.115000000e+00,  3.227000000e+00,  3.122000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.420000000e-01,  2.340000000e-01,  3.380000000e-01, /*DIAM*/ -2.051665444e-01, -1.200411321e-01, -1.495474034e-01, -1.340747952e-01, /*EDIAM*/  4.160487672e-02,  6.744801870e-02,  5.682133593e-02,  7.667789676e-02, /*DMEAN*/ -1.677099611e-01, /*EDMEAN*/  2.747441854e-02 },
        {422, /* SP */ 56, /*MAG*/  4.318000000e+00,  4.220000000e+00,  3.970000000e+00,  3.973000000e+00,  3.864000000e+00,  3.683000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.540000000e-01,  2.220000000e-01,  3.999999911e-02, /*DIAM*/ -3.080265460e-01, -2.830411345e-01, -2.632127748e-01, -2.355273515e-01, /*EDIAM*/  4.160487672e-02,  7.074757883e-02,  5.394625210e-02,  1.058130112e-02, /*DMEAN*/ -2.426004164e-01, /*EDMEAN*/  9.818165515e-03 },
        {423, /* SP */ 100, /*MAG*/  4.832000000e+00,  4.530000000e+00,  4.180000000e+00,  3.836000000e+00,  3.760000000e+00,  3.791000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.420000000e-01,  2.160000000e-01,  2.900000000e-01, /*DIAM*/ -1.417092839e-01, -1.398501281e-01, -1.461248764e-01, -1.740204930e-01, /*EDIAM*/  4.141222888e-02,  6.721943853e-02,  5.230079922e-02,  6.572121559e-02, /*DMEAN*/ -1.477655839e-01, /*EDMEAN*/  2.618877729e-02 },
        {424, /* SP */ 180, /*MAG*/  7.996000000e+00,  7.170000000e+00,  6.300000000e+00,  5.618000000e+00,  5.231000000e+00,  5.159000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  3.999999911e-02, /*DIAM*/ -4.181843415e-01, -3.309360062e-01, -3.510203346e-01, -3.541996866e-01, /*EDIAM*/  4.121196542e-02,  1.191927063e-02,  1.009084352e-02,  9.365552128e-03, /*DMEAN*/ -3.508034887e-01, /*EDMEAN*/  5.761461068e-03 },
        {425, /* SP */ 228, /*MAG*/  1.147300000e+01,  9.950000000e+00,  7.700000000e+00,  6.462000000e+00,  5.824000000e+00,  5.607000000e+00, /*EMAG*/  5.303187000e-02,  4.676943000e-02,  1.956716000e-01,  3.999999911e-02,  3.999999911e-02,  3.999999911e-02, /*DIAM*/ -3.277863698e-01, -2.974951823e-01, -3.097709658e-01, -3.117961918e-01, /*EDIAM*/  5.067327785e-02,  1.218987654e-02,  1.034745882e-02,  9.546848477e-03, /*DMEAN*/ -3.083223253e-01, /*EDMEAN*/  5.988025076e-03 },
        {426, /* SP */ 124, /*MAG*/  5.586000000e+00,  5.080000000e+00,  4.500000000e+00,  4.416000000e+00,  4.201000000e+00,  3.911000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.780000000e-01,  2.760000000e-01,  3.999999911e-02, /*DIAM*/ -1.303653146e-01, -2.334402118e-01, -2.141739308e-01, -1.684319706e-01, /*EDIAM*/  4.138687277e-02,  7.712183593e-02,  6.669904923e-02,  9.513108529e-03, /*DMEAN*/ -1.676256947e-01, /*EDMEAN*/  8.926407085e-03 },
        {427, /* SP */ 172, /*MAG*/  5.730000000e+00,  4.720000000e+00,  3.700000000e+00,  2.943000000e+00,  2.484000000e+00,  2.282000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  3.340000000e-01,  2.680000000e-01,  3.460000000e-01, /*DIAM*/  1.888860013e-01,  2.089110885e-01,  2.051443472e-01,  2.244351160e-01, /*EDIAM*/  4.121588091e-02,  9.254925920e-02,  6.471884658e-02,  7.832503065e-02, /*DMEAN*/  1.997211849e-01, /*EDMEAN*/  2.957611986e-02 },
        {428, /* SP */ 168, /*MAG*/  5.449000000e+00,  4.740000000e+00,  3.990000000e+00,  3.334000000e+00,  2.974000000e+00,  2.956000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.000000000e-01,  1.760000000e-01,  2.360000000e-01, /*DIAM*/  1.762214842e-03,  9.610512698e-02,  8.544380719e-02,  6.882049373e-02, /*EDIAM*/  4.122350083e-02,  5.553091771e-02,  4.256056871e-02,  5.344904608e-02, /*DMEAN*/  5.637231413e-02, /*EDMEAN*/  2.292455229e-02 },
        {429, /* SP */ 140, /*MAG*/  6.002000000e+00,  5.390000000e+00,  4.710000000e+00,  4.088000000e+00,  3.989000000e+00,  3.857000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.400000000e-01,  3.999999911e-02,  3.999999911e-02, /*DIAM*/ -1.472669671e-01, -1.010411622e-01, -1.440918271e-01, -1.382579222e-01, /*EDIAM*/  4.133861416e-02,  6.661764641e-02,  1.031452738e-02,  9.432684095e-03, /*DMEAN*/ -1.408599985e-01, /*EDMEAN*/  6.728822772e-03 },
        {430, /* SP */ 228, /*MAG*/  1.122200000e+01,  9.700000000e+00,  7.460000000e+00,  6.448000000e+00,  5.865000000e+00,  5.624000000e+00, /*EMAG*/  4.341659000e-02,  3.999999911e-02,  2.907594000e-01,  3.999999911e-02,  3.999999911e-02,  3.999999911e-02, /*DIAM*/ -2.784063691e-01, -3.128166111e-01, -3.299733008e-01, -3.222122504e-01, /*EDIAM*/  4.261857057e-02,  1.204698605e-02,  1.029906193e-02,  9.525582923e-03, /*DMEAN*/ -3.202560598e-01, /*EDMEAN*/  5.923025049e-03 },
        {431, /* SP */ 184, /*MAG*/  6.881000000e+00,  5.930000000e+00,  4.990000000e+00,  4.508000000e+00,  3.995000000e+00,  3.984000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  3.200000000e-01,  2.260000000e-01,  2.940000000e-01, /*DIAM*/ -9.167045044e-02, -1.126983459e-01, -1.007501924e-01, -1.165276108e-01, /*EDIAM*/  4.121507289e-02,  8.868525741e-02,  5.460527135e-02,  6.657271812e-02, /*DMEAN*/ -1.005063659e-01, /*EDMEAN*/  2.749554429e-02 },
        {432, /* SP */ 240, /*MAG*/  1.175700000e+01,  1.016000000e+01,  7.390000000e+00,  5.934000000e+00,  5.349000000e+00,  5.010000000e+00, /*EMAG*/  5.920883000e-02,  4.775652000e-02,  4.879227000e-02,  3.999999911e-02,  4.900000000e-02,  3.999999911e-02, /*DIAM*/ -1.649874921e-01, -1.390385849e-01, -1.558276773e-01, -1.479089807e-01, /*EDIAM*/  5.421895314e-02,  1.344516644e-02,  1.338424114e-02,  1.050936946e-02, /*DMEAN*/ -1.483310937e-01, /*EDMEAN*/  7.128439053e-03 },
        {433, /* SP */ 230, /*MAG*/  4.095000000e+00,  2.440000000e+00,  1.300000000e-01, -1.432000000e+00, -2.129000000e+00, -2.379000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  6.053098000e-02,  1.940000000e-01,  1.800000000e-01,  1.540000000e-01, /*DIAM*/  1.278622113e+00,  1.310928683e+00,  1.303784020e+00,  1.301667562e+00, /*EDIAM*/  4.135154739e-02,  5.392079872e-02,  4.359066989e-02,  3.498948922e-02, /*DMEAN*/  1.297360004e+00, /*EDMEAN*/  2.052092339e-02 },
        {434, /* SP */ 172, /*MAG*/  3.910000000e+00,  2.990000000e+00,  2.090000000e+00,  1.519000000e+00,  1.065000000e+00,  1.024000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.780000000e-01,  2.660000000e-01,  3.000000000e-01, /*DIAM*/  4.790860056e-01,  4.702146639e-01,  4.761171139e-01,  4.636321999e-01, /*EDIAM*/  4.121588091e-02,  7.706994324e-02,  6.423684567e-02,  6.792133830e-02, /*DMEAN*/  4.745344528e-01, /*EDMEAN*/  2.816343155e-02 },
        {435, /* SP */ 28, /*MAG*/  1.416000000e+00,  1.640000000e+00,  1.860000000e+00,  2.149000000e+00,  2.357000000e+00,  2.375000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.820000000e-01,  1.680000000e-01,  2.560000000e-01, /*DIAM*/ -1.140781490e-01, -1.071086832e-01, -1.167481210e-01, -1.199025758e-01, /*EDIAM*/  4.215539991e-02,  7.901896679e-02,  4.166170703e-02,  5.857814044e-02, /*DMEAN*/ -1.153717462e-01, /*EDMEAN*/  2.489066739e-02 },
        {436, /* SP */ 56, /*MAG*/  3.201000000e+00,  3.250000000e+00,  3.280000000e+00,  3.115000000e+00,  3.227000000e+00,  3.122000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.420000000e-01,  2.340000000e-01,  3.380000000e-01, /*DIAM*/ -2.051665444e-01, -1.200411321e-01, -1.495474034e-01, -1.340747952e-01, /*EDIAM*/  4.160487672e-02,  6.744801870e-02,  5.682133593e-02,  7.667789676e-02, /*DMEAN*/ -1.677099611e-01, /*EDMEAN*/  2.747441854e-02 },
        {437, /* SP */ 56, /*MAG*/  3.334000000e+00,  3.430000000e+00,  3.520000000e+00,  3.522000000e+00,  3.477000000e+00,  3.564000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  3.040000000e-01,  2.720000000e-01,  2.900000000e-01, /*DIAM*/ -2.703065454e-01, -2.188714907e-01, -2.024345638e-01, -2.293594682e-01, /*EDIAM*/  4.160487672e-02,  8.451418884e-02,  6.593723312e-02,  6.584894382e-02, /*DMEAN*/ -2.429902977e-01, /*EDMEAN*/  2.869337824e-02 },
        {438, /* SP */ 32, /*MAG*/  4.590000000e+00,  4.740000000e+00,  4.860000000e+00,  4.971000000e+00,  5.093000000e+00,  5.114000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  3.999999911e-02, /*DIAM*/ -6.696922467e-01, -6.252628363e-01, -6.292918877e-01, -6.378019821e-01, /*EDIAM*/  4.195292001e-02,  1.543866071e-02,  1.284890974e-02,  1.179233298e-02, /*DMEAN*/ -6.343223962e-01, /*EDMEAN*/  8.237714127e-03 },
        {439, /* SP */ 58, /*MAG*/  2.858000000e+00,  2.860000000e+00,  2.880000000e+00,  2.752000000e+00,  2.947000000e+00,  2.678000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  3.300000000e-01,  2.000000000e-01,  2.560000000e-01, /*DIAM*/ -9.045585345e-02, -4.340024983e-02, -9.233534520e-02, -3.818435890e-02, /*EDIAM*/  4.159416852e-02,  9.167621078e-02,  4.867517336e-02,  5.818222856e-02, /*DMEAN*/ -7.627551608e-02, /*EDMEAN*/  2.616505400e-02 },
        {440, /* SP */ 52, /*MAG*/  3.324000000e+00,  3.410000000e+00,  3.470000000e+00,  3.538000000e+00,  3.527000000e+00,  3.566000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.560000000e-01,  2.160000000e-01,  2.740000000e-01, /*DIAM*/ -2.758933306e-01, -2.381269121e-01, -2.275959606e-01, -2.425054306e-01, /*EDIAM*/  4.162711671e-02,  7.131181825e-02,  5.252068111e-02,  6.224870602e-02, /*DMEAN*/ -2.520198549e-01, /*EDMEAN*/  2.632059527e-02 },
        {441, /* SP */ 228, /*MAG*/  6.362000000e+00,  4.790000000e+00,  2.860000000e+00,  1.757000000e+00,  8.530000000e-01,  7.240000000e-01, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.280000000e-01,  1.660000000e-01,  2.140000000e-01, /*DIAM*/  7.345936460e-01,  6.085673312e-01,  6.766337181e-01,  6.575249905e-01, /*EDIAM*/  4.131329595e-02,  6.328558083e-02,  4.020807743e-02,  4.851428585e-02, /*DMEAN*/  6.818780537e-01, /*EDMEAN*/  2.256801258e-02 },
        {442, /* SP */ 186, /*MAG*/  4.774000000e+00,  3.560000000e+00,  2.430000000e+00,  1.694000000e+00,  1.097000000e+00,  1.025000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.640000000e-01,  2.100000000e-01,  2.660000000e-01, /*DIAM*/  5.466406272e-01,  4.872825914e-01,  5.023818472e-01,  4.930407451e-01, /*EDIAM*/  4.121758890e-02,  7.321229392e-02,  5.075450959e-02,  6.024418134e-02, /*DMEAN*/  5.169260764e-01, /*EDMEAN*/  2.584383260e-02 },
        {443, /* SP */ 180, /*MAG*/  4.665000000e+00,  3.600000000e+00,  2.550000000e+00,  1.896000000e+00,  1.401000000e+00,  1.289000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  3.480000000e-01,  2.760000000e-01,  3.000000000e-01, /*DIAM*/  4.439956713e-01,  4.251354337e-01,  4.257034124e-01,  4.276835368e-01, /*EDIAM*/  4.121196542e-02,  9.642255900e-02,  6.664900406e-02,  6.792585353e-02, /*DMEAN*/  4.354809494e-01, /*EDMEAN*/  2.918081647e-02 },
        {444, /* SP */ 220, /*MAG*/  6.341000000e+00,  4.720000000e+00,  2.820000000e+00,  1.748000000e+00,  8.650000000e-01,  6.770000000e-01, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.780000000e-01,  1.700000000e-01,  1.900000000e-01, /*DIAM*/  7.026640811e-01,  6.025749642e-01,  6.537525559e-01,  6.518683250e-01, /*EDIAM*/  4.124821477e-02,  7.708844697e-02,  4.114805030e-02,  4.308112484e-02, /*DMEAN*/  6.642440553e-01, /*EDMEAN*/  2.256599478e-02 },
        {445, /* SP */ 188, /*MAG*/  5.671000000e+00,  4.410000000e+00,  3.160000000e+00,  2.377000000e+00,  1.821000000e+00,  1.681000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  3.120000000e-01,  1.900000000e-01,  2.540000000e-01, /*DIAM*/  4.075742377e-01,  3.665694979e-01,  3.646298556e-01,  3.692980100e-01, /*EDIAM*/  4.122057722e-02,  8.647909303e-02,  4.594276375e-02,  5.753389308e-02, /*DMEAN*/  3.827954602e-01, /*EDMEAN*/  2.534660918e-02 },
        {446, /* SP */ 172, /*MAG*/  4.502000000e+00,  3.570000000e+00,  2.670000000e+00,  2.051000000e+00,  1.642000000e+00,  1.527000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  3.180000000e-01,  2.060000000e-01,  2.620000000e-01, /*DIAM*/  3.705260040e-01,  3.675003766e-01,  3.608408476e-01,  3.650555561e-01, /*EDIAM*/  4.121588091e-02,  8.812585555e-02,  4.978122727e-02,  5.932832436e-02, /*DMEAN*/  3.663913423e-01, /*EDMEAN*/  2.619106522e-02 },
        {447, /* SP */ 188, /*MAG*/  5.642000000e+00,  4.390000000e+00,  3.120000000e+00,  2.199000000e+00,  1.553000000e+00,  1.447000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.820000000e-01,  2.680000000e-01,  3.080000000e-01, /*DIAM*/  4.059942377e-01,  4.143016414e-01,  4.284586503e-01,  4.217213684e-01, /*EDIAM*/  4.122057722e-02,  7.818847391e-02,  6.472902191e-02,  6.974245765e-02, /*DMEAN*/  4.142733398e-01, /*EDMEAN*/  2.840169462e-02 },
        {448, /* SP */ 176, /*MAG*/  4.088000000e+00,  3.110000000e+00,  2.150000000e+00,  1.415000000e+00,  7.580000000e-01,  6.970000000e-01, /*EMAG*/  4.909175000e-02,  3.999999911e-02,  3.999999911e-02,  2.340000000e-01,  1.600000000e-01,  1.900000000e-01, /*DIAM*/  4.888010974e-01,  5.144115865e-01,  5.577179244e-01,  5.446454973e-01, /*EDIAM*/  4.483078376e-02,  6.491511309e-02,  3.870861305e-02,  4.305564365e-02, /*DMEAN*/  5.304820342e-01, /*EDMEAN*/  2.225747080e-02 },
        {449, /* SP */ 200, /*MAG*/  5.861000000e+00,  4.320000000e+00,  2.690000000e+00,  1.449000000e+00,  6.100000000e-01,  5.860000000e-01, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.680000000e-01,  1.900000000e-01,  2.040000000e-01, /*DIAM*/  6.233826901e-01,  6.339190665e-01,  6.664094798e-01,  6.303328924e-01, /*EDIAM*/  4.123976885e-02,  7.433904802e-02,  4.596300727e-02,  4.625061248e-02, /*DMEAN*/  6.381711598e-01, /*EDMEAN*/  2.370430458e-02 },
        {450, /* SP */ 190, /*MAG*/  5.213000000e+00,  3.900000000e+00,  2.610000000e+00,  1.680000000e+00,  9.530000000e-01,  8.740000000e-01, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.920000000e-01,  2.400000000e-01,  2.600000000e-01, /*DIAM*/  5.441861531e-01,  5.233598908e-01,  5.549470773e-01,  5.409310594e-01, /*EDIAM*/  4.122389129e-02,  8.095479457e-02,  5.798577935e-02,  5.889263353e-02, /*DMEAN*/  5.434446098e-01, /*EDMEAN*/  2.696164624e-02 },
        {451, /* SP */ 228, /*MAG*/  6.970000000e+00,  5.360000000e+00,  3.420000000e+00,  2.041000000e+00,  1.105000000e+00,  1.090000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  4.031129000e-02,  2.560000000e-01,  1.660000000e-01,  1.860000000e-01, /*DIAM*/  6.441536447e-01,  5.737280450e-01,  6.393496709e-01,  5.896855734e-01, /*EDIAM*/  4.131329595e-02,  7.101611037e-02,  4.020807743e-02,  4.219217598e-02, /*DMEAN*/  6.202514604e-01, /*EDMEAN*/  2.209787580e-02 },
        {452, /* SP */ 188, /*MAG*/  5.102000000e+00,  3.880000000e+00,  2.750000000e+00,  1.974000000e+00,  1.327000000e+00,  1.364000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.380000000e-01,  1.740000000e-01,  2.240000000e-01, /*DIAM*/  4.893942389e-01,  4.374177132e-01,  4.619450321e-01,  4.271009306e-01, /*EDIAM*/  4.122057722e-02,  6.603478707e-02,  4.209258029e-02,  5.075352291e-02, /*DMEAN*/  4.606168730e-01, /*EDMEAN*/  2.325389247e-02 },
        {453, /* SP */ 184, /*MAG*/  4.144000000e+00,  3.000000000e+00,  1.910000000e+00,  1.030000000e+00,  3.760000000e-01,  4.290000000e-01, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  1.800000000e-01,  1.700000000e-01,  1.980000000e-01, /*DIAM*/  6.139895601e-01,  6.249802365e-01,  6.514677176e-01,  6.108957577e-01, /*EDIAM*/  4.121507289e-02,  5.002351234e-02,  4.112310640e-02,  4.487293174e-02, /*DMEAN*/  6.258732627e-01, /*EDMEAN*/  2.141081688e-02 },
        {454, /* SP */ 220, /*MAG*/  5.433000000e+00,  3.820000000e+00,  2.030000000e+00,  8.950000000e-01, -4.900000000e-02, -1.140000000e-01, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.120000000e-01,  1.960000000e-01,  1.740000000e-01, /*DIAM*/  8.777040837e-01,  7.695660381e-01,  8.371299905e-01,  8.072040937e-01, /*EDIAM*/  4.124821477e-02,  5.886468029e-02,  4.740238146e-02,  3.946879886e-02, /*DMEAN*/  8.300623899e-01, /*EDMEAN*/  2.208266016e-02 },
        {455, /* SP */ 224, /*MAG*/  5.541000000e+00,  4.040000000e+00,  2.250000000e+00,  1.176000000e+00,  3.250000000e-01,  1.570000000e-01, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.240000000e-01,  2.140000000e-01,  2.600000000e-01, /*DIAM*/  7.997255660e-01,  7.107168245e-01,  7.642066267e-01,  7.587721881e-01, /*EDIAM*/  4.126750894e-02,  6.217267618e-02,  5.173727108e-02,  5.889774034e-02, /*DMEAN*/  7.679092292e-01, /*EDMEAN*/  2.520772210e-02 },
        {456, /* SP */ 140, /*MAG*/  4.828000000e+00,  4.240000000e+00,  3.570000000e+00,  3.213000000e+00,  2.905000000e+00,  2.848000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.180000000e-01,  1.980000000e-01,  3.100000000e-01, /*DIAM*/  6.785303611e-02,  5.284276868e-02,  6.998599710e-02,  5.983697122e-02, /*EDIAM*/  4.133861416e-02,  6.054737921e-02,  4.790524751e-02,  7.019624188e-02, /*DMEAN*/  6.463811265e-02, /*EDMEAN*/  2.528427803e-02 },
        {457, /* SP */ 172, /*MAG*/  3.784000000e+00,  2.850000000e+00,  2.020000000e+00,  1.248000000e+00,  7.330000000e-01,  6.640000000e-01, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.760000000e-01,  2.440000000e-01,  2.780000000e-01, /*DIAM*/  5.157660061e-01,  5.344735934e-01,  5.504361811e-01,  5.414132229e-01, /*EDIAM*/  4.121588091e-02,  7.651726541e-02,  5.893536422e-02,  6.294625154e-02, /*DMEAN*/  5.307954676e-01, /*EDMEAN*/  2.722721514e-02 },
        {458, /* SP */ 200, /*MAG*/  6.282000000e+00,  4.800000000e+00,  3.250000000e+00,  2.372000000e+00,  1.631000000e+00,  1.493000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.720000000e-01,  1.820000000e-01,  1.780000000e-01, /*DIAM*/  4.908026882e-01,  4.153029918e-01,  4.398958577e-01,  4.377124516e-01, /*EDIAM*/  4.123976885e-02,  7.544383589e-02,  4.403858735e-02,  4.038051972e-02, /*DMEAN*/  4.531194695e-01, /*EDMEAN*/  2.253004838e-02 },
        {459, /* SP */ 230, /*MAG*/  6.286000000e+00,  4.680000000e+00,  2.620000000e+00,  1.455000000e+00,  6.200000000e-01,  4.270000000e-01, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.680000000e-01,  2.340000000e-01,  2.820000000e-01, /*DIAM*/  8.002421056e-01,  6.838483169e-01,  7.329902368e-01,  7.255945610e-01, /*EDIAM*/  4.135154739e-02,  7.434114940e-02,  5.657957351e-02,  6.388829915e-02, /*DMEAN*/  7.547155776e-01, /*EDMEAN*/  2.698056173e-02 },
        {460, /* SP */ 202, /*MAG*/  5.570000000e+00,  4.050000000e+00,  2.450000000e+00,  1.218000000e+00,  4.380000000e-01,  4.350000000e-01, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.640000000e-01,  1.800000000e-01,  1.820000000e-01, /*DIAM*/  6.709960806e-01,  6.797757539e-01,  6.993982570e-01,  6.602574786e-01, /*EDIAM*/  4.124177049e-02,  7.323617043e-02,  4.355984099e-02,  4.128494080e-02, /*DMEAN*/  6.764563844e-01, /*EDMEAN*/  2.255063423e-02 },
        {461, /* SP */ 232, /*MAG*/  6.152000000e+00,  4.580000000e+00,  2.230000000e+00,  8.560000000e-01, -5.400000000e-02, -2.400000000e-01, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.300000000e-01,  1.520000000e-01,  1.880000000e-01, /*DIAM*/  8.232280035e-01,  8.418154580e-01,  8.963173945e-01,  8.777025145e-01, /*EDIAM*/  4.140373314e-02,  6.387049264e-02,  3.689742495e-02,  4.267752776e-02, /*DMEAN*/  8.642441779e-01, /*EDMEAN*/  2.129408332e-02 },
        {462, /* SP */ 192, /*MAG*/  4.868000000e+00,  3.570000000e+00,  2.350000000e+00,  1.501000000e+00,  7.620000000e-01,  7.560000000e-01, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.640000000e-01,  1.480000000e-01,  1.720000000e-01, /*DIAM*/  6.038728528e-01,  5.505562343e-01,  5.894695645e-01,  5.614587272e-01, /*EDIAM*/  4.122737356e-02,  7.322238145e-02,  3.584981809e-02,  3.901476137e-02, /*DMEAN*/  5.820121119e-01, /*EDMEAN*/  2.081931247e-02 },
        {463, /* SP */ 232, /*MAG*/  6.472000000e+00,  4.800000000e+00,  2.670000000e+00,  1.477000000e+00,  5.360000000e-01,  3.810000000e-01, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.760000000e-01,  1.760000000e-01,  2.020000000e-01, /*DIAM*/  8.412280038e-01,  6.868243843e-01,  7.630566921e-01,  7.429652862e-01, /*EDIAM*/  4.140373314e-02,  7.656775998e-02,  4.265634987e-02,  4.583591694e-02, /*DMEAN*/  7.766148536e-01, /*EDMEAN*/  2.324362257e-02 },
        {464, /* SP */ 172, /*MAG*/  4.446000000e+00,  3.490000000e+00,  2.600000000e+00,  1.795000000e+00,  1.268000000e+00,  1.223000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  3.140000000e-01,  1.180000000e-01,  1.660000000e-01, /*DIAM*/  4.014060044e-01,  4.322146633e-01,  4.477669189e-01,  4.317416884e-01, /*EDIAM*/  4.121588091e-02,  8.702008678e-02,  2.861458696e-02,  3.763090451e-02, /*DMEAN*/  4.319252429e-01, /*EDMEAN*/  1.900005920e-02 },
        {465, /* SP */ 196, /*MAG*/  6.306000000e+00,  4.800000000e+00,  3.260000000e+00,  2.256000000e+00,  1.512000000e+00,  1.339000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.880000000e-01,  1.780000000e-01,  2.340000000e-01, /*DIAM*/  4.948001859e-01,  4.418550700e-01,  4.637054594e-01,  4.670682227e-01, /*EDIAM*/  4.123418936e-02,  7.985876155e-02,  4.307029638e-02,  5.302369806e-02, /*DMEAN*/  4.736426506e-01, /*EDMEAN*/  2.420508997e-02 },
        {466, /* SP */ 196, /*MAG*/  6.389000000e+00,  5.020000000e+00,  3.680000000e+00,  2.876000000e+00,  2.091000000e+00,  1.939000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.300000000e-01,  1.940000000e-01,  2.700000000e-01, /*DIAM*/  3.658601840e-01,  2.871407820e-01,  3.330984535e-01,  3.370828193e-01, /*EDIAM*/  4.123418936e-02,  6.384153072e-02,  4.691963506e-02,  6.115972228e-02, /*DMEAN*/  3.390010488e-01, /*EDMEAN*/  2.481411359e-02 },
        {467, /* SP */ 188, /*MAG*/  4.456000000e+00,  3.290000000e+00,  2.220000000e+00,  1.293000000e+00,  7.240000000e-01,  6.710000000e-01, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.200000000e-01,  1.460000000e-01,  2.000000000e-01, /*DIAM*/  5.726742402e-01,  5.806052153e-01,  5.830812207e-01,  5.684075020e-01, /*EDIAM*/  4.122057722e-02,  6.106568221e-02,  3.535986927e-02,  4.533094034e-02, /*DMEAN*/  5.765571018e-01, /*EDMEAN*/  2.110463901e-02 },
        {468, /* SP */ 188, /*MAG*/  3.797000000e+00,  2.630000000e+00,  1.540000000e+00,  8.310000000e-01,  2.890000000e-01,  1.500000000e-01, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.100000000e-01,  2.440000000e-01,  2.980000000e-01, /*DIAM*/  7.052942422e-01,  6.578016451e-01,  6.608010662e-01,  6.689549488e-01, /*EDIAM*/  4.122057722e-02,  5.830602992e-02,  5.894653790e-02,  6.748132796e-02, /*DMEAN*/  6.804263447e-01, /*EDMEAN*/  2.623452368e-02 },
        {469, /* SP */ 188, /*MAG*/  5.371000000e+00,  4.140000000e+00,  2.970000000e+00,  2.104000000e+00,  1.488000000e+00,  1.349000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.600000000e-01,  1.600000000e-01,  1.880000000e-01, /*DIAM*/  4.429742382e-01,  4.213998558e-01,  4.338283002e-01,  4.373272081e-01, /*EDIAM*/  4.122057722e-02,  7.211058090e-02,  3.872526035e-02,  4.262042011e-02, /*DMEAN*/  4.363369828e-01, /*EDMEAN*/  2.189883768e-02 },
        {470, /* SP */ 172, /*MAG*/  3.640000000e+00,  2.730000000e+00,  1.890000000e+00,  1.081000000e+00,  5.840000000e-01,  5.790000000e-01, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.300000000e-01,  1.520000000e-01,  2.080000000e-01, /*DIAM*/  5.248860063e-01,  5.714825225e-01,  5.814322905e-01,  5.574935151e-01, /*EDIAM*/  4.121588091e-02,  6.380995116e-02,  3.678400978e-02,  4.712066766e-02, /*DMEAN*/  5.580101868e-01, /*EDMEAN*/  2.172866824e-02 },
        {471, /* SP */ 170, /*MAG*/  4.396000000e+00,  3.480000000e+00,  2.590000000e+00,  1.768000000e+00,  1.366000000e+00,  1.333000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  3.040000000e-01,  1.920000000e-01,  2.200000000e-01, /*DIAM*/  3.802127545e-01,  4.358485630e-01,  4.225226513e-01,  4.047449260e-01, /*EDIAM*/  4.121922010e-02,  8.425632516e-02,  4.641071796e-02,  4.983235186e-02, /*DMEAN*/  4.034883951e-01, /*EDMEAN*/  2.455148321e-02 },
        {472, /* SP */ 188, /*MAG*/  3.928000000e+00,  2.760000000e+00,  1.660000000e+00,  1.184000000e+00,  4.670000000e-01,  4.370000000e-01, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.360000000e-01,  1.700000000e-01,  1.800000000e-01, /*DIAM*/  6.799142418e-01,  5.700784295e-01,  6.232212991e-01,  6.074294004e-01, /*EDIAM*/  4.122057722e-02,  6.548256646e-02,  4.113031968e-02,  4.081376174e-02, /*DMEAN*/  6.295116579e-01, /*EDMEAN*/  2.181049492e-02 },
        {473, /* SP */ 176, /*MAG*/  4.307000000e+00,  3.320000000e+00,  2.370000000e+00,  1.732000000e+00,  1.289000000e+00,  1.225000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.900000000e-01,  2.840000000e-01,  3.060000000e-01, /*DIAM*/  4.523810969e-01,  4.427955140e-01,  4.382782339e-01,  4.306892912e-01, /*EDIAM*/  4.121207570e-02,  8.038672421e-02,  6.857530659e-02,  6.927997521e-02, /*DMEAN*/  4.446892182e-01, /*EDMEAN*/  2.880398027e-02 },
        {474, /* SP */ 186, /*MAG*/  5.999000000e+00,  4.820000000e+00,  3.660000000e+00,  2.949000000e+00,  2.197000000e+00,  2.085000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.340000000e-01,  1.960000000e-01,  2.160000000e-01, /*DIAM*/  2.729406231e-01,  2.366665162e-01,  2.889810659e-01,  2.862962165e-01, /*EDIAM*/  4.121758890e-02,  6.492674065e-02,  4.738370932e-02,  4.894289006e-02, /*DMEAN*/  2.751766384e-01, /*EDMEAN*/  2.382205470e-02 },
        {475, /* SP */ 180, /*MAG*/  5.971000000e+00,  4.820000000e+00,  3.720000000e+00,  3.260000000e+00,  2.547000000e+00,  2.321000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.860000000e-01,  2.260000000e-01,  2.860000000e-01, /*DIAM*/  2.533156685e-01,  1.412782866e-01,  1.995555491e-01,  2.262236798e-01, /*EDIAM*/  4.121196542e-02,  7.928373753e-02,  5.460118552e-02,  6.476005635e-02, /*DMEAN*/  2.212091155e-01, /*EDMEAN*/  2.699732230e-02 },
        {476, /* SP */ 184, /*MAG*/  5.099000000e+00,  4.020000000e+00,  2.940000000e+00,  2.245000000e+00,  1.756000000e+00,  1.636000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.860000000e-01,  2.780000000e-01,  3.480000000e-01, /*DIAM*/  3.696895564e-01,  3.670070184e-01,  3.606194643e-01,  3.645818854e-01, /*EDIAM*/  4.121507289e-02,  7.928805486e-02,  6.713434792e-02,  7.878438042e-02, /*DMEAN*/  3.668143553e-01, /*EDMEAN*/  2.922587611e-02 },
        {477, /* SP */ 188, /*MAG*/  6.156000000e+00,  5.000000000e+00,  3.880000000e+00,  2.886000000e+00,  2.392000000e+00,  2.145000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  3.140000000e-01,  2.520000000e-01,  2.500000000e-01, /*DIAM*/  2.244742350e-01,  2.709891393e-01,  2.512135115e-01,  2.798089576e-01, /*EDIAM*/  4.122057722e-02,  8.703188878e-02,  6.087387628e-02,  5.662973365e-02, /*DMEAN*/  2.478664731e-01, /*EDMEAN*/  2.723803119e-02 },
        {478, /* SP */ 176, /*MAG*/  4.750000000e+00,  3.800000000e+00,  2.950000000e+00,  2.319000000e+00,  1.823000000e+00,  1.757000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.660000000e-01,  1.740000000e-01,  1.860000000e-01, /*DIAM*/  3.334410951e-01,  3.171794407e-01,  3.292509950e-01,  3.229228662e-01, /*EDIAM*/  4.121207570e-02,  7.375457400e-02,  4.207726525e-02,  4.215188575e-02, /*DMEAN*/  3.275673688e-01, /*EDMEAN*/  2.247004753e-02 },
        {479, /* SP */ 220, /*MAG*/  5.942000000e+00,  4.440000000e+00,  2.760000000e+00,  1.714000000e+00,  8.780000000e-01,  7.110000000e-01, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.980000000e-01,  1.700000000e-01,  2.120000000e-01, /*DIAM*/  6.848840808e-01,  5.904856783e-01,  6.390677308e-01,  6.368172299e-01, /*EDIAM*/  4.124821477e-02,  8.261464553e-02,  4.114805030e-02,  4.805006684e-02, /*DMEAN*/  6.500610312e-01, /*EDMEAN*/  2.337720631e-02 },
        {480, /* SP */ 184, /*MAG*/  5.188000000e+00,  4.080000000e+00,  3.030000000e+00,  2.372000000e+00,  1.931000000e+00,  1.752000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.700000000e-01,  2.000000000e-01,  2.100000000e-01, /*DIAM*/  3.756695565e-01,  3.364623751e-01,  3.208762730e-01,  3.399103522e-01, /*EDIAM*/  4.121507289e-02,  7.486701643e-02,  4.834378099e-02,  4.758426518e-02, /*DMEAN*/  3.476773567e-01, /*EDMEAN*/  2.423275313e-02 },
        {481, /* SP */ 184, /*MAG*/  5.658000000e+00,  4.550000000e+00,  3.480000000e+00,  2.462000000e+00,  2.002000000e+00,  1.722000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.080000000e-01,  1.520000000e-01,  2.040000000e-01, /*DIAM*/  2.816695551e-01,  3.476409467e-01,  3.231330824e-01,  3.590490386e-01, /*EDIAM*/  4.121507289e-02,  5.774639739e-02,  3.679384776e-02,  4.622853984e-02, /*DMEAN*/  3.223590291e-01, /*EDMEAN*/  2.133690037e-02 },
        {482, /* SP */ 174, /*MAG*/  5.435000000e+00,  4.420000000e+00,  3.390000000e+00,  2.567000000e+00,  2.003000000e+00,  1.880000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.320000000e-01,  1.700000000e-01,  2.180000000e-01, /*DIAM*/  2.506918967e-01,  2.930379744e-01,  3.100679761e-01,  3.094184438e-01, /*EDIAM*/  4.121350068e-02,  6.436220559e-02,  4.111413142e-02,  4.938194668e-02, /*DMEAN*/  2.880692615e-01, /*EDMEAN*/  2.285276637e-02 },
        {483, /* SP */ 224, /*MAG*/  6.099000000e+00,  4.540000000e+00,  2.750000000e+00,  1.584000000e+00,  7.730000000e-01,  6.080000000e-01, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.480000000e-01,  2.780000000e-01,  3.220000000e-01, /*DIAM*/  7.356855651e-01,  6.361811091e-01,  6.767513725e-01,  6.698597780e-01, /*EDIAM*/  4.126750894e-02,  6.879886249e-02,  6.715148608e-02,  7.291431182e-02, /*DMEAN*/  6.971724451e-01, /*EDMEAN*/  2.822373615e-02 },
        {484, /* SP */ 228, /*MAG*/  5.765000000e+00,  4.220000000e+00,  2.330000000e+00,  1.088000000e+00,  1.690000000e-01, -9.900000000e-02, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.340000000e-01,  2.860000000e-01,  3.500000000e-01, /*DIAM*/  8.318536475e-01,  7.499691190e-01,  8.181356658e-01,  8.287731683e-01, /*EDIAM*/  4.131329595e-02,  6.494173216e-02,  6.908990434e-02,  7.925270849e-02, /*DMEAN*/  8.124773355e-01, /*EDMEAN*/  2.840038256e-02 },
        {485, /* SP */ 176, /*MAG*/  4.616000000e+00,  3.700000000e+00,  2.730000000e+00,  2.021000000e+00,  1.486000000e+00,  1.393000000e+00, /*EMAG*/  3.999999911e-02,  3.999999911e-02,  3.999999911e-02,  2.800000000e-01,  2.380000000e-01,  2.540000000e-01, /*DIAM*/  3.323610951e-01,  3.919830133e-01,  4.064260934e-01,  4.026600937e-01, /*EDIAM*/  4.121207570e-02,  7.762310576e-02,  5.748993944e-02,  5.752155073e-02, /*DMEAN*/  3.715098734e-01, /*EDMEAN*/  2.662125509e-02 },
    };

    logInfo("check %d diameters:", NS);


    /* 3 diameters are required: */
    static const mcsUINT32 nbRequiredDiameters = 3;

    static const mcsDOUBLE DELTA_TH = 0.2; /* check within error bar instead ? */
    static const mcsDOUBLE DELTA_SIG = 1; /* 1 sigma */

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

    alxDATA weightedMeanDiam, maxResidualsDiam, chi2Diam, maxCorrelations;

    mcsDOUBLE delta, diam, err;
    mcsUINT32 NB = 0;
    mcsUINT32 bad = 0;

    for (i = 0; i < NS; i++)
    {
        spTypeIndex = datas[i][1] + 20.0; /* 201608: +20 to shift O5 from 0 to 20 */

        mags[alxB_BAND].isSet = mcsTRUE;
        mags[alxB_BAND].confIndex = alxCONFIDENCE_HIGH;
        mags[alxB_BAND].value = datas[i][2];
        mags[alxB_BAND].error = datas[i][8];

        mags[alxV_BAND].isSet = mcsTRUE;
        mags[alxV_BAND].confIndex = alxCONFIDENCE_HIGH;
        mags[alxV_BAND].value = datas[i][2 + 1];
        mags[alxV_BAND].error = datas[i][8 + 1];

        mags[alxJ_BAND].isSet = mcsTRUE;
        mags[alxJ_BAND].confIndex = alxCONFIDENCE_HIGH;
        mags[alxJ_BAND].value = datas[i][2 + 3];
        mags[alxJ_BAND].error = datas[i][8 + 3];

        mags[alxH_BAND].isSet = mcsTRUE;
        mags[alxH_BAND].confIndex = alxCONFIDENCE_HIGH;
        mags[alxH_BAND].value = datas[i][2 + 4];
        mags[alxH_BAND].error = datas[i][8 + 4];

        mags[alxK_BAND].isSet = mcsTRUE;
        mags[alxK_BAND].confIndex = alxCONFIDENCE_HIGH;
        mags[alxK_BAND].value = datas[i][2 + 5];
        mags[alxK_BAND].error = datas[i][8 + 5];

        bad = 0;

        if (alxComputeAngularDiameters   ("(SP)   ", mags, spTypeIndex, diameters, diametersCov) == mcsFAILURE)
        {
            logInfo("alxComputeAngularDiameters : fail");
        }

        for (j = 0; j < alxNB_DIAMS; j++)
        {
            /* diam (log10): */
            diam = log10(diameters[j].value);
            /* diam V-B at 14 (skipped) */
            delta = fabs(datas[i][15 + j] - diam);

            if (delta > DELTA_TH)
            {
                logInfo("diam: %.9lf %.9lf", datas[i][15 + j], diam);
                bad = 1;
            }

            /* rel error: */
            err = relError(diameters[j].value, diameters[j].error);
            /* e_diam V-B at 18 (skipped) */
            delta = fabs(datas[i][19 + j] - err);

            if (delta > DELTA_TH)
            {
                logInfo("err : %.9lf %.9lf", datas[i][19 + j], err);
                bad = 1;
            }

            delta = fabs(datas[i][15 + j] - diam) / alxNorm(datas[i][19 + j], err);

            if (delta > DELTA_SIG)
            {
                logInfo("res : %.9lf sigma", delta);
                bad = 1;
            }
        }

        /* may set low confidence to inconsistent diameters */
        if (alxComputeMeanAngularDiameter(diameters, diametersCov, nbRequiredDiameters, &weightedMeanDiam,
                                          &maxResidualsDiam, &chi2Diam, &maxCorrelations, &nbDiameters, NULL) == mcsFAILURE)
        {
            logInfo("alxComputeMeanAngularDiameter : fail");
        }

        /* diam (log10): */
        diam = log10(weightedMeanDiam.value);
        delta = fabs(datas[i][22] - diam);

        if (delta > DELTA_TH)
        {
            logInfo("Mean: %.9lf %.9lf", datas[i][22], diam);
            bad = 1;
        }

        /* rel error: */
        err = relError(weightedMeanDiam.value, weightedMeanDiam.error);
        delta = fabs(datas[i][23] - err);

        if (delta > DELTA_TH)
        {
            logInfo("errM: %.9lf %.9lf", datas[i][23], err);
            bad = 1;
        }

        delta = fabs(datas[i][22] - diam) / alxNorm(datas[i][23], err);

        if (delta > DELTA_SIG)
        {
            logInfo("resM: %.9lf sigma", delta);
            bad = 1;
        }

        if (bad == 1)
        {
            NB++;
        }
    }
    logInfo("check %u diameters: done : %u diffs.", NS, NB);
}

/*___oOo___*/
