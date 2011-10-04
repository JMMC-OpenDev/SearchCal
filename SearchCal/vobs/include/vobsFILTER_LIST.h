#ifndef vobsFILTER_LIST_H
#define vobsFILTER_LIST_H
/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * \file
 * Declaration of vobsFILTER_LIST class.
 */

#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif

using namespace std;
#include <map>

/*
 * MCS header
 */
#include "mcs.h"

/*
 * Local header
 */
#include "vobsFILTER.h"

/*
 * const char* comparator used by map<const char*, ...> defined in vobsSTAR.h
 */
struct constStringComparator;

/* filter map type using char* keys and custom comparator functor */
typedef std::map<const char*, vobsFILTER*, constStringComparator> FilterList;

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

    virtual mcsCOMPL_STAT Add(vobsFILTER* filter, const char* name);
    virtual mcsCOMPL_STAT Reset(void);
    virtual mcsUINT32 Size(void) const;
    
    virtual vobsFILTER* GetFilter(const char* name);
    virtual mcsCOMPL_STAT Apply(vobsSTAR_LIST* list);
protected:
    
private:
    // Declaration of copy constructor and assignment operator as private
    // methods, in order to hide them from the users.
    vobsFILTER_LIST(const vobsFILTER_LIST&);
    vobsFILTER_LIST& operator=(const vobsFILTER_LIST&);

    FilterList _filterList;
};

#endif /*!vobsFILTER_LIST_H*/

/*___oOo___*/
