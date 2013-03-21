#include "AHash.h"
#include <string.h>

size_t MurmurHash2_x86(const void* key, size_t len);
size_t MurmurHash2_x64(const void* key, size_t len);

size_t pointerHash(const void* key);
size_t intHash(const void* key);
size_t stringHash(const void* key);

/* The hash function is MurmurHash2: https://code.google.com/p/smhasher/source/browse/trunk/MurmurHash2.cpp */
#define hashFunc (sizeof(size_t) == 8 ? MurmurHash2_x64 : MurmurHash2_x86)

static const __AHash _AHash = { hashFunc, pointerHash, intHash, stringHash };
const __AHash* AHash = &_AHash;

size_t MurmurHash2_x86(const void* key, size_t len)
{
	/* 'm' and 'r' are mixing constants generated offline.
	   They're not really 'magic', they just happen to work well. */
	const size_t m = 0x5bd1e995;
	const int r = 24;

	size_t h = len;

	/* Mix 4 bytes at a time into the hash */
	const unsigned char * data = (const unsigned char *)key;

	while (len >= 4)
	{
		size_t k = *(size_t *)data;

		k *= m;
		k ^= k >> r;
		k *= m;

		h *= m;
		h ^= k;

		data += 4;
		len -= 4;
	}

	/* Handle the last few bytes of the input array */
	switch (len)
	{
		case 3: h ^= data[2] << 16;
		case 2: h ^= data[1] << 8;
		case 1:	h ^= data[0];
		h *= m;
	};

	/* Do a few final mixes of the hash to ensure the last few
	   bytes are well-incorporated. */

	h ^= h >> 13;
	h *= m;
	h ^= h >> 15;

	return h;
}

size_t MurmurHash2_x64(const void* key, size_t len)
{
	const unsigned char* data2;
	const size_t m = sizeof(size_t) == 8 ? 0xc6a4a7935bd1e995 : 0;
	const int r = 47;

	size_t h = len * m;

	const size_t* data = (const size_t *)key;
	const size_t* end = data + (len / 8);

	while (data != end)
	{
		size_t k = *data++;

		k *= m;
		k ^= k >> r;
		k *= m;

		h ^= k;
		h *= m;
	}

	data2 = (const unsigned char*)data;

	switch (len & 7)
	{
		case 7: h ^= (size_t)data2[6] << (sizeof(size_t) == 8 ? 48 : 0);
		case 6: h ^= (size_t)data2[5] << (sizeof(size_t) == 8 ? 40 : 0);
		case 5: h ^= (size_t)data2[4] << (sizeof(size_t) == 8 ? 32 : 0);
		case 4:	h ^= (size_t)data2[3] << 24;
		case 3:	h ^= (size_t)data2[2] << 16;
		case 2:	h ^= (size_t)data2[1] << 8;
		case 1:	h ^= (size_t)data2[0];
		h *= m;
	};

	h ^= h >> r;
	h *= m;
	h ^= h >> r;

	return h;
}

size_t pointerHash(const void* key)
{
	return hashFunc(&key, sizeof key);
}

size_t intHash(const void* key)
{
	return hashFunc(key, sizeof(int));
}

size_t stringHash(const void* key)
{
	return hashFunc(key, strlen((const char*)key));
}
