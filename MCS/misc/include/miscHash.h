#ifndef miscHash_H
#define miscHash_H
/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: miscHash.h,v 1.3 2005-04-06 12:59:33 gluck Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.2  2005/02/15 09:37:52  gzins
 * Added CVS log as file modification history
 *
 * gzins     16-Dec-2004  Created
 *
 ******************************************************************************/

/**
 * \file
 * Declaration of structures and functions for Hash table management.
 */

/* The following piece of code alternates the linkage type to C for all 
functions declared within the braces, which is necessary to use the 
functions in C++-code.
*/
#ifdef __cplusplus
extern "C" {
#endif

#include "mcs.h"

/*
 * Structure type definition
 */

/**
 * Element of the hash table.
 *
 * The field key points to the NUL-terminated string which is  the  search
 * key. The field data points to the data associated with that key.
 * The field next is used for linked list management. 
 */
typedef struct miscHASH_ELEMENT 
{
    char                     *key;
    void                     *data;
    mcsLOGICAL               allocatedMemory;
    struct miscHASH_ELEMENT  *previous;
    struct miscHASH_ELEMENT  *next;
} miscHASH_ELEMENT;

/**
 * Hash table
 */
typedef struct miscHASH_TABLE
{
    miscHASH_ELEMENT *currElement;
    mcsINT32         currHashIndex; 
    mcsINT32         tableSize; 
    miscHASH_ELEMENT **table;
} miscHASH_TABLE;

/*
 * Pubic functions declaration
 */
mcsCOMPL_STAT miscHashCreate(miscHASH_TABLE *hashTable, mcsINT32 tableSize);

mcsCOMPL_STAT miscHashAddElement(miscHASH_TABLE *hashTable,
                                 const char     *key,
                                 void           **data,
                                 mcsLOGICAL     allocatedMemory);

mcsCOMPL_STAT miscHashDeleteElement(miscHASH_TABLE *hashTable,
                                    const char     *key);

void *miscHashGetElement(const miscHASH_TABLE *hashTable, 
                         const char           *key);
void *miscHashGetNextElement(miscHASH_TABLE   *hashTable, 
                             const mcsLOGICAL init);

void miscHashDelete(miscHASH_TABLE *hashTable);

void miscHashDisplay(miscHASH_TABLE *hashTable);

#ifdef __cplusplus
}
#endif


#endif /*!miscHash_H*/

/*___oOo___*/
