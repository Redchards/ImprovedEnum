#ifndef DB_ENTRY_HXX
#define DB_ENTRY_HXX

#include <Schema.hxx>

#include <vector>

/* WARNING : The DbSchema must be persistent, should it be destroyed at on point, the DbEntry would become unusable */
template<Endianess endian>
class DbEntry
{
public:
	DbEntry(const DbSchema& schema) : schema_{schema}
	{
		storage_.reserve(schema.getSize());
	}

	template<class T>
	DbEntry(const DbSchema& schema, const std::vector<T>& data) : schema_{schema}
	{
		static_assert(std::is_convertible<T, char>::value || std::is_convertible<T, unsigned char>::value,
					 "The data type must be convertible to a type with byte size.");

		storage_.reserve(schema.getSize());
		std::copy(data.begin(), data.end(), storage_.begin());
	}
	std::string toString() const noexcept
	{
		std::string result;
		auto it = storage_.begin();
		
		for(size_type i = 0; i < schema_.getFieldCount(); ++i)
		{
			auto fieldDescriptor = schema_[i];
			auto typeDescriptor = fieldDescriptor.type;
			result += fieldDescriptor.name + " : ";
			result += Utils::RawDataStringizer<endian>::stringize(it, it + typeDescriptor.getSize(), typeDescriptor.getType());
			result += '\n';
			it += typeDescriptor.getSize();
		}	

		return result;
	}

	private:
	std::vector<uint8_t> storage_;
	const DbSchema& schema_;
};

#endif // DB_ENTRY_HXX
