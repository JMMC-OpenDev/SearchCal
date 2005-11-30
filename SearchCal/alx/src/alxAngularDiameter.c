/*******************************************************************************
 * JMMC project
 * 
 * "@(#) $Id: alxAngularDiameter.c,v 1.17 2005-11-30 10:47:59 scetre Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.16  2005/10/26 11:24:01  lafrasse
 * Code review
 *
 * Revision 1.15  2005/07/06 05:06:04  gzins
 * Improved log used for test
 *
 * Revision 1.14  2005/06/01 14:16:07  scetre
 * Changed logExtDbg to logTrace
 *
 * Revision 1.13  2005/04/06 12:15:22  scetre
 * Changed used of float for properties to computed in alxDATA
 * removed alxNO_CONFIDENCE
 *
 * Revision 1.12  2005/04/04 07:22:11  scetre
 * Updated documentation
 *
 * Revision 1.11  2005/03/30 12:46:57  scetre
 * Added structure in order to simplify the code.
 * Changed API with this structure
 *
 * Revision 1.10  2005/03/03 14:46:19  gzins
 * Changed alxCONFIDENCE_LOW to alxNO_CONFIDENCE
 *
 * Revision 1.9  2005/02/22 16:51:58  gzins
 * Updated misDynBufGetNextLine API
 * Added errors for B-V, V-R and V-K diameters
 *
 * Revision 1.8  2005/02/22 14:05:13  scetre
 * Patch diam error : use 0.1*d_v_k insted of mean 0.1*diam
 *
 * Revision 1.7  2005/02/16 15:10:57  gzins
 * Updated call to miscDynBufGetNextLine()
 *
 * Revision 1.6  2005/02/12 15:13:11  gzins
 * Removed call to miscResolvePath; done by miscLocateFile
 *
 * Revision 1.5  2005/02/10 07:55:31  gzins
 * Updated alxComputeAngularDiameter to return diameter from B-V, V-R and V-K calibration
 *
 * Revision 1.4  2005/02/04 13:50:39  gzins
 * Changed alxCONFIDENCE_VERY_LOW to alxCONFIDENCE_LOW
 *
 * Revision 1.3  2005/01/31 13:32:37  scetre
 * changed misc...Pointer in misc...
 *
 * Revision 1.2  2005/01/26 15:49:09  scetre
 * solve memory problem with miscDYN_BUF
 *
 * Revision 1.1  2005/01/21 08:14:25  gluck
 * Creation
 *
 * 
 * scetre    11-Oct-2004  Created
 * gzins     12-Jan-2005  - Updated to be compliant with programming standards
 *                        - Improved reliability and error handling
 *                        - Add new confidence index computation
 *
 ******************************************************************************/

/**
 * @file
 * Function definition for angular diameter computation.
 *
 * @sa JMMC-MEM-2600-0009 document.
 */

static char *rcsId="@(#) $Id: alxAngularDiameter.c,v 1.17 2005-11-30 10:47:59 scetre Exp $"; 
static void *use_rcsId = ((void)&use_rcsId,(void *) &rcsId);


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


/*
 * Local Functions declaration
 */
static alxPOLYNOMIAL_ANGULAR_DIAMETER *alxGetPolynamialForAngularDiameter(void);


/* 
 * Local functions definition
 */
/**
 * Return the polynomial coefficients for angular diameter computation 
 *
 * @return pointer onto the structure containing polynomial coefficients, or
 * NULL if an error occured.
 *
 * @usedfiles alxAngDiamPolynomialForBrightStar.cfg : file containing the
 * polynomial coefficients to compute the angular diameter for bright star. The
 * polynomial coefficients are given for B-V, V-R and V-K.
 */
static alxPOLYNOMIAL_ANGULAR_DIAMETER *alxGetPolynamialForAngularDiameter(void)
{
    logTrace("alxGetPolynamialForAngularDiameter()");

    /*
     * Check wether the polynomial structure in which polynomial coefficients
     * will be stored to compute angular diameter is loaded into memory or not,
     * and load it if necessary.
     */
    static alxPOLYNOMIAL_ANGULAR_DIAMETER polynomial = 
                            {mcsFALSE, "alxAngDiamPolynomialForBrightStar.cfg"};
    if (polynomial.loaded == mcsTRUE)
    {
        return &polynomial;
    }

    /* 
     * Build the dynamic buffer which will contain the coefficient file 
     * coefficient for angular diameter computation
     */
    /* Find the location of the file */
    char* fileName = miscLocateFile(polynomial.fileName);
    if (fileName == NULL)
    {
        return NULL;
    }

    /* Load file. Comment lines start with '#' */
    miscDYN_BUF dynBuf;
    miscDynBufInit(&dynBuf);
    if (miscDynBufLoadFile(&dynBuf, fileName, "#") == mcsFAILURE)
    {
        miscDynBufDestroy(&dynBuf);
        return NULL;
    }

    /* For each line of the loaded file */
    mcsINT32 lineNum = 0;
    const char* pos = NULL;
    mcsSTRING1024 line;
    while ((pos = miscDynBufGetNextLine(&dynBuf, pos, line, sizeof(line),
                                        mcsTRUE)) != NULL)
    {
        logTest("miscDynBufGetNextLine() = '%s'", line);

        /* If the current line is not empty */
        if (miscIsSpaceStr(line) == mcsFALSE)
        {
            /* Check if there is to many lines in file */
            if (lineNum >= alxNB_COLOR_INDEXES)
            {
                miscDynBufDestroy(&dynBuf);
                errAdd(alxERR_TOO_MANY_LINES, fileName);
                return NULL;
            }

            /* Read polynomial coefficients */
            if (sscanf(line, "%*s %f %f %f %f %f %f",   
                       &polynomial.coeff[lineNum][0],
                       &polynomial.coeff[lineNum][1],
                       &polynomial.coeff[lineNum][2],
                       &polynomial.coeff[lineNum][3],
                       &polynomial.coeff[lineNum][4],
                       &polynomial.coeff[lineNum][5])
                != alxNB_POLYNOMIAL_COEFF_DIAMETER)
            {
                miscDynBufDestroy(&dynBuf);
                errAdd(alxERR_WRONG_FILE_FORMAT, line, fileName);
                return NULL;
            }

            /* Next line */
            lineNum++;
        }
    }

    /* Destroy the dynamic buffer where is stored the file information */
    miscDynBufDestroy(&dynBuf);

    /* Check if there is missing line */
    if (lineNum != alxNB_COLOR_INDEXES)
    {
        errAdd(alxERR_MISSING_LINE, lineNum, alxNB_COLOR_INDEXES, fileName);
        miscDynBufDestroy(&dynBuf);
        return NULL;
    }
    
    /* Specify that the polynomial has been loaded */
    polynomial.loaded = mcsTRUE;

    return &polynomial;
}


/*
 * Public functions definition
 */
/**
 * Compute a star angular diameters from its photometric properties.
 *
 * @param mgB star magnitude in band B
 * @param mgV star magnitude in band V
 * @param mgR star magnitude in band R
 * @param mgK star magnitude in band K
 * @param diameters the structure to give back all the computed diameters
 *  
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is
 * returned.
 */
mcsCOMPL_STAT alxComputeAngularDiameter(alxDATA mgB,
                                        alxDATA mgV,
                                        alxDATA mgR,
                                        alxDATA mgK,
                                        alxDIAMETERS *diameters)
{
    logTrace("alxComputeAngularDiameter()");

    /* Get polynamial for diameter computation */
    alxPOLYNOMIAL_ANGULAR_DIAMETER *polynomial;
    polynomial = alxGetPolynamialForAngularDiameter();
    if (polynomial == NULL)
    {
        return mcsFAILURE;        
    }

    /* Compute B-V, V-R, V-K */
    mcsFLOAT b_v = mgB.value - mgV.value;
    mcsFLOAT v_r = mgV.value - mgR.value;
    mcsFLOAT v_k = mgV.value - mgK.value;

    /* Declare polynomials P(B-V), P(V-R), P(V-K) */
    mcsFLOAT p_b_v, p_v_r, p_v_k;

    /* 
     * Declare the 3 diameters D(B-V), D(V-R), D(V-K),
     * the mean diameter
     * and the mean delta
     */
    mcsFLOAT meanDiam;
    mcsFLOAT meanDiamErr;

    /* Compute the polynomials P(B-V), P(V-R), P(V-K) */
    p_b_v =   polynomial->coeff[0][0]
            + polynomial->coeff[0][1] * b_v
            + polynomial->coeff[0][2] * pow(b_v, 2)
            + polynomial->coeff[0][3] * pow(b_v, 3)
            + polynomial->coeff[0][4] * pow(b_v, 4)
            + polynomial->coeff[0][5] * pow(b_v, 5);

    p_v_r =   polynomial->coeff[1][0]
            + polynomial->coeff[1][1] * v_r
            + polynomial->coeff[1][2] * pow(v_r, 2)
            + polynomial->coeff[1][3] * pow(v_r, 3)
            + polynomial->coeff[1][4] * pow(v_r, 4)
            + polynomial->coeff[1][5] * pow(v_r, 5);

    p_v_k =   polynomial->coeff[2][0]
            + polynomial->coeff[2][1] * v_k
            + polynomial->coeff[2][2] * pow(v_k, 2)
            + polynomial->coeff[2][3] * pow(v_k, 3)
            + polynomial->coeff[2][4] * pow(v_k, 4)
            + polynomial->coeff[2][5] * pow(v_k, 5);

    /* Compute the diameters D(B-V), D(V-R), D(V-K) */
    diameters->bv.value    = 9.306 * pow(10, -0.2 * mgV.value) * p_b_v;
    diameters->vr.value    = 9.306 * pow(10, -0.2 * mgV.value) * p_v_r;
    diameters->vk.value    = 9.306 * pow(10, -0.2 * mgV.value) * p_v_k;
    diameters->bvErr.value = diameters->bv.value * 8.0/100.0;
    diameters->vrErr.value = diameters->vr.value * 9.7/100.0;;
    diameters->vkErr.value = diameters->vk.value * 6.9/100.0;;

    /* Compute mean diameter and its associated error (10%) */
    meanDiam = (  diameters->vk.value 
                + diameters->vr.value
                + diameters->bv.value) / 3;
    meanDiamErr = 0.1 * meanDiam;

    /* Check whether the diameter is coherent or not */
    if ((fabs(diameters->bv.value - meanDiam) > 2.0 * meanDiamErr) ||
        (fabs(diameters->vr.value - meanDiam) > 2.0 * meanDiamErr) ||
        (fabs(diameters->vk.value - meanDiam) > 2.0 * meanDiamErr) )
    {
        /* Reject star (i.e the diameter should not appear as computed) */
        diameters->areComputed = mcsFALSE;
    }
    else
    {
        /* Set Confidence index to CONFIDENCE_HIGH */
        diameters->areComputed = mcsTRUE;
    }

    /*
     * If diameter is OK (i.e. confidence index is alxCONFIDENCE_HIGH), set
     * confidence index of the computed diameter according to the ones of
     * magnitudes used to compute it. 
     */
    if (diameters->areComputed == mcsTRUE)
    {
        if ((mgK.confIndex == alxCONFIDENCE_LOW) ||
            (mgR.confIndex == alxCONFIDENCE_LOW))
        {
            diameters->confidenceIdx = alxCONFIDENCE_LOW;
        }
        else if ((mgK.confIndex == alxCONFIDENCE_MEDIUM) ||
                 (mgR.confIndex == alxCONFIDENCE_MEDIUM))
        {
            diameters->confidenceIdx = alxCONFIDENCE_MEDIUM;
        }
        else 
        {
            diameters->confidenceIdx = alxCONFIDENCE_HIGH;            
        }
    }
    
    /* Display results */
    logTest("Diameter BV = %.3f(%.4f), VR = %.3f(%.4f), VK = %.3f(%.4f)", 
            diameters->bv.value, diameters->bvErr.value, diameters->vr.value,
            diameters->vrErr.value, diameters->vk.value,
            diameters->vkErr.value);
    if (diameters->areComputed == mcsTRUE)
    {
        logTest("Confidence index = %d - (%d=LOW, %d=MEDIUM and %d=HIGH)", 
                diameters->confidenceIdx, alxCONFIDENCE_LOW, 
                alxCONFIDENCE_MEDIUM, alxCONFIDENCE_HIGH);
    }
    else
    {
        logTest("Diameters have not been computed");
    }

    return mcsSUCCESS;
}

/**
 * Compute a faint star angular diameters from its photometric properties.
 *
 * @param mgI star magnitude in band I
 * @param mgJ star magnitude in band J
 * @param mgK star magnitude in band K
 * @param mgH star magnitude in band H
 * @param diameters the structure to give back all the computed diameters
 *  
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is
 * returned.
 */
mcsCOMPL_STAT alxComputeAngularDiameterFaint(alxDATA mgI,
                                             alxDATA mgJ,
                                             alxDATA mgK,
                                             alxDATA mgH,
                                             alxDIAMETERS *diameters)
{
    logTrace("alxComputeAngularDiameterFaint()");

    /* Get polynamial for diameter computation */
    alxPOLYNOMIAL_ANGULAR_DIAMETER *polynomial;
    polynomial = alxGetPolynamialForAngularDiameter();
    if (polynomial == NULL)
    {
        return mcsFAILURE;        
    }

    /* Compute I-J, I-K, J-K, J-H */
    mcsFLOAT i_j = mgI.value - mgJ.value;
    mcsFLOAT i_k = mgI.value - mgK.value;
    mcsFLOAT j_k = mgJ.value - mgK.value;
    mcsFLOAT j_h = mgJ.value - mgH.value;

    /* Declare polynomials P(I-J), P(I-K), P(J-K), P(J-H) */
    mcsFLOAT p_i_j, p_i_k, p_j_k, p_j_h;
    
    /* 
     * Declare the 4 diameters D(I-J), D(I-K), D(J-K), D(J-H)
     * the mean diameter
     * and the mean delta
     */
    mcsFLOAT meanDiam;
    mcsFLOAT meanDiamErr;

    /* Compute the polynomials P(I-J), P(I-K), P(J-K), P(J-H) */
    p_i_j =   polynomial->coeff[3][0]
            + polynomial->coeff[3][1] * i_j
            + polynomial->coeff[3][2] * pow(i_j, 2)
            + polynomial->coeff[3][3] * pow(i_j, 3)
            + polynomial->coeff[3][4] * pow(i_j, 4)
            + polynomial->coeff[3][5] * pow(i_j, 5);

    p_i_k =   polynomial->coeff[4][0]
            + polynomial->coeff[4][1] * i_k
            + polynomial->coeff[4][2] * pow(i_k, 2)
            + polynomial->coeff[4][3] * pow(i_k, 3)
            + polynomial->coeff[4][4] * pow(i_k, 4)
            + polynomial->coeff[4][5] * pow(i_k, 5);

    p_j_k =   polynomial->coeff[5][0]
            + polynomial->coeff[5][1] * j_k
            + polynomial->coeff[5][2] * pow(j_k, 2)
            + polynomial->coeff[5][3] * pow(j_k, 3)
            + polynomial->coeff[5][4] * pow(j_k, 4)
            + polynomial->coeff[5][5] * pow(j_k, 5);

    p_j_h =   polynomial->coeff[6][0]
            + polynomial->coeff[6][1] * j_h
            + polynomial->coeff[6][2] * pow(j_h, 2)
            + polynomial->coeff[6][3] * pow(j_h, 3)
            + polynomial->coeff[6][4] * pow(j_h, 4)
            + polynomial->coeff[6][5] * pow(j_h, 5);

    /* Compute the diameters D(B-V), D(V-R), D(V-K) */
    diameters->ij.value    = 9.306 * pow(10, -0.2 * mgI.value) * p_i_j;
    diameters->ik.value    = 9.306 * pow(10, -0.2 * mgI.value) * p_i_k;
    diameters->jk.value    = 9.306 * pow(10, -0.2 * mgI.value) * p_j_k;
    diameters->jh.value    = 9.306 * pow(10, -0.2 * mgI.value) * p_j_h;
    diameters->ijErr.value = diameters->ij.value * 10.4/100.0;
    diameters->ikErr.value = diameters->ik.value * 8.5/100.0;;
    diameters->jkErr.value = diameters->jk.value * 9.1/100.0;;
    diameters->jhErr.value = diameters->jh.value * 10.7/100.0;;

    /* Compute mean diameter and its associated error (10%) */
    meanDiam = (  diameters->ij.value 
                + diameters->ik.value
                + diameters->jk.value
                + diameters->jh.value) / 4;
    meanDiamErr = 0.1 * meanDiam;

    /* Check whether the diameter is coherent or not */
    if ((fabs(diameters->ij.value - meanDiam) > 2.0 * meanDiamErr) ||
        (fabs(diameters->ik.value - meanDiam) > 2.0 * meanDiamErr) ||
        (fabs(diameters->jk.value - meanDiam) > 2.0 * meanDiamErr) ||
        (fabs(diameters->jh.value - meanDiam) > 2.0 * meanDiamErr) )
    {
        /* Reject star (i.e the diameter should not appear as computed) */
        diameters->areComputed = mcsFALSE;
    }
    else
    {
        /* Set Confidence index to CONFIDENCE_HIGH */
        diameters->areComputed = mcsTRUE;
    }

    /*
     * If diameter is OK (i.e. confidence index is alxCONFIDENCE_HIGH), set
     * confidence index of the computed diameter according to the ones of
     * magnitudes used to compute it. 
     */
    /*if (diameters->areComputed == mcsTRUE)
    {
        if ((mgK.confIndex == alxCONFIDENCE_LOW) ||
            (mgR.confIndex == alxCONFIDENCE_LOW))
        {
            diameters->confidenceIdx = alxCONFIDENCE_LOW;
        }
        else if ((mgK.confIndex == alxCONFIDENCE_MEDIUM) ||
                 (mgR.confIndex == alxCONFIDENCE_MEDIUM))
        {
            diameters->confidenceIdx = alxCONFIDENCE_MEDIUM;
        }
        else 
        {*/
            diameters->confidenceIdx = alxCONFIDENCE_HIGH;            
    /*    }
    }*/
    
    /* Display results */
    logTest("Diameter IJ = %.3f(%.4f), IK = %.3f(%.4f), JK = %.3f(%.4f), JH = %.3f(%.4f) ", diameters->ij.value, diameters->ijErr.value, diameters->ik.value,
            diameters->ikErr.value, diameters->jk.value,
            diameters->jkErr.value, diameters->jh.value,
            diameters->jhErr.value);
    if (diameters->areComputed == mcsTRUE)
    {
        logTest("Confidence index = %d - (%d=LOW, %d=MEDIUM and %d=HIGH)", 
                diameters->confidenceIdx, alxCONFIDENCE_LOW, 
                alxCONFIDENCE_MEDIUM, alxCONFIDENCE_HIGH);
    }
    else
    {
        logTest("Diameters have not been computed");
    }

    return mcsSUCCESS;
}
/*___oOo___*/
