/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: msgMESSAGE_FILTER.cpp,v 1.3 2006-05-11 13:04:56 mella Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.2  2005/02/14 07:59:18  gzins
 * Minor documentation changes
 *
 * Revision 1.1  2005/02/09 16:42:26  lafrasse
 * Added msgMESSAGE_FILTER class to manage message queues
 *
 ******************************************************************************/

/**
 * \file
 *  Class used to filter message on reception.
 */

static char *rcsId __attribute__ ((unused)) ="@(#) $Id: msgMESSAGE_FILTER.cpp,v 1.3 2006-05-11 13:04:56 mella Exp $";
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
    // Initialize the command name member
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
 * Return the filtered command name.
 *
 * \return pointer to the name of the filtered command. 
 */
const char* msgMESSAGE_FILTER::GetCommand(void) const
{
    logExtDbg("msgMESSAGE_FILTER::GetCommand()");

    return _command;
}

/**
 * Return the filtered command identifier.
 *
 * \return identifier of the filtered command. 
 */
const mcsINT32 msgMESSAGE_FILTER::GetCommandId(void) const
{
    logExtDbg("msgMESSAGE_FILTER::GetCommandId()");

    return _commandId;
}

/**
 * Check whether the given message is the expected one or not 
 *
 * \return mcsTRUE if the given msgMESSAGE object match the filter (i.e is the
 * expected one), or mcsFALSE otherwise. 
 */
const mcsLOGICAL msgMESSAGE_FILTER::IsMatchedBy(const msgMESSAGE& message) const
{
    logExtDbg("msgMESSAGE_FILTER::IsMatchedBy()");

    // If the given msgMESSAGE object is the expected one.
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

/*___oOo___*/
