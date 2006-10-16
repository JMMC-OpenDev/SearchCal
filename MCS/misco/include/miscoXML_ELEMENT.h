#ifndef miscoXML_ELEMENT_H
#define miscoXML_ELEMENT_H
/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: miscoXML_ELEMENT.h,v 1.1 2006-10-16 07:34:19 mella Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 ******************************************************************************/

/**
 * @file
 * Declaration of miscoXML_ELEMENT class.
 */

#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif


/*
 * MCS header
 */
#include "mcs.h"


/*
 * Class declaration
 */

/**
 * Brief description of the class, which ends at this dot.
 * 
 * OPTIONAL detailed description of the class follows here.
 *
 * @usedfiles
 * OPTIONAL. If files are used, for each one, name, and usage description.
 * @filename fileName1 :  usage description of fileName1
 * @filename fileName2 :  usage description of fileName2
 *
 * @env
 * OPTIONAL. If needed, environmental variables accessed by the class. For
 * each variable, name, and usage description, as below.
 * @envvar envVar1 :  usage description of envVar1
 * @envvar envVar2 :  usage description of envVar2
 * 
 * @warning OPTIONAL. Warning if any (software requirements, ...)
 *
 * @ex
 * OPTIONAL. Code example if needed
 * \n Brief example description.
 * @code
 * Insert your code example here
 * @endcode
 *
 * @sa OPTIONAL. See also section, in which you can refer other documented
 * entities. Doxygen will create the link automatically.
 * @sa modcppMain.C
 * 
 * @bug OPTIONAL. Bugs list if it exists.
 * @bug For example, description of the first bug
 * @bug For example, description of the second bug
 * 
 * @todo OPTIONAL. Things to forsee list, if needed. For example, 
 * @todo add other methods, dealing with operations.
 * 
 */
class miscoXML_ELEMENT
{

public:
    // Class constructor
    miscoXML_ELEMENT(string name);

    // Class destructor
    virtual ~miscoXML_ELEMENT();

    virtual mcsCOMPL_STAT AddElement(miscoXML_ELEMENT * e);
    virtual mcsCOMPL_STAT AddAttribute(string attributeName,
                                       string attributeValue);
    virtual string ToString();    

protected:
    
private:
    // Declaration of copy constructor and assignment operator as private
    // methods, in order to hide them from the users.
    miscoXML_ELEMENT(const miscoXML_ELEMENT&);
    miscoXML_ELEMENT& operator=(const miscoXML_ELEMENT&);
};

#endif /*!miscoXML_ELEMENT_H*/

/*___oOo___*/
