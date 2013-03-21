/**
 * @file AStructBase.h
 */

#ifndef ASTRUCTBASE_H_
#define ASTRUCTBASE_H_

/**
 * Value function type
 *
 * This function accepts a value pointer and does
 * something with it and returns a pointer.
 */
typedef void* (*AValueFunc)(void *);

/**
 * Value freeing function
 *
 * This function accepts a value pointer and frees
 * the memory associated with it. Usually you would
 * use free().
 */
typedef void (*AValueFree)(void *);

#ifdef DOXYGEN

struct
{
	void* (*const ANew)(Type, ...); /**< Create a new data structure. */
	void* (*const ADup)(Data, ...); /**< Duplicate data. */
} *AStruct;

/**<
 * Generic data structure functions.
 *
 * AStruct is a pointer identifier which provides you with functions
 * related to AStruct data structures usage.
 *
 * The most useful one is @link ANew AStruct->ANew()@endlink which creates
 * a new data structure.
 *
 * Other convenience functions are: @link ADup AStruct->ADup()@endlink
 */

/**
 * @var void* (*ANew)(Type, ...)
 * @param Type Data structure type
 * @param ... Additional/optional arguments
 * @return Pointer to the new data structure or NULL on error (such as missing or invalid arguments)
 *
 * Create a new data structure. This function accepts an AStruct type such as AList
 * and creates a new instance of it with all the default values it should
 * have.
 *
 * Each data structure may require additional or optional arguments which are supposed to be
 * passed too, after the type name. To pass an argument, you must as well pass all the arguments
 * before that argument, even if they're optional.
 *
 * Examples of usage:
 * @code
 * AList* list = AStruct->ANew(AList); // Create a new list
 * AVector* vector = AStruct->ANew(AVector, 100); // Create a new vector with initial capacity of 100 items
 * @endcode
 */

/**
 * @var void* (*ADup)(Data, ...)
 * @param Data A lvalue expression
 * @param ... Optional size argument
 * @return Pointer to malloced() memory with the same value as Data.
 *
 * Convenience function for duplicating a variable. This function accepts
 * any variable (which has to be a lvalue) and duplicates it to new memory
 * allocted by malloc().
 *
 * By default this function duplicates sizeof(Data) bytes, but if passed another
 * argument after Data, that argument will be considered as the number of bytes
 * to duplicate.
 *
 * Examples of usage:
 * @code
 * int foo = 15;
 * int* bar = AStruct->ADup(foo); // bar points to dynamically allocted memory. *bar == foo
 *
 * char text[] = "Hello!";
 * char* copy = AStruct->ADup(*text, sizeof text); // same effect as calling strdup(text)
 * @endcode
 */

#endif

typedef struct __AStruct __AStruct;
struct __AStruct
{
	void* (*const ANew)(const void* proto, size_t size, int numArgs, ...);
	void* (*const ADup)(const void* data, size_t size);
};
extern const __AStruct* AStruct;

#include "AMacros.h"

#define ANew(type, ...) ANew(&type##Proto, sizeof(type), A_NUM_ARGS(__VA_ARGS__), ##__VA_ARGS__)

#ifdef _MSC_VER
#define ADup(...) ADup A_CHOOSE_ARG((__VA_ARGS__, A_SPECIFY_SIZE(__VA_ARGS__), A_DEFAULT_SIZE(__VA_ARGS__)))
#else
#define ADup(data, ...) ADup(&data, A_OPTIONAL_ARG(__VA_ARGS__ A_VA_COMMA(__VA_ARGS__) sizeof(data)))
#endif

#endif /* ASTRUCTBASE_H_ */
