#ifndef evhKEY_H
#define evhKEY_H
/*******************************************************************************
* JMMC project
*
* "@(#) $Id: evhKEY.h,v 1.2 2004-11-19 06:34:06 gzins Exp $"
*
* who       when         what
* --------  -----------  -------------------------------------------------------
* gzins     24-Sep-2004  Created
* gzins     19-Nov-2004  Added evhTYPE_COMMAND_REPLY
*
*
*******************************************************************************/

/**
 * \file
 * Declaration of the evhKEY class.
 */
#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif

#include "mcs.h"
#include "fnd.h"

/*
 *   Definition of the event types 
 */
typedef enum 
{
    evhTYPE_UNDEFINED = 0,
    evhTYPE_COMMAND, 
    evhTYPE_COMMAND_REPLY, 
    evhTYPE_IOSTREAM 
} evhTYPE; 

/*
 * Class declaration
 */

/**
 * Base class to hold general event id keys
 * 
 * The class evhKEY just holds the message type (evhTYPE type) which is
 * the command part of all the events which are treated by the event handler.
 * This class must be derived for each type of event treated by event handler.
 */
class evhKEY : public fndOBJECT
{
public:
    evhKEY(const evhTYPE type);
    evhKEY (const evhKEY&);
    virtual ~evhKEY();

    evhKEY& operator=(const evhKEY&);
    
    virtual mcsLOGICAL IsSame(const evhKEY& key);
    virtual mcsLOGICAL Match(const evhKEY& key) = 0;

    virtual evhKEY     &SetType(const evhTYPE type);
    virtual evhTYPE    GetType() const;
 
protected:

private:
    evhTYPE   _type; /** Event type */
};

#endif /*!evhKEY_H*/

/*___oOo___*/
