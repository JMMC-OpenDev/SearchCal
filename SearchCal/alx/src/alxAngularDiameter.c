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
            /* NBD (ALAIN) or NBD-1 (LAURENT) ? */
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
                        /* TODO: fixme */
                        /*
                            diameters[color].confIndex = alxCONFIDENCE_LOW;
                         */

                        /* Append each color (tolerance) in diameter flag information */
                        sprintf(tmp, "%s (%.1lf) ", alxGetDiamLabel(color), residual);
                        miscDynBufAppendString(diamInfo, tmp);
                    }
                }
            }

            /* Check if max(residuals) < 5 or chi2 < 25
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

    /* enable / disable regression tests */
    if (0)
    {
        /*
        #REF STAR DATA:         489
         */
#define IDL_NS 489
#define IDL_COL_SP 1
#define IDL_COL_MAG 2
#define IDL_COL_EMAG 8
#define IDL_COL_DIAM 14
#define IDL_COL_EDIAM 17
#define IDL_COL_MEAN 20
#define IDL_COL_EMEAN 21
#define IDL_COL_CHI2 22

        mcsUINT32 NS = IDL_NS;
        mcsDOUBLE datas[IDL_NS][IDL_COL_MAG + 6 + 6 + 3 + 3 + 2 + 1] = {
            {0, /* SP "B2III          " idx */ 48, /*MAG*/  1.416000000e+00,  1.640000000e+00,  1.860000000e+00,  2.149000000e+00,  2.357000000e+00,  2.375000000e+00, /*EMAG*/  1.131371000e-02,  8.000000000e-03,  2.154066000e-02,  2.000000030e-01,  1.680000000e-01,  2.000000030e-01, /*DIAM*/ -1.334766727e-01, -1.392005737e-01, -1.372485854e-01, /*EDIAM*/  5.610633724e-02,  4.142260920e-02,  4.597068544e-02, /*DMEAN*/ -1.372173424e-01, /*EDMEAN*/  3.972407478e-02, /*CHI2*/  7.097456863e-02 },
            {1, /* SP "B0Ib           " idx */ 40, /*MAG*/  1.506000000e+00,  1.690000000e+00,  1.850000000e+00,  2.191000000e+00,  2.408000000e+00,  2.273000000e+00, /*EMAG*/  9.433981000e-03,  5.000000000e-03,  2.061553000e-02,  2.000000030e-01,  1.800000000e-01,  2.000000030e-01, /*DIAM*/ -1.868839347e-01, -1.882669491e-01, -1.534060545e-01, /*EDIAM*/  5.663038471e-02,  4.479328271e-02,  4.640162380e-02, /*DMEAN*/ -1.751882568e-01, /*EDMEAN*/  4.171015546e-02, /*CHI2*/  3.223086934e-01 },
            {2, /* SP "F0II           " idx */ 120, /*MAG*/ -4.560000000e-01, -6.200000000e-01, -8.500000000e-01, -1.169000000e+00, -1.280000000e+00, -1.304000000e+00, /*EMAG*/  2.334524000e-02,  1.600000000e-02,  2.561250000e-02,  1.130000000e-01,  1.900000000e-01,  2.000000030e-01, /*DIAM*/  8.632686480e-01,  8.619146996e-01,  8.506373772e-01, /*EDIAM*/  3.164027610e-02,  4.603727054e-02,  4.539923299e-02, /*DMEAN*/  8.598152133e-01, /*EDMEAN*/  3.355954775e-02, /*CHI2*/  1.817021036e-01 },
            {3, /* SP "B2Iab:         " idx */ 48, /*MAG*/  1.289000000e+00,  1.500000000e+00,  1.700000000e+00,  1.977000000e+00,  2.158000000e+00,  2.222000000e+00, /*EMAG*/  7.810250000e-03,  5.000000000e-03,  2.061553000e-02,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/ -9.661952931e-02, -9.696711007e-02, -1.063069791e-01, /*EDIAM*/  5.610428801e-02,  4.900122678e-02,  4.597039254e-02, /*DMEAN*/ -1.005523563e-01, /*EDMEAN*/  3.976175846e-02, /*CHI2*/  2.605135517e-02 },
            {4, /* SP "B8IVn          " idx */ 72, /*MAG*/  1.273000000e+00,  1.360000000e+00,  1.460000000e+00,  1.665000000e+00,  1.658000000e+00,  1.640000000e+00, /*EMAG*/  1.655295000e-02,  7.000000000e-03,  1.220656000e-02,  2.000000030e-01,  1.860000000e-01,  2.000000030e-01, /*DIAM*/  1.138533835e-01,  1.276698682e-01,  1.353078698e-01, /*EDIAM*/  5.582641487e-02,  4.531280000e-02,  4.557558700e-02, /*DMEAN*/  1.270934327e-01, /*EDMEAN*/  3.564269840e-02, /*CHI2*/  7.268301403e-02 },
            {5, /* SP "A0Va           " idx */ 80, /*MAG*/  2.900000000e-02,  3.000000000e-02,  4.000000000e-02, -1.770000000e-01, -2.900000000e-02,  1.290000000e-01, /*EMAG*/  5.001988000e-03,  1.000000047e-03,  2.000050000e-02,  2.000000030e-01,  1.460000000e-01,  1.860000000e-01, /*DIAM*/  5.566844984e-01,  5.092371058e-01,  4.714901362e-01, /*EDIAM*/  5.585870990e-02,  3.580132847e-02,  4.241861761e-02, /*DMEAN*/  5.057223436e-01, /*EDMEAN*/  3.103959749e-02, /*CHI2*/  9.144526211e-01 },
            {6, /* SP "B6V            " idx */ 64, /*MAG*/  1.660000000e+00,  1.730000000e+00,  1.780000000e+00,  2.021000000e+00,  2.027000000e+00,  2.016000000e+00, /*EMAG*/  6.403124000e-03,  4.000000000e-03,  1.077033000e-02,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/  2.459223545e-03,  2.018235671e-02,  2.563048815e-02, /*EDIAM*/  5.580792547e-02,  4.867270601e-02,  4.562590980e-02, /*DMEAN*/  1.768628478e-02, /*EDMEAN*/  4.147261914e-02, /*CHI2*/  5.208618552e-02 },
            {7, /* SP "A4V            " idx */ 96, /*MAG*/  1.315000000e+00,  1.170000000e+00,  1.010000000e+00,  1.037000000e+00,  9.370000000e-01,  9.450000000e-01, /*EMAG*/  1.303840000e-02,  7.000000000e-03,  1.220656000e-02,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/  3.580609034e-01,  3.676706123e-01,  3.590711580e-01, /*EDIAM*/  5.581921765e-02,  4.864050679e-02,  4.553792622e-02, /*DMEAN*/  3.617717027e-01, /*EDMEAN*/  3.940922817e-02, /*CHI2*/  3.794635958e-01 },
            {8, /* SP "M2III          " idx */ 248, /*MAG*/  5.356000000e+00,  3.730000000e+00,  1.660000000e+00,  4.230000000e-01, -4.300000000e-01, -6.680000000e-01, /*EMAG*/  4.428318000e-02,  5.000000000e-03,  2.061553000e-02,  1.820000000e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/  8.913040420e-01,  9.462109810e-01,  9.430045539e-01, /*EDIAM*/  5.046496686e-02,  4.833491930e-02,  4.531253805e-02, /*DMEAN*/  9.285340201e-01, /*EDMEAN*/  3.426809347e-02, /*CHI2*/  2.712592300e-01 },
            {9, /* SP "O4If(n)p       " idx */ 16, /*MAG*/  1.941000000e+00,  2.210000000e+00,  2.430000000e+00,  2.790000000e+00,  2.955000000e+00,  2.968000000e+00, /*EMAG*/  1.824829000e-02,  3.000000000e-03,  2.022375000e-02,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/ -3.716308789e-01, -3.831452751e-01, -3.724741507e-01, /*EDIAM*/  7.035102742e-02,  6.211184719e-02,  5.853215927e-02, /*DMEAN*/ -3.759300046e-01, /*EDMEAN*/  5.034929107e-02, /*CHI2*/  1.939055255e-02 },
            {10, /* SP "M1III          " idx */ 244, /*MAG*/  6.797000000e+00,  5.150000000e+00,  3.130000000e+00,  1.839000000e+00,  9.960000000e-01,  8.340000000e-01, /*EMAG*/  1.300000000e-02,  5.000000000e-03,  5.024938000e-02,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/  6.138686625e-01,  6.586419350e-01,  6.354661066e-01, /*EDIAM*/  5.546791054e-02,  4.835636467e-02,  4.533110626e-02, /*DMEAN*/  6.378018369e-01, /*EDMEAN*/  5.492000551e-02, /*CHI2*/  2.059551912e-01 },
            {11, /* SP "K3.5III        " idx */ 214, /*MAG*/  6.299000000e+00,  4.770000000e+00,  3.150000000e+00,  2.160000000e+00,  1.390000000e+00,  1.247000000e+00, /*EMAG*/  1.676305000e-02,  5.000000000e-03,  4.031129000e-02,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/  4.731745510e-01,  5.048631624e-01,  4.910127783e-01, /*EDIAM*/  5.544824285e-02,  4.832402560e-02,  4.531456419e-02, /*DMEAN*/  4.911181680e-01, /*EDMEAN*/  5.101776301e-02, /*CHI2*/  1.957637715e+00 },
            {12, /* SP "M3.0IIIa       " idx */ 252, /*MAG*/  4.491000000e+00,  2.870000000e+00,  5.700000000e-01, -9.120000000e-01, -1.608000000e+00, -1.862000000e+00, /*EMAG*/  9.219544000e-03,  6.000000000e-03,  5.035871000e-02,  1.880000000e-01,  1.900000000e-01,  1.800000000e-01, /*DIAM*/  1.186547604e+00,  1.195403174e+00,  1.195587294e+00, /*EDIAM*/  5.213692774e-02,  4.595499851e-02,  4.080505428e-02, /*DMEAN*/  1.193222111e+00, /*EDMEAN*/  2.666461312e-02, /*CHI2*/  6.148360826e-01 },
            {13, /* SP "M3.0IIIa       " idx */ 252, /*MAG*/  4.491000000e+00,  2.870000000e+00,  5.700000000e-01, -9.120000000e-01, -1.608000000e+00, -1.862000000e+00, /*EMAG*/  9.219544000e-03,  6.000000000e-03,  5.035871000e-02,  1.880000000e-01,  1.900000000e-01,  1.800000000e-01, /*DIAM*/  1.186547604e+00,  1.195403174e+00,  1.195587294e+00, /*EDIAM*/  5.213692774e-02,  4.595499851e-02,  4.080505428e-02, /*DMEAN*/  1.193222111e+00, /*EDMEAN*/  3.354934537e-02, /*CHI2*/  2.245014074e-01 },
            {14, /* SP "K5III          " idx */ 220, /*MAG*/  2.408000000e+00,  8.700000000e-01, -8.000000000e-01, -2.095000000e+00, -2.775000000e+00, -3.044000000e+00, /*EMAG*/  1.360147000e-02,  1.100000000e-02,  2.282542000e-02,  1.940000000e-01,  1.700000000e-01,  1.400000000e-01, /*DIAM*/  1.362298596e+00,  1.359031537e+00,  1.368203100e+00, /*EDIAM*/  5.381611392e-02,  4.112575189e-02,  3.178263650e-02, /*DMEAN*/  1.364332356e+00, /*EDMEAN*/  8.482814493e-02, /*CHI2*/  1.788060608e-01 },
            {15, /* SP "B6Vep          " idx */ 64, /*MAG*/  2.920000000e-01,  4.500000000e-01,  6.200000000e-01,  8.150000000e-01,  8.650000000e-01,  8.800000000e-01, /*EMAG*/  1.140175000e-02,  9.000000000e-03,  2.193171000e-02,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/  2.379770842e-01,  2.477154340e-01,  2.490465499e-01, /*EDIAM*/  5.581135895e-02,  4.867384190e-02,  4.562640166e-02, /*DMEAN*/  2.456903371e-01, /*EDMEAN*/  3.290140868e-02, /*CHI2*/  4.444069306e-01 },
            {16, /* SP "B2III          " idx */ 48, /*MAG*/  1.416000000e+00,  1.640000000e+00,  1.860000000e+00,  2.149000000e+00,  2.357000000e+00,  2.375000000e+00, /*EMAG*/  1.131371000e-02,  8.000000000e-03,  2.154066000e-02,  2.000000030e-01,  1.680000000e-01,  2.000000030e-01, /*DIAM*/ -1.334766727e-01, -1.392005737e-01, -1.372485854e-01, /*EDIAM*/  5.610633724e-02,  4.142260920e-02,  4.597068544e-02, /*DMEAN*/ -1.372173424e-01, /*EDMEAN*/  3.665756948e-02, /*CHI2*/  9.666517689e-03 },
            {17, /* SP "B0Ib           " idx */ 40, /*MAG*/  1.506000000e+00,  1.690000000e+00,  1.850000000e+00,  2.191000000e+00,  2.408000000e+00,  2.273000000e+00, /*EMAG*/  9.433981000e-03,  5.000000000e-03,  2.061553000e-02,  2.000000030e-01,  1.800000000e-01,  2.000000030e-01, /*DIAM*/ -1.868839347e-01, -1.882669491e-01, -1.534060545e-01, /*EDIAM*/  5.663038471e-02,  4.479328271e-02,  4.640162380e-02, /*DMEAN*/ -1.751882568e-01, /*EDMEAN*/  3.826234556e-02, /*CHI2*/  1.645373641e-01 },
            {18, /* SP "F0II           " idx */ 120, /*MAG*/ -4.560000000e-01, -6.200000000e-01, -8.500000000e-01, -1.169000000e+00, -1.280000000e+00, -1.304000000e+00, /*EMAG*/  2.334524000e-02,  1.600000000e-02,  2.561250000e-02,  1.130000000e-01,  1.900000000e-01,  2.000000030e-01, /*DIAM*/  8.632686480e-01,  8.619146996e-01,  8.506373772e-01, /*EDIAM*/  3.164027610e-02,  4.603727054e-02,  4.539923299e-02, /*DMEAN*/  8.598152133e-01, /*EDMEAN*/  5.136440481e-02, /*CHI2*/  2.232778688e-01 },
            {19, /* SP "B2Iab:         " idx */ 48, /*MAG*/  1.289000000e+00,  1.500000000e+00,  1.700000000e+00,  1.977000000e+00,  2.158000000e+00,  2.222000000e+00, /*EMAG*/  7.810250000e-03,  5.000000000e-03,  2.061553000e-02,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/ -9.661952931e-02, -9.696711007e-02, -1.063069791e-01, /*EDIAM*/  5.610428801e-02,  4.900122678e-02,  4.597039254e-02, /*DMEAN*/ -1.005523563e-01, /*EDMEAN*/  3.998845898e-02, /*CHI2*/  1.159688972e-02 },
            {20, /* SP "F8Iab:         " idx */ 152, /*MAG*/  2.501000000e+00,  1.830000000e+00,  1.160000000e+00,  7.710000000e-01,  5.370000000e-01,  4.530000000e-01, /*EMAG*/  1.923538000e-02,  3.000000000e-03,  2.022375000e-02,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/  5.277277004e-01,  5.336807368e-01,  5.298456652e-01, /*EDIAM*/  5.544728984e-02,  4.834197395e-02,  4.530869959e-02, /*DMEAN*/  5.306135660e-01, /*EDMEAN*/  6.235444768e-02, /*CHI2*/  5.892070197e-02 },
            {21, /* SP "B5Ia           " idx */ 60, /*MAG*/  2.367000000e+00,  2.450000000e+00,  2.440000000e+00,  2.439000000e+00,  2.492000000e+00,  2.442000000e+00, /*EMAG*/  1.044031000e-02,  3.000000000e-03,  2.022375000e-02,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/ -8.044333735e-02, -8.049941995e-02, -7.063991279e-02, /*EDIAM*/  5.582664846e-02,  4.870689184e-02,  4.567587090e-02, /*DMEAN*/ -7.660744763e-02, /*EDMEAN*/  4.520915200e-02, /*CHI2*/  3.904630826e-01 },
            {22, /* SP "O4If(n)p       " idx */ 16, /*MAG*/  1.941000000e+00,  2.210000000e+00,  2.430000000e+00,  2.790000000e+00,  2.955000000e+00,  2.968000000e+00, /*EMAG*/  1.824829000e-02,  3.000000000e-03,  2.022375000e-02,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/ -3.716308789e-01, -3.831452751e-01, -3.724741507e-01, /*EDIAM*/  7.035102742e-02,  6.211184719e-02,  5.853215927e-02, /*DMEAN*/ -3.759300046e-01, /*EDMEAN*/  5.078670380e-02, /*CHI2*/  7.862988932e-03 },
            {23, /* SP "A2IV           " idx */ 88, /*MAG*/  1.740000000e+00,  1.670000000e+00,  1.650000000e+00,  1.493000000e+00,  1.500000000e+00,  1.487000000e+00, /*EMAG*/  1.000000000e-02,  6.000000000e-03,  2.088061000e-02,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/  2.486853904e-01,  2.326851907e-01,  2.310859542e-01, /*EDIAM*/  5.586430064e-02,  4.867448270e-02,  4.555824518e-02, /*DMEAN*/  2.362379193e-01, /*EDMEAN*/  3.449380731e-02, /*CHI2*/  3.627530019e-01 },
            {24, /* SP "B8IVn          " idx */ 72, /*MAG*/  1.273000000e+00,  1.360000000e+00,  1.460000000e+00,  1.665000000e+00,  1.658000000e+00,  1.640000000e+00, /*EMAG*/  1.655295000e-02,  7.000000000e-03,  1.220656000e-02,  2.000000030e-01,  1.860000000e-01,  2.000000030e-01, /*DIAM*/  1.138533835e-01,  1.276698682e-01,  1.353078698e-01, /*EDIAM*/  5.582641487e-02,  4.531280000e-02,  4.557558700e-02, /*DMEAN*/  1.270934327e-01, /*EDMEAN*/  3.386552864e-02, /*CHI2*/  5.672785723e-02 },
            {25, /* SP "A3Va           " idx */ 92, /*MAG*/  2.230000000e+00,  2.140000000e+00,  2.040000000e+00,  1.854000000e+00,  1.925000000e+00,  1.883000000e+00, /*EMAG*/  4.000000000e-03,  4.000000000e-03,  4.000000000e-03,  2.000000030e-01,  1.940000000e-01,  1.920000000e-01, /*DIAM*/  1.964486526e-01,  1.600390863e-01,  1.637103550e-01, /*EDIAM*/  5.584644767e-02,  4.722668997e-02,  4.375238828e-02, /*DMEAN*/  1.705525053e-01, /*EDMEAN*/  4.056491832e-02, /*CHI2*/  5.385495648e-01 },
            {26, /* SP "B8III          " idx */ 72, /*MAG*/  2.473000000e+00,  2.580000000e+00,  2.680000000e+00,  2.848000000e+00,  2.975000000e+00,  2.945000000e+00, /*EMAG*/  3.605551000e-03,  3.000000000e-03,  2.022375000e-02,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/ -1.199055486e-01, -1.397309140e-01, -1.279257108e-01, /*EDIAM*/  5.582430255e-02,  4.865891924e-02,  4.557528399e-02, /*DMEAN*/ -1.298983577e-01, /*EDMEAN*/  4.509018605e-02, /*CHI2*/  2.909714909e-02 },
            {27, /* SP "A5III          " idx */ 100, /*MAG*/  2.235000000e+00,  2.080000000e+00,  1.910000000e+00,  1.833000000e+00,  1.718000000e+00,  1.683000000e+00, /*EMAG*/  4.242641000e-03,  3.000000000e-03,  3.014963000e-02,  2.000000030e-01,  1.760000000e-01,  2.000000030e-01, /*DIAM*/  2.160338166e-01,  2.249057198e-01,  2.233864482e-01, /*EDIAM*/  5.577814290e-02,  4.286956014e-02,  4.551985837e-02, /*DMEAN*/  2.222500583e-01, /*EDMEAN*/  4.208822155e-02, /*CHI2*/  2.470473628e-02 },
            {28, /* SP "B9.5III        " idx */ 78, /*MAG*/  1.759000000e+00,  1.790000000e+00,  1.780000000e+00,  1.733000000e+00,  1.768000000e+00,  1.771000000e+00, /*EMAG*/  8.062258000e-03,  7.000000000e-03,  2.118962000e-02,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/  1.550140442e-01,  1.413857555e-01,  1.393901118e-01, /*EDIAM*/  5.585400102e-02,  4.867083984e-02,  4.556504286e-02, /*DMEAN*/  1.441630086e-01, /*EDMEAN*/  3.394324117e-02, /*CHI2*/  7.573352268e-02 },
            {29, /* SP "A0Va           " idx */ 80, /*MAG*/  2.900000000e-02,  3.000000000e-02,  4.000000000e-02, -1.770000000e-01, -2.900000000e-02,  1.290000000e-01, /*EMAG*/  5.001988000e-03,  1.000000047e-03,  2.000050000e-02,  2.000000030e-01,  1.460000000e-01,  1.860000000e-01, /*DIAM*/  5.566844984e-01,  5.092371058e-01,  4.714901362e-01, /*EDIAM*/  5.585870990e-02,  3.580132847e-02,  4.241861761e-02, /*DMEAN*/  5.057223436e-01, /*EDMEAN*/  2.643955041e-02, /*CHI2*/  5.111834713e-01 },
            {30, /* SP "B6V            " idx */ 64, /*MAG*/  1.660000000e+00,  1.730000000e+00,  1.780000000e+00,  2.021000000e+00,  2.027000000e+00,  2.016000000e+00, /*EMAG*/  6.403124000e-03,  4.000000000e-03,  1.077033000e-02,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/  2.459223545e-03,  2.018235671e-02,  2.563048815e-02, /*EDIAM*/  5.580792547e-02,  4.867270601e-02,  4.562590980e-02, /*DMEAN*/  1.768628478e-02, /*EDMEAN*/  4.147261914e-02, /*CHI2*/  5.208618552e-02 },
            {31, /* SP "A4V            " idx */ 96, /*MAG*/  1.315000000e+00,  1.170000000e+00,  1.010000000e+00,  1.037000000e+00,  9.370000000e-01,  9.450000000e-01, /*EMAG*/  1.303840000e-02,  7.000000000e-03,  1.220656000e-02,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/  3.580609034e-01,  3.676706123e-01,  3.590711580e-01, /*EDIAM*/  5.581921765e-02,  4.864050679e-02,  4.553792622e-02, /*DMEAN*/  3.617717027e-01, /*EDMEAN*/  3.932116522e-02, /*CHI2*/  3.448511845e-01 },
            {32, /* SP "M3.0IIIa       " idx */ 252, /*MAG*/  4.491000000e+00,  2.870000000e+00,  5.700000000e-01, -9.120000000e-01, -1.608000000e+00, -1.862000000e+00, /*EMAG*/  9.219544000e-03,  6.000000000e-03,  5.035871000e-02,  1.880000000e-01,  1.900000000e-01,  1.800000000e-01, /*DIAM*/  1.186547604e+00,  1.195403174e+00,  1.195587294e+00, /*EDIAM*/  5.213692774e-02,  4.595499851e-02,  4.080505428e-02, /*DMEAN*/  1.193222111e+00, /*EDMEAN*/  5.044841964e-02, /*CHI2*/  2.476736949e-01 },
            {33, /* SP "M3.0IIIa       " idx */ 252, /*MAG*/  4.491000000e+00,  2.870000000e+00,  5.700000000e-01, -9.120000000e-01, -1.608000000e+00, -1.862000000e+00, /*EMAG*/  9.219544000e-03,  6.000000000e-03,  5.035871000e-02,  1.880000000e-01,  1.900000000e-01,  1.800000000e-01, /*DIAM*/  1.186547604e+00,  1.195403174e+00,  1.195587294e+00, /*EDIAM*/  5.213692774e-02,  4.595499851e-02,  4.080505428e-02, /*DMEAN*/  1.193222111e+00, /*EDMEAN*/  3.484498217e-02, /*CHI2*/  2.091802415e+00 },
            {34, /* SP "K3III          " idx */ 212, /*MAG*/  6.272000000e+00,  4.970000000e+00,  3.720000000e+00,  2.764000000e+00,  2.192000000e+00,  1.978000000e+00, /*EMAG*/  5.608030000e-02,  3.000000000e-03,  2.022375000e-02,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/  3.173544510e-01,  3.162598578e-01,  3.279414125e-01, /*EDIAM*/  5.544217557e-02,  4.831867364e-02,  4.531124159e-02, /*DMEAN*/  3.211331802e-01, /*EDMEAN*/  6.071458711e-02, /*CHI2*/  3.847303766e-01 },
            {35, /* SP "M3III          " idx */ 252, /*MAG*/  7.629000000e+00,  6.000000000e+00,  3.760000000e+00,  2.252000000e+00,  1.356000000e+00,  1.097000000e+00, /*EMAG*/  1.166190000e-02,  6.000000000e-03,  5.035871000e-02,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/  5.511368799e-01,  6.094498583e-01,  6.082807161e-01, /*EDIAM*/  5.545229501e-02,  4.836154354e-02,  4.532310140e-02, /*DMEAN*/  5.936998828e-01, /*EDMEAN*/  8.309799675e-02, /*CHI2*/  5.051688282e-01 },
            {36, /* SP "G8Ib           " idx */ 192, /*MAG*/  4.437000000e+00,  3.060000000e+00,  1.840000000e+00,  8.860000000e-01,  2.320000000e-01,  1.250000000e-01, /*EMAG*/  1.513275000e-02,  2.000000000e-03,  1.019804000e-02,  2.000000030e-01,  1.540000000e-01,  1.720000000e-01, /*DIAM*/  6.468827577e-01,  6.809498324e-01,  6.692447512e-01, /*EDIAM*/  5.541761655e-02,  3.721861252e-02,  3.896781983e-02, /*DMEAN*/  6.699340477e-01, /*EDMEAN*/  4.824846577e-02, /*CHI2*/  2.349677808e+00 },
            {37, /* SP "K4III          " idx */ 216, /*MAG*/  5.838000000e+00,  4.390000000e+00,  2.880000000e+00,  1.897000000e+00,  1.220000000e+00,  1.022000000e+00, /*EMAG*/  4.242641000e-03,  3.000000000e-03,  6.007495000e-02,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/  5.206200686e-01,  5.336058303e-01,  5.348556376e-01, /*EDIAM*/  5.545333446e-02,  4.832968341e-02,  4.531819496e-02, /*DMEAN*/  5.306924917e-01, /*EDMEAN*/  8.853556655e-02, /*CHI2*/  7.728089093e-02 },
            {38, /* SP "M2III          " idx */ 248, /*MAG*/  7.554000000e+00,  5.970000000e+00,  3.770000000e+00,  2.450000000e+00,  1.582000000e+00,  1.444000000e+00, /*EMAG*/  1.029563000e-02,  5.000000000e-03,  3.041381000e-02,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/  5.022593934e-01,  5.532148662e-01,  5.239680513e-01, /*EDIAM*/  5.543902597e-02,  4.833491930e-02,  4.531253805e-02, /*DMEAN*/  5.283681549e-01, /*EDMEAN*/  8.457084324e-02, /*CHI2*/  5.667060327e-01 },
            {39, /* SP "M1III          " idx */ 244, /*MAG*/  6.614000000e+00,  4.930000000e+00,  2.990000000e+00,  1.651000000e+00,  8.140000000e-01,  5.980000000e-01, /*EMAG*/  1.802776000e-02,  6.000000000e-03,  5.035871000e-02,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/  6.490115201e-01,  6.934746204e-01,  6.830865453e-01, /*EDIAM*/  5.546849517e-02,  4.835655816e-02,  4.533119004e-02, /*DMEAN*/  6.777390312e-01, /*EDMEAN*/  6.453764094e-02, /*CHI2*/  9.931913174e-01 },
            {40, /* SP "K4III          " idx */ 216, /*MAG*/  5.838000000e+00,  4.390000000e+00,  2.880000000e+00,  1.897000000e+00,  1.220000000e+00,  1.022000000e+00, /*EMAG*/  4.242641000e-03,  3.000000000e-03,  6.007495000e-02,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/  5.206200686e-01,  5.336058303e-01,  5.348556376e-01, /*EDIAM*/  5.545333446e-02,  4.832968341e-02,  4.531819496e-02, /*DMEAN*/  5.306924917e-01, /*EDMEAN*/  8.089246524e-02, /*CHI2*/  1.683417675e-02 },
            {41, /* SP "M6III          " idx */ 264, /*MAG*/  7.212000000e+00,  5.760000000e+00,  2.170000000e+00,  2.300000000e-01, -6.520000000e-01, -8.680000000e-01, /*EMAG*/  5.385165000e-02,  1.400000000e-02,  2.000000030e-01,  2.000000030e-01,  2.000000000e-01,  2.000000030e-01, /*DIAM*/  1.048959756e+00,  1.072381527e+00,  1.063134414e+00, /*EDIAM*/  5.713862410e-02,  5.016499596e-02,  4.649705455e-02, /*DMEAN*/  1.062596345e+00, /*EDMEAN*/  3.071449578e-02, /*CHI2*/  6.306882436e-02 },
            {42, /* SP "M1III          " idx */ 244, /*MAG*/  6.614000000e+00,  4.930000000e+00,  2.990000000e+00,  1.651000000e+00,  8.140000000e-01,  5.980000000e-01, /*EMAG*/  1.802776000e-02,  6.000000000e-03,  5.035871000e-02,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/  6.490115201e-01,  6.934746204e-01,  6.830865453e-01, /*EDIAM*/  5.546849517e-02,  4.835655816e-02,  4.533119004e-02, /*DMEAN*/  6.777390312e-01, /*EDMEAN*/  4.010093017e-02, /*CHI2*/  2.845243516e-01 },
            {43, /* SP "M4III          " idx */ 256, /*MAG*/  7.911000000e+00,  6.320000000e+00,  2.900000000e+00,  8.970000000e-01, -1.640000000e-01, -5.120000000e-01, /*EMAG*/  2.842534000e-02,  1.800000000e-02,  6.264184000e-02,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/  9.395321850e-01,  9.878673835e-01,  9.859160695e-01, /*EDIAM*/  5.561470925e-02,  4.853804425e-02,  4.542932273e-02, /*DMEAN*/  9.744354232e-01, /*EDMEAN*/  8.178947060e-02, /*CHI2*/  2.795872216e-01 },
            {44, /* SP "M2III          " idx */ 248, /*MAG*/  6.269000000e+00,  4.680000000e+00,  2.720000000e+00,  1.530000000e+00,  7.060000000e-01,  4.900000000e-01, /*EMAG*/  4.110961000e-02,  3.000000000e-03,  2.022375000e-02,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/  6.578486814e-01,  7.113393822e-01,  7.059388570e-01, /*EDIAM*/  5.543817515e-02,  4.833463774e-02,  4.531241614e-02, /*DMEAN*/  6.951932492e-01, /*EDMEAN*/  4.669182498e-02, /*CHI2*/  1.019694373e+00 },
            {45, /* SP "G9II/III       " idx */ 196, /*MAG*/  4.671000000e+00,  3.520000000e+00,  2.430000000e+00,  1.603000000e+00,  1.040000000e+00,  9.760000000e-01, /*EMAG*/  7.111259000e-02,  4.000000000e-03,  2.039608000e-02,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/  4.924411440e-01,  5.100102864e-01,  4.939652206e-01, /*EDIAM*/  5.542028483e-02,  4.830130616e-02,  4.529855092e-02, /*DMEAN*/  4.991050272e-01, /*EDMEAN*/  5.899253899e-02, /*CHI2*/  1.502304428e+00 },
            {46, /* SP "G9III          " idx */ 196, /*MAG*/  5.283000000e+00,  4.300000000e+00,  3.320000000e+00,  2.781000000e+00,  2.267000000e+00,  2.184000000e+00, /*EMAG*/  3.605551000e-02,  2.000000000e-03,  2.009975000e-02,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/  2.262804258e-01,  2.461737066e-01,  2.411185015e-01, /*EDIAM*/  5.541964649e-02,  4.830109484e-02,  4.529845946e-02, /*DMEAN*/  2.389734663e-01, /*EDMEAN*/  6.839661298e-02, /*CHI2*/  3.077510209e+00 },
            {47, /* SP "M5III          " idx */ 260, /*MAG*/  7.246000000e+00,  5.780000000e+00,  3.010000000e+00,  1.429000000e+00,  5.260000000e-01,  2.550000000e-01, /*EMAG*/  1.000000000e-02,  6.000000000e-03,  2.088061000e-02,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/  7.364065786e-01,  7.953876844e-01,  8.036138353e-01, /*EDIAM*/  5.605799090e-02,  4.903140760e-02,  4.574769814e-02, /*DMEAN*/  7.831934094e-01, /*EDMEAN*/  4.426656321e-02, /*CHI2*/  4.564409264e-01 },
            {48, /* SP "K3.5III        " idx */ 214, /*MAG*/  6.299000000e+00,  4.770000000e+00,  3.150000000e+00,  2.160000000e+00,  1.390000000e+00,  1.247000000e+00, /*EMAG*/  1.676305000e-02,  5.000000000e-03,  4.031129000e-02,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/  4.731745510e-01,  5.048631624e-01,  4.910127783e-01, /*EDIAM*/  5.544824285e-02,  4.832402560e-02,  4.531456419e-02, /*DMEAN*/  4.911181680e-01, /*EDMEAN*/  7.356871779e-02, /*CHI2*/  7.405285646e-02 },
            {49, /* SP "M3.0IIIa       " idx */ 252, /*MAG*/  4.491000000e+00,  2.870000000e+00,  5.700000000e-01, -9.120000000e-01, -1.608000000e+00, -1.862000000e+00, /*EMAG*/  9.219544000e-03,  6.000000000e-03,  5.035871000e-02,  1.880000000e-01,  1.900000000e-01,  1.800000000e-01, /*DIAM*/  1.186547604e+00,  1.195403174e+00,  1.195587294e+00, /*EDIAM*/  5.213692774e-02,  4.595499851e-02,  4.080505428e-02, /*DMEAN*/  1.193222111e+00, /*EDMEAN*/  7.244740583e-02, /*CHI2*/  1.866263891e-02 },
            {50, /* SP "K4III          " idx */ 216, /*MAG*/  5.838000000e+00,  4.390000000e+00,  2.880000000e+00,  1.897000000e+00,  1.220000000e+00,  1.022000000e+00, /*EMAG*/  4.242641000e-03,  3.000000000e-03,  6.007495000e-02,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/  5.206200686e-01,  5.336058303e-01,  5.348556376e-01, /*EDIAM*/  5.545333446e-02,  4.832968341e-02,  4.531819496e-02, /*DMEAN*/  5.306924917e-01, /*EDMEAN*/  3.479103054e-02, /*CHI2*/  3.088730052e+00 },
            {51, /* SP "M1III          " idx */ 244, /*MAG*/  6.614000000e+00,  4.930000000e+00,  2.990000000e+00,  1.651000000e+00,  8.140000000e-01,  5.980000000e-01, /*EMAG*/  1.802776000e-02,  6.000000000e-03,  5.035871000e-02,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/  6.490115201e-01,  6.934746204e-01,  6.830865453e-01, /*EDIAM*/  5.546849517e-02,  4.835655816e-02,  4.533119004e-02, /*DMEAN*/  6.777390312e-01, /*EDMEAN*/  7.985123580e-02, /*CHI2*/  5.273865805e-01 },
            {52, /* SP "M2III          " idx */ 248, /*MAG*/  7.794000000e+00,  6.140000000e+00,  3.990000000e+00,  2.840000000e+00,  1.915000000e+00,  1.705000000e+00, /*EMAG*/  1.208305000e-02,  5.000000000e-03,  3.041381000e-02,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/  4.073665348e-01,  4.798919079e-01,  4.693768096e-01, /*EDIAM*/  5.543902597e-02,  4.833491930e-02,  4.531253805e-02, /*DMEAN*/  4.567466931e-01, /*EDMEAN*/  3.438912223e-02, /*CHI2*/  1.765734002e+00 },
            {53, /* SP "M4             " idx */ 256, /*MAG*/  9.383000000e+00,  7.820000000e+00,  5.230000000e+00,  3.714000000e+00,  2.709000000e+00,  2.443000000e+00, /*EMAG*/  2.549510000e-02,  1.100000000e-02,  4.148494000e-02,  2.000000030e-01,  1.900000000e-01,  2.000000030e-01, /*DIAM*/  2.750053894e-01,  3.566378021e-01,  3.566824835e-01, /*EDIAM*/  5.560394759e-02,  4.613696347e-02,  4.542777996e-02, /*DMEAN*/  3.360194292e-01, /*EDMEAN*/  5.981094970e-02, /*CHI2*/  2.241751316e+00 },
            {54, /* SP "M2III          " idx */ 248, /*MAG*/  6.269000000e+00,  4.680000000e+00,  2.720000000e+00,  1.530000000e+00,  7.060000000e-01,  4.900000000e-01, /*EMAG*/  4.110961000e-02,  3.000000000e-03,  2.022375000e-02,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/  6.578486814e-01,  7.113393822e-01,  7.059388570e-01, /*EDIAM*/  5.543817515e-02,  4.833463774e-02,  4.531241614e-02, /*DMEAN*/  6.951932492e-01, /*EDMEAN*/  4.061509552e-02, /*CHI2*/  2.078429952e-01 },
            {55, /* SP "M1.5III        " idx */ 246, /*MAG*/  6.653000000e+00,  5.030000000e+00,  2.990000000e+00,  1.717000000e+00,  8.910000000e-01,  7.010000000e-01, /*EMAG*/  1.029563000e-02,  5.000000000e-03,  5.024938000e-02,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/  6.360256904e-01,  6.802714845e-01,  6.649027621e-01, /*EDIAM*/  5.545177280e-02,  4.834337296e-02,  4.532062238e-02, /*DMEAN*/  6.626355529e-01, /*EDMEAN*/  3.644991963e-02, /*CHI2*/  1.238326960e-01 },
            {56, /* SP "K5III          " idx */ 220, /*MAG*/  6.779000000e+00,  5.190000000e+00,  3.590000000e+00,  2.466000000e+00,  1.778000000e+00,  1.611000000e+00, /*EMAG*/  8.062258000e-03,  4.000000000e-03,  9.008885000e-02,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/  4.315932248e-01,  4.388214062e-01,  4.284001659e-01, /*EDIAM*/  5.546766575e-02,  4.834418472e-02,  4.532739880e-02, /*DMEAN*/  4.328343553e-01, /*EDMEAN*/  6.584439877e-02, /*CHI2*/  9.371123060e-02 },
            {57, /* SP "M4III          " idx */ 256, /*MAG*/  7.911000000e+00,  6.320000000e+00,  2.900000000e+00,  8.970000000e-01, -1.640000000e-01, -5.120000000e-01, /*EMAG*/  2.842534000e-02,  1.800000000e-02,  6.264184000e-02,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/  9.395321850e-01,  9.878673835e-01,  9.859160695e-01, /*EDIAM*/  5.561470925e-02,  4.853804425e-02,  4.542932273e-02, /*DMEAN*/  9.744354232e-01, /*EDMEAN*/  2.951517742e-02, /*CHI2*/  1.849952813e-01 },
            {58, /* SP "M3III          " idx */ 252, /*MAG*/  1.003300000e+01,  8.290000000e+00,  5.350000000e+00,  3.565000000e+00,  2.542000000e+00,  2.215000000e+00, /*EMAG*/  3.862642000e-02,  1.400000000e-02,  4.237924000e-02,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/  3.635565199e-01,  4.177844858e-01,  4.154777935e-01, /*EDIAM*/  5.546080047e-02,  4.836435753e-02,  4.532432019e-02, /*DMEAN*/  4.026627107e-01, /*EDMEAN*/  7.436165914e-02, /*CHI2*/  2.254109571e+00 },
            {59, /* SP "K5III          " idx */ 220, /*MAG*/  2.408000000e+00,  8.700000000e-01, -8.000000000e-01, -2.095000000e+00, -2.775000000e+00, -3.044000000e+00, /*EMAG*/  1.360147000e-02,  1.100000000e-02,  2.282542000e-02,  1.940000000e-01,  1.700000000e-01,  1.400000000e-01, /*DIAM*/  1.362298596e+00,  1.359031537e+00,  1.368203100e+00, /*EDIAM*/  5.381611392e-02,  4.112575189e-02,  3.178263650e-02, /*DMEAN*/  1.364332356e+00, /*EDMEAN*/  2.369625318e-02, /*CHI2*/  1.811084654e+00 },
            {60, /* SP "K5III          " idx */ 220, /*MAG*/  2.408000000e+00,  8.700000000e-01, -8.000000000e-01, -2.095000000e+00, -2.775000000e+00, -3.044000000e+00, /*EMAG*/  1.360147000e-02,  1.100000000e-02,  2.282542000e-02,  1.940000000e-01,  1.700000000e-01,  1.400000000e-01, /*DIAM*/  1.362298596e+00,  1.359031537e+00,  1.368203100e+00, /*EDIAM*/  5.381611392e-02,  4.112575189e-02,  3.178263650e-02, /*DMEAN*/  1.364332356e+00, /*EDMEAN*/  5.284675689e-02, /*CHI2*/  4.748484985e-01 },
            {61, /* SP "K5III          " idx */ 220, /*MAG*/  2.408000000e+00,  8.700000000e-01, -8.000000000e-01, -2.095000000e+00, -2.775000000e+00, -3.044000000e+00, /*EMAG*/  1.360147000e-02,  1.100000000e-02,  2.282542000e-02,  1.940000000e-01,  1.700000000e-01,  1.400000000e-01, /*DIAM*/  1.362298596e+00,  1.359031537e+00,  1.368203100e+00, /*EDIAM*/  5.381611392e-02,  4.112575189e-02,  3.178263650e-02, /*DMEAN*/  1.364332356e+00, /*EDMEAN*/  7.565714856e-02, /*CHI2*/  2.854950214e-02 },
            {62, /* SP "K5III          " idx */ 220, /*MAG*/  2.408000000e+00,  8.700000000e-01, -8.000000000e-01, -2.095000000e+00, -2.775000000e+00, -3.044000000e+00, /*EMAG*/  1.360147000e-02,  1.100000000e-02,  2.282542000e-02,  1.940000000e-01,  1.700000000e-01,  1.400000000e-01, /*DIAM*/  1.362298596e+00,  1.359031537e+00,  1.368203100e+00, /*EDIAM*/  5.381611392e-02,  4.112575189e-02,  3.178263650e-02, /*DMEAN*/  1.364332356e+00, /*EDMEAN*/  4.163322368e-02, /*CHI2*/  8.060669008e-01 },
            {63, /* SP "K5III          " idx */ 220, /*MAG*/  2.408000000e+00,  8.700000000e-01, -8.000000000e-01, -2.095000000e+00, -2.775000000e+00, -3.044000000e+00, /*EMAG*/  1.360147000e-02,  1.100000000e-02,  2.282542000e-02,  1.940000000e-01,  1.700000000e-01,  1.400000000e-01, /*DIAM*/  1.362298596e+00,  1.359031537e+00,  1.368203100e+00, /*EDIAM*/  5.381611392e-02,  4.112575189e-02,  3.178263650e-02, /*DMEAN*/  1.364332356e+00, /*EDMEAN*/  2.373856288e-02, /*CHI2*/  2.547147223e+00 },
            {64, /* SP "K4III          " idx */ 216, /*MAG*/  5.838000000e+00,  4.390000000e+00,  2.880000000e+00,  1.897000000e+00,  1.220000000e+00,  1.022000000e+00, /*EMAG*/  4.242641000e-03,  3.000000000e-03,  6.007495000e-02,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/  5.206200686e-01,  5.336058303e-01,  5.348556376e-01, /*EDIAM*/  5.545333446e-02,  4.832968341e-02,  4.531819496e-02, /*DMEAN*/  5.306924917e-01, /*EDMEAN*/  3.104089271e-02, /*CHI2*/  4.372891938e-01 },
            {65, /* SP "M4III          " idx */ 256, /*MAG*/  7.911000000e+00,  6.320000000e+00,  2.900000000e+00,  8.970000000e-01, -1.640000000e-01, -5.120000000e-01, /*EMAG*/  2.842534000e-02,  1.800000000e-02,  6.264184000e-02,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/  9.395321850e-01,  9.878673835e-01,  9.859160695e-01, /*EDIAM*/  5.561470925e-02,  4.853804425e-02,  4.542932273e-02, /*DMEAN*/  9.744354232e-01, /*EDMEAN*/  2.998357251e-02, /*CHI2*/  1.808925508e+00 },
            {66, /* SP "M3             " idx */ 252, /*MAG*/  8.490000000e+00,  6.870000000e+00,  4.600000000e+00,  3.157000000e+00,  2.237000000e+00,  1.982000000e+00, /*EMAG*/  1.565248000e-02,  7.000000000e-03,  6.040695000e-02,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/  3.674493771e-01,  4.327961592e-01,  4.308865528e-01, /*EDIAM*/  5.545298613e-02,  4.836177219e-02,  4.532320043e-02, /*DMEAN*/  4.149113042e-01, /*EDMEAN*/  5.363322323e-02, /*CHI2*/  1.142341539e+00 },
            {67, /* SP "K5III          " idx */ 220, /*MAG*/  7.147000000e+00,  5.680000000e+00,  4.200000000e+00,  3.076000000e+00,  2.378000000e+00,  2.217000000e+00, /*EMAG*/  8.602325000e-03,  5.000000000e-03,  1.118034000e-02,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/  3.003789371e-01,  3.142844393e-01,  3.041519888e-01, /*EDIAM*/  5.546814409e-02,  4.834434307e-02,  4.532746735e-02, /*DMEAN*/  3.066605532e-01, /*EDMEAN*/  8.580235646e-02, /*CHI2*/  1.843402697e+00 },
            {68, /* SP "M1III          " idx */ 244, /*MAG*/  6.797000000e+00,  5.150000000e+00,  3.130000000e+00,  1.839000000e+00,  9.960000000e-01,  8.340000000e-01, /*EMAG*/  1.300000000e-02,  5.000000000e-03,  5.024938000e-02,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/  6.138686625e-01,  6.586419350e-01,  6.354661066e-01, /*EDIAM*/  5.546791054e-02,  4.835636467e-02,  4.533110626e-02, /*DMEAN*/  6.378018369e-01, /*EDMEAN*/  5.664810478e-02, /*CHI2*/  1.332168420e-01 },
            {69, /* SP "M2Iab:         " idx */ 248, /*MAG*/  6.380000000e+00,  4.320000000e+00,  1.780000000e+00,  3.930000000e-01, -6.090000000e-01, -9.130000000e-01, /*EMAG*/  2.915476000e-02,  1.100000000e-02,  3.195309000e-02,  2.000000030e-01,  1.860000000e-01,  1.620000000e-01, /*DIAM*/  9.449111857e-01,  1.013728492e+00,  1.013946161e+00, /*EDIAM*/  5.544413062e-02,  4.496575488e-02,  3.672826177e-02, /*DMEAN*/  9.995073867e-01, /*EDMEAN*/  2.850684638e-02, /*CHI2*/  4.086335423e-01 },
            {70, /* SP "K0IIIa         " idx */ 200, /*MAG*/  3.410000000e+00,  2.240000000e+00,  1.110000000e+00,  3.110000000e-01, -2.100000000e-01, -3.030000000e-01, /*EMAG*/  2.000000000e-03,  2.000000000e-03,  2.000000000e-03,  1.980000000e-01,  1.600000000e-01,  1.700000000e-01, /*DIAM*/  7.606407879e-01,  7.642768240e-01,  7.551693564e-01, /*EDIAM*/  5.486992441e-02,  3.866636548e-02,  3.851998247e-02, /*DMEAN*/  7.598911155e-01, /*EDMEAN*/  5.394778229e-02, /*CHI2*/  8.640942047e-02 },
            {71, /* SP "M0III          " idx */ 240, /*MAG*/  3.646000000e+00,  2.070000000e+00,  3.300000000e-01, -9.570000000e-01, -1.674000000e+00, -1.846000000e+00, /*EMAG*/  1.077033000e-02,  4.000000000e-03,  3.026549000e-02,  2.000000030e-01,  1.480000000e-01,  1.600000000e-01, /*DIAM*/  1.154180919e+00,  1.172248446e+00,  1.155909685e+00, /*EDIAM*/  5.549592368e-02,  3.588190133e-02,  3.632185839e-02, /*DMEAN*/  1.162435730e+00, /*EDMEAN*/  5.215301918e-02, /*CHI2*/  8.180784079e-02 },
            {72, /* SP "F5Iab:         " idx */ 140, /*MAG*/  2.271000000e+00,  1.790000000e+00,  1.160000000e+00,  8.300000000e-01,  4.130000000e-01,  5.200000000e-01, /*EMAG*/  5.656854000e-03,  4.000000000e-03,  4.019950000e-02,  1.840000000e-01,  1.620000000e-01,  1.640000000e-01, /*DIAM*/  5.028587584e-01,  5.604393399e-01,  5.095917567e-01, /*EDIAM*/  5.103681428e-02,  3.921418585e-02,  3.719301983e-02, /*DMEAN*/  5.269423379e-01, /*EDMEAN*/  6.253450834e-02, /*CHI2*/  5.947402641e-01 },
            {73, /* SP "A2Iae          " idx */ 88, /*MAG*/  1.342000000e+00,  1.250000000e+00,  1.090000000e+00,  1.139000000e+00,  9.020000000e-01,  1.010000000e+00, /*EMAG*/  9.899495000e-03,  7.000000000e-03,  3.080584000e-02,  2.000000030e-01,  1.880000000e-01,  2.000000030e-01, /*DIAM*/  3.144175342e-01,  3.596268268e-01,  3.279837659e-01, /*EDIAM*/  5.586498666e-02,  4.580667822e-02,  4.555834370e-02, /*DMEAN*/  3.363854984e-01, /*EDMEAN*/  5.583903166e-02, /*CHI2*/  1.113838964e+00 },
            {74, /* SP "K5III          " idx */ 220, /*MAG*/  2.408000000e+00,  8.700000000e-01, -8.000000000e-01, -2.095000000e+00, -2.775000000e+00, -3.044000000e+00, /*EMAG*/  1.360147000e-02,  1.100000000e-02,  2.282542000e-02,  1.940000000e-01,  1.700000000e-01,  1.400000000e-01, /*DIAM*/  1.362298596e+00,  1.359031537e+00,  1.368203100e+00, /*EDIAM*/  5.381611392e-02,  4.112575189e-02,  3.178263650e-02, /*DMEAN*/  1.364332356e+00, /*EDMEAN*/  4.593647876e-02, /*CHI2*/  1.995287122e-01 },
            {75, /* SP "M4III          " idx */ 256, /*MAG*/  7.911000000e+00,  6.320000000e+00,  2.900000000e+00,  8.970000000e-01, -1.640000000e-01, -5.120000000e-01, /*EMAG*/  2.842534000e-02,  1.800000000e-02,  6.264184000e-02,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/  9.395321850e-01,  9.878673835e-01,  9.859160695e-01, /*EDIAM*/  5.561470925e-02,  4.853804425e-02,  4.542932273e-02, /*DMEAN*/  9.744354232e-01, /*EDMEAN*/  7.954329155e-02, /*CHI2*/  2.298548208e-01 },
            {76, /* SP "K0III          " idx */ 200, /*MAG*/  5.893000000e+00,  4.880000000e+00,  3.890000000e+00,  3.025000000e+00,  2.578000000e+00,  2.494000000e+00, /*EMAG*/  2.517936000e-02,  3.000000000e-03,  3.014963000e-02,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/  2.121586369e-01,  2.005725354e-01,  1.916438006e-01, /*EDIAM*/  5.542309987e-02,  4.830284756e-02,  4.530042645e-02, /*DMEAN*/  2.001049285e-01, /*EDMEAN*/  8.002947152e-02, /*CHI2*/  1.457040923e+00 },
            {77, /* SP "M6III          " idx */ 264, /*MAG*/  7.212000000e+00,  5.760000000e+00,  2.170000000e+00,  2.300000000e-01, -6.520000000e-01, -8.680000000e-01, /*EMAG*/  5.385165000e-02,  1.400000000e-02,  2.000000030e-01,  2.000000030e-01,  2.000000000e-01,  2.000000030e-01, /*DIAM*/  1.048959756e+00,  1.072381527e+00,  1.063134414e+00, /*EDIAM*/  5.713862410e-02,  5.016499596e-02,  4.649705455e-02, /*DMEAN*/  1.062596345e+00, /*EDMEAN*/  6.123769078e-02, /*CHI2*/  7.782681900e-02 },
            {78, /* SP "M3.0IIIa       " idx */ 252, /*MAG*/  4.491000000e+00,  2.870000000e+00,  5.700000000e-01, -9.120000000e-01, -1.608000000e+00, -1.862000000e+00, /*EMAG*/  9.219544000e-03,  6.000000000e-03,  5.035871000e-02,  1.880000000e-01,  1.900000000e-01,  1.800000000e-01, /*DIAM*/  1.186547604e+00,  1.195403174e+00,  1.195587294e+00, /*EDIAM*/  5.213692774e-02,  4.595499851e-02,  4.080505428e-02, /*DMEAN*/  1.193222111e+00, /*EDMEAN*/  3.731775798e-02, /*CHI2*/  1.331424304e-01 },
            {79, /* SP "M0III          " idx */ 240, /*MAG*/  6.618000000e+00,  5.090000000e+00,  3.490000000e+00,  2.308000000e+00,  1.494000000e+00,  1.285000000e+00, /*EMAG*/  6.403124000e-03,  5.000000000e-03,  5.024938000e-02,  2.000000030e-01,  1.560000000e-01,  2.000000000e-01, /*DIAM*/  4.823684086e-01,  5.325441564e-01,  5.267928869e-01, /*EDIAM*/  5.549640177e-02,  3.780299780e-02,  4.534958063e-02, /*DMEAN*/  5.199213670e-01, /*EDMEAN*/  7.858060851e-02, /*CHI2*/  3.498396403e+00 },
            {80, /* SP "K3IIIb         " idx */ 212, /*MAG*/  5.797000000e+00,  4.450000000e+00,  3.080000000e+00,  2.195000000e+00,  1.495000000e+00,  1.357000000e+00, /*EMAG*/  4.809366000e-02,  3.000000000e-03,  2.022375000e-02,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/  4.349169527e-01,  4.629602491e-01,  4.547954290e-01, /*EDIAM*/  5.544217557e-02,  4.831867364e-02,  4.531124159e-02, /*DMEAN*/  4.524029983e-01, /*EDMEAN*/  5.239260891e-02, /*CHI2*/  1.378197491e-01 },
            {81, /* SP "M2Iab:         " idx */ 248, /*MAG*/  6.380000000e+00,  4.320000000e+00,  1.780000000e+00,  3.930000000e-01, -6.090000000e-01, -9.130000000e-01, /*EMAG*/  2.915476000e-02,  1.100000000e-02,  3.195309000e-02,  2.000000030e-01,  1.860000000e-01,  1.620000000e-01, /*DIAM*/  9.449111857e-01,  1.013728492e+00,  1.013946161e+00, /*EDIAM*/  5.544413062e-02,  4.496575488e-02,  3.672826177e-02, /*DMEAN*/  9.995073867e-01, /*EDMEAN*/  2.552210007e-02, /*CHI2*/  4.635761933e-01 },
            {82, /* SP "K0III          " idx */ 200, /*MAG*/  5.023000000e+00,  3.940000000e+00,  2.930000000e+00,  2.209000000e+00,  1.685000000e+00,  1.567000000e+00, /*EMAG*/  5.830952000e-03,  3.000000000e-03,  2.022375000e-02,  2.000000030e-01,  1.780000000e-01,  2.000000030e-01, /*DIAM*/  3.658372106e-01,  3.772340167e-01,  3.767021975e-01, /*EDIAM*/  5.542309987e-02,  4.300209699e-02,  4.530042645e-02, /*DMEAN*/  3.743024768e-01, /*EDMEAN*/  5.198046360e-02, /*CHI2*/  1.377069639e-01 },
            {83, /* SP "M5III          " idx */ 260, /*MAG*/  8.550000000e+00,  7.140000000e+00,  3.500000000e+00,  1.294000000e+00,  2.910000000e-01, -4.000000000e-03, /*EMAG*/  1.835756000e-02,  9.000000000e-03,  4.100000000e-02,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/  8.782012236e-01,  9.081736783e-01,  8.979569024e-01, /*EDIAM*/  5.606035734e-02,  4.903218824e-02,  4.574803775e-02, /*DMEAN*/  8.962958399e-01, /*EDMEAN*/  2.920666683e-02, /*CHI2*/  5.079311425e-01 },
            {84, /* SP "M1III          " idx */ 244, /*MAG*/  8.299000000e+00,  6.530000000e+00,  4.250000000e+00,  2.857000000e+00,  1.994000000e+00,  1.740000000e+00, /*EMAG*/  7.217340000e-02,  5.000000000e-03,  2.061553000e-02,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/  4.380650884e-01,  4.747975743e-01,  4.667215786e-01, /*EDIAM*/  5.546791054e-02,  4.835636467e-02,  4.533110626e-02, /*DMEAN*/  4.619964518e-01, /*EDMEAN*/  4.673851265e-02, /*CHI2*/  2.421926683e-01 },
            {85, /* SP "K7III:         " idx */ 228, /*MAG*/  9.730000000e+00,  7.630000000e+00,  5.450000000e+00,  3.641000000e+00,  2.588000000e+00,  2.234000000e+00, /*EMAG*/  1.920937000e-02,  1.200000000e-02,  1.405133000e-01,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/  3.044613737e-01,  3.572984470e-01,  3.628696872e-01, /*EDIAM*/  5.550562221e-02,  4.837981709e-02,  4.534998806e-02, /*DMEAN*/  3.456422857e-01, /*EDMEAN*/  4.954792590e-02, /*CHI2*/  3.163629668e+00 },
            {86, /* SP "M4III          " idx */ 256, /*MAG*/  7.911000000e+00,  6.320000000e+00,  2.900000000e+00,  8.970000000e-01, -1.640000000e-01, -5.120000000e-01, /*EMAG*/  2.842534000e-02,  1.800000000e-02,  6.264184000e-02,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/  9.395321850e-01,  9.878673835e-01,  9.859160695e-01, /*EDIAM*/  5.561470925e-02,  4.853804425e-02,  4.542932273e-02, /*DMEAN*/  9.744354232e-01, /*EDMEAN*/  3.477557875e-02, /*CHI2*/  9.932302194e-01 },
            {87, /* SP "M3III          " idx */ 252, /*MAG*/  8.102000000e+00,  6.520000000e+00,  3.840000000e+00,  2.202000000e+00,  1.270000000e+00,  1.039000000e+00, /*EMAG*/  1.341641000e-02,  6.000000000e-03,  2.088061000e-02,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/  6.049047378e-01,  6.516444115e-01,  6.350690376e-01, /*EDIAM*/  5.545229501e-02,  4.836154354e-02,  4.532310140e-02, /*DMEAN*/  6.328771027e-01, /*EDMEAN*/  3.401876153e-02, /*CHI2*/  1.702379170e-01 },
            {88, /* SP "M1III          " idx */ 244, /*MAG*/  6.797000000e+00,  5.150000000e+00,  3.130000000e+00,  1.839000000e+00,  9.960000000e-01,  8.340000000e-01, /*EMAG*/  1.300000000e-02,  5.000000000e-03,  5.024938000e-02,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/  6.138686625e-01,  6.586419350e-01,  6.354661066e-01, /*EDIAM*/  5.546791054e-02,  4.835636467e-02,  4.533110626e-02, /*DMEAN*/  6.378018369e-01, /*EDMEAN*/  4.566922797e-02, /*CHI2*/  1.991388037e-01 },
            {89, /* SP "M3III          " idx */ 252, /*MAG*/  6.543000000e+00,  4.930000000e+00,  2.370000000e+00,  8.460000000e-01, -1.230000000e-01, -3.650000000e-01, /*EMAG*/  2.000000000e-02,  1.200000000e-02,  5.141984000e-02,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/  8.581368844e-01,  9.221191237e-01,  9.109814505e-01, /*EDIAM*/  5.545803634e-02,  4.836344300e-02,  4.532392409e-02, /*DMEAN*/  9.009693545e-01, /*EDMEAN*/  2.968550335e-02, /*CHI2*/  3.810512234e-01 },
            {90, /* SP "M2III          " idx */ 248, /*MAG*/  7.554000000e+00,  5.970000000e+00,  3.770000000e+00,  2.450000000e+00,  1.582000000e+00,  1.444000000e+00, /*EMAG*/  1.029563000e-02,  5.000000000e-03,  3.041381000e-02,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/  5.022593934e-01,  5.532148662e-01,  5.239680513e-01, /*EDIAM*/  5.543902597e-02,  4.833491930e-02,  4.531253805e-02, /*DMEAN*/  5.283681549e-01, /*EDMEAN*/  7.168890461e-02, /*CHI2*/  4.765620576e-01 },
            {91, /* SP "M2Iab:         " idx */ 248, /*MAG*/  6.380000000e+00,  4.320000000e+00,  1.780000000e+00,  3.930000000e-01, -6.090000000e-01, -9.130000000e-01, /*EMAG*/  2.915476000e-02,  1.100000000e-02,  3.195309000e-02,  2.000000030e-01,  1.860000000e-01,  1.620000000e-01, /*DIAM*/  9.449111857e-01,  1.013728492e+00,  1.013946161e+00, /*EDIAM*/  5.544413062e-02,  4.496575488e-02,  3.672826177e-02, /*DMEAN*/  9.995073867e-01, /*EDMEAN*/  4.570288714e-02, /*CHI2*/  6.482391277e-01 },
            {92, /* SP "M4III          " idx */ 256, /*MAG*/  7.911000000e+00,  6.320000000e+00,  2.900000000e+00,  8.970000000e-01, -1.640000000e-01, -5.120000000e-01, /*EMAG*/  2.842534000e-02,  1.800000000e-02,  6.264184000e-02,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/  9.395321850e-01,  9.878673835e-01,  9.859160695e-01, /*EDIAM*/  5.561470925e-02,  4.853804425e-02,  4.542932273e-02, /*DMEAN*/  9.744354232e-01, /*EDMEAN*/  4.712249663e-02, /*CHI2*/  1.382461953e+00 },
            {93, /* SP "M5III          " idx */ 260, /*MAG*/  7.246000000e+00,  5.780000000e+00,  3.010000000e+00,  1.429000000e+00,  5.260000000e-01,  2.550000000e-01, /*EMAG*/  1.000000000e-02,  6.000000000e-03,  2.088061000e-02,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/  7.364065786e-01,  7.953876844e-01,  8.036138353e-01, /*EDIAM*/  5.605799090e-02,  4.903140760e-02,  4.574769814e-02, /*DMEAN*/  7.831934094e-01, /*EDMEAN*/  4.593358022e-02, /*CHI2*/  1.334851576e+00 },
            {94, /* SP "K5III          " idx */ 220, /*MAG*/  2.408000000e+00,  8.700000000e-01, -8.000000000e-01, -2.095000000e+00, -2.775000000e+00, -3.044000000e+00, /*EMAG*/  1.360147000e-02,  1.100000000e-02,  2.282542000e-02,  1.940000000e-01,  1.700000000e-01,  1.400000000e-01, /*DIAM*/  1.362298596e+00,  1.359031537e+00,  1.368203100e+00, /*EDIAM*/  5.381611392e-02,  4.112575189e-02,  3.178263650e-02, /*DMEAN*/  1.364332356e+00, /*EDMEAN*/  2.300636515e-02, /*CHI2*/  2.307493635e-01 },
            {95, /* SP "M2Iab:         " idx */ 248, /*MAG*/  6.380000000e+00,  4.320000000e+00,  1.780000000e+00,  3.930000000e-01, -6.090000000e-01, -9.130000000e-01, /*EMAG*/  2.915476000e-02,  1.100000000e-02,  3.195309000e-02,  2.000000030e-01,  1.860000000e-01,  1.620000000e-01, /*DIAM*/  9.449111857e-01,  1.013728492e+00,  1.013946161e+00, /*EDIAM*/  5.544413062e-02,  4.496575488e-02,  3.672826177e-02, /*DMEAN*/  9.995073867e-01, /*EDMEAN*/  2.676701311e-02, /*CHI2*/  4.198306449e-01 },
            {96, /* SP "K1IIIb         " idx */ 204, /*MAG*/  3.161000000e+00,  2.010000000e+00,  8.800000000e-01, -1.960000000e-01, -7.490000000e-01, -7.830000000e-01, /*EMAG*/  7.211103000e-03,  4.000000000e-03,  4.000000000e-03,  2.000000030e-01,  1.560000000e-01,  1.760000000e-01, /*DIAM*/  8.922131942e-01,  8.907645204e-01,  8.633529261e-01, /*EDIAM*/  5.542796088e-02,  3.770730718e-02,  3.987875299e-02, /*DMEAN*/  8.806498809e-01, /*EDMEAN*/  5.650041524e-02, /*CHI2*/  1.038785682e-01 },
            {97, /* SP "K4III          " idx */ 216, /*MAG*/  3.535000000e+00,  2.070000000e+00,  6.100000000e-01, -4.950000000e-01, -1.225000000e+00, -1.287000000e+00, /*EMAG*/  5.830952000e-03,  3.000000000e-03,  3.014963000e-02,  1.940000000e-01,  1.840000000e-01,  2.000000030e-01, /*DIAM*/  1.004548647e+00,  1.027761480e+00,  9.963665933e-01, /*EDIAM*/  5.379558896e-02,  4.447676085e-02,  4.531819496e-02, /*DMEAN*/  1.010342324e+00, /*EDMEAN*/  5.547416982e-02, /*CHI2*/  4.276822503e-01 },
            {98, /* SP "K5III          " idx */ 220, /*MAG*/  3.761000000e+00,  2.240000000e+00,  7.000000000e-01, -2.450000000e-01, -1.034000000e+00, -1.162000000e+00, /*EMAG*/  5.830952000e-03,  3.000000000e-03,  3.014963000e-02,  2.000000030e-01,  1.800000000e-01,  1.600000000e-01, /*DIAM*/  9.554414469e-01,  9.955295857e-01,  9.783490792e-01, /*EDIAM*/  5.546729371e-02,  4.352969319e-02,  3.629417771e-02, /*DMEAN*/  9.793578021e-01, /*EDMEAN*/  4.230364964e-02, /*CHI2*/  2.775873400e-01 },
            {99, /* SP "G9III          " idx */ 196, /*MAG*/  4.060000000e+00,  3.070000000e+00,  2.130000000e+00,  1.375000000e+00,  8.370000000e-01,  8.830000000e-01, /*EMAG*/  1.811077000e-02,  2.000000000e-03,  3.006659000e-02,  2.000000030e-01,  1.660000000e-01,  1.980000000e-01, /*DIAM*/  5.209947159e-01,  5.404227382e-01,  5.031841988e-01, /*EDIAM*/  5.541964649e-02,  4.010940873e-02,  4.484633782e-02, /*DMEAN*/  5.232269392e-01, /*EDMEAN*/  4.308811491e-02, /*CHI2*/  7.523930246e-01 },
            {100, /* SP "M0III          " idx */ 240, /*MAG*/  3.646000000e+00,  2.070000000e+00,  3.300000000e-01, -9.570000000e-01, -1.674000000e+00, -1.846000000e+00, /*EMAG*/  1.077033000e-02,  4.000000000e-03,  3.026549000e-02,  2.000000030e-01,  1.480000000e-01,  1.600000000e-01, /*DIAM*/  1.154180919e+00,  1.172248446e+00,  1.155909685e+00, /*EDIAM*/  5.549592368e-02,  3.588190133e-02,  3.632185839e-02, /*DMEAN*/  1.162435730e+00, /*EDMEAN*/  2.722627221e-02, /*CHI2*/  2.758410653e-01 },
            {101, /* SP "K5III          " idx */ 220, /*MAG*/  2.408000000e+00,  8.700000000e-01, -8.000000000e-01, -2.095000000e+00, -2.775000000e+00, -3.044000000e+00, /*EMAG*/  1.360147000e-02,  1.100000000e-02,  2.282542000e-02,  1.940000000e-01,  1.700000000e-01,  1.400000000e-01, /*DIAM*/  1.362298596e+00,  1.359031537e+00,  1.368203100e+00, /*EDIAM*/  5.381611392e-02,  4.112575189e-02,  3.178263650e-02, /*DMEAN*/  1.364332356e+00, /*EDMEAN*/  2.424132964e-02, /*CHI2*/  8.080722381e-01 },
            {102, /* SP "M3.0IIIa       " idx */ 252, /*MAG*/  4.491000000e+00,  2.870000000e+00,  5.700000000e-01, -9.120000000e-01, -1.608000000e+00, -1.862000000e+00, /*EMAG*/  9.219544000e-03,  6.000000000e-03,  5.035871000e-02,  1.880000000e-01,  1.900000000e-01,  1.800000000e-01, /*DIAM*/  1.186547604e+00,  1.195403174e+00,  1.195587294e+00, /*EDIAM*/  5.213692774e-02,  4.595499851e-02,  4.080505428e-02, /*DMEAN*/  1.193222111e+00, /*EDMEAN*/  3.247356409e-02, /*CHI2*/  5.379957707e-02 },
            {103, /* SP "K5III          " idx */ 220, /*MAG*/  3.761000000e+00,  2.240000000e+00,  7.000000000e-01, -2.450000000e-01, -1.034000000e+00, -1.162000000e+00, /*EMAG*/  5.830952000e-03,  3.000000000e-03,  3.014963000e-02,  2.000000030e-01,  1.800000000e-01,  1.600000000e-01, /*DIAM*/  9.554414469e-01,  9.955295857e-01,  9.783490792e-01, /*EDIAM*/  5.546729371e-02,  4.352969319e-02,  3.629417771e-02, /*DMEAN*/  9.793578021e-01, /*EDMEAN*/  6.328122767e-02, /*CHI2*/  2.410428891e-01 },
            {104, /* SP "K5III          " idx */ 220, /*MAG*/  2.408000000e+00,  8.700000000e-01, -8.000000000e-01, -2.095000000e+00, -2.775000000e+00, -3.044000000e+00, /*EMAG*/  1.360147000e-02,  1.100000000e-02,  2.282542000e-02,  1.940000000e-01,  1.700000000e-01,  1.400000000e-01, /*DIAM*/  1.362298596e+00,  1.359031537e+00,  1.368203100e+00, /*EDIAM*/  5.381611392e-02,  4.112575189e-02,  3.178263650e-02, /*DMEAN*/  1.364332356e+00, /*EDMEAN*/  2.370585701e-02, /*CHI2*/  1.970242909e+00 },
            {105, /* SP "M5IIIv         " idx */ 260, /*MAG*/  9.482000000e+00,  8.000000000e+00,  4.820000000e+00,  2.148000000e+00,  1.292000000e+00,  1.006000000e+00, /*EMAG*/  2.773085000e-02,  1.200000000e-02,  1.205985000e-01,  2.000000030e-01,  1.540000000e-01,  2.000000030e-01, /*DIAM*/  7.078619353e-01,  7.021581110e-01,  6.920152935e-01, /*EDIAM*/  5.606367020e-02,  3.816472952e-02,  4.574851319e-02, /*DMEAN*/  7.001090629e-01, /*EDMEAN*/  5.262886003e-02, /*CHI2*/  2.169702115e-01 },
            {106, /* SP "G8Ib           " idx */ 192, /*MAG*/  4.437000000e+00,  3.060000000e+00,  1.840000000e+00,  8.860000000e-01,  2.320000000e-01,  1.250000000e-01, /*EMAG*/  1.513275000e-02,  2.000000000e-03,  1.019804000e-02,  2.000000030e-01,  1.540000000e-01,  1.720000000e-01, /*DIAM*/  6.468827577e-01,  6.809498324e-01,  6.692447512e-01, /*EDIAM*/  5.541761655e-02,  3.721861252e-02,  3.896781983e-02, /*DMEAN*/  6.699340477e-01, /*EDMEAN*/  2.592776990e-02, /*CHI2*/  9.654073442e-02 },
            {107, /* SP "M1.5IIIb       " idx */ 246, /*MAG*/  7.126000000e+00,  5.430000000e+00,  3.520000000e+00,  1.960000000e+00,  1.002000000e+00,  9.590000000e-01, /*EMAG*/  1.992486000e-02,  6.000000000e-03,  5.035871000e-02,  2.000000030e-01,  1.880000000e-01,  1.820000000e-01, /*DIAM*/  5.994810470e-01,  6.699913287e-01,  6.170341482e-01, /*EDIAM*/  5.545235759e-02,  4.545464564e-02,  4.125415361e-02, /*DMEAN*/  6.312992523e-01, /*EDMEAN*/  4.375832049e-02, /*CHI2*/  5.220447894e-01 },
            {108, /* SP "M1III          " idx */ 244, /*MAG*/  5.541000000e+00,  4.040000000e+00,  2.250000000e+00,  1.176000000e+00,  3.250000000e-01,  1.570000000e-01, /*EMAG*/  3.224903000e-02,  4.000000000e-03,  2.039608000e-02,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/  7.121454496e-01,  7.747353220e-01,  7.594880063e-01, /*EDIAM*/  5.546743221e-02,  4.835620636e-02,  4.533103772e-02, /*DMEAN*/  7.523389713e-01, /*EDMEAN*/  3.109731134e-02, /*CHI2*/  2.860578743e-01 },
            {109, /* SP "M5III          " idx */ 260, /*MAG*/  8.550000000e+00,  7.140000000e+00,  3.500000000e+00,  1.294000000e+00,  2.910000000e-01, -4.000000000e-03, /*EMAG*/  1.835756000e-02,  9.000000000e-03,  4.100000000e-02,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/  8.782012236e-01,  9.081736783e-01,  8.979569024e-01, /*EDIAM*/  5.606035734e-02,  4.903218824e-02,  4.574803775e-02, /*DMEAN*/  8.962958399e-01, /*EDMEAN*/  3.207850881e-02, /*CHI2*/  3.342680949e+00 },
            {110, /* SP "K5III          " idx */ 220, /*MAG*/  5.620000000e+00,  4.050000000e+00,  2.330000000e+00,  1.261000000e+00,  4.290000000e-01,  3.100000000e-01, /*EMAG*/  3.000000000e-03,  3.000000000e-03,  2.022375000e-02,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/  6.775842999e-01,  7.172416438e-01,  6.928308267e-01, /*EDIAM*/  5.546729371e-02,  4.834406156e-02,  4.532734548e-02, /*DMEAN*/  6.972605051e-01, /*EDMEAN*/  3.336745938e-02, /*CHI2*/  5.985633310e-01 },
            {111, /* SP "M0III          " idx */ 240, /*MAG*/  3.646000000e+00,  2.070000000e+00,  3.300000000e-01, -9.570000000e-01, -1.674000000e+00, -1.846000000e+00, /*EMAG*/  1.077033000e-02,  4.000000000e-03,  3.026549000e-02,  2.000000030e-01,  1.480000000e-01,  1.600000000e-01, /*DIAM*/  1.154180919e+00,  1.172248446e+00,  1.155909685e+00, /*EDIAM*/  5.549592368e-02,  3.588190133e-02,  3.632185839e-02, /*DMEAN*/  1.162435730e+00, /*EDMEAN*/  2.380981613e-02, /*CHI2*/  2.351302644e-01 },
            {112, /* SP "M4II           " idx */ 256, /*MAG*/  4.848000000e+00,  3.320000000e+00,  5.600000000e-01, -7.790000000e-01, -1.675000000e+00, -1.904000000e+00, /*EMAG*/  1.345362000e-02,  9.000000000e-03,  6.067125000e-02,  1.820000000e-01,  1.580000000e-01,  1.520000000e-01, /*DIAM*/  1.173067903e+00,  1.228653379e+00,  1.222062058e+00, /*EDIAM*/  5.064375999e-02,  3.847655000e-02,  3.462016529e-02, /*DMEAN*/  1.214380478e+00, /*EDMEAN*/  2.344648200e-02, /*CHI2*/  4.772388841e-01 },
            {113, /* SP "K5III          " idx */ 220, /*MAG*/  2.408000000e+00,  8.700000000e-01, -8.000000000e-01, -2.095000000e+00, -2.775000000e+00, -3.044000000e+00, /*EMAG*/  1.360147000e-02,  1.100000000e-02,  2.282542000e-02,  1.940000000e-01,  1.700000000e-01,  1.400000000e-01, /*DIAM*/  1.362298596e+00,  1.359031537e+00,  1.368203100e+00, /*EDIAM*/  5.381611392e-02,  4.112575189e-02,  3.178263650e-02, /*DMEAN*/  1.364332356e+00, /*EDMEAN*/  2.361533474e-02, /*CHI2*/  1.412672483e+00 },
            {114, /* SP "M3.0IIIa       " idx */ 252, /*MAG*/  4.491000000e+00,  2.870000000e+00,  5.700000000e-01, -9.120000000e-01, -1.608000000e+00, -1.862000000e+00, /*EMAG*/  9.219544000e-03,  6.000000000e-03,  5.035871000e-02,  1.880000000e-01,  1.900000000e-01,  1.800000000e-01, /*DIAM*/  1.186547604e+00,  1.195403174e+00,  1.195587294e+00, /*EDIAM*/  5.213692774e-02,  4.595499851e-02,  4.080505428e-02, /*DMEAN*/  1.193222111e+00, /*EDMEAN*/  2.671678448e-02, /*CHI2*/  5.248429497e-02 },
            {115, /* SP "K5III          " idx */ 220, /*MAG*/  3.761000000e+00,  2.240000000e+00,  7.000000000e-01, -2.450000000e-01, -1.034000000e+00, -1.162000000e+00, /*EMAG*/  5.830952000e-03,  3.000000000e-03,  3.014963000e-02,  2.000000030e-01,  1.800000000e-01,  1.600000000e-01, /*DIAM*/  9.554414469e-01,  9.955295857e-01,  9.783490792e-01, /*EDIAM*/  5.546729371e-02,  4.352969319e-02,  3.629417771e-02, /*DMEAN*/  9.793578021e-01, /*EDMEAN*/  2.681634088e-02, /*CHI2*/  4.277840914e-01 },
            {116, /* SP "M2.5II-IIIe    " idx */ 250, /*MAG*/  4.095000000e+00,  2.440000000e+00,  1.300000000e-01, -1.432000000e+00, -2.129000000e+00, -2.379000000e+00, /*EMAG*/  1.280625000e-02,  8.000000000e-03,  6.053098000e-02,  1.940000000e-01,  1.800000000e-01,  1.540000000e-01, /*DIAM*/  1.301933759e+00,  1.303342579e+00,  1.298761056e+00, /*EDIAM*/  5.377975122e-02,  4.352332154e-02,  3.491941172e-02, /*DMEAN*/  1.300835543e+00, /*EDMEAN*/  2.505622228e-02, /*CHI2*/  3.135747747e+00 },
            {117, /* SP "K4III          " idx */ 216, /*MAG*/  6.212000000e+00,  4.840000000e+00,  3.420000000e+00,  2.521000000e+00,  1.801000000e+00,  1.663000000e+00, /*EMAG*/  8.544004000e-03,  3.000000000e-03,  2.022375000e-02,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/  3.824593523e-01,  4.120027156e-01,  4.016366575e-01, /*EDIAM*/  5.545333446e-02,  4.832968341e-02,  4.531819496e-02, /*DMEAN*/  4.001880100e-01, /*EDMEAN*/  5.818403977e-02, /*CHI2*/  1.823568665e+00 },
            {118, /* SP "G8Ib           " idx */ 192, /*MAG*/  4.437000000e+00,  3.060000000e+00,  1.840000000e+00,  8.860000000e-01,  2.320000000e-01,  1.250000000e-01, /*EMAG*/  1.513275000e-02,  2.000000000e-03,  1.019804000e-02,  2.000000030e-01,  1.540000000e-01,  1.720000000e-01, /*DIAM*/  6.468827577e-01,  6.809498324e-01,  6.692447512e-01, /*EDIAM*/  5.541761655e-02,  3.721861252e-02,  3.896781983e-02, /*DMEAN*/  6.699340477e-01, /*EDMEAN*/  4.939485068e-02, /*CHI2*/  2.357259292e-01 },
            {119, /* SP "K0IIIa         " idx */ 200, /*MAG*/  3.410000000e+00,  2.240000000e+00,  1.110000000e+00,  3.110000000e-01, -2.100000000e-01, -3.030000000e-01, /*EMAG*/  2.000000000e-03,  2.000000000e-03,  2.000000000e-03,  1.980000000e-01,  1.600000000e-01,  1.700000000e-01, /*DIAM*/  7.606407879e-01,  7.642768240e-01,  7.551693564e-01, /*EDIAM*/  5.486992441e-02,  3.866636548e-02,  3.851998247e-02, /*DMEAN*/  7.598911155e-01, /*EDMEAN*/  4.068373421e-02, /*CHI2*/  2.157425425e-01 },
            {120, /* SP "M0III          " idx */ 240, /*MAG*/  3.646000000e+00,  2.070000000e+00,  3.300000000e-01, -9.570000000e-01, -1.674000000e+00, -1.846000000e+00, /*EMAG*/  1.077033000e-02,  4.000000000e-03,  3.026549000e-02,  2.000000030e-01,  1.480000000e-01,  1.600000000e-01, /*DIAM*/  1.154180919e+00,  1.172248446e+00,  1.155909685e+00, /*EDIAM*/  5.549592368e-02,  3.588190133e-02,  3.632185839e-02, /*DMEAN*/  1.162435730e+00, /*EDMEAN*/  2.397305245e-02, /*CHI2*/  8.042500523e-02 },
            {121, /* SP "K3III          " idx */ 212, /*MAG*/  4.865000000e+00,  3.590000000e+00,  2.360000000e+00,  1.420000000e+00,  7.410000000e-01,  6.490000000e-01, /*EMAG*/  4.472136000e-03,  2.000000000e-03,  2.009975000e-02,  2.000000030e-01,  1.540000000e-01,  2.000000030e-01, /*DIAM*/  5.833901692e-01,  6.093882668e-01,  5.924012705e-01, /*EDIAM*/  5.544190970e-02,  3.724204128e-02,  4.531120349e-02, /*DMEAN*/  5.984804122e-01, /*EDMEAN*/  4.316512000e-02, /*CHI2*/  1.849000834e+00 },
            {122, /* SP "K1IIIb         " idx */ 204, /*MAG*/  3.161000000e+00,  2.010000000e+00,  8.800000000e-01, -1.960000000e-01, -7.490000000e-01, -7.830000000e-01, /*EMAG*/  7.211103000e-03,  4.000000000e-03,  4.000000000e-03,  2.000000030e-01,  1.560000000e-01,  1.760000000e-01, /*DIAM*/  8.922131942e-01,  8.907645204e-01,  8.633529261e-01, /*EDIAM*/  5.542796088e-02,  3.770730718e-02,  3.987875299e-02, /*DMEAN*/  8.806498809e-01, /*EDMEAN*/  3.470924895e-02, /*CHI2*/  1.344210396e+00 },
            {123, /* SP "K4.5III        " idx */ 218, /*MAG*/  5.800000000e+00,  4.250000000e+00,  2.640000000e+00,  1.263000000e+00,  4.530000000e-01,  3.750000000e-01, /*EMAG*/  4.242641000e-03,  3.000000000e-03,  3.014963000e-02,  2.000000030e-01,  1.660000000e-01,  1.780000000e-01, /*DIAM*/  6.889837218e-01,  7.162850744e-01,  6.804855455e-01, /*EDIAM*/  5.545999202e-02,  4.015201556e-02,  4.035267580e-02, /*DMEAN*/  6.964975064e-01, /*EDMEAN*/  3.362048397e-02, /*CHI2*/  1.770111630e+00 },
            {124, /* SP "K7III          " idx */ 228, /*MAG*/  4.690000000e+00,  3.140000000e+00,  1.490000000e+00,  1.200000000e-01, -5.780000000e-01, -6.560000000e-01, /*EMAG*/  8.944272000e-03,  4.000000000e-03,  3.026549000e-02,  2.000000030e-01,  1.700000000e-01,  1.720000000e-01, /*DIAM*/  9.342560259e-01,  9.358898953e-01,  8.988258996e-01, /*EDIAM*/  5.549882344e-02,  4.116281821e-02,  3.902843436e-02, /*DMEAN*/  9.200622442e-01, /*EDMEAN*/  2.690875198e-02, /*CHI2*/  1.433817280e+00 },
            {125, /* SP "K3II           " idx */ 212, /*MAG*/  4.597000000e+00,  3.160000000e+00,  1.850000000e+00,  7.240000000e-01, -1.120000000e-01, -2.300000000e-02, /*EMAG*/  2.000000000e-03,  2.000000000e-03,  2.000000000e-03,  1.780000000e-01,  1.780000000e-01,  1.600000000e-01, /*DIAM*/  7.430151716e-01,  7.974349622e-01,  7.331603967e-01, /*EDIAM*/  4.936308091e-02,  4.301977428e-02,  3.627401617e-02, /*DMEAN*/  7.558225402e-01, /*EDMEAN*/  3.770157848e-02, /*CHI2*/  5.896187800e-01 },
            {126, /* SP "K5III          " idx */ 220, /*MAG*/  3.761000000e+00,  2.240000000e+00,  7.000000000e-01, -2.450000000e-01, -1.034000000e+00, -1.162000000e+00, /*EMAG*/  5.830952000e-03,  3.000000000e-03,  3.014963000e-02,  2.000000030e-01,  1.800000000e-01,  1.600000000e-01, /*DIAM*/  9.554414469e-01,  9.955295857e-01,  9.783490792e-01, /*EDIAM*/  5.546729371e-02,  4.352969319e-02,  3.629417771e-02, /*DMEAN*/  9.793578021e-01, /*EDMEAN*/  2.624188621e-02, /*CHI2*/  1.187896410e+00 },
            {127, /* SP "M0III          " idx */ 240, /*MAG*/  5.942000000e+00,  4.440000000e+00,  2.760000000e+00,  1.714000000e+00,  8.780000000e-01,  7.110000000e-01, /*EMAG*/  4.472136000e-03,  2.000000000e-03,  2.009975000e-02,  2.000000030e-01,  1.700000000e-01,  2.000000030e-01, /*DIAM*/  5.968684103e-01,  6.543418236e-01,  6.395958083e-01, /*EDIAM*/  5.549528622e-02,  4.116615276e-02,  4.534942143e-02, /*DMEAN*/  6.359149709e-01, /*EDMEAN*/  4.082390465e-02, /*CHI2*/  2.879833330e+00 },
            {128, /* SP "M2.5II-IIIe    " idx */ 250, /*MAG*/  4.095000000e+00,  2.440000000e+00,  1.300000000e-01, -1.432000000e+00, -2.129000000e+00, -2.379000000e+00, /*EMAG*/  1.280625000e-02,  8.000000000e-03,  6.053098000e-02,  1.940000000e-01,  1.800000000e-01,  1.540000000e-01, /*DIAM*/  1.301933759e+00,  1.303342579e+00,  1.298761056e+00, /*EDIAM*/  5.377975122e-02,  4.352332154e-02,  3.491941172e-02, /*DMEAN*/  1.300835543e+00, /*EDMEAN*/  2.767621301e-02, /*CHI2*/  1.256261793e-02 },
            {129, /* SP "M1.5Iab-b      " idx */ 246, /*MAG*/  2.925000000e+00,  1.060000000e+00, -1.840000000e+00, -2.850000000e+00, -3.725000000e+00, -4.100000000e+00, /*EMAG*/  1.612452000e-02,  8.000000000e-03,  3.104835000e-02,  9.300000000e-02,  1.460000000e-01,  1.600000000e-01, /*DIAM*/  1.595266776e+00,  1.630115857e+00,  1.646939273e+00, /*EDIAM*/  2.594838159e-02,  3.535185798e-02,  3.628615203e-02, /*DMEAN*/  1.617291150e+00, /*EDMEAN*/  1.821024091e-02, /*CHI2*/  1.095773948e+00 },
            {130, /* SP "K0IIIa         " idx */ 200, /*MAG*/  3.410000000e+00,  2.240000000e+00,  1.110000000e+00,  3.110000000e-01, -2.100000000e-01, -3.030000000e-01, /*EMAG*/  2.000000000e-03,  2.000000000e-03,  2.000000000e-03,  1.980000000e-01,  1.600000000e-01,  1.700000000e-01, /*DIAM*/  7.606407879e-01,  7.642768240e-01,  7.551693564e-01, /*EDIAM*/  5.486992441e-02,  3.866636548e-02,  3.851998247e-02, /*DMEAN*/  7.598911155e-01, /*EDMEAN*/  2.480967744e-02, /*CHI2*/  1.014002075e-01 },
            {131, /* SP "M0III          " idx */ 240, /*MAG*/  3.646000000e+00,  2.070000000e+00,  3.300000000e-01, -9.570000000e-01, -1.674000000e+00, -1.846000000e+00, /*EMAG*/  1.077033000e-02,  4.000000000e-03,  3.026549000e-02,  2.000000030e-01,  1.480000000e-01,  1.600000000e-01, /*DIAM*/  1.154180919e+00,  1.172248446e+00,  1.155909685e+00, /*EDIAM*/  5.549592368e-02,  3.588190133e-02,  3.632185839e-02, /*DMEAN*/  1.162435730e+00, /*EDMEAN*/  2.356997617e-02, /*CHI2*/  9.264468096e-02 },
            {132, /* SP "K1IIIb         " idx */ 204, /*MAG*/  3.161000000e+00,  2.010000000e+00,  8.800000000e-01, -1.960000000e-01, -7.490000000e-01, -7.830000000e-01, /*EMAG*/  7.211103000e-03,  4.000000000e-03,  4.000000000e-03,  2.000000030e-01,  1.560000000e-01,  1.760000000e-01, /*DIAM*/  8.922131942e-01,  8.907645204e-01,  8.633529261e-01, /*EDIAM*/  5.542796088e-02,  3.770730718e-02,  3.987875299e-02, /*DMEAN*/  8.806498809e-01, /*EDMEAN*/  6.235465061e-02, /*CHI2*/  2.889873151e-01 },
            {133, /* SP "K5III          " idx */ 220, /*MAG*/  2.408000000e+00,  8.700000000e-01, -8.000000000e-01, -2.095000000e+00, -2.775000000e+00, -3.044000000e+00, /*EMAG*/  1.360147000e-02,  1.100000000e-02,  2.282542000e-02,  1.940000000e-01,  1.700000000e-01,  1.400000000e-01, /*DIAM*/  1.362298596e+00,  1.359031537e+00,  1.368203100e+00, /*EDIAM*/  5.381611392e-02,  4.112575189e-02,  3.178263650e-02, /*DMEAN*/  1.364332356e+00, /*EDMEAN*/  2.316340004e-02, /*CHI2*/  7.650665462e-01 },
            {134, /* SP "K0III-IV       " idx */ 200, /*MAG*/  3.501000000e+00,  2.480000000e+00,  1.480000000e+00,  6.410000000e-01,  1.040000000e-01, -7.000000000e-03, /*EMAG*/  1.513275000e-02,  2.000000000e-03,  2.009975000e-02,  2.000000030e-01,  1.600000000e-01,  2.000000030e-01, /*DIAM*/  6.877300725e-01,  6.984246829e-01,  6.944978227e-01, /*EDIAM*/  5.542283391e-02,  3.866636548e-02,  4.530038834e-02, /*DMEAN*/  6.947847039e-01, /*EDMEAN*/  2.623973864e-02, /*CHI2*/  4.701647664e-01 },
            {135, /* SP "M2.5II-IIIe    " idx */ 250, /*MAG*/  4.095000000e+00,  2.440000000e+00,  1.300000000e-01, -1.432000000e+00, -2.129000000e+00, -2.379000000e+00, /*EMAG*/  1.280625000e-02,  8.000000000e-03,  6.053098000e-02,  1.940000000e-01,  1.800000000e-01,  1.540000000e-01, /*DIAM*/  1.301933759e+00,  1.303342579e+00,  1.298761056e+00, /*EDIAM*/  5.377975122e-02,  4.352332154e-02,  3.491941172e-02, /*DMEAN*/  1.300835543e+00, /*EDMEAN*/  2.437548560e-02, /*CHI2*/  3.737018838e-01 },
            {136, /* SP "M5III          " idx */ 260, /*MAG*/  9.985000000e+00,  8.420000000e+00,  4.470000000e+00,  2.159000000e+00,  1.224000000e+00,  9.010000000e-01, /*EMAG*/  3.492850000e-02,  1.400000000e-02,  4.237924000e-02,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/  7.370672929e-01,  7.358857379e-01,  7.268109144e-01, /*EDIAM*/  5.606640448e-02,  4.903418315e-02,  4.574890562e-02, /*DMEAN*/  7.326132009e-01, /*EDMEAN*/  5.157131155e-02, /*CHI2*/  9.599559691e-02 },
            {137, /* SP "K4IIIb         " idx */ 216, /*MAG*/  5.940000000e+00,  4.440000000e+00,  2.860000000e+00,  2.031000000e+00,  1.198000000e+00,  1.105000000e+00, /*EMAG*/  8.544004000e-03,  3.000000000e-03,  2.022375000e-02,  2.000000030e-01,  1.680000000e-01,  1.740000000e-01, /*DIAM*/  4.873700681e-01,  5.409754802e-01,  5.173884840e-01, /*EDIAM*/  5.545333446e-02,  4.062516806e-02,  3.944433866e-02, /*DMEAN*/  5.202713349e-01, /*EDMEAN*/  3.103771007e-02, /*CHI2*/  3.070866079e+00 },
            {138, /* SP "M2III          " idx */ 248, /*MAG*/  7.554000000e+00,  5.970000000e+00,  3.770000000e+00,  2.450000000e+00,  1.582000000e+00,  1.444000000e+00, /*EMAG*/  1.029563000e-02,  5.000000000e-03,  3.041381000e-02,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/  5.022593934e-01,  5.532148662e-01,  5.239680513e-01, /*EDIAM*/  5.543902597e-02,  4.833491930e-02,  4.531253805e-02, /*DMEAN*/  5.283681549e-01, /*EDMEAN*/  3.545090162e-02, /*CHI2*/  2.169045828e-01 },
            {139, /* SP "G8Ib           " idx */ 192, /*MAG*/  4.437000000e+00,  3.060000000e+00,  1.840000000e+00,  8.860000000e-01,  2.320000000e-01,  1.250000000e-01, /*EMAG*/  1.513275000e-02,  2.000000000e-03,  1.019804000e-02,  2.000000030e-01,  1.540000000e-01,  1.720000000e-01, /*DIAM*/  6.468827577e-01,  6.809498324e-01,  6.692447512e-01, /*EDIAM*/  5.541761655e-02,  3.721861252e-02,  3.896781983e-02, /*DMEAN*/  6.699340477e-01, /*EDMEAN*/  2.566308428e-02, /*CHI2*/  1.007003371e+00 },
            {140, /* SP "M2III          " idx */ 248, /*MAG*/  6.993000000e+00,  5.350000000e+00,  3.230000000e+00,  1.876000000e+00,  1.037000000e+00,  8.150000000e-01, /*EMAG*/  1.334166000e-02,  3.000000000e-03,  5.008992000e-02,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/  6.135272522e-01,  6.591214825e-01,  6.500045496e-01, /*EDIAM*/  5.543817515e-02,  4.833463774e-02,  4.531241614e-02, /*DMEAN*/  6.435861822e-01, /*EDMEAN*/  4.702032456e-02, /*CHI2*/  3.263773675e-01 },
            {141, /* SP "M0III          " idx */ 240, /*MAG*/  3.646000000e+00,  2.070000000e+00,  3.300000000e-01, -9.570000000e-01, -1.674000000e+00, -1.846000000e+00, /*EMAG*/  1.077033000e-02,  4.000000000e-03,  3.026549000e-02,  2.000000030e-01,  1.480000000e-01,  1.600000000e-01, /*DIAM*/  1.154180919e+00,  1.172248446e+00,  1.155909685e+00, /*EDIAM*/  5.549592368e-02,  3.588190133e-02,  3.632185839e-02, /*DMEAN*/  1.162435730e+00, /*EDMEAN*/  2.386134177e-02, /*CHI2*/  4.389417966e-02 },
            {142, /* SP "M4II           " idx */ 256, /*MAG*/  4.848000000e+00,  3.320000000e+00,  5.600000000e-01, -7.790000000e-01, -1.675000000e+00, -1.904000000e+00, /*EMAG*/  1.345362000e-02,  9.000000000e-03,  6.067125000e-02,  1.820000000e-01,  1.580000000e-01,  1.520000000e-01, /*DIAM*/  1.173067903e+00,  1.228653379e+00,  1.222062058e+00, /*EDIAM*/  5.064375999e-02,  3.847655000e-02,  3.462016529e-02, /*DMEAN*/  1.214380478e+00, /*EDMEAN*/  2.410586105e-02, /*CHI2*/  8.914929005e-01 },
            {143, /* SP "K5III          " idx */ 220, /*MAG*/  2.408000000e+00,  8.700000000e-01, -8.000000000e-01, -2.095000000e+00, -2.775000000e+00, -3.044000000e+00, /*EMAG*/  1.360147000e-02,  1.100000000e-02,  2.282542000e-02,  1.940000000e-01,  1.700000000e-01,  1.400000000e-01, /*DIAM*/  1.362298596e+00,  1.359031537e+00,  1.368203100e+00, /*EDIAM*/  5.381611392e-02,  4.112575189e-02,  3.178263650e-02, /*DMEAN*/  1.364332356e+00, /*EDMEAN*/  2.298437270e-02, /*CHI2*/  3.054689122e-01 },
            {144, /* SP "M3II           " idx */ 252, /*MAG*/  6.001000000e+00,  4.300000000e+00,  1.790000000e+00,  2.450000000e-01, -6.020000000e-01, -8.130000000e-01, /*EMAG*/  1.486607000e-02,  5.000000000e-03,  3.041381000e-02,  2.000000030e-01,  1.740000000e-01,  1.680000000e-01, /*DIAM*/  9.761101005e-01,  1.011691109e+00,  9.957989700e-01, /*EDIAM*/  5.545171021e-02,  4.210571161e-02,  3.809498145e-02, /*DMEAN*/  9.974262117e-01, /*EDMEAN*/  2.878822463e-02, /*CHI2*/  7.400353396e-01 },
            {145, /* SP "M3.0IIIa       " idx */ 252, /*MAG*/  4.491000000e+00,  2.870000000e+00,  5.700000000e-01, -9.120000000e-01, -1.608000000e+00, -1.862000000e+00, /*EMAG*/  9.219544000e-03,  6.000000000e-03,  5.035871000e-02,  1.880000000e-01,  1.900000000e-01,  1.800000000e-01, /*DIAM*/  1.186547604e+00,  1.195403174e+00,  1.195587294e+00, /*EDIAM*/  5.213692774e-02,  4.595499851e-02,  4.080505428e-02, /*DMEAN*/  1.193222111e+00, /*EDMEAN*/  2.743899520e-02, /*CHI2*/  7.602482949e-02 },
            {146, /* SP "M4II           " idx */ 256, /*MAG*/  5.795000000e+00,  4.220000000e+00,  1.620000000e+00, -1.120000000e-01, -1.039000000e+00, -1.255000000e+00, /*EMAG*/  1.140175000e-02,  7.000000000e-03,  3.080584000e-02,  2.000000030e-01,  1.660000000e-01,  1.760000000e-01, /*DIAM*/  1.057558972e+00,  1.112342093e+00,  1.098857677e+00, /*EDIAM*/  5.560013014e-02,  4.038864725e-02,  4.001988941e-02, /*DMEAN*/  1.095617791e+00, /*EDMEAN*/  2.726910175e-02, /*CHI2*/  4.161631247e-01 },
            {147, /* SP "M5III          " idx */ 260, /*MAG*/  5.477000000e+00,  4.080000000e+00,  9.400000000e-01, -7.380000000e-01, -1.575000000e+00, -1.837000000e+00, /*EMAG*/  1.910497000e-02,  1.300000000e-02,  4.205948000e-02,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/  1.205665514e+00,  1.232126990e+00,  1.232314572e+00, /*EDIAM*/  5.606498477e-02,  4.903371479e-02,  4.574870186e-02, /*DMEAN*/  1.225275340e+00, /*EDMEAN*/  2.958391813e-02, /*CHI2*/  4.330595545e+00 },
            {148, /* SP "M2.5II-IIIe    " idx */ 250, /*MAG*/  4.095000000e+00,  2.440000000e+00,  1.300000000e-01, -1.432000000e+00, -2.129000000e+00, -2.379000000e+00, /*EMAG*/  1.280625000e-02,  8.000000000e-03,  6.053098000e-02,  1.940000000e-01,  1.800000000e-01,  1.540000000e-01, /*DIAM*/  1.301933759e+00,  1.303342579e+00,  1.298761056e+00, /*EDIAM*/  5.377975122e-02,  4.352332154e-02,  3.491941172e-02, /*DMEAN*/  1.300835543e+00, /*EDMEAN*/  2.447258460e-02, /*CHI2*/  6.180884099e-01 },
            {149, /* SP "M2.5II-IIIe    " idx */ 250, /*MAG*/  4.095000000e+00,  2.440000000e+00,  1.300000000e-01, -1.432000000e+00, -2.129000000e+00, -2.379000000e+00, /*EMAG*/  1.280625000e-02,  8.000000000e-03,  6.053098000e-02,  1.940000000e-01,  1.800000000e-01,  1.540000000e-01, /*DIAM*/  1.301933759e+00,  1.303342579e+00,  1.298761056e+00, /*EDIAM*/  5.377975122e-02,  4.352332154e-02,  3.491941172e-02, /*DMEAN*/  1.300835543e+00, /*EDMEAN*/  3.723823273e-02, /*CHI2*/  1.237768982e+00 },
            {150, /* SP "F5Iab:+B7-8    " idx */ 140, /*MAG*/  4.848000000e+00,  4.070000000e+00,  3.260000000e+00,  2.865000000e+00,  2.608000000e+00,  2.354000000e+00, /*EMAG*/  5.661272000e-02,  3.300000000e-02,  4.459821000e-02,  2.000000000e-01,  1.900000000e-01,  2.000000030e-01, /*DIAM*/  1.146712526e-01,  1.249451700e-01,  1.545114594e-01, /*EDIAM*/  5.551375268e-02,  4.596943740e-02,  4.533216813e-02, /*DMEAN*/  1.335659973e-01, /*EDMEAN*/  4.135105264e-02, /*CHI2*/  2.066597288e+00 },
            {151, /* SP "M2III          " idx */ 248, /*MAG*/  7.554000000e+00,  5.970000000e+00,  3.770000000e+00,  2.450000000e+00,  1.582000000e+00,  1.444000000e+00, /*EMAG*/  1.029563000e-02,  5.000000000e-03,  3.041381000e-02,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/  5.022593934e-01,  5.532148662e-01,  5.239680513e-01, /*EDIAM*/  5.543902597e-02,  4.833491930e-02,  4.531253805e-02, /*DMEAN*/  5.283681549e-01, /*EDMEAN*/  3.780295261e-02, /*CHI2*/  2.106908809e-01 },
            {152, /* SP "M5IIIv         " idx */ 260, /*MAG*/  9.482000000e+00,  8.000000000e+00,  4.820000000e+00,  2.148000000e+00,  1.292000000e+00,  1.006000000e+00, /*EMAG*/  2.773085000e-02,  1.200000000e-02,  1.205985000e-01,  2.000000030e-01,  1.540000000e-01,  2.000000030e-01, /*DIAM*/  7.078619353e-01,  7.021581110e-01,  6.920152935e-01, /*EDIAM*/  5.606367020e-02,  3.816472952e-02,  4.574851319e-02, /*DMEAN*/  7.001090629e-01, /*EDMEAN*/  3.192874817e-02, /*CHI2*/  2.031627556e-01 },
            {153, /* SP "M4III          " idx */ 256, /*MAG*/  7.911000000e+00,  6.320000000e+00,  2.900000000e+00,  8.970000000e-01, -1.640000000e-01, -5.120000000e-01, /*EMAG*/  2.842534000e-02,  1.800000000e-02,  6.264184000e-02,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/  9.395321850e-01,  9.878673835e-01,  9.859160695e-01, /*EDIAM*/  5.561470925e-02,  4.853804425e-02,  4.542932273e-02, /*DMEAN*/  9.744354232e-01, /*EDMEAN*/  3.164787099e-02, /*CHI2*/  7.057733498e-01 },
            {154, /* SP "K0III          " idx */ 200, /*MAG*/  5.893000000e+00,  4.880000000e+00,  3.890000000e+00,  3.025000000e+00,  2.578000000e+00,  2.494000000e+00, /*EMAG*/  2.517936000e-02,  3.000000000e-03,  3.014963000e-02,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/  2.121586369e-01,  2.005725354e-01,  1.916438006e-01, /*EDIAM*/  5.542309987e-02,  4.830284756e-02,  4.530042645e-02, /*DMEAN*/  2.001049285e-01, /*EDMEAN*/  5.222655114e-02, /*CHI2*/  1.228791150e-01 },
            {155, /* SP "F2III          " idx */ 128, /*MAG*/  2.660000000e+00,  2.280000000e+00,  1.880000000e+00,  1.714000000e+00,  1.585000000e+00,  1.454000000e+00, /*EMAG*/  5.000000000e-03,  3.000000000e-03,  3.014963000e-02,  2.000000030e-01,  1.740000000e-01,  2.000000030e-01, /*DIAM*/  2.918193771e-01,  2.947763133e-01,  3.069684562e-01, /*EDIAM*/  5.549301985e-02,  4.213801821e-02,  4.535822627e-02, /*DMEAN*/  2.983920941e-01, /*EDMEAN*/  2.892948132e-02, /*CHI2*/  3.301054859e-01 },
            {156, /* SP "G8III          " idx */ 192, /*MAG*/  5.211000000e+00,  4.340000000e+00,  3.420000000e+00,  2.578000000e+00,  2.154000000e+00,  2.074000000e+00, /*EMAG*/  4.242641000e-03,  3.000000000e-03,  1.044031000e-02,  2.000000030e-01,  1.980000000e-01,  2.000000030e-01, /*DIAM*/  2.768470379e-01,  2.700704488e-01,  2.618651831e-01, /*EDIAM*/  5.541788253e-02,  4.781864759e-02,  4.529689591e-02, /*DMEAN*/  2.686358536e-01, /*EDMEAN*/  3.424834557e-02, /*CHI2*/  6.122379077e-02 },
            {157, /* SP "K0IIIa         " idx */ 200, /*MAG*/  3.410000000e+00,  2.240000000e+00,  1.110000000e+00,  3.110000000e-01, -2.100000000e-01, -3.030000000e-01, /*EMAG*/  2.000000000e-03,  2.000000000e-03,  2.000000000e-03,  1.980000000e-01,  1.600000000e-01,  1.700000000e-01, /*DIAM*/  7.606407879e-01,  7.642768240e-01,  7.551693564e-01, /*EDIAM*/  5.486992441e-02,  3.866636548e-02,  3.851998247e-02, /*DMEAN*/  7.598911155e-01, /*EDMEAN*/  2.476431193e-02, /*CHI2*/  4.015716373e-02 },
            {158, /* SP "G8IIIb         " idx */ 192, /*MAG*/  5.577000000e+00,  4.620000000e+00,  3.610000000e+00,  3.123000000e+00,  2.680000000e+00,  2.468000000e+00, /*EMAG*/  2.236068000e-03,  2.000000000e-03,  4.004997000e-02,  2.000000030e-01,  1.880000000e-01,  2.000000030e-01, /*DIAM*/  1.474988217e-01,  1.547241436e-01,  1.800695614e-01, /*EDIAM*/  5.541761655e-02,  4.540887120e-02,  4.529685780e-02, /*DMEAN*/  1.624301382e-01, /*EDMEAN*/  3.826706178e-02, /*CHI2*/  7.782585992e-01 },
            {159, /* SP "G8.5IIIa       " idx */ 194, /*MAG*/  5.684000000e+00,  4.660000000e+00,  3.670000000e+00,  3.074000000e+00,  2.513000000e+00,  2.481000000e+00, /*EMAG*/  3.000000000e-03,  3.000000000e-03,  2.022375000e-02,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/  1.684463603e-01,  1.991049726e-01,  1.807440219e-01, /*EDIAM*/  5.541875328e-02,  4.830077202e-02,  4.529766084e-02, /*DMEAN*/  1.838586228e-01, /*EDMEAN*/  3.476703674e-02, /*CHI2*/  8.966356019e-01 },
            {160, /* SP "G9IIIb         " idx */ 196, /*MAG*/  5.671000000e+00,  4.680000000e+00,  3.670000000e+00,  3.019000000e+00,  2.481000000e+00,  2.311000000e+00, /*EMAG*/  2.828427000e-03,  2.000000000e-03,  3.006659000e-02,  2.000000000e-01,  1.640000000e-01,  2.000000030e-01, /*DIAM*/  1.895839966e-01,  2.102203987e-01,  2.223666764e-01, /*EDIAM*/  5.541964567e-02,  3.962768208e-02,  4.529845946e-02, /*DMEAN*/  2.096705576e-01, /*EDMEAN*/  3.519900191e-02, /*CHI2*/  1.163661990e-01 },
            {161, /* SP "K3III          " idx */ 212, /*MAG*/  4.865000000e+00,  3.590000000e+00,  2.360000000e+00,  1.420000000e+00,  7.410000000e-01,  6.490000000e-01, /*EMAG*/  4.472136000e-03,  2.000000000e-03,  2.009975000e-02,  2.000000030e-01,  1.540000000e-01,  2.000000030e-01, /*DIAM*/  5.833901692e-01,  6.093882668e-01,  5.924012705e-01, /*EDIAM*/  5.544190970e-02,  3.724204128e-02,  4.531120349e-02, /*DMEAN*/  5.984804122e-01, /*EDMEAN*/  2.645200292e-02, /*CHI2*/  1.660474801e-01 },
            {162, /* SP "K3IIIb         " idx */ 212, /*MAG*/  5.797000000e+00,  4.450000000e+00,  3.080000000e+00,  2.195000000e+00,  1.495000000e+00,  1.357000000e+00, /*EMAG*/  4.809366000e-02,  3.000000000e-03,  2.022375000e-02,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/  4.349169527e-01,  4.629602491e-01,  4.547954290e-01, /*EDIAM*/  5.544217557e-02,  4.831867364e-02,  4.531124159e-02, /*DMEAN*/  4.524029983e-01, /*EDMEAN*/  2.877429875e-02, /*CHI2*/  6.398294410e-02 },
            {163, /* SP "K1IIIb         " idx */ 204, /*MAG*/  3.161000000e+00,  2.010000000e+00,  8.800000000e-01, -1.960000000e-01, -7.490000000e-01, -7.830000000e-01, /*EMAG*/  7.211103000e-03,  4.000000000e-03,  4.000000000e-03,  2.000000030e-01,  1.560000000e-01,  1.760000000e-01, /*DIAM*/  8.922131942e-01,  8.907645204e-01,  8.633529261e-01, /*EDIAM*/  5.542796088e-02,  3.770730718e-02,  3.987875299e-02, /*DMEAN*/  8.806498809e-01, /*EDMEAN*/  2.466137519e-02, /*CHI2*/  8.072156142e-01 },
            {164, /* SP "K1.5III        " idx */ 206, /*MAG*/  5.632000000e+00,  4.520000000e+00,  3.480000000e+00,  2.875000000e+00,  2.347000000e+00,  2.099000000e+00, /*EMAG*/  2.828427000e-03,  2.000000000e-03,  3.006659000e-02,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/  2.393413814e-01,  2.505103055e-01,  2.800367097e-01, /*EDIAM*/  5.543016616e-02,  4.830805876e-02,  4.530441161e-02, /*DMEAN*/  2.591737590e-01, /*EDMEAN*/  3.639315471e-02, /*CHI2*/  2.429256723e-01 },
            {165, /* SP "K2III          " idx */ 208, /*MAG*/  5.383000000e+00,  4.350000000e+00,  3.390000000e+00,  2.775000000e+00,  2.303000000e+00,  2.169000000e+00, /*EMAG*/  5.830952000e-03,  3.000000000e-03,  1.044031000e-02,  2.000000030e-01,  1.800000000e-01,  2.000000030e-01, /*DIAM*/  2.583085539e-01,  2.573116018e-01,  2.626143576e-01, /*EDIAM*/  5.543376170e-02,  4.349292148e-02,  4.530631186e-02, /*DMEAN*/  2.594795776e-01, /*EDMEAN*/  3.862598942e-02, /*CHI2*/  1.050258695e-01 },
            {166, /* SP "F5Iab:         " idx */ 140, /*MAG*/  2.271000000e+00,  1.790000000e+00,  1.160000000e+00,  8.300000000e-01,  4.130000000e-01,  5.200000000e-01, /*EMAG*/  5.656854000e-03,  4.000000000e-03,  4.019950000e-02,  1.840000000e-01,  1.620000000e-01,  1.640000000e-01, /*DIAM*/  5.028587584e-01,  5.604393399e-01,  5.095917567e-01, /*EDIAM*/  5.103681428e-02,  3.921418585e-02,  3.719301983e-02, /*DMEAN*/  5.269423379e-01, /*EDMEAN*/  2.395757356e-02, /*CHI2*/  8.802169954e-01 },
            {167, /* SP "K0III          " idx */ 200, /*MAG*/  5.424000000e+00,  4.360000000e+00,  3.340000000e+00,  2.579000000e+00,  2.055000000e+00,  2.033000000e+00, /*EMAG*/  9.486833000e-03,  3.000000000e-03,  1.044031000e-02,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/  2.956764953e-01,  3.052962724e-01,  2.822934369e-01, /*EDIAM*/  5.542309987e-02,  4.830284756e-02,  4.530042645e-02, /*DMEAN*/  2.937438023e-01, /*EDMEAN*/  3.481731467e-02, /*CHI2*/  9.249277740e-01 },
            {168, /* SP "F0Ib           " idx */ 120, /*MAG*/  2.791000000e+00,  2.580000000e+00,  2.260000000e+00,  2.030000000e+00,  1.857000000e+00,  1.756000000e+00, /*EMAG*/  8.062258000e-03,  1.000000047e-03,  2.002498000e-02,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/  2.235454242e-01,  2.371131338e-01,  2.423162003e-01, /*EDIAM*/  5.554979357e-02,  4.843525047e-02,  4.539729374e-02, /*DMEAN*/  2.356019139e-01, /*EDMEAN*/  3.608749993e-02, /*CHI2*/  6.256165700e-02 },
            {169, /* SP "G8Ib           " idx */ 192, /*MAG*/  4.437000000e+00,  3.060000000e+00,  1.840000000e+00,  8.860000000e-01,  2.320000000e-01,  1.250000000e-01, /*EMAG*/  1.513275000e-02,  2.000000000e-03,  1.019804000e-02,  2.000000030e-01,  1.540000000e-01,  1.720000000e-01, /*DIAM*/  6.468827577e-01,  6.809498324e-01,  6.692447512e-01, /*EDIAM*/  5.541761655e-02,  3.721861252e-02,  3.896781983e-02, /*DMEAN*/  6.699340477e-01, /*EDMEAN*/  2.431041212e-02, /*CHI2*/  1.650710317e-01 },
            {170, /* SP "K4III          " idx */ 216, /*MAG*/  5.011000000e+00,  3.530000000e+00,  2.060000000e+00,  1.191000000e+00,  2.670000000e-01,  1.900000000e-01, /*EMAG*/  6.708204000e-03,  3.000000000e-03,  2.022375000e-02,  2.000000030e-01,  1.600000000e-01,  2.000000030e-01, /*DIAM*/  6.499950706e-01,  7.280416308e-01,  7.005198736e-01, /*EDIAM*/  5.545333446e-02,  3.869999407e-02,  4.531819496e-02, /*DMEAN*/  7.018441105e-01, /*EDMEAN*/  2.614256354e-02, /*CHI2*/  5.478171767e-01 },
            {171, /* SP "M2III          " idx */ 248, /*MAG*/  6.269000000e+00,  4.680000000e+00,  2.720000000e+00,  1.530000000e+00,  7.060000000e-01,  4.900000000e-01, /*EMAG*/  4.110961000e-02,  3.000000000e-03,  2.022375000e-02,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/  6.578486814e-01,  7.113393822e-01,  7.059388570e-01, /*EDIAM*/  5.543817515e-02,  4.833463774e-02,  4.531241614e-02, /*DMEAN*/  6.951932492e-01, /*EDMEAN*/  2.878019924e-02, /*CHI2*/  2.531696401e-01 },
            {172, /* SP "K4III          " idx */ 216, /*MAG*/  5.838000000e+00,  4.390000000e+00,  2.880000000e+00,  1.897000000e+00,  1.220000000e+00,  1.022000000e+00, /*EMAG*/  4.242641000e-03,  3.000000000e-03,  6.007495000e-02,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/  5.206200686e-01,  5.336058303e-01,  5.348556376e-01, /*EDIAM*/  5.545333446e-02,  4.832968341e-02,  4.531819496e-02, /*DMEAN*/  5.306924917e-01, /*EDMEAN*/  2.887486607e-02, /*CHI2*/  1.601536595e-02 },
            {173, /* SP "K3.5III        " idx */ 214, /*MAG*/  6.299000000e+00,  4.770000000e+00,  3.150000000e+00,  2.160000000e+00,  1.390000000e+00,  1.247000000e+00, /*EMAG*/  1.676305000e-02,  5.000000000e-03,  4.031129000e-02,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/  4.731745510e-01,  5.048631624e-01,  4.910127783e-01, /*EDIAM*/  5.544824285e-02,  4.832402560e-02,  4.531456419e-02, /*DMEAN*/  4.911181680e-01, /*EDMEAN*/  2.854646608e-02, /*CHI2*/  1.587709250e-01 },
            {174, /* SP "K0.5IIIb       " idx */ 202, /*MAG*/  4.871000000e+00,  3.690000000e+00,  2.540000000e+00,  1.679000000e+00,  1.025000000e+00,  9.880000000e-01, /*EMAG*/  2.236068000e-03,  2.000000000e-03,  3.006659000e-02,  2.000000030e-01,  1.620000000e-01,  1.900000000e-01, /*DIAM*/  4.977963602e-01,  5.290649772e-01,  5.039349661e-01, /*EDIAM*/  5.542489719e-02,  3.914966893e-02,  4.304117962e-02, /*DMEAN*/  5.134243363e-01, /*EDMEAN*/  2.573030352e-02, /*CHI2*/  1.150345451e-01 },
            {175, /* SP "G8III          " idx */ 192, /*MAG*/  3.784000000e+00,  2.850000000e+00,  2.020000000e+00,  1.248000000e+00,  7.330000000e-01,  6.640000000e-01, /*EMAG*/  1.513275000e-02,  2.000000000e-03,  1.019804000e-02,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/  5.305613274e-01,  5.514245386e-01,  5.417629975e-01, /*EDIAM*/  5.541761655e-02,  4.830052997e-02,  4.529685780e-02, /*DMEAN*/  5.421615329e-01, /*EDMEAN*/  2.853560795e-02, /*CHI2*/  4.869561940e-01 },
            {176, /* SP "K5.5III        " idx */ 222, /*MAG*/  5.570000000e+00,  4.050000000e+00,  2.450000000e+00,  1.218000000e+00,  4.380000000e-01,  4.350000000e-01, /*EMAG*/  1.236932000e-02,  3.000000000e-03,  3.014963000e-02,  2.000000030e-01,  1.800000000e-01,  1.820000000e-01, /*DIAM*/  6.926328261e-01,  7.184648881e-01,  6.674188039e-01, /*EDIAM*/  5.547508980e-02,  4.353880137e-02,  4.126723331e-02, /*DMEAN*/  6.918129770e-01, /*EDMEAN*/  2.662847423e-02, /*CHI2*/  2.583611680e-01 },
            {177, /* SP "G8IV           " idx */ 192, /*MAG*/  4.421000000e+00,  3.460000000e+00,  2.500000000e+00,  1.659000000e+00,  9.850000000e-01,  1.223000000e+00, /*EMAG*/  3.605551000e-03,  2.000000000e-03,  3.006659000e-02,  2.000000030e-01,  1.920000000e-01,  1.960000000e-01, /*DIAM*/  4.636416836e-01,  5.157902969e-01,  4.313031418e-01, /*EDIAM*/  5.541761655e-02,  4.637271169e-02,  4.439259140e-02, /*DMEAN*/  4.696807002e-01, /*EDMEAN*/  2.796255132e-02, /*CHI2*/  9.316534623e-01 },
            {178, /* SP "G7.5IIIb       " idx */ 190, /*MAG*/  4.396000000e+00,  3.480000000e+00,  2.590000000e+00,  1.768000000e+00,  1.366000000e+00,  1.333000000e+00, /*EMAG*/  7.280110000e-03,  2.000000000e-03,  3.006659000e-02,  2.000000030e-01,  1.920000000e-01,  2.000000030e-01, /*DIAM*/  4.307762586e-01,  4.223890868e-01,  4.044438589e-01, /*EDIAM*/  5.541704516e-02,  4.637281693e-02,  4.529616812e-02, /*DMEAN*/  4.176808623e-01, /*EDMEAN*/  3.061896836e-02, /*CHI2*/  3.239108130e-01 },
            {179, /* SP "K3II           " idx */ 212, /*MAG*/  4.597000000e+00,  3.160000000e+00,  1.850000000e+00,  7.240000000e-01, -1.120000000e-01, -2.300000000e-02, /*EMAG*/  2.000000000e-03,  2.000000000e-03,  2.000000000e-03,  1.780000000e-01,  1.780000000e-01,  1.600000000e-01, /*DIAM*/  7.430151716e-01,  7.974349622e-01,  7.331603967e-01, /*EDIAM*/  4.936308091e-02,  4.301977428e-02,  3.627401617e-02, /*DMEAN*/  7.558225402e-01, /*EDMEAN*/  2.426339365e-02, /*CHI2*/  1.362765007e+00 },
            {180, /* SP "K0III          " idx */ 200, /*MAG*/  5.608000000e+00,  4.350000000e+00,  3.220000000e+00,  2.213000000e+00,  1.598000000e+00,  1.514000000e+00, /*EMAG*/  1.019804000e-02,  2.000000000e-03,  3.006659000e-02,  2.000000030e-01,  1.920000000e-01,  2.000000030e-01, /*DIAM*/  3.962122111e-01,  4.151328499e-01,  3.994686212e-01, /*EDIAM*/  5.542283391e-02,  4.637503392e-02,  4.530038834e-02, /*DMEAN*/  4.043394412e-01, /*EDMEAN*/  3.200185015e-02, /*CHI2*/  9.635633749e-01 },
            {181, /* SP "G9III          " idx */ 196, /*MAG*/  4.750000000e+00,  3.800000000e+00,  2.950000000e+00,  2.319000000e+00,  1.823000000e+00,  1.757000000e+00, /*EMAG*/  9.219544000e-03,  2.000000000e-03,  3.006659000e-02,  2.000000030e-01,  1.740000000e-01,  1.860000000e-01, /*DIAM*/  3.157625700e-01,  3.326639803e-01,  3.246002545e-01, /*EDIAM*/  5.541964649e-02,  4.203649048e-02,  4.213380760e-02, /*DMEAN*/  3.257603480e-01, /*EDMEAN*/  3.100493697e-02, /*CHI2*/  2.220053855e-02 },
            {182, /* SP "K3II           " idx */ 212, /*MAG*/  4.227000000e+00,  2.720000000e+00,  1.280000000e+00,  2.760000000e-01, -5.440000000e-01, -7.200000000e-01, /*EMAG*/  1.414214000e-02,  2.000000000e-03,  2.009975000e-02,  1.680000000e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/  8.332294587e-01,  8.835050024e-01,  8.793136835e-01, /*EDIAM*/  4.660087998e-02,  4.831858562e-02,  4.531120349e-02, /*DMEAN*/  8.651948368e-01, /*EDMEAN*/  2.704833148e-02, /*CHI2*/  3.420331748e-01 },
            {183, /* SP "G9.5IV         " idx */ 198, /*MAG*/  4.565000000e+00,  3.710000000e+00,  2.820000000e+00,  2.294000000e+00,  1.925000000e+00,  1.712000000e+00, /*EMAG*/  7.280110000e-03,  2.000000000e-03,  2.009975000e-02,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/  3.201969942e-01,  3.070383847e-01,  3.351066750e-01, /*EDIAM*/  5.542109199e-02,  4.830177684e-02,  4.529937491e-02, /*DMEAN*/  3.215072658e-01, /*EDMEAN*/  3.354746036e-02, /*CHI2*/  1.674793792e-01 },
            {184, /* SP "F8Iab          " idx */ 152, /*MAG*/  2.903000000e+00,  2.230000000e+00,  1.580000000e+00,  1.369000000e+00,  8.520000000e-01,  8.270000000e-01, /*EMAG*/  1.236932000e-02,  3.000000000e-03,  3.014963000e-02,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/  3.929241270e-01,  4.741865725e-01,  4.557288757e-01, /*EDIAM*/  5.544728984e-02,  4.834197395e-02,  4.530869959e-02, /*DMEAN*/  4.456346070e-01, /*EDMEAN*/  3.016481986e-02, /*CHI2*/  8.675081353e-01 },
            {185, /* SP "K0IV           " idx */ 200, /*MAG*/  4.322000000e+00,  3.410000000e+00,  2.470000000e+00,  1.914000000e+00,  1.504000000e+00,  1.393000000e+00, /*EMAG*/  2.828427000e-03,  2.000000000e-03,  4.004997000e-02,  2.000000030e-01,  1.700000000e-01,  2.000000030e-01, /*DIAM*/  4.067925684e-01,  3.990394645e-01,  4.021474533e-01, /*EDIAM*/  5.542283391e-02,  4.107487343e-02,  4.530038834e-02, /*DMEAN*/  4.019120311e-01, /*EDMEAN*/  2.749431437e-02, /*CHI2*/  2.049208664e-01 },
            {186, /* SP "K4.5Ib-II      " idx */ 218, /*MAG*/  5.329000000e+00,  3.720000000e+00,  2.090000000e+00,  9.820000000e-01,  2.700000000e-02, -3.800000000e-02, /*EMAG*/  5.830952000e-03,  3.000000000e-03,  1.044031000e-02,  2.000000030e-01,  1.780000000e-01,  2.000000030e-01, /*DIAM*/  7.260640795e-01,  7.971955814e-01,  7.600110941e-01, /*EDIAM*/  5.545999202e-02,  4.303987350e-02,  4.532250818e-02, /*DMEAN*/  7.667020388e-01, /*EDMEAN*/  2.732670474e-02, /*CHI2*/  5.626862510e-01 },
            {187, /* SP "G5III          " idx */ 180, /*MAG*/  4.865000000e+00,  3.980000000e+00,  3.040000000e+00,  2.485000000e+00,  2.013000000e+00,  1.901000000e+00, /*EMAG*/  4.472136000e-03,  2.000000000e-03,  3.006659000e-02,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/  2.512893642e-01,  2.774012193e-01,  2.778125782e-01, /*EDIAM*/  5.541916550e-02,  4.830530066e-02,  4.529436886e-02, /*DMEAN*/  2.707172222e-01, /*EDMEAN*/  3.550416959e-02, /*CHI2*/  6.827443283e-02 },
            {188, /* SP "G2Ib           " idx */ 168, /*MAG*/  3.919000000e+00,  2.950000000e+00,  2.030000000e+00,  1.278000000e+00,  7.400000000e-01,  5.900000000e-01, /*EMAG*/  1.170470000e-02,  4.000000000e-03,  2.039608000e-02,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/  4.880741912e-01,  5.349118504e-01,  5.369983543e-01, /*EDIAM*/  5.543202953e-02,  4.832007577e-02,  4.529757523e-02, /*DMEAN*/  5.234544766e-01, /*EDMEAN*/  2.871894941e-02, /*CHI2*/  4.531277104e-01 },
            {189, /* SP "G8.5IIIb       " idx */ 194, /*MAG*/  5.435000000e+00,  4.420000000e+00,  3.390000000e+00,  2.567000000e+00,  2.003000000e+00,  1.880000000e+00, /*EMAG*/  5.385165000e-03,  2.000000000e-03,  3.006659000e-02,  2.000000030e-01,  1.700000000e-01,  2.000000030e-01, /*DIAM*/  2.903481478e-01,  3.122411611e-01,  3.104301552e-01, /*EDIAM*/  5.541848730e-02,  4.107243263e-02,  4.529762273e-02, /*DMEAN*/  3.065444463e-01, /*EDMEAN*/  2.679314624e-02, /*CHI2*/  1.348461412e-01 },
            {190, /* SP "F5Iab:+B7-8    " idx */ 140, /*MAG*/  4.848000000e+00,  4.070000000e+00,  3.260000000e+00,  2.865000000e+00,  2.608000000e+00,  2.354000000e+00, /*EMAG*/  5.661272000e-02,  3.300000000e-02,  4.459821000e-02,  2.000000000e-01,  1.900000000e-01,  2.000000030e-01, /*DIAM*/  1.146712526e-01,  1.249451700e-01,  1.545114594e-01, /*EDIAM*/  5.551375268e-02,  4.596943740e-02,  4.533216813e-02, /*DMEAN*/  1.335659973e-01, /*EDMEAN*/  2.852938518e-02, /*CHI2*/  1.601758756e+00 },
            {191, /* SP "K2.5III        " idx */ 210, /*MAG*/  5.818000000e+00,  4.500000000e+00,  3.250000000e+00,  2.435000000e+00,  1.817000000e+00,  1.668000000e+00, /*EMAG*/  3.162278000e-03,  1.000000047e-03,  3.001666000e-02,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/  3.681877095e-01,  3.840130890e-01,  3.828233596e-01, /*EDIAM*/  5.543722654e-02,  4.831429315e-02,  4.530849098e-02, /*DMEAN*/  3.793971975e-01, /*EDMEAN*/  2.954343955e-02, /*CHI2*/  9.370500646e-01 },
            {192, /* SP "G8Iab:         " idx */ 192, /*MAG*/  5.040000000e+00,  3.970000000e+00,  2.980000000e+00,  2.030000000e+00,  1.462000000e+00,  1.508000000e+00, /*EMAG*/  2.828427000e-03,  2.000000000e-03,  3.006659000e-02,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/  4.001148969e-01,  4.217513849e-01,  3.802155498e-01, /*EDIAM*/  5.541761655e-02,  4.830052997e-02,  4.529685780e-02, /*DMEAN*/  3.997720845e-01, /*EDMEAN*/  3.419643414e-02, /*CHI2*/  6.236913482e-01 },
            {193, /* SP "G8III          " idx */ 192, /*MAG*/  4.443000000e+00,  3.510000000e+00,  2.620000000e+00,  1.700000000e+00,  1.243000000e+00,  1.181000000e+00, /*EMAG*/  1.019804000e-02,  2.000000000e-03,  3.006659000e-02,  2.000000030e-01,  1.700000000e-01,  1.560000000e-01, /*DIAM*/  4.561327549e-01,  4.556113076e-01,  4.421206602e-01, /*EDIAM*/  5.541761655e-02,  4.107225153e-02,  3.535230944e-02, /*DMEAN*/  4.494263656e-01, /*EDMEAN*/  2.775117799e-02, /*CHI2*/  8.682563110e-01 },
            {194, /* SP "G2Ia0e         " idx */ 168, /*MAG*/  5.700000000e+00,  4.510000000e+00,  3.360000000e+00,  2.269000000e+00,  1.915000000e+00,  1.670000000e+00, /*EMAG*/  1.252996000e-02,  6.000000000e-03,  1.166190000e-02,  2.000000030e-01,  1.640000000e-01,  2.000000030e-01, /*DIAM*/  3.335652604e-01,  3.157912245e-01,  3.336114899e-01, /*EDIAM*/  5.543309318e-02,  3.965124424e-02,  4.529772767e-02, /*DMEAN*/  3.257755019e-01, /*EDMEAN*/  2.773766659e-02, /*CHI2*/  1.978057050e+00 },
            {195, /* SP "G9II-III       " idx */ 196, /*MAG*/  4.088000000e+00,  3.110000000e+00,  2.150000000e+00,  1.415000000e+00,  7.580000000e-01,  6.970000000e-01, /*EMAG*/  4.909175000e-02,  3.000000000e-03,  2.022375000e-02,  2.000000030e-01,  1.600000000e-01,  1.900000000e-01, /*DIAM*/  5.129947158e-01,  5.611309098e-01,  5.463228856e-01, /*EDIAM*/  5.541991247e-02,  3.866439591e-02,  4.303798524e-02, /*DMEAN*/  5.457121008e-01, /*EDMEAN*/  2.713449022e-02, /*CHI2*/  3.958383670e-01 },
            {196, /* SP "K7III          " idx */ 228, /*MAG*/  4.690000000e+00,  3.140000000e+00,  1.490000000e+00,  1.200000000e-01, -5.780000000e-01, -6.560000000e-01, /*EMAG*/  8.944272000e-03,  4.000000000e-03,  3.026549000e-02,  2.000000030e-01,  1.700000000e-01,  1.720000000e-01, /*DIAM*/  9.342560259e-01,  9.358898953e-01,  8.988258996e-01, /*EDIAM*/  5.549882344e-02,  4.116281821e-02,  3.902843436e-02, /*DMEAN*/  9.200622442e-01, /*EDMEAN*/  2.536693075e-02, /*CHI2*/  6.932394126e-01 },
            {197, /* SP "G1II           " idx */ 164, /*MAG*/  3.778000000e+00,  2.970000000e+00,  2.160000000e+00,  1.511000000e+00,  1.107000000e+00,  1.223000000e+00, /*EMAG*/  6.708204000e-03,  3.000000000e-03,  2.022375000e-02,  2.000000030e-01,  1.820000000e-01,  2.000000030e-01, /*DIAM*/  4.204525854e-01,  4.457325606e-01,  3.916009327e-01, /*EDIAM*/  5.543651681e-02,  4.399111514e-02,  4.529975526e-02, /*DMEAN*/  4.197014302e-01, /*EDMEAN*/  3.104127272e-02, /*CHI2*/  2.937308224e-01 },
            {198, /* SP "K1III          " idx */ 204, /*MAG*/  4.144000000e+00,  3.000000000e+00,  1.910000000e+00,  1.030000000e+00,  3.760000000e-01,  4.290000000e-01, /*EMAG*/  5.000000000e-03,  3.000000000e-03,  3.014963000e-02,  1.800000000e-01,  1.700000000e-01,  1.980000000e-01, /*DIAM*/  6.288917617e-01,  6.601964236e-01,  6.151193457e-01, /*EDIAM*/  4.989968914e-02,  4.107858714e-02,  4.485081516e-02, /*DMEAN*/  6.367429791e-01, /*EDMEAN*/  2.645041498e-02, /*CHI2*/  3.958139091e-01 },
            {199, /* SP "M0III          " idx */ 240, /*MAG*/  5.433000000e+00,  3.820000000e+00,  2.030000000e+00,  8.950000000e-01, -4.900000000e-02, -1.140000000e-01, /*EMAG*/  9.486833000e-03,  3.000000000e-03,  3.014963000e-02,  2.000000030e-01,  1.960000000e-01,  1.740000000e-01, /*DIAM*/  7.759487701e-01,  8.524040833e-01,  8.099826722e-01, /*EDIAM*/  5.549555183e-02,  4.741818600e-02,  3.948025500e-02, /*DMEAN*/  8.155257246e-01, /*EDMEAN*/  2.674141350e-02, /*CHI2*/  3.960898238e-01 },
            {200, /* SP "M1III          " idx */ 244, /*MAG*/  5.541000000e+00,  4.040000000e+00,  2.250000000e+00,  1.176000000e+00,  3.250000000e-01,  1.570000000e-01, /*EMAG*/  3.224903000e-02,  4.000000000e-03,  2.039608000e-02,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/  7.121454496e-01,  7.747353220e-01,  7.594880063e-01, /*EDIAM*/  5.546743221e-02,  4.835620636e-02,  4.533103772e-02, /*DMEAN*/  7.523389713e-01, /*EDMEAN*/  2.853703351e-02, /*CHI2*/  2.862718104e-01 },
            {201, /* SP "G8IIIa         " idx */ 192, /*MAG*/  4.446000000e+00,  3.490000000e+00,  2.600000000e+00,  1.795000000e+00,  1.268000000e+00,  1.223000000e+00, /*EMAG*/  2.000000000e-03,  2.000000000e-03,  3.006659000e-02,  2.000000030e-01,  1.180000000e-01,  1.660000000e-01, /*DIAM*/  4.283023973e-01,  4.487552764e-01,  4.320914630e-01, /*EDIAM*/  5.541761655e-02,  2.855413123e-02,  3.761189234e-02, /*DMEAN*/  4.406000968e-01, /*EDMEAN*/  2.426634862e-02, /*CHI2*/  8.771247866e-01 },
            {202, /* SP "K2III          " idx */ 208, /*MAG*/  3.797000000e+00,  2.630000000e+00,  1.540000000e+00,  8.310000000e-01,  2.890000000e-01,  1.500000000e-01, /*EMAG*/  1.529706000e-02,  3.000000000e-03,  2.022375000e-02,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/  6.643085600e-01,  6.722376780e-01,  6.742712980e-01, /*EDIAM*/  5.543376170e-02,  4.831095443e-02,  4.530631186e-02, /*DMEAN*/  6.709573463e-01, /*EDMEAN*/  2.909303050e-02, /*CHI2*/  9.751334441e-02 },
            {203, /* SP "G8III-IV       " idx */ 192, /*MAG*/  3.640000000e+00,  2.730000000e+00,  1.890000000e+00,  1.081000000e+00,  5.840000000e-01,  5.790000000e-01, /*EMAG*/  2.236068000e-03,  2.000000000e-03,  3.006659000e-02,  2.000000030e-01,  1.520000000e-01,  2.000000030e-01, /*DIAM*/  5.675702566e-01,  5.824206480e-01,  5.578432897e-01, /*EDIAM*/  5.541761655e-02,  3.673700041e-02,  4.529685780e-02, /*DMEAN*/  5.716040746e-01, /*EDMEAN*/  2.655259179e-02, /*CHI2*/  9.995501677e-01 },
            {204, /* SP "G9III          " idx */ 196, /*MAG*/  4.060000000e+00,  3.070000000e+00,  2.130000000e+00,  1.375000000e+00,  8.370000000e-01,  8.830000000e-01, /*EMAG*/  1.811077000e-02,  2.000000000e-03,  3.006659000e-02,  2.000000030e-01,  1.660000000e-01,  1.980000000e-01, /*DIAM*/  5.209947159e-01,  5.404227382e-01,  5.031841988e-01, /*EDIAM*/  5.541964649e-02,  4.010940873e-02,  4.484633782e-02, /*DMEAN*/  5.232269392e-01, /*EDMEAN*/  2.713368238e-02, /*CHI2*/  1.285683220e-01 },
            {205, /* SP "K2Ib           " idx */ 208, /*MAG*/  3.957000000e+00,  2.380000000e+00,  9.600000000e-01, -1.170000000e-01, -7.500000000e-01, -8.600000000e-01, /*EMAG*/  3.000000000e-03,  4.000000000e-03,  2.039608000e-02,  1.660000000e-01,  1.920000000e-01,  2.000000030e-01, /*DIAM*/  9.075049922e-01,  9.125800941e-01,  8.962421042e-01, /*EDIAM*/  4.603915911e-02,  4.638369778e-02,  4.530636521e-02, /*DMEAN*/  9.053079286e-01, /*EDMEAN*/  2.657654078e-02, /*CHI2*/  7.461369130e-02 },
            {206, /* SP "M2.0V          " idx */ 248, /*MAG*/  9.650000000e+00,  8.090000000e+00,  5.610000000e+00,  5.252000000e+00,  4.476000000e+00,  4.018000000e+00, /*EMAG*/  4.472136000e-02,  2.000000000e-02,  1.711724000e-01,  2.000000030e-01,  2.000000000e-01,  2.000000000e-02, /*DIAM*/ -1.105084729e-01, -5.750887833e-02, -2.761883530e-03, /*EDIAM*/  5.545896336e-02,  4.834151725e-02,  5.089954669e-03, /*DMEAN*/ -4.048780522e-03, /*EDMEAN*/  2.226913164e-02, /*CHI2*/  1.676211477e+00 },
            {207, /* SP "K3V            " idx */ 212, /*MAG*/  6.708000000e+00,  5.790000000e+00,  4.730000000e+00,  4.152000000e+00,  3.657000000e+00,  3.481000000e+00, /*EMAG*/  3.319168000e-02,  2.721558000e-02,  3.377407000e-02,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/ -3.859839530e-03, -3.343259802e-03,  9.393962538e-03, /*EDIAM*/  5.548106791e-02,  4.833155223e-02,  4.531681636e-02, /*DMEAN*/  1.526076007e-03, /*EDMEAN*/  4.316382826e-02, /*CHI2*/  1.788910546e-01 },
            {208, /* SP "K8V            " idx */ 232, /*MAG*/  7.926000000e+00,  6.600000000e+00,  5.310000000e+00,  3.894000000e+00,  3.298000000e+00,  2.962000000e+00, /*EMAG*/  8.944272000e-03,  4.000000000e-03,  1.077033000e-02,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/  1.585447296e-01,  1.496008385e-01,  1.765349084e-01, /*EDIAM*/  5.550947364e-02,  4.838978658e-02,  4.535695344e-02, /*DMEAN*/  1.625258271e-01, /*EDMEAN*/  3.147016576e-02, /*CHI2*/  9.097032563e-01 },
            {209, /* SP "M2.0V          " idx */ 248, /*MAG*/  8.992000000e+00,  7.490000000e+00,  5.420000000e+00,  4.320000000e+00,  3.722000000e+00,  3.501000000e+00, /*EMAG*/  3.788781000e-02,  3.520633000e-02,  4.049057000e-02,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/  1.013843874e-01,  9.964287498e-02,  9.845709608e-02, /*EDIAM*/  5.550356982e-02,  4.835628667e-02,  4.532179065e-02, /*DMEAN*/  9.963220073e-02, /*EDMEAN*/  2.988764925e-02, /*CHI2*/  1.195534140e+00 },
            {210, /* SP "M2.0V          " idx */ 248, /*MAG*/  8.992000000e+00,  7.490000000e+00,  5.420000000e+00,  4.320000000e+00,  3.722000000e+00,  3.501000000e+00, /*EMAG*/  3.788781000e-02,  3.520633000e-02,  4.049057000e-02,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/  1.013843874e-01,  9.964287498e-02,  9.845709608e-02, /*EDIAM*/  5.550356982e-02,  4.835628667e-02,  4.532179065e-02, /*DMEAN*/  9.963220073e-02, /*EDMEAN*/  2.988764925e-02, /*CHI2*/  1.195534140e+00 },
            {211, /* SP "M2.0V          " idx */ 248, /*MAG*/  9.650000000e+00,  8.090000000e+00,  5.610000000e+00,  5.252000000e+00,  4.476000000e+00,  4.018000000e+00, /*EMAG*/  4.472136000e-02,  2.000000000e-02,  1.711724000e-01,  2.000000030e-01,  2.000000000e-01,  2.000000000e-02, /*DIAM*/ -1.105084729e-01, -5.750887833e-02, -2.761883530e-03, /*EDIAM*/  5.545896336e-02,  4.834151725e-02,  5.089954669e-03, /*DMEAN*/ -4.048780522e-03, /*EDMEAN*/  2.229492433e-02, /*CHI2*/  1.673112624e+00 },
            {212, /* SP "K3V            " idx */ 212, /*MAG*/  6.708000000e+00,  5.790000000e+00,  4.730000000e+00,  4.152000000e+00,  3.657000000e+00,  3.481000000e+00, /*EMAG*/  3.319168000e-02,  2.721558000e-02,  3.377407000e-02,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/ -3.859839530e-03, -3.343259802e-03,  9.393962538e-03, /*EDIAM*/  5.548106791e-02,  4.833155223e-02,  4.531681636e-02, /*DMEAN*/  1.526076007e-03, /*EDMEAN*/  4.316382826e-02, /*CHI2*/  1.788910546e-01 },
            {213, /* SP "sdM1.0         " idx */ 244, /*MAG*/  1.040300000e+01,  8.860000000e+00,  6.900000000e+00,  5.821000000e+00,  5.316000000e+00,  5.049000000e+00, /*EMAG*/  4.619940000e-02,  4.164594000e-02,  4.164594000e-02,  2.600000000e-02,  2.700000000e-02,  2.100000000e-02, /*DIAM*/ -2.034170640e-01, -2.305176113e-01, -2.208039791e-01, /*EDIAM*/  8.610633140e-03,  7.463546291e-03,  5.532755259e-03, /*DMEAN*/ -2.203238832e-01, /*EDMEAN*/  3.786527667e-02, /*CHI2*/  3.094762185e+00 },
            {214, /* SP "M1.5V          " idx */ 246, /*MAG*/  9.444000000e+00,  7.970000000e+00,  5.890000000e+00,  4.999000000e+00,  4.149000000e+00,  4.039000000e+00, /*EMAG*/  1.421267000e-02,  1.100000000e-02,  1.486607000e-02,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/ -4.663503404e-02,  1.555552128e-02, -1.315564218e-02, /*EDIAM*/  5.545687628e-02,  4.834506201e-02,  4.532135370e-02, /*DMEAN*/ -1.202366603e-02, /*EDMEAN*/  5.053395357e-02, /*CHI2*/  8.844293504e-01 },
            {215, /* SP "K8V            " idx */ 232, /*MAG*/  7.926000000e+00,  6.600000000e+00,  5.310000000e+00,  3.894000000e+00,  3.298000000e+00,  2.962000000e+00, /*EMAG*/  8.944272000e-03,  4.000000000e-03,  1.077033000e-02,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/  1.585447296e-01,  1.496008385e-01,  1.765349084e-01, /*EDIAM*/  5.550947364e-02,  4.838978658e-02,  4.535695344e-02, /*DMEAN*/  1.625258271e-01, /*EDMEAN*/  3.147016576e-02, /*CHI2*/  9.097032563e-01 },
            {216, /* SP "M2.0V          " idx */ 248, /*MAG*/  8.992000000e+00,  7.490000000e+00,  5.420000000e+00,  4.320000000e+00,  3.722000000e+00,  3.501000000e+00, /*EMAG*/  3.788781000e-02,  3.520633000e-02,  4.049057000e-02,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/  1.013843874e-01,  9.964287498e-02,  9.845709608e-02, /*EDIAM*/  5.550356982e-02,  4.835628667e-02,  4.532179065e-02, /*DMEAN*/  9.963220073e-02, /*EDMEAN*/  2.988764925e-02, /*CHI2*/  1.195534140e+00 },
            {217, /* SP "M2.0V          " idx */ 248, /*MAG*/  8.992000000e+00,  7.490000000e+00,  5.420000000e+00,  4.320000000e+00,  3.722000000e+00,  3.501000000e+00, /*EMAG*/  3.788781000e-02,  3.520633000e-02,  4.049057000e-02,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/  1.013843874e-01,  9.964287498e-02,  9.845709608e-02, /*EDIAM*/  5.550356982e-02,  4.835628667e-02,  4.532179065e-02, /*DMEAN*/  9.963220073e-02, /*EDMEAN*/  2.988764925e-02, /*CHI2*/  1.195534140e+00 },
            {218, /* SP "K0III          " idx */ 200, /*MAG*/  3.071000000e+00,  2.060000000e+00,  1.050000000e+00,  4.030000000e-01, -1.990000000e-01, -2.740000000e-01, /*EMAG*/  1.431782000e-02,  6.000000000e-03,  2.088061000e-02,  1.840000000e-01,  1.940000000e-01,  2.000000030e-01, /*DIAM*/  7.213550730e-01,  7.541990028e-01,  7.438773854e-01, /*EDIAM*/  5.100181599e-02,  4.685752876e-02,  4.530063222e-02, /*DMEAN*/  7.408965575e-01, /*EDMEAN*/  2.752393674e-02, /*CHI2*/  7.849933400e-02 },
            {219, /* SP "K0IIIa         " idx */ 200, /*MAG*/  3.410000000e+00,  2.240000000e+00,  1.110000000e+00,  3.110000000e-01, -2.100000000e-01, -3.030000000e-01, /*EMAG*/  2.000000000e-03,  2.000000000e-03,  2.000000000e-03,  1.980000000e-01,  1.600000000e-01,  1.700000000e-01, /*DIAM*/  7.606407879e-01,  7.642768240e-01,  7.551693564e-01, /*EDIAM*/  5.486992441e-02,  3.866636548e-02,  3.851998247e-02, /*DMEAN*/  7.598911155e-01, /*EDMEAN*/  2.485140655e-02, /*CHI2*/  1.167227811e-01 },
            {220, /* SP "M0III          " idx */ 240, /*MAG*/  3.646000000e+00,  2.070000000e+00,  3.300000000e-01, -9.570000000e-01, -1.674000000e+00, -1.846000000e+00, /*EMAG*/  1.077033000e-02,  4.000000000e-03,  3.026549000e-02,  2.000000030e-01,  1.480000000e-01,  1.600000000e-01, /*DIAM*/  1.154180919e+00,  1.172248446e+00,  1.155909685e+00, /*EDIAM*/  5.549592368e-02,  3.588190133e-02,  3.632185839e-02, /*DMEAN*/  1.162435730e+00, /*EDMEAN*/  2.361707992e-02, /*CHI2*/  9.281696065e-02 },
            {221, /* SP "K1IIIb         " idx */ 204, /*MAG*/  3.161000000e+00,  2.010000000e+00,  8.800000000e-01, -1.960000000e-01, -7.490000000e-01, -7.830000000e-01, /*EMAG*/  7.211103000e-03,  4.000000000e-03,  4.000000000e-03,  2.000000030e-01,  1.560000000e-01,  1.760000000e-01, /*DIAM*/  8.922131942e-01,  8.907645204e-01,  8.633529261e-01, /*EDIAM*/  5.542796088e-02,  3.770730718e-02,  3.987875299e-02, /*DMEAN*/  8.806498809e-01, /*EDMEAN*/  2.496971629e-02, /*CHI2*/  1.288524766e+00 },
            {222, /* SP "K3Ib...        " idx */ 212, /*MAG*/  5.460000000e+00,  3.770000000e+00,  2.130000000e+00,  1.074000000e+00,  1.620000000e-01,  1.620000000e-01, /*EMAG*/  7.615773000e-03,  3.000000000e-03,  3.014963000e-02,  2.000000030e-01,  2.000000000e-01,  1.700000000e-01, /*DIAM*/  6.929794566e-01,  7.564933274e-01,  7.073282795e-01, /*EDIAM*/  5.544217557e-02,  4.831867292e-02,  3.853274556e-02, /*DMEAN*/  7.188118313e-01, /*EDMEAN*/  2.686554974e-02, /*CHI2*/  3.720374018e-01 },
            {223, /* SP "K5III          " idx */ 220, /*MAG*/  6.114000000e+00,  4.560000000e+00,  2.890000000e+00,  1.642000000e+00,  8.000000000e-01,  7.670000000e-01, /*EMAG*/  1.581139000e-02,  5.000000000e-03,  3.041381000e-02,  2.000000030e-01,  1.700000000e-01,  1.800000000e-01, /*DIAM*/  6.112896560e-01,  6.487747167e-01,  6.028235261e-01, /*EDIAM*/  5.546814409e-02,  4.112376633e-02,  4.080990358e-02, /*DMEAN*/  6.225536559e-01, /*EDMEAN*/  2.607934429e-02, /*CHI2*/  3.298949679e-01 },
            {224, /* SP "M4II           " idx */ 256, /*MAG*/  4.848000000e+00,  3.320000000e+00,  5.600000000e-01, -7.790000000e-01, -1.675000000e+00, -1.904000000e+00, /*EMAG*/  1.345362000e-02,  9.000000000e-03,  6.067125000e-02,  1.820000000e-01,  1.580000000e-01,  1.520000000e-01, /*DIAM*/  1.173067903e+00,  1.228653379e+00,  1.222062058e+00, /*EDIAM*/  5.064375999e-02,  3.847655000e-02,  3.462016529e-02, /*DMEAN*/  1.214380478e+00, /*EDMEAN*/  2.344895447e-02, /*CHI2*/  2.908528848e-01 },
            {225, /* SP "F5Iab:         " idx */ 140, /*MAG*/  2.271000000e+00,  1.790000000e+00,  1.160000000e+00,  8.300000000e-01,  4.130000000e-01,  5.200000000e-01, /*EMAG*/  5.656854000e-03,  4.000000000e-03,  4.019950000e-02,  1.840000000e-01,  1.620000000e-01,  1.640000000e-01, /*DIAM*/  5.028587584e-01,  5.604393399e-01,  5.095917567e-01, /*EDIAM*/  5.103681428e-02,  3.921418585e-02,  3.719301983e-02, /*DMEAN*/  5.269423379e-01, /*EDMEAN*/  2.438580837e-02, /*CHI2*/  6.675692771e-01 },
            {226, /* SP "M1IIIb...      " idx */ 244, /*MAG*/  4.558000000e+00,  2.970000000e+00,  1.190000000e+00,  1.030000000e-01, -7.620000000e-01, -9.530000000e-01, /*EMAG*/  7.211103000e-03,  4.000000000e-03,  2.039608000e-02,  1.620000000e-01,  1.940000000e-01,  2.000000030e-01, /*DIAM*/  9.269758100e-01,  9.928364925e-01,  9.825391045e-01, /*EDIAM*/  4.497537377e-02,  4.691204197e-02,  4.533103772e-02, /*DMEAN*/  9.666625049e-01, /*EDMEAN*/  2.760175804e-02, /*CHI2*/  4.514773355e-01 },
            {227, /* SP "K5III          " idx */ 220, /*MAG*/  2.408000000e+00,  8.700000000e-01, -8.000000000e-01, -2.095000000e+00, -2.775000000e+00, -3.044000000e+00, /*EMAG*/  1.360147000e-02,  1.100000000e-02,  2.282542000e-02,  1.940000000e-01,  1.700000000e-01,  1.400000000e-01, /*DIAM*/  1.362298596e+00,  1.359031537e+00,  1.368203100e+00, /*EDIAM*/  5.381611392e-02,  4.112575189e-02,  3.178263650e-02, /*DMEAN*/  1.364332356e+00, /*EDMEAN*/  2.322039082e-02, /*CHI2*/  8.225253106e-01 },
            {228, /* SP "K3II           " idx */ 212, /*MAG*/  4.180000000e+00,  2.690000000e+00,  1.230000000e+00, -3.100000000e-02, -6.760000000e-01, -8.440000000e-01, /*EMAG*/  4.210701000e-02,  3.000000000e-03,  3.014963000e-02,  1.880000000e-01,  1.520000000e-01,  1.580000000e-01, /*DIAM*/  9.158991027e-01,  9.141120068e-01,  9.065837569e-01, /*EDIAM*/  5.212616468e-02,  3.676085180e-02,  3.582238937e-02, /*DMEAN*/  9.113512321e-01, /*EDMEAN*/  2.345369031e-02, /*CHI2*/  8.083850473e-01 },
            {229, /* SP "K2II           " idx */ 208, /*MAG*/  5.839000000e+00,  4.470000000e+00,  3.150000000e+00,  2.158000000e+00,  1.504000000e+00,  1.342000000e+00, /*EMAG*/  6.606815000e-02,  3.000000000e-03,  2.022375000e-02,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/  4.382996280e-01,  4.550158849e-01,  4.528990319e-01, /*EDIAM*/  5.543376170e-02,  4.831095443e-02,  4.530631186e-02, /*DMEAN*/  4.498024431e-01, /*EDMEAN*/  2.976225171e-02, /*CHI2*/  7.008704105e-02 },
            {230, /* SP "M3II           " idx */ 252, /*MAG*/  6.001000000e+00,  4.300000000e+00,  1.790000000e+00,  2.450000000e-01, -6.020000000e-01, -8.130000000e-01, /*EMAG*/  1.486607000e-02,  5.000000000e-03,  3.041381000e-02,  2.000000030e-01,  1.740000000e-01,  1.680000000e-01, /*DIAM*/  9.761101005e-01,  1.011691109e+00,  9.957989700e-01, /*EDIAM*/  5.545171021e-02,  4.210571161e-02,  3.809498145e-02, /*DMEAN*/  9.974262117e-01, /*EDMEAN*/  2.559760199e-02, /*CHI2*/  3.914380954e-01 },
            {231, /* SP "M3.0IIIa       " idx */ 252, /*MAG*/  4.491000000e+00,  2.870000000e+00,  5.700000000e-01, -9.120000000e-01, -1.608000000e+00, -1.862000000e+00, /*EMAG*/  9.219544000e-03,  6.000000000e-03,  5.035871000e-02,  1.880000000e-01,  1.900000000e-01,  1.800000000e-01, /*DIAM*/  1.186547604e+00,  1.195403174e+00,  1.195587294e+00, /*EDIAM*/  5.213692774e-02,  4.595499851e-02,  4.080505428e-02, /*DMEAN*/  1.193222111e+00, /*EDMEAN*/  2.669540312e-02, /*CHI2*/  3.089665554e-02 },
            {232, /* SP "G8Ib           " idx */ 192, /*MAG*/  4.437000000e+00,  3.060000000e+00,  1.840000000e+00,  8.860000000e-01,  2.320000000e-01,  1.250000000e-01, /*EMAG*/  1.513275000e-02,  2.000000000e-03,  1.019804000e-02,  2.000000030e-01,  1.540000000e-01,  1.720000000e-01, /*DIAM*/  6.468827577e-01,  6.809498324e-01,  6.692447512e-01, /*EDIAM*/  5.541761655e-02,  3.721861252e-02,  3.896781983e-02, /*DMEAN*/  6.699340477e-01, /*EDMEAN*/  2.463227585e-02, /*CHI2*/  8.714032893e-02 },
            {233, /* SP "K4III          " idx */ 216, /*MAG*/  5.011000000e+00,  3.530000000e+00,  2.060000000e+00,  1.191000000e+00,  2.670000000e-01,  1.900000000e-01, /*EMAG*/  6.708204000e-03,  3.000000000e-03,  2.022375000e-02,  2.000000030e-01,  1.600000000e-01,  2.000000030e-01, /*DIAM*/  6.499950706e-01,  7.280416308e-01,  7.005198736e-01, /*EDIAM*/  5.545333446e-02,  3.869999407e-02,  4.531819496e-02, /*DMEAN*/  7.018441105e-01, /*EDMEAN*/  2.663258106e-02, /*CHI2*/  6.590543497e-01 },
            {234, /* SP "M3III          " idx */ 252, /*MAG*/  6.282000000e+00,  4.740000000e+00,  2.590000000e+00,  1.381000000e+00,  3.890000000e-01,  3.050000000e-01, /*EMAG*/  1.019804000e-02,  2.000000000e-03,  2.000000000e-03,  2.000000030e-01,  1.720000000e-01,  2.000000030e-01, /*DIAM*/  6.954672392e-01,  7.907650361e-01,  7.543829080e-01, /*EDIAM*/  5.545059376e-02,  4.162428822e-02,  4.532285764e-02, /*DMEAN*/  7.557209028e-01, /*EDMEAN*/  2.901248438e-02, /*CHI2*/  6.374560064e-01 },
            {235, /* SP "K7III          " idx */ 228, /*MAG*/  4.690000000e+00,  3.140000000e+00,  1.490000000e+00,  1.200000000e-01, -5.780000000e-01, -6.560000000e-01, /*EMAG*/  8.944272000e-03,  4.000000000e-03,  3.026549000e-02,  2.000000030e-01,  1.700000000e-01,  1.720000000e-01, /*DIAM*/  9.342560259e-01,  9.358898953e-01,  8.988258996e-01, /*EDIAM*/  5.549882344e-02,  4.116281821e-02,  3.902843436e-02, /*DMEAN*/  9.200622442e-01, /*EDMEAN*/  2.562222908e-02, /*CHI2*/  8.150805910e-01 },
            {236, /* SP "K3II-III       " idx */ 212, /*MAG*/  3.430000000e+00,  1.990000000e+00,  6.000000000e-01, -2.560000000e-01, -9.900000000e-01, -1.127000000e+00, /*EMAG*/  4.000000000e-03,  4.000000000e-03,  2.039608000e-02,  1.470000000e-01,  1.920000000e-01,  2.000000030e-01, /*DIAM*/  9.244258886e-01,  9.609913849e-01,  9.522260934e-01, /*EDIAM*/  4.080362037e-02,  4.639173767e-02,  4.531129493e-02, /*DMEAN*/  9.440913836e-01, /*EDMEAN*/  2.577540655e-02, /*CHI2*/  1.098946359e+00 },
            {237, /* SP "G1II           " idx */ 164, /*MAG*/  3.778000000e+00,  2.970000000e+00,  2.160000000e+00,  1.511000000e+00,  1.107000000e+00,  1.223000000e+00, /*EMAG*/  6.708204000e-03,  3.000000000e-03,  2.022375000e-02,  2.000000030e-01,  1.820000000e-01,  2.000000030e-01, /*DIAM*/  4.204525854e-01,  4.457325606e-01,  3.916009327e-01, /*EDIAM*/  5.543651681e-02,  4.399111514e-02,  4.529975526e-02, /*DMEAN*/  4.197014302e-01, /*EDMEAN*/  3.050353634e-02, /*CHI2*/  3.056758955e-01 },
            {238, /* SP "K1III          " idx */ 204, /*MAG*/  4.144000000e+00,  3.000000000e+00,  1.910000000e+00,  1.030000000e+00,  3.760000000e-01,  4.290000000e-01, /*EMAG*/  5.000000000e-03,  3.000000000e-03,  3.014963000e-02,  1.800000000e-01,  1.700000000e-01,  1.980000000e-01, /*DIAM*/  6.288917617e-01,  6.601964236e-01,  6.151193457e-01, /*EDIAM*/  4.989968914e-02,  4.107858714e-02,  4.485081516e-02, /*DMEAN*/  6.367429791e-01, /*EDMEAN*/  2.628070771e-02, /*CHI2*/  4.215557158e-01 },
            {239, /* SP "K3III          " idx */ 212, /*MAG*/  4.890000000e+00,  3.490000000e+00,  2.120000000e+00,  1.073000000e+00,  1.830000000e-01,  2.820000000e-01, /*EMAG*/  9.219544000e-03,  2.000000000e-03,  3.006659000e-02,  1.900000000e-01,  1.760000000e-01,  1.820000000e-01, /*DIAM*/  6.717562420e-01,  7.398785412e-01,  6.728173301e-01, /*EDIAM*/  5.267850580e-02,  4.253817034e-02,  4.124371400e-02, /*DMEAN*/  6.972576494e-01, /*EDMEAN*/  2.620788321e-02, /*CHI2*/  7.195296433e-01 },
            {240, /* SP "M0III          " idx */ 240, /*MAG*/  5.433000000e+00,  3.820000000e+00,  2.030000000e+00,  8.950000000e-01, -4.900000000e-02, -1.140000000e-01, /*EMAG*/  9.486833000e-03,  3.000000000e-03,  3.014963000e-02,  2.000000030e-01,  1.960000000e-01,  1.740000000e-01, /*DIAM*/  7.759487701e-01,  8.524040833e-01,  8.099826722e-01, /*EDIAM*/  5.549555183e-02,  4.741818600e-02,  3.948025500e-02, /*DMEAN*/  8.155257246e-01, /*EDMEAN*/  2.705502539e-02, /*CHI2*/  3.819480674e-01 },
            {241, /* SP "M1III          " idx */ 244, /*MAG*/  5.541000000e+00,  4.040000000e+00,  2.250000000e+00,  1.176000000e+00,  3.250000000e-01,  1.570000000e-01, /*EMAG*/  3.224903000e-02,  4.000000000e-03,  2.039608000e-02,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/  7.121454496e-01,  7.747353220e-01,  7.594880063e-01, /*EDIAM*/  5.546743221e-02,  4.835620636e-02,  4.533103772e-02, /*DMEAN*/  7.523389713e-01, /*EDMEAN*/  2.957014604e-02, /*CHI2*/  8.422721921e-01 },
            {242, /* SP "M3III          " idx */ 252, /*MAG*/  4.961000000e+00,  3.390000000e+00,  1.150000000e+00, -1.130000000e-01, -1.010000000e+00, -1.189000000e+00, /*EMAG*/  1.431782000e-02,  3.000000000e-03,  2.022375000e-02,  1.530000000e-01,  1.740000000e-01,  1.940000000e-01, /*DIAM*/  1.005324387e+00,  1.072586052e+00,  1.056966854e+00, /*EDIAM*/  4.247064579e-02,  4.210538840e-02,  4.396731278e-02, /*DMEAN*/  1.044853199e+00, /*EDMEAN*/  2.512530903e-02, /*CHI2*/  4.658715906e-01 },
            {243, /* SP "G8III          " idx */ 192, /*MAG*/  3.784000000e+00,  2.850000000e+00,  2.020000000e+00,  1.248000000e+00,  7.330000000e-01,  6.640000000e-01, /*EMAG*/  1.513275000e-02,  2.000000000e-03,  1.019804000e-02,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/  5.305613274e-01,  5.514245386e-01,  5.417629975e-01, /*EDIAM*/  5.541761655e-02,  4.830052997e-02,  4.529685780e-02, /*DMEAN*/  5.421615329e-01, /*EDMEAN*/  2.873768151e-02, /*CHI2*/  2.444462691e-01 },
            {244, /* SP "K4III          " idx */ 216, /*MAG*/  3.535000000e+00,  2.070000000e+00,  6.100000000e-01, -4.950000000e-01, -1.225000000e+00, -1.287000000e+00, /*EMAG*/  5.830952000e-03,  3.000000000e-03,  3.014963000e-02,  1.940000000e-01,  1.840000000e-01,  2.000000030e-01, /*DIAM*/  1.004548647e+00,  1.027761480e+00,  9.963665933e-01, /*EDIAM*/  5.379558896e-02,  4.447676085e-02,  4.531819496e-02, /*DMEAN*/  1.010342324e+00, /*EDMEAN*/  2.770617809e-02, /*CHI2*/  9.163502449e-02 },
            {245, /* SP "G8IIIa         " idx */ 192, /*MAG*/  4.446000000e+00,  3.490000000e+00,  2.600000000e+00,  1.795000000e+00,  1.268000000e+00,  1.223000000e+00, /*EMAG*/  2.000000000e-03,  2.000000000e-03,  3.006659000e-02,  2.000000030e-01,  1.180000000e-01,  1.660000000e-01, /*DIAM*/  4.283023973e-01,  4.487552764e-01,  4.320914630e-01, /*EDIAM*/  5.541761655e-02,  2.855413123e-02,  3.761189234e-02, /*DMEAN*/  4.406000968e-01, /*EDMEAN*/  2.389769490e-02, /*CHI2*/  1.077437297e+00 },
            {246, /* SP "G8IV           " idx */ 192, /*MAG*/  4.421000000e+00,  3.460000000e+00,  2.500000000e+00,  1.659000000e+00,  9.850000000e-01,  1.223000000e+00, /*EMAG*/  3.605551000e-03,  2.000000000e-03,  3.006659000e-02,  2.000000030e-01,  1.920000000e-01,  1.960000000e-01, /*DIAM*/  4.636416836e-01,  5.157902969e-01,  4.313031418e-01, /*EDIAM*/  5.541761655e-02,  4.637271169e-02,  4.439259140e-02, /*DMEAN*/  4.696807002e-01, /*EDMEAN*/  2.818773911e-02, /*CHI2*/  9.930409067e-01 },
            {247, /* SP "K2III          " idx */ 208, /*MAG*/  3.797000000e+00,  2.630000000e+00,  1.540000000e+00,  8.310000000e-01,  2.890000000e-01,  1.500000000e-01, /*EMAG*/  1.529706000e-02,  3.000000000e-03,  2.022375000e-02,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/  6.643085600e-01,  6.722376780e-01,  6.742712980e-01, /*EDIAM*/  5.543376170e-02,  4.831095443e-02,  4.530631186e-02, /*DMEAN*/  6.709573463e-01, /*EDMEAN*/  2.873525803e-02, /*CHI2*/  8.744211771e-02 },
            {248, /* SP "M0.5III        " idx */ 242, /*MAG*/  4.314000000e+00,  2.730000000e+00,  9.100000000e-01, -1.310000000e-01, -1.025000000e+00, -1.173000000e+00, /*EMAG*/  1.044031000e-02,  3.000000000e-03,  2.022375000e-02,  1.660000000e-01,  1.800000000e-01,  2.000000030e-01, /*DIAM*/  9.750769458e-01,  1.044796871e+00,  1.023502603e+00, /*EDIAM*/  4.609759860e-02,  4.355780595e-02,  4.534115217e-02, /*DMEAN*/  1.015713938e+00, /*EDMEAN*/  2.644327718e-02, /*CHI2*/  4.733029334e-01 },
            {249, /* SP "G8III-IV       " idx */ 192, /*MAG*/  3.640000000e+00,  2.730000000e+00,  1.890000000e+00,  1.081000000e+00,  5.840000000e-01,  5.790000000e-01, /*EMAG*/  2.236068000e-03,  2.000000000e-03,  3.006659000e-02,  2.000000030e-01,  1.520000000e-01,  2.000000030e-01, /*DIAM*/  5.675702566e-01,  5.824206480e-01,  5.578432897e-01, /*EDIAM*/  5.541761655e-02,  3.673700041e-02,  4.529685780e-02, /*DMEAN*/  5.716040746e-01, /*EDMEAN*/  2.669748003e-02, /*CHI2*/  6.393617234e-02 },
            {250, /* SP "M1.5Iab-b      " idx */ 246, /*MAG*/  2.925000000e+00,  1.060000000e+00, -1.840000000e+00, -2.850000000e+00, -3.725000000e+00, -4.100000000e+00, /*EMAG*/  1.612452000e-02,  8.000000000e-03,  3.104835000e-02,  9.300000000e-02,  1.460000000e-01,  1.600000000e-01, /*DIAM*/  1.595266776e+00,  1.630115857e+00,  1.646939273e+00, /*EDIAM*/  2.594838159e-02,  3.535185798e-02,  3.628615203e-02, /*DMEAN*/  1.617291150e+00, /*EDMEAN*/  1.863579899e-02, /*CHI2*/  6.160706694e-01 },
            {251, /* SP "G7.5IIIb       " idx */ 190, /*MAG*/  4.396000000e+00,  3.480000000e+00,  2.590000000e+00,  1.768000000e+00,  1.366000000e+00,  1.333000000e+00, /*EMAG*/  7.280110000e-03,  2.000000000e-03,  3.006659000e-02,  2.000000030e-01,  1.920000000e-01,  2.000000030e-01, /*DIAM*/  4.307762586e-01,  4.223890868e-01,  4.044438589e-01, /*EDIAM*/  5.541704516e-02,  4.637281693e-02,  4.529616812e-02, /*DMEAN*/  4.176808623e-01, /*EDMEAN*/  2.855926486e-02, /*CHI2*/  5.269130604e-02 },
            {252, /* SP "K3II           " idx */ 212, /*MAG*/  4.597000000e+00,  3.160000000e+00,  1.850000000e+00,  7.240000000e-01, -1.120000000e-01, -2.300000000e-02, /*EMAG*/  2.000000000e-03,  2.000000000e-03,  2.000000000e-03,  1.780000000e-01,  1.780000000e-01,  1.600000000e-01, /*DIAM*/  7.430151716e-01,  7.974349622e-01,  7.331603967e-01, /*EDIAM*/  4.936308091e-02,  4.301977428e-02,  3.627401617e-02, /*DMEAN*/  7.558225402e-01, /*EDMEAN*/  2.482640028e-02, /*CHI2*/  1.075567575e+00 },
            {253, /* SP "G2Iab:         " idx */ 168, /*MAG*/  3.744000000e+00,  2.790000000e+00,  1.860000000e+00,  1.351000000e+00,  7.960000000e-01,  7.560000000e-01, /*EMAG*/  7.280110000e-03,  2.000000000e-03,  3.006659000e-02,  2.000000030e-01,  1.520000000e-01,  2.000000030e-01, /*DIAM*/  4.555831193e-01,  5.148029006e-01,  4.952319303e-01, /*EDIAM*/  5.543139134e-02,  3.676241711e-02,  4.529748377e-02, /*DMEAN*/  4.962596323e-01, /*EDMEAN*/  2.641360517e-02, /*CHI2*/  2.986472820e-01 },
            {254, /* SP "G5IV           " idx */ 180, /*MAG*/  4.170000000e+00,  3.420000000e+00,  2.710000000e+00,  1.867000000e+00,  1.559000000e+00,  1.511000000e+00, /*EMAG*/  1.513275000e-02,  2.000000000e-03,  2.009975000e-02,  2.000000030e-01,  1.840000000e-01,  1.980000000e-01, /*DIAM*/  3.793429375e-01,  3.638292362e-01,  3.513454260e-01, /*EDIAM*/  5.541916550e-02,  4.445026466e-02,  4.484220597e-02, /*DMEAN*/  3.629562759e-01, /*EDMEAN*/  2.881133684e-02, /*CHI2*/  2.270608817e+00 },
            {255, /* SP "K1IIaCN+...    " idx */ 204, /*MAG*/  5.210000000e+00,  3.860000000e+00,  2.690000000e+00,  1.729000000e+00,  1.056000000e+00,  9.990000000e-01, /*EMAG*/  3.605551000e-03,  2.000000000e-03,  3.006659000e-02,  2.000000030e-01,  1.660000000e-01,  1.900000000e-01, /*DIAM*/  5.014542598e-01,  5.316205462e-01,  5.087397821e-01, /*EDIAM*/  5.542732264e-02,  4.011511026e-02,  4.304257048e-02, /*DMEAN*/  5.167112946e-01, /*EDMEAN*/  2.633765019e-02, /*CHI2*/  2.719963902e-01 },
            {256, /* SP "K5III          " idx */ 220, /*MAG*/  3.761000000e+00,  2.240000000e+00,  7.000000000e-01, -2.450000000e-01, -1.034000000e+00, -1.162000000e+00, /*EMAG*/  5.830952000e-03,  3.000000000e-03,  3.014963000e-02,  2.000000030e-01,  1.800000000e-01,  1.600000000e-01, /*DIAM*/  9.554414469e-01,  9.955295857e-01,  9.783490792e-01, /*EDIAM*/  5.546729371e-02,  4.352969319e-02,  3.629417771e-02, /*DMEAN*/  9.793578021e-01, /*EDMEAN*/  2.555325661e-02, /*CHI2*/  2.998471433e-01 },
            {257, /* SP "A0Va           " idx */ 80, /*MAG*/  2.900000000e-02,  3.000000000e-02,  4.000000000e-02, -1.770000000e-01, -2.900000000e-02,  1.290000000e-01, /*EMAG*/  5.001988000e-03,  1.000000047e-03,  2.000050000e-02,  2.000000030e-01,  1.460000000e-01,  1.860000000e-01, /*DIAM*/  5.566844984e-01,  5.092371058e-01,  4.714901362e-01, /*EDIAM*/  5.585870990e-02,  3.580132847e-02,  4.241861761e-02, /*DMEAN*/  5.057223436e-01, /*EDMEAN*/  2.509144870e-02, /*CHI2*/  5.042659192e-01 },
            {258, /* SP "M4II           " idx */ 256, /*MAG*/  5.795000000e+00,  4.220000000e+00,  1.620000000e+00, -1.120000000e-01, -1.039000000e+00, -1.255000000e+00, /*EMAG*/  1.140175000e-02,  7.000000000e-03,  3.080584000e-02,  2.000000030e-01,  1.660000000e-01,  1.760000000e-01, /*DIAM*/  1.057558972e+00,  1.112342093e+00,  1.098857677e+00, /*EDIAM*/  5.560013014e-02,  4.038864725e-02,  4.001988941e-02, /*DMEAN*/  1.095617791e+00, /*EDMEAN*/  2.606703663e-02, /*CHI2*/  8.880931512e-01 },
            {259, /* SP "M5III          " idx */ 260, /*MAG*/  5.477000000e+00,  4.080000000e+00,  9.400000000e-01, -7.380000000e-01, -1.575000000e+00, -1.837000000e+00, /*EMAG*/  1.910497000e-02,  1.300000000e-02,  4.205948000e-02,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/  1.205665514e+00,  1.232126990e+00,  1.232314572e+00, /*EDIAM*/  5.606498477e-02,  4.903371479e-02,  4.574870186e-02, /*DMEAN*/  1.225275340e+00, /*EDMEAN*/  2.939292874e-02, /*CHI2*/  6.091469275e-01 },
            {260, /* SP "G9III          " idx */ 196, /*MAG*/  4.060000000e+00,  3.070000000e+00,  2.130000000e+00,  1.375000000e+00,  8.370000000e-01,  8.830000000e-01, /*EMAG*/  1.811077000e-02,  2.000000000e-03,  3.006659000e-02,  2.000000030e-01,  1.660000000e-01,  1.980000000e-01, /*DIAM*/  5.209947159e-01,  5.404227382e-01,  5.031841988e-01, /*EDIAM*/  5.541964649e-02,  4.010940873e-02,  4.484633782e-02, /*DMEAN*/  5.232269392e-01, /*EDMEAN*/  2.727115713e-02, /*CHI2*/  1.724322990e-01 },
            {261, /* SP "M0III          " idx */ 240, /*MAG*/  5.942000000e+00,  4.440000000e+00,  2.760000000e+00,  1.714000000e+00,  8.780000000e-01,  7.110000000e-01, /*EMAG*/  4.472136000e-03,  2.000000000e-03,  2.009975000e-02,  2.000000030e-01,  1.700000000e-01,  2.000000030e-01, /*DIAM*/  5.968684103e-01,  6.543418236e-01,  6.395958083e-01, /*EDIAM*/  5.549528622e-02,  4.116615276e-02,  4.534942143e-02, /*DMEAN*/  6.359149709e-01, /*EDMEAN*/  2.711548629e-02, /*CHI2*/  3.588213697e-01 },
            {262, /* SP "K3II           " idx */ 212, /*MAG*/  4.227000000e+00,  2.720000000e+00,  1.280000000e+00,  2.760000000e-01, -5.440000000e-01, -7.200000000e-01, /*EMAG*/  1.414214000e-02,  2.000000000e-03,  2.009975000e-02,  1.680000000e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/  8.332294587e-01,  8.835050024e-01,  8.793136835e-01, /*EDIAM*/  4.660087998e-02,  4.831858562e-02,  4.531120349e-02, /*DMEAN*/  8.651948368e-01, /*EDMEAN*/  2.733564909e-02, /*CHI2*/  2.430824829e-01 },
            {263, /* SP "F8Iab          " idx */ 152, /*MAG*/  2.903000000e+00,  2.230000000e+00,  1.580000000e+00,  1.369000000e+00,  8.520000000e-01,  8.270000000e-01, /*EMAG*/  1.236932000e-02,  3.000000000e-03,  3.014963000e-02,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/  3.929241270e-01,  4.741865725e-01,  4.557288757e-01, /*EDIAM*/  5.544728984e-02,  4.834197395e-02,  4.530869959e-02, /*DMEAN*/  4.456346070e-01, /*EDMEAN*/  2.876165147e-02, /*CHI2*/  8.988869737e-01 },
            {264, /* SP "A2Iae          " idx */ 88, /*MAG*/  1.342000000e+00,  1.250000000e+00,  1.090000000e+00,  1.139000000e+00,  9.020000000e-01,  1.010000000e+00, /*EMAG*/  9.899495000e-03,  7.000000000e-03,  3.080584000e-02,  2.000000030e-01,  1.880000000e-01,  2.000000030e-01, /*DIAM*/  3.144175342e-01,  3.596268268e-01,  3.279837659e-01, /*EDIAM*/  5.586498666e-02,  4.580667822e-02,  4.555834370e-02, /*DMEAN*/  3.363854984e-01, /*EDMEAN*/  3.008986660e-02, /*CHI2*/  9.776651902e-01 },
            {265, /* SP "K0III-IV       " idx */ 200, /*MAG*/  3.501000000e+00,  2.480000000e+00,  1.480000000e+00,  6.410000000e-01,  1.040000000e-01, -7.000000000e-03, /*EMAG*/  1.513275000e-02,  2.000000000e-03,  2.009975000e-02,  2.000000030e-01,  1.600000000e-01,  2.000000030e-01, /*DIAM*/  6.877300725e-01,  6.984246829e-01,  6.944978227e-01, /*EDIAM*/  5.542283391e-02,  3.866636548e-02,  4.530038834e-02, /*DMEAN*/  6.947847039e-01, /*EDMEAN*/  2.636336363e-02, /*CHI2*/  4.253472209e-01 },
            {266, /* SP "K4.5Ib-II      " idx */ 218, /*MAG*/  5.329000000e+00,  3.720000000e+00,  2.090000000e+00,  9.820000000e-01,  2.700000000e-02, -3.800000000e-02, /*EMAG*/  5.830952000e-03,  3.000000000e-03,  1.044031000e-02,  2.000000030e-01,  1.780000000e-01,  2.000000030e-01, /*DIAM*/  7.260640795e-01,  7.971955814e-01,  7.600110941e-01, /*EDIAM*/  5.545999202e-02,  4.303987350e-02,  4.532250818e-02, /*DMEAN*/  7.667020388e-01, /*EDMEAN*/  2.757179111e-02, /*CHI2*/  3.618126092e-01 },
            {267, /* SP "M1III          " idx */ 244, /*MAG*/  6.138000000e+00,  4.520000000e+00,  2.700000000e+00,  1.701000000e+00,  7.840000000e-01,  6.660000000e-01, /*EMAG*/  8.544004000e-03,  3.000000000e-03,  3.014963000e-02,  2.000000030e-01,  1.660000000e-01,  1.820000000e-01, /*DIAM*/  6.036900909e-01,  6.838014685e-01,  6.569259610e-01, /*EDIAM*/  5.546706016e-02,  4.017561066e-02,  4.126544473e-02, /*DMEAN*/  6.565134178e-01, /*EDMEAN*/  2.597852363e-02, /*CHI2*/  4.713946808e-01 },
            {268, /* SP "K2Ib           " idx */ 208, /*MAG*/  3.957000000e+00,  2.380000000e+00,  9.600000000e-01, -1.170000000e-01, -7.500000000e-01, -8.600000000e-01, /*EMAG*/  3.000000000e-03,  4.000000000e-03,  2.039608000e-02,  1.660000000e-01,  1.920000000e-01,  2.000000030e-01, /*DIAM*/  9.075049922e-01,  9.125800941e-01,  8.962421042e-01, /*EDIAM*/  4.603915911e-02,  4.638369778e-02,  4.530636521e-02, /*DMEAN*/  9.053079286e-01, /*EDMEAN*/  3.029718805e-02, /*CHI2*/  4.217860599e-01 },
            {269, /* SP "G2Ib           " idx */ 168, /*MAG*/  3.919000000e+00,  2.950000000e+00,  2.030000000e+00,  1.278000000e+00,  7.400000000e-01,  5.900000000e-01, /*EMAG*/  1.170470000e-02,  4.000000000e-03,  2.039608000e-02,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/  4.880741912e-01,  5.349118504e-01,  5.369983543e-01, /*EDIAM*/  5.543202953e-02,  4.832007577e-02,  4.529757523e-02, /*DMEAN*/  5.234544766e-01, /*EDMEAN*/  2.942392120e-02, /*CHI2*/  2.405229591e-01 },
            {270, /* SP "K1.5Iab:       " idx */ 206, /*MAG*/  4.948000000e+00,  3.390000000e+00,  1.810000000e+00,  1.231000000e+00,  4.250000000e-01,  3.430000000e-01, /*EMAG*/  4.472136000e-03,  2.000000000e-03,  2.000000000e-03,  2.000000030e-01,  1.720000000e-01,  1.700000000e-01, /*DIAM*/  6.076092440e-01,  6.675764595e-01,  6.476863503e-01, /*EDIAM*/  5.543016616e-02,  4.156278930e-02,  3.852471384e-02, /*DMEAN*/  6.467213926e-01, /*EDMEAN*/  2.556712133e-02, /*CHI2*/  2.902076609e+00 },
            {271, /* SP "G8Iab:         " idx */ 192, /*MAG*/  5.040000000e+00,  3.970000000e+00,  2.980000000e+00,  2.030000000e+00,  1.462000000e+00,  1.508000000e+00, /*EMAG*/  2.828427000e-03,  2.000000000e-03,  3.006659000e-02,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/  4.001148969e-01,  4.217513849e-01,  3.802155498e-01, /*EDIAM*/  5.541761655e-02,  4.830052997e-02,  4.529685780e-02, /*DMEAN*/  3.997720845e-01, /*EDMEAN*/  2.974131680e-02, /*CHI2*/  3.752115083e-01 },
            {272, /* SP "G8III          " idx */ 192, /*MAG*/  4.443000000e+00,  3.510000000e+00,  2.620000000e+00,  1.700000000e+00,  1.243000000e+00,  1.181000000e+00, /*EMAG*/  1.019804000e-02,  2.000000000e-03,  3.006659000e-02,  2.000000030e-01,  1.700000000e-01,  1.560000000e-01, /*DIAM*/  4.561327549e-01,  4.556113076e-01,  4.421206602e-01, /*EDIAM*/  5.541761655e-02,  4.107225153e-02,  3.535230944e-02, /*DMEAN*/  4.494263656e-01, /*EDMEAN*/  2.513156914e-02, /*CHI2*/  1.391018663e+00 },
            {273, /* SP "M2III          " idx */ 248, /*MAG*/  5.356000000e+00,  3.730000000e+00,  1.660000000e+00,  4.230000000e-01, -4.300000000e-01, -6.680000000e-01, /*EMAG*/  4.428318000e-02,  5.000000000e-03,  2.061553000e-02,  1.820000000e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/  8.913040420e-01,  9.462109810e-01,  9.430045539e-01, /*EDIAM*/  5.046496686e-02,  4.833491930e-02,  4.531253805e-02, /*DMEAN*/  9.285340201e-01, /*EDMEAN*/  2.821140510e-02, /*CHI2*/  2.620705770e-01 },
            {274, /* SP "M2.5II-IIIe    " idx */ 250, /*MAG*/  4.095000000e+00,  2.440000000e+00,  1.300000000e-01, -1.432000000e+00, -2.129000000e+00, -2.379000000e+00, /*EMAG*/  1.280625000e-02,  8.000000000e-03,  6.053098000e-02,  1.940000000e-01,  1.800000000e-01,  1.540000000e-01, /*DIAM*/  1.301933759e+00,  1.303342579e+00,  1.298761056e+00, /*EDIAM*/  5.377975122e-02,  4.352332154e-02,  3.491941172e-02, /*DMEAN*/  1.300835543e+00, /*EDMEAN*/  2.469117306e-02, /*CHI2*/  4.336554802e-01 },
            {275, /* SP "M3III          " idx */ 252, /*MAG*/  6.001000000e+00,  4.370000000e+00,  2.020000000e+00,  6.230000000e-01, -2.380000000e-01, -3.990000000e-01, /*EMAG*/  1.208305000e-02,  5.000000000e-03,  2.061553000e-02,  1.640000000e-01,  1.900000000e-01,  2.000000030e-01, /*DIAM*/  8.768600990e-01,  9.267650382e-01,  9.039595526e-01, /*EDIAM*/  4.550813455e-02,  4.595479491e-02,  4.532301761e-02, /*DMEAN*/  9.023770334e-01, /*EDMEAN*/  2.665037128e-02, /*CHI2*/  2.528097088e-01 },
            {276, /* SP "K4V            " idx */ 216, /*MAG*/  6.744000000e+00,  5.720000000e+00,  4.490000000e+00,  3.663000000e+00,  3.085000000e+00,  3.048000000e+00, /*EMAG*/  1.581139000e-02,  5.000000000e-03,  8.015610000e-02,  2.000000030e-01,  1.960000000e-01,  2.000000030e-01, /*DIAM*/  1.339414915e-01,  1.385396765e-01,  1.113665802e-01, /*EDIAM*/  5.545418505e-02,  4.736663268e-02,  4.531831685e-02, /*DMEAN*/  1.268308919e-01, /*EDMEAN*/  3.014468920e-02, /*CHI2*/  5.648978404e-01 },
            {277, /* SP "K5V            " idx */ 220, /*MAG*/  5.746000000e+00,  4.690000000e+00,  3.540000000e+00,  2.894000000e+00,  2.349000000e+00,  2.237000000e+00, /*EMAG*/  1.612452000e-02,  2.000000000e-03,  2.009975000e-02,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/  2.747360796e-01,  2.804478630e-01,  2.736118424e-01, /*EDIAM*/  5.546702797e-02,  4.834397359e-02,  4.532730740e-02, /*DMEAN*/  2.762661991e-01, /*EDMEAN*/  2.880029141e-02, /*CHI2*/  3.994099452e-03 },
            {278, /* SP "A3Va           " idx */ 92, /*MAG*/  2.230000000e+00,  2.140000000e+00,  2.040000000e+00,  1.854000000e+00,  1.925000000e+00,  1.883000000e+00, /*EMAG*/  4.000000000e-03,  4.000000000e-03,  4.000000000e-03,  2.000000030e-01,  1.940000000e-01,  1.920000000e-01, /*DIAM*/  1.964486526e-01,  1.600390863e-01,  1.637103550e-01, /*EDIAM*/  5.584644767e-02,  4.722668997e-02,  4.375238828e-02, /*DMEAN*/  1.705525053e-01, /*EDMEAN*/  2.910885010e-02, /*CHI2*/  1.321335082e-01 },
            {279, /* SP "A4V            " idx */ 96, /*MAG*/  1.315000000e+00,  1.170000000e+00,  1.010000000e+00,  1.037000000e+00,  9.370000000e-01,  9.450000000e-01, /*EMAG*/  1.303840000e-02,  7.000000000e-03,  1.220656000e-02,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/  3.580609034e-01,  3.676706123e-01,  3.590711580e-01, /*EDIAM*/  5.581921765e-02,  4.864050679e-02,  4.553792622e-02, /*DMEAN*/  3.617717027e-01, /*EDMEAN*/  2.904231896e-02, /*CHI2*/  8.346514996e-02 },
            {280, /* SP "K1III-IV       " idx */ 204, /*MAG*/  4.435000000e+00,  3.520000000e+00,  2.580000000e+00,  2.060000000e+00,  1.729000000e+00,  1.619000000e+00, /*EMAG*/  1.824829000e-02,  3.000000000e-03,  2.022375000e-02,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/  3.837310438e-01,  3.552392206e-01,  3.595135025e-01, /*EDIAM*/  5.542758857e-02,  4.830591756e-02,  4.530289212e-02, /*DMEAN*/  3.643869533e-01, /*EDMEAN*/  2.861897255e-02, /*CHI2*/  1.282670319e-01 },
            {281, /* SP "K7V            " idx */ 228, /*MAG*/  7.359000000e+00,  6.050000000e+00,  4.780000000e+00,  3.546000000e+00,  2.895000000e+00,  2.544000000e+00, /*EMAG*/  1.500000000e-02,  9.000000000e-03,  1.345362000e-02,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/  2.094345865e-01,  2.180688730e-01,  2.512054520e-01, /*EDIAM*/  5.550227604e-02,  4.837870945e-02,  4.534950843e-02, /*DMEAN*/  2.288232229e-01, /*EDMEAN*/  2.909907410e-02, /*CHI2*/  5.437025070e-01 },
            {282, /* SP "K0V            " idx */ 200, /*MAG*/  6.730000000e+00,  5.880000000e+00,  5.050000000e+00,  4.549000000e+00,  4.064000000e+00,  3.999000000e+00, /*EMAG*/  2.906705000e-02,  2.763862000e-02,  2.939206000e-02,  2.000000030e-01,  2.000000030e-01,  3.600000000e-02, /*DIAM*/ -1.328770825e-01, -1.166726055e-01, -1.226262771e-01, /*EDIAM*/  5.546323904e-02,  4.831613889e-02,  8.423121966e-03, /*DMEAN*/ -1.226744968e-01, /*EDMEAN*/  1.648567463e-02, /*CHI2*/  5.326591863e-01 },
            {283, /* SP "F9V            " idx */ 156, /*MAG*/  4.636000000e+00,  4.100000000e+00,  3.520000000e+00,  3.175000000e+00,  2.957000000e+00,  2.859000000e+00, /*EMAG*/  7.615773000e-03,  3.000000000e-03,  3.014963000e-02,  2.000000030e-01,  1.780000000e-01,  2.000000030e-01, /*DIAM*/  3.935909569e-02,  4.410711393e-02,  4.678248052e-02, /*EDIAM*/  5.544451910e-02,  4.304065456e-02,  4.530535600e-02, /*DMEAN*/  4.392987280e-02, /*EDMEAN*/  2.745950958e-02, /*CHI2*/  5.002911421e-03 },
            {284, /* SP "G5IV           " idx */ 180, /*MAG*/  6.990000000e+00,  6.270000000e+00,  5.500000000e+00,  5.386000000e+00,  4.678000000e+00,  4.601000000e+00, /*EMAG*/  1.029563000e-02,  5.000000000e-03,  1.118034000e-02,  2.000000030e-01,  3.100000000e-02,  2.100000000e-02, /*DIAM*/ -3.758267166e-01, -2.710657149e-01, -2.729611527e-01, /*EDIAM*/  5.542028258e-02,  7.835725515e-03,  5.110401768e-03, /*DMEAN*/ -2.729791886e-01, /*EDMEAN*/  4.142439708e-02, /*CHI2*/  1.500903621e+00 },
            {285, /* SP "G9VCN+1        " idx */ 196, /*MAG*/  7.237000000e+00,  6.420000000e+00,  5.570000000e+00,  5.023000000e+00,  4.637000000e+00,  4.491000000e+00, /*EMAG*/  1.581139000e-02,  5.000000000e-03,  2.061553000e-02,  2.600000000e-02,  2.100000000e-02,  2.000000000e-02, /*DIAM*/ -2.314874382e-01, -2.381375846e-01, -2.251953741e-01, /*EDIAM*/  7.670484305e-03,  5.543346790e-03,  4.938372821e-03, /*DMEAN*/ -2.309629661e-01, /*EDMEAN*/  5.555528071e-02, /*CHI2*/  1.145575474e+00 },
            {286, /* SP "K1II-III       " idx */ 204, /*MAG*/  9.035000000e+00,  7.570000000e+00,  6.100000000e+00,  5.190000000e+00,  4.323000000e+00,  3.997000000e+00, /*EMAG*/  1.835756000e-02,  9.000000000e-03,  2.193171000e-02,  2.000000030e-01,  2.000000030e-01,  2.600000000e-02, /*DIAM*/ -1.716261074e-01, -1.035078680e-01, -7.215073749e-02, /*EDIAM*/  5.543141790e-02,  4.830718533e-02,  6.241469878e-03, /*DMEAN*/ -7.378273413e-02, /*EDMEAN*/  1.512062653e-02, /*CHI2*/  1.201394545e+00 },
            {287, /* SP "F8V            " idx */ 152, /*MAG*/  5.645000000e+00,  5.070000000e+00,  4.440000000e+00,  4.174000000e+00,  3.768000000e+00,  3.748000000e+00, /*EMAG*/  5.830952000e-03,  3.000000000e-03,  2.022375000e-02,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/ -1.653883813e-01, -1.121480665e-01, -1.305996002e-01, /*EDIAM*/  5.544728984e-02,  4.834197395e-02,  4.530869959e-02, /*DMEAN*/ -1.333526571e-01, /*EDMEAN*/  3.184208491e-02, /*CHI2*/  4.704006013e-01 },
            {288, /* SP "F8             " idx */ 152, /*MAG*/  7.884000000e+00,  7.250000000e+00,  6.550000000e+00,  6.173000000e+00,  5.962000000e+00,  5.874000000e+00, /*EMAG*/  1.204159000e-02,  8.000000000e-03,  1.280625000e-02,  1.900000000e-02,  2.100000000e-02,  2.300000000e-02, /*DIAM*/ -5.512901728e-01, -5.515255050e-01, -5.543806284e-01, /*EDIAM*/  6.161921264e-03,  5.893828661e-03,  5.651886306e-03, /*DMEAN*/ -5.525172230e-01, /*EDMEAN*/  4.915754636e-02, /*CHI2*/  2.521171781e+00 },
            {289, /* SP "G4V            " idx */ 176, /*MAG*/  6.723000000e+00,  5.950000000e+00,  5.220000000e+00,  4.905000000e+00,  4.384000000e+00,  4.211000000e+00, /*EMAG*/  5.099020000e-03,  5.000000000e-03,  4.031129000e-02,  2.000000030e-01,  7.600000000e-02,  3.600000000e-02, /*DIAM*/ -2.740161517e-01, -2.162047180e-01, -1.969795481e-01, /*EDIAM*/  5.542347316e-02,  1.849343766e-02,  8.362145043e-03, /*DMEAN*/ -2.014870908e-01, /*EDMEAN*/  3.716621470e-02, /*CHI2*/  1.277449370e+00 },
            {290, /* SP "K2III          " idx */ 208, /*MAG*/  6.576000000e+00,  5.450000000e+00,  4.430000000e+00,  3.630000000e+00,  3.065000000e+00,  2.922000000e+00, /*EMAG*/  6.708204000e-03,  3.000000000e-03,  4.011234000e-02,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/  1.061210516e-01,  1.188524558e-01,  1.211326036e-01, /*EDIAM*/  5.543376170e-02,  4.831095443e-02,  4.530631186e-02, /*DMEAN*/  1.164099293e-01, /*EDMEAN*/  2.873342783e-02, /*CHI2*/  1.990516705e+00 },
            {291, /* SP "G9III          " idx */ 196, /*MAG*/  6.809000000e+00,  5.780000000e+00,  4.780000000e+00,  4.112000000e+00,  3.547000000e+00,  3.274000000e+00, /*EMAG*/  5.830952000e-03,  3.000000000e-03,  3.000000000e-03,  2.000000030e-01,  1.940000000e-01,  2.000000030e-01, /*DIAM*/ -2.847850660e-02, -1.577269840e-03,  3.336667355e-02, /*EDIAM*/  5.541991247e-02,  4.685532260e-02,  4.529849757e-02, /*DMEAN*/  4.946771065e-03, /*EDMEAN*/  2.957196095e-02, /*CHI2*/  3.120391089e-01 },
            {292, /* SP "G4V            " idx */ 176, /*MAG*/  5.684000000e+00,  4.970000000e+00,  4.200000000e+00,  3.798000000e+00,  3.457000000e+00,  3.500000000e+00, /*EMAG*/  7.615773000e-03,  3.000000000e-03,  3.000000000e-03,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/ -4.286436255e-02, -3.299070744e-02, -6.184815921e-02, /*EDIAM*/  5.542262210e-02,  4.830931589e-02,  4.529476049e-02, /*DMEAN*/ -4.691177620e-02, /*EDMEAN*/  3.008775751e-02, /*CHI2*/  1.016114762e+00 },
            {293, /* SP "F6IV+M2        " idx */ 144, /*MAG*/  5.008000000e+00,  4.500000000e+00,  3.990000000e+00,  3.617000000e+00,  3.546000000e+00,  3.507000000e+00, /*EMAG*/  2.236068000e-03,  2.000000000e-03,  3.006659000e-02,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/ -5.897395270e-02, -8.306368572e-02, -9.384786077e-02, /*EDIAM*/  5.545195447e-02,  4.835075164e-02,  4.531749516e-02, /*DMEAN*/ -8.098576900e-02, /*EDMEAN*/  2.963486584e-02, /*CHI2*/  3.081904128e-01 },
            {294, /* SP "G0V            " idx */ 160, /*MAG*/  6.002000000e+00,  5.390000000e+00,  4.710000000e+00,  4.088000000e+00,  3.989000000e+00,  3.857000000e+00, /*EMAG*/  3.605551000e-03,  2.000000000e-03,  2.000000000e-03,  2.000000030e-01,  3.600000000e-02,  3.600000000e-02, /*DIAM*/ -1.109755200e-01, -1.508310034e-01, -1.431486179e-01, /*EDIAM*/  5.544065063e-02,  9.132956733e-03,  8.402512199e-03, /*DMEAN*/ -1.462394376e-01, /*EDMEAN*/  2.793265431e-02, /*CHI2*/  3.991563120e-01 },
            {295, /* SP "K0V            " idx */ 200, /*MAG*/  7.487000000e+00,  6.610000000e+00,  5.690000000e+00,  5.158000000e+00,  4.803000000e+00,  4.714000000e+00, /*EMAG*/  7.211103000e-03,  4.000000000e-03,  1.077033000e-02,  2.900000000e-02,  1.600000000e-02,  1.600000000e-02, /*DIAM*/ -2.453860128e-01, -2.648438139e-01, -2.652321187e-01, /*EDIAM*/  8.472146752e-03,  4.484121186e-03,  4.145245124e-03, /*DMEAN*/ -2.629999975e-01, /*EDMEAN*/  5.046015785e-02, /*CHI2*/  5.354162378e+00 },
            {296, /* SP "K0+M3.5V       " idx */ 200, /*MAG*/  8.273000000e+00,  7.180000000e+00,  6.130000000e+00,  5.367000000e+00,  4.954000000e+00,  4.809000000e+00, /*EMAG*/  8.602325000e-03,  5.000000000e-03,  1.118034000e-02,  1.800000000e-02,  4.400000000e-02,  2.700000000e-02, /*DIAM*/ -2.594663701e-01, -2.777621020e-01, -2.717503669e-01, /*EDIAM*/  5.676512524e-03,  1.085805385e-02,  6.434870362e-03, /*DMEAN*/ -2.664462087e-01, /*EDMEAN*/  5.316938480e-02, /*CHI2*/  1.639862037e+00 },
            {297, /* SP "G3V            " idx */ 172, /*MAG*/  6.911000000e+00,  6.250000000e+00,  5.640000000e+00,  4.993000000e+00,  4.695000000e+00,  4.651000000e+00, /*EMAG*/  6.403124000e-03,  4.000000000e-03,  3.026549000e-02,  3.700000000e-02,  3.600000000e-02,  1.600000000e-02, /*DIAM*/ -2.814161715e-01, -2.812163016e-01, -2.921252011e-01, /*EDIAM*/  1.061333124e-02,  9.040932632e-03,  4.094570705e-03, /*DMEAN*/ -2.895921154e-01, /*EDMEAN*/  5.625518079e-02, /*CHI2*/  1.346609627e+00 },
            {298, /* SP "G5IV           " idx */ 180, /*MAG*/  8.093000000e+00,  7.300000000e+00,  6.480000000e+00,  5.819000000e+00,  5.441000000e+00,  5.352000000e+00, /*EMAG*/  7.810250000e-03,  5.000000000e-03,  5.000000000e-03,  1.800000000e-02,  2.000000000e-02,  1.600000000e-02, /*DIAM*/ -4.165856457e-01, -4.126532657e-01, -4.158297679e-01, /*EDIAM*/  5.640583343e-03,  5.361794014e-03,  4.079698315e-03, /*DMEAN*/ -4.151659258e-01, /*EDMEAN*/  4.325284913e-02, /*CHI2*/  1.019615917e+00 },
            {299, /* SP "G7IV-V         " idx */ 188, /*MAG*/  6.479000000e+00,  5.730000000e+00,  4.900000000e+00,  4.554000000e+00,  4.239000000e+00,  4.076000000e+00, /*EMAG*/  3.162278000e-03,  3.000000000e-03,  7.006426000e-02,  2.000000030e-01,  2.000000030e-01,  2.700000000e-02, /*DIAM*/ -1.717145843e-01, -1.800842125e-01, -1.595292995e-01, /*EDIAM*/  5.541705478e-02,  4.830108219e-02,  6.399963782e-03, /*DMEAN*/ -1.600069650e-01, /*EDMEAN*/  1.343382356e-02, /*CHI2*/  8.667726269e-02 },
            {300, /* SP "G2.5IVa        " idx */ 170, /*MAG*/  6.116000000e+00,  5.450000000e+00,  4.760000000e+00,  4.655000000e+00,  4.234000000e+00,  3.911000000e+00, /*EMAG*/  7.615773000e-03,  3.000000000e-03,  1.044031000e-02,  2.000000030e-01,  2.000000030e-01,  2.100000000e-02, /*DIAM*/ -2.520678368e-01, -2.039980497e-01, -1.472874725e-01, /*EDIAM*/  5.542921110e-02,  4.831704725e-02,  5.128683865e-03, /*DMEAN*/ -1.486448785e-01, /*EDMEAN*/  1.593100465e-02, /*CHI2*/  2.291411381e+00 },
            {301, /* SP "K0V            " idx */ 200, /*MAG*/  6.434000000e+00,  5.630000000e+00,  4.820000000e+00,  4.596000000e+00,  4.133000000e+00,  4.014000000e+00, /*EMAG*/  1.019804000e-02,  2.000000000e-03,  2.000000000e-03,  2.000000030e-01,  2.000000000e-01,  2.000000030e-01, /*DIAM*/ -1.650824402e-01, -1.436298043e-01, -1.325897809e-01, /*EDIAM*/  5.542283391e-02,  4.830275880e-02,  4.530038834e-02, /*DMEAN*/ -1.449202048e-01, /*EDMEAN*/  3.081577191e-02, /*CHI2*/  3.423858174e-01 },
            {302, /* SP "G9V            " idx */ 196, /*MAG*/  5.456000000e+00,  4.670000000e+00,  3.820000000e+00,  3.423000000e+00,  3.039000000e+00,  2.900000000e+00, /*EMAG*/  9.899495000e-03,  7.000000000e-03,  2.118962000e-02,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/  7.699470926e-02,  7.519315932e-02,  8.882652839e-02, /*EDIAM*/  5.542204020e-02,  4.830188728e-02,  4.529880244e-02, /*DMEAN*/  8.101787761e-02, /*EDMEAN*/  2.866560474e-02, /*CHI2*/  1.201265126e-01 },
            {303, /* SP "G5IV           " idx */ 180, /*MAG*/  7.500000000e+00,  6.830000000e+00,  6.100000000e+00,  5.679000000e+00,  5.346000000e+00,  5.266000000e+00, /*EMAG*/  8.062258000e-03,  7.000000000e-03,  7.000000000e-03,  2.100000000e-02,  4.000000000e-02,  2.300000000e-02, /*DIAM*/ -4.139249314e-01, -4.091201917e-01, -4.087202788e-01, /*EDIAM*/  6.396959758e-03,  9.931199335e-03,  5.535206018e-03, /*DMEAN*/ -4.106565175e-01, /*EDMEAN*/  4.272415277e-02, /*CHI2*/  2.008406981e+00 },
            {304, /* SP "K0III-IV       " idx */ 200, /*MAG*/  6.794000000e+00,  5.860000000e+00,  4.930000000e+00,  4.272000000e+00,  3.825000000e+00,  3.701000000e+00, /*EMAG*/  7.810250000e-03,  5.000000000e-03,  5.000000000e-03,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/ -5.774315285e-02, -5.983991981e-02, -5.572116663e-02, /*EDIAM*/  5.542395092e-02,  4.830312931e-02,  4.530054839e-02, /*DMEAN*/ -5.767325690e-02, /*EDMEAN*/  3.223056011e-02, /*CHI2*/  6.351451583e-01 },
            {305, /* SP "G8V            " idx */ 192, /*MAG*/  7.488000000e+00,  6.760000000e+00,  5.980000000e+00,  5.411000000e+00,  5.098000000e+00,  5.003000000e+00, /*EMAG*/  7.810250000e-03,  6.000000000e-03,  1.166190000e-02,  4.100000000e-02,  2.000000000e-02,  1.700000000e-02, /*DIAM*/ -3.214654710e-01, -3.403420116e-01, -3.373100083e-01, /*EDIAM*/  1.164772827e-02,  5.320401439e-03,  4.308904452e-03, /*DMEAN*/ -3.373007259e-01, /*EDMEAN*/  2.281892845e-02, /*CHI2*/  1.611052885e+00 },
            {306, /* SP "G6III:         " idx */ 184, /*MAG*/  6.444000000e+00,  5.510000000e+00,  4.580000000e+00,  4.032000000e+00,  3.440000000e+00,  3.366000000e+00, /*EMAG*/  5.000000000e-03,  3.000000000e-03,  3.000000000e-03,  2.000000030e-01,  1.860000000e-01,  2.000000030e-01, /*DIAM*/ -5.204328521e-02, -3.497782446e-04, -9.243723546e-03, /*EDIAM*/  5.541754852e-02,  4.492924171e-02,  4.529472499e-02, /*DMEAN*/ -1.652332025e-02, /*EDMEAN*/  2.960599246e-02, /*CHI2*/  1.743334635e+00 },
            {307, /* SP "K1III          " idx */ 204, /*MAG*/  6.881000000e+00,  5.930000000e+00,  4.990000000e+00,  4.508000000e+00,  3.995000000e+00,  3.984000000e+00, /*EMAG*/  3.162278000e-03,  3.000000000e-03,  3.014963000e-02,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/ -1.087868207e-01, -9.202148647e-02, -1.123040228e-01, /*EDIAM*/  5.542758857e-02,  4.830591756e-02,  4.530289212e-02, /*DMEAN*/ -1.043794120e-01, /*EDMEAN*/  2.962744432e-02, /*CHI2*/  9.055533736e-01 },
            {308, /* SP "G8III          " idx */ 192, /*MAG*/  6.249000000e+00,  5.220000000e+00,  4.220000000e+00,  3.019000000e+00,  2.608000000e+00,  2.331000000e+00, /*EMAG*/  4.242641000e-03,  3.000000000e-03,  3.000000000e-03,  1.800000000e-01,  1.840000000e-01,  2.000000030e-01, /*DIAM*/  2.223559657e-01,  1.968408758e-01,  2.268359855e-01, /*EDIAM*/  4.988890764e-02,  4.444517587e-02,  4.529689591e-02, /*DMEAN*/  2.146634150e-01, /*EDMEAN*/  2.692612198e-02, /*CHI2*/  3.577629705e+00 },
            {309, /* SP "K1III          " idx */ 204, /*MAG*/  7.531000000e+00,  6.430000000e+00,  5.370000000e+00,  4.531000000e+00,  3.992000000e+00,  3.911000000e+00, /*EMAG*/  6.403124000e-03,  4.000000000e-03,  1.077033000e-02,  2.000000030e-01,  1.440000000e-01,  3.600000000e-02, /*DIAM*/ -7.676003453e-02, -7.067518265e-02, -8.264708801e-02, /*EDIAM*/  5.542796088e-02,  3.481844113e-02,  8.405710054e-03, /*DMEAN*/ -8.189833319e-02, /*EDMEAN*/  1.378093584e-02, /*CHI2*/  4.326660632e-01 },
            {310, /* SP "K2III          " idx */ 208, /*MAG*/  7.053000000e+00,  5.930000000e+00,  4.840000000e+00,  4.303000000e+00,  3.726000000e+00,  3.548000000e+00, /*EMAG*/  7.211103000e-03,  4.000000000e-03,  1.077033000e-02,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/ -4.329859344e-02, -2.081291596e-02, -7.903894653e-03, /*EDIAM*/  5.543413396e-02,  4.831107767e-02,  4.530636521e-02, /*DMEAN*/ -2.163970767e-02, /*EDMEAN*/  2.871277441e-02, /*CHI2*/  7.731437363e-01 },
            {311, /* SP "K1III          " idx */ 204, /*MAG*/  5.769000000e+00,  4.590000000e+00,  3.410000000e+00,  2.619000000e+00,  2.021000000e+00,  1.734000000e+00, /*EMAG*/  2.000000000e-03,  2.000000000e-03,  3.006659000e-02,  2.000000030e-01,  1.860000000e-01,  2.000000030e-01, /*DIAM*/  3.111685427e-01,  3.289279361e-01,  3.616083931e-01, /*EDIAM*/  5.542732264e-02,  4.493266675e-02,  4.530285402e-02, /*DMEAN*/  3.366839227e-01, /*EDMEAN*/  2.791960731e-02, /*CHI2*/  2.641885933e-01 },
            {312, /* SP "K3III          " idx */ 212, /*MAG*/  6.546000000e+00,  5.270000000e+00,  4.030000000e+00,  3.392000000e+00,  2.884000000e+00,  2.632000000e+00, /*EMAG*/  2.236068000e-03,  2.000000000e-03,  2.000000000e-03,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/  1.665687344e-01,  1.616917621e-01,  1.878392207e-01, /*EDIAM*/  5.544190970e-02,  4.831858562e-02,  4.531120349e-02, /*DMEAN*/  1.732361561e-01, /*EDMEAN*/  2.846124131e-02, /*CHI2*/  4.313021106e-01 },
            {313, /* SP "K1III          " idx */ 204, /*MAG*/  6.931000000e+00,  5.830000000e+00,  4.770000000e+00,  3.835000000e+00,  3.344000000e+00,  3.103000000e+00, /*EMAG*/  3.605551000e-03,  2.000000000e-03,  2.000000000e-03,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/  6.981139622e-02,  6.090458585e-02,  8.441860791e-02, /*EDIAM*/  5.542732264e-02,  4.830582952e-02,  4.530285402e-02, /*DMEAN*/  7.247082804e-02, /*EDMEAN*/  2.862902652e-02, /*CHI2*/  4.016224075e-01 },
            {314, /* SP "K2III          " idx */ 208, /*MAG*/  6.899000000e+00,  5.720000000e+00,  4.580000000e+00,  3.633000000e+00,  3.005000000e+00,  2.780000000e+00, /*EMAG*/  5.830952000e-03,  3.000000000e-03,  1.044031000e-02,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/  1.260228377e-01,  1.444633511e-01,  1.603588816e-01, /*EDIAM*/  5.543376170e-02,  4.831095443e-02,  4.530631186e-02, /*DMEAN*/  1.458694259e-01, /*EDMEAN*/  2.944773247e-02, /*CHI2*/  5.283968146e-01 },
            {315, /* SP "K0III          " idx */ 200, /*MAG*/  7.030000000e+00,  6.000000000e+00,  5.000000000e+00,  4.396000000e+00,  3.821000000e+00,  3.659000000e+00, /*EMAG*/  7.211103000e-03,  4.000000000e-03,  1.077033000e-02,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/ -8.131458177e-02, -5.310062010e-02, -4.253868468e-02, /*EDIAM*/  5.542347221e-02,  4.830297083e-02,  4.530047979e-02, /*DMEAN*/ -5.635117693e-02, /*EDMEAN*/  2.989642819e-02, /*CHI2*/  9.330257760e-01 },
            {316, /* SP "K2III          " idx */ 208, /*MAG*/  6.719000000e+00,  5.500000000e+00,  4.320000000e+00,  3.388000000e+00,  2.795000000e+00,  2.527000000e+00, /*EMAG*/  5.830952000e-03,  3.000000000e-03,  1.044031000e-02,  2.000000030e-01,  1.840000000e-01,  2.000000030e-01, /*DIAM*/  1.769424813e-01,  1.860509003e-01,  2.118260356e-01, /*EDIAM*/  5.543376170e-02,  4.445640870e-02,  4.530631186e-02, /*DMEAN*/  1.933271538e-01, /*EDMEAN*/  2.844905492e-02, /*CHI2*/  3.307522777e+00 },
            {317, /* SP "K4III          " idx */ 216, /*MAG*/  6.389000000e+00,  5.020000000e+00,  3.680000000e+00,  2.876000000e+00,  2.091000000e+00,  1.939000000e+00, /*EMAG*/  3.605551000e-03,  3.000000000e-03,  3.000000000e-03,  2.000000030e-01,  1.940000000e-01,  2.000000030e-01, /*DIAM*/  2.980218510e-01,  3.494657497e-01,  3.439140289e-01, /*EDIAM*/  5.545333446e-02,  4.688470205e-02,  4.531819496e-02, /*DMEAN*/  3.341320476e-01, /*EDMEAN*/  2.835942251e-02, /*CHI2*/  8.061659260e-01 },
            {318, /* SP "K4III          " idx */ 216, /*MAG*/  7.035000000e+00,  5.720000000e+00,  4.440000000e+00,  3.689000000e+00,  3.002000000e+00,  2.782000000e+00, /*EMAG*/  5.830952000e-03,  3.000000000e-03,  1.044031000e-02,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/  1.267450628e-01,  1.585630231e-01,  1.715563621e-01, /*EDIAM*/  5.545333446e-02,  4.832968341e-02,  4.531819496e-02, /*DMEAN*/  1.553241298e-01, /*EDMEAN*/  2.943499204e-02, /*CHI2*/  2.318022014e+00 },
            {319, /* SP "K4III:         " idx */ 216, /*MAG*/  7.154000000e+00,  5.970000000e+00,  4.830000000e+00,  4.180000000e+00,  3.593000000e+00,  3.406000000e+00, /*EMAG*/  7.211103000e-03,  4.000000000e-03,  1.077033000e-02,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/  1.003970389e-02,  2.629842970e-02,  3.692862284e-02, /*EDIAM*/  5.545370659e-02,  4.832980660e-02,  4.531824829e-02, /*DMEAN*/  2.621041311e-02, /*EDMEAN*/  2.882956968e-02, /*CHI2*/  6.292942912e-02 },
            {320, /* SP "K5III          " idx */ 220, /*MAG*/  7.153000000e+00,  5.690000000e+00,  4.220000000e+00,  3.093000000e+00,  2.340000000e+00,  2.041000000e+00, /*EMAG*/  7.211103000e-03,  4.000000000e-03,  1.077033000e-02,  1.860000000e-01,  1.960000000e-01,  2.000000030e-01, /*DIAM*/  2.964414370e-01,  3.238642060e-01,  3.442395807e-01, /*EDIAM*/  5.160096540e-02,  4.738114152e-02,  4.532739880e-02, /*DMEAN*/  3.235747193e-01, /*EDMEAN*/  2.778015621e-02, /*CHI2*/  4.434207389e+00 },
            {321, /* SP "K1III          " idx */ 204, /*MAG*/  6.492000000e+00,  5.350000000e+00,  4.240000000e+00,  3.777000000e+00,  3.160000000e+00,  2.876000000e+00, /*EMAG*/  5.830952000e-03,  3.000000000e-03,  1.044031000e-02,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/  4.900782448e-02,  8.549602590e-02,  1.231704333e-01, /*EDIAM*/  5.542758857e-02,  4.830591756e-02,  4.530289212e-02, /*DMEAN*/  9.072013772e-02, /*EDMEAN*/  2.855223194e-02, /*CHI2*/  3.797259227e+00 },
            {322, /* SP "K1III          " idx */ 204, /*MAG*/  6.913000000e+00,  5.970000000e+00,  5.040000000e+00,  3.933000000e+00,  3.557000000e+00,  3.550000000e+00, /*EMAG*/  4.472136000e-03,  4.000000000e-03,  4.000000000e-03,  2.000000030e-01,  1.980000000e-01,  2.000000030e-01, /*DIAM*/  5.343639598e-02,  1.529369023e-02, -1.304854683e-02, /*EDIAM*/  5.542796088e-02,  4.782412503e-02,  4.530294547e-02, /*DMEAN*/  1.417460974e-02, /*EDMEAN*/  2.941846600e-02, /*CHI2*/  1.206506867e+00 },
            {323, /* SP "K1.5III        " idx */ 206, /*MAG*/  5.999000000e+00,  4.820000000e+00,  3.660000000e+00,  2.949000000e+00,  2.197000000e+00,  2.085000000e+00, /*EMAG*/  1.315295000e-02,  2.000000000e-03,  3.006659000e-02,  2.000000030e-01,  1.960000000e-01,  2.000000030e-01, /*DIAM*/  2.418949529e-01,  2.990706175e-01,  2.910878048e-01, /*EDIAM*/  5.543016616e-02,  4.734428071e-02,  4.530441161e-02, /*DMEAN*/  2.812009492e-01, /*EDMEAN*/  2.951125788e-02, /*CHI2*/  6.134985704e-01 },
            {324, /* SP "K5III:         " idx */ 220, /*MAG*/  7.483000000e+00,  6.240000000e+00,  5.040000000e+00,  4.453000000e+00,  3.704000000e+00,  3.574000000e+00, /*EMAG*/  8.944272000e-03,  4.000000000e-03,  1.077033000e-02,  1.980000000e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/ -3.775499650e-02,  1.749066061e-02,  1.180891878e-02, /*EDIAM*/  5.491520764e-02,  4.834418472e-02,  4.532739880e-02, /*DMEAN*/  5.750878131e-04, /*EDMEAN*/  2.993160949e-02, /*CHI2*/  2.252751193e-01 },
            {325, /* SP "K1III          " idx */ 204, /*MAG*/  6.825000000e+00,  5.670000000e+00,  4.550000000e+00,  3.795000000e+00,  3.171000000e+00,  2.997000000e+00, /*EMAG*/  5.830952000e-03,  3.000000000e-03,  1.044031000e-02,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/  6.859711049e-02,  9.604077314e-02,  1.041996301e-01, /*EDIAM*/  5.542758857e-02,  4.830591756e-02,  4.530289212e-02, /*DMEAN*/  9.204885324e-02, /*EDMEAN*/  2.963987920e-02, /*CHI2*/  6.573002950e-01 },
            {326, /* SP "K2III          " idx */ 208, /*MAG*/  7.167000000e+00,  6.280000000e+00,  5.390000000e+00,  4.889000000e+00,  4.322000000e+00,  4.317000000e+00, /*EMAG*/  6.403124000e-03,  4.000000000e-03,  4.000000000e-03,  2.000000030e-01,  2.400000000e-02,  2.000000030e-01, /*DIAM*/ -1.786200240e-01, -1.501592214e-01, -1.727141161e-01, /*EDIAM*/  5.543413396e-02,  6.286042212e-03,  4.530636521e-02, /*DMEAN*/ -1.508940575e-01, /*EDMEAN*/  1.336610265e-02, /*CHI2*/  6.597250702e-01 },
            {327, /* SP "K5III          " idx */ 220, /*MAG*/  7.286000000e+00,  5.810000000e+00,  4.320000000e+00,  3.076000000e+00,  2.361000000e+00,  2.128000000e+00, /*EMAG*/  8.062258000e-03,  4.000000000e-03,  1.077033000e-02,  1.980000000e-01,  1.660000000e-01,  2.000000030e-01, /*DIAM*/  3.103610801e-01,  3.237474745e-01,  3.277067337e-01, /*EDIAM*/  5.491520764e-02,  4.016128861e-02,  4.532739880e-02, /*DMEAN*/  3.220025593e-01, /*EDMEAN*/  2.640136448e-02, /*CHI2*/  1.210788534e+00 },
            {328, /* SP "K0III          " idx */ 200, /*MAG*/  7.218000000e+00,  6.200000000e+00,  5.210000000e+00,  4.676000000e+00,  4.064000000e+00,  3.899000000e+00, /*EMAG*/  5.830952000e-03,  3.000000000e-03,  3.000000000e-03,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/ -1.434574398e-01, -1.034741617e-01, -9.158978030e-02, /*EDIAM*/  5.542309987e-02,  4.830284756e-02,  4.530042645e-02, /*DMEAN*/ -1.092913229e-01, /*EDMEAN*/  3.474155645e-02, /*CHI2*/  8.226922475e-01 },
            {329, /* SP "K4III          " idx */ 216, /*MAG*/  6.951000000e+00,  5.540000000e+00,  4.150000000e+00,  3.145000000e+00,  2.453000000e+00,  2.147000000e+00, /*EMAG*/  6.708204000e-03,  3.000000000e-03,  1.044031000e-02,  2.000000030e-01,  1.840000000e-01,  2.000000030e-01, /*DIAM*/  2.634950648e-01,  2.835824802e-01,  3.105125685e-01, /*EDIAM*/  5.545333446e-02,  4.447676085e-02,  4.531819496e-02, /*DMEAN*/  2.885787865e-01, /*EDMEAN*/  2.903909101e-02, /*CHI2*/  3.676264707e-01 },
            {330, /* SP "K2.5III        " idx */ 210, /*MAG*/  5.818000000e+00,  4.500000000e+00,  3.250000000e+00,  2.435000000e+00,  1.817000000e+00,  1.668000000e+00, /*EMAG*/  3.162278000e-03,  1.000000047e-03,  3.001666000e-02,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/  3.681877095e-01,  3.840130890e-01,  3.828233596e-01, /*EDIAM*/  5.543722654e-02,  4.831429315e-02,  4.530849098e-02, /*DMEAN*/  3.793971975e-01, /*EDMEAN*/  2.857771497e-02, /*CHI2*/  2.143652527e+00 },
            {331, /* SP "G8.5III        " idx */ 194, /*MAG*/  5.611000000e+00,  4.680000000e+00,  3.730000000e+00,  3.162000000e+00,  2.695000000e+00,  2.590000000e+00, /*EMAG*/  3.605551000e-02,  2.000000000e-03,  2.009975000e-02,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/  1.456249314e-01,  1.560232599e-01,  1.566053354e-01, /*EDIAM*/  5.541848730e-02,  4.830068397e-02,  4.529762273e-02, /*DMEAN*/  1.535255074e-01, /*EDMEAN*/  3.235446510e-02, /*CHI2*/  1.072172795e+00 },
            {332, /* SP "K2.5III        " idx */ 210, /*MAG*/  5.781000000e+00,  4.590000000e+00,  3.470000000e+00,  2.639000000e+00,  2.127000000e+00,  1.967000000e+00, /*EMAG*/  2.828427000e-03,  2.000000000e-03,  2.009975000e-02,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/  3.186341374e-01,  3.129391580e-01,  3.175313878e-01, /*EDIAM*/  5.543738607e-02,  4.831434596e-02,  4.530851384e-02, /*DMEAN*/  3.162350719e-01, /*EDMEAN*/  3.065924080e-02, /*CHI2*/  8.936272739e-02 },
            {333, /* SP "G8II-III       " idx */ 192, /*MAG*/  4.245000000e+00,  3.330000000e+00,  2.420000000e+00,  1.879000000e+00,  1.557000000e+00,  1.439000000e+00, /*EMAG*/  6.324555000e-03,  2.000000000e-03,  2.009975000e-02,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/  3.927666825e-01,  3.724362091e-01,  3.790111702e-01, /*EDIAM*/  5.541761655e-02,  4.830052997e-02,  4.529685780e-02, /*DMEAN*/  3.803477772e-01, /*EDMEAN*/  3.108543193e-02, /*CHI2*/  6.595467502e-01 },
            {334, /* SP "K2III          " idx */ 208, /*MAG*/  6.674000000e+00,  5.450000000e+00,  4.290000000e+00,  3.566000000e+00,  2.953000000e+00,  2.804000000e+00, /*EMAG*/  2.000000000e-03,  2.000000000e-03,  4.004997000e-02,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/  1.238353376e-01,  1.458719114e-01,  1.478333339e-01, /*EDIAM*/  5.543349579e-02,  4.831086640e-02,  4.530627376e-02, /*DMEAN*/  1.408647320e-01, /*EDMEAN*/  4.171847214e-02, /*CHI2*/  4.485287776e-02 },
            {335, /* SP "F9V            " idx */ 156, /*MAG*/  5.340000000e+00,  4.800000000e+00,  4.190000000e+00,  3.936000000e+00,  3.681000000e+00,  3.638000000e+00, /*EMAG*/  8.246211000e-03,  2.000000000e-03,  3.006659000e-02,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/ -1.175248352e-01, -1.016827715e-01, -1.110934342e-01, /*EDIAM*/  5.544425325e-02,  4.833708903e-02,  4.530531790e-02, /*DMEAN*/ -1.095324651e-01, /*EDMEAN*/  2.887412603e-02, /*CHI2*/  8.182757834e-01 },
            {336, /* SP "K0V            " idx */ 200, /*MAG*/  6.434000000e+00,  5.630000000e+00,  4.820000000e+00,  4.596000000e+00,  4.133000000e+00,  4.014000000e+00, /*EMAG*/  1.019804000e-02,  2.000000000e-03,  2.000000000e-03,  2.000000030e-01,  2.000000000e-01,  2.000000030e-01, /*DIAM*/ -1.650824402e-01, -1.436298043e-01, -1.325897809e-01, /*EDIAM*/  5.542283391e-02,  4.830275880e-02,  4.530038834e-02, /*DMEAN*/ -1.449202048e-01, /*EDMEAN*/  3.020980260e-02, /*CHI2*/  3.189541723e-01 },
            {337, /* SP "F8V            " idx */ 152, /*MAG*/  4.614000000e+00,  4.100000000e+00,  3.510000000e+00,  3.031000000e+00,  2.863000000e+00,  2.697000000e+00, /*EMAG*/  2.828427000e-03,  2.000000000e-03,  2.000000000e-03,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/  7.649555086e-02,  6.617100226e-02,  8.172887017e-02, /*EDIAM*/  5.544702400e-02,  4.834188598e-02,  4.530866149e-02, /*DMEAN*/  7.498989754e-02, /*EDMEAN*/  2.861066111e-02, /*CHI2*/  4.905943086e-01 },
            {338, /* SP "F9.5V          " idx */ 158, /*MAG*/  4.645000000e+00,  4.050000000e+00,  3.400000000e+00,  3.143000000e+00,  2.875000000e+00,  2.723000000e+00, /*EMAG*/  7.615773000e-03,  3.000000000e-03,  1.044031000e-02,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/  4.595680155e-02,  6.220613612e-02,  7.720153426e-02, /*EDIAM*/  5.544282805e-02,  4.833455543e-02,  4.530383230e-02, /*DMEAN*/  6.383852476e-02, /*EDMEAN*/  2.853400586e-02, /*CHI2*/  4.272861459e-01 },
            {339, /* SP "G5Vv           " idx */ 180, /*MAG*/  5.521000000e+00,  4.840000000e+00,  4.110000000e+00,  3.407000000e+00,  3.039000000e+00,  2.957000000e+00, /*EMAG*/  6.708204000e-03,  3.000000000e-03,  2.022375000e-02,  1.920000000e-01,  1.820000000e-01,  2.000000030e-01, /*DIAM*/  6.212864711e-02,  6.535452356e-02,  6.146220999e-02, /*EDIAM*/  5.320773721e-02,  4.396854227e-02,  4.529440697e-02, /*DMEAN*/  6.311836452e-02, /*EDMEAN*/  2.937785069e-02, /*CHI2*/  3.370544128e+00 },
            {340, /* SP "F8V            " idx */ 152, /*MAG*/  4.865000000e+00,  4.290000000e+00,  3.630000000e+00,  3.194000000e+00,  2.916000000e+00,  2.835000000e+00, /*EMAG*/  8.246211000e-03,  2.000000000e-03,  2.009975000e-02,  2.000000030e-01,  2.000000000e-01,  2.000000030e-01, /*DIAM*/  4.596876469e-02,  6.122158584e-02,  5.549529313e-02, /*EDIAM*/  5.544702400e-02,  4.834188526e-02,  4.530866149e-02, /*DMEAN*/  5.497373677e-02, /*EDMEAN*/  2.899041442e-02, /*CHI2*/  2.198327993e-01 },
            {341, /* SP "G1V            " idx */ 164, /*MAG*/  5.320000000e+00,  4.690000000e+00,  3.990000000e+00,  3.397000000e+00,  3.154000000e+00,  3.038000000e+00, /*EMAG*/  4.242641000e-03,  3.000000000e-03,  3.000000000e-03,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/  3.050615106e-02,  2.284539475e-02,  2.610457692e-02, /*EDIAM*/  5.543651681e-02,  4.832593589e-02,  4.529975526e-02, /*DMEAN*/  2.613369468e-02, /*EDMEAN*/  2.919456826e-02, /*CHI2*/  5.228576553e-01 },
            {342, /* SP "A3V            " idx */ 92, /*MAG*/  3.686000000e+00,  3.580000000e+00,  3.460000000e+00,  3.540000000e+00,  3.495000000e+00,  3.535000000e+00, /*EMAG*/  6.324555000e-03,  2.000000000e-03,  2.009975000e-02,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/ -1.596406384e-01, -1.593227862e-01, -1.722604529e-01, /*EDIAM*/  5.584581421e-02,  4.866130746e-02,  4.555031280e-02, /*DMEAN*/ -1.645008872e-01, /*EDMEAN*/  2.948899846e-02, /*CHI2*/  2.863432483e+00 },
            {343, /* SP "F1V            " idx */ 124, /*MAG*/  4.480000000e+00,  4.160000000e+00,  3.760000000e+00,  3.221000000e+00,  3.156000000e+00,  2.978000000e+00, /*EMAG*/  1.236932000e-02,  3.000000000e-03,  3.014963000e-02,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/  1.736776740e-02, -8.583632274e-03,  9.642089941e-03, /*EDIAM*/  5.551786795e-02,  4.840968767e-02,  4.537614109e-02, /*DMEAN*/  5.379434883e-03, /*EDMEAN*/  2.927856776e-02, /*CHI2*/  2.245740104e+00 },
            {344, /* SP "F0IV           " idx */ 120, /*MAG*/  4.010000000e+00,  3.650000000e+00,  3.240000000e+00,  3.222000000e+00,  3.025000000e+00,  2.864000000e+00, /*EMAG*/  1.513275000e-02,  2.000000000e-03,  3.006659000e-02,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/ -2.422243666e-02, -5.288930589e-04,  1.971765679e-02, /*EDIAM*/  5.554995278e-02,  4.843530315e-02,  4.539731655e-02, /*DMEAN*/  1.217504376e-03, /*EDMEAN*/  2.875033905e-02, /*CHI2*/  1.247105982e+00 },
            {345, /* SP "G8+V           " idx */ 192, /*MAG*/  6.170000000e+00,  5.400000000e+00,  4.620000000e+00,  4.059000000e+00,  3.718000000e+00,  3.690000000e+00, /*EMAG*/  3.605551000e-03,  3.000000000e-03,  1.044031000e-02,  2.000000030e-01,  2.000000030e-01,  1.800000000e-02, /*DIAM*/ -5.167975266e-02, -6.351710477e-02, -7.594504093e-02, /*EDIAM*/  5.541788253e-02,  4.830061802e-02,  4.509996092e-03, /*DMEAN*/ -7.571187221e-02, /*EDMEAN*/  7.792217645e-03, /*CHI2*/  8.326946347e-01 },
            {346, /* SP "G3Va           " idx */ 172, /*MAG*/  6.046000000e+00,  5.370000000e+00,  4.630000000e+00,  4.267000000e+00,  4.040000000e+00,  3.821000000e+00, /*EMAG*/  5.656854000e-03,  4.000000000e-03,  4.000000000e-03,  2.000000030e-01,  2.000000030e-01,  3.600000000e-02, /*DIAM*/ -1.480411695e-01, -1.594964554e-01, -1.274390673e-01, /*EDIAM*/  5.542721846e-02,  4.831439669e-02,  8.367497549e-03, /*DMEAN*/ -1.287508137e-01, /*EDMEAN*/  1.061881119e-02, /*CHI2*/  7.902060013e-01 },
            {347, /* SP "F8V            " idx */ 152, /*MAG*/  5.361000000e+00,  4.820000000e+00,  4.240000000e+00,  4.033000000e+00,  3.758000000e+00,  3.644000000e+00, /*EMAG*/  8.246211000e-03,  2.000000000e-03,  1.019804000e-02,  2.000000030e-01,  1.980000000e-01,  2.000000030e-01, /*DIAM*/ -1.455580239e-01, -1.200468994e-01, -1.136360962e-01, /*EDIAM*/  5.544702400e-02,  4.786033113e-02,  4.530866149e-02, /*DMEAN*/ -1.241859830e-01, /*EDMEAN*/  2.935581134e-02, /*CHI2*/  2.837331040e-01 },
            {348, /* SP "A1IVps         " idx */ 84, /*MAG*/  2.373000000e+00,  2.340000000e+00,  2.320000000e+00,  2.269000000e+00,  2.359000000e+00,  2.285000000e+00, /*EMAG*/  3.605551000e-03,  2.000000000e-03,  3.006659000e-02,  2.000000030e-01,  1.640000000e-01,  2.000000030e-01, /*DIAM*/  7.205635008e-02,  4.136643113e-02,  5.691098086e-02, /*EDIAM*/  5.586655415e-02,  4.008566635e-02,  4.556168517e-02, /*DMEAN*/  5.351050755e-02, /*EDMEAN*/  2.715359467e-02, /*CHI2*/  9.053121189e-02 },
            {349, /* SP "A5IV(n)        " idx */ 100, /*MAG*/  2.688000000e+00,  2.560000000e+00,  2.440000000e+00,  2.238000000e+00,  2.191000000e+00,  2.144000000e+00, /*EMAG*/  1.414214000e-02,  2.000000000e-03,  3.006659000e-02,  2.000000030e-01,  1.880000000e-01,  2.000000030e-01, /*DIAM*/  1.407927441e-01,  1.305944344e-01,  1.316857169e-01, /*EDIAM*/  5.577787863e-02,  4.573843951e-02,  4.551982045e-02, /*DMEAN*/  1.335589015e-01, /*EDMEAN*/  2.818334429e-02, /*CHI2*/  5.265155137e-02 },
            {350, /* SP "G8V            " idx */ 192, /*MAG*/  6.033000000e+00,  5.310000000e+00,  4.530000000e+00,  3.988000000e+00,  3.648000000e+00,  3.588000000e+00, /*EMAG*/  1.431782000e-02,  3.000000000e-03,  4.011234000e-02,  2.000000030e-01,  2.000000030e-01,  3.600000000e-02, /*DIAM*/ -3.893868104e-02, -5.034200730e-02, -5.522971215e-02, /*EDIAM*/  5.541788253e-02,  4.830061802e-02,  8.373044384e-03, /*DMEAN*/ -5.475325125e-02, /*EDMEAN*/  9.243116512e-03, /*CHI2*/  5.830354755e-01 },
            {351, /* SP "F9V            " idx */ 156, /*MAG*/  4.108000000e+00,  3.590000000e+00,  2.980000000e+00,  2.597000000e+00,  2.363000000e+00,  2.269000000e+00, /*EMAG*/  1.552417000e-02,  4.000000000e-03,  2.039608000e-02,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/  1.601805261e-01,  1.663717072e-01,  1.668846721e-01, /*EDIAM*/  5.544489130e-02,  4.833730019e-02,  4.530540935e-02, /*DMEAN*/  1.649505207e-01, /*EDMEAN*/  2.848933549e-02, /*CHI2*/  3.897094731e-02 },
            {352, /* SP "G0V            " idx */ 160, /*MAG*/  4.828000000e+00,  4.240000000e+00,  3.570000000e+00,  3.213000000e+00,  2.905000000e+00,  2.848000000e+00, /*EMAG*/  9.219544000e-03,  2.000000000e-03,  1.019804000e-02,  2.000000030e-01,  1.980000000e-01,  2.000000030e-01, /*DIAM*/  4.290841087e-02,  6.324682084e-02,  5.494627560e-02, /*EDIAM*/  5.544065063e-02,  4.785004409e-02,  4.530235223e-02, /*DMEAN*/  5.471459727e-02, /*EDMEAN*/  3.023615753e-02, /*CHI2*/  4.994110404e-01 },
            {353, /* SP "G0V            " idx */ 160, /*MAG*/  4.802000000e+00,  4.230000000e+00,  3.560000000e+00,  3.232000000e+00,  2.992000000e+00,  2.923000000e+00, /*EMAG*/  6.708204000e-03,  3.000000000e-03,  4.011234000e-02,  2.000000030e-01,  1.920000000e-01,  2.000000030e-01, /*DIAM*/  3.688162507e-02,  4.184604231e-02,  3.771269870e-02, /*EDIAM*/  5.544091650e-02,  4.640526978e-02,  4.530239034e-02, /*DMEAN*/  3.900420125e-02, /*EDMEAN*/  2.834274506e-02, /*CHI2*/  4.969308099e-02 },
            {354, /* SP "F7V            " idx */ 148, /*MAG*/  4.537000000e+00,  4.040000000e+00,  3.450000000e+00,  3.179000000e+00,  2.980000000e+00,  2.739000000e+00, /*EMAG*/  5.385165000e-03,  2.000000000e-03,  1.200167000e-01,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/  2.872075347e-02,  3.497698068e-02,  6.916673241e-02, /*EDIAM*/  5.544932552e-02,  4.834626749e-02,  4.531259243e-02, /*DMEAN*/  4.677013838e-02, /*EDMEAN*/  2.857331068e-02, /*CHI2*/  1.405871959e-01 },
            {355, /* SP "F2V            " idx */ 128, /*MAG*/  4.834000000e+00,  4.470000000e+00,  4.060000000e+00,  3.561000000e+00,  3.462000000e+00,  3.336000000e+00, /*EMAG*/  5.385165000e-03,  2.000000000e-03,  3.006659000e-02,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/ -5.124312805e-02, -6.771396445e-02, -6.133811863e-02, /*EDIAM*/  5.549275423e-02,  4.838939247e-02,  4.535818821e-02, /*DMEAN*/ -6.089133220e-02, /*EDMEAN*/  2.928064726e-02, /*CHI2*/  1.092894472e-01 },
            {356, /* SP "kA2hA5mA7V     " idx */ 88, /*MAG*/  3.857000000e+00,  3.710000000e+00,  3.580000000e+00,  3.564000000e+00,  3.440000000e+00,  3.425000000e+00, /*EMAG*/  9.219544000e-03,  2.000000000e-03,  2.009975000e-02,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/ -1.678949729e-01, -1.511903014e-01, -1.538337596e-01, /*EDIAM*/  5.586261194e-02,  4.867392350e-02,  4.555800268e-02, /*DMEAN*/ -1.565985247e-01, /*EDMEAN*/  3.027547495e-02, /*CHI2*/  6.593208206e-01 },
            {357, /* SP "F6IV           " idx */ 144, /*MAG*/  4.328000000e+00,  3.850000000e+00,  3.310000000e+00,  3.149000000e+00,  2.875000000e+00,  2.703000000e+00, /*EMAG*/  1.906544000e-02,  1.809671000e-02,  2.697204000e-02,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/  2.065104848e-02,  5.200246415e-02,  7.099885702e-02, /*EDIAM*/  5.546914973e-02,  4.835644211e-02,  4.531995960e-02, /*DMEAN*/  5.125464626e-02, /*EDMEAN*/  2.851477561e-02, /*CHI2*/  5.993448112e-01 },
            {358, /* SP "G2Va           " idx */ 168, /*MAG*/  6.142000000e+00,  5.490000000e+00,  4.800000000e+00,  4.667000000e+00,  4.162000000e+00,  4.186000000e+00, /*EMAG*/  9.848858000e-03,  4.000000000e-03,  2.039608000e-02,  2.000000030e-01,  1.780000000e-01,  2.000000030e-01, /*DIAM*/ -2.549168913e-01, -1.858663705e-01, -2.099505620e-01, /*EDIAM*/  5.543202953e-02,  4.302144797e-02,  4.529757523e-02, /*DMEAN*/ -2.111412181e-01, /*EDMEAN*/  2.882435748e-02, /*CHI2*/  4.533727347e+00 },
            {359, /* SP "F5IV-V         " idx */ 140, /*MAG*/  5.004000000e+00,  4.570000000e+00,  4.070000000e+00,  3.803000000e+00,  3.648000000e+00,  3.502000000e+00, /*EMAG*/  5.000000000e-03,  3.000000000e-03,  3.000000000e-03,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/ -1.065608935e-01, -1.053272070e-01, -9.211628146e-02, /*EDIAM*/  5.545637112e-02,  4.835627081e-02,  4.532394209e-02, /*DMEAN*/ -1.004603996e-01, /*EDMEAN*/  3.066926524e-02, /*CHI2*/  2.159716194e+00 },
            {360, /* SP "F2V            " idx */ 128, /*MAG*/  5.010000000e+00,  4.620000000e+00,  4.170000000e+00,  3.928000000e+00,  3.665000000e+00,  3.639000000e+00, /*EMAG*/  2.716616000e-02,  3.000000000e-03,  2.022375000e-02,  2.000000030e-01,  1.920000000e-01,  2.000000030e-01, /*DIAM*/ -1.413056294e-01, -1.104999573e-01, -1.259585575e-01, /*EDIAM*/  5.549301985e-02,  4.646535272e-02,  4.535822627e-02, /*DMEAN*/ -1.242465080e-01, /*EDMEAN*/  3.191030075e-02, /*CHI2*/  1.199788544e-01 },
            {361, /* SP "F6V            " idx */ 144, /*MAG*/  4.673000000e+00,  4.190000000e+00,  3.640000000e+00,  3.527000000e+00,  3.286000000e+00,  3.190000000e+00, /*EMAG*/  2.236068000e-03,  1.000000047e-03,  1.000000000e-03,  2.000000030e-01,  1.760000000e-01,  2.000000030e-01, /*DIAM*/ -5.786680983e-02, -3.312594179e-02, -3.026391822e-02, /*EDIAM*/  5.545179498e-02,  4.257464386e-02,  4.531747230e-02, /*DMEAN*/ -3.799848632e-02, /*EDMEAN*/  2.725342533e-02, /*CHI2*/  6.706608166e-01 },
            {362, /* SP "A0IV-Vnn       " idx */ 80, /*MAG*/  3.004000000e+00,  2.990000000e+00,  3.000000000e+00,  3.084000000e+00,  3.048000000e+00,  2.876000000e+00, /*EMAG*/  4.242641000e-03,  3.000000000e-03,  6.007495000e-02,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/ -1.186280117e-01, -1.109885844e-01, -7.231279165e-02, /*EDIAM*/  5.585913211e-02,  4.867384227e-02,  4.556355266e-02, /*DMEAN*/ -9.775411476e-02, /*EDMEAN*/  2.974436913e-02, /*CHI2*/  1.101983727e+00 },
            {363, /* SP "G9V            " idx */ 196, /*MAG*/  5.456000000e+00,  4.670000000e+00,  3.820000000e+00,  3.423000000e+00,  3.039000000e+00,  2.900000000e+00, /*EMAG*/  9.899495000e-03,  7.000000000e-03,  2.118962000e-02,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/  7.699470926e-02,  7.519315932e-02,  8.882652839e-02, /*EDIAM*/  5.542204020e-02,  4.830188728e-02,  4.529880244e-02, /*DMEAN*/  8.101787761e-02, /*EDMEAN*/  2.866769800e-02, /*CHI2*/  9.834807072e-02 },
            {364, /* SP "F3+V           " idx */ 132, /*MAG*/  4.885000000e+00,  4.490000000e+00,  4.050000000e+00,  3.878000000e+00,  3.716000000e+00,  3.537000000e+00, /*EMAG*/  1.513275000e-02,  2.000000000e-03,  3.006659000e-02,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/ -1.360457632e-01, -1.267627583e-01, -1.047344236e-01, /*EDIAM*/  5.547499106e-02,  4.837435011e-02,  4.534370799e-02, /*DMEAN*/ -1.205425837e-01, /*EDMEAN*/  2.946842506e-02, /*CHI2*/  1.245560797e+00 },
            {365, /* SP "A1Va           " idx */ 84, /*MAG*/  3.606000000e+00,  3.520000000e+00,  3.430000000e+00,  3.455000000e+00,  3.390000000e+00,  3.377000000e+00, /*EMAG*/  2.828427000e-03,  2.000000000e-03,  2.009975000e-02,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/ -1.656043677e-01, -1.586880466e-01, -1.591766136e-01, /*EDIAM*/  5.586655415e-02,  4.867754330e-02,  4.556168517e-02, /*DMEAN*/ -1.606885438e-01, /*EDMEAN*/  2.996101829e-02, /*CHI2*/  3.439720822e+00 },
            {366, /* SP "A1V            " idx */ 84, /*MAG*/  3.791000000e+00,  3.760000000e+00,  3.710000000e+00,  3.830000000e+00,  3.867000000e+00,  3.851000000e+00, /*EMAG*/  3.605551000e-03,  2.000000000e-03,  2.000000000e-03,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/ -2.509704404e-01, -2.638631454e-01, -2.601255202e-01, /*EDIAM*/  5.586655415e-02,  4.867754330e-02,  4.556168517e-02, /*DMEAN*/ -2.590205740e-01, /*EDMEAN*/  3.211286891e-02, /*CHI2*/  1.217558913e+00 },
            {367, /* SP "F7V            " idx */ 148, /*MAG*/  4.702000000e+00,  4.200000000e+00,  3.600000000e+00,  3.358000000e+00,  3.078000000e+00,  2.961000000e+00, /*EMAG*/  7.615773000e-03,  3.000000000e-03,  3.014963000e-02,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/ -8.538175655e-03,  1.793417887e-02,  2.313753464e-02, /*EDIAM*/  5.544959135e-02,  4.834635546e-02,  4.531263052e-02, /*DMEAN*/  1.303954515e-02, /*EDMEAN*/  2.861979759e-02, /*CHI2*/  2.937675197e-01 },
            {368, /* SP "F7V            " idx */ 148, /*MAG*/  4.637000000e+00,  4.130000000e+00,  3.540000000e+00,  3.299000000e+00,  2.988000000e+00,  2.946000000e+00, /*EMAG*/  6.708204000e-03,  3.000000000e-03,  2.022375000e-02,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/  2.417181651e-03,  3.675908187e-02,  2.469227919e-02, /*EDIAM*/  5.544959135e-02,  4.834635546e-02,  4.531263052e-02, /*DMEAN*/  2.301621224e-02, /*EDMEAN*/  2.867096244e-02, /*CHI2*/  1.122031471e-01 },
            {369, /* SP "M2.0V          " idx */ 248, /*MAG*/  9.650000000e+00,  8.090000000e+00,  5.610000000e+00,  5.252000000e+00,  4.476000000e+00,  4.018000000e+00, /*EMAG*/  4.472136000e-02,  2.000000000e-02,  1.711724000e-01,  2.000000030e-01,  2.000000000e-01,  2.000000000e-02, /*DIAM*/ -1.105084729e-01, -5.750887833e-02, -2.761883530e-03, /*EDIAM*/  5.545896336e-02,  4.834151725e-02,  5.089954669e-03, /*DMEAN*/ -4.048780522e-03, /*EDMEAN*/  5.511347299e-03, /*CHI2*/  1.691802584e+00 },
            {370, /* SP "K2.5V          " idx */ 210, /*MAG*/  6.630000000e+00,  5.740000000e+00,  4.770000000e+00,  4.367000000e+00,  3.722000000e+00,  3.683000000e+00, /*EMAG*/  8.544004000e-03,  3.000000000e-03,  2.022375000e-02,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/ -7.134801131e-02, -2.441493263e-02, -4.054161021e-02, /*EDIAM*/  5.543765196e-02,  4.831443399e-02,  4.530855194e-02, /*DMEAN*/ -4.305019412e-02, /*EDMEAN*/  2.848488944e-02, /*CHI2*/  3.824688122e-01 },
            {371, /* SP "K3V            " idx */ 212, /*MAG*/  6.708000000e+00,  5.790000000e+00,  4.730000000e+00,  4.152000000e+00,  3.657000000e+00,  3.481000000e+00, /*EMAG*/  3.319168000e-02,  2.721558000e-02,  3.377407000e-02,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/ -3.859839530e-03, -3.343259802e-03,  9.393962538e-03, /*EDIAM*/  5.548106791e-02,  4.833155223e-02,  4.531681636e-02, /*DMEAN*/  1.526076007e-03, /*EDMEAN*/  2.859816536e-02, /*CHI2*/  2.355048396e-02 },
            {372, /* SP "M1.5V          " idx */ 246, /*MAG*/  9.444000000e+00,  7.970000000e+00,  5.890000000e+00,  4.999000000e+00,  4.149000000e+00,  4.039000000e+00, /*EMAG*/  1.421267000e-02,  1.100000000e-02,  1.486607000e-02,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/ -4.663503404e-02,  1.555552128e-02, -1.315564218e-02, /*EDIAM*/  5.545687628e-02,  4.834506201e-02,  4.532135370e-02, /*DMEAN*/ -1.202366603e-02, /*EDMEAN*/  2.847565699e-02, /*CHI2*/  4.810831155e-01 },
            {373, /* SP "K7V            " idx */ 228, /*MAG*/  9.120000000e+00,  7.700000000e+00,  5.980000000e+00,  4.779000000e+00,  4.043000000e+00,  4.136000000e+00, /*EMAG*/  3.754949000e-02,  3.619343000e-02,  4.701026000e-02,  1.740000000e-01,  2.000000030e-01,  2.000000000e-02, /*DIAM*/ -5.145773805e-03,  9.173928230e-03, -6.567046515e-02, /*EDIAM*/  4.840237325e-02,  4.840031193e-02,  5.463434046e-03, /*DMEAN*/ -6.426875298e-02, /*EDMEAN*/  9.909356201e-03, /*CHI2*/  1.848735210e+00 },
            {374, /* SP "K8V            " idx */ 232, /*MAG*/  7.926000000e+00,  6.600000000e+00,  5.310000000e+00,  3.894000000e+00,  3.298000000e+00,  2.962000000e+00, /*EMAG*/  8.944272000e-03,  4.000000000e-03,  1.077033000e-02,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/  1.585447296e-01,  1.496008385e-01,  1.765349084e-01, /*EDIAM*/  5.550947364e-02,  4.838978658e-02,  4.535695344e-02, /*DMEAN*/  1.625258271e-01, /*EDMEAN*/  2.861536143e-02, /*CHI2*/  3.042095155e+00 },
            {375, /* SP "M2.0V          " idx */ 248, /*MAG*/  8.992000000e+00,  7.490000000e+00,  5.420000000e+00,  4.320000000e+00,  3.722000000e+00,  3.501000000e+00, /*EMAG*/  3.788781000e-02,  3.520633000e-02,  4.049057000e-02,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/  1.013843874e-01,  9.964287498e-02,  9.845709608e-02, /*EDIAM*/  5.550356982e-02,  4.835628667e-02,  4.532179065e-02, /*DMEAN*/  9.963220073e-02, /*EDMEAN*/  2.875151479e-02, /*CHI2*/  9.818521360e-01 },
            {376, /* SP "M2.0V          " idx */ 248, /*MAG*/  8.992000000e+00,  7.490000000e+00,  5.420000000e+00,  4.320000000e+00,  3.722000000e+00,  3.501000000e+00, /*EMAG*/  3.788781000e-02,  3.520633000e-02,  4.049057000e-02,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/  1.013843874e-01,  9.964287498e-02,  9.845709608e-02, /*EDIAM*/  5.550356982e-02,  4.835628667e-02,  4.532179065e-02, /*DMEAN*/  9.963220073e-02, /*EDMEAN*/  2.875151479e-02, /*CHI2*/  9.818521360e-01 },
            {377, /* SP "M2.0V          " idx */ 248, /*MAG*/  1.031100000e+01,  8.820000000e+00,  6.810000000e+00,  5.538000000e+00,  5.002000000e+00,  4.769000000e+00, /*EMAG*/  4.150614000e-02,  4.145793000e-02,  4.264692000e-02,  1.900000000e-02,  2.100000000e-02,  2.000000000e-02, /*DIAM*/ -1.336156161e-01, -1.542948720e-01, -1.535137106e-01, /*EDIAM*/  6.834846553e-03,  6.069842652e-03,  5.178633280e-03, /*DMEAN*/ -1.495728203e-01, /*EDMEAN*/  1.007878587e-02, /*CHI2*/  4.690701204e+00 },
            {378, /* SP "M4.0V          " idx */ 256, /*MAG*/  9.895000000e+00,  8.460000000e+00,  6.390000000e+00,  5.181000000e+00,  4.775000000e+00,  4.415000000e+00, /*EMAG*/  3.275668000e-02,  1.700000000e-02,  2.624881000e-02,  3.700000000e-02,  2.000000030e-01,  1.700000000e-02, /*DIAM*/ -8.189640164e-02, -1.153777692e-01, -7.271898275e-02, /*EDIAM*/  1.154364659e-02,  4.853743091e-02,  5.526158853e-03, /*DMEAN*/ -7.450935907e-02, /*EDMEAN*/  8.514730415e-03, /*CHI2*/  4.932224765e-01 },
            {379, /* SP "K2V            " idx */ 208, /*MAG*/  6.597000000e+00,  5.770000000e+00,  4.900000000e+00,  4.446000000e+00,  4.053000000e+00,  4.039000000e+00, /*EMAG*/  1.264911000e-02,  4.000000000e-03,  2.039608000e-02,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/ -9.516466564e-02, -1.062992986e-01, -1.232104657e-01, /*EDIAM*/  5.543413396e-02,  4.831107767e-02,  4.530636521e-02, /*DMEAN*/ -1.100195190e-01, /*EDMEAN*/  2.918021185e-02, /*CHI2*/  5.050253395e-01 },
            {380, /* SP "M3.5V          " idx */ 254, /*MAG*/  1.065500000e+01,  9.150000000e+00,  7.040000000e+00,  5.335000000e+00,  4.766000000e+00,  4.548000000e+00, /*EMAG*/  3.178050000e-02,  2.900000000e-02,  1.041201000e-01,  2.100000000e-02,  3.300000000e-02,  2.100000000e-02, /*DIAM*/ -6.554200872e-02, -8.374710504e-02, -8.728195468e-02, /*EDIAM*/  7.376713682e-03,  9.016410391e-03,  5.689832059e-03, /*DMEAN*/ -8.054639326e-02, /*EDMEAN*/  7.932310072e-03, /*CHI2*/  3.316733903e+00 },
            {381, /* SP "M2.0V          " idx */ 248, /*MAG*/  1.003300000e+01,  8.550000000e+00,  6.580000000e+00,  5.429000000e+00,  4.919000000e+00,  4.618000000e+00, /*EMAG*/  1.920937000e-02,  1.500000000e-02,  7.158911000e-02,  2.900000000e-02,  5.900000000e-02,  2.400000000e-02, /*DIAM*/ -1.241781160e-01, -1.454077123e-01, -1.264407175e-01, /*EDIAM*/  8.641805867e-03,  1.453366696e-02,  5.899004648e-03, /*DMEAN*/ -1.276164496e-01, /*EDMEAN*/  6.870469432e-03, /*CHI2*/  4.137629289e+00 },
            {382, /* SP "K3V            " idx */ 212, /*MAG*/  6.570000000e+00,  5.570000000e+00,  4.530000000e+00,  3.981000000e+00,  3.469000000e+00,  3.261000000e+00, /*EMAG*/  2.618148000e-02,  2.618148000e-02,  2.618148000e-02,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/  2.657766092e-02,  3.293689638e-02,  5.339396319e-02, /*EDIAM*/  5.547813379e-02,  4.833058045e-02,  4.531639568e-02, /*DMEAN*/  3.930915116e-02, /*EDMEAN*/  2.857507414e-02, /*CHI2*/  5.780859849e-02 },
            {383, /* SP "F2V            " idx */ 128, /*MAG*/  6.176000000e+00,  5.780000000e+00,  5.320000000e+00,  4.990000000e+00,  4.878000000e+00,  4.715000000e+00, /*EMAG*/  8.062258000e-03,  4.000000000e-03,  1.077033000e-02,  2.600000000e-02,  3.300000000e-02,  2.100000000e-02, /*DIAM*/ -3.461806324e-01, -3.552859532e-01, -3.389512615e-01, /*EDIAM*/  8.178741934e-03,  8.772913717e-03,  5.647599442e-03, /*DMEAN*/ -3.439530389e-01, /*EDMEAN*/  1.256318566e-02, /*CHI2*/  1.081704540e+00 },
            {384, /* SP "F6IV-V         " idx */ 144, /*MAG*/  5.232000000e+00,  4.780000000e+00,  4.250000000e+00,  4.037000000e+00,  3.944000000e+00,  4.020000000e+00, /*EMAG*/  6.708204000e-03,  3.000000000e-03,  3.014963000e-02,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/ -1.537239541e-01, -1.675306130e-01, -2.025704901e-01, /*EDIAM*/  5.545222029e-02,  4.835083960e-02,  4.531753325e-02, /*DMEAN*/ -1.776781519e-01, /*EDMEAN*/  3.356964776e-02, /*CHI2*/  6.895107812e-01 },
            {385, /* SP "G5             " idx */ 180, /*MAG*/  7.293000000e+00,  6.710000000e+00,  6.060000000e+00,  5.703000000e+00,  5.418000000e+00,  5.346000000e+00, /*EMAG*/  1.280625000e-02,  8.000000000e-03,  1.280625000e-02,  2.400000000e-02,  3.400000000e-02,  2.000000000e-02, /*DIAM*/ -4.297820745e-01, -4.314392582e-01, -4.299757535e-01, /*EDIAM*/  7.166026052e-03,  8.533099463e-03,  4.903455554e-03, /*DMEAN*/ -4.301822795e-01, /*EDMEAN*/  9.599491442e-03, /*CHI2*/  3.427800427e+00 },
            {386, /* SP "G0             " idx */ 160, /*MAG*/  7.769000000e+00,  7.200000000e+00,  6.560000000e+00,  6.145000000e+00,  5.923000000e+00,  5.829000000e+00, /*EMAG*/  1.303840000e-02,  7.000000000e-03,  1.220656000e-02,  2.000000000e-02,  2.600000000e-02,  2.300000000e-02, /*DIAM*/ -5.413415978e-01, -5.427454061e-01, -5.418055581e-01, /*EDIAM*/  6.337342333e-03,  6.885051778e-03,  5.600158312e-03, /*DMEAN*/ -5.419072564e-01, /*EDMEAN*/  9.809810861e-03, /*CHI2*/  3.567286061e-02 },
            {387, /* SP "F2             " idx */ 128, /*MAG*/  7.008000000e+00,  6.570000000e+00,  6.060000000e+00,  5.748000000e+00,  5.560000000e+00,  5.513000000e+00, /*EMAG*/  7.810250000e-03,  5.000000000e-03,  1.118034000e-02,  2.100000000e-02,  3.300000000e-02,  2.600000000e-02, /*DIAM*/ -4.953234918e-01, -4.872314804e-01, -4.987614828e-01, /*EDIAM*/  6.995817072e-03,  8.773786272e-03,  6.628267817e-03, /*DMEAN*/ -4.949852216e-01, /*EDMEAN*/  1.360593197e-02, /*CHI2*/  1.672290252e+00 },
            {388, /* SP "F5             " idx */ 140, /*MAG*/  8.040000000e+00,  7.530000000e+00,  6.950000000e+00,  6.544000000e+00,  6.346000000e+00,  6.283000000e+00, /*EMAG*/  1.220656000e-02,  7.000000000e-03,  1.220656000e-02,  2.100000000e-02,  2.900000000e-02,  1.800000000e-02, /*DIAM*/ -6.379448300e-01, -6.341209892e-01, -6.436126400e-01, /*EDIAM*/  6.709456548e-03,  7.705443900e-03,  4.776885245e-03, /*DMEAN*/ -6.404296131e-01, /*EDMEAN*/  1.191807105e-02, /*CHI2*/  5.100609546e-01 },
            {389, /* SP "G0V            " idx */ 160, /*MAG*/  6.400000000e+00,  5.800000000e+00,  5.160000000e+00,  4.689000000e+00,  4.430000000e+00,  4.388000000e+00, /*EMAG*/  6.403124000e-03,  4.000000000e-03,  7.011419000e-02,  2.000000030e-01,  2.000000030e-01,  2.700000000e-02, /*DIAM*/ -2.458415934e-01, -2.403096039e-01, -2.525281815e-01, /*EDIAM*/  5.544128872e-02,  4.833191262e-02,  6.448199455e-03, /*DMEAN*/ -2.522497031e-01, /*EDMEAN*/  9.787074425e-03, /*CHI2*/  1.237522837e+00 },
            {390, /* SP "F0V            " idx */ 120, /*MAG*/  6.229000000e+00,  5.970000000e+00,  5.680000000e+00,  5.383000000e+00,  5.280000000e+00,  5.240000000e+00, /*EMAG*/  1.166190000e-02,  6.000000000e-03,  1.166190000e-02,  2.700000000e-02,  1.800000000e-02,  1.800000000e-02, /*DIAM*/ -4.442135143e-01, -4.488479659e-01, -4.569538832e-01, /*EDIAM*/  8.798952735e-03,  6.076259322e-03,  5.428603134e-03, /*DMEAN*/ -4.521218899e-01, /*EDMEAN*/  1.103328859e-02, /*CHI2*/  1.307881330e+00 },
            {391, /* SP "F6V            " idx */ 144, /*MAG*/  6.399000000e+00,  5.830000000e+00,  5.190000000e+00,  4.755000000e+00,  4.794000000e+00,  4.445000000e+00, /*EMAG*/  4.242641000e-03,  3.000000000e-03,  3.000000000e-03,  3.700000000e-02,  2.000000030e-01,  2.000000000e-02, /*DIAM*/ -2.718310987e-01, -3.292815882e-01, -2.711471335e-01, /*EDIAM*/  1.074313681e-02,  4.835083960e-02,  5.108954471e-03, /*DMEAN*/ -2.717143256e-01, /*EDMEAN*/  6.888854831e-03, /*CHI2*/  1.621940941e+00 },
            {392, /* SP "K1V            " idx */ 204, /*MAG*/  6.076000000e+00,  5.240000000e+00,  4.360000000e+00,  3.855000000e+00,  3.391000000e+00,  3.285000000e+00, /*EMAG*/  2.589697000e-02,  2.463033000e-02,  2.658295000e-02,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/  1.897210975e-02,  2.523143356e-02,  2.773247568e-02, /*EDIAM*/  5.545936680e-02,  4.831644001e-02,  4.530744656e-02, /*DMEAN*/  2.457462913e-02, /*EDMEAN*/  2.848448186e-02, /*CHI2*/  3.627908253e-01 },
            {393, /* SP "G5IV           " idx */ 180, /*MAG*/  6.990000000e+00,  6.270000000e+00,  5.500000000e+00,  5.386000000e+00,  4.678000000e+00,  4.601000000e+00, /*EMAG*/  1.029563000e-02,  5.000000000e-03,  1.118034000e-02,  2.000000030e-01,  3.100000000e-02,  2.100000000e-02, /*DIAM*/ -3.758267166e-01, -2.710657149e-01, -2.729611527e-01, /*EDIAM*/  5.542028258e-02,  7.835725515e-03,  5.110401768e-03, /*DMEAN*/ -2.729791886e-01, /*EDMEAN*/  1.053290968e-02, /*CHI2*/  1.407414605e+00 },
            {394, /* SP "G9VCN+1        " idx */ 196, /*MAG*/  7.237000000e+00,  6.420000000e+00,  5.570000000e+00,  5.023000000e+00,  4.637000000e+00,  4.491000000e+00, /*EMAG*/  1.581139000e-02,  5.000000000e-03,  2.061553000e-02,  2.600000000e-02,  2.100000000e-02,  2.000000000e-02, /*DIAM*/ -2.314874382e-01, -2.381375846e-01, -2.251953741e-01, /*EDIAM*/  7.670484305e-03,  5.543346790e-03,  4.938372821e-03, /*DMEAN*/ -2.309629661e-01, /*EDMEAN*/  1.134400075e-02, /*CHI2*/  1.514567341e+00 },
            {395, /* SP "F7IV           " idx */ 148, /*MAG*/  6.231000000e+00,  5.720000000e+00,  5.140000000e+00,  4.715000000e+00,  4.642000000e+00,  4.506000000e+00, /*EMAG*/  7.615773000e-03,  3.000000000e-03,  1.044031000e-02,  2.000000030e-01,  1.180000000e-01,  2.000000000e-02, /*DIAM*/ -2.674221081e-01, -2.966806118e-01, -2.865194043e-01, /*EDIAM*/  5.544959135e-02,  2.863157874e-02,  5.065281924e-03, /*DMEAN*/ -2.866531783e-01, /*EDMEAN*/  7.896261463e-03, /*CHI2*/  1.635524041e+00 },
            {396, /* SP "G2V            " idx */ 168, /*MAG*/  6.609000000e+00,  5.970000000e+00,  5.270000000e+00,  5.260000000e+00,  4.595000000e+00,  4.405000000e+00, /*EMAG*/  6.324555000e-03,  6.000000000e-03,  6.000000000e-03,  2.000000030e-01,  7.600000000e-02,  1.500000000e-02, /*DIAM*/ -3.821936789e-01, -2.705278504e-01, -2.468921684e-01, /*EDIAM*/  5.543309318e-02,  1.852171054e-02,  3.917457670e-03, /*DMEAN*/ -2.482665802e-01, /*EDMEAN*/  7.869081366e-03, /*CHI2*/  2.563052793e+00 },
            {397, /* SP "F6V            " idx */ 144, /*MAG*/  5.617000000e+00,  5.130000000e+00,  4.570000000e+00,  4.127000000e+00,  3.942000000e+00,  3.868000000e+00, /*EMAG*/  5.000000000e-03,  3.000000000e-03,  3.000000000e-03,  2.000000030e-01,  1.920000000e-01,  2.000000030e-01, /*DIAM*/ -1.517596684e-01, -1.526123249e-01, -1.589792486e-01, /*EDIAM*/  5.545222029e-02,  4.642511049e-02,  4.531753325e-02, /*DMEAN*/ -1.548255586e-01, /*EDMEAN*/  2.916698512e-02, /*CHI2*/  6.589434089e-03 },
            {398, /* SP "F9IV-V+L4+L4   " idx */ 156, /*MAG*/  6.436000000e+00,  5.860000000e+00,  5.190000000e+00,  4.998000000e+00,  4.688000000e+00,  4.458000000e+00, /*EMAG*/  1.627882000e-02,  3.000000000e-03,  7.006426000e-02,  2.000000030e-01,  2.000000030e-01,  2.000000000e-02, /*DIAM*/ -3.300784098e-01, -3.008967823e-01, -2.687868672e-01, /*EDIAM*/  5.544451910e-02,  4.833717702e-02,  4.999787866e-03, /*DMEAN*/ -2.695131321e-01, /*EDMEAN*/  9.791662066e-03, /*CHI2*/  2.282440831e+00 },
            {399, /* SP "F8III-IV       " idx */ 152, /*MAG*/  5.580000000e+00,  5.040000000e+00,  4.430000000e+00,  4.341000000e+00,  3.947000000e+00,  4.008000000e+00, /*EMAG*/  3.000000000e-03,  3.000000000e-03,  2.022375000e-02,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/ -2.139151678e-01, -1.565683007e-01, -1.902200390e-01, /*EDIAM*/  5.544728984e-02,  4.834197395e-02,  4.530869959e-02, /*DMEAN*/ -1.848214521e-01, /*EDMEAN*/  3.115280993e-02, /*CHI2*/  2.391937539e+00 },
            {400, /* SP "G2.5V          " idx */ 170, /*MAG*/  6.544000000e+00,  5.860000000e+00,  5.120000000e+00,  4.593000000e+00,  4.045000000e+00,  4.297000000e+00, /*EMAG*/  5.385165000e-03,  5.000000000e-03,  2.061553000e-02,  2.000000030e-01,  2.000000030e-01,  3.600000000e-02, /*DIAM*/ -2.034249790e-01, -1.414922122e-01, -2.238568167e-01, /*EDIAM*/  5.543006206e-02,  4.831732892e-02,  8.372077656e-03, /*DMEAN*/ -2.211639341e-01, /*EDMEAN*/  1.365963107e-02, /*CHI2*/  1.007470589e+00 },
            {401, /* SP "G0V            " idx */ 160, /*MAG*/  5.999000000e+00,  5.380000000e+00,  4.680000000e+00,  4.160000000e+00,  3.905000000e+00,  3.911000000e+00, /*EMAG*/  4.242641000e-03,  3.000000000e-03,  2.022375000e-02,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/ -1.316719489e-01, -1.309788630e-01, -1.556303699e-01, /*EDIAM*/  5.544091650e-02,  4.833178943e-02,  4.530239034e-02, /*DMEAN*/ -1.408453656e-01, /*EDMEAN*/  2.933204535e-02, /*CHI2*/  5.943613605e-02 },
            {402, /* SP "K0V            " idx */ 200, /*MAG*/  7.199000000e+00,  6.440000000e+00,  5.640000000e+00,  4.969000000e+00,  4.637000000e+00,  4.515000000e+00, /*EMAG*/  5.830952000e-03,  5.000000000e-03,  5.000000000e-03,  2.000000030e-01,  3.600000000e-02,  1.800000000e-02, /*DIAM*/ -2.061270836e-01, -2.318087940e-01, -2.246700743e-01, /*EDIAM*/  5.542395092e-02,  8.980525390e-03,  4.546533857e-03, /*DMEAN*/ -2.259266356e-01, /*EDMEAN*/  8.673203238e-03, /*CHI2*/  1.709034765e+00 },
            {403, /* SP "F5V            " idx */ 140, /*MAG*/  5.430000000e+00,  4.990000000e+00,  4.480000000e+00,  4.203000000e+00,  4.059000000e+00,  3.944000000e+00, /*EMAG*/  2.828427000e-03,  2.000000000e-03,  2.000000000e-03,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/ -1.850251804e-01, -1.871560020e-01, -1.810943850e-01, /*EDIAM*/  5.545610532e-02,  4.835618286e-02,  4.532390400e-02, /*DMEAN*/ -1.842159651e-01, /*EDMEAN*/  2.886946088e-02, /*CHI2*/  5.594946685e-01 },
            {404, /* SP "G1.5Vb         " idx */ 166, /*MAG*/  6.633000000e+00,  5.990000000e+00,  5.380000000e+00,  5.091000000e+00,  4.724000000e+00,  4.426000000e+00, /*EMAG*/  7.211103000e-03,  4.000000000e-03,  3.026549000e-02,  2.000000030e-01,  1.940000000e-01,  1.700000000e-02, /*DIAM*/ -3.363025114e-01, -3.016061252e-01, -2.525101303e-01, /*EDIAM*/  5.543448734e-02,  4.687787182e-02,  4.324985861e-03, /*DMEAN*/ -2.532924516e-01, /*EDMEAN*/  9.669149578e-03, /*CHI2*/  1.204772705e+00 },
            {405, /* SP "G3V            " idx */ 172, /*MAG*/  6.911000000e+00,  6.250000000e+00,  5.640000000e+00,  4.993000000e+00,  4.695000000e+00,  4.651000000e+00, /*EMAG*/  6.403124000e-03,  4.000000000e-03,  3.026549000e-02,  3.700000000e-02,  3.600000000e-02,  1.600000000e-02, /*DIAM*/ -2.814161715e-01, -2.812163016e-01, -2.921252011e-01, /*EDIAM*/  1.061333124e-02,  9.040932632e-03,  4.094570705e-03, /*DMEAN*/ -2.895921154e-01, /*EDMEAN*/  1.003828841e-02, /*CHI2*/  6.572279160e-01 },
            {406, /* SP "G2V            " idx */ 168, /*MAG*/  6.349000000e+00,  5.660000000e+00,  4.920000000e+00,  4.309000000e+00,  3.908000000e+00,  3.999000000e+00, /*EMAG*/  4.242641000e-03,  3.000000000e-03,  2.022375000e-02,  2.000000030e-01,  2.000000030e-01,  3.600000000e-02, /*DIAM*/ -1.427740325e-01, -1.175784317e-01, -1.631695394e-01, /*EDIAM*/  5.543165725e-02,  4.831995255e-02,  8.376430144e-03, /*DMEAN*/ -1.614879803e-01, /*EDMEAN*/  2.020030305e-02, /*CHI2*/  4.078674151e-01 },
            {407, /* SP "G0V            " idx */ 160, /*MAG*/  6.547000000e+00,  5.960000000e+00,  5.300000000e+00,  4.793000000e+00,  4.598000000e+00,  4.559000000e+00, /*EMAG*/  5.830952000e-03,  5.000000000e-03,  5.000000000e-03,  3.500000000e-02,  3.600000000e-02,  3.800000000e-02, /*DIAM*/ -2.623415937e-01, -2.742395655e-01, -2.870172331e-01, /*EDIAM*/  1.015991757e-02,  9.134912318e-03,  8.842766879e-03, /*DMEAN*/ -2.757150553e-01, /*EDMEAN*/  1.281261497e-02, /*CHI2*/  1.180028967e+00 },
            {408, /* SP "G2.5IVa        " idx */ 170, /*MAG*/  6.116000000e+00,  5.450000000e+00,  4.760000000e+00,  4.655000000e+00,  4.234000000e+00,  3.911000000e+00, /*EMAG*/  7.615773000e-03,  3.000000000e-03,  1.044031000e-02,  2.000000030e-01,  2.000000030e-01,  2.100000000e-02, /*DIAM*/ -2.520678368e-01, -2.039980497e-01, -1.472874725e-01, /*EDIAM*/  5.542921110e-02,  4.831704725e-02,  5.128683865e-03, /*DMEAN*/ -1.486448785e-01, /*EDMEAN*/  8.660352155e-03, /*CHI2*/  3.029430936e+00 },
            {409, /* SP "F7V            " idx */ 148, /*MAG*/  6.136000000e+00,  5.580000000e+00,  4.950000000e+00,  4.871000000e+00,  4.599000000e+00,  4.306000000e+00, /*EMAG*/  4.242641000e-03,  3.000000000e-03,  3.000000000e-03,  2.000000030e-01,  1.900000000e-01,  3.600000000e-02, /*DIAM*/ -3.213506803e-01, -2.920813900e-01, -2.449427613e-01, /*EDIAM*/  5.544959135e-02,  4.593901480e-02,  8.457752564e-03, /*DMEAN*/ -2.479870054e-01, /*EDMEAN*/  1.526643363e-02, /*CHI2*/  1.447678891e+00 },
            {410, /* SP "A7V            " idx */ 108, /*MAG*/  4.690000000e+00,  4.490000000e+00,  4.270000000e+00,  4.372000000e+00,  4.204000000e+00,  4.064000000e+00, /*EMAG*/  6.708204000e-03,  3.000000000e-03,  2.022375000e-02,  2.000000030e-01,  2.000000030e-01,  3.600000000e-02, /*DIAM*/ -2.889715562e-01, -2.625123643e-01, -2.405167809e-01, /*EDIAM*/  5.568340801e-02,  4.853862669e-02,  9.278874072e-03, /*DMEAN*/ -2.423138253e-01, /*EDMEAN*/  1.274730480e-02, /*CHI2*/  3.929617010e-01 },
            {411, /* SP "K0III          " idx */ 200, /*MAG*/  3.059000000e+00,  2.040000000e+00,  1.040000000e+00,  3.700000000e-01, -2.190000000e-01, -2.740000000e-01, /*EMAG*/  2.137756000e-02,  4.000000000e-03,  2.039608000e-02,  1.980000000e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/  7.289532874e-01,  7.581990029e-01,  7.433518380e-01, /*EDIAM*/  5.487056914e-02,  4.830297083e-02,  4.530047979e-02, /*DMEAN*/  7.446195905e-01, /*EDMEAN*/  2.833452899e-02, /*CHI2*/  5.881945169e-02 },
            {412, /* SP "M2III          " idx */ 248, /*MAG*/  4.850000000e+00,  3.260000000e+00,  1.320000000e+00,  1.610000000e-01, -8.510000000e-01, -1.025000000e+00, /*EMAG*/  5.108816000e-02,  3.000000000e-03,  2.022375000e-02,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/  9.277326140e-01,  1.028389971e+00,  1.011435212e+00, /*EDIAM*/  5.543817515e-02,  4.833463774e-02,  4.531241614e-02, /*DMEAN*/  9.953390154e-01, /*EDMEAN*/  2.843131867e-02, /*CHI2*/  1.781810667e+00 },
            {413, /* SP "M4III          " idx */ 256, /*MAG*/  5.980000000e+00,  4.480000000e+00,  2.060000000e+00,  6.710000000e-01, -2.310000000e-01, -4.680000000e-01, /*EMAG*/  1.401749000e-01,  7.000000000e-03,  2.118962000e-02,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/  8.608000410e-01,  9.281397561e-01,  9.276094993e-01, /*EDIAM*/  5.560013014e-02,  4.853322491e-02,  4.542723276e-02, /*DMEAN*/  9.102789320e-01, /*EDMEAN*/  2.855278836e-02, /*CHI2*/  9.710931182e-01 },
            {414, /* SP "M3SIII         " idx */ 252, /*MAG*/  6.483000000e+00,  4.710000000e+00,  2.080000000e+00,  4.830000000e-01, -4.760000000e-01, -6.620000000e-01, /*EMAG*/  8.062258000e-03,  4.000000000e-03,  2.039608000e-02,  1.880000000e-01,  1.920000000e-01,  1.640000000e-01, /*DIAM*/  9.417172428e-01,  9.982047279e-01,  9.724048091e-01, /*EDIAM*/  5.213579685e-02,  4.643589199e-02,  3.719174471e-02, /*DMEAN*/  9.728437447e-01, /*EDMEAN*/  2.540141423e-02, /*CHI2*/  3.767130343e-01 },
            {415, /* SP "M2Iab:         " idx */ 248, /*MAG*/  6.380000000e+00,  4.320000000e+00,  1.780000000e+00,  3.930000000e-01, -6.090000000e-01, -9.130000000e-01, /*EMAG*/  2.915476000e-02,  1.100000000e-02,  3.195309000e-02,  2.000000030e-01,  1.860000000e-01,  1.620000000e-01, /*DIAM*/  9.449111857e-01,  1.013728492e+00,  1.013946161e+00, /*EDIAM*/  5.544413062e-02,  4.496575488e-02,  3.672826177e-02, /*DMEAN*/  9.995073867e-01, /*EDMEAN*/  2.535089141e-02, /*CHI2*/  4.089745047e-01 },
            {416, /* SP "F0II           " idx */ 120, /*MAG*/ -4.560000000e-01, -6.200000000e-01, -8.500000000e-01, -1.169000000e+00, -1.280000000e+00, -1.304000000e+00, /*EMAG*/  2.334524000e-02,  1.600000000e-02,  2.561250000e-02,  1.130000000e-01,  1.900000000e-01,  2.000000030e-01, /*DIAM*/  8.632686480e-01,  8.619146996e-01,  8.506373772e-01, /*EDIAM*/  3.164027610e-02,  4.603727054e-02,  4.539923299e-02, /*DMEAN*/  8.598152133e-01, /*EDMEAN*/  2.272975065e-02, /*CHI2*/  2.690019305e-01 },
            {417, /* SP "K3II-III       " idx */ 212, /*MAG*/  3.430000000e+00,  1.990000000e+00,  6.000000000e-01, -2.560000000e-01, -9.900000000e-01, -1.127000000e+00, /*EMAG*/  4.000000000e-03,  4.000000000e-03,  2.039608000e-02,  1.470000000e-01,  1.920000000e-01,  2.000000030e-01, /*DIAM*/  9.244258886e-01,  9.609913849e-01,  9.522260934e-01, /*EDIAM*/  4.080362037e-02,  4.639173767e-02,  4.531129493e-02, /*DMEAN*/  9.440913836e-01, /*EDMEAN*/  2.541047974e-02, /*CHI2*/  5.140597263e-01 },
            {418, /* SP "M0.5III        " idx */ 242, /*MAG*/  4.314000000e+00,  2.730000000e+00,  9.100000000e-01, -1.310000000e-01, -1.025000000e+00, -1.173000000e+00, /*EMAG*/  1.044031000e-02,  3.000000000e-03,  2.022375000e-02,  1.660000000e-01,  1.800000000e-01,  2.000000030e-01, /*DIAM*/  9.750769458e-01,  1.044796871e+00,  1.023502603e+00, /*EDIAM*/  4.609759860e-02,  4.355780595e-02,  4.534115217e-02, /*DMEAN*/  1.015713938e+00, /*EDMEAN*/  2.601045818e-02, /*CHI2*/  5.918230423e-01 },
            {419, /* SP "K2II-III       " idx */ 208, /*MAG*/  3.357000000e+00,  1.910000000e+00,  4.600000000e-01, -3.500000000e-01, -1.024000000e+00, -1.176000000e+00, /*EMAG*/  2.828427000e-03,  2.000000000e-03,  2.000000000e-03,  1.430000000e-01,  1.920000000e-01,  2.000000030e-01, /*DIAM*/  9.359067783e-01,  9.592960481e-01,  9.553953898e-01, /*EDIAM*/  3.968707351e-02,  4.638347773e-02,  4.530627376e-02, /*DMEAN*/  9.487388845e-01, /*EDMEAN*/  2.513148927e-02, /*CHI2*/  2.107086826e-01 },
            {420, /* SP "K3III          " idx */ 212, /*MAG*/  4.672000000e+00,  3.120000000e+00,  1.520000000e+00,  4.090000000e-01, -4.520000000e-01, -6.330000000e-01, /*EMAG*/  9.708244000e-02,  4.000000000e-03,  2.039608000e-02,  1.700000000e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/  8.271312443e-01,  8.778085043e-01,  8.701385009e-01, /*EDIAM*/  4.715401733e-02,  4.831879686e-02,  4.531129493e-02, /*DMEAN*/  8.583765060e-01, /*EDMEAN*/  2.709185627e-02, /*CHI2*/  2.500817216e-01 },
            {421, /* SP "B2IV           " idx */ 48, /*MAG*/  3.494000000e+00,  3.690000000e+00,  3.920000000e+00,  4.135000000e+00,  4.248000000e+00,  4.247000000e+00, /*EMAG*/  4.472136000e-03,  2.000000000e-03,  3.006659000e-02,  2.000000030e-01,  2.000000030e-01,  3.600000000e-02, /*DIAM*/ -5.257623928e-01, -5.108426026e-01, -5.069712186e-01, /*EDIAM*/  5.610318455e-02,  4.900086225e-02,  1.147002980e-02, /*DMEAN*/ -5.075615833e-01, /*EDMEAN*/  1.794210395e-02, /*CHI2*/  3.868398486e-02 },
            {422, /* SP "A5V            " idx */ 100, /*MAG*/  3.990000000e+00,  3.860000000e+00,  3.720000000e+00,  3.621000000e+00,  3.652000000e+00,  3.636000000e+00, /*EMAG*/  2.000000000e-03,  2.000000000e-03,  2.000000000e-03,  2.000000030e-01,  1.820000000e-01,  2.000000030e-01, /*DIAM*/ -1.421804744e-01, -1.682460370e-01, -1.717595431e-01, /*EDIAM*/  5.577787863e-02,  4.430352800e-02,  4.551982045e-02, /*DMEAN*/ -1.631686679e-01, /*EDMEAN*/  2.868691994e-02, /*CHI2*/  1.343036039e-01 },
            {423, /* SP "B3V            " idx */ 52, /*MAG*/  3.032000000e+00,  3.180000000e+00,  3.350000000e+00,  3.611000000e+00,  3.761000000e+00,  3.857000000e+00, /*EMAG*/  5.000000000e-03,  3.000000000e-03,  3.014963000e-02,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/ -3.959947377e-01, -3.947778274e-01, -4.115517930e-01, /*EDIAM*/  5.596407893e-02,  4.886331513e-02,  4.584267184e-02, /*DMEAN*/ -4.016947532e-01, /*EDMEAN*/  3.103670486e-02, /*CHI2*/  1.181797882e+00 },
            {424, /* SP "F6IV-V         " idx */ 144, /*MAG*/  5.232000000e+00,  4.780000000e+00,  4.250000000e+00,  4.037000000e+00,  3.944000000e+00,  4.020000000e+00, /*EMAG*/  6.708204000e-03,  3.000000000e-03,  3.014963000e-02,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/ -1.537239541e-01, -1.675306130e-01, -2.025704901e-01, /*EDIAM*/  5.545222029e-02,  4.835083960e-02,  4.531753325e-02, /*DMEAN*/ -1.776781519e-01, /*EDMEAN*/  3.237061422e-02, /*CHI2*/  7.281260157e-01 },
            {425, /* SP "A1m            " idx */ 84, /*MAG*/  4.473000000e+00,  4.420000000e+00,  4.390000000e+00,  4.315000000e+00,  4.324000000e+00,  4.315000000e+00, /*EMAG*/  9.486833000e-03,  3.000000000e-03,  3.014963000e-02,  2.000000030e-01,  1.760000000e-01,  3.600000000e-02, /*DIAM*/ -3.345329417e-01, -3.468903840e-01, -3.477751566e-01, /*EDIAM*/  5.586681800e-02,  4.294556959e-02,  9.704107452e-03, /*DMEAN*/ -3.474069195e-01, /*EDMEAN*/  1.870081237e-02, /*CHI2*/  3.669658933e-01 },
            {426, /* SP "A2V            " idx */ 88, /*MAG*/  3.327000000e+00,  3.330000000e+00,  3.320000000e+00,  3.115000000e+00,  3.190000000e+00,  3.082000000e+00, /*EMAG*/  5.830952000e-03,  3.000000000e-03,  3.014963000e-02,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/ -7.279675724e-02, -1.065521684e-01, -8.620602133e-02, /*EDIAM*/  5.586287580e-02,  4.867401087e-02,  4.555804057e-02, /*DMEAN*/ -8.971390777e-02, /*EDMEAN*/  3.172387494e-02, /*CHI2*/  6.310137354e-01 },
            {427, /* SP "B9III          " idx */ 76, /*MAG*/  3.201000000e+00,  3.250000000e+00,  3.280000000e+00,  3.115000000e+00,  3.227000000e+00,  3.122000000e+00, /*EMAG*/  5.385165000e-03,  2.000000000e-03,  3.006659000e-02,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/ -1.239731395e-01, -1.575932718e-01, -1.350866836e-01, /*EDIAM*/  5.584289565e-02,  4.866587607e-02,  4.556669974e-02, /*DMEAN*/ -1.399398320e-01, /*EDMEAN*/  2.908911758e-02, /*CHI2*/  1.856078102e-01 },
            {428, /* SP "B9Iab          " idx */ 76, /*MAG*/  4.318000000e+00,  4.220000000e+00,  3.970000000e+00,  3.973000000e+00,  3.864000000e+00,  3.683000000e+00, /*EMAG*/  3.605551000e-03,  2.000000000e-03,  3.006659000e-02,  2.000000030e-01,  2.000000030e-01,  3.600000000e-02, /*DIAM*/ -2.869731419e-01, -2.712586432e-01, -2.365392399e-01, /*EDIAM*/  5.584289565e-02,  4.866587607e-02,  9.727446673e-03, /*DMEAN*/ -2.389446944e-01, /*EDMEAN*/  1.493157790e-02, /*CHI2*/  2.721094921e+00 },
            {429, /* SP "F1V            " idx */ 124, /*MAG*/  4.832000000e+00,  4.530000000e+00,  4.180000000e+00,  3.836000000e+00,  3.760000000e+00,  3.791000000e+00, /*EMAG*/  1.403567000e-02,  1.000000047e-03,  3.001666000e-02,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/ -1.244447347e-01, -1.390349961e-01, -1.645987886e-01, /*EDIAM*/  5.551744314e-02,  4.840954710e-02,  4.537608022e-02, /*DMEAN*/ -1.452542877e-01, /*EDMEAN*/  2.879971403e-02, /*CHI2*/  8.622396153e-01 },
            {430, /* SP "K0             " idx */ 200, /*MAG*/  7.996000000e+00,  7.170000000e+00,  6.300000000e+00,  5.618000000e+00,  5.231000000e+00,  5.159000000e+00, /*EMAG*/  8.485281000e-03,  6.000000000e-03,  1.166190000e-02,  2.600000000e-02,  3.300000000e-02,  2.000000000e-02, /*DIAM*/ -3.297074426e-01, -3.449994571e-01, -3.512102221e-01, /*EDIAM*/  7.697693620e-03,  8.283798142e-03,  4.956800951e-03, /*DMEAN*/ -3.451420988e-01, /*EDMEAN*/  1.457471967e-02, /*CHI2*/  2.479548527e+00 },
            {431, /* SP "M2             " idx */ 248, /*MAG*/  1.147300000e+01,  9.950000000e+00,  7.700000000e+00,  6.462000000e+00,  5.824000000e+00,  5.607000000e+00, /*EMAG*/  5.303187000e-02,  4.676943000e-02,  1.956716000e-01,  2.400000000e-02,  3.300000000e-02,  3.400000000e-02, /*DIAM*/ -3.025977615e-01, -3.059913723e-01, -3.134407203e-01, /*EDIAM*/  8.120920581e-03,  8.680626608e-03,  8.114808646e-03, /*DMEAN*/ -3.075541783e-01, /*EDMEAN*/  1.971295113e-02, /*CHI2*/  1.230950746e+00 },
            {432, /* SP "F6V            " idx */ 144, /*MAG*/  5.586000000e+00,  5.080000000e+00,  4.500000000e+00,  4.416000000e+00,  4.201000000e+00,  3.911000000e+00, /*EMAG*/  2.828427000e-03,  2.000000000e-03,  2.000000000e-03,  2.000000030e-01,  2.000000030e-01,  2.000000000e-02, /*DIAM*/ -2.355900268e-01, -2.171570729e-01, -1.700230444e-01, /*EDIAM*/  5.545195447e-02,  4.835075164e-02,  5.108616572e-03, /*DMEAN*/ -1.709288669e-01, /*EDMEAN*/  8.445775200e-03, /*CHI2*/  2.785010819e+00 },
            {433, /* SP "G8II-III       " idx */ 192, /*MAG*/  5.730000000e+00,  4.720000000e+00,  3.700000000e+00,  2.943000000e+00,  2.484000000e+00,  2.282000000e+00, /*EMAG*/  3.162278000e-03,  3.000000000e-03,  3.014963000e-02,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/  2.049988226e-01,  2.061327047e-01,  2.247848906e-01, /*EDIAM*/  5.541788253e-02,  4.830061802e-02,  4.529689591e-02, /*DMEAN*/  2.131581372e-01, /*EDMEAN*/  2.867832980e-02, /*CHI2*/  5.542737725e-02 },
            {434, /* SP "G7V            " idx */ 188, /*MAG*/  5.449000000e+00,  4.740000000e+00,  3.990000000e+00,  3.334000000e+00,  2.974000000e+00,  2.956000000e+00, /*EMAG*/  2.335387000e-02,  2.228011000e-02,  2.994000000e-02,  2.000000000e-01,  1.760000000e-01,  2.000000030e-01, /*DIAM*/  8.994613388e-02,  8.425820391e-02,  6.788676227e-02, /*EDIAM*/  5.544297631e-02,  4.252803677e-02,  4.529931805e-02, /*DMEAN*/  7.976787450e-02, /*EDMEAN*/  2.718035547e-02, /*CHI2*/  1.326433136e+00 },
            {435, /* SP "G0V            " idx */ 160, /*MAG*/  6.002000000e+00,  5.390000000e+00,  4.710000000e+00,  4.088000000e+00,  3.989000000e+00,  3.857000000e+00, /*EMAG*/  3.605551000e-03,  2.000000000e-03,  2.000000000e-03,  2.000000030e-01,  3.600000000e-02,  3.600000000e-02, /*DIAM*/ -1.109755200e-01, -1.508310034e-01, -1.431486179e-01, /*EDIAM*/  5.544065063e-02,  9.132956733e-03,  8.402512199e-03, /*DMEAN*/ -1.462394376e-01, /*EDMEAN*/  9.952196558e-03, /*CHI2*/  5.796748296e-01 },
            {436, /* SP "M2.0V          " idx */ 248, /*MAG*/  1.122200000e+01,  9.700000000e+00,  7.460000000e+00,  6.448000000e+00,  5.865000000e+00,  5.624000000e+00, /*EMAG*/  4.341659000e-02,  2.100000000e-02,  2.000000030e-01,  2.100000000e-02,  2.000000000e-02,  1.600000000e-02, /*DIAM*/ -3.179191903e-01, -3.261937072e-01, -3.238567788e-01, /*EDIAM*/  6.731286843e-03,  5.681825969e-03,  4.308473108e-03, /*DMEAN*/ -3.234226557e-01, /*EDMEAN*/  1.117907542e-02, /*CHI2*/  8.111281297e-01 },
            {437, /* SP "K1III          " idx */ 204, /*MAG*/  6.881000000e+00,  5.930000000e+00,  4.990000000e+00,  4.508000000e+00,  3.995000000e+00,  3.984000000e+00, /*EMAG*/  3.162278000e-03,  3.000000000e-03,  3.014963000e-02,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/ -1.087868207e-01, -9.202148647e-02, -1.123040228e-01, /*EDIAM*/  5.542758857e-02,  4.830591756e-02,  4.530289212e-02, /*DMEAN*/ -1.043794120e-01, /*EDMEAN*/  2.851752366e-02, /*CHI2*/  9.095494653e-01 },
            {438, /* SP "M2.5II-IIIe    " idx */ 250, /*MAG*/  4.095000000e+00,  2.440000000e+00,  1.300000000e-01, -1.432000000e+00, -2.129000000e+00, -2.379000000e+00, /*EMAG*/  1.280625000e-02,  8.000000000e-03,  6.053098000e-02,  1.940000000e-01,  1.800000000e-01,  1.540000000e-01, /*DIAM*/  1.301933759e+00,  1.303342579e+00,  1.298761056e+00, /*EDIAM*/  5.377975122e-02,  4.352332154e-02,  3.491941172e-02, /*DMEAN*/  1.300835543e+00, /*EDMEAN*/  3.386585682e-02, /*CHI2*/  3.596537288e-01 },
            {439, /* SP "G8III          " idx */ 192, /*MAG*/  3.910000000e+00,  2.990000000e+00,  2.090000000e+00,  1.519000000e+00,  1.065000000e+00,  1.024000000e+00, /*EMAG*/  3.605551000e-03,  2.000000000e-03,  2.009975000e-02,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/  4.663023979e-01,  4.771054714e-01,  4.639819744e-01, /*EDIAM*/  5.541761655e-02,  4.830052997e-02,  4.529685780e-02, /*DMEAN*/  4.691209279e-01, /*EDMEAN*/  7.873015041e-02, /*CHI2*/  7.344824022e-01 },
            {440, /* SP "B1Ib           " idx */ 44, /*MAG*/  3.111000000e+00,  2.840000000e+00,  2.660000000e+00,  2.602000000e+00,  2.621000000e+00,  2.603000000e+00, /*EMAG*/  2.009975000e-02,  2.000000000e-03,  3.006659000e-02,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/ -1.901037477e-01, -1.729806642e-01, -1.778262780e-01, /*EDIAM*/  5.631290420e-02,  4.919714731e-02,  4.614528953e-02, /*DMEAN*/ -1.793655088e-01, /*EDMEAN*/  3.010308611e-02, /*CHI2*/  2.882666379e+00 },
            {441, /* SP "B2III          " idx */ 48, /*MAG*/  1.416000000e+00,  1.640000000e+00,  1.860000000e+00,  2.149000000e+00,  2.357000000e+00,  2.375000000e+00, /*EMAG*/  1.131371000e-02,  8.000000000e-03,  2.154066000e-02,  2.000000030e-01,  1.680000000e-01,  2.000000030e-01, /*DIAM*/ -1.334766727e-01, -1.392005737e-01, -1.372485854e-01, /*EDIAM*/  5.610633724e-02,  4.142260920e-02,  4.597068544e-02, /*DMEAN*/ -1.372173424e-01, /*EDMEAN*/  2.776682311e-02, /*CHI2*/  5.971739380e-02 },
            {442, /* SP "B9III          " idx */ 76, /*MAG*/  3.201000000e+00,  3.250000000e+00,  3.280000000e+00,  3.115000000e+00,  3.227000000e+00,  3.122000000e+00, /*EMAG*/  5.385165000e-03,  2.000000000e-03,  3.006659000e-02,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/ -1.239731395e-01, -1.575932718e-01, -1.350866836e-01, /*EDIAM*/  5.584289565e-02,  4.866587607e-02,  4.556669974e-02, /*DMEAN*/ -1.399398320e-01, /*EDMEAN*/  2.928381766e-02, /*CHI2*/  2.427045831e-01 },
            {443, /* SP "B9Vn           " idx */ 76, /*MAG*/  3.334000000e+00,  3.430000000e+00,  3.520000000e+00,  3.522000000e+00,  3.477000000e+00,  3.564000000e+00, /*EMAG*/  5.385165000e-03,  2.000000000e-03,  2.009975000e-02,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/ -2.228034981e-01, -2.104804321e-01, -2.303713566e-01, /*EDIAM*/  5.584289565e-02,  4.866587607e-02,  4.556669974e-02, /*DMEAN*/ -2.215346931e-01, /*EDMEAN*/  2.882202184e-02, /*CHI2*/  8.817899128e-01 },
            {444, /* SP "B3IV           " idx */ 52, /*MAG*/  4.590000000e+00,  4.740000000e+00,  4.860000000e+00,  4.971000000e+00,  5.093000000e+00,  5.114000000e+00, /*EMAG*/  5.385165000e-03,  2.000000000e-03,  3.006659000e-02,  1.800000000e-02,  1.600000000e-02,  2.000000000e-02, /*DIAM*/ -6.526375987e-01, -6.517739401e-01, -6.549897528e-01, /*EDIAM*/  9.610260597e-03,  8.633968517e-03,  8.600564220e-03, /*DMEAN*/ -6.532744019e-01, /*EDMEAN*/  2.205170450e-02, /*CHI2*/  2.091102384e-01 },
            {445, /* SP "B9.5IV+...     " idx */ 78, /*MAG*/  2.858000000e+00,  2.860000000e+00,  2.880000000e+00,  2.752000000e+00,  2.947000000e+00,  2.678000000e+00, /*EMAG*/  4.242641000e-03,  3.000000000e-03,  3.014963000e-02,  2.000000030e-01,  2.000000000e-01,  2.000000030e-01, /*DIAM*/ -4.486988733e-02, -9.890996795e-02, -3.772667912e-02, /*EDIAM*/  5.585188975e-02,  4.867014008e-02,  4.556473977e-02, /*DMEAN*/ -6.068677459e-02, /*EDMEAN*/  2.880014824e-02, /*CHI2*/  1.016992938e+00 },
            {446, /* SP "B8V            " idx */ 72, /*MAG*/  3.324000000e+00,  3.410000000e+00,  3.470000000e+00,  3.538000000e+00,  3.527000000e+00,  3.566000000e+00, /*EMAG*/  8.246211000e-03,  2.000000000e-03,  3.006659000e-02,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/ -2.471555505e-01, -2.386647676e-01, -2.466337417e-01, /*EDIAM*/  5.582403850e-02,  4.865883184e-02,  4.557524611e-02, /*DMEAN*/ -2.440224315e-01, /*EDMEAN*/  2.960581528e-02, /*CHI2*/  9.895018353e-02 },
            {447, /* SP "M2III          " idx */ 248, /*MAG*/  6.362000000e+00,  4.790000000e+00,  2.860000000e+00,  1.757000000e+00,  8.530000000e-01,  7.240000000e-01, /*EMAG*/  5.000000000e-03,  3.000000000e-03,  3.014963000e-02,  2.000000030e-01,  1.660000000e-01,  2.000000030e-01, /*DIAM*/  6.034647520e-01,  6.804133117e-01,  6.558804621e-01, /*EDIAM*/  5.543817515e-02,  4.014979591e-02,  4.531241614e-02, /*DMEAN*/  6.546109137e-01, /*EDMEAN*/  2.652231648e-02, /*CHI2*/  4.752960922e-01 },
            {448, /* SP "K1.5III        " idx */ 206, /*MAG*/  4.774000000e+00,  3.560000000e+00,  2.430000000e+00,  1.694000000e+00,  1.097000000e+00,  1.025000000e+00, /*EMAG*/  3.605551000e-03,  2.000000000e-03,  2.009975000e-02,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/  4.925110280e-01,  5.124713989e-01,  4.978323334e-01, /*EDIAM*/  5.543016616e-02,  4.830805876e-02,  4.530441161e-02, /*DMEAN*/  5.014913313e-01, /*EDMEAN*/  2.924721786e-02, /*CHI2*/  2.078689634e+00 },
            {449, /* SP "K0III          " idx */ 200, /*MAG*/  4.665000000e+00,  3.600000000e+00,  2.550000000e+00,  1.896000000e+00,  1.401000000e+00,  1.289000000e+00, /*EMAG*/  4.472136000e-03,  2.000000000e-03,  2.009975000e-02,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/  4.263639972e-01,  4.317242899e-01,  4.306730012e-01, /*EDIAM*/  5.542283391e-02,  4.830275951e-02,  4.530038834e-02, /*DMEAN*/  4.299062221e-01, /*EDMEAN*/  2.844863185e-02, /*CHI2*/  3.283972745e-01 },
            {450, /* SP "M0III          " idx */ 240, /*MAG*/  6.341000000e+00,  4.720000000e+00,  2.820000000e+00,  1.748000000e+00,  8.650000000e-01,  6.770000000e-01, /*EMAG*/  1.044031000e-02,  3.000000000e-03,  3.014963000e-02,  2.000000030e-01,  1.700000000e-01,  1.900000000e-01, /*DIAM*/  6.089576962e-01,  6.690266487e-01,  6.546469034e-01, /*EDIAM*/  5.549555183e-02,  4.116625607e-02,  4.309162062e-02, /*DMEAN*/  6.502891151e-01, /*EDMEAN*/  2.634846114e-02, /*CHI2*/  3.119961172e-01 },
            {451, /* SP "K2III          " idx */ 208, /*MAG*/  5.671000000e+00,  4.410000000e+00,  3.160000000e+00,  2.377000000e+00,  1.821000000e+00,  1.681000000e+00, /*EMAG*/  3.000000000e-03,  3.000000000e-03,  3.014963000e-02,  2.000000030e-01,  1.900000000e-01,  2.000000030e-01, /*DIAM*/  3.730764128e-01,  3.760664674e-01,  3.746143592e-01, /*EDIAM*/  5.543376170e-02,  4.590175718e-02,  4.530631186e-02, /*DMEAN*/  3.747610959e-01, /*EDMEAN*/  2.807010365e-02, /*CHI2*/  2.192861964e+00 },
            {452, /* SP "G8IIIa         " idx */ 192, /*MAG*/  4.502000000e+00,  3.570000000e+00,  2.670000000e+00,  2.051000000e+00,  1.642000000e+00,  1.527000000e+00, /*EMAG*/  3.605551000e-03,  3.000000000e-03,  3.000000000e-03,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/  3.635881107e-01,  3.618292051e-01,  3.654053306e-01, /*EDIAM*/  5.541788253e-02,  4.830061802e-02,  4.529689591e-02, /*DMEAN*/  3.636943168e-01, /*EDMEAN*/  2.864032492e-02, /*CHI2*/  8.501143103e-02 },
            {453, /* SP "K2III          " idx */ 208, /*MAG*/  5.642000000e+00,  4.390000000e+00,  3.120000000e+00,  2.199000000e+00,  1.553000000e+00,  1.447000000e+00, /*EMAG*/  8.544004000e-03,  3.000000000e-03,  2.022375000e-02,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/  4.208085563e-01,  4.398952621e-01,  4.270377177e-01, /*EDIAM*/  5.543376170e-02,  4.831095443e-02,  4.530631186e-02, /*DMEAN*/  4.298436470e-01, /*EDMEAN*/  2.879929916e-02, /*CHI2*/  8.001651602e-02 },
            {454, /* SP "G9II-III       " idx */ 196, /*MAG*/  4.088000000e+00,  3.110000000e+00,  2.150000000e+00,  1.415000000e+00,  7.580000000e-01,  6.970000000e-01, /*EMAG*/  4.909175000e-02,  3.000000000e-03,  2.022375000e-02,  2.000000030e-01,  1.600000000e-01,  1.900000000e-01, /*DIAM*/  5.129947158e-01,  5.611309098e-01,  5.463228856e-01, /*EDIAM*/  5.541991247e-02,  3.866439591e-02,  4.303798524e-02, /*DMEAN*/  5.457121008e-01, /*EDMEAN*/  2.559272877e-02, /*CHI2*/  4.574794057e-01 },
            {455, /* SP "K5III          " idx */ 220, /*MAG*/  5.861000000e+00,  4.320000000e+00,  2.690000000e+00,  1.449000000e+00,  6.100000000e-01,  5.860000000e-01, /*EMAG*/  5.830952000e-03,  3.000000000e-03,  3.014963000e-02,  2.000000030e-01,  1.900000000e-01,  2.000000030e-01, /*DIAM*/  6.462807280e-01,  6.847124604e-01,  6.374731617e-01, /*EDIAM*/  5.546729371e-02,  4.593660069e-02,  4.532734548e-02, /*DMEAN*/  6.571138036e-01, /*EDMEAN*/  2.798904691e-02, /*CHI2*/  3.468861737e-01 },
            {456, /* SP "K2.5III        " idx */ 210, /*MAG*/  5.213000000e+00,  3.900000000e+00,  2.610000000e+00,  1.680000000e+00,  9.530000000e-01,  8.740000000e-01, /*EMAG*/  2.507987000e-02,  2.000000000e-03,  2.009975000e-02,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/  5.310894977e-01,  5.677018077e-01,  5.467211723e-01, /*EDIAM*/  5.543738607e-02,  4.831434596e-02,  4.530851384e-02, /*DMEAN*/  5.498664878e-01, /*EDMEAN*/  2.851380877e-02, /*CHI2*/  1.125349069e-01 },
            {457, /* SP "M2III          " idx */ 248, /*MAG*/  6.970000000e+00,  5.360000000e+00,  3.420000000e+00,  2.041000000e+00,  1.105000000e+00,  1.090000000e+00, /*EMAG*/  9.433981000e-03,  5.000000000e-03,  4.031129000e-02,  2.000000030e-01,  1.660000000e-01,  1.860000000e-01, /*DIAM*/  5.686254658e-01,  6.431292644e-01,  5.880410450e-01, /*EDIAM*/  5.543902597e-02,  4.015013487e-02,  4.214894327e-02, /*DMEAN*/  6.065146656e-01, /*EDMEAN*/  2.588691526e-02, /*CHI2*/  9.398259627e-01 },
            {458, /* SP "K2III          " idx */ 208, /*MAG*/  5.102000000e+00,  3.880000000e+00,  2.750000000e+00,  1.974000000e+00,  1.327000000e+00,  1.364000000e+00, /*EMAG*/  2.000000000e-03,  2.000000000e-03,  3.006659000e-02,  2.000000030e-01,  1.740000000e-01,  2.000000030e-01, /*DIAM*/  4.439246281e-01,  4.733816439e-01,  4.324172798e-01, /*EDIAM*/  5.543349579e-02,  4.204771791e-02,  4.530627376e-02, /*DMEAN*/  4.519482587e-01, /*EDMEAN*/  2.700216440e-02, /*CHI2*/  3.487863948e-01 },
            {459, /* SP "K1III          " idx */ 204, /*MAG*/  4.144000000e+00,  3.000000000e+00,  1.910000000e+00,  1.030000000e+00,  3.760000000e-01,  4.290000000e-01, /*EMAG*/  5.000000000e-03,  3.000000000e-03,  3.014963000e-02,  1.800000000e-01,  1.700000000e-01,  1.980000000e-01, /*DIAM*/  6.288917617e-01,  6.601964236e-01,  6.151193457e-01, /*EDIAM*/  4.989968914e-02,  4.107858714e-02,  4.485081516e-02, /*DMEAN*/  6.367429791e-01, /*EDMEAN*/  2.594319372e-02, /*CHI2*/  2.197212938e-01 },
            {460, /* SP "M0III          " idx */ 240, /*MAG*/  5.433000000e+00,  3.820000000e+00,  2.030000000e+00,  8.950000000e-01, -4.900000000e-02, -1.140000000e-01, /*EMAG*/  9.486833000e-03,  3.000000000e-03,  3.014963000e-02,  2.000000030e-01,  1.960000000e-01,  1.740000000e-01, /*DIAM*/  7.759487701e-01,  8.524040833e-01,  8.099826722e-01, /*EDIAM*/  5.549555183e-02,  4.741818600e-02,  3.948025500e-02, /*DMEAN*/  8.155257246e-01, /*EDMEAN*/  2.667692807e-02, /*CHI2*/  4.682257482e-01 },
            {461, /* SP "M1III          " idx */ 244, /*MAG*/  5.541000000e+00,  4.040000000e+00,  2.250000000e+00,  1.176000000e+00,  3.250000000e-01,  1.570000000e-01, /*EMAG*/  3.224903000e-02,  4.000000000e-03,  2.039608000e-02,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/  7.121454496e-01,  7.747353220e-01,  7.594880063e-01, /*EDIAM*/  5.546743221e-02,  4.835620636e-02,  4.533103772e-02, /*DMEAN*/  7.523389713e-01, /*EDMEAN*/  2.845700705e-02, /*CHI2*/  4.042555494e-01 },
            {462, /* SP "G0V            " idx */ 160, /*MAG*/  4.828000000e+00,  4.240000000e+00,  3.570000000e+00,  3.213000000e+00,  2.905000000e+00,  2.848000000e+00, /*EMAG*/  9.219544000e-03,  2.000000000e-03,  1.019804000e-02,  2.000000030e-01,  1.980000000e-01,  2.000000030e-01, /*DIAM*/  4.290841087e-02,  6.324682084e-02,  5.494627560e-02, /*EDIAM*/  5.544065063e-02,  4.785004409e-02,  4.530235223e-02, /*DMEAN*/  5.471459727e-02, /*EDMEAN*/  2.925267528e-02, /*CHI2*/  1.961364743e-01 },
            {463, /* SP "G8III          " idx */ 192, /*MAG*/  3.784000000e+00,  2.850000000e+00,  2.020000000e+00,  1.248000000e+00,  7.330000000e-01,  6.640000000e-01, /*EMAG*/  1.513275000e-02,  2.000000000e-03,  1.019804000e-02,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/  5.305613274e-01,  5.514245386e-01,  5.417629975e-01, /*EDIAM*/  5.541761655e-02,  4.830052997e-02,  4.529685780e-02, /*DMEAN*/  5.421615329e-01, /*EDMEAN*/  2.852728386e-02, /*CHI2*/  1.718299460e-01 },
            {464, /* SP "K5III-IV       " idx */ 220, /*MAG*/  6.282000000e+00,  4.800000000e+00,  3.250000000e+00,  2.372000000e+00,  1.631000000e+00,  1.493000000e+00, /*EMAG*/  3.605551000e-03,  2.000000000e-03,  3.006659000e-02,  2.000000030e-01,  1.820000000e-01,  1.780000000e-01, /*DIAM*/  4.276646533e-01,  4.581988384e-01,  4.448527209e-01, /*EDIAM*/  5.546702797e-02,  4.401092949e-02,  4.035806602e-02, /*DMEAN*/  4.457507363e-01, /*EDMEAN*/  2.627813470e-02, /*CHI2*/  1.796659771e+00 },
            {465, /* SP "M2.5III        " idx */ 250, /*MAG*/  6.286000000e+00,  4.680000000e+00,  2.620000000e+00,  1.455000000e+00,  6.200000000e-01,  4.270000000e-01, /*EMAG*/  3.522783000e-02,  4.000000000e-03,  2.039608000e-02,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/  6.748533923e-01,  7.325487960e-01,  7.226880546e-01, /*EDIAM*/  5.543541776e-02,  4.833747988e-02,  4.531110914e-02, /*DMEAN*/  7.135472010e-01, /*EDMEAN*/  2.975248974e-02, /*CHI2*/  1.571639829e+00 },
            {466, /* SP "K5.5III        " idx */ 222, /*MAG*/  5.570000000e+00,  4.050000000e+00,  2.450000000e+00,  1.218000000e+00,  4.380000000e-01,  4.350000000e-01, /*EMAG*/  1.236932000e-02,  3.000000000e-03,  3.014963000e-02,  2.000000030e-01,  1.800000000e-01,  1.820000000e-01, /*DIAM*/  6.926328261e-01,  7.184648881e-01,  6.674188039e-01, /*EDIAM*/  5.547508980e-02,  4.353880137e-02,  4.126723331e-02, /*DMEAN*/  6.918129770e-01, /*EDMEAN*/  2.644707631e-02, /*CHI2*/  2.544077973e-01 },
            {467, /* SP "M3III          " idx */ 252, /*MAG*/  6.152000000e+00,  4.580000000e+00,  2.230000000e+00,  8.560000000e-01, -5.400000000e-02, -2.400000000e-01, /*EMAG*/  1.077033000e-02,  4.000000000e-03,  3.026549000e-02,  2.000000030e-01,  1.520000000e-01,  1.880000000e-01, /*DIAM*/  8.284940269e-01,  8.910374112e-01,  8.734996981e-01, /*EDIAM*/  5.545123173e-02,  3.681672004e-02,  4.261191978e-02, /*DMEAN*/  8.724617216e-01, /*EDMEAN*/  2.519112414e-02, /*CHI2*/  3.786708635e-01 },
            {468, /* SP "K3III          " idx */ 212, /*MAG*/  4.868000000e+00,  3.570000000e+00,  2.350000000e+00,  1.501000000e+00,  7.620000000e-01,  7.560000000e-01, /*EMAG*/  7.280110000e-03,  2.000000000e-03,  3.006659000e-02,  2.000000030e-01,  1.480000000e-01,  1.720000000e-01, /*DIAM*/  5.594348117e-01,  6.034972162e-01,  5.676640438e-01, /*EDIAM*/  5.544190970e-02,  3.579822150e-02,  3.898449458e-02, /*DMEAN*/  5.820061635e-01, /*EDMEAN*/  2.386431511e-02, /*CHI2*/  5.409818916e-01 },
            {469, /* SP "M3III          " idx */ 252, /*MAG*/  6.472000000e+00,  4.800000000e+00,  2.670000000e+00,  1.477000000e+00,  5.360000000e-01,  3.810000000e-01, /*EMAG*/  7.810250000e-03,  5.000000000e-03,  3.041381000e-02,  2.000000030e-01,  1.760000000e-01,  2.000000030e-01, /*DIAM*/  6.735029531e-01,  7.577767088e-01,  7.387624698e-01, /*EDIAM*/  5.545171021e-02,  4.258673973e-02,  4.532301761e-02, /*DMEAN*/  7.308989732e-01, /*EDMEAN*/  2.720383582e-02, /*CHI2*/  1.843825237e+00 },
            {470, /* SP "G8IIIa         " idx */ 192, /*MAG*/  4.446000000e+00,  3.490000000e+00,  2.600000000e+00,  1.795000000e+00,  1.268000000e+00,  1.223000000e+00, /*EMAG*/  2.000000000e-03,  2.000000000e-03,  3.006659000e-02,  2.000000030e-01,  1.180000000e-01,  1.660000000e-01, /*DIAM*/  4.283023973e-01,  4.487552764e-01,  4.320914630e-01, /*EDIAM*/  5.541761655e-02,  2.855413123e-02,  3.761189234e-02, /*DMEAN*/  4.406000968e-01, /*EDMEAN*/  2.114190672e-02, /*CHI2*/  7.084614763e-01 },
            {471, /* SP "K4III          " idx */ 216, /*MAG*/  6.306000000e+00,  4.800000000e+00,  3.260000000e+00,  2.256000000e+00,  1.512000000e+00,  1.339000000e+00, /*EMAG*/  8.246211000e-03,  2.000000000e-03,  3.006659000e-02,  2.000000030e-01,  1.780000000e-01,  2.000000030e-01, /*DIAM*/  4.527361391e-01,  4.800727555e-01,  4.738994323e-01, /*EDIAM*/  5.545306865e-02,  4.303213980e-02,  4.531815687e-02, /*DMEAN*/  4.712772571e-01, /*EDMEAN*/  2.807757346e-02, /*CHI2*/  3.790924043e-01 },
            {472, /* SP "K4III          " idx */ 216, /*MAG*/  6.389000000e+00,  5.020000000e+00,  3.680000000e+00,  2.876000000e+00,  2.091000000e+00,  1.939000000e+00, /*EMAG*/  3.605551000e-03,  3.000000000e-03,  3.000000000e-03,  2.000000030e-01,  1.940000000e-01,  2.000000030e-01, /*DIAM*/  2.980218510e-01,  3.494657497e-01,  3.439140289e-01, /*EDIAM*/  5.545333446e-02,  4.688470205e-02,  4.531819496e-02, /*DMEAN*/  3.341320476e-01, /*EDMEAN*/  2.818768005e-02, /*CHI2*/  2.409843483e-01 },
            {473, /* SP "K2III          " idx */ 208, /*MAG*/  4.456000000e+00,  3.290000000e+00,  2.220000000e+00,  1.293000000e+00,  7.240000000e-01,  6.710000000e-01, /*EMAG*/  7.280110000e-03,  2.000000000e-03,  3.006659000e-02,  2.000000030e-01,  1.460000000e-01,  2.000000000e-01, /*DIAM*/  5.871121302e-01,  5.945178325e-01,  5.737238512e-01, /*EDIAM*/  5.543349579e-02,  3.530645295e-02,  4.530627309e-02, /*DMEAN*/  5.867547926e-01, /*EDMEAN*/  2.502073411e-02, /*CHI2*/  1.757857021e-01 },
            {474, /* SP "K2IIIb         " idx */ 208, /*MAG*/  3.797000000e+00,  2.630000000e+00,  1.540000000e+00,  8.310000000e-01,  2.890000000e-01,  1.500000000e-01, /*EMAG*/  1.529706000e-02,  3.000000000e-03,  2.022375000e-02,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/  6.643085600e-01,  6.722376780e-01,  6.742712980e-01, /*EDIAM*/  5.543376170e-02,  4.831095443e-02,  4.530631186e-02, /*DMEAN*/  6.709573463e-01, /*EDMEAN*/  2.842478742e-02, /*CHI2*/  1.693731553e-01 },
            {475, /* SP "K2III          " idx */ 208, /*MAG*/  5.371000000e+00,  4.140000000e+00,  2.970000000e+00,  2.104000000e+00,  1.488000000e+00,  1.349000000e+00, /*EMAG*/  2.236068000e-03,  2.000000000e-03,  3.006659000e-02,  2.000000030e-01,  1.600000000e-01,  1.880000000e-01, /*DIAM*/  4.279067707e-01,  4.452649120e-01,  4.426435573e-01, /*EDIAM*/  5.543349579e-02,  3.867649228e-02,  4.259418316e-02, /*DMEAN*/  4.406756366e-01, /*EDMEAN*/  2.639007799e-02, /*CHI2*/  1.528559225e+00 },
            {476, /* SP "G8III-IV       " idx */ 192, /*MAG*/  3.640000000e+00,  2.730000000e+00,  1.890000000e+00,  1.081000000e+00,  5.840000000e-01,  5.790000000e-01, /*EMAG*/  2.236068000e-03,  2.000000000e-03,  3.006659000e-02,  2.000000030e-01,  1.520000000e-01,  2.000000030e-01, /*DIAM*/  5.675702566e-01,  5.824206480e-01,  5.578432897e-01, /*EDIAM*/  5.541761655e-02,  3.673700041e-02,  4.529685780e-02, /*DMEAN*/  5.716040746e-01, /*EDMEAN*/  2.542809718e-02, /*CHI2*/  2.836097549e-01 },
            {477, /* SP "G7.5IIIb       " idx */ 190, /*MAG*/  4.396000000e+00,  3.480000000e+00,  2.590000000e+00,  1.768000000e+00,  1.366000000e+00,  1.333000000e+00, /*EMAG*/  7.280110000e-03,  2.000000000e-03,  3.006659000e-02,  2.000000030e-01,  1.920000000e-01,  2.000000030e-01, /*DIAM*/  4.307762586e-01,  4.223890868e-01,  4.044438589e-01, /*EDIAM*/  5.541704516e-02,  4.637281693e-02,  4.529616812e-02, /*DMEAN*/  4.176808623e-01, /*EDMEAN*/  2.805320934e-02, /*CHI2*/  8.509574717e-02 },
            {478, /* SP "K2III          " idx */ 208, /*MAG*/  3.928000000e+00,  2.760000000e+00,  1.660000000e+00,  1.184000000e+00,  4.670000000e-01,  4.370000000e-01, /*EMAG*/  2.209072000e-02,  2.000000000e-03,  2.009975000e-02,  2.000000030e-01,  1.700000000e-01,  1.800000000e-01, /*DIAM*/  5.765853444e-01,  6.346579109e-01,  6.127457496e-01, /*EDIAM*/  5.543349579e-02,  4.108440656e-02,  4.078636262e-02, /*DMEAN*/  6.135494683e-01, /*EDMEAN*/  2.570215016e-02, /*CHI2*/  1.511272506e+00 },
            {479, /* SP "G9III          " idx */ 196, /*MAG*/  4.307000000e+00,  3.320000000e+00,  2.370000000e+00,  1.732000000e+00,  1.289000000e+00,  1.225000000e+00, /*EMAG*/  3.512834000e-02,  3.000000000e-03,  2.022375000e-02,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/  4.413786433e-01,  4.416912193e-01,  4.323666795e-01, /*EDIAM*/  5.541991247e-02,  4.830118289e-02,  4.529849757e-02, /*DMEAN*/  4.379486305e-01, /*EDMEAN*/  2.903007169e-02, /*CHI2*/  2.657842168e-01 },
            {480, /* SP "K1.5III_Fe-1   " idx */ 206, /*MAG*/  5.999000000e+00,  4.820000000e+00,  3.660000000e+00,  2.949000000e+00,  2.197000000e+00,  2.085000000e+00, /*EMAG*/  1.315295000e-02,  2.000000000e-03,  3.006659000e-02,  2.000000030e-01,  1.960000000e-01,  2.000000030e-01, /*DIAM*/  2.418949529e-01,  2.990706175e-01,  2.910878048e-01, /*EDIAM*/  5.543016616e-02,  4.734428071e-02,  4.530441161e-02, /*DMEAN*/  2.812009492e-01, /*EDMEAN*/  2.828235549e-02, /*CHI2*/  1.379362421e+00 },
            {481, /* SP "K1III          " idx */ 204, /*MAG*/  5.099000000e+00,  4.020000000e+00,  2.940000000e+00,  2.245000000e+00,  1.756000000e+00,  1.636000000e+00, /*EMAG*/  3.805260000e-02,  2.000000000e-03,  2.009975000e-02,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/  3.709185436e-01,  3.693481702e-01,  3.688054735e-01, /*EDIAM*/  5.542732264e-02,  4.830582952e-02,  4.530285402e-02, /*DMEAN*/  3.695468289e-01, /*EDMEAN*/  2.846055026e-02, /*CHI2*/  5.042667289e-01 },
            {482, /* SP "K2III          " idx */ 208, /*MAG*/  6.156000000e+00,  5.000000000e+00,  3.880000000e+00,  2.886000000e+00,  2.392000000e+00,  2.145000000e+00, /*EMAG*/  3.605551000e-03,  2.000000000e-03,  2.000000000e-03,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/  2.774960542e-01,  2.626501233e-01,  2.851253068e-01, /*EDIAM*/  5.543349579e-02,  4.831086640e-02,  4.530627376e-02, /*DMEAN*/  2.753657818e-01, /*EDMEAN*/  2.852604206e-02, /*CHI2*/  9.069692371e-01 },
            {483, /* SP "G9III          " idx */ 196, /*MAG*/  4.750000000e+00,  3.800000000e+00,  2.950000000e+00,  2.319000000e+00,  1.823000000e+00,  1.757000000e+00, /*EMAG*/  9.219544000e-03,  2.000000000e-03,  3.006659000e-02,  2.000000030e-01,  1.740000000e-01,  1.860000000e-01, /*DIAM*/  3.157625700e-01,  3.326639803e-01,  3.246002545e-01, /*EDIAM*/  5.541964649e-02,  4.203649048e-02,  4.213380760e-02, /*DMEAN*/  3.257603480e-01, /*EDMEAN*/  2.632052919e-02, /*CHI2*/  2.328533435e-01 },
            {484, /* SP "M0III          " idx */ 240, /*MAG*/  5.942000000e+00,  4.440000000e+00,  2.760000000e+00,  1.714000000e+00,  8.780000000e-01,  7.110000000e-01, /*EMAG*/  4.472136000e-03,  2.000000000e-03,  2.009975000e-02,  2.000000030e-01,  1.700000000e-01,  2.000000030e-01, /*DIAM*/  5.968684103e-01,  6.543418236e-01,  6.395958083e-01, /*EDIAM*/  5.549528622e-02,  4.116615276e-02,  4.534942143e-02, /*DMEAN*/  6.359149709e-01, /*EDMEAN*/  2.678079872e-02, /*CHI2*/  6.000637255e-01 },
            {485, /* SP "K1III          " idx */ 204, /*MAG*/  5.658000000e+00,  4.550000000e+00,  3.480000000e+00,  2.462000000e+00,  2.002000000e+00,  1.722000000e+00, /*EMAG*/  4.472136000e-03,  2.000000000e-03,  3.006659000e-02,  2.000000030e-01,  1.520000000e-01,  2.000000030e-01, /*DIAM*/  3.515524718e-01,  3.318617883e-01,  3.632726267e-01, /*EDIAM*/  5.542732264e-02,  3.674396779e-02,  4.530285402e-02, /*DMEAN*/  3.458357393e-01, /*EDMEAN*/  2.550385206e-02, /*CHI2*/  2.634093736e+00 },
            {486, /* SP "G8.5IIIb       " idx */ 194, /*MAG*/  5.435000000e+00,  4.420000000e+00,  3.390000000e+00,  2.567000000e+00,  2.003000000e+00,  1.880000000e+00, /*EMAG*/  5.385165000e-03,  2.000000000e-03,  3.006659000e-02,  2.000000030e-01,  1.700000000e-01,  2.000000030e-01, /*DIAM*/  2.903481478e-01,  3.122411611e-01,  3.104301552e-01, /*EDIAM*/  5.541848730e-02,  4.107243263e-02,  4.529762273e-02, /*DMEAN*/  3.065444463e-01, /*EDMEAN*/  2.748615799e-02, /*CHI2*/  5.319582827e-02 },
            {487, /* SP "M1III          " idx */ 244, /*MAG*/  6.099000000e+00,  4.540000000e+00,  2.750000000e+00,  1.584000000e+00,  7.730000000e-01,  6.080000000e-01, /*EMAG*/  3.313608000e-02,  3.000000000e-03,  2.022375000e-02,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/  6.376097342e-01,  6.872800677e-01,  6.705755962e-01, /*EDIAM*/  5.546706016e-02,  4.835608323e-02,  4.533098440e-02, /*DMEAN*/  6.676979576e-01, /*EDMEAN*/  2.846419256e-02, /*CHI2*/  3.506533084e-01 },
            {488, /* SP "G9III:         " idx */ 196, /*MAG*/  4.616000000e+00,  3.700000000e+00,  2.730000000e+00,  2.021000000e+00,  1.486000000e+00,  1.393000000e+00, /*EMAG*/  2.296670000e-02,  1.739165000e-02,  2.650414000e-02,  2.000000030e-01,  2.000000030e-01,  2.000000030e-01, /*DIAM*/  3.905661425e-01,  4.098390787e-01,  4.043374820e-01, /*EDIAM*/  5.543552117e-02,  4.830635059e-02,  4.530073424e-02, /*DMEAN*/  4.026282672e-01, /*EDMEAN*/  2.846896997e-02, /*CHI2*/  4.397144758e-02 },
        };

        logInfo("check %d diameters:", NS);


        /* 3 diameters are required: */
        static const mcsUINT32 nbRequiredDiameters = 3;

        static const mcsDOUBLE DELTA_TH = 1e-7; /* check within error bar instead ? */
        static const mcsDOUBLE DELTA_SIG = 0.1; /* 1 sigma */

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

        mcsDOUBLE delta, diam, err, chi2;
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

            if (alxComputeAngularDiameters   ("(SP)   ", mags, spTypeIndex, diameters, diametersCov) == mcsFAILURE)
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
                                              &maxResidualsDiam, &chi2Diam, &maxCorrelations, &nbDiameters, NULL) == mcsFAILURE)
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

                if (0)
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

                    /* CHI2_MD != CHI2_SCL (no input diam) !*/
                    delta = fabs(datas[i][IDL_COL_CHI2] - chi2Diam.value);

                    if (delta > DELTA_TH)
                    {
                        logInfo("chi2 : %.9lf %.9lf", i, datas[i][IDL_COL_CHI2], chi2Diam.value);
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
