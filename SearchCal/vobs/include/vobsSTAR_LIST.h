#ifndef vobsSTAR_LIST_H
#define vobsSTAR_LIST_H
/*******************************************************************************
* JMMC project
*
* "@(#) $Id: vobsSTAR_LIST.h,v 1.16 2005-03-06 20:24:30 gzins Exp $"
*
* History
* -------
* $Log: not supported by cvs2svn $
* Revision 1.15  2005/03/04 16:28:28  scetre
* Changed Call to Save method
*
* Revision 1.14  2005/03/04 15:50:45  scetre
* Added typedef of vobsSTAR_PROPERTY_ID
* Updated call to Store method of CDATA
*
* Revision 1.13  2005/02/13 15:56:55  gzins
* Added optional origin parameter to Load() method
*
* Revision 1.12  2005/02/13 15:27:53  gzins
* Added Load() method
* Updated Save() method to use new vobsCDATA class
* Set default value of criteriaList to NULL
*
* Revision 1.11  2005/01/26 08:10:32  scetre
* change history
*
* scetre    06-Jul-2004  Created
*
*
*******************************************************************************/

#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif

/*
 * header files
 */
#include<list>
#include"vobsSTAR.h"

/** typedef of UCD order list*/
typedef std::list<string> vobsSTAR_PROPERTY_ID_LIST;

/**
 * vobsSTAR_LIST is a class which caracterises a list of star.
 *
 * methods of this class allows to :
 * \li known if the list is empty
 * \li clear the list
 * \li add at tail a star in the the list
 * \li remove a star from the list
 * \li get the list size
 * \li create a star from another one
 * \li get a star
 * \li merge two list
 * \li display the list o the console
 * \li save the list in a file
 */
class vobsSTAR_LIST
{ 
 public:
    vobsSTAR_LIST();//constructor
    virtual ~vobsSTAR_LIST();//destructor
   
    virtual mcsLOGICAL    IsEmpty(void);
    virtual mcsCOMPL_STAT Clear(void);
    virtual mcsCOMPL_STAT AddAtTail(vobsSTAR &star);
    virtual mcsCOMPL_STAT Remove(vobsSTAR &star);
    virtual mcsUINT32     Size(void);
    virtual mcsCOMPL_STAT Copy(vobsSTAR_LIST& list);

    virtual vobsSTAR      *GetNextStar(mcsLOGICAL init = mcsFALSE);
    virtual vobsSTAR      *GetStar(vobsSTAR &star,
                                   vobsSTAR_COMP_CRITERIA_LIST *criteriaList=NULL);
    
    virtual mcsCOMPL_STAT Merge(vobsSTAR_LIST &list,
                                vobsSTAR_COMP_CRITERIA_LIST *criteriaList=NULL, 
                                mcsLOGICAL updateOnly=mcsFALSE);

    virtual void          Display(void);

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
