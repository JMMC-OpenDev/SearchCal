/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: vobsTestCatalogList.cpp,v 1.1 2005-11-15 15:02:07 scetre Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 ******************************************************************************/

/**
 * \file
 * brief description of the program, which ends at this dot.
 *
 * \synopsis
 * \<Command Name\> [\e \<param1\> ... \e \<paramN\>] 
 *                     [\e \<option1\> ... \e \<optionN\>] 
 *
 * \param param1 : description of parameter 1, if it exists
 * \param paramN : description of parameter N, if it exists
 *
 * \n
 * \opt
 * \optname option1 : description of option 1, if it exists
 * \optname optionN : description of option N, if it exists
 * 
 * \n
 * \details
 * OPTIONAL detailed description of the c main file follows here.
 * 
 * \usedfiles
 * OPTIONAL. If files are used, for each one, name, and usage description.
 * \filename fileName1 :  usage description of fileName1
 * \filename fileName2 :  usage description of fileName2
 *
 * \n
 * \env
 * OPTIONAL. If needed, environmental variables accessed by the program. For
 * each variable, name, and usage description, as below.
 * \envvar envVar1 :  usage description of envVar1
 * \envvar envVar2 :  usage description of envVar2
 * 
 * \n
 * \warning OPTIONAL. Warning if any (software requirements, ...)
 *
 * \n
 * \ex
 * OPTIONAL. Command example if needed
 * \n Brief example description.
 * \code
 * Insert your command example here
 * \endcode
 *
 * \sa OPTIONAL. See also section, in which you can refer other documented
 * entities. Doxygen will create the link automatically.
 * \sa <entity to refer>
 * 
 * \bug OPTIONAL. Known bugs list if it exists.
 * \bug Bug 1 : bug 1 description
 *
 * \todo OPTIONAL. Things to forsee list, if needed. 
 * \todo Action 1 : action 1 description
 * 
 */

static char *rcsId="@(#) $Id: vobsTestCatalogList.cpp,v 1.1 2005-11-15 15:02:07 scetre Exp $"; 
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

    vobsCATALOG_LIST catalogList;
    vobsCATALOG *catalog;
    catalog=catalogList.Get(vobsCATALOG_ASCC_ID);

    printf("in the catalog list is present the catalog %s\n", 
            catalog->GetName());

    // Close MCS services
    mcsExit();
    
    // Exit from the application with SUCCESS
    exit (EXIT_SUCCESS);
}


/*___oOo___*/
