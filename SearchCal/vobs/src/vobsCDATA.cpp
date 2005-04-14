/*******************************************************************************
* JMMC project
*
* "@(#) $Id: vobsCDATA.cpp,v 1.20 2005-04-14 14:39:03 scetre Exp $"
*
* History
* -------
* $Log: not supported by cvs2svn $
* Revision 1.19  2005/03/30 12:49:26  scetre
* Updated documentation
*
* Revision 1.18  2005/02/22 15:59:43  gzins
* Updated because of a miscoDYN_BUFF::GetNextLine() API change (again)
*
* Revision 1.17  2005/02/22 15:48:38  lafrasse
* Updated because of a miscoDYN_BUFF::GetNextLine() API change
*
* Revision 1.16  2005/02/17 17:57:27  gzins
* Removed trailing and leading spaces of read values when adding parameter description
*
* Revision 1.15  2005/02/16 16:18:00  scetre
* Fixed remaining bugs in Extract and Unpack methods
*
* Revision 1.14  2005/02/16 15:55:42  gzins
* Fixed bug in AppendLines; missing CR after each line insertion
*
* Revision 1.13  2005/02/16 15:22:11  gzins
* Updated call to miscoDYN_BUF::GetNextLine()
*
* Revision 1.12  2005/02/14 14:11:22  scetre
* replace \n y \0 in LoadBuffer() method
*
* Revision 1.11  2005/02/13 15:29:50  gzins
* Inherited from miscoDYN_BUF
* Added methods to load from file or buffer
* Added methods to store and extract list of stars
*
* Revision 1.10  2005/02/11 10:37:19  gzins
* Added AddParamsDesc() method
*
* Revision 1.9  2005/02/10 10:46:42  gzins
* Changed column name to parameter name
*
* Revision 1.8  2005/02/08 20:59:33  gzins
* Added name of catalog from where data is coming from
* Changed some parameter types from char* to const char*
*
* Revision 1.7  2005/02/07 09:47:08  gzins
* Renamed vobsCDATA method to be compliant with programming standards; method name starts with capital
*
* Revision 1.6  2005/01/31 13:31:38  scetre
* changed misc...Pointer in misc...
*
* Revision 1.5  2005/01/26 08:11:28  scetre
* change history
*
* scetre    15-Nov-2004  Created
*
*
*******************************************************************************/

/**
 * \file
 * vobsCDATA class definition.
 */

static char *rcsId="@(#) $Id: vobsCDATA.cpp,v 1.20 2005-04-14 14:39:03 scetre Exp $"; 
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
#include "log.h"
#include "err.h"


/*
 * Local Headers 
 */
#include "vobsSTAR.h"
#include "vobsCDATA.h"
#include "vobsErrors.h"
#include "vobsPrivate.h"

/*
 * Class constructor
 */
vobsCDATA::vobsCDATA()
{
    _nbLines = 0;
    _nbLinesToSkip = 0;
}

/*
 * Class destructor
 */
vobsCDATA::~vobsCDATA()
{
    // Free all strings containing parameter names
    std::vector<char *>::iterator paramName;
    for (paramName = _paramName.begin(); 
         paramName != _paramName.end(); paramName++)
    {
        free(*paramName);
    }

    // Free all strings containing UCD names
    std::vector<char *>::iterator ucdName;
    for (ucdName = _ucdName.begin(); ucdName != _ucdName.end(); ucdName++)
    {
        free(*ucdName);
    }

    // Free all strings containing UCD names

    _paramName.clear();
    _ucdName.clear();

    _catalogName = "";
}

/*
 * Public methods
 */

/**
 * Set the catalog name from where data is coming from 
 *
 * \return Always mcsSUCCESS.
 */
mcsCOMPL_STAT vobsCDATA::SetCatalogName(const char *name)
{
    logExtDbg("vobsCDATA::SetCatalogName()");

    _catalogName = name;

    return mcsSUCCESS;
}

/**
 * Get the catalog name from where data is coming from 
 *
 * \return catalog name.
 */
const char *vobsCDATA::GetCatalogName()
{
    logExtDbg("vobsCDATA::GetCatalogName()");

    return _catalogName.c_str();
}
 
/** 
 * Add description (name and ucd) for all parameters
 *
 * This method parses the two lines containing the description of the parameters
 * contain in the CDATA section. The first line contains the parameter names,
 * while the second contain the corresponding UCD. The number of fields
 * (separated by tab) must be the same for name and UCD.
 *
 * \param paramNameLine line containing names of parameters separated by tab
 * \param ucdNameLine line containing UCD associated to parameters separated by
 * tab
 *
 * \return
 * mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
 */
mcsCOMPL_STAT vobsCDATA::AddParamsDesc(char *paramNameLine, char *ucdNameLine)
{
    logExtDbg("vobsCDATA::AddParamsDesc()");

    const mcsUINT32 nbMaxParams=256;
    mcsUINT32 nbOfUcdName;
    mcsSTRING256 ucdNameArray[nbMaxParams];
    mcsUINT32 nbOfParamName;
    mcsSTRING256 paramNameArray[nbMaxParams];

    // Parse UCD name line and store them in an array
    if (miscSplitString(ucdNameLine, '\t', ucdNameArray, 
                        nbMaxParams, &nbOfUcdName) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    // and remove trailind and leading blanks
    for (mcsUINT32 i = 0; i < nbOfUcdName; i++)
    {
        miscTrimString(ucdNameArray[i], " ");
    }


    // Parse parameter name line and strore them in an array
    if (miscSplitString(paramNameLine, '\t', paramNameArray, 
                        nbMaxParams, &nbOfParamName) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    // and remove trailind and leading blanks
    for (mcsUINT32 i = 0; i < nbOfParamName; i++)
    {
        miscTrimString(paramNameArray[i], " ");
    }

    // Check the numbers of parameters and of UCDs are the same
    if (nbOfUcdName != nbOfParamName)
    {
        errAdd(vobsERR_INCONSISTENT_PARAMS_DESC, nbOfUcdName, nbOfParamName);
        return mcsFAILURE;
    }

    // For each UCD name stored in the array
    logDebug("\t-> Add %d parameter names and UCDs to CDATA structure ...",
             nbOfUcdName);
    for (mcsUINT32 i=0; i<nbOfUcdName; i++)
    {
        // Add parameter name and UCD to CDATA structure
        AddUcdName(ucdNameArray[i]);
        AddParamName(paramNameArray[i]);
    }

    return mcsSUCCESS;
}

/**
 * Adds a parameter description (parameter name) at the end of the list
 *
 * This method adds the description (parameter name) of a parameter in the
 * CDATA section. The description of the parameters has to be done in the same
 * order they appear in the CDATA section.
 *
 * \param paramName parameter name to be added to the list.
 *
 * \return
 * Always mcsSUCCESS.
 */
mcsCOMPL_STAT vobsCDATA::AddParamName(const char *paramName)
{
    logExtDbg("vobsCDATA::AddParamName(%s)", paramName);

    _paramName.push_back(strdup(paramName)); 

    return mcsSUCCESS;
}
/**
 * Adds a parameter description (UCD) at the end of the list
 *
 * This method adds the description (UCD) of a parameter in the
 * CDATA section. The description of the parameters has to be done in the same
 * order they appear in the CDATA section.
 *
 * \param ucdName corresponding UCD to be added to the list.
 *
 * \return
 * Always mcsSUCCESS.
 */
mcsCOMPL_STAT vobsCDATA::AddUcdName(const char *ucdName)
{
    logExtDbg("vobsCDATA::AddUcdName(%s)", ucdName);

    _ucdName.push_back(strdup(ucdName)); 

    return mcsSUCCESS;
}

/**
 * Returns the number of parameters.
 *
 * \return 
 * The the number of parameters contained in CDATA
 */
mcsUINT32 vobsCDATA::GetNbParams(void) 
{
    return _paramName.size();
}
 
/**
 * Returns the next parameter description in the CDATA section.
 *
 * This method returns the pointers to the name and teh UCD of the next
 * parameter of the CDATA. If \em init is mcsTRUE, it returns the escription of
 * the first parameter.
 * 
 * \param paramName pointer to the name of the next parameter
 * \param ucdName pointer to the corresponding UCD 
 * \param init
 * 
 * \return mcsSUCCESS or mcsFAILURE if the end of the parameter list is reached.
 */
mcsCOMPL_STAT vobsCDATA::GetNextParamDesc(char **paramName, 
                                          char **ucdName,
                                          mcsLOGICAL init) 
{
    //logExtDbg("vobsCDATA::GetNextParamDesc()");

    if (init == mcsTRUE)
    {
        // Get first element
        _paramNameIterator = _paramName.begin();
        _ucdNameIterator = _ucdName.begin();
    }
    else
    {
        // Get next element
        _paramNameIterator++;
        _ucdNameIterator++;
        if ((_paramNameIterator == _paramName.end()) ||
            (_ucdNameIterator == _ucdName.end()))
        {
            return mcsFAILURE;
        }
    }

    *paramName = *_paramNameIterator;
    *ucdName = *_ucdNameIterator;

    return mcsSUCCESS;
}

/**
 * Set the number of lines to be skipped in CDATA. 
 *
 * \param nbLines number of lines to be skipped.
 *
 * \return
 * Always mcsSUCCESS.
 */
mcsCOMPL_STAT vobsCDATA::SetNbLinesToSkip(mcsINT32 nbLines)
{
    logExtDbg("vobsCDATA::SetNbLinesToSkip()");

    _nbLinesToSkip = nbLines;

    return mcsSUCCESS;
}

/**
 * Returns the number of lines to be skipped in CDATA.
 *
 * \return 
 * The number of lines to be skipped
 */
mcsUINT32 vobsCDATA::GetNbLinesToSkip(void) 
{
    return _nbLinesToSkip;
}
 
/**
 * Append lines of the given buffer to the CDATA 
 *
 * This method appends the lines which are provided in the buffer into an
 * internal buffer. The lines of the given buffer must be seperated by '\n'.
 * The first lines, defined with SetNbLinesToSkip() method, are skipped. 
 * The GetNbLines() method gives the number of lines currently stored in
 * internal buffer. And they can be retrieved using GetNextLine().
 *
 * \param buffer pointer to the CDATA, which must be null-terminated
 * \param nbLinesToSkip number of line to skip
 *
 * \return
 * mcsSUCCESS, or mcsFAILURE if an error occurs when manipuling internal dynamic
 * buffer.
 */
mcsCOMPL_STAT vobsCDATA::AppendLines(char *buffer, mcsINT32 nbLinesToSkip)
{
    logExtDbg("vobsCDATA::AppendLines()");

    // Store buffer into a temporary buffer
    miscoDYN_BUF tmpBuffer;
    if (tmpBuffer.AppendString(buffer) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    
    logDebug("Buffer to process : %s", buffer);

    // Store usefull line into internal buffer; i.e skip header lines and
    // empty lines
    int lineNum   = 0;
    const char *from = NULL;
    mcsSTRING1024 line;
    do 
    {
        from = tmpBuffer.GetNextLine
            (from, line, sizeof(mcsSTRING1024), mcsFALSE);
        lineNum++;
        if ((from != NULL) && (lineNum > (nbLinesToSkip)) &&
            (miscIsSpaceStr(line) == mcsFALSE))
        {
            logDebug("   > Add line : %s", line);
            
            if (AppendString(line) == mcsFAILURE)
            {
                return mcsFAILURE;
            }
            if (AppendString("\n") == mcsFAILURE)
            {
                return mcsFAILURE;
            }
            _nbLines++;
        }
        else
        {
            logDebug("   > Skip line : %s", line);
        }
    }
    while (from != NULL);
 
    return mcsSUCCESS;
}

/**
 * Returns the number of lines currently stored in internal buffer.
 *
 * \return 
 * The number of lines stored in internal buffer.
 */
mcsUINT32 vobsCDATA::GetNbLines(void) 
{
    return _nbLines;
}

/**
 * Load a file into itself
 *
 * \param fileName the file to load
 * 
 * \return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is 
 * returned. 
 */
mcsCOMPL_STAT vobsCDATA::LoadFile(const char *fileName)
{
    logExtDbg("vobsCDATA::Load(file)");
    
    // Use miscoDYN_BUF method to load file into the dynBuf of the class
    if (miscoDYN_BUF::LoadFile(fileName, "#") == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    
    // Load header of the CDATA : build list of UCD and parameter names
    if (SetParamsDesc() == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    
    return mcsSUCCESS;
}

/**
 * Load a buffer into itself
 *
 * \param buffer the buffer in which the data will be loaded
 *
 * \return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is 
 * returned. 
 */
mcsCOMPL_STAT vobsCDATA::LoadBuffer(const char *buffer)
{
    logExtDbg("vobsCDATA::Load(dynBuf)");

    // Get the content of the buffer and copy it in the CDATA
    if (AppendString(buffer) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    SetCommentPattern("#");

    // Load header of the CDATA : build list of UCD and parameter names
    if (SetParamsDesc() == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    
    return mcsSUCCESS;
}

/**
 * Set the parameter descriptions.
 *
 * The two first lines of the buffer contain the parameter description. i.e. the
 * UCDs and the parameter names lists. This method retreives these two lines and
 * set the parameter description using the AddParamsDesc() method. It also sets
 * the number of line to be skipped to 2.
 *
 * \return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is 
 * returned. 
 */
mcsCOMPL_STAT vobsCDATA::SetParamsDesc(void)
{
    logExtDbg("vobsCDATA::SetParamsDesc()");

    // Get pointer to the UCDs 
    const char    *from=NULL;
    mcsSTRING1024 ucdNameLine;
    mcsUINT32     ucdNameLineMaxLength = sizeof(ucdNameLine);
    from = GetNextLine(from, ucdNameLine, ucdNameLineMaxLength);
    if (ucdNameLine == NULL)
    {
        errAdd(vobsERR_MISSING_UCDS);
        return mcsFAILURE;
    }

    // Get pointer to the parameter names 
    mcsSTRING1024 paramNameLine;
    from = GetNextLine(from, paramNameLine, ucdNameLineMaxLength);
    if (paramNameLine == NULL)
    {
        errAdd(vobsERR_MISSING_PARAM_NAMES);
        return mcsFAILURE;
    }
    
    // Update parameters description 
    if (AddParamsDesc(paramNameLine, ucdNameLine) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    
    // Set the number of lines to skip to 2; the two lines containing containing
    // parameter description.
    SetNbLinesToSkip(2);

    return mcsSUCCESS;
}

/**
 * Get the property ID corresponding to the given parameter name and UCD.
 *
 * This method returns the property ID corresponding to the parameter name and
 * UCD, or NULL if they do not correspond to an existing object property.
 *
 * \param paramName parameter name
 * \param ucdName UCD name
 *
 * \return property Id or NULL
 */
char *vobsCDATA::GetPropertyId(const char *paramName, const char *ucdName)
{

    // object identifiers 
    if (strcmp(ucdName, "ID_ALTERNATIVE") == 0)
    {
        if (strcmp(paramName, "HD") == 0)
        {
            return vobsSTAR_ID_HD;
        }
        else if (strcmp(paramName, "HIP") == 0)
        {
            return vobsSTAR_ID_HIP;
        }
        else if (strcmp(paramName, "DM") == 0)
        {
            return vobsSTAR_ID_DM;
        }
    }

    // Diameters
    if (strcmp(ucdName, "EXTENSION_DIAM") == 0)
    {
        if (strcmp(paramName, "LD") == 0)
        {
            return vobsSTAR_LD_DIAM;
        }
        else if (strcmp(paramName, "UD") == 0)
        {
            return vobsSTAR_UD_DIAM;
        }
        else if (strcmp(paramName, "UDDK") == 0)
        {
            return vobsSTAR_UDDK_DIAM;
        }
    }

    // Diameter errors
    if (strcmp(ucdName, "ERROR") == 0)
    {
        if (strcmp(paramName, "e_LD") == 0)
        {
            return vobsSTAR_LD_DIAM_ERROR;
        }
        else if (strcmp(paramName, "e_UD") == 0)
        {
            return vobsSTAR_UD_DIAM_ERROR;
        }
        else if (strcmp(paramName, "e_UDDK") == 0)
        {
            return vobsSTAR_UDDK_DIAM_ERROR;
        }
    }

    // No property corresponding to the parameter name/UCD
    return NULL;
}
/*___oOo___*/
