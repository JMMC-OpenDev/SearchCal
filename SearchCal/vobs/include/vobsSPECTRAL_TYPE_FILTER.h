#ifndef vobsSPECTRAL_TYPE_FILTER_H
#define vobsSPECTRAL_TYPE_FILTER_H
/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: vobsSPECTRAL_TYPE_FILTER.h,v 1.3 2005-11-29 10:30:38 gzins Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.2  2005/11/24 08:13:50  scetre
 * Changed mother class of filter from vobsFILTER to vobsFILTER
 *
 * Revision 1.1  2005/06/20 11:31:53  scetre
 * Added filter class
 *
 * Revision 1.2  2005/06/07 12:35:59  scetre
 * Updated documentation
 *
 * Revision 1.1  2005/06/01 14:18:54  scetre
 * Added filters and filter list objects.
 * Changed logExtDbg to logTrace
 *
 ******************************************************************************/

/**
 * @file
 * Declaration of vobsSPECTRAL_TYPE_FILTER class.
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
#include "vobsSTAR_LIST.h"


#define vobsSPECTRAL_TYPE_FILTER_NAME "Filter by Spectral type"

/*
 * Class declaration
 */

/**
 * Spectral type filter class.
 * 
 * This class is a vobsFILTER object.
 *
 * @sa vobsFILTER.cpp
 * @sa vobsSTAR_LIST.cpp
 * 
 */
class vobsSPECTRAL_TYPE_FILTER : public vobsFILTER
{

public:
    // Class constructor
    vobsSPECTRAL_TYPE_FILTER();

    // Class destructor
    virtual ~vobsSPECTRAL_TYPE_FILTER();

    virtual mcsCOMPL_STAT SetSpectralType(std::list<char *> tempClassList);

    virtual std::list<char *> * GetSpectralType();

    virtual mcsCOMPL_STAT Apply(vobsSTAR_LIST *list);

protected:
    
private:
    // Declaration of copy constructor and assignment operator as private
    // methods, in order to hide them from the users.
    vobsSPECTRAL_TYPE_FILTER(const vobsSPECTRAL_TYPE_FILTER&);
    vobsSPECTRAL_TYPE_FILTER& operator=(const vobsSPECTRAL_TYPE_FILTER&);

    std::list<char *> _tempClassList;
};

#endif /*!vobsSPECTRAL_TYPE_FILTER_H*/

/*___oOo___*/
