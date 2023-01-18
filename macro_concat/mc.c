
// #define MAC(a,b) a,b
// MAC(1,2##,3)

#define MAKE_ARGS_N(...) __VA_ARGS__
#define MAKE_ARGS_3(arg_1, arg_2, arg_3) \
    arg_1, arg_2, arg_3

MAKE_ARGS_N(arg1, arg2, arg3)
MAKE_ARGS_3(arg1, arg2, arg3)

MAKE_ARGS_N()

#define INTERLEAVE_HELPER(a, b, ...) a ## b, INTERLEAVE_HELPER(__VA_ARGS__)
#define INTERLEAVE_HELPER2(a, b) a ## b
#define INTERLEAVE(a, ...) INTERLEAVE_HELPER(a, __VA_ARGS__)
#define INTERLEAVE_END(...) INTERLEAVE_HELPER2(__VA_ARGS__)

#define list_a 1, 2, 3
#define list_b a, b, c

INTERLEAVE(list_a, list_b)

// A macro that interleaves two space separated lists

// MAC( MAC(1,2),MAC(3,4) )
// MAC( (1 ## , ## 2), (3 ## , ## 4) )

// #define MAC(a) printf("%s", #a)
// #define COMBINE(a, b) a ## b

// MAC(COMBINE(1, 2), COMBINE(3, 4))

// #define JOIN_TYPE_ARG(type,arg) #type " " #arg
// #define JOIN_TYPE_ARG_SEP(type,arg) JOIN_TYPE_ARG(type,arg),
// #define JOIN(a,b) a ## b
// #define JOIN1(a,b) JOIN(a,b)
// #define COMBINE_ARGS(...) JOIN1(JOIN_TYPE_ARG_SEP __VA_ARGS__)


// #define JOIN_TYPE_ARG(type,arg) #type " " #arg
// #define JOIN_TYPE_ARG_SEP(type,arg) JOIN_TYPE_ARG(type,arg),
// #define JOIN(a,b) a ## b
// #define JOIN_ARGS_IMPL(x, y) JOIN_TYPE_ARG_SEP x JOIN_ARGS_IMPL_
// #define JOIN_ARGS_IMPL_(x, y) JOIN_TYPE_ARG_SEP x
// #define JOIN_ARGS_IMPL_()
// #define COMBINE_ARGS(...) JOIN_ARGS_IMPL(__VA_ARGS__,())

// COMBINE_ARGS(int, a, char, b, double, c)