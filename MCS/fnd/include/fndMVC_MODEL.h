#ifndef fndMVC_MODEL_H
#define fndMVC_MODEL_H
/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: fndMVC_MODEL.h,v 1.1 2005-06-13 10:24:06 scetre Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 ******************************************************************************/

/**
 * \file
 * Declaration of fndMVC_MODEL class.
 */

#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif

#include <list>

/*
 * MCS header
 */
#include "mcs.h"

/*
 * Local header
 */
#include "fndMVC_VIEW.h"

/** typedef of the stl list of view */
typedef std::list<fndMVC_VIEW *> fndViewList;

/*
 * Class declaration
 */

/**
 * Basic definition of the model class.
 * 
 * This class implement the Model-View-Controller concept.
 *
 * \n
 * \warning This class do nothing. In order to use it it is necessary to
 * implement the model variable and the Get and Set methods.
 *
 */
class fndMVC_MODEL
{

public:
    // Class constructor
    fndMVC_MODEL();

    // Class destructor
    virtual ~fndMVC_MODEL();

    virtual mcsCOMPL_STAT AddView(fndMVC_VIEW *view);
    virtual mcsCOMPL_STAT DeleteView(fndMVC_VIEW *view);
    virtual mcsCOMPL_STAT DeleteViews();
    virtual mcsCOMPL_STAT NotifyViews();
     
    virtual mcsINT32 GetNbViews();
    
protected:
    
private:
    // Declaration of copy constructor and assignment operator as private
    // methods, in order to hide them from the users.
    fndMVC_MODEL(const fndMVC_MODEL&);
    fndMVC_MODEL& operator=(const fndMVC_MODEL&);

    fndViewList _viewList; 
};

#endif /*!fndMVC_MODEL_H*/

/*___oOo___*/
