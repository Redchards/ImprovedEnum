#ifndef STRING_DETAILS_HXX
#define STRING_DETAILS_HXX

namespace Details
{
	
template<class ArrayClass1, class ArrayClass2>
constexpr bool equalAux(const size_t size, const ArrayClass1& lhs, const ArrayClass2& rhs)
{
	return size != 0 ? ((lhs[size - 1] == rhs[size - 1] && equalAux(size - 1, lhs, rhs))) : true;
}

}

#endif // STRING_DETAILS_HXX