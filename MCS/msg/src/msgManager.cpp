/*******************************************************************************
* JMMC project
*
* "@(#) $Id: msgManager.cpp,v 1.1 2004-12-08 18:31:55 gzins Exp $"
*
* who       when         what
* --------  -----------  -------------------------------------------------------
* gzins     06-Dec-2004  Created
*
*
*******************************************************************************/

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
 * \sa 
 * 
 * \bug OPTIONAL. Known bugs list if it exists.
 * \bug Bug 1 : bug 1 description
 *
 * \todo OPTIONAL. Things to forsee list, if needed. 
 * \todo Action 1 : action 1 description
 * 
 */

static char *rcsId="@(#) $Id: msgManager.cpp,v 1.1 2004-12-08 18:31:55 gzins Exp $"; 
static void *use_rcsId = ((void)&use_rcsId,(void *) &rcsId);


/* 
 * System Headers 
 */
#include <stdlib.h>
#include <iostream>
#include <signal.h>

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
#include "msgMANAGER.h"
#include "msgPrivate.h"

/*
 * Local Variables
 */
static msgMANAGER *msgManager=NULL;

/* 
 * Signal catching functions  
 */

/**
 * Trap certain system signals.
 *
 * Particulary manages 'broken pipe' and 'dead process' signals, plus the end
 * signal (i.e. CTRL-C).
 *
 * \param signalNumber the system signal to be trapped
 */

void msgSignalHandler (int signalNumber)
{
    logInfo("Received %d system signal...", signalNumber);
    if (signalNumber == SIGPIPE)
    {
        return;
    }
    logInfo("%s program aborted.", mcsGetProcName());
    delete (msgManager);
    exit (EXIT_SUCCESS);
}

/* 
 * Main
 */

int main(int argc, char *argv[])
{
    // Message manager instance
    msgManager = new msgMANAGER;
    
    /* Init system signal trapping */
    if (signal(SIGINT, msgSignalHandler) == SIG_ERR)
    {
        logError("signal(SIGINT, ...) function error");
        exit(EXIT_FAILURE);
    }
    if (signal (SIGTERM, msgSignalHandler) == SIG_ERR)
    {
        logError("signal(SIGTERM, ...) function error");
        exit(EXIT_FAILURE);
    }
    if (signal (SIGPIPE, msgSignalHandler) == SIG_ERR)
    {
        logError("signal(SIGPIPE, ...) function error");
        exit(EXIT_FAILURE);
    }

    // Initialization
    if (msgManager->Init(argc, argv) == FAILURE)
    {
        // Close error stack
        errCloseStack();
        exit (EXIT_FAILURE);
    }

    // Enter in main loop
    if (msgManager->MainLoop() == FAILURE)
    {
        // Error handling if necessary
        
        // Exit from the application with FAILURE
        exit (EXIT_FAILURE);
    }

    // Exit from the application with SUCCESS
    delete (msgManager);
    exit (EXIT_SUCCESS);
}


/*___oOo___*/
