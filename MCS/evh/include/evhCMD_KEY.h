#ifndef evhCMD_KEY_H
#define evhCMD_KEY_H
/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: evhCMD_KEY.h,v 1.5 2005-01-29 15:16:35 gzins Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * gzins     27-Sep-2004  Created
 * gzins     22-Dec-2004  Added command definition file (CDF)
 *
 ******************************************************************************/

/**
 * \file
 * Declaration of the evhCMD_KEY class.
 */


#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif

#include "mcs.h"
#include "evhKEY.h"

/*
 * Class declaration
 */

/**
 * Class to hold command event id keys.
 * 
 * This class derives from evhKEY, and is used to attach a callback to a
 * command type event. It is just characterized by the name of the command to
 * which the callback has to associated; i.e. callback which has to be
 * executed when command is received.
 *
 * If the command is set to mcsNULL_CMD, the callback will be executed
 * for each command which has not a identified callback.
 */
class evhCMD_KEY : public evhKEY
{
public:
    evhCMD_KEY(const mcsCMD command = "", const char *cdf = NULL);
    evhCMD_KEY (const evhCMD_KEY&);
    virtual ~evhCMD_KEY();

    evhCMD_KEY& operator=(const evhCMD_KEY&);

    virtual mcsLOGICAL IsSame(const evhKEY& key);
    virtual mcsLOGICAL Match(const evhKEY& key);

    virtual evhCMD_KEY &SetCommand(const mcsCMD command);
    virtual char       *GetCommand() const;

    virtual evhCMD_KEY &SetCdf(const char *cdf);
    virtual char       *GetCdf() const;

protected:

private:
    mcsCMD      _command;  /** Command name */
    mcsSTRING64 _cdf;      /** Command definition file */
};

#endif /*!evhCMD_KEY_H*/

/*___oOo___*/
