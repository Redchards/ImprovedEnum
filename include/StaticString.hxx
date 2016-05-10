#ifndef STATIC_STRING_HXX
#define STATIC_STRING_HXX

#include <ArrayIteratorPolicy.hxx>
#include <ConstexprAssert.hxx>
#include <StringDetails.hxx>

// TODO : Allow range of different StaticString<size> (right know, only the same Tsize is allowed)

template<size_t Tsize>
class StaticString
{
	using IterPolicy = ArrayIteratorPolicy<StaticString<Tsize>>;
	
public:
	using iterator = typename IterPolicy::iterator;
	using reverse_iterator = typename IterPolicy::reverse_iterator;
	using const_iterator = typename IterPolicy::const_iterator;
	using const_reverse_iterator = typename IterPolicy::const_reverse_iterator;
	using unchecked_iterator = typename IterPolicy::unchecked_iterator;
	using unchecked_const_iterator = typename IterPolicy::unchecked_const_iterator;
	using unchecked_reverse_iterator = typename IterPolicy::unchecked_reverse_iterator;
	using unchecked_const_reverse_iterator = typename IterPolicy::unchecked_const_reverse_iterator;
	
	using value_type = const char;
	using reference = const char&;
	using pointer = const char*;
	
public:
	// Serve for the sole purpose of begin able to be literal type even with default constructor
	constexpr StaticString() : str_{}
	{}
	template<size_t otherSize>
	constexpr StaticString(const StaticString<otherSize>& other) : str_{other.str_}, actualSize_{otherSize}
	{
		static_assert(otherSize <= Tsize, "The string used to initialize the StaticString do not fit !");
	}
	template<size_t otherSize>
	constexpr StaticString(const char(&cstr)[otherSize]) : str_{initStr<otherSize - 1>(ConstString{cstr})}, actualSize_{otherSize - 1}
	{
		static_assert(otherSize <= (Tsize + 1), "The string used to initialize the StaticString do not fit !");
	}

	// TODO : Make constexpr assert !!!
	template<class Iterator>
	constexpr StaticString(range<Iterator> range) : str_{initStrRange<Iterator, Tsize>(range)}, actualSize_{range.size()}
	{
		CONSTEXPR_ASSERT(range.size() <= Tsize, "Range do not fit in the StaticString !");
	}
	
	constexpr iterator begin() noexcept { return{ *this, 0 }; }
	constexpr const_iterator begin() const noexcept { return{ *this, 0 }; }
	constexpr const_iterator cbegin() const noexcept { return begin(); }
	constexpr iterator end() noexcept { return{ *this, size() }; }
	constexpr const_iterator end() const noexcept { return{ *this, size() }; }
	constexpr const_iterator cend() const noexcept { return end(); }

	constexpr reverse_iterator rbegin() noexcept { return end(); }
	constexpr const_reverse_iterator rbegin() const noexcept { return end(); }
	constexpr const_reverse_iterator crbegin() const noexcept { return end(); }
	constexpr reverse_iterator rend() noexcept { return begin(); }
	constexpr const_reverse_iterator rend() const noexcept { return begin(); }
	constexpr const_reverse_iterator crend() const noexcept { return begin(); }

	constexpr char at(size_t index) const
	{
		CONSTEXPR_ASSERT(index < actualSize_, "Attempt to access a non-existing index of a StaticString");
		return str_[index];
		//return (index < actualSize_ ? str_[index] : throw std::out_of_range("Attempt to access a non-existing index of a StaticString"));
	}

	constexpr char operator[](size_t index) const
	{
		return at(index);
		//return (index < actualSize_ ? str_[index] : throw std::out_of_range("Attempt to access a non-existing index of a StaticString"));
	}

	constexpr const char* data() const
	{
		return &str_[0];
	}
	
	constexpr char* data()
	{
		return &str_[0];
	}
	
	constexpr auto find(char c) const noexcept
	-> decltype(begin())
	{
		for(auto it = begin(); it != end(); ++it)
		{
			if(*it == c)
			{
				return it; 
			}
		}
		return end();
	}

	constexpr operator const char*() const noexcept
	{
		return data();
	}
	
	template<size_t TSize1, size_t TSize2>
	friend constexpr bool operator==(StaticString<TSize1>& lhs, StaticString<TSize2> rhs);
	
	/* Alternative definition, working on GCC 5.2.0 and Clang 3.6
	
	constexpr bool operator==(StaticString<Tsize> rhs) const
    {
    	for(uint8_t i = 0; i < size(); ++i)
    	{
    		if((*this)[i] != rhs[i])
    		{
    			return false;
    		}
    	}
    	return true;
    }
	*/
    
    /*template<size_t otherSize>
    constexpr bool operator==(StaticString<otherSize>) const
    {
    	return false;
    }*/

	constexpr size_t size() const noexcept { return actualSize_; }

private:
	template<class Iterator>
	class RangeInitializationHelper
	{
		public:
		constexpr RangeInitializationHelper(const range<Iterator> range) noexcept : range_{range}
		{}
		constexpr char operator[](size_t index) const noexcept
		{
			return (range_.begin() + index) < range_.end() ? *(range_.begin() + index) : '\0';
		}
		
		private:
		const range<Iterator> range_;	
	};

	template<size_t size>
	constexpr std::array<char, Tsize + 1> initStr(ConstString cstr) const noexcept
	{
		return initStrAux(cstr, std::make_integer_sequence<size_t, size>{});
	}
	
	template<class Iterator, size_t size>
	constexpr std::array<char, Tsize + 1> initStrRange(const range<Iterator> range) const noexcept
	{
		using IteratorType = std::conditional_t<Meta::is_iterator_of<StaticString, Iterator>::value, unchecked_const_iterator, Iterator>;
		return initStrRangeAux(RangeInitializationHelper<IteratorType>{range}, std::make_integer_sequence<size_t, size>{});
	}
	
	template<size_t ... indices>
	constexpr std::array<char, Tsize + 1> initStrAux(ConstString cstr, std::integer_sequence<size_t, indices...>) const noexcept
	{
		return {{cstr[indices]...}};
	}
	
	template<class Iterator, size_t ... indices>
	constexpr std::array<char, Tsize + 1> initStrRangeAux(RangeInitializationHelper<Iterator> helper, std::integer_sequence<size_t, indices...>) const noexcept
	{
		return {{helper[indices]...}};
	}

	std::array<char, Tsize + 1> str_;
	size_t actualSize_;
};

template<size_t TSize1, size_t TSize2>
constexpr bool operator==(StaticString<TSize1>& lhs, StaticString<TSize2> rhs)
{
	return lhs.size() != rhs.size() ? false : Details::equalAux(lhs.size(), lhs, rhs);
}

#endif // STATIC_STRING_HXX