#ifndef CONSTEXPR_ASSERT_HXX
#define CONSTEXPR_ASSERT_HXX

#include <cstdlib>
#include <cstdio>

#include <ConstString.hxx>
#include <Platform.hxx>

/* Little trick inspired by Eric Niebler's blog post :
 * http://ericniebler.com/2014/09/27/assert-and-constexpr-in-cxx11/
 * Slighty modified to be a bit more flexible.
 * I still consider it, however, a hack. This is a shame that the C++ does not yet give us a clean
 * way to express post and pre-conditions with optional constexpr.
 * There is, however, some interesting proposals on the subject :
 * https://www.google.fr/url?sa=t&rct=j&q=&esrc=s&source=web&cd=3&ved=0ahUKEwin9ZDnwMvMAhWJA8AKHc7fDJ0QFggvMAI&url=http%3A%2F%2Fwww.open-std.org%2Fjtc1%2Fsc22%2Fwg21%2Fdocs%2Fpapers%2F2015%2Fn4415.pdf&usg=AFQjCNH06Us_FdCLUrqKYmvD7r4swtRT-w&sig2=Mhbs3WJfX7zobX6pFICAcw&cad=rja
 * https://www.google.fr/url?sa=t&rct=j&q=&esrc=s&source=web&cd=2&ved=0ahUKEwin9ZDnwMvMAhWJA8AKHc7fDJ0QFggmMAE&url=http%3A%2F%2Fwww.open-std.org%2Fjtc1%2Fsc22%2Fwg21%2Fdocs%2Fpapers%2F2014%2Fn4293.pdf&usg=AFQjCNHRfOCc2KsD16oCJWg6KK3pClTp1Q&sig2=Rt6qGhRRtCW76YhxaturVQ
 */

#define CONSTEXPR_ASSERT(condition, msg) condition ? \
										 int{} : \
										 throw Details::ConstexprAssertFailure([](ConstString assertMsg){ \
										     std::fprintf(stderr, "Assertion failure: %s\n", static_cast<const char*>(assertMsg)); assert(!#condition); \
										 }, msg)

namespace Details
{

class ConstexprAssertFailure
{
	public:
	template<class Fn>
	constexpr ConstexprAssertFailure(Fn&& fn, ConstString msg)
	{
		fn(msg);
		std::quick_exit(EXIT_FAILURE);
	}
};

}

#endif // CONSTEXPR_ASSERT_HXX