#ifndef msgPROCESS_LIST_H
#define msgPROCESS_LIST_H
/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: msgPROCESS_LIST.h,v 1.3 2005-01-24 15:39:54 gzins Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * gzins     06-Dec-2004  Created
 * gzins     08-Dec-2004  Added descriptor argument to GetProcess()
 *
 ******************************************************************************/

/**
 * \file
 * msgPROCESS_LIST class declaration.
 */

#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif

/*
 * System header files
 */
#include<list>

/*
 * Local header files
 */
#include "msgPROCESS.h"

/*
 * Class declaration
 */

/**
 * List of processes connected to the message services
 * 
 * msgPROCESS_LIST is a class to manage a list of processes (msgPROCESS
 * instances). It provides methods to perform actions to the list such
 * adding/removing elements in list, getting list size and clearing list. 
 */
class msgPROCESS_LIST
{
public:
    // Class constructor
    msgPROCESS_LIST();

    // Class destructor
    virtual ~msgPROCESS_LIST();

    virtual mcsLOGICAL    IsEmpty(void);
    virtual mcsCOMPL_STAT Clear(void);
    virtual mcsCOMPL_STAT AddAtTail(msgPROCESS *process);
    virtual mcsCOMPL_STAT Remove(mcsINT32 sd);
    virtual mcsUINT32     Size(void);

    virtual msgPROCESS    *GetNextProcess(mcsLOGICAL init = mcsFALSE);
    virtual msgPROCESS    *GetProcess(char *name, mcsINT32 sd=-1);

protected:
    // List of clients
    std::list<msgPROCESS *>           _processList;
    std::list<msgPROCESS *>::iterator _processIterator;

private:
    // Declaration of copy constructor and assignment operator as private
    // methods, in order to hide them from the users.
     msgPROCESS_LIST(const msgPROCESS_LIST&);
     msgPROCESS_LIST& operator=(const msgPROCESS_LIST&);
};

#endif /*!msgPROCESS_LIST_H*/

/*___oOo___*/
