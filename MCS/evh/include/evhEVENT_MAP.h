#ifndef evhEVENT_MAP_H
#define evhEVENT_MAP_H
/*******************************************************************************
* JMMC project
*
* "@(#) $Id: evhEVENT_MAP.h,v 1.1 2004-10-18 09:40:10 gzins Exp $"
*
* who       when         what
* --------  -----------  -------------------------------------------------------
* gzins     27-Sep-2004  Created
*
*
*******************************************************************************/

/**
 * \file
 * evhEVENT_MAP class declaration.
 */

#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif

#include "fnd.h"
#include "evhKEY.h"
#include "evhCMD_KEY.h"
#include "evhCMD_CALLBACK.h"
#include "evhIOSTREAM_KEY.h"
#include "evhIOSTREAM_CALLBACK.h"
#include "evhCALLBACK_LIST.h"

/*
 * Class declaration
 */

/**
 * Brief description of the class, which ends at this dot.
 * 
 * OPTIONAL detailed description of the class follows here.
 *
 * \usedfiles
 * OPTIONAL. If files are used, for each one, name, and usage description.
 * \filename fileName1 :  usage description of fileName1
 * \filename fileName2 :  usage description of fileName2
 *
 * \n
 * \env
 * OPTIONAL. If needed, environmental variables accessed by the class. For
 * each variable, name, and usage description, as below.
 * \envvar envVar1 :  usage description of envVar1
 * \envvar envVar2 :  usage description of envVar2
 * 
 * \n
 * \warning OPTIONAL. Warning if any (software requirements, ...)
 *
 * \n
 * \ex
 * OPTIONAL. Code example if needed
 * \n Brief example description.
 * \code
 * Insert your code example here
 * \endcode
 *
 * \sa OPTIONAL. See also section, in which you can refer other documented
 * entities. Doxygen will create the link automatically.
 * \sa modcppMain.C
 * 
 * \bug OPTIONAL. Bugs list if it exists.
 * \bug For example, description of the first bug
 * \bug For example, description of the second bug
 * 
 * \todo OPTIONAL. Things to forsee list, if needed. For example, 
 * \todo add other methods, dealing with operations.
 * 
 */
class evhEVENT_MAP : public fndOBJECT
{
public:
    evhEVENT_MAP();
    virtual ~evhEVENT_MAP();

    virtual mcsCOMPL_STAT AddCallback(const evhCMD_KEY &key,
                                      evhCMD_CALLBACK &callback);
    virtual mcsCOMPL_STAT AddCallback(const evhIOSTREAM_KEY &key,
                                      evhIOSTREAM_CALLBACK &callback);
    virtual mcsCOMPL_STAT Run(const evhCMD_KEY &key, msgMESSAGE &msg);
    virtual mcsCOMPL_STAT Run(const evhIOSTREAM_KEY &key, int fd);
    
protected:

private:
    // Declaration of copy constructor and assignment operator as private
    // methods, in order to hide them from the users.
    evhEVENT_MAP& operator=(const evhEVENT_MAP&);
    evhEVENT_MAP (const evhEVENT_MAP&);

    std::list<std::pair<evhKEY *, evhCALLBACK_LIST *>>           _eventList;
    std::list<std::pair<evhKEY *, evhCALLBACK_LIST *>>::iterator _eventIterator;
};

#endif /*!evhEVENT_MAP_H*/
/*___oOo___*/
