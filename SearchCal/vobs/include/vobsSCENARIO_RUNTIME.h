#ifndef vobsSCENARIO_RUNTIME_H
#define vobsSCENARIO_RUNTIME_H
/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * \file
 * Declaration of vobsSCENARIO_RUNTIME class.
 */

#ifndef __cplusplus
#error This is a C++ include file and cannot be used from plain C
#endif

/* 
 * System Headers 
 */
#include <vector>
#include <stdio.h>

/*
 * MCS header
 */
#include "mcs.h"
#include "misco.h"
#include "vobsCDATA.h"

/*
 * Type declaration
 */
/*
 * char* comparator used by map<char*, ...>
 */
struct StringComparator
{

    bool operator()(char* s1, char* s2) const
    {
        return (s1 == s2) ? false : strcmp(s1, s2) < 0;
    }
};

/** TargetId mapping (targetId_epoch1, targetId_epoch2) type using char* key / value pairs */
typedef std::map<char*, char*, StringComparator> vobsTARGET_ID_MAPPING;

/** TargetId pair (targetId_epoch1, targetId_epoch2) */
typedef std::pair<char*, char*> vobsTARGET_ID_PAIR;

/* target Id length (char*) */
#define TARGET_ID_LENGTH 21

/*
 * Class declaration
 */

/**
 * vobsSCENARIO_RUNTIME is a class which contains buffer and variables used during scenario execution.
 * 
 */
class vobsSCENARIO_RUNTIME
{
public:
    // Class constructor

    /**
     * Build a scenario runtime.
     */
    vobsSCENARIO_RUNTIME()
    {
        // define targetId index to NULL: 
        _targetIdIndex = NULL;
    }

    // Class destructor

    ~vobsSCENARIO_RUNTIME()
    {
        // free targetId index:
        if (IS_NOT_NULL(_targetIdIndex))
        {
            // may recycle targetId to targetId pool
            ClearTargetIdIndex();

            delete(_targetIdIndex);
        }

        // free targetId pool
        for (std::vector<char*>::iterator iter = _targetIdPool.begin(); iter != _targetIdPool.end(); iter++)
        {
            delete[](*iter);
        }
        _targetIdPool.clear();
    }

    /**
     * Get the query buffer
     *
     * @return query buffer.
     */
    inline miscoDYN_BUF* GetQueryBuffer() __attribute__((always_inline))
    {
        // Reset the dynamic buffer
        _queryBuffer.Reset();

        /* Allocate some memory to store the complete query (32K ~ 1300 star ids) */
        _queryBuffer.Reserve(32 * 1024);

        return &_queryBuffer;
    }

    /**
     * Get the response buffer
     *
     * @return response buffer.
     */
    inline miscoDYN_BUF* GetResponseBuffer() __attribute__((always_inline))
    {
        // Reset the dynamic buffer
        _responseBuffer.Reset();

        /* Allocate some memory to store the complete response (256K) */
        _responseBuffer.Reserve(256 * 1024);

        return &_responseBuffer;
    }

    /**
     * Get the CDATA parser
     *
     * @return CDATA parser
     */
    inline vobsCDATA* GetCDataParser() __attribute__((always_inline))
    {
        // Reset the CDATA parser:
        _cData.Reset();

        /* Allocate some memory to store the CDATA blocks (128K) */
        _cData.Reserve(128 * 1024);

        return &_cData;
    }

    /**
     * Get the data buffer
     *
     * @return data buffer.
     */
    inline miscoDYN_BUF* GetDataBuffer() __attribute__((always_inline))
    {
        // Reset the dynamic buffer
        _dataBuffer.Reset();

        /* Allocate some memory to store the CDATA blocks (128K) */
        _dataBuffer.Reserve(128 * 1024);

        return &_dataBuffer;
    }

    inline vobsTARGET_ID_MAPPING* GetTargetIdIndex() __attribute__((always_inline))
    {
        // Prepare the targetId index:
        if (IS_NULL(_targetIdIndex))
        {
            // create the targetId index allocated until destructor is called:
            _targetIdIndex = new vobsTARGET_ID_MAPPING();
        }

        return _targetIdIndex;
    }

    /**
     * Clear the targetId index ie free allocated char arrays for key / value pairs
     */
    inline void ClearTargetIdIndex() __attribute__((always_inline))
    {
        // free targetId index:
        if (IS_NOT_NULL(_targetIdIndex))
        {
            for (vobsTARGET_ID_MAPPING::iterator iter = _targetIdIndex->begin(); iter != _targetIdIndex->end(); iter++)
            {
                // recycle allocated char[]
                _targetIdPool.push_back(iter->first);
                _targetIdPool.push_back(iter->second);
            }
            _targetIdIndex->clear();
        }
    }

    inline char* GetTargetId() __attribute__((always_inline))
    {
        char* targetId = NULL;

        if (!_targetIdPool.empty())
        {
            targetId = _targetIdPool.back();
            _targetIdPool.pop_back();
        }

        if (IS_NULL(targetId))
        {
            targetId = new char[TARGET_ID_LENGTH];
        }

        return targetId;
    }


private:
    // Declaration of copy constructor and assignment operator as private
    // methods, in order to hide them from the users.
    vobsSCENARIO_RUNTIME(const vobsSCENARIO_RUNTIME& runtime);
    vobsSCENARIO_RUNTIME& operator=(const vobsSCENARIO_RUNTIME& runtime);

    // runtime members:

    // Query buffer to send to the CDS
    miscoDYN_BUF _queryBuffer;

    // CDS Response buffer:
    miscoDYN_BUF _responseBuffer;

    // CDATA parser:
    vobsCDATA _cData;

    // CDATA buffer:
    miscoDYN_BUF _dataBuffer;

    /** targetId index: used only when the precession to catalog's epoch is needed */
    vobsTARGET_ID_MAPPING* _targetIdIndex;

    /** targetId object pool */
    std::vector<char*> _targetIdPool;

};

#endif /*!vobsSCENARIO_RUNTIME_H*/


/*___oOo___*/
