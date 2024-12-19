/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * @file
 * vobsSCENARIO class definition.
 */

/*
 * System Headers
 */
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <list>
using namespace std;


/*
 * MCS Headers
 */
#include "mcs.h"
#include "log.h"
#include "err.h"
#include "misc.h"
#include "timlog.h"
#include "sdb.h"

/*
 * Local Headers
 */
#include "vobsDISTANCE_FILTER.h"
#include "vobsSCENARIO.h"
#include "vobsPrivate.h"
#include "vobsErrors.h"

/**
 * Replace all occurences of the search string by the replace string in the given subject string
 * @param subject string to analyze
 * @param search token to find
 * @param replace token to replace by
 */
void ReplaceStringInPlace(std::string& subject, const std::string& search, const std::string& replace)
{
    size_t pos = 0;
    while ((pos = subject.find(search, pos)) != std::string::npos)
    {
        subject.replace(pos, search.length(), replace);
        pos += replace.length();
    }
}

/** Initialize static members */
bool vobsSCENARIO::vobsSCENARIO_DumpXML = false;

/*
 * Class constructor
 */
vobsSCENARIO::vobsSCENARIO(sdbENTRY* progress)
{
    // affect to NULL the catalog list
    _catalogList = NULL;
    _nbOfCatalogs = 0;
    _catalogIndex = 0;

    _progress = progress;

    // disable debugging flags by default:
    _saveSearchXml = mcsFALSE;
    _loadSearchList = false;
    _saveSearchList = false;
    _saveMergedList = false;

    // enable remove duplicates detection before the merge operation:
    _removeDuplicates = true;
}

/*
 * Class destructor
 */
vobsSCENARIO::~vobsSCENARIO()
{
    Clear();
}

/*
 * Public methods
 */

/**
 * Return the name of this scenario
 * @return "JSDC"
 */
const char* vobsSCENARIO::GetScenarioName() const
{
    return "UNDEF";
}

/**
 * Initialize the scenario and dump its configuration
 *
 * @param request the user constraint the found stars should conform to
 * @param starList optional input list
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned
 */
mcsCOMPL_STAT vobsSCENARIO::DumpAsXML(miscoDYN_BUF &xmlBuf, vobsREQUEST* request, vobsSTAR_LIST* starList)
{
    // maybe reuse context ?
    vobsSCENARIO_RUNTIME ctx;
    FAIL(Init(ctx, request, starList));

    // Prepare buffer:
    FAIL(xmlBuf.Reset());
    FAIL(xmlBuf.Reserve(8 * 1024));

    xmlBuf.AppendLine("<?xml version=\"1.0\"?>\n\n");

    FAIL(DumpAsXML(xmlBuf));

    mcsCOMPL_STAT result = mcsSUCCESS;

    // This file will be stored in the $MCSDATA/tmp repository
    mcsSTRING128 fileName;
    sprintf(fileName, "$MCSDATA/tmp/metadata/Scenario_%s.xml", GetScenarioName());

    // Resolve path
    char* resolvedPath = miscResolvePath(fileName);
    if (IS_NOT_NULL(resolvedPath))
    {
        logInfo("Saving scenario XML description: %s", resolvedPath);

        result = xmlBuf.SaveInASCIIFile(resolvedPath);
        free(resolvedPath);
    }
    return result;
}

/**
 * Dump the scenario as XML
 *
 * @param buffer buffer to append into
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned
 */
mcsCOMPL_STAT vobsSCENARIO::DumpAsXML(miscoDYN_BUF& buffer) const
{
    FAIL(buffer.AppendString("<scenario>\n"));
    FAIL(buffer.AppendString("  <name>"));
    FAIL(buffer.AppendString(GetScenarioName()));
    FAIL(buffer.AppendString("</name>\n"));

    FAIL(buffer.AppendString("  <removeDuplicates>"));
    FAIL(buffer.AppendString((_removeDuplicates) ? "true" : "false"));
    FAIL(buffer.AppendString("</removeDuplicates>\n"));

    FAIL(buffer.AppendString("\n  <scenarioEntryList>"));

    mcsSTRING32 tmp;
    
    // loop variables:
    vobsSCENARIO_ENTRY* entry;
    vobsORIGIN_INDEX catalogId;
    const char* catalogName;
    mcsUINT32 nStep = 0; // step count
    mcsSTRING512 logFileName;
    mcsSTRING32 scenarioName;
    mcsSTRING4 step;
    mcsSTRING32 catName;

    // Create an iterator on the scenario entries
    for (vobsSCENARIO_ENTRY_PTR_LIST::const_iterator _entryIterator = _entryList.begin(); _entryIterator != _entryList.end(); _entryIterator++)
    {
        entry = *_entryIterator;

        // Increment step count:
        nStep++;
        
        // Get entry information:
        catalogId    = entry->_catalogId;
        catalogName  = vobsGetOriginIndex(catalogId);

        sprintf(step, "%u", nStep);
        
        if (1)
        {
            // This file will be stored in the $MCSDATA/tmp repository
           strcpy(logFileName, "$MCSDATA/tmp/Search_");
           // Get scenario name, and replace ' ' by '_'
           strcpy(scenarioName, GetScenarioName());
           FAIL(miscReplaceChrByChr(scenarioName, ' ', '_'));
           strcat(logFileName, scenarioName);
           // Add step
           strcat(logFileName, "_");
           strcat(logFileName, step);
           // Get band used for search
           strcat(logFileName, "_");
           strcat(logFileName, "K"); /* request SearchBand */
           // Get catalog name, and replace '/' by '_'
           strcpy(catName, catalogName);
           FAIL(miscReplaceChrByChr(catName, '/', '_'));
           strcat(logFileName, "_");
           strcat(logFileName, catName);
           // Add request type (primary or not)
           strcat(logFileName, IS_NULL(entry->_listInput) ? "_1.log" : "_2.log");
        }

        FAIL(buffer.AppendString("\n    <scenarioEntry>\n"));

        FAIL(buffer.AppendString("      <logFileName>"));
        FAIL(buffer.AppendString(logFileName));
        FAIL(buffer.AppendString("</logFileName>\n"));

        FAIL(buffer.AppendString("      <step>"));
        FAIL(buffer.AppendString(step));
        FAIL(buffer.AppendString("</step>\n"));

        FAIL(buffer.AppendString("      <catalog>"));
        FAIL(buffer.AppendString(catalogName));
        FAIL(buffer.AppendString("</catalog>\n"));

        const char* queryOption = entry->GetQueryOption();
        if (IS_NOT_NULL(queryOption))
        {
            FAIL(buffer.AppendString("      <queryOption>"));
            // encode & char by &amp;
            std::string s = string(queryOption);
            ReplaceStringInPlace(s, "&", "&amp;");
            FAIL(buffer.AppendString(s.c_str()));
            FAIL(buffer.AppendString("</queryOption>\n"));
        }

        FAIL(buffer.AppendString("      <action>"));
        switch (entry->_action)
        {
            case vobsCLEAR_MERGE:
                FAIL(buffer.AppendString("CLEAR_MERGE"));
                break;
            case vobsMERGE:
                FAIL(buffer.AppendString("MERGE"));
                break;
            case vobsUPDATE_ONLY:
                FAIL(buffer.AppendString("UPDATE_ONLY"));
                break;

            default:
                FAIL(buffer.AppendString("UNDEFINED"));
        }
        FAIL(buffer.AppendString("</action>\n"));

        vobsSTAR_LIST* inputList = entry->_listInput;
        if (IS_NOT_NULL(inputList))
        {
            FAIL(buffer.AppendString("      <inputList>"));
            FAIL(buffer.AppendString(inputList->GetName()));
            FAIL(buffer.AppendString("</inputList>\n"));
        }

        vobsSTAR_LIST* outputList = entry->_listOutput;
        if (IS_NOT_NULL(outputList))
        {
            FAIL(buffer.AppendString("      <outputList>"));
            FAIL(buffer.AppendString(outputList->GetName()));
            FAIL(buffer.AppendString("</outputList>\n"));
        }

        vobsFILTER* filter = entry->_filter;
        if (IS_NOT_NULL(filter))
        {
            FAIL(buffer.AppendString("      <filter>"));
            FAIL(buffer.AppendString(filter->GetId()));
            // TODO: dump filter details

            FAIL(buffer.AppendString("</filter>\n"));
        }

        vobsSTAR_COMP_CRITERIA_LIST* criteriaList = entry->_criteriaList;
        if (IS_NOT_NULL(criteriaList))
        {
            FAIL(buffer.AppendString("      <criteriaList>\n"));

            // Get criteria informations:
            mcsINT32 nCriteria = 0;
            vobsSTAR_CRITERIA_INFO* criterias = NULL;

            FAIL(criteriaList->GetCriterias(criterias, nCriteria));

            if (nCriteria > 0)
            {
                vobsSTAR_CRITERIA_INFO* criteria = NULL;

                for (mcsINT32 i = 0; i < nCriteria; i++)
                {
                    criteria = &criterias[i];

                    FAIL(buffer.AppendString("        <criteria>\n"));

                    FAIL(buffer.AppendString("          <step>"));
                    sprintf(tmp, "%d", (i + 1));
                    FAIL(buffer.AppendString(tmp));
                    FAIL(buffer.AppendString("</step>\n"));

                    FAIL(buffer.AppendString("          <type>"));
                    switch (criteria->propCompType)
                    {
                        case vobsPROPERTY_COMP_RA_DEC:
                            FAIL(buffer.AppendString("RA_DEC_MATCH"));
                            break;
                        case vobsPROPERTY_COMP_FLOAT:
                            FAIL(buffer.AppendString("FLOAT_RANGE"));
                            break;
                        case vobsPROPERTY_COMP_STRING:
                            FAIL(buffer.AppendString("STRING_EQUAL"));
                            break;

                        default:
                            FAIL(buffer.AppendString("UNDEFINED"));
                    }
                    FAIL(buffer.AppendString("</type>\n"));

                    if (criteria->propCompType == vobsPROPERTY_COMP_RA_DEC)
                    {
                        // ra/dec criteria
                        FAIL(buffer.AppendString("          <area>"));
                        FAIL(buffer.AppendString((criteria->isRadius) ? "CONE" : "BOX"));
                        FAIL(buffer.AppendString("</area>\n"));

                        if (criteria->isRadius)
                        {
                            FAIL(buffer.AppendString("          <radius>"));
                            sprintf(tmp, "%0.2lf", criteria->rangeRA * alxDEG_IN_ARCSEC);
                            FAIL(buffer.AppendString(tmp));
                            FAIL(buffer.AppendString("</radius><!-- arcsec -->\n"));
                        }
                        else
                        {
                            FAIL(buffer.AppendString("          <ra>"));
                            sprintf(tmp, "%0.2lf", criteria->rangeRA * alxDEG_IN_ARCSEC);
                            FAIL(buffer.AppendString(tmp));
                            FAIL(buffer.AppendString("</ra><!-- arcsec -->\n"));

                            FAIL(buffer.AppendString("          <dec>"));
                            sprintf(tmp, "%0.2lf", criteria->rangeDEC * alxDEG_IN_ARCSEC);
                            FAIL(buffer.AppendString(tmp));
                            FAIL(buffer.AppendString("</dec><!-- arcsec -->\n"));
                        }
                    }
                    else
                    {
                        // other criteria
                        FAIL(buffer.AppendString("          <property>"));
                        FAIL(buffer.AppendString(criteria->propertyId));
                        FAIL(buffer.AppendString("</property>\n"));
                        FAIL(buffer.AppendString("          <other_property>"));
                        FAIL(buffer.AppendString(criteria->otherPropertyId));
                        FAIL(buffer.AppendString("</other_property>\n"));

                        if (criteria->propCompType == vobsPROPERTY_COMP_FLOAT)
                        {
                            FAIL(buffer.AppendString("          <range>"));
                            sprintf(tmp, "%.2lf", criteria->range);
                            FAIL(buffer.AppendString(tmp));
                            FAIL(buffer.AppendString("</range>\n"));
                        }
                    }
                    FAIL(buffer.AppendString("        </criteria>\n"));
                }
            }

            FAIL(buffer.AppendString("      </criteriaList>\n"));
        }

        FAIL(buffer.AppendString("    </scenarioEntry>\n"));
    }
    FAIL(buffer.AppendString("  </scenarioEntryList>\n"));

    FAIL(buffer.AppendString("</scenario>\n\n"));

    return mcsSUCCESS;
}

/**
 * Initialize the scenario.
 *
 * @param request the user constraint the found stars should conform to
 * @param starList optional input list
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
 */
mcsCOMPL_STAT vobsSCENARIO::Init(vobsSCENARIO_RUNTIME &ctx, vobsREQUEST* request, vobsSTAR_LIST* starList)
{
    logInfo("Scenario[%s] Init() used instead of sub class implementation !", GetScenarioName());

    return mcsFAILURE;
}

/**
 * Adds the element at the end of the list.
 *
 * The method create a entry from the parameters and put it in the list.
 * @param catalogId the identifier of the catalog to get
 * @param request request associated with the scenario
 * @param listInput list of star in enter of the research
 * @param listOutput list of star resulting of the asking of the catalog
 * @param action enumerate representing the action to do
 * @param criteriaList list of comparaison criteria
 * @param filter filter
 * @param queryOption query option
 *
 * @return
 * Always mcsSUCCESS.
 */
mcsCOMPL_STAT vobsSCENARIO::AddEntry(vobsORIGIN_INDEX catalogId,
                                     vobsREQUEST* request,
                                     vobsSTAR_LIST* listInput,
                                     vobsSTAR_LIST* listOutput,
                                     vobsACTION action,
                                     vobsSTAR_COMP_CRITERIA_LIST* criteriaList,
                                     vobsFILTER* filter,
                                     const char* queryOption)
{
    // Create a new entry
    // Affect in this entry the catalogName, the list input, the list output,
    // the action to do, and the criteria list
    vobsSCENARIO_ENTRY* entry = new vobsSCENARIO_ENTRY(catalogId,
                                                       request,
                                                       listInput,
                                                       listOutput,
                                                       action,
                                                       criteriaList,
                                                       filter);
    // Set query option
    if (IS_NOT_NULL(queryOption))
    {
        entry->SetQueryOption(queryOption);
    }

    // Put element in the list
    _entryList.push_back(entry);

    // Increment true catalog counter (if not a filter)
    if (isCatalog(catalogId))
    {
        _nbOfCatalogs++;
    }

    return mcsSUCCESS;
}

/**
 * Execute the scenario
 *
 * The methods execute the scenario which had been loaded before. It will
 * read each entry and query the specific catalog.
 * The scenario execution progress is reported using sdbENTRY instance given to
 * constructor. It writes a message having the following format:
 *      &lt;status&gt; &lt;catalog name&gt; &lt;catalog number&gt;
 *      &lt;number of catalogs&gt;
 * where
 *  <li> &lt;status&gt; is 1 meaning 'In progress'
 *  <li> &lt;catalog name&gt; is the name of catalog currently queried
 *  <li> &lt;catalog number&gt; is the catalog number in the list
 *  <li> &;number of catalogs&gt; is the number of catalogs in the list
 *
 * @param starList vobsSTAR_LIST which is the result of the interrogation,
 * this is the last list return of the last interrogation.
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
 */
mcsCOMPL_STAT vobsSCENARIO::Execute(vobsSCENARIO_RUNTIME &ctx, vobsSTAR_LIST &starList)
{
    logInfo("Scenario[%s] Execute() start", GetScenarioName());

    // Check if the list is not NULL, i.e the SetCatalogList has not been called before
    FAIL_NULL_DO(_catalogList, errAdd(vobsERR_CATALOG_LIST_EMPTY));

    mcsUINT32 nStep = 0; // step count
    mcsINT64 elapsedTime = 0; // current search time
    mcsINT64 sumSearchTime = 0; // cumulative search time

    // define action for timlog trace
    mcsSTRING256 timLogActionName;

    // loop variables:
    vobsSCENARIO_ENTRY* entry;
    vobsORIGIN_INDEX catalogId;
    const char* catalogName;
    vobsSTAR_LIST *inputList, *outputList;
    vobsFILTER* filter;
    vobsSTAR_COMP_CRITERIA_LIST* criteriaList;
    vobsACTION action;
    const char* actionName;

    bool hasCatalog;

    mcsUINT32 inputSize;
    mcsSTRING32 catalog;
    mcsSTRING256 message;
    vobsCATALOG* tempCatalog;
    vobsREQUEST* request;
    mcsINT32 nCriteria = 0;
    vobsSTAR_CRITERIA_INFO* criterias;
    vobsSTAR_CRITERIA_INFO* criteria;
    mcsDOUBLE radius;
    mcsSTRING512 logFileName;
    mcsSTRING32 scenarioName;
    mcsSTRING4 step;
    mcsSTRING32 catName;
    char* resolvedPath;
    mcsCOMPL_STAT loadedStatus;


    // Create a temporary list of star in which will be store the list input
    vobsSTAR_LIST tmpListA("Temporary_1");

    // Create a temporary list of star used to manage duplicates and load intermediate results
    vobsSTAR_LIST tmpListB("Temporary_2");

    // Create an iterator on the scenario entries
    for (vobsSCENARIO_ENTRY_PTR_LIST::iterator _entryIterator = _entryList.begin(); _entryIterator != _entryList.end(); _entryIterator++)
    {
        entry = *_entryIterator;

        // Increment step count:
        nStep++;

        // First clear the temporary list
        tmpListA.Clear();

        // Get entry information:
        catalogId    = entry->_catalogId;
        catalogName  = vobsGetOriginIndex(catalogId);
        inputList    = entry->_listInput;
        outputList   = entry->_listOutput;
        filter       = entry->_filter;
        criteriaList = entry->_criteriaList;
        action       = entry->_action;

        // Get action as string:
        actionName = vobsGetAction(action);

        // Test if this entry has a catalog to query
        hasCatalog = isCatalog(catalogId);

        // Get input list size
        inputSize = IS_NOT_NULL(inputList) ? inputList->Size() : 0;

        // Copy the list input into the temporary list
        if (inputSize > 0)
        {
            logTest("Execute: Step %d - inputList[%s] action[%s]", nStep, inputList->GetName(), actionName);

            // DEEP copy because inputList => tempList and outputList (= inputList) clear() will
            // delete also stars present in tempList (vobsCLEAR_MERGE only case)
            if (!hasCatalog && (inputList == outputList) && (action == vobsCLEAR_MERGE))
            {
                tmpListA.Copy(*inputList);
            }
            else
            {
                // only copy star pointers (still managed by input list to free them):
                tmpListA.CopyRefs(*inputList, mcsFALSE);
            }
        }
        else
        {
            logTest("Execute: Step %d - inputList[NONE] action[%s]", nStep, actionName);
        }

        // **** CATALOG QUERYING ****

        // If there is a catalog to query
        if (hasCatalog)
        {
            if ((action == vobsUPDATE_ONLY) && (inputSize == 0))
            {
                /* Do not perform secondary requests (vobsUPDATE_ONLY) if the input list is empty */
                logTest("Execute: Step %d - Skipping querying %s (empty input list)", nStep, catalogName);
            }
            else
            {
                // Get catalog name, and replace '/' by '_'
                strcpy(catalog, catalogName);
                FAIL(miscReplaceChrByChr(catalog, '/', '_'));
                strcpy(timLogActionName, catalog);
                // Add request type (primary or not)
                strcat(timLogActionName, (inputSize == 0) ? "_PRIMARY" : "_SECONDARY");

                // Write the current action in the shared database
                snprintf(message, sizeof (message) - 1, "1\t%s\t%u\t%u", catalogName, (_catalogIndex + 1), _nbOfCatalogs);
                FAIL(_progress->Write(message));

                // Get catalog from list
                tempCatalog = _catalogList->Get(catalogId);
                FAIL_NULL_DO(tempCatalog, errAdd(vobsERR_UNKNOWN_CATALOG));

                // Start research in entry's catalog
                logTest("Execute: Step %d - Querying %s [%s] ...", nStep, catalogName, tempCatalog->GetId());

                // Get request:
                request = entry->_request;

                // Optimize query radius:
                request->SetConeSearchRadius(-1.0); // means undefined

                if ((inputSize > 0) && IS_NOT_NULL(criteriaList))
                {
                    // Get criteria informations:
                    nCriteria = 0;
                    criterias = NULL;

                    FAIL(criteriaList->GetCriterias(criterias, nCriteria));

                    if (nCriteria > 0)
                    {
                        // note: RA_DEC criteria is always the first one
                        criteria = &criterias[0];

                        if ((criteria->propCompType == vobsPROPERTY_COMP_RA_DEC) && (criteria->isRadius))
                        {
                            // convert degrees to arcsec:
                            radius = vobsSTAR_CRITERIA_RADIUS_MATES + (criteria->rangeRA * alxDEG_IN_ARCSEC);
                            logTest("Execute: Step %d - optimized cone search radius=%0.1lf arcsec", nStep, radius);
                            request->SetConeSearchRadius(radius);
                        }
                    }
                }

                // Start time counter
                timlogInfoStart(timLogActionName);

                loadedStatus = mcsFAILURE;
                logFileName[0] = '\0';
                
                if (_loadSearchList || (_saveSearchList || doLog(logDEBUG))) {
                    // This file will be stored in the $MCSDATA/tmp repository
                    strcpy(logFileName, "$MCSDATA/tmp/Search_");
                    // Get scenario name, and replace ' ' by '_'
                    strcpy(scenarioName, GetScenarioName());
                    FAIL(miscReplaceChrByChr(scenarioName, ' ', '_'));
                    strcat(logFileName, scenarioName);
                    // Add step
                    sprintf(step, "%u", nStep);
                    strcat(logFileName, "_");
                    strcat(logFileName, step);
                    // Get band used for search
                    strcat(logFileName, "_");
                    strcat(logFileName, request->GetSearchBand());
                    // Get catalog name, and replace '/' by '_'
                    strcpy(catName, catalogName);
                    FAIL(miscReplaceChrByChr(catName, '/', '_'));
                    strcat(logFileName, "_");
                    strcat(logFileName, catName);
                    // Add request type (primary or not)
                    strcat(logFileName, IS_NULL(inputList) ? "_1.log" : "_2.log");
                }

                // If the loadSearchList flag is enabled, search
                // results can be loaded from file
                if (_loadSearchList)
                {
                    // Resolve path
                    resolvedPath = miscResolvePath(logFileName);

                    if (IS_NOT_NULL(resolvedPath))
                    {
                        logTest("Execute: Step %d - Load star list from: %s", nStep, resolvedPath);

                        // tmpListB is only used temporarly:
                        tmpListB.Clear();

                        // Try loading previous results with extended format (origin / confidence indexes)
                        loadedStatus = tmpListB.Load(resolvedPath, NULL, NULL, mcsTRUE);
                        free(resolvedPath);

                        if (loadedStatus == mcsFAILURE)
                        {
                            // Ignore error (for test only)
                            errCloseStack();
                            // will perform catalog search with inputs in tempList (as usual)
                        }
                        else
                        {
                            logTest("Loaded star list: %d", tmpListB.Size());

                            // clear the temporary list to store results:
                            tmpListA.Clear();

                            if (tmpListB.Size() != 0)
                            {
                                // just move stars into given list:
                                tmpListA.CopyRefs(tmpListB);
                            }
                        }

                        // clear anyway:
                        tmpListB.Clear();
                    }
                }

                if (loadedStatus == mcsFAILURE)
                {
                    // if research failed, return mcsFAILURE and tempList is empty
                    FAIL_DO(tempCatalog->Search(ctx, *request, tmpListA, entry->GetQueryOption(), &_propertyCatalogMap, _saveSearchXml),
                            timlogCancel(timLogActionName));
                }

                // Stop time counter
                timlogStopTime(timLogActionName, &elapsedTime);
                sumSearchTime += elapsedTime;

                // define catalog id / meta in temporary list:
                tmpListA.SetCatalogMeta(catalogId, tempCatalog->GetCatalogMeta());

                // If the saveSearchList flag is enabled
                // or the verbose level is higher or equal to debug level, search
                // results will be stored in file
                if ((loadedStatus == mcsFAILURE)
                        && (_saveSearchList || doLog(logDEBUG)))
                {
                    // Resolve path
                    resolvedPath = miscResolvePath(logFileName);
                    if (IS_NOT_NULL(resolvedPath))
                    {
                        logTest("Execute: Step %d - Save star list to: %s", nStep, resolvedPath);
                        // Save resulting list with extended format (origin / confidence indexes)
                        FAIL_DO(tmpListA.Save(resolvedPath, mcsTRUE), free(resolvedPath));
                        free(resolvedPath);
                    }
                }

                // Anyway perform custom post processing:
                FAIL_DO(tempCatalog->PostProcessList(tmpListA),
                            timlogCancel(timLogActionName));

                logTest("Execute: Step %d - number of returned stars=%d", nStep, tmpListA.Size());

                // DETECT duplicates on PRIMARY requests ONLY for catalogs not returning multiple rows:
                if (((action != vobsUPDATE_ONLY) || (inputSize == 0))
                        && IS_FALSE(tmpListA.GetCatalogMeta()->HasMultipleRows()))
                {
                    // note: tmpListB is only used temporarly:
                    FAIL(tmpListB.FilterDuplicates(tmpListA, criteriaList, _removeDuplicates));
                }
            }

            _catalogIndex++;

        } // has catalog

        // **** LIST COPYING/MERGING ****
        logTest("Execute: Step %d - outputList[%s]", nStep, outputList->GetName());
        logTest("Execute: Step %d - Performing action[%s] with %d stars", nStep, actionName, tmpListA.Size());

        // There are 3 different action to do when the scenario is executed
        switch (action)
        {
            case vobsCLEAR_MERGE:
            {
                // First action is vobsCLEAR_MERGE. The list output will be cleared and
                // it will be merge from the temporary list which contain the list input or query results
                logTest("Execute: Step %d - CLEAR outputList[%s]", nStep, outputList->GetName());
                outputList->Clear();

                // Perform vobsMERGE case:
            }
            case vobsMERGE:
            {
                // Second action is vobsMERGE. The list output will be
                // merge from the temporary list without being cleared.
                // The information which is stored in the the list
                // output is preserved and can be modified
                FAIL(outputList->Merge(tmpListA, criteriaList, mcsFALSE));
                break;
            }
            case vobsUPDATE_ONLY:
            {
                // Third action is vobsUPDATE_ONLY. The list output will
                // be merge from the temporary list, but this merge will
                // not modified the existant information of the list output
                FAIL(outputList->Merge(tmpListA, criteriaList, mcsTRUE));
                break;
            }

            default:
                logWarning("Execute: Step %d - unsupported action=%d", action);
                break;
        }

        // Clear the temporary list (to free memory):
        tmpListA.Clear();

        logTest("Execute: Step %d - after action[%s]: %d stars", nStep, actionName, outputList->Size());

        // If the saveMergedList flag is enabled
        // or the verbose level is higher or equal to debug level, search
        // results will be stored in file
        if (_saveMergedList || doLog(logDEBUG))
        {
            // This file will be stored in the $MCSDATA/tmp repository
            strcpy(logFileName, "$MCSDATA/tmp/Merge_");
            // Get scenario name, and replace ' ' by '_'
            strcpy(scenarioName, GetScenarioName());
            FAIL(miscReplaceChrByChr(scenarioName, ' ', '_'));
            strcat(logFileName, scenarioName);
            // Add step
            sprintf(step, "%u", nStep);
            strcat(logFileName, "_");
            strcat(logFileName, step);
            strcat(logFileName, "_MERGE.log");

            // Resolve path
            resolvedPath = miscResolvePath(logFileName);
            if (IS_NOT_NULL(resolvedPath))
            {
                logTest("Execute: Step %d - Save star list to: %s", nStep, resolvedPath);
                // Save resulting list
                FAIL_DO(outputList->Save(resolvedPath), free(resolvedPath));
                free(resolvedPath);
            }
        }

        // **** LIST FILTERING ****

        // Apply filter if defined
        if (IS_NOT_NULL(filter))
        {
            logTest("Execute: Step %d - apply FILTER[%s]", nStep, filter->GetId());

            filter->Apply(outputList);

            logTest("Execute: Step %d - after FILTER[%s]: %d stars", nStep, filter->GetId(), outputList->Size());
        }
    } // loop on scenario entries

    logTest("Execute: resultList[%s]", starList.GetName());

    // Copy resulting list (only references):
    starList.CopyRefs(*_entryList.back()->_listOutput);

    // clear scenario lists...
    for (vobsSCENARIO_ENTRY_PTR_LIST::iterator _entryIterator = _entryList.begin(); _entryIterator != _entryList.end(); _entryIterator++)
    {
        entry = *_entryIterator;

        inputList = entry->_listInput;
        if (IS_NOT_NULL(inputList))
        {
            // clear input list
            inputList->Clear();
        }
        outputList = entry->_listOutput;
        if (IS_NOT_NULL(outputList))
        {
            // clear output list
            outputList->Clear();
        }
    }

    logInfo("Scenario[%s] Execute() done: %d star(s) found.", GetScenarioName(), starList.Size());

    if (sumSearchTime != 0)
    {
        mcsSTRING16 time;
        timlogFormatTime(sumSearchTime, &time);
        logInfo("Scenario[%s] total time in catalog queries %s", GetScenarioName(), time);
    }

    _catalogIndex = 0;

    if (doLog(logTEST) && (_propertyCatalogMap.size() > 0))
    {
        logTest("Scenario[%s] Property usage by catalogs", GetScenarioName());

        const vobsSTAR_PROPERTY_META* meta;
        const vobsSTAR_PROPERTY_META* current = NULL;
        std::string buffer;

        for (vobsCATALOG_STAR_PROPERTY_CATALOG_MAPPING::const_iterator iter = _propertyCatalogMap.begin(); iter != _propertyCatalogMap.end(); iter++)
        {
            meta = iter->first;
            catalogName = iter->second;

            if (meta != current)
            {
                if (IS_NOT_NULL(current))
                {
                    logTest("Property '%s' [%s]: %s", current->GetId(), current->GetName(), buffer.c_str());
                }
                current = meta;
                buffer.clear();
            }
            // add catalog name into buffer:
            buffer.append(catalogName).append("\t");
        }
        if (IS_NOT_NULL(current))
        {
            logTest("Property '%s' [%s]: %s", current->GetId(), current->GetName(), buffer.c_str());
        }
    }
    // anyway prune property usage map:
    _propertyCatalogMap.clear();

    return mcsSUCCESS;
}

/**
 * Clear the scenario
 *
 * @return always mcsSUCCESS
 */
mcsCOMPL_STAT vobsSCENARIO::Clear(void)
{
    _catalogList = NULL;

    // Deallocate all entries
    for (vobsSCENARIO_ENTRY_PTR_LIST::iterator iter = _entryList.begin(); iter != _entryList.end(); iter++)
    {
        delete(*iter);
    }

    _entryList.clear();
    _nbOfCatalogs = 0;
    _catalogIndex = 0;

    return mcsSUCCESS;
}


/*___oOo___*/
