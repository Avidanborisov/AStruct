#include "minunit.h"
#include "AStack.h"

static AStack* stack = NULL;
char* testData[] = { "foo", "bar", "baz", "bug" };

const char* testCreate(void)
{
	stack = AStruct->ANew(AStack, 20);
	massert(stack != NULL, "Failed to create stack");
	massert(stack->stack->capacity == 20, "Wrong capacity after creation");
	stack->destroy(stack, NULL);

	stack = AStruct->ANew(AStack);
	massert(stack != NULL, "Failed to create stack");

	return NULL;
}

const char* testDestroy(void)
{
	massert(stack != NULL, "Invalid stack");
	stack->destroy(stack, NULL);

	return NULL;
}

const char* testPushTop(void)
{
	size_t i;

	for (i = 0; i < ARR_SIZE(testData); i++)
	{
		massert(stack->push(stack, testData[i]) != NULL, "Failed to push");
		massert(stack->top(stack) == testData[i], "Wrong top value");
	}

	massert(stack->size == ARR_SIZE(testData), "Wrong size after push");

	return NULL;
}

const char* testPop(void)
{
	size_t i = ARR_SIZE(testData);

	while (i-- > 0)
	{
		massert(stack->pop(stack) == testData[i], "Wrong value on pop");
	}

	massert(stack->size == 0, "Wrong size after pop");

	return NULL;
}

mrun(testCreate, testPushTop, testPop, testDestroy);
