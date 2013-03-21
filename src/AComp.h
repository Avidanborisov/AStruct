/**
 * @file AComp.h
 */

#ifndef ACOMP_H_
#define ACOMP_H_

/**
 * Comparison function type.
 *
 * The comparison function accepts two pointer arguments and
 * must return an integer equal to zero if the first argument
 * is considered to be equal to the second.
 *
 * If order is mandatory, the function should also return an integer
 * less than, or greater than zero if the first argument is considered
 * to be respectively less than, or greater than the second.
 */
typedef int (*AValueComp)(const void *, const void *);

#ifdef DOXYGEN

struct
{
	AValueComp pointerComp; /**< Pointer comparison function */
	AValueComp intComp;     /**< Integer comparison function */
	AValueComp stringComp;  /**< String comparison function  */
} *AComp;

/**<
 * Comparison functions.
 *
 * AComp is a pointer identifier which provides you with built-in comparison
 * functions for convenience (you don't have to implement them each time).
 *
 * The comparison function currently provided are:
 *
 * @link pointerComp AComp->pointerComp@endlink,
 * @link intComp AComp->intComp@endlink,
 * @link stringComp AComp->stringComp@endlink
 */

/**
 * @var AValueComp pointerComp
 * @return
 *         * 0  - if both pointers point to the same address.
 *         * <0 - if both pointers point to objects in the same array
 *                and the placement of the first object is before the second object.
 *         * >0 - if both pointers point to objects in the same array
 *                and the placement of the first object is after the second object.
 */

/**
 * @var AValueComp intComp
 *
 * Compare the integers dereferenced by the pointers.
 */

/**
 * @var AValueComp stringComp
 *
 * Compare the strings (pointers to char) lexicographically with case-sensitivity.
 * This function is basically a pointer to strcmp().
 */

#endif

typedef struct __AComp __AComp;
struct __AComp
{
	AValueComp pointerComp;
	AValueComp intComp;
	AValueComp stringComp;
};

extern const __AComp* AComp;

#endif /* ACOMP_H_ */
