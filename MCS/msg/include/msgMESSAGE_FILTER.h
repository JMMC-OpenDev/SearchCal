#ifndef msgMESSAGE_FILTER_H
#define msgMESSAGE_FILTER_H
/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * \file
 * Declaration of msgMESSAGE_FILTER class 
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
#include "msgMESSAGE.h"


/*
 * Class declaration
 */

/**
 * Class used to filter messages on reception.
 */
class msgMESSAGE_FILTER
{

public:
    // Class constructor
    msgMESSAGE_FILTER(const mcsCMD, const mcsINT32);

    // Class destructor
    virtual ~msgMESSAGE_FILTER();

    const char*      GetCommand              (void) const;
    const mcsINT32   GetCommandId            (void) const;
                                             
    const mcsLOGICAL IsMatchedBy             (const  msgMESSAGE&) const;

    friend  std::ostream&    operator<<      (       std::ostream&      stream,
                                               const msgMESSAGE_FILTER& filter);

protected:
    
private:
    mcsCMD      _command;
    mcsINT32    _commandId;

    // Declaration of copy constructor and assignment operator as private
    // methods, in order to hide them from the users.
    msgMESSAGE_FILTER(const msgMESSAGE_FILTER&);
    msgMESSAGE_FILTER& operator=(const msgMESSAGE_FILTER&);
};

#endif /*!msgMESSAGE_FILTER_H*/

/*___oOo___*/
