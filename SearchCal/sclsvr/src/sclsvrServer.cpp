/*******************************************************************************
* JMMC project
*
* "@(#) $Id: sclsvrServer.cpp,v 1.2 2004-12-06 14:31:08 scetre Exp $"
*
* who       when         what
* --------  -----------  -------------------------------------------------------
* gzins     23-Nov-2004  Created
*
*
*******************************************************************************/

/**
 * \file
 * Search Calibrators SERVER
 */

static char *rcsId="@(#) $Id: sclsvrServer.cpp,v 1.2 2004-12-06 14:31:08 scetre Exp $"; 
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


/*
 * Local Headers 
 */
#include "sclsvrSERVER.h"
#include "sclsvrPrivate.h"

/* 
 * Main
 */

int main(int argc, char *argv[])
{
    sclsvrSERVER scalibServer;

    // Init server
    if (scalibServer.Init(argc, argv) == FAILURE)
    {
        exit (EXIT_FAILURE);
    }

    // Main loop
    if (scalibServer.MainLoop() == FAILURE)
    {
        errDisplayStack();
    }

    // Exit from the application with SUCCESS
    exit (EXIT_SUCCESS);
}


/*___oOo___*/
