/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: alxTestAngularDiameter.c,v 1.6 2005-03-30 12:48:46 scetre Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.5  2005/02/22 16:21:26  gzins
 * Updated for new alxComputeAngularDiameter API
 *
 * Revision 1.4  2005/02/17 19:05:23  gzins
 * Changed remaining FAILURE to mcsFAILURE
 *
 * Revision 1.3  2005/02/12 15:18:56  gzins
 * Set logging service for test; do not print time stamp and file/line information and set level to logTEST
 *
 * Revision 1.2  2005/02/10 08:15:14  gzins
 * Updated for new alxComputeAngularDiameter API
 *
 * Revision 1.1  2005/01/21 08:14:25  gluck
 * Creation
 *
 * 
 * scetre    11-Oct-2004  Created
 *
 ******************************************************************************/

/**
 * \file
 * Test program of the function which computes the angular angle of the star. 
 */

static char *rcsId="@(#) $Id: alxTestAngularDiameter.c,v 1.6 2005-03-30 12:48:46 scetre Exp $"; 
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
#include "alx.h"
#include "alxPrivate.h"


/* 
 * Main
 */
int main (int argc, char *argv[])
{
    /* Configure logging service */
    logSetStdoutLogLevel(logTEST);
    logSetPrintDate(mcsFALSE);
    logSetPrintFileLine(mcsFALSE);

    /* Initializes MCS services */
    if (mcsInit(argv[0]) == mcsFAILURE)
    {
        /* Error handling if necessary */
        
        /* Exit from the application with mcsFAILURE */
        exit (EXIT_FAILURE);
    }

    mcsFLOAT mgB=11.190 ;
    mcsFLOAT mgV=10.079;
    mcsFLOAT mgR=7.530;
    mcsFLOAT mgK=3.092;
    alxDIAMETERS diameters;
    if (alxComputeAngularDiameter(mgB, mgV, mgR, mgK, &diameters)== mcsFAILURE)
    {
        return mcsFAILURE;
    }

    /* Close MCS services */
    mcsExit();
    
    /* Exit from the application with SUCCESS */
    exit (EXIT_SUCCESS);
}


/*___oOo___*/
