#ifndef FILE_VALUE_READER_HXX
#define FILE_VALUE_READER_HXX

#include <Configuration.hxx>
#include <FileStream.hxx>
#include <RawDataUtils.hxx>

#include <gsl/gsl_assert.h>

#include <array>
#include <fstream>
#include <iterator>
#include <memory>
#include <string>
#include <vector>

// Maybe should move all enums to Configuration.h header

enum class BufferPolicy : flag_type
{
	nonBuffered = false,
	buffered = true
};

class FileValueReaderBase : public FileStreamBase<StreamGoal::read>
{
	using Base = FileStreamBase<StreamGoal::read>;

protected:
	FileValueReaderBase(const std::string& filename, std::ios::ios_base::openmode flags = std::ios_base::in | std::ios::binary);

public:
	std::unique_ptr<uint8_t> retrieveRawData(std::streampos position, size_type size);
	std::vector<uint8_t> retrieveRawBuffer(std::streampos position, size_type size);
	
	std::string readString(std::streampos pos);
	std::string readString();
};

template<Endianess endian>
class FileValueReader : public FileValueReaderBase
{
public:
	FileValueReader(const std::string& filename, std::ios::ios_base::openmode flags = std::ios_base::in | std::ios::binary) 
    : FileValueReaderBase(filename, flags)
	{}

	size_type readValue(size_type size, std::streampos position)
	{
		std::array<unsigned char, sizeof(size_type)> value{};
		read(value, size, position);

		return Utils::RawDataConverter<endian>::rawDataToInteger(value.begin(), value.begin() + size);
	}

	size_type readValue(size_type size)
	{
		std::array<unsigned char, sizeof(size_type)> value{};
		read(value, size);
		
		return Utils::RawDataConverter<endian>::rawDataToInteger(value.begin(), value.begin() + size);
	}
};

using FileReader = FileValueReader<Endianess::little>;

#endif // FILE_VALUE_READER_HXX
