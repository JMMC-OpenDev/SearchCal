/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: vobsTestFilter.cpp,v 1.1 2005-11-23 10:22:20 scetre Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 ******************************************************************************/

/**
 * \file
 * Test file on filter 
 */

static char *rcsId="@(#) $Id: vobsTestFilter.cpp,v 1.1 2005-11-23 10:22:20 scetre Exp $"; 
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
#include "vobs.h"
#include "vobsPrivate.h"


/*
 * Local Variables
 */

 

/* 
 * Signal catching functions  
 */



/* 
 * Main
 */

int main(int argc, char *argv[])
{
    // Initialize MCS services
    if (mcsInit(argv[0]) == mcsFAILURE)
    {
        // Error handling if necessary
        
        // Exit from the application with FAILURE
        exit (EXIT_FAILURE);
    }
    
    logSetStdoutLogLevel(logTEST);
    logInfo("Starting ...");

    vobsSTAR star1;
    star1.SetPropertyValue(vobsSTAR_POS_EQ_RA_MAIN, "03 47 29.08", "");
    star1.SetPropertyValue(vobsSTAR_POS_EQ_DEC_MAIN, "+24 06 18.5", "");
    star1.SetPropertyValue(vobsSTAR_CODE_QUALITY, "AAA", "");

    vobsSTAR star2;
    star2.SetPropertyValue(vobsSTAR_POS_EQ_RA_MAIN, "03 47 30.08", "");
    star2.SetPropertyValue(vobsSTAR_POS_EQ_DEC_MAIN, "+24 06 18.5", "");

    vobsSTAR star3;
    star3.SetPropertyValue(vobsSTAR_POS_EQ_RA_MAIN, "03 47 31.08", "");
    star3.SetPropertyValue(vobsSTAR_POS_EQ_DEC_MAIN, "+24 06 18.5", "");
    star3.SetPropertyValue(vobsSTAR_CODE_QUALITY, "AAA", "");
    
    vobsSTAR star4;
    star4.SetPropertyValue(vobsSTAR_POS_EQ_RA_MAIN, "03 47 32.08", "");
    star4.SetPropertyValue(vobsSTAR_POS_EQ_DEC_MAIN, "+24 06 18.5", "");
    star4.SetPropertyValue(vobsSTAR_CODE_QUALITY, "OOO", "");
    
    vobsSTAR_LIST starList;
    starList.AddAtTail(star1);
    starList.AddAtTail(star2);
    starList.AddAtTail(star3);
    starList.AddAtTail(star4);
    logTest("size of the list to filter = %d", starList.Size());
    starList.Display();

    vobsFILTER filterOnQflag;
    filterOnQflag.SetValueOnProperty(vobsSTAR_CODE_QUALITY, "AAA");
    filterOnQflag.Enable();

    logTest("A filter on Qflag = AAA is applied on the list");
    filterOnQflag.Apply(&starList);

    logTest("The filtering list has a size = %d", starList.Size());
    starList.Display();
    
    //
    // Insert your code here
    // 



    // Close MCS services
    mcsExit();
    
    // Exit from the application with SUCCESS
    exit (EXIT_SUCCESS);
}


/*___oOo___*/
