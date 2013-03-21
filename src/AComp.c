#include <string.h>
#include "AComp.h"

int pointerComp(const void* a, const void* b);
int intComp(const void* a, const void* b);

static const __AComp _AComp = { pointerComp, intComp, (AValueComp)strcmp };
const __AComp* AComp = &_AComp;

int pointerComp(const void* a, const void* b)
{
	return (const char *)a - (const char *)b;
}

int intComp(const void* a, const void* b)
{
	return *(const int *)a - *(const int *)b;
}
