/*******************************************************************************
* JMMC project
*
* "@(#) $Id: miscLocateFile.c,v 1.4 2004-12-14 03:19:20 mella Exp $"
*
* who       when         what
* --------  -----------  -------------------------------------------------------
* gzins     09-Nov-2004  Created
* gzins     23-Nov-2004  Returned a 'resolved' path; i.e. where environment
*                        variables have substituated
*
*
*******************************************************************************/

/**
 * \file
 * Locate file in the path list.
 *
 * \synopsis
 * miscLocateFile \<fileName\> [\<pathList\>]
 *
 * \param fileName : name of the searched file  
 * \param pathList : list of path where the file has to be searched 
 *
 * \n
 * \details
 * This program searches for the specified filename in the given path list. If
 * no path list is given, it serch in the standard MCS directories, according
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

static char *rcsId="@(#) $Id: miscLocateFile.c,v 1.4 2004-12-14 03:19:20 mella Exp $"; 
static void *use_rcsId = ((void)&use_rcsId,(void *) &rcsId);

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
    if (mcsInit(argv[0]) == FAILURE)
    {
        /* Exit from the application with FAILURE */
        exit (EXIT_FAILURE);
    }

    /* Test number of arguments */
    if ((argc != 2) && (argc != 3))
    {
        printf ("Usage:\n");
        printf ("   %s <fileName> [<pathList>]\n", mcsGetProcName());
        exit (EXIT_FAILURE);
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
        printf("%s\n", miscResolvePath(fullFileName));
    }

    /* Close MCS services */
    mcsExit();
    
    /* Exit from the application with SUCCESS or FAILURE */
    if (fullFileName != NULL)
    {
        exit (EXIT_SUCCESS);
    }
    else
    {
        exit (EXIT_FAILURE);
    }

}


/*___oOo___*/
