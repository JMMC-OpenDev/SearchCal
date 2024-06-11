/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/
/*
 * 
 * This file has been automatically generated
 * 
 * !!!!!!!!!!!  DO NOT MANUALLY EDIT THIS FILE  !!!!!!!!!!!
 */
#ifndef evhDEBUG_CMD_H
#define evhDEBUG_CMD_H

/**
 * \file
 * Generated for evhDEBUG_CMD class declaration.
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
#define evhDEBUG_CMD_NAME "DEBUG"

/*
 * Command definition file
 */
#define evhDEBUG_CDF_NAME "evhDEBUG.cdf"

/*
 * Class declaration
 */
        
    
/**
 * This class is intented to be used for a
 * reception of the DEBUG command 
 */

class evhDEBUG_CMD: public cmdCOMMAND
{
public:
    evhDEBUG_CMD(string name, string params);
    virtual ~evhDEBUG_CMD();


    virtual mcsCOMPL_STAT GetStdoutLevel(mcsINT32 *_stdoutLevel_);
    virtual mcsLOGICAL IsDefinedStdoutLevel(void);
    virtual mcsCOMPL_STAT GetLogfileLevel(mcsINT32 *_logfileLevel_);
    virtual mcsLOGICAL IsDefinedLogfileLevel(void);
    virtual mcsCOMPL_STAT GetPrintDate(mcsLOGICAL *_printDate_);
    virtual mcsLOGICAL IsDefinedPrintDate(void);
    virtual mcsCOMPL_STAT GetPrintFileLine(mcsLOGICAL *_printFileLine_);
    virtual mcsLOGICAL IsDefinedPrintFileLine(void);

protected:

private:
    // Declaration of copy constructor and assignment operator as private
    // methods, in order to hide them from the users.
     evhDEBUG_CMD(const evhDEBUG_CMD&);
     evhDEBUG_CMD& operator=(const evhDEBUG_CMD&);

};

#endif /*!evhDEBUG_CMD_H*/

/*___oOo___*/
