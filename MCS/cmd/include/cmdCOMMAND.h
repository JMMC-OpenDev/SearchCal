#ifndef cmdCOMMAND_H
#define cmdCOMMAND_H
/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: cmdCOMMAND.h,v 1.14 2005-04-11 12:20:08 scetre Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.13  2005/02/27 19:44:17  gzins
 * Implemented parameter value range check
 *
 * Revision 1.12  2005/02/27 09:27:41  gzins
 * Improved error handling
 *
 * Revision 1.11  2005/02/17 09:03:01  gzins
 * Added GetCmdParamLine method
 * Updated to keep parameter order as defined in CDF
 *
 * Revision 1.10  2005/02/15 10:58:58  gzins
 * Added CVS log as file modification history
 *
 * mella     15-Nov-2004  Created
 * gzins     06-Dec-2004  Renamed _hasNotBeenYetParsed to _hasBeenYetParsed
 * gzins     09-Dec-2004  Added pure virtual Parse() method
 *                        Added cdfFilename argument to previous Parse() method
 * gzins     22-Dec-2004  Added cdfName parameter to constructor
 *                        Removed Parse(void) method
 *                        Renamed GetHelp to GetDescription
 *                        Added GetShortDescription
 * lafrasse  01-Feb-2005  Added GetFirstSentenceOfDescription()
 * lafrasse  02-Feb-2005  Moved GetFirstSentenceOfDescription() code in
 *                        GetShortDescription()
 *
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
     
    /** methods to get command description */
    virtual mcsCOMPL_STAT GetShortDescription(string &desc);
    virtual mcsCOMPL_STAT GetDescription(string &desc);
    
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
