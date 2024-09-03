/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * \file
 * brief description of the program, which ends at this dot.
 */


/* 
 * System Headers 
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/* 
 * MCS Headers
 */
#include "log.h"

/*
 * Local Headers 
 */
#include "simcli.h"
#include "simcliPrivate.h"

/* 
 * Main
 */

int main(int argc, char *argv[])
{
    const int N = 20;
    char name[256];
    char ra[256];
    char dec[256];
    mcsDOUBLE pmRa, pmDec;
    mcsDOUBLE plx, ePlx;
    mcsDOUBLE magV, eMagV;
    mcsSTRING64 spType, objTypes, mainId;
    int i;

    /* Get star name */
    memset(name, '\0', 256);
    if (argc >= 2)
    {
        for (i = 1; i < argc; i++)
        {
            strcat(name, argv[i]);
            if (i != (argc - 1))
            {
                strcat(name, " ");
            }
        }
    }
    else
    {
        printf("Usage %s <star name>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    
    for (i = 0; i < N; i++) {
        printf("--- Query [%d / %d] ---\n", i, N);
        
        if (simcliGetCoordinates(name, ra, dec, &pmRa, &pmDec, &plx, &ePlx, &magV, &eMagV, spType, objTypes, mainId) == -1)
        {
            printf("Star '%s' not found in Simbad\n", name);
            exit(EXIT_FAILURE);
        }
        printf("ra       = %s\n", ra);
        printf("dec      = %s\n", dec);
        printf("pmRa     = %lf\n", pmRa);
        printf("pmDec    = %lf\n", pmDec);
        printf("plx      = %lf\n", plx);
        printf("e_plx    = %lf\n", ePlx);
        printf("V        = %lf\n", magV);
        printf("e_V      = %lf\n", eMagV);
        printf("spType   = %s\n", spType);
        printf("objTypes = %s\n", objTypes);
        printf("MainId   = %s\n", mainId);
    }
    /* Exit from the application with SUCCESS */
    exit(EXIT_SUCCESS);
}


/*___oOo___*/
