/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: msgMESSAGE_FILTER.cpp,v 1.1 2005-02-09 16:42:26 lafrasse Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 ******************************************************************************/

/**
 * \file
 *  Interface class used to handle msgMESSAGE filtering on reception.
 */

static char *rcsId="@(#) $Id: msgMESSAGE_FILTER.cpp,v 1.1 2005-02-09 16:42:26 lafrasse Exp $"; 
static void *use_rcsId = ((void)&use_rcsId,(void *) &rcsId);

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

/*
 * Local Headers 
 */
#include "msgMESSAGE_FILTER.h"
#include "msgPrivate.h"


/**
 * Class constructor
 */
msgMESSAGE_FILTER::msgMESSAGE_FILTER(const mcsCMD   command,
                                     const mcsINT32 commandId )
{
    // Initialze the command name member
    memset (_command, 0, sizeof(_command));
    // Set the command name member
    strncpy(_command, command, sizeof(_command));

    // Set the command name member
    _commandId = commandId;
}

/**
 * Class destructor
 */
msgMESSAGE_FILTER::~msgMESSAGE_FILTER()
{
}


/*
 * Public methods
 */

/**
 * Return the filter command name.
 *
 * \return a character pointer on an mcsCMD value
 */
const char* msgMESSAGE_FILTER::GetCommand(void) const
{
    logExtDbg("msgMESSAGE_FILTER::GetCommand()");

    return _command;
}

/**
 * Return the filter command identifier.
 *
 * \return an mcsINT32 value
 */
const mcsINT32 msgMESSAGE_FILTER::GetCommandId(void) const
{
    logExtDbg("msgMESSAGE_FILTER::GetCommandId()");

    return _commandId;
}

/**
 * Return wether the given message must be filtered (is not the expected reply)
 * or not.
 *
 * \return mcsTRUE if the given msgMESSAGE object match the filter (i.e is the
 * expected one), or mcsFALSE if the given message is not the one corresponding
 * to the filter
 */
const mcsLOGICAL msgMESSAGE_FILTER::IsMatchedBy(const msgMESSAGE& message) const
{
    logExtDbg("msgMESSAGE_FILTER::IsMatchedBy()");

    // If the given msgMESSAGE object seems to be the one expected
    if (_commandId == message.GetCommandId())
    {
        return mcsTRUE;
    }

    return mcsFALSE;
}

/**
 * Show the msgMESSAGE_FILTER content on the standard output.
 */
std::ostream& operator<< (      std::ostream&      stream,
                          const msgMESSAGE_FILTER& filter)
{
    return stream << "msgMESSAGE_FILTER ="                           << endl
           << "{"                                                    << endl
           << "\t\tcommand      = '" << filter.GetCommand()   << "'" << endl
           << "\t\tcommandId    = '" << filter.GetCommandId() << "'" << endl
           << "}";
}


/*
 * Protected methods
 */


/*
 * Private methods
 */


/*___oOo___*/
