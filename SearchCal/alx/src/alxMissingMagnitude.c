/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * @file
 * Function definition for missing magnitude computation.
 *
 * @sa JMMC-MEM-2600-0008 document.
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

/* Needed for FP_NAN support */
#define  __USE_ISOC99 1
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

/* delta threshold to ensure differential magnitude is correct to compute missing magnitudes (0.1) */
/* TODO FIXME: where is this value coming from (make it larger and then check if diameters are coherent ?) */
/* 0.11 <> 0.1 (much larger than machine precision but changing it impacts a lot results */
#define DELTA_THRESHOLD 0.11

/*
 * Local Functions declaration
 */

static alxCOLOR_TABLE* alxGetColorTableForStar(alxSPECTRAL_TYPE* spectralType);

static alxSTAR_TYPE alxGetLuminosityClass(alxSPECTRAL_TYPE* spectralType);

static mcsINT32 alxGetLineFromSpectralType(alxCOLOR_TABLE *colorTable,
                                           alxSPECTRAL_TYPE *spectralType);

static mcsINT32 alxGetLineFromValue(alxCOLOR_TABLE *colorTable,
                                    mcsDOUBLE diffMag,
                                    mcsINT32 diffMagId);

static mcsCOMPL_STAT alxComputeMagnitude(mcsDOUBLE firstMag,
                                         alxDATA diffMag,
                                         mcsDOUBLE factor,
                                         alxDATA *magnitude,
                                         alxCONFIDENCE_INDEX confIndex);

static mcsCOMPL_STAT alxInterpolateDiffMagnitude(alxCOLOR_TABLE *colorTable,
                                                 mcsINT32 lineInf,
                                                 mcsINT32 lineSup,
                                                 mcsDOUBLE magDiff,
                                                 mcsINT32 magDiffId,
                                                 alxDIFFERENTIAL_MAGNITUDES diffMagnitudes);

/*
 * Local functions definition
 */

/**
 * Return the luminosity class (alxDWARF, alxGIANT or alxSUPER_GIANT)
 * @param spectralType (optional) spectral type structure
 * @return alxDWARF by default
 */
static alxSTAR_TYPE alxGetLuminosityClass(alxSPECTRAL_TYPE* spectralType)
{
    /* Default value of DWARF TO BE CONFIRMED! */
    alxSTAR_TYPE starType = alxDWARF;

    /* If no spectral type given or wrong format */
    if ((spectralType == NULL) || (spectralType->isSet == mcsFALSE))
    {
        logTest("Type of star = DWARF (by default as no Spectral Type provided)");
        return starType;
    }

    /* If no luminosity class given */
    if (strlen(spectralType->luminosityClass) == 0)
    {
        logTest("Type of star = DWARF (by default as no Luminosity Class provided)");
        return starType;
    }

    /* Determination of star type according to the spectral type */
    static char* spectralTypes[] = {"VIII", "VII", "VI", "III-IV", "III/IV", "IV-III", "IV/III",
                                    "II-III", "II/III", "I-II", "I/II", "III", "IB-II", "IB/II",
                                    "IBV", "II", "IV", "V", "(I)", "IA-O/IA", "IA-O", "IA/AB",
                                    "IAB-B", "IAB", "IA", "IB", "I",
                                    NULL};
    static alxSTAR_TYPE luminosityClasses[] = {alxDWARF, alxDWARF, alxDWARF, alxGIANT, alxGIANT,
                                               alxGIANT, alxGIANT, alxGIANT, alxGIANT, alxSUPER_GIANT,
                                               alxSUPER_GIANT, alxGIANT, alxSUPER_GIANT, alxSUPER_GIANT,
                                               alxSUPER_GIANT, alxGIANT, alxDWARF, alxDWARF,
                                               alxSUPER_GIANT, alxSUPER_GIANT, alxSUPER_GIANT,
                                               alxSUPER_GIANT, alxSUPER_GIANT, alxSUPER_GIANT,
                                               alxSUPER_GIANT, alxSUPER_GIANT, alxSUPER_GIANT};

    const char* luminosityClass = spectralType->luminosityClass;
    mcsUINT32 index = 0;
    while (spectralTypes[index] != NULL)
    {
        /* If the current spectral type is found */
        if (strstr(luminosityClass, spectralTypes[index]) != NULL)
        {
            /* Get the corresponding luminoisity class */
            starType = luminosityClasses[index];
            break;
        }
        index++;
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

    return starType;
}

/**
 * Return the color table corresponding to a given spectral type.
 *
 * This method determines the color table associated to the given spectral
 * type of a bright star, and reads (if not yet done) this table from
 * the configuration file.
 *
 * @param spectralType (optional) spectral type structure
 * @param isBright mcsTRUE for bright stars, mcsFALSE for faint ones.
 *
 * @return pointer to structure containing color table, or NULL if an error
 * occured.
 *
 * @usedfiles Files containing the color indexes, the absolute magnitude in V
 * and the stellar mass according to the temperature class for different star
 * types. These tables are used to compute missing magnitudes.
 *  - alxColorTableForDwarfStar.cfg : dwarf star 
 *  - see code for other tables! 
 */
static alxCOLOR_TABLE*
alxGetColorTableForStar(alxSPECTRAL_TYPE* spectralType)
{
    /* Existing ColorTables */
    static alxCOLOR_TABLE colorTables[alxNB_STAR_TYPES] = {
        {mcsFALSE, "alxColorTableForDwarfStar.cfg"},
        {mcsFALSE, "alxColorTableForGiantStar.cfg"},
        {mcsFALSE, "alxColorTableForSuperGiantStar.cfg"},
    };

    /* Determination of star type according to the given star type */
    alxSTAR_TYPE starType = alxGetLuminosityClass(spectralType);

    alxCOLOR_TABLE* colorTable = &colorTables[starType];

    /*
     * Check if the structure in which polynomial coefficients will be stored is
     * loaded into memory. If not load it.
     */
    if (colorTable->loaded == mcsTRUE)
    {
        return colorTable;
    }

    /* Find the location of the file */
    char* fileName = miscLocateFile(colorTable->fileName);
    if (fileName == NULL)
    {
        return NULL;
    }

    /* Load file (skipping comment lines starting with '#') */
    miscDYN_BUF dynBuf;
    miscDynBufInit(&dynBuf);

    logInfo("Loading %s ...", fileName);

    NULL_DO(miscDynBufLoadFile(&dynBuf, fileName, "#"),
            miscDynBufDestroy(&dynBuf);
            free(fileName));

    /* For each line of the loaded file */
    mcsINT32 lineNum = 0;
    const char *pos = NULL;
    mcsSTRING1024 line;

    while ((pos = miscDynBufGetNextLine(&dynBuf, pos, line, sizeof (line), mcsTRUE)) != NULL)
    {
        logTrace("miscDynBufGetNextLine() = '%s'", line);

        /* Trim line for any leading and trailing blank characters */
        miscTrimString(line, " ");

        /* If line is not empty */
        if (strlen(line) != 0)
        {
            /* Check if there are to many lines in file */
            if (lineNum >= alxNB_SPECTRAL_TYPES)
            {
                /* Destroy the temporary dynamic buffer, raise an error and return */
                miscDynBufDestroy(&dynBuf);
                errAdd(alxERR_TOO_MANY_LINES, fileName);
                free(fileName);
                return NULL;
            }

            /* Try to read each polynomial coefficients */
            mcsDOUBLE values[alxNB_COLOR_INDEXES];
            mcsINT32 nbOfReadTokens = sscanf(line, "%c%lf %lf %lf %lf %lf %lf %lf %lf %lf",
                                             &colorTable->spectralType[lineNum].code,
                                             &colorTable->spectralType[lineNum].quantity,
                                             &values[0],
                                             &values[1],
                                             &values[2],
                                             &values[3],
                                             &values[4],
                                             &values[5],
                                             &values[6],
                                             &values[7]);

            /* If parsing went wrong */
            if (nbOfReadTokens != (alxNB_DIFF_MAG + 1))
            {
                /* Destroy the temporary dynamic buffer, raise an error and return */
                miscDynBufDestroy(&dynBuf);
                errAdd(alxERR_WRONG_FILE_FORMAT, line, fileName);
                free(fileName);
                return NULL;
            }

            /* Set whether each colorTable cell has been written or not */
            int i;
            for (i = 0; i < alxNB_COLOR_INDEXES; i++)
            {
                alxDATA* colorTableCell = &(colorTable->index[lineNum][i]);

                mcsDOUBLE value = values[i];

                colorTableCell->value = value;
                colorTableCell->isSet = (alxIsBlankingValue(value) == mcsFALSE) ? mcsTRUE : mcsFALSE;
            }

            /* Next line */
            lineNum++;
        }
    }

    /* Set the total number of lines in the color table */
    colorTable->nbLines = lineNum;

    /* Mark the color table as "loaded" */
    colorTable->loaded = mcsTRUE;

    /* Destroy the temporary dynamic buffer used to parse the color table file */
    miscDynBufDestroy(&dynBuf);
    free(fileName);

    /* Return a pointer on the freshly loaded color table */
    return colorTable;
}

/**
 * Get the line in the color table which matches the spectral type
 * (if provided) or the magnitude difference. Return the line 
 * just after if interpolation is need. 
 *
 * @param colorTable color table 
 * @param spectralType (optional) spectral type structure
 * @param diffMag differential magnitude
 * @param diffMagId id of the differential mag in colorTable
 * 
 * @return a line number that matches the spectral type or the
 * star magnitude difference or the line just after or -1 if
 * no match is found or interpolation is impossible
 */
static mcsINT32 alxGetLineFromValue(alxCOLOR_TABLE *colorTable,
                                    mcsDOUBLE diffMag,
                                    mcsINT32 diffMagId)
{
    mcsINT32 line = 0;
    mcsLOGICAL found = mcsFALSE;

    while ((found == mcsFALSE) && (line < colorTable->nbLines))
    {
        /* If diffMag in table == diffMag */
        if (colorTable->index[line][diffMagId].value == diffMag)
        {
            found = mcsTRUE;
        }
            /* If diffMag in table > diffMag */
        else if (colorTable->index[line][diffMagId].value > diffMag)
        {
            if (line == 0)
            {
                return alxNOT_FOUND;
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
        return alxNOT_FOUND;
    }

    /* return the line found */
    return line;
}

/**
 * Get the line in the color table which matches the spectral type
 * (if provided) or the magnitude difference. Return the line 
 * egual or just after. 
 *
 * @param colorTable color table 
 * @param spectralType (optional) spectral type structure
 * @param diffMag differential magnitude
 * @param diffMagId id of the differential mag in colorTable
 * 
 * @return a line number that matches the spectral type or the
 * star magnitude difference or the line just after or -1 if
 * no match is found or interpolation is impossible
 */
static mcsINT32 alxGetLineFromSpectralType(alxCOLOR_TABLE *colorTable,
                                           alxSPECTRAL_TYPE *spectralType)
{
    /* If spectral type is unknown, return not found */
    if ((spectralType == NULL) || (spectralType->isSet == mcsFALSE))
    {
        return alxNOT_FOUND;
    }

    mcsINT32 line = 0;
    mcsLOGICAL codeFound = mcsFALSE;
    mcsLOGICAL found = mcsFALSE;

    while ((found == mcsFALSE) && (line < colorTable->nbLines))
    {
        /* If the spectral type code match */
        if (colorTable->spectralType[line].code == spectralType->code)
        {
            /*
             * And quantities match or star quantity is lower than the one 
             * of the current line -> stop search
             */
            if (colorTable->spectralType[line].quantity >= spectralType->quantity)
            {
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
             * have been scanned -> stop
             */
            if (codeFound == mcsTRUE)
            {
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
    if ((line == 0) && (colorTable->spectralType[line].quantity != spectralType->quantity))
    {
        found = mcsFALSE;
    }

    /* If spectral type not found in color table, return -1 (not found) */
    if (found == mcsFALSE)
    {
        logWarning("Cannot find spectral type '%s' in '%s'", spectralType->origSpType, colorTable->fileName);

        return alxNOT_FOUND;
    }

    /* return the line found */
    return line;
}

static mcsCOMPL_STAT alxInterpolateDiffMagnitude(alxCOLOR_TABLE *colorTable,
                                                 mcsINT32 lineInf,
                                                 mcsINT32 lineSup,
                                                 mcsDOUBLE magDiff,
                                                 mcsINT32 magDiffId,
                                                 alxDIFFERENTIAL_MAGNITUDES diffMagnitudes)
{
    /* Init */
    alxDATA* dataSup = NULL;
    alxDATA* dataInf = NULL;
    mcsDOUBLE ratio;
    mcsINT32 i;

    /* Compute ratio for interpolation. Note that this will take
       care of the case lineInf==lineSup (although sub-optimal for speed) */
    if (colorTable->index[lineSup][magDiffId].value !=
        colorTable->index[lineInf][magDiffId].value)
    {
        ratio = fabs(((magDiff) - colorTable->index[lineInf][magDiffId].value)
                     / (colorTable->index[lineSup][magDiffId].value
                     - colorTable->index[lineInf][magDiffId].value));
    }
        /* If both value in the ref column are equal, take the average */
    else
    {
        ratio = 0.5;
    }

    /* Initialize differential magnitudes structure */
    for (i = 0; i < alxNB_DIFF_MAG; i++)
    {
        diffMagnitudes[i].value = 0.0;
        diffMagnitudes[i].isSet = mcsFALSE;
    }

    /* Loop on differential magnitudes that are on the table
       (all except the last one which is K_M)  */
    for (i = 0; i < alxNB_DIFF_MAG - 1; i++)
    {
        /* Extract the value Sup and inf*/
        dataSup = &colorTable->index[lineSup][i];
        dataInf = &colorTable->index[lineInf][i];

        /* If both values are set, compute the interpolation */
        if ((dataSup->isSet == mcsTRUE) && (dataInf->isSet == mcsTRUE))
        {
            diffMagnitudes[i].value = dataInf->value + ratio * (dataSup->value - dataInf->value);
            diffMagnitudes[i].isSet = mcsTRUE;
        }
    }

    /* Now compute K_M (not in colorTable) from K-L & L-M */
    if ((colorTable->index[lineSup][alxK_L].isSet == mcsTRUE) && (colorTable->index[lineInf][alxK_L].isSet == mcsTRUE) &&
        (colorTable->index[lineSup][alxL_M].isSet == mcsTRUE) && (colorTable->index[lineInf][alxL_M].isSet == mcsTRUE))
    {
        diffMagnitudes[alxK_M].value = colorTable->index[lineInf][alxK_L].value + colorTable->index[lineInf][alxL_M].value
                + ratio * (colorTable->index[lineSup][alxK_L].value + colorTable->index[lineSup][alxL_M].value
                - colorTable->index[lineInf][alxK_L].value - colorTable->index[lineInf][alxL_M].value);
        diffMagnitudes[alxK_M].isSet = mcsTRUE;
    }

    return mcsSUCCESS;
}

/**
 * Compute magnitude and set confidence index of a specific magnitude
 * 
 * @param magnitude magnitude to modify
 * @param firstMag first magnitude of the calcul (X-Y)
 * @param diffMag difference magnitude (ex : V-K)
 * @param factor so that newB = A + (factor)*B_A
 * @param confIndex the confidence index to set to the modify magnitude if
 * necessary
 * 
 * @return always SUCCESS
 */
static mcsCOMPL_STAT alxComputeMagnitude(mcsDOUBLE firstMag,
                                         alxDATA diffMag,
                                         mcsDOUBLE factor,
                                         alxDATA *magnitude,
                                         alxCONFIDENCE_INDEX confIndex)
{
    /* If magnitude is not set and if the diffMag is set,
     * then compute a value from the given firstMag and diffMag. */
    if ((magnitude->isSet == mcsFALSE) && (diffMag.isSet == mcsTRUE))
    {
        /* Compute*/
        magnitude->value = firstMag + factor * diffMag.value;

        /* Set correct confidence index */
        magnitude->confIndex = confIndex;
        magnitude->isSet = mcsTRUE;
    }

    return mcsSUCCESS;
}

/*
 * Public functions definition
 */

/**
 * Initialize the given spectral type structure to defaults
 * @param decodedSpectralType the spectral type structure to initialize
 * 
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is
 * returned.
 */
mcsCOMPL_STAT alxInitializeSpectralType(alxSPECTRAL_TYPE* decodedSpectralType)
{
    FAIL_NULL_DO(decodedSpectralType, errAdd(alxERR_NULL_PARAMETER, "decodedSpectralType"));

    /* Initialize Spectral Type structure */
    decodedSpectralType->isSet = mcsFALSE;
    decodedSpectralType->origSpType[0] = '\0';
    decodedSpectralType->ourSpType[0] = '\0';
    decodedSpectralType->code = '\0';
    decodedSpectralType->quantity = FP_NAN;
    decodedSpectralType->luminosityClass[0] = '\0';
    decodedSpectralType->isDouble = mcsFALSE;
    decodedSpectralType->isSpectralBinary = mcsFALSE;
    decodedSpectralType->isVariable = mcsFALSE;

    return mcsSUCCESS;
}

/**
 * Create a spectral type structure from a string.
 *
 * Should get each part of the spectral type XN.NLLL where X is a letter, N.N a
 * number between 0 and 9 and LLL is the light class. Modified as to ingest the 
 * more complicated spectral types found
 *
 * @param spectralType the spectral type string to decode
 * @param decodedSpectralType the spectral type structure to return
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is
 * returned.
 */
mcsCOMPL_STAT alxString2SpectralType(mcsSTRING32 spectralType,
                                     alxSPECTRAL_TYPE* decodedSpectralType)
{
    /* initialize the spectral type structure anyway */
    FAIL(alxInitializeSpectralType(decodedSpectralType));

    /* Function parameter check */
    FAIL_NULL_DO(spectralType, errAdd(alxERR_NULL_PARAMETER, "spectralType"));

    /* copy spectral type */
    strncpy(decodedSpectralType->origSpType, spectralType, sizeof (decodedSpectralType->origSpType) - 1);

    char* tempSP = miscDuplicateString(spectralType);
    FAIL_NULL_DO(tempSP, errAdd(alxERR_NULL_PARAMETER, "tempSP"));

    /* backup char pointer to free later as tempSP is modified later */
    char* const tempSPPtr = tempSP;

    mcsUINT32 bufferLength = strlen(tempSP) + 1;

    logDebug("Original spectral type = '%s'.", spectralType);

    /* Remove ':', '(',')', ' ' from string, and move all the rest to UPPERCASE. */
    miscDeleteChr(tempSP, ':', mcsTRUE);
    miscDeleteChr(tempSP, '(', mcsTRUE);
    miscDeleteChr(tempSP, ')', mcsTRUE);
    miscDeleteChr(tempSP, ' ', mcsTRUE);
    miscStrToUpper(tempSP);

    logDebug("Cleaned spectral type = '%s'.", tempSP);

    /* If the spectral type contains a "+" sign, it is a sure sign that the star
     * is a close binary system.
     * Example: HD 47205 (J2000=06:36:41.0-19:15:21) which is K1III(+M) */
    mcsSTRING256 subStrings[4];
    mcsUINT32 nbSubString = 0;

    FAIL_DO(miscSplitString(tempSP, '+', subStrings, 4, &nbSubString),
            errAdd(alxERR_WRONG_SPECTRAL_TYPE_FORMAT, spectralType);
            free(tempSPPtr));

    if (nbSubString > 1)
    {
        strncpy(tempSP, subStrings[0], bufferLength);

        logDebug("Un-doubled spectral type = '%s'.", tempSP);

        decodedSpectralType->isDouble = mcsTRUE;
    }

    /* If the spectral type contains "SB", remove the trailing part and
     * tag it as spectral binary. */
    char* tokenPosition = strstr(tempSP, "SB");
    if (tokenPosition != NULL)
    {
        *tokenPosition = '\0'; /* Cut here */

        logDebug("Un-SB spectral type = '%s'.", tempSP);

        decodedSpectralType->isSpectralBinary = mcsTRUE;
    }

    /* Notice variability and remove it */
    tokenPosition = strstr(tempSP, "VAR");
    if (tokenPosition != NULL)
    {
        *tokenPosition = '\0'; /* Cut here */

        logDebug("Un-VAR spectral type = '%s'.", tempSP);

        decodedSpectralType->isVariable = mcsTRUE;
    }

    /* Index for the following */
    mcsUINT32 index = 0;

    /* If the spectral type contains "CN" or "BA" etc... (Cyanogen, Barium, etc) 
     * remove the annoying trailing part and tag it as hasSpectralLines */
    static char* hasSpectralIndicators[] = {"LAM", "FE", "MN", "HG", "CN", "BA", "SI", "SR", "CR", "EU", "MG", "EM", "CA", NULL};
    while (hasSpectralIndicators[index] != NULL)
    {
        /* If the current spectral type is found */
        if ((tokenPosition = strstr(tempSP, hasSpectralIndicators[index])) != NULL)
        {
            *tokenPosition = '\0'; /* Cut here */
            /* NO Break since the number and order of indicators is variable */
        }
        index++;
    }

    /*If O was wrongly written instead of 0 in normal classes, correct*/
    static char* hasWrongO[] = {"OO", "BO", "AO", "FO", "GO", "KO", "MO", NULL};
    index = 0;
    while (hasWrongO[index] != NULL)
    {
        tokenPosition = strstr(tempSP, hasWrongO[index]);
        if (tokenPosition != NULL)
        {
            *++tokenPosition = '0'; /* replace O by 0 */
            break;
        }
        index++;
    }

    /*Hesitates between consecutive classes: get inbetween*/
    static char* hesitateBetweenClasses[] = {"O/B", "O-B", "B/A", "B-A", "A/F", "A-F", "F/G", "F-G", "G/K", "G-K", "K/M", "K-M", NULL};
    index = 0;
    while (hesitateBetweenClasses[index] != NULL)
    {
        tokenPosition = strstr(tempSP, hesitateBetweenClasses[index]); /* Say "B/A" is a "B9." */
        if (tokenPosition != NULL)
        {
            *++tokenPosition = '9';
            *++tokenPosition = '.';
            break;
        }
        index++;
    }

    /* If the spectral type hesitates between two subclasses (A0/3, A0-3), 
     * or has a wrong comma, replace by a numerical value.
     */
    char type, separator, type2;
    mcsSTRING32 tempBuffer;
    mcsINT32 firstSubType, secondSubType;
    mcsUINT32 nbOfTokens = sscanf(tempSP, "%c%1d%c%1d", &type, &firstSubType, &separator, &secondSubType);
    if (nbOfTokens == 4)
    {
        char* luminosityClassPointer = tempSP + 1; /* Skipping first char */

        if ((separator == '/') || (separator == '-'))
        {
            mcsDOUBLE meanSubType = 0.5 * (firstSubType + secondSubType);
            sprintf(tempBuffer, "%3.1lf", meanSubType);
            strncpy(luminosityClassPointer, tempBuffer, 3);

            logDebug("Un-hesitated spectral type = '%s'.", tempSP);
        }
        else if (separator == ',')
        {
            sprintf(tempBuffer, "%1d%c%1d", firstSubType, '.', secondSubType);
            strncpy(luminosityClassPointer, tempBuffer, 3);

            logDebug("Un-comma-ed spectral type = '%s'.", tempSP);
        }
    }

    /* If the spectral type is Xx/Xy..., it is another hesitation */
    nbOfTokens = sscanf(tempSP, "%c%1d/%c%1d", &type, &firstSubType, &type2, &secondSubType);
    if (nbOfTokens == 4)
    {
        if (type == type2)
        {
            /* type A8/A9 , gives A8.50 for further interpretation*/
            char* luminosityClassPointer = tempSP + 1; /* Skipping first char */
            mcsDOUBLE meanSubType = 0.5 * (firstSubType + secondSubType);
            sprintf(tempBuffer, "%4.2lf", meanSubType);
            strncpy(luminosityClassPointer, tempBuffer, 4);

            logDebug("Un-hesitate(2) spectral type = '%s'.", tempSP);
        }
        else
        {
            /* in the case of, say, G8/K0, we want G8.50 */
            static char *hesitateBetweenClassesBis[] = {"O9/B0", "B9/A0", "A8/F0", "F8/G0", "G8/K0", "K7/M0", NULL};
            index = 0;
            while (hesitateBetweenClassesBis[index] != NULL)
            {
                tokenPosition = strstr(tempSP, hesitateBetweenClassesBis[index]); /* Say "B9/A0" is a "B9.50" */
                if (tokenPosition != NULL)
                {
                    tokenPosition++;
                    *++tokenPosition = '.';
                    *++tokenPosition = '5';
                    *++tokenPosition = '0';
                    break;
                }
                index++;
            }
        }
    }

    /* If the spectral type is AxM..., it is a peculiar A star which is normally a dwarf */
    nbOfTokens = sscanf(tempSP, "%c%1d%c", &type, &firstSubType, &separator);
    if (nbOfTokens == 3)
    {
        if (separator == 'M')
        {
            /* V for Dwarf, to be further interpreted */
            snprintf(tempSP, bufferLength, "%c%1dV (%c%1d%c)", type, firstSubType, type, firstSubType, separator);

            logDebug("Un-M spectral type = '%s'.", tempSP);
        }
    }

    /* If the spectral type is sd[OBAFG..]xx, it is a subdwarf of type VI */
    tokenPosition = strstr(tempSP, "SD");
    if (tokenPosition == tempSP)
    {
        nbOfTokens = sscanf(tempSP, "SD%c%c", &type, &separator);
        if (nbOfTokens == 2)
        {
            snprintf(tempSP, sizeof (tempSP), "%c%cVI", type, separator); /* VI for SubDwarf, to be further interpreted */
        }
        else
        {
            tempSP += 2; /* Skip leading 'SD' */
        }
        logDebug("Un-SD spectral type = '%s'.", tempSP);
    }

    /* Properly parse cleaned-up spectral type string */
    nbOfTokens = sscanf(tempSP, "%c%lf%s", &decodedSpectralType->code, &decodedSpectralType->quantity, decodedSpectralType->luminosityClass);

    /*nbitems 3 is OK*/
    /* If there is no luminosity class in given spectral type, reset it */
    if (nbOfTokens == 2)
    {
        decodedSpectralType->luminosityClass[0] = '\0';
    }
    else if (nbOfTokens == 1) /*meaning there is no numerical value for the spectral type */
    {
        /* try a simple [O-M] spectral type + luminosity class*/
        mcsINT32 nbOfTokens2 = sscanf(tempSP, "%c%s", &(decodedSpectralType->code), decodedSpectralType->luminosityClass);

        /* Null spectral code, go no further */
        FAIL_FALSE_DO(nbOfTokens2,
                      errAdd(alxERR_WRONG_SPECTRAL_TYPE_FORMAT, spectralType);
                      free(tempSPPtr));

        /* Spectral Type covers one whole class, artificially put subclass at 5.
         * This is what the CDS java decoder does in fact! */
        decodedSpectralType->quantity = 5.0;
    }
    else
    {
        /* Null spectral code, go no further */
        FAIL_FALSE_DO(nbOfTokens,
                      errAdd(alxERR_WRONG_SPECTRAL_TYPE_FORMAT, spectralType);
                      free(tempSPPtr));
    }

    /* Insure the decodedSpectralType is something we handle well */
    switch (decodedSpectralType->code)
    {
        case 'O':
        case 'B':
        case 'A':
        case 'F':
        case 'G':
        case 'K':
        case 'M':
            break;
        default:
            FAIL_DO(mcsFAILURE,
                    errAdd(alxERR_WRONG_SPECTRAL_TYPE_FORMAT, spectralType);
                    free(tempSPPtr));
    }

    /* Spectral type successfully parsed, define isSet flag to true */
    decodedSpectralType->isSet = mcsTRUE;

    /* Populate ourSpType string*/
    snprintf(decodedSpectralType->ourSpType, sizeof (decodedSpectralType->ourSpType) - 1,
             "%c%3.1f%s", decodedSpectralType->code, decodedSpectralType->quantity, decodedSpectralType->luminosityClass);

    logTest("Parsed spectral type = '%s' - Our spectral type = '%s' : Code = '%c', Sub-type Quantity = '%.2lf', Luminosity Class = '%s', "
            "Is Double  = '%s', Is Spectral Binary = '%s', Is Variable = '%s'",
            decodedSpectralType->origSpType, decodedSpectralType->ourSpType,
            decodedSpectralType->code, decodedSpectralType->quantity, decodedSpectralType->luminosityClass,
            (decodedSpectralType->isDouble == mcsTRUE ? "YES" : "NO"),
            (decodedSpectralType->isSpectralBinary == mcsTRUE ? "YES" : "NO"),
            (decodedSpectralType->isVariable == mcsTRUE ? "YES" : "NO")
            );

    /* Return the pointer on the created spectral type structure */
    free(tempSPPtr);

    return mcsSUCCESS;
}

/**
 * Correct the spectral type i.e. guess the luminosity class using magnitudes and color tables
 * @param spectralType spectral type 
 * @param magnitudes, B and V should be set
 *
 * @return mcsSUCCESS always return success.
 */
mcsCOMPL_STAT alxCorrectSpectralType(alxSPECTRAL_TYPE* spectralType,
                                     mcsDOUBLE diffBV)

{
    alxCOLOR_TABLE* colorTable;
    mcsINT32 line;

    /* luminosity Class is already present */
    SUCCESS_COND(strlen(spectralType->luminosityClass) != 0);

    logDebug("alxCorrectSpectralType: spectral type = '%s', B-V = %0.3lf, V = %0.3lf", spectralType->origSpType,
             diffBV);

    /* try a dwarf */
    strcpy(spectralType->luminosityClass, "V"); /* alxDWARF */

    /* Get color tables */
    colorTable = alxGetColorTableForStar(spectralType);
    if (colorTable == NULL)
    {
        goto correctError;
    }

    /* Line corresponding to the spectral type */
    line = alxGetLineFromSpectralType(colorTable, spectralType);
    /* if line not found, i.e = -1, return */
    if (line == alxNOT_FOUND)
    {
        goto correctError;
    }

    /* Compare B-V star differential magnitude to the one of the color table
     * line; delta should be less than +/- 0.1 */
    if (fabs(diffBV - colorTable->index[line][alxB_V].value) <= DELTA_THRESHOLD)
    {
        /* it is compatible with a dwarf */
        snprintf(spectralType->ourSpType, sizeof (spectralType->ourSpType) - 1,
                 "%c%3.1f(%s)", spectralType->code, spectralType->quantity, spectralType->luminosityClass);

        logTest("alxCorrectSpectralType: spectral type = '%s' - Our spectral type = '%s' : updated Luminosity Class = '%s'",
                spectralType->origSpType, spectralType->ourSpType, spectralType->luminosityClass);

        return mcsSUCCESS;
    }

    /* try a giant...*/
    strcpy(spectralType->luminosityClass, "III"); /* alxGIANT */

    colorTable = alxGetColorTableForStar(spectralType);
    if (colorTable == NULL)
    {
        goto correctError;
    }

    /* Line corresponding to the spectral type */
    line = alxGetLineFromSpectralType(colorTable, spectralType);
    /* if line not found, i.e = -1, return */
    if (line == alxNOT_FOUND)
    {
        goto correctError;
    }

    /* Compare B-V star differential magnitude to the one of the color table
     * line; delta should be less than +/- 0.1 */
    if (fabs(diffBV - colorTable->index[line][alxB_V].value) <= DELTA_THRESHOLD)
    {
        /* it is compatible with a giant */
        snprintf(spectralType->ourSpType, sizeof (spectralType->ourSpType) - 1,
                 "%c%3.1f(%s)", spectralType->code, spectralType->quantity, spectralType->luminosityClass);

        logTest("alxCorrectSpectralType: spectral type = '%s' - Our spectral type = '%s' : updated Luminosity Class = '%s'",
                spectralType->origSpType, spectralType->ourSpType, spectralType->luminosityClass);

        return mcsSUCCESS;
    }

    /* try a supergiant...*/
    strcpy(spectralType->luminosityClass, "I"); /* alxSUPER_GIANT */

    colorTable = alxGetColorTableForStar(spectralType);
    if (colorTable == NULL)
    {
        goto correctError;
    }

    /* Line corresponding to the spectral type */
    line = alxGetLineFromSpectralType(colorTable, spectralType);
    /* if line not found, i.e = -1, return */
    if (line == alxNOT_FOUND)
    {
        goto correctError;
    }

    /* Compare B-V star differential magnitude to the one of the color table
     * line; delta should be less than +/- 0.1 */
    if (fabs(diffBV - colorTable->index[line][alxB_V].value) <= DELTA_THRESHOLD)
    {
        /* it is compatible with a supergiant */
        snprintf(spectralType->ourSpType, sizeof (spectralType->ourSpType) - 1,
                 "%c%3.1f(%s)", spectralType->code, spectralType->quantity, spectralType->luminosityClass);

        logTest("alxCorrectSpectralType: spectral type = '%s' - Our spectral type = '%s' : updated Luminosity Class = '%s'",
                spectralType->origSpType, spectralType->ourSpType, spectralType->luminosityClass);

        return mcsSUCCESS;
    }

correctError:
    /* reset luminosity class to unknown */
    spectralType->luminosityClass[0] = '\0';

    return mcsSUCCESS;
}

/**
 * Compute *missing* magnitudes in R, I, J, H, K, L and M bands
 * from the spectal type and the V magnitude for the bright
 * star case.
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
mcsCOMPL_STAT alxComputeMagnitudesForBrightStar(alxSPECTRAL_TYPE* spectralType,
                                                alxMAGNITUDES magnitudes)
{
    /* If spectral type is unknown, return error. */
    SUCCESS_FALSE_DO(spectralType->isSet, logTest("Spectral type is not set; could not compute missing magnitudes"));

    /* If magnitude B or V are not set, return SUCCESS : the alxMAGNITUDE
     * structure will not be changed -> the magnitude won't be computed */
    SUCCESS_COND_DO((magnitudes[alxB_BAND].isSet == mcsFALSE) || (magnitudes[alxV_BAND].isSet == mcsFALSE),
                    logTest("B and V mag are not set; could not compute missing magnitudes"));

    /* If B and V are defined, get magnitudes in B and V bands */
    mcsDOUBLE mgB, mgV;
    mgB = magnitudes[alxB_BAND].value;
    mgV = magnitudes[alxV_BAND].value;

    /* Get the color table according to the spectral type of the star */
    alxCOLOR_TABLE* colorTable = alxGetColorTableForStar(spectralType);
    FAIL_NULL(colorTable);

    /* Line corresponding to the spectral type */
    mcsINT32 lineSup, lineInf;
    lineSup = alxGetLineFromSpectralType(colorTable, spectralType);

    /* if line not found, i.e = -1, return mcsSUCCESS */
    SUCCESS_COND_DO(lineSup == alxNOT_FOUND, logTest("Could not compute missing magnitudes"));

    /* If the spectral type matches the line of the color table, take this line */
    if (colorTable->spectralType[lineSup].quantity == spectralType->quantity)
    {
        lineInf = lineSup;
    }
        /* Otherwise interpolate */
    else
    {
        lineInf = lineSup - 1;
    }

    /* Compare B-V star differential magnitude to the one of the color table
     * line; delta should be less than +/- 0.1 */
    if ((fabs((mgB - mgV) - colorTable->index[lineSup][alxB_V].value) > DELTA_THRESHOLD) &&
        (fabs((mgB - mgV) - colorTable->index[lineInf][alxB_V].value) > DELTA_THRESHOLD))
    {
        logTest("Could not compute differential magnitudes; mgB-mgV = %.3lf / B-V [%.3lf..%.3lf]; delta > 0.1",
                (mgB - mgV), colorTable->index[lineInf][alxB_V].value,
                colorTable->index[lineSup][alxB_V].value);

        return mcsSUCCESS;
    }

    /* Define the structure of differential magnitudes */
    alxDIFFERENTIAL_MAGNITUDES diffMag;

    /* Perform the interpolation to obtain the best estimate of
       B_V V_I V_R I_J J_H J_K K_L L_M K_M */
    FAIL(alxInterpolateDiffMagnitude(colorTable, lineInf, lineSup, mgB - mgV, alxB_V, diffMag));

    /* Set confidence index for computed values.
     * If magnitude in K band is unknown, set confidence index to LOW,
     * otherise set to MEDIUM (LBO: 12/04/2013).
     * FIXME: check this rule. */
    alxCONFIDENCE_INDEX confIndex;
    if (magnitudes[alxK_BAND].isSet == mcsFALSE)
    {
        confIndex = alxCONFIDENCE_LOW;
    }
        /* Else B, V and K are known */
    else
    {
        confIndex = alxCONFIDENCE_MEDIUM;
    }

    /* Compute *missing* magnitudes in R, I, J, H, K, L and M bands.
       Only missing magnitude are updated by alxComputeMagnitude  */

    /* Compute R = V - V_R */
    alxComputeMagnitude(mgV,
                        diffMag[alxV_R],
                        -1.,
                        &(magnitudes[alxR_BAND]),
                        confIndex);

    /* Compute I = V - V_I */
    alxComputeMagnitude(mgV,
                        diffMag[alxV_I],
                        -1.,
                        &(magnitudes[alxI_BAND]),
                        confIndex);

    /* Compute J = I - I_J */
    alxComputeMagnitude(magnitudes[alxI_BAND].value,
                        diffMag[alxI_J],
                        -1.,
                        &(magnitudes[alxJ_BAND]),
                        confIndex);

    /* Compute H = J - J_H */
    alxComputeMagnitude(magnitudes[alxJ_BAND].value,
                        diffMag[alxJ_H],
                        -1.,
                        &(magnitudes[alxH_BAND]),
                        confIndex);

    /* Compute K = J - J_K */
    alxComputeMagnitude(magnitudes[alxJ_BAND].value,
                        diffMag[alxJ_K],
                        -1.,
                        &(magnitudes[alxK_BAND]),
                        confIndex);

    /* Compute L = K - K_L */
    alxComputeMagnitude(magnitudes[alxK_BAND].value,
                        diffMag[alxK_L],
                        -1.,
                        &(magnitudes[alxL_BAND]),
                        confIndex);

    /* Compute M = K - K_M */
    alxComputeMagnitude(magnitudes[alxK_BAND].value,
                        diffMag[alxK_M],
                        -1.,
                        &(magnitudes[alxM_BAND]),
                        confIndex);

    /* Print out results */
    alxLogTestMagnitudes("Computed magnitudes (bright)", "", magnitudes);

    return mcsSUCCESS;
}

/**
 * Compute magnitudes in H, I, V, R, and B bands for faint star.
 *
 * It computes magnitudes in H, I, V, R, and B bands according to the
 * magnitudes in J and K bands for a faint star.
 * If magnitude in J and K band is unkwown, the confidence index of computed
 * values is set to LOW, otherwise (J and K known) it is set to HIGH.
 * If magnitude can not be computed, its associated confidence index is set to
 * NO CONFIDENCE.
 *
 * @param spectralType (optional) spectral type structure
 * @param magnitudes contains magnitudes in J and K bands, and the computed
 * magnitudes in B, V, R, I, K, L and M bands
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is
 * returned.
 */
mcsCOMPL_STAT alxComputeMagnitudesForFaintStar(alxSPECTRAL_TYPE* spectralType,
                                               alxMAGNITUDES magnitudes)
{
    /* If magnitude J or K are not set, return SUCCESS : the alxMAGNITUDE
     * structure will not be changed -> the magnitude won't be computed */
    SUCCESS_COND_DO((magnitudes[alxJ_BAND].isSet == mcsFALSE) || (magnitudes[alxK_BAND].isSet == mcsFALSE),
                    logTest("J and K mag are not set; could not compute missing magnitudes"));

    /* Get magnitudes in J and K bands */
    mcsDOUBLE mgJ, mgK;
    mgJ = magnitudes[alxJ_BAND].value;
    mgK = magnitudes[alxK_BAND].value;

    /* Get the color table according to the spectral type of the star */
    alxCOLOR_TABLE* colorTable = alxGetColorTableForStar(spectralType);
    FAIL_NULL(colorTable);

    /* Line corresponding to the spectral type (actually the line just
     * following if perfect match is not found).
     * FIXME: not exactly the same in BRIGHT: we don't interpolate
     * if perfect match is found */
    mcsINT32 lineInf, lineSup;
    lineSup = alxGetLineFromSpectralType(colorTable, spectralType);

    /* If no match found, then try to match the column of magDiff */
    if (lineSup == alxNOT_FOUND)
    {
        logTest("try with J-K");

        lineSup = alxGetLineFromValue(colorTable, mgJ - mgK, alxJ_K);

        /* If line still not found, i.e = -1, return */
        if (lineSup == alxNOT_FOUND)
        {
            logTest("Cannot find J-K (%.3lf) in '%s'; could not compute missing magnitudes",
                    mgJ - mgK, colorTable->fileName);

            return mcsSUCCESS;
        }
    }

    /* Define the structure of differential magnitudes */
    alxDIFFERENTIAL_MAGNITUDES diffMag;

    /* Perform the interpolation to obtain the best estimate of
     * B_V V_I V_R I_J J_H J_K K_L L_M K_M */
    lineInf = lineSup - 1;
    FAIL(alxInterpolateDiffMagnitude(colorTable, lineInf, lineSup, mgJ - mgK, alxJ_K, diffMag));

    /* Set confidence index for computed values.
     * Set MEDIUM (LBO: 12/04/2013) because magnitude in K band is unknown.
     * FIXME: check this rule */
    alxCONFIDENCE_INDEX confIndex = alxCONFIDENCE_MEDIUM;

    /* Compute *missing* magnitudes in R, I, J, H, K, L and M bands.
       Only missing magnitude are updated by alxComputeMagnitude  */

    /* Compute H = J - J_H */
    alxComputeMagnitude(mgJ,
                        diffMag[alxJ_H],
                        -1.,
                        &(magnitudes[alxH_BAND]),
                        confIndex);

    /* Compute I = J + I_J */
    alxComputeMagnitude(mgJ,
                        diffMag[alxI_J],
                        1.,
                        &(magnitudes[alxI_BAND]),
                        confIndex);

    /* Compute V = I + V_I */
    alxComputeMagnitude(magnitudes[alxI_BAND].value,
                        diffMag[alxV_I],
                        1.,
                        &(magnitudes[alxV_BAND]),
                        confIndex);

    /* Compute R = V - V_R */
    alxComputeMagnitude(magnitudes[alxV_BAND].value,
                        diffMag[alxV_R],
                        -1.,
                        &(magnitudes[alxR_BAND]),
                        confIndex);

    /* Compute B = V + B_V */
    alxComputeMagnitude(magnitudes[alxV_BAND].value,
                        diffMag[alxB_V],
                        1.,
                        &(magnitudes[alxB_BAND]),
                        confIndex);

    /* Print out results */
    alxLogTestMagnitudes("Computed magnitudes (faint)", "", magnitudes);

    return mcsSUCCESS;
}

/**
 * Returns the ratio of the plack function B(lambda1,T1)/B(lambda2,T2)
 * using constants and reduced planck function as in T. Michels, 
 * Nasa Technical Note D-4446.
 *
 * @param Teff1 double the first BlackBody temperature 
 * @param lambda1 double the first BlackBody wavelength (in microns)
 * @param Teff2 double the second BlackBody temperature 
 * @param lambda2 double the second BlackBody wavelength (in microns)
 *
 * @return double the flux ratio
 */
static mcsDOUBLE alxBlackBodyFluxRatio(mcsDOUBLE Teff1,
                                       mcsDOUBLE lambda1,
                                       mcsDOUBLE Teff2,
                                       mcsDOUBLE lambda2)
{
    /* Constants are for lambda^-1 in cm^-1, but exponent 3 is correct for flux
     * densities in wavelength */
    mcsDOUBLE nu1 = 10000.0 / lambda1; /*wavenumber cm^-1*/
    mcsDOUBLE nu2 = 10000.0 / lambda2; /*wavenumber cm^-1*/

    mcsDOUBLE x = nu1 / Teff1;
    mcsDOUBLE y = nu2 / Teff2;

    mcsDOUBLE ratio = pow((x / y), 3.0) * (exp(1.43879 * y) - 1.) / (exp(1.43879 * x) - 1.);
    return ratio;
}

static alxAKARI_TABLE* alxLoadAkariTable()
{
    /* To know if it was loaded already */
    static alxAKARI_TABLE akariTable = {mcsFALSE, "alxAkariBlackBodyCorrectionTable.cfg"};

    /*
     * Check if the structure in which polynomial coefficients will be stored is
     * loaded into memory. If not load it.
     */
    if (akariTable.loaded == mcsTRUE)
    {
        return &akariTable;
    }

    /* Find the location of the file */
    char* fileName = miscLocateFile(akariTable.fileName);
    if (fileName == NULL)
    {
        return NULL;
    }

    /* Load file (skipping comment lines starting with '#') */
    miscDYN_BUF dynBuf;
    miscDynBufInit(&dynBuf);

    logInfo("Loading %s ...", fileName);

    NULL_DO(miscDynBufLoadFile(&dynBuf, fileName, "#"),
            miscDynBufDestroy(&dynBuf);
            free(fileName));

    /* For each line of the loaded file */
    mcsINT32 lineNum = 0;
    const char *pos = NULL;
    mcsSTRING1024 line;

    while ((pos = miscDynBufGetNextLine(&dynBuf, pos, line, sizeof (line), mcsTRUE)) != NULL)
    {
        logTrace("miscDynBufGetNextLine() = '%s'", line);

        /* Trim line for any leading and trailing blank characters */
        miscTrimString(line, " ");

        /* If line is not empty */
        if (strlen(line) != 0)
        {
            /* Check if there are to many lines in file */
            if (lineNum >= alxNB_AKARI_TEFF)
            {
                /* Destroy the temporary dynamic buffer, raise an error and return */
                miscDynBufDestroy(&dynBuf);
                errAdd(alxERR_TOO_MANY_LINES, fileName);
                free(fileName);
                return NULL;
            }

            /* Try to read each BB correction coefficients */
            mcsINT32 nbOfReadTokens = sscanf(line, "%lf %lf %lf %lf %lf %lf %lf",
                                             &akariTable.teff[lineNum],
                                             &akariTable.coeff[lineNum][0],
                                             &akariTable.coeff[lineNum][1],
                                             &akariTable.coeff[lineNum][2],
                                             &akariTable.coeff[lineNum][3],
                                             &akariTable.coeff[lineNum][4],
                                             &akariTable.coeff[lineNum][5]);

            /* If parsing went wrong */
            if (nbOfReadTokens != (alxNB_AKARI_BANDS + 1))
            {
                /* Destroy the temporary dynamic buffer, raise an error and return */
                miscDynBufDestroy(&dynBuf);
                errAdd(alxERR_WRONG_FILE_FORMAT, line, fileName);
                free(fileName);
                return NULL;
            }

            /* Next line */
            lineNum++;
        }
    }

    /* Set the total number of lines in the akari table */
    akariTable.nbLines = lineNum;

    /* Mark the akari table as "loaded" */
    akariTable.loaded = mcsTRUE;

    /* Destroy the temporary dynamic buffer used to parse the akari table file */
    miscDynBufDestroy(&dynBuf);
    free(fileName);

    /* Return a pointer on the freshly loaded akari table */
    return &akariTable;
}

static mcsINT32 alxGetLineForAkari(alxAKARI_TABLE *akariTable,
                                   mcsDOUBLE Teff)
{
    mcsLOGICAL found = mcsFALSE;
    mcsINT32 line = 0;

    while ((found == mcsFALSE) && (line < akariTable->nbLines))
    {
        /* get line immediately above Teff */
        if (akariTable->teff[line] > Teff)
        {
            found = mcsTRUE;
        }
        else /* Else go to the next line */
        {
            line++;
        }
    }

    /* If Teff not found in akari table, return error */
    if (line == 0)
    {
        return alxNOT_FOUND;
    }

    /* return the line found */
    return line;
}

mcsCOMPL_STAT alxComputeFluxesFromAkari18(mcsDOUBLE Teff,
                                          mcsDOUBLE *fnu_18,
                                          mcsDOUBLE *fnu_12,
                                          mcsDOUBLE *fnu_9)
{
    mcsDOUBLE correctionFactor;
    mcsDOUBLE bandFluxRatio;
    mcsDOUBLE mono18, mono9;

    /* Load Akari table  */
    alxAKARI_TABLE* akariTable = alxLoadAkariTable();
    FAIL_NULL(akariTable);

    /* Line corresponding to the spectral type */
    mcsINT32 line = alxGetLineForAkari(akariTable, Teff);
    /* if line not found, i.e = -1, return mcsFAILURE */
    FAIL_COND(line == alxNOT_FOUND);

    /* interpolate */
    mcsDOUBLE ratio; /* Needed to compute ratio */
    mcsINT32 lineSup = line;
    mcsINT32 lineInf = line - 1;

    /* logTest("Inferior line = %d", lineInf); */
    /* logTest("Superior line = %d", lineSup); */
    /* logTest("%f < Teff (%f) < %f", akariTable->teff[lineInf], Teff, akariTable->teff[lineSup]); */

    /* Compute ratio for interpolation */
    if (akariTable->teff[lineSup] != akariTable->teff[lineInf])
    {
        ratio = fabs(((Teff) - akariTable->teff[lineInf])
                     / (akariTable->teff[lineSup] - akariTable->teff[lineInf]));
    }
    else
    {
        ratio = 0.5;
    }

    /* logTest("Ratio = %f", ratio); */

    /* Compute correction Factor */
    mcsDOUBLE dataSup = akariTable->coeff[lineSup][alx18mu];
    mcsDOUBLE dataInf = akariTable->coeff[lineInf][alx18mu];

    FAIL_COND((alxIsBlankingValue(dataSup) == mcsTRUE) || (alxIsBlankingValue(dataInf) == mcsTRUE));

    correctionFactor = dataInf + ratio * (dataSup - dataInf);
    /* logTest("correctionFactor = %f", correctionFactor); */

    mono18 = (*fnu_18) / correctionFactor;

    /* compute new flux at 12 mu by black_body approximation */
    bandFluxRatio = alxBlackBodyFluxRatio(Teff, (mcsDOUBLE) 12.0, Teff, (mcsDOUBLE) AKARI_18MU);
    *fnu_12 = mono18 * bandFluxRatio;

    /* compute (complementary) fnu_9 in the same manner:*/
    bandFluxRatio = alxBlackBodyFluxRatio(Teff, (mcsDOUBLE) AKARI_9MU, Teff, (mcsDOUBLE) AKARI_18MU);
    mono9 = mono18 * bandFluxRatio;

    /* for coherence of data, use the correction factor to give an estimate of the akari band flux,
     * not the monochromatic flux: */
    dataSup = akariTable->coeff[lineSup][alx9mu];
    dataInf = akariTable->coeff[lineInf][alx9mu];
    correctionFactor = dataInf + ratio * (dataSup - dataInf);
    *fnu_9 = mono9 * correctionFactor;

    return mcsSUCCESS;
}

mcsCOMPL_STAT alxComputeFluxesFromAkari09(mcsDOUBLE Teff,
                                          mcsDOUBLE *fnu_9,
                                          mcsDOUBLE *fnu_12,
                                          mcsDOUBLE *fnu_18)
{
    mcsDOUBLE correctionFactor;
    mcsDOUBLE bandFluxRatio;
    mcsDOUBLE mono18, mono9;

    /* Load Akari table  */
    alxAKARI_TABLE* akariTable = alxLoadAkariTable();
    FAIL_NULL(akariTable);

    /* Line corresponding to the spectral type */
    mcsINT32 line = alxGetLineForAkari(akariTable, Teff);
    /* if line not found, i.e = -1, return mcsFAILURE */
    FAIL_COND(line == alxNOT_FOUND);

    /* interpolate */
    mcsDOUBLE ratio; /* Needed to compute ratio */
    mcsINT32 lineSup = line;
    mcsINT32 lineInf = line - 1;

    /* logTest("Inferior line = %d", lineInf); */
    /* logTest("Superior line = %d", lineSup); */
    /* logTest("%f < Teff (%f) < %f", akariTable->teff[lineInf], Teff, akariTable->teff[lineSup]); */

    /* Compute ratio for interpolation */
    if (akariTable->teff[lineSup] != akariTable->teff[lineInf])
    {
        ratio = fabs(((Teff) - akariTable->teff[lineInf])
                     / (akariTable->teff[lineSup] - akariTable->teff[lineInf]));
    }
    else
    {
        ratio = 0.5;
    }

    /* logTest("Ratio = %f", ratio); */

    /* Compute correction Factor */
    mcsDOUBLE dataSup = akariTable->coeff[lineSup][alx9mu];
    mcsDOUBLE dataInf = akariTable->coeff[lineInf][alx9mu];

    FAIL_COND((alxIsBlankingValue(dataSup) == mcsTRUE) || (alxIsBlankingValue(dataInf) == mcsTRUE));

    correctionFactor = dataInf + ratio * (dataSup - dataInf);
    /* logTest("correctionFactor = %f", correctionFactor); */

    mono9 = (*fnu_9) / correctionFactor;

    /* compute new flux at 12 mu by black_body approximation */
    bandFluxRatio = alxBlackBodyFluxRatio(Teff, (mcsDOUBLE) 12.0, Teff, (mcsDOUBLE) AKARI_9MU);
    *fnu_12 = mono9 * bandFluxRatio;

    /* compute (complementary) fnu_18 in the same manner:*/
    bandFluxRatio = alxBlackBodyFluxRatio(Teff, (mcsDOUBLE) AKARI_18MU, Teff, (mcsDOUBLE) AKARI_9MU);
    mono18 = mono9 * bandFluxRatio;

    /* for coherence of data, use the correction factor to give an estimate of the akari band flux,
     * not the monochromatic flux: */
    dataSup = akariTable->coeff[lineSup][alx18mu];
    dataInf = akariTable->coeff[lineInf][alx18mu];
    correctionFactor = dataInf + ratio * (dataSup - dataInf);
    *fnu_18 = mono18 * correctionFactor;

    return mcsSUCCESS;
}

static alxTEFFLOGG_TABLE* alxGetTeffLoggTable()
{
    /* Existing ColorTables */
    static alxTEFFLOGG_TABLE teffloggTable = {mcsFALSE, "alxTableTeffLogg.cfg"};

    /*
     * Check if the structure in which polynomial coefficients will be stored is
     * loaded into memory. If not load it.
     */
    if (teffloggTable.loaded == mcsTRUE)
    {
        return &teffloggTable;
    }

    /* Find the location of the file */
    char* fileName = miscLocateFile(teffloggTable.fileName);
    if (fileName == NULL)
    {
        return NULL;
    }

    /* Load file (skipping comment lines starting with '#') */
    miscDYN_BUF dynBuf;
    miscDynBufInit(&dynBuf);

    logInfo("Loading %s ...", fileName);

    NULL_DO(miscDynBufLoadFile(&dynBuf, fileName, "#"),
            miscDynBufDestroy(&dynBuf);
            free(fileName));

    /* For each line of the loaded file */
    mcsINT32 lineNum = 0;
    const char *pos = NULL;
    mcsSTRING1024 line;

    while ((pos = miscDynBufGetNextLine(&dynBuf, pos, line, sizeof (line), mcsTRUE)) != NULL)
    {
        logTrace("miscDynBufGetNextLine() = '%s'", line);

        /* Trim line for any leading and trailing blank characters */
        miscTrimString(line, " ");

        /* If line is not empty */
        if (strlen(line) != 0)
        {
            /* Check if there are to many lines in file */
            if (lineNum >= alxNB_SPECTRAL_TYPES_FOR_TEFF)
            {
                /* Destroy the temporary dynamic buffer, raise an error and return */
                miscDynBufDestroy(&dynBuf);
                errAdd(alxERR_TOO_MANY_LINES, fileName);
                free(fileName);
                return NULL;
            }

            /* Try to read each polynomial coefficients */
            mcsDOUBLE unused;
            mcsINT32 nbOfReadTokens = sscanf(line, "%c%lf %lf %lf %lf %lf %lf %lf %lf",
                                             &teffloggTable.spectralType[lineNum].code,
                                             &teffloggTable.spectralType[lineNum].quantity,
                                             &unused,
                                             &teffloggTable.teff[lineNum][alxDWARF],
                                             &teffloggTable.logg[lineNum][alxDWARF],
                                             &teffloggTable.teff[lineNum][alxGIANT],
                                             &teffloggTable.logg[lineNum][alxGIANT],
                                             &teffloggTable.teff[lineNum][alxSUPER_GIANT],
                                             &teffloggTable.logg[lineNum][alxSUPER_GIANT]);

            /* If parsing went wrong */
            if (nbOfReadTokens != (2 * alxNB_LUMINOSITY_CLASSES + 3))
            {
                /* Destroy the temporary dynamic buffer, raise an error and return */
                miscDynBufDestroy(&dynBuf);
                errAdd(alxERR_WRONG_FILE_FORMAT, line, fileName);
                free(fileName);
                return NULL;
            }

            /* Next line */
            lineNum++;
        }
    }

    /* Set the total number of lines in the tefflogg table */
    teffloggTable.nbLines = lineNum;

    /* Mark the tefflogg table as "loaded" */
    teffloggTable.loaded = mcsTRUE;

    /* Destroy the temporary dynamic buffer used to parse the tefflogg table file */
    miscDynBufDestroy(&dynBuf);
    free(fileName);

    /* Return a pointer on the freshly loaded tefflogg table */
    return &teffloggTable;
}

static mcsINT32 alxGetLineForTeffLogg(alxTEFFLOGG_TABLE *teffloggTable,
                                      alxSPECTRAL_TYPE *spectralType)
{
    mcsLOGICAL codeFound = mcsFALSE;
    mcsLOGICAL found = mcsFALSE;
    mcsINT32 line = 0;

    while ((found == mcsFALSE) && (line < teffloggTable->nbLines))
    {
        /* If the spectral type code match */
        if (teffloggTable->spectralType[line].code == spectralType->code)
        {
            /*
             * And quantities match or star quantity is lower than the one of
             * the current line
             */
            if (teffloggTable->spectralType[line].quantity >= spectralType->quantity)
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
     * in the tefflogg table. The quantity is strictly lower than the first entry
     * of the table 
     */
    if (line >= teffloggTable->nbLines)
    {
        found = mcsFALSE;
    }

    if ((line == 0) && (teffloggTable->spectralType[line].quantity != spectralType->quantity))
    {
        found = mcsFALSE;
    }

    /* If spectral type not found in tefflogg table, return error */
    if (found == mcsFALSE)
    {
        return alxNOT_FOUND;
    }

    /* else return the line found */
    return line;
}

mcsCOMPL_STAT alxComputeTeffAndLoggFromSptype(alxSPECTRAL_TYPE* spectralType,
                                              mcsDOUBLE* Teff,
                                              mcsDOUBLE* LogG)
{
    /* Get the teff,logg table */
    alxTEFFLOGG_TABLE* teffloggTable = alxGetTeffLoggTable();
    FAIL_NULL(teffloggTable);

    /* Line corresponding to the spectral type */
    mcsINT32 line = alxGetLineForTeffLogg(teffloggTable, spectralType);
    /* if line not found, i.e = -1, return mcsFAILURE */
    FAIL_COND(line == alxNOT_FOUND);

    /* interpolate */
    mcsDOUBLE ratio; /* Need to compute ratio */
    mcsINT32 lineSup = line;
    mcsINT32 lineInf = line - 1;

    /* Compute ratio for interpolation */
    mcsDOUBLE sup = teffloggTable->spectralType[lineSup].quantity;
    mcsDOUBLE inf = teffloggTable->spectralType[lineInf].quantity;
    alxSTAR_TYPE lumClass = alxGetLuminosityClass(spectralType);
    mcsDOUBLE subClass = spectralType->quantity;
    if (sup != inf)
    {
        ratio = fabs((subClass - inf) / (sup - inf));
    }
    else
    {
        ratio = 0.5;
    }
    /* logTest("Ratio = %f", ratio); */

    /* Compute Teff */
    mcsDOUBLE dataSup = teffloggTable->teff[lineSup][lumClass];
    mcsDOUBLE dataInf = teffloggTable->teff[lineInf][lumClass];

    FAIL_COND((alxIsBlankingValue(dataSup) == mcsTRUE) || (alxIsBlankingValue(dataInf) == mcsTRUE));

    *Teff = dataInf + ratio * (dataSup - dataInf);

    dataSup = teffloggTable->logg[lineSup][lumClass];
    dataInf = teffloggTable->logg[lineInf][lumClass];
    /* logg is blank if Teff is blank, no need to check here */
    /* We add the LogG of the Sun = 4.378 to get LogG in cm s^-2 */
    *LogG = dataInf + ratio * (dataSup - dataInf) + 4.378;

    return mcsSUCCESS;
}

/**
 * Initialize this file
 */
void alxMissingMagnitudeInit(void)
{
    alxSPECTRAL_TYPE* spectralType = malloc(sizeof (alxSPECTRAL_TYPE));

    /* Initialize the spectral type structure */
    alxInitializeSpectralType(spectralType);

    /* flag as valid */
    spectralType->isSet = mcsTRUE;

    strcpy(spectralType->luminosityClass, "VIII"); /* alxDWARF */
    alxGetColorTableForStar(spectralType);

    strcpy(spectralType->luminosityClass, "IV/III"); /* alxGIANT */
    alxGetColorTableForStar(spectralType);

    strcpy(spectralType->luminosityClass, "I"); /* alxSUPER_GIANT */
    alxGetColorTableForStar(spectralType);

    free(spectralType);

    alxLoadAkariTable();

    alxGetTeffLoggTable();
}

/*___oOo___*/
