#ifndef cmdPARAM_H
#define cmdPARAM_H
/*******************************************************************************
* JMMC project
*
* "@(#) $Id: cmdPARAM.h,v 1.4 2004-12-05 18:57:21 gzins Exp $"
*
* who       when         what
* --------  -----------  -------------------------------------------------------
* mella     15-Nov-2004  Created
*
*
*******************************************************************************/

/**
 * \file
 * cmdPARAM class declaration.
 */


#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif


/*
 * Class declaration
 */

/**
 * 
 */
class cmdPARAM
{

public:
  //  cmdPARAM(){};
    // Brief description of the constructor
    cmdPARAM(string name, string desc, string unit, mcsLOGICAL optional);

    // Brief description of the destructor
    virtual ~cmdPARAM();

    virtual string GetName();
    virtual string GetDesc();
    virtual string GetUnit();
    virtual string GetUserValue();
    virtual string GetDefaultValue();
    virtual mcsLOGICAL IsDefined();
    virtual mcsLOGICAL HasDefaultValue();
    virtual mcsLOGICAL IsOptional();
    virtual string GetHelp();
    virtual mcsCOMPL_STAT SetUserValue(string value);
    virtual mcsCOMPL_STAT SetDefaultValue(string value);
    virtual mcsCOMPL_STAT GetUserValue(mcsINT32 *value);
    virtual mcsCOMPL_STAT GetUserValue(mcsDOUBLE *value);
    virtual mcsCOMPL_STAT GetUserValue(mcsLOGICAL *value);
    virtual mcsCOMPL_STAT GetUserValue(char **value);
    virtual mcsCOMPL_STAT GetDefaultValue(mcsINT32 *value);
    virtual mcsCOMPL_STAT GetDefaultValue(mcsDOUBLE *value);
    virtual mcsCOMPL_STAT GetDefaultValue(mcsLOGICAL *value);
    virtual mcsCOMPL_STAT GetDefaultValue(char **value);

protected:


private:
    // Declaration of copy constructor and assignment operator as private
    // methods, in order to hide them from the users.
    cmdPARAM(const cmdPARAM&);
    cmdPARAM& operator=(const cmdPARAM&);

    /** name of the parameter */
    string _name;
    /** description of the parameter */
    string _desc;
    /** unit of the parameter */
    string _unit;
    /** flag an optional parameter */ 
    mcsLOGICAL _optional; 
    /** user value of the parameter */
    string _userValue;
    /** default value of the parameter */
    string _defaultValue;

};




#endif /*!cmdPARAM_H*/

/*___oOo___*/
