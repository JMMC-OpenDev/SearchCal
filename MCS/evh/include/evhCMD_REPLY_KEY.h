#ifndef evhCMD_REPLY_KEY_H
#define evhCMD_REPLY_KEY_H
/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: evhCMD_REPLY_KEY.h,v 1.2 2005-01-26 18:06:42 gzins Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * gzins     04-Jan-2005  Created
 *
 ******************************************************************************/

/**
 * \file
 * evhCMD_REPLY_KEY class declaration.
 */

#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif


/*
 * MCS header
 */
#include "mcs.h"
#include "msg.h"
#include "evhKEY.h"

/*
 * Class declaration
 */

/**
 * Class to hold command reply event id keys.
 * 
 * This class derives from evhKEY, and is used to attach a callback to a
 * command reply type event. It is characterized by the name and id of the
 * command reply to which the callback has to associated; i.e. callback which
 * has to be executed when command reply is received or when the timeout is
 * expired.
 *
 * If the command is set to mcsNULL_CMD, the callback will be executed
 * for each reply which has not a identified callback.
 */
class evhCMD_REPLY_KEY : public evhKEY
{
public:
    // Class constructors
    evhCMD_REPLY_KEY(const mcsCMD command = "", const mcsINT32 commandId = 0,
                     const mcsINT32 timeout = msgWAIT_FOREVER);
    evhCMD_REPLY_KEY(const evhCMD_REPLY_KEY&);

    // Class destructor
    virtual ~evhCMD_REPLY_KEY();

    evhCMD_REPLY_KEY& operator=(const evhCMD_REPLY_KEY&);

    virtual mcsLOGICAL IsSame(const evhKEY& key);
    virtual mcsLOGICAL Match(const evhKEY& key);

    virtual evhCMD_REPLY_KEY &SetCommand(const mcsCMD command);
    virtual char             *GetCommand() const;

    virtual evhCMD_REPLY_KEY &SetCommandId(const mcsINT32 commandId);
    virtual mcsINT32         GetCommandId() const;

    virtual evhCMD_REPLY_KEY &SetTimeout(const mcsINT32 timeout);
    virtual mcsINT32         GetTimeout() const;
    virtual mcsCOMPL_STAT    GetTimeoutExpDate(struct timeval *expDate) const;
protected:
    
private:
    mcsCMD   _command;       /** Command name */
    mcsINT32 _commandId;     /** Command Id */
    mcsINT32 _timeout;       /** Timeout in ms */
    struct timeval _expirationDate; /** Expiration time of timeout */
};

#endif /*!evhCMD_REPLY_KEY_H*/

/*___oOo___*/
