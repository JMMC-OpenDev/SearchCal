/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: vobsTestParser.cpp,v 1.2 2005-02-13 08:54:00 gzins Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * scetre    12-Jul-2004  Created
 *
 ******************************************************************************/

static char *rcsId="@(#) $Id: vobsTestParser.cpp,v 1.2 2005-02-13 08:54:00 gzins Exp $"; 
static void *use_rcsId = ((void)&use_rcsId,(void *) &rcsId);

/* 
 * System Headers 
 */
#include <iostream>
using namespace std;
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
#include "vobsPARSER.h"
#include "vobsPrivate.h"

/*
 * Local Variables
 */

 
/* 
 * Signal catching functions  
 */


/*
 * Local Functions
 */



int main(int argc, char *argv[])
{
    mcsInit(argv[0]);

    logSetStdoutLogLevel(logEXTDBG);

    logInfo("Starting ...");

    vobsSTAR_LIST starList;
    vobsPARSER    parser;
    char          *uri;

    uri = "http://vizier.u-strasbg.fr/viz-bin/asu-xml?-source=I/280";
    if (parser.Parse(uri, "I/280", starList) == FAILURE)
    {
        errDisplayStack();
        errCloseStack();
        exit(EXIT_FAILURE);
    }

    logInfo("Size of the list = %d", starList.Size());

    logInfo("Exiting ...");
    exit(EXIT_SUCCESS);

}


/*___oOo___*/
