#include <iterator>
#include <map>
#include <string>
#include <unordered_map>
#include <vector>


#include <mettle/header_only.hpp>
using namespace mettle;

#include <Range.hxx>

template<class Container,
		 std::enable_if_t<
		 	std::is_same<typename std::iterator_traits<typename Container::iterator>::iterator_category, std::bidirectional_iterator_tag>::value
		 || std::is_same<typename std::iterator_traits<typename Container::iterator>::iterator_category, std::random_access_iterator_tag>::value>*
	= nullptr>
auto generateContainerTestFor(const Container& container)
{
	return [=](){
		Container nonConstContainer = container;
		
		range<typename Container::iterator> rnge{nonConstContainer.begin(), nonConstContainer.end()};
		range<typename Container::reverse_iterator> rrnge{nonConstContainer.rbegin(), nonConstContainer.rend()};
		
		range<typename Container::const_iterator> crnge{container.begin(), container.end()};
		range<typename Container::const_reverse_iterator> crrnge{container.crbegin(), container.crend()};
	
		expect(rnge.begin(), equal_to(nonConstContainer.begin()));
		expect(rnge.end(), equal_to(nonConstContainer.end()));
		expect(rnge.size(), equal_to(nonConstContainer.size()));
		
		expect(rrnge.begin(), equal_to(nonConstContainer.rbegin()));
		expect(rrnge.end(), equal_to(nonConstContainer.rend()));
		expect(rrnge.size(), equal_to(nonConstContainer.size()));
		
		expect(crnge.begin(), equal_to(container.cbegin()));
		expect(crnge.end(), equal_to(container.cend()));
		expect(crnge.size(), equal_to(container.size()));
		
		expect(crrnge.begin(), equal_to(container.crbegin()));
		expect(crrnge.end(), equal_to(container.crend()));
		expect(crrnge.size(), equal_to(container.size()));
	};
}

template<class Container,
		 std::enable_if_t<
		 	!std::is_same<typename std::iterator_traits<typename Container::iterator>::iterator_category, std::bidirectional_iterator_tag>::value
		 && !std::is_same<typename std::iterator_traits<typename Container::iterator>::iterator_category, std::random_access_iterator_tag>::value>*
	= nullptr>
auto generateContainerTestFor(const Container& container)
{
	return [=](){
		Container nonConstContainer = container;
		
		range<typename Container::iterator> rnge{nonConstContainer.begin(), nonConstContainer.end()};

		range<typename Container::const_iterator> crnge{container.begin(), container.end()};
	
		expect(rnge.begin(), equal_to(nonConstContainer.begin()));
		expect(rnge.end(), equal_to(nonConstContainer.end()));
		expect(rnge.size(), equal_to(nonConstContainer.size()));
		
		expect(crnge.begin(), equal_to(container.cbegin()));
		expect(crnge.end(), equal_to(container.cend()));
		expect(crnge.size(), equal_to(container.size()));
	};
}

suite<> rangeSuite("Range testing", [](auto& _){
	_.test("Construct range for std::vector<int>", generateContainerTestFor(std::vector<int>{
		1, 2, 3
	}));
	
	_.test("Construct range for std::vector<float>", generateContainerTestFor(std::vector<float>{
		1.2, 2.5879, 3.9
	}));
	
	_.test("Construct range for std::vector<std::string>", generateContainerTestFor(std::vector<std::string>{
			"Hello",
			"Go",
			"Brezel"	
	}));

	_.test("Construct range for std::unordered_map<std::string, int>", generateContainerTestFor(		
		std::unordered_map<std::string, int>{
			{"Hello", 5},
			{"Go", 7},
			{"Brezel", 9}	
	}));
	
	_.test("Construct range for std::array<std::string>", generateContainerTestFor(
		std::array<std::string, 6>{{
			"Hello",
			"Nop",
			"Maybe",
			"A test",
			"Another test",
			"Yet another test"	
		}}
	));
	
	_.test("Construct range for std::array<int>", generateContainerTestFor(
		std::array<int, 6>{{
			5, 275, 40, 27, 259798, 0x965	
		}}
	));
	
	_.test("Construct range for std::array<float>", generateContainerTestFor(
		std::array<float, 6>{{
			5.0, 275.0, 40.0, 27.0, 259798.0, 564.2	
		}}
	));
	
	_.test("Construct range for map<std::string, int>", generateContainerTestFor(
		std::map<std::string, int>{
			{"Hello", 1},
			{"Brezel", 59},
			{"Godrik", 789},
			{"Potato", 1},
			{"Random", 26},
			{"More random", 74},
			{"Lol so random", 654},
			{"Pretty neat", 89},
			{"Got out of ideas", 1564},
			{"Last random ?", 5}	
	}));
	
	_.test("Construct range for std::string", generateContainerTestFor(
		std::string{"The brown fox jumps over the lazy dog."}
	));
});