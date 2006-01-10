/*******************************************************************************
 * JMMC project
 * 
 * "@(#) $Id: miscHash.c,v 1.5 2006-01-10 14:40:39 mella Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.4  2005/05/20 12:55:42  gzins
 * Updated after code review
 *
 * Revision 1.3  2005/04/06 12:59:33  gluck
 * Code review: minor changes
 *
 * Revision 1.2  2005/01/28 18:39:10  gzins
 * Changed FAILURE/SUCCESS to mcsFAILURE/mscSUCCESS
 *
 * gzins     16-Dec-2004  Created
 *
 ******************************************************************************/

/**
 * \file
 * Hash table management.
 *
 * Functions allow user to create a hash table which associates a key
 * with any data, and perform classical operations such as adding, deleting or
 * getting an element. When an element is added in the hash-table with a data
 * defined as dynamically allocated, therefore this data is freed when the
 * element is deleted from the hash table.
 *
 * \ex
 * The following code gives and example of hash table usage: 
 * \code
 * /# 
 *  * System Headers 
 *  #/
 * #include <stdlib.h>
 * #include <stdio.h>
 *
 * /#
 *  * MCS Headers 
 *  #/
 * #include "mcs.h"
 * #include "err.h"
 * #include "misc.h"
   
 * /# Test data #/
 * char *data[] = 
 * {   
 *     "alpha", "bravo", "charlie", "delta", "echo", "foxtrot", "golf",
 *     "hotel", "india", "juliet", "kilo", "lima", "mike", "november",
 *     "oscar", "papa", "quebec", "romeo", "sierra", "tango", "uniform",
 *     "victor", "whisky", "x-ray", "yankee", "zulu"
 * };
 * 
 * int main (int argc, char *argv[])
 * {
 *     miscHASH_TABLE hashTable;
 *     
 *     /# Initializes MCS services #/
 *     if (mcsInit(argv[0]) == mcsFAILURE)
 *     {
 *         exit (EXIT_mcsFAILURE);
 *     }
 * 
 *     /# Create the hash table #/
 *     miscHashCreate(&hashTable, 100);
 * 
 *     /# Add elements #/
 *     printf("\nmiscHashAddElement() Function Test :\n");
 *     int i;
 *     for (i = 0; i < 24; i++)
 *     {
 *         if (miscHashAddElement(&hashTable, data[i],
 *                                (void **)&data[i], mcsFALSE)== mcsFAILURE)
 *         {
 *             errCloseStack();
 *             exit (EXIT_mcsFAILURE);
 *         }
 *     }
 *     /# Display the content of hash table #/
 *     miscHashDisplay (&hashTable);
 * 
 *     /# Get an element #/
 *     char *dataPtr;
 *     dataPtr = (char *)miscHashGetElement(&hashTable, data[3]);
 *     if (dataPtr == NULL)
 *     {
 *         errCloseStack();
 *         exit (EXIT_mcsFAILURE);
 *     }
 *     printf("  key = %s - data = %s\n", data[3], dataPtr);
 * 
 *     /# Delete an element #/
 *     if (miscHashDeleteElement(&hashTable, data[3])== mcsFAILURE)
 *     {
 *         errCloseStack();
 *         exit (EXIT_mcsFAILURE);
 *     }
 * 
 *     /# Get sequencialy all element of the hash table #/
 *     dataPtr = miscHashGetNextElement(&hashTable, mcsTRUE);
 *     while (dataPtr != NULL)
 *     {
 *         printf("  %-10s\n", dataPtr); 
 *         dataPtr = miscHashGetNextElement(&hashTable, mcsFALSE);
 *     }
 * 
 *     /# Delete the hash table #/
 *     miscHashDelete(&hashTable);
 * 
 *     /# Close MCS services #/
 *     mcsExit();
 *     
 *     /# Exit from the application with mcsSUCCESS #/
 *     exit (EXIT_mcsSUCCESS);
 * }
 *  
 * \endcode
 */

static char *rcsId __attribute__ ((unused)) = "@(#) $Id: miscHash.c,v 1.5 2006-01-10 14:40:39 mella Exp $"; 


/* 
 * System Headers
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/*
 * MCS Headers 
 */
#include "mcs.h"
#include "log.h"
#include "err.h"

/* 
 * Local Headers
 */
#include "miscHash.h"
#include "miscString.h"
#include "miscErrors.h"
#include "miscPrivate.h"

/*
 * Local Variables
 */


/*
 * Local Functions declaration
 */
static mcsUINT32 miscGetHashValue(const char *key, mcsINT32 nbMaxHashValues);
static miscHASH_ELEMENT *miscHashLookUp(const miscHASH_TABLE *hashTable,
                                        const char           *key);

/* 
 * Local functions definition
 */
/**
 * Get the hash value of a given key.
 *
 * This function calculates the hash value of the given key.
 *
 * \param key              key to be digested.
 * \param nbMaxHashValues  possible maximum value of the computed hash value,
 * which should be the maximum size of the hash table.
 *
 * \return hash value.
 */
static mcsUINT32 miscGetHashValue(const char *key, mcsINT32 nbMaxHashValues)
{
    mcsUINT32   hashValue;
    for (hashValue = 0; *key != '\0'; key++)
    {
        hashValue += (hashValue<<3) + *key;
    }
    return (hashValue % nbMaxHashValues);
}

/**
 * Look for an element in the hash table.
 *
 * This function returns the pointer to element which matches the given key.
 * It is used by miscHashGetElement function.
 *
 * \param hashTable hash table.
 * \param key       searched key.
 *
 * \return pointer the element associated to the given key, or NULL if key not
 * found in table.
 */
static miscHASH_ELEMENT *miscHashLookUp(const miscHASH_TABLE *hashTable,
                                        const char           *key)
{
    mcsUINT32        hashValue;
    miscHASH_ELEMENT *element; 

    /* Get the hash value of the given key */
    hashValue = miscGetHashValue(key, hashTable->tableSize);  

    /* Get the first element of the hash table entry */
    element = hashTable->table[hashValue]; 

    /* Search the key the list */
    while (element != NULL)
    {
        /* If keys match */
        if (strcmp(element->key, key) == 0)
        {
            /* Return pointer to the associated data */
            return element;
        }
        else
        {
            /* Go to the next element */
            element = element->next;
        }
    }

    /* If end of list reachs, return NULL */
    return NULL;
}


/*
 * Public functions definition
 */

/**
 * Create the hash table.
 *
 * This function creates the hash table. The argument tableSize is the maximum
 * number of entries in the table. This value defines the memory occupied by
 * the hash table, but also the performance of the resulting hash table. To
 * obtain good performance, the table size should not be less than 1/10 of the
 * maximum number of foreseen elements in the hash table.
 *
 * \param hashTable hash table to be initialized.
 * \param tableSize number of entries in the table.
 *
 * \warning This function MUST be called before any operation on the hash table.
 *
 * \return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is
 * returned.
 *
 * \b Errors code:\n
 * The possible errors are :
 * \li miscERR_ALLOC
 */
mcsCOMPL_STAT miscHashCreate(miscHASH_TABLE *hashTable, mcsINT32 tableSize)
{
    mcsINT32 i;
    
    logExtDbg("miscHashCreate()"); 

    /* Create table */
    hashTable->table = calloc(tableSize, sizeof(miscHASH_ELEMENT *));
    if (hashTable->table == NULL)
    {
        errAdd(miscERR_ALLOC);
        return mcsFAILURE;
    }
    hashTable->tableSize = tableSize;

    /* Set all table entries to NULL */
    for (i = 0; i < tableSize; i++)
    {
        hashTable->table[i] = NULL;
    }

    /* Current element and hash index to the first element of the table */
    hashTable->currElement = NULL;
    hashTable->currHashIndex = 0;

    return mcsSUCCESS;
}

/**
 * Add or replace element to the hash table.
 *
 * This function adds the given key to the hash table. If key is already in
 * the table, the associated data is replaced by the new one. The
 * allocatedMemory flag indicates if the memory pointed by data has been
 * dynamically allocated and must be freed when element is removed from the
 ²* table.
 *
 * \param hashTable hash table.
 * \param key NULL-terminated string which is the search key
 * \param data pointer to the data associated with that key
 * \param allocatedMemory true (mcsTRUE) if data pointer points to allocated
 * area of memory which has to be freed when element is deleted.
 *
 * \return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
 *
 * \b Errors code:\n
 * The possible errors are :
 * \li miscERR_NULL_PARAM
 * \li miscERR_ALLOC
 */
mcsCOMPL_STAT miscHashAddElement(miscHASH_TABLE  *hashTable,
                                 const char      *key,
                                 void            **data,
                                 mcsLOGICAL      allocatedMemory)
{
    miscHASH_ELEMENT *element; 

    logExtDbg("miscHashAddElement(%s)", key); 
    
    /* Check param */
    if (data == NULL)
    {
        errAdd(miscERR_NULL_PARAM, "data");
        return mcsFAILURE;
    }

    /* Look for the element entry hash table entry */
    element  = miscHashLookUp(hashTable, key); 

    /* If key is not yet is hash table */
    if (element == NULL)
    {
        mcsINT32 hashValue;

        /* Create entry to store new key */
        element = malloc(sizeof(miscHASH_ELEMENT));
        if (element == NULL)
        {
            errAdd(miscERR_ALLOC);
            return mcsFAILURE;
        }

        /* Compute hash value */
        hashValue = miscGetHashValue(key, hashTable->tableSize);

        /* Check if first elemt to be stored for this entrance */
        /* If list for this entry is empty */
        if (hashTable->table[hashValue] == NULL)
        {
            /* Put element at the beginning of the list */
            hashTable->table[hashValue] = element;
            element->next = NULL;
            element->previous = NULL;
        }
        else
        {
            /* Go to the end of the list */
            miscHASH_ELEMENT *tmpElement; 
            tmpElement = hashTable->table[hashValue];
            while (tmpElement->next != NULL)
            {
                tmpElement = tmpElement->next;
            }

            /* Put element at the end of the list */
            tmpElement->next = element;
            element->next = NULL;
            element->previous = tmpElement;
        }
        
        /* Store the key in the newly added record */
        element->key = miscDuplicateString(key);
        if (element->key == NULL)
        {
            errAdd(miscERR_ALLOC);
            return mcsFAILURE;
        }
    }
    else
    {
        /* Free the associated data */
        if (element->allocatedMemory == mcsTRUE)
        {
            free(element->data);
        }
    }

    /* Insert the new "data" in the list + the key */
    element->data = *data;  
    element->allocatedMemory = allocatedMemory;

    return mcsSUCCESS;
}

/**
 * Delete element from the hash table.
 *
 * This function deletes the given key from the hash table and frees the
 * associated data (if it is an dynamic allocated memory).
 *
 * \param hashTable hash table.
 * \param key NULL-terminated string which is the key of the element to be
 * deleted.
 *
 * \return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is
 * returned.
 *
 * \b Errors code:\n
 * The possible errors are :
 * \li miscERR_HASH_KEY_NOT_FOUND
 */
mcsCOMPL_STAT miscHashDeleteElement(miscHASH_TABLE  *hashTable,
                                    const char      *key)
{
    logExtDbg("miscHashDeleteElement(%s)", key); 

    /* Look for the element entry in hash table */
    miscHASH_ELEMENT *element; 
    element  = miscHashLookUp(hashTable, key); 

    /* If key is not found */
    if (element == NULL)
    {
        /* Return pointer to the associated data */
        errAdd(miscERR_HASH_KEY_NOT_FOUND, key);
        return mcsFAILURE;
    }

    /* Remove element from the double linked-list */
    if (element->next != NULL)
    {
        (element->next)->previous = element->previous;
    }
    if (element->previous != NULL)
    {
        (element->previous)->next = element->next;
    }
    else
    {
        /* It is the first element of the list */
        mcsUINT32 hashValue;
        hashValue = miscGetHashValue(key, hashTable->tableSize);  
        hashTable->table[hashValue] = element->next;
    }

    /* Delete element */
    /* Delete the key */
    free (element->key);

    /* Delete associated data */
    if (element->allocatedMemory == mcsTRUE)
    {
        free(element->data);
    }
    /* Delete element */
    free(element);

    return mcsSUCCESS;
}

/**
 * Get an element from the hash table.
 *
 * This function returns the element which match the given key.
 *
 * \param hashTable hash table.
 * \param key       searched key.
 *
 * \return pointer the data associated to the given key, or NULL if key not
 * found in table.
 */
void *miscHashGetElement(const miscHASH_TABLE *hashTable, 
                         const char           *key)
{
    miscHASH_ELEMENT *element; 

    logExtDbg("miscHashGetElement(%s)", key); 

    /* Look for the element entry in hash table */
    element = miscHashLookUp(hashTable, key); 

    /* If key found */
    if (element != NULL)
    {
        /* Return pointer to the associated data */
        return element->data;
    }
    else
    {
        /* return NULL */
        return NULL;
    }
}

/**
 * Get the next element of the hash table.
 *
 * This function can be used to get sequentially all elements stored in the
 * hash table, as shown in the example below.
 *
 * \param hashTable hash table.
 * \param init       searched key.
 *
 * \warning The current implementation of miscHashGetNextElement does not
 * allow to add or delete element when scanning the table. Adding or deleting
 * element may produce unpredictable results.
 *
 * \ex
 * \code 
 *     
 *     dataPtr = miscHashGetNextElement(&hashTable, mcsTRUE);
 *     while (dataPtr != NULL)
 *     {
 *         ... use data, and get the next one ... 
 *
 *         dataPtr = miscHashGetNextElement(&hashTable, mcsFALSE);
 *     }
 * \endcode
 *
 * \return pointer the data associated to the next element, or NULL if there
 * is no more element in table.
 */
void *miscHashGetNextElement(miscHASH_TABLE *hashTable,
                             const mcsLOGICAL      init)
{
    mcsINT32 i;

    logExtDbg("miscHashGetNextElement()"); 

    /* If first element is requested */
    if (init == mcsTRUE)
    {
        /* Reset pointer */
        hashTable->currElement = NULL;
        hashTable->currHashIndex = 0;
    }

    /* If end of table has been already reached */
    if (hashTable->currHashIndex == (hashTable->tableSize - 1) && 
        (hashTable->currElement == NULL))
    {
        return NULL;        
    }
    
    /* If search from the beginning of the table */
    if ((hashTable->currElement == NULL) && (hashTable->currHashIndex == 0))
    {
        /* Find the first element in the hash table */
        for (i = 0; i < hashTable->tableSize; i++)
        {
            if (hashTable->table[i] != NULL)
            {
                hashTable->currHashIndex = i;
                hashTable->currElement = 
                    hashTable->table[hashTable->currHashIndex];
                return (hashTable->currElement->data);
            }
        }

        /* If table is empty */
        hashTable->currHashIndex = hashTable->tableSize - 1;
        hashTable->currElement = NULL;
        return NULL;        
    }
    else
    {
        /* If there is an element after the current one */
        if (hashTable->currElement->next != NULL)
        {
            /* Go to the next element */
            hashTable->currElement = hashTable->currElement->next;
            return (hashTable->currElement->data);
        }
        else
        {
            /* Find the next 'not-empty' list in the table */
            for (i = (hashTable->currHashIndex + 1); 
                 i < hashTable->tableSize; i++)
            {
                if (hashTable->table[i] != NULL)
                {
                    hashTable->currHashIndex = i;
                    hashTable->currElement = 
                        hashTable->table[hashTable->currHashIndex];
                    return (hashTable->currElement->data);
                }
            }

            /* If end of table is reached */
            hashTable->currHashIndex = hashTable->tableSize - 1;
            hashTable->currElement = NULL;
            return NULL;        
        }
    }
}

/**
 * Delete the whole hash table (free all memory).
 *
 * \param hashTable hash table.
 */
void miscHashDelete(miscHASH_TABLE *hashTable)
{
    mcsINT32 i;
    
    logExtDbg("miscHashFree()"); 
    
    /* For all entries of the table */
    for (i = 0; i < hashTable->tableSize; i++)
    {
        /* For each element of the list */
        miscHASH_ELEMENT *element; 
        miscHASH_ELEMENT *prevElement; 
        element = hashTable->table[i];
        while (element != NULL)
        {
            prevElement = element;
            element = element->next;

            /* Delete the key */
            free (prevElement->key);

            /* Delete associated data */
            if (prevElement->allocatedMemory == mcsTRUE)
            {
                free(prevElement->data);
            }
            /* Delete element */
            free(prevElement);
        }
        /* For end */
    }
    /* For end */

    /* Delete table itself */
    free (hashTable->table);
    hashTable->table = NULL;
}

/**
 * Display all keys of the hash table.
 *
 * \param hashTable hash table.
 */
void miscHashDisplay(miscHASH_TABLE *hashTable)
{
    mcsINT32 i;
    
    logExtDbg("miscHashDisplay()"); 

    /* Set all table entries to NULL */
    printf ("Content of the hash table :\n");
    for (i = 0; i < hashTable->tableSize; i++)
    {
        miscHASH_ELEMENT *element; 
        element = hashTable->table[i];
        if (element != NULL)
        {
            printf ("\n   %4d -", i);
        }
        while (element != NULL)
        {
            printf (" %s", element->key);
            element = element->next;
        }
    }
    printf("\n");
}
/*___oOo___*/
