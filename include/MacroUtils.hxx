#ifndef MACRO_UTILS_HXX
#define MACRO_UTILS_HXX

// Map macro from :
// https://github.com/pfultz2/Cloak/wiki/Is-the-C-preprocessor-Turing-complete%3F
// https://github.com/swansontec/map-macro

// A 5 level / 3 calls eval macro. Making at most 365 expansion (like a stack limit)
// An explanation of the result is provided in the second link, but basically :
// Evaluating a macro adda new expansion. So, the first EVAL will add one expansion.
// Then, we have a tree like structure, multiplying by 3 the calls, and adding a new call.
// This is used to force the compiler to make multiple passes.
#define EVAL4(...) __VA_ARGS__
#define EVAL3(...) EVAL4(EVAL4(EVAL4(__VA_ARGS__)))
#define EVAL2(...) EVAL3(EVAL3(EVAL3(__VA_ARGS__)))
#define EVAL1(...) EVAL2(EVAL2(EVAL2(__VA_ARGS__)))
#define EVAL0(...) EVAL1(EVAL1(EVAL1(__VA_ARGS__)))
#define EVAL(...) EVAL0(EVAL0(EVAL0(__VA_ARGS__)))

#define EXPAND(...) __VA_ARGS__

// Allows to defer evaluation. For instance
// #define A(x) hello x
// A(world) -> will expand to hello world
// A(A(world)) -> will expand to hello A(world)
// EVAL(DEFER(A)(A(world))) -> will expand to hello hello world
// The reason for that is that the preprocessor will do the following :
// -> sees EVAL with an open parenthesis, it's a function macro, so it looks to the right
// -> sees DEFER with an open parenthesis, expands it
// -> sees A(world), expands it
// -> expands EVAL
// -> now it will expand the arguments of EVAL, which is effectively a second pass
// -> sees A("hello world"), expands it
#define EMPTY()
#define DEFER_EVAL(x) x EMPTY()

// Simple concatenation macro
#define CONCATENATE(x, ...) x##__VA_ARGS__

#define SECOND(x, n, ...) n
#define CHECK(...) SECOND(__VA_ARGS__, 0,)
#define PROBE(x) x, 1
#define VA_ARGS_PROBE(...) PROBE(~)

#define IS_EMPTY_PARENTHESIS(x) CHECK(PROBE x)

#define NOT(x) IF_ELSE_HELPER(IS_EMPTY_PARENTHESIS(x))(DEFER_EVAL(1), DEFER_EVAL(CHECK(CONCATENATE(NOT_, x))))
#define NOT_0 PROBE(~)
#define NOT_1 

#define COMPL(x) CONCATENATE(COMPL_AUX_, x)
#define COMPL_AUX_1 0
#define COMPL_AUX_0 1

// If method by pfultz2
#define IF_HELPER(x) CONCATENATE(IF_AUX_, x)
#define IF_AUX_0(_) EMPTY
#define IF_AUX_1(if_true) EXPAND(if_true)

#define IF_ELSE_HELPER(x) CONCATENATE(IF_ELSE_AUX_, x)
#define IF_ELSE_AUX_0(_, if_not_true) EXPAND(if_not_true)
#define IF_ELSE_AUX_1(if_true, _) EXPAND(if_true)


#define COND(x) COMPL(NOT(x))
#define IF(cond) IF_HELPER(COND(cond))
#define IF_ELSE(cond) IF_ELSE_HELPER(COND(cond))

#define NOT_EMPTY_LIST_ACTION(_, if_not_empty) DEFER_EVAL(if_not_empty)
#define EMPTY_LIST_ACTION(if_empty, _) DEFER_EVAL(if_empty)
#define EMPTY_LIST_TRIGGER() 0, NOT_EMPTY_LIST_ACTION
#define EMPTY_INDICATOR_EXPANDER(is_empty, ...) IF(is_empty)
#define EMPTY_INDICATOR(...) EMPTY_INDICATOR_EXPANDER(EMPTY_LIST_TRIGGER __VA_ARGS__)
#define IF_EMPTY(__VA_ARGS__) EMPTY_INDICATOR(__VA_ARGS__, ())

#define STRINGIZE(x) char const* _string##__x = #x;

#define STRINGIFY_AUX(x) #x
#define STRINGIFY(x) STRINGIFY_AUX(x)

#define VA_ARGS_COUNT(...) std::tuple_size<decltype(std::make_tuple(__VA_ARGS__))>::value

#define ADD_COMMA() ,
#define EMPTY_ACTION()

// Void macro to stop the call tree
#define END_OF_VA_ARGS(...)

// The MAP_ITERATOR_EXPANDER is needed, because if we have nextSymbol == '()', the preprocessor
// will not detect that we want a function call. This is a simple text replace. So, we need to 
// pass it to another function to induce another scan.
// Moreover, here, a simple expand will not work. Of course, the arguments of a macro function are 
// considered simple text, and thus are not expanded.

// MAP MACROS
#define MAP_END_HANDLER ()
#define MAP_ITERATOR(NEXT_SYMBOL, NEXT_MACRO)       \
    DEFER_EVAL                                      \
    (                                               \
        IF_ELSE(IS_EMPTY_PARENTHESIS(NEXT_SYMBOL))  \
        (                                           \
            END_OF_VA_ARGS,                         \
            NEXT_MACRO                              \
        )                                           \
    )                                               \

#define MAP_AUX1(FCT, CURRENT, NEXT, ...)                   \
    FCT(CURRENT)                                            \
    MAP_ITERATOR(NEXT, MAP_AUX2)(FCT, NEXT, __VA_ARGS__)    \

#define MAP_AUX2(FCT, CURRENT, NEXT, ...)                   \
    FCT(CURRENT)                                            \
    MAP_ITERATOR(NEXT, MAP_AUX1)(FCT, NEXT, __VA_ARGS__)    \

#define MAP(FCT, ...)                                   \
    EVAL                                                \
    (                                                   \
        MAP_AUX1(FCT, __VA_ARGS__, MAP_END_HANDLER)     \
    )                                                   \

#define MAP2_AUX1(FCT, CURRENT, NEXT, ...)                  \
    FCT(CURRENT, NEXT)                                      \
    MAP_ITERATOR(NEXT, MAP2_AUX2)(FCT, NEXT, __VA_ARGS__)   \

#define MAP2_AUX2(FCT, CURRENT, NEXT, ...)                  \
    FCT(CURRENT, NEXT)                                      \
    MAP_ITERATOR(NEXT, MAP2_AUX1)(FCT, NEXT, __VA_ARGS__)   \

#define MAP2(FCT, ...)                                  \
    EVAL                                                \
    (                                                   \
        MAP2_AUX1(FCT, __VA_ARGS__, MAP_END_HANDLER)    \
    )                                                   \

#endif // MACRO_UTILS_HXX