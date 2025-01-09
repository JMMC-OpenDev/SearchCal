#ifndef vobsCDATA_H
#define vobsCDATA_H
/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * \file
 * vobsCDATA class declaration.
 */

#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif

/*
 * system header files
 */
#include <vector>
#include <string.h>
#include <stdio.h>
/*
 * MCS header files
 */
#include "err.h"
#include "log.h"
#include "mcs.h"
#include "misc.h"
#include "misco.h"

/*
 * Local header files
 */
#include "vobsCATALOG.h"
#include "vobsSTAR_LIST.h"

/* Define module id for logs */
#ifndef MODULE_ID
#define MODULE_ID_HACK
#define MODULE_ID "vobs"
#endif

/* line type to set capacity to 64Kb */
#define mcsSTRING_LINE  mcsSTRING65536

/*
 * Type declaration
 */

/** String vector as char* */
typedef std::vector<char*> vobsSTR_LIST;


/*
 * Class declaration
 */

/**
 * vobsCDATA represent the CDATA part of the resulting xml files coming from
 * the CDS.
 *
 * It is build with several part :
 * \li a list of UCD name.
 * \li a list of parameter name.
 * \li a number of line to skip in order to get the information data in the
 * CDATA block
 * \li the body of the CDATA
 * \li the number of line stored in the CDATA
 **/
class vobsCDATA : public miscoDYN_BUF
{
public:
    // Constructor
    vobsCDATA();

    // Destructor
    virtual ~vobsCDATA();

    void Reset(void);

    virtual mcsCOMPL_STAT ParseParamsAndUCDsNamesLines(char *paramNameLine, char *ucdNameLine);
    virtual mcsCOMPL_STAT AddParamName(const char *paramName);
    virtual mcsCOMPL_STAT AddUcdName(const char *ucdName);

    virtual mcsUINT32 GetNbParams(void);
    virtual mcsCOMPL_STAT GetNextParamDesc(char **paramName,
                                           char **ucdName,
                                           mcsLOGICAL init);
    virtual mcsCOMPL_STAT SetNbLinesToSkip(mcsINT32 nbLines);
    virtual mcsUINT32 GetNbLinesToSkip(void);
    virtual mcsCOMPL_STAT AppendLines(miscoDYN_BUF *buffer, mcsINT32 nbLinesToSkip);
    virtual mcsUINT32 GetNbLines(void);

    virtual mcsCOMPL_STAT LoadFile(const char *fileName);
    virtual mcsCOMPL_STAT LoadBuffer(const char *buffer);

    /**
     * Set the catalog identifier from where data is coming.
     *
     * @return Always mcsSUCCESS.
     */
    inline void SetCatalogId(vobsORIGIN_INDEX originIndex) __attribute__ ((always_inline))
    {
        _catalogId = originIndex;
    }

    /**
     * Set the catalog name from where data is coming.
     *
     * @return Always mcsSUCCESS.
     */
    inline void SetCatalogMeta(const vobsCATALOG_META* catalogMeta) __attribute__ ((always_inline))
    {
        _catalogMeta = catalogMeta;
        _catalogId = catalogMeta->GetCatalogId();
    }

    /**
     * Store a star list into the CDATA object
     *
     * It stores all the star properties into the internal buffer. The star
     * properties are separated by tabs with one line per star.
     *
     * @param object object contained in the list
     * @param objectList list of objects
     * @param ucdList list of UCD
     * @param extendedFormat if true, each property is stored with its
     * attributes (origin and confidence index), otherwise only only property is
     * stored.
     * @param simple if true, property Ids are not written and numeric values are formatted (limited precision)
     *
     * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
     */
    template <class Star, class list>
    mcsCOMPL_STAT Store(Star &object,
                        list &objectList,
                        mcsLOGICAL extendedFormat = mcsFALSE,
                        mcsLOGICAL simple = mcsFALSE)
    {
        /*
         * Note: error are written in form:
         * 'error \t originIndex \t confidenceIndex (\t error)'
         */
        const mcsUINT32 nbStars = objectList.Size();
        const mcsINT32  nbProps = object.NbProperties();

        mcsINT32 propertyIndex;
        vobsSTAR_PROPERTY* property;
        const vobsSTAR_PROPERTY_META* propMeta;

        // Write all property Ids into the buffer
        if (IS_FALSE(simple))
        {
            for (propertyIndex = 0; propertyIndex < nbProps; propertyIndex++)
            {
                property = object.GetProperty(propertyIndex);
                propMeta = property->GetMeta();

                if (IS_NOT_NULL(propMeta))
                {
                    // UCD of the property value
                    AppendString(propMeta->GetId());
                    AppendString("\t");

                    if (IS_TRUE(extendedFormat))
                    {
                        AppendString("\t\t");
                    }

                    propMeta = property->GetErrorMeta();
                    if (IS_NOT_NULL(propMeta))
                    {
                        // UCD of the property error
                        AppendString(propMeta->GetId());
                        AppendString("\t");

                        if (IS_TRUE(extendedFormat))
                        {
                            AppendString("\t\t");
                        }
                    }
                }
            }
            AppendString("\n");
        }

        FAIL(SaveBufferIfNeeded());

        // Write all property Names into the buffer
        for (propertyIndex = 0; propertyIndex < nbProps; propertyIndex++)
        {
            property = object.GetProperty(propertyIndex);
            propMeta = property->GetMeta();

            if (IS_NOT_NULL(propMeta)) {
                // Name of the property value
                AppendString(propMeta->GetName());
                AppendString("\t");

                if (IS_TRUE(extendedFormat))
                {
                    AppendString("\t\t");
                }

                propMeta = property->GetErrorMeta();
                if (IS_NOT_NULL(propMeta))
                {
                    // Name of the property error
                    AppendString(propMeta->GetName());
                    AppendString("\t");

                    if (IS_TRUE(extendedFormat))
                    {
                        AppendString("\t\t");
                    }
                }
            }
        }
        AppendString("\n");

        FAIL(SaveBufferIfNeeded());

        Star *starPtr;
        mcsDOUBLE numerical;
        mcsSTRING32 converted;

        // For each object of the list
        for (mcsUINT32 starIdx = 0; starIdx < nbStars; starIdx++)
        {
            // Get each object of the list
            starPtr = (Star*) objectList.GetNextStar((mcsLOGICAL) (starIdx == 0));

            // For each property of the object
            for (propertyIndex = 0; propertyIndex < nbProps; propertyIndex++)
            {
                // Get each property
                property = starPtr->GetProperty(propertyIndex);
                
                if (IS_NOT_NULL(property)) {
                    // Each star property is placed in buffer in form:
                    // 'value \t originIndex \t confidenceIndex (\t error)'
                    if (isPropSet(property))
                    {
                        if (property->GetType() == vobsFLOAT_PROPERTY)
                        {
                            if (IS_TRUE(simple))
                            {
                                FAIL(property->GetFormattedValue(&converted));
                                AppendString(converted);
                            }
                            else
                            {
                                FAIL(property->GetValue(&numerical));
                                // Export numeric values with maximum precision (up to 15-digits)
                                sprintf(converted, FORMAT_MAX_PRECISION, numerical);
                                AppendString(converted);
                            }
                        }
                        else if (property->GetType() == vobsSTRING_PROPERTY)
                        {
                            AppendString(property->GetValue());
                        }
                        else
                        {
                            // Integer or Boolean values are converted to integer values as string
                            FAIL(property->GetFormattedValue(&converted));
                            AppendString(converted);
                        }
                    }
                    AppendString("\t");

                    if (IS_TRUE(extendedFormat))
                    {
                        AppendString(vobsGetOriginIndexAsInt(property->GetOriginIndex()));
                        AppendString("\t");
                        AppendString(vobsGetConfidenceIndexAsInt(property->GetConfidenceIndex()));
                        AppendString("\t");
                    }

                    if (IS_NOT_NULL(property->GetErrorMeta()))
                    {
                        if (IS_TRUE(property->IsErrorSet()))
                        {
                            if (IS_TRUE(simple))
                            {
                                FAIL(property->GetFormattedError(&converted));
                                AppendString(converted);
                            }
                            else
                            {
                                FAIL(property->GetError(&numerical));
                                // Export numeric values with maximum precision (up to 15-digits)
                                sprintf(converted, FORMAT_MAX_PRECISION, numerical);
                                AppendString(converted);
                            }
                        }
                        AppendString("\t");

                        // origin and confidence indexes for error are useless
                        if (IS_TRUE(extendedFormat))
                        {
                            AppendString("\t\t");
                        }
                    }
                }
            }

            // If it's not the last star of the list
            if (starIdx != (nbStars - 1))
            {
                // Go to the next line
                AppendString("\n");
            }

            FAIL(SaveBufferIfNeeded());
        }
        return mcsSUCCESS;
    }

    /**
     * Parse the CDATA section.
     *
     * This method parses the CDATA section defined by the parameter
     * description. The CDATA section contains a list of stars which are stored
     * in a CSV table where each line corresponds to a star; i.e. all star
     * properties are placed on one line, and separated by the '\\t' character.
     * The list of star properties to be extracted is given by \em ucd.ucdName
     * list.
     *
     * The first lines of the CDATA containing the description of the parameters
     * has to be skipped. The number of lines to be skipped is given by \em
     * _nbLinesToSkip
     *
     * The found stars are put in the \em starList parameter.
     *
     * \param object type of object contained in the list (polymophism).
     * \param objectList list where extracted stars should be put.
     * \param extendedFormat if true, each property is stored with its attributes
     * (origin and confidence index), otherwise only the value is stored (e.g
     * when loading the local catalog).
     *
     * \return mcsSUCCESS on successful completion, mcsFAILURE otherwise.
     */
    template <class Star, class list>
    mcsCOMPL_STAT Extract(Star& object, list &objectList,
                          mcsLOGICAL extendedFormat = mcsFALSE,
                          vobsCATALOG_STAR_PROPERTY_CATALOG_MAPPING* propertyCatalogMap = NULL)
    {
        const bool isLogTest = doLog(logTEST);
        const bool isLogDebug = doLog(logDEBUG);
        const bool isLogTrace = doLog(logTRACE);

        const bool usePropertyCatalogMap = IS_NOT_NULL(propertyCatalogMap);

        const vobsORIGIN_INDEX catalogId = _catalogId;
        const char* const catalogName = vobsGetOriginIndex(catalogId);

        // may be not defined:
        const vobsCATALOG_META* catalogMeta = _catalogMeta;
        const bool useCatalogMeta = IS_NOT_NULL(catalogMeta);

        if (isLogDebug)
        {
            logDebug("vobsCDATA::Extract: catalogMeta [%s][%s]",
                     (useCatalogMeta) ? catalogMeta->GetId() : "UNDEFINED", catalogName);
        }

        // Number of UCDs per line
        mcsUINT32 nbOfUCDSPerLine = GetNbParams();

        // Determine the number of attributes per property
        mcsUINT32 nbOfAttributesPerProperty = 1;
        // If extended format then nb attributes per properties is
        // 3 (value, origin index, confidence index) else 1 (value only)
        if (IS_TRUE(extendedFormat))
        {
            nbOfAttributesPerProperty = 3;
        }

        // Find matching Param/UCD in star properties:
        vobsSTAR_PROPERTY* property;

        // flag indicating if the value is property error or a property value
        bool isError;

        // flag indicating RA or DEC property:
        bool isRaDec;

        // special case of catalog II/225 (CIO)
        bool isCatalogCIO = isCatalogCio(catalogId);
        bool isWaveLength;
        bool isFlux;
        // global flag indicating special case (wavelength or flux)
        bool isWaveLengthOrFlux = false;

        // star properties:
        vobsSTAR_PROPERTY * properties[nbOfUCDSPerLine];
        bool propIsError[nbOfUCDSPerLine];
        bool propIsRaDec[nbOfUCDSPerLine];
        bool propIsWaveLength[nbOfUCDSPerLine];
        bool propIsFlux[nbOfUCDSPerLine];

        if (isLogTest)
        {
            logTest("Extract: Property / Parameter mapping for catalog '%s':", catalogName);
        }

        bool isPropWLenOrFlux;
        vobsCATALOG_COLUMN* columnMeta;
        const char* propertyID;
        char *paramName, *ucdName;
        const vobsSTAR_PROPERTY_META* propMeta;

        for (mcsUINT32 el = 0; el < nbOfUCDSPerLine; el++)
        {
            // Get the parameter name and UCD
            FAIL(GetNextParamDesc(&paramName, &ucdName, (mcsLOGICAL) (el == 0)));

            if (isLogDebug)
            {
                logDebug("Extracting parameter '%s' (UCD = '%s') :", paramName, ucdName);
            }

            // reset first:
            propertyID = NULL;
            property = NULL;
            isError = false;
            isRaDec = false;
            isWaveLength = false;
            isFlux = false;
            // property flag indicating special case (wavelength or flux)
            isPropWLenOrFlux = false;

            // If catalog is the special case of catalog II/225 (CIO)
            if (isCatalogCIO)
            {
                isWaveLength = (strcmp(ucdName, vobsSTAR_INST_WAVELENGTH_VALUE) == 0);
                isFlux = (strcmp(ucdName, vobsSTAR_PHOT_FLUX_IR_MISC) == 0);

                if (isWaveLength || isFlux)
                {
                    isPropWLenOrFlux = true;
                    // update global flag:
                    isWaveLengthOrFlux = true;
                }
            }

            // all other properties behave normally:
            if (!isPropWLenOrFlux)
            {
                if (useCatalogMeta)
                {
                    // Try catalog meta first:
                    columnMeta = catalogMeta->GetColumnMeta(paramName);

                    if (IS_NULL(columnMeta))
                    {
                        if (isLogTest)
                        {
                            logTest("Extract: Parameter '%s' not found in catalog meta [%s][%s] !",
                                    paramName, catalogMeta->GetId(), catalogName);
                        }
                    }
                    else
                    {
                        propertyID = columnMeta->GetPropertyId();
                        isError    = columnMeta->IsError();

                        // resolve property using property index (faster):
                        property   = object.GetProperty(columnMeta->GetPropertyIdx());

                        if (isLogDebug)
                        {
                            logDebug("ColumnMeta[%s]: property [%s]", columnMeta->GetId(), columnMeta->GetPropertyId());
                        }
                    }
                }

                // Fallback mode (no catalog meta data)
                if (IS_NULL(propertyID))
                {
                    // resolve property:
                    property = object.GetProperty(ucdName);

                    if (IS_NOT_NULL(property))
                    {
                        propertyID = ucdName;
                    }
                    else
                    {
                        // resolve property error:
                        property = object.GetPropertyError(ucdName);

                        if (IS_NOT_NULL(property))
                        {
                            isError = true;
                            propertyID = ucdName;
                        }
                    }

                    if (IS_NOT_NULL(propertyID))
                    {
                        if (isLogDebug)
                        {
                            logDebug("\tUCD '%s' is a known property [%s]", ucdName, propertyID);
                        }
                    }
                    else if (isLogDebug)
                    {
                        logDebug("Extract: Parameter '%s' is NOT a known property ID '%s' !", paramName, ucdName);
                    }
                }
            }

            if (IS_NOT_NULL(propertyID))
            {
                isRaDec = isPropRA(propertyID) || isPropDEC(propertyID);
            }

            if (IS_NULL(property))
            {
                if (isWaveLength)
                {
                    logTest("Extract: Wavelength Property found for parameter '%s' (UCD='%s') in catalog '%s'",
                            paramName, ucdName, catalogName);
                }
                else if (isFlux)
                {
                    logTest("Extract: Flux property found for parameter '%s' (UCD='%s') in catalog '%s'",
                            paramName, ucdName, catalogName);
                }
                else if (isLogDebug)
                {
                    logDebug("Extract: No property found for parameter '%s' (UCD = '%s') in catalog '%s'",
                             paramName, ucdName, catalogName);
                }
            }
            else
            {
                propMeta = (isError) ? property->GetErrorMeta() : property->GetMeta();

                if (isLogTest)
                {
                    logTest("Extract: Property '%s' [%s] found for parameter '%s' (UCD='%s')",
                            propMeta->GetName(), propMeta->GetId(), paramName, ucdName);
                }

                if (usePropertyCatalogMap)
                {
                    bool add = true;

                    if (propertyCatalogMap->count(propMeta) > 0)
                    {
                        std::pair<vobsCATALOG_STAR_PROPERTY_CATALOG_MAPPING::iterator, vobsCATALOG_STAR_PROPERTY_CATALOG_MAPPING::iterator> range = propertyCatalogMap->equal_range(propMeta);

                        // Find the last catalogName:
                        range.second--;
                        if (strcmp(range.second->second, catalogName) == 0)
                        {
                            add = false;
                        }
                    }

                    if (add)
                    {
                        propertyCatalogMap->insert(vobsCATALOG_STAR_PROPERTY_CATALOG_PAIR(propMeta, catalogName));
                    }
                }
            }

            // memorize star property because star is one single instance so
            // vobsSTAR_PROPERTY* is constant during the main loop:
            properties[el] = property;

            // is error ?
            propIsError[el] = isError;

            // memorize wavelength/flux flags:
            propIsWaveLength[el] = isWaveLength;
            propIsFlux[el] = isFlux;

            // is RA or DEC:
            propIsRaDec[el] = isRaDec;
        }

        // Get flux properties in the johnson order (J,H,K,L,M,N)
        vobsSTAR_PROPERTY * fluxProperties[6];

        if (isWaveLengthOrFlux)
        {
            // get flux properties for special case of catalog II/225 (CIO)
            fluxProperties[0] = object.GetProperty(vobsSTAR_PHOT_JHN_J);
            fluxProperties[1] = object.GetProperty(vobsSTAR_PHOT_JHN_H);
            fluxProperties[2] = object.GetProperty(vobsSTAR_PHOT_JHN_K);
            fluxProperties[3] = object.GetProperty(vobsSTAR_PHOT_JHN_L);
            fluxProperties[4] = object.GetProperty(vobsSTAR_PHOT_JHN_M);
            fluxProperties[5] = object.GetProperty(vobsSTAR_PHOT_JHN_N);
        }
        else
        {
            for (mcsUINT32 i = 0; i < 6; i++)
            {
                fluxProperties[i] = NULL;
            }
        }

        // +1 for characters at EOL
        mcsUINT32 nbOfTokens = nbOfUCDSPerLine * nbOfAttributesPerProperty + 1;

        //        mcsSTRING256 lineSubStrings[nbOfTokens];
        char* lineSubStrings[nbOfTokens];
        mcsUINT32 maxSubStrLen[nbOfTokens];

        mcsUINT32 i, el, realIndex, len;

        // allocate string table:
        for (el = 0; el < nbOfUCDSPerLine; el++)
        {
            // Get related property:
            property = properties[el];

            if (IS_NOT_NULL(property) && (strcmp(property->GetId(), vobsSTAR_XM_LOG) == 0))
            {
                len = 16384; // 16K
            }
            else
            {
                len = 256; // default length
            }
            realIndex = el * nbOfAttributesPerProperty;
            lineSubStrings[realIndex] = new char[len];
            maxSubStrLen[realIndex] = len;

            if (IS_TRUE(extendedFormat))
            {
                len = 3; // origin / confidence integer values only need 2 characters
                lineSubStrings[realIndex + 1] = new char[len]; // origin
                maxSubStrLen[realIndex + 1] = len;
                lineSubStrings[realIndex + 2] = new char[len]; // confidence
                maxSubStrLen[realIndex + 2] = len;
            }
        }
        // add extra slot (EOL):
        len = 256;
        realIndex = el * nbOfAttributesPerProperty;
        lineSubStrings[realIndex] = new char[len];
        maxSubStrLen[realIndex] = len;

        /* macro to free allocated string table */
#define vobsCDATA_FREE_SUB_STRINGS() \
{ for (el = 0; el < nbOfTokens; el++) { delete(lineSubStrings[el]); lineSubStrings[el] = NULL; } }

        const char* from = NULL;
        mcsSTRING_LINE line;
        const mcsUINT32 maxLineLength = sizeof (line) - 1;
        mcsINT32 nbOfLine = 0;
        mcsUINT32 nbOfSubStrings;
        char* value;
        mcsINT32 originValue;
        vobsORIGIN_INDEX originIndex;
        mcsINT32 confidenceValue;
        vobsCONFIDENCE_INDEX confidenceIndex;
        mcsSTRING32 wavelength;
        mcsSTRING32 flux;
        mcsDOUBLE lambdaValue;

        // For each line in the internal buffer, get the value for each defined
        // UCD (values are separated by '\t' characters), store them in object,
        // then add this new object to the given list.
        do
        {
            // Get next line of the internal buffer
            from = GetNextLine(from, line, maxLineLength);
            nbOfLine++;

            if (isLogDebug)
            {
                logDebug("Extract: Next line = '%s'", line);
            }

            if ((nbOfLine > _nbLinesToSkip) && IS_NOT_NULL(from) && IS_FALSE(miscIsSpaceStr(line)))
            {
                // Split line on '\t' character, and store each token
                FAIL_DO(miscSplitStringDyn(line, '\t', lineSubStrings, maxSubStrLen, nbOfTokens, &nbOfSubStrings), 
                        vobsCDATA_FREE_SUB_STRINGS());

                // Remove each token trailing and leading blanks
                for (i = 0; i < nbOfSubStrings; i++)
                {
                    miscTrimString(lineSubStrings[i], " ");

                    // logInfo("token[%d]: '%s'", i, lineSubStrings[i]);
                }

                if (isWaveLengthOrFlux)
                {
                    // Temporary variables to parse in special case of catalog II/225 (CIO)
                    wavelength[0] = '\0';
                    flux[0] = '\0';
                }

                // Clear completely star object:
                object.ClearValues();

                for (el = 0; el < nbOfUCDSPerLine; el++)
                {
                    // Get related property:
                    property = properties[el];

                    // flags ?
                    isError = propIsError[el];
                    isRaDec = propIsRaDec[el];

                    if (IS_NOT_NULL(property) && isLogDebug)
                    {
                        logDebug("Extract: property '%s' :", (isError) ? property->GetErrorId() : property->GetId());
                    }

                    // Get the value
                    realIndex = el * nbOfAttributesPerProperty;
                    if (realIndex < nbOfSubStrings)
                    {
                        // Value is the first token
                        value = lineSubStrings[realIndex];

                        if (IS_TRUE(extendedFormat))
                        {
                            // Origin is the second token
                            originValue = vobsORIG_NONE;
                            sscanf(lineSubStrings[realIndex + 1], "%d", &originValue);
                            originIndex = (vobsORIGIN_INDEX) originValue;

                            // Confidence is the third token
                            confidenceValue = vobsCONFIDENCE_NO;
                            sscanf(lineSubStrings[realIndex + 2], "%d", &confidenceValue);
                            confidenceIndex = (vobsCONFIDENCE_INDEX) confidenceValue;
                        }
                        else // In local catalog case
                        {
                            // Load the properties with the global catalog identifier as origin index
                            originIndex = catalogId;
                            confidenceIndex = vobsCONFIDENCE_HIGH;
                        }
                        if (isLogDebug)
                        {
                            logDebug("\tValue = '%s'; Origin = '%s'; Confidence = '%s'.", value,
                                     vobsGetOriginIndex(originIndex),
                                     vobsGetConfidenceIndex(confidenceIndex));
                        }
                    }
                    else
                    {
                        // End of line reached : stop UCD scan and skip to next line
                        if (isLogDebug)
                        {
                            logDebug("\tNO VALUE FOUND.");
                        }
                        break;
                    }

                    if (!isWaveLengthOrFlux)
                    {
                        // Only set property if the extracted value is not empty
                        if (IS_FALSE(miscIsSpaceStr(value)) && IS_NOT_NULL(property))
                        {
                            if (isRaDec)
                            {
                                // Custom string converter for RA/DEC:
                                // Replace ':' by ' ' if present
                                FAIL_DO(miscReplaceChrByChr(value, ':', ' '), 
                                        vobsCDATA_FREE_SUB_STRINGS());
                            }

                            if (isError)
                            {
                                FAIL_DO(object.SetPropertyError(property, value), 
                                        vobsCDATA_FREE_SUB_STRINGS());
                            }
                            else
                            {
                                // Ignore any failure when parsing double values:
                                if (object.SetPropertyValue(property, value, originIndex, confidenceIndex) == mcsFAILURE)
                                {
                                    // Log error (for debugging only)
                                    errCloseStack();

                                    logInfo("Bad data line: [%s]", line);

                                    // reset property anyway:
                                    object.ClearPropertyValue(property);
                                }
                            }
                        }

                        // go to next parameter:
                        continue;
                    }

                    // special case of catalog II/225 (CIO)

                    isWaveLength = propIsWaveLength[el];
                    isFlux = propIsFlux[el];

                    // Specific treatement of the flux
                    // If wavelength is found, save it
                    if (isWaveLength)
                    {
                        strncpy(wavelength, value, sizeof (wavelength) - 1);
                    }
                    else if (isFlux)
                    {
                        // If flux is found, save it
                        strncpy(flux, value, sizeof (flux) - 1);
                    }
                    else
                    {
                        // Only set property if the extracted value is not empty
                        if (IS_FALSE(miscIsSpaceStr(value)) && IS_NOT_NULL(property))
                        {
                            if (isError)
                            {
                                FAIL_DO(object.SetPropertyError(property, value), 
                                        vobsCDATA_FREE_SUB_STRINGS());
                            }
                            else
                            {
                                FAIL_DO(object.SetPropertyValue(property, value, originIndex, confidenceIndex), 
                                        vobsCDATA_FREE_SUB_STRINGS());
                            }
                        }

                        // go to next parameter:
                        continue;
                    }

                    // If wavelength and flux have been found, find the
                    // corresponding magnitude band
                    if ((wavelength[0] != '\0') && (flux[0] != '\0'))
                    {
                        // Get the wavelength value
                        lambdaValue = -1.0;
                        if (sscanf(wavelength, "%lf", &lambdaValue) == 1)
                        {
                            property = NULL;

                            // Determine to corresponding magnitude
                            if ((lambdaValue >= (mcsDOUBLE) 1.24) && (lambdaValue <= (mcsDOUBLE) 1.26))
                            {
                                property = fluxProperties[0];
                            }
                            else if ((lambdaValue >= (mcsDOUBLE) 1.64) && (lambdaValue <= (mcsDOUBLE) 1.66))
                            {
                                property = fluxProperties[1];
                            }
                            else if ((lambdaValue >= (mcsDOUBLE) 2.19) && (lambdaValue <= (mcsDOUBLE) 2.21))
                            {
                                property = fluxProperties[2];
                            }
                            else if ((lambdaValue >= (mcsDOUBLE) 3.49) && (lambdaValue <= (mcsDOUBLE) 3.51))
                            {
                                property = fluxProperties[3];
                            }
                            else if ((lambdaValue >= (mcsDOUBLE) 4.99) && (lambdaValue <= (mcsDOUBLE) 5.01))
                            {
                                property = fluxProperties[4];
                            }
                            else if ((lambdaValue >= (mcsDOUBLE) 9.99) && (lambdaValue <= (mcsDOUBLE) 10.01))
                            {
                                property = fluxProperties[5];
                            }

                            // If the given flux correspond to an expected magnitude
                            if (IS_NOT_NULL(property))
                            {
                                if (isLogDebug)
                                {
                                    logDebug("\tFlux = '%s' and wavelength = '%s' --> magnitude band = '%s'",
                                             flux, wavelength, property->GetId());
                                }

                                // Set object property with extracted values
                                FAIL_DO(object.SetPropertyValue(property, flux, originIndex), 
                                        vobsCDATA_FREE_SUB_STRINGS());
                            }
                        }

                        // reset wavelength and flux:
                        wavelength[0] = '\0';
                        flux[0] = '\0';
                    }

                } // ucd parsing

                if (isLogTrace)
                {
                    object.Display(mcsTRUE);
                }

                // Store the object in the list
                objectList.AddAtTail(object);
            }
        } while (IS_NOT_NULL(from));

        vobsCDATA_FREE_SUB_STRINGS();
        
        // Print out error stack if it is not empty
        if (errStackIsEmpty() == mcsFALSE)
        {
            errCloseStack();
            return mcsFAILURE;
        }
        return mcsSUCCESS;
    }

protected:

private:
    // Declaration of copy constructor and assignment operator as private
    // methods, in order to hide them from the users.
    vobsCDATA(const vobsCDATA&);
    vobsCDATA& operator=(const vobsCDATA&) ;

    mcsCOMPL_STAT LoadParamsAndUCDsNamesLines(void);

    vobsSTR_LIST _paramName; // Name of parameters
    vobsSTR_LIST _ucdName; // Name of corresponding UCD
    vobsSTR_LIST::iterator _paramNameIterator;
    vobsSTR_LIST::iterator _ucdNameIterator;

    mcsINT32 _nbLinesToSkip; // Number of lines to be skipped in CDATA section
    mcsINT32 _nbLines; // Number of lines stored in buffer

    vobsORIGIN_INDEX _catalogId; // Catalog Id from where CDATA comming from
    const vobsCATALOG_META* _catalogMeta; // Catalog meta data from where CDATA comming from

} ;

#ifdef MODULE_ID_HACK
#undef MODULE_ID
#endif


#endif /*!vobsCDATA_H*/

/*___oOo___*/
