/*******************************************************************************
 * JMMC project
 * 
 * "@(#) $Id: alxMagnitude.c,v 1.10 2005-02-22 10:16:10 gzins Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.9  2005/02/22 08:08:20  gzins
 * Updated to set confidence index to alxNO_CONFIDENCE when magnitude can not be computed
 *
 * Revision 1.8  2005/02/21 19:32:44  gzins
 * Updated to set confidence index for each computed magnitudes; when a magnitude can not be computed confidence index is set to low
 *
 * Revision 1.7  2005/02/16 15:10:57  gzins
 * Updated call to miscDynBufGetNextLine()
 *
 * Revision 1.6  2005/02/12 15:13:55  gzins
 * Removed call to miscResolvePath; done by miscLocateFile
 *
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

static char *rcsId="@(#) $Id: alxMagnitude.c,v 1.10 2005-02-22 10:16:10 gzins Exp $"; 
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
    const char *pos = NULL;
    mcsSTRING1024 line;
    while ((pos = miscDynBufGetNextLine(&dynBuf, pos, line, mcsTRUE)) != NULL)
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
            if (sscanf(line, "%c%f %f %f %f %f %f %f %f %f",   
                       &colorTables[starType].spectralType[lineNum].code,
                       &colorTables[starType].spectralType[lineNum].quantity,
                       &colorTables[starType].index[lineNum][0],
                       &colorTables[starType].index[lineNum][1],
                       &colorTables[starType].index[lineNum][2],
                       &colorTables[starType].index[lineNum][3],
                       &colorTables[starType].index[lineNum][4],
                       &colorTables[starType].index[lineNum][5],
                       &colorTables[starType].index[lineNum][6],
                       &colorTables[starType].index[lineNum][7]) != 
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
 * If magnitude can not be computed, its associated confidence index is set to
 * alxNO_CONFIDENCE.
 *
 * \param spType spectral type
 * \param magnitudes contains magnitudes in B and V bands, and the computed
 * magnitudes in R, I, J, H, K, L and M bands
 * \param confIndexes confidence indexes for computed magnitudes
 *
 * \return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is
 * returned.
 */
mcsCOMPL_STAT 
alxComputeMagnitudesForBrightStar(mcsSTRING32         spType,
                                  mcsFLOAT magnitudes[alxNB_BANDS],
                                  alxCONFIDENCE_INDEX confIndexes[alxNB_BANDS])
{ 
    logExtDbg("alxComputeMagnitudesForBrightStar()");

    /* Get magnitudes in B and V bands */
    mcsFLOAT mgB, mgV;
    mgB = magnitudes[alxB_BAND];
    mgV = magnitudes[alxV_BAND];

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
        /* Get differential magnitudes */
        b_v = colorTable->index[line][alxB_V];
        v_i = colorTable->index[line][alxV_I];
        v_r = colorTable->index[line][alxV_R];
        i_j = colorTable->index[line][alxI_J];
        j_h = colorTable->index[line][alxJ_H];
        j_k = colorTable->index[line][alxJ_K];
        k_l = colorTable->index[line][alxK_L];
        if ((colorTable->index[line][alxK_L] == alxBLANKING_VALUE) ||
            (colorTable->index[line][alxL_M] == alxBLANKING_VALUE))
        {
            k_m = alxBLANKING_VALUE;
        }
        else
        {
            k_m = colorTable->index[line][alxK_L] + 
                colorTable->index[line][alxL_M];
        }
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
        if ((colorTable->index[lineSup][alxV_R] == alxBLANKING_VALUE) ||
            (colorTable->index[lineInf][alxV_R] == alxBLANKING_VALUE))
        {
            v_r = alxBLANKING_VALUE;
        }
        else
        {
            v_r = colorTable->index[lineInf][alxV_R] +
                ratio *(colorTable->index[lineSup][alxV_R] -
                        colorTable->index[lineInf][alxV_R]);
        }
        
        if ((colorTable->index[lineSup][alxV_I] == alxBLANKING_VALUE) ||
            (colorTable->index[lineInf][alxV_I] == alxBLANKING_VALUE))
        {
            v_i = alxBLANKING_VALUE;
        }
        else
        {
            v_i = colorTable->index[lineInf][alxV_I] +
                ratio *(colorTable->index[lineSup][alxV_I] -
                        colorTable->index[lineInf][alxV_I]);
        }
        
        if ((colorTable->index[lineSup][alxI_J] == alxBLANKING_VALUE) ||
            (colorTable->index[lineInf][alxI_J] == alxBLANKING_VALUE))
        {
            i_j = alxBLANKING_VALUE;
        }
        else
        {
            i_j = colorTable->index[lineInf][alxI_J] +
                ratio *(colorTable->index[lineSup][alxI_J] -
                        colorTable->index[lineInf][alxI_J]);
        }

        if ((colorTable->index[lineSup][alxJ_H] == alxBLANKING_VALUE) ||
            (colorTable->index[lineInf][alxJ_H] == alxBLANKING_VALUE))
        {
            j_h = alxBLANKING_VALUE;
        }
        else
        {
            j_h = colorTable->index[lineInf][alxJ_H] +
                ratio *(colorTable->index[lineSup][alxJ_H] -
                        colorTable->index[lineInf][alxJ_H]);
        }

        if ((colorTable->index[lineSup][alxJ_K] == alxBLANKING_VALUE) ||
            (colorTable->index[lineInf][alxJ_K] == alxBLANKING_VALUE))
        {
            j_k = alxBLANKING_VALUE;
        }
        else
        {
            j_k = colorTable->index[lineInf][alxJ_K] +
                ratio *(colorTable->index[lineSup][alxJ_K] -
                        colorTable->index[lineSup][alxJ_K]);
        }

        if ((colorTable->index[lineSup][alxK_L] == alxBLANKING_VALUE) ||
            (colorTable->index[lineInf][alxK_L] == alxBLANKING_VALUE))
        {
            k_l = alxBLANKING_VALUE;
        }
        else
        {
            k_l = colorTable->index[lineInf][alxK_L] +
                ratio * (colorTable->index[lineSup][alxK_L] -
                         colorTable->index[lineInf][alxK_L]);
        }

        if ((colorTable->index[lineSup][alxK_L] == alxBLANKING_VALUE) ||
            (colorTable->index[lineInf][alxK_L] == alxBLANKING_VALUE) ||
            (colorTable->index[lineSup][alxL_M] == alxBLANKING_VALUE) ||
            (colorTable->index[lineInf][alxL_M] == alxBLANKING_VALUE))
        {
            k_m = alxBLANKING_VALUE;
        }
        else
        {
            k_m = colorTable->index[lineInf][alxK_L] +
                colorTable->index[lineInf][alxL_M] +
                ratio *(colorTable->index[lineSup][alxK_L] +
                        colorTable->index[lineSup][alxL_M] -
                        colorTable->index[lineInf][alxK_L] -
                        colorTable->index[lineInf][alxL_M]);
        }
    }

    /* Compute *missing* magnitudes in R, I, J, H, K, L and M bands */
    if (magnitudes [alxR_BAND] == alxBLANKING_VALUE)
    {
        if (v_r == alxBLANKING_VALUE)
        {
            magnitudes [alxR_BAND] = alxBLANKING_VALUE;
            confIndexes[alxR_BAND] = alxNO_CONFIDENCE;
        }
        else
        {
            magnitudes [alxR_BAND] = mgV - v_r;
            confIndexes[alxR_BAND] = alxCONFIDENCE_HIGH;
        }
    }
    else
    {
        confIndexes[alxR_BAND] = alxCONFIDENCE_HIGH;
    }

    if (magnitudes [alxI_BAND] == alxBLANKING_VALUE)
    {
        if (v_i == alxBLANKING_VALUE)
        {
            magnitudes [alxI_BAND] = alxBLANKING_VALUE;
            confIndexes[alxI_BAND] = alxNO_CONFIDENCE;
        }
        else
        {
            magnitudes [alxI_BAND] = mgV - v_i;
            confIndexes[alxI_BAND] = alxCONFIDENCE_HIGH;
        }
    }
    else
    {
        confIndexes[alxI_BAND] = alxCONFIDENCE_HIGH;
    }

    if (magnitudes [alxJ_BAND] == alxBLANKING_VALUE)
    {
        if ((magnitudes [alxI_BAND] == alxBLANKING_VALUE) ||
            (i_j == alxBLANKING_VALUE))
        {
            magnitudes [alxJ_BAND] = alxBLANKING_VALUE;
            confIndexes[alxJ_BAND] = alxNO_CONFIDENCE;
        }
        else
        {
            magnitudes [alxJ_BAND] = magnitudes [alxI_BAND] - i_j;
            confIndexes[alxJ_BAND] = alxCONFIDENCE_HIGH;
        }
    }
    else
    {
        confIndexes[alxJ_BAND] = alxCONFIDENCE_HIGH;
    }

    if (magnitudes [alxH_BAND] == alxBLANKING_VALUE)
    {
        if ((magnitudes [alxJ_BAND] == alxBLANKING_VALUE) ||
            (j_h == alxBLANKING_VALUE))
        {
            magnitudes [alxH_BAND] = alxBLANKING_VALUE;
            confIndexes[alxH_BAND] = alxNO_CONFIDENCE;
        }
        else
        {
            magnitudes [alxH_BAND] = magnitudes [alxJ_BAND] - j_h;
            confIndexes[alxH_BAND] = alxCONFIDENCE_HIGH;
        }
    }
    else
    {
        confIndexes[alxH_BAND] = alxCONFIDENCE_HIGH;
    }

    if (magnitudes [alxK_BAND] == alxBLANKING_VALUE)
    {
        if ((magnitudes [alxJ_BAND] == alxBLANKING_VALUE) ||
            (j_k == alxBLANKING_VALUE))
        {
            magnitudes [alxK_BAND] = alxBLANKING_VALUE;
            confIndexes[alxK_BAND] = alxNO_CONFIDENCE;
        }
        else
        {
            magnitudes [alxK_BAND] = magnitudes [alxJ_BAND] - j_k;
            confIndexes[alxK_BAND] = alxCONFIDENCE_HIGH;
        }
    }
    else
    {
        confIndexes[alxK_BAND] = alxCONFIDENCE_HIGH;
    }

    if (magnitudes [alxL_BAND] == alxBLANKING_VALUE)
    {
        if ((magnitudes [alxK_BAND] == alxBLANKING_VALUE) ||
            (k_l == alxBLANKING_VALUE))
        {
            magnitudes [alxL_BAND] = alxBLANKING_VALUE;
            confIndexes[alxL_BAND] = alxNO_CONFIDENCE;
        }
        else
        {
            magnitudes [alxL_BAND] = magnitudes [alxK_BAND] - k_l;
            confIndexes[alxL_BAND] = alxCONFIDENCE_HIGH;
        }
    }
    else
    {
        confIndexes[alxL_BAND] = alxCONFIDENCE_HIGH;
    }

    if (magnitudes [alxM_BAND] == alxBLANKING_VALUE)
    {
        if ((magnitudes [alxK_BAND] == alxBLANKING_VALUE) ||
            (k_m == alxBLANKING_VALUE))
        {
            magnitudes [alxM_BAND] = alxBLANKING_VALUE;
            confIndexes[alxM_BAND] = alxNO_CONFIDENCE;
        }
        else
        {
            magnitudes [alxM_BAND] = magnitudes [alxK_BAND] - k_m;
            confIndexes[alxM_BAND] = alxCONFIDENCE_HIGH;
        }
    }
    else
    {
        confIndexes[alxM_BAND] = alxCONFIDENCE_HIGH;
    }


    /* Print out results */
    logTest("B                = %0.3f", mgB);
    logTest("V                = %0.3f", mgV);
    logTest("R                = %0.3lf (%d)",
            magnitudes[alxR_BAND], confIndexes[alxR_BAND]);
    logTest("I                = %0.3lf (%d)",
            magnitudes[alxI_BAND], confIndexes[alxI_BAND]);
    logTest("J                = %0.3lf (%d)",
            magnitudes[alxJ_BAND], confIndexes[alxJ_BAND]);
    logTest("H                = %0.3lf (%d)",
            magnitudes[alxH_BAND], confIndexes[alxH_BAND]);
    logTest("K                = %0.3lf (%d)",
            magnitudes[alxK_BAND], confIndexes[alxK_BAND]);
    logTest("L                = %0.3lf (%d)",
            magnitudes[alxL_BAND], confIndexes[alxL_BAND]);
    logTest("M                = %0.3lf (%d)",
            magnitudes[alxM_BAND], confIndexes[alxM_BAND]);

    return mcsSUCCESS;
}

/*___oOo___*/
