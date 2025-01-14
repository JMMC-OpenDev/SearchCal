/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * \file
 * Locate file in the path list.
 *
 * \synopsis
 * miscLocateFile \<fileName\> [\<pathList\>]
 *
 * \param fileName name of the searched file  
 * \param pathList list of path where the file has to be searched 
 *
 * \n
 * \details
 * This program searches for the specified filename in the given path list. If
 * no path list is given, it searches in the standard MCS directories, according
 * to the extension of the given file. The possible first occurrence of the
 * file is returned.
 * 
 * \n
 * \ex
 * \n Search for the file named 'string.h' in the ../include or /usr/include 
 * \code
 * miscLocateFile string.h ../include:/usr/include
 * \endcode
 *
 * \n Search for the file named 'myServer.cfg' in the standard MCS directories
 * \code
 * miscLocateFile myServer.cfg
 * \endcode
 */



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
    char *fullFileName;

    /* Initializes MCS services */
    FAIL_DO(mcsInit(argv[0]), 
        /* Exit from the application with mcsFAILURE */
        errCloseStack(); 
        exit(EXIT_FAILURE);
    );

    /* Test number of arguments */
    if ((argc != 2) && (argc != 3))
    {
        printf("Usage:\n");
        printf("   %s <fileName> [<pathList>]\n", mcsGetProcName());
        exit(EXIT_FAILURE);
    }

    /* Search for the specified file */
    if (argc == 2)
    {
        fullFileName = miscLocateFile(argv[1]);
    }
    else
    {
        fullFileName = miscLocateFileInPath(argv[2], argv[1]);
    }

    /* Print result */
    if (fullFileName != NULL)
    {
        printf("%s\n", fullFileName);
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
