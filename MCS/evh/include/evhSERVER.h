#ifndef evhSERVER_H
#define evhSERVER_H
/*******************************************************************************
* JMMC project
*
* "@(#) $Id: evhSERVER.h,v 1.1 2004-11-17 10:28:48 gzins Exp $"
*
* who       when         what
* --------  -----------  -------------------------------------------------------
* gzins     09-Nov-2004  Created
*
*
*******************************************************************************/

/**
 * \file
 * evhSERVER class declaration.
 */


#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif

#include "evhTASK.h"
#include "evhHANDLER.h"

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
class evhSERVER : public evhTASK, public evhHANDLER
{
public:
    evhSERVER();
    virtual ~evhSERVER();

    // Init method
    virtual mcsCOMPL_STAT Init();

    // Command callbacks
    virtual evhCB_COMPL_STAT VersionCB(msgMESSAGE &msg, void*);

protected:

private:
    // Declaration of copy constructor and assignment operator as private
    // methods, in order to hide them from the users.
    evhSERVER& operator=(const evhSERVER&);
    evhSERVER (const evhSERVER&);
};

#endif /*!evhSERVER_H*/

/*___oOo___*/
