/*******************************************************************************
* JMMC project
*
* "@(#) $Id: evhCMD_REPLY_KEY.cpp,v 1.1 2005-01-07 17:43:55 gzins Exp $"
*
* who       when         what
* --------  -----------  -------------------------------------------------------
* gzins     04-Jan-2005  Created
*
*
*******************************************************************************/

/**
 * \file
 * evhCMD_REPLY_KEY class definition.
 */

static char *rcsId="@(#) $Id: evhCMD_REPLY_KEY.cpp,v 1.1 2005-01-07 17:43:55 gzins Exp $"; 
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
#include "evhCMD_REPLY_KEY.h"
#include "evhPrivate.h"

/**
 * Class constructor
 */
evhCMD_REPLY_KEY::evhCMD_REPLY_KEY(const mcsCMD command,
                                   const mcsINT32 commandId):
    evhKEY(evhTYPE_COMMAND_REPLY)
{
    SetCommand(command);
    SetCommandId(commandId);
}

/**
 * Copy constructor.
 */
evhCMD_REPLY_KEY::evhCMD_REPLY_KEY(const evhCMD_REPLY_KEY &key) : evhKEY(key)
{
    logExtDbg("evhCMD_REPLY_KEY::evhCMD_REPLY_KEY()"); 
    *this = key;
}

/**
 * Class destructor
 */
evhCMD_REPLY_KEY::~evhCMD_REPLY_KEY()
{
}

/**
 * Assignment operator
 */
evhCMD_REPLY_KEY& evhCMD_REPLY_KEY::operator =( const evhCMD_REPLY_KEY& key)
{
    logExtDbg("evhCMD_REPLY_KEY::operator =()"); 

    SetCommand(key._command);
    SetCommandId(key._commandId);

    return *this;
}

/*
 * Public methods
 */
/**
 * Determines whether the given key is equal to this.
 *
 * \param key element to be compared to this.
 * 
 * \return mcsTRUE if it is equal, mcsFALSE otherwise.
 */
mcsLOGICAL evhCMD_REPLY_KEY::IsSame(const evhKEY& key)
{
    logExtDbg("evhCMD_REPLY_KEY::IsSame()");

    // If it is the same event type (i.e. command event)
    if (evhKEY::IsSame(key) == mcsTRUE)
    {
        // Check the command name and Id match
        if ((strcmp(_command, ((evhCMD_REPLY_KEY *)&key)->_command) == 0) &&
            (_commandId == ((evhCMD_REPLY_KEY *)&key)->_commandId))
        {
            return mcsTRUE;
        }
    }
    // End if
    return mcsFALSE;
}

/**
 * Determines whether the given key matches to this.
 *
 * \param key element to be compared to this.
 * 
 * \return mcsTRUE if it matches, mcsFALSE otherwise.
 */
mcsLOGICAL evhCMD_REPLY_KEY::Match(const evhKEY& key)
{
    logExtDbg("evhCMD_REPLY_KEY::Match()");

    // If it is the same event type (i.e. command event)
    if (evhKEY::IsSame(key) == mcsTRUE)
    {
        // Check whether the command is NULL or the command name and Id match
        if ((strcmp(((evhCMD_REPLY_KEY *)&key)->_command, mcsNULL_CMD) == 0) ||
            ((strcmp(_command, ((evhCMD_REPLY_KEY *)&key)->_command) == 0) &&
             (_commandId == ((evhCMD_REPLY_KEY *)&key)->_commandId)))
        {
            return mcsTRUE;
        }
    }
    return mcsFALSE;
}

/**
 * Set command name 
 *
 * \return reference to the object itself
 *
 * \warning If command name length exceeds mcsCMD_LEN characters, it is
 * truncated
 */
evhCMD_REPLY_KEY & evhCMD_REPLY_KEY::SetCommand(const mcsCMD command)
{
    logExtDbg("evhCMD_REPLY_KEY::SetCommand()");

    strncpy(_command, command, sizeof(mcsCMD));

    return *this;
}

/**
 * Get command name.
 *
 * \return command name
 */
char *evhCMD_REPLY_KEY::GetCommand() const
{
    logExtDbg("evhCMD_REPLY_KEY::GetCommand()");

    return ((char *)_command);
}

/**
 * Set command id 
 *
 * \return reference to the object itself
 */
evhCMD_REPLY_KEY & evhCMD_REPLY_KEY::SetCommandId(const mcsINT32 commandId)
{
    logExtDbg("evhCMD_REPLY_KEY::SetCommandId()");

    _commandId = commandId;

    return *this;
}

/**
 * Get command id
 *
 * \return command id
 */
mcsINT32 evhCMD_REPLY_KEY::GetCommandId() const
{
    logExtDbg("evhCMD_REPLY_KEY::GetCommandId()");

    return (_commandId);
}

/*___oOo___*/
