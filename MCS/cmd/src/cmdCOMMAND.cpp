/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: cmdCOMMAND.cpp,v 1.22 2005-02-17 09:03:01 gzins Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
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
 * \todo get Default value from cdf
 * \todo perform better check for argument parsing
 */

static char *rcsId="@(#) $Id: cmdCOMMAND.cpp,v 1.22 2005-02-17 09:03:01 gzins Exp $"; 
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
    
    // for each parameter entry: delete each object associated to the pointer object
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
 * \param cdfName  the cdf file name.
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
        
    // and flag a right performed parsing only after this point
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
        // Parse The cdf file to obtain full description
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

    // clear recipient
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
        // Parse The cdf file to obtain full description
        if (ParseCdf()==mcsFAILURE)
        {
            return mcsFAILURE;
        }

        // append the command name
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

        // append description of command
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

        // append help for each parameter if any
        if (_paramList.size() > 0)
        {
            options.append("\n\nPARAMETERS\n");
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
            options.append("\t\tThis command takes no parameter\n");
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
        errAdd(cmdERR_PARAM_UNKNOWN, paramName.data(), _name.data()); 
        return mcsFAILURE;
    }

    // Try to return the user value
    if( p->GetUserValue(param) == mcsSUCCESS ){
        return mcsSUCCESS;
    }

    // Else try to return the default value
    if( p->HasDefaultValue() == mcsTRUE ){
        return p->GetDefaultValue(param);    
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
        errAdd(cmdERR_PARAM_UNKNOWN, paramName.data(), _name.data()); 
        return mcsFAILURE;
    }

    // Try to return the user value
    if( p->GetUserValue(param) == mcsSUCCESS ){
        return mcsSUCCESS;
    }

    // Else try to return the default value
    if( p->HasDefaultValue() == mcsTRUE ){
        return p->GetDefaultValue(param);    
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
        errAdd(cmdERR_PARAM_UNKNOWN, paramName.data(), _name.data()); 
        return mcsFAILURE;
    }

    // Try to return the user value
    if( p->GetUserValue(param) == mcsSUCCESS ){
        return mcsSUCCESS;
    }

    // Else try to return the default value
    if( p->HasDefaultValue() == mcsTRUE ){
        return p->GetDefaultValue(param);    
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
        errAdd(cmdERR_PARAM_UNKNOWN, paramName.data(), _name.data()); 
        return mcsFAILURE;
    }

    // Try to return the user value
    if( p->GetUserValue(param) == mcsSUCCESS ){
        return mcsSUCCESS;
    }

    // Else try to return the default value
    if( p->HasDefaultValue() == mcsTRUE ){
        return p->GetDefaultValue(param);    
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
 * Protected methods
 */



/*
 * Private methods
 */

/** 
 *  Parse a cdf file and build new parameters for the command.
 *  After this step the parameters can be parsed.
 *
 *  \returns an MCS completion status code (mcsSUCCESS or mcsFAILURE)
 */
mcsCOMPL_STAT cmdCOMMAND::ParseCdf()
{
    logExtDbg ("cmdCOMMAND::ParseCdf()");

    // If the cdf has been already parsed, return
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
 
    // find the correcsponding cdf file
    char * fullCdfFilename = miscLocateFile(_cdfName.data());
    // check if the cdf file has been found   
    if (fullCdfFilename == NULL)
    {
        return mcsFAILURE;
    }

    /* Get a DOMImplementation reference */
    domimpl = gdome_di_mkref ();

    /* create a new Document from the cdf file */
    const char *xmlFilename = miscResolvePath(fullCdfFilename);
    logDebug("Using cdf file %s",xmlFilename);
    doc = gdome_di_createDocFromURI(domimpl, xmlFilename, GDOME_LOAD_PARSING,
                                    &exc);
    if (doc == NULL)
    {
        logWarning ("Illegal format encountered for cdf file "
                    "'%.100s'. DOMImplementation.createDocFromURI() failed "
                    "with exception #%d", xmlFilename, exc);
        goto errCond;
    }

    /* Get reference to the root element of the document */
    root = gdome_doc_documentElement (doc, &exc);
    if (root == NULL) 
    {
        logWarning ("Illegal format encountered for cdf file "
                    "'%.100s'. Document.documentElement() failed "
                    "with exception #%d", xmlFilename, exc);
        goto errCond;
    }

    /* Parse for Description */
    if (ParseCdfForDesc(root)==mcsFAILURE)
    {
        goto errCond;
    }
    
    /* Parse for Parameters */
    if (ParseCdfForParameters(root)==mcsFAILURE)
    {
        goto errCond;
    }

//endCond:
    /* Free the document structure and the DOMImplementation */
    gdome_el_unref(root, &exc);
    gdome_doc_unref (doc, &exc);
    gdome_di_unref (domimpl, &exc);
    xmlCleanupParser();
    _cdfHasBeenYetParsed = mcsTRUE;
    return mcsSUCCESS;

errCond:
    /* Free the document structure and the DOMImplementation */
    gdome_el_unref(root, &exc);
    gdome_doc_unref (doc, &exc);
    gdome_di_unref (domimpl, &exc);
    xmlCleanupParser();
    errAdd (cmdERR_PARSE_CDF, fullCdfFilename, _name.data());
    return mcsFAILURE;
}

/** 
 *  Parse the cdf document to extract description.
 *
 * \param root  the root node of the cdf document.
 *
 *  \returns an MCS completion status code (mcsSUCCESS or mcsFAILURE)
 */
mcsCOMPL_STAT cmdCOMMAND::ParseCdfForDesc(GdomeElement *root)
{
    logExtDbg ("cmdCOMMAND::ParseCdfForDesc()");

    GdomeElement *el,*el2;
    GdomeNodeList *nl;
    GdomeException exc;
    GdomeDOMString *name, *value;
    int nbChildren;
    
    name = gdome_str_mkref ("desc");
    /* Get the reference to the childrens NodeList of the root element */
    nl = gdome_el_getElementsByTagName (root, name, &exc);
    gdome_str_unref(name);
    if (nl == NULL)
    {
        logWarning ("Illegal format encountered for cdf file "
                    ". Element.childNodes() failed "
                    "with exception #%d", exc);
        return mcsFAILURE;
    }
 
    nbChildren = gdome_nl_length (nl, &exc);

    /* if a desc does exist get first item (xsd assumes there is only one
     * desc) */
    if (nbChildren > 0)
    {
        el = (GdomeElement *)gdome_nl_item (nl, 0, &exc);
        if (el == NULL)
        {
            logWarning ("Illegal format encountered for cdf file "
                        ". NodeList.item(%d) failed "
                        "with exception #%d", 0, exc);
            gdome_nl_unref(nl, &exc);
            return mcsFAILURE;
        }
        el2=(GdomeElement *)gdome_el_firstChild(el, &exc);
        value=gdome_el_nodeValue(el2,&exc);
        SetDescription(value->str);
        gdome_str_unref(value);
        gdome_el_unref(el2, &exc);
        gdome_el_unref(el, &exc);
    }
    
    gdome_nl_unref(nl, &exc);
    
    return mcsSUCCESS;
}

/** 
 *  Parse the cdf document to extract the parameters.
 *
 * \param root  the root node of the cdf document.
 *
 *  \returns an MCS completion status code (mcsSUCCESS or mcsFAILURE)
 */
mcsCOMPL_STAT cmdCOMMAND::ParseCdfForParameters(GdomeElement *root)
{
    logExtDbg ("cmdCOMMAND::ParseCdfForParameters()");
    
    GdomeElement *el;
    GdomeNodeList *nl;
    GdomeException exc;
    GdomeDOMString *name;
    int nbChildren, i;
    
    name = gdome_str_mkref ("params");
    /* Get the reference to the childrens NodeList of the root element */
    nl = gdome_el_getElementsByTagName (root, name, &exc);
    gdome_str_unref(name);
    if (nl == NULL)
    {
        logWarning ("Illegal format encountered for cdf file "
                    ". Element.childNodes() failed "
                    "with exception #%d", exc);
        return mcsFAILURE;
    }
 
    nbChildren = gdome_nl_length (nl, &exc);
    /* if params does exist */
    if (nbChildren > 0)
    {
        el = (GdomeElement *)gdome_nl_item (nl, 0, &exc);
        if (el == NULL)
        {
            logWarning ("Illegal format encountered for cdf file "
                        ". NodeList.item(%d) failed "
                        "with exception #%d", 0, exc);
            return mcsFAILURE;
        }
        gdome_nl_unref(nl, &exc);

        /* Get the reference to the list of param elements */
        name = gdome_str_mkref ("param");
        nl = gdome_el_getElementsByTagName (el, name, &exc);
        gdome_el_unref(el, &exc);
        gdome_str_unref(name);
        if (nl == NULL)
        {
            logWarning ("Illegal format encountered for cdf file "
                        ". Element.childNodes() failed "
                        "with exception #%d", exc);
            return mcsFAILURE;
        }

        nbChildren = gdome_nl_length (nl, &exc);
        /* if param does exist */
        if (nbChildren > 0)
        {
            for (i=0;i<nbChildren;i++){
                el = (GdomeElement *)gdome_nl_item (nl, i, &exc);
                if (el == NULL)
                {
                    logWarning ("Illegal format encountered for cdf file "
                                ". NodeList.item(%d) failed "
                                "with exception #%d", i, exc);
                    gdome_nl_unref(nl, &exc);
                    return mcsFAILURE;
                }

                if (ParseCdfForParam(el)==mcsFAILURE)
                {
                    gdome_el_unref(el, &exc);
                    gdome_nl_unref(nl, &exc);
                    return mcsFAILURE;
                }
                gdome_el_unref(el, &exc);
            }
        }
    }
    gdome_nl_unref(nl, &exc);
    return mcsSUCCESS;
}

/** 
 *  Parse the cdf document to extract the parameters.
 *
 *  \param param  one param node of the cdf document.
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
    string unit;
    mcsLOGICAL optional;
    
    /* get mandatory name */
    if (CmdGetNodeContent(param, "name", name) == mcsFAILURE)
    {
        // \todo add error
        return mcsFAILURE;   
    }
    /* get mandatory type */
    if (CmdGetNodeContent(param, "type", type) == mcsFAILURE)
    {
        // \todo add error
        return mcsFAILURE;   
    }
    
    /* get optional description */
    CmdGetNodeContent(param, "desc", desc);
    /* get optional unit */
    CmdGetNodeContent(param, "unit", unit);
   
    /* get optional defaultValue */
    { 
        GdomeNodeList *nl;
        GdomeElement *el;
        GdomeException exc;
        GdomeDOMString *name;
        int nbChildren;

        name = gdome_str_mkref ("defaultValue");
        /* Get the reference to the defaultValue elements  */
        nl = gdome_el_getElementsByTagName (param, name, &exc);
        gdome_str_unref(name);
        if (nl == NULL)
        {
            logWarning ("Illegal format encountered for cdf file "
                        ". Element.childNodes() failed "
                        "with exception #%d", exc);
            return mcsFAILURE;
        }
        nbChildren = gdome_nl_length (nl, &exc);
        /* if a defaultValue does exist */
        if (nbChildren > 0)
        {
            /* get defaultValue element */
            el = (GdomeElement *)gdome_nl_item (nl, 0, &exc);
            gdome_nl_unref(nl, &exc);
            if (el == NULL)
            {
                logWarning ("Illegal format encountered for cdf file "
                            ". NodeList.item(%d) failed "
                            "with exception #%d", 0, exc);
                gdome_el_unref(el, &exc);
                return mcsFAILURE;
            }
            if (CmdGetNodeContent(el, type, defaultValue) == mcsFAILURE )
            {
                // \todo add error
                gdome_el_unref(el, &exc);
                return mcsFAILURE;   
            }
            gdome_el_unref(el, &exc);
        }
        else
        {
            // there should not be any defaultValue
            logDebug("no defaultValue found ");
            gdome_nl_unref(nl, &exc);
        }
    }
    /* check if it is an optional parameter */
    { 
        GdomeAttr *attribute;
        GdomeException exc;
        GdomeDOMString *attrName,*attrValue, *str, *str2;

        attrName = gdome_str_mkref ("optional");
        /* Get the reference to the optional element  */
        attribute = gdome_el_getAttributeNode (param, attrName, &exc);
        gdome_str_unref(attrName);
        if (attribute == NULL)
        {
            // by default it is not optional.
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
            gdome_a_unref(attribute,&exc);
        }
    }
    
    // Create the new Parameter and add it to the inner list of parameters
    cmdPARAM *p = new cmdPARAM(name, desc, type, unit, optional);
    if (! defaultValue.empty())
    {
        p->SetDefaultValue(defaultValue);
    }
    AddParam(p);
    
    return mcsSUCCESS;
}

/** 
 * Get the content of the first child node using tagName. If tagName is empty
 * the first child is used without any care of the child's tag.
 *
 * \param parentNode the parent node. 
 * \param tagName  the tag of the child or empty.
 * \param content  the storage string.
 *
 *  \returns an MCS completion status code (mcsSUCCESS or mcsFAILURE)
 */
mcsCOMPL_STAT cmdCOMMAND::CmdGetNodeContent(GdomeElement *parentNode, string tagName, string &content)
{
    logExtDbg("cmdCOMMAND::CmdGetNodeContent()");
    
    GdomeElement *el, *el2;
    GdomeNodeList *nl;
    GdomeException exc;
    GdomeDOMString *name, *value;
    int nbChildren;
    
    if (tagName.empty())
    {
        nl = gdome_el_childNodes(parentNode, &exc);
    }
    else
    {
        name = gdome_str_mkref (tagName.data());
        /* Get the reference to the childrens NodeList of the root element */
        nl = gdome_el_getElementsByTagName (parentNode, name, &exc);
        gdome_str_unref(name);
    }
    
    if (nl == NULL)
    {
        logDebug("searching content for '%s' element failed ",tagName.data());
        logWarning ("Illegal format encountered for cdf file "
                    ". Element.childNodes() failed "
                    "with exception #%d", exc);
        return mcsFAILURE;
    }

    nbChildren = gdome_nl_length (nl, &exc);
    /* if a desc does exist */
    if (nbChildren > 0)
    {
        el = (GdomeElement *)gdome_nl_item (nl, 0, &exc);
        gdome_nl_unref(nl, &exc);
        if (el == NULL)
        {
            logDebug("searching content for '%s' element failed ",tagName.data());
            logWarning ("Illegal format encountered for cdf file "
                        ". NodeList.item(%d) failed "
                        "with exception #%d", 0, exc);
            return mcsFAILURE;
        }
        el2=(GdomeElement *)gdome_el_firstChild(el, &exc);

        if (el2 == NULL)
        {
            logDebug("searching content for '%s' element failed ",tagName.data());
            // \todo errAdd
            gdome_el_unref(el, &exc);
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
        logDebug("searching content for '%s' element failed, no element found ",tagName.data());
        // no child found
        return mcsFAILURE;
    }
    
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

    // we start walking out of a parameter value.
    mcsLOGICAL valueZone=mcsFALSE;

    while(i != _params.end())
    {
        if (*i=='-')
        {
            /* If the dash is not included into a string value */
            if (! valueZone)
            {
                if( ( *(i+1) >= '0' ) &&  ( *(i+1) <= '9' ))
                {
                    // do nothing because all the tuple string must be catched
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

        // if double quotes are encountered it opens or closes a valueZone
        if (*i=='"')
        {
            valueZone = (mcsLOGICAL)!valueZone;
        }
        
        i++;
        posEnd++;
    }

    // parse last tuple if posEnd is not null
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
    
    /* start from 1 to remove '-' and remove the last space*/
    /* \todo enhance code to accept more than one space between name and value
     * */
    string paramName = str.substr(1, spacePos-1);
    string paramValue = str.substr(spacePos+1);
    
    logDebug("found new tuple: [%s,%s]", paramName.data(), paramValue.data());
   
    cmdPARAM *p;
    /* If parameter does'nt exist in the cdf */
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

    /* assign value to the parameter */
    p->SetUserValue(paramValue);
    
    return mcsSUCCESS;
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
            // no problem
        }
        else if (child->HasDefaultValue())
        {
            // no problem
        }
        else
        {
            // there should be one userValue defined
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
