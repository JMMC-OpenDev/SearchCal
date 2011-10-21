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
    _nbLines       = 0;
    _nbLinesToSkip = 0;
    _catalogName   = "";
    
    // reserve space in vectors:
    _paramName.reserve(INITIAL_CAPACITY);
    _ucdName.reserve(INITIAL_CAPACITY);
}

/*
 * Class destructor.
 */
vobsCDATA::~vobsCDATA()
{
    // Free all strings containing parameter names
    std::vector<char *>::iterator paramName;
    for (paramName = _paramName.begin(); paramName != _paramName.end(); paramName++)
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
    logTrace("vobsCDATA::ParseParamsAndUCDsNamesLines()");

    const mcsUINT32 nbMaxParams = 512;
    mcsUINT32 nbOfUcdName;
    mcsSTRING256 ucdNameArray[nbMaxParams];
    mcsUINT32 nbOfParamName;
    mcsSTRING256 paramNameArray[nbMaxParams];

    // Split UCD name line on '\t' character, and store each token
    if (miscSplitString(ucdNameLine, '\t', ucdNameArray, 
                        nbMaxParams, &nbOfUcdName) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    // Remove each token trailing and leading blanks
    for (mcsUINT32 i = 0; i < nbOfUcdName; i++)
    {
        miscTrimString(ucdNameArray[i], " ");
    }

    // Split parameter name line on '\t' character, and store each token
    if (miscSplitString(paramNameLine, '\t', paramNameArray, 
                        nbMaxParams, &nbOfParamName) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    // Remove each token trailing and leading blanks
    for (mcsUINT32 i = 0; i < nbOfParamName; i++)
    {
        miscTrimString(paramNameArray[i], " ");
    }
    
    // Check that we found the same number of parameters and UCDs
    if (nbOfUcdName != nbOfParamName)
    {
        errAdd(vobsERR_INCONSISTENT_PARAMS_DESC, nbOfUcdName, nbOfParamName);
        return mcsFAILURE;
    }

    // For each UCD name token stored in the array
    for (mcsUINT32 i=0; i<nbOfUcdName; i++)
    {
        // If both UCD and the param names are not empty
        if ((strcmp(ucdNameArray[i], "") != 0) &&
            (strcmp(paramNameArray[i], "") != 0))
        {
            // Add parameter name and UCD to CDATA structure
            AddUcdName(ucdNameArray[i]);
            AddParamName(paramNameArray[i]);
        }
    }
    logDebug("\t-> Added '%d' parameter names and UCDs to CDATA structure ...",
             nbOfUcdName);

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
    logTrace("vobsCDATA::AddParamName(%s)", paramName);

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
    logTrace("vobsCDATA::AddUcdName(%s)", ucdName);

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
        
        if ((_paramNameIterator == _paramName.end()) || (_ucdNameIterator == _ucdName.end()))
        {
            return mcsFAILURE;
        }
    }

    *paramName = *_paramNameIterator;
    *ucdName   = *_ucdNameIterator;

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
mcsCOMPL_STAT vobsCDATA::AppendLines(char *buffer, mcsINT32 nbLinesToSkip)
{
    // Store buffer into a temporary buffer
    miscoDYN_BUF tmpBuffer;
    if (tmpBuffer.AppendString(buffer) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    /* Allocate some memory to store the CDATA stream (8K) */
    if ((_dynBuf.allocatedBytes == 0) && (Alloc(8192) == mcsFAILURE))
    {
        return mcsFAILURE;
    }
    
    // Store usefull lines into internal buffer; i.e skip header lines and
    // empty lines
    int nbOfLine = 0;
    const char *from = NULL;
    mcsSTRING1024 line;
    do 
    {
        from = tmpBuffer.GetNextLine(from, line, sizeof(mcsSTRING1024), mcsFALSE);
        nbOfLine++;

        // If a non-empty, non-header, line was found
        if ((from != NULL) && (nbOfLine > (nbLinesToSkip)) &&
            (miscIsSpaceStr(line) == mcsFALSE))
        {
            logDebug("\t-> Add line : %s", line);
            
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
            logDebug("\t-> Skip line : %s", line);
        }
    }
    while (from != NULL);
 
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
    logTrace("vobsCDATA::LoadFile()");
    
    // Use miscoDYN_BUF method to load file into the dynBuf of the class
    if (miscoDYN_BUF::LoadFile(fileName, "#") == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    
    // Parse header of the CDATA and build the list of UCD and parameter names
    if (LoadParamsAndUCDsNamesLines() == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    
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
    logTrace("vobsCDATA::LoadBuffer()");

    // Get the content of the buffer and copy it in the internal buffer
    if (AppendString(buffer) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    SetCommentPattern("#");

    // Parse header of the CDATA and build the list of UCD and parameter names
    if (LoadParamsAndUCDsNamesLines() == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    
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
    logTrace("vobsCDATA::LoadParamsAndUCDsNamesLines()");

    const char* from = NULL;

    // Get a pointer to the UCD name line
    mcsSTRING2048 ucdNameLine;
    mcsUINT32     ucdNameLineMaxLength = sizeof(ucdNameLine);
    from = GetNextLine(from, ucdNameLine, ucdNameLineMaxLength);
    if (ucdNameLine == NULL)
    {
        errAdd(vobsERR_MISSING_UCDS);
        return mcsFAILURE;
    }

    // Get a pointer to the parameter name line
    mcsSTRING2048 paramNameLine;
    from = GetNextLine(from, paramNameLine, ucdNameLineMaxLength);
    if (paramNameLine == NULL)
    {
        errAdd(vobsERR_MISSING_PARAM_NAMES);
        return mcsFAILURE;
    }
    
    // Retrieve each parameter and UCD names.
    if (ParseParamsAndUCDsNamesLines(paramNameLine, ucdNameLine) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    
    // Set the number of lines to skip to 2 : the 2 lines containing parameter description.
    SetNbLinesToSkip(2);

    return mcsSUCCESS;
}

/**
 * Get the property ID corresponding to the given parameter name and UCD.
 *
 * This method returns the property ID corresponding to the parameter name and
 * UCD, or NULL if they do not correspond to an existing object property.
 *
 * @param paramName parameter name
 * @param ucdName UCD name
 *
 * @return the found property ID, or NULL id none corresponded.
 */
const char* vobsCDATA::GetPropertyId(const char* paramName, const char* ucdName)
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
        else if (strcmp(paramName, "TYC1") == 0)
        {
            return vobsSTAR_ID_TYC1;
        }
        else if (strcmp(paramName, "TYC2") == 0)
        {
            return vobsSTAR_ID_TYC2;
        }
        else if (strcmp(paramName, "TYC3") == 0)
        {
            return vobsSTAR_ID_TYC3;
        }
        return NULL;
    }

    // Object identifier
    if (strcmp(ucdName, "ID_MAIN") == 0)
    {
        if (strcmp(paramName, "DENIS") == 0)
        {
            return vobsSTAR_ID_DENIS;
        }
        else if (strcmp(paramName, "2MASS") == 0)
        {
            return vobsSTAR_ID_2MASS;
        }
        else if (strcmp(paramName, "Seq") == 0) // SBC9 catalog ID
        {
            return vobsSTAR_ID_SB9;
        }
        else if (strcmp(paramName, "WDS") == 0) // WDS catalog ID
        {
            return vobsSTAR_ID_WDS;
        }
        else if (strcmp(paramName, "HD") == 0) // SBSC catalog ID
        {
            return vobsSTAR_ID_HD;
        }
        return NULL;
    }

    if (strcmp(ucdName, "ID_NUMBER") == 0)
    {
        if (strcmp(paramName, "objID") == 0) // AKARI catalog
        {
            return vobsSTAR_ID_AKARI;
        }
        return NULL;
    }
    
    // Flag of variability
    if (strcmp(ucdName, "CODE_VARIAB") == 0)
    {
        if (strcmp(paramName, "v1") == 0) // ASCC catalog
        {
            return vobsSTAR_CODE_VARIAB_V1;
        }
        else if (strcmp(paramName, "v2") == 0) // ASCC catalog
        {
            return vobsSTAR_CODE_VARIAB_V2;
        }
        else if (strcmp(paramName, "Var") == 0) // MIDI catalog
        {
            return vobsSTAR_CODE_BIN_FLAG;
        }
        return NULL;
    }
    
    // Code misc
    if (strcmp(ucdName, "CODE_MISC") == 0)
    {
        if (strcmp(paramName, "Iflg") == 0)
        {
            return vobsSTAR_CODE_MISC_I;
        }
        return NULL;
    }
    
    // Diameters
    if (strcmp(ucdName, "EXTENSION_DIAM") == 0)
    {
        if ((strcmp(paramName, "UDDK") == 0) ||
            (strcmp(paramName, "UDdiamKs") == 0))
        {
            return vobsSTAR_UDDK_DIAM;
        }
        return NULL;
    }

    // Diameter errors
    if (strcmp(ucdName, "ERROR") == 0)
    {
        if ((strcmp(paramName, "e_UDDK") == 0) ||
            (strcmp(paramName, "e_UDdiam") == 0))
        {
            return vobsSTAR_UDDK_DIAM_ERROR;
        }
        else if ((strcmp(paramName, "e_Plx") == 0))
        {
            return vobsSTAR_POS_PARLX_TRIG_ERROR;
        }
        else if (strcmp(paramName, "e_S09") == 0)
        {
          return vobsSTAR_PHOT_FLUX_IR_09_ERROR;
        }
        else if (strcmp(paramName, "e_S18") == 0)
        {
          return vobsSTAR_PHOT_FLUX_IR_18_ERROR;
        }
        return NULL;
    }

    // Orbit Separation
    if (strcmp(ucdName, "ORBIT_SEPARATION") == 0)
    {
        if (strcmp(paramName, "sep1") == 0)
        {
            return vobsSTAR_ORBIT_SEPARATION_SEP1;
        }
        else if (strcmp(paramName, "sep2") == 0)
        {
            return vobsSTAR_ORBIT_SEPARATION_SEP2;
        }
        return NULL;
    }
    
    if (strcmp(paramName, "S18") == 0)
    {
        // Akari Photometry at 18 mu
        // at the moment, patch an UCD error at CDS (PHOT_FLUX_IR_25)
        return vobsSTAR_PHOT_FLUX_IR_18; 
    }
    
    if (strcmp(ucdName, "PHOT_IR_N:10.4") == 0)
    {
        // Photometric catalog II/7A: Flux N
        return vobsSTAR_PHOT_JHN_N;
    }

    // No property corresponding to the parameter name/UCD
    return NULL;
}

/*___oOo___*/
