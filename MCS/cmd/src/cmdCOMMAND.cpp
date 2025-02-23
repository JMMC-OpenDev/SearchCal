/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * \file
 * cmdCOMMAND class definition.
 * \todo perform better check for argument parsing
 */

/*
 * System Headers
 */
#include <iostream>
#include <string.h>
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

/**
 * Class constructor
 *
 * \param name  the name of the command (mnemonic).
 * \param params  the arguments of the command.
 * \param cdfName the name of the command definition file.
 *
 */
cmdCOMMAND::cmdCOMMAND(string name, string params, string cdfName)
{
    // By default, the command has not been parsed
    _hasBeenYetParsed = mcsFALSE;
    // By default, the common definition file has not been parsed
    _cdfHasBeenYetParsed = mcsFALSE;
    _name = name;
    _params = params;
    _cdfName = cdfName;
}

/**
 * Class destructor
 */
cmdCOMMAND::~cmdCOMMAND()
{
    // For each parameter entry: delete each object associated to the
    // pointer object
    if (_paramList.size() > 0)
    {
        STRING2PARAM::iterator i;
        for (i = _paramList.begin(); i != _paramList.end(); i++)
        {
            delete (i->second);
        }
    }
    // Clear the parameter list
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
 * It calls  parseCdf() parseParams() and  checkParams().
 *
 * \param cdfName  the CDF file name.
 *
 * \returns an MCS completion status code (mcsSUCCESS or mcsFAILURE)
 */
mcsCOMPL_STAT cmdCOMMAND::Parse(string cdfName)
{
    // If command has been already parsed, return success
    if ( _hasBeenYetParsed == mcsTRUE)
    {
        return mcsSUCCESS;
    }

    // Use the given CDF (if any)
    if (cdfName.size() != 0)
    {
        _cdfName = cdfName;
    }

    // Parse the cdf file
    if (ParseCdf() == mcsFAILURE)
    {
        errAdd (cmdERR_PARSE_CDF, _cdfName.data(), _name.data());
        return mcsFAILURE;
    }

    // Parse parameters
    if (ParseParams() == mcsFAILURE)
    {
        errAdd (cmdERR_PARSE_PARAMETERS, _params.data(), _name.data());
        return mcsFAILURE;
    }

    // Check that parameter are correct
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
 *  \param desc the short description string
 *
 *  \returns the short description string.
 */
mcsCOMPL_STAT cmdCOMMAND::GetShortDescription(string &desc)
{
    // Clear description
    desc.clear();

    // If there is no CDF for this command, write in the description this status
    if (_cdfName.size() == 0 )
    {
        desc.append("No description available.");
    }
        // If the cdf file had information
    else
    {
        // Parse the CDF file to obtain full description
        if (ParseCdf() == mcsFAILURE)
        {
            return mcsFAILURE;
        }

        // Check if there are descriptions in the cdf file
        if (_desc.empty())
        {
            // If no description does exist, write it in the description file
            desc.append("No description found in CDF.");
        }
            // if description are present in the cdf file
        else
        {
            // Get only the first sentence of the main description.
            size_t end = _desc.find_first_of(".\n");

            // If dot is found
            if (end != string::npos)
            {
                // To include the dot
                end++;
            }
                // If dot is not found, write it in the description string
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
 *  \param desc the detailed description string
 *
 *  \returns the detailed description string.
 */
mcsCOMPL_STAT cmdCOMMAND::GetDescription(string &desc)
{
    // Clear recipient
    desc.clear();

    // Declare 3 string which are the 3 part of a detailed description
    string synopsis;
    string description;
    string options;

    // If there is no CDF for this command
    if (_cdfName.size() == 0 )
    {
        // Write in the synopsis part of the description that there is no help
        // for the wanted command
        synopsis.append("There is no help available for '");
        synopsis.append(_name);
        synopsis.append("' command.");
    }
        // if the CDF exist
    else
    {
        // Parse the CDF file to obtain full description
        if (ParseCdf() == mcsFAILURE)
        {
            return mcsFAILURE;
        }

        // Append the command name in the synopsis part of the description
        synopsis.append("NAME\n\t");
        synopsis.append(_name);

        // Append the first sentence of the command description
        string shortSentence;
        if (GetShortDescription(shortSentence) == mcsFAILURE)
        {
            return mcsFAILURE;
        }
        synopsis.append(" - ");
        synopsis.append(shortSentence);

        // Append description of command
        // if description coming from CDF is empty
        if (_desc.empty())
        {
            // Write in the description part of the detailed description that no
            // description had been found
            description.append("No description found.");
        }
        else
        {
            description.append("\n\nDESCRIPTION\n\t");
            description.append(_desc);
        }

        // add the second part of the synopsis
        synopsis.append("\n\nSYNOPSIS\n\t");
        synopsis.append(_name);

        // Append help for each parameter if any
        options.append("\n\nPARAMETERS\n");
        // if there is parameters to used
        if (_paramList.size() > 0)
        {
            STRING2PARAM::iterator i = _paramList.begin();
            // for each parameter of the parameter list
            while (i != _paramList.end())
            {
                cmdPARAM* child = i->second;

                synopsis.append(" ");
                // if it is an optional parameter, write it beetwen []
                if (child->IsOptional() == mcsTRUE)
                {
                    synopsis.append("[");
                }

                synopsis.append("-");
                // Get the parameter name
                synopsis.append(child->GetName());
                synopsis.append(" <");
                // Get the parameter type
                synopsis.append(child->GetType());
                synopsis.append(">");

                // if it is an optional parameter, write it beetwen []
                if (child->IsOptional() == mcsTRUE)
                {
                    synopsis.append("]");
                }

                // Get the help of this parameter
                string childHelp = child->GetHelp();
                if (!childHelp.empty())
                {
                    options.append(childHelp);
                    options.append("\n");
                }
                i++;
            }
        }
            // if there is no parameter
        else
        {
            options.append("\tThis command takes no parameter.\n");
        }
    }

    // Write in the detailed description the 3 parts
    desc.append(synopsis);
    desc.append(description);
    desc.append(options);

    return mcsSUCCESS;
}

/**
 * Append a VOTable serailization of the command and its parameters.
 *
 * @param voTable the string in which the PARAMs will be appent
 *
 * @returns an MCS completion status code (mcsSUCCESS or mcsFAILURE)
 */
mcsCOMPL_STAT cmdCOMMAND::AppendParamsToVOTable(string &voTable)
{
    // If there is no CDF for this command
    if (_cdfName.size() == 0 )
    {
        return mcsFAILURE;
    }
        // if the CDF exist
    else
    {
        // Parse the CDF file to obtain full description
        if (ParseCdf() == mcsFAILURE)
        {
            return mcsFAILURE;
        }

        // if there is parameters to used
        if (_paramList.size() > 0)
        {
            voTable.append("\n");
            // Append each parameter if any
            STRING2PARAM::iterator i = _paramList.begin();
            // for each parameter of the parameter list
            while (i != _paramList.end())
            {
                cmdPARAM* child = i->second;

                // Write the parameter name
                voTable.append("<PARAM name=\"");
                voTable.append(child->GetName());
                voTable.append("\"");

                // Get the param type
                voTable.append(" datatype=\"");
                string datatype = child->GetType().c_str();
                // If the datatype matches 'string'
                if (datatype == "string")
                {
                    voTable.append("char\" arraysize=\"*");
                }
                    // Else if the datatype matches 'integer'
                else if (datatype == "integer")
                {
                    voTable.append("int");
                }
                    // Else for all other datatypes
                else
                {
                    voTable.append(datatype);
                }
                voTable.append("\"");

                // Write the user value, or default value if any
                voTable.append(" value=\"");
                if (child->IsDefined() == mcsTRUE)
                {
                    // Append user value
                    voTable.append(child->GetUserValue());
                }
                else
                {
                    if (child->HasDefaultValue() == mcsTRUE)
                    {
                        // Append default value
                        voTable.append(child->GetDefaultValue());
                    }
                }
                voTable.append("\"");

                // If there is a given unit
                if (! child->GetUnit().empty())
                {
                    // Get the param unit
                    voTable.append(" unit=\"");
                    voTable.append(child->GetUnit());
                    voTable.append("\"");
                }

                // TODO : handle optionnal params
                /*
                                // if it is an optional parameter
                                voTable.append(" optionnal=\"");
                                if (child->IsOptional() == mcsTRUE)
                                {
                                    voTable.append("true");
                                }
                                else
                                {
                                    voTable.append("false");
                                }
                                voTable.append("\"");
                 */

                // Close the PARAM tag
                voTable.append("/>\n");

                // TODO : handle min and max range

                i++;
            }
        }
    }
    return mcsSUCCESS;
}

/**
 *  Add a new parameter to the command.
 *
 * \param param the parameter to add.
 *
 *  \returns an MCS completion status code (mcsSUCCESS or mcsFAILURE)
 */
mcsCOMPL_STAT cmdCOMMAND::AddParam(cmdPARAM* param)
{
    // Put in the parameters list the parameter gave as parameter of the method
    _paramList.push_back( make_pair(param->GetName(), param) );

    return mcsSUCCESS;
}

/**
 * Get the parameter associated to paramName.
 *
 * \warning This method must not be called during parsing steps because it
 * begins to check if it has been parsed.
 *
 * \param paramName  the name of the requested parameter.
 * \param param  a pointer where to store the parameter pointer
 *
 * \returns an MCS completion status code (mcsSUCCESS or mcsFAILURE)
 * param should be considered valid only on mcsSUCCESS case.
 */
mcsCOMPL_STAT cmdCOMMAND::GetParam(string paramName, cmdPARAM** param)
{
    // Parse parameter list
    if (Parse() == mcsFAILURE )
    {
        return mcsFAILURE;
    }

    // Get parameter from list
    STRING2PARAM::iterator iter = FindParam(paramName);

    // If found parameter in the list
    if (iter != _paramList.end())
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
    // create a pointer of cmdPARAM
    cmdPARAM* p;

    if (GetParam(paramName, &p) == mcsFAILURE)
    {
        logWarning("%s parameter doesn't exist", paramName.data());
        return mcsFALSE;
    }
    // Returned the logical according to the parameter
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
    // create a pointer of cmdPARAM
    cmdPARAM* p;
    if (GetParam(paramName, &p) == mcsFAILURE)
    {
        logWarning("%s parameter doesn't exist", paramName.data());
        return mcsFALSE;
    }
    // Return default value
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
    // create a pointer of cmdPARAM
    cmdPARAM* p;
    if (GetParam(paramName, &p) == mcsFAILURE)
    {
        logWarning("%s parameter doesn't exist", paramName.data());
        return mcsFALSE;
    }
    // Return logical IsOptional?
    return p->IsOptional();
}

/**
 *  Get the value of a parameter.
 *
 *  If the parameter has neither user value nor default value,
 *  then an error is returned. \n
 *  If one user value and/or default value does
 *  exist, then this function try to return the user value before returning the
 *  default one.
 *
 * \param paramName  the name of the parameter.
 * \param param  the storage data pointer.
 *
 *  \returns an MCS completion status code (mcsSUCCESS or mcsFAILURE)
 */
mcsCOMPL_STAT cmdCOMMAND::GetParamValue(string paramName, mcsINT32* param)
{
    // create a pointer of cmdPARAM
    cmdPARAM* p;
    // Check if parameter does exit
    if (GetParam(paramName, &p) == mcsFAILURE)
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
        if (p->HasDefaultValue() == mcsTRUE)
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
 *  Get the value of a parameter.
 *
 *  If the parameter has neither user value nor default value,
 *  then an error is returned. \n
 *  If one user value and/or default value does
 *  exist, then this function try to return the user value before returning the
 *  default one.
 *
 * \param paramName  the name of the parameter.
 * \param param  the storage data pointer.
 *
 *  \returns an MCS completion status code (mcsSUCCESS or mcsFAILURE)
 */
mcsCOMPL_STAT cmdCOMMAND::GetParamValue(string paramName, char** param)
{
    // create a pointer of cmdPARAM
    cmdPARAM* p;
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
 *  Get the value of a parameter.
 *
 *  If the parameter has neither user value nor default value,
 *  then an error is returned. \n
 *  If one user value and/or default value does
 *  exist, then this function try to return the user value before returning the
 *  default one.
 *
 * \param paramName  the name of the parameter.
 * \param param  the storage data pointer.
 *
 *  \returns an MCS completion status code (mcsSUCCESS or mcsFAILURE)
 */
mcsCOMPL_STAT cmdCOMMAND::GetParamValue(string paramName, mcsDOUBLE* param)
{
    // create a pointer of cmdPARAM
    cmdPARAM* p;
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
 *  Get the value of a parameter.
 *
 *  If the parameter has neither user value nor default value,
 *  then an error is returned. \n
 *  If one user value and/or default value does
 *  exist, then this function try to return the user value before returning the
 *  default one.
 *
 * \param paramName  the name of the parameter.
 * \param param  the storage data pointer.
 *
 *  \returns an MCS completion status code (mcsSUCCESS or mcsFAILURE)
 */
mcsCOMPL_STAT cmdCOMMAND::GetParamValue(string paramName, mcsLOGICAL* param)
{
    // create a pointer of cmdPARAM
    cmdPARAM* p;
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
 *  Get the default value of a parameter.
 *
 * \param paramName  the name of the parameter.
 * \param param  the storage data pointer.
 *
 *  \returns an MCS completion status code (mcsSUCCESS or mcsFAILURE)
 */
mcsCOMPL_STAT cmdCOMMAND::GetDefaultParamValue(string paramName,
                                               mcsINT32* param)
{
    // create a pointer of cmdPARAM
    cmdPARAM* p;

    // Check if parameter does exit
    if (GetParam(paramName, &p) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    // return the wanted parameter
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
mcsCOMPL_STAT cmdCOMMAND::GetDefaultParamValue(string paramName, char** param)
{
    // create a pointer of cmdPARAM
    cmdPARAM* p;

    // Check if parameter does exit
    if (GetParam(paramName, &p) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    // return the wanted parameter
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
mcsCOMPL_STAT cmdCOMMAND::GetDefaultParamValue(string paramName,
                                               mcsDOUBLE* param)
{
    // create a pointer of cmdPARAM
    cmdPARAM* p;

    // Check if parameter does exit
    if (GetParam(paramName, &p) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    // return the wanted parameter
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
mcsCOMPL_STAT cmdCOMMAND::GetDefaultParamValue(string paramName,
                                               mcsLOGICAL* param)
{
    // create a pointer of cmdPARAM
    cmdPARAM* p;

    // Check if parameter does exit
    if (GetParam(paramName, &p) == mcsFAILURE )
    {
        return mcsFAILURE;
    }
    // return the wanted parameter
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
    // Parse parameter list
    if (Parse() == mcsFAILURE )
    {
        return mcsFAILURE;
    }

    // If the parameter list is not empty
    if (_paramList.size() > 0)
    {
        STRING2PARAM::iterator i = _paramList.begin();
        // for each parameter of the list
        while (i != _paramList.end())
        {
            // Get the cmdPARAM of the element of the list
            cmdPARAM* child = i->second;

            // if it is an optional parameter
            if (child->IsOptional() == mcsTRUE)
            {
                if (child->IsDefined() == mcsTRUE)
                {
                    paramLine.append("-");
                    paramLine.append(child->GetName());
                    paramLine.append(" ");
                    paramLine.append(child->GetUserValue());
                    paramLine.append(" ");
                }
            }
                // if it is not an optional parameter,
            else
            {
                if (child->IsDefined() == mcsTRUE)
                {
                    paramLine.append("-");
                    paramLine.append(child->GetName());
                    paramLine.append(" ");
                    paramLine.append(child->GetUserValue());
                    paramLine.append(" ");
                }
                    // check if the parameter has a default value
                else if (child->HasDefaultValue() == mcsTRUE)
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
    // If the CDF has been already parsed, return
    if ( _cdfHasBeenYetParsed == mcsTRUE)
    {
        return mcsSUCCESS;
    }

    // Check that a command definition file name has been given
    if ( _cdfName.size() == 0 )
    {
        errAdd(cmdERR_NO_CDF, _name.data());
        return mcsFAILURE;
    }

    // Find the correcsponding CDF file
    char* xmlFilename = miscLocateFile(_cdfName.data());
    // Check if the CDF file has been found
    if (xmlFilename == NULL)
    {
        errAdd(cmdERR_NO_CDF, _cdfName.data());
        return mcsFAILURE;
    }
    logDebug("Using CDF file '%s'", xmlFilename);

    // Created a libgdome exception
    GdomeException exc;
    GdomeElement* root = NULL;

    mcsLockGdomeMutex();

    // Get a DOMImplementation reference
    GdomeDOMImplementation* domimpl = gdome_di_mkref();

    // Create a new Document from the CDF file
    GdomeDocument* doc = gdome_di_createDocFromURI(domimpl, xmlFilename, GDOME_LOAD_PARSING, &exc);

    // if can't create the gdome document (i.e. = NULL)
    if (doc == NULL)
    {
        errAdd (cmdERR_CDF_FORMAT, xmlFilename, exc);
        goto errCond;
    }

    // Get reference to the root element of the document
    root = gdome_doc_documentElement(doc, &exc);
    // if can't reference to the root element of the document (i.e. = NULL)
    if (root == NULL)
    {
        errAdd (cmdERR_CDF_FORMAT, xmlFilename, exc);
        goto errCond;
    }

    // Parse for Description
    if (ParseCdfForDesc(root) == mcsFAILURE)
    {
        goto errCond;
    }

    // Parse for Parameters
    if (ParseCdfForParameters(root) == mcsFAILURE)
    {
        goto errCond;
    }

    // Free the document structure and the DOMImplementation
    gdome_el_unref(root, &exc);
    gdome_doc_unref(doc, &exc);
    gdome_di_unref(domimpl, &exc);

    mcsUnlockGdomeMutex();

    free(xmlFilename);

    _cdfHasBeenYetParsed = mcsTRUE;

    return mcsSUCCESS;

errCond:
    // Free the document structure and the DOMImplementation
    gdome_el_unref(root, &exc);
    gdome_doc_unref(doc, &exc);
    gdome_di_unref(domimpl, &exc);

    mcsUnlockGdomeMutex();

    errAdd (cmdERR_PARSE_CDF, xmlFilename, _name.data());

    free(xmlFilename);

    return mcsFAILURE;
}

/**
 *  Parse the CDF document to extract description.
 *
 * \param root  the root node of the CDF document.
 *
 *  \returns an MCS completion status code (mcsSUCCESS or mcsFAILURE)
 */
mcsCOMPL_STAT cmdCOMMAND::ParseCdfForDesc(GdomeElement* root)
{
    // Create a libgdome exception
    GdomeException exc;

    GdomeDOMString* name = gdome_str_mkref("desc");

    // Get the reference to the childrens NodeList of the root element
    GdomeNodeList* nl = gdome_el_getElementsByTagName(root, name, &exc);

    // if can't reference to the node list of the root element comming from
    // theparameter method
    if (nl == NULL)
    {
        errAdd (cmdERR_CDF_FORMAT_ELEMENT, name->str, exc);
        // unref the libgdome string "name"
        gdome_str_unref(name);
        return mcsFAILURE;
    }

    // Get the number of childs of the root element (i.e. it is the length of
    // the node list)
    int nbChildren = gdome_nl_length(nl, &exc);

    // If a desc does exist get first item (xsd assumes there is only one desc)
    if (nbChildren > 0)
    {
        GdomeElement* el = (GdomeElement*) gdome_nl_item(nl, 0, &exc);
        if (el == NULL)
        {
            errAdd (cmdERR_CDF_FORMAT_ITEM, 0, name->str, exc);
            gdome_nl_unref(nl, &exc);
            gdome_str_unref(name);
            return mcsFAILURE;
        }
        // Put a libgdome pointer on the first child
        GdomeElement* el2 = (GdomeElement*) gdome_el_firstChild(el, &exc);
        if (el2 == NULL)
        {
            errAdd (cmdERR_CDF_FORMAT_CONTENT, name->str, exc);
            gdome_el_unref(el, &exc);
            gdome_nl_unref(nl, &exc);
            gdome_str_unref(name);
            return mcsFAILURE;
        }

        // Get value of the this element
        GdomeDOMString* value = gdome_el_nodeValue(el2, &exc);
        // Write this value in the description
        SetDescription(value->str);

        // unref the libgdome onject need in this scope
        gdome_str_unref(value);
        gdome_el_unref(el2, &exc);
        gdome_el_unref(el, &exc);
    }

    // unref remaining libgdome object of the method
    gdome_nl_unref(nl, &exc);
    gdome_str_unref(name);

    return mcsSUCCESS;
}

/**
 *  Parse the CDF document to extract the parameters.
 *
 * \param root  the root node of the CDF document.
 *
 *  \returns an MCS completion status code (mcsSUCCESS or mcsFAILURE)
 */
mcsCOMPL_STAT cmdCOMMAND::ParseCdfForParameters(GdomeElement* root)
{
    // Create a libgdome exception
    GdomeException exc;

    GdomeDOMString* name = gdome_str_mkref("params");

    // Get the reference to the params childrens of the root element
    GdomeNodeList* params_nl = gdome_el_getElementsByTagName(root, name, &exc);

    // if extracting node list from a root element failed (i.e. = NULL)
    if (params_nl == NULL)
    {
        errAdd (cmdERR_CDF_FORMAT_ELEMENT, name->str, exc);
        gdome_str_unref(name);
        return mcsFAILURE;
    }
    // unref unused libgdome string "name"
    gdome_str_unref(name);

    // Get the number of params elements
    int nbChildren = gdome_nl_length(params_nl, &exc);
    // If params does exist
    if (nbChildren == 1)
    {
        // Get the element of the parameter node list
        GdomeElement* params_el = (GdomeElement*) gdome_nl_item(params_nl, 0, &exc);
        // if the reference failed (i.e. = NULL)
        if (params_el == NULL)
        {
            errAdd (cmdERR_CDF_FORMAT_ITEM, 0, name->str, exc);
            // unref libgdome object of this scope
            gdome_nl_unref(params_nl, &exc);
            return mcsFAILURE;
        }

        // Get the reference to the list of param elements
        name = gdome_str_mkref("param");

        // Create a libgdome node list which will be used to get information of one parameter
        GdomeNodeList* param_nl = gdome_el_getElementsByTagName(params_el, name, &exc);

        // if the reference failed (i.e. = NULL)
        if (param_nl == NULL)
        {
            errAdd (cmdERR_CDF_FORMAT_ELEMENT, name->str, exc);
            // unref libgdome object of this scope
            gdome_str_unref(name);
            gdome_el_unref(params_el, &exc);
            gdome_nl_unref(params_nl, &exc);
            return mcsFAILURE;
        }

        // Get the number of children
        nbChildren = gdome_nl_length(param_nl, &exc);
        // If param has children
        if (nbChildren > 0)
        {
            // for each children
            for (int i = 0; i < nbChildren; i++)
            {
                // Create a libgdome element corresponding to the element i
                GdomeElement* param_el = (GdomeElement*) gdome_nl_item(param_nl, i, &exc);
                // reference failed (i.e = NULL)
                if (param_el == NULL)
                {
                    errAdd (cmdERR_CDF_FORMAT_ITEM, i, name->str, exc);
                    // unref libgdome object of this scope
                    gdome_nl_unref(param_nl, &exc);
                    gdome_str_unref(name);
                    gdome_el_unref(params_el, &exc);
                    gdome_nl_unref(params_nl, &exc);
                    return mcsFAILURE;
                }

                if (ParseCdfForParam(param_el) == mcsFAILURE)
                {
                    // unref libgdome object of this scope
                    gdome_el_unref(param_el, &exc);
                    gdome_nl_unref(param_nl, &exc);
                    gdome_str_unref(name);
                    gdome_el_unref(params_el, &exc);
                    gdome_nl_unref(params_nl, &exc);
                    return mcsFAILURE;
                }
                // unref libgdome object of this scope
                gdome_el_unref(param_el, &exc);
            }
        }
        // unref libgdome object of this scope
        gdome_nl_unref(param_nl, &exc);
        gdome_str_unref(name);
        gdome_el_unref(params_el, &exc);
    }
    // unref still referenced libgdome object of the method
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
mcsCOMPL_STAT cmdCOMMAND::ParseCdfForParam(GdomeElement* param)
{
    // create several string for the name, description, type, and if necessary
    // default value, minimum value, maximum value and unit
    string name;
    string desc;
    string type;
    string defaultValue;
    string minValue;
    string maxValue;
    string unit;
    // flag to know if the parameter is optional or not
    mcsLOGICAL optional = mcsFALSE;

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
    // Create a libgdome element
    GdomeElement* el;
    // Create a libgdome exception
    GdomeException exc;

    // Get the reference on the node element
    if (CmdGetNodeElement(param, "defaultValue", &el, mcsTRUE) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
        // if it is possible to get default value
    else
    {
        // if reference succeed, Get the type and the value of this element
        if (el != NULL)
        {
            if (CmdGetNodeContent(el, type, defaultValue) == mcsFAILURE)
            {
                // unref libgdome object
                gdome_el_unref(el, &exc);
                return mcsFAILURE;
            }
        }
    }
    // unref libgdome object
    gdome_el_unref(el, &exc);

    // Get optional minValue
    if (CmdGetNodeElement(param, "minValue", &el, mcsTRUE) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
        // if it is possible to get the element
    else
    {
        // if there is nothing in the element
        if (el != NULL)
        {
            if (CmdGetNodeContent(el, type, minValue) == mcsFAILURE)
            {
                // unref libgdome object
                gdome_el_unref(el, &exc);
                return mcsFAILURE;
            }
        }
    }
    // unref libgdome object
    gdome_el_unref(el, &exc);

    // Get optional maxValue
    if (CmdGetNodeElement(param, "maxValue", &el, mcsTRUE) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
        // if it is possible to get the element
    else
    {
        // if there is nothing in the element
        if (el != NULL)
        {
            if (CmdGetNodeContent(el, type, maxValue) == mcsFAILURE)
            {
                // unref libgdome object
                gdome_el_unref(el, &exc);
                return mcsFAILURE;
            }
        }
    }
    // unref libgdome object
    gdome_el_unref(el, &exc);

    // Check if it is an optional parameter

    // note: gdome_el_getAttribute/gdome_el_getAttributeNode methods
    // are not used because they have memory leaks.

    // Get the attributes list
    GdomeNamedNodeMap* attrList = gdome_n_attributes((GdomeNode*) param, &exc);
    if (exc == GDOME_NOEXCEPTION_ERR)
    {
        // Get the number of attributes
        unsigned int nbAttrs = gdome_nnm_length(attrList, &exc);

        // For each attribute
        for (unsigned int j = 0; j < nbAttrs; j++)
        {
            // Get the ith attribute in the node list
            GdomeNode* attr = gdome_nnm_item(attrList, j, &exc);
            if (exc == GDOME_NOEXCEPTION_ERR)
            {
                // Get the attribute name
                GdomeDOMString* attrName = gdome_n_nodeName(attr, &exc);
                if (exc == GDOME_NOEXCEPTION_ERR)
                {
                    if ((strcmp(attrName->str, "optional")  == 0))
                    {
                        // Get the attribute value
                        GdomeDOMString* attrValue = gdome_n_nodeValue(attr, &exc);
                        if (exc == GDOME_NOEXCEPTION_ERR)
                        {
                            if (strlen(attrValue->str) == 0)
                            {
                                // By default it is not optional.
                                optional = mcsFALSE;
                            }
                            else
                            {
                                if (strcmp(attrValue->str, "true")  == 0)
                                {
                                    optional = mcsTRUE;
                                }
                                else if (strcmp(attrValue->str, "1")  == 0)
                                {
                                    optional = mcsTRUE;
                                }
                                else
                                {
                                    optional = mcsFALSE;
                                }
                            }

                            // free gdome objects
                            gdome_str_unref(attrValue);
                        }
                    }

                    // free gdome objects
                    gdome_str_unref(attrName);
                }
            }
            // free gdome object
            gdome_n_unref(attr, &exc);

        } // For attr

        // free gdome object
        gdome_nnm_unref(attrList, &exc);
    }

    // Create the new Parameter and add it to the inner list of parameters
    cmdPARAM* p = new cmdPARAM(name, desc, type, unit, optional);
    // if minValue is not empty
    if (! minValue.empty())
    {
        if (p->SetMinValue(minValue) == mcsFAILURE)
        {
            return mcsFAILURE;
        }
    }
    // if maxValue is not empty
    if (! maxValue.empty())
    {
        if (p->SetMaxValue(maxValue) == mcsFAILURE)
        {
            return mcsFAILURE;
        }
    }
    // if dedfaultValue is not empty
    if (! defaultValue.empty())
    {
        if (p->SetDefaultValue(defaultValue) == mcsFAILURE)
        {
            return mcsFAILURE;
        }
    }
    // add parameter in the list
    AddParam(p);

    return mcsSUCCESS;
}

/**
 * Get the node element child of a node element.
 *
 * If the element is not affected and if it is not an optional element, return
 * an error. If it is an optional element, it can be not affected. In this case,
 * success is return
 *
 * \param parentNode the parent node.
 * \param nodeName  the name of the child.
 * \param element  the storage element.
 * \param isOptional specify whether node is an optional in the CDF or not.
 *
 * \returns an MCS completion status code (mcsSUCCESS or mcsFAILURE)
 */
mcsCOMPL_STAT cmdCOMMAND::CmdGetNodeElement(GdomeElement* parentNode,
                                            string nodeName,
                                            GdomeElement** element,
                                            mcsLOGICAL isOptional)
{
    // Create a libgdome exception
    GdomeException exc;

    // Get the reference to the node elements
    GdomeDOMString* name = gdome_str_mkref(nodeName.data());

    GdomeNodeList* nl = gdome_el_getElementsByTagName(parentNode, name, &exc);
    if (nl == NULL)
    {
        errAdd (cmdERR_CDF_FORMAT_ELEMENT, name->str, exc);
        // unref libgdome object
        gdome_str_unref(name);
        return mcsFAILURE;
    }
    // number of the children
    int nbChildren = gdome_nl_length(nl, &exc);

    // If there is element in list
    if (nbChildren > 0)
    {
        // Get first element
        *element = (GdomeElement*) gdome_nl_item(nl, 0, &exc);
        if (*element == NULL)
        {
            errAdd (cmdERR_CDF_FORMAT_ITEM, 0, name->str, exc);
            // unref libgdome object
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
        // unref libgdome object
        gdome_str_unref(name);
        gdome_nl_unref(nl, &exc);
        return mcsFAILURE;
    }
    else
    {
        // Else return NULL element
        *element = NULL;
    }

    // unref libgdome object
    gdome_str_unref(name);
    gdome_nl_unref(nl, &exc);

    return mcsSUCCESS;
}

/**
 * Get the content of the first child node using tagName.
 *
 * If tagName is empty the first child is used without any care of the child's
 * tag.
 *
 * \param parentNode the parent node.
 * \param tagName  the tag of the child or empty.
 * \param content  the storage string.
 * \param isOptional specify whether node is an optional in the CDF or not.
 *
 *  \returns an MCS completion status code (mcsSUCCESS or mcsFAILURE)
 */
mcsCOMPL_STAT cmdCOMMAND::CmdGetNodeContent(GdomeElement* parentNode,
                                            string tagName,
                                            string &content,
                                            mcsLOGICAL isOptional)
{
    // Create a libgdome exception
    GdomeException exc;

    GdomeDOMString* name = gdome_str_mkref(tagName.data());

    // Get the reference to the named childrens of the parentNode element
    GdomeNodeList* nl = gdome_el_getElementsByTagName(parentNode, name, &exc);
    // if reference is on an NULL object
    if (nl == NULL)
    {
        errAdd (cmdERR_CDF_FORMAT_ELEMENT, name->str, exc);
        // unref libgdome object of the scope
        gdome_str_unref(name);
        return mcsFAILURE;
    }
    // Get the number of children corresponding to the size of the node list
    int nbChildren = gdome_nl_length(nl, &exc);

    // Inform that we are maybe missing some data
    if (nbChildren > 1)
    {
        logWarning("Only use the first children but %d are present", nbChildren);
    }
    // If one or more children do exist, work:
    if (nbChildren > 0)
    {
        GdomeElement* el = (GdomeElement*) gdome_nl_item(nl, 0, &exc);
        if (el == NULL)
        {
            errAdd (cmdERR_CDF_FORMAT_ITEM, 0, name->str, exc);
            // unref libgdome object
            gdome_str_unref(name);
            gdome_nl_unref(nl, &exc);
            return mcsFAILURE;
        }
        GdomeElement* el2 = (GdomeElement*) gdome_el_firstChild(el, &exc);
        if (el2 == NULL)
        {
            errAdd (cmdERR_CDF_FORMAT_CONTENT, name->str, exc);
            // unref libgdome object
            gdome_el_unref(el, &exc);
            gdome_str_unref(name);
            gdome_nl_unref(nl, &exc);
            return mcsFAILURE;
        }

        GdomeDOMString* value = gdome_el_nodeValue(el2, &exc);
        content.append(value->str);

        // unref libgdome object
        gdome_str_unref(value);
        gdome_el_unref(el2, &exc);
        gdome_el_unref(el, &exc);
    }
    else
    {
        // if it is not optional, return failure
        if (isOptional == mcsFALSE)
        {
            errAdd (cmdERR_CDF_NO_ELEMENT_CONTENT, name->str);
            // unref libgdome object
            gdome_str_unref(name);
            gdome_nl_unref(nl, &exc);
            return mcsFAILURE;
        }
    }

    // unref libgdome object
    gdome_str_unref(name);
    gdome_nl_unref(nl, &exc);

    return mcsSUCCESS;
}

/**
 *  Parse parameter of the command.
 *
 *  This method should be called before any real action on any parameter.
 *  It parses the parameters given to the constructor.
 *
 *  \returns an MCS completion status code (mcsSUCCESS or mcsFAILURE)
 */
mcsCOMPL_STAT cmdCOMMAND::ParseParams()
{
    int posStart = 0;
    int posEnd   = 0;

    // Start walking out of a parameter value.
    mcsLOGICAL valueZone = mcsFALSE;

    string::iterator i = _params.begin();

    while (i != _params.end())
    {
        if (*i == '-')
        {
            // If the dash is not included into a string value
            if (! valueZone)
            {
                if ( ( *(i + 1) >= '0' ) &&  ( *(i + 1) <= '9' ))
                {
                    // Do nothing because all the tuple string must be catched
                }
                else if ( (i != _params.begin()) && (*(i - 1) != ' ') )
                {
                    // Do nothing because all the tuple string must be catched
                    // the paramvalue should be xxx-xxx
                }
                else if ( posEnd > 0 )
                {
                    if (ParseTupleParam(_params.substr(posStart,
                                                       posEnd - posStart)) ==
                            mcsFAILURE)
                    {
                        return mcsFAILURE;
                    }
                    posStart = posEnd;
                }
            }
        }

        // If double quotes are encountered it opens or closes a valueZone
        if (*i == '"')
        {
            valueZone = (mcsLOGICAL)!valueZone;
        }
        i++;
        posEnd++;
    }

    // Parse last tuple if posEnd is not null
    if (posEnd > 0)
    {
        if (ParseTupleParam(_params.substr(posStart,
                                           posEnd - posStart)) == mcsFAILURE)
        {
            return mcsFAILURE;
        }
    }
    return mcsSUCCESS;
}

/**
 *  Parse a tuple (-<paramName> <paramValue>).
 *
 *  Tuples are extracted from the line of parameter by the parseParams method.
 *
 *  \param tuple  the name value tuple for the a parameter.
 *
 *  \returns an MCS completion status code (mcsSUCCESS or mcsFAILURE)
 */
mcsCOMPL_STAT cmdCOMMAND::ParseTupleParam(string tuple)
{
    size_t dashPos = tuple.find_first_of("-");
    size_t endPos = tuple.find_last_not_of(" ");

    // If parameter name does not start with '-'
    if ((dashPos == string::npos) || (endPos == string::npos))
    {
        // Find parameter name
        string paramName;
        size_t spacePos = tuple.find_first_of(" ");
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
    string str = tuple.substr(dashPos, endPos + 1);

    // Find end of parameter name
    size_t spacePos = str.find_first_of(" ");
    if (spacePos == string::npos)
    {
        errAdd(cmdERR_PARAMS_FORMAT, tuple.data());
        return mcsFAILURE;
    }

    // Start from 1 to remove '-' and remove the last space
    // \todo enhance code to accept more than one space between name and value
    string paramName = str.substr(1, spacePos - 1);
    string paramValue = str.substr(spacePos + 1);

    cmdPARAM* p;
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
 *
 *  The actual code exit on the first error detection.
 *
 *  \returns an MCS completion status code (mcsSUCCESS or mcsFAILURE)
 */
mcsCOMPL_STAT cmdCOMMAND::CheckParams()
{
    STRING2PARAM::iterator i = _paramList.begin();
    // for each parameter of the list
    while (i != _paramList.end())
    {
        cmdPARAM* child = i->second;
        // if the parameter is optional
        if (child->IsOptional() == mcsTRUE)
        {
            // No problem
        }
            // or if the parameter has a default value
        else if (child->HasDefaultValue() == mcsTRUE)
        {
            // No problem
        }
        else
        {
            // There should be one userValue defined
            if (child->GetUserValue().empty())
            {
                errAdd(cmdERR_MISSING_PARAM, child->GetName().data(), _name.data());
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
 *  \returns always mcsSUCCESS
 */
mcsCOMPL_STAT cmdCOMMAND::SetDescription(string desc)
{
    _desc = desc;
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
    while (i != _paramList.end())
    {
        cmdPARAM* child = i->second;
        if (child->GetName() == name)
        {
            break ;
        }
        i++;
    }
    return i;
}

/*___oOo___*/
