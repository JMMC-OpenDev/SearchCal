/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * \file
 *  Class used to filter message on reception.
 */

/*
 * System Headers
 */
#include <iostream>
#include <string.h>
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
    memset (_command, 0, mcsLEN16);
    // Set the command name member
    strncpy(_command, command, mcsLEN16 - 1);

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
    return _command;
}

/**
 * Return the filtered command identifier.
 *
 * \return identifier of the filtered command.
 */
mcsINT32 msgMESSAGE_FILTER::GetCommandId(void) const
{
    return _commandId;
}

/**
 * Check whether the given message is the expected one or not
 *
 * \return mcsTRUE if the given msgMESSAGE object match the filter (i.e is the
 * expected one), or mcsFALSE otherwise.
 */
mcsLOGICAL msgMESSAGE_FILTER::IsMatchedBy(const msgMESSAGE& message) const
{
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
