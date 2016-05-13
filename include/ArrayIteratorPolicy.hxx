#ifndef ARRAY_ITERATOR_POLICY_HXX
#define ARRAY_ITERATOR_POLICY_HXX

#include <cstddef>

#include <ConstexprAssert.hxx>
#include <Platform.hxx>

/* Define an helper class to ease defining array style class iterators.
 * To take full advantage of this code, the array-like class should have an overloaded operator[],
 * and a data() function, in order to allow forced unchecked iterator, which may be useful when using
 * constexpr.
 */

/* There's a warning about "multiple copy constructor specified", due to partial visual studio
 * SFINAE implementation.
 * We can safely silence them out, as it should not be like this.
 * The warning is due to the fact that a non-const iterator can be copied to a const iterator, but not
 * the other way. So, a non-const iterator has a copy constructor AND a constructor accepting a const
 * iterator argument. But for const iterator class, this constructor is disabled.
 * It does work as expected in clang and gcc, but not un visual studio yet. At least, this is only
 * a warning and not an error (constructor implementations are the same anyway).
 */

// TODO : Reverse dependency by inheritance of unchecked an possibly checked iterators ?

#if(COMPILER == MVSC_COMPILER)
#pragma warning(push)
#pragma warning(disable : 4521)
#endif
template<class ArrayClass>
class ArrayIteratorPolicy
{
protected:
	template<bool constFlag>
	class BaseIterator : public std::iterator<std::random_access_iterator_tag, typename ArrayClass::value_type>
	{
		friend BaseIterator<!constFlag>;
		//friend ArrayClass;
	protected:
		using IndexType = size_t;
		using ArrayClassRef = typename std::conditional<constFlag, const typename std::remove_cv<ArrayClass>::type&, typename std::remove_cv<ArrayClass>::type&>::type;
	public:
		using value_type = typename ArrayClass::value_type;
		using reference = typename std::conditional<constFlag, const typename std::remove_cv<value_type>::type&, value_type&>::type;
		using pointer = typename std::conditional<constFlag, const typename std::remove_cv<value_type>::type*, value_type*>::type;
		using difference = ptrdiff_t;
		
	protected:
		constexpr BaseIterator(ArrayClassRef rhs, IndexType index) : it_(rhs), index_(index) {}

	public:
		constexpr BaseIterator() = delete;
		constexpr BaseIterator(const BaseIterator& other) : it_(other.it_),
															index_(other.index_)
		{}
		template<bool TconstFlag = constFlag,
				 typename std::enable_if<TconstFlag == true, bool>::type = false>
		constexpr BaseIterator(const BaseIterator<false>& other) : it_(other.it_),
																	   index_(other.index_)
		{}
		
		constexpr BaseIterator& operator=(const BaseIterator& other)
		{
			CONSTEXPR_ASSERT(hasSameArray(other), "The other iterator do not iterate on the same array as this iterator");
			
			index_ = other.index_;
			
			return *this;
		}

		template<bool TconstFlag>
		constexpr bool operator==(const BaseIterator<TconstFlag>& rhs) const noexcept { return (hasSameIndex(rhs) && hasSameArray(rhs)); }
		template<bool TconstFlag>
		constexpr bool operator!=(const BaseIterator<TconstFlag>& rhs) const noexcept { return !(*this == rhs); }

		constexpr operator BaseIterator<true>() const noexcept
		{
			return{ it_, index_ };
		}
		
		template<bool otherConstFlag>
		constexpr bool hasSmallerIndex(const BaseIterator<otherConstFlag>& other) const noexcept
		{
			return index_ < other.index_;
		}
		
		template<bool otherConstFlag>
		constexpr bool hasGreaterIndex(const BaseIterator<otherConstFlag>& other) const noexcept
		{
			return index_ > other.index_;
		}
		
		template<bool otherConstFlag>
		constexpr bool hasSameIndex(const BaseIterator<otherConstFlag>& other) const noexcept
		{
			return index_ == other.index_;
		}
		
		template<bool otherConstFlag>
		constexpr bool hasSameArray(const BaseIterator<otherConstFlag>& other) const noexcept
		{
			return &it_ == &other.it_;
		}
		
	protected:
		ArrayClassRef it_;
		IndexType index_;
	};

	template<bool constFlag>
	class Iterator : public BaseIterator<constFlag>
	{
		friend Iterator<!constFlag>;
		
		protected:
		using Base = BaseIterator<constFlag>;
		using IndexType = typename Base::IndexType;

		public:
		using value_type = typename Base::value_type;
		using reference = typename Base::reference;
		using pointer = typename Base::pointer;
		using difference = typename Base::difference;
		
		private:
		friend ArrayClass;
		using Base::Base;

		public:
		// The first contructor should be protected maybe.
		constexpr Iterator(const Base& other) : Base(other)
		{}
		
		constexpr Iterator& operator=(const Iterator&) = default;
		
		template<bool otherConstFlag>
		constexpr difference operator-(const Iterator<otherConstFlag>& rhs) const noexcept
		{
			return this->index_ - rhs.index_;
		}
		
		constexpr Iterator& operator++() noexcept
		{
			++this->index_;
			return *this;
		}

		constexpr Iterator operator++(int) noexcept
		{
			Iterator tmp{ *this };
			++this->index_;
			return tmp;
		}

		constexpr Iterator& operator--() noexcept
		{
			--this->index_;
			return *this;
		}

		constexpr Iterator operator--(int) noexcept
		{
			Iterator tmp{ *this };
			--this->index_;
			return tmp;
		}
		
		constexpr Iterator operator+(size_t n) const noexcept
		{
			Iterator tmp{ this->it_, this->index_ + n};
			return tmp;
		}
		
		constexpr Iterator operator-(size_t n) const noexcept
		{
			Iterator tmp{ this->it_, this->index_ - n};
			return tmp;
		}
		
		constexpr Iterator& operator+=(size_t n)
		{
			*this = *this + n;
			return *this;
		}
		
		constexpr Iterator& operator-=(size_t n)
		{
			*this = *this - n;
			return *this;
		}
		
		constexpr auto& operator*()
		{ 
			return this->it_[this->index_]; 
		}
		constexpr auto operator->()
		{ 
			return &(this->it_[this->index_]); 
		}
		
		constexpr auto& operator*() const 
		{ 
			return this->it_[this->index_]; 
		}
		constexpr auto operator->() const 
		{ 
			return &(this->it_[this->index_]); 
		}
		
		template<bool otherConstFlag> 
		constexpr bool operator<(const Iterator<otherConstFlag>& rhs) const noexcept
		{
			return this->hasSameArray(rhs) 
			    && this->hasSmallerIndex(rhs);
		}
		
		template<bool otherConstFlag> 
		constexpr bool operator<=(const Iterator<otherConstFlag>& rhs) const noexcept
		{
			return this->hasSameArray(rhs) 
				&& !this->hasGreaterIndex(rhs);
		}
		
		template<bool otherConstFlag> 
		constexpr bool operator>(const Iterator<otherConstFlag>& rhs) const noexcept
		{
			return this->hasSameArray(rhs) 
				&& this->hasGreaterIndex(rhs);
		}
		
		template<bool otherConstFlag> 
		constexpr bool operator>=(const Iterator<otherConstFlag>& rhs) noexcept
		{
			return this->hasSameArray(rhs) 
				&& !this->hasSmallerIndex(rhs);
		}
	};

	template<bool constFlag>
	class ReverseIterator : public BaseIterator<constFlag>
	{
		friend ReverseIterator<!constFlag>;
		
		protected:
		using Base = BaseIterator<constFlag>;
		using IndexType = typename Base::IndexType;

		public:
		using value_type = typename Base::value_type;
		using reference = typename Base::reference;
		using pointer = typename Base::pointer;
		using difference = typename Base::difference;
		
		private:
		friend ArrayClass;
		using Base::Base;

		public:
		// Right now, this constructor do not work (visual studio 2015 update 1). I guess it's again the bad implementation of templates
		// and two pass lookup that do not play in our favor
		//constexpr ReverseIterator(const ArrayClass& rhs, IndexType index) noexcept : Base(rhs, index) {}
		//using Base::Base;
		
		constexpr ReverseIterator(const Base& other) noexcept : Base(other)
		{}
		
		constexpr ReverseIterator& operator=(const ReverseIterator&) = default;
		
		template<bool otherConstFlag>
		constexpr difference operator-(const ReverseIterator<otherConstFlag>& rhs) const noexcept
		{
			return this->index_ - rhs.index_;
		}
		
		constexpr ReverseIterator& operator++() noexcept
		{
			--this->index_;
			return *this;
		}

		constexpr ReverseIterator operator++(int) noexcept
		{
			ReverseIterator tmp{ *this };
			--this->index_;
			return tmp;
		}

		constexpr ReverseIterator& operator--() noexcept
		{
			++this->index_;
			return *this;
		}

		constexpr ReverseIterator operator--(int) noexcept
		{
			ReverseIterator tmp{ *this };
			++this->index_;
			return tmp;
		}
		
		constexpr ReverseIterator operator+(size_t n) const noexcept
		{
			ReverseIterator tmp{ this->it_, this->index_ - n };
			return tmp;
		}
		
		constexpr ReverseIterator operator-(size_t n) const noexcept
		{
			ReverseIterator tmp{ this->it_, this->index_ + n };
			return tmp;
		}
		
		constexpr ReverseIterator& operator+=(size_t n)
		{
			*this = *this - n;
			return *this;
		}
		
		constexpr ReverseIterator& operator-=(size_t n)
		{
			*this = *this + n;
			return *this;
		}
		
		constexpr auto& operator*()
		{ 
			return this->it_[this->index_ - 1]; 
		}
		constexpr auto operator->()
		{
			return &this->it_[this->index_ - 1];
		}
		
		constexpr auto& operator*() const
		{ 
			return this->it_[this->index_ - 1]; 
		}
		constexpr auto operator->() const
		{
			return &this->it_[this->index_ - 1];
		}
		
		template<bool otherConstFlag>
		constexpr bool operator<(const ReverseIterator<otherConstFlag>& rhs) const noexcept
		{
			return this->hasSameArray(rhs) 
				&& this->hasGreaterIndex(rhs);
		}
		
		template<bool otherConstFlag>
		constexpr bool operator<=(const ReverseIterator<otherConstFlag>& rhs) noexcept
		{
			return this->hasSameArray(rhs) 
				&& !this->hasSmallerIndex(rhs);
		}
		
		template<bool otherConstFlag>
		constexpr bool operator>(const ReverseIterator<otherConstFlag>& rhs) noexcept
		{
			return this->hasSameArray(rhs) 
				&& this->hasSmallerIndex(rhs);
		}
		
		template<bool otherConstFlag>
		constexpr bool operator>=(const ReverseIterator<otherConstFlag>& rhs) noexcept
		{
			return this->hasSameArray(rhs) 
				&& !this->hasGreaterIndex(rhs);
		}
	};
	
	template<bool constFlag>
	class UncheckedIterator : public Iterator<constFlag>
	{
		using Base = Iterator<constFlag>;

		public:
		using value_type = typename Base::value_type;
		using reference = typename Base::reference;
		using pointer = typename Base::pointer;
		
		using Base::Base;
		
		constexpr typename std::remove_cv<value_type>::type operator*() const noexcept
		{
			return *(this->operator->());
		}
		
		constexpr pointer operator->() const noexcept
		{
			return this->it_.data() + this->index_;
		}
	};
	
	template<bool constFlag>
	class UncheckedReverseIterator : public ReverseIterator<constFlag>
	{	
		using Base = ReverseIterator<constFlag>;
		
		public:
		using value_type = typename Base::value_type;
		using reference = typename Base::reference;
		using pointer = typename Base::pointer;
		
		using Base::Base;
		
		constexpr typename std::remove_cv<value_type>::type operator*() const noexcept
		{
			return *(this->operator->());
		}
		
		constexpr pointer operator->() const noexcept
		{
			return this->it_.data() + this->index_;
		}
	};

public:
	using iterator = Iterator<false>;
	using reverse_iterator = ReverseIterator<false>;
	using const_iterator = Iterator<true>;
	using const_reverse_iterator = ReverseIterator<true>;
	
	using unchecked_iterator = UncheckedIterator<false>;
	using unchecked_reverse_iterator = UncheckedReverseIterator<false>;
	using unchecked_const_iterator = UncheckedIterator<true>;
	using unchecked_const_reverse_iterator = UncheckedReverseIterator<true>;
};

#if(COMPILER == MVSC_COMPILER)
#pragma warning(pop)
#endif

#endif // ARRAY_ITERATOR_POLICY_HXX