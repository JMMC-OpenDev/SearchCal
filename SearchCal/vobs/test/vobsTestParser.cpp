/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: vobsTestParser.cpp,v 1.3 2005-02-13 16:01:08 gzins Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.2  2005/02/13 08:54:00  gzins
 * Updated after vobs classes changes
 * Added CVS log as modifification history
 *
 * scetre    12-Jul-2004  Created
 *
 ******************************************************************************/

static char *rcsId="@(#) $Id: vobsTestParser.cpp,v 1.3 2005-02-13 16:01:08 gzins Exp $"; 
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

    logSetStdoutLogLevel(logDEBUG);

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
