#include <stdlib.h>
#include "AStructBase.h"
#include "AList.h"
#include "AQueue.h"

static void*        AQueueCreate(AQueue* self, int numArgs, va_list args);
static void         AQueueClear(AQueue* self, AValueFree freeValue);
static void         AQueueDestroy(AQueue* self, AValueFree freeValue);
static AQueueNode*  AQueueEnqueue(AQueue* self, void* value);
static void*        AQueueFront(AQueue* self);
static void*        AQueueDequeue(AQueue* self);

const AQueue AQueueProto =
{
	AQueueCreate, AQueueClear, AQueueDestroy, AQueueEnqueue, AQueueFront, AQueueDequeue
};

/*
 * Create a new queue
 */
static void* AQueueCreate(AQueue* self, int numArgs, va_list args)
{
	self->queue = AStruct->ANew(AList);

	if (self->queue == NULL)
	{
		free(self);
		return NULL;
	}

	self->size = 0;

	return self;
}

/**
 * @fn void (*AQueue::clear)(AQueue* self, AValueFree freeValue)
 * @param self The queue
 * @param freeValue Callback function to free the value pointer
 *
 * Clear the queue by removing all the items and their values from
 * the queue using freeValue to free the values (if it's not NULL).
 */
static void AQueueClear(AQueue* self, AValueFree freeValue)
{
	if (self != NULL)
	{
		self->queue->clear(self->queue, freeValue);
		self->size = 0;
	}
}

/**
 * @fn void (*AQueue::destroy)(AQueue* self, AValueFree freeValue)
 * @param self The queue
 * @param freeValue Callback function to free the value pointer
 *
 * @link AQueue::clear() Clear@endlink the queue and free all
 * of its storage. Any access to a destroyed queue is forbidden.
 */
static void AQueueDestroy(AQueue* self, AValueFree freeValue)
{
	if (self != NULL)
	{
		self->queue->destroy(self->queue, freeValue);
		free(self);
	}
}

/**
 * @fn AQueueNode* (*AQueue::enqueue)(AQueue* self, void* value)
 * @param self The queue
 * @param value The value
 * @return Pointer to the queue node containing the value or NULL on error
 */
static AQueueNode* AQueueEnqueue(AQueue* self, void* value)
{
	AListNode* ret = NULL;

	if (self != NULL)
	{
		if ((ret = self->queue->prepend(self->queue, value)) != NULL)
		{
			self->size = self->queue->size;
		}
	}

	return (AQueueNode *)ret;
}

/**
 * @fn void* (*AQueue::front)(AQueue* self)
 * @param self The queue
 * @return The value at the front of the queue or NULL on error
 */
static void* AQueueFront(AQueue* self)
{
	if (self != NULL && self->size > 0)
	{
		return self->queue->last(self->queue);
	}

	return NULL;
}

/**
 * @fn void* (*AQueue::dequeue)(AQueue* self)
 * @param self The queue
 * @return The value at the front of the queue or NULL on error
 *
 * Remove the item at the front of the queue and return its value.
 */
static void* AQueueDequeue(AQueue* self)
{
	void* front = NULL;

	if (self != NULL && self->size > 0)
	{
		front = self->queue->popLast(self->queue);
		self->size = self->queue->size;
	}

	return front;
}
