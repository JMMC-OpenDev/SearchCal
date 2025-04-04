/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * \file
 * Utility showing the list of environments defined in the mcscfgEnvList file,
 * and to verify that there is no duplicated environment, or several
 * environments using same port on the same workstation. 
 *
 * \synopsis
 * \<envEnvShow\>
 *
 * \usedfiles
 * \filename mcscfgEnvList :  MCS environment definition file, located in
 * $MCSDATA
 */


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
#include "env.h"
#include "envPrivate.h"

/* 
 * Main
 */
int main(int argc, char *argv[])
{
    /* Initializes MCS services */
    FAIL_DO(mcsInit(argv[0]), 
        /* Exit from the application with mcsFAILURE */
        errCloseStack(); 
        exit(EXIT_FAILURE);
    );

    envLIST list;
    list.Show();

    // Show errors (if any)
    errDisplayStack();

    // Close MCS services
    mcsExit();
    
    // Exit from the application with SUCCESS
    exit (EXIT_SUCCESS);
}


/*___oOo___*/
