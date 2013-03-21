/**
 * @file AHashtable.h
 */

#ifndef AHASHTABLE_H_
#define AHASHTABLE_H_

#include <stdarg.h>
#include "AStructBase.h"
#include "APair.h"
#include "AHash.h"
#include "AComp.h"

/* HashtableNode - A list node containing a key, a value and a pointer to the next node */
typedef struct AHashtableNode AHashtableNode;
struct AHashtableNode
{
	void* key;
	void* value;
	AHashtableNode* next;
};

/**
 * Hashtable traversal function.
 * @param pair A key-value pair
 * @return NULL in case of success or anything else in case of failure.
 *
 * This function is callbacked by AHashtable::traverse.
 */
typedef void* (*AHashtableTraverseFunc)(APair* pair);

typedef struct AHashtable AHashtable;

/**
 * Hash table
 *
 * This data structure is a generic separately chained hash table used for storing key-value pairs.
 * Use it when you need to map some value to some key, but you don't care about the order of the keys or values.
 *
 * The arguments passed to @link ANew AStruct->ANew()@endlink to create a new hash table are:
 * @code AStruct->ANew(AHashtable, AHashFunc hash, AValueComp comp, AValueFree freeKey, AValueFree freeValue, int minCapacity)@endcode
 * @param hash Hash function to hash the key. You can (and should) use the functions provided by ::AHash.
 * @param comp Comparison function to compare keys. Since hash tables are unordered, this function
 *        is only required to specify whether elements are equal. You can (and should) use the functions provided by ::AComp.
 * @param [opt]freeKey Optional callback function to free the key. NULL by default.
 * @param [opt]freeKey Optional callback function to free the value. NULL by default.
 * @param [opt]minCapcity Optional minimum initial capacity argument. If passed, this argument will specify the minimum
 *        capacity the hash table will have with its creation.
 *
 * Examples of creating a new hash table:
 * @code
 * // Create a new hash table with default capacity using strings as keys. Use free to free the keys and values.
 * AHashtable* table = AStruct->ANew(AHashtable, AHash->stringHash, AComp->stringComp, free, free);
 *
 * // Create a new hash table with minimum capacity of 20 items using
 * // ints as keys. Keys won't be freed and values will be freed using free.
 * AHashtable* table = AStruct->ANew(AHashtable, AHash->intHash, AComp->intComp, NULL, free, 20);
 *
 * size_t doubleHash(const void* key)
 * {
 *     return AHash->hash(key, sizeof double);
 * }
 *
 * int doubleComp(const void* a, const void* b)
 * {
 *     return !(*(const double *)a == *(const double *)b);
 * }
 *
 * // Create a new hash table with default capacity using doubles as keys. Nothing will be freed.
 * AHashtable* table = AStruct->ANew(AHashtable, doubleHash, doubleComp);
 * @endcode
 */
struct AHashtable
{
	void*   (*const create)(AHashtable* self, int numArgs, va_list args);      /*<  Default creator function called by AStruct->ANew() */
	void    (*const clear)(AHashtable* self);                                  /**< Clear all the entries in the hash table */
	void    (*const destroy)(AHashtable* self);                                /**< Destroy the hash table and all of it's entries */
	APair*  (*const set)(AHashtable* self, void* key, void* value);            /**< Set a value to a key */
	void*   (*const get)(AHashtable* self, void* key);                         /**< Get a value from a key */
	void    (*const remove)(AHashtable* self, void* key);                      /**< Remove a key and its value */
	void*   (*const traverse)(AHashtable* self, AHashtableTraverseFunc func);  /**< Traverse all the entries in the hash table */

	AHashFunc hash;         /**< The hash function */
	AValueComp comp;        /**< The comparison function */
	AValueFree freeKey;     /**< Key destructor function */
	AValueFree freeValue;   /**< Value destructor function */

	AHashtableNode** table; /*<  Array of Hashtable nodes of 'capacity' + 1 size */
	size_t capacity;        /*<  The entire capacity of the table - 1 */
	size_t lists;           /*<  Number of chained lists of Hashtable nodes */
	size_t size;            /**< The number of entries in the hash table */
};

extern const AHashtable AHashtableProto;

#endif /* AHASHTABLE_H_ */
