#ifndef cmdPARAM_H
#define cmdPARAM_H
/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: cmdPARAM.h,v 1.12 2005-04-17 16:18:48 mella Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.11  2005/04/11 12:33:25  scetre
 * Revue : changed documentation
 *
 * Revision 1.10  2005/02/28 13:38:18  lafrasse
 * Added minimum and maximum parameter values information output in 
 * cmdPARAM:GetHelp()
 *
 * Revision 1.9  2005/02/27 19:44:17  gzins
 * Implemented parameter value range check
 *
 * Revision 1.8  2005/02/27 09:27:41  gzins
 * Improved error handling
 *
 * Revision 1.7  2005/02/15 10:58:58  gzins
 * Added CVS log as file modification history
 *
 * mella     15-Nov-2004  Created
 * lafrasse  01-Feb-2005  Added type management
 *
 ******************************************************************************/

/**
 * \file
 * cmdPARAM class declaration.
 */


#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif

/*
 * System headers
 */
#include <string>
using namespace std;

/*
 * Class declaration
 */

/**
 * This class is used to get a model for any parameter of an MCS command.
 */
class cmdPARAM
{

public:
  //  cmdPARAM(){};
    // Brief description of the constructor
    cmdPARAM(string name, string desc, string type, string unit,
             mcsLOGICAL optional);

    // Brief description of the destructor
    virtual ~cmdPARAM();

    virtual string         GetName          (void);
    virtual string         GetDesc          (void);
    virtual string         GetType          (void);
    virtual string         GetUnit          (void);
    virtual mcsLOGICAL     IsOptional       (void);
    virtual string         GetHelp          (void);

    virtual mcsLOGICAL     IsDefined        (void);
    virtual string         GetUserValue     (void);
    virtual mcsCOMPL_STAT  GetUserValue     (mcsINT32     *value);
    virtual mcsCOMPL_STAT  GetUserValue     (mcsDOUBLE    *value);
    virtual mcsCOMPL_STAT  GetUserValue     (mcsLOGICAL   *value);
    virtual mcsCOMPL_STAT  GetUserValue     (char        **value);
    virtual mcsCOMPL_STAT  SetUserValue     (string        value);

    virtual mcsLOGICAL     HasDefaultValue  (void);
    virtual string         GetDefaultValue  (void);
    virtual mcsCOMPL_STAT  GetDefaultValue  (mcsINT32     *value);
    virtual mcsCOMPL_STAT  GetDefaultValue  (mcsDOUBLE    *value);
    virtual mcsCOMPL_STAT  GetDefaultValue  (mcsLOGICAL   *value);
    virtual mcsCOMPL_STAT  GetDefaultValue  (char        **value);
    virtual mcsCOMPL_STAT  SetDefaultValue  (string        value);

    virtual mcsCOMPL_STAT  SetMinValue      (string        value);
    virtual mcsCOMPL_STAT  SetMaxValue      (string        value);

protected:
    virtual mcsCOMPL_STAT  CheckValueType   (string        value);
    virtual mcsCOMPL_STAT  CheckValueRange  (string        value);

private:
    // Declaration of copy constructor and assignment operator as private
    // methods, in order to hide them from the users.
    cmdPARAM(const cmdPARAM&);
    cmdPARAM& operator=(const cmdPARAM&);

    /** name of the parameter */
    string _name;
    /** description of the parameter */
    string _desc;
    /** type of the parameter */
    string _type;
    /** unit of the parameter */
    string _unit;
    /** flag an optional parameter */ 
    mcsLOGICAL _optional; 
    /** user value of the parameter */
    string _userValue;
    /** default value of the parameter */
    string _defaultValue;
    /** min value of the parameter */
    string _minValue;
    /** max value of the parameter */
    string _maxValue;
};




#endif /*!cmdPARAM_H*/

/*___oOo___*/
