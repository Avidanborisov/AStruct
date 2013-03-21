/**
 * @file AStack.h
 */

#ifndef ASTACK_H_
#define ASTACK_H_

#include <stdarg.h>
#include "AStructBase.h"
#include "AVector.h"

typedef struct AStack AStack;

/**
 * LIFO Stack
 *
 * This data structure is a generic LIFO container of values. AStack is implemented internally using an AVector.
 *
 * The arguments passed to @link ANew AStruct->ANew()@endlink to create a new stack are:
 * @code AStruct->ANew(AStack, int capacity)@endcode
 * @param [opt]capacity Optional argument to specify the initial capacity of the stack.
 *
 * Examples of creating a new stack:
 * @code
 * AStack* stack = AStruct->ANew(AStack);
 *
 * // create a stack with initial capacity of 100 items
 * AStack* stack = AStruct->ANew(AStack, 100);
 * @endcode
 */
struct AStack
{
	void*   (*const create)(AStack* self, int numArgs, va_list args);  /*<  Default creator function called by AStruct->ANew() */
	void    (*const clear)(AStack* self, AValueFree freeValue);        /**< Clear the stack */
	void    (*const destroy)(AStack* self, AValueFree freeValue);      /**< Clear and destroy the stack */
	void**  (*const push)(AStack* self, void* value);                  /**< Push a value onto the top of the stack */
	void*   (*const top)(AStack* self);                                /**< Get the value on the top of the stack */
	void*   (*const pop)(AStack* self);                                /**< Pop the top of the stack */

	AVector* stack; /*<  The actual stack is a vector */
	size_t size;    /**< The number of items on the stack */
};

extern const AStack AStackProto;

#endif /* ASTACK_H_ */
