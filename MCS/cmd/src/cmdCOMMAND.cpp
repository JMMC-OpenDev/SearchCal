/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: cmdCOMMAND.cpp,v 1.28 2005-03-08 09:46:48 gzins Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.27  2005/02/27 19:44:17  gzins
 * Implemented parameter value range check
 *
 * Revision 1.26  2005/02/27 09:27:41  gzins
 * Improved error handling
 *
 * Revision 1.25  2005/02/23 11:15:51  mella
 * Reorder code , add an error to before failure return and add one missing unref_nl into GetNodeContent
 *
 * Revision 1.24  2005/02/23 07:36:09  mella
 * Place variable declaration into functionnal blocks
 *
 * Revision 1.23  2005/02/22 12:43:53  mella
 * Reduce some error dues to bad unref and add some user returned informations
 *
 * Revision 1.22  2005/02/17 09:03:01  gzins
 * Added GetCmdParamLine method
 * Updated to keep parameter order as defined in CDF
 *
 * Revision 1.21  2005/02/15 11:02:48  gzins
 * Changed SUCCESS/FAILURE to mcsSUCCESS/mcsFAILURE
 *
 * Revision 1.20  2005/02/15 10:58:58  gzins
 * Added CVS log as file modification history
 *
 * Revision 1.19  2005/02/03 13:54:52  mella
 * Replace all c_str by data for strings
 * Correct bug with defaultValue that wasn't returned
 *
 * Revision 1.18  2005/02/03 11:13:26  mella
 * GetParamValue tries to return the default value if no user value was defined
 *
 * Revision 1.17  2005/02/02 14:19:46  lafrasse
 * Moved GetFirstSenteceOfDescription() code in GetShortDescription()
 *
 * Revision 1.16  2005/02/01 12:52:32  lafrasse
 * Refined the command and parameter descriptions
 *
 * Revision 1.15  2005/01/26 10:51:30  gzins
 * Added CVS log as modification history.
 * Re-formated command short description.
 *
 * mella     15-Nov-2004  Created
 * gzins     06-Dec-2004  Renamed _hasNotBeenYetParsed to _hasBeenYetParsed and
 *                        fixed bug related to flag check
 * gzins     09-Dec-2004  Fixed cast problem with new mcsLOGICAL enumerate
 * gzins     09-Dec-2004  Added cdfFilename argument to Parse() method
 * gzins     10-Dec-2004  Resolved path before loading CDF file in ParseCdf()
 * gzins     15-Dec-2004  Added error handling
 * gzins     22-Dec-2004  Added cdfName parameter to constructor
 *                        Removed Parse(void) method
 *                        Renamed GetHelp to GetDescription
 *                        Added GetShortDescription
 *
 ******************************************************************************/

/**
 * \file
 * cmdCOMMAND class definition.
 * \todo get Default value from CDF
 * \todo perform better check for argument parsing
 */

static char *rcsId="@(#) $Id: cmdCOMMAND.cpp,v 1.28 2005-03-08 09:46:48 gzins Exp $"; 
static void *use_rcsId = ((void)&use_rcsId,(void *) &rcsId);


/* 
 * System Headers 
 */
#include <iostream>
using namespace std;


/*
 * MCS Headers 
 */
#include "mcs.h"
#include "misc.h"
#include "log.h"
#include "err.h"


/*
 * Local Headers 
 */
#include "cmd.h"
#include "cmdCOMMAND.h"
#include "cmdPrivate.h"
#include "cmdErrors.h"

/*
 * Class constructor
 */

/** 
 *  Constructs a new command.
 *
 * \param name  the name of the command (mnemonic).
 * \param params  the arguments of the command.
 * \param cdfName the name of the command definition file.
 *
 */
cmdCOMMAND::cmdCOMMAND(string name, string params, string cdfName)
{
    logExtDbg("cmdCOMMAND::cmdCOMMAND()");
    _hasBeenYetParsed = mcsFALSE;
    _cdfHasBeenYetParsed = mcsFALSE;
    _name = name;
    _params = params;
    _cdfName = cdfName;
}


/*
 * Class destructor
 */

/** 
 *  Destructor.
 *
 *  \returns an MCS completion status code (mcsSUCCESS or mcsFAILURE)
 */
cmdCOMMAND::~cmdCOMMAND()
{
    logExtDbg("cmdCOMMAND::~cmdCOMMAND()");
    
    // For each parameter entry: delete each object associated to the pointer object
    if (_paramList.size()>0)
    {
        STRING2PARAM::iterator i;
        for(i = _paramList.begin(); i != _paramList.end(); i++)
        {
            delete (i->second);
        }
    }

    // \todo delete _paramList ...
    _paramList.clear();
}


/*
 * Public methods
 */

/** 
 * Parse the command parameters.
 *
 * This method loads the command definition file (CDF) given as argument,
 * parses the command parameters and then check the parameters against the
 * CDF.
 * It calls  parseCdf() and  parseParams().
 *
 * \param cdfName  the CDF file name.
 * 
 * \returns an MCS completion status code (mcsSUCCESS or mcsFAILURE)
 */
mcsCOMPL_STAT cmdCOMMAND::Parse(string cdfName)
{
    logExtDbg ("cmdCOMMAND::Parse()");
    
    // If command has been already parsed, return
    if ( _hasBeenYetParsed == mcsTRUE)
    {
        return mcsSUCCESS;
    }

   
    // Use the given CDF (if any)
    if (cdfName.size() != 0)
    {
        _cdfName = cdfName;
    }
  
    if (ParseCdf()==mcsFAILURE)
    {
        errAdd (cmdERR_PARSE_CDF, _cdfName.data(), _name.data());
        return mcsFAILURE;
    }
    
    if (ParseParams() == mcsFAILURE)
    {
        errAdd (cmdERR_PARSE_PARAMETERS, _params.data(), _name.data());
        return mcsFAILURE;
    }

    if (CheckParams() == mcsFAILURE)
    {
        return mcsFAILURE;
    }
        
    // And flag a right performed parsing only after this point
    _hasBeenYetParsed = mcsTRUE;
    return mcsSUCCESS;
}

/** 
 *  Return the first sentence of the complete description of the command.
 *
 *  \returns the short description string.
 */
mcsCOMPL_STAT cmdCOMMAND::GetShortDescription(string &desc)
{
    logExtDbg ("cmdCOMMAND::GetShortDescription()");

    // Clear description
    desc.clear();

    // If there is no CDF for this command
    if (_cdfName.size() == 0 )
    {
        desc.append("No description available.");
    }
    else
    {
        // Parse the CDF file to obtain full description
        if (ParseCdf() == mcsFAILURE)
        {
            return mcsFAILURE;
        }

        if (_desc.empty())
        {
            // If no description does exist.
            desc.append("No description found in CDF.");
        }
        else
        {
            // Get only the first sentence of the main description.
            unsigned int end = _desc.find_first_of(".\n");

            // If dot is found
            if (end != string::npos)
            {
                // To include the dot
                end++;
            }
            else
            {
                _desc.append(".");
                end = _desc.length() + 1;
            }

            desc.append(_desc.substr(0, end));
        }
    }

    return mcsSUCCESS;
}

/** 
 *  Return the detailed description of the command and its parameters.
 *
 *  \returns the detailed description string.
 */
mcsCOMPL_STAT cmdCOMMAND::GetDescription(string &desc)
{
    logExtDbg ("cmdCOMMAND::GetDescription()");

    // Clear recipient
    desc.clear();

    string synopsis;
    string description;
    string options;

    // If there is no CDF for this command
    if (_cdfName.size() == 0 )
    {
        synopsis.append("There is no help available for '");
        synopsis.append(_name);
        synopsis.append("' command.");
    }
    else
    {
        // Parse the CDF file to obtain full description
        if (ParseCdf()==mcsFAILURE)
        {
            return mcsFAILURE;
        }

        // Append the command name
        synopsis.append("NAME\n\t");
        synopsis.append(_name);

        // Append the first sentence of the command description
        string shortSentence;
        if (GetShortDescription(shortSentence) ==
            mcsFAILURE)
        {
            return mcsFAILURE;
        }
        synopsis.append(" - ");
        synopsis.append(shortSentence);

        // Append description of command
        if (_desc.empty())
        {
            description.append("No description found.");
        }
        else
        {
            description.append("\n\nDESCRIPTION\n\t");
            description.append(_desc);
        }

        synopsis.append("\n\nSYNOPSIS\n\t");
        synopsis.append(_name);

        // Append help for each parameter if any
        options.append("\n\nPARAMETERS\n");
        if (_paramList.size() > 0)
        {
            STRING2PARAM::iterator i = _paramList.begin();
            while(i != _paramList.end())
            {
                cmdPARAM * child = i->second;

                synopsis.append(" ");
                if (child->IsOptional())
                {
                    synopsis.append("[");
                }

                synopsis.append("-");
                synopsis.append(child->GetName());
                synopsis.append(" <");
                synopsis.append(child->GetType());
                synopsis.append(">");

                if (child->IsOptional())
                {
                    synopsis.append("]");
                }

                string childHelp = child->GetHelp();
                if ( ! childHelp.empty() )
                {
                    options.append(childHelp);
                    options.append("\n");
                }
                i++;
            }
        }
        else
        {
            options.append("\tThis command takes no parameter.\n");
        }
    }

    desc.append(synopsis);
    desc.append(description);
    desc.append(options);

    return mcsSUCCESS;
}

/** 
 *  Add a new parameter to the command.
 *
 * \param param the parameter to add.
 *
 *  \returns an MCS completion status code (mcsSUCCESS or mcsFAILURE)
 */
mcsCOMPL_STAT cmdCOMMAND::AddParam(cmdPARAM *param)
{
    logExtDbg ("cmdCOMMAND::AddParam()");
    _paramList.push_back( make_pair(param->GetName(), param) );
    return mcsSUCCESS;
}

/** 
 *  Get the parameter associated to paramName. This method must not be called
 *  during parsing steps because it begins to check if it has been parsed.
 *
 * \param paramName  the name of the requested parameter.
 * \param param  a pointer where to store the parameter pointer
 *
 *  \returns an MCS completion status code (mcsSUCCESS or mcsFAILURE)
 *  param should be considered valid only on mcsSUCCESS case.
 */
mcsCOMPL_STAT cmdCOMMAND::GetParam(string paramName, cmdPARAM **param)
{
    logExtDbg ("cmdCOMMAND::GetParam()");
    
    // Parse parameter list 
    if (Parse() == mcsFAILURE )
    {
        return mcsFAILURE;
    }
 
    // Get parameter from list
    STRING2PARAM::iterator iter = FindParam(paramName);
  
    // If found
    if (iter!= _paramList.end())
    {
        // Return parameter value
        *param = iter->second;
        return mcsSUCCESS;
    }
    // Else
    else
    {
        // Handle error
        errAdd(cmdERR_PARAM_UNKNOWN, paramName.data(), _name.data());
        return mcsFAILURE;
    }
    // End if
}

/** 
 *  Indicates if the parameter is defined by the user parameters.
 *
 * \param paramName the name of the parameter. 
 *
 *  \returns mcsFALSE or mcsTRUE
 */
mcsLOGICAL cmdCOMMAND::IsDefined(string paramName)
{
    logExtDbg("cmdCOMMAND::IsDefined()");
    cmdPARAM *p;
    if (GetParam(paramName, &p) == mcsFAILURE )
    {
        logWarning("%s parameter doesn't exist",paramName.data());
        return mcsFALSE;
    }
    return p->IsDefined();
}

/** 
 *  Indicates if the parameter has a defaultValue.
 *
 * \param paramName the name of the parameter. 
 *
 *  \returns mcsFALSE or mcsTRUE
 */
mcsLOGICAL cmdCOMMAND::HasDefaultValue(string paramName)
{
    logExtDbg("cmdCOMMAND::HasDefaultValue()");
    cmdPARAM *p;
    if (GetParam(paramName, &p) == mcsFAILURE )
    {
        logWarning("%s parameter doesn't exist",paramName.data());
        return mcsFALSE;
    }
    return p->HasDefaultValue();
}

/** 
 *  Indicates if the parameter is optional.
 *
 * \param paramName the name of the parameter. 
 *
 *  \returns mcsFALSE or mcsTRUE
 */
mcsLOGICAL cmdCOMMAND::IsOptional(string paramName)
{
    logExtDbg("cmdCOMMAND::IsOptional()");
    cmdPARAM *p;
    if (GetParam(paramName, &p) == mcsFAILURE )
    {
        logWarning("%s parameter doesn't exist",paramName.data());
        return mcsFALSE;
    }
    return p->IsOptional();
}

/** 
 *  Get the value of a parameter. Begin to return the user value. If the parameter is not defined by the user, the default
 *  is returned if it exist. If the parameter has neither user value nor default
 *  value, then an error is returned.
 *
 * \param paramName  the name of the parameter.
 * \param param  the storage data pointer.
 *
 *  \returns an MCS completion status code (mcsSUCCESS or mcsFAILURE)
 */
mcsCOMPL_STAT cmdCOMMAND::GetParamValue(string paramName, mcsINT32 *param)
{
    logExtDbg("cmdCOMMAND::GetParamValue()");
    cmdPARAM *p;

    // Check if parameter does exit
    if (GetParam(paramName, &p) == mcsFAILURE )
    {   
        return mcsFAILURE;
    }

    // If user value is given
    if (p->IsDefined() == mcsTRUE)
    {
        // Return the user value
        return (p->GetUserValue(param));
    }
    // Else 
    else
    {
        // If a default value exist
        if (p->HasDefaultValue() == mcsTRUE )
        {
            // Return the default value
            return p->GetDefaultValue(param);    
        }
    }

    // Finally return an error
    errAdd(cmdERR_MISSING_PARAM, paramName.data(), _name.data()); 
    return mcsFAILURE;   
}

/** 
 *  Get the value of a parameter. Begin to return the user value. If the parameter is not defined by the user, the default
 *  is returned if it exist. If the parameter has neither user value nor default
 *  value, then an error is returned.
 *  Get the user value of a parameter.
 *
 * \param paramName  the name of the parameter.
 * \param param  the storage data pointer.
 *
 *  \returns an MCS completion status code (mcsSUCCESS or mcsFAILURE)
 */
mcsCOMPL_STAT cmdCOMMAND::GetParamValue(string paramName, char **param)
{
    logExtDbg("cmdCOMMAND::GetParamValue()");
    cmdPARAM *p;

    // Check if parameter does exit
    if (GetParam(paramName, &p) == mcsFAILURE )
    {   
        return mcsFAILURE;
    }

    // If user value is given
    if (p->IsDefined() == mcsTRUE)
    {
        // Return the user value
        return (p->GetUserValue(param));
    }
    else
    {
        // Else return the default value
        if (p->HasDefaultValue() == mcsTRUE )
        {
            return p->GetDefaultValue(param);    
        }
    }

    // Finally return an error
    errAdd(cmdERR_MISSING_PARAM, paramName.data(), _name.data()); 
    return mcsFAILURE;    
}

/** 
 *  Get the value of a parameter. Begin to return the user value. If the parameter is not defined by the user, the default
 *  is returned if it exist. If the parameter has neither user value nor default
 *  value, then an error is returned.
 *
 * \param paramName  the name of the parameter.
 * \param param  the storage data pointer.
 *
 *  \returns an MCS completion status code (mcsSUCCESS or mcsFAILURE)
 */
mcsCOMPL_STAT cmdCOMMAND::GetParamValue(string paramName, mcsDOUBLE *param)
{
    logExtDbg("cmdCOMMAND::GetParamValue()");
    cmdPARAM *p;

    // Check if parameter does exit
    if (GetParam(paramName, &p) == mcsFAILURE )
    {   
        return mcsFAILURE;
    }

    // If user value is given
    if (p->IsDefined() == mcsTRUE)
    {
        // Return the user value
        return (p->GetUserValue(param));
    }
    else
    {
        // Else return the default value
        if (p->HasDefaultValue() == mcsTRUE )
        {
            return p->GetDefaultValue(param);    
        }
    }

    // Finally return an error
    errAdd(cmdERR_MISSING_PARAM, paramName.data(), _name.data()); 
    return mcsFAILURE;   
}

/** 
 *  Get the value of a parameter. Begin to return the user value. If the parameter is not defined by the user, the default
 *  is returned if it exist. If the parameter has neither user value nor default
 *  value, then an error is returned.
 *
 * \param paramName  the name of the parameter.
 * \param param  the storage data pointer.
 *
 *  \returns an MCS completion status code (mcsSUCCESS or mcsFAILURE)
 */
mcsCOMPL_STAT cmdCOMMAND::GetParamValue(string paramName, mcsLOGICAL *param)
{
    logExtDbg("cmdCOMMAND::GetParamValue()");
    cmdPARAM *p;

    // Check if parameter does exit
    if (GetParam(paramName, &p) == mcsFAILURE )
    {   
        return mcsFAILURE;
    }

    // If user value is given
    if (p->IsDefined() == mcsTRUE)
    {
        // Return the user value
        return (p->GetUserValue(param));
    }
    else
    {
        // Else return the default value
        if (p->HasDefaultValue() == mcsTRUE )
        {
            return p->GetDefaultValue(param);    
        }
    }

    // Finally return an error
    errAdd(cmdERR_MISSING_PARAM, paramName.data(), _name.data()); 
    return mcsFAILURE;   
}

/** 
 *  Get the value of a parameter. Begin to return the user value. If the parameter is not defined by the user, the default
 *  is returned if it exist. If the parameter has neither user value nor default
 *  value, then an error is returned.
 *
 * \param paramName  the name of the parameter.
 * \param param  the storage data pointer.
 *
 *  \returns an MCS completion status code (mcsSUCCESS or mcsFAILURE)
 */
mcsCOMPL_STAT cmdCOMMAND::GetDefaultParamValue(string paramName, mcsINT32 *param)
{
    logExtDbg("cmdCOMMAND::GetParamValue()");
    cmdPARAM *p;
    if (GetParam(paramName, &p) == mcsFAILURE )
    {
        return mcsFAILURE;
    }
    return p->GetDefaultValue(param);
}

/** 
 *  Get the default value of a parameter.
 *
 * \param paramName  the name of the parameter.
 * \param param  the storage data pointer.
 *
 *  \returns an MCS completion status code (mcsSUCCESS or mcsFAILURE)
 */
mcsCOMPL_STAT cmdCOMMAND::GetDefaultParamValue(string paramName, char **param)
{
    logExtDbg("cmdCOMMAND::GetParamValue()");
    cmdPARAM *p;
    if (GetParam(paramName, &p) == mcsFAILURE )
    {
        return mcsFAILURE;
    }
    return p->GetDefaultValue(param);
}

/** 
 *  Get the default value of a parameter.
 *
 * \param paramName  the name of the parameter.
 * \param param  the storage data pointer.
 *
 *  \returns an MCS completion status code (mcsSUCCESS or mcsFAILURE)
 */
mcsCOMPL_STAT cmdCOMMAND::GetDefaultParamValue(string paramName, mcsDOUBLE *param)
{
    logExtDbg("cmdCOMMAND::GetParamValue()");
    cmdPARAM *p;
    if (GetParam(paramName, &p) == mcsFAILURE )
    {
        return mcsFAILURE;
    }
    return p->GetDefaultValue(param);
}

/** 
 *  Get the default value of a parameter.
 *
 * \param paramName  the name of the parameter.
 * \param param  the storage data pointer.
 *
 *  \returns an MCS completion status code (mcsSUCCESS or mcsFAILURE)
 */
mcsCOMPL_STAT cmdCOMMAND::GetDefaultParamValue(string paramName, mcsLOGICAL *param)
{
    logExtDbg("cmdCOMMAND::GetParamValue()");
    cmdPARAM *p;
    if (GetParam(paramName, &p) == mcsFAILURE )
    {
        return mcsFAILURE;
    }
    return p->GetDefaultValue(param);
}

/** 
 * Get the command parameter line.
 *
 * \param paramLine string in which the list of parameters with associated value
 * will be stored.
 *
 *  \returns an MCS completion status code (mcsSUCCESS or mcsFAILURE)
 */
mcsCOMPL_STAT cmdCOMMAND::GetCmdParamLine(string &paramLine)
{
    logExtDbg("cmdCOMMAND::GetCmdParamLine()");

    // Parse parameter list 
    if (Parse() == mcsFAILURE )
    {
        return mcsFAILURE;
    }

    if (_paramList.size() > 0)
    {
        STRING2PARAM::iterator i = _paramList.begin();
        while(i != _paramList.end())
        {
            cmdPARAM * child = i->second;

            if (child->IsOptional())
            {
                if (child->IsDefined())
                {
                    paramLine.append("-");
                    paramLine.append(child->GetName());
                    paramLine.append(" ");
                    paramLine.append(child->GetUserValue());
                    paramLine.append(" ");
                }
            }
            else
            {
                if (child->IsDefined())
                {
                    paramLine.append("-");
                    paramLine.append(child->GetName());
                    paramLine.append(" ");
                    paramLine.append(child->GetUserValue());
                    paramLine.append(" ");
                }
                else if (child->HasDefaultValue())
                {
                    paramLine.append("-");
                    paramLine.append(child->GetName());
                    paramLine.append(" ");
                    paramLine.append(child->GetDefaultValue());
                    paramLine.append(" ");
                }
            }

            i++;
        }
    }

    return mcsSUCCESS;
}

/*
 * Private methods
 */

/** 
 *  Parse a CDF file and build new parameters for the command.
 *  After this step the parameters can be parsed.
 *
 *  \returns an MCS completion status code (mcsSUCCESS or mcsFAILURE)
 */
mcsCOMPL_STAT cmdCOMMAND::ParseCdf()
{
    logExtDbg ("cmdCOMMAND::ParseCdf()");

    // If the CDF has been already parsed, return
    if ( _cdfHasBeenYetParsed == mcsTRUE)
    {
        return mcsSUCCESS;
    }


    GdomeDOMImplementation *domimpl;
    GdomeDocument *doc;
    GdomeElement *root=NULL;
    GdomeException exc;
  
    // Check that a command definition file name has been given
    if ( _cdfName.size() == 0 )
    {
        errAdd(cmdERR_NO_CDF, _name.data());
        return mcsFAILURE;
    }
 
    // Find the correcsponding CDF file
    char * fullCdfFilename = miscLocateFile(_cdfName.data());
    // Check if the CDF file has been found   
    if (fullCdfFilename == NULL)
    {
        errAdd(cmdERR_NO_CDF, _cdfName.data());
        return mcsFAILURE;
    }

    // Get a DOMImplementation reference
    domimpl = gdome_di_mkref ();

    // Create a new Document from the CDF file
    const char *xmlFilename = miscResolvePath(fullCdfFilename);
    logDebug("Using CDF file %s",xmlFilename);
    doc = gdome_di_createDocFromURI(domimpl, xmlFilename, GDOME_LOAD_PARSING,
                                    &exc);
    if (doc == NULL)
    {
        errAdd (cmdERR_CDF_FORMAT, xmlFilename, exc);
        goto errCond;
    }

    // Get reference to the root element of the document
    root = gdome_doc_documentElement (doc, &exc);
    if (root == NULL) 
    {
        errAdd (cmdERR_CDF_FORMAT, xmlFilename, exc);
        goto errCond;
    }

    // Parse for Description
    if (ParseCdfForDesc(root)==mcsFAILURE)
    {
        goto errCond;
    }
    
    // Parse for Parameters
    if (ParseCdfForParameters(root)==mcsFAILURE)
    {
        goto errCond;
    }

    // Free the document structure and the DOMImplementation
    gdome_el_unref(root, &exc);
    gdome_doc_unref (doc, &exc);
    gdome_di_unref (domimpl, &exc);
    xmlCleanupParser();
    _cdfHasBeenYetParsed = mcsTRUE;
    return mcsSUCCESS;

errCond:
    // Free the document structure and the DOMImplementation
    gdome_el_unref(root, &exc);
    gdome_doc_unref (doc, &exc);
    gdome_di_unref (domimpl, &exc);
    xmlCleanupParser();
    errAdd (cmdERR_PARSE_CDF, fullCdfFilename, _name.data());
    return mcsFAILURE;
}

/** 
 *  Parse the CDF document to extract description.
 *
 * \param root  the root node of the CDF document.
 *
 *  \returns an MCS completion status code (mcsSUCCESS or mcsFAILURE)
 */
mcsCOMPL_STAT cmdCOMMAND::ParseCdfForDesc(GdomeElement *root)
{
    logExtDbg ("cmdCOMMAND::ParseCdfForDesc()");

    GdomeNodeList *nl;
    GdomeException exc;
    GdomeDOMString *name, *value;
    int nbChildren;
    
    name = gdome_str_mkref ("desc");
    // Get the reference to the childrens NodeList of the root element
    nl = gdome_el_getElementsByTagName (root, name, &exc);
    if (nl == NULL)
    {
        errAdd (cmdERR_CDF_FORMAT_ELEMENT, name->str, exc);
        gdome_str_unref(name);
        return mcsFAILURE;
    }
 
    nbChildren = gdome_nl_length (nl, &exc);

    // If a desc does exist get first item (xsd assumes there is only one desc
    if (nbChildren > 0)
    {
        GdomeElement *el,*el2;
        el = (GdomeElement *)gdome_nl_item (nl, 0, &exc);
        if (el == NULL)
        {
            errAdd (cmdERR_CDF_FORMAT_ITEM, 0, name->str, exc);
            gdome_str_unref(name);
            gdome_nl_unref(nl, &exc);
            return mcsFAILURE;
        }
        el2=(GdomeElement *)gdome_el_firstChild(el, &exc);
        if (el2 == NULL)
        {
            errAdd (cmdERR_CDF_FORMAT_CONTENT, name->str, exc);
            gdome_str_unref(name);
            gdome_el_unref(el2, &exc);
            gdome_nl_unref(nl, &exc);
            return mcsFAILURE;
        }
        
        value=gdome_el_nodeValue(el2,&exc);
        SetDescription(value->str);
        gdome_str_unref(value);
        gdome_el_unref(el2, &exc);
        gdome_el_unref(el, &exc);
    }
    
    gdome_str_unref(name);
    gdome_nl_unref(nl, &exc);
    
    return mcsSUCCESS;
}

/** 
 *  Parse the CDF document to extract the parameters.
 *
 * \param root  the root node of the CDF document.
 *
 *  \returns an MCS completion status code (mcsSUCCESS or mcsFAILURE)
 */
mcsCOMPL_STAT cmdCOMMAND::ParseCdfForParameters(GdomeElement *root)
{
    logExtDbg ("cmdCOMMAND::ParseCdfForParameters()");
    
    GdomeNodeList *params_nl;
    GdomeException exc;
    GdomeDOMString *name;
    int nbChildren;
    name = gdome_str_mkref ("params");
    // Get the reference to the params childrens of the root element
    params_nl = gdome_el_getElementsByTagName (root, name, &exc);
    if (params_nl == NULL)
    {
        errAdd (cmdERR_CDF_FORMAT_ELEMENT, name->str, exc);
        gdome_str_unref(name);
        return mcsFAILURE;
    }
    gdome_str_unref(name);
 
    nbChildren = gdome_nl_length (params_nl, &exc);
    if (nbChildren == 1)    // If params does exist
    {
        GdomeElement *params_el;
        GdomeNodeList *param_nl;
        params_el = (GdomeElement *)gdome_nl_item (params_nl, 0, &exc);
        if (params_el == NULL)
        {
            errAdd (cmdERR_CDF_FORMAT_ITEM, 0, name->str, exc);
            gdome_str_unref(name);
            gdome_nl_unref(params_nl, &exc);
            return mcsFAILURE;
        }

        // Get the reference to the list of param elements
        name = gdome_str_mkref ("param");
        param_nl = gdome_el_getElementsByTagName (params_el, name, &exc);
        if (param_nl == NULL)
        {
            errAdd (cmdERR_CDF_FORMAT_ELEMENT, name->str, exc);
            gdome_str_unref(name);
            gdome_el_unref(params_el, &exc);
            gdome_nl_unref(params_nl, &exc);
            return mcsFAILURE;
        }

        nbChildren = gdome_nl_length (param_nl, &exc);
        // If param has children
        if (nbChildren > 0)
        {  
            int i;
            for (i=0; i<nbChildren; i++)
            {
                GdomeElement *param_el;
                param_el = (GdomeElement *)gdome_nl_item (param_nl, i, &exc);
                if (param_el == NULL)
                {
                    errAdd (cmdERR_CDF_FORMAT_ITEM, i, name->str, exc);
                    gdome_str_unref(name);
                    gdome_el_unref(params_el, &exc);
                    gdome_nl_unref(param_nl, &exc);
                    gdome_nl_unref(params_nl, &exc);
                    return mcsFAILURE;
                }

                if (ParseCdfForParam(param_el) == mcsFAILURE)
                {
                    gdome_str_unref(name);
                    gdome_el_unref(param_el, &exc);
                    gdome_el_unref(params_el, &exc);
                    gdome_nl_unref(param_nl, &exc);
                    gdome_nl_unref(params_nl, &exc);
                    return mcsFAILURE;
                }
                gdome_el_unref(param_el, &exc);
            }
        }
        gdome_str_unref(name);
        gdome_nl_unref(param_nl, &exc);
        gdome_el_unref(params_el, &exc);
    }
    gdome_nl_unref(params_nl, &exc);

    return mcsSUCCESS;
}

/** 
 *  Parse the CDF document to extract the parameters.
 *
 *  \param param  one param node of the CDF document.
 *
 *  \returns an MCS completion status code (mcsSUCCESS or mcsFAILURE)
 */
mcsCOMPL_STAT cmdCOMMAND::ParseCdfForParam(GdomeElement *param)
{
    logExtDbg ("cmdCOMMAND::ParseCdfForParam()");
    string name;
    string desc;
    string type;
    string defaultValue;
    string minValue;
    string maxValue;
    string unit;
    mcsLOGICAL optional;

    // Get mandatory name
    if (CmdGetNodeContent(param, "name", name) == mcsFAILURE)
    {
        return mcsFAILURE;   
    }
    // Get mandatory type
    if (CmdGetNodeContent(param, "type", type) == mcsFAILURE)
    {
        return mcsFAILURE;   
    }
    
    // Get optional description
    if (CmdGetNodeContent(param, "desc", desc, mcsTRUE) == mcsFAILURE)
    {
        return mcsFAILURE;   
    }
    // Get optional unit
    if (CmdGetNodeContent(param, "unit", unit, mcsTRUE) == mcsFAILURE)
    {
        return mcsFAILURE;   
    }
    // Get optional defaultValue
    GdomeElement *el;
    GdomeException exc;
    if (CmdGetNodeElement(param, "defaultValue", &el, mcsTRUE) == mcsFAILURE)
    { 
        return mcsFAILURE;   
    }
    else
    {
        if (el != NULL)
        {
            if (CmdGetNodeContent(el, type, defaultValue) == mcsFAILURE )
            {
                gdome_el_unref(el, &exc);
                return mcsFAILURE;   
            }
        }
        else
        {
            // There should not be any defaultValue
            logDebug("No defaultValue for %.40s parameter", name.data());
        }
    }
    gdome_el_unref(el, &exc);

    // Get optional minValue
    if (CmdGetNodeElement(param, "minValue", &el, mcsTRUE) == mcsFAILURE)
    { 
        return mcsFAILURE;   
    }
    else
    {
        if (el != NULL)
        {
            if (CmdGetNodeContent(el, type, minValue) == mcsFAILURE )
            {
                gdome_el_unref(el, &exc);
                return mcsFAILURE;   
            }
        }
        else
        {
            // There should not be any defaultValue
            logDebug("No minValue for %.40s parameter", name.data());
        }
    }
    gdome_el_unref(el, &exc);

    // Get optional maxValue
    if (CmdGetNodeElement(param, "maxValue", &el, mcsTRUE) == mcsFAILURE)
    { 
        return mcsFAILURE;   
    }
    else
    {
        if (el != NULL)
        {
            if (CmdGetNodeContent(el, type, maxValue) == mcsFAILURE )
            {
                gdome_el_unref(el, &exc);
                return mcsFAILURE;   
            }
        }
        else
        {
            // There should not be any defaultValue
            logDebug("No maxValue for %.40s parameter", name.data());
        }
    }
    gdome_el_unref(el, &exc);

    // Check if it is an optional parameter
    { 
        GdomeAttr *attribute;
        GdomeException exc;
        GdomeDOMString *attrName,*attrValue, *str, *str2;

        attrName = gdome_str_mkref ("optional");
        // Get the reference to the optional element
        attribute = gdome_el_getAttributeNode (param, attrName, &exc);
        gdome_str_unref(attrName);
        if (attribute == NULL)
        {
            // By default it is not optional.
            optional = mcsFALSE;
        }
        else
        {
            attrValue = gdome_a_nodeValue(attribute, &exc);

            str = gdome_str_mkref ("true");
            str2 = gdome_str_mkref ("1");
            if ( gdome_str_equal(attrValue, str) )
            {
                optional = mcsTRUE;
            }
            else if ( gdome_str_equal(attrValue,str2) )
            {
                optional =mcsTRUE;
            }
            else
            {
                optional = mcsFALSE;
            }
            
            gdome_str_unref(str);
            gdome_str_unref(str2);
            gdome_str_unref(attrValue);
            gdome_a_unref(attribute, &exc);
        }
    }
    
    // Create the new Parameter and add it to the inner list of parameters
    cmdPARAM *p = new cmdPARAM(name, desc, type, unit, optional);
    if (! minValue.empty())
    {
        if (p->SetMinValue(minValue) == mcsFAILURE)
        {
            return mcsFAILURE;   
        }
    }
    if (! maxValue.empty())
    {
        if (p->SetMaxValue(maxValue) == mcsFAILURE)
        {
            return mcsFAILURE;   
        }
    }
    if (! defaultValue.empty())
    {
        if (p->SetDefaultValue(defaultValue) == mcsFAILURE)
        {
            return mcsFAILURE;   
        }
    }
    AddParam(p);

    return mcsSUCCESS;
}

mcsCOMPL_STAT cmdCOMMAND::CmdGetNodeElement(GdomeElement *parentNode,
                                            string nodeName, 
                                            GdomeElement **element,
                                            mcsLOGICAL isOptional)
{
    GdomeNodeList *nl;
    GdomeException exc;
    GdomeDOMString *name;
    int nbChildren;
    // Get the reference to the node elements
    name = gdome_str_mkref(nodeName.data());
    nl = gdome_el_getElementsByTagName (parentNode, name, &exc);
    if (nl == NULL)
    {
        errAdd (cmdERR_CDF_FORMAT_ELEMENT, name->str, exc);
        gdome_str_unref(name);
        gdome_nl_unref(nl, &exc);
        return mcsFAILURE;
    }
    nbChildren = gdome_nl_length (nl, &exc);

    // If there is element in list
    if (nbChildren > 0)
    {
        // Get first element
        *element = (GdomeElement *)gdome_nl_item (nl, 0, &exc);
        if (*element == NULL)
        {
            errAdd (cmdERR_CDF_FORMAT_ITEM, 0, name->str, exc);
            gdome_str_unref(name);
            gdome_nl_unref(nl, &exc);
            return mcsFAILURE;
        } 
    }
    // Else if element is not optional
    else if (isOptional == mcsFALSE)
    {
        // Return error
        errAdd (cmdERR_CDF_NO_NODE_ELEMENT, name->str);
        gdome_str_unref(name);
        gdome_nl_unref(nl, &exc);
        return mcsFAILURE; 
    }
    else
    {
        // Else return NULL element
        *element = NULL;
        gdome_str_unref(name);
        gdome_nl_unref(nl, &exc);
        return mcsSUCCESS;
    }

    gdome_str_unref(name);
    gdome_nl_unref(nl, &exc);

    return mcsSUCCESS;
}

/** 
 * Get the content of the first child node using tagName. If tagName is empty
 * the first child is used without any care of the child's tag.
 *
 * \param parentNode the parent node. 
 * \param tagName  the tag of the child or empty.
 * \param content  the storage string.
 * \param isOptional specify whether node is an optional in the CDF or not.
 *
 *  \returns an MCS completion status code (mcsSUCCESS or mcsFAILURE)
 */
mcsCOMPL_STAT cmdCOMMAND::CmdGetNodeContent(GdomeElement *parentNode,
                                            string tagName, string &content,
                                            mcsLOGICAL isOptional)
{
    logExtDbg("cmdCOMMAND::CmdGetNodeContent()");
    
    GdomeNodeList *nl;
    GdomeException exc;
    GdomeDOMString *name;
    int nbChildren;
    
    name = gdome_str_mkref (tagName.data());
    // Get the reference to the named childrens of the parentNode element
    nl = gdome_el_getElementsByTagName (parentNode, name, &exc);

    if (nl == NULL)
    {
        errAdd (cmdERR_CDF_FORMAT_ELEMENT, name->str, exc);
        gdome_str_unref(name);
        return mcsFAILURE;
    }

    nbChildren = gdome_nl_length (nl, &exc);

    // Inform that we are maybe missing some data
    if (nbChildren > 1)
    {
        logWarning("Only use the first children but %d are present",nbChildren);
    }
    // If one or more children do exist work*/
    if (nbChildren > 0)
    {
        GdomeElement *el, *el2;
        GdomeDOMString *value;
        el = (GdomeElement *)gdome_nl_item (nl, 0, &exc);
        if (el == NULL)
        {
            errAdd (cmdERR_CDF_FORMAT_ITEM, 0, name->str, exc);
            gdome_nl_unref(nl, &exc);
            gdome_str_unref(name);
            return mcsFAILURE;
        }
        el2=(GdomeElement *)gdome_el_firstChild(el, &exc);

        if (el2 == NULL)
        {
            errAdd (cmdERR_CDF_FORMAT_CONTENT, name->str, exc);
            gdome_el_unref(el, &exc);
            gdome_nl_unref(nl, &exc);
            gdome_str_unref(name);
            return mcsFAILURE;
        }
        
        value=gdome_el_nodeValue(el2,&exc);
        content.append(value->str);
        gdome_str_unref(value);

        gdome_el_unref(el2, &exc);
        gdome_el_unref(el, &exc);
    }
    else
    {
        mcsCOMPL_STAT status = mcsSUCCESS;
        if (isOptional == mcsFALSE)
        {
            errAdd (cmdERR_CDF_NO_ELEMENT_CONTENT, name->str);
            status = mcsFAILURE;
        }
        gdome_str_unref(name);
        gdome_nl_unref(nl, &exc);
        return status;
    }
    
    gdome_str_unref(name);
    gdome_nl_unref(nl, &exc);
    logDebug("content of '%s' element is '%s'",tagName.data(),content.data());
    return mcsSUCCESS;
}

/** 
 *  This method should be called before any real action on any parameter.
 *  It parses the parameters given to the constructor.
 *
 *  \returns an MCS completion status code (mcsSUCCESS or mcsFAILURE)
 */
mcsCOMPL_STAT cmdCOMMAND::ParseParams()
{
    logExtDbg ("cmdCOMMAND::ParseParams()");

    logDebug ( "working on params '%s'", _params.data());
   
    string::iterator i = _params.begin();
    int posStart=0;
    int posEnd=0;

    // Start walking out of a parameter value.
    mcsLOGICAL valueZone=mcsFALSE;

    while(i != _params.end())
    {
        if (*i=='-')
        {
            // If the dash is not included into a string value
            if (! valueZone)
            {
                if( ( *(i+1) >= '0' ) &&  ( *(i+1) <= '9' ))
                {
                    // Do nothing because all the tuple string must be catched
                }
                else if (posEnd>0)
                {
                    if (ParseTupleParam(_params.substr(posStart, posEnd-posStart))==mcsFAILURE)
                    {
                        return mcsFAILURE;
                    }

                    posStart=posEnd;
                }
            }
        }

        // If double quotes are encountered it opens or closes a valueZone
        if (*i=='"')
        {
            valueZone = (mcsLOGICAL)!valueZone;
        }
        
        i++;
        posEnd++;
    }

    // Parse last tuple if posEnd is not null
    if (posEnd>0)
    {
        if (ParseTupleParam(_params.substr(posStart, posEnd-posStart))==mcsFAILURE)
        {
            return mcsFAILURE;
        }
    }

    return mcsSUCCESS;
}

/** 
 *  Parse a tuple (-<paramName> <paramValue>). Tuples are extracted from the
 *  line of parameter by the parseParams method.
 *
 *  \param tuple  the name value tuple for the a parameter.
 *
 *  \returns an MCS completion status code (mcsSUCCESS or mcsFAILURE)
 */
mcsCOMPL_STAT cmdCOMMAND::ParseTupleParam(string tuple)
{
    logExtDbg ("cmdCOMMAND::ParseTupleParam()");
    
    unsigned int dashPos = tuple.find_first_of("-");
    unsigned int endPos = tuple.find_last_not_of(" ");

    // If parameter name does not start with '-'
    if ((dashPos == string::npos) || (endPos == string::npos))
    {
        // Find parameter name
        string paramName;
        unsigned int spacePos = tuple.find_first_of(" ");
        if (spacePos == string::npos)
        {
            paramName = tuple;
        }
        else
        {
            paramName = tuple.substr(0, spacePos);
        }
        // Handle error
        errAdd(cmdERR_PARAM_NAME, paramName.data());
        return mcsFAILURE;
    }
    // End if

    string str = tuple.substr(dashPos, endPos+1);

    // Find end of parameter name
    unsigned int spacePos = str.find_first_of(" ");
    if (spacePos == string::npos)
    {
        errAdd(cmdERR_PARAMS_FORMAT, tuple.data());
        return mcsFAILURE;
    }
    
    // Start from 1 to remove '-' and remove the last space*/
    // \todo enhance code to accept more than one space between name and value
    string paramName = str.substr(1, spacePos-1);
    string paramValue = str.substr(spacePos+1);
    
    logDebug("Found new tuple: [%s,%s]", paramName.data(), paramValue.data());
   
    cmdPARAM *p;
    // If parameter does'nt exist in the CDF
    STRING2PARAM::iterator iter = FindParam(paramName);
    if (iter != _paramList.end())
    {
        p = iter->second;
    }
    else
    {
        errAdd(cmdERR_PARAM_UNKNOWN, paramName.data(), _name.data());
        return mcsFAILURE;
    }

    // Assign value to the parameter
    return (p->SetUserValue(paramValue));
}


/** 
 *  Check if all mandatory parameters have a user value.
 *  The actual code exit on the first error detection.
 *
 *  \returns an MCS completion status code (mcsSUCCESS or mcsFAILURE)
 */
mcsCOMPL_STAT cmdCOMMAND::CheckParams(){
    logExtDbg("cmdCOMMAND::CheckParams()");

    STRING2PARAM::iterator i = _paramList.begin();
    while(i != _paramList.end())
    {
        cmdPARAM * child = i->second;
        if (child->IsOptional())
        {
            // No problem
        }
        else if (child->HasDefaultValue())
        {
            // No problem
        }
        else
        {
            // There should be one userValue defined
            if (child->GetUserValue().empty())
            {
                errAdd(cmdERR_MISSING_PARAM, child->GetName().data(),
                       _name.data());
                return mcsFAILURE;
            }
        }
        
        i++;
    }
    
    return mcsSUCCESS;
}

/** 
 *  Set the description of the command.
 *
 * \param desc  the description string.
 *
 *  \returns an MCS completion status code (mcsSUCCESS or mcsFAILURE)
 */
mcsCOMPL_STAT cmdCOMMAND::SetDescription(string desc)
{
    logExtDbg ("cmdCOMMAND::SetDescription()");
    _desc=desc;
    return mcsSUCCESS;
}

/** 
 * Look at the parameter in the parameter list.  
 *
 * \param name name of the searched parameter. 
 *
 * \returns iterator on the found paramater, or end of list if not found.
 */
cmdCOMMAND::STRING2PARAM::iterator cmdCOMMAND::FindParam(string name)
{
    STRING2PARAM::iterator i = _paramList.begin();
    while(i != _paramList.end())
    {
        cmdPARAM * child = i->second;
        if (child->GetName() == name)
        {
            break ;
        }
        i++;
    }
    return(i);
}

/*___oOo___*/
