#ifndef evhIOSTREAM_CALLBACK_H
#define evhIOSTREAM_CALLBACK_H
/*******************************************************************************
* JMMC project
*
* "@(#) $Id: evhIOSTREAM_CALLBACK.h,v 1.1 2004-10-18 09:40:10 gzins Exp $"
*
* who       when         what
* --------  -----------  -------------------------------------------------------
* gzins     22-Sep-2004  Created
*
*******************************************************************************/
/**
 * \file
 * Declaration of the evhIOSTREAM_CALLBACK class 
 */

#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif

/*
 * Class declaration
 */
#include "fnd.h"
#include "msg.h"
#include "evhCALLBACK.h"
/**
 * Defines for the return value from callback routines.
 * It is a bitmask, built by putting in OR some of the following values:
 *	  - Bit 1 : evhCB_FAILURE or evhCB_SUCCESS \n
 *	    Success/Failure of the procedure. The default is SUCCESS.
 *	  - Bit 2 : evhCB_NO_DELETE or evhCB_DELETE \n
 *	    The callback must be deinstalled or not from the event handler. The
 *	    default is NO_DELETE.
 *	  - Bit 3 : evhCB_RETURN \n
 *	    The main loop must return immediately. No other callback will be
 *	    executed.
 *	  - Bit 4 : evhCB_NOCALLS \n
 *	    Not for users. Used internally by the main loop to comunicate that no
 *	    callback has been executed for a certain event.
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
/** Typedef for the callback proc type */
typedef evhCB_COMPL_STAT
              (fndOBJECT::*evhIOSTREAM_CB_METHOD)(const int, void*);

/**
 * Class to hold object's callback for file I/O event.
 * 
 * This class is derived form evhCALLBACK, and defined the method prototype
 * for the callback associated the file I/O event.
 *
 * The method must be of type evhIOSTREAM_CB_METHOD, i.e. must be a virtual
 * method of a subclass of fndOBJECT and have a prototype in the following
 * form:\n
 * \code
 * evhCB_COMPL_STAT method(const int fd, void* userData);
 * \endcode
 *
 */
class evhIOSTREAM_CALLBACK : public evhCALLBACK
{
public:
    evhIOSTREAM_CALLBACK(fndOBJECT *obj,
                const evhIOSTREAM_CB_METHOD method = NULL,
                void *userData = NULL);
    virtual ~evhIOSTREAM_CALLBACK();

    evhIOSTREAM_CALLBACK& operator=(const evhIOSTREAM_CALLBACK&);

    evhIOSTREAM_CALLBACK(const evhIOSTREAM_CALLBACK &source);

    virtual mcsLOGICAL IsSame (evhCALLBACK &callback);

    virtual evhCB_COMPL_STAT Run(const int fd);

protected:

private:
    evhIOSTREAM_CB_METHOD _method;    /** Method to be executed */
};

#endif /*!evhIOSTREAM_CALLBACK_H*/

/*___oOo___*/
