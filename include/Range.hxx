// #include <Meta.hxx>

// A very small range class, adapted from Eric' Niebler work.

#ifndef RANGE_HXX
#define RANGE_HXX

#include <iterator>

#include <ConstexprAssert.hxx>
#include <MetaUtils.hxx>

namespace Details
{
    template<class Iterator,
             std::enable_if_t<std::is_same<typename std::iterator_traits<Iterator>::iterator_category,
                              std::random_access_iterator_tag>::value>* = nullptr>
    constexpr auto difference(Iterator first, Iterator last)
    -> typename std::iterator_traits<Iterator>::difference_type
    {
        return last - first;
    }
    
    template<class Iterator,
             std::enable_if_t<!std::is_same<typename std::iterator_traits<Iterator>::iterator_category,
                              std::random_access_iterator_tag>::value>* = nullptr>
    constexpr auto difference(Iterator first, Iterator last)
    -> typename std::iterator_traits<Iterator>::difference_type
    {
        typename std::iterator_traits<Iterator>::difference_type i = 0;
        for(auto it = first; it != last; ++it, ++i);
        return i;
    }

template<class T>
static constexpr bool is_empty_and_trivial = std::is_empty<T>::value && std::is_trivial<T>::value;

template<class T>
using empty_and_trivial_enabler = std::enable_if_t<std::is_empty<T>::value && std::is_trivial<T>::value>;

template<class T>
struct basic_pair_attribute
{
    using value_type = T;
    
    basic_pair_attribute() = default;
    
    template<class Arg>
    constexpr explicit basic_pair_attribute(Arg&&)
    {
        static_assert(std::is_constructible<value_type, Arg>::value,
                      "Type can't be constructed from the passed argument types");
    }
};

template<class T, class = void>
struct first_pair_attribute : public basic_pair_attribute<T>
{
    T first;
    
    first_pair_attribute() = default;
    
    template<class Arg>
    constexpr explicit first_pair_attribute(Arg&& arg)
    : base(std::forward<Arg>(arg)), first(std::forward<Arg>(arg))
    {}
    
private:
    using base = basic_pair_attribute<T>;
};

template<class T>
struct first_pair_attribute<T,
                            Meta::void_t<empty_and_trivial_enabler<T>>
                           > : public basic_pair_attribute<T>
{
    static T first;  
};

template<class T>
T first_pair_attribute<T, Meta::void_t<empty_and_trivial_enabler<T>>>::first{};

template<class T, class = void>
struct last_pair_attribute : public basic_pair_attribute<T>
{
    T last;
    
    last_pair_attribute() = default;
    
    template<class Arg>
    constexpr explicit last_pair_attribute(Arg&& arg)
    : base(std::forward<Arg>(arg)), last(std::forward<Arg>(arg))
    {}
   
private:
    using base = basic_pair_attribute<T>;
};

template<class T>
struct last_pair_attribute<T,
                           Meta::void_t<empty_and_trivial_enabler<T>>
                           > : public basic_pair_attribute<T>
{
    static T last;
};

template<class T>
T last_pair_attribute<T, Meta::void_t<empty_and_trivial_enabler<T>>>::last{};

template<class First, class Last>
struct reduced_pair
: public first_pair_attribute<First>,
  public last_pair_attribute<Last>
{
    using first_type = First;
    using last_type = Last;
    using first_base = first_pair_attribute<first_type>;
    using last_base = last_pair_attribute<last_type>;
    
    template<class Arg1, class Arg2>
    constexpr explicit reduced_pair(Arg1&& t, Arg2&& u) 
    : first_base(std::forward<Arg1>(t)), last_base(std::forward<Arg2>(u))
    {}
    
    template<class T1, class Arg2>
    constexpr explicit reduced_pair(std::initializer_list<T1> list, Arg2&& u) 
    : first_base(list), last_base(std::forward<Arg2>(u))
    {}
    
    template<class Arg1, class T2>
    constexpr explicit reduced_pair(Arg1&& t, std::initializer_list<T2> list)
    : first_base(std::forward<Arg1>(t)), last_base(list)
    {}
    
    template<class T1, class T2>
    constexpr explicit reduced_pair(std::initializer_list<T1> list1, std::initializer_list<T2> list2)
    : first_base(list1), last_base(list2)
    {}
    
    constexpr operator std::pair<First, Last>()
    {
        return {this->first, this->last};
    }
};

}

template<class Iterator>
struct range : private Details::reduced_pair<Iterator, Iterator>
{
    using iterator = Iterator;
    using base = Details::reduced_pair<Iterator, Iterator>;
    
    using Details::reduced_pair<Iterator, Iterator>::first;
    using Details::reduced_pair<Iterator, Iterator>::last;
    
    using value_type = typename std::iterator_traits<Iterator>::value_type;
    using difference_type = typename std::iterator_traits<Iterator>::difference_type;
    using reference = typename std::iterator_traits<Iterator>::reference;
    using pointer = typename std::iterator_traits<Iterator>::pointer;

    range() = default;
    
    template<class OtherIterator,
             typename = std::enable_if_t<std::is_convertible<OtherIterator, Iterator>::value>>
    constexpr range(OtherIterator first, OtherIterator last)
    : base(std::move(first), std::move(last))
    {
        CONSTEXPR_ASSERT(Details::difference(begin(), end()), "The begining of the range is past after the end of the range");
    }
    
    template<class OtherIterator,
             typename = std::enable_if_t<std::is_convertible<OtherIterator, Iterator>::value>>
    constexpr range(range<OtherIterator> rg)
    : range(rg.first, rg.last)
    {
        CONSTEXPR_ASSERT(Details::difference(begin(), end()), "The begining of the range is past after the end of the range");
    }
    
    template<class OtherIterator1, class OtherIterator2,
             typename = std::enable_if_t<std::is_convertible<OtherIterator1, Iterator>::value && std::is_convertible<OtherIterator2, Iterator>::value>>
    constexpr range(std::pair<OtherIterator1, OtherIterator2> rg)
    : range(rg.first, rg.last)
    {
        CONSTEXPR_ASSERT(Details::difference(begin(), end()), "The begining of the range is past after the end of the range");
    }
    
    constexpr reference front() const
    {
        CONSTEXPR_ASSERT(!empty(), "Attempted to take the first element of an empty range !");
        
        return *begin();
    }
    
    constexpr reference back() const
    {
        CONSTEXPR_ASSERT(!empty(), "Attempted to take the last element of an empty range !");
        
        Iterator tmp = end();
        --tmp;
        return *tmp;
    }
    
    constexpr bool empty() const
    {
        return size() == 0;
    }

    constexpr size_t size() const
    {
        return Details::difference(begin(), end());
    }
    
    constexpr void pop_front()
    {
        CONSTEXPR_ASSERT(!empty(), "Attempted to pop on an empty range !");    
    
        ++first;
    }
    
    constexpr void pop_back()
    {
        CONSTEXPR_ASSERT(!empty(), "Attempted to pop on an empty range !");
        
        --last;
    }
    
    constexpr iterator begin() const noexcept
    {
        return first;
    }
    
    constexpr iterator end() const noexcept
    {
        return last;
    }
};

#endif // RANGE_HXX