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

/* enable/disable checks for high correlations => discard redundant color bands */
#define CHECK_HIGH_CORRELATION  mcsTRUE

/* enable/disable discarding redundant color bands (high correlation) */
#define FILTER_HIGH_CORRELATION mcsTRUE

/* enable/disable checks that the weighted mean diameter is within diameter range +/- 3 sigma => low confidence */
#define CHECK_MEAN_WITHIN_RANGE mcsFALSE

/* log Level to dump covariance matrix and its inverse */
#define LOG_MATRIX logTEST


#define absError(diameter, relDiameterError) \
   (relDiameterError * diameter * LOG_10) /* absolute error */

#define relError(diameter, absDiameterError) \
   (absDiameterError / (diameter * LOG_10)) /* relative error */


/* flag to enable finding effective polynom domains */
#define alxDOMAIN_LOG mcsTRUE

/* effective polynom domains */
/* disable in concurrent context (static vars) i.e. SOAP server */
static mcsDOUBLE effectiveDomainMin[alxNB_DIAMS];
static mcsDOUBLE effectiveDomainMax[alxNB_DIAMS];

/** alx dev flag */
static mcsLOGICAL alxDevFlag = mcsFALSE;

mcsLOGICAL alxGetDevFlag(void)
{
    return alxDevFlag;
}

void alxSetDevFlag(mcsLOGICAL flag)
{
    alxDevFlag = flag;
    logInfo("alxDevFlag: %s", isTrue(alxDevFlag) ? "true" : "false");
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
    static alxPOLYNOMIAL_ANGULAR_DIAMETER polynomial = {mcsFALSE, "alxAngDiamPolynomial.cfg", "alxAngDiamPolynomialCovariance.cfg"};

    /* Check if the structure is loaded into memory. If not load it. */
    if (isTrue(polynomial.loaded))
    {
        return &polynomial;
    }

    /*
     * Build the dynamic buffer which will contain the coefficient file for angular diameter computation
     */
    /* Find the location of the file */
    char* fileName;
    fileName = miscLocateFile(polynomial.fileName);
    if (isNull(fileName))
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

    while (isNotNull(pos = miscDynBufGetNextLine(&dynBuf, pos, line, sizeof (line), mcsTRUE)))
    {
        logTrace("miscDynBufGetNextLine()='%s'", line);

        /* If the current line is not empty */
        if (isFalse(miscIsSpaceStr(line)))
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
             * # Polynom coefficients (2) from idl fit on 108 stars.
             * #Color a0 a1 DomainMin DomainMax
             */
            if (sscanf(line, "%3s %lf %lf %lf %lf",
                       color,
                       &polynomial.coeff[lineNum][0],
                       &polynomial.coeff[lineNum][1],
                       &polynomial.domainMin[lineNum],
                       &polynomial.domainMax[lineNum]
                       ) != (1 + alxNB_POLYNOMIAL_COEFF_DIAMETER + 2))
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
    if (isNull(fileName))
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

    while (isNotNull(pos = miscDynBufGetNextLine(&dynBuf, pos, line, sizeof (line), mcsTRUE)))
    {
        logTrace("miscDynBufGetNextLine()='%s'", line);

        /* If the current line is not empty */
        if (isFalse(miscIsSpaceStr(line)))
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
             * Read the Covariance matrix of polynomial coefficients [a0ij...a1ij][a0ij...a1ij] (i < 10) (20 x 20)
             *
             * MCOV_POL matrix from IDL [20,20] for 10 colors and 2 polynomial coefficients (a0 a1)
             *
             */
            polynomCoefCovLine = polynomial.polynomCoefCovMatrix[lineNum];

            if (sscanf(line, "%d %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf",
                       &index,
                       &polynomCoefCovLine[ 0], &polynomCoefCovLine[ 1],
                       &polynomCoefCovLine[ 2], &polynomCoefCovLine[ 3],
                       &polynomCoefCovLine[ 4], &polynomCoefCovLine[ 5],
                       &polynomCoefCovLine[ 6], &polynomCoefCovLine[ 7],
                       &polynomCoefCovLine[ 8], &polynomCoefCovLine[ 9]
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
void alxComputeDiameter(alxDATA mA,
                        alxDATA mB,
                        mcsDOUBLE e_Av,
                        alxPOLYNOMIAL_ANGULAR_DIAMETER *polynomial,
                        alxEXTINCTION_RATIO_TABLE *extinctionRatioTable,
                        mcsUINT32 color,
                        alxDATA *diam)
{
    mcsDOUBLE a_b = mA.value - mB.value;

    /* update effective polynom domains */
    if (isTrue(alxDOMAIN_LOG) && alxIsDevFlag())
    {
        if (a_b < effectiveDomainMin[color])
        {
            effectiveDomainMin[color] = a_b;
        }
        if (a_b > effectiveDomainMax[color])
        {
            effectiveDomainMax[color] = a_b;
        }
    }

    /* initialize the confidence */
    diam->confIndex = alxCONFIDENCE_HIGH;

    /* check if the color relation is enabled */
    if (polynomial->domainMin[color] > polynomial->domainMax[color])
    {
        alxDATAClear((*diam));
        return;
    }

    /* check the polynom domain */
    if (isTrue(alxDIAM_CHECK_DOMAIN[color])
            && ((a_b < polynomial->domainMin[color]) || (a_b > polynomial->domainMax[color])))
    {
        /* return no diameter */
        logTest("Color %s out of validity domain: %lf < %lf < %lf",
                alxGetDiamLabel(color), polynomial->domainMin[color], a_b, polynomial->domainMax[color]);

        alxDATAClear((*diam));
        return;
    }

    diam->isSet = mcsTRUE;

    /*
     * Alain Chelli's new diameter and error computation (12/2013)
     */

    mcsUINT32 nbCoeffs    = polynomial->nbCoeff[color];
    mcsDOUBLE *diamCoeffs = polynomial->coeff[color];

    /* optimize pow(a_b, n) calls by pre-computing values [0; n] */
    mcsUINT32 nPowLen = nbCoeffs * (nbCoeffs - 1) + 1;

    mcsDOUBLE pows[nPowLen];
    alxComputePows(nPowLen, a_b, pows); /* pows is the serie of (M1-M2)^[0..n] */


    /* Compute the angular diameter */
    /*
     * A=MAG_C(II,NBI) & B=MAG_C(II,NBJ)
     * DIAM_C(II,*)=10.^(PAR(0:NBD-1)+PAR(NBD:2*NBD-1)*(A-B)-0.2*A); modeled diameter
     */
    mcsDOUBLE p_a_b = alxComputePolynomial(nbCoeffs, diamCoeffs, pows);
    diam->value     = alxPow10(p_a_b - 0.2 * mA.value);

    /* Compute diameter error */
    mcsUINT32 nI = alxDIAM_BAND_A[color]; /* NI */
    mcsUINT32 nJ = alxDIAM_BAND_B[color]; /* NJ */
    mcsDOUBLE *avCoeffs = extinctionRatioTable->coeff;

    /*
     * A=MAG_C(II,NBI) & B=MAG_C(II,NBJ)
     * E1=MCOV_POL(DC,DC) & E3=MCOV_POL(NBD+DC,NBD+DC) & E2=MCOV_POL(DC,NBD+DC)
     * EDIAM_C(II,*) =E1+2D*E2*(A-B)+E3*(A-B)^2
     * EDIAM_C(II,*)+=EAV_C(II)^2*(0.2D*CF(NBI)+PAR(NBD+DC)*(CF(NBJ)-CF(NBI)))^2
     * EDIAM_C(II,*)+=(PAR(NBD:NBD2m1)-0.2D)^2*EMAG_C(II,NBI)^2+PAR(NBD:NBD2m1)^2*EMAG_C(II,NBJ)^2
     * EDIAM_C(II,*)=DIAM_C(II,*)*SQRT(EDIAM_C(II,*))*LOG_10 ; error
     */
    diam->error =
            polynomial->polynomCoefCovMatrix[color][color]                                         /* E1=MCOV_POL(DC,DC) */
            + 2.0 * polynomial->polynomCoefCovMatrix[color][alxNB_DIAMS + color] * pows[1]         /* 2*E2*(A-B) & E2=MCOV_POL(DC,NBD+DC) */
            + polynomial->polynomCoefCovMatrix[alxNB_DIAMS + color][alxNB_DIAMS + color] * pows[2] /* E3*(A-B)^2 & E3=MCOV_POL(NBD+DC,NBD+DC) */
            + alxSquare(e_Av * (0.2 * avCoeffs[nI] + diamCoeffs[1] * (avCoeffs[nJ] - avCoeffs[nI]))) /* EAV_C(II)^2 * (0.2D * CF(NBI) + PAR(NBD + DC)*(CF(NBJ) - CF(NBI)))^2 */
            + alxSquare((diamCoeffs[1] - 0.2) * mA.error)                                          /* (PAR(NBD:NBD2m1)-0.2D)^2*EMAG_C(II,NBI)^2 */
            + alxSquare( diamCoeffs[1]        * mB.error)                                          /* PAR(NBD:NBD2m1)^2*EMAG_C(II,NBJ)^2  */
            ; /* relative variance (normal distribution) */

    /* Convert log normal diameter distribution to normal distribution */
    /* EDIAM_C(II,*)=DIAM_C(II,*)*SQRT(EDIAM_C(II,*))*ALOG(10) ; error of output diameter */
    diam->error = absError(diam->value, sqrt(diam->error));

    logDebug("Diameter %s = %.4lf(%.4lf %.1lf%%) for magA=%.3lf(%.3lf) magB=%.3lf(%.3lf)",
             alxGetDiamLabel(color),
             diam->value, diam->error, alxDATALogRelError(*diam),
             mA.value, mA.error, mB.value, mB.error
             );
}

mcsCOMPL_STAT alxComputeDiameterWithMagErr(alxDATA mA,
                                           alxDATA mB,
                                           mcsDOUBLE e_Av,
                                           alxPOLYNOMIAL_ANGULAR_DIAMETER *polynomial,
                                           alxEXTINCTION_RATIO_TABLE *extinctionRatioTable,
                                           mcsUINT32 color,
                                           alxDATA *diam)
{
    /* If any magnitude is not available, then the diameter is not computed. */
    SUCCESS_COND_DO(alxIsNotSet(mA) || alxIsNotSet(mB),
                    alxDATAClear((*diam)));

    /** check domain and compute diameter and its error */
    alxComputeDiameter(mA, mB, e_Av, polynomial, extinctionRatioTable, color, diam);

    /* If diameter is not computed (domain check), return */
    SUCCESS_COND(isFalse(diam->isSet));

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
 * @param magnitudes B V R Ic Jc Hc Kc L M (Johnson / Cousin CIT)
 * @param e_Av error on AV
 * @param diameters the structure to give back all the computed diameters
 * @param diametersCov the structure to give back all the covariance matrix of computed diameters
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
 */
mcsCOMPL_STAT alxComputeAngularDiameters(const char* msg,
                                         alxMAGNITUDES magnitudes,
                                         mcsDOUBLE e_Av,
                                         alxDIAMETERS diameters,
                                         alxDIAMETERS_COVARIANCE diametersCov)
{
    /* Get polynomial for diameter computation */
    alxPOLYNOMIAL_ANGULAR_DIAMETER *polynomial;
    polynomial = alxGetPolynomialForAngularDiameter();
    FAIL_NULL(polynomial);

    /* Get extinction ratio table */
    alxEXTINCTION_RATIO_TABLE *extinctionRatioTable;
    extinctionRatioTable = alxGetExtinctionRatioTable();
    FAIL_NULL(extinctionRatioTable);

    /* Compute diameters and the covariance matrix of computed diameters */
    mcsUINT32 i, j, nI, nJ, nK, nL;
    alxDATA mI, mJ, mK, mL;
    mcsDOUBLE mI_J, mK_L, varMI, varMJ, diamCov;

    mcsDOUBLE varAv = alxSquare(e_Av);
    const mcsDOUBLE *avCoeffs = extinctionRatioTable->coeff;

    for (i = 0; i < alxNB_DIAMS; i++) /* II */
    {
        /* NI=NBI(II) & MI=MAG_C(Z,NI) & EMI=EMAG_C(Z,NI) & NJ=NBJ(II) & MJ=MAG_C(Z,NJ) & EMJ=EMAG_C(Z,NJ) ; magnitudes & errors */
        nI = alxDIAM_BAND_A[i]; /* NI */
        nJ = alxDIAM_BAND_B[i]; /* NJ */

        mI = magnitudes[nI]; /* MI / EMI */
        mJ = magnitudes[nJ]; /* MJ / EMJ */

        alxComputeDiameterWithMagErr(mI, mJ, e_Av, polynomial, extinctionRatioTable, i, &diameters[i]);

        /* fill the covariance matrix */
        /*
         * Only use diameters with HIGH confidence
         * ie computed from catalog magnitudes and not interpolated magnitudes.
         */
        if (isDiameterValid(diameters[i]))
        {
            /*
            FOR II=0, NBDm1 DO BEGIN
              NI=NBI(II) & MI=MAG_C(Z,NI) & EMI=EMAG_C(Z,NI) & NJ=NBJ(II) & MJ=MAG_C(Z,NJ) & EMJ=EMAG_C(Z,NJ) ; magnitudes & errors
              FOR JJ=0, II DO BEGIN
                NK=NBI(JJ) & MK=MAG_C(Z,NK) & NL=NBJ(JJ) & ML=MAG_C(Z,NL)
                VA=MCOV_POL(II,JJ) & VB=MCOV_POL(NBD+II,NBD+JJ) & VAB=MCOV_POL(II,NBD+JJ) & VBA=MCOV_POL(JJ,NBD+II)
                ; initial value: fixed error term
                DCOV =VA+VBA*(MI-MJ)+VAB*(MK-ML)+VB*(MI-MJ)*(MK-ML)
                DCOV+=EAV_C(Z)^2*(0.2D*CF(NI)+PAR(NBD+II)*(CF(NJ)-CF(NI)))*(0.2D*CF(NK)+PAR(NBD+JJ)*(CF(NL)-CF(NK)))
                IF (NI EQ NK AND NJ EQ NL) THEN DCOV+=(PAR(NBD+II)-0.2D)^2*EMI^2+PAR(NBD+II)^2*EMJ^2
                IF (NI EQ NK AND NJ NE NL) THEN DCOV+=(PAR(NBD+II)-0.2D)*(PAR(NBD+JJ)-0.2D)*EMI^2
                IF (NI NE NK AND NJ EQ NL) THEN DCOV+= PAR(NBD+II)*PAR(NBD+JJ)*EMJ^2
                IF (NJ NE NK AND NI EQ NL) THEN DCOV+=-PAR(NBD+JJ)*(PAR(NBD+II)-0.2D)*EMI^2
                IF (NJ EQ NK AND NI NE NL) THEN DCOV+=-PAR(NBD+II)*(PAR(NBD+JJ)-0.2D)*EMJ^2
                IF (NJ EQ NK AND NI EQ NL) THEN DCOV+=-PAR(NBD+JJ)*(PAR(NBD+II)-0.2D)*EMI^2-PAR(NBD+II)*(PAR(NBD+JJ)-0.2D)*EMJ^2
                DCOV_OC(II,JJ,Z)=DCOV & DCOV_OC(JJ,II,*)=DCOV_OC(II,JJ,*) ; covariance matrix of computed diameters
              ENDFOR
            ENDFOR
             */

            mI_J  = (mI.value - mJ.value); /* (MI-MJ) */

            varMI = alxSquare(mI.error);   /* EMI^2 */
            varMJ = alxSquare(mJ.error);   /* EMJ^2 */

            for (j = 0; j <= i; j++) /* JJ */
            {
                /* NK=NBI(JJ) & NL=NBJ(JJ) */
                nK = alxDIAM_BAND_A[j]; /* NK */
                nL = alxDIAM_BAND_B[j]; /* NL */

                mK = magnitudes[nK]; /* MK */
                mL = magnitudes[nL]; /* ML */

                mK_L  = (mK.value - mL.value); /* (MK-ML) */

                /*
                 * VA=MCOV_POL(II,JJ) & VB=MCOV_POL(NBD+II,NBD+JJ) & VAB=MCOV_POL(II,NBD+JJ) & VBA=MCOV_POL(JJ,NBD+II)
                 * initial value: fixed error term
                 * DCOV =VA+VBA*(MI-MJ)+VAB*(MK-ML)+VB*(MI-MJ)*(MK-ML)
                 */
                diamCov = polynomial->polynomCoefCovMatrix[i][j]                          /* VA=MCOV_POL(II,JJ) */
                        + polynomial->polynomCoefCovMatrix[j][alxNB_DIAMS + i] * mI_J     /* VBA=MCOV_POL(JJ,NBD+II) * (MI-MJ) */
                        + polynomial->polynomCoefCovMatrix[i][alxNB_DIAMS + j] * mK_L     /* VAB=MCOV_POL(II,NBD+JJ) * (MK-ML) */
                        + polynomial->polynomCoefCovMatrix[alxNB_DIAMS + i][alxNB_DIAMS + j] * mI_J * mK_L; /* VB=MCOV_POL(NBD+II,NBD+JJ) * (MI-MJ) * (MK-ML) */

                /* DCOV+=EAV_C(Z)^2*(0.2D*CF(NI)+PAR(NBD+II)*(CF(NJ)-CF(NI)))*(0.2D*CF(NK)+PAR(NBD+JJ)*(CF(NL)-CF(NK))) */
                diamCov += varAv * (0.2 * avCoeffs[nI] + polynomial->coeff[i][1] * (avCoeffs[nJ] - avCoeffs[nI]))
                        * (0.2 * avCoeffs[nK] + polynomial->coeff[j][1] * (avCoeffs[nL] - avCoeffs[nK]));

                /*
                mcsDOUBLE diamCovAvMags;
                diamCovAvMags =  ( (polynomial->coeff[i][1] - 0.2) * covAvMags[nI] - polynomial->coeff[i][1] * covAvMags[nJ])
                 * (0.2 * avCoeffs[nK] + polynomial->coeff[j][1] * (avCoeffs[nL] - avCoeffs[nK]));

                diamCovAvMags += ( (polynomial->coeff[j][1] - 0.2) * covAvMags[nK] - polynomial->coeff[j][1] * covAvMags[nL])
                 * (0.2 * avCoeffs[nI] + polynomial->coeff[i][1] * (avCoeffs[nJ] - avCoeffs[nI]));

                diamCov += diamCovAvMags;
                 */

                if (nI == nK)
                {
                    if (nJ == nL)
                    {
                        /* IF (NI EQ NK AND NJ EQ NL) THEN DCOV_OB(II,JJ,*)=Q+(PAR(NBD+II)-0.2)^2*EMI^2+PAR(NBD+II)^2*EMJ^2 */
                        diamCov += alxSquare(polynomial->coeff[i][1] - 0.2) * varMI + alxSquare(polynomial->coeff[i][1]) * varMJ;
                    }
                    else
                    {
                        /* IF (NI EQ NK AND NJ NE NL) THEN DCOV_OB(II,JJ,*)=Q+(PAR(NBD+II)-0.2)*(PAR(NBD+JJ)-0.2)*EMI^2 */
                        diamCov += (polynomial->coeff[i][1] - 0.2) * (polynomial->coeff[j][1] - 0.2) * varMI;
                    }
                }
                else if (nJ == nL)
                {
                    /* IF (NI NE NK AND NJ EQ NL) THEN DCOV_OB(II,JJ,*)=Q+PAR(NBD+II)*PAR(NBD+JJ)*EMJ^2 */
                    diamCov += polynomial->coeff[i][1] * polynomial->coeff[j][1] * varMJ;
                }

                if (nI == nL)
                {
                    if (nJ != nK)
                    {
                        /* IF (NJ NE NK AND NI EQ NL) THEN DCOV_OB(II,JJ,*)=Q-PAR(NBD+JJ)*(PAR(NBD+II)-0.2)*EMI^2 */
                        diamCov -= polynomial->coeff[j][1] * (polynomial->coeff[i][1] - 0.2) * varMI;
                    }
                }
                else if (nJ == nK)
                {
                    /* IF (NJ EQ NK AND NI NE NL) THEN DCOV_OB(II,JJ,*)=Q-PAR(NBD+II)*(PAR(NBD+JJ)-0.2)*EMJ^2 */
                    diamCov  -= polynomial->coeff[i][1] * (polynomial->coeff[j][1] - 0.2) * varMJ;
                }

                /* DCOV_C(JJ,II,*)=DCOV_C(II,JJ,*) */
                diametersCov[j][i] = diametersCov[i][j] = diamCov;
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

        logP(LOG_MATRIX, "Diameter %s B-K=%.4lf(%.4lf %.4lf) "
             "V-J=%.4lf(%.4lf %.4lf) V-H=%.4lf(%.4lf %.4lf) V-K=%.4lf(%.4lf %.4lf) "
             "I-K=%.4lf(%.4lf %.4lf) ", msg,
             diameters[alxB_K_DIAM].value, diametersMin[alxB_K_DIAM], diametersMax[alxB_K_DIAM],
             diameters[alxV_J_DIAM].value, diametersMin[alxV_J_DIAM], diametersMax[alxV_J_DIAM],
             diameters[alxV_H_DIAM].value, diametersMin[alxV_H_DIAM], diametersMax[alxV_H_DIAM],
             diameters[alxV_K_DIAM].value, diametersMin[alxV_K_DIAM], diametersMax[alxV_K_DIAM],
             diameters[alxI_K_DIAM].value, diametersMin[alxI_K_DIAM], diametersMax[alxI_K_DIAM]
             );
    }

    return mcsSUCCESS;
}

mcsCOMPL_STAT alxComputeMeanAngularDiameter(alxDIAMETERS diameters,
                                            alxDIAMETERS_COVARIANCE diametersCov,
                                            alxDATA     *meanDiam,
                                            alxDATA     *weightedMeanDiam,
                                            alxDATA     *medianDiam,
                                            alxDATA     *stddevDiam,
                                            alxDATA     *maxResidualDiam,
                                            alxDATA     *chi2Diam,
                                            alxDATA     *maxCorrelation,
                                            mcsUINT32   *nbDiameters,
                                            mcsUINT32    nbRequiredDiameters,
                                            miscDYN_BUF *diamInfo)
{
    /*
     * Only use diameters with HIGH confidence
     * ie computed from catalog magnitudes and not interpolated magnitudes.
     */

    /* initialize structures */
    alxDATAClear((*meanDiam));
    alxDATAClear((*weightedMeanDiam));
    alxDATAClear((*medianDiam));
    alxDATAClear((*stddevDiam));
    alxDATAClear((*maxResidualDiam));
    alxDATAClear((*chi2Diam));
    alxDATAClear((*maxCorrelation));

    mcsUINT32   color;
    mcsLOGICAL  consistent = mcsTRUE;
    mcsSTRING32 tmp;
    alxDATA     diameter;
    mcsDOUBLE   diamRelError, min, max;

    /* valid diameters to compute median and weighted mean diameter and their errors */
    mcsUINT32 nValidDiameters;
    mcsDOUBLE validDiams[alxNB_DIAMS];
    mcsUINT32 validDiamsBand[alxNB_DIAMS];
    mcsDOUBLE validDiamsVariance[alxNB_DIAMS];
    mcsDOUBLE inverse[alxNB_DIAMS]; /* temporary */

    /* residual (diameter vs weighted mean diameter) */
    mcsDOUBLE residual;
    mcsDOUBLE residuals[alxNB_DIAMS];
    mcsDOUBLE maxResidual = 0.0;
    mcsDOUBLE chi2 = NAN;
    mcsDOUBLE diamMin =  INFINITY;
    mcsDOUBLE diamMax = -INFINITY;


    static const mcsDOUBLE nSigma = 3.0;

    /* check correlation in covariance matrix */
    /* threshold determined empirically with topcat: plot dmean vs (V-K)- Av
     * colored by 1.0/abs(1.0 - diam_max_correlation) ~ 480 */
    static const mcsDOUBLE THRESHOLD_CORRELATION = 0.997916667;

    /*
     * TODO: alain: correct directly (x2) the covariance matrix on polynoms => no correction on mean error.
     */


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


            /* min/max diameters using 3 sigma (log-normal) */
            min = alxPow10(validDiams[nValidDiameters] - nSigma * diamRelError);
            max = alxPow10(validDiams[nValidDiameters] + nSigma * diamRelError);

            /* min/max diameters +/- 2 sigma */
            if (diamMin > min)
            {
                diamMin = min;
            }
            if (diamMax < max)
            {
                diamMax = max;
            }

            nValidDiameters++;
        }
    }

    /* Set the diameter count */
    *nbDiameters = nValidDiameters;


    /* if less than required diameters, can not compute mean diameter... */
    if (nValidDiameters < nbRequiredDiameters)
    {
        logTest("Cannot compute mean diameter (%d < %d valid diameters)", nValidDiameters, nbRequiredDiameters);

        if (isNotNull(diamInfo))
        {
            /* Set diameter flag information */
            sprintf(tmp, "REQUIRED_DIAMETERS (%1d): %1d", nbRequiredDiameters, nValidDiameters);
            miscDynBufAppendString(diamInfo, tmp);
        }

        return mcsSUCCESS;
    }

    /*
     * Compute Median diameter and its error FOR INFORMATION ONLY
     * A=DIAM_C(II,DR(W)) & B=EDIAM_C(II,DR(W))/(A*ALOG(10.)) & D=1./SQRT(TOTAL(1./B^2)) & M=MEDIAN(ALOG10(A))
     */
    medianDiam->isSet = mcsTRUE;
    medianDiam->value = alxMedian(nValidDiameters, validDiams); /* M=MEDIAN(ALOG10(A)) */

    /*
     * Compute relative error on median diameter D=1./SQRT(TOTAL(1./B^2))
     * (weighted mean diameter error formula for statistically independent diameters)
     */
    medianDiam->error = 1.0 / sqrt(alxTotal(nValidDiameters, alxInvert(nValidDiameters, validDiamsVariance, inverse) ) );


    /* Compute correlations */
    mcsDOUBLE maxCor = 0.0; /* max(correlation coefficients) */
    mcsUINT32 nThCor = 0;
    mcsUINT32 i, j;

    if (isTrue(CHECK_HIGH_CORRELATION))
    {
        mcsDOUBLE maxCors[alxNB_DIAMS];

        alxDIAMETERS_COVARIANCE diamCorrelations;
        for (i = 0; i < alxNB_DIAMS; i++) /* II */
        {
            for (j = 0; j < alxNB_DIAMS; j++)
            {
                diamCorrelations[i][j] = (i < nValidDiameters) ? ( (j < nValidDiameters) ?
                        (diametersCov[i][j] / sqrt(diametersCov[i][i] * diametersCov[j][j])) : NAN ) : NAN;
            }

            /* check max(correlation) out of diagonal (1) */
            mcsDOUBLE max = 0.0;
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
            logP(LOG_MATRIX, "Max Correlation=%.6lf from [%15.4lf %15.4lf %15.4lf %15.4lf %15.4lf]",
                 maxCor, maxCors[0], maxCors[1], maxCors[2], maxCors[3], maxCors[4]
                 );
        }

        /* Set the maximum correlation */
        maxCorrelation->isSet     = mcsTRUE;
        maxCorrelation->confIndex = alxCONFIDENCE_HIGH;
        maxCorrelation->value     = maxCor;

        if (isTrue(FILTER_HIGH_CORRELATION) && (nThCor != 0))
        {
            /* at least 2 colors are too much correlated => eliminate redundant colors */

            logMatrix("Covariance",  diametersCov);
            logMatrix("Correlation", diamCorrelations);


            mcsUINT32 nBands = 0;
            mcsUINT32 uncorBands[alxNB_DIAMS];

            /** preferred bands when discarding correlated colors */
            static const mcsUINT32 preferedBands[alxNB_DIAMS] = { alxV_K_DIAM, alxB_K_DIAM, alxI_K_DIAM, alxV_H_DIAM, alxV_J_DIAM };

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
                        /* eliminate color */
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

            /* if less than required diameters, can not compute mean diameter... */
            if (nValidDiameters < nbRequiredDiameters)
            {
                logTest("Cannot compute mean diameter (%d < %d valid diameters)", nValidDiameters, nbRequiredDiameters);

                if (isNotNull(diamInfo))
                {
                    /* Set diameter flag information */
                    sprintf(tmp, "REQUIRED_DIAMETERS (%1d): %1d", nbRequiredDiameters, nValidDiameters);
                    miscDynBufAppendString(diamInfo, tmp);
                }

                return mcsSUCCESS;
            }

            /* Update the diameter count */
            *nbDiameters = nValidDiameters;
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
                        /* TODO: fix indices when some bands are discarded */
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

            /* corresponding standard deviation method 1. */
            /* divagation by GD ... forget!
                mcsDOUBLE validDiamsDeviation[alxNB_DIAMS];
                for (i=0; i< nDiameters; ++i)
                {
                    validDiamsDeviation[i] = validDiams[i] - weightedMeanDiam->value;
                    validDiamsDeviation[i] *= validDiamsDeviation[i];
                }
                alxProductMatrix(icov,validDiamsDeviation,matrixprod,nDiameters,nDiameters,1);
                weightedMeanDiam->error = sqrt(alxTotal(nDiameters,matrixprod)/totalicov);
             */

            /* EDMEAN_C(II)=1./SQRT(TOTAL(M)) */
            const mcsDOUBLE weightedMeanDiamVariance = 1.0 / total_icov;
            /* correct error: */
            weightedMeanDiam->error = sqrt(weightedMeanDiamVariance);


            /* compute chi2 on mean diameter estimates */

            /*
                DIFF=A-ALOG10(DMEAN_C(II)) & CHI2_DS(II)=(TRANSPOSE(DIFF)#ICOV#DIFF)/(NBDm1)
             */
            for (i = 0; i < nValidDiameters; i++)
            {
                /* DIFF=ALOG10(DIAM_C(II,*)) - ALOG10(DMEAN_C(II)) */
                residuals[i] = fabs(validDiams[i] - weightedMeanDiam->value);
            }

            /* compute M=ICOV#DIFF */
            alxProductMatrix(icov, residuals, matrix_prod, nValidDiameters, nValidDiameters, 1);

            /* compute CHI2_DS(II)=TRANSPOSE(DIFF)#M / NBD-1*/
            mcsDOUBLE matrix_11[1];
            alxProductMatrix(residuals, matrix_prod, matrix_11, 1, nValidDiameters, 1);
            chi2 = matrix_11[0] / (nValidDiameters - 1);


            /* Compute residuals and find the max residual (number of sigma) on all diameters */
            for (i = 0; i < nValidDiameters; i++)
            {
                /* RES_C(NBD*II+DC) = ALOG10(DIAM_C(II,*)/DMEAN_C(II)) (=DIFF)
                                    / SQRT( (EDIAM_C(II,*)/(DIAM_C(II,*)*LOG_10) )^2 + 1D/TOTAL_ICOV ); residual(DIAM_C, DMEAN_C) */
                residual = residuals[i] /= sqrt(validDiamsVariance[i] + weightedMeanDiamVariance);

                if (residual > maxResidual)
                {
                    maxResidual = residual;
                }
            }

            if (isNotNull(diamInfo) && (maxResidual > LOG_RESIDUAL_THRESHOLD))
            {
                /* Report high tolerance between weighted mean diameter and individual diameters in diameter flag information */
                for (i = 0; i < nValidDiameters; i++)
                {
                    residual = residuals[i];
                    color    = validDiamsBand[i];

                    if (residual > LOG_RESIDUAL_THRESHOLD)
                    {
                        if (isTrue(consistent))
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

            /* Check if max(residuals) < 10 or chi2 < 80
             * If higher i.e. inconsistency is found, the weighted mean diameter has a LOW confidence */
            if ((maxResidual > MAX_RESIDUAL_THRESHOLD) || (chi2 > DIAM_CHI2_THRESHOLD))
            {
                /* Set confidence to LOW */
                weightedMeanDiam->confIndex = alxCONFIDENCE_LOW;

                if (isNotNull(diamInfo))
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


    /* compute mean diameter and dispersion (stddev) on consistent diameters */
    meanDiam->isSet = mcsTRUE;
    meanDiam->confIndex = weightedMeanDiam->confIndex;
    meanDiam->value = alxMean(nValidDiameters, validDiams); /* mean( ALOG10(A) ) */
    meanDiam->error = alxRmsDistance(nValidDiameters, validDiams, meanDiam->value); /* stddev ( ALOG10(A) - M ) */

    if (alxIsSet(*weightedMeanDiam))
    {
        /* stddev of all diameters wrt. weighted Mean value */
        stddevDiam->isSet = mcsTRUE;
        stddevDiam->confIndex = weightedMeanDiam->confIndex;
        stddevDiam->value = alxWeightedRmsDistance(nValidDiameters, validDiams, validDiamsVariance, weightedMeanDiam->value);
    }

    /* Convert log normal diameter distributions to normal distributions */
    if (alxIsSet(*weightedMeanDiam))
    {
        weightedMeanDiam->value = alxPow10(weightedMeanDiam->value);
        weightedMeanDiam->error = absError(weightedMeanDiam->value, weightedMeanDiam->error);

        if (isTrue(CHECK_MEAN_WITHIN_RANGE))
        {
            /* ensure minDiam < weightedMeanDiam < maxDiam within n sigma (n ~ 2 or 3).
             * If not, fix = median or simple mean (increase error !) */

            /* redundant with consistency check to 5 sigma ??? */
            if ((weightedMeanDiam->value < diamMin) || (weightedMeanDiam->value) > diamMax)
            {
                logTest("weightedMeanDiam=%.4lf (%.4lf) out of range [%.4lf - %.4lf]",
                        weightedMeanDiam->value, weightedMeanDiam->error,
                        diamMin, diamMax);

                /* TODO: fix such computations; for now set confidence to MEDIUM (diamFlag=false) */
                if (weightedMeanDiam->confIndex == alxCONFIDENCE_HIGH)
                {
                    weightedMeanDiam->confIndex = alxCONFIDENCE_LOW;

                    /* Set diameter flag information */
                    miscDynBufAppendString(diamInfo, "MEAN_OUT_OF_RANGE ");
                }
            }
        }
    }

    meanDiam->value = alxPow10(meanDiam->value);
    meanDiam->error = absError(meanDiam->value, meanDiam->error);

    medianDiam->confIndex = weightedMeanDiam->confIndex;
    medianDiam->value = alxPow10(medianDiam->value);
    medianDiam->error = absError(medianDiam->value, medianDiam->error);

    if (alxIsSet(*weightedMeanDiam))
    {
        stddevDiam->value = absError(weightedMeanDiam->value, stddevDiam->value);
    }

    if (!isnan(chi2))
    {
        chi2Diam->isSet = mcsTRUE;
        chi2Diam->confIndex = alxCONFIDENCE_HIGH;
        chi2Diam->value = chi2;
    }

    logTest("Diameter mean=%.4lf(%.4lf %.1lf%%) median=%.4lf(%.4lf %.1lf%%) stddev=(%.4lf %.1lf%%)"
            " weighted=%.4lf(%.4lf %.1lf%%) valid=%s [%s] tolerance=%.2lf chi2=%.4lf from %d diameters: %s",
            meanDiam->value, meanDiam->error, alxDATALogRelError(*meanDiam),
            medianDiam->value, medianDiam->error, alxDATALogRelError(*medianDiam),
            stddevDiam->value, alxIsSet(*weightedMeanDiam) ? 100.0 * stddevDiam->value / (LOG_10 * weightedMeanDiam->value) : 0.0,
            weightedMeanDiam->value, weightedMeanDiam->error, alxDATALogRelError(*weightedMeanDiam),
            (weightedMeanDiam->confIndex == alxCONFIDENCE_HIGH) ? "true" : "false",
            alxGetConfidenceIndex(weightedMeanDiam->confIndex),
            maxResidual, chi2,
            nValidDiameters,
            isNotNull(diamInfo) ? miscDynBufGetBuffer(diamInfo) : "");

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
    miscDYN_BUF checkColors;
    miscDynBufInit(&checkColors);

    mcsUINT32 color;
    for (color = 0; color < alxNB_DIAMS; color++)
    {
        if (isTrue(alxDIAM_CHECK_DOMAIN[color]))
        {
            miscDynBufAppendString(&checkColors, alxGetDiamLabel(color));
            miscDynBufAppendString(&checkColors, " ");
        }
        effectiveDomainMin[color] = +100.0;
        effectiveDomainMax[color] = -100.0;
    }

    mcsUINT32 bufLen = 0;
    miscDynBufGetNbStoredBytes(&checkColors, &bufLen);
    if (bufLen != 0)
    {
        logInfo("Validity domain to check: %s", miscDynBufGetBuffer(&checkColors));
    }

    miscDynBufDestroy(&checkColors);

    alxGetPolynomialForAngularDiameter();
}

/**
 * Log the effective diameter polynom domains
 */
void alxShowDiameterEffectiveDomains(void)
{
    if (isTrue(alxDOMAIN_LOG) && alxIsDevFlag())
    {
        logInfo("Effective domains for diameter polynoms:");

        mcsUINT32 color;
        for (color = 0; color < alxNB_DIAMS; color++)
        {
            logInfo("Color %s - validity domain: %lf < color < %lf",
                    alxGetDiamLabel(color), effectiveDomainMin[color], effectiveDomainMax[color]);
        }
    }
}
/*___oOo___*/
