/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * @file
 * Function definition for angular diameter computation.
 *
 * @sa JMMC-MEM-2600-0009 document.
 * 
 * Directives:
 * CORRECT_FROM_MODELLING_NOISE: use diameter error correction (disabled)
 * USE_DIAMETER_CORRELATION: to use correlation matrix between angular diameters (disabled until correlations are computed correctly)
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

/** number of sigma to consider a diameter as an outlier (5 sigma) */
static mcsDOUBLE ERRANCE = 5.0;

/** number of sigma to consider a diameter to be consistent (1 sigma) */
static mcsDOUBLE TOLERANCE = 1.0;


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
 * Local Functions declaration
 */
static alxPOLYNOMIAL_ANGULAR_DIAMETER *alxGetPolynomialForAngularDiameter(void);
static alxPOLYNOMIAL_ANGULAR_DIAMETER_CORRELATION *alxGetPolynomialForAngularDiameterCorrelation(mcsLOGICAL use_identity);

const char* alxGetDiamLabel(const alxDIAM diam);

/* 
 * Local functions definition
 */

/**
 * Calculate ponderated mean.
 *
 * @param nbValues size of table.
 * @param vector input data.
 * @param sigma2 ponderation values (variance).
 *
 * @return the average value.
 */
mcsDOUBLE alxWeightedMean(int nbValues, mcsDOUBLE *vector, mcsDOUBLE *sigma2)
{
    mcsDOUBLE ponderation = 0.0;
    mcsDOUBLE avg = 0.0;
    int i;

    for (i = 0; i < nbValues; i++)
    {
        if (!isnan(vector[i]) && (sigma2[i] > 0.0))
        {
            avg += vector[i] / sigma2[i];
            ponderation += 1.0 / sigma2[i];
        }
    }
    if (ponderation > 0.0)
    {
        avg /= ponderation;
    }

    return avg;
}

/**
 * Calculate the residual rms.
 *
 * @param nbValues size of vector.
 * @param vector input data.
 * @param sigma2 ponderation values.
 * @return the value of rms (square root of variance).
 */
mcsDOUBLE alxWeightedRms(int nbValues, mcsDOUBLE *vector, mcsDOUBLE *sigma2)
{
    mcsDOUBLE ponderation = 0.0;
    mcsDOUBLE avg = 0.0;
    mcsDOUBLE rms = 0.0;
    int i;

    avg = alxWeightedMean(nbValues, vector, sigma2);

    for (i = 0; i < nbValues; i++)
    {
        if (!isnan(vector[i]) && (sigma2[i] > 0.0))
        {
            rms += (vector[i] - avg) * (vector[i] - avg) / sigma2[i];
            ponderation += 1.0 / sigma2[i];
        }
    }
    if (ponderation > 0.0)
    {
        rms /= ponderation;
        rms = sqrt(rms);
    }
    return rms;
}

/**
 * Calculate the residual rms wrt a special value (e.g., not the mean).
 *
 * @param nbValues size of vector.
 * @param vector input data.
 * @param sigma2 ponderation values.
 * @param specialValue the value wrt which the rms is computed
 * @return the value of rms (square root of variance).
 */
mcsDOUBLE alxWeightedRmsDistance(int nbValues, mcsDOUBLE *vector, mcsDOUBLE *sigma2, mcsDOUBLE specialValue)
{
    mcsDOUBLE ponderation = 0.0;
    mcsDOUBLE rms = 0.0;
    int i;

    for (i = 0; i < nbValues; i++)
    {
        if (!isnan(vector[i]) && (sigma2[i] > 0.0))
        {
            rms += (vector[i] - specialValue) * (vector[i] - specialValue) / sigma2[i];
            ponderation += 1.0 / sigma2[i];
        }
    }
    if (ponderation > 0.0)
    {
        rms /= ponderation;
        rms = sqrt(rms);
    }
    return rms;
}

/**
 * Calculate mean.
 *
 * @param nbValues size of vector.
 * @param vector input data.
 * @return the average value.
 */
mcsDOUBLE alxMean(int nbValues, mcsDOUBLE *vector)
{
    mcsDOUBLE ponderation = 0.0;
    mcsDOUBLE avg = 0.0;
    int i;

    for (i = 0 ; i < nbValues; i++)
    {
        if (!isnan(vector[i])) /*nan-protected*/
        {
            avg += vector[i];
            ponderation += 1.0;
        }
    }
    if (ponderation > 0.0)
    {
        avg /= ponderation;
    }

    return avg;
}

/**
 * Calculate the residual rms.
 *
 * @param nbValues size of vector.
 * @param vector input data.
 * @return the value of rms.
 */
mcsDOUBLE alxRms(int nbValues, mcsDOUBLE *vector)
{
    mcsDOUBLE ponderation = 0.0;
    mcsDOUBLE avg = 0.0;
    mcsDOUBLE rms = 0.0;
    int i;

    avg = alxMean(nbValues, vector);

    for (i = 0; i < nbValues; i++)
    {
        if (!isnan(vector[i])) /*nan-protected*/
        {
            rms += (vector[i] - avg) * (vector[i] - avg);
            ponderation += 1.0;
        }
    }
    if (ponderation > 0.0)
    {
        rms /= ponderation;
        rms = sqrt(rms);
    }
    return rms;
}

/**
 * Calculate the residual rms wrt a special value (not the mean).
 *
 * @param nbValues size of vector.
 * @param vector input data.
 * @param specialValue the value wrt which the rms is computed
 * @return the value of rms.
 */
mcsDOUBLE alxRmsDistance(int nbValues, mcsDOUBLE *vector, mcsDOUBLE specialValue)
{
    mcsDOUBLE ponderation = 0.0;
    mcsDOUBLE rms = 0.0;
    int i;

    for (i = 0; i < nbValues; i++)
    {
        if (!isnan(vector[i])) /*nan-protected*/
        {
            rms += (vector[i] - specialValue) * (vector[i] - specialValue);
            ponderation += 1.0;
        }
    }
    if (ponderation > 0.0)
    {
        rms /= ponderation;
        rms = sqrt(rms);
    }
    return rms;
}

/**
 * Return median value from given x[n] values
 * @param n number of elements in x array
 * @param x x array
 * @return median value 
 */
mcsDOUBLE alxMedian(int n, mcsDOUBLE x[])
{
    int i, j, mid;
    mcsDOUBLE tmp;

    /* the following two loops sort the array x in ascending order */
    for (i = 0; i < n - 1; i++)
    {
        for (j = i + 1; j < n; j++)
        {
            if (x[j] < x[i])
            {
                /* swap elements */
                tmp = x[i];
                x[i] = x[j];
                x[j] = tmp;
            }
        }
    }

    mid = n / 2;

    if (n % 2 == 0)
    {
        /* if there is an even number of elements, return mean of the two elements in the middle */
        tmp = 0.5 * (x[mid] + x[mid - 1]);
    }
    else
    {
        /* else return the element in the middle */
        tmp = x[mid];
    }

    return tmp;
}

mcsDOUBLE alxTotal(int n, mcsDOUBLE x[])
{
    mcsDOUBLE total = 0.0;
    int i;

    for (i = 0; i < n; i++)
    {
        if (!isnan(x[i])) /*nan-protected*/
        {
            total += x[i];
        }
    }
    return total;
}

int Lower_Triangular_Inverse(double *L, int n)
{
    int i, j, k;
    double *p_i, *p_j, *p_k;
    double sum;

    /*         Invert the diagonal elements of the lower triangular matrix L. */

    for (k = 0, p_k = L; k < n; p_k += (n + 1), k++)
    {
        if (*p_k == 0.0)
        {
            return -1;
        }
        *p_k = 1.0 / *p_k;
    }

    /*         Invert the remaining lower triangular matrix L row by row. */

    for (i = 1, p_i = L + n; i < n; i++, p_i += n)
    {
        for (j = 0, p_j = L; j < i; p_j += n, j++)
        {
            sum = 0.0;
            for (k = j, p_k = p_j; k < i; k++, p_k += n)
            {
                sum += *(p_i + k) * *(p_k + j);
            }
            *(p_i + j) = - *(p_i + i) * sum;
        }
    }

    return 0;
}

int Choleski_LU_Inverse(double *LU, int n)
{
    int i, j, k;
    double *p_i, *p_j, *p_k;
    double sum;

    if (Lower_Triangular_Inverse(LU, n) != 0)
    {
        return -1;
    }

    /*         Premultiply L inverse by the transpose of L inverse.      */

    for (i = 0, p_i = LU; i < n; i++, p_i += n)
    {
        for (j = 0, p_j = LU; j <= i; j++, p_j += n)
        {
            sum = 0.0;
            for (k = i, p_k = p_i; k < n; k++, p_k += n)
            {
                sum += *(p_k + i) * *(p_k + j);
            }
            *(p_i + j) = sum;
            *(p_j + i) = sum;
        }
    }

    return 0;
}

int Choleski_LU_Decomposition(double *A, int n)
{
    int i, k, p;
    double *p_Lk0;                   /* pointer to L[k][0] */
    double *p_Lkp;                   /* pointer to L[k][p] */
    double *p_Lkk;                   /* pointer to diagonal element on row k.*/
    double *p_Li0;                   /* pointer to L[i][0]*/
    double reciprocal;

    for (k = 0, p_Lk0 = A; k < n; p_Lk0 += n, k++)
    {

        /*            Update pointer to row k diagonal element.   */

        p_Lkk = p_Lk0 + k;

        /*            Calculate the difference of the diagonal element in row k  */
        /*            from the sum of squares of elements row k from column 0 to */
        /*            column k-1.                                                */

        for (p = 0, p_Lkp = p_Lk0; p < k; p_Lkp += 1,  p++)
        {
            *p_Lkk -= *p_Lkp * *p_Lkp;
        }

        /*            If diagonal element is not positive, return the error code, */
        /*            the matrix is not positive definite symmetric.              */

        if (*p_Lkk <= 0.0)
        {
            return -1;
        }

        /*            Otherwise take the square root of the diagonal element. */

        *p_Lkk = sqrt(*p_Lkk);
        reciprocal = 1.0 / *p_Lkk;

        /*            For rows i = k+1 to n-1, column k, calculate the difference   */
        /*            between the i,k th element and the inner product of the first */
        /*            k-1 columns of row i and row k, then divide the difference by */
        /*            the diagonal element in row k.                                */
        /*            Store the transposed element in the upper triangular matrix.  */

        p_Li0 = p_Lk0 + n;
        for (i = k + 1; i < n; p_Li0 += n, i++)
        {
            for (p = 0; p < k; p++)
            {
                *(p_Li0 + k) -= *(p_Li0 + p) * *(p_Lk0 + p);
            }
            *(p_Li0 + k) *= reciprocal;
            *(p_Lk0 + i) = *(p_Li0 + k);
        }
    }
    return 0;
}

/**
 * Invert squared matrix by LU decomposition. 
 *
 * This function inverts a squared (dim,dim) matrix and writes the result in
 * place.
 *
 * @param matrix matrix to invert
 * @param dim dimension of the matrix
 *
 * @return
 * Always return mcsSUCCESS
 */
mcsCOMPL_STAT alxInvertMatrix(double *matrix, int dim)
{
    FAIL_COND(Choleski_LU_Decomposition(matrix, dim) != 0);
    FAIL_COND(Choleski_LU_Inverse(matrix, dim) != 0);
    return mcsSUCCESS;
}

/*////////////////////////////////////////////////////////////////////////////*/
/*  void Multiply_Matrices(double *C, double *A, int nrows, int ncols,        */
/*                                                    double *B, int mcols)   */
/*                                                                            */
/*  Description:                                                              */
/*     Post multiply the nrows x ncols matrix A by the ncols x mcols matrix   */
/*     B to form the nrows x mcols matrix C, i.e. C = A B.                    */
/*     The matrix C should be declared as double C[nrows][mcols] in the       */
/*     calling routine.  The memory allocated to C should not include any     */
/*     memory allocated to A or B.                                            */
/*                                                                            */
/*  Arguments:                                                                */
/*     double *C    Pointer to the first element of the matrix C.             */
/*     double *A    Pointer to the first element of the matrix A.             */
/*     int    nrows The number of rows of the matrices A and C.               */
/*     int    ncols The number of columns of the matrices A and the           */
/*                   number of rows of the matrix B.                          */
/*     double *B    Pointer to the first element of the matrix B.             */
/*     int    mcols The number of columns of the matrices B and C.            */
/*                                                                            */
/*  Return Values:                                                            */
/*     void                                                                   */
/*                                                                            */
/*  Example:                                                                  */
/*     #define N                                                              */
/*     #define M                                                              */
/*     #define NB                                                             */
/*     double A[M][N],  B[N][NB], C[M][NB];                                   */
/*                                                                            */
/*     (your code to initialize the matrices A and B)                         */
/*                                                                            */
/*     Multiply_Matrices(&C[0][0], &A[0][0], M, N, &B[0][0], NB);             */
/*     printf("The matrix C is \n"); ...                                      */

/*////////////////////////////////////////////////////////////////////////////*/
void alxProductMatrix(double *A, double *B, double *C, int nrows,
                      int ncols, int mcols)
{
    double *pA = A;
    double *pB;
    double *p_B;
    int i, j, k;

    for (i = 0; i < nrows; A += ncols, i++)
    {
        for (p_B = B, j = 0; j < mcols; C++, p_B++, j++)
        {
            pB = p_B;
            pA = A;
            *C = 0.0;
            for (k = 0; k < ncols; pA++, pB += mcols, k++)
            {
                *C += *pA * *pB;
            }
        }
    }
}

/**
 * Compute the pow values x^n where n < max
 * @param max max integer power
 * @param x input value
 * @param pows pow array
 */
static void alxComputePow(mcsUINT32 max, mcsDOUBLE x, mcsDOUBLE* pows)
{
    mcsUINT32 i;
    mcsDOUBLE xn = 1.0;

    /* iterative algorithm */
    for (i = 0; i < max; i++)
    {
        pows[i] = xn;
        xn *= x;
    }
}

/**
 * Compute the polynomial value y = p(x) = coeffs[0] + x * coeffs[1] + x^2 * coeffs[2] ... + x^n * coeffs[n]
 * @param len coefficient array length
 * @param coeffs coefficient array
 * @param pows x^n array where n > len - 1
 * @return polynomial value
 */
static mcsDOUBLE alxComputePolynomial(mcsUINT32 len, mcsDOUBLE* coeffs, mcsDOUBLE* pows)
{
    mcsUINT32 i;
    mcsDOUBLE p  = 0.0;

    /* iterative algorithm */
    for (i = 0; i < len; i++)
    {
        p += coeffs[i] * pows[i];
    }
    return p;
}

/**
 * Return the polynomial coefficients for angular diameter computation and their covariance matrix
 *
 * @return pointer onto the structure containing polynomial coefficients, or
 * NULL if an error occured.
 *
 * @usedfiles alxAngDiamPolynomial.cfg : file containing the polynomial coefficients to compute the angular diameters.
 * @usedfiles alxAngDiamPolynomialCovariance.cfg : file containing the covariance between polynomial coefficients to compute the angular diameter error.
 */
static alxPOLYNOMIAL_ANGULAR_DIAMETER *alxGetPolynomialForAngularDiameter(void)
{
    /*
     * Check wether the polynomial structure in which polynomial coefficients
     * will be stored to compute angular diameter is loaded into memory or not,
     * and load it if necessary.
     */
    static alxPOLYNOMIAL_ANGULAR_DIAMETER polynomial = {mcsFALSE, "alxAngDiamPolynomial.cfg", "alxAngDiamPolynomialCovariance.cfg"};
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
             * #Color	a0	a1	a2	ecorr	DomainMin	DomainMax
             */
            if (sscanf(line, "%4s %lf %lf %lf %lf %lf %lf",
                       color,
                       &polynomial.coeff[lineNum][0],
                       &polynomial.coeff[lineNum][1],
                       &polynomial.coeff[lineNum][2],
                       &polynomial.polynomCoefFormalError[lineNum],
                       &polynomial.domainMin[lineNum],
                       &polynomial.domainMax[lineNum]
                       ) != (1 + alxNB_POLYNOMIAL_COEFF_DIAMETER + 3))
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
             * Read Covariance matrix coefficients [3 x 3]
             * #Color	a00	a01	a02	a10	a11	a12	a20	a21	a22
             */
            if (sscanf(line, "%4s %lf %lf %lf %lf %lf %lf %lf %lf %lf",
                       color,
                       &polynomial.polynomCoefCovMatrix[lineNum][0][0],
                       &polynomial.polynomCoefCovMatrix[lineNum][0][1],
                       &polynomial.polynomCoefCovMatrix[lineNum][0][2],
                       &polynomial.polynomCoefCovMatrix[lineNum][1][0],
                       &polynomial.polynomCoefCovMatrix[lineNum][1][1],
                       &polynomial.polynomCoefCovMatrix[lineNum][1][2],
                       &polynomial.polynomCoefCovMatrix[lineNum][2][0],
                       &polynomial.polynomCoefCovMatrix[lineNum][2][1],
                       &polynomial.polynomCoefCovMatrix[lineNum][2][2]
                       ) != (1 + alxNB_POLYNOMIAL_COEFF_DIAMETER * alxNB_POLYNOMIAL_COEFF_DIAMETER))
            {
                miscDynBufDestroy(&dynBuf);
                errAdd(alxERR_WRONG_FILE_FORMAT, line, fileName);
                free(fileName);
                return NULL;
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

    /* Destroy the dynamic buffer where is stored the file information */
    miscDynBufDestroy(&dynBuf);

    free(fileName);

    /* Check if there is missing line */
    if (lineNum != alxNB_DIAMS)
    {
        errAdd(alxERR_MISSING_LINE, lineNum, alxNB_DIAMS, fileName);
        return NULL;
    }

    /* Specify that the polynomial has been loaded */
    polynomial.loaded = mcsTRUE;

    return &polynomial;
}

/**
 * Return the correlation matrix of angular diameters 
 *
 * @return pointer onto the structure containing polynomial coefficients, or
 * NULL if an error occured.
 *
 * @usedfiles alxAngDiamPolynomialCorrelation.cfg : file containing the correlation matrix of the polynomial diameter computations. 
 * It is a [alxNB_DIAMS][alxNB_DIAMS] matrix
 */
static alxPOLYNOMIAL_ANGULAR_DIAMETER_CORRELATION *alxGetPolynomialForAngularDiameterCorrelation(mcsLOGICAL useIdentity)
{
    int i, j;
    /*
     * Check whether the structure is loaded into memory or not,
     * and load it if necessary.
     */
    static alxPOLYNOMIAL_ANGULAR_DIAMETER_CORRELATION correlation = {mcsFALSE, "alxAngDiamPolynomialCorrelation.cfg"};
    if (isTrue(correlation.loaded))
    {
        return &correlation;
    }

    /* if we want a fake unity matrix, make it and exit */
    if (isTrue(useIdentity))
    {
        /* fill 0 and 1 --- with a double loop. pity! */
        for (i = 0; i < alxNB_DIAMS; ++i)
        {
            for (j = 0; j < alxNB_DIAMS; ++j)
            {
                correlation.correlationMatrix[i][j] = 0.0;
            }
        }
        for (i = 0; i < alxNB_DIAMS; ++i)
        {
            correlation.correlationMatrix[i][i] = 1.0;
        }

        /* Specify that the polynomial has been loaded */
        correlation.loaded = mcsTRUE;

        return &correlation;
    }

    /* 
     * Build the dynamic buffer which will contain the coefficient file for angular diameter computation
     */
    /* Find the location of the file */
    char* fileName;
    fileName = miscLocateFile(correlation.fileName);
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
             * #Color a0...an (alxNB_DIAMS values)
             */
            if (sscanf(line, "%4s %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf", color,
                       &correlation.correlationMatrix[lineNum][0],
                       &correlation.correlationMatrix[lineNum][1],
                       &correlation.correlationMatrix[lineNum][2],
                       &correlation.correlationMatrix[lineNum][3],
                       &correlation.correlationMatrix[lineNum][4],
                       &correlation.correlationMatrix[lineNum][5],
                       &correlation.correlationMatrix[lineNum][6],
                       &correlation.correlationMatrix[lineNum][7],
                       &correlation.correlationMatrix[lineNum][8],
                       &correlation.correlationMatrix[lineNum][9],
                       &correlation.correlationMatrix[lineNum][10],
                       &correlation.correlationMatrix[lineNum][11],
                       &correlation.correlationMatrix[lineNum][12],
                       &correlation.correlationMatrix[lineNum][13],
                       &correlation.correlationMatrix[lineNum][14],
                       &correlation.correlationMatrix[lineNum][15]
                       ) != (1 + alxNB_DIAMS))
            {
                errAdd(alxERR_WRONG_FILE_FORMAT, line, fileName);
                miscDynBufDestroy(&dynBuf);
                free(fileName);
                return NULL;
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

    /* Destroy the dynamic buffer where is stored the file information */
    miscDynBufDestroy(&dynBuf);
    free(fileName);

    /* Check if there is missing line */
    if (lineNum != alxNB_DIAMS)
    {
        miscDynBufDestroy(&dynBuf);
        errAdd(alxERR_MISSING_LINE, lineNum, alxNB_DIAMS, fileName);
        return NULL;
    }

    /* Specify that the polynomial has been loaded */
    correlation.loaded = mcsTRUE;

    return &correlation;
}

/**
 * Compute am angular diameters for a given color-index based
 * on the coefficients from table. If a magnitude is not set,
 * the diameter is not computed.
 *
 * @param mA is the first input magnitude of the color index
 * @param mB is the second input magnitude of the color index
 * @param polynomial coeficients for angular diameter
 * @param band is the line corresponding to the color index A-B
 * @param diam is the structure to get back the computation 
 */
void alxComputeDiameter(alxDATA mA,
                        alxDATA mB,
                        alxPOLYNOMIAL_ANGULAR_DIAMETER *polynomial,
                        mcsINT32 band,
                        alxDATA *diam)
{
    mcsDOUBLE a_b;

    a_b = mA.value - mB.value;

    /* update effective polynom domains */
    if (isTrue(alxDOMAIN_LOG) && alxIsDevFlag())
    {
        if (a_b < effectiveDomainMin[band])
        {
            effectiveDomainMin[band] = a_b;
        }
        if (a_b > effectiveDomainMax[band])
        {
            effectiveDomainMax[band] = a_b;
        }
    }

    /* initialize the confidence */
    diam->confIndex = alxCONFIDENCE_HIGH;

    /* check if band is enabled */
    if (polynomial->domainMin[band] > polynomial->domainMax[band])
    {
        alxDATAClear((*diam));
        return;
    }

    /* Always check the polynom's domain */
    if ((a_b < polynomial->domainMin[band]) || (a_b > polynomial->domainMax[band]))
    {
        /* return no diameter */
        if (doLog(logTEST))
        {
            logTest("Color %s out of validity domain: %lf < %lf < %lf",
                    alxGetDiamLabel(band), polynomial->domainMin[band], a_b, polynomial->domainMax[band]);
        }
        alxDATAClear((*diam));
        return;
    }

    diam->isSet = mcsTRUE;

    /* 
     * Alain Chelli's new diameter and error computation (09/2013)
     */

    int i, j;
    int nbCoeffs = polynomial->nbCoeff[band];
    mcsDOUBLE *coeffs;
    VEC_COEFF_DIAMETER *polynomCoefCovMatrix;
    coeffs = polynomial->coeff[band];
    polynomCoefCovMatrix = polynomial->polynomCoefCovMatrix[band];

    /* optimize pow(a_b, n) calls by precomputing values [0; n] */
    int nPowLen = (nbCoeffs - 1) * (nbCoeffs - 1) + 1;
    mcsDOUBLE pows[nPowLen];

    alxComputePow(nPowLen, a_b, pows); /*pows is the serie of (M1-M2)^[0..n]*/


    /* Compute the angular diameter */
    /* FOR II=0, DEG1 DO DIAMC1=DIAMC1+COEFS(II)*(M1-M2)^II */
    mcsDOUBLE p_a_b = alxComputePolynomial(nbCoeffs, coeffs, pows);

    /* DIAMC1=DIAMC1-0.2*M1 & DIAMC1=9.306*10.^DIAMC1 ; output diameter */
    diam->value = 9.306 * pow(10.0, p_a_b - 0.2 * mA.value);


    /* Compute error */
    mcsDOUBLE varMa = mA.error * mA.error; /* EM1^2 */
    mcsDOUBLE varMb = mB.error * mB.error; /* EM2^2 */
    mcsDOUBLE sumVarMag = varMa + varMb;  /* EM1^2+EM2^2 */

    mcsDOUBLE cov = 0.0;
    /*
     * E_D^2/D^2 = [A] + 0.04 EM1^2 -0.4 * COV
     * where A= A1+A2;
     *       A1 = sum_ij(corpol_ij*(M1-M2)^(i+j)) --> err1
     *       A2 = sum_ij(coef[i]*coef[j]*i*j*(M1-M2)^(i+j-2))*[EM1^2+EM2^2]  ( with i>1, j>1) --> err2*sumVarMag
     * and COV = sum_i(coeff[i]*i*(M1-M2)^(i-1) EM1^2 ( with i>1) --> cov * varMa
     * i.e., err1+err2*sumVarMag + (0.04 -0.4 cov)*varMa
     */
    /*FOR II=0, DEG1 DO COV=COV-0.2*COEFS(II)*II*(M1-M2)^(II-1)*EM1^2 */

    for (i = 1; i < nbCoeffs; i++)
    {
        cov += coeffs[i] * i * pows[i - 1];
    }

    /* FOR II=0, DEG1 DO BEGIN  FOR JJ=0, DEG1 DO BEGIN
    EDIAMC1 += MAT1(II,JJ) * (M1-M2)^(II+JJ) + COEFS(II) * COEFS(JJ) * II * JJ * (M1-M2)^(II+JJ-2) *(EM1^2+EM2^2) */

    mcsDOUBLE err1 = 0.0, err2 = 0.0;
    /*A1*/
    for (i = 0; i < nbCoeffs; i++)
    {
        for (j = 0; j < nbCoeffs; j++)
        {
            err1 += polynomCoefCovMatrix[i][j] * pows[i + j];
        }
    }
    /*A2*/
    for (i = 1; i < nbCoeffs; i++)
    {
        for (j = 1; j < nbCoeffs; j++)
        {
            err2 += coeffs[i] * coeffs[j] * i * j * pows[i + j - 2];
        }
    }

    /* EDIAMC1=EDIAMC1+2.*COV+0.04*EM1^2 & EDIAMC1=DIAMC1*SQRT(EDIAMC1)*ALOG(10.) ; error of output diameter */
    mcsDOUBLE err = sqrt(err1 + err2 * sumVarMag + (0.04 - 0.4 * cov) * varMa) * log(10.0);
    diam->error = diam->value * err;

    /* below: should not be used if full covariance matrix is in use when computing weighted mean diameters. */
#ifdef CORRECT_FROM_MODELLING_NOISE
    /* Errors corrected with modelling noise */
    /* EDIAMC1_COR=SQRT(EDIAMC1(B)^2+PP2(2)^2*DIAM1(B)^2) */
    mcsDOUBLE errCorr = polynomial->polynomCoefFormalError[band];
    diam->error = sqrt(diam->error * diam->error + (errCorr * errCorr * diam->value * diam->value) ); /*ok*/
#endif

    if (doLog(logDEBUG))
    {
        logDebug("Diameter %s = %.3lf(%.3lf %.1lf%%) for magA=%.3lf(%.3lf) magB=%.3lf(%.3lf)",
                 alxGetDiamLabel(band),
                 diam->value, diam->error, alxDATARelError(*diam),
                 mA.value, mA.error, mB.value, mB.error
                 );
    }
}

mcsCOMPL_STAT alxComputeDiameterWithMagErr(alxDATA mA,
                                           alxDATA mB,
                                           alxPOLYNOMIAL_ANGULAR_DIAMETER *polynomial,
                                           mcsINT32 band,
                                           alxDATA *diam)
{
    /* If any magnitude is not available, then the diameter is not computed. */
    SUCCESS_COND_DO(alxIsNotSet(mA) || alxIsNotSet(mB),
                    alxDATAClear((*diam)));

    /** check domain and compute diameter and its error */
    alxComputeDiameter(mA, mB, polynomial, band, diam);

    /* If diameter is not computed (domain check), return */
    SUCCESS_COND(isFalse(diam->isSet));

    /* Set confidence as the smallest confidence of the two magnitudes */
    diam->confIndex = (mA.confIndex <= mB.confIndex) ? mA.confIndex : mB.confIndex;

    /* If any missing magnitude error (should not happen as error = 0.1 mag if missing error),
     * return diameter with low confidence */
    if ((mA.error == 0.0) || (mB.error == 0.0))
    {
        diam->confIndex = alxCONFIDENCE_LOW;
    }

    return mcsSUCCESS;
}

/*
 * Public functions definition
 */

/**
 * Compute stellar angular diameters from its photometric properties.
 * 
 * @param magnitudes B V R Ic Jc Hc Kc L M (Johnson / Cousin CIT)
 * @param diameters the structure to give back all the computed diameters
 *  
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is
 * returned.
 */
mcsCOMPL_STAT alxComputeAngularDiameters(const char* msg,
                                         alxMAGNITUDES magnitudes,
                                         alxDIAMETERS diameters)
{
    /* Get polynomial for diameter computation */
    alxPOLYNOMIAL_ANGULAR_DIAMETER *polynomial;
    polynomial = alxGetPolynomialForAngularDiameter();
    FAIL_NULL(polynomial);

    /* Compute diameters for all bands */

    alxComputeDiameterWithMagErr(magnitudes[alxB_BAND], magnitudes[alxV_BAND], polynomial, alxB_V_DIAM, &diameters[alxB_V_DIAM]);
    alxComputeDiameterWithMagErr(magnitudes[alxB_BAND], magnitudes[alxI_BAND], polynomial, alxB_I_DIAM, &diameters[alxB_I_DIAM]);
    alxComputeDiameterWithMagErr(magnitudes[alxB_BAND], magnitudes[alxJ_BAND], polynomial, alxB_J_DIAM, &diameters[alxB_J_DIAM]);
    alxComputeDiameterWithMagErr(magnitudes[alxB_BAND], magnitudes[alxH_BAND], polynomial, alxB_H_DIAM, &diameters[alxB_H_DIAM]);
    alxComputeDiameterWithMagErr(magnitudes[alxB_BAND], magnitudes[alxK_BAND], polynomial, alxB_K_DIAM, &diameters[alxB_K_DIAM]);

    alxComputeDiameterWithMagErr(magnitudes[alxV_BAND], magnitudes[alxR_BAND], polynomial, alxV_R_DIAM, &diameters[alxV_R_DIAM]);
    alxComputeDiameterWithMagErr(magnitudes[alxV_BAND], magnitudes[alxI_BAND], polynomial, alxV_I_DIAM, &diameters[alxV_I_DIAM]);
    alxComputeDiameterWithMagErr(magnitudes[alxV_BAND], magnitudes[alxJ_BAND], polynomial, alxV_J_DIAM, &diameters[alxV_J_DIAM]);
    alxComputeDiameterWithMagErr(magnitudes[alxV_BAND], magnitudes[alxH_BAND], polynomial, alxV_H_DIAM, &diameters[alxV_H_DIAM]);
    alxComputeDiameterWithMagErr(magnitudes[alxV_BAND], magnitudes[alxK_BAND], polynomial, alxV_K_DIAM, &diameters[alxV_K_DIAM]);

    alxComputeDiameterWithMagErr(magnitudes[alxI_BAND], magnitudes[alxJ_BAND], polynomial, alxI_J_DIAM, &diameters[alxI_J_DIAM]);
    alxComputeDiameterWithMagErr(magnitudes[alxI_BAND], magnitudes[alxH_BAND], polynomial, alxI_H_DIAM, &diameters[alxI_H_DIAM]);
    alxComputeDiameterWithMagErr(magnitudes[alxI_BAND], magnitudes[alxK_BAND], polynomial, alxI_K_DIAM, &diameters[alxI_K_DIAM]);

    alxComputeDiameterWithMagErr(magnitudes[alxJ_BAND], magnitudes[alxH_BAND], polynomial, alxJ_H_DIAM, &diameters[alxJ_H_DIAM]);
    alxComputeDiameterWithMagErr(magnitudes[alxJ_BAND], magnitudes[alxK_BAND], polynomial, alxJ_K_DIAM, &diameters[alxJ_K_DIAM]);

    alxComputeDiameterWithMagErr(magnitudes[alxH_BAND], magnitudes[alxK_BAND], polynomial, alxH_K_DIAM, &diameters[alxH_K_DIAM]);

    /* Display results */
    alxLogTestAngularDiameters(msg, diameters);

    return mcsSUCCESS;
}

mcsCOMPL_STAT alxComputeMeanAngularDiameter(alxDIAMETERS diameters,
                                            alxDATA     *meanDiam,
                                            alxDATA     *weightedMeanDiam,
                                            alxDATA     *medianDiam,
                                            alxDATA     *stddevDiam,
                                            mcsUINT32   *nbDiameters,
                                            mcsUINT32    nbRequiredDiameters,
                                            miscDYN_BUF *diamInfo)
{

    /* initialize structures */
    alxDATAClear((*meanDiam));
    alxDATAClear((*weightedMeanDiam));
    alxDATAClear((*medianDiam));
    alxDATAClear((*stddevDiam));

    /*
     * Only use diameters with HIGH confidence 
     * ie computed from catalog magnitudes and not interpolated magnitudes.
     */

    mcsUINT32 band, i, j;
    alxDATA   diameter;
    mcsUINT32 nDiameters = 0;
    mcsDOUBLE dist = 0.0;
    mcsLOGICAL inconsistent = mcsFALSE;
    mcsSTRING32 tmp;
    /* valid diameters (high confidence) to compute median diameter */
    mcsDOUBLE validDiams[alxNB_DIAMS];
    mcsDOUBLE validDiamsVariance[alxNB_DIAMS];
    mcsDOUBLE validDiamsError[alxNB_DIAMS];
    mcsINT32  validDiamsIndex[alxNB_DIAMS];
    mcsDOUBLE icov[alxNB_DIAMS * alxNB_DIAMS];
    mcsDOUBLE matrixprod[alxNB_DIAMS];

    /* Get diameter correlations for weighted mean diameter computation */
    alxPOLYNOMIAL_ANGULAR_DIAMETER_CORRELATION *correlation;
#ifdef USE_DIAMETER_CORRELATION
    correlation = alxGetPolynomialForAngularDiameterCorrelation(mcsFALSE);
#else
    correlation = alxGetPolynomialForAngularDiameterCorrelation(mcsTRUE);
#endif
    FAIL_NULL(correlation);

    /* Count valid diameters */
    for (band = 0; band < alxNB_DIAMS; band++)
    {
        diameter = diameters[band];

        /* Note: high confidence means diameter computed from catalog magnitudes */
        if (alxIsSet(diameter) && (diameter.confIndex == alxCONFIDENCE_HIGH) && (diameter.error > 0.0))
        {
            nDiameters++;
        }
    }

    /* update diameter count */
    *nbDiameters = nDiameters;

    /* if less than required diameters, can not compute mean diameter... */
    if (nDiameters < nbRequiredDiameters)
    {
        logTest("Cannot compute mean diameter (%d < %d valid diameters)", nDiameters, nbRequiredDiameters);

        /* Set diameter flag information */
        sprintf(tmp, "REQUIRED_DIAMETERS (%1d): %1d", nbRequiredDiameters, nDiameters);
        miscDynBufAppendString(diamInfo, tmp);

        return mcsSUCCESS;
    }

#if 0   /* For the time being we want all statistics */
    /*
     * LBO: 04/07/2013: if more than 3 diameters, discard H-K diameter 
     * as it provides poor quality diameters / accuracy 
     */
    if ((nDiameters > 3) && alxIsSet(diameters[alxH_K_DIAM]))
    {
        diameters[alxH_K_DIAM].confIndex = alxCONFIDENCE_MEDIUM;
    }
#endif

    /* count diameters again */
    nDiameters = 0;

    /* compute statistics */
    for (band = 0; band < alxNB_DIAMS; band++)
    {
        diameter = diameters[band];

        /* Populate diam and diamVariance Vectors */

        /* Note: high confidence means diameter computed from catalog magnitudes. We reject diameters with
         * negative or null errors beforehand, although this is taken into account in the alxMean() functions */
        if (alxIsSet(diameter) && (diameter.confIndex == alxCONFIDENCE_HIGH) && (diameter.error > 0.0) )
        {
            validDiamsIndex[nDiameters] = band;
            validDiams[nDiameters] = diameter.value;
            validDiamsVariance[nDiameters] = diameter.error * diameter.error;
            validDiamsError[nDiameters] = diameter.error;
            nDiameters++;
        }
    }
    /* final diameter count */
    *nbDiameters = nDiameters;

    /* Note: initialize to high confidence as only high confidence diameters are used */
    medianDiam->isSet = mcsTRUE;
    medianDiam->confIndex = alxCONFIDENCE_HIGH;
    medianDiam->value = alxMedian(nDiameters, validDiams);
    medianDiam->error = alxRmsDistance(nDiameters, validDiams, medianDiam->value);


    /* eliminate all measurements N sigmas from median value, then recount */

    /* count diameters again */
    nDiameters = 0;

    /* compute statistics */
    for (band = 0; band < alxNB_DIAMS; band++)
    {
        diameter = diameters[band];
        if (alxIsSet(diameter) && (diameter.confIndex == alxCONFIDENCE_HIGH) && (diameter.error > 0.0) &&
            (fabs(diameter.value - medianDiam->value) < ERRANCE * medianDiam->error))
        {
            validDiamsIndex[nDiameters] = band;
            validDiams[nDiameters] = diameter.value;
            validDiamsVariance[nDiameters] = diameter.error * diameter.error;
            validDiamsError[nDiameters] = diameter.error;
            nDiameters++;
        }
    }
    /* final diameter count */
    *nbDiameters = nDiameters;
    /* if less than required diameters, can not compute mean diameter... */
    if (nDiameters < nbRequiredDiameters)
    {
        logTest("Cannot compute mean diameter (%d < %d valid diameters)", nDiameters, nbRequiredDiameters);

        /* Set diameter flag information */
        sprintf(tmp, "REQUIRED_DIAMETERS (%1d): %1d", nbRequiredDiameters, nDiameters);
        miscDynBufAppendString(diamInfo, tmp);

        return mcsSUCCESS;
    }

#ifdef CORRECT_FROM_MODELLING_NOISE
    /* Compute statistical values of median, mean, weighted mean and their rms */
    /* Note: initialize to high confidence as only high confidence diameters are used */
    weightedMeanDiam->isSet = mcsTRUE;
    weightedMeanDiam->confIndex = alxCONFIDENCE_HIGH;
    weightedMeanDiam->value = alxWeightedMean(nDiameters, validDiams, validDiamsVariance);
    weightedMeanDiam->error = alxWeightedRms(nDiameters, validDiams, validDiamsVariance);
#else
    /* populate icov matrix[nDiameters x nDiameters] with covariance matrix and actual errors */
    for (i = 0; i < nDiameters; ++i)
    {
        for (j = 0; j < nDiameters; ++j)
        {
            icov[i * nDiameters + j] = correlation->correlationMatrix[validDiamsIndex[i]][validDiamsIndex[j]] * validDiamsError[i] * validDiamsError[j];
        }
    }
    /* invert cov => the real icov */
    /* LBO: what to do if matrix inversion fails ??? */
    SUCCESS_DO(alxInvertMatrix(icov, nDiameters), logError("Cannot invert covariance matrix (%d diameters)", nDiameters));

    /* compute icov#diameters */
    alxProductMatrix(icov, validDiams, matrixprod, nDiameters, nDiameters, 1);

    /* Compute statistical values of median, mean, weighted mean and their rms */
    /* Note: initialize to high confidence as only high confidence diameters are used */
    weightedMeanDiam->isSet = mcsTRUE;
    weightedMeanDiam->confIndex = alxCONFIDENCE_HIGH;
    mcsDOUBLE totalicov = alxTotal(nDiameters * nDiameters, icov);
    weightedMeanDiam->value = alxTotal(nDiameters, matrixprod) / totalicov;

    /*corresponding standard deviation method 1. gives Nans if total is negative */
    /* divagation by GD ... forget!
        mcsDOUBLE validDiamsDeviation[alxNB_DIAMS];
        for (i=0; i< nDiameters; ++i)
        {
            validDiamsDeviation[i] = validDiams[i]-weightedMeanDiam->value;
            validDiamsDeviation[i] *= validDiamsDeviation[i];
        }
        alxProductMatrix(icov,validDiamsDeviation,matrixprod,nDiameters,nDiameters,1);
        weightedMeanDiam->error = sqrt(alxTotal(nDiameters,matrixprod)/totalicov);
     */

    /*corresponding standard deviation method 2 */
    weightedMeanDiam->error = 1.0 / sqrt(totalicov); /* (initial formula by Alain --- gives almost too good errors!) */
#endif

    /* Note: initialize to high confidence as only high confidence diameters are used */
    meanDiam->isSet = mcsTRUE;
    meanDiam->confIndex = alxCONFIDENCE_HIGH;
    meanDiam->value = alxMean(nDiameters, validDiams);
    meanDiam->error = alxRms(nDiameters, validDiams);

    /* stddev of all diameters wrt. weighted Mean value*/
    stddevDiam->isSet = mcsTRUE;
    stddevDiam->confIndex = weightedMeanDiam->confIndex;
    stddevDiam->value = alxWeightedRmsDistance(nDiameters, validDiams, validDiamsVariance, weightedMeanDiam->value);

    /* FOLLOWING IS TO BE MODIFIED AFTER CONCLUSIONS ON OBSERVED STATISTICS */
    /* 
     Check consistency between weighted mean diameter and individual
     diameters. If inconsistency is found, the
     weighted mean diameter has a LOW confidence.
     */
    for (band = 0; band < alxNB_DIAMS; band++)
    {
        diameter = diameters[band];

        /* Note: high confidence means diameter computed from catalog magnitudes */
        if (alxIsSet(diameter) && (diameter.confIndex == alxCONFIDENCE_HIGH) && (diameter.error > 0.0))
        {
            dist = fabs(diameter.value - weightedMeanDiam->value);

            if ((dist > TOLERANCE * sqrt(weightedMeanDiam->error * weightedMeanDiam->error +
                                         diameter.error * diameter.error)))
            {
                if (isFalse(inconsistent))
                {
                    inconsistent = mcsTRUE;

                    /* Set confidence to LOW */
                    weightedMeanDiam->confIndex = alxCONFIDENCE_LOW;

                    /* Set diameter flag information */
                    miscDynBufAppendString(diamInfo, "INCONSISTENT_DIAMETER ");
                }

                /* Set confidence to LOW for each inconsistent diameter */
                diameters[band].confIndex = alxCONFIDENCE_LOW;

                /* Append each color (distance relError%) in diameter flag information */
                sprintf(tmp, "%s (%.3lf %.1lf%%) ", alxGetDiamLabel(band), dist, 100.0 * dist / diameter.value);
                miscDynBufAppendString(diamInfo, tmp);
            }
        }
    }


    logTest("Diameter mean=%.3lf(%.3lf %.1lf%%) median=%.3lf(%.3lf %.1lf%%) stddev=(%.3lf %.1lf%%)"
            " weighted=%.3lf(%.3lf %.1lf%%) valid=%s [%s] from %d diameters",
            meanDiam->value, meanDiam->error, alxDATARelError(*meanDiam),
            medianDiam->value, medianDiam->error, alxDATARelError(*medianDiam),
            stddevDiam->value, 100.0 * stddevDiam->value / weightedMeanDiam->value,
            weightedMeanDiam->value, weightedMeanDiam->error, alxDATARelError(*weightedMeanDiam),
            (weightedMeanDiam->confIndex == alxCONFIDENCE_HIGH) ? "true" : "false",
            alxGetConfidenceIndex(weightedMeanDiam->confIndex), nDiameters);

    return mcsSUCCESS;
}

/**
 * Return the string literal representing the confidence index 
 * @return string literal "NO", "LOW", "MEDIUM" or "HIGH"
 */
const char* alxGetConfidenceIndex(const alxCONFIDENCE_INDEX confIndex)
{
    return alxCONFIDENCE_STR[confIndex];
}

/**
 * Return the string literal representing the diam
 * @return string literal
 */
const char* alxGetDiamLabel(const alxDIAM diam)
{
    return alxDIAM_STR[diam];
}

/**
 * Initialize this file
 */
void alxAngularDiameterInit(void)
{
    mcsUINT32 band;
    for (band = 0; band < alxNB_DIAMS; band++)
    {
        effectiveDomainMin[band] = +100.0;
        effectiveDomainMax[band] = -100.0;
    }

    alxGetPolynomialForAngularDiameter();

#ifdef USE_DIAMETER_CORRELATION
    alxGetPolynomialForAngularDiameterCorrelation(mcsFALSE);
#else
    alxGetPolynomialForAngularDiameterCorrelation(mcsTRUE);
#endif
}

/**
 * Log the effective diameter polynom domains
 */
void alxShowDiameterEffectiveDomains(void)
{
    if (isTrue(alxDOMAIN_LOG) && alxIsDevFlag())
    {
        logInfo("Effective domains for diameter polynoms:");

        mcsUINT32 band;
        for (band = 0; band < alxNB_DIAMS; band++)
        {
            logInfo("Color %s - validity domain: %lf < color < %lf",
                    alxGetDiamLabel(band), effectiveDomainMin[band], effectiveDomainMax[band]);
        }
    }
}
/*___oOo___*/
