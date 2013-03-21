#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "AStructBase.h"

#undef ANew
#undef ADup
static void* ANew(const void* proto, size_t size, int numArgs, ...);
static void* ADup(const void* data, size_t size);

static const __AStruct _AStruct = { ANew, ADup };
const __AStruct* AStruct = &_AStruct;

static void* ANew(const void* proto, size_t size, int numArgs, ...)
{
	void* newStruct;
	struct createFnc
	{
		void* (*create)(void* self, int numArgs, va_list args);
	} *create;

	va_list args;
	va_start(args, numArgs);

	newStruct = malloc(size);

	if (newStruct != NULL)
	{
		void* ret;
		memcpy(newStruct, proto, size);
		create = (struct createFnc *)newStruct;
		ret = create->create(newStruct, numArgs, args);
		va_end(args);
		return ret;
	}

	va_end(args);
	return newStruct;
}

static void* ADup(const void* data, size_t size)
{
	void* p = malloc(size);

	if (p != NULL)
	{
		memcpy(p, data, size);
	}

	return p;
}
