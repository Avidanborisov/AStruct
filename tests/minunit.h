/*
 * Minimal unit testing framework for C
 *
 * Based on: http://www.jera.com/techinfo/jtns/jtn002.html
 */

#ifndef MINUNIT_H_
#define MINUNIT_H_

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>

#define ARR_SIZE(a) (sizeof a / sizeof *a)

#define merrno() (errno == 0 ? "None" : strerror(errno))

#define mfile()                                                 \
	do                                                          \
	{                                                           \
		if (!mprinted)                                          \
		{                                                       \
			fprintf(stderr, "%s:\n", __FILE__);                 \
			mprinted = 1;                                       \
		}                                                       \
	}                                                           \
	while (0)

#define mlog(message)                                           \
	do                                                          \
	{                                                           \
		mfile();                                                \
		fprintf(stderr,                                         \
		        "\t[ERROR] %d: %s\n\t(errno: %s)\n\n",          \
		        __LINE__, message, merrno());                   \
	}                                                           \
	while (0)

#define massert(expr, message)                                  \
	do                                                          \
	{                                                           \
		if (!(expr))                                            \
		{                                                       \
			mlog(#expr ", \"" message "\"");                    \
			return message;                                     \
		}                                                       \
	}                                                           \
	while (0)

#define mtest(test)                                             \
	do                                                          \
	{                                                           \
		message = test();                                       \
		mtests++;                                               \
		                                                        \
		if (message != NULL)                                    \
		{                                                       \
			return message;                                     \
		}                                                       \
	}                                                           \
	while (0)

#define mrun(...)                                               \
	const char* all_tests(void)                                 \
	{                                                           \
		const char* message = NULL;                             \
		const char* (*tests[])(void) = { __VA_ARGS__ };         \
		const char* tests_str = #__VA_ARGS__;                   \
		                                                        \
		int i;                                                  \
		for (i = 0; i < sizeof tests / sizeof *tests; i++)      \
		{                                                       \
			mfile();                                            \
			fprintf(stderr, "\t----\n\tTest: ");                \
			mtprint(tests_str, stderr);                         \
			mtest(tests[i]);                                    \
		}                                                       \
		                                                        \
		return NULL;                                            \
	}                                                           \
	                                                            \
	int main(int argc, char* argv[])                            \
	{                                                           \
		fprintf(stderr, "----\nRunning: %s\n", argv[0]);        \
		printf("----\nRunning: %s\n", argv[0]);                 \
		                                                        \
		const char* result = all_tests();                       \
		if (result != NULL)                                     \
		{                                                       \
			printf("FAILED: %s\n", result);                     \
		}                                                       \
		else                                                    \
		{                                                       \
			printf("ALL TESTS PASSED\n");                       \
		}                                                       \
		                                                        \
		printf("Tests run: %d\n", mtests);                      \
		                                                        \
		return result != NULL;                                  \
	}

static void mtprint(const char* s, FILE* stream)
{
	static int i = 0;

	while (isspace(s[i]) || s[i] == ',')
	{
		i++;
	}

	for (; s[i] != ',' && s[i] != '\0'; i++)
	{
		fputc(s[i], stream);
	}

	fputc('\n', stream);
}

static int mtests = 0;
static int mprinted = 0;

#endif /* MINUNIT_H_ */
