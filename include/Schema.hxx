#ifndef SCHEMA_HXX
#define SCHEMA_HXX

#include <string>
#include <unordered_map>
#include <vector>

#include <Configuration.hxx>
#include <DataTypes.hxx>

struct FieldDescriptor
{
	FieldDescriptor(const std::string& name_, DataTypeDescriptor type_)
	: name{name_},
	  type{type_}
	{}
	
	std::string name;
	DataTypeDescriptor type;	
};

class DbSchema 
{
	public:
	DbSchema(std::string name, const std::vector<FieldDescriptor>& internal) 
	: name_{name}, 
	  internal_{internal},
	  size_{computeSize()}
	{}
	
	DbSchema(std::string name, std::vector<FieldDescriptor>&& internal) 
	: name_{name}, 
	  internal_{std::move(internal)},
	  size_{computeSize()}
	{}

	
	const std::string& getName() const noexcept
	{
		return name_;
	}

	private:
	// TODO : making it private instead ?
	size_type computeSize() const noexcept
	{
		size_type space = name_.length() + 1;
		for(auto& elem : internal_)
		{

	std::cout << "LLLL ? " << space << std::endl;	
			space += elem.name.length() + 1 + DataTypeDescriptor::getPackedSize();
		std::cout << "LLLL ... " << DataTypeDescriptor::getPackedSize() << " : " << elem.name.size() << std::endl;
		}
		
		return space;
	}

	public:
	size_type getSize() const noexcept
	{
		return size_;
	}
	
	size_type getFieldCount() const noexcept
	{
		return internal_.size();
	}
	
	FieldDescriptor at(size_type index) const noexcept
	{
		return internal_[index];
	}
	
	FieldDescriptor operator[](size_type index) const noexcept
	{
		return at(index);
	}
	
	size_type getIndexOf(const std::string& fieldName)
	{
		return std::distance(
		std::find_if(internal_.begin(), internal_.end(), [&fieldName](const auto& elem) {
			return elem.name == fieldName;
		}), internal_.begin());
	}

	private:
	std::string name_;
	std::vector<FieldDescriptor> internal_;
	size_type size_;

	static constexpr size_type nameLengthFieldSize = 2;
};

#endif // SCHEMA_HXX
