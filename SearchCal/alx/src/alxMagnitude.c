/*******************************************************************************
 * JMMC project
 * 
 * "@(#) $Id: alxMagnitude.c,v 1.6 2005-02-12 15:13:55 gzins Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.5  2005/01/31 13:32:37  scetre
 * changed misc...Pointer in misc...
 *
 * Revision 1.4  2005/01/26 15:49:09  scetre
 * solve memory problem with miscDYN_BUF
 *
 * Revision 1.3  2005/01/24 13:35:59  scetre
 * change the correct value of number of part of spectral type
 *
 * Revision 1.2  2005/01/24 10:56:25  scetre
 * Changed valid format for spectral type
 *
 * Revision 1.1  2005/01/21 08:14:25  gluck
 * Creation
 *
 *
 * scetre    20-Sep-2004  Created
 * gzins     12-Jan-2005  - Updated to be compliant with programming standards
 *                        - Improved reliability and error handling
 *                        - Add new confidence index computation
 *
 ******************************************************************************/

/**
 * \file
 * Function definition for magnitude computation.
 *
 * \sa JMMC-MEM-2600-0006 document.
 */

static char *rcsId="@(#) $Id: alxMagnitude.c,v 1.6 2005-02-12 15:13:55 gzins Exp $"; 
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
static alxCOLOR_TABLE *alxGetColorTableForBrightStar
      (alxSPECTRAL_TYPE spectralType);

    
/* 
 * Local functions definition
 */

/**
 * Return the color table corresponding to a given spectral type.
 *
 * This method determines the color table associated the given spectral from
 * bright star, and reads (if not yet done) this table from the configuration
 * file.
 *
 * \param spectralType spectral type of the star 
 * 
 * \return pointer to structure containing color table or NULL if an error
 * occured.
 *
 * \usedfiles Files containing the color indexes, the absolute magnitude in V
 * and the stellar mass according to the temperature class for different star
 * types. This tables are used to compute missing magnitudes.
 *  - alxColorTableForBrightDwarfStar.cfg : bright dwarf star 
 *  - alxColorTableForBrightGiantStar.cfg : bright giant star
 *  - alxColorTableForBrightSuperGiantStar.cfg : bright super giant star
 */
static alxCOLOR_TABLE *alxGetColorTableForBrightStar
    (alxSPECTRAL_TYPE spectralType)
{
    logExtDbg("alxGetColorTableForBrightStar()");

    /* Color table for the different type of stars */
    static alxCOLOR_TABLE colorTables[alxNB_STAR_TYPES] = {
        {mcsFALSE, "alxColorTableForBrightDwarfStar.cfg"},
        {mcsFALSE, "alxColorTableForBrightGiantStar.cfg"},
        {mcsFALSE, "alxColorTableForBrightSuperGiantStar.cfg"},
    };

    /* Determination of star type according to the spectral type */
    alxSTAR_TYPE starType;
    /* If light class = 
     *      Ia-O, Ia-O/Ia, Ia, Ia/ab, Iab, Iab-b, Ib, Ib-II,
     *          ==> SuperGiant
     * If light class =
     *      II, II/III, III, III/IV,
     *          ==> Giant
     * If light class =
     *      IV, IV/V, V, V/VI, VI,
     *          ==> Dwarfs
     */
    if (spectralType.lightClass[0] == 'I')
    {
        if (spectralType.lightClass[1] == 'I')
        {
            /* 
             * case light class =
             * II, II/III, III, III/IV
             */
            starType = alxGIANT;
        }
        else if (spectralType.lightClass[1] == 'V')
        {
            /*
             * case light class =
             * IV, IV/V, V, V/VI, VI
             */
            starType = alxDWARF;
        }
        else 
        {
            /* 
             * case light class =
             * Ia-O, Ia-O/Ia, Ia, Ia/ab, Iab, Iab-b, Ib, Ib-II
             */
            starType = alxSUPER_GIANT;
        }
    }
    else if (spectralType.lightClass[0] == 'V')
    {
        /* 
         * case light class = 
         * IV, IV/V, V, V/VI, VI
         */
        starType = alxDWARF;
    }
    else 
    {
        /* case no light class */
        starType = alxDWARF;
    }

    /* Print out tyep of star */
    switch (starType)
    {
        case alxDWARF:
            logTest("Type of star = DWARF"); 
            break;
        case alxGIANT:
            logTest("Type of star = GIANT"); 
            break;
        case alxSUPER_GIANT:
            logTest("Type of star = SUPER GIANT"); 
            break;
    }

    /*
     * Check if the structure, where will be stored polynomial coefficients, is
     * loaded into memory. If not load it.
     */
    if (colorTables[starType].loaded == mcsTRUE)
    {
        return (&colorTables[starType]);
    }

    /* 
     * Build the dynamic buffer which will contain file
     */
    /* Find the location of the file */
    char *fileName;
    fileName = miscLocateFile(colorTables[starType].fileName);
    if (fileName == NULL)
    {
        return NULL;
    }
    logDebug("Loading %s ...", fileName); 
    
    /* Load file where comment lines started with '#' */
    miscDYN_BUF    dynBuf;
    miscDynBufInit(&dynBuf);    
    if (miscDynBufLoadFile(&dynBuf, fileName, "#") == mcsFAILURE)
    {
        miscDynBufDestroy(&dynBuf);
        return NULL;
    }

    /* For each line */
    int  lineNum=0;
    char *line=NULL;
    while ((line = miscDynBufGetNextLine(&dynBuf,
                                                line, mcsTRUE)) != NULL)
    {
        logDebug("miscDynBufGetNextLine() = '%s'", line);

        /* If line is not empty */
        miscTrimString (line, " ");
        if (strlen(line) != 0)
        {
            /* Check if there is to many lines in file */
            if (lineNum >= alxNB_SPECTRAL_TYPES)
            {
                miscDynBufDestroy(&dynBuf);
                errAdd(alxERR_TOO_MANY_LINES, fileName);
                return NULL;
            }

            /* Get polynomial coefficients */
            if (sscanf(line, "%c%f %f %f %f %f %f %f %f %f %f %f",   
                       &colorTables[starType].spectralType[lineNum].code,
                       &colorTables[starType].spectralType[lineNum].quantity,
                       &colorTables[starType].index[lineNum][0],
                       &colorTables[starType].index[lineNum][1],
                       &colorTables[starType].index[lineNum][2],
                       &colorTables[starType].index[lineNum][3],
                       &colorTables[starType].index[lineNum][4],
                       &colorTables[starType].index[lineNum][5],
                       &colorTables[starType].index[lineNum][6],
                       &colorTables[starType].index[lineNum][7],
                       &colorTables[starType].index[lineNum][8],
                       &colorTables[starType].index[lineNum][9]) != 
                (alxNB_DIFF_MAG + 2))
            {
                miscDynBufDestroy(&dynBuf);
                errAdd(alxERR_WRONG_FILE_FORMAT, line, fileName);
                return NULL;
            }

            /* Next line */
            lineNum++;
        }
    }
    miscDynBufDestroy(&dynBuf);

    colorTables[starType].nbLines = lineNum;
    colorTables[starType].loaded = mcsTRUE;

    return &(colorTables[starType]);
}


/*
 * Public functions definition
 */

/**
 * Compute magnitudes in R, I, J, H, K, L and M bands for bright star.
 *
 * It computes magnitudes in R, I, J, H, K, L and M bands according to the
 * spectral type and the magnitudes in B and V bands for a bright star.
 *
 * \param spType spectral type
 * \param mgB magnitude in band B
 * \param mgV magnitude in band V
 * \param mgR computed magnitude in band R
 * \param mgI computed magnitude in band I
 * \param mgJ computed magnitude in band J
 * \param mgH computed magnitude in band H
 * \param mgK computed magnitude in band K
 * \param mgL computed magnitude in band L
 * \param mgM computed magnitude in band M
 * \param confIdx confidence index
 *
 * \return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is
 * returned.
 */
mcsCOMPL_STAT alxComputeMagnitudesForBrightStar(mcsSTRING32         spType,
                                                mcsFLOAT            mgB,
                                                mcsFLOAT            mgV,
                                                mcsFLOAT            *mgR,
                                                mcsFLOAT            *mgI,
                                                mcsFLOAT            *mgJ,
                                                mcsFLOAT            *mgH,
                                                mcsFLOAT            *mgK,
                                                mcsFLOAT            *mgL,
                                                mcsFLOAT            *mgM,
                                                alxCONFIDENCE_INDEX *confIdx)
{ 
    logExtDbg("alxComputeMagnitudesForBrightStar()");

    /* 
     * Get each part of the spectral type XN.NLLL where X is a letter, N.N a
     * number between 0 and 9 and LLL is the light class
     */
    alxSPECTRAL_TYPE spectralType;
    mcsINT32 nbItems;
    nbItems = sscanf(spType, "%c%f%s",
                     &spectralType.code, 
                     &spectralType.quantity, 
                     spectralType.lightClass);
    if ((nbItems != 2) && (nbItems != 3))
    {
        errAdd(alxERR_WRONG_SPECTRAL_TYPE_FORMAT, spType);
        return mcsFAILURE;
    }

    /* If there is no light class in given spectral type, reset it */
    if (nbItems == 2)
    {
        strcpy(spectralType.lightClass, ""); 
    }

    logTest("Type spectral = %s", spType);
    logTest("\tCode              = %c", spectralType.code);
    logTest("\tSub-type Quantity = %f", spectralType.quantity);
    logTest("\tLight class       = %s", spectralType.lightClass);
    
    /* Get the color table according to the spectral type of the star */
    alxCOLOR_TABLE *colorTable;
    colorTable = alxGetColorTableForBrightStar(spectralType);
    if (colorTable == NULL)
    {
        return mcsFAILURE;
    }

    /*
     * Find the line in the color table which matches with the star spectral
     * type or the line just after
     */
    mcsLOGICAL codeFound = mcsFALSE; 
    mcsLOGICAL found = mcsFALSE; 
    mcsINT32 line = 0;
    while ((found == mcsFALSE) && (line < colorTable->nbLines))
    {
        /* If the spectral type codes match */
        if (colorTable->spectralType[line].code ==  spectralType.code)
        {
            /*
             * And quantities match or star quantity is lower than the one of
             * the current line
             */
            if (colorTable->spectralType[line].quantity>=spectralType.quantity)
            {
                /* Stop search */
                found = mcsTRUE;
            }
            /* Else go to the next line */
            else
            {
                line++;         
            }   
            codeFound = mcsTRUE;
        }
        /* Else */
        else
        {
            /*
             * If the lines corresponding to the star spectral type code have
             * been scanned
             */
            if (codeFound == mcsTRUE)
            {
                /* Stop search */
                found = mcsTRUE;
            }
            /* Else go to the next line */
            else
            {
                line++;
            }
        }
    }

    /*
     * Check spectral type is not out of the table; i.e. before the first entry
     * in the color table
     */
    if ((line == 0) && 
        (colorTable->spectralType[line].quantity != spectralType.quantity))
    {
        found = mcsFALSE;
    }

    /* If spectral type not found in color table, return error */
    if (found != mcsTRUE)
    {
        errAdd(alxERR_SPECTRAL_TYPE_NOT_FOUND, spType);
        return mcsFAILURE;
    }
    
    /* If the spectral type matches the line of the color table */
    mcsFLOAT b_v, v_i, v_r, i_j, j_h, j_k, k_l, k_m;
    if (colorTable->spectralType[line].quantity == spectralType.quantity)
    {
        /* Compute magnitude in R, I, J, H, K, L and M bands */
        b_v = colorTable->index[line][alxB_V];
        v_i = colorTable->index[line][alxV_I];
        v_r = colorTable->index[line][alxV_R];
        i_j = colorTable->index[line][alxI_J];
        j_h = colorTable->index[line][alxJ_H];
        j_k = colorTable->index[line][alxJ_K];
        k_l = colorTable->index[line][alxK_L];
        k_m = colorTable->index[line][alxK_L] + colorTable->index[line][alxL_M];
        *mgR = mgV - v_r;
        *mgI = mgV - v_i;
        *mgJ = *mgI - i_j;
        *mgH = *mgJ - j_h;
        *mgK = *mgJ - j_k;
        *mgL = *mgK - k_l;
        *mgM = mgV - k_m;
        *confIdx = alxCONFIDENCE_HIGH;
    }
    /* Else, interpolate */
    else
    {
        mcsFLOAT ratio;
        mcsINT32 lineInf, lineSup;
        lineSup = line;
        lineInf = line-1;
        logTest("Inferior line = %d", lineInf);
        logTest("Superior line = %d", lineSup);
        
        /* Compute ratio for interpolation */
        ratio = fabs(((mgB-mgV) - colorTable->index[lineInf][alxB_V]) /
                     (colorTable->index[lineSup][alxB_V] -
                      colorTable->index[lineInf][alxB_V]));
        logTest("Ratio = %f", ratio);

        /* Compute differential magnitudes */
        v_r = colorTable->index[lineInf][alxV_R] +
            ratio *(colorTable->index[lineSup][alxV_R] -
                    colorTable->index[lineInf][alxV_R]);
        v_i = colorTable->index[lineInf][alxV_I] +
            ratio *(colorTable->index[lineSup][alxV_I] -
                    colorTable->index[lineInf][alxV_I]);
        i_j = colorTable->index[lineInf][alxI_J] +
            ratio *(colorTable->index[lineSup][alxI_J] -
                    colorTable->index[lineInf][alxI_J]);
        j_h = colorTable->index[lineInf][alxJ_H] +
            ratio *(colorTable->index[lineSup][alxJ_H] -
                    colorTable->index[lineInf][alxJ_H]);
        j_k = colorTable->index[lineInf][alxJ_K] +
            ratio *(colorTable->index[lineSup][alxJ_K] -
                    colorTable->index[lineSup][alxJ_K]);
        k_l = colorTable->index[lineInf][alxK_L] +
            ratio * (colorTable->index[lineSup][alxK_L] -
                     colorTable->index[lineInf][alxK_L]);
        k_m = colorTable->index[lineInf][alxK_L] +
            colorTable->index[lineInf][alxL_M] +
            ratio *(colorTable->index[lineSup][alxK_L] +
                    colorTable->index[lineSup][alxL_M] -
                    colorTable->index[lineInf][alxK_L] -
                    colorTable->index[lineInf][alxL_M]);

        /* Compute magnitudes in R, I, J, H, K, L and M bands */
        *mgR =  mgV - v_r;
        *mgI =  mgV - v_i;
        *mgJ = *mgI - i_j;
        *mgH = *mgJ - j_h;
        *mgK = *mgJ - j_k;
        *mgL = *mgK - k_l;
        *mgM =  mgV - k_m;
        *confIdx = alxCONFIDENCE_HIGH;
    }

    /* Print out results */
    logTest("B                = %0.3f", mgB);
    logTest("V                = %0.3f", mgV);
    logTest("R                = %0.3lf", *mgR);
    logTest("I                = %0.3lf", *mgI);
    logTest("J                = %0.3lf", *mgJ);
    logTest("H                = %0.3lf", *mgH);
    logTest("K                = %0.3lf", *mgK);
    logTest("L                = %0.3lf", *mgL);
    logTest("M                = %0.3lf", *mgM);
    logTest("Confidence index = %d", *confIdx);

    return mcsSUCCESS;
}

/*___oOo___*/
