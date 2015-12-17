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

 

/* 
 * Signal catching functions  
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

static void alxGiveIndexInTableOfSpectralCodes(alxSPECTRAL_TYPE spectralType, mcsINT32 *color_table_index, mcsINT32 *color_table_delta, mcsINT32 *lum_class , mcsINT32 *lum_class_delta )
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
    mcsDOUBLE value=-1;
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
    *color_table_index = (int)(4.0*(value+spectralType.quantity));
    *color_table_delta = (int)(4.0*(spectralType.deltaQuantity));

    if (spectralType.otherStarType != alxSTAR_UNDEFINED){
                mcsDOUBLE lcMain  = alxConvertLumClass(spectralType.starType);
                mcsDOUBLE lcOther = (spectralType.otherStarType != spectralType.starType) ?
                        alxConvertLumClass(spectralType.otherStarType) : lcMain;

                *lum_class = (mcsINT32) alxMin(lcMain, lcOther);
                *lum_class_delta = (mcsINT32) fabs(lcMain - lcOther);
    }
}


/* 
 * Main
 */

int main (int argc, char **argv)
{
    
    mcsINT32 sptypIndex=-1, sptypRange=0, lumIndex=-1, lumRange=0;
    /* Configure logging service */
    logSetStdoutLogLevel(logERROR);
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
    
    mcsLOGICAL doTyp=mcsFALSE, doTypRang=mcsFALSE, doLum=mcsFALSE, doLumRang=mcsFALSE;
    while (1)
    {
        c =
        getopt (argc, argv,
                "tTlLh");

        if (c == -1)
        {
            break;
        }
        switch (c)
        {
            case 't':
              doTyp=mcsTRUE;
              break;
            case 'T':
              doTypRang=mcsTRUE;
              break;
            case 'l':
              doLum=mcsTRUE;
              break;
            case 'L':
              doLumRang=mcsTRUE;
              break;
            case 'h':
            case '?':
                printf ("Usage: %s [options]\n","alxDecodeSpectralType");
                printf ("Options:\n");
                printf ("     -t print color_table_index\n");
                printf ("     -T print color_table_delta\n");
                printf ("     -l print lum_class\n");
                printf ("     -L print lum_class_delta\n");
                printf ("     -h: (this help)\n");
                exit(0);
         }                
    }   
    int ntypes = (argc - optind);
    if (optind == 1) {
            doTyp=mcsTRUE, doTypRang=mcsTRUE, doLum=mcsTRUE, doLumRang=mcsTRUE;
    }
    alxSPECTRAL_TYPE decodedSpectralType;
    /* initialize the spectral type structure anyway */
    FAIL(alxInitializeSpectralType(&decodedSpectralType));

    int i;
    for (i=0; i< ntypes; i++)
    {
      sptypIndex=-1; sptypRange=0; lumIndex=-1; lumRange=0;
      if (alxString2SpectralType(argv[optind + i], &decodedSpectralType) == mcsFAILURE)
        {
	  if (doTyp) printf ("-1 ");
          if (doTypRang) printf ("-1 ");
          if (doLum) printf ("-1 ");
          if (doLumRang) printf ("-1 ");
          printf("\n");
        } else {
	alxGiveIndexInTableOfSpectralCodes(decodedSpectralType, &sptypIndex, &sptypRange, &lumIndex, &lumRange );
	  if (doTyp) printf ("%d ",sptypIndex);
          if (doTypRang) printf ("%d ",sptypRange);
          if (doLum) printf ("%d ", lumIndex);
          if (doLumRang) printf ("%d ",lumRange);
          printf("\n");
      }
    }

    /* Close MCS services */
    mcsExit();
    
    /* Exit from the application with SUCCESS */
    exit (EXIT_SUCCESS);

}
