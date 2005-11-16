#ifndef vobsFILTER_LIST_H
#define vobsFILTER_LIST_H
/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: vobsFILTER_LIST.h,v 1.3 2005-11-16 10:47:54 scetre Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.2  2005/11/15 14:57:56  scetre
 * Added new scenario structure
 * Added possibility to query merand and borde as primary catalog
 *
 * Revision 1.1  2005/06/20 11:31:53  scetre
 * Added filter class
 *
 * Revision 1.1  2005/06/01 14:18:54  scetre
 * Added filters and filter list objects.
 * Changed logExtDbg to logTrace
 *
 ******************************************************************************/

/**
 * \file
 * Declaration of vobsFILTER_LIST class.
 */

#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif


/*
 * MCS header
 */
#include "mcs.h"

/*
 * Local header
 */
#include "vobsFILTER.h"

/*
 * Class declaration
 */

/**
 * Brief description of the class, which ends at this dot.
 */
class vobsFILTER_LIST : public vobsFILTER
{

public:
    // Class constructor
    vobsFILTER_LIST();

    // Class destructor
    virtual ~vobsFILTER_LIST();

    virtual mcsCOMPL_STAT Add(vobsFILTER *filter);
    virtual mcsCOMPL_STAT Reset(void);
    virtual mcsUINT32 Size(void);
    virtual vobsFILTER *GetNextFilter(mcsLOGICAL init = mcsFALSE);
    virtual vobsFILTER *GetFilter(mcsSTRING32 name);
    virtual mcsCOMPL_STAT Apply(vobsSTAR_LIST *list);
protected:
    
private:
    // Declaration of copy constructor and assignment operator as private
    // methods, in order to hide them from the users.
    vobsFILTER_LIST(const vobsFILTER_LIST&);
    vobsFILTER_LIST& operator=(const vobsFILTER_LIST&);

    std::list<vobsFILTER *> _filterList;
    std::list<vobsFILTER *>::iterator _filterlistIterator;
};

#endif /*!vobsFILTER_LIST_H*/

/*___oOo___*/
