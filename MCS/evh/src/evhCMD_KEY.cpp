/*******************************************************************************
* JMMC project
*
* "@(#) $Id: evhCMD_KEY.cpp,v 1.2 2004-12-22 08:56:18 gzins Exp $"
*
* who       when         what
* --------  -----------  -------------------------------------------------------
* gzins     27-Sep-2004  Created
* gzins     22-Dec-2004  Added command definition file (CDF)
*
*******************************************************************************/

/**
 * \file
 * evhCMD_KEY class definition.
 */

static char *rcsId="@(#) $Id: evhCMD_KEY.cpp,v 1.2 2004-12-22 08:56:18 gzins Exp $"; 
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
    logExtDbg("evhCMD_KEY::evhCMD_KEY()"); 
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
    logExtDbg("evhCMD_KEY::operator =()"); 

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
    logExtDbg("evhCMD_KEY::IsSame()");

    // If it is the same event type (i.e. command event)
    if (evhKEY::IsSame(key) == mcsTRUE)
    {
        if (strcmp(_command, ((evhCMD_KEY *)&key)->_command) == 0)
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
    logExtDbg("evhCMD_KEY::Match()");

    // If it is the same event type (i.e. command event)
    if (evhKEY::IsSame(key) == mcsTRUE)
    {
        if ((strcmp(((evhCMD_KEY *)&key)->_command, mcsNULL_CMD) == 0) ||
            (strcmp(_command, ((evhCMD_KEY *)&key)->_command) == 0))
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
    logExtDbg("evhCMD_KEY::SetCommand()");

    strncpy(_command, command, sizeof(mcsCMD));

    return *this;
}

/**
 * Get command name.
 *
 * \return command name
 */
char *evhCMD_KEY::GetCommand() const
{
    logExtDbg("evhCMD_KEY::GetCommand()");

    return ((char *)_command);
}

/**
 * Set command definition file 
 *
 * \return reference to the object itself
 *
 * \warning If command name length exceeds 64 characters, it is
 * truncated
 */
evhCMD_KEY & evhCMD_KEY::SetCdf(const char *cdf)
{
    logExtDbg("evhCMD_KEY::SetCdf()");

    if (cdf != NULL)
    {
        strncpy(_cdf, cdf, sizeof(mcsSTRING64));
    }
    else
    {
        memset(_cdf, '\0', sizeof(_cdf)); 
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
    logExtDbg("evhCMD_KEY::GetCdf()");

    return ((char *)_cdf);
}
/*___oOo___*/
