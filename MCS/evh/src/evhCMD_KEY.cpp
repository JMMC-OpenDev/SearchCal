/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * \file
 * Definition of the evhCMD_KEY class.
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
#include "evhCMD_KEY.h"
#include "evhPrivate.h"

/*
 * Class constructor
 */
evhCMD_KEY::evhCMD_KEY(const mcsCMD command, const char *cdf) :
evhKEY(evhTYPE_COMMAND)
{
    SetCommand(command);
    SetCdf(cdf);
}

/**
 * Copy constructor.
 */
evhCMD_KEY::evhCMD_KEY(const evhCMD_KEY &key) : evhKEY(key)
{
    *this = key;
}

/**
 * Class destructor
 */
evhCMD_KEY::~evhCMD_KEY()
{
}

/**
 * Assignment operator
 */
evhCMD_KEY& evhCMD_KEY::operator =( const evhCMD_KEY& key)
{
    SetCommand(key._command);
    SetCdf(key._cdf);

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
mcsLOGICAL evhCMD_KEY::IsSame(const evhKEY& key)
{
    // If it is the same event type (i.e. command event)
    if (evhKEY::IsSame(key) == mcsTRUE)
    {
        if (strcmp(_command, ((evhCMD_KEY *) & key)->_command) == 0)
        {
            return mcsTRUE;
        }
    }
    return mcsFALSE;
}

/**
 * Determines whether the given key matches to this.
 *
 * \param key element to be compared to this.
 *
 * \return mcsTRUE if it matches, mcsFALSE otherwise.
 */
mcsLOGICAL evhCMD_KEY::Match(const evhKEY& key)
{
    // If it is the same event type (i.e. command event)
    if (evhKEY::IsSame(key) == mcsTRUE)
    {
        if ((strcmp(((evhCMD_KEY *) & key)->_command, mcsNULL_CMD) == 0) ||
                (strcmp(_command, ((evhCMD_KEY *) & key)->_command) == 0))
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
evhCMD_KEY & evhCMD_KEY::SetCommand(const mcsCMD command)
{
    strncpy(_command, command, mcsLEN16 - 1);

    return *this;
}

/**
 * Get command name.
 *
 * \return command name
 */
char *evhCMD_KEY::GetCommand() const
{
    return ((char *) _command);
}

/**
 * Set command definition file
 *
 * \return reference to the object itself
 *
 * \warning If command definition file name length exceeds 64 characters, it
 * is truncated
 */
evhCMD_KEY & evhCMD_KEY::SetCdf(const char *cdf)
{
    if (cdf != NULL)
    {
        strncpy(_cdf, cdf, mcsLEN64 - 1);
    }
    else
    {
        memset(_cdf, '\0', mcsLEN64);
    }
    return *this;
}

/**
 * Get command definition file name.
 *
 * \return command definition file name
 */
char *evhCMD_KEY::GetCdf() const
{
    return ((char *) _cdf);
}
/*___oOo___*/
