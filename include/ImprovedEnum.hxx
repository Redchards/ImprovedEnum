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

#define SANE_LIST_BUILDER_HELPER(x, next) x IF(next)(ADD_COMMA, EMPTY_ACTION)()
#define ENUM_ASSIGN_REMOVE(type) (const Details::AssignmentRemover<type::Internal##type>)type::SANE_LIST_BUILDER_HELPER
#define ENUM_UPWARD_CONV(type) (type) SANE_LIST_BUILDER_HELPER
#define BRACE_ENCLOSE_INIT(x, next) {x} IF(next)(ADD_COMMA, EMPTY_ACTION)()

namespace Details
{
template<size_t Tsize>
constexpr StaticString<Tsize> stringifyEnumInitializerHelper(ConstString str) noexcept
{
    return { range<ConstString::const_iterator>{str.begin(), str.find('=')} };
}
}

#define STRINGIFY_ENUM_EQUAL_RANGE(string, stringType) range<stringType::const_iterator>{stringType{string}.begin(), stringType{string}.find('=')}
#define STRINGIFY_ENUM_HELPER(string, stringType) stringType{STRINGIFY_ENUM_EQUAL_RANGE(string, stringType)}
#define STRINGIFY_ENUM(x, next) Details::stringifyEnumInitializerHelper<ConstString{STRINGIFY(x)}.size()>(STRINGIFY(x)) IF(next)(ADD_COMMA, EMPTY_ACTION)()
//#define STRINGIFY_ENUM(x, next) STRINGIFY(x) IF(next)(ADD_COMMA, EMPTY_ACTION)()
#define ENUM_NAME_TUPLE_DECL(x, next) StaticString<ConstString{STRINGIFY(x)}.size()> IF(next)(ADD_COMMA, EMPTY_ACTION)()

namespace EnumUtils
{

// Idea : could deducd the size by using the EnumName::tupleType size, instead of the VA_ARGS_COUNT macro.
// Would reduce the time taken to build the enum, even if it's not really an issue right now.

// Very verbose iterator implementation. Unfortunatly, this is cause by the lack of constexpr qualifier in the implementation
// of the std::reverse_iterator. Fortunatly this time, this may change, as per the following proposal :
// http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2015/p0031r0.html
// Let's hope this got accepted in the next standard, this would cut a lot of verbose in this code.

// NOTE : The operator-> reintepret_cast is safe, as the class EnumName will contain only a EnumName::Internal##EnumName value anyway.
// The other members are static constexpr variables, or member functions.

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
         
    constexpr EnumIterator(typename Base::IndexType index, indexInitFlag) : Base{EnumName::values, index}
    {}
    
    public:
    using Base::Base;
    
    constexpr EnumIterator(EnumName e) : Base{EnumName::values, e.getIndex()}
    {}
    
    constexpr EnumIterator(const EnumIterator& other) = default;
    
    constexpr EnumIterator& operator=(const EnumIterator& other) = default;
    
    constexpr reference operator*() const
    {
        return *reinterpret_cast<pointer>(Base::operator->());
    }
    
    constexpr pointer operator->() const
    {
        return reinterpret_cast<pointer>(Base::operator->());
    }
};

}

#define iterable_ENUM(EnumName, underlyingType, ...)                                                                          \
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
    using TupleType = std::tuple<MAP2(ENUM_NAME_TUPLE_DECL, __VA_ARGS__)>;                                                      \
                                                                                                                                \
    public:                                                                                                                     \
    constexpr EnumName(Internal##EnumName value) noexcept : value_{value} {}                                                    \
    constexpr EnumName(const EnumName& other) noexcept : value_{other.value_} {}                                                \
    constexpr EnumName operator=(Internal##EnumName value) noexcept { value_ = value; return *this; }                           \
    constexpr EnumName operator=(const EnumName& other) noexcept { value_ = other.value_; return *this; }                       \
                                                                                                                                \
    constexpr underlyingType toUnderlying() const noexcept                                                                      \
    {                                                                                                                           \
        return static_cast<underlyingType>(value_);                                                                             \
    }                                                                                                                           \
    constexpr explicit operator underlyingType() const noexcept                                                                 \
    {                                                                                                                           \
        return toUnderlying();                                                                                                  \
    }                                                                                                                           \
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
        constexpr IterableHelper() : value_{values[0]}{}                                                                        \
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
    static constexpr IterableHelper iterable() noexcept{ return {}; }                                                           \
    static constexpr IterableHelper iterableFrom(Internal##EnumName value) noexcept{ return {value}; }                          \
                                                                                                                                \
    constexpr size_t getIndex() const noexcept                                                                                  \
    {                                                                                                                           \
        for(size_t i = 0; i < size_; ++i)                                                                                       \
        {                                                                                                                       \
            if(values[i] == value_) return i;                                                                                   \
        }                                                                                                                       \
        return size_; /* Or error ? */                                                                                          \
    }                                                                                                                           \
                                                                                                                                \
    constexpr ConstString getEnumName() const noexcept                                                                          \
    {                                                                                                                           \
        return enumName;                                                                                                        \
    }                                                                                                                           \
                                                                                                                                \
    protected:                                                                                                                  \
    Internal##EnumName value_;                                                                                                  \
                                                                                                                                \
    static constexpr size_t size_ = std::tuple_size<TupleType>::value;                                                          \
    static constexpr ConstString enumName = #EnumName;                                                                          \
                                                                                                                                \
    public:                                                                                                                     \
    using ValuesArrayType = std::array<Internal##EnumName, size_>;                                                              \
    static constexpr ValuesArrayType values{{MAP2(ENUM_ASSIGN_REMOVE(EnumName), __VA_ARGS__)}};                                 \
};                                                                                                                              \
constexpr ConstString EnumName::enumName;                                                                                       \
constexpr EnumName::ValuesArrayType EnumName::values

/* Declarations like :
 * IMPROVED_ENUM(MyEnum, size_t, Hello = 5);
 * will produce names like "Hello ", with a space at the end. Should trim the name strings ?
 * Or is it miscallenaous and do not matter ?
 */

#define IMPROVED_ENUM(EnumName, underlyingType, ...)                                                                            \
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
    using TupleType = std::tuple<MAP2(ENUM_NAME_TUPLE_DECL, __VA_ARGS__)>;                                                      \
                                                                                                                                \
    public:                                                                                                                     \
    constexpr EnumName(Internal##EnumName value) noexcept : value_{value} {}                                                    \
    constexpr EnumName(const EnumName& other) noexcept : value_{other.value_} {}                                                \
    constexpr EnumName operator=(Internal##EnumName value) noexcept { value_ = value; return *this; }                           \
    constexpr EnumName operator=(const EnumName& other) noexcept { value_ = other.value_; return *this; }                       \
                                                                                                                                \
    constexpr underlyingType toUnderlying() const noexcept                                                                      \
    {                                                                                                                           \
        return static_cast<underlyingType>(value_);                                                                             \
    }                                                                                                                           \
    constexpr explicit operator underlyingType() const noexcept                                                                 \
    {                                                                                                                           \
        return toUnderlying();                                                                                                  \
    }                                                                                                                           \
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
        constexpr IterableHelper() : value_{values[0]}{}                                                                        \
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
    static constexpr IterableHelper iterable() noexcept{ return {}; }                                                           \
    static constexpr IterableHelper iterableFrom(Internal##EnumName value) noexcept{ return {value}; }                          \
                                                                                                                                \
    constexpr size_t getIndex() const noexcept                                                                                  \
    {                                                                                                                           \
        for(size_t i = 0; i < size_; ++i)                                                                                       \
        {                                                                                                                       \
            if(values[i] == value_) return i;                                                                                   \
        }                                                                                                                       \
        return size_; /* Or error ? */                                                                                          \
    }                                                                                                                           \
                                                                                                                                \
    template<size_t index>                                                                                                      \
    struct Looper                                                                                                               \
    {                                                                                                                           \
        static constexpr const char* toStringImpl(EnumName e)                                                                   \
        {                                                                                                                       \
            static_assert(index < e.size_, "Out of range !");                                                                   \
            return e.values[index] == e.value_ ? std::get<index>(e.names_) : Looper<index + 1>::toStringImpl(e);                \
        }                                                                                                                       \
    };                                                                                                                          \
    constexpr const char* toString() const;                                                                                     \
    constexpr ConstString getEnumName() const noexcept                                                                          \
    {                                                                                                                           \
        return enumName;                                                                                                        \
    }                                                                                                                           \
                                                                                                                                \
    private:                                                                                                                    \
    Internal##EnumName value_;                                                                                                  \
                                                                                                                                \
    static constexpr size_t size_ = std::tuple_size<TupleType>::value;                                                          \
    static constexpr TupleType names_{MAP2(STRINGIFY_ENUM, __VA_ARGS__)};                                                       \
    static constexpr ConstString enumName = #EnumName;                                                                          \
                                                                                                                                \
    public:                                                                                                                     \
    using ValuesArrayType = std::array<Internal##EnumName, size_>;                                                              \
    static constexpr ValuesArrayType values{{MAP2(ENUM_ASSIGN_REMOVE(EnumName), __VA_ARGS__)}};                                 \
};                                                                                                                              \
                                                                                                                                \
template<>                                                                                                                      \
struct EnumName::Looper<EnumName::size_>                                                                                        \
{                                                                                                                               \
    static constexpr const char* toStringImpl(EnumName)                                                                         \
    {                                                                                                                           \
        return nullptr;                                                                                                         \
    }                                                                                                                           \
};                                                                                                                              \
constexpr const char* EnumName::toString() const                                                                                \
{                                                                                                                               \
    return Looper<0>::toStringImpl(*this);                                                                                      \
}                                                                                                                               \
                                                                                                                                \
constexpr EnumName::ValuesArrayType EnumName::values;                                                                           \
constexpr ConstString EnumName::enumName;                                                                                       \
constexpr EnumName::TupleType EnumName::names_
                                                               
#endif // ENUM_UTILS_HXX


