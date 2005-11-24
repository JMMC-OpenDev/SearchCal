/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: vobsTestFilter.cpp,v 1.2 2005-11-24 08:15:27 scetre Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.1  2005/11/23 10:22:20  scetre
 * Generalized filter
 *
 ******************************************************************************/

/**
 * \file
 * Test file on filter 
 */

static char *rcsId="@(#) $Id: vobsTestFilter.cpp,v 1.2 2005-11-24 08:15:27 scetre Exp $"; 
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
    star1.SetPropertyValue(vobsSTAR_PHOT_JHN_K, 0.0, "");

    vobsSTAR star2;
    star2.SetPropertyValue(vobsSTAR_POS_EQ_RA_MAIN, "03 47 30.08", "");
    star2.SetPropertyValue(vobsSTAR_POS_EQ_DEC_MAIN, "+24 06 18.5", "");
    star2.SetPropertyValue(vobsSTAR_PHOT_JHN_K, 2.0, "");

    vobsSTAR star3;
    star3.SetPropertyValue(vobsSTAR_POS_EQ_RA_MAIN, "03 47 31.08", "");
    star3.SetPropertyValue(vobsSTAR_POS_EQ_DEC_MAIN, "+24 06 18.5", "");
    star3.SetPropertyValue(vobsSTAR_CODE_QUALITY, "AAA", "");
    star3.SetPropertyValue(vobsSTAR_PHOT_JHN_K, 4.0, "");
    
    vobsSTAR star4;
    star4.SetPropertyValue(vobsSTAR_POS_EQ_RA_MAIN, "03 47 32.08", "");
    star4.SetPropertyValue(vobsSTAR_POS_EQ_DEC_MAIN, "+24 06 18.5", "");
    star4.SetPropertyValue(vobsSTAR_CODE_QUALITY, "OOO", "");
    star4.SetPropertyValue(vobsSTAR_PHOT_JHN_K, 5.0, "");
    
    vobsSTAR_LIST starList;
    starList.AddAtTail(star1);
    starList.AddAtTail(star2);
    starList.AddAtTail(star3);
    starList.AddAtTail(star4);
    vobsSTAR_LIST starList2;
    starList2.Copy(starList);
    vobsSTAR_LIST starList3;
    starList3.Copy(starList);
    vobsSTAR_LIST starList4;
    starList4.Copy(starList);
    vobsSTAR_LIST starList5;
    starList5.Copy(starList);
    vobsSTAR_LIST starList6;
    starList6.Copy(starList);
    vobsSTAR_LIST starList7;
    starList7.Copy(starList);
    
    logTest("size of the list to filter = %d", starList.Size());
    starList.Display();

    vobsFILTER filterOnQflag;
    filterOnQflag.SetPropertyId(vobsSTAR_CODE_QUALITY);
    filterOnQflag.AddCondition("AAA");
    filterOnQflag.AddCondition("OOO");
    filterOnQflag.Enable();

    vobsFILTER filteronKmagLess;
    filteronKmagLess.SetPropertyId(vobsSTAR_PHOT_JHN_K);
    filteronKmagLess.AddCondition(vobsLESS, 4);
    filteronKmagLess.Enable();
    
    vobsFILTER filteronKmagMore;
    filteronKmagMore.SetPropertyId(vobsSTAR_PHOT_JHN_K);
    filteronKmagMore.AddCondition(vobsMORE, 2);
    filteronKmagMore.Enable();

    vobsFILTER filteronKmagLessEqual;
    filteronKmagLessEqual.SetPropertyId(vobsSTAR_PHOT_JHN_K);
    filteronKmagLessEqual.AddCondition(vobsLESS_EQUAL, 4);
    filteronKmagLessEqual.Enable();

    vobsFILTER filteronKmagMoreEqual;
    filteronKmagMoreEqual.SetPropertyId(vobsSTAR_PHOT_JHN_K);
    filteronKmagMoreEqual.AddCondition(vobsMORE_EQUAL, 2);
    filteronKmagMoreEqual.Enable();
    
    vobsFILTER filteronKmagEqual;
    filteronKmagEqual.SetPropertyId(vobsSTAR_PHOT_JHN_K);
    filteronKmagEqual.AddCondition(vobsEQUAL, 2);
    filteronKmagEqual.Enable();
        
    vobsFILTER filteronKmagPlusPlus;
    filteronKmagPlusPlus.SetPropertyId(vobsSTAR_PHOT_JHN_K);
    filteronKmagPlusPlus.AddCondition(vobsEQUAL, 0);
    filteronKmagPlusPlus.AddCondition(vobsEQUAL, 2);
    filteronKmagPlusPlus.AddCondition(vobsEQUAL, 4);
    filteronKmagPlusPlus.AddCondition(vobsEQUAL, 5);
    filteronKmagPlusPlus.Enable();
    
    logTest("A filter on Qflag = AAA  and OOO is applied on the list");
    filterOnQflag.Apply(&starList);
    logTest("The filtering list has a size = %d", starList.Size());
    starList.Display();
    
    logTest("A filter on Kmag < 4 is applied on the list");
    filteronKmagLess.Apply(&starList2);
    logTest("The filtering list has a size = %d", starList2.Size());
    starList2.Display();

    logTest("A filter on Kmag > 2 is applied on the list");
    filteronKmagMore.Apply(&starList3);
    logTest("The filtering list has a size = %d", starList3.Size());
    starList3.Display();

    logTest("A filter on Kmag <= 4 is applied on the list");
    filteronKmagLessEqual.Apply(&starList4);
    logTest("The filtering list has a size = %d", starList4.Size());
    starList4.Display();

    logTest("A filter on Kmag >=2 is applied on the list");
    filteronKmagMoreEqual.Apply(&starList5);
    logTest("The filtering list has a size = %d", starList5.Size());
    starList5.Display();

    logTest("A filter on Kmag = 2 is applied on the list");
    filteronKmagEqual.Apply(&starList6);
    logTest("The filtering list has a size = %d", starList6.Size());
    starList6.Display();

    logTest("A filter on Kmag =0 =2 =4 =5 is applied on the list");
    filteronKmagPlusPlus.Apply(&starList7);
    logTest("The filtering list has a size = %d", starList7.Size());
    starList7.Display();
    //
    // Insert your code here
    // 



    // Close MCS services
    mcsExit();
    
    // Exit from the application with SUCCESS
    exit (EXIT_SUCCESS);
}


/*___oOo___*/
