/*******************************************************************************
* JMMC project
*
* "@(#) $Id: vobsTestParser.C,v 1.3 2004-10-19 14:48:18 scetre Exp $"
*
* who       when         what
* --------  -----------  -------------------------------------------------------
* scetre    12-Jul-2004  Created
*
*
*******************************************************************************/

static char *rcsId="@(#) $Id: vobsTestParser.C,v 1.3 2004-10-19 14:48:18 scetre Exp $"; 
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
    if (parser.Parse(uri, starList) == FAILURE)
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
