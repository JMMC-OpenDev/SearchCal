#ifndef vobsSCENARIO_ENTRY_H
#define vobsSCENARIO_ENTRY_H
/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: vobsSCENARIO_ENTRY.h,v 1.1 2005-01-27 13:42:54 scetre Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 ******************************************************************************/

/**
 * \file
 * Declaration of vobsSCENARIO_ENTRY class.
 */

#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif


/*
 * MCS header
 */
#include "mcs.h"

/*
 * Local Header
 */
#include "vobsCATALOG.h"
#include "vobsSTAR_LIST.h"
#include "vobsSTAR_COMP_CRITERIA_LIST.h"

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

/*
 * Enumeration type definition
 */
/**
 * vobsACTION_ID is an enumeration which allow correspondance between an id and
 * an ACTION.
 */
typedef enum
{
    NO_ACTION = -1,
    COPY,
    MERGE,
    UPDATE_ONLY
}vobsACTION;

class vobsSCENARIO_ENTRY
{

public:
    // Class constructor
    vobsSCENARIO_ENTRY();
    vobsSCENARIO_ENTRY(const vobsSCENARIO_ENTRY&);

    // Class destructor
    virtual ~vobsSCENARIO_ENTRY();
    
    vobsCATALOG                 *catalog;
    vobsSTAR_LIST               *listInput;
    vobsSTAR_LIST               *listOutput;
    vobsACTION                  action;
    vobsSTAR_COMP_CRITERIA_LIST criteriaList;
protected:
    
private:
    // Declaration of copy constructor as private
    // methods, in order to hide them from the users.
    vobsSCENARIO_ENTRY& operator=(const vobsSCENARIO_ENTRY&);

    
};

#endif /*!vobsSCENARIO_ENTRY_H*/

/*___oOo___*/
