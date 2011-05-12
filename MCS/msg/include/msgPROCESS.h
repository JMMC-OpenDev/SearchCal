#ifndef msgPROCESS_H
#define msgPROCESS_H
/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * \file
 * Declaration of msgPROCESS class 
 */

#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif

/*
 * Local header files
 */
#include "msgSOCKET_CLIENT.h"

/*
 * Class declaration
 */

/**
 * Class storing all the details about each process connected to the
 * \<msgManager\>.
 * 
 * Provides all the methods allowing to get/set informations about any process
 * connected to the \<msgManager\>. 
 */
class msgPROCESS : public msgSOCKET_CLIENT
{

public:
    // Class constructor
    msgPROCESS();

    // Class destructor
    virtual ~msgPROCESS();

    virtual mcsCOMPL_STAT SetName     (const char *name);
    virtual const char   *GetName     (void       ) const;

    virtual mcsCOMPL_STAT SetId       (const mcsINT32 id);
    virtual mcsINT32      GetId       (void          ) const;

    virtual mcsCOMPL_STAT SetUnicity  (const mcsLOGICAL flag);
    virtual mcsLOGICAL    IsUnique    (void            ) const;

protected:

private:
    // Declaration of copy constructor and assignment operator as private
    // methods, in order to hide them from the users.
    msgPROCESS(const msgPROCESS &process);
    msgPROCESS& operator=(const msgPROCESS&);

    // Name of the process; i.e. MCS registering name
    mcsPROCNAME _name;
   
    // Proces ID
    mcsINT32 _id;

    // Unicity flag
    mcsLOGICAL _unicity;
};

#endif /*!msgPROCESS_H*/

/*___oOo___*/
