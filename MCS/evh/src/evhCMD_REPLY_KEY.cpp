/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * \file
 * evhCMD_REPLY_KEY class definition.
 */

/*
 * System Headers
 */
#include <iostream>
#include <string.h>
using namespace std;
#include <time.h>
#include <sys/time.h>

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
 * \param command command name associated to callback.
 * \param commandId command Id associated to callback.
 * \param timeout specify the maximum waiting time for reply. If timeout
 * expires before reply is received the callback is executed with message
 * tagged as 'error reply'.
 */
evhCMD_REPLY_KEY::evhCMD_REPLY_KEY(const mcsCMD command,
                                   const mcsINT32 commandId,
                                   const mcsINT32 timeout) :
evhKEY(evhTYPE_COMMAND_REPLY)
{
    SetCommand(command);
    SetCommandId(commandId);
    SetTimeout(timeout);
}

/**
 * Copy constructor.
 */
evhCMD_REPLY_KEY::evhCMD_REPLY_KEY(const evhCMD_REPLY_KEY &key) : evhKEY(key)
{
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
    SetCommand(key._command);
    SetCommandId(key._commandId);
    SetTimeout(key._timeout);

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
    // If it is the same event type (i.e. command reply event)
    if (evhKEY::IsSame(key) == mcsTRUE)
    {
        // Check the command name and Id match
        if ((strcmp(_command, ((evhCMD_REPLY_KEY *) & key)->_command) == 0) &&
                (_commandId == ((evhCMD_REPLY_KEY *) & key)->_commandId))
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
    // If it is the same event type (i.e. command reply event)
    if (evhKEY::IsSame(key) == mcsTRUE)
    {
        // Check whether the command is NULL or the command name and Id match
        if ((strcmp(((evhCMD_REPLY_KEY *) & key)->_command, mcsNULL_CMD) == 0) ||
                ((strcmp(_command, ((evhCMD_REPLY_KEY *) & key)->_command) == 0) &&
                (_commandId == ((evhCMD_REPLY_KEY *) & key)->_commandId)))
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
    strncpy(_command, command, mcsLEN16 - 1);

    return *this;
}

/**
 * Get command name.
 *
 * \return command name
 */
char *evhCMD_REPLY_KEY::GetCommand() const
{
    return ((char *) _command);
}

/**
 * Set command id
 *
 * \return reference to the object itself
 */
evhCMD_REPLY_KEY & evhCMD_REPLY_KEY::SetCommandId(const mcsINT32 commandId)
{
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
    return (_commandId);
}

/**
 * Set timeout, i.e. maximum time for waiting reply.
 *
 * \return reference to the object itself
 */
evhCMD_REPLY_KEY & evhCMD_REPLY_KEY::SetTimeout(const mcsINT32 timeout)
{
    _timeout = timeout;

    // If there is a timeout
    if (timeout != msgWAIT_FOREVER)
    {
        // Get the system time
        struct timeval  time;
        gettimeofday(&time, NULL);

        // Compute the expiration time
        _expirationDate.tv_sec  = time.tv_sec  + (mcsINT32) (timeout / 1000);
        _expirationDate.tv_usec = time.tv_usec + (timeout % 1000) * 1000;

        // If N usec is greater than 1 sec
        if (_expirationDate.tv_usec >= (1000 * 1000))
        {
            // Re-computed sec and usec
            _expirationDate.tv_sec  += (_expirationDate.tv_usec / (1000 * 1000));
            _expirationDate.tv_usec %= 1000 * 1000;
        }
        // End if
    }
        // Else
    else
    {
        // Set expiration date to 0
        _expirationDate.tv_sec = 0;
        _expirationDate.tv_usec = 0;
    }
    // End if

    return *this;
}

/**
 * Get timeout
 *
 * \return timeout
 */
mcsINT32 evhCMD_REPLY_KEY::GetTimeout() const
{
    return (_timeout);
}

/**
 * Get expiration date of the timeout
 *
 * \return mcsSUCCESS
 */
mcsCOMPL_STAT evhCMD_REPLY_KEY::GetTimeoutExpDate(struct timeval *expDate) const
{
    expDate->tv_sec = _expirationDate.tv_sec;
    expDate->tv_usec = _expirationDate.tv_usec;

    return (mcsSUCCESS);
}

/*___oOo___*/
