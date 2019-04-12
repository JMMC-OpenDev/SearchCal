/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/
/*
 * 
 * This file has been automatically generated
 * 
 * !!!!!!!!!!!  DO NOT MANUALLY EDIT THIS FILE  !!!!!!!!!!!
 */
#ifndef sclsvrGETSTAR_CMD_H
#define sclsvrGETSTAR_CMD_H

/**
 * \file
 * Generated for sclsvrGETSTAR_CMD class declaration.
 * This file has been automatically generated. If this file is missing in your
 * modArea, just type make all to regenerate.
 */

#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif

/*
 * MCS Headers
 */
#include "cmd.h"

/*
 * Command name definition
 */
#define sclsvrGETSTAR_CMD_NAME "GETSTAR"

/*
 * Command definition file
 */
#define sclsvrGETSTAR_CDF_NAME "sclsvrGETSTAR.cdf"

/*
 * Class declaration
 */
        
    
/**
 * This class is intented to be used for a
 * reception of the GETSTAR command 
 */

class sclsvrGETSTAR_CMD: public cmdCOMMAND
{
public:
    sclsvrGETSTAR_CMD(string name, string params);
    virtual ~sclsvrGETSTAR_CMD();


    virtual mcsCOMPL_STAT GetObjectName(char **_objectName_);
    virtual mcsCOMPL_STAT GetFile(char **_file_);
    virtual mcsLOGICAL IsDefinedFile(void);
    virtual mcsCOMPL_STAT GetBaseMax(mcsDOUBLE *_baseMax_);
    virtual mcsLOGICAL HasDefaultBaseMax(void);
    virtual mcsCOMPL_STAT GetDefaultBaseMax(mcsDOUBLE *_baseMax_);
    virtual mcsCOMPL_STAT GetWlen(mcsDOUBLE *_wlen_);
    virtual mcsLOGICAL HasDefaultWlen(void);
    virtual mcsCOMPL_STAT GetDefaultWlen(mcsDOUBLE *_wlen_);
    virtual mcsCOMPL_STAT GetFormat(char **_format_);
    virtual mcsLOGICAL IsDefinedFormat(void);
    virtual mcsLOGICAL HasDefaultFormat(void);
    virtual mcsCOMPL_STAT GetDefaultFormat(char **_format_);
    virtual mcsCOMPL_STAT GetDiagnose(mcsLOGICAL *_diagnose_);
    virtual mcsLOGICAL IsDefinedDiagnose(void);
    virtual mcsLOGICAL HasDefaultDiagnose(void);
    virtual mcsCOMPL_STAT GetDefaultDiagnose(mcsLOGICAL *_diagnose_);

protected:

private:
    // Declaration of copy constructor and assignment operator as private
    // methods, in order to hide them from the users.
     sclsvrGETSTAR_CMD(const sclsvrGETSTAR_CMD&);
     sclsvrGETSTAR_CMD& operator=(const sclsvrGETSTAR_CMD&);

};

#endif /*!sclsvrGETSTAR_CMD_H*/

/*___oOo___*/
