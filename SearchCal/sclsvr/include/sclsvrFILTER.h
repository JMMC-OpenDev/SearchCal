#ifndef sclsvrFILTER_H
#define sclsvrFILTER_H
/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: sclsvrFILTER.h,v 1.1 2005-06-01 14:18:54 scetre Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 ******************************************************************************/

/**
 * \file
 * Declaration of sclsvrFILTER class.
 */

#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif


/*
 * MCS header
 */
#include "mcs.h"
#include "sclsvrCALIBRATOR_LIST.h"

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
class sclsvrFILTER
{

public:
    // Class constructor
    sclsvrFILTER();

    // Class destructor
    virtual ~sclsvrFILTER();

    virtual char * GetName();
    virtual mcsLOGICAL IsEnable();
    virtual mcsCOMPL_STAT Enable();
    virtual mcsCOMPL_STAT Disable();

    virtual mcsCOMPL_STAT Apply(sclsvrCALIBRATOR_LIST *list) = 0;

protected:
    mcsSTRING32 _name;
    
private:
    // Declaration of copy constructor and assignment operator as private
    // methods, in order to hide them from the users.
    sclsvrFILTER(const sclsvrFILTER&);
    sclsvrFILTER& operator=(const sclsvrFILTER&);

    mcsLOGICAL _isEnable;
};

#endif /*!sclsvrFILTER_H*/

/*___oOo___*/
