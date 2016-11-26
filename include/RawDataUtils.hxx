#ifndef RAW_DATA_UTILS_HXX
#define RAW_DATA_UTILS_HXX

#include <array>
#include <iostream>
#include <exception>
#include <sstream>
#include <Configuration.hxx>
#include <DataTypes.hxx>
#include <MetaUtils.hxx>


namespace Utils
{

namespace 
{

template<size_type ... indices, class Iterator, 
		 class Arr = std::array<typename std::iterator_traits<Iterator>::value_type, sizeof...(indices)>>
Arr makeArray(range<Iterator> rg, Meta::index_sequence<indices...>)
{
	Ensures(rg.size() <= sizeof...(indices));
	return {{rg.begin()[indices]...}};
}

}

template<size_type size, class Iterator>
auto makeArray(range<Iterator> rg)
{
	return makeArray(rg, Meta::make_index_sequence<size>{});
}

template<size_type size, class Iterator>
auto makeArray(Iterator begin, Iterator end)
{
	return makeArray(range<Iterator>{begin, end}, Meta::make_index_sequence<size>{});
}

template<class Iterator>
static void rawDataSwitchEndianess(Iterator begin, Iterator end)
{
	static_assert(std::is_convertible<typename std::iterator_traits<Iterator>::value_type, uint8_t>::value 
				  || std::is_convertible<typename std::iterator_traits<Iterator>::value_type, int8_t>::value,
				  "The underlying type must be convertible to a byte (uint8_t for instance)");
	
	auto last = end - 1;
	
	for(auto it = begin; it < last; ++it, --last)
	{
		std::swap(*last, *it);
	}
}

template<class Iterator>
static void rawDataSwitchEndiness(range<Iterator>& rg)
{
	rawDataSwitchEndianess(rg.begin(), rg.end());
}

template<class Container>
static void rawDataSwitchEndianess(Container& cont)
{
	rawDataSwitchEndianess(cont.begin(), cont.end());
}
	
template<class T, size_type N, Endianess endian = Endianess::little>
union RawDataAdaptator
{
	static_assert(N <= sizeof(size_type), 
				 "The adaptator structure size must not exceed the size of size_type");

	public:
	RawDataAdaptator(std::array<uint8_t, N> arr) : bytes{ arr }
	{
		rawDataSwitchEndianess(bytes);
	}

	template<class Iterator>
	RawDataAdaptator(Iterator begin, Iterator end) : bytes{ makeArray<N>(begin, end) }
	{
		static_assert(std::is_convertible<typename std::iterator_traits<Iterator>::value_type, uint8_t>::value 
					  || std::is_convertible<typename std::iterator_traits<Iterator>::value_type, int8_t>::value,
					  "The underlying type must be convertible to a byte (uint8_t for instance)");
		
		rawDaraSwitchEndianess(bytes);
	}
	
	RawDataAdaptator(T val) : value{ val }
	{}

	std::array<uint8_t, N> bytes;
	T value;

	static constexpr size_type size = N;
};

template<class T, size_type N>
union RawDataAdaptator<T, N, Endianess::big>
{
	static_assert(N <= sizeof(size_type),
				 "The adaptator structure size must not exceed the size of size_type");

	public:
	RawDataAdaptator(std::array<uint8_t, N> arr) : bytes{ arr }
	{}

	template<class Iterator>
	RawDataAdaptator(Iterator begin, Iterator end) : bytes{ makeArray<N>(begin, end) }
	{
		static_assert(std::is_convertible<typename std::iterator_traits<Iterator>::value_type, uint8_t>::value 
					  || std::is_convertible<typename std::iterator_traits<Iterator>::value_type, int8_t>::value,
					  "The underlying type must be convertible to a byte (uint8_t for instance)");
	}

	RawDataAdaptator(T val) : value{ val }
	{
		rawDataSwitchEndianess(bytes);
	}

	std::array<uint8_t, N> bytes;
	T value;

	static constexpr size_type size = N;
};

template<class Iterable>
class ReverseIteratorAdaptator
{
	public:
	constexpr ReverseIteratorAdaptator(Iterable iterable) : iterable_{iterable}
	{}
	
	constexpr auto begin() noexcept { iterable_.rbegin(); }
	constexpr auto begin() const noexcept { iterable_.crbegin(); }
	constexpr auto cbegin() const noexcept { iterable_.crbegin(); }
	constexpr auto end() noexcept { iterable_.rend(); }
	constexpr auto end() const noexcept { iterable_.crend(); }
	constexpr auto cend() const noexcept { iterable_.crend(); }
	
	private:
	Iterable& iterable_;
};

template<class Iterator>
class ReverseIteratorAdaptator<range<Iterator>>
{
	public:
	constexpr ReverseIteratorAdaptator(range<Iterator> iterable) : iterable_{iterable}
	{}
	
	constexpr auto begin() noexcept { return std::reverse_iterator<Iterator>{ iterable_.end() }; }
	constexpr auto end() noexcept { return std::reverse_iterator<Iterator>{ iterable_.begin() }; }	
	
	private:
	range<Iterator>& iterable_;
};

template<class Iterable>
auto reverseIterator(Iterable cont)
{
	return ReverseIteratorAdaptator<Iterable>{cont};
}

template<Endianess>
class EndianessRangeIteratorSelector;

template<>
class EndianessRangeIteratorSelector<Endianess::little>
{

public:
	template<class Iterator>
	static auto select(range<Iterator> rg)
	{
		return rg;
	}
};

template<>
class EndianessRangeIteratorSelector<Endianess::big>
{

public:
	template<class Iterator>
	static auto select(range<Iterator> rg)
	{
		return reverseIterator(rg);
	}	
};



template<Endianess endian>
class RawDataConverter
{

public:
	template<class Iterator>
	static size_type rawDataToInteger(Iterator begin, Iterator end)
	{
		static_assert(std::is_convertible<typename std::iterator_traits<Iterator>::value_type, uint8_t>::value 
				  || std::is_convertible<typename std::iterator_traits<Iterator>::value_type, int8_t>::value,
				  "The underlying type must be convertible to a byte (uint8_t for instance)");


		range<Iterator> rg{begin, end};

		Ensures(rg.size() <= sizeof(size_type));

		size_type out = 0;
		uint8_t offset = 0;

		for(auto byte : EndianessRangeIteratorSelector<endian>::select(rg))
		{
			std::cout << "b : " <<(int)byte << std::endl;
			out |= (byte << (offset * 8));
			std::cout << (int)byte << std::endl;
			++offset;
		}

		return out;
	}
	
	template<class Container>
	static size_type rawDataToInteger(Container& cont)
	{
		return rawDataToInteger(cont.begin(), cont.end());
	}
	
	template<class Iterator>
	static size_type rawDataToInteger(range<Iterator> rg)
	{
		return rawDataToInteger(rg.begin(), rg.end());
	}

};


class DataConversionFailure : public std::exception
{

public:
	DataConversionFailure(const std::string& msg) : msg_{"Data conversion failure : " + msg}
	{}

	virtual const char* what() const noexcept override
	{
		return msg_.c_str();
	}

private:
	const std::string msg_;
};

template<Endianess endian>
class RawDataStringizer
{
	public:
	template<class Iterator>
	static std::string stringize(Iterator begin, Iterator end, DataTypeDescriptor type)
	{
		static_assert(std::is_convertible<typename std::iterator_traits<Iterator>::value_type, uint8_t>::value 
					  || std::is_convertible<typename std::iterator_traits<Iterator>::value_type, int8_t>::value,
				      "The underlying type must be convertible to a byte (uint8_t for instance)");

		if((type.getType() != DataType::CHARACTER) && (type.getType() != DataType::BINARY))
		{
			std::stringstream sstr;
			if((type.getType() == DataType::FLOAT) || (type.getType() == DataType::TIME)) 
			{
				if(type.getSize() <= sizeof(float))
				{
					RawDataAdaptator<float, sizeof(float), endian> adapt{begin, begin + type.getSize()};;
					sstr << adapt.value;
				}
				else if(type.getSize() <= sizeof(double))
				{
					RawDataAdaptator<double, sizeof(double), endian> adapt{begin, begin + type.getSize()};
					sstr << adapt.value;
				}
				else
				{
					throw DataConversionFailure("Failed to convert data to floating point, the size of the type is higher than the biggest floating point type on this platform");
				}
			}
			else if(type.getType() == DataType::INTEGER)
			{
				if(type.getSize() >= sizeof(size_type))
				{
					throw DataConversionFailure("Failed to convert data to integer, the size of the type is higher than the biggest integer type on this platform");
				} 
				sstr << RawDataConverter<endian>::rawDataToInteger(begin, end);
			}
			else if(type.getType() == DataType::BOOLEAN)
			{
				sstr << std::boolalpha;
				sstr << *begin;
			}
			else if(type.getType() == DataType::DATE)
			{
				if(Details::difference(begin, end) < 4)
				{
					throw DataConversionFailure("Need at least 4 bytes of data for a date");
				}
				sstr << RawDataConverter<endian>::rawDataToInteger(begin, begin + 1)
					 << " : "
					 << RawDataConverter<endian>::rawDataToInteger(begin + 1, begin + 2)
					 << " : "
					 << RawDataConverter<endian>::rawDataToInteger(begin + 2, begin + 4);
			}
			return sstr.str();
		}
		else
		{
			std::string result;

			for(auto byte : range<Iterator>{begin, end})
			{
				result += byte;
			}

			std::cout << "Result : " << result << std::endl;
			return result;
		}
	}
};

}

#endif // RAW_DATA_UTILS
