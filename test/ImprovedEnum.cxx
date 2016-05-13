#ifndef ENUM_UTILS_TEST_HXX
#define ENUM_UTILS_TEST_HXX

#include <iterator>
#include <tuple>

#include <mettle/header_only.hpp>
using namespace mettle;

#include <ImprovedEnum.hxx>

iterable_ENUM(iterableEnumTst1, size_t,
	Test1,
	Test2,
	Test3,
	Test4,
	Test5
);

iterable_ENUM(iterableEnumTst2, size_t,
	Test1,
	Test2,
	Test3,
	Test4,
	Test5,
	Test6,
	Test7,
	Test8,
	Test9,
	Test10
);

iterable_ENUM(iterableEnumTst3, size_t,
	Test1,
	Test2 = 9,
	Test3 = 12,
	Test4 = 27,
	Test5
);

IMPROVED_ENUM(ImprovedEnumTst1, size_t,
	Test1,
	Test2,
	Test3,
	Test4,
	Test5
);

IMPROVED_ENUM(ImprovedEnumTst2, size_t,
	Test1,
	Test2,
	Test3,
	Test4,
	Test5,
	Test6,
	Test7,
	Test8,
	Test9,
	Test10
);

IMPROVED_ENUM(ImprovedEnumTst3, size_t,
	Test1,
	Test2=9,
	Test3=12,
	Test4=27,
	Test5
);

using iterableEnumTestList = 
	std::tuple<
		iterableEnumTst1,
		iterableEnumTst2,
		iterableEnumTst3
	>;
	
using improvedEnumTestList = 
	std::tuple<
		ImprovedEnumTst1,
		ImprovedEnumTst2,
		ImprovedEnumTst3
	>;

template<class EnumName>
struct enumFixture
{
	std::remove_const_t<decltype(EnumName::values)> values;
};

template<class EnumName>
auto iterableEnumTestCode()
{
	return [](auto &_) {
		_.setup([](enumFixture<EnumName>& fixture) {
			std::copy(std::begin(EnumName::values), std::end(EnumName::values), std::begin(fixture.values));
		});
		
		_.test("Iteration test", [](enumFixture<EnumName>& fixture) {
			size_t i = 0;
			for(auto value : EnumName::iterable())
			{
				expect(value.toUnderlying(), equal_to(fixture.values[i]));
				++i;
			}
		});
	
		_.test("Index of first element", [](enumFixture<EnumName>&) {
			EnumName tst = EnumName::Test1;
			expect(tst.getIndex(), equal_to(0));	  
		});
		
		_.test("Index of second element", [](enumFixture<EnumName>&) {
			EnumName tst = EnumName::Test2;
			expect(tst.getIndex(), equal_to(1));	  
		});
		
		_.test("Index of third element", [](enumFixture<EnumName>&) {
			EnumName tst = EnumName::Test3;
			expect(tst.getIndex(), equal_to(2));	  
		});
		
		_.test("Index of fourth element", [](enumFixture<EnumName>&) {
			EnumName tst = EnumName::Test4;
			expect(tst.getIndex(), equal_to(3));	  
		});
		
		_.test("Index of fifth element", [](enumFixture<EnumName>&) {
			EnumName tst = EnumName::Test4;
			expect(tst.getIndex(), equal_to(3));	  
		});
		
		_.test("Iteration from first element with 'from()' method", [](enumFixture<EnumName>& fixture) {
			size_t i = 0;
			for(auto it = EnumName::iterable().from(EnumName::Test1); it != EnumName::iterable().end(); ++it)
			{
				expect(it->toUnderlying(), equal_to(fixture.values[i]));
				++i;
			}
		});
		
		_.test("Iteration from second element with 'from()' method", [](enumFixture<EnumName>& fixture) {
			size_t i = 1;
			for(auto it = EnumName::iterable().from(EnumName::Test2); it != EnumName::iterable().end(); ++it)
			{
				expect(it->toUnderlying(), equal_to(fixture.values[i]));
				++i;
			}
		});
		
		_.test("Iteration from third element with 'from()' method", [](enumFixture<EnumName>& fixture) {
			size_t i = 2;
			for(auto it = EnumName::iterable().from(EnumName::Test3); it != EnumName::iterable().end(); ++it)
			{
				expect(it->toUnderlying(), equal_to(fixture.values[i]));
				++i;
			}
		});
		
		_.test("Iteration from fourth element with 'from()' method", [](enumFixture<EnumName>& fixture) {
			size_t i = 3;
			for(auto it = EnumName::iterable().from(EnumName::Test4); it != EnumName::iterable().end(); ++it)
			{
				expect(it->toUnderlying(), equal_to(fixture.values[i]));
				++i;
			}
		});
		
		_.test("Iteration from fifth element with 'from()' method", [](enumFixture<EnumName>& fixture) {
			size_t i = 4;
			for(auto it = EnumName::iterable().from(EnumName::Test5); it != EnumName::iterable().end(); ++it)
			{
				expect(it->toUnderlying(), equal_to(fixture.values[i]));
				++i;
			}
		});
		
		_.test("Iteration from first element with iterator", [](enumFixture<EnumName>& fixture) {
			size_t i = 0;
			for(auto it = typename EnumName::iterator(EnumName::Test1); it != EnumName::iterable().end(); ++it)
			{
				expect(it->toUnderlying(), equal_to(fixture.values[i]));
				++i;
			}
		});
		
		_.test("Iteration from second element with iterator", [](enumFixture<EnumName>& fixture) {
			size_t i = 1;
			for(auto it = typename EnumName::iterator(EnumName::Test2); it != EnumName::iterable().end(); ++it)
			{
				expect(it->toUnderlying(), equal_to(fixture.values[i]));
				++i;
			}
		});
		
		_.test("Iteration from third element with iterator", [](enumFixture<EnumName>& fixture) {
			size_t i = 2;
			for(auto it = typename EnumName::iterator(EnumName::Test3); it != EnumName::iterable().end(); ++it)
			{
				expect(it->toUnderlying(), equal_to(fixture.values[i]));
				++i;
			}
		});
		
		_.test("Iteration from fourth element with iterator", [](enumFixture<EnumName>& fixture) {
			size_t i = 3;
			for(auto it = typename EnumName::iterator(EnumName::Test4); it != EnumName::iterable().end(); ++it)
			{
				expect(it->toUnderlying(), equal_to(fixture.values[i]));
				++i;
			}
		});
		
		_.test("Iteration from fifth element with iterator", [](enumFixture<EnumName>& fixture) {
			size_t i = 4;
			for(auto it = typename EnumName::iterator(EnumName::Test5); it != EnumName::iterable().end(); ++it)
			{
				expect(it->toUnderlying(), equal_to(fixture.values[i]));
				++i;
			}
		});
		
		_.test("Reverse iteration", [](enumFixture<EnumName>& fixture) {
			size_t i = EnumName::size() - 1;
			for(auto it = EnumName::iterable().rbegin(); it != EnumName::iterable().rend(); ++it)
			{
				expect(it->toUnderlying(), equal_to(fixture.values[i]));
				--i;
			}
		});
		
		_.test("Iterate from first element", [](enumFixture<EnumName>& fixture) {
			size_t i = 0;
			for(auto val : EnumName::iterableFrom(EnumName::Test1))
			{
				expect(val.toUnderlying(), equal_to(fixture.values[i]));
				++i;
			}
		});
		
		_.test("Iterate from second element", [](enumFixture<EnumName>& fixture) {
			size_t i = 1;
			for(auto val : EnumName::iterableFrom(EnumName::Test2))
			{
				expect(val.toUnderlying(), equal_to(fixture.values[i]));
				++i;
			}
		});
		
		_.test("Iterate from third element", [](enumFixture<EnumName>& fixture) {
			size_t i = 2;
			for(auto val : EnumName::iterableFrom(EnumName::Test3))
			{
				expect(val.toUnderlying(), equal_to(fixture.values[i]));
				++i;
			}
		});
		
		_.test("Iterate from fourth element", [](enumFixture<EnumName>& fixture) {
			size_t i = 3;
			for(auto val : EnumName::iterableFrom(EnumName::Test4))
			{
				expect(val.toUnderlying(), equal_to(fixture.values[i]));
				++i;
			}
		});
		
		_.test("Iterate from fifth element", [](enumFixture<EnumName>& fixture) {
			size_t i = 4;
			for(auto val : EnumName::iterableFrom(EnumName::Test5))
			{
				expect(val.toUnderlying(), equal_to(fixture.values[i]));
				++i;
			}
		});
	};
}

template<class EnumName>
auto improvedEnumTestCode()
{
	return [](auto& _){
		_.setup([](enumFixture<EnumName>& fixture) {
			std::copy(std::begin(EnumName::values), std::end(EnumName::values), std::begin(fixture.values));
		});
		
		_.test("Conversion of each enum value to string", [](enumFixture<EnumName>&){
			size_t i = 1;
			for(auto value : EnumName::iterable())
			{
				expect(value.toString(), equal_to(std::string{"Test"} + std::to_string(i)));
				++i;
			}
		});
		
		_.test("Iteration 2 by 2 retrieve string value", [](enumFixture<EnumName>&){
			size_t i = 1;
			for(auto it = EnumName::iterable().begin(); it < EnumName::iterable().end(); it += 2)
			{
				expect(it->toString(), equal_to(std::string{"Test"} + std::to_string(i)));
				i += 2;
			}
		});
		
		_.test("Iterate from first element", [](enumFixture<EnumName>&) {
			size_t i = 0;
			for(auto val : EnumName::iterableFrom(EnumName::Test1))
			{
				expect(val.toString(), equal_to(std::string{"Test"} + std::to_string(i + 1)));
				++i;
			}
		});
		
		_.test("Iterate from second element", [](enumFixture<EnumName>&) {
			size_t i = 1;
			for(auto val : EnumName::iterableFrom(EnumName::Test2))
			{
				expect(val.toString(), equal_to(std::string{"Test"} + std::to_string(i + 1)));
				++i;
			}
		});
		
		_.test("Iterate from third element", [](enumFixture<EnumName>&) {
			size_t i = 2;
			for(auto val : EnumName::iterableFrom(EnumName::Test3))
			{
				expect(val.toString(), equal_to(std::string{"Test"} + std::to_string(i + 1)));
				++i;
			}
		});
		
		_.test("Iterate from fourth element", [](enumFixture<EnumName>&) {
			size_t i = 3;
			for(auto val : EnumName::iterableFrom(EnumName::Test4))
			{
				expect(val.toString(), equal_to(std::string{"Test"} + std::to_string(i + 1)));
				++i;
			}
		});
		
		_.test("Iterate from fifth element", [](enumFixture<EnumName>&) {
			size_t i = 4;
			for(auto val : EnumName::iterableFrom(EnumName::Test5))
			{
				expect(val.toString(), equal_to(std::string{"Test"} + std::to_string(i + 1)));
				++i;
			}
		});
	};
}



template<class iterableEnumList, class ImprovedEnumList>
class EnumTestRunner
{
	public:
	EnumTestRunner()
	{
		runEachiterableEnumTest();
		runEachImprovedEnumTest();
	}
	
	static void runEachiterableEnumTest()
	{
		runEachiterableEnumTestImpl(std::make_integer_sequence<size_t, std::tuple_size<iterableEnumList>::value>{});
	}
	
	template<size_t ... Indices>
	static void runEachiterableEnumTestImpl(std::integer_sequence<size_t, Indices...>)
	{
		(void)std::initializer_list<bool>{(suite<enumFixture<std::remove_reference_t<decltype(std::get<Indices>(std::declval<iterableEnumList>()))>>>
										 {"Automatic enum test", iterableEnumTestCode<std::remove_reference_t<decltype(std::get<Indices>(std::declval<iterableEnumList>()))>>()}, true)...};
	}
	
	static void runEachImprovedEnumTest()
	{
		runEachImprovedEnumTestImpl(std::make_integer_sequence<size_t, std::tuple_size<ImprovedEnumList>::value>{});
	}
	
	template<size_t ... Indices>
	static void runEachImprovedEnumTestImpl(std::integer_sequence<size_t, Indices...>)
	{
		(void)std::initializer_list<bool>{
			(suite<enumFixture<std::remove_reference_t<decltype(std::get<Indices>(std::declval<ImprovedEnumList>()))>>>
			{"Automatic enum test", iterableEnumTestCode<std::remove_reference_t<decltype(std::get<Indices>(std::declval<ImprovedEnumList>()))>>()},
			suite<enumFixture<std::remove_reference_t<decltype(std::get<Indices>(std::declval<ImprovedEnumList>()))>>>
			{"Automatic enum test", improvedEnumTestCode<std::remove_reference_t<decltype(std::get<Indices>(std::declval<ImprovedEnumList>()))>>()}, true)...
		};
	}
};

EnumTestRunner<iterableEnumTestList, improvedEnumTestList> runner{};

#endif // ENUM_UTILS_TEST_HXX