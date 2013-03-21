#include "minunit.h"
#include <stdlib.h>
#include "AVector.h"

static AVector* vector = NULL;
char testData[][4] = { "foo", "bar", "baz", "bug" };

const char* testCreate(void)
{
	vector = AStruct->ANew(AVector, 20);
	massert(vector != NULL, "Failed to create a new vector");
	massert(vector->capacity == 20, "Wrong capacity after creation");

	return NULL;
}

const char* testDestroy(void)
{
	massert(vector != NULL, "Invalid vector");
	vector->destroy(vector, free);

	return NULL;
}

const char* testAppend(void)
{
	size_t i;

	for (i = 0; i < ARR_SIZE(testData); i++)
	{
		char* value;
		massert(vector->append(vector, AStruct->ADup(testData[i])) != NULL, "Failed to append");
		massert((value = vector->get(vector, i)) != NULL, "Failed to get appended value");
		massert(!strcmp(value, testData[i]), "Wrong value on append");
	}

	massert(vector->size == ARR_SIZE(testData), "Wrong size after append");

	return NULL;
}

const char* testInsert(void)
{
	size_t i;

	for (i = 0; i < ARR_SIZE(testData); i++)
	{
		char* value;
		massert(vector->insert(vector, i, AStruct->ADup(testData[i])) != NULL, "Failed to insert");
		massert((value = vector->get(vector, i)) != NULL, "Failed to get inserted value");
		massert(!strcmp(value, testData[i]), "Wrong value on insert");
	}

	massert(vector->size == 2 * ARR_SIZE(testData), "Wrong size after insert");

	return NULL;
}

const char* testReplace(void)
{
	size_t i;

	for (i = 0; i < ARR_SIZE(testData); i++)
	{
		char* value;
		massert(vector->replace(vector, i, testData[ARR_SIZE(testData) - i - 1], free),
				"Failed to replace value");
		massert((value = vector->get(vector, i)) != NULL, "Failed to get replaced value");
		massert(value == testData[ARR_SIZE(testData) - i - 1], "Wrong value on replace");
	}

	massert(vector->size == 2 * ARR_SIZE(testData), "Wrong size after replace");

	return NULL;
}

const char* testSet(void)
{
	size_t i;

	for (i = 0; i < ARR_SIZE(testData); i++)
	{
		char* value;
		massert(vector->set(vector, i, AStruct->ADup(testData[i])) != NULL, "Failed to set value");
		massert((value = vector->get(vector, i)) != NULL, "Failed to get set value");
		massert(!strcmp(value, testData[i]), "Wrong value on set");
	}

	massert(vector->size == 2 * ARR_SIZE(testData), "Wrong size after set");

	return NULL;
}

const char* testGet(void)
{
	size_t i;

	for (i = 0; i < vector->size; i++)
	{
		char* value = vector->get(vector, i);
		massert(value != NULL, "Failed to get value");
		massert(!strcmp(value, testData[i % ARR_SIZE(testData)]), "Wrong value on get");
	}

	return NULL;
}

const char* testRemove(void)
{
	size_t i;

	for (i = 0; i < ARR_SIZE(testData); i++)
	{
		char* value = vector->remove(vector, 0);
		massert(value != NULL, "Failed to remove value");
		massert(!strcmp(value, testData[i]), "Wrong value on get");
		free(value);
	}

	massert(vector->size == ARR_SIZE(testData), "Wrong size after remove");

	return NULL;
}

const char* testSubVector(void)
{
	size_t i;
	AVector* sub = vector->subVector(vector, 1, 2, (AValueFunc)strdup);
	massert(sub != NULL, "Failed to create sub vector");
	massert(sub->size == 2, "Wrong size of sub vector");

	for (i = 0; i < sub->size; i++)
	{
		char* subValue = sub->get(sub, i);
		char* vecValue = vector->get(vector, i + 1);
		massert(subValue != NULL && vecValue != NULL, "Failed to get values");
		massert(!strcmp(subValue, vecValue), "Wrong value in sub vector");
	}

	sub->destroy(sub, free);

	return NULL;
}

const char* testCopyJoin(void)
{
	size_t i;
	AVector* copy = vector->copy(vector, (AValueFunc)strdup);
	massert(copy != NULL, "Failed to create a copy");
	massert(copy->size == vector->size, "Wrong size after copy");

	for (i = 0; i < copy->size; i++)
	{
		char* copyValue = copy->get(copy, i);
		char* vecValue = vector->get(vector, i);
		massert(copyValue != NULL && vecValue != NULL, "Failed to get value");
		massert(!strcmp(copyValue, vecValue), "Wrong value in copied vector");
	}

	vector->join(vector, copy);
	massert(vector != NULL, "Failed to join vector");
	massert(vector->size == 2 * ARR_SIZE(testData), "Wrong size after join");

	return testGet();
}

mrun(testCreate, testAppend, testInsert, testReplace, testSet,
     testGet, testRemove, testSubVector, testCopyJoin, testDestroy );
