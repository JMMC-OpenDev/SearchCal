/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/


/* 
 * System Headers 
 */
#include <iostream>
using namespace std;
#include <string.h>
#include <stdlib.h>

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

int main(int argc, char *argv[])
{
    mcsInit(argv[0]);

    logSetStdoutLogLevel(logTEST);
    logSetPrintDate(mcsFALSE);
    logSetPrintFileLine(mcsFALSE);

    vobsSTAR_LIST starList("TestList");
    vobsSTAR stars[6];

    logTest("Is star list empty? : %s\n",
            (starList.IsEmpty() == mcsTRUE) ? "YES" : "NO");
    logTest("Size of the list = %d\n", starList.Size());
    logTest("Add 5 stars in the list.\n");
    for (int i = 1; i <= 5; i++)
    {

        mcsSTRING16 id;
        mcsSTRING16 ra, dec;
        sprintf(id, "%d", i);
        sprintf(ra, "12 30 %d.3", (i * 20) % 17);
        sprintf(dec, "04 30 %d.3", (i * 20) % 17);
        stars[i].SetPropertyValue(vobsSTAR_ID_HD, id, vobsNO_CATALOG_ID);
        stars[i].SetPropertyValue(vobsSTAR_POS_EQ_RA_MAIN, ra, vobsNO_CATALOG_ID);
        stars[i].SetPropertyValue(vobsSTAR_POS_EQ_DEC_MAIN, dec, vobsNO_CATALOG_ID);
        starList.AddAtTail(stars[i]);
    }

    logTest("Is star list empty? : %s\n",
            (starList.IsEmpty() == mcsTRUE) ? "YES" : "NO");
    logTest("Size of the list = %d\n", starList.Size());
    logTest("Save the list into starList.txt.\n");
    if (starList.Save("starList.txt", mcsTRUE) == mcsFAILURE)
    {
        errCloseStack();
        exit(EXIT_FAILURE);
    }
    logTest("Save the list as VOTable into starList.xml.\n");
    if (starList.SaveToVOTable("test", "starList.xml",
                               "JMMC_team@Grenoble.france",
                               "vobsTestStarList v?? :)",
                               "No request for this one...",
                               "No XML request either.",mcsTRUE) == mcsFAILURE)
    {
        errCloseStack();
        exit(EXIT_FAILURE);
    }
    logTest("Clear the list.\n");
    starList.Clear();
    logTest("Size of the list = %d\n", starList.Size());
    logTest("Re-load the list from starList.txt.\n");
    if (starList.Load("starList.txt", NULL, NULL, mcsTRUE) == mcsFAILURE)
    {
        errCloseStack();
        exit(EXIT_FAILURE);
    }
    logTest("Size of the list = %d\n", starList.Size());
    logTest("Display the list.\n");
    starList.Display();
    logTest("Sort list by DEC\n");
    if (starList.Sort(vobsSTAR_POS_EQ_RA_MAIN) == mcsFAILURE)
    {
        errCloseStack();
        exit(EXIT_FAILURE);
    }
    logTest("Display the list.\n");
    starList.Display();
    logTest("Display all elements, one by one, of the list\n");
    logTest("and delete first element of the list.\n");
    for (unsigned int el = 0; el < starList.Size(); el++)
    {
        starList.GetNextStar((mcsLOGICAL) (el == 0))->Display();
        starList.Remove(stars[1]);
    }
    logTest("Display first element of the list.\n");
    starList.GetNextStar(mcsTRUE)->Display();

    // criteria list: RA/DEC within 1.5 arcsec
    vobsSTAR_COMP_CRITERIA_LIST criteriaListRaDec;

    mcsDOUBLE raDecRadius = 1.5 * alxARCSEC_IN_DEGREES;

    // Build criteria list on ra dec (1 arcsec)
    // Add Criteria on coordinates
    FAIL(criteriaListRaDec.Add(vobsSTAR_POS_EQ_RA_MAIN, raDecRadius));
    FAIL(criteriaListRaDec.Add(vobsSTAR_POS_EQ_DEC_MAIN, raDecRadius));

    logTest("Merge list.\n");
    starList.Merge(starList, &criteriaListRaDec, mcsFALSE);
    logTest("Remove star 2.\n");
    starList.Remove(stars[2]);
    logTest("Display the list.\n");
    starList.Display();
    logTest("Clear the list.\n");
    starList.Clear();
    logTest("Is star list empty? : %s\n",
            (starList.IsEmpty() == mcsTRUE) ? "YES" : "NO");
    exit(EXIT_SUCCESS);

}


/*___oOo___*/
