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
#define vobsSTAR_COMP_GAIA_MAGS                 "COMP_GAIA_MAGS"

/* define min radius to identify mates (arcsecs) */
#define vobsSTAR_CRITERIA_RADIUS_MATES          3.0 


/* convenience macros */
#define isPropRA(propertyID) \
    (strcmp(propertyID, vobsSTAR_POS_EQ_RA_MAIN) == 0)

#define isPropDEC(propertyID) \
    (strcmp(propertyID, vobsSTAR_POS_EQ_DEC_MAIN) == 0)

#define isCompGaiaMags(propertyID) \
    (strcmp(propertyID, vobsSTAR_COMP_GAIA_MAGS) == 0)

typedef enum
{
    vobsPROPERTY_COMP_RA_DEC = 0,
    vobsPROPERTY_COMP_FLOAT  = 1,
    vobsPROPERTY_COMP_STRING = 2,
    vobsPROPERTY_COMP_GAIA_MAGS = 3    
} vobsPROPERTY_COMP_TYPE;

/**
 * Information used for fast traversal: see vobsSTAR::IsSame()
 */
struct vobsSTAR_CRITERIA_INFO
{
    // criteria members:
    const char* propertyId;
    const char* otherPropertyId;
    mcsDOUBLE range;

    // internal members:
    mcsINT32 propertyIndex;
    mcsINT32 otherPropertyIndex;
    vobsPROPERTY_COMP_TYPE propCompType;

    // special case RA/DEC:
    // RA bounds are used to perform fast distance check
    mcsDOUBLE lowerBoundRA; // -180 + rangeRA
    mcsDOUBLE upperBoundRA; // +180 - rangeRA
    bool isRadius; // box or circular area
    mcsDOUBLE rangeRA; // rangeRA  in degrees
    mcsDOUBLE rangeDEC; // rangeDEC in degrees
} ;

/*
 * const char* comparator used by map<const char*, ...>
 *
 * Special case: vobsSTAR_POS_EQ_RA_MAIN / vobsSTAR_POS_EQ_DEC_MAIN values first !!
 */
struct RaDecStringComparator
{

    /**
     * Return true if s1 < s2
     * @param s1 first  string
     * @param s2 second string
     * @return true if s1 < s2
     */
    bool operator()(const char* s1, const char* s2) const
    {
        if (s1 == s2)
        {
            // lower (first):
            return false;
        }

        // hack RA / DEC are put FIRST (faster comparison that strings and most selective criteria)

        if (isPropRA(s1))
        {
            // S1 (first):
            return true;
        }

        if (isPropRA(s2))
        {
            // S2 (first):
            return false;
        }


        if (isPropDEC(s1))
        {
            // S1 (first):
            return true;
        }

        if (isPropDEC(s2))
        {
            // S2 (first):
            return false;
        }

        // return true if s1 < s2:
        return strcmp(s1, s2) < 0;
    }
} ;

/** Criteria information mapping keyed by criteria name type using const char* keys and custom comparator functor */
typedef std::map<const char*, mcsDOUBLE, RaDecStringComparator> vobsSTAR_CRITERIA_MAP;

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
    vobsSTAR_COMP_CRITERIA_LIST& operator=(const vobsSTAR_COMP_CRITERIA_LIST&) ;

    // Method to clear the criteria list
    mcsCOMPL_STAT Clear();

    // Method to add a criteria in the list
    mcsCOMPL_STAT Add(const char* propertyId, mcsDOUBLE range = 0.0);
    // Method to remove a criteria of the list
    mcsCOMPL_STAT Remove(const char* propertyId);

    // Method to get the number of criteria
    mcsINT32 Size();

    // Method to show criteria in logs
    void log(logLEVEL level, const char* prefix = "");

    // Method to prepare criteria traversal (lazily initialized)
    mcsCOMPL_STAT InitializeCriterias();

    // Method to get criteria
    mcsCOMPL_STAT GetCriterias(vobsSTAR_CRITERIA_INFO*& criteriaInfo, mcsINT32& size);

protected:

private:

    // List of criteria
    vobsSTAR_CRITERIA_MAP _criteriaList;

    // flag indicating that criteria informations have been initialized
    bool _initialized;

    // Internal members used for fast traversal
    mcsINT32 _size;
    vobsSTAR_CRITERIA_INFO* _criteriaInfos;

    void resetCriterias();
} ;

#endif /*!vobsSTAR_COMP_CRITERIA_LIST_H*/

/*___oOo___*/
