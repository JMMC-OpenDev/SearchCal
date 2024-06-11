#ifndef evhTASK_H
#define evhTASK_H
/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * \file
 * Declaration of the evhTASK class.
 */

#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif

#include "mcs.h"

class evhTASK
{
public:
    evhTASK();
    virtual ~evhTASK();

    virtual mcsCOMPL_STAT Init(mcsINT32 argc, char *argv[]);
    virtual mcsCOMPL_STAT AddionalInit();
    virtual mcsCOMPL_STAT AppInit();

    virtual const char *Name();
    virtual mcsCOMPL_STAT Usage();
    virtual mcsCOMPL_STAT PrintSynopsis();
    virtual mcsCOMPL_STAT PrintStdOptions();
    virtual mcsCOMPL_STAT PrintAppOptions();
    virtual mcsCOMPL_STAT PrintArguments();
    virtual mcsCOMPL_STAT ParseOptions(mcsINT32 argc, char *argv[]);
    virtual mcsCOMPL_STAT ParseStdOptions(mcsINT32 argc, char *argv[],
                                          mcsINT32 *optInd,
                                          mcsLOGICAL *optUsed);
    virtual mcsCOMPL_STAT ParseAppOptions(mcsINT32 argc, char *argv[],
                                          mcsINT32 *optInd,
                                          mcsLOGICAL *optUsed);
    virtual mcsCOMPL_STAT ParseArguments(mcsINT32 argc, char *argv[],
                                         mcsINT32 *optInd,
                                         mcsLOGICAL *optUsed);
    virtual mcsLOGICAL IsFileLogOption();
    virtual mcsLOGICAL IsStdoutLogOption();

    virtual const char *GetSwVersion();

protected:
private:
    // Declaration of copy constructor and assignment operator as private
    // methods, in order to hide them from the users.
    evhTASK& operator=(const evhTASK&);
    evhTASK (const evhTASK&);

    mcsLOGICAL _fileLogOption;
    mcsLOGICAL _stdoutLogOption;
};

#endif /*!evhTASK_H*/

/*___oOo___*/
