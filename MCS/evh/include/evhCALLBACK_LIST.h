#ifndef evhCALLBACK_LIST_H
#define evhCALLBACK_LIST_H
/*******************************************************************************
* JMMC project
*
* "@(#) $Id: evhCALLBACK_LIST.h,v 1.1 2004-10-18 09:40:10 gzins Exp $"
*
* who       when         what
* --------  -----------  -------------------------------------------------------
* gzins     23-Sep-2004  Created from VLT SW.
*
*
*******************************************************************************/

/**
 * \file
 * Declaration of the evhCALLBACK_LIST class.
 */


#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif

#include <list>

#include "fnd.h"
#include "evhCALLBACK.h"

/*
 * Class declaration
 */

/**
 * List of callbacks.
 * 
 * This class is used to handle a list of callbacks, i.e. a list of instances
 * of objects of class evhCALLBACK, or subclasses.
 * It is internally used in the evhHANDLER class.
 */
class evhCALLBACK_LIST : public fndOBJECT
{
public:
    evhCALLBACK_LIST();
    virtual ~evhCALLBACK_LIST();

    virtual mcsLOGICAL    IsEmpty(void);
    virtual mcsCOMPL_STAT Clear(void);
    virtual mcsCOMPL_STAT AddAtTail(evhCALLBACK *callback);
    virtual mcsCOMPL_STAT Remove(evhCALLBACK *callback);
    virtual mcsUINT32     Size(void);

    virtual mcsCOMPL_STAT Run(const msgMESSAGE &msg);
    virtual mcsCOMPL_STAT Run(const int fd);

protected:

private:
    // Declaration of copy constructor and assignment operator as private
    // methods, in order to hide them from the users.
    evhCALLBACK_LIST& operator=(const evhCALLBACK_LIST&);
    evhCALLBACK_LIST (const evhCALLBACK_LIST&);

    // List of callbacks
    std::list<evhCALLBACK *>           _callbackList;
    std::list<evhCALLBACK *>::iterator _callbackIterator;
};


#endif /*!evhCALLBACK_LIST_H*/

/*___oOo___*/
