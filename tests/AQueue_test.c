#include "minunit.h"
#include "AQueue.h"

static AQueue* queue = NULL;
char* testData[] = { "foo", "bar", "baz", "bug" };

const char* testCreate(void)
{
	queue = AStruct->ANew(AQueue);
	massert(queue != NULL, "Failed to create queue");

	return NULL;
}

const char* testDestroy(void)
{
	massert(queue != NULL, "Invalid queue");
	queue->destroy(queue, NULL);

	return NULL;
}

const char* testEnqueueFront(void)
{
	size_t i;

	for (i = 0; i < ARR_SIZE(testData); i++)
	{
		massert(queue->enqueue(queue, testData[i]) != NULL, "Failed to enqueue");
		massert(queue->front(queue) == testData[0], "Wrong front value");
	}

	massert(queue->size == ARR_SIZE(testData), "Wrong size after enqueue");

	return NULL;
}

const char* testDequeue(void)
{
	size_t i;

	for (i = 0; i < ARR_SIZE(testData); i++)
	{
		massert(queue->dequeue(queue) == testData[i], "Wrong value on dequeue");
	}

	massert(queue->size == 0, "Wrong size after dequeue");

	return NULL;
}

mrun(testCreate, testEnqueueFront, testDequeue, testDestroy);
