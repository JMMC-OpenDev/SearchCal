#ifndef msgMANAGER_IF_H
#define msgMANAGER_IF_H
/*******************************************************************************
* JMMC project
*
* "@(#) $Id: msgMANAGER_IF.h,v 1.1 2004-11-19 17:19:42 lafrasse Exp $"
*
* who       when         what
* --------  -----------  -------------------------------------------------------
* lafrasse  18-Nov-2004  Created
*
*
*******************************************************************************/

/**
 * \file
 * msgMANAGER_IF class declaration.
 */


#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif

/*
 * MCS Headers 
 */
#include "mcs.h"


/*
 * Local Headers 
 */
#include "msgMESSAGE.h"


/*
 * Class declaration
 */

/**
 * Brief description of the class, which ends at this dot.
 * 
 * OPTIONAL detailed description of the class follows here.
 *
 * \usedfiles
 * OPTIONAL. If files are used, for each one, name, and usage description.
 * \filename fileName1 :  usage description of fileName1
 * \filename fileName2 :  usage description of fileName2
 *
 * \n
 * \env
 * OPTIONAL. If needed, environmental variables accessed by the class. For
 * each variable, name, and usage description, as below.
 * \envvar envVar1 :  usage description of envVar1
 * \envvar envVar2 :  usage description of envVar2
 * 
 * \n
 * \warning OPTIONAL. Warning if any (software requirements, ...)
 *
 * \n
 * \ex
 * OPTIONAL. Code example if needed
 * \n Brief example description.
 * \code
 * Insert your code example here
 * \endcode
 *
 * \sa OPTIONAL. See also section, in which you can refer other documented
 * entities. Doxygen will create the link automatically.
 * \sa modcppMain.C
 * 
 * \bug OPTIONAL. Bugs list if it exists.
 * \bug For example, description of the first bug
 * \bug For example, description of the second bug
 * 
 * \todo OPTIONAL. Things to forsee list, if needed. For example, 
 * \todo add other methods, dealing with operations.
 * 
 */
class msgMANAGER_IF
{
public:
    msgMANAGER_IF();
    virtual ~msgMANAGER_IF();

    virtual mcsCOMPL_STAT Connect     (const mcsPROCNAME  procName,
                                       const char        *msgManagerHost);

    virtual mcsCOMPL_STAT SendCommand (const char        *command,
                                       const mcsPROCNAME  destProc,
                                       const char        *buffer,  
                                       mcsINT32           bufLen);
    virtual mcsCOMPL_STAT SendReply   (msgMESSAGE        &msg,
                                       mcsLOGICAL         lastReply);

    virtual mcsCOMPL_STAT Receive     (msgMESSAGE        &msg,
                                       const mcsINT32     timeoutInMs);

    virtual mcsLOGICAL    IsConnected ();

    virtual mcsCOMPL_STAT Disconnect  ();

protected:

private:
    int msgManagerSd;

    // Declaration of copy constructor and assignment operator as private
    // methods, in order to hide them from the users.
    msgMANAGER_IF& operator=(const msgMANAGER_IF&);
    msgMANAGER_IF (const msgMANAGER_IF&);
};

#endif /*!msgMANAGER_IF_H*/

/*___oOo___*/
