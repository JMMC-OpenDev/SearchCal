#ifndef evhCALLBACK_H
#define evhCALLBACK_H
/*******************************************************************************
* JMMC project
*
* "@(#) $Id: evhCALLBACK.h,v 1.2 2004-12-08 13:29:56 gzins Exp $"
*
* who       when         what
* --------  -----------  -------------------------------------------------------
* gzins     22-Sep-2004  Created
* gzins     08-Dec-2004  Removed evhCB_RETURN and evhCB_NOCALLS callback
*                        return values
*                        Added IsDetached() method
*
*******************************************************************************/
/**
 * \file
 * Declaration of the evhCALLBACK class 
 */

#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif

/*
 * Class declaration
 */
#include "fnd.h"
#include "msg.h"

/**
 * Defines for the return value from callback routines.
 * It is a bitmask, built by putting in OR some of the following values:
 *	  - Bit 1 : evhCB_FAILURE or evhCB_SUCCESS \n
 *	    Success/Failure of the procedure. The default is SUCCESS.
 *	  - Bit 2 : evhCB_NO_DELETE or evhCB_DELETE \n
 *	    The callback must be deinstalled or not from the event handler. The
 *	    default is NO_DELETE.
 *
 * Group of values must be in OR to build the actual return code.
 * Some examples of return codes:\n
 *    - evhCB_DELETE                  \n
 *      SUCCESS and delete the callback.
 *    - evhCB_DELETE | evhCB_SUCCESS  \n
 *      SUCCESS and delete the callback.
 *    - evhCB_DELETE | evhCB_FAILURE  \n
 *      FAILURE and delete the callback.
 *    - evhCB_NO_DELETE               \n
 *      SUCCESS and do not delete the callback.
 *    - evhCB_FAILURE                 \n
 *      FAILURE and DO NOT delete the callback.
 *
 *    Returning 0 means:   evhCB_NO_DELETE | evhCB_SUCCESS
 */
typedef mcsUINT32 evhCB_COMPL_STAT;

/** Bit 1:    success/failure of the method */
#define evhCB_FAILURE   1
#define evhCB_SUCCESS   0

/** Bit 2:    0 if NO DELETE     1 if delete requested */
#define evhCB_NO_DELETE 0
#define evhCB_DELETE    2

/**
 * Base class to hold object's callback data for event.
 * 
 * This class is the base class to be used to define callbacks, i.e. functions
 * that have to be executed by the event handler (see evhHANDLER) when a
 * specific event occurs. 
 * A callback is defined by the triplet (object, method, userData) where:
 *   - \em object \n
 *     is the object for which the method has to be executed. It MUST be an
 *     instance of a subclass of fndOBJECT
 *   - \em method \n
 *     is a pointer to a method to be executed.
 *   - \em userData \n
 *     is a void* used to pass data to the method.
 *
 * This class only provides constructors to set \em object and \em userData,
 * and must be derived for each type of event handled by event handler, to
 * define the method prototype associated to this event type (see
 * evhCMD_CALLBACK or evhIOSTREAM_CALLBACK).
 */
class evhCALLBACK : public fndOBJECT
{
public:
    evhCALLBACK(fndOBJECT *obj,
                void *userData = NULL);
    virtual ~evhCALLBACK();

    evhCALLBACK& operator=(const evhCALLBACK&);

    evhCALLBACK(const evhCALLBACK &source);

    virtual mcsLOGICAL IsSame (evhCALLBACK &callback);

    virtual mcsCOMPL_STAT Detach();

    virtual mcsLOGICAL IsDetached();

protected:
    fndOBJECT    *_object;   /** Object to which the method to be executed,
                              * belongs. */
    void         *_userData; /** Pointer to user data */
    mcsLOGICAL   _detached;  /** Detach flag */
private:
};

#endif /*!evhCALLBACK_H*/

/*___oOo___*/
