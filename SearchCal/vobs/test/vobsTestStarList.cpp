/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: vobsTestStarList.cpp,v 1.2 2005-02-13 08:54:00 gzins Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * scetre    12-Jul-2004  Created
 *
 ******************************************************************************/

static char *rcsId="@(#) $Id: vobsTestStarList.cpp,v 1.2 2005-02-13 08:54:00 gzins Exp $"; 
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

    logSetStdoutLogLevel(logEXTDBG);

    vobsSTAR_LIST starList;
    vobsSTAR      star[6];

    printf("Is star list empty? : %s\n", 
           (starList.IsEmpty() == mcsTRUE)?"YES":"NO");
    printf("Size of the list = %d\n", starList.Size());
    printf("Add one stars in the list.\n");
    for (int i = 1; i <= 5; i++)
    {
        mcsSTRING16 id;
        mcsSTRING16 ra;
        sprintf(id, "%d", i);
        sprintf(ra, "12 30 %d.3", i*10);
        star[i].SetPropertyValue("ID_MAIN", id, "");
        star[i].SetPropertyValue("POS_EQ_RA_MAIN", ra, "");
        starList.AddAtTail(star[i]); 
    }
    printf("Is star list empty? : %s\n", 
           (starList.IsEmpty() == mcsTRUE)?"YES":"NO");
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
    starList.Remove(star[2]); 
    printf("Display the list.\n");
    starList.Display();
    printf("Clear the list.\n");
    starList.Clear(); 
    printf("Is star list empty? : %s\n", 
           (starList.IsEmpty() == mcsTRUE)?"YES":"NO");

    exit(EXIT_SUCCESS);

}


/*___oOo___*/
