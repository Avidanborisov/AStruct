#ifndef AMACROS_H_
#define AMACROS_H_

#ifdef _MSC_VER /* Visual Studio macros */

#define A_NUM_ARGS(...) A_ARGS_EXIST(__VA_ARGS__) ? A_NUM_ARGS_(__VA_ARGS__) : 0
#define A_NUM_ARGS_(...) (A_LAST_ARG((__VA_ARGS__, A_CREATE_ARGS())))
#define A_ARGS_EXIST(...) sizeof(A_STRINGIFY(A_APPEND(__VA_ARGS__))) != 2 * sizeof(char)
#define A_LAST_ARG(tuple) A_LAST_ARG_ tuple
#define A_LAST_ARG_(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, N, ...) N
#define A_CREATE_ARGS() 10, 9, 8, 7, 6, 5, 4, 3, 2, 1
#define A_STRINGIFY(...) A_STRINGIFY_(__VA_ARGS__)
#define A_STRINGIFY_(...) #__VA_ARGS__
#define A_APPEND(...) __VA_ARGS__##1

#define A_CHOOSE_ARG_(_1, _2, N, ...) N
#define A_CHOOSE_ARG(tuple) A_CHOOSE_ARG_ tuple
#define A_DEFAULT_SIZE(a, ...) (&a, sizeof(a))
#define A_SPECIFY_SIZE(a, ...) (&a, __VA_ARGS__)

#else /* Works on anything else */

/* A_NUM_ARGS(): expand to the number of arguments (0 to 10) passed */
#define A_NUM_ARGS(...) A_NUM_ARGS_HELP(__VA_ARGS__ A_VA_COMMA(__VA_ARGS__) A_CREATE_ARGS())
#define A_NUM_ARGS_HELP(...) A_LAST_ARG(__VA_ARGS__)
#define A_LAST_ARG(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, N, ...) N
#define A_CREATE_ARGS() 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0

/* A_OPTIONAL_ARG():  expand to the first argument */
#define A_OPTIONAL_ARG(arg, ...) A_FIRST_ARG(arg, ##__VA_ARGS__)
#define A_FIRST_ARG(a, ...) a

/* A_VA_COMMA():  expand to a comma when 1-10 arguments are given, and to nothing if no arguments are given */
#define A_VA_COMMA(...) \
	A_VA_COMMA_HELP(, ##__VA_ARGS__, A_COMMA, A_COMMA, A_COMMA, A_COMMA, \
	A_COMMA, A_COMMA, A_COMMA, A_COMMA, A_COMMA, A_COMMA, A_COMMA,)
#define A_VA_COMMA_HELP(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, ...) _13
#define A_COMMA ,

#endif /* _MSC_VER */

#endif /* AMACROS_H_ */
