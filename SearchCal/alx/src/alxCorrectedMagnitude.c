/*******************************************************************************
 * JMMC project
 * 
 * "@(#) $Id: alxCorrectedMagnitude.c,v 1.7 2006-04-07 06:15:03 gzins Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.6  2006/04/07 06:04:30  gzins
 * Added NO_CONFIDENCE when can not compute corrected magnitude
 *
 * Revision 1.5  2006/04/06 14:49:39  gzins
 * Fixed when computing ratio for interpolation.
 *
 * Revision 1.4  2006/03/03 14:48:24  scetre
 * Changed rcsId to rcsId __attribute__ ((unused))
 *
 * Revision 1.3  2006/01/26 12:48:25  scetre
 * Added test on blanking value in the color table
 *
 * Revision 1.2  2006/01/18 15:59:06  scetre
 * Updated Documentation
 *
 * Revision 1.1  2005/12/22 10:08:58  scetre
 * Added extinction coefficient computation
 * changed realMag to CorrectedMag
 *
 * Revision 1.14  2005/10/26 11:24:01  lafrasse
 * Code review
 *
 * Revision 1.13  2005/06/01 14:16:07  scetre
 * Changed logExtDbg to logTrace
 *
 * Revision 1.12  2005/04/04 07:22:11  scetre
 * Updated documentation
 *
 * Revision 1.11  2005/03/30 12:48:15  scetre
 * Added structure in order to simplify the code.
 * Changed API with this structure.
 * Share the code of the main function in sevral function in order to simplify the code
 *
 * Revision 1.10  2005/02/22 16:18:13  gzins
 * Updated misDynBufGetNextLine API
 *
 * Revision 1.9  2005/02/22 10:16:43  gzins
 * Removed \n in logTest message
 *
 * Revision 1.8  2005/02/22 08:08:59  gzins
 * Changed printf to logTest
 *
 * Revision 1.7  2005/02/21 19:35:45  gzins
 * Updated alxComputeRealMagnitudes to have API compatible with alxComputeRealMagnitudes and handle blanking values.
 *
 * Revision 1.6  2005/02/16 15:10:57  gzins
 * Updated call to miscDynBufGetNextLine()
 *
 * Revision 1.5  2005/02/12 15:13:55  gzins
 * Removed call to miscResolvePath; done by miscLocateFile
 *
 * Revision 1.4  2005/01/31 13:32:37  scetre
 * changed misc...Pointer in misc...
 *
 * Revision 1.3  2005/01/26 15:49:09  scetre
 * solve memory problem with miscDYN_BUF
 *
 * Revision 1.2  2005/01/24 10:56:25  scetre
 * Changed valid format for spectral type
 *
 * Revision 1.1  2005/01/21 08:14:25  gluck
 * Creation
 *
 * 
 * scetre    28-Sep-2004  Created
 * gzins     12-Jan-2005  - Updated to be compliant with programming standards
 *                        - Improved reliability and error handling
 *
 ******************************************************************************/

/**
 * @file
 * Function definition for corrected magnitude computation (i.e taking into
 * account interstellar absorption).
 *
 * @sa JMMC-MEM-2600-0008 document.
 */

static char *rcsId __attribute__ ((unused)) ="@(#) $Id: alxCorrectedMagnitude.c,v 1.7 2006-04-07 06:15:03 gzins Exp $"; 


/* 
 * System Headers
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>


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

static alxCOLOR_TABLE *
    alxGetColorTableForFaintStar(alxSPECTRAL_TYPE *spectralType);
    
static mcsLOGICAL alxIsBlankingValue(mcsFLOAT cellValue);
    
static mcsCOMPL_STAT alxString2SpectralType(mcsSTRING32        spType,
                                            alxSPECTRAL_TYPE  *spectralType);

static mcsINT32 alxGetLineForBrightStar(alxCOLOR_TABLE    *colorTable,
                                        alxSPECTRAL_TYPE  *spectralType,
                                        mcsSTRING32        spType);

static mcsINT32 alxGetLineForFaintStar(alxCOLOR_TABLE    *colorTable,
                                       alxSPECTRAL_TYPE  *spectralType,
                                       mcsSTRING32        spType,
                                       mcsFLOAT          diffMagJK);

static mcsCOMPL_STAT 
alxComputeDiffMagnitudeForBrightStar(mcsSTRING32                 spType,
                                     mcsFLOAT                    mgB,
                                     mcsFLOAT                    mgV,
                                     alxDIFFERENTIAL_MAGNITUDES  diffMagnitudes);

static mcsCOMPL_STAT 
alxComputeDiffMagnitudeForFaintStar(mcsSTRING32                 spType,
                                    mcsFLOAT                    mgJ,
                                    mcsFLOAT                    mgK,
                                    alxDIFFERENTIAL_MAGNITUDES  diffMagnitudes);

static mcsCOMPL_STAT
alxComputeAllMagnitudesForBrightStar(alxDIFFERENTIAL_MAGNITUDES  diffMagnitudes,
                                     alxMAGNITUDES               magnitudes,
                                     mcsFLOAT                    mgV);

static mcsCOMPL_STAT
alxComputeAllMagnitudesForFaintStar(alxDIFFERENTIAL_MAGNITUDES  diffMagnitudes,
                                    alxMAGNITUDES               magnitudes,
                                    mcsFLOAT                    mgJ);

static mcsCOMPL_STAT alxComputeMagnitude(mcsFLOAT              firstMag,
                                         alxDATA               diffMag,
                                         alxDATA              *magnitude,
                                         alxCONFIDENCE_INDEX   confIndex);

static alxEXTINCTION_RATIO_TABLE *alxGetExtinctionRatioTable(void);

/* 
 * Local functions definition
 */
/**
 * Return the extinction ratio for interstellar absorption computation .
 *
 * @return pointer onto structure containing extinction ratio table, or NULL if
 * an error occured.
 *
 * @usedfiles : alxExtinctionRatioTable.cfg : configuration file containing the
 * extinction ratio according to the color (i.e. magnitude band)
 */
static alxEXTINCTION_RATIO_TABLE* alxGetExtinctionRatioTable(void)
{ 
    logTrace("alxGetExtinctionRatioTable()");

    /*
     * Check if the structure extinctionRatioTable, where will be stored
     * extinction ratio to compute interstellar extinction, is loaded into
     * memory. If not load it.
     */
    static alxEXTINCTION_RATIO_TABLE extinctionRatioTable = 
        {mcsFALSE, "alxExtinctionRatioTable.cfg"};
    if (extinctionRatioTable.loaded == mcsTRUE)
    {
        return (&extinctionRatioTable);
    }
    
    /*
     * Reset all extinction ratio
     */
    int i;
    for (i=0; i<alxNB_BANDS; i++)
    {
        extinctionRatioTable.rc[i] = 0.0;
    }

    /* 
     * Build the dynamic buffer which will contain the file of extinction ratio
     */
    /* Find the location of the file */
    char *fileName;
    fileName = miscLocateFile(extinctionRatioTable.fileName);
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
    mcsINT32  lineNum = 0;
    const char *pos = NULL;
    mcsSTRING1024 line;
    while ((pos = miscDynBufGetNextLine
            (&dynBuf, pos, line, sizeof(mcsSTRING1024), mcsTRUE)) != NULL)
    {
        logDebug("miscDynBufGetNextLine() = '%s'", line);

        /* Trim line for leading and trailing characters */        
        miscTrimString (line, " ");

        /* If line is not empty */
        if (strlen(line) != 0)
        {
            /* Check if there is to many lines in file */
            if (lineNum >= alxNB_BANDS)
            {
                miscDynBufDestroy(&dynBuf);
                errAdd(alxERR_TOO_MANY_LINES, fileName);
                return NULL;
            }

            /* Get extinction ratio */
            char band;
            float rc;
            if (sscanf(line, "%c %f", &band, &rc) != 2)
            {
                miscDynBufDestroy(&dynBuf);
                errAdd(alxERR_WRONG_FILE_FORMAT, line, fileName);
                return NULL;
            }
            else
            {
                /* fitzpatrick identifier */
                alxBAND fitzId;
                switch toupper(band)
                {
                    case 'M':
                        fitzId = alxM_BAND;
                        break;

                    case 'L':
                        fitzId = alxL_BAND;
                        break;

                    case 'K':
                        fitzId = alxK_BAND;
                        break;

                    case 'H':
                        fitzId = alxH_BAND;
                        break;

                    case 'J':
                        fitzId = alxJ_BAND;
                        break;

                    case 'I':
                        fitzId = alxI_BAND;
                        break;

                    case 'R':
                        fitzId = alxR_BAND;
                        break;

                    case 'V':
                        fitzId = alxV_BAND;
                        break;

                    case 'B':
                        fitzId = alxB_BAND;
                        break;

                    default:
                        errAdd(alxERR_INVALID_BAND, band, fileName);
                        miscDynBufDestroy(&dynBuf);
                        return NULL;
                }

                /* Store read extinction ratio */
                if (extinctionRatioTable.rc[fitzId] == 0)
                {
                    extinctionRatioTable.rc[fitzId] = rc;
                }
                else
                {
                    errAdd(alxERR_DUPLICATED_LINE, line, fileName);
                    miscDynBufDestroy(&dynBuf);
                    return NULL;
                }
            }

            /* Next line */
            lineNum++;
        }
    }

    /* Destroy the dynamic buffer where is stored the file information */
    miscDynBufDestroy(&dynBuf);

    /* Check if there is missing line */
    if (lineNum != alxNB_BANDS)
    {
        errAdd(alxERR_MISSING_LINE, lineNum, alxNB_BANDS, fileName);
        return NULL;
    }

    extinctionRatioTable.loaded = mcsTRUE;

    return (&extinctionRatioTable);
}

/**
 * Return the color table corresponding to a given spectral type.
 *
 * This method determines the color table associated to the given spectral
 * type of a bright star, and reads (if not yet done) this table from
 * the configuration file.
 *
 * @param spectralType spectral type of the star 
 * 
 * @return pointer to structure containing color table, or NULL if an error
 * occured.
 *
 * @usedfiles Files containing the color indexes, the absolute magnitude in V
 * and the stellar mass according to the temperature class for different star
 * types. These tables are used to compute missing magnitudes.
 *  - alxColorTableForBrightDwarfStar.cfg : bright dwarf star 
 *  - alxColorTableForBrightGiantStar.cfg : bright giant star
 *  - alxColorTableForBrightSuperGiantStar.cfg : bright super giant star
 */
static alxCOLOR_TABLE *alxGetColorTableForBrightStar
                                                 (alxSPECTRAL_TYPE spectralType)
{
    logTrace("alxGetColorTableForBrightStar()");

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
     * Check if the structure in which polynomial coefficients will be stored is
     * loaded into memory. If not load it.
     */
    if (colorTables[starType].loaded == mcsTRUE)
    {
        return (&colorTables[starType]);
    }

    /* Find the location of the file */
    char* fileName = miscLocateFile(colorTables[starType].fileName);
    if (fileName == NULL)
    {
        return NULL;
    }
    
    /* Load file. Comment lines start with '#' */
    miscDYN_BUF dynBuf;
    miscDynBufInit(&dynBuf);    
    logDebug("Loading %s ...", fileName); 
    if (miscDynBufLoadFile(&dynBuf, fileName, "#") == mcsFAILURE)
    {
        miscDynBufDestroy(&dynBuf);
        return NULL;
    }

    /* For each line of the loaded file*/
    mcsINT32 lineNum = 0;
    const char *pos = NULL;
    mcsSTRING1024 line;
    while ((pos = miscDynBufGetNextLine(&dynBuf, pos, line, sizeof(line),
                                        mcsTRUE)) != NULL)
    {
        logDebug("miscDynBufGetNextLine() = '%s'", line);

        /* Trim line for any leading and trailing blank characters */
        miscTrimString (line, " ");

        /* If line is not empty */
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
                       &colorTables[starType].index[lineNum][0].value,
                       &colorTables[starType].index[lineNum][1].value,
                       &colorTables[starType].index[lineNum][2].value,
                       &colorTables[starType].index[lineNum][3].value,
                       &colorTables[starType].index[lineNum][4].value,
                       &colorTables[starType].index[lineNum][5].value,
                       &colorTables[starType].index[lineNum][6].value,
                       &colorTables[starType].index[lineNum][7].value)
                != (alxNB_DIFF_MAG + 1))
            {
                /* destroy dynamic buffer used and return an error */
                miscDynBufDestroy(&dynBuf);
                errAdd(alxERR_WRONG_FILE_FORMAT, line, fileName);
                return NULL;
            }

            colorTables[starType].index[lineNum][0].isSet = 
                alxIsBlankingValue(colorTables[starType].
                                           index[lineNum][0].value);
            colorTables[starType].index[lineNum][1].isSet = 
                alxIsBlankingValue(colorTables[starType].
                                           index[lineNum][1].value);
            colorTables[starType].index[lineNum][2].isSet = 
                alxIsBlankingValue(colorTables[starType].
                                           index[lineNum][2].value);
            colorTables[starType].index[lineNum][3].isSet = 
                alxIsBlankingValue(colorTables[starType].
                                           index[lineNum][3].value);
            colorTables[starType].index[lineNum][4].isSet = 
                alxIsBlankingValue(colorTables[starType].
                                           index[lineNum][4].value);
            colorTables[starType].index[lineNum][5].isSet = 
                alxIsBlankingValue(colorTables[starType].
                                           index[lineNum][5].value);
            colorTables[starType].index[lineNum][6].isSet = 
                alxIsBlankingValue(colorTables[starType].
                                           index[lineNum][6].value);
            colorTables[starType].index[lineNum][7].isSet = 
                alxIsBlankingValue(colorTables[starType].
                                           index[lineNum][7].value);

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
 * Say if a cell value is defined or not.
 *
 * If the cell value = '99.99'(alxBLANKING_VALUE), return false.
 *
 * @param cellValue the value of the cell
 *
 * @return mcsTRUE if cell value != alxBLANKING_VALUE, otherwise mcsFALSE is 
 * returned.
 */
static mcsLOGICAL alxIsBlankingValue(mcsFLOAT cellValue)
{
    logTrace("alxIsBlankingValue()");
    
    if (cellValue == (mcsFLOAT)alxBLANKING_VALUE)
    {
        return mcsFALSE;
    }

    return mcsTRUE;
}

/**
 * Return the color table corresponding to a given spectral type.
 *
 * This method determines the color table associated to the given spectral
 * type of a bright star, and reads (if not yet done) this table from
 * the configuration file.
 *
 * 
 * @return pointer to structure containing color table, or NULL if an error
 * occured.
 *
 * @usedfiles Files containing the color indexes, the absolute magnitude in V
 * and the stellar mass according to the temperature class for different star
 * types. These tables are used to compute missing magnitudes.
 *  - alxColorTableForFaintDwarfStar.cfg : faint dwarf star 
 */
static alxCOLOR_TABLE *
alxGetColorTableForFaintStar(alxSPECTRAL_TYPE *spectralType)
{
    logTrace("alxGetColorTableForFaintStar()");

    /* Color table for the different type of stars */
    static alxCOLOR_TABLE colorTables[alxNB_STAR_TYPES] = {
        {mcsFALSE, "alxColorTableForFaintDwarfStar.cfg"},
        {mcsFALSE, "alxColorTableForFaintGiantStar.cfg"},
        {mcsFALSE, "alxColorTableForFaintSuperGiantStar.cfg"},
    };

    /* Determination of star type according to the spectral type */
    alxSTAR_TYPE starType;
        
    if (spectralType == NULL)
    {
        /* If no spectral type are defined, by default, starType is DWARF */
        starType = alxDWARF;
    }
    else
    {
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
        if (spectralType->lightClass[0] == 'I')
        {
            if (spectralType->lightClass[1] == 'I')
            {
                /* 
                 * case light class =
                 * II, II/III, III, III/IV
                 */
                starType = alxGIANT;
            }
            else if (spectralType->lightClass[1] == 'V')
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
        else if (spectralType->lightClass[0] == 'V')
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
     * Check if the structure in which polynomial coefficients will be stored is
     * loaded into memory. If not load it.
     */
    if (colorTables[starType].loaded == mcsTRUE)
    {
        return (&colorTables[starType]);
    }

    /* Find the location of the file */
    char* fileName = miscLocateFile(colorTables[starType].fileName);
    if (fileName == NULL)
    {
        return NULL;
    }
    
    /* Load file. Comment lines start with '#' */
    miscDYN_BUF dynBuf;
    miscDynBufInit(&dynBuf);    
    logDebug("Loading %s ...", fileName); 
    if (miscDynBufLoadFile(&dynBuf, fileName, "#") == mcsFAILURE)
    {
        miscDynBufDestroy(&dynBuf);
        return NULL;
    }

    /* For each line of the loaded file*/
    mcsINT32 lineNum = 0;
    const char *pos = NULL;
    mcsSTRING1024 line;
    while ((pos = miscDynBufGetNextLine(&dynBuf, pos, line, sizeof(line),
                                        mcsTRUE)) != NULL)
    {
        logDebug("miscDynBufGetNextLine() = '%s'", line);

        /* Trim line for any leading and trailing blank characters */
        miscTrimString (line, " ");

        /* If line is not empty */
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
                       &colorTables[starType].index[lineNum][0].value,
                       &colorTables[starType].index[lineNum][1].value,
                       &colorTables[starType].index[lineNum][2].value,
                       &colorTables[starType].index[lineNum][3].value,
                       &colorTables[starType].index[lineNum][4].value,
                       &colorTables[starType].index[lineNum][5].value,
                       &colorTables[starType].index[lineNum][6].value,
                       &colorTables[starType].index[lineNum][7].value)
                != (alxNB_DIFF_MAG + 1))
            {
                /* destroy dynamic buffer used and return an error */
                miscDynBufDestroy(&dynBuf);
                errAdd(alxERR_WRONG_FILE_FORMAT, line, fileName);
                return NULL;
            }
            
            colorTables[starType].index[lineNum][0].isSet = 
                alxIsBlankingValue(colorTables[starType].
                                           index[lineNum][0].value);
            colorTables[starType].index[lineNum][1].isSet = 
                alxIsBlankingValue(colorTables[starType].
                                           index[lineNum][1].value);
            colorTables[starType].index[lineNum][2].isSet = 
                alxIsBlankingValue(colorTables[starType].
                                           index[lineNum][2].value);
            colorTables[starType].index[lineNum][3].isSet = 
                alxIsBlankingValue(colorTables[starType].
                                           index[lineNum][3].value);
            colorTables[starType].index[lineNum][4].isSet = 
                alxIsBlankingValue(colorTables[starType].
                                           index[lineNum][4].value);
            colorTables[starType].index[lineNum][5].isSet = 
                alxIsBlankingValue(colorTables[starType].
                                           index[lineNum][5].value);
            colorTables[starType].index[lineNum][6].isSet = 
                alxIsBlankingValue(colorTables[starType].
                                           index[lineNum][6].value);
            colorTables[starType].index[lineNum][7].isSet = 
                alxIsBlankingValue(colorTables[starType].
                                           index[lineNum][7].value);
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
 * Create a spectral type structure from a string.
 *
 * @param spType spectral type string
 * @param spectralType spectral type structure to create
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is
 * returned.
 */
static mcsCOMPL_STAT alxString2SpectralType(mcsSTRING32        spType,
                                            alxSPECTRAL_TYPE  *spectralType)
{
    logTrace("alxString2SpectralType()");

    /* 
     * Get each part of the spectral type XN.NLLL where X is a letter, N.N a
     * number between 0 and 9 and LLL is the light class
     */
    mcsINT32 nbItems = sscanf(spType, "%c%f%s", &(spectralType->code), 
                             &spectralType->quantity, spectralType->lightClass);
    if ((nbItems != 2) && (nbItems != 3))
    {
        /*
         * if an error occured during sscanf (i.e nbItems != 2 or 3), 
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
 * Get the line in the color table which matches the star spectral type or the
 * line just after if interpolation is need
 *
 * @param colorTable color table 
 * @param spectralType spectral type 
 * @param spType string of the spectral
 * 
 * @return a line number that matches the star spectral type or the line just
 * after or -1 if no match is found or interpolation is impossible
 */
static mcsINT32 alxGetLineForBrightStar(alxCOLOR_TABLE    *colorTable,
                                        alxSPECTRAL_TYPE  *spectralType,
                                        mcsSTRING32        spType)
{
    logTrace("alxGetLineForBrightStar()");

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
            else /* Else go to the next line */
            {
                line++;         
            }   

            /* the code of the spectral type had been found */
            codeFound = mcsTRUE;
        }
        else /* Spectral type code doesn't match */
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
            else /* Else go to the next line */
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
 * Get the line in the color table which matches the star magnitude difference
 * between J an K or the line just after if interpolation is need. In this case
 * the founs tag will be false
 *
 * @param colorTable color table 
 * @param found logical to know if the line is found 
 * @param spType string of the spectral
 * 
 * @return a line number that matches the star magnitude difference
 * between J an K or the line just after or -1 if no match is found or
 * interpolation is impossible
 */
static mcsINT32 alxGetLineForFaintStar(alxCOLOR_TABLE    *colorTable,
                                       alxSPECTRAL_TYPE  *spectralType,
                                       mcsSTRING32       spType,
                                       mcsFLOAT          diffMagJK)
{
    logTrace("alxGetLineForFaintStar()");

    mcsINT32 line = 0;

    if (spectralType == NULL) 
    {
        mcsLOGICAL found = mcsFALSE; 
        while ((found == mcsFALSE) && (line < colorTable->nbLines))
        {
            /* If J-K == diff mag */
            if (colorTable->index[line][alxJ_K].value == diffMagJK)
            {
                found = mcsTRUE;
            }
            /* J-K > diff mag*/
            else if (colorTable->index[line][alxJ_K].value > diffMagJK)
            {
                if (line == 0)
                {
                    errAdd(alxERR_NO_LINE_FOUND);
                    return -1;
                }
                else
                {
                    found = mcsTRUE;
                }
            }
            else 
            {
                line++;
            }
        }

        if (found == mcsFALSE)
        {
            errAdd(alxERR_DIFFJK_NOT_IN_TABLE);
            return -1;
        }
    }
    else
    {
        mcsLOGICAL codeFound = mcsFALSE; 
        mcsLOGICAL found = mcsFALSE; 
        while ((found == mcsFALSE) && (line < colorTable->nbLines))
        {
            /* If the spectral type code match */
            if (colorTable->spectralType[line].code ==  spectralType->code)
            {
                /*
                 * And quantities match or star quantity is lower than the one 
                 * of the current line
                 */
                if (colorTable->spectralType[line].quantity >=
                    spectralType->quantity)
                {
                    /* Stop search */
                    found = mcsTRUE;
                }
                else /* Else go to the next line */
                {
                    line++;         
                }   

                /* the code of the spectral type had been found */
                codeFound = mcsTRUE;
            }
            else /* Spectral type code doesn't match */
            {
                /*
                 * If the lines corresponding to the star spectral type code 
                 * have been scanned
                 */
                if (codeFound == mcsTRUE)
                {
                    /* Stop search */
                    found = mcsTRUE;
                }
                else /* Else go to the next line */
                {
                    line++;
                }
            }
        }

        /*
         * Check if spectral type is out of the table; i.e. before the first 
         * entry in the color table. The quantity is strictly lower than the
         * first entry of the table 
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
    }

    /* return the line found */
    return line;
}

/**
 * Compute differential magnitudes.
 *
 * @param spType spectral type structure
 * @param mgB magnitude in B band
 * @param mgV magnitude in V band
 * @param diffMag differential magnitudes
 *
 * @return mcsSUCCESS on successfull completion otherwise, mcsFAILURE is
 * returned
 */
static mcsCOMPL_STAT 
alxComputeDiffMagnitudeForBrightStar(mcsSTRING32                spType,
                        mcsFLOAT                   mgB,
                        mcsFLOAT                   mgV,
                        alxDIFFERENTIAL_MAGNITUDES diffMagnitudes)
{
    logTrace("alxComputeDiffMagnitudeForBrightStar()");
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
    mcsINT32 line = alxGetLineForBrightStar(colorTable, &spectralType, spType);
    /* if line not found, i.e = -1, return mcsFAILURE */
    if (line == -1)
    {
        return mcsFAILURE;
    }
    /* If the spectral type matches the line of the color table */
    diffMagnitudes[alxB_V].isSet = mcsFALSE;
    diffMagnitudes[alxV_I].isSet = mcsFALSE;
    diffMagnitudes[alxV_R].isSet = mcsFALSE;
    diffMagnitudes[alxI_J].isSet = mcsFALSE;
    diffMagnitudes[alxJ_H].isSet = mcsFALSE;
    diffMagnitudes[alxJ_K].isSet = mcsFALSE;
    diffMagnitudes[alxK_L].isSet = mcsFALSE;
    diffMagnitudes[alxK_M].isSet = mcsFALSE;

    if (colorTable->spectralType[line].quantity == spectralType.quantity)
    {
        /* 
         * Compare B-V star differential magnitude to the one of the color table
         * line; delta should be less than +/- 0.1 
         */
        if (fabs((mgB-mgV) - colorTable->index[line][alxB_V].value) <= 0.11)
        {
            /* Get differential magnitudes */
            diffMagnitudes[alxB_V].value =
                colorTable->index[line][alxB_V].value;
            diffMagnitudes[alxB_V].isSet = mcsTRUE;
            diffMagnitudes[alxV_I].value =
                colorTable->index[line][alxV_I].value;
            diffMagnitudes[alxV_I].isSet = mcsTRUE;
            diffMagnitudes[alxV_R].value =
                colorTable->index[line][alxV_R].value;
            diffMagnitudes[alxV_R].isSet = mcsTRUE;
            diffMagnitudes[alxI_J].value =
                colorTable->index[line][alxI_J].value;
            diffMagnitudes[alxI_J].isSet = mcsTRUE;
            diffMagnitudes[alxJ_H].value =
                colorTable->index[line][alxJ_H].value;
            diffMagnitudes[alxJ_H].isSet = mcsTRUE;
            diffMagnitudes[alxJ_K].value =
                colorTable->index[line][alxJ_K].value;
            diffMagnitudes[alxJ_K].isSet = mcsTRUE;
            diffMagnitudes[alxK_L].value =
                colorTable->index[line][alxK_L].value;
            diffMagnitudes[alxK_L].isSet = mcsTRUE;
            if ((colorTable->index[line][alxK_L].isSet != mcsFALSE) &&
                (colorTable->index[line][alxL_M].isSet != mcsFALSE))
            {
                diffMagnitudes[alxK_M].value = 
                      colorTable->index[line][alxK_L].value 
                    + colorTable->index[line][alxL_M].value;
            }
        }
        else
        {
            logTest("mgB-mgV = %.8e / B-V %.8e; delta > 0.1",
                    (mgB-mgV), colorTable->index[line][alxB_V].value);
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
        if ((fabs((mgB-mgV) - colorTable->index[lineSup][alxB_V].value) <= 0.1) ||
            (fabs((mgB-mgV) - colorTable->index[lineInf][alxB_V].value) <= 0.1))
        {
            /* Compute ratio for interpolation */
            ratio = fabs(((mgB-mgV) - colorTable->index[lineInf][alxB_V].value) 
                         / (colorTable->index[lineSup][alxB_V].value 
                            - colorTable->index[lineInf][alxB_V].value));
            logTest("Ratio = %f", ratio);
            
            /* Compute differential magnitudes */
            if ((colorTable->index[lineSup][alxV_R].isSet != mcsFALSE) &&
                (colorTable->index[lineInf][alxV_R].isSet != mcsFALSE))
            {
                diffMagnitudes[alxV_R].value = 
                    colorTable->index[lineInf][alxV_R].value
                    + ratio * (colorTable->index[lineSup][alxV_R].value 
                               - colorTable->index[lineInf][alxV_R].value);
            diffMagnitudes[alxV_R].isSet = mcsTRUE;
            }

            if ((colorTable->index[lineSup][alxV_I].isSet != mcsFALSE) &&
                (colorTable->index[lineInf][alxV_I].isSet != mcsFALSE))
            {
                diffMagnitudes[alxV_I].value = 
                    colorTable->index[lineInf][alxV_I].value
                    + ratio *(colorTable->index[lineSup][alxV_I].value 
                              - colorTable->index[lineInf][alxV_I].value);
            diffMagnitudes[alxV_I].isSet = mcsTRUE;
            }

            if ((colorTable->index[lineSup][alxI_J].isSet != mcsFALSE) &&
                (colorTable->index[lineInf][alxI_J].isSet != mcsFALSE))
            {
                diffMagnitudes[alxI_J].value =
                    colorTable->index[lineInf][alxI_J].value
                    + ratio *(colorTable->index[lineSup][alxI_J].value 
                              - colorTable->index[lineInf][alxI_J].value);
            diffMagnitudes[alxI_J].isSet = mcsTRUE;
            }

            if ((colorTable->index[lineSup][alxJ_H].isSet != mcsFALSE) &&
                (colorTable->index[lineInf][alxJ_H].isSet != mcsFALSE))
            {
                diffMagnitudes[alxJ_H].value = 
                    colorTable->index[lineInf][alxJ_H].value 
                    + ratio *(colorTable->index[lineSup][alxJ_H].value 
                              - colorTable->index[lineInf][alxJ_H].value);
            diffMagnitudes[alxJ_H].isSet = mcsTRUE;
            }

            if ((colorTable->index[lineSup][alxJ_K].isSet != mcsFALSE) &&
                (colorTable->index[lineInf][alxJ_K].isSet != mcsFALSE))
            {
                diffMagnitudes[alxJ_K].value =
                    colorTable->index[lineInf][alxJ_K].value 
                    + ratio *(colorTable->index[lineSup][alxJ_K].value 
                              - colorTable->index[lineSup][alxJ_K].value);
            diffMagnitudes[alxJ_K].isSet = mcsTRUE;
            }

            if ((colorTable->index[lineSup][alxK_L].isSet != mcsFALSE) &&
                (colorTable->index[lineInf][alxK_L].isSet != mcsFALSE))
            {
                diffMagnitudes[alxK_L].value =
                    colorTable->index[lineInf][alxK_L].value
                    + ratio * (colorTable->index[lineSup][alxK_L].value 
                               - colorTable->index[lineInf][alxK_L].value);
            diffMagnitudes[alxK_L].isSet = mcsTRUE;
            }

            if ((colorTable->index[lineSup][alxK_L].isSet != mcsFALSE) &&
                (colorTable->index[lineInf][alxK_L].isSet != mcsFALSE) &&
                (colorTable->index[lineSup][alxL_M].isSet != mcsFALSE) &&
                (colorTable->index[lineInf][alxL_M].isSet != mcsFALSE))
            {
                diffMagnitudes[alxK_M].value =
                    colorTable->index[lineInf][alxK_L].value 
                    + colorTable->index[lineInf][alxL_M].value 
                    + ratio *(colorTable->index[lineSup][alxK_L].value 
                              + colorTable->index[lineSup][alxL_M].value 
                              - colorTable->index[lineInf][alxK_L].value 
                              - colorTable->index[lineInf][alxL_M].value);
            diffMagnitudes[alxK_M].isSet = mcsTRUE;
            }
        }
        else
        {
            logTest("mgB-mgV = %.3f / B-V [%.3f..%.3f]; delta > 0.1",
                    (mgB-mgV), colorTable->index[lineInf][alxB_V].value, 
                    colorTable->index[lineSup][alxB_V].value);

        }
    }
    
    return mcsSUCCESS;
}

/**
 * Compute differential magnitudes in faint.
 *
 * @param mgB magnitude in B band
 * @param mgV magnitude in V band
 * @param diffMag differential magnitudes
 *
 * @return mcsSUCCESS on successfull completion otherwise, mcsFAILURE is
 * returned
 */
static mcsCOMPL_STAT
alxComputeDiffMagnitudeForFaintStar(mcsSTRING32                spType,
                                    mcsFLOAT                   mgJ,
                                    mcsFLOAT                   mgK,
                                    alxDIFFERENTIAL_MAGNITUDES diffMagnitudes)
{
    logTrace("alxComputeDiffMagnitudeForFaintStar()");

    alxSPECTRAL_TYPE *spectralType = malloc(sizeof(alxSPECTRAL_TYPE));
    if (spType != NULL)
    {
        /* 
         * Get each part of the spectral type XN.NLLL where X is a letter, N.N a
         * number between 0 and 9 and LLL is the light class
         */
        if (alxString2SpectralType(spType, spectralType) == mcsFAILURE)
        {
            return mcsFAILURE;
        }
    }
    else
    {
        spectralType = NULL;
    }
    
    /* Get the color table according to the spectral type of the star */
    alxCOLOR_TABLE *colorTable;
    colorTable = alxGetColorTableForFaintStar(spectralType);
    if (colorTable == NULL)
    {
        return mcsFAILURE;
    }
    /* Line corresponding to the spectral type */
    mcsINT32 line = alxGetLineForFaintStar(colorTable, spectralType, spType,
                                           mgJ-mgK);
    /* if line not found, i.e = -1, return mcsFAILURE */
    if (line == -1)
    {
        return mcsFAILURE;
    }
    /* If the spectral type matches the line of the color table */
    diffMagnitudes[alxB_V].isSet = mcsFALSE;
    diffMagnitudes[alxV_I].isSet = mcsFALSE;
    diffMagnitudes[alxV_R].isSet = mcsFALSE;
    diffMagnitudes[alxI_J].isSet = mcsFALSE;
    diffMagnitudes[alxJ_H].isSet = mcsFALSE;

    if (colorTable->index[line][alxJ_K].value == (mgJ - mgK))
    {
        /* Get differential magnitudes */
        diffMagnitudes[alxB_V].value = colorTable->index[line][alxB_V].value;
        diffMagnitudes[alxB_V].isSet = mcsTRUE;
        diffMagnitudes[alxV_I].value = colorTable->index[line][alxV_I].value;
        diffMagnitudes[alxV_I].isSet = mcsTRUE;
        diffMagnitudes[alxV_R].value = colorTable->index[line][alxV_R].value;
        diffMagnitudes[alxV_R].isSet = mcsTRUE;
        diffMagnitudes[alxI_J].value = colorTable->index[line][alxI_J].value;
        diffMagnitudes[alxI_J].isSet = mcsTRUE;
        diffMagnitudes[alxJ_H].value = colorTable->index[line][alxJ_H].value;
        diffMagnitudes[alxJ_H].isSet = mcsTRUE;

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
        logTest("%f < J-K (%f) < %f", 
                colorTable->index[lineInf][alxJ_K].value,
                mgJ-mgK,
                colorTable->index[lineSup][alxJ_K].value);
        /* Compute ratio for interpolation */
        if (colorTable->index[lineSup][alxJ_K].value != 
            colorTable->index[lineInf][alxJ_K].value)
        {
            ratio = fabs(((mgJ-mgK) - colorTable->index[lineInf][alxJ_K].value) 
                         / (colorTable->index[lineSup][alxJ_K].value 
                            - colorTable->index[lineInf][alxJ_K].value));
        }
        /* If ratio can be computed, return failure */
        else
        {
            ratio = 0.5;
        }
        logTest("Ratio = %f", ratio);

        /* Compute differential magnitudes */

        if ((colorTable->index[lineSup][alxV_R].isSet != mcsFALSE) &&
            (colorTable->index[lineInf][alxV_R].isSet != mcsFALSE))
        {
            diffMagnitudes[alxV_R].value =  
                colorTable->index[lineInf][alxV_R].value
                + ratio * (colorTable->index[lineSup][alxV_R].value 
                           - colorTable->index[lineInf][alxV_R].value);
            diffMagnitudes[alxV_R].isSet = mcsTRUE;
        }

        if ((colorTable->index[lineSup][alxV_I].isSet != mcsFALSE) &&
            (colorTable->index[lineInf][alxV_I].isSet != mcsFALSE))
        {
            diffMagnitudes[alxV_I].value = 
                (-1) * (colorTable->index[lineInf][alxV_I].value                                        + ratio *
                        (colorTable->index[lineSup][alxV_I].value
                         - colorTable->index[lineInf][alxV_I].value));
            diffMagnitudes[alxV_I].isSet = mcsTRUE;
        }

        if ((colorTable->index[lineSup][alxI_J].isSet != mcsFALSE) &&
            (colorTable->index[lineInf][alxI_J].isSet != mcsFALSE))
        {
            diffMagnitudes[alxI_J].value = 
                (-1) * (colorTable->index[lineInf][alxI_J].value
                        + ratio *
                        (colorTable->index[lineSup][alxI_J].value 
                         - colorTable->index[lineInf][alxI_J].value));
            diffMagnitudes[alxI_J].isSet = mcsTRUE;
        }

        if ((colorTable->index[lineSup][alxJ_H].isSet != mcsFALSE) &&
            (colorTable->index[lineInf][alxJ_H].isSet != mcsFALSE))
        {
            diffMagnitudes[alxJ_H].value = 
                colorTable->index[lineInf][alxJ_H].value 
                + ratio *(colorTable->index[lineSup][alxJ_H].value 
                          - colorTable->index[lineInf][alxJ_H].value);
            diffMagnitudes[alxJ_H].isSet = mcsTRUE;
        }

        if ((colorTable->index[lineSup][alxB_V].isSet != mcsFALSE) &&
            (colorTable->index[lineInf][alxB_V].isSet != mcsFALSE))
        {
            diffMagnitudes[alxB_V].value = 
                (-1) * (colorTable->index[lineInf][alxB_V].value 
                        + ratio *
                        (colorTable->index[lineSup][alxB_V].value 
                         - colorTable->index[lineSup][alxB_V].value));
            diffMagnitudes[alxB_V].isSet = mcsTRUE;
        }
    }

    free(spectralType);
    
    return mcsSUCCESS;
}

/**
 * Compute magnitude and set confidence index of a specific magnitude
 * 
 * @param magnitude magnitude to modify
 * @param firstMag first magnitude of the calcul (X-Y)
 * @param diffMag difference magnitude (ex : V-K)
 * @param confIndex the confidence index to set to the modify magnitude if
 * necessary
 * 
 * @return always SUCCESS
 */
static mcsCOMPL_STAT alxComputeMagnitude(mcsFLOAT firstMag,
                                         alxDATA diffMag,
                                         alxDATA *magnitude,
                                         alxCONFIDENCE_INDEX confIndex)
{
    logTrace("alxDiffMagnitude()");
    /* If magnitude is not set */
    if (magnitude->isSet == mcsFALSE)
    {
        /* If magnitude difference needed is equal to blanking value */
        if (diffMag.isSet != mcsFALSE)
        {
            /* Else compute*/
            magnitude->value = firstMag - diffMag.value;
            /* Set correct confidence index */
            magnitude->confIndex = confIndex;
            magnitude->isSet = mcsTRUE;
        }
        else
        {
            magnitude->confIndex = alxNO_CONFIDENCE;
        }
    }
    
    return mcsSUCCESS;
}

/**
 * Compute missing magnitude according to the differential magnitudes
 *
 * @param diffMag diffrential magnitudes
 * @param magnitudes all magnitudes bands
 * @param mgV magnitude in V band
 *
 * @return always mcsSUCCESS
 */
static mcsCOMPL_STAT 
alxComputeAllMagnitudesForBrightStar(alxDIFFERENTIAL_MAGNITUDES diffMagnitudes,
                                     alxMAGNITUDES magnitudes,
                                     mcsFLOAT mgV)
{
    logTrace("alxComputeAllMagnitudesForBrightStar()");

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
                        diffMagnitudes[alxV_R],
                        &(magnitudes[alxR_BAND]),
                        confIndex);
    alxComputeMagnitude(mgV,
                        diffMagnitudes[alxV_I],
                        &(magnitudes[alxI_BAND]),
                        confIndex);
    alxComputeMagnitude(magnitudes[alxI_BAND].value,
                        diffMagnitudes[alxI_J],
                        &(magnitudes[alxJ_BAND]),
                        confIndex);
    alxComputeMagnitude(magnitudes[alxJ_BAND].value,
                        diffMagnitudes[alxJ_H],
                        &(magnitudes[alxH_BAND]),
                        confIndex);
    alxComputeMagnitude(magnitudes[alxJ_BAND].value,
                        diffMagnitudes[alxJ_K],
                        &(magnitudes[alxK_BAND]),
                        confIndex);
    alxComputeMagnitude(magnitudes[alxK_BAND].value,
                        diffMagnitudes[alxK_L],
                        &(magnitudes[alxL_BAND]),
                        confIndex);
    alxComputeMagnitude(magnitudes[alxK_BAND].value,
                        diffMagnitudes[alxK_M],
                        &(magnitudes[alxM_BAND]),
                        confIndex);

    return mcsSUCCESS;
}

/**
 * Compute missing magnitude according to the differential magnitudes
 *
 * @param diffMag diffrential magnitudes
 * @param magnitudes all magnitudes bands
 * @param mgJ magnitude in J band
 *
 * @return always mcsSUCCESS
 */
static mcsCOMPL_STAT 
alxComputeAllMagnitudesForFaintStar(alxDIFFERENTIAL_MAGNITUDES diffMagnitudes,
                                    alxMAGNITUDES magnitudes,
                                    mcsFLOAT mgJ)
{
    logTrace("alxComputeAllMagnitudesForFaintStar()");
    
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
    /* Compute *missing* magnitudes in R, I, J, H, K, bands */
    alxComputeMagnitude(mgJ,
                        diffMagnitudes[alxJ_H],
                        &(magnitudes[alxH_BAND]),
                        confIndex);
    alxComputeMagnitude(mgJ,
                        diffMagnitudes[alxI_J],
                        &(magnitudes[alxI_BAND]),
                        confIndex);
    alxComputeMagnitude(magnitudes[alxI_BAND].value,
                        diffMagnitudes[alxV_I],
                        &(magnitudes[alxV_BAND]),
                        confIndex);
    alxComputeMagnitude(magnitudes[alxV_BAND].value,
                        diffMagnitudes[alxV_R],
                        &(magnitudes[alxR_BAND]),
                        confIndex);
    alxComputeMagnitude(magnitudes[alxV_BAND].value,
                        diffMagnitudes[alxB_V],
                        &(magnitudes[alxB_BAND]),
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
 * If magnitude in B or V are unknown, no magnitude will be computed.
 * otherwise, if K band is unkwown, the confidence index of
 * computed values is set to LOW, otherwise (K known) it is set to 
 * HIGH.
 * If magnitude can not be computed, its associated confidence index is set to
 * NO CONFIDENCE.
 *
 * @param spType spectral type
 * @param magnitudes contains magnitudes in B and V bands, and the computed
 * magnitudes in R, I, J, H, K, L and M bands
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is
 * returned.
 */
mcsCOMPL_STAT 
alxComputeMagnitudesForBrightStar(mcsSTRING32 spType,
                                  alxMAGNITUDES magnitudes)
{ 
    logTrace("alxComputeMagnitudesForBrightStar()");

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
    if (alxComputeDiffMagnitudeForBrightStar(spType, mgB, mgV, diffMag) ==
        mcsFAILURE)
    {
        return mcsFAILURE;
    }

    /* Compute all new magnitude */
    alxComputeAllMagnitudesForBrightStar(diffMag, magnitudes, mgV);
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

/**
 * Compute magnitudes in R, I, J, H, K, L and M bands for faint star.
 *
 * It computes magnitudes in R, I, J, H, K, L and M bands according to the
 * magnitudes in J and K bands for a faint star.
 * If magnitude in J and K band is unkwown, the confidence index of computed
 * values is set to LOW, otherwise (J and K known) it is set to HIGH.
 * If magnitude can not be computed, its associated confidence index is set to
 * NO CONFIDENCE.
 *
 * @param spType spectral type
 * @param magnitudes contains magnitudes in B and V bands, and the computed
 * magnitudes in R, I, J, H, K, L and M bands
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is
 * returned.
 */
mcsCOMPL_STAT 
alxComputeMagnitudesForFaintStar(mcsSTRING32 spType,
                                 alxMAGNITUDES magnitudes)
{ 
    logTrace("alxComputeMagnitudesForFaintStar()");

    /* 
     * If magnitude B or V are not set, return SUCCESS : the alxMAGNITUDE
     * structure will not be changed -> the magnitude won't be computed 
     */
    if ((magnitudes[alxJ_BAND].isSet == mcsFALSE) ||
        (magnitudes[alxK_BAND].isSet == mcsFALSE))
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
    mcsFLOAT mgJ, mgK;
    mgJ = magnitudes[alxJ_BAND].value;
    mgK = magnitudes[alxK_BAND].value;
    
    /* Create a differential magnitudes structure */
    alxDIFFERENTIAL_MAGNITUDES diffMag;
    /* Compute differential magnitude */
    if (alxComputeDiffMagnitudeForFaintStar(spType, mgJ, mgK, diffMag) ==
        mcsFAILURE)
    {
        return mcsFAILURE;
    }
    /* Compute all new magnitude */
    alxComputeAllMagnitudesForFaintStar(diffMag, magnitudes, mgJ);
    
    /* Print out results */
    logTest("J = %0.3f", mgJ);
    logTest("K = %0.3f", mgK);
    logTest("R = %0.3lf (%d)",
            magnitudes[alxR_BAND].value,
            magnitudes[alxR_BAND].confIndex);
    logTest("I = %0.3lf (%d)",
            magnitudes[alxI_BAND].value,
            magnitudes[alxI_BAND].confIndex);
    logTest("B = %0.3lf (%d)",
            magnitudes[alxB_BAND].value,
            magnitudes[alxB_BAND].confIndex);
    logTest("H = %0.3lf (%d)",
            magnitudes[alxH_BAND].value,
            magnitudes[alxH_BAND].confIndex);
    logTest("V = %0.3lf (%d)",
            magnitudes[alxV_BAND].value,
            magnitudes[alxV_BAND].confIndex);
    logTest("L = %0.3lf (%d)",
            magnitudes[alxL_BAND].value,
            magnitudes[alxL_BAND].confIndex);
    logTest("M = %0.3lf (%d)",
            magnitudes[alxM_BAND].value,
            magnitudes[alxM_BAND].confIndex);

    return mcsSUCCESS;
}

/**
 * Compute corrected magnitudes according to the interstellar absorption and the
 * corrected ones.
 *
 * The corrected magnitudes are computed using the galactic interstellar
 * extinction model based on galatic coordinates and distance of the star.
 *
 * @param av the extinction ratio
 * @param magnitudes computed magnitudes in B, V, R, I, J, H, K, L and M bands
 * 
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is
 * returned.
 */
/* WARNING: the computation of corrected magnitudes based on interstellar
 * absorption, described in JMMC-MEM-2600-0008 document (section 2.1.1), has not
 * been implemented because there is no catalog which provides this information
 * for a representative number of stars.
 */
mcsCOMPL_STAT alxComputeCorrectedMagnitudes(mcsFLOAT      av,
                                            alxMAGNITUDES magnitudes)
{
    logTrace("alxComputeRealMagnitudes()");

    /* Get extinction ratio table */
    alxEXTINCTION_RATIO_TABLE *extinctionRatioTable;
    extinctionRatioTable = alxGetExtinctionRatioTable();
    if (extinctionRatioTable == NULL)
    {
        return mcsFAILURE;        
    }

    /* 
     * Computed corrected magnitudes.
     * Co = C - Ac
     * where Ac = Av*Rc/Rv, with Rv=3.10
     * If the pointer of a magnitude is NULL its means that there is nothing
     * to compute. In this case, do nothing
     */
    int band;
    for (band = alxB_BAND; band <= alxM_BAND; band++)
    {
        if (magnitudes[band].isSet == mcsTRUE)
        {
            magnitudes[band].value = magnitudes[band].value 
                - (av * extinctionRatioTable->rc[band] / 3.10);
        
            logTest("Corrected magnitude[%d] = %0.3f", band,
                    magnitudes[band].value); 
        }
    }

    return mcsSUCCESS;
}


/*___oOo___*/
