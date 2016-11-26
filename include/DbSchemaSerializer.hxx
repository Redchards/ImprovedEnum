#ifndef DB_SCHEMA_SERIALIZER_HXX
#define DB_SCHEMA_SERIALIZER_HXX

#include <Configuration.hxx>
#include <DataTypes.hxx>
#include <MetaUtils.hxx>
#include <RawDataUtils.hxx>
#include <Schema.hxx>

#include <vector>

template<Endianess endian>
class DbSchemaSerializerBase;


template<Endianess endian>
class DbSchemaSerializer// : public DbSchemaSerializerBase<endian>
{
	
	using Base = DbSchemaSerializerBase<endian>;

	public:
	static std::vector<uint8_t> serialize(const DbSchema& sch)
	{	
		using dataTypeUnderlying = typename DataType::underlying_type;

		std::vector<uint8_t> result{};
		
		static constexpr size_type offset = sizeof(size_type);
		
		// Accounting for the schema size field;
		auto totalSize = sch.getSize() + sizeof(size_type);

		result.reserve(totalSize);
		result.resize(offset);
		
		const std::string& schemeName = sch.getName();
		result.insert(result.end(), schemeName.begin(), schemeName.end());		

		result.push_back('\0');


		for(size_type i = 0; i < sch.getFieldCount(); ++i)
		{
			const auto& elem = sch[i];
			result.insert(result.end(), elem.name.begin(), elem.name.end());
			result.push_back('\0');

			Utils::RawDataAdaptator<dataTypeUnderlying, sizeof(dataTypeUnderlying), endian> typeId{ elem.type.getType().toUnderlying() };
			Utils::RawDataAdaptator<size_type, sizeof(size_type), endian> modifier{ elem.type.getModifier() };
			
			result.insert(result.end(), typeId.bytes.begin(), typeId.bytes.end());
			
			result.insert(result.end(), modifier.bytes.begin(), modifier.bytes.end());
		}

		Utils::RawDataAdaptator<decltype(offset), sizeof(decltype(totalSize)), endian> schemaSize = totalSize;
		std::copy(schemaSize.bytes.begin(), schemaSize.bytes.end(), result.begin());
	
		return result;
	}
	
	/*
	 * The raw data passed must be in the format of the serialized method, with the schema size in
	 * the beginning removed.
	 */
	template<class Iterator>
	static DbSchema deserialize(Iterator begin, Iterator end)
	{
		static_assert(std::is_convertible<typename std::iterator_traits<Iterator>::value_type, uint8_t>::value, 
			     	  "The underlying type must be convertible to a byte (uint8_t for instance)");
		
		using DataConverter = Utils::RawDataConverter<endian>;
		
		Iterator current = begin;
		
		auto findNullTerminator = [&current, end]() {
			return std::find(current, end, '\0');
		};
		
		std::vector<FieldDescriptor> data;
		std::string name{current, findNullTerminator()};
		current += name.length() + 1;
		
		while(current != end)
		{
			std::string tmp{current, findNullTerminator()};
			current += tmp.length() + 1;

			DataType type{ DataType::fromValue(DataConverter::rawDataToInteger(current, current + sizeof(typename DataType::underlying_type))) };
			current += sizeof(DataType);
			
			DataTypeDescriptor desc { 
				type,
				DataConverter::rawDataToInteger(current, current + sizeof(size_type))
			};
			current += sizeof(size_type);
			
			data.emplace_back(tmp, desc);
		}
		
		return {name, data};
	}
};

#endif // DB_SCHEMA_SERIALIZER_HXX
