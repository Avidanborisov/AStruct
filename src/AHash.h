/**
 * @file AHash.h
 */

#ifndef AHASH_H_
#define AHASH_H_

#include <stdlib.h>


/**
 * Hash function type
 *
 * The hash function gets a key pointer and returns an unsigned
 * integer hash value of the key. The hash function should uniformly
 * distribute the hash values for each key to minimize collisions.
 *
 * It is advised not to roll your own hash functions but to use
 * @link hash AHash->hash()@endlink to implement your hash functions.
 */
typedef size_t (*AHashFunc)(const void*);
typedef struct __AHash __AHash;

#ifdef DOXYGEN

struct
{
	size_t (*const hash)(const void* key, size_t size); /**< Generic hash function */
	AHashFunc pointerHash; /**< Pointer hash function */
	AHashFunc intHash;     /**< Integer hash function */
	AHashFunc stringHash;  /**< String hash function  */
} *AHash;

/**<
 * Hash functions
 *
 * AHash is a pointer identifier which provides you with hash functions
 * (suitable for data structures such as AHashtable).
 *
 * @link hash AHash->hash()@endlink is a generic hash function which should
 * be used to implement hash functions for custom types not provided by AHash.
 *
 * AHash also provides you with built-in hash functions for convenience:
 *
 * @link pointerHash AHash->pointerHash@endlink,
 * @link intHash AHash->intHash@endlink,
 * @link stringHash AHash->stringHash@endlink,
 */

/**
 * @var size_t (*hash)(const void* key, size_t size)
 * @param key Pointer to data
 * @param size Size of the data in bytes
 * @return Hash value of the data
 *
 * Hash the data pointed to by key with the given size of bytes. This generic hash
 * function is and should be used to implement hash function for various data types.
 *
 * Currently the hash function is <a href="https://code.google.com/p/smhasher/source/browse/trunk/MurmurHash2.cpp">MurmurHash2</a>.
 */

/**
 * @var AHashFunc pointerHash
 * @return Hash value of the pointer itself
 *
 * Hash the numerical address the pointer points to.
 */

/**
 * @var AHashFunc intHash
 *
 * Hash the integer dereferenced by the pointer.
 */

/**
 * @var AHashFunc stringHash
 *
 * Hash the string pointed to by the pointer.
 */

#endif

struct __AHash
{
	size_t (*const hash)(const void* key, size_t size);
	AHashFunc pointerHash;
	AHashFunc intHash;
	AHashFunc stringHash;
};

extern const __AHash* AHash;

#endif /* AHASHES_H_ */
