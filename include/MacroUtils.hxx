#ifndef MACRO_UTILS_HXX
#define MACRO_UTILS_HXX

// Map macro from :
// https://github.com/pfultz2/Cloak/wiki/Is-the-C-preprocessor-Turing-complete%3F
// https://github.com/swansontec/map-macro

// A 5 level / 3 calls eval macro. Making at most 365 expansion (like a stack limit)
// An explanation of the result is provided in the second link, but basically :
// Evaluating a macro adda new expansion. So, the first EVAL will add one expansion.
// Then, we have a tree like structure, multiplying by 3 the calls, and adding a new call.
#define EVAL4(...) __VA_ARGS__
#define EVAL3(...) EVAL4(EVAL4(EVAL4(__VA_ARGS__)))
#define EVAL2(...) EVAL3(EVAL3(EVAL3(__VA_ARGS__)))
#define EVAL1(...) EVAL2(EVAL2(EVAL2(__VA_ARGS__)))
#define EVAL0(...) EVAL1(EVAL1(EVAL1(__VA_ARGS__)))
#define EVAL(...) EVAL0(EVAL0(EVAL0(__VA_ARGS__)))

#define EXPAND(...) __VA_ARGS__

#define DEFER_HELPER()
#define DEFER_EVAL(x) x DEFER_HELPER()

#define CONCATENATE(x, ...) x##__VA_ARGS__

#define CHECK_AUX(x, n, ...) n
#define CHECK(...) CHECK_AUX(__VA_ARGS__, 0,)
#define PROBE(x) x, 1
#define VA_ARGS_PROBE(...) PROBE(~)

#define IS_EMPTY_PARENTHESIS(x) CHECK(PROBE x)

#define NOT(x) IF_HELPER(IS_EMPTY_PARENTHESIS(x))(DEFER_EVAL(1), DEFER_EVAL(CHECK(CONCATENATE(NOT_, x))))
#define NOT_0 PROBE(~)

#define COMPL(x) CONCATENATE(COMPL_AUX_, x)
#define COMPL_AUX_1 0
#define COMPL_AUX_0 1

// If method by pfultz2
#define IF_HELPER(x) CONCATENATE(IF_AUX_, x)
#define IF_AUX_1(ifTrue, ifNotTrue) EXPAND(ifTrue)
#define IF_AUX_0(ifTrue, ifNotTrue) EXPAND(ifNotTrue)


#define COND(x) COMPL(NOT(x))
#define IF(cond) IF_HELPER(COND(cond))

#define NOT_EMPTY_LIST_ACTION(ifEmpty, ifNotEmpy) DEFER_EVAL(ifNotEmpty)
#define EMPTY_LIST_ACTION(ifEmpty, ifNotEmpty) DEFER_EVAL(ifEmpty)
#define EMPTY_LIST_TRIGGER() 0, NOT_EMPTY_LIST_ACTION
#define EMPTY_INDICATOR_EXPANDER(isEmpty, ...) IF(isEmpty)
#define EMPTY_INDICATOR(...) EMPTY_INDICATOR_EXPANDER(EMPTY_LIST_TRIGGER __VA_ARGS__)
#define IF_EMPTY(__VA_ARGS__) EMPTY_INDICATOR(__VA_ARGS__, ())

#define STRINGIZE(x) char const* _string##__x = #x;
#define TST_MAC(...) MAP(STRINGIZE, __VA_ARGS__)

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

#define MAP_END_HANDLER ()
#define MAP_LIST_PLACEHOLDER 0
#define MAP_ITERATOR_END() 0, END_OF_VA_ARGS
#define MAP_ITERATOR_DEFER(nextSymbol, nextMacro, ...) DEFER_EVAL(nextMacro)
#define MAP_ITERATOR_EXPANDER(nextSymbol, nextMacro) MAP_ITERATOR_DEFER(nextSymbol, nextMacro, 0)
//#define MAP_ITERATOR(nextSymbol, nextMacro) MAP_ITERATOR_EXPANDER(MAP_ITERATOR_END nextSymbol, nextMacro)
#define MAP_ITERATOR2(conditional, nextMacro) DEFER_EVAL(conditional(END_OF_VA_ARGS, nextMacro))
#define MAP_ITERATOR(nextSymbol, nextMacro) DEFER_EVAL(IF(IS_EMPTY_PARENTHESIS(nextSymbol))(END_OF_VA_ARGS, nextMacro))

#define MAP_AUX1(fun, current, next, ...) fun(current) MAP_ITERATOR(next, MAP_AUX2)(fun, next, __VA_ARGS__)
#define MAP_AUX2(fun, current, next, ...) fun(current) MAP_ITERATOR(next, MAP_AUX1)(fun, next, __VA_ARGS__)

#define MAP(fun, ...) EVAL(MAP_AUX1(fun, __VA_ARGS__, (), 0))

#define MAP2_AUX1(fun, current, next, ...) fun(current, next) MAP_ITERATOR(next, MAP2_AUX2)(fun, next, __VA_ARGS__)
#define MAP2_AUX2(fun, current, next, ...) fun(current, next) MAP_ITERATOR(next, MAP2_AUX1)(fun, next, __VA_ARGS__)

#define MAP2(fun, ...) EVAL(MAP2_AUX1(fun, __VA_ARGS__, (), 0))

#endif // MACRO_UTILS_HXX