/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * \file
 * Test program of the function which computes magnitudes.
 */


/*
 * System Headers
 */
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <getopt.h>
#include <string.h>


/*
 * MCS Headers
 */
#include "mcs.h"
#include "log.h"
#include "err.h"


/*
 * Local Headers
 */
#include "alx.h"
#include "alxPrivate.h"

/*
 * Local Variables
 */

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

static void alxGiveMyIndexInTableOfSpectralCodes(alxSPECTRAL_TYPE spectralType, mcsINT32 *color_table_index, mcsINT32 *color_table_delta, mcsINT32 *lum_class , mcsINT32 *lum_class_delta )
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
    mcsDOUBLE value = -1;
    switch (spectralType.code)
    {
        case 'O':
            value = 0.0 ;
            break;
        case 'B':
            value = 10.0;
            break;
        case 'A':
            value = 20.0;
            break;
        case 'F':
            value = 30.0;
            break;
        case 'G':
            value = 40.0;
            break;
        case 'K':
            value = 50.0;
            break;
        case 'M':
            value = 60.0;
            break;
        default:
            /* unsupported code */
            *color_table_index = -1;
            return;
    }
    *color_table_index = (int) (4.0 * (value + spectralType.quantity));
    *color_table_delta = (int) (4.0 * (spectralType.deltaQuantity));

    if (spectralType.otherStarType != alxSTAR_UNDEFINED)
    {
        mcsDOUBLE lcMain  = alxConvertLumClass(spectralType.starType);
        mcsDOUBLE lcOther = (spectralType.otherStarType != spectralType.starType) ?
                alxConvertLumClass(spectralType.otherStarType) : lcMain;

        *lum_class = (mcsINT32) alxMin(lcMain, lcOther);
        *lum_class_delta = (mcsINT32) fabs(lcMain - lcOther);
    }
}

static const char* alxGetMyLumClass(mcsINT32 classnum)
{
    switch (classnum)
    {
        case 1:
            return "I";
        case 2:
            return "II";
        case 3:
            return "III";
        case 4:
            return "IV";
        case 5:
            return "V";
        default:
            return "?";
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
/*
 * Main
 */

int main (int argc, char **argv)
{
    mcsINT32 sptypIndex = -1, sptypRange = 0, lumIndex = -1, lumRange = 0;
    /* Configure logging service */
    logSetStdoutLogLevel(logQUIET);
    logSetPrintDate(mcsFALSE);
    logSetPrintFileLine(mcsFALSE);

    /* /\* Initializes MCS services *\/ */
    if (mcsInit(argv[0]) == mcsFAILURE)
    {
        /* Error handling if necessary */

        /* Exit from the application with mcsFAILURE */
        exit (EXIT_FAILURE);
    }
    extern char *optarg;
    extern int optind;
    optind = 1;
    int c;
    char* ifileName = NULL;
    char* ofileName = NULL;
    int ifileInput = 0;
    int ofileInput = 0;

    mcsLOGICAL doTyp = mcsFALSE, doTypRang = mcsFALSE, doLum = mcsFALSE, doLumRang = mcsFALSE, doSP=mcsFALSE;
    while (1)
    {
        c =
                getopt (argc, argv,
                        "vtTSlLhi:o:");

        if (c == -1)
        {
            break;
        }
        switch (c)
        {
            case 'v':
            {
                logSetStdoutLogLevel(logERROR);
            }
                break;
            case 't':
                doTyp = mcsTRUE;
                break;
            case 'T':
                doTypRang = mcsTRUE;
                break;
            case 'l':
                doLum = mcsTRUE;
                break;
            case 'L':
                doLumRang = mcsTRUE;
                break;
            case 'S':
                doSP = mcsTRUE;
                break;
            case 'i':
            {
                int l;
                l = strlen((char*) optarg);
                if (l > 0) ifileName = (char*) calloc(l + 1, sizeof (char));
                if (sscanf (optarg, "%s", ifileName) == 0)
                {
                    exit(1);
                }
                ifileInput = 1;
            }
                break;
            case 'o':
            {
                int l;
                l = strlen((char*) optarg);
                if (l > 0) ofileName = (char*) calloc(l + 1, sizeof (char));
                if (sscanf (optarg, "%s", ofileName) == 0)
                {
                    exit(1);
                }
                ofileInput = 1;
            }
                break;
            case 'h':
            case '?':
                printf ("Usage: %s [options]\n", "alxDecodeSpectralType");
                printf ("Options:\n");
                printf ("     -v verbose (icrease each time invoked)\n");
                printf ("     -t print color_table_index\n");
                printf ("     -T print color_table_delta\n");
                printf ("     -l print lum_class\n");
                printf ("     -L print lum_class_delta\n");
                printf ("     -S print interpreted spectral type\n");
                printf ("     -i inputfile (must be STILTS ascii format)\n");
                printf ("     -o outputfile (default:stdout)\n");
                printf ("        NOTE: format is STILTS TST\n");
                printf ("     -h: (this help)\n");
                exit(0);
        }
    }
    int ntypes = (argc - optind);
    if (optind == 1 || ifileInput)
    {
        doTyp = mcsTRUE, doTypRang = mcsTRUE, doLum = mcsTRUE, doLumRang = mcsTRUE, doSP=mcsTRUE;
    }
    alxSPECTRAL_TYPE decodedSpectralType;
    /* initialize the spectral type structure anyway */
    FAIL(alxInitializeSpectralType(&decodedSpectralType));

    if (ofileInput && !ifileInput)
    {
        printf("No input file given, aborting.\n");
        exit(1);
    }

    FILE* of = stdout;

    if (ifileInput)
    {
        if (ofileInput)
        {
            of = fopen(ofileName, "w");
            if (of == NULL)
            {
                printf("unable to open output file %s aborting.\n", ofileName);
                exit(1);
            }
            fprintf(of, "# alxDecodeSpectralType STS file\n# Start of parameter definitions.\nid_col:0\nra_col:0\ndec_col:0\n#End of parameter definitions.\n");
            if (doTyp) fprintf(of, "color_table_index\t");
            if (doTypRang) fprintf(of, "color_table_delta\t");
            if (doLum) fprintf(of, "lum_class\t");
            if (doLumRang) fprintf(of, "lum_class_delta\t");
            if (doSP) fprintf(of, "SpType_JMMC");
            fprintf(of, "\n");
            if (doTyp) fprintf(of, "---\t");
            if (doTypRang) fprintf(of, "---\t");
            if (doLum) fprintf(of, "---\t");
            if (doLumRang) fprintf(of, "---\t");
            if (doSP) fprintf(of, "---");
            fprintf(of, "\n");
        }
        /* Load file (skipping comment lines starting with '#') */
        miscDYN_BUF dynBuf;
        miscDynBufInit(&dynBuf);

        logInfo("Loading %s ...", ifileName);

        FAIL_DO(miscDynBufLoadFile(&dynBuf, ifileName, "#"),
                miscDynBufDestroy(&dynBuf);
                free(ifileName));

        /* For each line of the loaded file */
        const char *pos = NULL;
        mcsSTRING1024 line;

        /* note: miscDynBufLoadFile skips first ascii comment line */
        while (IS_NOT_NULL(pos = miscDynBufGetNextLine(&dynBuf, pos, line, sizeof (line), mcsTRUE)))
        {
            logTrace("miscDynBufGetNextLine() = '%s'", line);

            /* Trim line for any leading and trailing blank characters */
            miscTrimString(line, " ");
            /* Trim line for any leading and trailing '"' (strings with blanks in topcat's ascii files) */
            miscTrimString(line, "\"");
            sptypIndex = -1;
            sptypRange = 0;
            lumIndex = -1;
            lumRange = 0;
            if (alxString2SpectralType(line, &decodedSpectralType) == mcsFAILURE)
            {
                if (doTyp) fprintf(of, "-1\t");
                if (doTypRang) fprintf(of, "-1\t");
                if (doLum) fprintf(of, "-1\t");
                if (doLumRang) fprintf(of, "-1\t");
                if (doSP) fprintf(of, "?");
                fprintf(of, "\n");
            }
            else
            {
                alxGiveMyIndexInTableOfSpectralCodes(decodedSpectralType, &sptypIndex, &sptypRange, &lumIndex, &lumRange);
                if (doTyp) fprintf(of, "%d\t", sptypIndex);
                if (doTypRang) fprintf(of, "%d\t", sptypRange);
                if (doLum) fprintf(of, "%d\t", lumIndex);
                if (doLumRang) fprintf(of, "%d\t", lumRange);
                if (doSP) {
                  mcsDOUBLE x=sptypIndex/4.;
                  char code=alxConvertSpectralCode(&x);
                  fprintf(of, "%c%4.2lf%s", code, x, alxGetMyLumClass(lumIndex));
                }
                fprintf(of, "\n");
            }

        }
        /* EOF will produce a spurious line which will be removed when merging : TO BE PATCHED!*/
        exit(0);

        int i;
        for (i = 0; i < ntypes; i++)
        {
            sptypIndex = -1;
            sptypRange = 0;
            lumIndex = -1;
            lumRange = 0;
            if (alxString2SpectralType(argv[optind + i], &decodedSpectralType) == mcsFAILURE)
            {
                if (doTyp) printf ("-1 ");
                if (doTypRang) printf ("-1 ");
                if (doLum) printf ("-1 ");
                if (doLumRang) printf ("-1 ");
                if (doSP) printf ("?");
                printf("\n");
            }
            else
            {
                alxGiveMyIndexInTableOfSpectralCodes(decodedSpectralType, &sptypIndex, &sptypRange, &lumIndex, &lumRange );
                if (doTyp) printf ("%d ", sptypIndex);
                if (doTypRang) printf ("%d ", sptypRange);
                if (doLum) printf ("%d ", lumIndex);
                if (doLumRang) printf ("%d ", lumRange);
                if (doSP) {
                  mcsDOUBLE x=sptypIndex/4.;
                  char code=alxConvertSpectralCode(&x);
                  printf("%c%4.2lf%s", code, x, alxGetMyLumClass(lumIndex));
                }
                printf("\n");
            }
        }
    }
    else
    {
        int i;
        for (i = 0; i < ntypes; i++)
        {
            sptypIndex = -1;
            sptypRange = 0;
            lumIndex = -1;
            lumRange = 0;
            if (alxString2SpectralType(argv[optind + i], &decodedSpectralType) == mcsFAILURE)
            {
                if (doTyp) printf ("-1 ");
                if (doTypRang) printf ("-1 ");
                if (doLum) printf ("-1 ");
                if (doLumRang) printf ("-1 ");
                if (doSP) printf ("?");
                printf("\n");
            }
            else
            {
                alxGiveMyIndexInTableOfSpectralCodes(decodedSpectralType, &sptypIndex, &sptypRange, &lumIndex, &lumRange );
                if (doTyp) printf ("%d ", sptypIndex);
                if (doTypRang) printf ("%d ", sptypRange);
                if (doLum) printf ("%d ", lumIndex);
                if (doLumRang) printf ("%d ", lumRange);
                if (doSP) {
                  mcsDOUBLE x=sptypIndex/4.;
                  char code=alxConvertSpectralCode(&x);
                  printf("%c%4.2lf%s", code, x, alxGetMyLumClass(lumIndex));
                }
                printf("\n");
            }
        }
    }

    /* Close MCS services */
    mcsExit();

    /* Exit from the application with SUCCESS */
    exit (EXIT_SUCCESS);

}
