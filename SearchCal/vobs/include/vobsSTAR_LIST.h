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
#include <map>

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
typedef std::list<const char*> vobsSTAR_PROPERTY_ID_LIST;

/* star list type using star pointers */
typedef std::list<vobsSTAR*> StarList;

/* star index map type using double (declination in fact) keys */
typedef std::multimap<double, vobsSTAR*> StarIndex;

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
    
    // following methods are NOT virtual as only defined in vobsSTAR_LIST (not overriden):
    // note: not virtual for iteration performance too
    mcsCOMPL_STAT Clear(void);
    mcsCOMPL_STAT Remove(vobsSTAR &star);
    
    vobsSTAR*     GetStar(vobsSTAR* star);
    vobsSTAR*     GetStar(vobsSTAR* star, 
                          vobsSTAR_CRITERIA_INFO* criterias, mcsUINT32 nCriteria);
    
    // TODO: check Merge use cases without criteria !!
    mcsCOMPL_STAT Merge(vobsSTAR_LIST &list,
                        vobsSTAR_COMP_CRITERIA_LIST* criteriaList, 
                        mcsLOGICAL updateOnly,
                        mcsLOGICAL enableStarIndex = mcsFALSE);

    mcsCOMPL_STAT FilterDuplicates(vobsSTAR_LIST &list,
                                   vobsSTAR_COMP_CRITERIA_LIST* criteriaList = NULL,
                                   mcsLOGICAL enableStarIndex = mcsFALSE);
    
    mcsCOMPL_STAT Sort(const char *propertyId,
                       mcsLOGICAL reverseOrder = mcsFALSE);

    void          Display(void) const;

    mcsCOMPL_STAT GetVOTable(const char*    header,
                             const char*    softwareVersion,
                             const char*    request,
                             const char*    xmlRquest,
                             miscoDYN_BUF*  buffer);

    mcsCOMPL_STAT SaveToVOTable(const char *filename,
                                        const char *header,
                                        const char *softwareVersion,
                                        const char *request,
                                        const char *xmlRequest);

    
    /** Set the flag indicating to free star pointers or not (shadow copy) */
    inline void SetFreeStarPointers(const bool freeStarPtrs) __attribute__((always_inline))
    {
        _freeStarPtrs = freeStarPtrs;
    }
  
    /** Return the flag indicating to free star pointers or not (shadow copy) */
    inline bool IsFreeStarPointers() const __attribute__((always_inline))
    {
        return _freeStarPtrs;
    }

    /**
     * Return whether the list is empty or not.  
     *
     * @return mcsTRUE if the number of elements is zero, mcsFALSE otherwise.
     */
    inline mcsLOGICAL IsEmpty(void) const __attribute__((always_inline))
    {
        if (_starList.empty() == false)
        {
            return mcsFALSE;
        }

        return mcsTRUE;
    }

    /**
     * Returns the number of stars currently stored in the list.
     *
     * @return The numbers of stars in the list.
     */
    inline mcsUINT32 Size(void) const __attribute__((always_inline))
    {
        return _starList.size();
    }
    
    /**
     * Return the next star in the list.
     *
     * This method returns the pointer to the next star of the list. If @em
     * init is mcsTRUE, it returns the first star of the list.
     * 
     * This method can be used to move forward in the list, as shown below:
     * @code
     * for (unsigned int el = 0; el < starList.Size(); el++)
     * {
     *     starList.GetNextStar((mcsLOGICAL)(el==0))->View();
     * }
     * @endcode
     *
     * @return pointer to the next element of the list or NULL if the end of the
     * list is reached.
     */
    inline vobsSTAR* GetNextStar(mcsLOGICAL init = mcsFALSE) __attribute__((always_inline))
    {
        if ((init == mcsTRUE) || _starIterator == _starList.end())
        {
            _starIterator = _starList.begin();
        }
        else
        {
            _starIterator++;
        }

        if (_starIterator == _starList.end())
        {
            return NULL;
        }

        return (*_starIterator);
    }

    // virtual methods: overriden by sub classes:
    
    virtual mcsCOMPL_STAT AddAtTail(const vobsSTAR &star);
    virtual mcsCOMPL_STAT AddRefAtTail(vobsSTAR* star);
    
    // Copy means AddAll:
    virtual mcsCOMPL_STAT Copy(vobsSTAR_LIST& list);
    virtual mcsCOMPL_STAT CopyRefs(vobsSTAR_LIST& list, mcsLOGICAL doFreePointers = mcsTRUE);


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
    StarList           _starList;
    StarList::iterator _starIterator;

private:
    // flag to indicate to free star pointers or not (shadow copy) 
    bool               _freeStarPtrs;

    // flag to indicate that the star index is initialized
    // and can be by merge and filterDuplicates operations
    bool               _starIndexInitialized;
    
    // star index used only by merge and filterDuplicates operations
    // (based on declination for now)
    StarIndex*         _starIndex;
    
    // distance map used to discriminate multiple "same" stars (GetStar)
    StarIndex*         _sameStarDistMap;

    // Declaration assignment operator as private
    // methods, in order to hide them from the users.
    vobsSTAR_LIST& operator=(const vobsSTAR_LIST&);
    vobsSTAR_LIST (const vobsSTAR_LIST& list);//copy constructor
    
    void logStarIndex(StarIndex* index) const;
};

#endif /*!vobSTAR_LIST_H*/


/*___oOo___*/
