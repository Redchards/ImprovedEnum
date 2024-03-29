#ifndef ENUM_UTILS_TEST_HXX
#define ENUM_UTILS_TEST_HXX

#include <iterator>
#include <tuple>

#include <mettle/header_only.hpp>
using namespace mettle;

#include <ImprovedEnum.hxx>

ITERABLE_ENUM(IterableEnumTst1, size_t,
	Test1,
	Test2,
	Test3,
	Test4,
	Test5
);

ITERABLE_ENUM(IterableEnumTst2, size_t,
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

ITERABLE_ENUM(IterableEnumTst3, size_t,
	Test1,
	Test2 = 9,
	Test3 = 12,
	Test4 = 27,
	Test5
);

ITERABLE_ENUM(IterableEnumTst4, size_t,
	Test1,
	Test2 = 9,
	Test3 = 13,
	Test4 = 27,
	Test5 = 12,
	OtherTest = 50,
	LastTest = 28
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

using IteratableEnumTestList = 
	std::tuple<
		IterableEnumTst1,
		IterableEnumTst2,
		IterableEnumTst3,
		IterableEnumTst4
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
	std::decay_t<decltype(EnumName::values())> values;
};

template<class EnumName>
auto IterableEnumTestCode()
{
	return [](auto &_) {
		_.setup([](enumFixture<EnumName>& fixture) {
			std::copy(std::begin(EnumName::values()), std::end(EnumName::values()), std::begin(fixture.values));
		});
		
		_.test("Iteration test", [](enumFixture<EnumName>& fixture) {
			size_t i = 0;
			for(auto value : EnumName::iter())
			{
				expect(value.to_value(), equal_to(fixture.values[i]));
				++i;
			}
		});
	
		_.test("Index of first element", [](enumFixture<EnumName>&) {
			EnumName tst = EnumName::Test1;
			expect(tst.get_index(), equal_to(0));	  
		});
		
		_.test("Index of second element", [](enumFixture<EnumName>&) {
			EnumName tst = EnumName::Test2;
			expect(tst.get_index(), equal_to(1));	  
		});
		
		_.test("Index of third element", [](enumFixture<EnumName>&) {
			EnumName tst = EnumName::Test3;
			expect(tst.get_index(), equal_to(2));	  
		});
		
		_.test("Index of fourth element", [](enumFixture<EnumName>&) {
			EnumName tst = EnumName::Test4;
			expect(tst.get_index(), equal_to(3));	  
		});
		
		_.test("Index of fifth element", [](enumFixture<EnumName>&) {
			EnumName tst = EnumName::Test4;
			expect(tst.get_index(), equal_to(3));	  
		});
		
		_.test("Iteration from first element with 'from()' method", [](enumFixture<EnumName>& fixture) {
			size_t i = 0;
			for(auto it = EnumName::iter().from(EnumName::Test1); it != EnumName::iter().end(); ++it)
			{
				expect(it->to_value(), equal_to(fixture.values[i]));
				++i;
			}
		});
		
		_.test("Iteration from second element with 'from()' method", [](enumFixture<EnumName>& fixture) {
			size_t i = 1;
			for(auto it = EnumName::iter().from(EnumName::Test2); it != EnumName::iter().end(); ++it)
			{
				expect(it->to_value(), equal_to(fixture.values[i]));
				++i;
			}
		});
		
		_.test("Iteration from third element with 'from()' method", [](enumFixture<EnumName>& fixture) {
			size_t i = 2;
			for(auto it = EnumName::iter().from(EnumName::Test3); it != EnumName::iter().end(); ++it)
			{
				expect(it->to_value(), equal_to(fixture.values[i]));
				++i;
			}
		});
		
		_.test("Iteration from fourth element with 'from()' method", [](enumFixture<EnumName>& fixture) {
			size_t i = 3;
			for(auto it = EnumName::iter().from(EnumName::Test4); it != EnumName::iter().end(); ++it)
			{
				expect(it->to_value(), equal_to(fixture.values[i]));
				++i;
			}
		});
		
		_.test("Iteration from fifth element with 'from()' method", [](enumFixture<EnumName>& fixture) {
			size_t i = 4;
			for(auto it = EnumName::iter().from(EnumName::Test5); it != EnumName::iter().end(); ++it)
			{
				expect(it->to_value(), equal_to(fixture.values[i]));
				++i;
			}
		});
		
		_.test("Iteration from first element with iterator", [](enumFixture<EnumName>& fixture) {
			size_t i = 0;
			for(auto it = typename EnumName::iterator(EnumName::Test1); it != EnumName::iter().end(); ++it)
			{
				expect(it->to_value(), equal_to(fixture.values[i]));
				++i;
			}
		});
		
		_.test("Iteration from second element with iterator", [](enumFixture<EnumName>& fixture) {
			size_t i = 1;
			for(auto it = typename EnumName::iterator(EnumName::Test2); it != EnumName::iter().end(); ++it)
			{
				expect(it->to_value(), equal_to(fixture.values[i]));
				++i;
			}
		});
		
		_.test("Iteration from third element with iterator", [](enumFixture<EnumName>& fixture) {
			size_t i = 2;
			for(auto it = typename EnumName::iterator(EnumName::Test3); it != EnumName::iter().end(); ++it)
			{
				expect(it->to_value(), equal_to(fixture.values[i]));
				++i;
			}
		});
		
		_.test("Iteration from fourth element with iterator", [](enumFixture<EnumName>& fixture) {
			size_t i = 3;
			for(auto it = typename EnumName::iterator(EnumName::Test4); it != EnumName::iter().end(); ++it)
			{
				expect(it->to_value(), equal_to(fixture.values[i]));
				++i;
			}
		});
		
		_.test("Iteration from fifth element with iterator", [](enumFixture<EnumName>& fixture) {
			size_t i = 4;
			for(auto it = typename EnumName::iterator(EnumName::Test5); it != EnumName::iter().end(); ++it)
			{
				expect(it->to_value(), equal_to(fixture.values[i]));
				++i;
			}
		});
		
		_.test("Reverse iteration", [](enumFixture<EnumName>& fixture) {
			size_t i = EnumName::size() - 1;
			for(auto it = EnumName::iter().rbegin(); it != EnumName::iter().rend(); ++it)
			{
				expect(it->to_value(), equal_to(fixture.values[i]));
				--i;
			}
		});
		
		_.test("Iterate from first element", [](enumFixture<EnumName>& fixture) {
			size_t i = 0;
			for(auto val : EnumName::iter_from(EnumName::Test1))
			{
				expect(val.to_value(), equal_to(fixture.values[i]));
				++i;
			}
		});
		
		_.test("Iterate from second element", [](enumFixture<EnumName>& fixture) {
			size_t i = 1;
			for(auto val : EnumName::iter_from(EnumName::Test2))
			{
				expect(val.to_value(), equal_to(fixture.values[i]));
				++i;
			}
		});
		
		_.test("Iterate from third element", [](enumFixture<EnumName>& fixture) {
			size_t i = 2;
			for(auto val : EnumName::iter_from(EnumName::Test3))
			{
				expect(val.to_value(), equal_to(fixture.values[i]));
				++i;
			}
		});
		
		_.test("Iterate from fourth element", [](enumFixture<EnumName>& fixture) {
			size_t i = 3;
			for(auto val : EnumName::iter_from(EnumName::Test4))
			{
				expect(val.to_value(), equal_to(fixture.values[i]));
				++i;
			}
		});
		
		_.test("Iterate from fifth element", [](enumFixture<EnumName>& fixture) {
			size_t i = 4;
			for(auto val : EnumName::iter_from(EnumName::Test5))
			{
				expect(val.to_value(), equal_to(fixture.values[i]));
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
			std::copy(std::begin(EnumName::values()), std::end(EnumName::values()), std::begin(fixture.values));
		});
		
		_.test("Conversion of each enum value to string", [](enumFixture<EnumName>&){
			size_t i = 1;
			for(auto value : EnumName::iter())
			{
				expect(value.to_string(), equal_to(std::string{"Test"} + std::to_string(i)));
				++i;
			}
		});
		
		_.test("Iteration 2 by 2 retrieve string value", [](enumFixture<EnumName>&){
			size_t i = 1;
			for(auto it = EnumName::iter().begin(); it < EnumName::iter().end(); it += 2)
			{
				expect(it->to_string(), equal_to(std::string{"Test"} + std::to_string(i)));
				i += 2;
			}
		});
		
		_.test("Iterate from first element", [](enumFixture<EnumName>&) {
			size_t i = 0;
			for(auto val : EnumName::iter_from(EnumName::Test1))
			{
				expect(val.to_string(), equal_to(std::string{"Test"} + std::to_string(i + 1)));
				++i;
			}
		});
		
		_.test("Iterate from second element", [](enumFixture<EnumName>&) {
			size_t i = 1;
			for(auto val : EnumName::iter_from(EnumName::Test2))
			{
				expect(val.to_string(), equal_to(std::string{"Test"} + std::to_string(i + 1)));
				++i;
			}
		});
		
		_.test("Iterate from third element", [](enumFixture<EnumName>&) {
			size_t i = 2;
			for(auto val : EnumName::iter_from(EnumName::Test3))
			{
				expect(val.to_string(), equal_to(std::string{"Test"} + std::to_string(i + 1)));
				++i;
			}
		});
		
		_.test("Iterate from fourth element", [](enumFixture<EnumName>&) {
			size_t i = 3;
			for(auto val : EnumName::iter_from(EnumName::Test4))
			{
				expect(val.to_string(), equal_to(std::string{"Test"} + std::to_string(i + 1)));
				++i;
			}
		});
		
		_.test("Iterate from fifth element", [](enumFixture<EnumName>&) {
			size_t i = 4;
			for(auto val : EnumName::iter_from(EnumName::Test5))
			{
				expect(val.to_string(), equal_to(std::string{"Test"} + std::to_string(i + 1)));
				++i;
			}
		});
	};
}



template<class IteratableEnumList, class ImprovedEnumList>
class EnumTestRunner
{
	public:
	EnumTestRunner()
	{
		runEachIteratableEnumTest();
		runEachImprovedEnumTest();
	}
	
	static void runEachIteratableEnumTest()
	{
		runEachIteratableEnumTestImpl(std::make_integer_sequence<size_t, std::tuple_size<IteratableEnumList>::value>{});
	}
	
	template<size_t ... Indices>
	static void runEachIteratableEnumTestImpl(std::integer_sequence<size_t, Indices...>)
	{
		(void)std::initializer_list<bool>{(suite<enumFixture<std::remove_reference_t<decltype(std::get<Indices>(std::declval<IteratableEnumList>()))>>>
										 {"Automatic enum test", IterableEnumTestCode<std::remove_reference_t<decltype(std::get<Indices>(std::declval<IteratableEnumList>()))>>()}, true)...};
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
			{"Automatic enum test", IterableEnumTestCode<std::remove_reference_t<decltype(std::get<Indices>(std::declval<ImprovedEnumList>()))>>()},
			suite<enumFixture<std::remove_reference_t<decltype(std::get<Indices>(std::declval<ImprovedEnumList>()))>>>
			{"Automatic enum test", improvedEnumTestCode<std::remove_reference_t<decltype(std::get<Indices>(std::declval<ImprovedEnumList>()))>>()}, true)...
		};
	}
};

EnumTestRunner<IteratableEnumTestList, improvedEnumTestList> runner{};

#endif // ENUM_UTILS_TEST_HXX