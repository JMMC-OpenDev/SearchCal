#ifndef cmdPARAM_H
#define cmdPARAM_H
/*******************************************************************************
* JMMC project
*
* "@(#) $Id: cmdPARAM.h,v 1.1 2004-11-19 16:29:38 mella Exp $"
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

    virtual string getName();
    virtual string getDesc();
    virtual string getUnit();
    virtual string getUserValue();
    virtual string getDefaultValue();
    virtual mcsLOGICAL hasDefaultValue();
    virtual mcsLOGICAL isOptional();
    virtual string getHelp();
    virtual mcsCOMPL_STAT setUserValue(string value);
    virtual mcsCOMPL_STAT setDefaultValue(string value);

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
