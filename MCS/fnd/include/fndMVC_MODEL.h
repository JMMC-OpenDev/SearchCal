#ifndef fndMVC_MODEL_H
#define fndMVC_MODEL_H
/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
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
 * Model object class, or "data" in the model-view paradigm.
 * It has to be subclassed to represent an object that the application wants to
 * have observed.  
 *
 * A model object can have one or more views. A view may be any
 * object that implements interface fndMVC_VIEW. After a model instance
 * changes, an application calling the fndMVC_MODEL's NotifyViews method
 * causes all of its views to be notified of the change by a call to their
 * Update method.
 *
 * The order in which notifications will be delivered is unspecified. The
 * default implementation provided in the fndMVC_MODEL class will notify
 * views in the order in which they registered interest, but subclasses may
 * change this order, use no guaranteed order, deliver notifications on separate
 * threads, or may guarantee that their subclass follows this order, as they
 * choose.
 *
 * When a model object is newly created, its set of views is empty.
 *
 * @note This class has been strongly influenced by java.util.Observable
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
