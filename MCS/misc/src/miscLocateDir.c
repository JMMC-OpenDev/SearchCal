/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * \file
 * Locate a directory amongst the parent directory, INTROOT and MCSROOT.
 *
 * \synopsis
 * miscLocateDir \<directoryName\> [\<pathList\>]
 *
 * \param directoryName name of the searched directory
 * \param pathList list of path where the file has to be searched
 *
 * \n
 * \details
 * This program searches for the specified directory name in the given path
 * list. If no path list is given, it begins in the local directory, an then
 * searches in the standard MCS directories : INTROOT first, MCSROOT at last.
 * The possible first occurrence of the directory is returned.
 * 
 * \n
 * \ex
 * \n Search for the directory named 'misc' in the ../include or /usr/include 
 * \code
 * miscLocateDir misc ../include:/usr/include
 * \endcode
 *
 * \n Search for the file directory 'misc' in the standard MCS directories
 * \code
 * miscLocateDir misc
 * \endcode
 */

static char *rcsId __attribute__ ((unused)) = "@(#) $Id: miscLocateDir.c,v 1.2 2006-01-10 14:40:39 mella Exp $"; 


/* 
 * System Headers 
 */
#include <stdlib.h>
#include <stdio.h>

/*
 * MCS Headers 
 */
#include "mcs.h"
#include "log.h"
#include "err.h"

/*
 * Local Headers 
 */
#include "misc.h"
#include "miscPrivate.h"


int main (int argc, char *argv[])
{
    char *fullDirName;

    /* Initializes MCS services */
    if (mcsInit(argv[0]) == mcsFAILURE)
    {
        /* Exit from the application with mcsFAILURE */
        errCloseStack();
        exit(EXIT_FAILURE);
    }

    /* Test number of arguments */
    if ((argc != 2) && (argc != 3))
    {
        printf("Usage:\n");
        printf("   %s <directoryName> [<pathList>]\n", mcsGetProcName());
        exit(EXIT_FAILURE);
    }

    /* Search for the specified directory */
    if (argc == 2)
    {
        fullDirName = miscLocateDir(argv[1]);
    }
    else
    {
        fullDirName = miscLocateFileInPath(argv[2], argv[1]);
    }

    /* Print result */
    if (fullDirName != NULL)
    {
        printf("%s\n", fullDirName);
    }
    else
    {
        errCloseStack();
        exit(EXIT_FAILURE);
    }

    /* Close MCS services */
    mcsExit();
    
    /* Exit from the application with mcsSUCCESS or mcsFAILURE */
    exit(EXIT_SUCCESS);
}

/*___oOo___*/
