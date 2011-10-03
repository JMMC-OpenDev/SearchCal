#ifndef vobsSTAR_COMP_CRITERIA_LIST_H
#define vobsSTAR_COMP_CRITERIA_LIST_H
/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * \file
 * vobsSTAR_COMP_CRITERIA_LIST class declaration.
 */

#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif

/*
 * header files
 */
#include <map>

/*
 * local header
 */
#include "vobsSTAR_PROPERTY_META.h"

/* duplicate definition from vobsSTAR.h to avoid cyclic dependencies */
#define vobsSTAR_POS_EQ_RA_MAIN                 "POS_EQ_RA_MAIN"
#define vobsSTAR_POS_EQ_DEC_MAIN                "POS_EQ_DEC_MAIN"

typedef enum
{
    vobsPROPERTY_COMP_RA = 0,
    vobsPROPERTY_COMP_DEC = 1,
    vobsPROPERTY_COMP_OTHER = 2
} vobsPROPERTY_COMP_TYPE;


/**
 * Information used for fast traversal: see vobsSTAR::IsSame()
 */
struct vobsSTAR_CRITERIA_INFO
{
    // criteria members:
    const char*       propertyId;
    mcsDOUBLE         range;
    // internal members:
    int               propertyIndex;
    vobsPROPERTY_TYPE comparisonType;
    vobsPROPERTY_COMP_TYPE propCompType;
};

/*
 * Class declaration
 */

/**
 * vobsSTAR_COMP_CRITERIA_LIST is a class which caracterises a list of
 * criteria.
 * 
 * Note: child classes are not allowed as functions are not virtual for performance reasons !
 */
class vobsSTAR_COMP_CRITERIA_LIST
{

public:
    // Class constructor
    vobsSTAR_COMP_CRITERIA_LIST();
    explicit vobsSTAR_COMP_CRITERIA_LIST(const vobsSTAR_COMP_CRITERIA_LIST&);

    // Class destructor
    ~vobsSTAR_COMP_CRITERIA_LIST();

    // operator =
    vobsSTAR_COMP_CRITERIA_LIST& operator=(const vobsSTAR_COMP_CRITERIA_LIST&);
    
    // Method to clear the criteria list
    mcsCOMPL_STAT Clear();
    
    // Method to add a criteria in the list
    mcsCOMPL_STAT Add(const char* propertyId, mcsDOUBLE range);
    // Method to remove a criteria of the list
    mcsCOMPL_STAT Remove(const char* propertyId);

    // Method to get the number of criteria
    int Size();

    // Method to show criteria in logs
    void log(logLEVEL level, const char* prefix = "");
    
    // Method to prepare criteria traversal (lazily initialized)
    mcsCOMPL_STAT InitializeCriterias();
    
    // Method to get criteria
    mcsCOMPL_STAT GetCriterias(vobsSTAR_CRITERIA_INFO*& criteriaInfo, int& size);

protected:
    
private:

    /*
     * const char* comparator used by map<const char*, ...>
     * 
     * Special case: vobsSTAR_POS_EQ_RA_MAIN / vobsSTAR_POS_EQ_DEC_MAIN values first !!
     */
    struct RaDecStringComparator {
        /**
         * Return true if s1 < s2
         * @param s1 first  string
         * @param s2 second string
         * @return true if s1 < s2 
         */
        bool operator()(const char* s1, const char* s2) const {
            if (s1 == s2)
            {
                // lower (first):
                return false;
            }

            // hack RA / DEC are put FIRST (faster comparison that strings and most selective criteria)

            if (strcmp(s1, vobsSTAR_POS_EQ_RA_MAIN) == 0)
            {
                // S1 (first):
                return true;
            }
            
            if (strcmp(s2, vobsSTAR_POS_EQ_RA_MAIN) == 0)
            {
                // S2 (first):
                return false;
            }


            if (strcmp(s1, vobsSTAR_POS_EQ_DEC_MAIN) == 0)
            {
                // S1 (first):
                return true;
            }
            
            if (strcmp(s2, vobsSTAR_POS_EQ_DEC_MAIN) == 0)
            {
                // S2 (first):
                return false;
            }
            
            // return true if s1 < s2:
            return strcmp(s1, s2) < 0;
        }
    };

    // List of criteria
    std::map<const char*, mcsDOUBLE, RaDecStringComparator> _criteriaList;
    
    // flag indicating that criteria informations have been initialized 
    bool _initialized;
    
    // Internal members used for fast traversal
    int _size;
    vobsSTAR_CRITERIA_INFO* _criteriaInfos;
    
    void resetCriterias();
};

#endif /*!vobsSTAR_COMP_CRITERIA_LIST_H*/

/*___oOo___*/
