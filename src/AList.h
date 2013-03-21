/**
 * @file AList.h
 */

#ifndef ALIST_H_
#define ALIST_H_

#include <stdarg.h>
#include "AStructBase.h"

typedef struct AListNode AListNode;

/**
 * @link AList Doubly linked list@endlink node.
 *
 * The node contains a value and a pointer
 * to the next and previous nodes. A pointer to NULL
 * from either direction marks the end of the list.
 */
struct AListNode
{
	void* value; /**< The value */
	AListNode* prev; /**< Previous node */
	AListNode* next; /**< Next node */
};

typedef struct AList AList;

/**
 * Doubly linked list
 *
 * This data structure is a generic doubly linked list used for storing a list of values. Use it when you
 * need fast insertations and removals from both ends of list, or just when you feel a linked list is
 * appropriate for storing a collection of values. Random access to values is linear. If you need
 * fast random access use AVector.
 *
 * The arguments passed to @link ANew AStruct->ANew()@endlink to create a new list are:
 * @code AStruct->ANew(AList) @endcode
 * No additional arguments should be passed.
 *
 * Example of creating a new list:
 * @code AList* list = AStruct->ANew(AList); @endcode
 */
struct AList
{
	void*       (*const create)(AList* self, int numArgs, va_list args);     /*<  Default creator function called by AStruct->ANew() */
	void        (*const clear)(AList* self, AValueFree freeValue);           /**< Clear all the nodes in the list */
	void        (*const destroy)(AList* self, AValueFree freeValue);         /**< Destroy the list and clear it */
	AListNode*  (*const append)(AList* self, void* value);                   /**< Append a value to the end of the list */
	void*       (*const last)(AList* self);                                  /**< Get the value of the last node in the list */
	void*       (*const popLast)(AList* self);                               /**< Pop the last node from the list */
	AListNode*  (*const prepend)(AList* self, void* value);                  /**< Prepend a value to the start of the list */
	void*       (*const first)(AList* self);                                 /**< Get the value of the first node in the list */
	void*       (*const popFirst)(AList* self);                              /**< Pop the first node from the list */
	AListNode*  (*const insert)(AList* self, AListNode* prev, void* value);  /**< Insert a value anywhere in the list */
	AListNode*  (*const insertAt)(AList* self, size_t pos, void* value);     /**< Insert a value at a position in the list */
	void*       (*const valueAt)(AList* self, size_t pos);                   /**< Get the value at a position in the list */
	void*       (*const replaceAt)(AList* self, size_t pos, void* value);    /**< Replace the value at a position in the list */
	void*       (*const remove)(AList* self, AListNode* node);               /**< Remove a node from the list */
	void*       (*const removeAt)(AList* self, size_t pos);                  /**< Remove a node at a position from the list */
	AList*      (*const copy)(AList* self, AValueFunc copyValue);            /**< Copy the entire list */
	AList*      (*const split)(AList* self, AListNode* node);                /**< Split the list by a node */
	AList*      (*const splitAt)(AList* self, size_t pos);                   /**< Split the list by a position */
	void        (*const join)(AList* first, AList* second);                  /**< Join two lists */

	size_t size;      /**< Number of nodes in the list */
	AListNode* head;  /**< First node in the list */
	AListNode* tail;  /**< Last node in the list */
};

extern const AList AListProto;

#endif /* ALIST_H_ */
