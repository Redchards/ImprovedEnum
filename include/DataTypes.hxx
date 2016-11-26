#ifndef DATA_TYPE_HXX
#define DATA_TYPE_HXX

#include <cmath>
#include <sstream>
#include <string>

#include <Configuration.hxx>
#include <ConstString.hxx>
#include <ImprovedEnum.hxx>

IMPROVED_ENUM(DataType, uint16_t,
	CHARACTER,
	BINARY,
	BOOLEAN,
	INTEGER,
	FLOAT,
	DATE,
	TIME
);

/** A descriptor representing a type along with its modifier (precision or byte count) **/
class DataTypeDescriptor
{
	static constexpr size_t dateTypeSize = 4;
	static constexpr size_t booleanTypeSize = 1;

	public:
	DataTypeDescriptor(DataType type, size_t modifier = 0) 
	: modifier_{modifier},
	  type_{type}
	{}

	std::string toString()
	{
		std::string result;

		if(modifier_ != 0)
		{
			std::stringstream sstr;
			sstr << modifier_;
			result =  std::string{type_.toString()} + "(" + sstr.str() + ")";
		}
		else 
		{
			result = type_.toString();
		}
	
		return result;
	}
	
	DataType getType() const noexcept
	{
		return type_;
	}
	
	size_type getModifier() const noexcept
	{
		return modifier_;
	}

	size_t getSize() const noexcept
	{
		size_t result = modifier_;

		if((type_ == DataType::FLOAT) || (type_ == DataType::TIME))
		{ 
			result =  4 * ceil(static_cast<float>(modifier_)/24);
		}
		else if(type_ == DataType::DATE) 
		{	
			result = dateTypeSize;
		}
		else if(type_ == DataType::BOOLEAN) 
		{
			result = booleanTypeSize;
		}
		else if(type_ == DataType::INTEGER)
		{
			result = modifier_ / 8; 
		}

		return result;
	}

	static constexpr size_type getPackedSize()  noexcept
	{
		return packedSize;
	}

	private:
	size_type modifier_;
	DataType type_;
	static constexpr size_type packedSize = sizeof(size_type) + sizeof(DataType);
};
	

#endif // DATA_TYPE_HXX
