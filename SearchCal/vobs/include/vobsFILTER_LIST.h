#ifndef vobsFILTER_LIST_H
#define vobsFILTER_LIST_H
/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: vobsFILTER_LIST.h,v 1.8 2005-12-13 16:30:33 lafrasse Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.7  2005/11/29 13:48:33  gzins
 * Associated name to filter added in the list; used to retrieve it later
 *
 * Revision 1.6  2005/11/29 10:28:18  gzins
 * Moved vobsBASE_FILTER to vobsFILTER
 *
 * Revision 1.5  2005/11/24 08:13:50  scetre
 * Changed mother class of filter from vobsFILTER to vobsFILTER
 *
 * Revision 1.4  2005/11/16 10:47:54  scetre
 * Updated documentation
 *
 * Revision 1.3  2005/11/16 10:47:54  scetre
 * Updated documentation
 *
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
    vobsFILTER_LIST(const char* filterId);

    // Class destructor
    virtual ~vobsFILTER_LIST();

    virtual mcsCOMPL_STAT Add(vobsFILTER *filter, string name);
    virtual mcsCOMPL_STAT Reset(void);
    virtual mcsUINT32 Size(void);
    virtual vobsFILTER *GetFilter(string name);
    virtual mcsCOMPL_STAT Apply(vobsSTAR_LIST *list);
protected:
    
private:
    // Declaration of copy constructor and assignment operator as private
    // methods, in order to hide them from the users.
    vobsFILTER_LIST(const vobsFILTER_LIST&);
    vobsFILTER_LIST& operator=(const vobsFILTER_LIST&);

    map<string, vobsFILTER *> _filterList;
};

#endif /*!vobsFILTER_LIST_H*/

/*___oOo___*/
