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
#include<map>

/*
 * local header
 */

/*
 * Class declaration
 */

/**
 * vobsSTAR_COMP_CRITERIA_LIST is a class which caracterises a list of
 * criteria.
 */
class vobsSTAR_COMP_CRITERIA_LIST
{

public:
    // Class constructor
    vobsSTAR_COMP_CRITERIA_LIST();
    vobsSTAR_COMP_CRITERIA_LIST(vobsSTAR_COMP_CRITERIA_LIST&);

    // Class destructor
    virtual ~vobsSTAR_COMP_CRITERIA_LIST();

    // operator =
    vobsSTAR_COMP_CRITERIA_LIST& operator=(const vobsSTAR_COMP_CRITERIA_LIST&);
    
    // Method to clear the criteria list
    virtual mcsCOMPL_STAT Clear(void);
    
    // Method to add a criteria in the list
    virtual mcsCOMPL_STAT Add(const char* propertyId, mcsDOUBLE range);
    // Method to remove a criteria of the list
    virtual mcsCOMPL_STAT Remove(const char* propertyId);
    // Method to Get the next criteria
    virtual mcsCOMPL_STAT GetNextCriteria(const char** propertyId,
                                          mcsDOUBLE* range, mcsLOGICAL init = mcsFALSE);

    // Method to get the number of criteria
    virtual int Size(void);

protected:
    
private:
    // List of criteria
    std::map<const char *, mcsDOUBLE>           _criteriaList;
    std::map<const char *, mcsDOUBLE>::iterator _criteriaIterator;
};

#endif /*!vobsSTAR_COMP_CRITERIA_LIST_H*/

/*___oOo___*/
