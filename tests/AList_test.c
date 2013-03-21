#include "minunit.h"
#include "AList.h"

char* testData[] = { "foo", "bar", "baz", "bug" };
static AList* list = NULL;

const char* testCreate(void)
{
	list = AStruct->ANew(AList);
	massert(list != NULL, "Failed to create list");

	return NULL;
}

const char* testDestroy(void)
{
	massert(list != NULL, "Invalid list");
	list->destroy(list, NULL);

	return NULL;
}

const char* testAppend(void)
{
	size_t i;

	for (i = 0; i < ARR_SIZE(testData); i++)
	{
		massert(list->append(list, testData[i]) != NULL, "Failed to append");
		massert(list->last(list) == testData[i], "Wrong last value");
	}

	massert(list->size == ARR_SIZE(testData), "Wrong size after append");

	while (i-- > 0)
	{
		massert(list->popLast(list) == testData[i], "Wrong value on popLast");
	}

	massert(list->size == 0, "Wrong size after popLast");

	return NULL;
}

const char* testPrepend(void)
{
	size_t i;

	for (i = 0; i < ARR_SIZE(testData); i++)
	{
		massert(list->prepend(list, testData[i]) != NULL, "Failed to prepend");
		massert(list->first(list) == testData[i], "Wrong first value");
	}

	massert(list->size == ARR_SIZE(testData), "Wrong size after prepend");

	while (i-- > 0)
	{
		massert(list->popFirst(list) == testData[i], "Wrong value on popFirst");
	}

	massert(list->size == 0, "Wrong size after popFirst");

	return NULL;
}

const char* testInsert(void)
{
	massert(list->insert(list, list->head, testData[2]) != NULL, "Failed to insert");
	massert(list->head->value == testData[2], "Wrong value on insert");
	massert(list->size == 1, "Wrong size after insert");

	massert(list->insertAt(list, 0, testData[3]) != NULL, "Failed to insertAt");
	massert(list->valueAt(list, 0) == testData[3], "Wrong value on valueAt");
	massert(list->insertAt(list, 2, testData[1]) != NULL, "Failed to insertAt");
	massert(list->valueAt(list, 2) == testData[1], "Wrong value on valueAt");
	massert(list->size == 3, "Wrong size after insert");

	massert(list->replaceAt(list, 2, testData[0]) == testData[1], "Failed to replaceAt");
	massert(list->valueAt(list, 2) == testData[0], "Wrong value on replaceAt");
	massert(list->size == 3, "Wrong size after replaceAt");

	return NULL;
}

const char* testRemove(void)
{
	AListNode* node = list->tail;
	massert(node != NULL, "Failed to get node");

	massert(list->remove(list, node) == testData[0], "Failed to remove node or wrong value");
	massert(list->size == 2, "Wrong size after remove");
	massert(list->first(list) == testData[3], "Wrong first after remove");
	massert(list->last(list) == testData[2], "Wrong last after remove");

	massert(list->removeAt(list, 0) == testData[3], "Failed to removeAt or wrong value");
	massert(list->size == 1, "Wrong size after removeAt");
	massert(list->last(list) == testData[2], "Wrong last after remove");
	massert(list->removeAt(list, 1) == testData[2], "Failed to removeAt or wrong value");
	massert(list->size == 0, "Wrong size after removeAt");

	return NULL;
}

const char* testCopySplitJoin(void)
{
	AList* copy;
	AListNode* copyNode;
	AList* copySplit;
	AListNode* copySplitNode;
	size_t i, n = 10;
	size_t dataSize = ARR_SIZE(testData);

	for (i = 0; i < dataSize * n; i++)
	{
		massert(list->append(list, testData[i % dataSize]), "Failed to append");
		massert(list->last(list) == testData[i % dataSize], "Wrong last value");
	}

	copy = list->copy(list, NULL);
	massert(copy != NULL, "Failed to create a copy");
	massert(copy->size == list->size, "Wrong size after copy");
	for (copyNode = copy->head; i < copy->size; copyNode = copyNode->next, i++)
	{
		massert(copyNode->value == testData[i % dataSize], "Wrong values after copy");
	}

	copySplit = copy->splitAt(copy, dataSize);
	massert(copySplit != NULL, "Failed to split");
	massert(copy->size == dataSize, "Wrong size of first list after split");
	massert(copySplit->size == dataSize * (n - 1), "Wrong size of second list after split");

	for (copyNode = copy->head, copySplitNode = copySplit->head, i = 0;
	     i < dataSize;
	     copyNode = copyNode->next, copySplitNode = copySplitNode->next, i++)
	{
		massert(copyNode->value == copySplitNode->value &&
				copyNode->value == testData[i % dataSize],
				"Wrong values after split");
	}

	copy->join(copy, copySplit);
	massert(copy->size == list->size, "Wrong size after join");
	for (copyNode = copy->head; i < copy->size; copyNode = copyNode->next, i++)
	{
		massert(copyNode->value == testData[i % dataSize], "Wrong values after join");
	}

	copy->destroy(copy, NULL);

	return NULL;
}

mrun(testCreate, testAppend, testPrepend, testInsert, testRemove, testCopySplitJoin, testDestroy);
