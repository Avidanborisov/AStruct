/**
 * @file AQueue.h
 */

#ifndef AQUEUE_H_
#define AQUEUE_H_

#include <stdarg.h>
#include "AStructBase.h"
#include "AList.h"

typedef struct AQueueNode AQueueNode;

/**
 * @link AQueue Queue@endlink node containing a value
 */
struct AQueueNode
{
	void* value; /**< The value */
};

typedef struct AQueue AQueue;

/**
 * FIFO Queue
 *
 * This data structure is a generic FIFO container of values. AQueue is implemented internally using an AList.
 *
 * The arguments passed to @link ANew AStruct->ANew()@endlink to create a new queue are:
 * @code AStruct->ANew(AQueue) @endcode
 * No additional arguments should be passed.
 *
 * Example of creating a new queue:
 * @code AQueue* queue = AStruct->ANew(AQueue); @endcode
 */
struct AQueue
{
	void*       (*const create)(AQueue* self, int numArgs, va_list args);  /*<  Default creator function called by AStruct->ANew() */
	void        (*const clear)(AQueue* self, AValueFree freeValue);        /**< Clear the queue */
	void        (*const destroy)(AQueue* self, AValueFree freeValue);      /**< Clear the queue and destroy it */
	AQueueNode* (*const enqueue)(AQueue* self, void* value);               /**< Add a value to the back of the queue */
	void*       (*const front)(AQueue* self);                              /**< Get the value at the front of the queue */
	void*       (*const dequeue)(AQueue* self);                            /**< Pop the front of the queue */

	AList* queue; /*<  The actual queue is a list */
	size_t size;  /**< The number of items in the queue */
};

extern const AQueue AQueueProto;

#endif /* AQUEUE_H_ */
