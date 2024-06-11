/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/


/* 
 * System Headers 
 */
#include <iostream>
#include <stdlib.h>
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

    vobsSTAR_PROPERTY_META* meta = new vobsSTAR_PROPERTY_META("UDDK_DIAM", "UDDK", vobsFLOAT_PROPERTY, NULL, "%.3f");

    vobsSTAR_PROPERTY property(meta);

    mcsDOUBLE testingFloats[] = {1.234567890123456789, 987654321.123456789, 0.00000123};
    for (int i = 0; i < 3; i++)
    {
        property.SetValue(testingFloats[i], vobsNO_CATALOG_ID, (vobsCONFIDENCE_INDEX) 0, mcsTRUE);
        mcsDOUBLE value = FP_NAN;
        property.GetValue(&value);
        mcsDOUBLE reference = testingFloats[i];
        mcsDOUBLE delta = reference - value;
        cout << "FLOAT --- value = '" << value << "' " << (reference == value ? "==" : "!=") << " '" << reference << "' (original value), delta = '" << delta << "'." << endl;
        cout << "FLOAT --- Sumary : " << property.GetSummaryString() << endl << endl;
    }

    cout << endl << endl;

    mcsSTRING64 testingStrings[] = {"1.234567890123456789", "987654321.123456789", "0.00000123"};
    for (int i = 0; i < 3; i++)
    {
        property.SetValue(testingStrings[i], vobsNO_CATALOG_ID, (vobsCONFIDENCE_INDEX) 0, mcsTRUE);
        mcsDOUBLE value = FP_NAN;
        property.GetValue(&value);
        //mcsDOUBLE reference = atof(testingStrings[i]);
        mcsDOUBLE reference = testingFloats[i];
        mcsDOUBLE delta = reference - value;
        cout << "STRING --- value = '" << value << "' " << (reference == value ? "==" : "!=") << " '" << string(testingStrings[i]) << "' (original value), delta = '" << delta << "'." << endl;
        cout << "STRING --- Sumary : " << property.GetSummaryString() << endl << endl;
    }

    logInfo("Exiting ...");
    exit(EXIT_SUCCESS);
}

/*___oOo___*/
