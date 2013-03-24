#include <stdlib.h>
#include <string.h> /* for memcpy() */

#include "AStructBase.h"
#include "AList.h"

static void*        AListCreate(AList* self, int numArgs, va_list args);
static void         AListClear(AList* self, AValueFree freeValue);
static void         AListDestroy(AList* self, AValueFree freeValue);
static AListNode*   AListAppend(AList* self, void* value);
static void*        AListLast(AList* self);
static void*        AListPopLast(AList* self);
static AListNode*   AListPrepend(AList* self, void* value);
static void*        AListFirst(AList* self);
static void*        AListPopFirst(AList* self);
static AListNode*   AListInsert(AList* self, AListNode* prev, void* value);
static AListNode*   AListInsertAt(AList* self, size_t pos, void* value);
static void*        AListValueAt(AList* self, size_t pos);
static void*        AListReplaceAt(AList* self, size_t pos, void* value);
static void*        AListRemove(AList* self, AListNode* node);
static void*        AListRemoveAt(AList* self, size_t pos);
static AList*       AListCopy(AList* self, AValueFunc copyValue);
static AList*       AListSplitReal(AList* self, AListNode* node);
static AList*       AListSplit(AList* self, AListNode* node);
static AList*       AListSplitAt(AList* self, size_t pos);
static void         AListJoin(AList* first, AList* second);

const AList AListProto =
{
	AListCreate, AListClear, AListDestroy, AListAppend, AListLast, AListPopLast, AListPrepend, AListFirst,
	AListPopFirst, AListInsert, AListInsertAt, AListValueAt, AListReplaceAt, AListRemove, AListRemoveAt, AListCopy,
	AListSplit,	AListSplitAt, AListJoin
};

/*
 * Create a new list
 */
static void* AListCreate(AList* self, int numArgs, va_list args)
{
	self->head = self->tail = NULL;
	self->size = 0;

	return self;
}

/**
 * @fn void (*AList::clear)(AList* self, AValueFree freeValue)
 * @param self The list
 * @param freeValue Callback function to free the value pointer
 *
 * Clear the list by removing all the nodes and their values
 * using freeValue to free the values (if it's not NULL).
 */
static void AListClear(AList* self, AValueFree freeValue)
{
	AListNode* node = self->head;

	while (node != NULL)
	{
		AListNode* temp = node;
		node = node->next;

		if (freeValue != NULL)
		{
			freeValue(temp->value);
		}

		free(temp);
	}

	self->head = self->tail = NULL;
	self->size = 0;
}

/**
 * @fn void (*AList::destroy)(AList* self, AValueFree freeValue)
 * @param self The list
 * @param freeValue Callback function to free the value pointer
 *
 * @link AList::clear() Clear@endlink the list and free all
 * of its storage. Any access to a destroyed list is forbidden.
 */
static void AListDestroy(AList* self, AValueFree freeValue)
{
	if (self != NULL)
	{
		AListClear(self, freeValue);
		free(self);
	}
}

/**
 * @fn AListNode* (*AList::append)(AList* self, void* value)
 * @param self The list
 * @param value The value
 * @return Pointer to the new node or NULL on error.
 */
static AListNode* AListAppend(AList* self, void* value)
{
	AListNode* node = malloc(sizeof *node);

	if (node != NULL)
	{
		node->value = value;
		node->next = NULL;

		if (self->tail == NULL) /* first node */
		{
			self->head = self->tail = node;
			node->prev = NULL;
		}
		else
		{
			self->tail->next = node;
			node->prev = self->tail;
			self->tail = node;
		}

		self->size++;
	}

	return node;
}

/**
 * @fn void* (*AList::last)(AList* self)
 * @param self The list
 * @return The value of the last node or NULL on error
 */
static void* AListLast(AList* self)
{
	AListNode* tail = self->tail;

	return tail != NULL ? tail->value : NULL;
}

/**
 * @fn void* (*AList::popLast)(AList* self)
 * @param self The list
 * @return The value of the last node or NULL on error
 *
 * Remove the last node from the list and return its value.
 */
static void* AListPopLast(AList* self)
{
	AListNode* tail = self->tail;

	return tail != NULL ? AListRemove(self, tail) : NULL;
}

/**
 * @fn AListNode* (*AList::prepend)(AList* self, void* value)
 * @param self The list
 * @param value The value
 * @return Pointer to the new node or NULL on error.
 */
static AListNode* AListPrepend(AList* self, void* value)
{
	AListNode* node = malloc(sizeof *node);

	if (node != NULL)
	{
		node->value = value;
		node->prev = NULL;

		if (self->head == NULL) /* first node */
		{
			self->head = self->tail = node;
			node->next = NULL;
		}
		else
		{
			self->head->prev = node;
			node->next = self->head;
			self->head = node;
		}

		self->size++;
	}

	return node;
}

/**
 * @fn void* (*AList::first)(AList* self)
 * @param self The list
 * @return The value of the first node or NULL on error.
 */
static void* AListFirst(AList* self)
{
	AListNode* head = self->head;

	return head != NULL ? head->value : NULL;
}

/**
 * @fn void* (*AList::popFirst)(AList* self)
 * @param self The list
 * @return The value of the first node or NULL on error
 *
 * Remove the first node from the list and return its value.
 */
static void* AListPopFirst(AList* self)
{
	AListNode* head = self->head;

	return head != NULL ? AListRemove(self, head) : NULL;
}

/**
 * @fn AListNode* (*AList::insert)(AList* self, AListNode* prev, void* value)
 * @param self The list
 * @param prev The node to insert after
 * @param value The value
 * @return Pointer to the new node or NULL on error
 *
 * Insert a value to a new node which will be after prev.
 */
static AListNode* AListInsert(AList* self, AListNode* prev, void* value)
{
	AListNode* node;

	if (prev == self->tail)
	{
		return AListAppend(self, value);
	}

	node = malloc(sizeof *node);

	if (node != NULL && prev != NULL)
	{
		node->value = value;
		node->next = prev->next;
		node->next->prev = node;
		node->prev = prev;
		prev->next = node;
		self->size++;

		return node;
	}

	return NULL;
}

/**
 * @fn AListNode* (*AList::insertAt)(AList* self, size_t pos, void* value)
 * @param self The list
 * @param pos Position index
 * @param value The value
 * @return Pointer to the new node or NULL on error
 *
 * Insert a value to a new node which will be placed at a zero-based position in the list.
 */
static AListNode* AListInsertAt(AList* self, size_t pos, void* value)
{
	AListNode* node;
	size_t i;

	/* invalid positions */
	if (pos < 0 || pos > self->size)
	{
		return NULL;
	}

	if (pos == 0) /* first */
	{
		return AListPrepend(self, value);
	}
	else if (pos == self->size) /* last */
	{
		return AListAppend(self, value);
	}

	/* iterate from the nearest end */
	if (pos < self->size / 2)
	{
		for (node = self->head, i = 0; i < pos; i++) /* from head */
		{
			node = node->next;
		}
	}
	else
	{
		for (node = self->tail, i = self->size - 1; i > pos; i--) /* from tail */
		{
			node = node->prev;
		}
	}

	return AListInsert(self, node->prev, value);
}

/**
 * @fn void* (*AList::valueAt)(AList* self, size_t pos)
 * @param self The list
 * @param pos Position index
 * @return The value of the node at the zero-based position or NULL on error
 */
static void* AListValueAt(AList* self, size_t pos)
{
	AListNode* node;
	size_t i;

	/* invalid positions */
	if (pos < 0 || pos >= self->size)
	{
		return NULL;
	}

	/* iterate from the nearest end */
	if (pos < self->size / 2)
	{
		for (node = self->head, i = 0; i < pos; i++) /* from head */
		{
			node = node->next;
		}
	}
	else
	{
		for (node = self->tail, i = self->size - 1; i > pos; i--) /* from tail */
		{
			node = node->prev;
		}
	}

	return node->value;
}

/**
 * @fn void* (*AList::replaceAt)(AList* self, size_t pos, void* value)
 * @param self The list
 * @param pos Position index
 * @param value The new value
 * @return The original value or NULL on error
 *
 * Replace the value at a zero-based position in the list with a new value and return the original one.
 */
static void* AListReplaceAt(AList* self, size_t pos, void* value)
{
	AListNode* node;
	size_t i;
	void* oldValue;

	/* invalid positions */
	if (pos < 0 || pos >= self->size)
	{
		return NULL;
	}

	/* iterate from the nearest end */
	if (pos < self->size / 2)
	{
		for (node = self->head, i = 0; i < pos; i++) /* from head */
		{
			node = node->next;
		}
	}
	else
	{
		for (node = self->tail, i = self->size - 1; i > pos; i--) /* from tail */
		{
			node = node->prev;
		}
	}

	oldValue = node->value;
	node->value = value;

	return oldValue;
}

/**
 * @fn void* (*AList::remove)(AList* self, AListNode* node)
 * @param self The list
 * @param node The node to remove
 * @return The value in the node or NULL on error
 *
 * Remove the node from the list and return its value.
 */
static void* AListRemove(AList* self, AListNode* node)
{
	void* value = NULL;

	/* empty list or invalid node */
	if (self->head == NULL || self->tail == NULL || node == NULL)
	{
		return NULL;
	}

	if (node == self->head && node == self->tail) /* the only node in the list */
	{
		self->head = self->tail = NULL;
	}
	else if (node == self->head) /* first node */
	{
		self->head = self->head->next;
		self->head->prev = NULL;
	}
	else if (node == self->tail) /* last node */
	{
		self->tail = self->tail->prev;
		self->tail->next = NULL;
	}
	else /* middle node */
	{
		node->prev->next = node->next;
		node->next->prev = node->prev;
	}

	value = node->value;
	free(node);
	self->size--;

	return value;
}

/**
 * @fn void* (*AList::removeAt)(AList* self, size_t pos)
 * @param self The list
 * @param pos Position index
 * @return The value at the position or NULL on error
 *
 * Remove the node at a zero-based position and return its value.
 */
static void* AListRemoveAt(AList* self, size_t pos)
{
	AListNode* node;
	size_t i;

	/* iterate from the nearest end */
	if (pos < self->size / 2)
	{
		for (node = self->head, i = 0; i < pos; i++) /* from head */
		{
			node = node->next;
		}
	}
	else
	{
		for (node = self->tail, i = self->size - 1; i > pos; i--) /* from tail */
		{
			node = node->prev;
		}
	}

	return AListRemove(self, node);
}

/**
 * @fn AList* (*AList::copy)(AList* self, AValueFunc copyValue)
 * @param self The list
 * @param copyValue Callback function that returns a pointer to a copy of the value
 * @return A new copy of the list or NULL on error
 *
 * Create a new copy of the list and all of its values (if copyValue isn't NULL)
 * and return the new list.
 */
static AList* AListCopy(AList* self, AValueFunc copyValue)
{
	AListNode* node = self->head;
	AList* newList = AStruct->ANew(AList);

	if (newList != NULL)
	{
		while (node != NULL)
		{
			newList->append(newList, copyValue != NULL ? copyValue(node->value) : node->value);
			node = node->next;
		}

		newList->size = self->size;
	}

	return newList;
}

/*
 * Split the list 'self' by node 'node' and return the second list
 * (which will include the node). Don't change the sizes of the lists.
 */
static AList* AListSplitReal(AList* self, AListNode* node)
{
	AList* newList = AStruct->ANew(AList);

	if (newList != NULL)
	{
		if (node == self->head) /* first node */
		{
			newList->head = node;
			newList->tail = self->tail;
			self->head = self->tail = NULL; /* first list is empty now */
		}
		else
		{
			newList->head = node;
			newList->tail = self->tail;
			self->tail = node->prev;
			self->tail->next = NULL;
			newList->head->prev = NULL;
		}
	}

	return newList;
}

/**
 * @fn AList* (*AList::split)(AList* self, AListNode* node)
 * @param self The list
 * @param node The node to split by
 * @return The second list
 *
 * Split the list by the node. This will trim the list to contain
 * everything up to (but not including) the node and create a second
 * list which will contain everything from (and including) the node
 * up to the end of the original list.
 */
static AList* AListSplit(AList* self, AListNode* node)
{
	AListNode* iter;
	size_t size;
	AList* newList = AListSplitReal(self, node);

	if (newList != NULL)
	{
		/* get the size of the second list */
		for (iter = newList->head, size = 0; iter != NULL; iter = iter->next)
		{
			size++;
		}

		newList->size = size;
		self->size -= size;
	}

	return newList;
}

/**
 * @fn AList* (*AList::splitAt)(AList* self, size_t pos)
 * @param self The list
 * @param pos Position index
 * @return The second list
 *
 * Split the list by a zero-based position. This will trim the list
 * to contain everything up to (but not including) the node at the
 * position and create a second list which will contain everything
 * from (and including) the node at the position up to the end of
 * the original list.
 */
static AList* AListSplitAt(AList* self, size_t pos)
{
	AList* newList;
	AListNode* node;
	size_t i;

	/* iterate from the nearest end */
	if (pos < self->size / 2)
	{
		for (node = self->head, i = 0; i < pos; i++) /* from head */
		{
			node = node->next;
		}
	}
	else
	{
		for (node = self->tail, i = self->size - 1; i > pos; i--) /* from tail */
		{
			node = node->prev;
		}
	}

	newList = AListSplitReal(self, node);
	if (newList != NULL)
	{
		newList->size = self->size - pos;
		self->size -= newList->size;
	}

	return newList;
}

/**
 * @fn void (*AList::join)(AList* first, AList* second)
 * @param first The first list
 * @param second The second list
 *
 * Join the first list with the second list. The second list
 * will be destroyed afterwards.
 */
static void AListJoin(AList* first, AList* second)
{
	first->tail->next = second->head;
	second->head->prev = first->tail;
	first->tail = second->tail;

	first->size += second->size;
	free(second);
}
