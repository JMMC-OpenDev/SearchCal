#ifndef envLIST_H
#define envLIST_H
/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

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
 * This class deals with the MCS environments. It can be used to show all MCS
 * environments or to get informations (host name and port number) of a given
 * environment.
 * 
 * \usedfiles
 * In MCS all the environments are listed in the mcsEnvList file.
 * \filename mcsEnvList :  MCS environment list definition file, located in
 * $MCSTOP/etc/mcsEnvList
 *
 * \n
 * \env
 * The environment name to be used is defined by the value of the $MCSENV
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
