#ifndef evhCMD_CALLBACK_H
#define evhCMD_CALLBACK_H
/*******************************************************************************
* JMMC project
*
* "@(#) $Id: evhCMD_CALLBACK.h,v 1.2 2004-12-22 08:53:43 gzins Exp $"
*
* who       when         what
* --------  -----------  -------------------------------------------------------
* gzins     22-Sep-2004  Created
* gzins     22-Dec-2004  Added SetMethod()
*
*******************************************************************************/
/**
 * \file
 * Declaration of the evhCMD_CALLBACK class 
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

/** Typedef for the callback method type for command event*/
typedef evhCB_COMPL_STAT
              (fndOBJECT::*evhCMD_CB_METHOD)(const msgMESSAGE &,void*);

/**
 * Class to hold object's callback for command event.
 * 
 * This class is derived form evhCALLBACK, and defined the method prototype
 * for the callback associated the command event.
 *
 * The method must be of type evhCMD_CB_METHOD, i.e. must be a virtual
 * method of a subclass of fndOBJECT and have a prototype in the following
 * form:\n
 * \code
 * evhCB_COMPL_STAT method(const msgMESSAGE &msg, void* userData);
 * \endcode
 *
 */
class evhCMD_CALLBACK : public evhCALLBACK
{
public:
    evhCMD_CALLBACK(fndOBJECT *obj,
                    const evhCMD_CB_METHOD method = NULL,
                    void *userData = NULL);
    virtual ~evhCMD_CALLBACK();

    evhCMD_CALLBACK& operator=(const evhCMD_CALLBACK&);

    evhCMD_CALLBACK(const evhCMD_CALLBACK &source);

    virtual evhCMD_CALLBACK &SetMethod(const evhCMD_CB_METHOD method, 
                                       void *userData = NULL);
    
    virtual mcsLOGICAL IsSame (evhCALLBACK &callback);

    virtual evhCB_COMPL_STAT Run(const msgMESSAGE &msg);

protected:

private:
    evhCMD_CB_METHOD _method;    /** Method to be executed */
};

#endif /*!evhCMD_CALLBACK_H*/

/*___oOo___*/
