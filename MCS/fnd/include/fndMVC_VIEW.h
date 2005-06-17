#ifndef fndMVC_VIEW_H
#define fndMVC_VIEW_H
/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: fndMVC_VIEW.h,v 1.2 2005-06-17 08:29:00 gzins Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.1  2005/06/13 10:24:06  scetre
 * Implementation of MVC base class
 *
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
 * View class in the model-view paradigm.
 *
 * It has to be subclassed to create an object which wants to be informed of
 * a model object changes (see fndMVC_MODEL). The subclass has to implement the
 * Update() method which is called whenever the model (observed object) is
 * changed. 
 *
 * @note This class has been strongly influenced by java.util.Observer
 */
class fndMVC_VIEW
{

public:
    // Class constructor
    fndMVC_VIEW();

    // Class destructor
    virtual ~fndMVC_VIEW();
    
    /** Method called whenever the model (observed object) is changed. It has to
     * be implemented in subclass to update the view according to the associated
     * model state. */
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
