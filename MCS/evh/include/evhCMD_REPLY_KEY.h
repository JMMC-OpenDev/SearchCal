#ifndef evhCMD_REPLY_KEY_H
#define evhCMD_REPLY_KEY_H
/*******************************************************************************
* JMMC project
*
* "@(#) $Id: evhCMD_REPLY_KEY.h,v 1.1 2005-01-07 17:43:44 gzins Exp $"
*
* who       when         what
* --------  -----------  -------------------------------------------------------
* gzins     04-Jan-2005  Created
*
*
*******************************************************************************/

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
#include "evhKEY.h"

/*
 * Class declaration
 */

/**
 * Class to hold command reply event id keys.
 * 
 * This class derives from evhKEY, and is used to attach a callback to a
 * command reply type event. It is just characterized by the name and id of the
 * command reply to which the callback has to associated; i.e. callback which
 * has to be executed when command is received.
 *
 * If the command is set to mcsNULL_CMD, the callback will be executed
 * for each reply which has not a identified callback.
 */
class evhCMD_REPLY_KEY : public evhKEY
{
public:
    // Class constructors
    evhCMD_REPLY_KEY(const mcsCMD command = "", const mcsINT32 commandId = 0);
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
protected:
    
private:
    mcsCMD   _command;   /** Command name */
    mcsINT32 _commandId; /** Command Id */
};

#endif /*!evhCMD_REPLY_KEY_H*/

/*___oOo___*/
