/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: vobsTestFilter.cpp,v 1.7 2011-02-25 15:58:58 lafrasse Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.6  2006/03/03 15:18:35  scetre
 * Changed rcsId to rcsId __attribute__ ((unused))
 * Updated test filter
 *
 * Revision 1.5  2005/12/07 12:23:38  gzins
 * Updated according to the new vobsGENERIC_FILTER API
 *
 * Revision 1.4  2005/11/30 15:22:50  gzins
 * Changed vobsFILTER to vobsGENERIC_FILTER
 *
 * Revision 1.3  2005/11/28 10:11:24  scetre
 * Changed OR test to AND test
 *
 * Revision 1.2  2005/11/24 08:15:27  scetre
 * Updated filter test
 *
 * Revision 1.1  2005/11/23 10:22:20  scetre
 * Generalized filter
 *
 ******************************************************************************/

/**
 * \file
 * Test file on filter 
 */

static char *rcsId __attribute__ ((unused))="@(#) $Id: vobsTestFilter.cpp,v 1.7 2011-02-25 15:58:58 lafrasse Exp $"; 


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
    logSetPrintDate(mcsFALSE);
    logSetPrintFileLine(mcsFALSE);

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

    vobsGENERIC_FILTER filterOnQflag("Qflg", vobsSTAR_CODE_QUALITY, vobsOR);
    filterOnQflag.AddCondition(vobsEQUAL, "AAA");
    filterOnQflag.AddCondition(vobsEQUAL, "OOO");
    filterOnQflag.Enable();

    vobsGENERIC_FILTER filteronKmagLess("KLess", vobsSTAR_PHOT_JHN_K);
    filteronKmagLess.AddCondition(vobsLESS, 4);
    filteronKmagLess.Enable();
    
    vobsGENERIC_FILTER filteronKmagMore("KMore", vobsSTAR_PHOT_JHN_K);
    filteronKmagMore.AddCondition(vobsGREATER, 2);
    filteronKmagMore.Enable();

    vobsGENERIC_FILTER filteronKmagLessEqual("KLessEqual", vobsSTAR_PHOT_JHN_K);
    filteronKmagLessEqual.AddCondition(vobsLESS_OR_EQUAL, 4);
    filteronKmagLessEqual.AddCondition(vobsGREATER_OR_EQUAL, 2);
    filteronKmagLessEqual.Enable();

    vobsGENERIC_FILTER filteronKmagMoreEqual("KMoreEquel", vobsSTAR_PHOT_JHN_K);
    filteronKmagMoreEqual.AddCondition(vobsGREATER_OR_EQUAL, 2);
    filteronKmagMoreEqual.Enable();
    
    vobsGENERIC_FILTER filteronKmagEqual("KEqual", vobsSTAR_PHOT_JHN_K);
    filteronKmagEqual.AddCondition(vobsEQUAL, 2);
    filteronKmagEqual.Enable();
        
    vobsGENERIC_FILTER filteronKmagPlusPlus("K++", vobsSTAR_PHOT_JHN_K, vobsOR);
    filteronKmagPlusPlus.AddCondition(vobsEQUAL, 2);
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

    logTest("A filter on 2 <= Kmag <= 4 is applied on the list");
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

    logTest("A filter on Kmag = 2  or Kmag = 5 is applied on the list");
    filteronKmagPlusPlus.Apply(&starList7);
    logTest("The filtering list has a size = %d", starList7.Size());
    starList7.Display();
  
    // Close MCS services
    mcsExit();
    
    // Exit from the application with SUCCESS
    exit (EXIT_SUCCESS);
}


/*___oOo___*/
