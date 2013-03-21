#include <stdlib.h>
#include "AStructBase.h"
#include "AHashtable.h"

/* Private hash table node functions */
static AHashtableNode*  makeNode(void* key, void* value, AHashtableNode* next);
static AHashtableNode*  lookupNode(AHashtableNode* start, void* key, AValueComp comp);
static void             clearNode(AHashtableNode* node, AValueFree freeKey, AValueFree freeValue);
static void             freeNode(AHashtableNode* node, AValueFree freeKey, AValueFree freeValue);

static void    AHashtableMaybeExpand(AHashtable* self); /* private */

static void*   AHashtableCreate(AHashtable* self, int numArgs, va_list args);
static void    AHashtableClear(AHashtable* self);
static void    AHashtableDestroy(AHashtable* self);
static APair*  AHashtableSet(AHashtable* self, void* key, void* value);
static void*   AHashtableGet(AHashtable* self, void* key);
static void    AHashtableRemove(AHashtable* self, void* key);
static void*   AHashtableTraverse(AHashtable* self, AHashtableTraverseFunc func);

const AHashtable AHashtableProto =
{
		AHashtableCreate, AHashtableClear, AHashtableDestroy, AHashtableSet, AHashtableGet, AHashtableRemove, AHashtableTraverse
};

/*
 * Make a new AHashtableNode from 'key', 'value' and 'next'
 * Return the node or NULL on error
 */
static AHashtableNode* makeNode(void* key, void* value, AHashtableNode* next)
{
	AHashtableNode* node = malloc(sizeof *node);

	if (node != NULL)
	{
		node->key = key;
		node->value = value;
		node->next = next;
	}

	return node;
}

/*
 * Return the node with key 'key' using 'comp' function from a list starting from 'start'
 * Return NULL on error
 */
static AHashtableNode* lookupNode(AHashtableNode* start, void* key, AValueComp comp)
{
	AHashtableNode* node;

	for (node = start; node != NULL; node = node->next)
	{
		if (comp(node->key, key) == 0)
		{
			return node;
		}
	}

	return NULL;
}

/*
 * Clear key and value in 'node' using the functions 'freeKey' and 'freeValue' (if they're not NULL)
 */
static void clearNode(AHashtableNode* node, AValueFree freeKey, AValueFree freeValue)
{
	if (freeKey != NULL)
	{
		freeKey(node->key);
	}

	if (freeValue != NULL)
	{
		freeValue(node->value);
	}
}

/*
 * clear the node 'node' and free it
 */
static void freeNode(AHashtableNode* node, AValueFree freeKey, AValueFree freeValue)
{
	clearNode(node, freeKey, freeValue);
	free(node);
}

/*
 * Expand the hash table 'self' if the buckets contain too much nodes on average
 */
static void	AHashtableMaybeExpand(AHashtable* self)
{
	size_t i, newCapacity, newLists;
	AHashtableNode** newTable;
	const size_t MAX_NODES = 10;

	if (self->size <= self->lists * MAX_NODES)
	{
		return;
	}

	newCapacity = self->capacity << 1 | 1; /* == self->capacity * 2 + 1 */
	newTable = calloc(self->capacity + 1, sizeof *newTable); /* calloc() to make all headers NULL */
	newLists = 0;

	for (i = 0; i <= self->capacity; i++)
	{
		if (self->table[i] != NULL) /* used bucket */
		{
			AHashtableNode* node = self->table[i];

			/* insert all the nodes from the bucket to the new table */
			while (node != NULL)
			{
				AHashtableNode* next = node->next;
				size_t newBucket = self->hash(node->key) & newCapacity;
				newLists += newTable[newBucket] == NULL;
				node->next = newTable[newBucket];
				newTable[newBucket] = node;
				node = next;
			}
		}
	}

	free(self->table);
	self->capacity = newCapacity;
	self->table = newTable;
	self->lists = newLists;
}

/*
 * Round n up to the closest power of 2
 */
static int upperPower2(int n)
{
	int shift;

	if (n-- <= 0)
	{
		return 0;
	}

	for (shift = 1; shift < 8 * sizeof n; shift <<= 1)
	{
		n |= n >> shift;
	}

	return ++n;
}

/*
 * Create a new AHashtable
 */
static void* AHashtableCreate(AHashtable* self, int numArgs, va_list args)
{
	const size_t DEFAULT_CAPACITY = 64;

	/* Missing arguments */
	if (numArgs < 2)
	{
		free(self);
		return NULL;
	}

	self->hash = va_arg(args, AHashFunc);
	self->comp = va_arg(args, AValueComp);
	self->freeKey = NULL;
	self->freeValue = NULL;

	if (numArgs >= 4) /* key and value destructors */
	{
		self->freeKey = va_arg(args, AValueFree);
		self->freeValue = va_arg(args, AValueFree);
	}

	/* Use 5th argument to set the capacity if non-negative */
	if (numArgs < 5 || (self->capacity = upperPower2(va_arg(args, int)) - 1) <= 0)
	{
		self->capacity = DEFAULT_CAPACITY - 1; /* Else, just use the default one */
	}

	self->table = calloc(self->capacity + 1, sizeof *self->table); /* calloc() to make all headers NULL */
	if (self->table == NULL)
	{
		free(self);
		return NULL;
	}

	self->size = 0;
	self->lists = 0;
	return self;
}

/**
 * @fn void (*AHashtable::clear)(AHashtable* self)
 * @param self The hash table
 *
 * Clear the hash table by removing all the keys and values
 * using @link AHashtable::freeKey self->freeKey@endlink and @link AHashtable::freeValue self->freeValue@endlink (if they're not NULL).
 */
static void AHashtableClear(AHashtable* self)
{
	size_t i;

	for (i = 0; i <= self->capacity; i++)
	{
		if (self->table[i] != NULL) /* used bucket */
		{
			AHashtableNode* node = self->table[i];
			AHashtableNode* next;

			while (node != NULL)
			{
				next = node->next;
				freeNode(node, self->freeKey, self->freeValue);
				node = next;
			}
		}
	}

	self->size = 0;
}

/**
 * @fn void (*AHashtable::destroy)(AHashtable* self)
 * @param self The hash table
 *
 * @link AHashtable::clear() Clear@endlink the hash table
 * and free all of its storage. Any access to a destroyed
 * table is forbidden.
 */
static void AHashtableDestroy(AHashtable* self)
{
	AHashtableClear(self);
	free(self->table);
	free(self);
}

/**
 * @fn APair* (*AHashtable::set)(AHashtable* self, void* key, void* value)
 * @param self The hash table
 * @param key The key
 * @param value The value
 * @return A key-value pair or NULL on error
 *
 * Maps the value to the key. If the same key was inserted before,
 * it and the previous value will be removed using @link AHashtable::freeKey self->freeKey@endlink
 * and @link AHashtable::freeValue self->freeValue@endlink (if they're not NULL).
 */
static APair* AHashtableSet(AHashtable* self, void* key, void* value)
{
	size_t bucket;
	AHashtableNode* node;

	AHashtableMaybeExpand(self);
	bucket = self->hash(key) & self->capacity;
	node = lookupNode(self->table[bucket], key, self->comp);

	/* New key */
	if (node == NULL)
	{
		if ((node = makeNode(key, value, self->table[bucket])) == NULL)
		{
			return NULL;
		}

		self->lists += self->table[bucket] == NULL;
		self->table[bucket] = node;
		self->size++;
	}
	else /* Replace the old one */
	{
		clearNode(node, self->freeKey, self->freeValue);
		node->key = key;
		node->value = value;
		self->size++;
	}

	return (APair *)node;
}

/**
 * @fn void* (*AHashtable::get)(AHashtable* self, void* key)
 * @param self The hash table
 * @param key The key
 * @return The value or NULL on error
 *
 * Get the value a key previously inserted by AHashtable::set().
 */
static void* AHashtableGet(AHashtable* self, void* key)
{
	AHashtableNode* node;
	size_t bucket = self->hash(key) & self->capacity;

	if ((node = lookupNode(self->table[bucket], key, self->comp)) == NULL)
	{
		return NULL;
	}

	return node->value;
}

/**
 * @fn void (*AHashtable::remove)(AHashtable* self, void* key)
 * @param self The hash table
 * @param key The key
 *
 * Remove the key and its value from the hash table and free
 * them using @link AHashtable::freeKey self->freeKey@endlink and
 * @link AHashtable::freeValue self->freeValue@endlink (if they're not NULL).
 */
static void AHashtableRemove(AHashtable* self, void* key)
{
	AValueComp comp = self->comp;
	size_t bucket = self->hash(key) & self->capacity;
	AHashtableNode* prev = NULL;
	AHashtableNode* current = self->table[bucket];

	/* Look up the key */
	while (current != NULL)
	{
		if (comp(current->key, key) == 0)
		{
			break;
		}

		prev = current;
		current = current->next;
	}

	if (prev == NULL) /* first node in the bucket */
	{
		self->table[bucket] = current->next;
	}
	else
	{
		prev->next = current->next;
	}

	freeNode(current, self->freeKey, self->freeValue);
	self->size--;
	self->lists -= self->table[bucket] == NULL;
}

/**
 * @fn void (*AHashtable::traverse)(AHashtable* self, AHashtableTraverseFunc func)
 * @param self The hash table
 * @param func The function to apply to each key-value pair
 * @return NULL in case of success, anything else in case of failure.
 *
 * Call func for each key-value pair in the hash table.
 */
static void* AHashtableTraverse(AHashtable* self, AHashtableTraverseFunc func)
{
	size_t i;

	for (i = 0; i <= self->capacity; i++)
	{
		if (self->table[i] != NULL)
		{
			AHashtableNode* node = self->table[i];

			while (node != NULL)
			{
				void* ret = func((APair *)node);

				if (ret != NULL)
				{
					return ret;
				}

				node = node->next;
			}
		}
	}

	return NULL;
}
