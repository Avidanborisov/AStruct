#include "minunit.h"
#include "AHashtable.h"

static AHashtable* hashtable = NULL;

struct
{
	char* key;
	char* value;
} testData[] = { { "0fooK", "fooV" }, { "1barK", "barV" }, { "2bazK", "bazV" }, { "3bugK", "bugV" } };

const char* testCreate(void)
{
	hashtable = AStruct->ANew(AHashtable, AHash->stringHash, AComp->stringComp);
	massert(hashtable != NULL, "Failed to create hash table");

	return NULL;
}

const char* testDestroy(void)
{
	massert(hashtable != NULL, "Invalid hash table");
	hashtable->destroy(hashtable);

	return NULL;
}

const char* testSet(void)
{
	size_t i;

	for (i = 0; i < ARR_SIZE(testData); i++)
	{
		APair* rc = hashtable->set(hashtable, testData[i].key, testData[i].value);
		massert(rc != NULL, "Failed to set key");
	}

	massert(hashtable->size == ARR_SIZE(testData), "Wrong size after set");

	return NULL;
}

const char* testGet(void)
{
	size_t i;

	for (i = 0; i < ARR_SIZE(testData); i++)
	{
		void* value = hashtable->get(hashtable, testData[i].key);
		massert(value == testData[i].value, "Wrong value for key");
	}

	return NULL;
}

int traversals = 0;

void* traverseFunc(APair* pair)
{
	size_t index;

	massert(pair != NULL, "Invalid key-value pair");
	massert(pair->key != NULL, "Invalid key");
	massert(pair->value != NULL, "Invalid value");

	index = ((char *)(pair->key))[0] - '0';

	massert(pair->key == testData[index].key, "Wrong key");
	massert(pair->value == testData[index].value, "Wrong value");

	traversals++;

	return NULL;
}

const char* testTraverse(void)
{
	void* rc = hashtable->traverse(hashtable, traverseFunc);
	massert(rc == NULL, "Failed to traverse");
	massert(traversals == hashtable->size, "Wrong number of traversals");

	return NULL;
}

const char* testRemove(void)
{
	size_t i;

	for (i = 0; i < ARR_SIZE(testData); i++)
	{
		void* deleted;
		hashtable->remove(hashtable, testData[i].key);
		deleted = hashtable->get(hashtable, testData[i].key);
		massert(deleted == NULL, "Value wasn't deleted");
	}

	massert(hashtable->size == 0, "Wrong size after remove");

	return NULL;
}

mrun(testCreate, testSet, testGet, testTraverse, testRemove, testDestroy);
