/*******************************************************************************
 * JMMC project
 * 
 * "@(#) $Id: alxMagnitude.c,v 1.16 2005-04-04 07:22:11 scetre Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.15  2005/03/30 12:48:10  scetre
 * Added structure in order to simplify the code.
 * Changed API with this structure.
 * Share the code of the main function in sevral function in order to simplify the code
 *
 * Revision 1.14  2005/03/04 17:51:02  scetre
 * Changed logInfo to logTest
 *
 * Revision 1.13  2005/03/02 17:11:10  gzins
 * Set confidence index to alxCONFIDENCE_LOW when K-band magnitude is unknown.
 *
 * Revision 1.12  2005/02/25 15:13:09  gluck
 * Changed 2 logTest to logInfo to have them in test outputs
 *
 * Revision 1.11  2005/02/22 16:20:13  gzins
 * Updated misDynBufGetNextLine API
 * Added check of B-V before computing missing magnitude; delta between V-B (star) and B-V (color table) must be less than 0.1
 *
 * Revision 1.10  2005/02/22 10:16:10  gzins
 * Fixed bug; only computed missing magnitudes.
 *
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

static char *rcsId="@(#) $Id: alxMagnitude.c,v 1.16 2005-04-04 07:22:11 scetre Exp $"; 
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
static alxCOLOR_TABLE *
    alxGetColorTableForBrightStar(alxSPECTRAL_TYPE spectralType);
static mcsCOMPL_STAT alxString2SpectralType(mcsSTRING32 spType,
                                          alxSPECTRAL_TYPE *spectralType);
static mcsINT32 alxGetLine(alxCOLOR_TABLE *colorTable,
                           alxSPECTRAL_TYPE *spectralType,
                           mcsSTRING32 spType);
static mcsCOMPL_STAT 
    alxComputeDiffMagnitude(mcsSTRING32 spType,
                            mcsFLOAT mgB,
                            mcsFLOAT mgV,
                            alxDIFFERENTIAL_MAGNITUDES *diffMagnitudes);
static mcsCOMPL_STAT
    alxComputeAllMagnitudes(alxDIFFERENTIAL_MAGNITUDES diffMagnitudes,
                            alxMAGNITUDES magnitudes,
                            mcsFLOAT mgV);
static mcsCOMPL_STAT alxComputeMagnitude(mcsFLOAT firstMag,
                                         mcsFLOAT diffMag,
                                         alxMAGNITUDE *magnitude,
                                         alxCONFIDENCE_INDEX confIndex);
/* 
 * Local functions definition
 */

/**
 * Return the color table corresponding to a given spectral type.
 *
 * This method determines the color table associated to the given spectral
 * type of bright star, and reads (if not yet done) this table from
 * the configuration file.
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
             * IV, IV/V
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
         * V, V/VI, VI
         */
        starType = alxDWARF;
    }
    else 
    {
        /* case no light class */
        starType = alxDWARF;
    }

    /* Print out type of star */
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
    
    /* Load file. Comment lines started with '#' */
    miscDYN_BUF dynBuf;
    miscDynBufInit(&dynBuf);    
    if (miscDynBufLoadFile(&dynBuf, fileName, "#") == mcsFAILURE)
    {
        miscDynBufDestroy(&dynBuf);
        return NULL;
    }

    /* For each line */
    mcsINT32 lineNum = 0;
    const char *pos = NULL;
    mcsSTRING1024 line;
    while ((pos = miscDynBufGetNextLine
            (&dynBuf, pos, line, sizeof(mcsSTRING1024), mcsTRUE)) != NULL)
    {
        logDebug("miscDynBufGetNextLine() = '%s'", line);

        /* If line is not empty */
        /* Trim line for leading and trailing characters */
        miscTrimString (line, " ");
        if (strlen(line) != 0)
        {
            /* Check if there are to many lines in file */
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
                /* destroy dynamic buffer used and return an error */
                miscDynBufDestroy(&dynBuf);
                errAdd(alxERR_WRONG_FILE_FORMAT, line, fileName);
                return NULL;
            }

            /* Next line */
            lineNum++;
        }
    }
    /* Destroy dynamic buffer used */
    miscDynBufDestroy(&dynBuf);

    /* Set the number of line in the color table */
    colorTables[starType].nbLines = lineNum;
    /* Set to "loaded" the structure table */
    colorTables[starType].loaded = mcsTRUE;

    return &(colorTables[starType]);
}

/**
 * Create a spectral type structure from a string
 *
 * \param spType spectral type string
 * \param spectralType spectral type structure to create
 *
 * \return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is
 * returned.
 */
static mcsCOMPL_STAT alxString2SpectralType(mcsSTRING32 spType,
                               alxSPECTRAL_TYPE *spectralType)
{
    logExtDbg("alxString2SpectralType()");

    /* 
     * Get each part of the spectral type XN.NLLL where X is a letter, N.N a
     * number between 0 and 9 and LLL is the light class
     */
    mcsINT32 nbItems;
    nbItems = sscanf(spType, "%c%f%s",
                     &(spectralType->code), 
                     &spectralType->quantity, 
                     spectralType->lightClass);
    if ((nbItems != 2) && (nbItems != 3))
    {
        /*
         * if an error occurs during sscanf (i.e nbItems != 2 or 3), 
         * return error
         */
        errAdd(alxERR_WRONG_SPECTRAL_TYPE_FORMAT, spType);
        return mcsFAILURE;
    }

    /* If there is no light class in given spectral type, reset it */
    if (nbItems == 2)
    {
        strcpy(spectralType->lightClass, ""); 
    }

    logTest("Type spectral = %s", spType);
    logTest("\tCode              = %c", spectralType->code);
    logTest("\tSub-type Quantity = %f", spectralType->quantity);
    logTest("\tLight class       = %s", spectralType->lightClass);

    /* return the pointer on the created spectral type structure */
    return mcsSUCCESS;
}

/**
 * Get the line in the color table which matches with the star spectral
 * type or the line just after if interpolation is need
 *
 * \param colorTable color table 
 * \param spectralType spectral type 
 * \param spType string of the spectral
 * 
 * \return a line number which matches with the star spectral type or the line
 * just after or -1 if no match is found or interpolation is impossible
 */
static mcsINT32 alxGetLine(alxCOLOR_TABLE *colorTable,
                         alxSPECTRAL_TYPE *spectralType,
                         mcsSTRING32 spType)
{
    logExtDbg("alxGetLine()");

    mcsLOGICAL codeFound = mcsFALSE; 
    mcsLOGICAL found = mcsFALSE; 
    mcsINT32 line = 0;
    while ((found == mcsFALSE) && (line < colorTable->nbLines))
    {
        /* If the spectral type code match */
        if (colorTable->spectralType[line].code ==  spectralType->code)
        {
            /*
             * And quantities match or star quantity is lower than the one of
             * the current line
             */
            if (colorTable->spectralType[line].quantity>=spectralType->quantity)
            {
                /* Stop search */
                found = mcsTRUE;
            }
            /* Else go to the next line */
            else
            {
                line++;         
            }   
            /* the code of the spectral type had benn found */
            codeFound = mcsTRUE;
        }
        /* Else spectral type code doesn't match */
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
     * Check if spectral type is out of the table; i.e. before the first entry
     * in the color table. The quantity is strictly lower than the first entry
     * of the table 
     */
    if ((line == 0) && 
        (colorTable->spectralType[line].quantity != spectralType->quantity))
    {
        found = mcsFALSE;
    }

    /* If spectral type not found in color table, return error */
    if (found != mcsTRUE)
    {
        errAdd(alxERR_SPECTRAL_TYPE_NOT_FOUND, spType);
        return -1;
    }

    /* return the line found */
    return line;
}

/**
 * Compute differential magnitudes
 *
 * \param spType spectral type structure
 * \param mgB magnitude in B band
 * \param mgV magnitude in V band
 * \param diffMag differential magnitudes
 *
 * \return mcsSUCCESS on successfull completion otherwise, mcsFAILURE is
 * returned
 */
static mcsCOMPL_STAT 
    alxComputeDiffMagnitude(mcsSTRING32 spType,
                            mcsFLOAT mgB,
                            mcsFLOAT mgV,
                            alxDIFFERENTIAL_MAGNITUDES *diffMagnitudes)
{
    logExtDbg("alxComputeDiffMagnitude()");

    /* 
     * Get each part of the spectral type XN.NLLL where X is a letter, N.N a
     * number between 0 and 9 and LLL is the light class
     */
    alxSPECTRAL_TYPE spectralType;
    if (alxString2SpectralType(spType, &spectralType) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    /* Get the color table according to the spectral type of the star */
    alxCOLOR_TABLE *colorTable;
    colorTable = alxGetColorTableForBrightStar(spectralType);
    if (colorTable == NULL)
    {
        return mcsFAILURE;
    }

    /* Line corresponding to the spectral type */
    mcsINT32 line = alxGetLine(colorTable, &spectralType, spType);
    /* if line not found, i.e = -1, return mcsFAILURE */
    if (line == -1)
    {
        return mcsFAILURE;
    }

    /* If the spectral type matches the line of the color table */
    diffMagnitudes->b_v = alxBLANKING_VALUE;
    diffMagnitudes->v_i = alxBLANKING_VALUE;
    diffMagnitudes->v_r = alxBLANKING_VALUE;
    diffMagnitudes->i_j = alxBLANKING_VALUE;
    diffMagnitudes->j_h = alxBLANKING_VALUE;
    diffMagnitudes->j_k = alxBLANKING_VALUE;
    diffMagnitudes->k_l = alxBLANKING_VALUE;
    diffMagnitudes->k_m = alxBLANKING_VALUE;

    if (colorTable->spectralType[line].quantity == spectralType.quantity)
    {
        /* 
         * Compare B-V star differential magnitude to the one of the color table
         * line; delta should be less than +/- 0.1 
         */
        if (fabs((mgB-mgV) - colorTable->index[line][alxB_V]) <= 0.1)
        {
            /* Get differential magnitudes */
            diffMagnitudes->b_v = colorTable->index[line][alxB_V];
            diffMagnitudes->v_i = colorTable->index[line][alxV_I];
            diffMagnitudes->v_r = colorTable->index[line][alxV_R];
            diffMagnitudes->i_j = colorTable->index[line][alxI_J];
            diffMagnitudes->j_h = colorTable->index[line][alxJ_H];
            diffMagnitudes->j_k = colorTable->index[line][alxJ_K];
            diffMagnitudes->k_l = colorTable->index[line][alxK_L];
            if ((colorTable->index[line][alxK_L] != alxBLANKING_VALUE) &&
                (colorTable->index[line][alxL_M] != alxBLANKING_VALUE))
            {
                diffMagnitudes->k_m = colorTable->index[line][alxK_L] + 
                                      colorTable->index[line][alxL_M];
            }
        }
        else
        {
            logTest("mgB-mgV = %.3f / B-V %.3f; delta > 0.1",
                    (mgB-mgV), colorTable->index[line][alxB_V]);
        }
    }
    /* Else, interpolate */
    else
    {
        mcsFLOAT ratio; /* need to compute ratio */
        mcsINT32 lineInf, lineSup; /* integer to have the lines sup and inf */
        lineSup = line;
        lineInf = line - 1;
        logTest("Inferior line = %d", lineInf);
        logTest("Superior line = %d", lineSup);
        
        /*
         * Compare B-V star differential magnitude to the ones of the color
         * table inferior/superior lines; delta should be less than +/- 0.1 
         */
        if ((fabs((mgB-mgV) - colorTable->index[lineSup][alxB_V]) <= 0.1) ||
            (fabs((mgB-mgV) - colorTable->index[lineInf][alxB_V]) <= 0.1))
        {
            /* Compute ratio for interpolation */
            ratio = fabs(((mgB-mgV) - colorTable->index[lineInf][alxB_V]) 
                         / (colorTable->index[lineSup][alxB_V] 
                            - colorTable->index[lineInf][alxB_V]));
            logTest("Ratio = %f", ratio);

            /* Compute differential magnitudes */
            if ((colorTable->index[lineSup][alxV_R] != alxBLANKING_VALUE) &&
                (colorTable->index[lineInf][alxV_R] != alxBLANKING_VALUE))
            {
                diffMagnitudes->v_r = colorTable->index[lineInf][alxV_R]
                    + ratio * (colorTable->index[lineSup][alxV_R] 
                               - colorTable->index[lineInf][alxV_R]);
            }

            if ((colorTable->index[lineSup][alxV_I] != alxBLANKING_VALUE) &&
                (colorTable->index[lineInf][alxV_I] != alxBLANKING_VALUE))
            {
                diffMagnitudes->v_i = colorTable->index[lineInf][alxV_I] 
                    + ratio *(colorTable->index[lineSup][alxV_I] 
                              - colorTable->index[lineInf][alxV_I]);
            }

            if ((colorTable->index[lineSup][alxI_J] != alxBLANKING_VALUE) &&
                (colorTable->index[lineInf][alxI_J] != alxBLANKING_VALUE))
            {
                diffMagnitudes->i_j = colorTable->index[lineInf][alxI_J] 
                    + ratio *(colorTable->index[lineSup][alxI_J] 
                              - colorTable->index[lineInf][alxI_J]);
            }

            if ((colorTable->index[lineSup][alxJ_H] != alxBLANKING_VALUE) &&
                (colorTable->index[lineInf][alxJ_H] != alxBLANKING_VALUE))
            {
                diffMagnitudes->j_h = colorTable->index[lineInf][alxJ_H] 
                    + ratio *(colorTable->index[lineSup][alxJ_H] 
                              - colorTable->index[lineInf][alxJ_H]);
            }

            if ((colorTable->index[lineSup][alxJ_K] != alxBLANKING_VALUE) &&
                (colorTable->index[lineInf][alxJ_K] != alxBLANKING_VALUE))
            {
                diffMagnitudes->j_k = colorTable->index[lineInf][alxJ_K] 
                    + ratio *(colorTable->index[lineSup][alxJ_K] 
                              - colorTable->index[lineSup][alxJ_K]);
            }

            if ((colorTable->index[lineSup][alxK_L] != alxBLANKING_VALUE) &&
                (colorTable->index[lineInf][alxK_L] != alxBLANKING_VALUE))
            {
                diffMagnitudes->k_l = colorTable->index[lineInf][alxK_L]
                    + ratio * (colorTable->index[lineSup][alxK_L] 
                               - colorTable->index[lineInf][alxK_L]);
            }

            if ((colorTable->index[lineSup][alxK_L] != alxBLANKING_VALUE) &&
                (colorTable->index[lineInf][alxK_L] != alxBLANKING_VALUE) &&
                (colorTable->index[lineSup][alxL_M] != alxBLANKING_VALUE) &&
                (colorTable->index[lineInf][alxL_M] != alxBLANKING_VALUE))
            {
                diffMagnitudes->k_m = colorTable->index[lineInf][alxK_L] 
                    + colorTable->index[lineInf][alxL_M] 
                    + ratio *(colorTable->index[lineSup][alxK_L] 
                              + colorTable->index[lineSup][alxL_M] 
                              - colorTable->index[lineInf][alxK_L] 
                              - colorTable->index[lineInf][alxL_M]);
            }
        }
        else
        {
            logTest("mgB-mgV = %.3f / B-V [%.3f..%.3f]; delta > 0.1",
                    (mgB-mgV), colorTable->index[lineInf][alxB_V], 
                    colorTable->index[lineSup][alxB_V]);
        }

    }

    return mcsSUCCESS;
}

/**
 * Compute magnitude and set confidence index of a specific magnitude
 * 
 * \param magnitude magnitude to modify
 * \param firstMag first magnitude of the calcul (X-Y)
 * \param diffMag difference magnitude (ex : V-K)
 * \param confIndex the confidence index to set to the modify magnitude if
 * necessary
 * 
 * \return always SUCCESS
 */
static mcsCOMPL_STAT alxComputeMagnitude(mcsFLOAT firstMag,
                                         mcsFLOAT diffMag,
                                         alxMAGNITUDE *magnitude,
                                         alxCONFIDENCE_INDEX confIndex)
{
    logExtDbg("alxDiffMagnitude()");
    
    /* If magnitude is set */
    if (magnitude->isSet == mcsFALSE)
    {
        /* If magnitude difference needed is equal to blanking value */
        if (diffMag == alxBLANKING_VALUE)
        {
            /* Set no confidence index */
            magnitude->confIndex = alxNO_CONFIDENCE;
        }
        else
        {
            /* Else compute*/
            magnitude->value = firstMag - diffMag;
            /* Set correct confidence index */
            magnitude->confIndex = confIndex;
        }
    }
    
    return mcsSUCCESS;
}

/**
 * Compute missing magnitude according to the differential magnitudes
 *
 * \param diffMag diffrential magnitudes
 * \param magnitudes all magnitudes bands
 * \param mgV magnitude in V band
 *
 * \return always mcsSUCCESS
 */
static mcsCOMPL_STAT 
    alxComputeAllMagnitudes(alxDIFFERENTIAL_MAGNITUDES diffMagnitudes,
                            alxMAGNITUDES magnitudes,
                            mcsFLOAT mgV)
{
    logExtDbg("alxComputeAllMagnitudes()");
    
    /* Set confidence index for computed values */
    /* If magnitude in K band is unknown, set confidence index to Low */
    alxCONFIDENCE_INDEX confIndex;
    if (magnitudes[alxK_BAND].isSet == mcsFALSE)
    {
        confIndex = alxCONFIDENCE_LOW;
    }
    /* Else B, V and K is known */
    else
    {
        confIndex = alxCONFIDENCE_HIGH;
    }

    /* Compute *missing* magnitudes in R, I, J, H, K, L and M bands */
    alxComputeMagnitude(mgV,
                        diffMagnitudes.v_r,
                        &(magnitudes[alxR_BAND]),
                        confIndex);
    alxComputeMagnitude(mgV,
                        diffMagnitudes.v_i,
                        &(magnitudes[alxI_BAND]),
                        confIndex);
    alxComputeMagnitude(magnitudes[alxI_BAND].value,
                        diffMagnitudes.i_j,
                        &(magnitudes[alxJ_BAND]),
                        confIndex);
    alxComputeMagnitude(magnitudes[alxJ_BAND].value,
                        diffMagnitudes.j_h,
                        &(magnitudes[alxH_BAND]),
                        confIndex);
    alxComputeMagnitude(magnitudes[alxJ_BAND].value,
                        diffMagnitudes.j_k,
                        &(magnitudes[alxK_BAND]),
                        confIndex);
    alxComputeMagnitude(magnitudes[alxK_BAND].value,
                        diffMagnitudes.k_l,
                        &(magnitudes[alxL_BAND]),
                        confIndex);
    alxComputeMagnitude(magnitudes[alxK_BAND].value,
                        diffMagnitudes.k_m,
                        &(magnitudes[alxM_BAND]),
                        confIndex);

    return mcsSUCCESS;
}

/*
 * Public functions definition
 */

/**
 * Compute magnitudes in R, I, J, H, K, L and M bands for bright star.
 *
 * It computes magnitudes in R, I, J, H, K, L and M bands according to the
 * spectral type and the magnitudes in B and V bands for a bright star.
 * If magnitude in K-band is unkwown, the confidence index of computed values is
 * set to LOW, otherwise (B, V and K known) it is set to HIGH.
 * If magnitude can not be computed, its associated confidence index is set to
 * NO CONFIDENCE.
 *
 * \param spType spectral type
 * \param magnitudes contains magnitudes in B and V bands, and the computed
 * magnitudes in R, I, J, H, K, L and M bands
 *
 * \return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is
 * returned.
 */
mcsCOMPL_STAT 
alxComputeMagnitudesForBrightStar(mcsSTRING32 spType,
                                  alxMAGNITUDES magnitudes)
{ 
    logExtDbg("alxComputeMagnitudesForBrightStar()");

    /* 
     * If magnitude B or V are not set, return SUCCESS : the alxMAGNITUDE
     * structure will not be changed -> the magnitude won't be computed 
     */
    if ((magnitudes[alxB_BAND].isSet == mcsFALSE) ||
        (magnitudes[alxV_BAND].isSet == mcsFALSE))
    {
        int i;
        /* Set to low the confidence index of each unknown magnitude */
        for (i=0; i<alxNB_BANDS; i++)
        {
            /* 
             * If the band is not affected , write alxCONFIDENCE_LOW into the
             * confidence index table 
             */
            if (magnitudes[i].isSet == mcsFALSE)
            {
                magnitudes[i].confIndex = alxCONFIDENCE_LOW;
            }
        }
        return mcsSUCCESS;
    }
    /* If B and V are affected, get magnitudes in B and V bands */
    mcsFLOAT mgB, mgV;
    mgB = magnitudes[alxB_BAND].value;
    mgV = magnitudes[alxV_BAND].value;
    
    /* Create a differential magnitudes structure */
    alxDIFFERENTIAL_MAGNITUDES diffMag;
    /* Compute differential magnitude */
    if (alxComputeDiffMagnitude(spType, mgB, mgV, &diffMag) ==
        mcsFAILURE)
    {
        return mcsFAILURE;
    }
    
    /* Compute all new magnitude */
    alxComputeAllMagnitudes(diffMag, magnitudes, mgV);
    
    /* Print out results */
    logTest("B = %0.3f", mgB);
    logTest("V = %0.3f", mgV);
    logTest("R = %0.3lf (%d)",
            magnitudes[alxR_BAND].value,
            magnitudes[alxR_BAND].confIndex);
    logTest("I = %0.3lf (%d)",
            magnitudes[alxI_BAND].value,
            magnitudes[alxI_BAND].confIndex);
    logTest("J = %0.3lf (%d)",
            magnitudes[alxJ_BAND].value,
            magnitudes[alxJ_BAND].confIndex);
    logTest("H = %0.3lf (%d)",
            magnitudes[alxH_BAND].value,
            magnitudes[alxH_BAND].confIndex);
    logTest("K = %0.3lf (%d)",
            magnitudes[alxK_BAND].value,
            magnitudes[alxK_BAND].confIndex);
    logTest("L = %0.3lf (%d)",
            magnitudes[alxL_BAND].value,
            magnitudes[alxL_BAND].confIndex);
    logTest("M = %0.3lf (%d)",
            magnitudes[alxM_BAND].value,
            magnitudes[alxM_BAND].confIndex);

    return mcsSUCCESS;
}

/*___oOo___*/
