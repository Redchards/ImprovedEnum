#ifndef FILE_VALUE_WRITER_HXX
#define FILE_VALUE_WRITER_HXX

#include <Configuration.hxx>
#include <FileStream.hxx>
#include <Range.hxx>
#include <RawDataUtils.hxx>

#include <array>
#include <fstream>
#include <string>
#include <vector>

class FileValueWriterBase : public FileStreamBase<StreamGoal::write>
{
	using Base = FileStreamBase<StreamGoal::write>;

protected:
	FileValueWriterBase(const std::string& filename, std::ios::ios_base::openmode flags) : Base(filename, flags)
	{}
	

public:

};

template<Endianess endian = Endianess::little>
class FileValueWriter : public FileValueWriterBase
{
	
public:
	FileValueWriter(const std::string& filename, std::ios::ios_base::openmode flags = std::ios_base::out) : FileValueWriterBase(filename, flags)
	{}

	/* Warning : may not work properly right now */
	template<class Iterator,
			 typename std::enable_if_t<std::is_convertible<typename Iterator::value_type, uint8_t>::value>* = nullptr>
	void writeDataToValue(range<Iterator> rg, std::streampos position)
	{
		Utils::RawDataAdaptator<size_type, sizeof(size_type), endian> tmp{rg.begin(), rg.end()};
		writeRawData(tmp.bytes, position);
	}
	
};

#endif // FILE_VALUE_WRITER_HXX
