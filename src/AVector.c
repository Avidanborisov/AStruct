#include <stdlib.h>
#include <string.h> /* for memcpy() */
#include "AStructBase.h"
#include "AVector.h"

static void**   AVectorMaybeExpand(AVector* self); /* Private function */

static void*    AVectorCreate(AVector* self, int numArgs, va_list args);
static void     AVectorClear(AVector* self, AValueFree freeValue);
static void     AVectorDestroy(AVector* self, AValueFree freeValue);
static void**   AVectorAppend(AVector* self, void* value);
static void**   AVectorInsert(AVector* self, size_t pos, void* value);
static void**   AVectorReplace(AVector* self, size_t pos, void* value, AValueFree freeValue);
static void*    AVectorRemove(AVector* self, size_t pos);
static void**   AVectorSet(AVector* self, size_t pos, void* value);
static void*    AVectorGet(AVector* self, size_t pos);
static AVector* AVectorSubVector(AVector* self, size_t pos, size_t size, AValueFunc copyValue);
static AVector* AVectorCopy(AVector* self, AValueFunc copyValue);
static AVector* AVectorJoin(AVector* first, AVector* second);

const AVector AVectorProto =
{
	AVectorCreate, AVectorClear, AVectorDestroy, AVectorAppend, AVectorInsert, AVectorReplace, AVectorRemove,
	AVectorSet,	AVectorGet, AVectorSubVector, AVectorCopy, AVectorJoin
};

const size_t DEFAULT_CAPACITY = 16;
const size_t EXPAND_RATIO = 2;

/*
 * Create a new vector
 */
void* AVectorCreate(AVector* self, int numArgs, va_list args)
{
	/* If the user supplied an additional size argument, use it (in case it's valid) */
	if (numArgs == 0 || (self->capacity = va_arg(args, int)) <= 0)
	{
		self->capacity = DEFAULT_CAPACITY; /* Otherwise use the default capacity */
	}

	self->size = 0;
	self->values = malloc(self->capacity * sizeof *self->values);

	if (self->values == NULL)
	{
		free(self);
		return NULL;
	}

	return self;
}

/**
 * @fn void (*AVector::clear)(AVector* self, AValueFree freeValue)
 * @param self The vector
 * @param freeValue Callback function to free the value pointer
 *
 * Clear the vector by removing all the values using freeValue (if it's not NULL).
 */
static void AVectorClear(AVector* self, AValueFree freeValue)
{
	size_t i;

	if (self != NULL && freeValue != NULL)
	{
		for (i = 0; i < self->size; i++)
		{
			freeValue(self->values[i]);
		}
	}
}

/**
 * @fn void (*AVector::destroy)(AVector* self, AValueFree freeValue)
 * @param self The vector
 * @param freeValue Callback function to free the value pointer
 *
 * @link AVector::clear() Clear@endlink the vector and free all
 * of its storage. Any access to a destroyed vector is forbidden.
 */
static void AVectorDestroy(AVector* self, AValueFree freeValue)
{
	if (self != NULL)
	{
		AVectorClear(self, freeValue);
		free(self->values);
		free(self);
	}
}

/*
 * Expand the vector if it filled its capacity
 */
static void** AVectorMaybeExpand(AVector* self)
{
	void** newValues = self->values;

	if (self->size >= self->capacity)
	{
		do
		{
			self->capacity *= EXPAND_RATIO;
		} while (self->size >= self->capacity);

		newValues = realloc(self->values, self->capacity * sizeof *newValues);
	}

	return newValues != NULL ? self->values = newValues : NULL;
}

/**
 * @fn void** (*AVector::append)(AVector* self, void* value)
 * @param self The vector
 * @param value The value
 * @return Pointer to the value in the vector or NULL on error
 */
static void** AVectorAppend(AVector* self, void* value)
{
	if (AVectorMaybeExpand(self) != NULL)
	{
		self->values[self->size++] = value;
		return &self->values[self->size - 1];
	}

	return NULL;
}

/**
 * @fn void** (*AVector::insert)(AVector* self, size_t pos, void* value)
 * @param self The vector
 * @param pos Position index
 * @param value The value
 * @return Pointer to the value in the vector or NULL on error
 *
 * Inserting at an index other the the last, moves all
 * the items after the index one step forward.
 */
static void** AVectorInsert(AVector* self, size_t pos, void* value)
{
	if (self != NULL)
	{
		if (pos <= self->size && AVectorMaybeExpand(self) != NULL)
		{
			size_t i;

			/* move all the items after pos one step forward */
			for (i = self->size; i > pos; i--)
			{
				self->values[i] = self->values[i - 1];
			}

			self->size++;
			self->values[pos] = value;
			return &self->values[pos];
		}
	}

	return NULL;
}

/**
 * @fn void** (*AVector::replace)(AVector* self, size_t pos, void* value, AValueFree freeValue)
 * @param self The vector
 * @param pos Position index
 * @param value The new value
 * @param freeValue Callback function to free the original value pointer
 * @return Pointer to the new value in the vector or NULL on error
 *
 * Replace the value the position with a new value, and free the old one
 * using freeValue (if it isn't NULL). If position equals self->size then
 * the call would be equivalent to AVector::append() and freeValue won't
 * be called.
 */
static void** AVectorReplace(AVector* self, size_t pos, void* value, AValueFree freeValue)
{
	if (self != NULL)
	{
		if (pos == self->size)
		{
			return AVectorAppend(self, value);
		}

		if (pos < self->size)
		{
			if (freeValue != NULL)
			{
				freeValue(self->values[pos]);
			}

			self->values[pos] = value;
			return &self->values[pos];
		}
	}

	return NULL;
}

/**
 * @fn void* (*AVector::remove)(AVector* self, size_t pos)
 * @param self The vector
 * @param pos Position index
 * @return The value at the position or NULL on error
 *
 * Remove the position from the vector by moving all
 * the items after the position one step backwards.
 */
static void* AVectorRemove(AVector* self, size_t pos)
{
	if (self != NULL && pos < self->size)
	{
		void* value = self->values[pos];
		size_t i;

		self->size--;
		for (i = pos; i < self->size; i++)
		{
			self->values[i] = self->values[i + 1];
		}

		return value;
	}

	return NULL;
}

/**
 * @fn void** (*AVector::set)(AVector* self, size_t pos, void* value)
 * @param self The vector
 * @param pos Position index
 * @param value The value
 * @return Pointer to the value in the vector or NULL on error
 *
 * Set the value at the position. If position equals self->size then
 * the call would be equivalent to AVector::append().
 */
static void** AVectorSet(AVector* self, size_t pos, void* value)
{
	if (self != NULL)
	{
		if (pos == self->size)
		{
			return AVectorAppend(self, value);
		}

		if (pos < self->size)
		{
			self->values[pos] = value;
			return &self->values[pos];
		}
	}

	return NULL;
}

/**
 * @fn void* (*AVector::get)(AVector* self, size_t pos)
 * @param self The vector
 * @param pos Position index
 * @return The value at the position or NULL on error
 */
static void* AVectorGet(AVector* self, size_t pos)
{
	if (self != NULL && pos < self->size)
	{
		return self->values[pos];
	}

	return NULL;
}

/**
 * @fn AVector* (*AVector::subVector)(AVector* self, size_t pos, size_t size, AValueFunc copyValue)
 * @param self The vector
 * @param pos Position index of the start of the new sub-vector
 * @param size The size of the new sub-vector
 * @param copyValue Callback function to copy a value
 * @return A new sub-vector or NULL on error
 *
 * Create a new vector starting from the position with the given size. If copyValue isn't
 * NULL, all the values from the vector will be copied using that function to the new sub-vector.
 */
static AVector*	AVectorSubVector(AVector* self, size_t pos, size_t size, AValueFunc copyValue)
{
	AVector* subVector = NULL;

	/* Check validness of the position and the size */
	if (self != NULL && size > 0 && pos + size <= self->size)
	{
		subVector = AStruct->ANew(AVector, (int)size);

		if (subVector != NULL)
		{
			size_t k, end = pos + size;
			subVector->size = size;

			/* Copy all the items from the vector to the sub-vector */
			for (k = 0; pos < end; pos++)
			{
				subVector->values[k++] = copyValue != NULL ?
				copyValue(self->values[pos]) : self->values[pos];
			}
		}
	}

	return subVector;
}

/**
 * @fn AVector* (*AVector::copy)(AVector* self, AValueFunc copyValue)
 * @param self The vector
 * @param copyValue Callback function that returns a pointer to a copy of the value
 * @return A new vector or NULL on error
 *
 * Create a copy of the vector using copyValue to copy the values in the vector (if it's not NULL).
 */
static AVector* AVectorCopy(AVector* self, AValueFunc copyValue)
{
	return self != NULL ? AVectorSubVector(self, 0, self->size, copyValue) : NULL;
}

/**
 * @fn AVector* (*AVector::join)(AVector* first, AVector* second)
 * @param first The first vector
 * @param second The second vector
 * @return The first vector or NULL on error
 *
 * Join the first vector with the second vector. The second vector
 * will be destroyed afterwards.
 */
static AVector*	AVectorJoin(AVector* first, AVector* second)
{
	if (first != NULL && second != NULL)
	{
		size_t originalSize = first->size;
		first->size += second->size;

		if (AVectorMaybeExpand(first) != NULL)
		{
			/* Copy the values from the second vector */
			memcpy(first->values + originalSize, second->values, second->size * sizeof *second->values);
			AVectorDestroy(second, NULL);
			return first;
		}
	}

	return NULL;
}
