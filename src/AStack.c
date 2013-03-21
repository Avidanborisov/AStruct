#include <stdlib.h>
#include "AStructBase.h"
#include "AVector.h"
#include "AStack.h"

static void*   AStackCreate(AStack* self, int numArgs, va_list args);
static void    AStackClear(AStack* self, AValueFree freeValue);
static void    AStackDestroy(AStack* self, AValueFree freeValue);
static void**  AStackPush(AStack* self, void* value);
static void*   AStackTop(AStack* self);
static void*   AStackPop(AStack* self);

const AStack AStackProto =
{
	AStackCreate, AStackClear, AStackDestroy, AStackPush, AStackTop, AStackPop
};

/*
 * Create a new stack
 */
static void* AStackCreate(AStack* self, int numArgs, va_list args)
{
	if (numArgs > 0) /* Create a stack with a given capacity */
	{
		self->stack = AStruct->ANew(AVector, va_arg(args, int));
	}
	else /* Create a stack with default capacity */
	{
		self->stack = AStruct->ANew(AVector);
	}

	if (self->stack == NULL)
	{
		free(self);
		return NULL;
	}

	self->size = self->stack->size;

	return self;
}

/**
 * @fn void (*AStack::clear)(AStack* self, AValueFree freeValue)
 * @param self The stack
 * @param freeValue Callback function to free the value pointer
 *
 * Clear the stack by removing all the items and their values from
 * the stack using freeValue to free the values (if it's not NULL).
 */
static void AStackClear(AStack* self, AValueFree freeValue)
{
	if (self != NULL)
	{
		self->stack->clear(self->stack, freeValue);
		self->size = 0;
	}
}

/**
 * @fn void (*AStack::destroy)(AStack* self, AValueFree freeValue)
 * @param self The stack
 * @param freeValue Callback to free the value pointer
 *
 * @link AStack::clear() Clear@endlink the stack and free all
 * of its storage. Any access to a destroyed stack is forbidden.
 */
static void AStackDestroy(AStack* self, AValueFree freeValue)
{
	if (self != NULL)
	{
		self->stack->destroy(self->stack, freeValue);
		free(self);
	}
}

/**
 * @fn void** (*AStack::push)(AStack* self, void* value)
 * @param self The stack
 * @param value The value
 * @return Pointer to the value on the stack or NULL on error
 */
static void** AStackPush(AStack* self, void* value)
{
	void** ret = NULL;

	if (self != NULL)
	{
		if ((ret = self->stack->append(self->stack, value)) != NULL)
		{
			self->size = self->stack->size;
		}
	}

	return ret;
}

/**
 * @fn void* (*AStack::top)(AStack* self)
 * @param self The stack
 * @return The value on the top of the stack or NULL on error
 */
static void* AStackTop(AStack* self)
{
	if (self != NULL && self->size > 0)
	{
		return self->stack->get(self->stack, self->size - 1);
	}

	return NULL;
}

/**
 * @fn void* (*AStack::pop)(AStack* self)
 * @param self The stack
 * @return The value on the top of the stack or NULL on error
 *
 * Remove the top of the stack and return its value.
 */
static void* AStackPop(AStack* self)
{
	void* top = NULL;

	if (self != NULL && self->size > 0)
	{
		top = self->stack->remove(self->stack, self->size - 1);
		self->size = self->stack->size;
	}

	return top;
}
