#ifndef vobsSTAR_LIST_H
#define vobsSTAR_LIST_H
/*******************************************************************************
* JMMC project
*
* "@(#) $Id: vobsSTAR_LIST.h,v 1.9 2004-11-30 10:32:31 scetre Exp $"
*
* who       when         what
* --------  -----------  -------------------------------------------------------
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
                                   float intervalRa=0,
                                   float intervalDec=0);
    
    virtual mcsCOMPL_STAT Merge(vobsSTAR_LIST &list,
                                float intervalRa=0,
                                float intervalDec=0,
                                mcsLOGICAL updateOnly=mcsFALSE);

    virtual void          Display(void);

    virtual mcsCOMPL_STAT Save(mcsSTRING256 filename); 

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
