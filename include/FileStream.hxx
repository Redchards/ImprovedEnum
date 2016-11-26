#ifndef FILE_STREAM_HXX
#define FILE_STREAM_HXX

#include <Configuration.hxx>

#include <gsl/gsl_assert.h>

#include <fstream>
#include <iterator>
#include <string>
#include <unordered_map>
#include <vector>

enum class StreamGoal : flag_type
{
	read,
	write
};

// Selector, allowing us to add new class easily, along with stream goals

template<StreamGoal goal>
class FileStreamBase;

template<StreamGoal goal>
class FileStreamSelector;

template<>
class FileStreamSelector<StreamGoal::read>
{
public:
	using type = std::ifstream;

public:
	FileStreamSelector(const std::string& filename, std::ios::ios_base::openmode flags)
	: filename_{filename}
	{
		fstream_ = &streamMap_[filename];
		if (!fstream_->is_open())
		{
			fstream_->open(filename, flags);
		}
	}

	void read(char* buffer, size_type size, std::streampos position)
	{
		goTo(position);
		fstream_->read(buffer, size);
		checkRead();
	}

	void read(unsigned char* buffer, size_type size, std::streampos position)
	{
		read(reinterpret_cast<char*>(buffer), size, position);
	}

	template<class T, size_type n>
	void read(std::array<T, n>& buffer, size_type size, std::streampos position)
	{
		static_assert(std::is_convertible<T, char>::value || std::is_convertible<T, unsigned char>::value,
				     "The buffer must contain byte size values.");
		Ensures(size <= n);
		read(buffer.data(), buffer.size(), position);
	}

	template<class T>
	void read(std::vector<T>& buffer, size_type size,  std::streampos position)
	{
		static_assert(std::is_convertible<T, char>::value || std::is_convertible<T, unsigned char>::value,
				     "The buffer must contain byte size values.");
		buffer.resize(size);
		read(buffer.data(), buffer.size(), position);
	}
	
	void read(char* buffer, size_type size)
	{
		fstream_->read(buffer, size);
		checkRead();
	}

	void read(unsigned char* buffer, size_type size)
	{
		read(reinterpret_cast<char*>(buffer), size);
	}

	template<class T, size_type n>
	void read(std::array<T, n>& buffer, size_type size)
	{
		static_assert(std::is_convertible<T, char>::value || std::is_convertible<T, unsigned char>::value,
				     "The buffer must contain byte size values.");
		Ensures(size <= n);
		read(buffer.data(), buffer.size());
	}

	template<class T>
	void read(std::vector<T>& buffer, size_type size)
	{
		static_assert(std::is_convertible<T, char>::value || std::is_convertible<T, unsigned char>::value,
				     "The buffer must contain byte size values.");
		buffer.resize(size);
		read(buffer.data(), buffer.size());
	}

	void rewind()
	{
		goTo(0);
	}

	void goTo(std::streampos position)
	{
		if (!fstream_->seekg(position))
		{
			throw std::ios_base::failure("Error when processing the file !");
		}
	}

	std::streampos getCurrentPosition()
	{
		return fstream_->tellg();
	}

protected:

	void checkRead() const	
	{	
		if (!fstream_->good())
		{
			if(fstream_->eof())
			{
				throw std::ios_base::failure(std::string{"Error : attempting to read beyond the end of the file '" + filename_ + "'."});
			}
			else if(fstream_->bad())
			{
				throw std::ios_base::failure(std::string{"Unknown error when read data from the file '"} + filename_ + "' !");
			}
			else if(fstream_->fail())
			{
				throw std::ios_base::failure(std::string{"Failed to read data from the file '"} + filename_ + "' !");
			}
		}
	}

	type* fstream_;
	std::string filename_;
	static std::unordered_map<std::string, type> streamMap_;
};

template<>
class FileStreamSelector<StreamGoal::write>
{
public:
	using type = std::ofstream;

public:
	FileStreamSelector(const std::string& filename, std::ios::ios_base::openmode flags)
	: filename_{filename}
	{
		fstream_ = &streamMap_[filename];
		if (!fstream_->is_open())
		{
			fstream_->open(filename, flags);
		}
	}

	void write(const char* buffer, size_type size, std::streampos position)
	{
		goTo(position);
		fstream_->write(buffer, size);
		checkWrite();
	}

	void write(const unsigned char* buffer, size_type size, std::streampos position)
	{
		write(reinterpret_cast<const char*>(buffer), size, position);
	}

	template<class T, size_type n>
	void write(const std::array<T, n>& buffer, std::streampos position)
	{
		static_assert(std::is_convertible<T, char>::value || std::is_convertible<T, unsigned char>::value,
					 "The buffer must contain byte size values.");
		write(buffer.data(), buffer.size(), position);
	}

	template<class T>
	void write(const std::vector<T>& buffer, std::streampos position)
	{
		static_assert(std::is_convertible<T, char>::value || std::is_convertible<T, unsigned char>::value,
					  "The buffer must contain byte size values.");
		write(buffer.data(), buffer.size(),  position);
	}

	void write(const char* buffer, size_type size)
	{
		fstream_->write(buffer, size);
		checkWrite();
	}

	void write(const unsigned char* buffer, size_type size)
	{
		write(reinterpret_cast<const char*>(buffer), size);
	}

	template<class T, size_type n>
	void write(const std::array<T, n>& buffer)
	{
		static_assert(std::is_convertible<T, char>::value || std::is_convertible<T, unsigned char>::value,
					 "The buffer must contain byte size values.");
		write(buffer.data(), buffer.size());
	}

	template<class T>
	void write(const std::vector<T>& buffer)
	{
		static_assert(std::is_convertible<T, char>::value || std::is_convertible<T, unsigned char>::value,
					 "The buffer must contain byte size values.");
		write(buffer.data(), buffer.size());
	}

	void rewind()
	{
		goTo(0);
	}

	void goTo(std::streampos position)
	{
		if (!fstream_->seekp(position))
		{
			throw std::ios_base::failure("Error when processing the file !");
		}
	}

	std::streampos getCurrentPosition() const noexcept
	{
		return fstream_->tellp();
	}


protected:
	void checkWrite() const	
	{	
		if(!fstream_->good())
		{
			if(fstream_->bad())
			{
				throw std::ios_base::failure(std::string{"Unknown error when writing data to the file '"} + filename_ + "' !");
			}
			else if(fstream_->fail())
			{
				throw std::ios_base::failure(std::string{"Failed to write data to the file '"} + filename_ + "' !");
			}
		}
	}


	type* fstream_;
	std::string filename_;
	static std::unordered_map<std::string, type> streamMap_;
};

template<StreamGoal goal>
class FileStreamBase : public FileStreamSelector<goal>
{
	using Base = FileStreamSelector<goal>;

protected:
	FileStreamBase(const std::string& filename, std::ios::ios_base::openmode flags) 
	: Base(filename, flags)
	{
		init();
	}

public:
	void loadFile(const std::string& filename, std::ios::ios_base::openmode flags = std::ios_base::in)
	{
		unloadFile();
		this->fstream_->open(filename, flags);
		this->filename_ = filename;
		init();
	}

	void unloadFile()
	{
		if (this->fstream_->is_open())
		{
			this->fstream_->close();
			this->filename_ = "";
		}
	}

	bool isOpen()
	{
		return this->fstream_->is_open();
	}

	const std::string& getCurrentFileName() const noexcept
	{
		return this->filename_;
	}

	bool eof() const noexcept
	{
		return this->fstream_->peek() == EOF;
	}

private:
	void init()
	{
		if (!this->fstream_->good())
		{
			throw std::ios_base::failure((std::string("Error : failed to open the file ") + getCurrentFileName() + ". Please check that the file exists, and is a valid Win32 file !").c_str());
		}
		// So that stream will not interpret "whitespace" bytes as real whitespaces, and remove them.
		this->fstream_->unsetf(std::ios::skipws);
	}
};

#endif // FILE_STREAM_HXX
