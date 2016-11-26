#pragma once

#include <cstdint>

#include <Platform.hxx>

#ifdef X86
	typedef unsigned long size_type;
#else
	typedef std::size_t size_type;
#endif

typedef uint16_t flag_type;

#define stringify(x) stringify_(x)
#define stringify_(x) #x

enum class Endianess : flag_type
{
	little,
	big
};

