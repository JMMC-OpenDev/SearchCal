/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: vobsTestStarList.cpp,v 1.3 2005-02-13 16:01:56 gzins Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.2  2005/02/13 08:54:00  gzins
 * Updated after vobs classes changes
 * Added CVS log as modifification history
 *
 * scetre    12-Jul-2004  Created
 *
 ******************************************************************************/

static char *rcsId="@(#) $Id: vobsTestStarList.cpp,v 1.3 2005-02-13 16:01:56 gzins Exp $"; 
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

    printf("Is star list empty? : %s\n", 
           (starList.IsEmpty() == mcsTRUE)?"YES":"NO");
    printf("Size of the list = %d\n", starList.Size());
    printf("Add 5 stars in the list.\n");
    for (int i = 1; i <= 5; i++)
    {
        mcsSTRING16 id;
        mcsSTRING16 ra;
        sprintf(id, "%d", i);
        sprintf(ra, "12 30 %d.3", i*10);
        stars[i].SetPropertyValue(vobsSTAR_ID_HD, id, "unknown");
        stars[i].SetPropertyValue(vobsSTAR_POS_EQ_RA_MAIN, ra, "unknown");
        starList.AddAtTail(stars[i]); 
    }
    
    printf("Is star list empty? : %s\n", 
           (starList.IsEmpty() == mcsTRUE)?"YES":"NO");
    printf("Size of the list = %d\n", starList.Size());
    printf("Save the list into starList.txt.\n");
    if (starList.Save("starList.txt", mcsTRUE) == mcsFAILURE)
    {
        errCloseStack();
        exit(EXIT_FAILURE);
    }
    printf("Clear the list.\n");
    starList.Clear();
    printf("Size of the list = %d\n", starList.Size());
    printf("Re-load the list from starList.txt.\n");
    if (starList.Load("starList.txt", mcsTRUE) == mcsFAILURE)
    {
        errCloseStack();
        exit(EXIT_FAILURE);
    }
    printf("Size of the list = %d\n", starList.Size());
    printf("Display the list.\n");
    starList.Display();
    printf("Display all elements, one by one,  of the list.\n");
    for (unsigned int el = 0; el < starList.Size(); el++)
    {
        starList.GetNextStar((mcsLOGICAL)(el==0))->Display();
    }
    printf("Display first element of the list.\n");
    starList.GetNextStar(mcsTRUE)->Display();

    printf("Merge list.\n");
    starList.Merge(starList);
    printf("Remove star 2.\n");    
    starList.Remove(stars[2]); 
    printf("Display the list.\n");
    starList.Display();
    printf("Clear the list.\n");
    starList.Clear(); 
    printf("Is star list empty? : %s\n", 
           (starList.IsEmpty() == mcsTRUE)?"YES":"NO");
    exit(EXIT_SUCCESS);

}


/*___oOo___*/
