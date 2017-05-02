/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * @file
 * Definition and init of generic alx functions.
 */


/* Needed to preclude warnings on snprintf() */
#define  _BSD_SOURCE 1

/*
 * System Headers
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
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
 * Public functions definition
 */

/**
 * Return the string literal representing the band
 * @return string literal "B", "V" ... "M"
 */
const char* alxGetBandLabel(const alxBAND band)
{
    return alxBAND_STR[band];
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
 * Return the string literal representing the color index
 * @return string literal
 */
const char* alxGetDiamLabel(const alxDIAM color)
{
    return alxDIAM_STR[color];
}

/**
 * Return the string literal representing the star type
 * @return string literal "SUPER_GIANT", "SUB_SUPER_GIANT", "GIANT", "SUB_GIANT", "DWARF", "UNDEFINED"
 */
const char* alxGetStarTypeLabel(const alxSTAR_TYPE starType)
{
    return alxSTAR_TYPE_STR[starType];
}

/**
 * Return the string literal representing the star type
 * @return string literal "DWARF", "GIANT", "SUPER_GIANT"
 */
const char* alxGetTableStarTypeLabel(const alxTABLE_STAR_TYPE starType)
{
    return alxTABLE_STAR_TYPE_STR[starType];
}

void alxLogTestMagnitudes(const char* line, const char* msg, alxMAGNITUDES magnitudes)
{
    if (doLog(logTEST))
    {
        /* Print out results */
        logTest("%s %s B=%0.3lf(%0.3lf %s) V=%0.3lf(%0.3lf %s) "
                "R=%0.3lf(%0.3lf %s) I=%0.3lf(%0.3lf %s) J=%0.3lf(%0.3lf %s) H=%0.3lf(%0.3lf %s) "
                "K=%0.3lf(%0.3lf %s) L=%0.3lf(%0.3lf %s) M=%0.3lf(%0.3lf %s)",
                line, msg,
                magnitudes[alxB_BAND].value, magnitudes[alxB_BAND].error, alxGetConfidenceIndex(magnitudes[alxB_BAND].confIndex),
                magnitudes[alxV_BAND].value, magnitudes[alxV_BAND].error, alxGetConfidenceIndex(magnitudes[alxV_BAND].confIndex),
                magnitudes[alxR_BAND].value, magnitudes[alxR_BAND].error, alxGetConfidenceIndex(magnitudes[alxR_BAND].confIndex),
                magnitudes[alxI_BAND].value, magnitudes[alxI_BAND].error, alxGetConfidenceIndex(magnitudes[alxI_BAND].confIndex),
                magnitudes[alxJ_BAND].value, magnitudes[alxJ_BAND].error, alxGetConfidenceIndex(magnitudes[alxJ_BAND].confIndex),
                magnitudes[alxH_BAND].value, magnitudes[alxH_BAND].error, alxGetConfidenceIndex(magnitudes[alxH_BAND].confIndex),
                magnitudes[alxK_BAND].value, magnitudes[alxK_BAND].error, alxGetConfidenceIndex(magnitudes[alxK_BAND].confIndex),
                magnitudes[alxL_BAND].value, magnitudes[alxL_BAND].error, alxGetConfidenceIndex(magnitudes[alxL_BAND].confIndex),
                magnitudes[alxM_BAND].value, magnitudes[alxM_BAND].error, alxGetConfidenceIndex(magnitudes[alxM_BAND].confIndex));
    }
}

void alxLogAngularDiameters(const char* msg, alxDIAMETERS diameters, logLEVEL logLevel)
{
    if (doLog(logLevel))
    {
        logP(logLevel,
             "Diameter %s V-J=%.4lf(%.4lf %.1lf%%) V-H=%.4lf(%.4lf %.1lf%%) V-K=%.4lf(%.4lf %.1lf%%) ", msg,
             diameters[alxV_J_DIAM].value, diameters[alxV_J_DIAM].error, alxDATALogRelError(diameters[alxV_J_DIAM]),
             diameters[alxV_H_DIAM].value, diameters[alxV_H_DIAM].error, alxDATALogRelError(diameters[alxV_H_DIAM]),
             diameters[alxV_K_DIAM].value, diameters[alxV_K_DIAM].error, alxDATALogRelError(diameters[alxV_K_DIAM])
             );
    }
}

/* Mathematical Functions */

/**
 * Return the minimum of a and b values
 * @param a double value
 * @param b double value
 * @return minimum value
 */
mcsINT32 alxIntMin(mcsINT32 a, mcsINT32 b)
{
    return (a <= b) ? a : b;
}

/**
 * Return the maximum of a and b values
 * @param a double value
 * @param b double value
 * @return maximum value
 */
mcsINT32 alxIntMax(mcsINT32 a, mcsINT32 b)
{
    return (a >= b) ? a : b;
}

/**
 * Return the minimum of a and b values
 * @param a double value
 * @param b double value
 * @return minimum value
 */
mcsDOUBLE alxMin(mcsDOUBLE a, mcsDOUBLE b)
{
    return (a <= b) ? a : b;
}

/**
 * Return the maximum of a and b values
 * @param a double value
 * @param b double value
 * @return maximum value
 */
mcsDOUBLE alxMax(mcsDOUBLE a, mcsDOUBLE b)
{
    return (a >= b) ? a : b;
}

/**
 * Return the square of a ie a^2
 * @param a double value
 * @return a^2
 */
mcsDOUBLE alxSquare(mcsDOUBLE a)
{
    return a * a;
}

/**
 * Return the norm of a and b values ie sqrt( a * a + b * b)
 * @param a double value
 * @param b double value
 * @return norm value
 */
mcsDOUBLE alxNorm(mcsDOUBLE a, mcsDOUBLE b)
{
    return sqrt(a * a + b * b);
}

/**
 * Calculate ponderated mean.
 *
 * @param nbValues size of table.
 * @param vector input data.
 * @param sigma2 ponderation values (variance).
 *
 * @return the average value.
 */
mcsDOUBLE alxWeightedMean(mcsUINT32 nbValues, mcsDOUBLE *vector, mcsDOUBLE *sigma2)
{
    mcsDOUBLE ponderation = 0.0;
    mcsDOUBLE avg = 0.0;
    mcsUINT32 i;
    mcsDOUBLE weight;

    for (i = 0; i < nbValues; i++)
    {
        if (!isnan(vector[i]) && (sigma2[i] > 0.0))
        {
            weight = 1.0 / sigma2[i];
            avg += vector[i] * weight;
            ponderation += weight;
        }
    }
    if (ponderation > 0.0)
    {
        avg /= ponderation;
    }

    return avg;
}

/**
 * Calculate the residual rms wrt a special value (e.g. the weighted mean, median ...).
 *
 * @param nbValues size of vector.
 * @param vector input data.
 * @param sigma2 ponderation values.
 * @param specialValue the value wrt which the rms is computed
 * @return the value of rms (square root of variance).
 */
mcsDOUBLE alxWeightedRmsDistance(mcsUINT32 nbValues, mcsDOUBLE *vector, mcsDOUBLE *sigma2, mcsDOUBLE specialValue)
{
    mcsDOUBLE ponderation = 0.0;
    mcsDOUBLE rms = 0.0;
    mcsUINT32 i;
    mcsDOUBLE weight, dist;

    for (i = 0; i < nbValues; i++)
    {
        if (!isnan(vector[i]) && (sigma2[i] > 0.0))
        {
            dist = vector[i] - specialValue;
            weight = 1.0 / sigma2[i];
            rms += dist * dist * weight;
            ponderation += weight;
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
mcsDOUBLE alxMean(mcsUINT32 nbValues, mcsDOUBLE *vector)
{
    mcsDOUBLE ponderation = 0.0;
    mcsDOUBLE avg = 0.0;
    mcsUINT32 i;

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
 * Calculate the residual rms wrt a special value (not the mean).
 *
 * @param nbValues size of vector.
 * @param vector input data.
 * @param specialValue the value wrt which the rms is computed
 * @return the value of rms.
 */
mcsDOUBLE alxRmsDistance(mcsUINT32 nbValues, mcsDOUBLE *vector, mcsDOUBLE specialValue)
{
    mcsDOUBLE ponderation = 0.0;
    mcsDOUBLE rms = 0.0;
    mcsUINT32 i;
    mcsDOUBLE dist;

    for (i = 0; i < nbValues; i++)
    {
        if (!isnan(vector[i])) /*nan-protected*/
        {
            dist = vector[i] - specialValue;
            rms += dist * dist;
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
 * Return median value from given input values
 * @param n number of elements in the input array
 * @param in input array
 * @return median value
 */
mcsDOUBLE alxMedian(mcsUINT32 n, mcsDOUBLE in[])
{
    mcsUINT32 i, j, mid;
    mcsDOUBLE tmp;
    mcsDOUBLE x[n];

    /* copy values into local array to keep input array unchanged */
    for (i = 0; i < n; i++)
    {
        x[i] = in[i];
    }

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

mcsDOUBLE* alxInvert(mcsUINT32 n, mcsDOUBLE x[], mcsDOUBLE y[])
{
    mcsUINT32 i;

    for (i = 0; i < n; i++)
    {
        y[i] = (isnan(x[i])) ? NAN : (1.0 / x[i]); /*nan-protected*/
    }
    return y;
}

mcsDOUBLE alxTotal(mcsUINT32 n, mcsDOUBLE x[])
{
    mcsDOUBLE total = 0.0;
    mcsUINT32 i;

    for (i = 0; i < n; i++)
    {
        if (!isnan(x[i])) /*nan-protected*/
        {
            total += x[i];
        }
    }
    return total;
}

mcsCOMPL_STAT Lower_Triangular_Inverse(mcsDOUBLE *L, mcsUINT32 n)
{
    mcsUINT32 i, j, k;
    mcsDOUBLE *p_i, *p_j, *p_k;
    mcsDOUBLE sum;

    /*         Invert the diagonal elements of the lower triangular matrix L. */

    for (k = 0, p_k = L; k < n; p_k += (n + 1), k++)
    {
        if (*p_k == 0.0)
        {
            return mcsFAILURE;
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

    return mcsSUCCESS;
}

mcsCOMPL_STAT Choleski_LU_Inverse(mcsDOUBLE *LU, mcsUINT32 n)
{
    mcsUINT32 i, j, k;
    mcsDOUBLE *p_i, *p_j, *p_k;
    mcsDOUBLE sum;

    FAIL(Lower_Triangular_Inverse(LU, n));

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

    return mcsSUCCESS;
}

mcsCOMPL_STAT Choleski_LU_Decomposition(mcsDOUBLE *A, mcsUINT32 n)
{
    mcsUINT32 i, k, p;
    mcsDOUBLE *p_Lk0;                   /* pointer to L[k][0] */
    mcsDOUBLE *p_Lkp;                   /* pointer to L[k][p] */
    mcsDOUBLE *p_Lkk;                   /* pointer to diagonal element on row k.*/
    mcsDOUBLE *p_Li0;                   /* pointer to L[i][0]*/
    mcsDOUBLE reciprocal;

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
            return mcsFAILURE;
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
            *(p_Lk0 + i)  = *(p_Li0 + k);
        }
    }
    return mcsSUCCESS;
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
mcsCOMPL_STAT alxInvertMatrix(mcsDOUBLE *matrix, mcsUINT32 dim)
{
    FAIL(Choleski_LU_Decomposition(matrix, dim));
    FAIL(Choleski_LU_Inverse(matrix, dim));
    return mcsSUCCESS;
}

/*////////////////////////////////////////////////////////////////////////////*/
/*  void Multiply_Matrices(double *A, double *B, double *C,                   */
/*                         int nrows, int ncols, int mcols)                   */
/*                                                                            */
/*  Description:                                                              */
/*     Post multiply the nrows x ncols matrix A by the ncols x mcols matrix   */
/*     B to form the nrows x mcols matrix C, i.e. C = A B.                    */
/*     The matrix C should be declared as double C[nrows][mcols] in the       */
/*     calling routine.  The memory allocated to C should not include any     */
/*     memory allocated to A or B.                                            */
/*                                                                            */
/*  Arguments:                                                                */
/*     double *A    Pointer to the first element of the matrix A.             */
/*     double *B    Pointer to the first element of the matrix B.             */
/*     double *C    Pointer to the first element of the matrix C.             */
/*     int    nrows The number of rows of the matrices A and C.               */
/*     int    ncols The number of columns of the matrices A and the           */
/*                   number of rows of the matrix B.                          */
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
void alxProductMatrix(mcsDOUBLE *A, mcsDOUBLE *B, mcsDOUBLE *C, mcsUINT32 nrows,
                      mcsUINT32 ncols, mcsUINT32 mcols)
{
    mcsDOUBLE *pA = A;
    mcsDOUBLE *pB;
    mcsDOUBLE *p_B;
    mcsUINT32 i, j, k;

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
void alxComputePows(mcsUINT32 max, mcsDOUBLE x, mcsDOUBLE* pows)
{
    mcsUINT32 i = 0;
    mcsDOUBLE xn = 1.0;

    /* iterative algorithm */
    for (; ; )
    {
        pows[i++] = xn;

        /* fast return */
        if (i == max)
        {
            return;
        }
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
mcsDOUBLE alxComputePolynomial(mcsUINT32 len, mcsDOUBLE* coeffs, mcsDOUBLE* pows)
{
    mcsUINT32 i = 0;
    mcsDOUBLE p = 0.0;

    /* iterative algorithm */
    for (; i < len; i++)
    {
        p += coeffs[i] * pows[i];
    }
    return p;
}

/**
 * Initialize the alx module: preload all configuration tables
 */
void alxInit(void)
{
    alxAngularDiameterInit();
    alxMissingMagnitudeInit();
    alxInterstellarAbsorptionInit();
    alxResearchAreaInit();
    alxLD2UDInit();
    alxSedFittingInit();
}

/*___oOo___*/
