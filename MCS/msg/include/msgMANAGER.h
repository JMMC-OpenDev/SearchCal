#ifndef msgMANAGER_H
#define msgMANAGER_H
/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: msgMANAGER.h,v 1.3 2005-01-24 15:39:54 gzins Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * gzins     06-Dec-2004  Created
 * gzins     15-Dec-2004  Set mcsTRUE as default value of lastReply parameter
 *                        of SendReply method
 *
 ******************************************************************************/

/**
 * \file
 * msgMANAGER class declaration.
 */

#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif

/*
 * MCS headers
 */
#include "mcs.h"

/*
 * Local headers
 */
#include "msgSOCKET_SERVER.h"
#include "msgPROCESS_LIST.h"

/*
 * Class declaration
 */

/**
 * MCS message manager class 
 * 
 * This msgMANAGER class is the communication server allowing message exchange
 * between processes. Each process connected to this server can send message to
 * the other connected processes.
 */
class msgMANAGER
{
public:
    // Class constructor
    msgMANAGER();

    // Class destructor
    virtual ~msgMANAGER();

     // Initialization
    virtual mcsCOMPL_STAT Init(int argc, char *argv[]);

    // Main loop
    virtual mcsCOMPL_STAT MainLoop();
    
protected:
    virtual mcsCOMPL_STAT ParseOptions(mcsINT32 argc, char *argv[]);
    virtual mcsCOMPL_STAT Usage(void);
    virtual const char *GetSwVersion(void);

    // Accept connection
    virtual mcsCOMPL_STAT SetConnection();

    virtual mcsCOMPL_STAT Forward (msgMESSAGE &msg);
    virtual mcsCOMPL_STAT SendReply (msgMESSAGE &msg,
                                     mcsLOGICAL lastReply=mcsTRUE,
                                     msgPROCESS *sender=NULL);
    virtual mcsCOMPL_STAT HandleCmd (msgMESSAGE &msg);
private:
    // Declaration of copy constructor and assignment operator as private
    // methods, in order to hide them from the users.
    msgMANAGER(const msgMANAGER&);
    msgMANAGER& operator=(const msgMANAGER&);

    // Connection socket
    msgSOCKET_SERVER _connectionSocket;

    // List of connected processes
    msgPROCESS_LIST  _processList;
};

#endif /*!msgMANAGER_H*/

/*___oOo___*/
