#include <array>
#include <string>

#include <mettle/header_only.hpp>
using namespace mettle;

#include <ConstString.hxx>

suite<> constStringSuite("Testing suite for ConstString", [](auto& _){
	_.test("Testing construction from literal string", []() {
		ConstString str = "Foo";
		
		expect(str, equal_to(std::string{"Foo"}));
	});
	
	_.test("Testing construction from an other ConstString", []() {
		ConstString str1 = "Foo";
		ConstString str2 = str1;
		
		expect(str2, equal_to(str1));
	});
	
	_.test("Testing string construction from ConstString", []() {
		ConstString cstr = "Foo";
		std::string realStr{cstr};
		
		expect(realStr, equal_to(cstr));
	});
	
	_.test("Test ConstString construction from std::array<char>", []() {
		std::array<char, 12> arr{{
			'H', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd', '!'
		}};
		
		ConstString str = arr;

		expect(str, equal_to(std::string{"Hello world!"}));
	});
	
	_.test("Test ConstString construction from std::string", []() {
		std::string realStr = "Foo";
		ConstString cstr = realStr;
		
		expect(cstr, equal_to(realStr));
	});
	
	_.test("Test ConstString range based loop iteration",  []() {
		ConstString str = "Hello world !";
		size_t i = 0;
		
		for(char c : str)
		{
			expect(c, equal_to(str[i]));
			++i;
		}
	});
	
	_.test("Test find method", []() {
		ConstString str = "I got only on A in this sentence";
		auto findit = str.find('A');
		
		expect(*findit, equal_to('A'));
	});
	
	_.test("Test size validity", []() {
		ConstString str1 = "Foo";
		ConstString str2 = "Hello world !";
		
		expect(str1.size(), equal_to(3));
		expect(str2.size(), equal_to(13));
	});
	
	_.test("Test raw data retrieval", []() {
		ConstString str = "Foo";
		auto data = str.data();
		
		expect(data[0], equal_to('F'));
		expect(data[1], equal_to('o'));
		expect(data[2], equal_to('o'));
		expect(data[3], equal_to('\0'));
	});
});