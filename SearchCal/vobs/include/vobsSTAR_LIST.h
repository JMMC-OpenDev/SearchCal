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
#include <list>
#include <map>
#include <set>

/*
 * MCS Headers
 */
#include "miscoDYN_BUF.h"

/*
 * Local Headers
 */
#include "vobsCATALOG_META.h"
#include "vobsSTAR.h"

/*
 * Type declaration
 */

/**
 * vobsSTAR_MATCH is an enumeration which define star matching algorithm.
 */
typedef enum
{
    vobsSTAR_MATCH_INDEX,
    vobsSTAR_MATCH_DISTANCE_MAP,
    vobsSTAR_MATCH_DISTANCE_MAP_PRECESSED,
    vobsSTAR_MATCH_FIRST_IN_LIST
} vobsSTAR_MATCH;

/** Star pointer ordered list */
typedef std::list<vobsSTAR*> vobsSTAR_PTR_LIST;

/** Star pointer / double value mapping (declination or distance) */
typedef std::multimap<mcsDOUBLE, vobsSTAR*> vobsSTAR_PTR_MAP;

/** Star pointer / double value pair */
typedef std::pair<mcsDOUBLE, vobsSTAR*> vobsSTAR_PTR_PAIR;

/** Star pointer set */
typedef std::set<vobsSTAR*> vobsSTAR_PTR_SET;

/*
 * Class declaration
 */

/**
 * Information retrieved from xmatch: see vobsSTAR_LIST::GetStarMatchingCriteria()
 */
struct vobsSTAR_LIST_MATCH_INFO
{
    mcsDOUBLE separation;
    mcsDOUBLE sep2nd;
    mcsINT32 nMates;

    mcsSTRING16384 xm_log;
} ;

/**
 * vobsSTAR_LIST handles a list of stars.
 */
class vobsSTAR_LIST
{
public:
    // Class constructors
    vobsSTAR_LIST(const char* name);

    // Class destructor
    virtual ~vobsSTAR_LIST();

    // following methods are NOT virtual as only defined in vobsSTAR_LIST (not overriden):
    // note: not virtual for iteration performance too
    void Clear(void);
    mcsCOMPL_STAT Remove(vobsSTAR &star);
    void RemoveRef(vobsSTAR* starPtr);

    vobsSTAR* GetStar(vobsSTAR* star);
    vobsSTAR* GetStarMatchingCriteria(vobsORIGIN_INDEX originIdx,
                                      vobsSTAR* star,
                                      vobsSTAR_CRITERIA_INFO* criterias, mcsUINT32 nCriteria,
                                      vobsSTAR_MATCH matcher = vobsSTAR_MATCH_INDEX,
                                      vobsSTAR_LIST_MATCH_INFO* mInfo = NULL,
                                      mcsUINT32* noMatchs = NULL);

    mcsCOMPL_STAT GetStarsMatchingTargetId(vobsSTAR* star,
                                           vobsSTAR_CRITERIA_INFO* criterias,
                                           vobsSTAR_LIST &outputList);

    mcsCOMPL_STAT GetStarsMatchingCriteria(vobsSTAR* star,
                                           vobsSTAR_CRITERIA_INFO* criterias, mcsUINT32 nCriteria,
                                           vobsSTAR_MATCH matcher,
                                           vobsSTAR_LIST &outputList,
                                           mcsUINT32 maxMatches);

    mcsCOMPL_STAT Merge(vobsSTAR_LIST &list,
                        vobsSTAR_COMP_CRITERIA_LIST* criteriaList,
                        mcsLOGICAL updateOnly);

    mcsCOMPL_STAT PrepareIndex();

    mcsCOMPL_STAT Search(vobsSTAR* referenceStar,
                         vobsSTAR_COMP_CRITERIA_LIST* criteriaList,
                         vobsSTAR_LIST &outputList,
                         mcsUINT32 maxMatches);

    mcsCOMPL_STAT FilterDuplicates(vobsSTAR_LIST &list,
                                   vobsSTAR_COMP_CRITERIA_LIST* criteriaList = NULL,
                                   bool doRemove = false);

    mcsCOMPL_STAT Sort(const char *propertyId,
                       mcsLOGICAL reverseOrder = mcsFALSE);

    void Display(void) const;

    mcsCOMPL_STAT GetVOTable(const char* command,
                             const char* header,
                             const char* softwareVersion,
                             const char* request,
                             const char* xmlRquest,
                             miscoDYN_BUF* votBuffer,
                             mcsLOGICAL trimColumns,
                             const char *log = NULL);

    mcsCOMPL_STAT SaveToVOTable(const char* command,
                                const char *filename,
                                const char *header,
                                const char *softwareVersion,
                                const char *request,
                                const char *xmlRequest,
                                mcsLOGICAL trimColumns,
                                const char *log = NULL);

    /**
     * Get the name of the star list as string literal
     *
     * @return name of the star list
     */
    inline const char* GetName() const __attribute__ ((always_inline))
    {
        return _name;
    }

    /**
     * Set the flag indicating to free star pointers or not (shadow copy)
     */
    inline void SetFreeStarPointers(const bool freeStarPtrs) const __attribute__ ((always_inline))
    {
        _freeStarPtrs = freeStarPtrs;
    }

    /**
     * Return the flag indicating to free star pointers or not (shadow copy)
     */
    inline bool IsFreeStarPointers() const __attribute__ ((always_inline))
    {
        return _freeStarPtrs;
    }

    /**
     * Return the catalog id as origin index
     */
    inline vobsORIGIN_INDEX GetCatalogId() const __attribute__ ((always_inline))
    {
        return _catalogId;
    }

    /**
     * Return the catalog name
     */
    inline const char* GetCatalogName() const __attribute__ ((always_inline))
    {
        return vobsGetOriginIndex(_catalogId);
    }

    /**
     * Return the optional catalog meta data or NULL
     */
    inline const vobsCATALOG_META* GetCatalogMeta() const __attribute__ ((always_inline))
    {
        return _catalogMeta;
    }

    /**
     * Set the optional catalog id / meta where stars are coming from
     */
    inline void SetCatalogMeta(vobsORIGIN_INDEX catalogId, const vobsCATALOG_META* catalogMeta) __attribute__ ((always_inline))
    {
        _catalogId = catalogId;
        _catalogMeta = catalogMeta;
    }

    /**
     * Return whether the list is empty or not.
     *
     * @return mcsTRUE if the number of elements is zero, mcsFALSE otherwise.
     */
    inline mcsLOGICAL IsEmpty(void) const __attribute__ ((always_inline))
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
    inline mcsUINT32 Size(void) const __attribute__ ((always_inline))
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
     * for (mcsUINT32 el = 0; el < starList.Size(); el++)
     * {
     *     starList.GetNextStar((mcsLOGICAL)(el==0))->View();
     * }
     * @endcode
     *
     * @return pointer to the next element of the list or NULL if the end of the
     * list is reached.
     */
    inline vobsSTAR* GetNextStar(mcsLOGICAL init = mcsFALSE) const __attribute__ ((always_inline))
    {
        if (IS_TRUE(init) || (_starIterator == _starList.end()))
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

        return *_starIterator;
    }

    /**
     * Add the given element AS one Reference (= pointer only) at the end of the list
     * i.e. it does not copy the given star
     *
     * @param star element to be added to the list.
     */
    inline void AddRefAtTail(vobsSTAR* star) __attribute__ ((always_inline))
    {
        if (IS_NOT_NULL(star))
        {
            // Put the reference in the list
            _starList.push_back(star);
        }
    }

    /**
     * Copy only references from the given list
     * i.e. Add all pointers present in the given list at the end of this list
     *
     * this list must free pointers (_freeStarPtrs = true)
     * the source list must NOT free pointers (list._freeStarPtrs = false)
     *
     * @param list the list to copy
     * @param doFreePointers flag to indicate that this list must free pointers and the source list not; if false, the contrary
     */
    inline void CopyRefs(const vobsSTAR_LIST& list, mcsLOGICAL doFreePointers = mcsTRUE) __attribute__ ((always_inline))
    {
        // Copy catalog id / meta:
        SetCatalogMeta(list.GetCatalogId(), list.GetCatalogMeta());

        const mcsUINT32 nbStars = list.Size();
        for (mcsUINT32 el = 0; el < nbStars; el++)
        {
            AddRefAtTail(list.GetNextStar((mcsLOGICAL) (el == 0)));
        }

        // if list.IsFreeStarPointers(), adjust freeStarPtrs flag for both lists:
        if (list.IsFreeStarPointers())
        {
            SetFreeStarPointers(IS_TRUE(doFreePointers));
            list.SetFreeStarPointers(IS_FALSE(doFreePointers));
        }
        else
        {
            // none will free star pointers (another list will do it):
            SetFreeStarPointers(false);
        }
    }


    // virtual methods: overriden by sub classes:

    virtual void AddAtTail(const vobsSTAR &star);

    virtual void Copy(const vobsSTAR_LIST& list);

    virtual mcsCOMPL_STAT Save(const char *filename,
                               mcsLOGICAL extendedFormat = mcsFALSE);

    virtual mcsCOMPL_STAT Load(const char *filename,
                               const vobsCATALOG_META* catalogMeta,
                               vobsCATALOG_STAR_PROPERTY_CATALOG_MAPPING* propertyCatalogMap = NULL,
                               mcsLOGICAL extendedFormat = mcsFALSE,
                               vobsORIGIN_INDEX originIndex = vobsORIG_NONE);

protected:
    // List of stars
    vobsSTAR_PTR_LIST _starList;

    // starIterator is mutable to be modified even by const methods
    mutable vobsSTAR_PTR_LIST::const_iterator _starIterator;

private:
    // name of the star list
    const char* _name;

    // flag to indicate to free star pointers or not (shadow copy)
    // freeStarPtrs is mutable to be modified even by const methods
    mutable bool _freeStarPtrs;

    // flag to indicate that the star index is initialized
    // and can be by merge and filterDuplicates operations
    bool _starIndexInitialized;

    // star index used only by merge and filterDuplicates operations
    // (based on declination for now)
    vobsSTAR_PTR_MAP* _starIndex;

    // distance map used to discriminate multiple "same" stars (GetStar)
    vobsSTAR_PTR_MAP* _sameStarDistMap;

    // catalog id:
    vobsORIGIN_INDEX _catalogId;

    // optional catalog meta data where stars come from (source):
    const vobsCATALOG_META* _catalogMeta;

    // Declaration assignment operator as private
    // methods, in order to hide them from the users.
    vobsSTAR_LIST& operator=(const vobsSTAR_LIST&) ;
    vobsSTAR_LIST(const vobsSTAR_LIST& list); //copy constructor

    void logStarIndex(const char* operationName, const char* keyName, vobsSTAR_PTR_MAP* index,
                      const bool isArcSec = false, const bool doLog = true, char* strLog = NULL);

} ;

#endif /*!vobSTAR_LIST_H*/


/*___oOo___*/
