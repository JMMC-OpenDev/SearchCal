
/*******************************************************************************
* JMMC project
*
* "@(#) $Id: getInformation.C,v 1.1 2004-09-07 11:57:33 scetre Exp $"
*
* who       when         what
* --------  -----------  -------------------------------------------------------
* scetre    25-Aug-2004  Created
*
*
*******************************************************************************/

static char *rcsId="@(#) $Id: getInformation.C,v 1.1 2004-09-07 11:57:33 scetre Exp $"; 
static void *use_rcsId = ((void)&use_rcsId,(void *) &rcsId);


/* 
 * System Headers 
 */
#include <stdlib.h>
#include <iostream>

/**
 * \namespace std
 * Export standard iostream objects (cin, cout,...).
 */
using namespace std;


/*
 * MCS Headers 
 */
#include "mcs.h"
#include "log.h"
#include "err.h"
#include "misc.h"


/*
 * Local Headers 
 */
#include "vobsSTAR.h"
#include "vobsSTAR_LIST.h"
#include "vobsREQUEST.h"
#include "vobsVIRTUAL_OBSERVATORY.h"
#include "vobsPrivate.h"

/* 
 * Main
 */

int main(int argc, char *argv[])
{
        // Initialize MCS services
    if (mcsInit(argv[0]) == FAILURE)
    {
        // Error handling if necessary
        
        // Exit from the application with FAILURE
        exit (EXIT_FAILURE);
    }
    miscDYN_BUF starName;
    if (argv[1]!=NULL)
    {
        miscDynBufAppendString(&starName, argv[1]);
        int i=2;
        while (argv[i]!=NULL)
        {
            miscDynBufAppendString(&starName, "%20");
            miscDynBufAppendString(&starName, argv[i]);
            i++;
        }
        //printf("%s\n",miscDynBufGetBufferPointer(&starName));
    }
    else 
    {
        printf("NOT VALID NAME\n");
    }

    logSetStdoutLogLevel(logEXTDBG);
    
    logInfo("Starting ...");
    vobsREQUEST request;

    // Affect the kind of request
    if (request.SetKindOfRequest(0) == FAILURE)
    {
        errDisplayStack();
        errCloseStack();
        exit(EXIT_FAILURE); 
    }
    
    if (request.SetConstraint(STAR_NAME_ID,miscDynBufGetBufferPointer(&starName)) == FAILURE)
    {
        errDisplayStack();
        errCloseStack();
        exit(EXIT_FAILURE);
    }

    vobsSTAR_LIST starList;

    vobsVIRTUAL_OBSERVATORY vobs;

    if (vobs.Research(request, starList)==FAILURE)
    {
        //errDisplayStack();
        //errCloseStack();
        exit(EXIT_FAILURE);
    }
    
    //errDisplayStack();
    
    logInfo("Exiting ...");
    exit(EXIT_SUCCESS);

}


/*___oOo___*/
