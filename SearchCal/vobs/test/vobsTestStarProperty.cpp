/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: vobsTestStarProperty.cpp,v 1.4 2011-02-25 15:58:58 lafrasse Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.3  2009/10/26 12:59:16  lafrasse
 * Changed delta compution method on String tests.
 *
 * Revision 1.2  2009/10/16 10:00:54  mella
 * add string tests
 *
 * Revision 1.1  2009/10/14 14:50:58  lafrasse
 * Added proper support of floating point value (without loosing presion with
 * conversion from string representation) with accompagning tests.
 *
 ******************************************************************************/

static char *rcsId __attribute__ ((unused))="@(#) $Id: vobsTestStarProperty.cpp,v 1.4 2011-02-25 15:58:58 lafrasse Exp $"; 

/* 
 * System Headers 
 */
#include <iostream>
using namespace std;


/*
 * MCS Headers 
 */
#include "mcs.h"
#include "log.h"
#include "err.h"
#include "timlog.h"

/*
 * Local Headers 
 */
#include "vobsSTAR.h"
#include "vobsSTAR_LIST.h"
#include "vobsPrivate.h"

int main(int argc, char *argv[])
{
    mcsInit(argv[0]);

    logSetStdoutLogLevel(logTEST);
    logSetPrintDate(mcsFALSE);
    logSetPrintFileLine(mcsFALSE);

    timlogStart(MODULE_ID, logINFO, "98", "testStarProperty");
    logInfo("Starting ...");

    vobsSTAR_PROPERTY property(vobsSTAR_UDDK_DIAM, "UDDK", vobsFLOAT_PROPERTY, vobsSTAR_PROP_NOT_SET,  "%.3f");

    mcsFLOAT testingFloats[] = {1.234567890123456789, 987654321.123456789, 0.00000123};
    for (int i = 0; i < 3; i++)
    {
        property.SetValue(testingFloats[i], "Float Test", (vobsCONFIDENCE_INDEX)0, mcsTRUE);
        mcsFLOAT value = FP_NAN;
        property.GetValue(&value);
        mcsFLOAT reference = testingFloats[i];
        mcsDOUBLE delta = reference - value;
        cout << "FLOAT --- value = '" << value << "' " << (reference == value ? "==" : "!=") << " '" << reference << "' (original value), delta = '" << delta << "'." << endl;
        cout << "FLOAT --- Sumary : " << property.GetSummaryString() << endl << endl;
    }

    cout << endl << endl;

    mcsSTRING64 testingStrings[]= {"1.234567890123456789", "987654321.123456789", "0.00000123"};
    for (int i = 0; i < 3; i++)
    {
        property.SetValue(testingStrings[i], "String Test", (vobsCONFIDENCE_INDEX)0, mcsTRUE);
        mcsFLOAT value = FP_NAN;
        property.GetValue(&value);
        //mcsFLOAT reference = atof(testingStrings[i]);
        mcsFLOAT reference = testingFloats[i];
        mcsDOUBLE delta = reference - value;
        cout << "STRING --- value = '" << value << "' " << (reference == value ? "==" : "!=") << " '" << string(testingStrings[i]) << "' (original value), delta = '" << delta << "'." << endl;
        cout << "STRING --- Sumary : " << property.GetSummaryString() << endl << endl;
    }
    
    logInfo("Exiting ...");
    exit(EXIT_SUCCESS);
}

/*___oOo___*/
