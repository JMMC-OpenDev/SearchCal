#ifndef evhINTERFACE_H
#define evhINTERFACE_H
/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * \file
 * evhINTERFACE class declaration.
 */

#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif


/*
 * MCS header
 */
#include "mcs.h"
#include "msg.h"

#include "evhCMD_CALLBACK.h"

/*
 * Class declaration
 */

/**
 * Interface class to process
 * 
 * The evhINTERFACE is interface to process. This class is used to send commands
 * to process and provides two kind of protocoles: synchronous and
 * asynchronous. The Send() method is used for the synchronous transfer i.e. the
 * method returns when the reply is received or when a timeout is expired. The
 * Forward() command is used for asynchronous transfer. It installs callback
 * which is called when either a reply, error or timeout are received.
 */
class evhINTERFACE : fndOBJECT
{

public:
    // Class constructor
    evhINTERFACE(const char *name, const char *procName, 
                 const mcsINT32 timeout=msgWAIT_FOREVER);

    // Class destructor
    virtual ~evhINTERFACE();

    // Send command
    virtual mcsCOMPL_STAT Send(const char *command,
                               const char *parameters,  
                               mcsINT32   timeout=-2);
    // Forward command
    virtual mcsCOMPL_STAT Forward(const char *command,
                                  const char *parameters,  
                                  evhCMD_CALLBACK &callback,
                                  mcsINT32   timeout=-2);
    virtual evhCB_COMPL_STAT ReplyCB(msgMESSAGE &msg, void*);

    // Get reply of the last sent command 
    virtual const char       *GetLastReply(void);

protected:
    
private:
    // Declaration of copy constructor and assignment operator as private
    // methods, in order to hide them from the users.
    evhINTERFACE(const evhINTERFACE&);
    evhINTERFACE& operator=(const evhINTERFACE&);

    string        _name;       /** Interface name. */
    mcsSTRING32   _procName;   /** Process name */
    mcsSTRING32   _envName;    /** Environment name */
    mcsINT32      _timeout;    /** Default time-out */

    msgMANAGER_IF _msgManager; /** Interface to msgManager process */

    msgMESSAGE    _msg;        /** Used to receive message */

};

#endif /*!evhINTERFACE_H*/

/*___oOo___*/
