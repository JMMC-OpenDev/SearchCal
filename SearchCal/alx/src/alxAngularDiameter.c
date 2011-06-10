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
 * @usedfiles alxAngDiamPolynomial.cfg : file containing the polynomial
 * coefficients to compute the angular diameter for bright star. The polynomial
 * coefficients are given for B-V, V-R, V-K, I-J, I-K, J-H and J-K.n
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
                            {mcsFALSE, "alxAngDiamPolynomial.cfg"};
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
            if (sscanf(line, "%*s %lf %lf %lf %lf %lf %lf %lf",   
                       &polynomial.coeff[lineNum][0],
                       &polynomial.coeff[lineNum][1],
                       &polynomial.coeff[lineNum][2],
                       &polynomial.coeff[lineNum][3],
                       &polynomial.coeff[lineNum][4],
                       &polynomial.coeff[lineNum][5],
                       &polynomial.error[lineNum])
                != (alxNB_POLYNOMIAL_COEFF_DIAMETER + 1))
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
mcsCOMPL_STAT alxComputeAngularDiameterForBrightStar(alxDATA mgB,
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
    mcsDOUBLE b_v = mgB.value - mgV.value;
    mcsDOUBLE v_r = mgV.value - mgR.value;
    mcsDOUBLE v_k = mgV.value - mgK.value;

    /* Declare polynomials P(B-V), P(V-R), P(V-K) */
    mcsDOUBLE p_b_v, p_v_r, p_v_k;

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
    diameters->bvErr.value = diameters->bv.value * polynomial->error[0]/100.0;
    diameters->vrErr.value = diameters->vr.value * polynomial->error[1]/100.0;;
    diameters->vkErr.value = diameters->vk.value * polynomial->error[2]/100.0;;

    /* Compute mean diameter and its associated error (10%) */
    diameters->mean.value = (  diameters->vk.value 
                               + diameters->vr.value
                               + diameters->bv.value) / 3;
     diameters->meanErr.value = 0.1 * diameters->mean.value;

    /* Check whether the diameter is coherent or not */
    if ((fabs(diameters->bv.value - diameters->mean.value) > 
         2.0 * diameters->meanErr.value) ||
        (fabs(diameters->vr.value - diameters->mean.value) > 
         2.0 * diameters->meanErr.value) ||
        (fabs(diameters->vk.value - diameters->mean.value) > 
         2.0 * diameters->meanErr.value) )
    {
        /* Reject star (i.e the diameter should not appear as computed) */
        diameters->areCoherent = mcsFALSE;
    }
    else
    {
        /* Set Confidence index to CONFIDENCE_HIGH */
        diameters->areCoherent = mcsTRUE;
    }

    /*
     * If diameter is OK (i.e. confidence index is alxCONFIDENCE_HIGH), set
     * confidence index of the computed diameter according to the ones of
     * magnitudes used to compute it. 
     */
    if (diameters->areCoherent == mcsTRUE)
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
    else
    {
        diameters->confidenceIdx = alxCONFIDENCE_LOW;            
    }     

    /* Display results */
    logTest("Diameter BV = %.3f(%.4f), VR = %.3f(%.4f), VK = %.3f(%.4f)", 
            diameters->bv.value, diameters->bvErr.value, diameters->vr.value,
            diameters->vrErr.value, diameters->vk.value,
            diameters->vkErr.value);
    if (diameters->areCoherent == mcsTRUE)
    {
        logTest("Confidence index = %d - (%d=LOW, %d=MEDIUM and %d=HIGH)", 
                diameters->confidenceIdx, alxCONFIDENCE_LOW, 
                alxCONFIDENCE_MEDIUM, alxCONFIDENCE_HIGH);
    }
    else
    {
        logTest("Computed diameters are not coherent between them; they are not kept");
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
 * @param mgV star magnitude in band V
 * @param mgKJnk star magnitude in band K (in Jonshon)
 * @param diameters the structure to give back all the computed diameters
 *  
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is
 * returned.
 */
mcsCOMPL_STAT alxComputeAngularDiameterForFaintStar(alxDATA mgI,
                                                    alxDATA mgJ,
                                                    alxDATA mgK,
                                                    alxDATA mgH,
                                                    alxDATA mgV,
                                                    alxDATA mgKJnk,
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

    /* Compute I-J, I-K, J-K, J-H, V-K */
    mcsDOUBLE i_j = mgI.value - mgJ.value;
    mcsDOUBLE i_k = mgI.value - mgK.value;
    mcsDOUBLE j_k = mgJ.value - mgK.value;
    mcsDOUBLE j_h = mgJ.value - mgH.value;
    mcsDOUBLE h_k = mgH.value - mgK.value;
    mcsDOUBLE v_k = mgV.value - mgKJnk.value;

    /* Compute the polynomials P(I-J), P(I-K), P(J-K), P(J-H), P(H-K), P(V-K) */
    mcsDOUBLE p_i_j, p_i_k, p_j_k, p_j_h, p_h_k, p_v_k;
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

    p_j_h =   polynomial->coeff[5][0]
            + polynomial->coeff[5][1] * j_h
            + polynomial->coeff[5][2] * pow(j_h, 2)
            + polynomial->coeff[5][3] * pow(j_h, 3)
            + polynomial->coeff[5][4] * pow(j_h, 4)
            + polynomial->coeff[5][5] * pow(j_h, 5);

    p_j_k =   polynomial->coeff[6][0]
            + polynomial->coeff[6][1] * j_k
            + polynomial->coeff[6][2] * pow(j_k, 2)
            + polynomial->coeff[6][3] * pow(j_k, 3)
            + polynomial->coeff[6][4] * pow(j_k, 4)
            + polynomial->coeff[6][5] * pow(j_k, 5);
    
    p_h_k =   polynomial->coeff[7][0]
            + polynomial->coeff[7][1] * h_k
            + polynomial->coeff[7][2] * pow(h_k, 2)
            + polynomial->coeff[7][3] * pow(h_k, 3)
            + polynomial->coeff[7][4] * pow(h_k, 4)
            + polynomial->coeff[7][5] * pow(h_k, 5);

    p_v_k =   polynomial->coeff[2][0]
        + polynomial->coeff[2][1] * v_k
        + polynomial->coeff[2][2] * pow(v_k, 2)
        + polynomial->coeff[2][3] * pow(v_k, 3)
        + polynomial->coeff[2][4] * pow(v_k, 4)
        + polynomial->coeff[2][5] * pow(v_k, 5);

    /* Compute the diameters D(I-J), D(I-K), D(J-K), D(J-H), D(H-K), D(V-K) */
    diameters->ij.value    = 9.306 * pow(10, -0.2 * mgI.value) * p_i_j;
    diameters->ik.value    = 9.306 * pow(10, -0.2 * mgI.value) * p_i_k;
    diameters->jk.value    = 9.306 * pow(10, -0.2 * mgJ.value) * p_j_k;
    diameters->jh.value    = 9.306 * pow(10, -0.2 * mgJ.value) * p_j_h;
    diameters->hk.value    = 9.306 * pow(10, -0.2 * mgH.value) * p_h_k;
    diameters->vk.value    = 9.306 * pow(10, -0.2 * mgV.value) * p_v_k;
    diameters->ijErr.value = diameters->ij.value * polynomial->error[3]/100.0;
    diameters->ikErr.value = diameters->ik.value * polynomial->error[4]/100.0;;
    diameters->jhErr.value = diameters->jh.value * polynomial->error[5]/100.0;;
    diameters->jkErr.value = diameters->jk.value * polynomial->error[6]/100.0;;
    diameters->hkErr.value = diameters->hk.value * polynomial->error[7]/100.0;;
    diameters->vkErr.value = diameters->vk.value * polynomial->error[2]/100.0;;

    /* Computer mean diameter and associated error */
    int nbDiameters = 2;
    diameters->mean.value = diameters->jh.value + diameters->jk.value;
    if (mgV.isSet == mcsTRUE)
    {
        diameters->mean.value +=diameters->vk.value;
        nbDiameters += 1;
    }
    if (mgI.isSet == mcsFALSE)
    {
        diameters->mean.value +=diameters->hk.value;
        nbDiameters += 1;
    }
    else
    {
        diameters->mean.value += diameters->ij.value;
        diameters->mean.value += diameters->ik.value;
        nbDiameters += 2;
    }
    diameters->mean.value = diameters->mean.value / nbDiameters;
    diameters->meanErr.value = 0.2 * diameters->mean.value;

    /* Check whether the diameter is coherent or not */
    if ((fabs(diameters->jh.value - diameters->mean.value) >
         diameters->meanErr.value) ||
        (fabs(diameters->jk.value - diameters->mean.value) >
         diameters->meanErr.value) ||
        ((mgV.isSet == mcsTRUE) && 
         (fabs(diameters->vk.value - diameters->mean.value) >
         diameters->meanErr.value)) ||
        ((mgI.isSet == mcsFALSE) && 
         (fabs(diameters->hk.value - diameters->mean.value) >
         diameters->meanErr.value)) ||
        ((mgI.isSet == mcsTRUE) && 
         (fabs(diameters->ij.value - diameters->mean.value) >
         diameters->meanErr.value)) ||
        ((mgI.isSet == mcsTRUE) && 
         (fabs(diameters->ik.value - diameters->mean.value) >
         diameters->meanErr.value)))
    {
        /* Reject star (i.e the diameter should not appear as computed) */
        diameters->areCoherent = mcsFALSE;
    }
    else
    {
        /* Set Confidence index to CONFIDENCE_HIGH */
        diameters->areCoherent = mcsTRUE;
    }

    /*
     * If diameter is OK (i.e. confidence index is alxCONFIDENCE_HIGH), set
     * confidence index of the computed diameter according to the ones of
     * magnitudes used to compute it. 
     */
    if (diameters->areCoherent == mcsTRUE)
    {
        if ((mgJ.confIndex == alxCONFIDENCE_LOW) ||
            (mgK.confIndex == alxCONFIDENCE_LOW) ||
            (mgH.confIndex == alxCONFIDENCE_LOW) ||
            ((mgI.isSet == mcsTRUE) && (mgI.confIndex == alxCONFIDENCE_LOW)))
        {
            diameters->confidenceIdx = alxCONFIDENCE_LOW;
        }
        else if ((mgJ.confIndex == alxCONFIDENCE_MEDIUM) ||
                 (mgK.confIndex == alxCONFIDENCE_MEDIUM) ||
                 (mgH.confIndex == alxCONFIDENCE_MEDIUM) ||
                 ((mgI.isSet == mcsTRUE) &&
                  (mgI.confIndex == alxCONFIDENCE_MEDIUM)))
        {
            diameters->confidenceIdx = alxCONFIDENCE_MEDIUM;
        }
        else 
        {
            diameters->confidenceIdx = alxCONFIDENCE_HIGH;            
        }
    }
    else
    {
        diameters->confidenceIdx = alxCONFIDENCE_LOW;            
    }        

    /* Display results */
    logInfo("Diameter JH = %.3f(%.4f)", 
            diameters->jh.value, diameters->jhErr.value);
    logInfo("Diameter JK = %.3f(%.4f)", 
            diameters->jk.value, diameters->jkErr.value);
    if (mgV.isSet == mcsTRUE)
    {
        logInfo("Diameter VK = %.3f(%.4f)", 
                diameters->vk.value, diameters->vkErr.value);
    }
    if (mgI.isSet == mcsFALSE)
    {
        logInfo("Diameter HK = %.3f(%.4f)", 
                diameters->hk.value, diameters->hkErr.value);
    }
    else
    {
        logInfo("Diameter IJ = %.3f(%.4f)", 
                diameters->ij.value, diameters->ijErr.value);
        
        logInfo("Diameter IK = %.3f(%.4f)", 
                diameters->ik.value, diameters->ikErr.value);
    }
    logInfo("Mean diameter = %.3f(%.4f)", 
            diameters->mean.value, diameters->meanErr.value);

    if (diameters->areCoherent == mcsTRUE)
    {
        logTest("Confidence index = %d - (%d=LOW, %d=MEDIUM and %d=HIGH)", 
                diameters->confidenceIdx, alxCONFIDENCE_LOW, 
                alxCONFIDENCE_MEDIUM, alxCONFIDENCE_HIGH);
    }
    else
    {
        logTest("Computed diameters are not coherent between them; "
                "they are not kept");
    }

    return mcsSUCCESS;
}

/**
 * Initialize this code
 * @return void
 */
mcsCOMPL_STAT alxAngularDiameterInit(void)
{
    alxGetPolynamialForAngularDiameter();
    
    return mcsSUCCESS;
}
/*___oOo___*/
