#ifndef evhSERVER_H
#define evhSERVER_H
/*******************************************************************************
* JMMC project
*
* "@(#) $Id: evhSERVER.h,v 1.3 2004-11-23 09:13:22 gzins Exp $"
*
* who       when         what
* --------  -----------  -------------------------------------------------------
* gzins     09-Nov-2004  Created
* gzins     23-Nov-2004  Used new msg C++ library.
*                        Added SendReply method
*
*******************************************************************************/

/**
 * \file
 * evhSERVER class declaration.
 */


#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif

#include "evhTASK.h"
#include "evhHANDLER.h"

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
class evhSERVER : public evhTASK, public evhHANDLER
{
public:
    evhSERVER();
    virtual ~evhSERVER();

    // Usage of the application 
    virtual mcsCOMPL_STAT PrintSynopsis();
    virtual mcsCOMPL_STAT PrintArguments();

    // Parse command-line arguments 
    virtual mcsCOMPL_STAT ParseArguments(mcsINT32 argc, char *argv[],
                                         mcsINT32 *optInd,
                                         mcsLOGICAL *optUsed);

    // Init method
    virtual mcsCOMPL_STAT Init(mcsINT32 argc, char *argv[]);

    // Command callbacks
    virtual evhCB_COMPL_STAT VersionCB(msgMESSAGE &msg, void*);

    // Connection to MCS message manager
    virtual mcsCOMPL_STAT Connect();
    virtual mcsCOMPL_STAT Disconnect();

    // Main loop
    virtual mcsCOMPL_STAT MainLoop(msgMESSAGE *msg=NULL);

    // Main loop
    virtual mcsCOMPL_STAT SendReply(msgMESSAGE &msg, 
                                    mcsLOGICAL lastReply=mcsTRUE);

protected:

private:
    // Declaration of copy constructor and assignment operator as private
    // methods, in order to hide them from the users.
    evhSERVER& operator=(const evhSERVER&);
    evhSERVER (const evhSERVER&);

    // Interface to msgManager process
    msgMANAGER_IF   _msgManager;

    // Command given as command-line argument (is any)
    msgMESSAGE _msg;
};

#endif /*!evhSERVER_H*/

/*___oOo___*/
