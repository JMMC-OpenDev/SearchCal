#ifndef vobsLUMINOSITY_FILTER_H
#define vobsLUMINOSITY_FILTER_H
/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * @file
 * Declaration of vobsLUMINOSITY_FILTER class.
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

#define vobsNB_LUMINOSITY_CLASS 6
/*
 * Class declaration
 */

/**
 * luminosity filter class.
 * 
 * This class is a vobsFILTER object.
 *
 * @sa vobsFILTER.cpp
 * @sa vobsSTAR_LIST.cpp
 * 
 */
class vobsLUMINOSITY_FILTER : public vobsFILTER
{

public:
    // Class constructor
    vobsLUMINOSITY_FILTER(const char* filterId);

    // Class destructor
    virtual ~vobsLUMINOSITY_FILTER();

    virtual mcsCOMPL_STAT
        SetLuminosity(std::list<char *> lumClassList);

    virtual std::list<char *> * GetLuminosity();

    virtual mcsCOMPL_STAT Apply(vobsSTAR_LIST *list);
   
protected:
    
private:
    // Declaration of copy constructor and assignment operator as private
    // methods, in order to hide them from the users.
    vobsLUMINOSITY_FILTER(const vobsLUMINOSITY_FILTER&);
    vobsLUMINOSITY_FILTER& operator=(const vobsLUMINOSITY_FILTER&);

    std::list<char *> _luminosityClassList;
};

#endif /*!vobsLUMINOSITY_FILTER_H*/

/*___oOo___*/
