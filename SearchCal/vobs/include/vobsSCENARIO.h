#ifndef vobsSCENARIO_H
#define vobsSCENARIO_H
/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * \file
 * Brief description of the header file, which ends at this dot.
 */

#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif


/*
 * System Header
 */
#include <list>


/*
 * MCS header
 */
#include "sdb.h"


/*
 * local Header
 */
#include "vobsSTAR_LIST.h"
#include "vobsCATALOG.h"
#include "vobsSTAR_COMP_CRITERIA_LIST.h"
#include "vobsSCENARIO_ENTRY.h"
#include "vobsCATALOG_LIST.h"
#include "vobsREQUEST.h"


/*
 * Class declaration
 */

/**
 * vobsSCENARIO is a class which represent the catalog interrogation scenario 
 * of the catalogues.
 * 
 * vobsSCENARIO methods allow to
 * \li Add action into the scenario : an action is represented by the
 * catalog of destination and the specific action to apply
 * \li Execute all the action loaded
 * 
 */

class vobsSCENARIO
{
public :
    vobsSCENARIO(sdbENTRY* progress);
    virtual ~vobsSCENARIO(); 
    
    mcsCOMPL_STAT AddEntry(const char*                  catalog,
                           vobsREQUEST*                 request,
                           vobsSTAR_LIST*               listInput,
                           vobsSTAR_LIST*               listOutput,
                           vobsACTION                   action,
                           vobsSTAR_COMP_CRITERIA_LIST* criteriaList = NULL,
                           vobsFILTER*                  filter       = NULL,
                           const char*                  queryOption  = NULL);

    virtual const char* GetScenarioName();

    virtual mcsCOMPL_STAT Init (vobsREQUEST* request);
    
    // Execute the scenario
    virtual mcsCOMPL_STAT Execute(vobsSTAR_LIST &starList);

    mcsCOMPL_STAT Clear(void);


    /**
     * Set catalog List
     *
     * This method affect to the pointer of catalog list the value of the pointer
     * gave as parmameter
     *
     * @param catalogList a catalog list
     */
    inline void SetCatalogList(vobsCATALOG_LIST* catalogList) __attribute__((always_inline))
    {
        // equal the two pointer
        _catalogList = catalogList;
    }

    /**
     * Return the total number of catalog queried by the scenario.
     *
     * @return an mcsUINT32 
     */
    inline mcsUINT32 GetNbOfCatalogs() const __attribute__((always_inline))
    {
        return _nbOfCatalogs;
    }

    /**
     * Return the current index of the catalog being queried.
     *
     * @return an mcsUINT32 
     */
    inline mcsUINT32 GetCatalogIndex() const __attribute__((always_inline))
    {
        return _catalogIndex;
    }
    
    
protected :
    // Progression monitoring
    sdbENTRY* _progress;
    
    // flag to save the xml output from any Search query
    bool _saveSearchXml;
    // flag to save the star list coming from any Search query
    bool _saveSearchList;
    // flag to save the star list after the merge operation
    bool _saveMergedList;
    // flag to detect duplicates before the merge operation
    bool _filterDuplicates;
    // flag to enable star index use to perform faster merge operations
    bool _enableStarIndex;

private :
    // Declaration of copy constructor and assignment operator as private
    // methods, in order to hide them from the users.
    vobsSCENARIO& operator=(const vobsSCENARIO&);
    vobsSCENARIO (const vobsSCENARIO&);

    // List of entries
    std::list<vobsSCENARIO_ENTRY*> _entryList;

    // pointer of list of catalog
    vobsCATALOG_LIST* _catalogList;
    mcsUINT32         _nbOfCatalogs;
    mcsUINT32         _catalogIndex;
};



#endif /*!vobsSCENARIO_H*/

/*___oOo___*/
