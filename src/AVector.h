/**
 * @file AVector.h
 */

#ifndef AVECTOR_H_
#define AVECTOR_H_

#include <stdarg.h>
#include "AStructBase.h"

typedef struct AVector AVector;

/**
 * Dynamic array
 *
 * This data structure is a generic dynamic array which expands on its own. Use a vector whenever
 * you need an array of unknown size which expands on its own. Dynamic arrays have fast insertations
 * and removals from the end, and random access is constant.
 *
 * The arguments passed to @link ANew AStruct->ANew()@endlink to create a new vector are:
 * @code AStruct->ANew(AVector, int capacity)@endcode
 * @param [opt]capacity = Optional argument to specify the initial capacity of the vector
 *
 * Examples of creating a new vector:
 * @code
 * AStack* vector = AStruct->ANew(AVector);
 *
 * // create a vector with initial capacity of 100 items
 * AStack* vector = AStruct->ANew(AVector, 100);
 * @endcode
 */
struct AVector
{
	void*     (*const create)(AVector* self, int numArgs, va_list args);  /*<  Default creator function called by AStruct->ANew() */
	void      (*const clear)(AVector* self, AValueFree freeValue);        /**< Clear all the vector */
	void      (*const destroy)(AVector* self, AValueFree freeValue);      /**< Clear and destroy the vector */
	void**    (*const append)(AVector* self, void* value);                /**< Append a value to the end of the vector */
	void**    (*const insert)(AVector* self, size_t pos, void* value);    /**< Insert a value at the position */
	void**    (*const replace)(AVector* self, size_t pos,
			                   void* value, AValueFree freeValue);        /**< Insert a value at the position */
	void*     (*const remove)(AVector* self, size_t pos);                 /**< Remove the position from the vector */
	void**    (*const set)(AVector* self, size_t pos, void* value);       /**< Set a value at the position */
	void*     (*const get)(AVector* self, size_t pos);                    /**< Get the value at the position */
	AVector*  (*const subVector)(AVector* self, size_t pos, size_t size,  /**< Get a sub-vector of some size from a position */
	                             AValueFunc copyValue);
	AVector*  (*const copy)(AVector* self, AValueFunc copyValue);         /**< Copy the entire vector */
	AVector*  (*const join)(AVector* first, AVector* second);             /**< Join two vectors */

	void** values;   /*<  Dynamic array of pointers to values */
	size_t size;     /**< Number of items in the vector */
	size_t capacity; /*<  The allocated size of the array of values */
};

extern const AVector AVectorProto;

#endif /* AVECTOR_H_ */
