/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * @file
 * Search Calibrators SERVER
 */



/*
 * System Headers
 */
#include <stdlib.h>
#include <iostream>
#include <signal.h>
#include <malloc.h>
#include <execinfo.h>
#include <unistd.h>
using namespace std;

/*
 * MCS Headers
 */
#include "mcs.h"
#include "log.h"
#include "err.h"
#include "timlog.h"

#include "alx.h"

/*
 * Local Headers
 */
#include "sclsvr.h"
#include "sclsvrSERVER.h"
#include "sclsvrPrivate.h"


/*
 * Local macro
 */

/** flag to avoid reentrant shutdown hook */
static mcsLOGICAL sclsvrShutdown = mcsFALSE;

/**
 * Exit hook
 * @param returnCode
 */
void sclsvrServerExit(int returnCode)
{
    if (sclsvrShutdown == mcsFALSE)
    {
        sclsvrShutdown = mcsTRUE;

        logWarning("sclsvrServerExit[%d] : ", returnCode);

        if (returnCode != 0)
        {
            errCloseStack();
        }

        // free property meta data:
        sclsvrExit();

        // free the timlog table:
        timlogClear();

        // Stop err and log module:
        errExit();
        logExit();

        mcsExit();

        // Exit from the application:
        exit(returnCode);
    }
    else
    {
        logWarning("sclsvrServerExit already in progress; please wait ...");
    }
}

#define addSignalHandler(num, sh)                    \
if (signal(num, sh) == SIG_ERR)                      \
{                                                    \
    logError("signal(%d, ...) function error", num); \
    exit(EXIT_FAILURE);                              \
}

/*
 * Signal catching functions
 */
static inline void printStackTrace(mcsUINT32 max_frames = 63)
{
    printf("Stack trace:\n");

    // storage array for stack trace address data
    void* addrlist[max_frames + 1];

    // retrieve current stack addresses
    mcsINT32 addrlen = backtrace(addrlist, sizeof ( addrlist ) / sizeof (void*));

    if (addrlen == 0)
    {
        printf("[none]\n");
        return;
    }
    fflush(stdout);

    // write symbols to stdout
    backtrace_symbols_fd(addrlist, addrlen, STDOUT_FILENO);
}

void sclsvrSignalHandler(int signum)
{
    // associate each signal with a signal name string.
    const char* name = NULL;
    switch ( signum )
    {
        case SIGINT:  name = "SIGINT";
            break;
        case SIGQUIT: name = "SIGQUIT";
            break;
        case SIGILL:  name = "SIGILL";
            break;
        case SIGABRT: name = "SIGABRT";
            break;
        case SIGBUS:  name = "SIGBUS";
            break;
        case SIGFPE:  name = "SIGFPE";
            break;
        case SIGSEGV: name = "SIGSEGV";
            break;
        case SIGPIPE: name = "SIGPIPE";
            break;
        case SIGTERM: name = "SIGTERM";
            break;
        default:      name = "[undefined]";
    }

    printf("Received a %s [%d] system signal ...\n", name, signum);
    fflush(stdout);

    if ((signum == SIGINT)
            || (signum == SIGQUIT)
            || (signum == SIGPIPE))
    {
        // just ignore such signal
        return;
    }
    if (signum == SIGTERM)
    {
        // Stop properly the service:
        sclsvrServerExit(EXIT_SUCCESS);
    }
    else
    {
        // Fatal error:
        // Dump a stack trace.
        printStackTrace();

        exit(signum);
    }
}

/*
 * Main
 */
int main(int argc, char *argv[])
{
    // Turn off mmap usage (slower and do not release memory efficiently)
    mallopt(M_MMAP_MAX, 0);

    // Init system signal trapping
    addSignalHandler(SIGINT, sclsvrSignalHandler);
    addSignalHandler(SIGQUIT, sclsvrSignalHandler);
    addSignalHandler(SIGILL, sclsvrSignalHandler);
    addSignalHandler(SIGABRT, sclsvrSignalHandler);
    addSignalHandler(SIGBUS, sclsvrSignalHandler);
    addSignalHandler(SIGFPE, sclsvrSignalHandler);
    addSignalHandler(SIGSEGV, sclsvrSignalHandler);
    addSignalHandler(SIGPIPE, sclsvrSignalHandler);
    addSignalHandler(SIGTERM, sclsvrSignalHandler);

    mcsCOMPL_STAT cmdStatus = mcsFAILURE;

    // The following instructions have been placed in {} in order to call
    // sclsvrSERVER destructor when application exits.
    {
        // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!! Warning !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        //
        // As of version 3.7.0 and later, sclsvrServer callback mecanism based
        // on MCS module 'evh' is no longuer in use, in order to at last support
        // GCC 4.x and later.
        //
        // The workarround consists in statically parsing the two last CLI args,
        // and run the proper method accordinally.

        // Initialize log and err module:
        logInit();
        errInit();

        // initialize alx module (preload tables):
        alxInit();

        // initialize sclsvr module but do not preload JSDC:
        sclsvrInit(false);

        // Init MCS event server, only to handle MCS standard options like '-v',
        // '-h' and so on.
        sclsvrSERVER scalibServer;

        // Initialize mcs / log:
        if (scalibServer.Init(argc, argv) == mcsSUCCESS)
        {
            // Retrieve the COMMAND name to select among GETCAL or GETSTAR mode.
            char* cmdName = argv[argc - 2];

            // Get COMMAND argument
            char* cmdArgs = argv[argc - 1];

            //            try
            //            {
            // GETCAL mode
            if (strcmp(cmdName, "GETCAL") == 0)
            {
                cmdStatus = scalibServer.GetCal(cmdArgs);
            }
            else
                if (strcmp(cmdName, "GETSTAR") == 0)
            {
                cmdStatus = scalibServer.GetStar(cmdArgs);
            }
            else
            {
                printf("Unknown COMMAND '%s'.\n", cmdName);
            }
            //            } catch (std::exception const& e)
            //            {
            //                logInfo("exception occured: %s", e.what());
            //                return mcsFAILURE;
            //            }
        }
    } // free sclsvrSERVER

    sclsvrServerExit((cmdStatus == mcsSUCCESS) ? EXIT_SUCCESS : EXIT_FAILURE);
}
