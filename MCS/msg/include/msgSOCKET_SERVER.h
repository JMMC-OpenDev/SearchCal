#ifndef msgSOCKET_SERVER_H
#define msgSOCKET_SERVER_H
/*******************************************************************************
* JMMC project
*
* "@(#) $Id: msgSOCKET_SERVER.h,v 1.1 2004-11-22 15:55:08 scetre Exp $"
*
* who       when         what
* --------  -----------  -------------------------------------------------------
* scetre    22-Nov-2004  Created
*
*
*******************************************************************************/

/**
 * \file
 * msgSOCKET_SERVER class declaration.
 */


#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif


/*
 * Class declaration
 */

/**
 * Brief description of the class, which ends at this dot.
 * 
 * OPTIONAL detailed description of the class follows here.
 *
 * \n
 * \ex
 * OPTIONAL. Code example if needed
 * \n Brief example description.
 * \code
 * Insert your code example here
 * \endcode
 *
 * \sa msgSOCKET.cpp
 * 
 */
class msgSOCKET_SERVER
{

public:
    // Brief description of the constructor
    msgSOCKET_SERVER();

    // Brief description of the destructor
    virtual ~msgSOCKET_SERVER();

    virtual mcsCOMPL_STAT Open(std::string host,
                   mcsINT32 port);
protected:

    
private:
    // Declaration of copy constructor and assignment operator as private
    // methods, in order to hide them from the users.
     msgSOCKET_SERVER(const msgSOCKET_SERVER&);
     msgSOCKET_SERVER& operator=(const msgSOCKET_SERVER&);


};




#endif /*!msgSOCKET_SERVER_H*/

/*___oOo___*/
