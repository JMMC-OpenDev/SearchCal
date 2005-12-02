/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: vobsTestStarList.cpp,v 1.7 2005-12-02 17:44:00 lafrasse Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.6  2005/11/16 10:45:14  scetre
 * Updated vobs test
 *
 * Revision 1.5  2005/11/11 16:39:54  gzins
 * Updated to test Sort() method
 *
 * Revision 1.4  2005/06/20 13:38:17  scetre
 * Update program for test of Remove() method
 *
 * Revision 1.3  2005/02/13 16:01:56  gzins
 * Added test for Load and Save methods
 *
 * Revision 1.2  2005/02/13 08:54:00  gzins
 * Updated after vobs classes changes
 * Added CVS log as modifification history
 *
 * scetre    12-Jul-2004  Created
 *
 ******************************************************************************/

static char *rcsId="@(#) $Id: vobsTestStarList.cpp,v 1.7 2005-12-02 17:44:00 lafrasse Exp $"; 
static void *use_rcsId = ((void)&use_rcsId,(void *) &rcsId);

/* 
 * System Headers 
 */
#include <iostream>
using namespace std;
#include <string.h>

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

    vobsSTAR_LIST starList;
    vobsSTAR      stars[6];

    logTest("Is star list empty? : %s\n", 
           (starList.IsEmpty() == mcsTRUE)?"YES":"NO");
    logTest("Size of the list = %d\n", starList.Size());
    logTest("Add 5 stars in the list.\n");
    for (int i = 1; i <= 5; i++)
    {

        mcsSTRING16 id;
        mcsSTRING16 ra, dec;
        sprintf(id, "%d", i);
        sprintf(ra, "12 30 %d.3", (i*20) % 17);
        sprintf(dec, "04 30 %d.3", (i*20) % 17);
        stars[i].SetPropertyValue(vobsSTAR_ID_HD, id, "unknown");
        stars[i].SetPropertyValue(vobsSTAR_POS_EQ_RA_MAIN, ra, "unknown");
        stars[i].SetPropertyValue(vobsSTAR_POS_EQ_DEC_MAIN, dec, "unknown");
        starList.AddAtTail(stars[i]); 
    }

    logTest("Is star list empty? : %s\n", 
           (starList.IsEmpty() == mcsTRUE)?"YES":"NO");
    logTest("Size of the list = %d\n", starList.Size());
    logTest("Save the list into starList.txt.\n");
    if (starList.Save("starList.txt", mcsTRUE) == mcsFAILURE)
    {
        errCloseStack();
        exit(EXIT_FAILURE);
    }
    logTest("Save the list as VOTable into starList.xml.\n");
    if (starList.SaveToVOTable("starList.xml") == mcsFAILURE)
    {
        errCloseStack();
        exit(EXIT_FAILURE);
    }
    logTest("Clear the list.\n");
    starList.Clear();
    logTest("Size of the list = %d\n", starList.Size());
    logTest("Re-load the list from starList.txt.\n");
    if (starList.Load("starList.txt", mcsTRUE) == mcsFAILURE)
    {
        errCloseStack();
        exit(EXIT_FAILURE);
    }
    logTest("Size of the list = %d\n", starList.Size());
    logTest("Display the list.\n");
    starList.Display();
    logTest("Sort list by DEC\n");
    if (starList.Sort(vobsSTAR_POS_EQ_RA_MAIN)== mcsFAILURE)
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
        starList.GetNextStar((mcsLOGICAL)(el==0))->Display();
        starList.Remove(stars[1]); 
    }
    logTest("Display first element of the list.\n");
    starList.GetNextStar(mcsTRUE)->Display();

    logTest("Merge list.\n");
    starList.Merge(starList);
    logTest("Remove star 2.\n");    
    starList.Remove(stars[2]); 
    logTest("Display the list.\n");
    starList.Display();
    logTest("Clear the list.\n");
    starList.Clear(); 
    logTest("Is star list empty? : %s\n", 
           (starList.IsEmpty() == mcsTRUE)?"YES":"NO");
    exit(EXIT_SUCCESS);

}


/*___oOo___*/
