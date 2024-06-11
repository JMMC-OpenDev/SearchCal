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


/* log Level to dump av/dist for each spType */
#define LOG_EACH_SPTYPE logDEBUG

/* color table index for IDL code */
#define SCALE_IDX 4.0

/* typical error on absolute magnitudes */
#define ABS_MAG_ERROR   0.1

/* delta threshold to ensure differential magnitude is correct to compute missing magnitudes (0.1) */
#define DELTA_THRESHOLD 0.1

#define setLuminosityClass(spectralType, value) \
    spectralType->starType      = alxSTAR_UNDEFINED; /* reset */ \
    spectralType->otherStarType = alxSTAR_UNDEFINED; /* reset */ \
    strcpy(spectralType->luminosityClass, value);

#define updateOurSpType(spectralType) \
    snprintf(spectralType->ourSpType, sizeof(spectralType->ourSpType) - 1, \
             "%c%4.2lf%s", spectralType->code, spectralType->quantity, spectralType->luminosityClass);

/** compute the distance modulus from the given distance in parsec */
#define getMu(distInPc) \
    (5.0 * log10(distInPc) - 5.0)

/** compute the distance modulus error from the given distance and error in parsec */
#define getMuError(distInPc, e_distInPc) \
    (e_distInPc / (0.2 * LOG_10 * distInPc))

/*
 * Structure of the mapping between luminosity class as string to star types
 */
typedef struct
{
    const char*  lumClass;      /* luminosity class */
    alxSTAR_TYPE starTypeMain;  /* main  star type (I,III,V) */
    alxSTAR_TYPE starTypeOther; /* other star type (equal or different) */
} alxLUM_CLASS_MAPPING;

/* Existing ColorTables */
static alxCOLOR_TABLE colorTables[alxNB_TABLE_STAR_TYPES] = {
    {mcsFALSE, "alxColorTableForDwarfStar.cfg", 0, 0.0,
     {
            {0}},
        {
            {
                {0.0, 0.0, 0, 0}
            }
        },
        {
            {0}
        },
        {
            {
                {0.0, 0.0, 0, 0}
            }
        },
        {
            {0.0, 0.0, 0, 0}
        }, 0, 0},
    {mcsFALSE, "alxColorTableForGiantStar.cfg", 0, 0.0,
     {
            {0}},
        {
            {
                {0.0, 0.0, 0, 0}
            }
        },
        {
            {0}
        },
        {
            {
                {0.0, 0.0, 0, 0}
            }
        },
        {
            {0.0, 0.0, 0, 0}
        }, 0, 0},
    {mcsFALSE, "alxColorTableForSuperGiantStar.cfg", 0, 0.0,
     {
            {0}},
        {
            {
                {0.0, 0.0, 0, 0}
            }
        },
        {
            {0}
        },
        {
            {
                {0.0, 0.0, 0, 0}
            }
        },
        {
            {0.0, 0.0, 0, 0}
        }, 0, 0}
};

/*
 * Local Functions declaration
 */

static alxCOLOR_TABLE* alxGetColorTableForStar(alxSPECTRAL_TYPE* spectralType);
static alxCOLOR_TABLE* alxGetColorTableForStarType(alxSTAR_TYPE starType);
static alxCOLOR_TABLE* alxGetColorTableForTableStarType(alxTABLE_STAR_TYPE tableStarType);

static alxSTAR_TYPE alxGetLuminosityClass(alxSPECTRAL_TYPE* spectralType);

static mcsINT32 alxGetLineFromSpectralType(alxCOLOR_TABLE *colorTable,
                                           alxSPECTRAL_TYPE *spectralType,
                                           mcsLOGICAL checkMissing);

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

static alxTABLE_STAR_TYPE alxGetTableStarType(alxSTAR_TYPE starType);

static void alxUpdateOurSpType(alxSPECTRAL_TYPE *spectralType);

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
    alxSTAR_TYPE starType      = alxDWARF;
    alxSTAR_TYPE otherStarType = alxSTAR_UNDEFINED;

    /* If no spectral type given or wrong format */
    if (IS_NULL(spectralType) || IS_FALSE(spectralType->isSet))
    {
        logTest("Type of star=%s (by default as no Spectral Type provided)", alxGetStarTypeLabel(starType));
        return starType;
    }

    /* If already parsed, return cached star type */
    if (spectralType->starType != alxSTAR_UNDEFINED)
    {
        return spectralType->starType;
    }

    /* If no luminosity class given */
    if (strlen(spectralType->luminosityClass) == 0)
    {
        logTest("Type of star=%s (by default as no Luminosity Class provided)", alxGetStarTypeLabel(starType));
        return starType;
    }

    /* Determination of star types according to the spectral type (no '/' but '-' instead) */
    static alxLUM_CLASS_MAPPING lumClassMappings[] = {
        {"VIII", alxDWARF, alxDWARF},
        {"VII", alxDWARF, alxDWARF},
        {"VI", alxDWARF, alxDWARF},
        {"V-VI", alxDWARF, alxDWARF},
        {"V-IV", alxDWARF, alxSUB_GIANT},
        {"IV-V", alxDWARF, alxSUB_GIANT},
        {"III-V", alxGIANT, alxDWARF},
        {"III-IV", alxGIANT, alxSUB_GIANT},
        {"IV-III", alxGIANT, alxSUB_GIANT},
        {"II-III", alxGIANT, alxSUB_SUPER_GIANT},
        {"I-II", alxSUPER_GIANT, alxSUB_SUPER_GIANT},
        {"III", alxGIANT, alxGIANT},
        {"IB-II", alxSUPER_GIANT, alxSUB_SUPER_GIANT},
        {"IBV", alxSUPER_GIANT, alxSUPER_GIANT},
        {"II", alxSUB_SUPER_GIANT, alxSUB_SUPER_GIANT},
        {"IV", alxSUB_GIANT, alxSUB_GIANT},
        {"V", alxDWARF, alxDWARF},
        {"IA-O-IA", alxSUPER_GIANT, alxSUPER_GIANT},
        {"IA-O", alxSUPER_GIANT, alxSUPER_GIANT},
        {"IA-AB", alxSUPER_GIANT, alxSUPER_GIANT},
        {"IAB-B", alxSUPER_GIANT, alxSUPER_GIANT},
        {"IAB", alxSUPER_GIANT, alxSUPER_GIANT},
        {"IA", alxSUPER_GIANT, alxSUPER_GIANT},
        {"IB", alxSUPER_GIANT, alxSUPER_GIANT},
        {"I", alxSUPER_GIANT, alxSUPER_GIANT},
        {NULL, alxSTAR_UNDEFINED, alxSTAR_UNDEFINED}
    };

    const char* luminosityClass = spectralType->luminosityClass;

    mcsLOGICAL found = mcsFALSE;
    mcsUINT32 index;

    /* first try exact search */
    for (index = 0; IS_NOT_NULL(lumClassMappings[index].lumClass); index++)
    {
        /* If the current spectral type is found */
        if (strcmp(luminosityClass, lumClassMappings[index].lumClass) == 0)
        {
            found = mcsTRUE;
            /* Get the corresponding main  luminosity class */
            starType = lumClassMappings[index].starTypeMain;
            /* Get the corresponding other luminosity class */
            otherStarType = lumClassMappings[index].starTypeOther;
            break;
        }
    }

    if (IS_FALSE(found))
    {
        logDebug("alxGetLuminosityClass: luminosityClass[%s] not found from [%s ~ %s]; use PARTIAL SEARCH...",
                 luminosityClass, spectralType->origSpType, spectralType->ourSpType);

        /* use partial search (take care of spectral type order to avoid side effects) */
        for (index = 0; IS_NOT_NULL(lumClassMappings[index].lumClass); index++)
        {
            /* If the current spectral type is found */
            if (IS_NOT_NULL(strstr(luminosityClass, lumClassMappings[index].lumClass)))
            {
                found = mcsTRUE;

                /* Get the corresponding main  luminosity class */
                starType = lumClassMappings[index].starTypeMain;
                /* Get the corresponding other luminosity class */
                otherStarType = lumClassMappings[index].starTypeOther;

                logDebug("alxGetLuminosityClass: luminosityClass[%s] found partially from [%s ~ %s]; use [%s] = %s / %s (to be checked)",
                         luminosityClass, spectralType->origSpType, spectralType->ourSpType, lumClassMappings[index].lumClass,
                         alxGetStarTypeLabel(starType), alxGetStarTypeLabel(otherStarType));

                /* fix luminosity class */
                setLuminosityClass(spectralType, lumClassMappings[index].lumClass);
                break;
            }
        }

        if (IS_FALSE(found))
        {
            logTest("alxGetLuminosityClass: luminosityClass[%s] NOT FOUND from [%s ~ %s]; discard invalid Luminosity Class",
                    luminosityClass, spectralType->origSpType, spectralType->ourSpType);

            /* No spectral type found; reset luminosity class to try correcting it (see CorrectSpType) */
            setLuminosityClass(spectralType, "\0");
        }

        /* Update ourSpType string anyway */
        updateOurSpType(spectralType);

        logTest("alxGetLuminosityClass: spectral type='%s' - our spectral type='%s': updated luminosity class='%s'",
                spectralType->origSpType, spectralType->ourSpType, spectralType->luminosityClass);
    }

    /* store star types into spectral type */
    spectralType->starType      = starType;
    spectralType->otherStarType = otherStarType;

    /* Print out type of star */
    if (starType != otherStarType)
    {
        logTest("Type of star=[%s - %s]", alxGetStarTypeLabel(starType), alxGetStarTypeLabel(otherStarType));
    }
    else
    {
        logTest("Type of star=%s", alxGetStarTypeLabel(starType));
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
 *
 * @return pointer to structure containing color table, or NULL if an error
 * occurred.
 *
 * @usedfiles Files containing the color indexes, the absolute magnitude in V
 * and the stellar mass according to the temperature class for different star
 * types. These tables are used to compute missing magnitudes and AV.
 */
static alxCOLOR_TABLE* alxGetColorTableForStar(alxSPECTRAL_TYPE* spectralType)
{
    /* Determination of the main star type according to the given star type */
    alxSTAR_TYPE starType = alxGetLuminosityClass(spectralType);

    return alxGetColorTableForStarType(starType);
}

/**
 * Return the color table corresponding to a given star type.
 *
 * This method determines the color table associated to the given spectral
 * type of a bright star, and reads (if not yet done) this table from
 * the configuration file.
 *
 * @param starType star type
 *
 * @return pointer to structure containing color table, or NULL if an error
 * occurred.
 *
 * @usedfiles Files containing the color indexes, the absolute magnitude in V
 * and the stellar mass according to the temperature class for different star
 * types. These tables are used to compute missing magnitudes.
 *  - alxColorTableForDwarfStar.cfg : dwarf star
 *  - see code for other tables!
 */
static alxCOLOR_TABLE* alxGetColorTableForStarType(alxSTAR_TYPE starType)
{
    return alxGetColorTableForTableStarType(alxGetTableStarType(starType));
}

/**
 * Return the color table corresponding to a given table star type.
 *
 * This method determines the color table associated to the given spectral
 * type of a bright star, and reads (if not yet done) this table from
 * the configuration file.
 *
 * @param starType table star type
 *
 * @return pointer to structure containing color table, or NULL if an error
 * occurred.
 *
 * @usedfiles Files containing the color indexes, the absolute magnitude in V
 * and the stellar mass according to the temperature class for different star
 * types. These tables are used to compute missing magnitudes.
 *  - alxColorTableForDwarfStar.cfg : dwarf star
 *  - see code for other tables!
 */
static alxCOLOR_TABLE* alxGetColorTableForTableStarType(alxTABLE_STAR_TYPE tableStarType)
{
    if ((tableStarType < alxTABLE_DWARF) || (tableStarType > alxTABLE_SUPER_GIANT))
    {
        return NULL;
    }
    alxCOLOR_TABLE* colorTable = &colorTables[tableStarType];

    /* Check if the structure is loaded into memory. If not load it. */
    if (IS_TRUE(colorTable->loaded))
    {
        return colorTable;
    }

    /* Find the location of the file */
    char* fileName = miscLocateFile(colorTable->fileName);
    if (IS_NULL(fileName))
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
    mcsUINT32 lineNum = 0;
    const char *pos = NULL;
    mcsSTRING1024 line;
    mcsDOUBLE values[alxNB_COLOR_INDEXES];
    mcsUINT32 i;
    alxDATA* colorTableRow;
    mcsDOUBLE value;
    char prevCode = '\0';
    mcsUINT32 lineIndexNum = 0;
    alxDATA* absMagRow;
    mcsUINT32 absMagCount;
    mcsUINT32 absMagLineFirst = alxNB_SPECTRAL_TYPES;
    mcsUINT32 absMagLineLast  = 0;

    while (IS_NOT_NULL(pos = miscDynBufGetNextLine(&dynBuf, pos, line, sizeof (line), mcsTRUE)))
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

            /* Try to read each values
             * # TC    B-V   V-Ic    V-R   Ic-Jc  Jc-Hc  Jc-Kc  Kc-L    L-M     Mv
             */
            if (sscanf(line, "%c%lf %lf %lf %lf %lf %lf %lf %lf %lf %lf",
                       &colorTable->spectralType[lineNum].code,
                       &colorTable->spectralType[lineNum].quantity,
                       &values[0],
                       &values[1],
                       &values[2],
                       &values[3],
                       &values[4],
                       &values[5],
                       &values[6],
                       &values[7],
                       &values[8]) != (alxNB_COLOR_INDEXES + 2)) /* If parsing went wrong */
            {
                /* Destroy the temporary dynamic buffer, raise an error and return */
                miscDynBufDestroy(&dynBuf);
                errAdd(alxERR_WRONG_FILE_FORMAT, line, fileName);
                free(fileName);
                return NULL;
            }

            /* prepare the line index */
            if (prevCode != colorTable->spectralType[lineNum].code)
            {
                colorTable->lineIndex[lineIndexNum].code = prevCode = colorTable->spectralType[lineNum].code;
                colorTable->lineIndex[lineIndexNum].firstLine = lineNum;

                lineIndexNum++;

                /* check capacity */
                if (lineIndexNum > alxNB_SPECTRAL_TYPE_CODES)
                {
                    logError("Line index limit reached: %d !", alxNB_SPECTRAL_TYPE_CODES);

                    /* Destroy the temporary dynamic buffer, raise an error and return */
                    miscDynBufDestroy(&dynBuf);
                    errAdd(alxERR_WRONG_FILE_FORMAT, line, fileName);
                    free(fileName);
                    return NULL;
                }
            }

            colorTableRow = colorTable->index[lineNum];

            /* Set whether each colorTable cell has been written or not */
            for (i = 0; i < alxNB_COLOR_INDEXES; i++)
            {
                value = values[i];

                if (alxIsBlankingValue(value))
                {
                    colorTableRow[i].isSet = mcsFALSE;
                    colorTableRow[i].value = NAN;
                }
                else
                {
                    colorTableRow[i].isSet = mcsTRUE;
                    colorTableRow[i].value = value;
                }
            }

            /* initialize absolute magnitudes for the current row */
            absMagRow = colorTable->absMag[lineNum];
            for (i = alxB_BAND; i < alxL_BAND; i++)
            {
                absMagRow[i].isSet = mcsFALSE;
                absMagRow[i].value = NAN;
            }
            /* initialize error on absolute magnitudes for the current row to the default value */
            colorTable->absMagError[lineNum].isSet = mcsTRUE;
            colorTable->absMagError[lineNum].value = ABS_MAG_ERROR;


            /* compute absolute magnitudes from color table [B-V  V-Ic  V-R  Ic-J2  J2-H2  J2-K2  K2-L  L-M  Mv] */
            if (alxIsSet(colorTableRow[alxMv]))
            {
                absMagRow[alxV_BAND].isSet = mcsTRUE;
                absMagRow[alxV_BAND].value = colorTableRow[alxMv].value;

                /* MB=MV + B_V */
                if (IS_TRUE(colorTableRow[alxB_V].isSet))
                {
                    absMagRow[alxB_BAND].isSet = mcsTRUE;
                    absMagRow[alxB_BAND].value = absMagRow[alxV_BAND].value + colorTableRow[alxB_V].value;
                }
                /* MR=MV - V_R */
                if (IS_TRUE(colorTableRow[alxV_R].isSet))
                {
                    absMagRow[alxR_BAND].isSet = mcsTRUE;
                    absMagRow[alxR_BAND].value = absMagRow[alxV_BAND].value - colorTableRow[alxV_R].value;
                }
                /* MIc=MV - V_Ic */
                if (IS_TRUE(colorTableRow[alxV_I].isSet))
                {
                    absMagRow[alxI_BAND].isSet = mcsTRUE;
                    absMagRow[alxI_BAND].value = absMagRow[alxV_BAND].value - colorTableRow[alxV_I].value;

                    /* MJ2=MIc - Ic_J2 */
                    if (IS_TRUE(colorTableRow[alxI_J].isSet))
                    {
                        absMagRow[alxJ_BAND].isSet = mcsTRUE;
                        absMagRow[alxJ_BAND].value = absMagRow[alxI_BAND].value - colorTableRow[alxI_J].value;

                        /* MH2=MJ2 - J2_H2 */
                        if (IS_TRUE(colorTableRow[alxJ_H].isSet))
                        {
                            absMagRow[alxH_BAND].isSet = mcsTRUE;
                            absMagRow[alxH_BAND].value = absMagRow[alxJ_BAND].value - colorTableRow[alxJ_H].value;
                        }
                        /* MK2=MJ2 - J2_K2 */
                        if (IS_TRUE(colorTableRow[alxJ_K].isSet))
                        {
                            absMagRow[alxK_BAND].isSet = mcsTRUE;
                            absMagRow[alxK_BAND].value = absMagRow[alxJ_BAND].value - colorTableRow[alxJ_K].value;
                        }

                        /* Convert IR absolute magnitudes to 2MASS photometric system */
                        if (alxIsSet(absMagRow[alxK_BAND]))
                        {
                            /* Kc */
                            absMagRow[alxK_BAND].value -= 0.024; /* AA(5, *) = AA(5, *) - 0.024D */

                            if (alxIsSet(absMagRow[alxH_BAND]))
                            {
                                /* Hc */
                                /* AA(4, *) = (ABS_MAG_DWARF(4, *) - ABS_MAG_DWARF(5, *))*1.026D + 0.028D + AA(5, *) */
                                absMagRow[alxH_BAND].value = absMagRow[alxK_BAND].value + 0.028
                                        + 1.026 * (absMagRow[alxH_BAND].value - absMagRow[alxK_BAND].value);
                            }

                            if (alxIsSet(absMagRow[alxJ_BAND]))
                            {
                                /* Jc */
                                /* AA(3, *) = (ABS_MAG_DWARF(3, *) - ABS_MAG_DWARF(5, *))*1.056D - 0.013D + AA(5, *) */
                                absMagRow[alxJ_BAND].value = absMagRow[alxK_BAND].value - 0.013
                                        + 1.056 * (absMagRow[alxJ_BAND].value - absMagRow[alxK_BAND].value);
                            }
                        }
                        else
                        {
                            /* reset JHK 2MASS */
                            absMagRow[alxJ_BAND].isSet = mcsFALSE;
                            absMagRow[alxJ_BAND].value = NAN;
                            absMagRow[alxH_BAND].isSet = mcsFALSE;
                            absMagRow[alxH_BAND].value = NAN;
                            absMagRow[alxK_BAND].isSet = mcsFALSE;
                            absMagRow[alxK_BAND].value = NAN;
                        }
                    }
                }

                /* check if there are at least 4 absolute magnitudes */
                absMagCount = 0;
                for (i = alxB_BAND; i < alxL_BAND; i++)
                {
                    if (alxIsSet(absMagRow[i]))
                    {
                        absMagCount++;
                    }
                }

                if (absMagCount >= 4)
                {
                    /* update line range */
                    if (absMagLineFirst > lineNum)
                    {
                        absMagLineFirst = lineNum;
                    }
                    if (absMagLineLast < lineNum)
                    {
                        absMagLineLast = lineNum;
                    }
                }
            }

            /* Next line */
            lineNum++;
        }
    }

    /* Set the total number of lines in the color table */
    colorTable->nbLines = lineNum;

    /* Set the valid line range for absolute magnitudes */
    colorTable->absMagLineFirst = absMagLineFirst;
    colorTable->absMagLineLast  = absMagLineLast;

    logDebug("absMag Lines: [%d - %d]", absMagLineFirst, absMagLineLast);

    /* Initialize the end of the line index if necessary */
    for (; lineIndexNum < alxNB_SPECTRAL_TYPE_CODES; lineIndexNum++)
    {
        colorTable->lineIndex[lineIndexNum].code = '\0';
        colorTable->lineIndex[lineIndexNum].firstLine = 0;
    }

    /** Set the quantity step between lines (supposing a regular sampling) using the first two lines */
    colorTable->step = (lineNum > 0) ? (colorTable->spectralType[1].quantity - colorTable->spectralType[0].quantity) : 0.0;

    /* Mark the color table as "loaded" */
    colorTable->loaded = mcsTRUE;

    /* Destroy the temporary dynamic buffer used to parse the color table file */
    miscDynBufDestroy(&dynBuf);
    free(fileName);

    if (doLog(logDEBUG))
    {
        for (lineIndexNum = absMagLineFirst; lineIndexNum <= absMagLineLast; lineIndexNum++)
        {
            logDebug("[%c%.2lf] = B=%7.3lf V=%7.3lf R=%7.3lf I=%7.3lf J=%7.3lf H=%7.3lf K=%7.3lf",
                     colorTable->spectralType[lineIndexNum].code,
                     colorTable->spectralType[lineIndexNum].quantity,
                     colorTable->absMag[lineIndexNum][alxB_BAND].value,
                     colorTable->absMag[lineIndexNum][alxV_BAND].value,
                     colorTable->absMag[lineIndexNum][alxR_BAND].value,
                     colorTable->absMag[lineIndexNum][alxI_BAND].value,
                     colorTable->absMag[lineIndexNum][alxJ_BAND].value,
                     colorTable->absMag[lineIndexNum][alxH_BAND].value,
                     colorTable->absMag[lineIndexNum][alxK_BAND].value
                     );
        }
    }

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
    mcsUINT32 line = 0;
    mcsLOGICAL found = mcsFALSE;

    while (IS_FALSE(found) && (line < colorTable->nbLines))
    {
        /* check blanking values */
        if (alxIsSet(colorTable->index[line][diffMagId]))
        {
            if (colorTable->index[line][diffMagId].value == diffMag) /* If diffMag in table == diffMag */
            {
                found = mcsTRUE;
            }
            else if (colorTable->index[line][diffMagId].value > diffMag) /* If diffMag in table > diffMag */
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
        else
        {
            line++;
        }
    }

    if (IS_FALSE(found))
    {
        return alxNOT_FOUND;
    }

    /* return the line found */
    return line;
}

/**
 * Get the line in the color table which matches the spectral type.
 * Return the line equal or just after.
 *
 * @param colorTable color table
 * @param spectralType spectral type structure
 * @param checkMissing true to return -1 if missing spectral type; false to return 0 or last line
 *
 * @return a line number that matches the spectral type or the line just after
 * or -1 if no match is found
 */
static mcsINT32 alxGetLineFromSpectralType(alxCOLOR_TABLE *colorTable,
                                           alxSPECTRAL_TYPE *spectralType,
                                           mcsLOGICAL checkMissing)
{
    /* If spectral type is unknown, return not found */
    if (IS_NULL(spectralType) || IS_FALSE(spectralType->isSet))
    {
        return alxNOT_FOUND;
    }

    mcsUINT32 lineIndexNum, line = 0;

    /* Use line index to find first line matching spectral type's code */
    for (lineIndexNum = 0; lineIndexNum < alxNB_SPECTRAL_TYPE_CODES; lineIndexNum++)
    {
        if (colorTable->lineIndex[lineIndexNum].code == spectralType->code)
        {
            line = colorTable->lineIndex[lineIndexNum].firstLine;
            break;
        }
    }

    mcsLOGICAL codeFound = mcsFALSE;
    mcsLOGICAL found     = mcsFALSE;

    while (IS_FALSE(found) && (line < colorTable->nbLines))
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
            if (IS_TRUE(codeFound))
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
    if ((line == 0) && (colorTable->spectralType[line].quantity > spectralType->quantity))
    {
        found = mcsFALSE;
    }

    /* If spectral type not found in color table, return -1 (not found) */
    if (IS_FALSE(found))
    {
        if (IS_TRUE(checkMissing))
        {
            logWarning("Cannot find spectral type '%s' in '%s'", spectralType->origSpType, colorTable->fileName);

            return alxNOT_FOUND;
        }
        else
        {
            /* fix last line */
            if (line >= colorTable->nbLines)
            {
                line = colorTable->nbLines - 1;
            }
            logTest("Cannot find spectral type '%s' in '%s' but use line=%d [%c%.2lf]", spectralType->origSpType, colorTable->fileName,
                    line, colorTable->spectralType[line].code, colorTable->spectralType[line].quantity);
        }
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

    /* Initialize differential magnitudes structure */
    for (i = 0; i < alxNB_DIFF_MAG; i++)
    {
        diffMagnitudes[i].value = 0.0;
        diffMagnitudes[i].isSet = mcsFALSE;
    }

    /* check blanking values */
    if (alxIsSet(colorTable->index[lineSup][magDiffId]) && alxIsSet(colorTable->index[lineInf][magDiffId]))
    {
        /* Compute ratio for interpolation. Note that this will take
           care of the case lineInf==lineSup (although sub-optimal for speed) */
        if (colorTable->index[lineSup][magDiffId].value != colorTable->index[lineInf][magDiffId].value)
        {
            ratio = fabs((magDiff - colorTable->index[lineInf][magDiffId].value)
                         / (colorTable->index[lineSup][magDiffId].value - colorTable->index[lineInf][magDiffId].value));
        }
        else
        {
            /* If both value in the ref column are equal, take the average */
            ratio = 0.5;
        }
    }
    else
    {
        /* lineInf or lineSup contain a blanking value => do not compute diff mags */
        return mcsSUCCESS;
    }

    /* Loop on differential magnitudes that are on the table
       (all except the last one which is K_M)  */
    for (i = 0; i < alxNB_DIFF_MAG - 1; i++)
    {
        /* Extract the value Sup and inf*/
        dataSup = &colorTable->index[lineSup][i];
        dataInf = &colorTable->index[lineInf][i];

        /* If both values are set, compute the interpolation (check blanking values) */
        if (alxIsSet(*dataSup) && alxIsSet(*dataInf))
        {
            diffMagnitudes[i].value = dataInf->value + ratio * (dataSup->value - dataInf->value);
            diffMagnitudes[i].isSet = mcsTRUE;
            diffMagnitudes[i].confIndex = (ratio > 1.0) ? alxCONFIDENCE_LOW : alxCONFIDENCE_MEDIUM;
        }
    }

    /* Now compute K_M (not in colorTable) from K-L & L-M (check blanking values) */
    if (alxIsSet(colorTable->index[lineSup][alxK_L]) && alxIsSet(colorTable->index[lineInf][alxK_L]) &&
            alxIsSet(colorTable->index[lineSup][alxL_M]) && alxIsSet(colorTable->index[lineInf][alxL_M]))
    {
        diffMagnitudes[alxK_M].value = colorTable->index[lineInf][alxK_L].value + colorTable->index[lineInf][alxL_M].value
                + ratio * (colorTable->index[lineSup][alxK_L].value + colorTable->index[lineSup][alxL_M].value
                - colorTable->index[lineInf][alxK_L].value - colorTable->index[lineInf][alxL_M].value);
        diffMagnitudes[alxK_M].isSet = mcsTRUE;
        diffMagnitudes[i].confIndex = (ratio > 1.0) ? alxCONFIDENCE_LOW : alxCONFIDENCE_MEDIUM;
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
    if (IS_FALSE(magnitude->isSet) && alxIsSet(diffMag))
    {
        /* Compute*/
        magnitude->value = firstMag + factor * diffMag.value;

        /* Set correct confidence index */
        magnitude->confIndex = alxMin(confIndex, diffMag.confIndex);
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
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
 */
mcsCOMPL_STAT alxInitializeSpectralType(alxSPECTRAL_TYPE* decodedSpectralType)
{
    FAIL_NULL_DO(decodedSpectralType, errAdd(alxERR_NULL_PARAMETER, "decodedSpectralType"));

    /* Initialize Spectral Type structure */
    decodedSpectralType->isSet            = mcsFALSE;
    decodedSpectralType->isInvalid        = mcsFALSE;
    decodedSpectralType->origSpType[0]    = '\0';
    decodedSpectralType->ourSpType[0]     = '\0';
    decodedSpectralType->code             = '\0';
    decodedSpectralType->quantity         = NAN;
    decodedSpectralType->deltaQuantity    = NAN;
    decodedSpectralType->isDouble         = mcsFALSE;
    decodedSpectralType->isSpectralBinary = mcsFALSE;
    decodedSpectralType->isVariable       = mcsFALSE;
    decodedSpectralType->isCorrected      = mcsFALSE;
    /* initialize luminosityClass, starType, otherStarType: */
    setLuminosityClass(decodedSpectralType, "\0");

    return mcsSUCCESS;
}

static mcsDOUBLE alxParseSpectralCode(char code)
{
    /*
     * O0 - O9 => [00..09]
     * B0 - B9 => [10..19]
     * A0 - A9 => [20..29]
     * F0 - F9 => [30..39]
     * G0 - G9 => [40..49]
     * K0 - K9 => [50..59]
     * M0 - M9 => [60..69]
     */
    switch (code)
    {
        case 'O':
            return 0.0;
        case 'B':
            return 10.0;
        case 'A':
            return 20.0;
        case 'F':
            return 30.0;
        case 'G':
            return 40.0;
        case 'K':
            return 50.0;
        case 'M':
            return 60.0;
        default:
            /* unsupported code */
            return -1.0;
    }
}

static char alxConvertSpectralCode(mcsDOUBLE *code)
{
    /*
     * O0 - O9 => [00..09]
     * B0 - B9 => [10..19]
     * A0 - A9 => [20..29]
     * F0 - F9 => [30..39]
     * G0 - G9 => [40..49]
     * K0 - K9 => [50..59]
     * M0 - M9 => [60..69]
     */
    if (*code < 0.0)
    {
        /* unsupported code */
        return '_';
    }
    else if (*code < 10.0)
    {
        return 'O';
    }
    else if (*code < 20.0)
    {
        *code -= 10.0;
        return 'B';
    }
    else if (*code < 30.0)
    {
        *code -= 20.0;
        return 'A';
    }
    else if (*code < 40.0)
    {
        *code -= 30.0;
        return 'F';
    }
    else if (*code < 50.0)
    {
        *code -= 40.0;
        return 'G';
    }
    else if (*code < 60.0)
    {
        *code -= 50.0;
        return 'K';
    }
    else if (*code < 70.0)
    {
        *code -= 60.0;
        return 'M';
    }
    /* unsupported code */
    return '_';
}

static mcsDOUBLE alxParseLumClass(alxSTAR_TYPE starType)
{
    switch (starType)
    {
        case alxSUPER_GIANT:
            return 1.0;
        case alxSUB_SUPER_GIANT:
            return 2.0;
        case alxGIANT:
            return 3.0;
        case alxSUB_GIANT:
            return 4.0;
        case alxDWARF:
            return 5.0;
        default:
            return 0.0;
    }
}

static mcsDOUBLE alxConvertLumClass(alxSTAR_TYPE starType)
{
    mcsDOUBLE lc = alxParseLumClass(starType);
    return (lc == 0.0) ? -1.0 : lc;
}

static const char* alxGetLumClass(alxSTAR_TYPE starType)
{
    switch (starType)
    {
        case alxSUPER_GIANT:
            return "I";
        case alxSUB_SUPER_GIANT:
            return "II";
        case alxGIANT:
            return "III";
        case alxSUB_GIANT:
            return "IV";
        case alxDWARF:
            return "V";
        default:
            return '\0';
    }
}

/*
 * Return the type of star used in tables (teff/logg and color tables).
 */
static alxTABLE_STAR_TYPE alxGetTableStarType(alxSTAR_TYPE starType)
{
    switch (starType)
    {
        case alxSUPER_GIANT:
            return alxTABLE_SUPER_GIANT;
        case alxSUB_SUPER_GIANT:
        case alxGIANT:
            return alxTABLE_GIANT;
        case alxSUB_GIANT:
        case alxDWARF:
        default:
            return alxTABLE_DWARF;
    }
}

/*
 * Return the star type corresponding to the type of star used in tables (teff/logg and color tables).
 */
static alxSTAR_TYPE alxGetStarType(alxTABLE_STAR_TYPE starType)
{
    switch (starType)
    {
        case alxTABLE_SUPER_GIANT:
            return alxSUPER_GIANT;
        case alxTABLE_GIANT:
            return alxGIANT;
        case alxTABLE_DWARF:
        default:
            return alxDWARF;
    }
}

void alxGiveIndexInTableOfSpectralCodes(alxSPECTRAL_TYPE spectralType,
                                        mcsINT32 *colorTableIndex,
                                        mcsINT32 *colorTableDelta,
                                        mcsINT32 *lumClass,
                                        mcsINT32 *lumClassDelta)
{
    /* Initialize color table index, delta and luminosity class */
    *colorTableIndex = alxNOT_FOUND;
    *colorTableDelta = alxNOT_FOUND;
    *lumClass        = alxNOT_FOUND;
    *lumClassDelta   = alxNOT_FOUND;

    if (IS_TRUE(spectralType.isSet))
    {
        /* Update line index, delta and luminosity class */
        mcsDOUBLE value = alxParseSpectralCode(spectralType.code);

        *colorTableIndex = (value == -1.0) ? -1 : (int) (SCALE_IDX * (value + spectralType.quantity));
        *colorTableDelta = (int) (SCALE_IDX * spectralType.deltaQuantity);

        /* has luminosity class ? */
        if (spectralType.otherStarType != alxSTAR_UNDEFINED)
        {
            mcsDOUBLE lcMain  = alxConvertLumClass(spectralType.starType);

            /* other luminosity class ? */
            mcsDOUBLE lcOther = (spectralType.otherStarType != spectralType.starType) ?
                    alxConvertLumClass(spectralType.otherStarType) : lcMain;

            *lumClass      = (mcsINT32) alxMin(lcMain, lcOther);
            *lumClassDelta = (mcsINT32) fabs(lcMain - lcOther);
        }
    }
}

void alxFixSpType(mcsUINT32 colorTableIndex,
                  mcsUINT32 colorTableDelta,
                  alxSPECTRAL_TYPE* spectralType)
{
    /* set flag for FAINT case */
    spectralType->isSet = mcsTRUE;

    /* set corrected flag (quantity) */
    spectralType->isCorrected = mcsTRUE;

    /* fix code / quantity from best line index */
    mcsDOUBLE index = colorTableIndex / SCALE_IDX;

    spectralType->code     = alxConvertSpectralCode(&index); /* index = remainder */
    spectralType->quantity = index;
    spectralType->deltaQuantity = (colorTableDelta / SCALE_IDX);

    alxUpdateOurSpType(spectralType);
}

static void alxUpdateOurSpType(alxSPECTRAL_TYPE *spectralType)
{
    mcsSTRING16 spPart;
    spPart[0] = '\0';
    if (spectralType->deltaQuantity == 0)
    {
        snprintf(spPart, sizeof (spPart) - 1, "%c%4.2lf", spectralType->code, spectralType->quantity);
    }
    else
    {
        mcsDOUBLE code = alxParseSpectralCode(spectralType->code) + spectralType->quantity;
        mcsDOUBLE index1 = (code - spectralType->deltaQuantity);
        mcsDOUBLE index2 = (code + spectralType->deltaQuantity);

        char c1 = alxConvertSpectralCode(&index1); /* index1 = remainder */
        char c2 = alxConvertSpectralCode(&index2); /* index2 = remainder */

        snprintf(spPart, sizeof (spPart) - 1, "%c%4.2lf/%c%4.2lf", c1, index1, c2, index2);
    }

    mcsSTRING8 lumPart;
    lumPart[0] = '\0';
    if (strlen(spectralType->luminosityClass) != 0)
    {
        snprintf(lumPart, sizeof (lumPart) - 1, "(%s)", spectralType->luminosityClass);
    }

    snprintf(spectralType->ourSpType, sizeof (spectralType->ourSpType) - 1, "%s%s", spPart, lumPart);

    logInfo("Spectral type='%s' - Our spectral type='%s': updated spectral code / quantity class='%c%.2lf' (%.2lf)",
            spectralType->origSpType, spectralType->ourSpType,
            spectralType->code, spectralType->quantity, spectralType->deltaQuantity);
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
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
 */
mcsCOMPL_STAT alxString2SpectralType(mcsSTRING32 spectralType,
                                     alxSPECTRAL_TYPE* decodedSpectralType)
{
    /*
     * Note: SpType comes from ASCC:
     * Spectral classes in the MK or HD systems were taken from Hipparcos,
     * CMC11, PPM, and Tycho-2 Spectral Type catalogues.
     *
     * Tycho-2 Spectral Type catalog. (III/231)
     * We have obtained spectral types for 351,863 stars in the Tycho-2
     * catalog I/259 by cross-referencing the Tycho-2 catalog against the
     * Michigan Catalogs III/31, III/51, III/80, III/133, and
     * III/214; the Catalog of Stellar Spectra Classified in the
     * Morgan-Keenan System III/18; the MK Classification Extension
     * III/78; the FK5 catalog parts I and II I/149 and I/175; and the
     * PPM North and South catalogs I/146 and I/193.
     *
     * From III/214/vol5:
     * http://cdsarc.u-strasbg.fr/viz-bin/getCatFile_Redirect/?-plus=-%2b&III/214/./preface.tex
     */

    /* initialize the spectral type structure anyway */
    FAIL(alxInitializeSpectralType(decodedSpectralType));

    /* Function parameter check */
    FAIL_NULL_DO(spectralType, errAdd(alxERR_NULL_PARAMETER, "spectralType"));

    mcsSTRING32 tempSP;
    mcsDOUBLE deltaSubType = 0.0;

    /* copy spectral type */
    strncpy(decodedSpectralType->origSpType, spectralType, sizeof (decodedSpectralType->origSpType) - 1);
    strncpy(tempSP, spectralType, sizeof (tempSP) - 1);

    mcsUINT32 bufferLength = strlen(tempSP) + 1;

    logDebug("Original spectral type = '%s'.", spectralType);

    /* Remove chars that can be ignored (LBO: 18/11/2013) */
    miscDeleteChr(tempSP, '$', mcsTRUE);
    miscDeleteChr(tempSP, '?', mcsTRUE);
    miscDeleteChr(tempSP, '*', mcsTRUE); /* ?? */


    /*
     * Notice variability and remove it (var or 'v')
     * check it first to avoid confusion with luminosity classes (V in upper case)
     */
    char* tokenPosition = strstr(tempSP, "var"); /* (case sensitive) */
    if (IS_NOT_NULL(tokenPosition))
    {
        *tokenPosition++ = ' ';
        *tokenPosition++ = ' ';
        *tokenPosition++ = ' ';
        logDebug("Un-VAR spectral type = '%s'.", tempSP);
        decodedSpectralType->isVariable = mcsTRUE;
    }
    /*
     *
     * "vega" is a spectral type, apparently. Convert to vega's sptype, i.e., A0V
     */
    tokenPosition = strstr(tempSP, "vega"); /* case sensitive */
    if (IS_NOT_NULL(tokenPosition))
    {
        strncpy(tempSP, "A0V", sizeof (tempSP) - 1);
    }

    tokenPosition = strstr(tempSP, "v"); /* case sensitive */
    if (IS_NOT_NULL(tokenPosition))
    {
        *tokenPosition++ = ' ';
        logDebug("Un-VAR(2) spectral type = '%s'.", tempSP);
        decodedSpectralType->isVariable = mcsTRUE;
    }
    /*
     * Remove special classes (dXX, gXX, kXX, wXX, rXX) to avoid confusion with Dx, Kx and Gx classes
     */
    tokenPosition = strstr(tempSP, "d");
    if (tokenPosition == tempSP)
    {
        *tokenPosition++ = ' ';
        logDebug("Un-d spectral type = '%s'.", tempSP);
    }
    tokenPosition = strstr(tempSP, "D");
    if (tokenPosition == tempSP)
    {
        *tokenPosition++ = ' ';
        logDebug("Warning: Un-D (abnormal) spectral type = '%s'.", tempSP);
    }
    tokenPosition = strstr(tempSP, "g");
    if (tokenPosition == tempSP)
    {
        *tokenPosition++ = ' ';
        logDebug("Un-g spectral type = '%s'.", tempSP);
    }
    tokenPosition = strstr(tempSP, "k");
    if (tokenPosition == tempSP)
    {
        *tokenPosition++ = ' ';
        logDebug("Un-k spectral type = '%s'.", tempSP);
    }
    tokenPosition = strstr(tempSP, "r");
    if (tokenPosition == tempSP)
    {
        *tokenPosition++ = ' ';
        logDebug("Un-k spectral type = '%s'.", tempSP);
    }
    tokenPosition = strstr(tempSP, "w");
    if (tokenPosition == tempSP)
    {
        *tokenPosition++ = ' ';
        logDebug("Un-k spectral type = '%s'.", tempSP);
    }

    /* Special case: M5+-7: Mira ? replace '+-' by / */
    /* If the spectral type contains "+-", replace by '/' (Mira like M5+-7) */
    tokenPosition = strstr(tempSP, "+-");
    if (IS_NOT_NULL(tokenPosition))
    {
        logDebug("Fix '+-' in spectral type = '%s'.", tempSP);
        *tokenPosition++ = '/';
        *tokenPosition   = ' '; /* removed below */
    }

    /* Special case: K5+III: keep luminosity class (but means K5/6) */
    /* For now, just remove '+' but should set delta quantity to 1 (TBD) */
    tokenPosition = strstr(tempSP, "+I"); /* case sensitive */
    if (IS_NOT_NULL(tokenPosition))
    {
        logDebug("Fix '+I' in spectral type = '%s'.", tempSP);
        *tokenPosition++ = ' '; /* removed below */
        *tokenPosition   = 'I';
        deltaSubType = 1.0;
    }
    tokenPosition = strstr(tempSP, "+V"); /* case sensitive */
    if (IS_NOT_NULL(tokenPosition))
    {
        logDebug("Fix '+V' in spectral type = '%s'.", tempSP);
        *tokenPosition++ = ' '; /* removed below */
        *tokenPosition   = 'V';
        deltaSubType = 1.0;
    }

    /* Remove ':', '(',')' from string */
    miscDeleteChr(tempSP, ':', mcsTRUE);
    miscDeleteChr(tempSP, '(', mcsTRUE);
    miscDeleteChr(tempSP, ')', mcsTRUE);
    /* Remove white spaces */
    miscDeleteChr(tempSP, ' ', mcsTRUE);

    /* Convert to UPPERCASE. */
    miscStrToUpper(tempSP);

    logDebug("Cleaned spectral type = '%s'.", tempSP);

    /* If the spectral type contains a "+" sign, it is a sure sign that the star
     * is a close binary system.
     * Example: HD 47205 (J2000=06:36:41.0-19:15:21) which is K1III(+M) */
    mcsSTRING256 subStrings[4];
    mcsUINT32 nbSubString = 0;

    FAIL_DO(miscSplitString(tempSP, '+', subStrings, 4, &nbSubString),
            errAdd(alxERR_WRONG_SPECTRAL_TYPE_FORMAT, spectralType));

    if (nbSubString > 1)
    {
        strncpy(tempSP, subStrings[0], bufferLength);
        logDebug("Un-doubled spectral type = '%s'.", tempSP);
        decodedSpectralType->isDouble = mcsTRUE;
    }

    /* Remove trailing '.' (... peculiarities that was cropped in catalogs)
     * A1V+... */
    miscTrimString(tempSP, ".");

    /* If the spectral type contains "SB", remove the trailing part and
     * tag it as spectral binary. */
    tokenPosition = strstr(tempSP, "SB");
    if (IS_NOT_NULL(tokenPosition))
    {
        *tokenPosition = '\0'; /* Cut here (no risk after luminosity class) */
        logDebug("Un-SB spectral type = '%s'.", tempSP);
        decodedSpectralType->isSpectralBinary = mcsTRUE;
    }

    /* Index for the following */
    mcsUINT32 index = 0;

    /* If the spectral type contains "CN" or "BA" etc... (Cyanogen, Barium, etc)
     * remove the annoying trailing part and tag it as hasSpectralLines */
    static char* hasSpectralIndicators[] = {"LAM", "FE", "MN", "HG", "CN", "BA", "SI", "SR", "CR", "EU", "MG", "EM", "CA", "HE", "TC", NULL};
    while (IS_NOT_NULL(hasSpectralIndicators[index]))
    {
        /* If the current spectral type is found */
        if (IS_NOT_NULL(tokenPosition = strstr(tempSP, hasSpectralIndicators[index])))
        {
            *tokenPosition = '\0'; /* Cut here (no risk after luminosity class) */
            /* NO Break since the number and order of indicators is variable */
        }
        index++;
    }

    /* Fix peculiarities (PEC, COMP ...)
     * see http://en.wikipedia.org/wiki/Stellar_classification
     * ie remove the annoying trailing part */
    static char* hasPeculiaritiesCut[] = {"PEC", "COMP", "NN", "SHELL", "SH", "WL",
                                          "WA", "WF", "WG", NULL}; /* Remove WFn, WGn and WAn parts like K0WG5, G0WF3/5, F5/7WA9 */
    index = 0;
    while (IS_NOT_NULL(hasPeculiaritiesCut[index]))
    {
        /* If the current peculiarity is found */
        if (IS_NOT_NULL(tokenPosition = strstr(tempSP, hasPeculiaritiesCut[index])))
        {
            *tokenPosition = '\0'; /* Cut here */
            /* NO Break since the number and order of peculiarities is variable */
        }
        index++;
    }

    /* Fix peculiarities that may be present before luminosity class (ER, EP, EQ, EV ...)
     * ie remove the annoying part */
    static char* hasPeculiaritiesTrim[] = {"ER", "EP", "EQ", "EV", NULL};

    index = 0;
    while (IS_NOT_NULL(hasPeculiaritiesTrim[index]))
    {
        /* If the current peculiarity is found */
        if (IS_NOT_NULL(tokenPosition = strstr(tempSP, hasPeculiaritiesTrim[index])))
        {
            /* 2 chars */
            *tokenPosition++ = ' ';
            *tokenPosition++ = ' ';
            /* NO Break since the number and order of peculiarities is variable */
        }
        index++;
    }

    /*If O was wrongly written instead of 0 in normal classes, correct*/
    static char* hasWrongO[] = {"OO", "BO", "AO", "FO", "GO", "KO", "MO", NULL};
    index = 0;
    while (IS_NOT_NULL(hasWrongO[index]))
    {
        tokenPosition = strstr(tempSP, hasWrongO[index]);
        if (IS_NOT_NULL(tokenPosition))
        {
            *++tokenPosition = '0'; /* replace O by 0 */
            break;
        }
        index++;
    }
    /* Hesitates between consecutive classes: get in between */
    static char* PicklesSptypesMashedByVizir[] = {"B57V", "A47IV", "F02IV", "B12III", "K01II", "K34II", NULL};
    index = 0;
    while (IS_NOT_NULL(PicklesSptypesMashedByVizir[index]))
    {
        tokenPosition = strstr(tempSP, PicklesSptypesMashedByVizir[index]); /* This is typical of Vizier SPTYES where the '-' has disappeared */
        if (IS_NOT_NULL(tokenPosition))
        {
            switch (index)
            {
                case 0:
                    sprintf(tempSP, "B5-7V");
                    break;
                case 1:
                    sprintf(tempSP, "A4-7IV");
                    break;
                case 2:
                    sprintf(tempSP, "F0-2IV");
                    break;
                case 3:
                    sprintf(tempSP, "B1-2III");
                    break;
                case 4:
                    sprintf(tempSP, "K0-1II");
                    break;
                case 5:
                    sprintf(tempSP, "K3-4II");
                    break;
            }
            break;
        }
        index++;
    }
    /* Hesitates between consecutive classes: get in between */
    static char* hesitateBetweenClasses[] = {"O/B", "O-B", "B/A", "B-A", "A/F", "A-F", "F/G", "F-G", "G/K", "G-K", "K/M", "K-M", NULL};
    index = 0;
    while (IS_NOT_NULL(hesitateBetweenClasses[index]))
    {
        tokenPosition = strstr(tempSP, hesitateBetweenClasses[index]); /* Say "B/A" is between a B0 and a A9 (more probably between B8 and A2) */
        if (IS_NOT_NULL(tokenPosition))
        { /* make a fake code more explicit, such as B1/A9 to be decoded below, with correct uncertainty */
            mcsSTRING32 morePreciseCode;
            int before = tokenPosition - tempSP;
            if (before > 0) strncpy(morePreciseCode, tempSP, before);
            char* pos = morePreciseCode + before;
            *pos = *tokenPosition;
            *++pos = '1';
            *++pos = '/';
            *++pos = *(tokenPosition + 2);

            if ((*(tokenPosition + 3) != '\0') && isdigit(*(tokenPosition + 3)))
            {
                /* keep potential remaining digit like 'A/F2' ie copy last part */
                strcpy(++pos, (tokenPosition + 3));
            }
            else
            {
                *++pos = '9';
                /* copy last part */
                strcpy(++pos, (tokenPosition + 3));
            }
            strcpy(tempSP, morePreciseCode);
            break;
        }
        index++;
    }

    /*
     * If the spectral type hesitates between two subclasses (A0/3, A0-3),
     * or has a wrong comma, replace by a numerical value.
     */
    char type, type2, separator;
    mcsSTRING32 tempBuffer;
    mcsDOUBLE firstSubType, secondSubType;
    mcsDOUBLE meanSubType;
    static char* possibleLumClasses[] = {"VII", "VI", "V", "IV", "III", "II", "I", NULL};
    separator     = '\0';
    tempBuffer[0] = '\0';
    mcsUINT32 nbOfTokens = sscanf(tempSP, "%c%lf%c%lf%s", &type, &firstSubType, &separator, &secondSubType, tempBuffer);

    if (nbOfTokens >= 4)
    {
        if ((separator == '/') || (separator == '-'))
        {
            meanSubType  = 0.5 *     (firstSubType + secondSubType);
            deltaSubType = 0.5 * fabs(firstSubType - secondSubType);
            sprintf(tempSP, "%c%4.2lf%s", type, meanSubType, tempBuffer);
            logDebug("Un-hesitated spectral type = '%s'.", tempSP);
        }
        else if (separator == ',')
        {
            sprintf(tempSP, "%c%1.0lf%c%1.0lf%s", type, firstSubType, '.', secondSubType, tempBuffer);
            logDebug("Un-comma-ed spectral type = '%s'.", tempSP);
        }
    }
    else if (nbOfTokens == 3 && separator == '-') /* case: K0.5-III*/
    {
        separator     = '\0';
        tempBuffer[0] = '\0';
        nbOfTokens = sscanf(tempSP, "%c%lf%c%s", &type, &firstSubType, &separator, tempBuffer);
        if (strlen(tempBuffer) > 0)
        {
            index = 0;
            while (IS_NOT_NULL(possibleLumClasses[index]))
            {
                tokenPosition = strstr(tempBuffer, possibleLumClasses[index]);
                if (tokenPosition == tempBuffer)
                {
                    sprintf(tempSP, "%c%4.2lf%s", type, firstSubType, tempBuffer);
                    logDebug("Un-dash-ed spectral type = '%s'.\n", tempSP);
                    break;
                }
                index++;
            }
        }
    }

    /* If the spectral type is Xx/Yy... or Xx-Yy, it is another hesitation */
    separator     = '\0';
    tempBuffer[0] = '\0';
    nbOfTokens = sscanf(tempSP, "%c%lf%c%c%lf%s", &type, &firstSubType, &separator, &type2, &secondSubType, tempBuffer);

    mcsLOGICAL found = mcsFALSE;
    if (nbOfTokens == 4)
    {
        secondSubType = 0.0;
        nbOfTokens = sscanf(tempSP, "%c%lf%c%c%s", &type, &firstSubType, &separator, &type2, tempBuffer);
        found = mcsTRUE;
    }
    else if (nbOfTokens >= 5)
    {
        found = mcsTRUE;
    }
    if (IS_TRUE(found) && ((separator == '/') || (separator == '-')))
    {
        mcsDOUBLE c1, c2;
        c1 = alxParseSpectralCode(type);
        c2 = alxParseSpectralCode(type2);

        if (c1 < 0.0)
        {
            /* unsupported code; keep it */
            logWarning("Unsupported code[%c] in spectral type = '%s'.", type, tempSP);
            sprintf(tempSP, "%c%4.2lf%s", type, firstSubType, tempBuffer);
        }
        else if (c2 < 0.0)
        {
            /* unsupported code; keep it */
            logWarning("Unsupported code[%c] in spectral type = '%s'.", type2, tempSP);
            sprintf(tempSP, "%c%4.2lf%s", type2, secondSubType, tempBuffer);
        }
        else
        {
            /* both are supported; interpolate code between classes */
            meanSubType  = 0.5 *     ((c1 + firstSubType) + (c2 + secondSubType));
            deltaSubType = 0.5 * fabs((c1 + firstSubType) - (c2 + secondSubType));
            type = alxConvertSpectralCode(&meanSubType); /* meanSubType = remainder */
            sprintf(tempSP, "%c%4.2lf%s", type, meanSubType, tempBuffer);
            logDebug("Un-hesitate(2) spectral type = '%s'.", tempSP);
        }
    }

    /* If the spectral type is AxM..., it is a peculiar A star which is normally a dwarf */
    separator = '\0';
    nbOfTokens = sscanf(tempSP, "%c%lf%c", &type, &firstSubType, &separator);

    if ((nbOfTokens == 1) && (type == 'A'))
    {
        /* try AM star (A M A5-F2) */
        nbOfTokens = sscanf(tempSP, "AM%c%lf", &type, &firstSubType);
        if (nbOfTokens == 2)
        {
            /* V for Dwarf, to be further interpreted */
            snprintf(tempSP, bufferLength, "%c%4.2lfV", type, firstSubType);
            logDebug("Un-AM spectral type = '%s'.", tempSP);
        }

    }
    else if ((nbOfTokens == 3) && (separator == 'M'))
    {
        /* A1/2mA7-F2 */
        /* V for Dwarf, to be further interpreted */
        snprintf(tempSP, bufferLength, "%c%4.2lfV", type, firstSubType);
        logDebug("Un-M spectral type = '%s'.", tempSP);
    }

    /*
     * LBO: Fix Fm delta DEL group (ie F5 or A5 ?) ie dwarf ??
     * F Stars
     * There are a very few unusually late metallic-line stars called Fm which have G bands but also have line strengths
     * (...) similar to Am stars.
     * There is a larger intermediate group which is called `Fm delta Del' since the characteristics are more similar to Am's than Ap's.
     */
    tokenPosition = strstr(tempSP, "FMDELTADEL");
    if (tokenPosition == tempSP)
    {
        strcpy(tempSP, "FV");
        logDebug("Un-FM spectral type = '%s'.", tempSP);
    }

    /* If the spectral type is sd[OBAFG..]xx, it is a subdwarf of type VI */
    tokenPosition = strstr(tempSP, "SD");
    if (tokenPosition == tempSP)
    {
        separator = '\0';
        nbOfTokens = sscanf(tempSP, "SD%c%c", &type, &separator);
        if (nbOfTokens == 2)
        {
            snprintf(tempSP, sizeof (tempSP), "%c%cVI", type, separator); /* VI for SubDwarf, to be further interpreted */
        }
        else
        {
            /* Skip leading 'SD' */
            *tokenPosition++ = ' ';
            *tokenPosition++ = ' ';
        }
        logDebug("Un-SD spectral type = '%s'.", tempSP);
    }

    /* Remove leading / trailing white spaces */
    miscTrimString(tempSP, " ");

    /* Properly parse cleaned-up spectral type string */
    nbOfTokens = sscanf(tempSP, "%c%lf%s", &decodedSpectralType->code, &decodedSpectralType->quantity, decodedSpectralType->luminosityClass);

    /* If there is no luminosity class in given spectral type, reset it */
    if (nbOfTokens == 2)
    {
        decodedSpectralType->luminosityClass[0] = '\0';
    }
    else if (nbOfTokens == 1) /* meaning there is no numerical value for the spectral type */
    {
        /* try a simple [O-M] spectral type + luminosity class*/
        mcsINT32 nbOfTokens2 = sscanf(tempSP, "%c%s", &(decodedSpectralType->code), decodedSpectralType->luminosityClass);

        /* Null spectral code, go no further */
        FAIL_FALSE_DO(nbOfTokens2,
                      errAdd(alxERR_WRONG_SPECTRAL_TYPE_FORMAT, spectralType));

        /* Spectral Type covers one whole class, artificially put subclass at 5.
         * This is what the CDS java decoder does in fact! */
        if (decodedSpectralType->code == 'M')
        {
            /* try M3 for M... to avoid going into unsupported classes M > 6 */
            decodedSpectralType->quantity = 3;
            deltaSubType = 3.0; /* Use M0 < M3 < M6 */
        }
        else
        {
            decodedSpectralType->quantity = 5.0;
            deltaSubType = 5.0; /* Use X0 < 5 < X10 */
        }
    }
    else
    {
        /* Null spectral code, go no further */
        FAIL_FALSE_DO(nbOfTokens,
                      errAdd(alxERR_WRONG_SPECTRAL_TYPE_FORMAT, spectralType));
    }

    /* Ensure the decodedSpectralType is something we handle well */
    if (alxParseSpectralCode(decodedSpectralType->code) < 0.0)
    {
        /* reset spectral type structure */
        FAIL(alxInitializeSpectralType(decodedSpectralType));
        decodedSpectralType->isInvalid = mcsTRUE;

        FAIL_DO(mcsFAILURE, errAdd(alxERR_WRONG_SPECTRAL_TYPE_FORMAT, spectralType));
    }

    if (strlen(decodedSpectralType->luminosityClass) != 0)
    {
        /* Clean luminosity class from remaining peculiarities */
        strcpy(tempSP, decodedSpectralType->luminosityClass);

        miscDeleteChr(tempSP, 'D', mcsTRUE); /* ?? */
        miscDeleteChr(tempSP, 'E', mcsTRUE); /* Emission lines present */
        miscDeleteChr(tempSP, 'F', mcsTRUE); /* N III and He II emission */
        miscDeleteChr(tempSP, 'G', mcsTRUE); /* ?? */
        miscDeleteChr(tempSP, 'H', mcsTRUE); /* WR stars with emission lines due to hydrogen */
        miscDeleteChr(tempSP, 'K', mcsTRUE); /* Spectra with interstellar absorption features */
        miscDeleteChr(tempSP, 'M', mcsTRUE); /* Enhanced metal features */
        miscDeleteChr(tempSP, 'N', mcsTRUE); /* Broad ("nebulous") absorption due to spinning */
        miscDeleteChr(tempSP, 'P', mcsTRUE); /* Unspecified peculiarity */
        miscDeleteChr(tempSP, 'Q', mcsTRUE); /* Red & blue shifts line present */
        miscDeleteChr(tempSP, 'S', mcsTRUE); /* Narrowly "sharp" absorption lines */
        miscDeleteChr(tempSP, 'W', mcsTRUE); /* Weak lines */
        miscDeleteChr(tempSP, 'C', mcsTRUE); /* C absorption enhanced */
        miscDeleteChr(tempSP, 'N', mcsTRUE); /* N absorption enhanced */
        miscDeleteChr(tempSP, 'T', mcsTRUE); /* ?? */

        /* Remove leading / trailing white spaces */
        miscTrimString(tempSP, " ");

        /* Replace '/' by '-' to have simple matches (I/III => I-III) */
        miscReplaceChrByChr(tempSP, '/', '-');

        strcpy(decodedSpectralType->luminosityClass, tempSP);
    }

    /* Spectral type successfully parsed, define isSet flag to true */
    decodedSpectralType->isSet = mcsTRUE;

    /* Define the quantity uncertainty */
    decodedSpectralType->deltaQuantity = deltaSubType;

    /* Update ourSpType string */
    updateOurSpType(decodedSpectralType);

    /* last consitency test on quantity: never above 10, except M10 which will thus not pass this test. */
    if (decodedSpectralType->quantity > 9.999)
    {
        /* reset spectral type structure */
        FAIL(alxInitializeSpectralType(decodedSpectralType));
        decodedSpectralType->isInvalid = mcsTRUE;

        FAIL_DO(mcsFAILURE, errAdd(alxERR_WRONG_SPECTRAL_TYPE_FORMAT, spectralType));
    }

    logTest("Parsed spectral type='%s' - our spectral type='%s': code='%c' quantity='%.2lf' (%.2lf) luminosity class='%s' "
            "Double='%s' SpectralBinary='%s' Variable='%s'",
            decodedSpectralType->origSpType, decodedSpectralType->ourSpType,
            decodedSpectralType->code,
            decodedSpectralType->quantity, decodedSpectralType->deltaQuantity,
            decodedSpectralType->luminosityClass,
            (IS_TRUE(decodedSpectralType->isDouble) ? "YES" : "NO"),
            (IS_TRUE(decodedSpectralType->isSpectralBinary) ? "YES" : "NO"),
            (IS_TRUE(decodedSpectralType->isVariable) ? "YES" : "NO")
            );

    /* parse luminosity class once */
    alxGetLuminosityClass(decodedSpectralType);

    return mcsSUCCESS;
}

/**
 * Compute *missing* magnitudes in R, I, J, H, K, L and M bands
 * from the spectral type and the V magnitude for the bright
 * star case.
 *
 * It computes magnitudes in R, I, J, H, K, L and M bands according to the
 * spectral type and the magnitudes in B and V bands for a bright star.
 * If magnitude in B or V are unknown, no magnitude will be computed.
 * otherwise, if K band is unknown, the confidence index of
 * computed values is set to LOW, otherwise (K known) it is set to
 * HIGH.
 * If magnitude can not be computed, its associated confidence index is set to
 * NO CONFIDENCE.
 *
 * @param spType spectral type
 * @param magnitudes contains magnitudes in B and V bands, and the computed
 * magnitudes in R, I, J, H, K, L and M bands
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
 */
mcsCOMPL_STAT alxComputeMagnitudesForBrightStar(alxSPECTRAL_TYPE* spectralType,
                                                alxMAGNITUDES magnitudes)
{
    /* If spectral type is unknown, return error. */
    SUCCESS_FALSE_DO(spectralType->isSet, logTest("Spectral type is not set; could not compute missing magnitudes"));

    /* If magnitude B or V are not set, return SUCCESS : the alxMAGNITUDE
     * structure will not be changed -> the magnitude won't be computed */
    SUCCESS_COND_DO(alxIsNotSet(magnitudes[alxB_BAND]) || alxIsNotSet(magnitudes[alxV_BAND]),
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
    lineSup = alxGetLineFromSpectralType(colorTable, spectralType, mcsTRUE);

    /* if line not found, i.e = -1, return mcsSUCCESS */
    SUCCESS_COND_DO(lineSup == alxNOT_FOUND, logTest("Could not compute missing magnitudes"));

    /* If the spectral type matches the line of the color table, take this line */
    if (colorTable->spectralType[lineSup].quantity == spectralType->quantity)
    {
        lineInf = lineSup;
    }
    else
    {
        /* Otherwise interpolate */
        lineInf = lineSup - 1;
    }

    /* Compare B-V star differential magnitude to the one of the color table line; delta should be less than +/- 0.1 */

    /* check blanking values */
    if (alxIsNotSet(colorTable->index[lineSup][alxB_V]) || alxIsNotSet(colorTable->index[lineInf][alxB_V]) ||
            ((fabs((mgB - mgV) - colorTable->index[lineSup][alxB_V].value) > DELTA_THRESHOLD) &&
            (fabs((mgB - mgV) - colorTable->index[lineInf][alxB_V].value) > DELTA_THRESHOLD)))
    {
        logTest("Could not compute differential magnitudes; mgB-mgV=%.3lf / B-V [%.3lf..%.3lf]; delta > 0.1",
                (mgB - mgV), colorTable->index[lineInf][alxB_V].value, colorTable->index[lineSup][alxB_V].value);

        return mcsSUCCESS;
    }

    /* Define the structure of differential magnitudes */
    alxDIFFERENTIAL_MAGNITUDES diffMag;

    /* Perform the interpolation to obtain the best estimate of
           B_V V_I V_R I_J J_H J_K K_L L_M K_M */
    FAIL(alxInterpolateDiffMagnitude(colorTable, lineInf, lineSup, mgB - mgV, alxB_V, diffMag));

    /*
     * Set confidence index for computed values.
     * If magnitude in K band is unknown, set confidence index to LOW, otherise set to MEDIUM. */
    alxCONFIDENCE_INDEX confIndex;
    if (alxIsNotSet(magnitudes[alxK_BAND]))
    {
        confIndex = alxCONFIDENCE_LOW;
    }
    else
    {
        /* Else B, V and K are known */
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
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
 */
mcsCOMPL_STAT alxComputeMagnitudesForFaintStar(alxSPECTRAL_TYPE* spectralType,
                                               alxMAGNITUDES magnitudes)
{
    /* If magnitude J or K are not set, return SUCCESS : the alxMAGNITUDE
     * structure will not be changed -> the magnitude won't be computed */
    SUCCESS_COND_DO(alxIsNotSet(magnitudes[alxJ_BAND]) || alxIsNotSet(magnitudes[alxK_BAND]),
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
    lineSup = alxGetLineFromSpectralType(colorTable, spectralType, mcsTRUE);

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
 * @param Teff1 the first BlackBody temperature
 * @param lambda1 the first BlackBody wavelength (in microns)
 * @param Teff2 the second BlackBody temperature
 * @param lambda2 the second BlackBody wavelength (in microns)
 *
 * @return the flux ratio
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
    static alxAKARI_TABLE akariTable = {mcsFALSE, "alxAkariBlackBodyCorrectionTable.cfg", 0,
                                        {0},
        {
            {0}
        }};

    /* Check if the structure is loaded into memory. If not load it. */
    if (IS_TRUE(akariTable.loaded))
    {
        return &akariTable;
    }

    /* Find the location of the file */
    char* fileName = miscLocateFile(akariTable.fileName);
    if (IS_NULL(fileName))
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

    while (IS_NOT_NULL(pos = miscDynBufGetNextLine(&dynBuf, pos, line, sizeof (line), mcsTRUE)))
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

    while (IS_FALSE(found) && (line < akariTable->nbLines))
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
    /* logTest("%lf < Teff (%lf) < %lf", akariTable->teff[lineInf], Teff, akariTable->teff[lineSup]); */

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

    /* logTest("Ratio = %lf", ratio); */

    /* Compute correction Factor */
    mcsDOUBLE dataSup = akariTable->coeff[lineSup][alx18mu];
    mcsDOUBLE dataInf = akariTable->coeff[lineInf][alx18mu];

    FAIL_COND(alxIsBlankingValue(dataSup) || alxIsBlankingValue(dataInf));

    correctionFactor = dataInf + ratio * (dataSup - dataInf);
    /* logTest("correctionFactor = %lf", correctionFactor); */

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
    /* logTest("%lf < Teff (%lf) < %lf", akariTable->teff[lineInf], Teff, akariTable->teff[lineSup]); */

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

    /* logTest("Ratio = %lf", ratio); */

    /* Compute correction Factor */
    mcsDOUBLE dataSup = akariTable->coeff[lineSup][alx9mu];
    mcsDOUBLE dataInf = akariTable->coeff[lineInf][alx9mu];

    FAIL_COND(alxIsBlankingValue(dataSup) || alxIsBlankingValue(dataInf));

    correctionFactor = dataInf + ratio * (dataSup - dataInf);
    /* logTest("correctionFactor = %lf", correctionFactor); */

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
    static alxTEFFLOGG_TABLE teffloggTable = {mcsFALSE, "alxTableTeffLogg.cfg", 0,
                                              {
            {0}},
        {
            {0}
        },
        {
            {0}
        }};

    /* Check if the structure is loaded into memory. If not load it. */
    if (IS_TRUE(teffloggTable.loaded))
    {
        return &teffloggTable;
    }

    /* Find the location of the file */
    char* fileName = miscLocateFile(teffloggTable.fileName);
    if (IS_NULL(fileName))
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

    while (IS_NOT_NULL(pos = miscDynBufGetNextLine(&dynBuf, pos, line, sizeof (line), mcsTRUE)))
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
                                             &teffloggTable.teff[lineNum][alxTABLE_DWARF],
                                             &teffloggTable.logg[lineNum][alxTABLE_DWARF],
                                             &teffloggTable.teff[lineNum][alxTABLE_GIANT],
                                             &teffloggTable.logg[lineNum][alxTABLE_GIANT],
                                             &teffloggTable.teff[lineNum][alxTABLE_SUPER_GIANT],
                                             &teffloggTable.logg[lineNum][alxTABLE_SUPER_GIANT]);

            /* If parsing went wrong */
            if (nbOfReadTokens != (2 * alxNB_LUM_CLASS_FOR_TEFF + 3))
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

    /* Set the total number of lines in the Teff/Logg table */
    teffloggTable.nbLines = lineNum;

    /* Mark the Teff/Logg table as "loaded" */
    teffloggTable.loaded = mcsTRUE;

    /* Destroy the temporary dynamic buffer used to parse the Teff/Logg table file */
    miscDynBufDestroy(&dynBuf);
    free(fileName);

    /* Return a pointer on the freshly loaded Teff/Logg table */
    return &teffloggTable;
}

static mcsINT32 alxGetLineForTeffLogg(alxTEFFLOGG_TABLE *teffloggTable,
                                      alxSPECTRAL_TYPE *spectralType)
{
    mcsLOGICAL codeFound = mcsFALSE;
    mcsLOGICAL found = mcsFALSE;
    mcsINT32 line = 0;

    while (IS_FALSE(found) && (line < teffloggTable->nbLines))
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
            if (IS_TRUE(codeFound))
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
     * in the Teff/Logg table. The quantity is strictly lower than the first entry
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

    /* If spectral type not found in Teff/Logg table, return error */
    if (IS_FALSE(found))
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
    mcsDOUBLE subClass = spectralType->quantity;
    if (sup != inf)
    {
        ratio = fabs((subClass - inf) / (sup - inf));
    }
    else
    {
        ratio = 0.5;
    }
    /* logTest("Ratio = %lf", ratio); */

    /* Compute Teff */

    /* get the correct Teff/Logg columns from the star type */
    alxTABLE_STAR_TYPE lumClass = alxGetTableStarType(alxGetLuminosityClass(spectralType));

    mcsDOUBLE dataSup = teffloggTable->teff[lineSup][lumClass];
    mcsDOUBLE dataInf = teffloggTable->teff[lineInf][lumClass];

    FAIL_COND(alxIsBlankingValue(dataSup) || alxIsBlankingValue(dataInf));

    *Teff = dataInf + ratio * (dataSup - dataInf);

    dataSup = teffloggTable->logg[lineSup][lumClass];
    dataInf = teffloggTable->logg[lineInf][lumClass];

    /* logg is blank if Teff is blank, no need to check here */
    /* We add the LogG of the Sun = 4.378 to get LogG in cm s^-2 */
    *LogG = dataInf + ratio * (dataSup - dataInf) + 4.378;

    return mcsSUCCESS;
}

mcsDOUBLE getMuChi2(mcsDOUBLE muObs, mcsDOUBLE e_muObs, mcsDOUBLE mu, mcsDOUBLE e_mu)
{
    /* For very high error on dist_plx (~100%), use upper limit on error */

    /* mu_error = (e_distInPc / (0.2 * LOG_10 * distInPc)) => mu_error(100%) = 1. / (0.2 * LOG_10) = 5 / LOG_10 */
    static const mcsDOUBLE MAX_ERROR_SQUARE = (5.0 / LOG_10) * (5.0 / LOG_10);

    mcsDOUBLE squareError = alxMin(MAX_ERROR_SQUARE, alxSquare(e_muObs) + alxSquare(e_mu));

    return alxSquare(muObs - mu) / squareError;
}

/**
 * Compute the extinction coefficient in V band (Av) according to the spectral type
 * and photometric magnitudes using the absolute magnitude table.
 *
 * @param starId star identifier
 * @param dist_plx distance given by parallax (optional = NAN)
 * @param e_dist_plx distance error given by parallax (optional = NAN)
 * @param av extinction coefficient to compute
 * @param e_av error on extinction coefficient to compute
 * @param dist distance to compute
 * @param e_dist error on distance to compute
 * @param chi2 chi2 of the fit
 * @param distChi2 chi2 on distances
 * @param magnitudes observed magnitudes: will compute E(B-V)
 * @param spectralType the decoded spectral type
 * @param minDeltaQuantity minimum value for delta on spectral quantity
 * @param useLumClass true to use spectral type's luminosity class; false to guess it
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
 */
mcsCOMPL_STAT alxComputeAvFromMagnitudes(const char* starId,
                                         mcsDOUBLE  dist_plx,
                                         mcsDOUBLE  e_dist_plx,
                                         mcsDOUBLE* av,
                                         mcsDOUBLE* e_av,
                                         mcsDOUBLE* dist,
                                         mcsDOUBLE* e_dist,
                                         mcsDOUBLE* chi2,
                                         mcsDOUBLE* distChi2,
                                         alxMAGNITUDES magnitudes,
                                         alxSPECTRAL_TYPE* spectralType,
                                         mcsDOUBLE minDeltaQuantity,
                                         mcsLOGICAL useLumClass,
                                         mcsLOGICAL useDeltaQuantity)
{
    /* minimum number of magnitudes required */
    static mcsINT32 MIN_REQUIRED_BANDS = 3;
    /* high magnitude mean error threshold to log a warning */
    static mcsDOUBLE HIGH_MEAN_MAG_ERROR = 0.1;
    /* minimal error on Av ~ 0.1 */
    static mcsDOUBLE MIN_AV_ERROR = 0.1;


    /* check error on magnitudes */
    mcsINT32 nBands, band;
    mcsDOUBLE varMags = 0.0;

    for (nBands = 0, band = alxB_BAND; band < alxL_BAND; band++)
    {
        if (alxIsSet(magnitudes[band]) && (magnitudes[band].confIndex == alxCONFIDENCE_HIGH)
                && (magnitudes[band].error > 0.0) && (!isnan(magnitudes[band].error)))
        {
            varMags += alxSquare(magnitudes[band].error);
            nBands++;
        }
        else
        {
            logDebug("star[%10s]: undefined magnitude %s (or error) !",
                     starId, alxGetBandLabel(band));
        }
    }

    if (nBands == 0)
    {
        goto correctError;
    }

    mcsDOUBLE meanMagError = sqrt(varMags) / nBands;

    if (meanMagError > HIGH_MEAN_MAG_ERROR)
    {
        logTest("High mean error on magnitudes: %.4lf", meanMagError);
    }


    if (IS_FALSE(spectralType->isSet))
    {
        logDebug("star[%10s]: no spectral type !", starId);
        goto correctError;
    }

    /* Get extinction ratio table */
    alxEXTINCTION_RATIO_TABLE *extinctionRatioTable;
    extinctionRatioTable = alxGetExtinctionRatioTable();

    if (extinctionRatioTable == NULL)
    {
        goto correctError;
    }


    /* Find range in star types (dwarf, giant, super giant) */
    const alxSTAR_TYPE mainStarType  = spectralType->starType;
    const alxSTAR_TYPE otherStarType = spectralType->otherStarType;

    alxTABLE_STAR_TYPE starTypeMin, starTypeMax;

    /* check luminosity class */
    if (IS_FALSE(useLumClass))
    {
        logDebug("star[%10s]: no luminosity class for '%10s' !", starId, spectralType->origSpType);

        /* if no luminosity class or discarded, try all star types (different color tables) to guess Av */
        starTypeMin = alxTABLE_DWARF;
        starTypeMax = alxTABLE_SUPER_GIANT;
    }
    else
    {
        const alxTABLE_STAR_TYPE mainTableStarType  = alxGetTableStarType(mainStarType);
        const alxTABLE_STAR_TYPE otherTableStarType = alxGetTableStarType(otherStarType);

        /* Determination of star type according to given star types */
        starTypeMin = alxIntMin(mainTableStarType, otherTableStarType);
        starTypeMax = alxIntMax(mainTableStarType, otherTableStarType);
    }


    /* check number of required magnitudes */
    mcsLOGICAL maskMags[alxL_BAND];
    mcsINT32  maskBands[alxL_BAND];
    mcsINT32 minBand = alxNB_BANDS;

    for (nBands = 0, band = alxB_BAND; band < alxL_BAND; band++)
    {
        maskMags [band] = mcsFALSE;
        maskBands[band] = 0;

        if (alxIsSet(magnitudes[band]) && (magnitudes[band].confIndex == alxCONFIDENCE_HIGH)
                && (magnitudes[band].error > 0.0) && (!isnan(magnitudes[band].error)))
        {
            maskMags[band] = mcsTRUE;
            nBands++;

            /* lower band */
            if (minBand > band)
            {
                minBand = band;
            }
        }
    }

    /* Find common bands when using multiple color tables */
    const mcsDOUBLE deltaQuantity = spectralType->deltaQuantity;

    alxTABLE_STAR_TYPE starType;
    alxCOLOR_TABLE* colorTable;
    mcsINT32 line, i;
    alxDATA* absMagRow;
    mcsLOGICAL maskTableMags[alxL_BAND];
    mcsINT32 nTables = 0, deltaLine;
    mcsDOUBLE step;

    /* iterate on color tables */
    for (starType = starTypeMin; starType <= starTypeMax; starType++)
    {
        /* Get color tables */
        colorTable = alxGetColorTableForTableStarType(starType);

        /* assert: should not happen */
        if (IS_NULL(colorTable))
        {
            goto correctError;
        }

        /* Line corresponding to the spectral type */
        line = alxGetLineFromSpectralType(colorTable, spectralType, mcsFALSE); /* ignore missing */

        /* assert: should not happen due to ignore missing */
        if (line == alxNOT_FOUND)
        {
            logTest("star[%10s]: skip [%s] (no line found) for '%10s' (%.1lf) !",
                    starId, alxGetTableStarTypeLabel(starType), spectralType->origSpType, spectralType->quantity);

            goto correctError;
        }

        line = alxIntMin(colorTable->absMagLineLast, alxIntMax(colorTable->absMagLineFirst, line));

        /* check bands */
        for (nBands = 0, band = alxB_BAND; band < alxL_BAND; band++)
        {
            maskTableMags[band] = mcsFALSE;

            if (IS_TRUE(maskMags[band]))
            {
                if (alxIsNotSet(colorTable->absMag[line][band]))
                {
                    logDebug("star[%10s]: blanking value for line %d in color table [%s] for '%10s' (%.1lf) !",
                             starId, line, alxGetTableStarTypeLabel(starType), spectralType->origSpType, spectralType->quantity);

                    continue;
                }
                /* valid absolute mag */
                maskTableMags[band] = mcsTRUE;
                nBands++;
            }
        }

        /* require at least 3 bands to fit 2 parameters (av, dist). */
        if (nBands < MIN_REQUIRED_BANDS)
        {
            /* not enough useful bands in this color table */
            logDebug("star[%10s]: discard table: not enough absolute magnitudes [%d]; expected [%d] !",
                     starId, nBands, MIN_REQUIRED_BANDS);
        }
        else
        {
            nTables++;

            /* merge bands */
            for (band = alxB_BAND; band < alxL_BAND; band++)
            {
                if (IS_TRUE(maskTableMags[band]))
                {
                    maskBands[band]++;
                }
            }
        }
    } /* iterate on color tables */

    if (nBands < MIN_REQUIRED_BANDS)
    {
        logTest("star[%10s]: not enough valid magnitudes [%d]; expected [%d] !",
                starId, nBands, MIN_REQUIRED_BANDS);

        goto correctError;
    }
    /* Require at least one band <= I (visible ie Av coefficient is meaningfull) */
    if (minBand >= alxJ_BAND)
    {
        logTest("star[%10s]: no visible magnitude only infrared !", starId);

        goto correctError;
    }

    /* Filter mask to find common bands */
    mcsUINT32 bandCounts[alxNB_TABLE_STAR_TYPES];
    for (i = 0; i <= nTables; i++)
    {
        bandCounts[i] = 0;
    }
    for (band = alxB_BAND; band < alxL_BAND; band++)
    {
        if (maskBands[band] != 0)
        {
            logDebug("band[%s]: %d", alxGetBandLabel(band), maskBands[band]);

            for (i = 1; i <= maskBands[band]; i++)
            {
                bandCounts[i]++;
            }
        }
    }

    /* Find best common bands */
    for (nBands = 0, i = nTables; i >= 0; i--)
    {
        nBands = bandCounts[i];
        logDebug("bandCounts[%d]: %d", i, nBands);

        /* require at least 3 bands to fit 2 parameters (av, dist). */
        if (nBands >= MIN_REQUIRED_BANDS)
        {
            /* fix number of tables */
            nTables = i;
            break;
        }
    }

    const mcsINT32 nCommonBands  = nBands;
    const mcsINT32 nCommonTables = nTables;

    logDebug("common Bands: %d / Tables: %d", nCommonBands, nCommonTables);

    if (nBands < MIN_REQUIRED_BANDS)
    {
        goto correctError;
    }

    /* define common band mask (best) */
    for (band = alxB_BAND; band < alxL_BAND; band++)
    {
        maskTableMags[band] = mcsFALSE;

        if (maskBands[band] >= nCommonTables)
        {
            logDebug("band[%s] used", alxGetBandLabel(band));

            maskTableMags[band] = mcsTRUE;
        }
    }


    /* use distance (plx) when available */
    const mcsLOGICAL hasDistPlx = (!isnan(dist_plx) && !isnan(e_dist_plx));
    mcsDOUBLE mu_plx, e_mu_plx;

    if (IS_TRUE(hasDistPlx))
    {
        /* convert distance (from plx in parsec) into distance modulus */
        mu_plx   = getMu(dist_plx);
        e_mu_plx = getMuError(dist_plx, e_dist_plx);

        logDebug("mu_plx: %.4lf (%.5lf)", mu_plx, e_mu_plx);
    }
    else
    {
        mu_plx = e_mu_plx = NAN;
    }

    /* define initial lum class to stay close (middle or unknown) */
    const mcsDOUBLE initialLumClass = (spectralType->otherStarType != alxSTAR_UNDEFINED) ?
            0.5 * (alxParseLumClass(mainStarType) + alxParseLumClass(otherStarType)) : 0.0;

    const mcsDOUBLE *avCoeffs     = extinctionRatioTable->coeff;

    mcsINT32 nAvs = 0;
    mcsDOUBLE Avs [alxNB_TABLE_STAR_TYPES],    e_Avs[alxNB_TABLE_STAR_TYPES];
    mcsDOUBLE dists[alxNB_TABLE_STAR_TYPES],   e_dists[alxNB_TABLE_STAR_TYPES], chis2 [alxNB_TABLE_STAR_TYPES];
    mcsINT32  lineIdx[alxNB_TABLE_STAR_TYPES], lineRef[alxNB_TABLE_STAR_TYPES], nbBands[alxNB_TABLE_STAR_TYPES];
    mcsDOUBLE chis2Mu[alxNB_TABLE_STAR_TYPES];
    alxTABLE_STAR_TYPE starTypes[alxNB_TABLE_STAR_TYPES];

    mcsINT32 lineInf, lineSup, cur, nUsed;
    mcsINT32 j, n;
    alxBAND bands[alxNB_BANDS];

    mcsDOUBLE coeff, absMi;
    mcsDOUBLE vals_AA[alxL_BAND], vals_BB[alxL_BAND], vals_CC[alxL_BAND];
    mcsDOUBLE vals_DD[alxL_BAND], vals_EE[alxL_BAND], vals_RES2[alxL_BAND];
    mcsDOUBLE AA, BB, CC, DD, EE, DEN;
    mcsDOUBLE minChi2, _chi2, avForChi2, distForChi2;
    mcsDOUBLE minChi2Mu = NAN, _chi2Mu = NAN;


    /* repeat Av computation using deltaQuantity (uncertainty on spectral type) and use all possible colors */

    /* iterate on color tables */
    for (starType = starTypeMin; starType <= starTypeMax; starType++)
    {
        /* Get color tables */
        colorTable = alxGetColorTableForTableStarType(starType);

        /* assert: should not happen */
        if (IS_NULL(colorTable))
        {
            goto correctError;
        }

        /* Line corresponding to the spectral type */
        line = alxGetLineFromSpectralType(colorTable, spectralType, mcsFALSE); /* ignore missing */

        /* assert: should not happen due to ignore missing */
        if (line == alxNOT_FOUND)
        {
            logTest("star[%10s]: skip [%s] (no line found) for '%10s' (%.1lf) !",
                    starId, alxGetTableStarTypeLabel(starType), spectralType->origSpType, spectralType->quantity);

            /* if unknown luminosity class, try another class */
            continue;
        }

        step      = colorTable->step;
        /* delta line = number of lines to traverse before / after the current line (uncertainty) */
        /* use given minDeltaQuantity converted in number of lines */
        deltaLine = (IS_TRUE(useDeltaQuantity)) ? (mcsUINT32) ceil(fabs(alxMax(minDeltaQuantity, deltaQuantity) / step)) : 0;

        /* Finds lines corresponding to quantity [+/-] deltaQuantity */
        line    = alxIntMin(colorTable->absMagLineLast, alxIntMax(colorTable->absMagLineFirst, line));
        lineInf = alxIntMax(colorTable->absMagLineFirst, line - deltaLine);
        lineSup = alxIntMin(colorTable->absMagLineLast,  line + deltaLine);


        /* use bands from common bands mask */
        for (nBands = 0, band = alxB_BAND; band < alxL_BAND; band++)
        {
            if (IS_TRUE(maskTableMags[band]))
            {
                /* to be sure: a color table may have not all bands in the mask */
                if (alxIsNotSet(colorTable->absMag[line][band]))
                {
                    logDebug("star[%10s]: blanking value for line %d in color table [%s] for '%10s' (%.1lf) !",
                             starId, line, alxGetTableStarTypeLabel(starType), spectralType->origSpType, spectralType->quantity);
                }
                else
                {
                    bands[nBands++] = band;
                }
            }
        }

        /* requires at least common bands */
        if (nBands >= nCommonBands)
        {
            /* memorize current line and star type*/
            lineRef[nAvs]   = line;
            starTypes[nAvs] = starType;
            nbBands[nAvs]   = nBands;

            /* NOTE: use error correction ie error=sum(var(mag)+var(absMag)) */
            /*
                ERR=SQRT(EMAG_B^2+EMG_COR^2) & AA=TOTAL(1./ERR^2) & BB=TOTAL(CF/ERR^2) & CC=TOTAL(CF^2/ERR^2)
                DEN=AA*CC-BB^2 & DD=(TT-MAG_B(KK,*))/ERR^2 & NUM=TOTAL(DD)*BB-TOTAL(CF*DD)*AA
                DD=(MAG_B(II,*)-TT)/ERR^2 & AV_B(II)=(AA*TOTAL(CF*DD)-BB*TOTAL(DD))/DEN & EAV_B(II)=SQRT(AA/DEN)
                DIST_B(II)=(CC*TOTAL(DD)-BB*TOTAL(CF*DD))/DEN & EDIST_B(II)=SQRT(CC/DEN)
                CH_B(II)=TOTAL((MAG_B(II,*)-CF*AV_B(II)-TT-DIST_B(II))^2/ERR^2)/NMAG
                DIST_B(II)=10D^(0.2D*DIST_B(II)+1D) & EDIST_B(II)=0.2D*LOG_10*DIST_B(II)*EDIST_B(II)
             */

            const mcsUINT32 nbLines = lineSup - lineInf + 1;

            mcsDOUBLE _Avs [nbLines],  _e_Avs[nbLines];
            mcsDOUBLE _dists[nbLines], _e_dists[nbLines], _chis2 [nbLines];
            mcsINT32  _lineIdx[nbLines];

            n = 0;  /* reset */

            for (cur = lineInf; cur <= lineSup; cur++)
            {
                j = 0; /* reset */

                for (i = 0; i < nBands; i++)
                {
                    band = bands[i];

                    /* initialize absMag row */
                    absMagRow = colorTable->absMag[cur];

                    if (alxIsNotSet(absMagRow[band]))
                    {
                        logDebug("star[%10s]: blanking value for line %d in color table [%s] for '%10s' (%.1lf) !",
                                 starId, cur, alxGetTableStarTypeLabel(starType), spectralType->origSpType, spectralType->quantity);
                        continue;
                    }

                    coeff      = avCoeffs[band];
                    absMi      = absMagRow[band].value;      /* TT */

                    /* ERR=SQRT(EMAG_B^2 + EMG_COR^2) */
                    varMags    = alxSquare(magnitudes[band].error) + alxSquare(colorTable->absMagError[cur].value);

                    vals_AA[j] = 1.0 / varMags;              /* (1./ERR^2) */
                    vals_BB[j] = coeff / varMags;            /* (CF/ERR^2) */
                    vals_CC[j] = alxSquare(coeff) / varMags; /* (CF^2/ERR^2) */

                    logDebug("line   : %c%.2lf [%s] - absMi=%.3lf", colorTable->spectralType[cur].code, colorTable->spectralType[cur].quantity,
                             alxGetTableStarTypeLabel(starType), absMi);

                    /* DD=(MAG_B(II,*)-TT)/ERR^2 */
                    vals_DD[j] = (magnitudes[band].value - absMi) / varMags;

                    /* (CF*DD) */
                    vals_EE[j] = coeff * vals_DD[j];

                    /* increment j */
                    j++;
                }

                nUsed = j;

                /* require at least 3 bands to fit 2 parameters (av, dist). */
                if (nUsed >= 3)
                {
                    AA = alxTotal(nUsed, vals_AA); /* AA=TOTAL(1./ERR^2)    = sum ( 1 / varMags) */
                    BB = alxTotal(nUsed, vals_BB); /* BB=TOTAL(CF/ERR^2)    = sum (ci / varMags) */
                    CC = alxTotal(nUsed, vals_CC); /* CC=TOTAL(CF^2/ERR^2)  = sum (ci^2 / varMags) */
                    DD = alxTotal(nUsed, vals_DD); /* DD=TOTAL(DD)          = sum ((Mi - mi) / varMags) */
                    EE = alxTotal(nUsed, vals_EE); /* EE=TOTAL(CF*DD)       = sum (ci x (Mi - mi) / varMags) */
                    DEN = AA * CC - alxSquare(BB); /* DEN=AA*CC-BB^2 */

                    /* avoid divide by 0 */
                    if (DEN != 0.0)
                    {
                        /* AV */
                        _Avs  [n]   = (AA * EE - BB * DD) / DEN;        /* AV_B(II)=(AA*TOTAL(CF*DD)-BB*TOTAL(DD))/DEN */
                        _e_Avs[n]   = sqrt(AA / DEN);                   /* EAV_B(II)=SQRT(AA/DEN) */
                        /* Distance modulus = µ */
                        _dists[n]   = (CC * DD - BB * EE) / DEN;        /* DIST_B(II)=(CC*TOTAL(DD)-BB*TOTAL(CF*DD))/DEN */
                        _e_dists[n] = sqrt(CC / DEN);                   /* EDIST_B(II)=SQRT(CC/DEN) */

                        /* av / distance modulus for chi2 */
                        avForChi2   = _Avs[n];
                        distForChi2 = _dists[n];

                        /* check AV validity */
                        if (_Avs[n] < 0.0)
                        {
                            /* negative Av */
                            logDebug("Fix negative Av=%.4lf (%.5lf)", _Avs[n], _e_Avs[n]);

                            /* increase error up to abs(Av) */
                            _e_Avs[n]   = alxMax(_e_Avs[n], -_Avs[n]);

                            /* Fix Av = 0.0 */
                            _Avs[n] = avForChi2 = 0.0;

                            /* Fix distance given by formula with Av=0 */
                            _dists[n] = distForChi2 = DD / AA;                      /* TOTAL(DD)/AA */
                        }

                        j = 0; /* reset */

                        /* chi2 */
                        for (i = 0; i < nBands; i++)
                        {
                            band = bands[i];

                            /* initialize absMag row */
                            absMagRow = colorTable->absMag[cur];

                            if (alxIsSet(absMagRow[band]))
                            {
                                coeff      = avCoeffs[band];
                                absMi      = absMagRow[band].value;      /* TT */

                                /* ERR=SQRT(EMAG_B^2 + EMG_COR^2) */
                                varMags    = alxSquare(magnitudes[band].error) + alxSquare(colorTable->absMagError[cur].value);

                                /* RES^2 = (MAG_B(II, *) - CF * AV_B(II) - TT - DIST_B(II))^2 / ERR^2 */
                                vals_RES2[j] = alxSquare(magnitudes[band].value - coeff * avForChi2 - absMi - distForChi2) / varMags;

                                /* increment j */
                                j++;
                            }
                        }

                        /* assert j == nUsed */


                        /* use nUsed (= number of used bands) */
                        _chis2[n]    = alxTotal(nUsed, vals_RES2) / nUsed; /* CH_C(II)=TOTAL( RES ) / N_BANDS */

                        /* keep line index to fix spectral type */
                        _lineIdx[n]  = cur;

                        /* Convert distance modulus in parsecs */
                        _dists[n]    = alxPow10(0.2 * _dists[n] + 1.0);   /* DIST_B(II)  = 10D^(0.2D * DIST_B(II) + 1D) */
                        _e_dists[n] *= 0.2 * LOG_10 * _dists[n];          /* EDIST_B(II) = 0.2D * LOG_10 * DIST_B(II) * EDIST_B(II) */

                        logP(LOG_EACH_SPTYPE, "line[%c%.2lf] Av=%.4lf (%.5lf) dist=%.3lf (%.5lf) chi2=%.4lf [%d bands]",
                             colorTable->spectralType[cur].code, colorTable->spectralType[cur].quantity,
                             _Avs[n], _e_Avs[n], _dists[n], _e_dists[n], _chis2[n], nUsed);

                        /* increment n */
                        n++;

                    } /* DEN != 0 */

                } /* nUsed > 2 */

            } /* iterate on lines */

            if (n != 0)
            {
                j = 0; /* index of the best solution in arrays */

                minChi2Mu = NAN;

                if (IS_TRUE(hasDistPlx))
                {
                    minChi2Mu = getMuChi2(mu_plx, e_mu_plx, getMu(_dists[j]), getMuError(_dists[j], _e_dists[j]));
                    logDebug("chi2Mu: %.3lf", minChi2Mu);
                }

                /* there is a solution */
                if (n != 1)
                {
                    minChi2 = _chis2[j];

                    if (IS_TRUE(hasDistPlx))
                    {
                        /* Add the chi2 contribution of the distance modulus */
                        minChi2 += 0.5 * minChi2Mu; /* 1/2 chi2_mu */
                    }
                    else
                    {
                        /* try to minimize distance to lineRef too ie best chi2 but closest from initial spectral type */
                        minChi2 += alxSquare(step * (lineRef[nAvs] - _lineIdx[j]) / 16.); /* (deltaQuantity / 16)^2 */
                    }

                    logDebug("cor minchi2 : %.5lf [%.5lf]", minChi2, _chis2[j]);

                    /* Find minimum chi2 */
                    for (i = 1; i < n; i++)
                    {
                        _chi2 = _chis2[i];

                        if (IS_TRUE(hasDistPlx))
                        {
                            _chi2Mu = getMuChi2(mu_plx, e_mu_plx, getMu(_dists[i]), getMuError(_dists[i], _e_dists[i]));
                            logDebug("chi2Mu: %.3lf", _chi2Mu);

                            /* Add the chi2 contribution of the distance modulus */
                            _chi2 += 0.5 * _chi2Mu;  /* 1/2 chi2_mu */
                        }
                        else
                        {
                            /* try to minimize distance to lineRef too ie best chi2 but closest from initial spectral type */
                            _chi2 += alxSquare(step * (lineRef[nAvs] - _lineIdx[i]) / 16.); /* (deltaQuantity / 16)^2 */
                        }

                        logDebug("cor minchi2 : %.5lf [%.5lf]", _chi2, _chis2[i]);

                        if (_chi2 < minChi2)
                        {
                            j = i;
                            minChi2 = _chi2;

                            if (IS_TRUE(hasDistPlx))
                            {
                                minChi2Mu = _chi2Mu;
                                logDebug("minChi2Mu: %.3lf", minChi2Mu);
                            }
                        }
                    }

                    logDebug("best chi2: %d == %.3lf dist=%.3lf", j, _chis2[j], _dists[j]);
                }

                /* best solution */
                Avs  [nAvs]   =     _Avs[j];
                e_Avs[nAvs]   =   _e_Avs[j];
                dists[nAvs]   =   _dists[j];
                e_dists[nAvs] = _e_dists[j];
                chis2[nAvs]   =   _chis2[j];
                lineIdx[nAvs] = _lineIdx[j];
                chis2Mu[nAvs] = minChi2Mu;

                logTest("Av=%.4lf (%.5lf) distance=%.3lf (%.5lf) chi2=%.4lf chi2Mu=%.4lf [%d bands] [%s] from line[%c%.2lf]",
                        Avs[nAvs], e_Avs[nAvs], dists[nAvs], e_dists[nAvs], chis2[nAvs], chis2Mu[nAvs], nbBands[nAvs], alxGetTableStarTypeLabel(starType),
                        colorTable->spectralType[lineIdx[nAvs]].code, colorTable->spectralType[lineIdx[nAvs]].quantity);

                nAvs++;

            } /* n > 0 */

        } /* nBands > 0 */

    } /* loop on star types */


    /* Process results */
    if (nAvs == 0)
    {
        goto correctError;
    }

    /* there is a solution */
    j = 0; /* index of the best solution in arrays */

    nBands = nbBands[j];

    /* fix table star type */
    starType = starTypes[j];

    if (nAvs != 1)
    {
        minChi2 = chis2[j];

        if (IS_TRUE(hasDistPlx))
        {
            logDebug("chi2Mu: %.3lf", chis2Mu[j]);

            /* Add the chi2 contribution of the distance modulus */
            minChi2 += chis2Mu[j];
        }
        else if (initialLumClass != 0.0)
        {
            /* try to minimize distance to initial lumClass too ie best chi2 but closest from initial spectral type */
            minChi2 += 0.5 * alxSquare(alxParseLumClass(alxGetStarType(starTypes[j])) - initialLumClass);  /* 1/2 (deltaLumClass)^2 */
        }
        logDebug("cor chi2 : %.5lf [%.5lf]", minChi2, chis2[j]);

        /* Find minimum chi2 */
        for (i = 1; i < nAvs; i++)
        {
            /* ensure same number of bands (= maximum) */
            if (nBands == nbBands[i])
            {
                _chi2 = chis2[i];

                if (IS_TRUE(hasDistPlx))
                {
                    _chi2Mu = chis2Mu[i];

                    logDebug("chi2Mu: %.3lf", _chi2Mu);

                    /* Add the chi2 contribution of the distance modulus */
                    _chi2 += _chi2Mu;
                }
                else if (initialLumClass != 0.0)
                {
                    /* try to minimize distance to initial lumClass too ie best chi2 but closest from initial spectral type */
                    _chi2 += 0.5 * alxSquare(alxParseLumClass(alxGetStarType(starTypes[i])) - initialLumClass);  /* 1/2 (deltaLumClass)^2 */
                }
                logDebug("cor chi2 : %.5lf [%.5lf]", _chi2, chis2[i]);

                if (_chi2 < minChi2)
                {
                    j = i;
                    minChi2 = _chi2;

                    if (IS_TRUE(hasDistPlx))
                    {
                        logDebug("minChi2Mu: %.3lf", chis2Mu[i]);
                    }
                }
            }
        }

        /* fix star type */
        starType = starTypes[j];

        logDebug("best chi2: %d == %.3lf [%s]", j, chis2[j], alxGetTableStarTypeLabel(starType));
    }

    /* best solution index */
    mcsUINT32 best = j;

    /* final line index */
    line = lineIdx[best];

    /* Get color table */
    colorTable = alxGetColorTableForTableStarType(starType);


    const alxSTAR_TYPE finalStarType = alxGetStarType(starType);

    if ((starTypeMin != starTypeMax) && (finalStarType != mainStarType) && (finalStarType != otherStarType))
    {
        /* set corrected flag (luminosity class) */
        spectralType->isCorrected = mcsTRUE;

        /* Fix luminosity class in corrected spectral type */
        spectralType->starType = spectralType->otherStarType = finalStarType;

        strcpy(spectralType->luminosityClass, alxGetLumClass(finalStarType));

        alxUpdateOurSpType(spectralType);

        logInfo("spectral type='%s' - our spectral type='%s': updated luminosity class='%s'",
                spectralType->origSpType, spectralType->ourSpType, spectralType->luminosityClass);
    }

    if ((IS_TRUE(useDeltaQuantity))
            && ((spectralType->quantity != colorTable->spectralType[line].quantity)
            || (spectralType->code != colorTable->spectralType[line].code)))
    {
        /* set corrected flag (quantity) */
        spectralType->isCorrected = mcsTRUE;

        /* fix code / quantity from best line index */
        spectralType->code     = colorTable->spectralType[line].code;
        spectralType->quantity = colorTable->spectralType[line].quantity;
        spectralType->deltaQuantity = 0.0; /* no uncertainty anymore */

        alxUpdateOurSpType(spectralType);

        logInfo("spectral type='%s' - our spectral type='%s': updated spectral code / quantity class='%c%.2lf'",
                spectralType->origSpType, spectralType->ourSpType, spectralType->code, spectralType->quantity);
    }

    /* Copy final results */
    *av     = Avs  [best];
    *e_av   = e_Avs[best];
    *dist   = dists[best];
    *e_dist = e_dists[best];
    *chi2   = chis2[best];
    *distChi2 = chis2Mu[best];

    /*
     * ensure minimal error on Av ~ 0.1:
     * IF (EAV_B(II) LT 0.1D) THEN EAV_B(II) = 0.1D
     */
    *e_av = alxMax(*e_av, MIN_AV_ERROR);

    logTest("Fitted Av=%.4lf (%.5lf) distance=%.3lf (%.3lf) chi2=%.4lf chi2Mu=%.4lf [%d bands] [%s]",
            *av, *e_av, *dist, *e_dist, *chi2, *distChi2, nBands, alxGetStarTypeLabel(finalStarType));

    return mcsSUCCESS;

correctError:
    /* reset to NaN */
    *av       = NAN;
    *e_av     = NAN;
    *dist     = NAN;
    *e_dist   = NAN;
    *chi2     = NAN;
    *distChi2 = NAN;

    return mcsFAILURE;
}

/**
 * Initialize this file
 */
void alxMissingMagnitudeInit(void)
{
    alxSPECTRAL_TYPE spectralType;

    /* Initialize the spectral type structure */
    alxInitializeSpectralType(&spectralType);

    /* flag as valid */
    spectralType.isSet = mcsTRUE;

    setLuminosityClass((&spectralType), alxGetLumClass(alxSUPER_GIANT));
    alxGetColorTableForStar(&spectralType);

    setLuminosityClass((&spectralType), alxGetLumClass(alxSUB_SUPER_GIANT));
    alxGetColorTableForStar(&spectralType);

    setLuminosityClass((&spectralType), alxGetLumClass(alxGIANT));
    alxGetColorTableForStar(&spectralType);

    setLuminosityClass((&spectralType), alxGetLumClass(alxSUB_GIANT));
    alxGetColorTableForStar(&spectralType);

    setLuminosityClass((&spectralType), alxGetLumClass(alxDWARF));
    alxGetColorTableForStar(&spectralType);

    /* Debug spectral type decoding */
    /*
        logSetStdoutLogLevel(logDEBUG);
        alxString2SpectralType("A M A5-F0", spectralType);
        alxString2SpectralType("A2/3mA(8)-F0", spectralType);
        alxString2SpectralType("Fm delta DEL", spectralType);
        alxString2SpectralType("B9,5P", spectralType);
        alxString2SpectralType("B9.1,5.661P", spectralType);
        alxString2SpectralType("A0/1", spectralType);
        alxString2SpectralType("A2.5/A8.1", spectralType);
        alxString2SpectralType("B0/B0.5II/I", spectralType);
        alxString2SpectralType("B9.5/A0 III", spectralType);
        alxString2SpectralType("K5.0/M0V", spectralType);
        alxString2SpectralType("G8/K III + A3/5", spectralType);
        alxString2SpectralType("F5-G1I-II", spectralType);
        alxString2SpectralType("F7.5Ib-IIv", spectralType);
        alxString2SpectralType("G2IIIevar", spectralType);
        alxString2SpectralType("G0WF5", spectralType);
        alxString2SpectralType("kA9hA9mF2", spectralType);
        alxString2SpectralType("gG7", spectralType);
        alxString2SpectralType("dF8", spectralType);
        alxString2SpectralType("dK5e", spectralType);
        alxString2SpectralType("G8Ve-K3Ve(T)", spectralType);
        alxString2SpectralType("M2epIa-Iab+B8:eV", spectralType);
        alxString2SpectralType("M0Iab-b", spectralType);
        alxString2SpectralType("M1.5Ia0-Ia", spectralType);
        alxString2SpectralType("M1.5Iab-b", spectralType);
        alxString2SpectralType("F8Ib-II", spectralType);
        alxString2SpectralType("B1Ia/Iab", spectralType);
        alxString2SpectralType("M3Iab/Ib", spectralType);
        alxString2SpectralType("A/F2", &spectralType);
        alxString2SpectralType("O/B2", &spectralType);
        alxString2SpectralType("A/FVpec", &spectralType);
        alxString2SpectralType("K5+Vk:", &spectralType);
        alxString2SpectralType("M5+-6.5e:", &spectralType);
        alxString2SpectralType("M4.5-M5+II", &spectralType);
        alxString2SpectralType("G8+VCN+1", &spectralType);
        alxString2SpectralType("K2+IVe_Fe-1", &spectralType);
     */

    /* bad cases (= failure) */
    /*
        logSetStdoutLogLevel(logDEBUG);

        // III/150A: In the Catalog the temperature types are generally given to a quarter of a subclass (e.g., K0, K0+, K0.5, K1-, K1)
        alxString2SpectralType("K3-III", &spectralType);

        alxString2SpectralType("?", spectralType);
        alxString2SpectralType("C+...", spectralType);
        alxString2SpectralType("C5,4J", spectralType);
        alxString2SpectralType("DA:", spectralType);
        alxString2SpectralType("DA1.5+K0", spectralType);
        alxString2SpectralType("DA2.8", spectralType);
        alxString2SpectralType("DBQA5", spectralType);
        alxString2SpectralType("DQ6", spectralType);
        alxString2SpectralType("Nev", spectralType);
        alxString2SpectralType("Nova", spectralType);
        alxString2SpectralType("R", spectralType);
        alxString2SpectralType("s...", spectralType);
        alxString2SpectralType("S", spectralType);
        alxString2SpectralType("S3/2", spectralType);
        alxString2SpectralType("S4+/1+", spectralType);
        alxString2SpectralType("SC2", spectralType);
        alxString2SpectralType("WC4", spectralType);
        alxString2SpectralType("WC8+O7.5", spectralType);
        alxString2SpectralType("WN4o+O8V", spectralType);
        alxString2SpectralType("WN5", spectralType);
     */

    alxLoadAkariTable();

    alxGetTeffLoggTable();
}

/*___oOo___*/
