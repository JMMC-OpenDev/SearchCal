#ifndef msgPROCESS_LIST_H
#define msgPROCESS_LIST_H
/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * \file
 * Declaration of msgPROCESS_LIST class 
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
 * List of all the processes connected to the \<msgManager\>.
 * 
 * Manage the list of all the connected processes (msgPROCESS instances). It
 * provides methods to perform actions on the list, such as :
 * \li getting the number of processes in the list;
 * \li adding new processes;
 * \li searching some processes;
 * \li removing some processes;
 * \li clearing all the list. 
 */
class msgPROCESS_LIST
{
public:
    // Class constructor
    msgPROCESS_LIST();

    // Class destructor
    virtual ~msgPROCESS_LIST();

    virtual mcsLOGICAL    IsEmpty(void);
    virtual mcsUINT32     Size(void);

    virtual mcsCOMPL_STAT AddAtTail(msgPROCESS *process);

    virtual msgPROCESS    *GetNextProcess(const mcsLOGICAL init = mcsFALSE);
    virtual msgPROCESS    *GetProcess(const char *name, const mcsINT32 sd=-1);

    virtual mcsCOMPL_STAT Remove(mcsINT32 sd);
    virtual mcsCOMPL_STAT Clear(void);

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
