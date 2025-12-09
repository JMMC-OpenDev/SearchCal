#ifndef vobsFILTER_H
#define vobsFILTER_H
/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * @file
 * Declaration of vobsFILTER class.
 */

#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif


/*
 * MCS header
 */
#include "mcs.h"
#include "vobsSTAR_LIST.h"

/*
 * Class declaration
 */

/**
 * Filter class
 *
 * Class to filter out a list of stars; i.e. all stars which do not match the
 * filter conditions are removed from the list. 
 */
class vobsFILTER
{
public:
    // Class constructor
    vobsFILTER(const char* filterId);

    // Class destructor
    virtual ~vobsFILTER();

    virtual mcsLOGICAL IsEnabled(void) const;
    virtual mcsCOMPL_STAT Enable(void);
    virtual mcsCOMPL_STAT Disable(void);

    virtual const char* GetId(void) const;

    virtual mcsCOMPL_STAT Apply(vobsSTAR_LIST* list) = 0;
    
   /**
    * Dump the filter as XML
    *
    * @param buffer buffer to append into
    * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned
    */
    virtual mcsCOMPL_STAT DumpAsXML(miscoDYN_BUF& buffer) const;
    
protected:

private:
    // Declaration of copy constructor and assignment operator as private
    // methods, in order to hide them from the users.
    vobsFILTER(const vobsFILTER&);
    vobsFILTER& operator=(const vobsFILTER&);

    const char* _id;
    mcsLOGICAL _isEnable;
};

#endif /*!vobsFILTER_H*/

/*___oOo___*/
