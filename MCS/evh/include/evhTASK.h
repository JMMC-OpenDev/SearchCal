#ifndef evhTASK_H
#define evhTASK_H
/*******************************************************************************
* JMMC project
*
* "@(#) $Id: evhTASK.h,v 1.1 2004-11-17 10:28:53 gzins Exp $"
*
* who       when		 what
* --------  -----------	 -------------------------------------------------------
* gzins     09-Jun-2004  created
*
*
*******************************************************************************/

#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif

#include "mcs.h"

class evhTASK
{
public:
    evhTASK();
    virtual ~evhTASK();

    virtual const char *Name();
    virtual mcsCOMPL_STAT Usage();
    virtual mcsCOMPL_STAT AppUsage();
    virtual mcsCOMPL_STAT ParseOptions(mcsINT32 argc, mcsINT8 *argv[]);
    virtual mcsCOMPL_STAT ParseAppOptions(mcsINT32 argc, mcsINT8 *argv[],
                                          mcsINT32 *optind);
    virtual mcsLOGICAL IsFileLogOption();
    virtual mcsLOGICAL IsStdoutLogOption();
    virtual mcsLOGICAL IsActionLogOption();
    virtual mcsLOGICAL IsTimerLogOption();

    virtual const char *GetSwVersion();

protected:
private:
    // Declaration of copy constructor and assignment operator as private
    // methods, in order to hide them from the users.
    evhTASK& operator=(const evhTASK&);
    evhTASK (const evhTASK&);

    mcsLOGICAL _fileLogOption;
    mcsLOGICAL _stdoutLogOption;
    mcsLOGICAL _actionLogOption; 
    mcsLOGICAL _timerLogOption;
};

#endif /*!evhTASK_H*/

/*___oOo___*/
