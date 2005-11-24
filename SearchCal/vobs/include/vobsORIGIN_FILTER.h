#ifndef vobsORIGIN_FILTER_H
#define vobsORIGIN_FILTER_H
/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: vobsORIGIN_FILTER.h,v 1.2 2005-11-24 08:13:50 scetre Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.1  2005/11/15 14:54:33  scetre
 * Added filter on origin and B-V
 *
 ******************************************************************************/

/**
 * \file
 * Declaration of vobsORIGIN_FILTER class.
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
#include "vobsBASE_FILTER.h"
#include "vobsSTAR_LIST.h"

#define vobsORIGIN_FILTER_NAME "Filter by Origin"


/*
 * Class declaration
 */

/**
 * Origin filter class.
 * 
 * This class is a vobsBASE_FILTER object.
 *
 * @sa vobsBASE_FILTER.cpp
 * @sa vobsSTAR_LIST.cpp
 * 
 */
class vobsORIGIN_FILTER : public vobsBASE_FILTER
{

public:
    // Class constructor
    vobsORIGIN_FILTER();

    // Class destructor
    virtual ~vobsORIGIN_FILTER();

    virtual mcsCOMPL_STAT SetOriginName(mcsSTRING32 origin,
                                        mcsSTRING32 ucd);

    virtual mcsCOMPL_STAT Apply(vobsSTAR_LIST *list);    

protected:
    
private:
    // Declaration of copy constructor and assignment operator as private
    // methods, in order to hide them from the users.
    vobsORIGIN_FILTER(const vobsORIGIN_FILTER&);
    vobsORIGIN_FILTER& operator=(const vobsORIGIN_FILTER&);

    mcsSTRING32 _origin;
    mcsSTRING32 _ucd;
};

#endif /*!vobsORIGIN_FILTER_H*/

/*___oOo___*/
