#ifndef vobsORIGIN_FILTER_H
#define vobsORIGIN_FILTER_H
/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: vobsORIGIN_FILTER.h,v 1.1 2005-11-15 14:54:33 scetre Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
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
#include "vobsFILTER.h"
#include "vobsSTAR_LIST.h"

#define vobsORIGIN_FILTER_NAME "Filter by Origin"


/*
 * Class declaration
 */

/**
 * Origin filter class.
 * 
 * This class is a vobsFILTER object.
 *
 * @sa vobsFILTER.cpp
 * @sa vobsSTAR_LIST.cpp
 * 
 */
class vobsORIGIN_FILTER : public vobsFILTER
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
