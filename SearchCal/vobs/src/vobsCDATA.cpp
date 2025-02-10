/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * @file
 * vobsCDATA class definition.
 */

/*
 * System Headers
 */
#include <iostream>
#include <stdlib.h>
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

/** initial capacity for vectors */
#define INITIAL_CAPACITY 20

/*
 * Class constructor.
 */
vobsCDATA::vobsCDATA()
{
    _nbLines = 0;
    _nbLinesToSkip = 0;
    _catalogId = vobsNO_CATALOG_ID;
    _catalogMeta = NULL;

    // reserve space in vectors:
    _paramName.reserve(INITIAL_CAPACITY);
    _ucdName.reserve(INITIAL_CAPACITY);
}

/*
 * Class destructor.
 */
vobsCDATA::~vobsCDATA()
{
    Reset();
}

void vobsCDATA::Reset(void)
{

    // call overriden method to clear internal buffer first:
    miscoDYN_BUF::Reset();

    _nbLines = 0;
    _nbLinesToSkip = 0;
    _catalogId = vobsNO_CATALOG_ID;
    _catalogMeta = NULL;

    // Free all strings containing parameter names
    for (vobsSTR_LIST::iterator paramName = _paramName.begin(); paramName != _paramName.end(); paramName++)
    {
        free(*paramName);
    }

    // Free all strings containing UCD names
    for (vobsSTR_LIST::iterator ucdName = _ucdName.begin(); ucdName != _ucdName.end(); ucdName++)
    {
        free(*ucdName);
    }

    // Free all strings containing UCD names
    _paramName.clear();
    _ucdName.clear();
}


/*
 * Public methods
 */

/**
 * Parse and store each parameters and UCD names.
 *
 * This method parses the two lines containing the description of the parameters
 * contained in the CDATA section. The first line contains the parameter names,
 * while the second contains the corresponding UCD. The number of fields
 * (separated by tab) must be the same for each line.
 *
 * @param paramNameLine line containing names of parameters separated by tab.
 * @param ucdNameLine line containing UCD associated to parameters separated by
 * tab.
 *
 * @return mcsSUCCESS on successful completion, mcsFAILURE otherwise.
 */
mcsCOMPL_STAT vobsCDATA::ParseParamsAndUCDsNamesLines(char *paramNameLine, char *ucdNameLine)
{
    const mcsUINT32 nbMaxParams = 512;
    mcsUINT32 nbOfUcdName;
    mcsSTRING256 ucdNameArray[nbMaxParams];
    mcsUINT32 nbOfParamName;
    mcsSTRING256 paramNameArray[nbMaxParams];

    // Split UCD name line on '\t' character, and store each token
    FAIL(miscSplitString(ucdNameLine, '\t', ucdNameArray, nbMaxParams, &nbOfUcdName));

    // Remove each token trailing and leading blanks
    for (mcsUINT32 i = 0; i < nbOfUcdName; i++)
    {
        TRIM_SPACE(ucdNameArray[i]);
    }

    // Split parameter name line on '\t' character, and store each token
    FAIL(miscSplitString(paramNameLine, '\t', paramNameArray, nbMaxParams, &nbOfParamName));

    // Remove each token trailing and leading blanks
    for (mcsUINT32 i = 0; i < nbOfParamName; i++)
    {
        TRIM_SPACE(paramNameArray[i]);
    }

    // Check that we found the same number of parameters and UCDs
    FAIL_COND_DO((nbOfUcdName != nbOfParamName), 
                 errAdd(vobsERR_INCONSISTENT_PARAMS_DESC, nbOfUcdName, nbOfParamName));

    // For each UCD name token stored in the array
    for (mcsUINT32 i = 0; i < nbOfUcdName; i++)
    {
        // If both UCD and the param names are not empty
        if ((strlen(ucdNameArray[i]) != 0) && (strlen(paramNameArray[i]) != 0))
        {
            // Add parameter name and UCD to CDATA structure
            AddUcdName(ucdNameArray[i]);
            AddParamName(paramNameArray[i]);
        }
    }
    logDebug("\t-> Added '%d' parameter names and UCDs to CDATA structure ...", nbOfUcdName);

    return mcsSUCCESS;
}

/**
 * Add a parameter name at the end of the internal parameter name list.
 *
 * This method adds the description (parameter name) of a parameter in the
 * CDATA section. The name of the parameters have to be added in the same order
 * they appear in the CDATA section.
 *
 * @param paramName parameter name to be added to the list.
 *
 * @return Always mcsSUCCESS.
 */
mcsCOMPL_STAT vobsCDATA::AddParamName(const char *paramName)
{
    _paramName.push_back(strdup(paramName));

    return mcsSUCCESS;
}

/**
 * Add a UCD name at the end of the internal UCD name list.
 *
 * This method adds the description (UCD) of a parameter in the CDATA section.
 * The name of the UCD have to be added in the same order they appear in the
 * CDATA section.
 *
 * @param ucdName corresponding UCD to be added to the list.
 *
 * @return Always mcsSUCCESS.
 */
mcsCOMPL_STAT vobsCDATA::AddUcdName(const char *ucdName)
{
    _ucdName.push_back(strdup(ucdName));

    return mcsSUCCESS;
}

/**
 * Returns the number of stored parameters.
 *
 * @return The the number of parameters contained in CDATA
 */
mcsUINT32 vobsCDATA::GetNbParams(void)
{
    return _paramName.size();
}

/**
 * Returns pointers on the next parameter and UCD names.
 *
 * This method returns the pointers to the name and the UCD of the next
 * parameter of the CDATA. If \em init is mcsTRUE, it returns the names of the
 * first parameter.
 *
 * @param paramName pointer to the name of the next parameter.
 * @param ucdName pointer to the corresponding UCD.
 * @param init returns the description of the first parameter if mcsTRUE.
 *
 * @warning This method call is NOT re-entrant.
 *
 * @return mcsSUCCESS, or mcsFAILURE if the end of the parameter list is reached.
 */
mcsCOMPL_STAT vobsCDATA::GetNextParamDesc(char **paramName,
                                          char **ucdName,
                                          mcsLOGICAL init)
{
    if (IS_TRUE(init))
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

        FAIL_COND((_paramNameIterator == _paramName.end()) || (_ucdNameIterator == _ucdName.end()));
    }

    *paramName = *_paramNameIterator;
    *ucdName = *_ucdNameIterator;

    return mcsSUCCESS;
}

/**
 * Set the number of lines to be skipped in CDATA (headers, empty lines).
 *
 * @param nbLines number of lines to be skipped.
 *
 * @return Always mcsSUCCESS.
 */
mcsCOMPL_STAT vobsCDATA::SetNbLinesToSkip(mcsINT32 nbLines)
{
    _nbLinesToSkip = nbLines;

    return mcsSUCCESS;
}

/**
 * Returns the number of lines to be skipped in CDATA (headers, empty lines).
 *
 * @return The number of lines to be skipped.
 */
mcsUINT32 vobsCDATA::GetNbLinesToSkip(void)
{
    return _nbLinesToSkip;
}

/**
 * Append lines of the given buffer to the CDATA internal buffer.
 *
 * This method appends the lines which are provided in the given buffer into an
 * internal buffer. Each line of the given buffer must be seperated by '\n'.
 * The first lines, defined with SetNbLinesToSkip() method, are skipped.
 * The GetNbLines() method gives the number of lines currently stored in
 * internal buffer. And they can be retrieved using GetNextLine().
 *
 * @param buffer pointer to the CDATA raw data, which must be null-terminated.
 * @param nbLinesToSkip number of line to skip.
 *
 * @return mcsSUCCESS on successful completion, mcsFAILURE otherwise.
 */
mcsCOMPL_STAT vobsCDATA::AppendLines(miscoDYN_BUF *buffer, mcsINT32 nbLinesToSkip)
{
    const bool isLogDebug = doLog(logDEBUG);

    // Store useful lines into internal buffer; i.e skip header lines and
    // empty lines
    mcsINT32 nbOfLine = 0;
    const char *from = NULL;
    mcsSTRING_LINE line;
    const mcsUINT32 maxLineLength = sizeof (line) - 1;
    
    do
    {
        from = buffer->GetNextLine(from, line, maxLineLength, mcsFALSE);
        nbOfLine++;

        // If a non-empty, non-header, line was found
        if (IS_NOT_NULL(from) && (nbOfLine > nbLinesToSkip) && IS_FALSE(miscIsSpaceStr(line)))
        {
            if (isLogDebug)
            {
                logDebug("\t-> Add line : %s", line);
            }

            FAIL(AppendString(line));
            FAIL(AppendString("\n"));

            _nbLines++;
        }
        else if (isLogDebug)
        {
            logDebug("\t-> Skip line : %s", line);
        }
    }
    while (IS_NOT_NULL(from));

    return mcsSUCCESS;
}

/**
 * Returns the number of lines currently stored in internal buffer.
 *
 * @return The number of lines stored in the internal buffer.
 */
mcsUINT32 vobsCDATA::GetNbLines(void)
{
    return _nbLines;
}

/**
 * Load raw CDATA from file.
 *
 * @param fileName the file to load.
 *
 * @return mcsSUCCESS on successful completion, mcsFAILURE otherwise.
 */
mcsCOMPL_STAT vobsCDATA::LoadFile(const char *fileName)
{
    // Use miscoDYN_BUF method to load file into the dynBuf of the class
    FAIL(miscoDYN_BUF::LoadFile(fileName, "#"));

    // Parse header of the CDATA and build the list of UCD and parameter names
    FAIL(LoadParamsAndUCDsNamesLines());

    return mcsSUCCESS;
}

/**
 * Load raw CDATA from a given buffer.
 *
 * @param buffer the buffer from which the raw data will be loaded
 *
 * @return mcsSUCCESS on successful completion, mcsFAILURE otherwise.
 */
mcsCOMPL_STAT vobsCDATA::LoadBuffer(const char *buffer)
{
    // Get the content of the buffer and copy it in the internal buffer
    FAIL(AppendString(buffer));

    SetCommentPattern("#");

    // Parse header of the CDATA and build the list of UCD and parameter names
    FAIL(LoadParamsAndUCDsNamesLines());

    return mcsSUCCESS;
}

/**
 * Find and parse the parameter and UCD names.
 *
 * The two first lines of the buffer contain the parameter description. i.e. the
 * UCDs and the parameter names lists. This method retreives these two lines and
 * set the parameter description using the ParseParamsAndUCDsNamesLines() method. It also sets
 * the number of line to be skipped to 2.
 *
 * @return mcsSUCCESS on successful completion, mcsFAILURE otherwise.
 */
mcsCOMPL_STAT vobsCDATA::LoadParamsAndUCDsNamesLines(void)
{
    const char* from = NULL;

    mcsSTRING_LINE ucdNameLine;
    mcsSTRING_LINE paramNameLine;
    mcsUINT32 maxLineLength = sizeof (ucdNameLine) - 1;
    
    // Get a pointer to the UCD name line
    from = GetNextLine(from, ucdNameLine, maxLineLength);
    FAIL_NULL_DO(ucdNameLine, 
                 errAdd(vobsERR_MISSING_UCDS));

    // Get a pointer to the parameter name line
    from = GetNextLine(from, paramNameLine, maxLineLength);
    FAIL_NULL_DO(paramNameLine, 
                 errAdd(vobsERR_MISSING_PARAM_NAMES));

    // Retrieve each parameter and UCD names.
    FAIL(ParseParamsAndUCDsNamesLines(paramNameLine, ucdNameLine));

    // Set the number of lines to skip to 2 : the 2 lines containing parameter description.
    SetNbLinesToSkip(2);

    return mcsSUCCESS;
}

/*___oOo___*/
