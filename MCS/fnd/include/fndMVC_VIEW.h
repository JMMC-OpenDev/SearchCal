#ifndef fndMVC_VIEW_H
#define fndMVC_VIEW_H
/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: fndMVC_VIEW.h,v 1.1 2005-06-13 10:24:06 scetre Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 ******************************************************************************/

/**
 * \file
 * Declaration of fndMVC_VIEW class.
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

/*
 * Class declaration
 */

/**
 * Basic definition of the view class.
 * 
 * This class implement the Model-View-Controller concept.
 *
 * \n
 * \warning This class do nothing. In order to use it it is necessary to
 * re-implement the Update() method.
 *
 */
class fndMVC_VIEW
{

public:
    // Class constructor
    fndMVC_VIEW();

    // Class destructor
    virtual ~fndMVC_VIEW();
    
    virtual mcsCOMPL_STAT Update()=0;

protected:
    
private:
    // Declaration of copy constructor and assignment operator as private
    // methods, in order to hide them from the users.
    fndMVC_VIEW(const fndMVC_VIEW&);
    fndMVC_VIEW& operator=(const fndMVC_VIEW&);

};

#endif /*!fndMVC_VIEW_H*/

/*___oOo___*/
