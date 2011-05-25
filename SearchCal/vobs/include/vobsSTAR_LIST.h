#ifndef vobsSTAR_LIST_H
#define vobsSTAR_LIST_H
/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif


/* 
 * System Headers 
 */
#include<list>

/*
 * MCS Headers 
 */
#include "miscoDYN_BUF.h"

/*
 * Local Headers 
 */
#include"vobsSTAR.h"

/*
 * Type declaration
 */
/**
 * typedef of UCD order list
 */
typedef std::list<string> vobsSTAR_PROPERTY_ID_LIST;


/*
 * Class declaration
 */
/**
 * vobsSTAR_LIST handles a list of stars.
 */
class vobsSTAR_LIST
{ 
 public:
    // Class constructors
    vobsSTAR_LIST();

    // Class destructor
    virtual ~vobsSTAR_LIST();
   
    virtual mcsLOGICAL    IsEmpty(void);
    virtual mcsCOMPL_STAT Clear(void);
    virtual mcsCOMPL_STAT AddAtTail(vobsSTAR &star);
    virtual mcsCOMPL_STAT Remove(vobsSTAR &star);
    virtual mcsUINT32     Size(void);
    virtual mcsCOMPL_STAT Copy(vobsSTAR_LIST& list);

    virtual vobsSTAR*     GetNextStar(mcsLOGICAL init = mcsFALSE);
    virtual vobsSTAR*     GetStar(vobsSTAR &star,
                                  vobsSTAR_COMP_CRITERIA_LIST *criteriaList=NULL);
    
    virtual mcsCOMPL_STAT Merge(vobsSTAR_LIST &list,
                                vobsSTAR_COMP_CRITERIA_LIST *criteriaList=NULL, 
                                mcsLOGICAL updateOnly=mcsFALSE);

    virtual mcsCOMPL_STAT Sort(char *propertyId,
                               mcsLOGICAL reverseOrder=mcsFALSE);

    virtual void          Display(void);

    virtual mcsCOMPL_STAT GetVOTable(const char*    header,
                                     const char*    softwareVersion,
                                     const char*    request,
                                     const char*    xmlRquest,
                                     miscoDYN_BUF*  buffer);

    virtual mcsCOMPL_STAT SaveToVOTable(const char *filename,
                                        const char *header,
                                        const char *softwareVersion,
                                        const char *request,
                                        const char *xmlRequest);

    virtual mcsCOMPL_STAT Save(const char *filename,
                               mcsLOGICAL extendedFormat=mcsFALSE);

    virtual mcsCOMPL_STAT Save(const char *filename,
                               vobsSTAR_PROPERTY_ID_LIST ucdList,
                               mcsLOGICAL extendedFormat=mcsFALSE); 
    virtual mcsCOMPL_STAT Load(const char *filename,
                               mcsLOGICAL extendedFormat=mcsFALSE,
                               const char *origin=NULL); 

protected:
    // List of stars
    std::list<vobsSTAR *>           _starList;
    std::list<vobsSTAR *>::iterator _starIterator;

private:
    // Declaration assignment operator as private
    // methods, in order to hide them from the users.
    vobsSTAR_LIST& operator=(const vobsSTAR_LIST&);
    vobsSTAR_LIST (const vobsSTAR_LIST& list);//copy constructor
};

#endif /*!vobSTAR_LIST_H*/


/*___oOo___*/
