/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * @file
 * Get information on MCS environment 
 *
 * @synopsis
 * \<envGet\> [-h] \<name\>
 *
 * @param name : name of MCS environment 
 * 
 * @opt
 * @optname -h : print command usage 
 *
 * @details
 * This command shows information (host name and port number) of the given
 * environment.
 * 
 * @return 0 if environment exist; and 1 otherwise .
 * 
 */

static char *rcsId __attribute__ ((unused)) = "@(#) $Id: envGet.cpp,v 1.1 2006-03-31 13:20:16 gzins Exp $"; 

/* 
 * System Headers 
 */
#include <stdlib.h>
#include <iostream>
using namespace std;

/*
 * MCS Headers 
 */
#include "mcs.h"
#include "log.h"
#include "err.h"

/*
 * Local Headers 
 */
#include "env.h"
#include "envPrivate.h"

/*
 * Local function
 */
void envPrintUsage(void)
{
    printf("Usage: envGet [-h] [<envName>]\n");
}
/* 
 * Main
 */
int main (int argc, char *argv[])
{
    /* Initializes MCS services */
    if (mcsInit(argv[0]) == mcsFAILURE)
    {
        /* Exit from the application with FAILURE */
        exit (EXIT_FAILURE);
    }

    /*
     * Check parameter list: bad number of argument/option.
     */ 
    if (argc != 2) 
    {
        /* Exit from the application with FAILURE */
        printf("Bad number of argument(s)/option(s)\n");
        envPrintUsage();
        exit (EXIT_FAILURE);
    }
   
    /*
     * Check parameter list: wrong option.
     */ 
    if ((argv[1][0] == '-') && (strcmp(argv[1], "-h") != 0))
    {
        /* Exit from the application with FAILURE */
        printf("Bad option '%s'\n", argv[1]);
        envPrintUsage();
        exit (EXIT_FAILURE);
    }

    /*
     * Print usage.
     */
    if (strcmp(argv[1], "-h") == 0)
    {
        envPrintUsage();
        exit (EXIT_SUCCESS);
    }
    
    /*
     * Get environment informations
     */
    const char *hostname;
    envLIST list;
    hostname = list.GetHostName(argv[1]);
    if (hostname == NULL)
    {
        errCloseStack();
        exit (EXIT_FAILURE);
    }
    mcsINT32   portNumber;
    portNumber = list.GetPortNumber(argv[1]);
    if (portNumber == -1)
    {
        errCloseStack();
        exit (EXIT_FAILURE);
    }

    /*
     * Print-out environment informations
     */
    printf("%s %s %d\n", argv[1], hostname, portNumber);

    /* Close MCS services */
    mcsExit();
    
    /* Exit from the application with SUCCESS */
    exit (EXIT_SUCCESS);
}


/*___oOo___*/
