#ifndef evhHANDLER_H
#define evhHANDLER_H
/*******************************************************************************
* JMMC project
*
* "@(#) $Id: evhHANDLER.h,v 1.5 2005-01-07 17:48:17 gzins Exp $"
*
* who       when         what
* --------  -----------  -------------------------------------------------------
* gzins     22-Sep-2004  Created based on VLT SW
* gzins     23-Nov-2004  Used new msg C++ library.
* gzins     22-Dec-2004  Added GetHelp()
* gzins     07-Jan-2005  Renamed GetHelp() to HandeHelpCmd()
*                        Added AddCallback() and Run() for command reply
*                        Declared Run() and Find as protected
*                        Added evhMainHandler global variable
*******************************************************************************/

/**
 * \file
 * Definition of the evhHANDLER class
 */

#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif

#include "fnd.h"
#include "msg.h"
#include "evhKEY.h"
#include "evhCMD_KEY.h"
#include "evhCMD_REPLY_KEY.h"
#include "evhCMD_CALLBACK.h"
#include "evhIOSTREAM_KEY.h"
#include "evhIOSTREAM_CALLBACK.h"
#include "evhCALLBACK_LIST.h"

/*
 * Class declaration
 */

/**
 * Event handler.
 *
 * This class is the core of the Event Handler. It is an event dispatcher
 * that receives all the events directed to the application (coming from the
 * outside world or generated internally), parses them and searches in an
 * internal dictionary for a list of functions (callbacks) that must be called
 * to handle that particular event. If such a list of functions exist, they
 * are called in sequence. Upon return from the last callback, the dispatcher
 * is ready to manage other events.
 *
 * The callback lists associated with events are dynamic and evhHANDLER
 * provides methods to add and delete elements from the list. 
 *
 */
class evhHANDLER : public fndOBJECT
{
public:
    evhHANDLER();
    virtual ~evhHANDLER();

    virtual mcsCOMPL_STAT AddCallback(const evhCMD_KEY &key,
                                      evhCMD_CALLBACK &callback);
    virtual mcsCOMPL_STAT AddCallback(const evhCMD_REPLY_KEY &key,
                                      evhCMD_CALLBACK &callback);
    virtual mcsCOMPL_STAT AddCallback(const evhIOSTREAM_KEY &key,
                                      evhIOSTREAM_CALLBACK &callback);
    virtual mcsCOMPL_STAT MainLoop(msgMESSAGE *msg=NULL);

    virtual mcsCOMPL_STAT HandeHelpCmd(msgMESSAGE &msg);

protected:
    virtual evhKEY *Select();
    virtual evhCALLBACK_LIST *Find(const evhKEY &key);
    virtual mcsCOMPL_STAT Run(const evhCMD_KEY &key, msgMESSAGE &msg);
    virtual mcsCOMPL_STAT Run(const evhCMD_REPLY_KEY &key, msgMESSAGE &msg);
    virtual mcsCOMPL_STAT Run(const evhIOSTREAM_KEY &key, int fd);

private:
    // Declaration of copy constructor and assignment operator as private
    // methods, in order to hide them from the users.
    evhHANDLER& operator=(const evhHANDLER&);
    evhHANDLER (const evhHANDLER&);

    std::list<std::pair<evhKEY*, evhCALLBACK_LIST *> >           _eventList;
    std::list<std::pair<evhKEY*, evhCALLBACK_LIST *> >::iterator _eventIterator;

    evhKEY           _msgEvent;
    evhIOSTREAM_KEY  _iostreamEvent;

    msgMANAGER_IF    _msgManager; // Interface to message manager
};

/* Standard event handler */
extern evhHANDLER *evhMainHandler;

#endif /*!evhHANDLER_H*/

/*___oOo___*/
