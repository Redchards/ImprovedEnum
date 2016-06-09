#ifndef CONST_STRING_HXX
#define CONST_STRING_HXX

#include <stddef.h>

#include <algorithm>
#include <cstddef>
#include <stdexcept>

#include <gsl/gsl_assert.h>

#include <ArrayIteratorPolicy.hxx>
#include <MetaUtils.hxx>
#include <Range.hxx>
#include <Platform.hxx>
#include <StringDetails.hxx>

// Workaround for clang to work with libstdc++
#if COMPILER == COMPILER_CLANG
using max_align_t = size_t;
#endif

class ConstString
{
	using IterPolicy = ArrayIteratorPolicy<ConstString>;
	
public:	
	using iterator = typename IterPolicy::const_iterator;
	using reverse_iterator = typename IterPolicy::const_reverse_iterator;
	using const_iterator = typename IterPolicy::const_iterator;
	using const_reverse_iterator = typename IterPolicy::const_reverse_iterator;
	
	using value_type = const char;
	using reference = const char&;
	using pointer = const char*;
	
public:
	// Serve for the sole purpose of begin able to be literal type even with default constructor
	ConstString() = default;
	constexpr ConstString(const ConstString& str) = default;

	template<size_t size>
	constexpr ConstString(const char(&cstr)[size]) noexcept : size_(size - 1), cstr_(cstr)
	{}
	
	template<class TString>
	constexpr ConstString(const TString& other) noexcept : size_(other.size()), cstr_(other.data())
	{}
	
	constexpr iterator begin() noexcept { return { *this, 0 }; }
	constexpr const_iterator begin() const noexcept { return { *this, 0 }; }
	constexpr const_iterator cbegin() const noexcept { return begin(); }
	constexpr iterator end() noexcept { return { *this, size() }; }
	constexpr const_iterator end() const noexcept { return { *this, size() }; }
	constexpr const_iterator cend() const noexcept { return end(); }

	constexpr reverse_iterator rbegin() noexcept { return end(); }
	constexpr const_reverse_iterator rbegin() const noexcept { return end(); }
	constexpr const_reverse_iterator crbegin() const noexcept { return end(); }
	constexpr reverse_iterator rend() noexcept { return begin(); }
	constexpr const_reverse_iterator rend() const noexcept { return begin(); }
	constexpr const_reverse_iterator crend() const noexcept { return begin(); }

	constexpr const char& at(size_t index) const
	{
		CONSTEXPR_ASSERT(index < size_, "Attempt to access a non-existing index of a constant string");
		return cstr_[index];
	}

	constexpr const char& operator[](size_t index) const
	{
		return at(index);
	}
	
	constexpr ConstString drop(size_t num) const
	{
		ConstString tmp{*this};
		tmp.cstr_ += num;
		tmp.size_ = size() - num;
		
		return (num < size() ? tmp : throw std::out_of_range("Attempt to access a non-existing index of a constant string"));
	}
	
	constexpr ConstString drop(const_iterator it) const
	{
		size_t interval = it - begin();
		ConstString tmp{*this};
		tmp.cstr_ += interval;
		tmp.size_ = end() - it;
		
		return (it >= begin() && it < end() ? tmp : throw std::out_of_range("Attempt to access a non-existing index of a constant string"));
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

	// Modified to build with visual studio. Gcc and clang do not need recursion, simple if branches and for loop are working.
	friend constexpr bool operator==(const ConstString& lhs, const ConstString& rhs);
	
	/* Alternative definition, working on GCC 5.2.0 and Clang 3.7
	It also avoid the recursion depth problem with big strings
	
	constexpr bool operator==(const ConstString& rhs) const
    {
        if(size_ != rhs.size()) return false;
        for(uint8_t i = 0; i < size_; ++i)
        {
            if((*this)[i] != rhs[i])
            {
                return false;
            }
        }
        return true;
    }
	*/
	constexpr size_t size() const noexcept { return size_; }
	constexpr pointer data() const noexcept { return cstr_; }
	
	// With this operator, the class would be named ConstStringRef
	/*ConstString operator=(const ConstString& other) noexcept
	{
		cstr_ = other.cstr_;
		return *this;
	}*/

	constexpr operator const char*() const noexcept
	{
		return data();
	}

private:
	size_t size_;
	
protected:
	pointer cstr_;
};

constexpr bool operator==(const ConstString& lhs, const ConstString& rhs)
{
	return lhs.size() != rhs.size() ? false : Details::equalAux(lhs.size(), lhs, rhs);
}

#endif // CONST_STRING_HXX