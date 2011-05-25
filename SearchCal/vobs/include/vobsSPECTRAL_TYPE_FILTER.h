#ifndef vobsSPECTRAL_TYPE_FILTER_H
#define vobsSPECTRAL_TYPE_FILTER_H
/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
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
    vobsSPECTRAL_TYPE_FILTER(const char* filterId);

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
