#ifndef cmdCMD_H
#define cmdCMD_H
/*******************************************************************************
* JMMC project
*
* "@(#) $Id: cmdCMD.h,v 1.4 2004-11-26 08:37:47 mella Exp $"
*
* who       when         what
* --------  -----------  -------------------------------------------------------
* mella     15-Nov-2004  Created
*
*
*******************************************************************************/

/**
 * \file
 * cmdCMD class declaration.
 */


#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif

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
class cmdCMD
{
public:
    // Brief description of the constructor
    cmdCMD(string name, string params);

    // Brief description of the destructor
    virtual ~cmdCMD();

    /** typedef for map of cmdPARAM */
    typedef map<string, cmdPARAM *> STRING2PARAM;

    virtual mcsCOMPL_STAT doParsing();
    virtual string getHelp();
    
    /* methods to handle parameters */
    virtual mcsCOMPL_STAT addParam(cmdPARAM *param);
    virtual mcsCOMPL_STAT getParam(string paramName, cmdPARAM **param);
    virtual mcsLOGICAL isDefined(string paramName);
    virtual mcsLOGICAL hasDefaultValue(string paramName);
    virtual mcsLOGICAL isOptional(string paramName);
    virtual mcsCOMPL_STAT getParamValue(string paramName, mcsINT32 *param);
    virtual mcsCOMPL_STAT getParamValue(string paramName, char **param);
    virtual mcsCOMPL_STAT getParamValue(string paramName, mcsDOUBLE *param);
    virtual mcsCOMPL_STAT getParamValue(string paramName, mcsLOGICAL *param);
    virtual mcsCOMPL_STAT getDefaultParamValue(string paramName, mcsINT32 *param);
    virtual mcsCOMPL_STAT getDefaultParamValue(string paramName, char **param);
    virtual mcsCOMPL_STAT getDefaultParamValue(string paramName, mcsDOUBLE *param);
    virtual mcsCOMPL_STAT getDefaultParamValue(string paramName, mcsLOGICAL *param);

protected:

    
private:
    // Declaration of copy constructor and assignment operator as private
    // methods, in order to hide them from the users.
     cmdCMD(const cmdCMD&);
     cmdCMD& operator=(const cmdCMD&);

     virtual mcsCOMPL_STAT parse();
     
     virtual mcsCOMPL_STAT parseCdf(string cdfFilename);
     virtual mcsCOMPL_STAT parseCdfForDesc(GdomeElement *node);
     virtual mcsCOMPL_STAT parseCdfForParameters(GdomeElement *node);
     virtual mcsCOMPL_STAT parseCdfForParam(GdomeElement *param);
     virtual mcsCOMPL_STAT cmdGetNodeContent(GdomeElement *parentNode, string tagName, string &content);

     virtual mcsCOMPL_STAT parseParams();
     virtual mcsCOMPL_STAT parseTupleParam(string tuple);
     
     virtual mcsCOMPL_STAT checkParams();
     
     virtual mcsCOMPL_STAT setDescription(string desc);

     /** given string to the constructor */
     string _params;
     /** map of params */
     STRING2PARAM _children;
     /** Flag that indicates if the params have been parsed */
     mcsLOGICAL _hasNotBeenYetParsed;
     /** name of the command */
     string _name;
     /** description of the command */
     string _desc;
};

#endif /*!cmdCMD_H*/

/*___oOo___*/
