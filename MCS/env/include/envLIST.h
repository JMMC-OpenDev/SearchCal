#ifndef envLIST_H
#define envLIST_H
/*******************************************************************************
* JMMC project
*
* "@(#) $Id: envLIST.h,v 1.2 2004-12-08 14:59:27 lafrasse Exp $"
*
* who       when         what
* --------  -----------  -------------------------------------------------------
* lafrasse  07-Dec-2004  Created
* lafrasse  08-Dec-2004  Comments refinments
*
*
*******************************************************************************/

/**
 * \file
 * envLIST class declaration.
 */

#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif

/* 
 * System Headers 
 */
#include <map>

/*
 * Class declaration
 */

/**
 * This class retrieve and verfiy all the MCS environments informations, used to
 * isolate one MCS environment from all the others at the message communication
 * level.
 * 
 * For exemple, you can launch multiple msgManager processes using differents
 * MCS environments, thus enabling concurrent msg-based modules debugging.
 * 
 * \usedfiles
 * In MCS all the environments are listed in the mcsEnvList file.
 * \filename mcsEnvList :  MCS environment list definition file, located in
 * $MCSROOT/ect/mcsEnvList
 *
 * \n
 * \env
 * In MCS, the environment to be used is defined by the value of the $MCSENV
 * evironment variable.
 * \envvar MCSENV :  MCS environment name to be used
 * 
 * \sa msg module
 * 
 */
class envLIST
{

public:
    // Class constructor
    envLIST();

    // Class destructor
    virtual ~envLIST();

    virtual const char*    GetHostName   (const char *envName = NULL);
    virtual const mcsINT32 GetPortNumber (const char *envName = NULL);
    virtual void           Show          (void);

protected:
    
private:
    mcsLOGICAL                     _fileAlreadyLoaded;
    map<string,pair<string,int> >  _map;
    mcsSTRING256                   _hostName;

    mcsCOMPL_STAT LoadEnvListFile(void);

    // Declaration of copy constructor and assignment operator as private
    // methods, in order to hide them from the users.
    envLIST(const envLIST&);
    envLIST& operator=(const envLIST&);
};

#endif /*!envLIST_H*/

/*___oOo___*/
