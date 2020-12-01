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
    vobsSTAR_MATCH_FIRST_IN_LIST
} vobsSTAR_MATCH;

typedef enum
{
    vobsSTAR_MATCH_TYPE_NONE,
    vobsSTAR_MATCH_TYPE_BAD_DIST,
    vobsSTAR_MATCH_TYPE_BAD_BEST,
    vobsSTAR_MATCH_TYPE_GOOD,
    vobsSTAR_MATCH_TYPE_GOOD_AMBIGUOUS_REF_SCORE,
    vobsSTAR_MATCH_TYPE_GOOD_AMBIGUOUS_MATCH_SCORE,
    vobsSTAR_MATCH_TYPE_GOOD_AMBIGUOUS_REF_SCORE_BETTER,
    vobsSTAR_MATCH_TYPE_GOOD_AMBIGUOUS_MATCH_SCORE_BETTER
} vobsSTAR_MATCH_TYPE;

static const char* const vobsSTAR_MATCH_TYPE_CHAR[] = {"None", "Bad_dist", "Bad_best", "Good",
                                                       "Ambiguous_ref_score", "Ambiguous_match_score",
                                                       "Ambiguous_ref_score_better", "Ambiguous_match_score_better"};

static const char* vobsGetMatchType(vobsSTAR_MATCH_TYPE type)
{
    return vobsSTAR_MATCH_TYPE_CHAR[type];
}

static mcsINT32 vobsGetMatchTypeAsFlag(vobsSTAR_MATCH_TYPE type)
{
    switch (type)
    {
        default:
            return 0; // ignore
        case vobsSTAR_MATCH_TYPE_GOOD:
            return 0; // ignore
        case vobsSTAR_MATCH_TYPE_BAD_DIST:
            return 1;
        case vobsSTAR_MATCH_TYPE_BAD_BEST:
            return 2;
        case vobsSTAR_MATCH_TYPE_GOOD_AMBIGUOUS_REF_SCORE:
            return 4; // ambiguous ref
        case vobsSTAR_MATCH_TYPE_GOOD_AMBIGUOUS_MATCH_SCORE:
            return 8; // ambiguous match
        case vobsSTAR_MATCH_TYPE_GOOD_AMBIGUOUS_REF_SCORE_BETTER:
            return 16; // ambiguous ref (better)
        case vobsSTAR_MATCH_TYPE_GOOD_AMBIGUOUS_MATCH_SCORE_BETTER:
            return 32; // ambiguous match (better)
    }
}

typedef enum
{
    vobsSTAR_PRECESS_NONE,
    vobsSTAR_PRECESS_LIST,
    vobsSTAR_PRECESS_BOTH,
} vobsSTAR_PRECESS_MODE;

/** Star pointer ordered list */
typedef std::list<vobsSTAR*> vobsSTAR_PTR_LIST;

/** Star pointer set */
typedef std::set<vobsSTAR*> vobsSTAR_PTR_SET;

/** Star pointer / double value pair */
typedef std::pair<mcsDOUBLE, vobsSTAR*> vobsSTAR_PTR_DBL_PAIR;
/** Star pointer / double value mapping (declination or distance) */
typedef std::multimap<mcsDOUBLE, vobsSTAR*> vobsSTAR_PTR_DBL_MAP;

/**
 * Information stored for an xmatch entry
 */
class vobsSTAR_PTR_MATCH_ENTRY
{
public:
    mcsDOUBLE score;   // weighted score
    mcsDOUBLE distAng; // as
    mcsDOUBLE distMag; // mag
    vobsSTAR* starPtr;
    // Ref coords used (epoch corrected)
    mcsDOUBLE ra1;
    mcsDOUBLE de1;
    // Star coords used (epoch corrected)
    mcsDOUBLE ra2;
    mcsDOUBLE de2;

    vobsSTAR_PTR_MATCH_ENTRY(mcsDOUBLE _distAng, vobsSTAR* _starPtr)
    {
        distAng = _distAng * alxDEG_IN_ARCSEC;
        distMag = NAN;
        score = computeScore(distAng, distMag);
        starPtr = _starPtr;
        // unused coords:
        ra1 = NAN;
        de1 = NAN;
        ra2 = NAN;
        de2 = NAN;
    }

    vobsSTAR_PTR_MATCH_ENTRY(mcsDOUBLE _distAng, mcsDOUBLE _distMag, vobsSTAR* _starPtr,
                             mcsDOUBLE _ra1, mcsDOUBLE _de1, mcsDOUBLE _ra2, mcsDOUBLE _de2)
    {
        distAng = _distAng * alxDEG_IN_ARCSEC;
        distMag = _distMag;
        score = computeScore(distAng, distMag);
        starPtr = _starPtr;
        // store coords used:
        ra1 = _ra1; // ref star
        de1 = _de1;
        ra2 = _ra2; // list star
        de2 = _de2;
    }

    vobsSTAR_PTR_MATCH_ENTRY(vobsSTAR_PTR_MATCH_ENTRY& entry, vobsSTAR* _starPtr)
    {
        distAng = entry.distAng;
        distMag = entry.distMag;
        score = entry.score;
        starPtr = _starPtr;
        // switch role in coords:
        ra1 = entry.ra2; // list star
        de1 = entry.de2;
        ra2 = entry.ra1; // ref star
        de2 = entry.de1;
    }

private:

    static mcsDOUBLE computeScore(mcsDOUBLE distAng, mcsDOUBLE distMag)
    {
        // distance:  normalize to typical radius ~ 1 as ?
        // distAng /= 1.0;
        if (isnan(distMag))
        {
            return distAng;
        }
        // magnitude: normalize to typical max mag error ~ 1 mag:
        // distMag /= 1.0;
        // conclusion: 1 as <=> 1 mag
        //  weighted score = sum(distAng^2 + distMag^2) where (distAng, distMag) is normalized by max errors
        return sqrt(distAng * distAng + distMag * distMag);
    }
} ;

/** Star pointer tuple / double value (score) pair */
typedef std::pair<mcsDOUBLE, vobsSTAR_PTR_MATCH_ENTRY> vobsSTAR_PTR_MATCH_PAIR;
/** Star pointer tuple / double value (score) mapping (distance map) */
typedef std::multimap<mcsDOUBLE, vobsSTAR_PTR_MATCH_ENTRY> vobsSTAR_PTR_MATCH_MAP;

/** Star pointer / distance map pointer pair */
typedef std::pair<vobsSTAR*, vobsSTAR_PTR_MATCH_MAP*> vobsSTAR_PTR_MATCH_MAP_PAIR;
/** Star pointer / distance map pointer map */
typedef std::map<vobsSTAR*, vobsSTAR_PTR_MATCH_MAP*> vobsSTAR_PTR_MATCH_MAP_MAP;

/*
 * Class declaration
 */

/**
 * Information retrieved from xmatch: see vobsSTAR_LIST::GetStarMatchingCriteria()
 */
class vobsSTAR_LIST_MATCH_INFO
{
public:
    mcsINT32 shared; /* 1 means shared; 0 not shared (free pointer) */
    vobsSTAR_MATCH_TYPE type;
    mcsINT32 nMates;
    mcsDOUBLE distAng12;
    mcsSTRING16384 xm_log;
    // data from vobsSTAR_PTR_MATCH_ENTRY:
    mcsDOUBLE score;   // weighted score
    mcsDOUBLE distAng; // as
    mcsDOUBLE distMag; // mag
    vobsSTAR* starPtr;

    vobsSTAR_LIST_MATCH_INFO()
    {
        shared = 0;
        Clear();
    }

    void Set(vobsSTAR_MATCH_TYPE matchType, vobsSTAR_PTR_MATCH_ENTRY& entry)
    {
        type = matchType;
        score = entry.score;
        distAng = entry.distAng;
        distMag = entry.distMag;
        starPtr = (isGood()) ? entry.starPtr : NULL;
    }

    void Clear(void)
    {
        type = vobsSTAR_MATCH_TYPE_NONE;
        nMates = -1;
        distAng12 = NAN;
        xm_log[0] = '\0';
        // data from vobsSTAR_PTR_MATCH_ENTRY:
        score = NAN;
        distAng = NAN;
        distMag = NAN;
        starPtr = NULL;
    }

    bool isGood()
    {
        return type >= vobsSTAR_MATCH_TYPE_GOOD;
    }
} ;

/** Star pointer (1) / Star pointer (2) pair */
typedef std::pair<vobsSTAR*, vobsSTAR_LIST_MATCH_INFO*> vobsSTAR_XM_PAIR;
/** Star pointer (1) / Star pointer (2) mapping */
typedef std::map<vobsSTAR*, vobsSTAR_LIST_MATCH_INFO*> vobsSTAR_XM_PAIR_MAP;

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
    void ClearRefs(const bool freeStarPtrs);

    mcsCOMPL_STAT Remove(vobsSTAR &star);
    void RemoveRef(vobsSTAR* starPtr);

    vobsSTAR* GetStar(vobsSTAR* star);
    vobsSTAR* GetStarMatchingCriteria(vobsSTAR* star,
                                      vobsSTAR_CRITERIA_INFO* criterias, mcsUINT32 nCriteria,
                                      vobsSTAR_MATCH matcher = vobsSTAR_MATCH_INDEX,
                                      vobsSTAR_LIST_MATCH_INFO* mInfo = NULL,
                                      mcsUINT32* noMatchs = NULL);

    mcsCOMPL_STAT GetStarMatchingCriteriaUsingDistMap(vobsSTAR_LIST_MATCH_INFO* mInfo,
                                                      vobsORIGIN_INDEX originIdx,
                                                      vobsSTAR_PRECESS_MODE precessMode, const mcsDOUBLE listEpoch,
                                                      vobsSTAR* star,
                                                      vobsSTAR_CRITERIA_INFO* criterias, mcsUINT32 nCriteria,
                                                      vobsSTAR_MATCH matcher = vobsSTAR_MATCH_INDEX,
                                                      mcsDOUBLE thresholdScore = 1.0,
                                                      mcsUINT32* noMatchs = NULL);

    mcsCOMPL_STAT GetStarsMatchingCriteriaUsingDistMap(vobsSTAR_XM_PAIR_MAP* mapping,
                                                       vobsORIGIN_INDEX originIdx, const vobsSTAR_MATCH_MODE matchMode,
                                                       vobsSTAR_PRECESS_MODE precessMode, const mcsDOUBLE listEpoch,
                                                       vobsSTAR_LIST* starRefList,
                                                       vobsSTAR_CRITERIA_INFO* criterias, mcsUINT32 nCriteria,
                                                       vobsSTAR_MATCH matcher = vobsSTAR_MATCH_INDEX,
                                                       mcsDOUBLE thresholdScore = 1.0,
                                                       mcsUINT32* noMatchs = NULL);

    mcsCOMPL_STAT GetStarsMatchingTargetId(vobsSTAR* star,
                                           vobsSTAR_CRITERIA_INFO* criterias,
                                           const mcsDOUBLE radius,
                                           vobsSTAR_LIST &outputList);

    mcsCOMPL_STAT GetStarsMatchingCriteria(vobsSTAR* star,
                                           vobsSTAR_CRITERIA_INFO* criterias, mcsUINT32 nCriteria,
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

    static void logStarMap(const char* operationName, vobsSTAR_PTR_MATCH_MAP* distMap,
                           const bool doLog = true, char* strLog = NULL);

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
    vobsSTAR_PTR_DBL_MAP* _starIndex;

    // distance map used to discriminate multiple "same" stars (GetStar)
    vobsSTAR_PTR_MATCH_MAP* _sameStarDistMap;

    // catalog id:
    vobsORIGIN_INDEX _catalogId;

    // optional catalog meta data where stars come from (source):
    const vobsCATALOG_META* _catalogMeta;

    // Declaration assignment operator as private
    // methods, in order to hide them from the users.
    vobsSTAR_LIST& operator=(const vobsSTAR_LIST&) ;
    vobsSTAR_LIST(const vobsSTAR_LIST& list); //copy constructor

    void logStarIndex(const char* operationName, const char* keyName, vobsSTAR_PTR_DBL_MAP* index,
                      const bool isArcSec = false, const bool doLog = true, char* strLog = NULL);

    mcsCOMPL_STAT logNoMatch(const vobsSTAR* starRefPtr);

    static void DumpXmatchMapping(vobsSTAR_XM_PAIR_MAP* mapping);
    static void ClearXmatchMapping(vobsSTAR_XM_PAIR_MAP* mapping);
} ;

#endif /*!vobSTAR_LIST_H*/


/*___oOo___*/
