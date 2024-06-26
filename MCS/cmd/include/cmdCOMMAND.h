#ifndef cmdCOMMAND_H
#define cmdCOMMAND_H
/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * \file
 * cmdCOMMAND class declaration.
 */


#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif

#include <list>
#include <map>
#include "cmdPARAM.h"


#include <libgdome/gdome.h>
#include <libxml/parser.h>

/*
 * Class declaration
 */

/**
 * This class is used to get a model for any MCS command. Each command should
 * have a Command Definition File (SAMPLE.cdf). The file cmdDefinitionFile.xsd
 * describes the xml format that must follow each cdf file.
 */
class cmdCOMMAND
{
public:
    // Brief description of the constructor
    cmdCOMMAND(string name, string params, string cdfName="");

    // Brief description of the destructor
    virtual ~cmdCOMMAND();

    /** typedef for map of cmdPARAM */
    typedef std::list<std::pair<string, cmdPARAM *> > STRING2PARAM;

    virtual mcsCOMPL_STAT Parse(string cdfName="");
     
    /** methods to get command descriptions */
    virtual mcsCOMPL_STAT GetShortDescription(string &desc);
    virtual mcsCOMPL_STAT GetDescription(string &desc);
    virtual mcsCOMPL_STAT AppendParamsToVOTable(string &voTable);

    /* methods to handle parameters */
    virtual mcsCOMPL_STAT AddParam(cmdPARAM *param);
    virtual mcsCOMPL_STAT GetParam(string paramName, cmdPARAM **param);
    virtual mcsLOGICAL    IsDefined(string paramName);
    virtual mcsLOGICAL    HasDefaultValue(string paramName);
    virtual mcsLOGICAL    IsOptional(string paramName);
    virtual mcsCOMPL_STAT GetParamValue(string paramName, mcsINT32 *param);
    virtual mcsCOMPL_STAT GetParamValue(string paramName, char **param);
    virtual mcsCOMPL_STAT GetParamValue(string paramName, mcsDOUBLE *param);
    virtual mcsCOMPL_STAT GetParamValue(string paramName, mcsLOGICAL *param);
    virtual mcsCOMPL_STAT GetDefaultParamValue(string paramName, 
                                               mcsINT32 *param);
    virtual mcsCOMPL_STAT GetDefaultParamValue(string paramName, 
                                               char **param);
    virtual mcsCOMPL_STAT GetDefaultParamValue(string paramName,
                                               mcsDOUBLE *param);
    virtual mcsCOMPL_STAT GetDefaultParamValue(string paramName,
                                               mcsLOGICAL *param);

    /* methods to get command parameter line */
    virtual mcsCOMPL_STAT GetCmdParamLine(string &paramLine);

protected:
    
private:
    // Declaration of copy constructor and assignment operator as private
    // methods, in order to hide them from the users.
     cmdCOMMAND(const cmdCOMMAND&);
     cmdCOMMAND& operator=(const cmdCOMMAND&);

     virtual mcsCOMPL_STAT ParseCdf();
     virtual mcsCOMPL_STAT ParseCdfForDesc(GdomeElement *node);
     virtual mcsCOMPL_STAT ParseCdfForParameters(GdomeElement *node);
     virtual mcsCOMPL_STAT ParseCdfForParam(GdomeElement *param);
     virtual mcsCOMPL_STAT CmdGetNodeElement(GdomeElement *parentNode,
                                             string nodeName, 
                                             GdomeElement **element,
                                             mcsLOGICAL isOptional=mcsFALSE);
     virtual mcsCOMPL_STAT CmdGetNodeContent(GdomeElement *parentNode,
                                             string tagName,
                                             string &content,
                                             mcsLOGICAL isOptional=mcsFALSE);

     virtual mcsCOMPL_STAT ParseParams();
     virtual mcsCOMPL_STAT ParseTupleParam(string tuple);
     
     virtual mcsCOMPL_STAT CheckParams();
     
     virtual mcsCOMPL_STAT SetDescription(string desc);

     virtual STRING2PARAM::iterator FindParam(string name);

     /** Flag that indicates if the params have been parsed */
     mcsLOGICAL _hasBeenYetParsed;
     /** Flag that indicates if the cdf has been parsed */
     mcsLOGICAL _cdfHasBeenYetParsed;
     /** name of the command */
     string _name;
     /** parameters of the command */
     string _params;
     /** map of params */
     STRING2PARAM _paramList;
     /** command definition file name */
     string _cdfName;
     /** description of the command */
     string _desc;
};

#endif /*!cmdCOMMAND_H*/

/*___oOo___*/
