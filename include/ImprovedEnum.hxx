#ifndef ENUM_UTILS_HXX
#define ENUM_UTILS_HXX

#include <array>
#include <iterator>
#include <tuple>
#include <type_traits>

#include <iostream>

#include <MacroUtils.hxx>
#include <StaticString.hxx>
#include <Range.hxx>

#define TRIM_ENUM_NAME

// Would be better to replace the specialization method by a thing like
// "is_in_list", and a typelist.

namespace Details
{   
    template<class T>
    class AssignmentRemover
    {
        public:
        constexpr AssignmentRemover(const T& obj) : innerObj_{obj}
        {}
        
        constexpr AssignmentRemover(T&& obj) : innerObj_{std::move(obj)}
        {}
        
        template<class U>
        constexpr const AssignmentRemover& operator=(const U&) const &
        {
            return *this;
        }
        
        template<class U>
        constexpr const AssignmentRemover& operator=(U&&) const &
        {
            return *this;
        }
        
        template<class U>
        constexpr AssignmentRemover&& operator=(const U&) &&
        {
            return std::move(*this);
        }
        
        template<class U>
        constexpr AssignmentRemover&& operator=(U&&) &&
        {
            return std::move(*this);
        }
        
        constexpr operator T() const &
        {
            return innerObj_;
        }
        
        constexpr operator T() &&
        {
            return std::move(innerObj_);
        }
        
        private:
        T innerObj_;
    };
}

#define SANE_LIST_BUILDER_HELPER(x, next) x IF(next)(ADD_COMMA)()
#define ENUM_ASSIGN_REMOVE(type) (const Details::AssignmentRemover<type::Internal##type>)type::SANE_LIST_BUILDER_HELPER
#define ENUM_UPWARD_CONV(type) (type) SANE_LIST_BUILDER_HELPER
#define BRACE_ENCLOSE_INIT(x, next) {x} IF(next)(ADD_COMMA)()

namespace Details
{
/* Usually, returning by const value is pretty useless, but here, it's used to indicate to the compiler that we want to call
 * the const version of our 'trim()' function later.
 * Without the qualifier, the compiler wants to call the other StaticString<TSize>:trim(), which is not constexpr, because of
 * the modifications inside the function.
 */
template<size_t Tsize>
constexpr const StaticString<Tsize> stringifyEnumInitializerHelper(ConstString str) noexcept
{
    return {str.begin(), str.find('=')};
}

template<size_t Tsize>
constexpr const StaticString<Tsize> stringifyTrimEnumInitializerHelper(ConstString str) noexcept
{
    return stringifyEnumInitializerHelper<Tsize>(str).trim();
}

}

#define STRINGIFY_ENUM_HELPER(string, stringType) stringType{STRINGIFY_ENUM_EQUAL_RANGE(string, stringType)}
#ifdef TRIM_ENUM_NAME
#define STRINGIZE_ENUM_ELEM_NAMES_FCT stringifyTrimEnumInitializerHelper
#else
#define STRINGIZE_ENUM_ELEM_NAMES_FCT stringifyEnumInitializerHelper
#endif
#define STRINGIFY_ENUM(x, next) Details::STRINGIZE_ENUM_ELEM_NAMES_FCT<ConstString{STRINGIFY(x)}.size()>(STRINGIFY(x)) IF(next)(ADD_COMMA)()
#define ENUM_NAME_TUPLE_DECL(x, next) StaticString<ConstString{STRINGIFY(x)}.size()> IF(next)(ADD_COMMA)()

namespace EnumUtils
{

/* Idea : could deducd the size by using the EnumName::tupleType size, instead of the VA_ARGS_COUNT macro.
 * Would reduce the time taken to build the enum, even if it's not really an issue right now.
 */

/* http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2015/p0031r0.html
 * Let's hope this got accepted in the next standard, this would cut a lot of verbose in this code.
 */

/* NOTE : The operator-> reintepret_cast is safe, as the class EnumName will contain only a EnumName::Internal##EnumName value anyway.
 * The other members are static constexpr variables, or member functions.
 */

enum class EnumIteratorTag : uint8_t
{
    Normal,
    Reversed
};

template<class EnumName, EnumIteratorTag tag>
class EnumIterator : public std::conditional_t<tag == EnumIteratorTag::Normal,
                                                typename ArrayIteratorPolicy<typename EnumName::ValuesArrayType>::const_iterator,
                                                typename ArrayIteratorPolicy<typename EnumName::ValuesArrayType>::const_reverse_iterator>
{
    public:
    using pointer = const EnumName*;
    using reference = const EnumName&;
    
    using Base = std::conditional_t<tag == EnumIteratorTag::Normal,
                                                typename ArrayIteratorPolicy<typename EnumName::ValuesArrayType>::const_iterator,
                                                typename ArrayIteratorPolicy<typename EnumName::ValuesArrayType>::const_reverse_iterator>;
                                                
    friend EnumName;
    
    protected:
    struct indexInitFlag{};
         
    constexpr EnumIterator(typename Base::IndexType index, indexInitFlag) : Base{EnumName::values(), index}
    {}
    
    public:
    using Base::Base;
    
    constexpr EnumIterator(EnumName e) : Base{EnumName::values(), e.get_index()}
    {}
    
    constexpr EnumIterator(const EnumIterator& other) = default;
    
    constexpr EnumIterator& operator=(const EnumIterator& other) = default;
    
    constexpr reference operator*() const
    {
        return reinterpret_cast<reference>(Base::operator*());
    }
    
    constexpr pointer operator->() const
    {
        return reinterpret_cast<pointer>(Base::operator->());
    }
};

}

#define ITERABLE_ENUM(EnumName, underlyingType, ...)                                                                            \
static_assert(std::is_integral<underlyingType>::value,                                                                          \
    "The defined underlying type is not an integral type");                                                                     \
class EnumName                                                                                                                  \
{                                                                                                                               \
    public:                                                                                                                     \
    enum Internal##EnumName : underlyingType{ __VA_ARGS__ };                                                                    \
                                                                                                                                \
    using iterator = EnumUtils::EnumIterator<EnumName, EnumUtils::EnumIteratorTag::Normal>;                                     \
    using const_iterator = iterator;                                                                                            \
    using reverse_iterator = EnumUtils::EnumIterator<EnumName, EnumUtils::EnumIteratorTag::Reversed>;                           \
    using const_reverse_iterator = reverse_iterator;                                                                            \
    using underlying_type = underlyingType;                                                                                     \
    using UnderlyingEnumType = Internal##EnumName;                                                                              \
    using TupleType = std::tuple<MAP2(ENUM_NAME_TUPLE_DECL, __VA_ARGS__)>;                                                      \
                                                                                                                                \
    public:                                                                                                                     \
    constexpr EnumName() noexcept : value_{values_[0]} {}                                                                       \
    constexpr EnumName(const EnumName& other) noexcept : value_{other.value_} {}                                                \
    constexpr EnumName(Internal##EnumName value) noexcept : value_{value} {}                                                    \
    constexpr EnumName operator=(const EnumName& other) noexcept { value_ = other.value_; return *this; }                       \
    constexpr EnumName operator=(Internal##EnumName value) noexcept { value_ = value; return *this; }                           \
                                                                                                                                \
    constexpr bool operator==(EnumName other) const noexcept { return value_ == other.value_; }                                 \
    constexpr bool operator!=(EnumName other) const noexcept { return !(*this == other); }                                      \
    constexpr bool operator<(EnumName other) const noexcept { return value_ < other.value_; }                                   \
    constexpr bool operator>=(EnumName other) const noexcept { return !(*this < other); }                                       \
    constexpr bool operator>(EnumName other) const noexcept { return value_ > other.value_; }                                   \
    constexpr bool operator<=(EnumName other) const noexcept { return !(*this > other); }                                       \
                                                                                                                                \
    constexpr underlyingType to_value() const noexcept                                                                          \
    {                                                                                                                           \
        return static_cast<underlyingType>(value_);                                                                             \
    }                                                                                                                           \
    constexpr explicit operator underlyingType() const noexcept                                                                 \
    {                                                                                                                           \
        return to_value();                                                                                                      \
    }                                                                                                                           \
    template<class T>                                                                                                           \
    static constexpr EnumName from_value(T val)                                                                                 \
    {                                                                                                                           \
        static_assert(std::is_convertible<T, underlying_type>::value,                                                           \
        "Construction from value require the value to be convertible to the underlying type");                                  \
                                                                                                                                \
        for(const auto value : values_)                                                                                         \
        {                                                                                                                       \
            if(value == val)                                                                                                    \
            {                                                                                                                   \
                return {static_cast<Internal##EnumName>(val)};                                                                  \
            }                                                                                                                   \
        }                                                                                                                       \
                                                                                                                                \
        CONSTEXPR_ASSERT(false, "The value to build from is invalid");                                                          \
    }                                                                                                                           \
	static constexpr bool is_contiguous() noexcept 																				\
	{ 																															\
		if(values_.size() == 0) return true; 																					\
		underlyingType last = values_[0];																						\
		for(auto it = values_.cbegin() + 1; it != values_.cend(); ++it, ++last)												    \
		{																														\
			if(*it != last + 1) return false;																					\
		}																														\
		return true; 																											\
	}																															\
                                                                                                                                \
    static constexpr size_t size() noexcept                                                                                     \
    {                                                                                                                           \
        return size_;                                                                                                           \
    }                                                                                                                           \
                                                                                                                                \
    private:                                                                                                                    \
    class IterableHelper                                                                                                        \
    {                                                                                                                           \
        public:                                                                                                                 \
        constexpr IterableHelper() : value_{values_[0]}{}                                                                       \
        constexpr IterableHelper(Internal##EnumName value) : value_{value}{}                                                    \
                                                                                                                                \
                                                                                                                                \
        constexpr EnumName::iterator begin() noexcept { return { value_ }; }                                                    \
        constexpr EnumName::const_iterator cbegin() noexcept { return begin(); }                                                \
        constexpr EnumName::iterator end() noexcept { return { size_, EnumName::iterator::indexInitFlag{} }; }                  \
        constexpr EnumName::const_iterator cend() noexcept { return end(); }                                                    \
                                                                                                                                \
        constexpr EnumName::reverse_iterator rbegin() noexcept { return end(); }                                                \
        constexpr EnumName::const_reverse_iterator crbegin() noexcept { return end(); }                                         \
        constexpr EnumName::reverse_iterator rend() noexcept { return begin(); }                                                \
        constexpr EnumName::const_reverse_iterator crend() noexcept { return begin(); }                                         \
                                                                                                                                \
        static constexpr EnumName::iterator from(EnumName e) noexcept { return { e }; }                                         \
        static constexpr EnumName::const_iterator cfrom(EnumName e) noexcept { return { e }; }                                  \
        static constexpr EnumName::reverse_iterator rfrom(EnumName e) noexcept { return { e }; }                                \
        static constexpr EnumName::const_reverse_iterator crfrom(EnumName e) noexcept { return { e }; }                         \
                                                                                                                                \
        private:                                                                                                                \
        Internal##EnumName value_;                                                                                              \
    };                                                                                                                          \
                                                                                                                                \
    public:                                                                                                                     \
    static constexpr IterableHelper iter() noexcept{ return {}; }                                                               \
    static constexpr IterableHelper iter_from(Internal##EnumName value) noexcept{ return {value}; }                             \
                                                                                                                                \
    constexpr size_t get_index() const noexcept                                                                                 \
    {                                                                                                                           \
        for(size_t i = 0; i < size_; ++i)                                                                                       \
        {                                                                                                                       \
            if(values_[i] == value_) return i;                                                                                  \
        }                                                                                                                       \
        return size_; /* Or error ? */                                                                                          \
    }                                                                                                                           \
                                                                                                                                \
    private: 	                                                                                                                \
    Internal##EnumName value_;                                                                                                  \
                                                                                                                                \
    static constexpr size_t size_ = std::tuple_size<TupleType>::value;                                                          \
	static constexpr const ConstString enum_name() noexcept																		\
	{																															\
   		return #EnumName;                                                                     		                            \
	}																															\
                                                                                                                                \
    public:                                                                                                                     \
    using ValuesArrayType = std::array<Internal##EnumName, size_>;                                                              \
                                                                                                                                \
    static constexpr const ValuesArrayType& values() noexcept { return values_; }                                               \
                                                                                                                                \
    private:                                                                                                                    \
    static constexpr ValuesArrayType values_{{MAP2(ENUM_ASSIGN_REMOVE(EnumName), __VA_ARGS__)}};                                \
};                                                                                                                              \

/* Declarations like :
 * IMPROVED_ENUM(MyEnum, size_t, Hello = 5);
 * will produce names like "Hello ".
 */

#define IMPROVED_ENUM(EnumName, underlyingType, ...)                                                                            \
namespace { using EnumName##TupleType = std::tuple<MAP2(ENUM_NAME_TUPLE_DECL, __VA_ARGS__)>;                                    \
static constexpr EnumName##TupleType EnumName##names_{MAP2(STRINGIFY_ENUM, __VA_ARGS__)};}                                      \
static_assert(std::is_integral<underlyingType>::value,                                                                          \
    "The defined underlying type is not an integral type");                                                                     \
class EnumName                                                                                                                  \
{                                                                                                                               \
    public:                                                                                                                     \
    enum Internal##EnumName : underlyingType{ __VA_ARGS__ };                                                                    \
                                                                                                                                \
    using iterator = EnumUtils::EnumIterator<EnumName, EnumUtils::EnumIteratorTag::Normal>;                                     \
    using const_iterator = iterator;                                                                                            \
    using reverse_iterator = EnumUtils::EnumIterator<EnumName, EnumUtils::EnumIteratorTag::Reversed>;                           \
    using const_reverse_iterator = reverse_iterator;                                                                            \
    using underlying_type = underlyingType;                                                                                     \
    using UnderlyingEnumType = Internal##EnumName;                                                                              \
                                                                                                                                \
    public:                                                                                                                     \
    constexpr EnumName() noexcept : value_{values_[0]} {}                                                                       \
    constexpr EnumName(const EnumName& other) noexcept : value_{other.value_} {}                                                \
    constexpr EnumName(Internal##EnumName value) noexcept : value_{value} {}                                                    \
    constexpr EnumName operator=(const EnumName& other) noexcept { value_ = other.value_; return *this; }                       \
    constexpr EnumName operator=(Internal##EnumName value) noexcept { value_ = value; return *this; }                           \
                                                                                                                                \
    constexpr bool operator==(EnumName other) const noexcept { return value_ == other.value_; }                                 \
    constexpr bool operator!=(EnumName other) const noexcept { return !(*this == other); }                                      \
    constexpr bool operator<(EnumName other) const noexcept { return value_ < other.value_; }                                   \
    constexpr bool operator>=(EnumName other) const noexcept { return !(*this < other); }                                       \
    constexpr bool operator>(EnumName other) const noexcept { return value_ > other.value_; }                                   \
    constexpr bool operator<=(EnumName other) const noexcept { return !(*this > other); }                                       \
                                                                                                                                \
    constexpr underlyingType to_value() const noexcept                                                                          \
    {                                                                                                                           \
        return static_cast<underlyingType>(value_);                                                                             \
    }                                                                                                                           \
    constexpr explicit operator underlyingType() const noexcept                                                                 \
    {                                                                                                                           \
        return to_value();                                                                                                      \
    }                                                                                                                           \
    template<class T>                                                                                                           \
    static constexpr EnumName from_value(T val) noexcept                                                                        \
    {                                                                                                                           \
        static_assert(std::is_convertible<T, underlying_type>::value,                                                           \
        "Construction from value require the value to be convertible to the underlying type");                                  \
                                                                                                                                \
        for(const auto value : values_)                                                                                         \
        {                                                                                                                       \
            if(value == static_cast<Internal##EnumName>(val))                                                                   \
            {                                                                                                                   \
                return {static_cast<Internal##EnumName>(val)};                                                                  \
            }                                                                                                                   \
        }                                                                                                                       \
                                                                                                                                \
        CONSTEXPR_ASSERT(false, "The value to build from is invalid");                                                          \
    }                                                                                                                           \
                                                                                                                                \
	static constexpr bool is_contiguous() noexcept		 																		\
	{ 																															\
		if(values_.size() == 0) return true; 																					\
		underlyingType last = values_[0];																						\
		for(auto it = values_.cbegin() + 1; it != values_.cend(); ++it, ++last)												    \
		{																														\
			if(*it != last + 1) return false;																					\
		}																														\
		return true; 																											\
	}																															\
																																\
    static constexpr size_t size() noexcept                                                                                     \
    {                                                                                                                           \
        return size_;                                                                                                           \
    }                                                                                                                           \
                                                                                                                                \
    private:                                                                                                                    \
    class IterableHelper                                                                                                        \
    {                                                                                                                           \
        public:                                                                                                                 \
        constexpr IterableHelper() : value_{values_[0]}{}                                                                       \
        constexpr IterableHelper(Internal##EnumName value) : value_{value}{}                                                    \
                                                                                                                                \
                                                                                                                                \
        constexpr EnumName::iterator begin() noexcept { return { value_ }; }                                                    \
        constexpr EnumName::const_iterator cbegin() noexcept { return begin(); }                                                \
        constexpr EnumName::iterator end() noexcept { return { size_, EnumName::iterator::indexInitFlag{} }; }                  \
        constexpr EnumName::const_iterator cend() noexcept { return end(); }                                                    \
                                                                                                                                \
        constexpr EnumName::reverse_iterator rbegin() noexcept { return end(); }                                                \
        constexpr EnumName::const_reverse_iterator crbegin() noexcept { return end(); }                                         \
        constexpr EnumName::reverse_iterator rend() noexcept { return begin(); }                                                \
        constexpr EnumName::const_reverse_iterator crend() noexcept { return begin(); }                                         \
                                                                                                                                \
        static constexpr EnumName::iterator from(EnumName e) noexcept { return { e }; }                                         \
        static constexpr EnumName::const_iterator cfrom(EnumName e) noexcept { return { e }; }                                  \
        static constexpr EnumName::reverse_iterator rfrom(EnumName e) noexcept { return { e }; }                                \
        static constexpr EnumName::const_reverse_iterator crfrom(EnumName e) noexcept { return { e }; }                         \
                                                                                                                                \
        private:                                                                                                                \
        Internal##EnumName value_;                                                                                              \
    };                                                                                                                          \
                                                                                                                                \
    public:                                                                                                                     \
    static constexpr IterableHelper iter() noexcept{ return {}; }                                                               \
    static constexpr IterableHelper iter_from(Internal##EnumName value) noexcept{ return {value}; }                             \
                                                                                                                                \
    constexpr size_t get_index() const noexcept                                                                                 \
    {                                                                                                                           \
        for(size_t i = 0; i < size_; ++i)                                                                                       \
        {                                                                                                                       \
            if(values_[i] == value_) return i;                                                                                  \
        }                                                                                                                       \
        return size_; /* Or error ? */                                                                                          \
    }                                                                                                                           \
                                                                                                                                \
                                                                                                                                \
                                                                                                                                \
    public:                                                                                                                     \
    constexpr ConstString to_string() const;                                                                                    \
    static constexpr ConstString get_enum_name() noexcept                                                                       \
    {                                                                                                                           \
        return #EnumName;                                                                     		                            \
    }                                                                                                                           \
                                                                                                                                \
    private:                                                                                                                    \
    Internal##EnumName value_;                                                                                                  \
    static constexpr size_t size_ = std::tuple_size<EnumName##TupleType>::value;                                                \
																														        \
    template<size_t index>                                                                                                      \
    struct Looper                                                                                                               \
    {                                                                                                                           \
        static constexpr ConstString to_string_impl(const EnumName e)                                                           \
        {                                                                                                                       \
            static_assert(index < e.size_, "Out of range !");                                                                   \
                                                                                                                                \
            return e.values_[index] == e.value_ ?                                                                               \
            ConstString{std::get<index>(EnumName##names_)}                                                                      \
            : Looper<index + 1>::to_string_impl(e);                                                                             \
        }                                                                                                                       \
    };    	                                                                                                                    \
                                                                                                                                \
    public:                                                                                                                     \
    using ValuesArrayType = std::array<Internal##EnumName, size_>;                                                              \
    static constexpr const ValuesArrayType& values() noexcept { return values_; }                                               \
                                                                                                                                \
    private:                                                                                                                    \
    static constexpr ValuesArrayType values_{{MAP2(ENUM_ASSIGN_REMOVE(EnumName), __VA_ARGS__)}};                             	\
};                                                                                                                              \
                                                                                                                                \
template<>                                                                                                                      \
struct EnumName::Looper<EnumName::size_>                                                                                        \
{                                                                                                                               \
    static constexpr ConstString to_string_impl(EnumName)                                                                       \
    {                                                                                                                           \
        return "";                                                                                                              \
    }                                                                                                                           \
};                                                                                                                              \
constexpr ConstString EnumName::to_string() const                                                                               \
{                                                                                                                               \
    return Looper<0>::to_string_impl(*this);                                                                                    \
}                                                                                                                               \
                                                               
#endif // ENUM_UTILS_HXX


