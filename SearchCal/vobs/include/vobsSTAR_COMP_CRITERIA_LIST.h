#ifndef vobsSTAR_COMP_CRITERIA_LIST_H
#define vobsSTAR_COMP_CRITERIA_LIST_H
/*******************************************************************************
* JMMC project
*
* "@(#) $Id: vobsSTAR_COMP_CRITERIA_LIST.h,v 1.7 2005-11-16 14:26:19 scetre Exp $"
*
* History
* -------
* $Log: not supported by cvs2svn $
* Revision 1.6  2005/11/16 10:47:54  scetre
* Updated documentation
*
* Revision 1.5  2005/11/16 10:47:54  scetre
* Updated documentation
*
* Revision 1.4  2005/01/26 08:10:32  scetre
* change history
*
* scetre    14-Dec-2004  Created
*
*
*******************************************************************************/

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
    virtual mcsCOMPL_STAT Add(char *propertyId, mcsFLOAT range);
    // Method to remove a criteria of the list
    virtual mcsCOMPL_STAT Remove(char *propertyId);
    // Method to Get the next criteria
    virtual mcsCOMPL_STAT GetNextCriteria(char *propertyId,
                                          mcsFLOAT *range,
                                          mcsLOGICAL init = mcsFALSE);

    // Method to get the number of criteria
    virtual int Size(void);

protected:
    
private:
    // List of criteria
    std::map<char *, float>           _criteriaList;
    std::map<char *, float>::iterator _criteriaIterator;
};

#endif /*!vobsSTAR_COMP_CRITERIA_LIST_H*/

/*___oOo___*/
