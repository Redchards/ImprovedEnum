// This file is part of DAWN ( Do Anything With Nothing ) engine.

#ifndef CORE_PLATFORM
#define CORE_PLATFORM

/**@todo Remove useless lines, check if warning should be error, or remove the "try to compile" policy
   @todo Test it on lot of platform in order to be sure all works as expected.
*/
///@note Early version, do not rely on it too much !

#define CLANG_COMPILER   1
#define GCC_COMPILER     2
#define MSVC_COMPILER    3
#define ICC_COMPILER     4
#define BORLAND_COMPILER 5

#define LINUX   1
#define WINDOWS 2
#define MACOSX  3

#define UNKOWN "unknown"

#if defined( __STDC__ ) && !defined( __cplusplus )
#   error "This project require a C++ compiler !"
#endif
#if defined( __cplusplus ) && ( __cplusplus < 201703L )
#   error "This project require a C++17 compliant compiler in order to build properly!\
             If your compiler support C++17, please turn C++17 support option on !"
#endif

#if defined( __clang__ ) || defined( __llvm__ )
#   define FUNCTION __PRETTY_FUNCTION__
#   define restrict __restrict__
#   if __has_attribute(always_inline)
#       define force_inline __atribute__((always_inline))
#   endif
#   if __has_builtin(__builtin_expect)
#       define likely(x)    __builtin_expect((x), 1)
#       define unlikely(x)  __builtin_expect((x), 0)
#   endif
#   define COMPILER CLANG_COMPILER
#   if defined( __clang_patchlevel__ )
#       define COMPILER_VERSION ( __clang_major__ * 10000 \
                                + __clang_minor__ * 100   \
                                + __clang_patchlevel__ )
#   else
#       define COMPILER_VERSION ( __clang_major * 10000  \
                                + __clang_minor * 100)
#   endif
#   if __has_extension(attribute_deprecated_with_message)
#       define DEPRECATED(msg) __attribute((deprecated(msg)))
#   elif __has_attribute(deprecated)
#       define DEPRECATED(msg) __attribute((deprecated))
#   endif
#   if __has_feature(cxx_override_control)
#       define override override
#       define final final
#   endif
#   if __has_attribute(naked)
#       define naked __attribute__((naked)) 
#   endif
#elif defined( __GNUC__ ) || defined( __MINGW__ )
#   define FUNCTION __PRETTY_FUNCTION__
#	define restrict __restrict__
#	define force_inline __attribute__((always_inline))
#	define likely(x)    __builtin_expect((x),1)
#	define unlikely(x)  __builtin_expect((x),0)
#   if defined( __MINGW__ )
#       define COMPILER_NAME "MinGW"
#   else
#       define COMPILER_NAME "gcc"
#   endif
#   define COMPILER GCC_COMPILER
#   if defined( __GNUC_PATCHLEVEL__ )
#       define COMPILER_VERSION ( __GNUC__ * 10000     \
                                + __GNUC_MINOR__ * 100 \
                                + __GNUC_PATCHLEVEL__ )
#   else
#       define COMPILER_VERSION ( __GNUC__ * 10000 \
                                + __GNUC__ * 100 )
#   endif
#   if( COMPILER_VERSION < 40500 )
#       define DEPRECATED(msg) __attribute__((deprecated))
#   else
#       define DEPRECATED(msg) __attribute__((deprecated(msg)))
#	endif
#	if( COMPILER_VERSION >= 40700 )
#		define override override
#		define final final
#   endif
#   define naked __attribute__((naked)) 
#elif defined( _MSC_VER )
#   define FUNCTION __FUNCSIG__
#	define restrict __declspec(restrict)
#	define force_inline __forceinline
#	define likely(x)
#	define unlikely(x)
#   define COMPILE_NAME "Microsoft compiler"
#   define COMPILER MSVC_COMPILER
#   if( ( _MSC_VER <= 1200 ) )
#       define COMPILER_VERSION _MSC_VER
#   else
#       define COMPILER_VERSION _MSC_FULL_VER
#   endif
#	if( COMPILER_VERSION >= 1400 )
#		define override override
#		define final sealed
#	endif
#   define naked __declspec(naked)
#   define DEPRECATED(msg) __delcspec( deprecated(msg) )
#elif defined( __INTEL_COMPILER ) || defined( __ICL )
#   define FUNCTION __PRETTY_FUNCTION__
#	define restrict restrict
#	define force_inline // Could be a way, but I don't know it yet
#	define likely(x) // No support atm
#	define unlikely(x)
#   define COMPILER_NAME "Intel compiler"
#   define COMPILER ICC_COMPILER
#   define COMPILER_VERSION __INTEL_COMPILER
#	if( COMPILER_VERSION >= 20130000 )
#		define override override
#		define final final
#   else
#       define override
#       define final
#	endif
#   define DEPRECATED(msg) __attribute__( ( deprecated ) )
#   define naked __attribute__ ((naked ))
#else
#   warning "Unknown compiler, there might be some troubles during compilation, and some special features are disabled. \
			 This may lead to performance problems and/or debugging troubles! Please check list of supported compiler."
#   ifdef( __func__ ) // standard since C++ 11 but compiler may not support it
#       define FUNCTION __func__
#   else
#       warning "Compiler is not AINSI C99 standard, can't use __func__. Some debug functions will be disabled"
#   endif
#	define restrict
#	define always_inline
#   define COMPILER_NAME "UNKNWON"
#   define COMPILER UNKNOWN
#   define COMPILER_VERSION UNKNOWN
#   warning "Compiler not recongnized, results may be unexpected"
#   define DEPRECATED(t)
#endif

#if !defined( restrict )
#   define restrict
#   warning "The 'restrict' keyword is not supported. The feature 'restrict' will be disabled, and the keyword will have no effect".
#endif

#if !defined( override ) && !defined( final )
#	define override
#	define final
#	warning The 'final' and 'override' keywords are not supported. They are disabled, and will have no effect if used !
#elif !defined( override )
#   define override
#   warning The 'override' keyword is not supported. It is disabled, and will have no effect if used !
#elif !defined( final )
#   define final
#   warning The 'final' keyword is not supported. It is disabled, and will have no effect if used !
#endif

#if !defined( naked )
#   define naked
#   warning Keyword 'naked' is not defined by your compiler. The keyword is disabled, and will have no effect if used !
#endif

#if !defined( force_inline )
#   define force_inline
#   warning The 'force_inline' specifier is not supported. It is disabled, and will have no effect if used !
#endif

/* Assume if one of those is not defined, the other must not be so */
#if !defined( likely ) || !defined( unlikely )
#   define likely(x)
#   define unlikely(x)
#   warning The 'likely' and 'unlikely' features are note supported. They are disabled, and there use will have no effect if used !
#endif

#if !defined( DEPRECATED )
#   define DEPRECATED(msg)
#   warning The deprecated attribute is not supported. Depreciation report is disabled. You should see the documentation to avoid using deprecated functions !
#endif

#if defined( __gnu_linux__ ) || defined( __linux__ )
#   define OS_NAME "Linux"
#   define OS LINUX
#elif defined( macintosh ) || defined( Macintosh ) || ( defined( __APPLE__ ) && defined( __MACH__ ) )
#   define OS_NAME "MacOSX"
#   define OS MACOSX
#elif defined( _WIN32 )
#   define OS_NAME "Windows"
#   define OS WINDOWS
#else
#   define OS_NAME UNKNOWN
#   define OS UNKNOWN
#   warning "Operating system not recognized. Unexpected results might happen"
#endif

#if( OS == LINUX || OS == MACOSX ) && \
    COMPILER_VERSION >= 4000 && \
    ( COMPILER == ICC_COMPILER || COMPILER == GCC_COMPILER )
#       define EXPORT __attribute__( ( visibility( "default" ) ) )
#       define IMPORT __attribute__( ( visibility( "default" ) ) )
#       define LOCAL __attribute__( ( visibility( "hidden" ) ) )
#elif( OS == WINDOWS )
#   define EXPORT __declspec( dllexport )
#   define IMPORT __declspec( dllimport )
#   define LOCAL
#else
#   define EXPORT
#   define IMPORT
#   define LOCAL
#endif

#if !defined( BUILD_STATIC ) && !defined( IMPORT_LIBRARY )
#   define API EXPORT
#elif defined( IMPORT_LIBRARY )
#   define API IMPORT
#else
#   define API
#endif

#if defined( __amd64__ ) || defined( __x86_64__ ) || defined( _M_X64 ) || defined( _M_AMD64) || \
    defined( _ia64__ ) || defined( _M_IA64 ) || defined( __itanium__ )
#   define PLATFORM_X64
#else
#   define PLATFORM_X86
#endif

#if defined(_DEBUG) || !defined(NDEBUG)
#	define DEBUG true
#else
#	define DEBUG false
#endif



#endif // CORE_PLATFORM
