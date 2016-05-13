#include <deque>
#include <forward_list>
#include <iterator>
#include <list>
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
	
	/* Tests on few standard containers */
	_.test("Construct range for std::vector<int>", generateContainerTestFor(std::vector<int>{
		1, 2, 3
	}));
	
	_.test("Construct range for std::vector<float>", generateContainerTestFor(std::vector<float>{
		1.2, 2.5879, 3.9
	}));
	
	_.test("Construct range for std::vector<bool>", generateContainerTestFor(std::vector<bool>{
		true, true, false, false, true, true, true, true, false, true, false, false, true
	}));
	
	_.test("Construct range for std::vector<std::string>", generateContainerTestFor(std::vector<std::string>{
		"Hello",
		"Go",
		"Brezel"
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
	
	_.test("Construct range for std::deque<int>", generateContainerTestFor(
		std::deque<int>{
			5, 89, 78, 565, 12, 654, 10
	}));
	
	_.test("Construct range for std::deque<float>", generateContainerTestFor(
		std::deque<float>{
			5.56, 89.156, 78.0, 565.0, 12.0, 654.0, 10.0
	}));
	
	_.test("Construct range for std::list<int>", generateContainerTestFor(
		std::list<int>{
			5, 89, 78, 565, 12, 654, 10
	}));
	
	_.test("Construct range for std::list<float>", generateContainerTestFor(
		std::list<float>{
			5.56, 89.156, 78.0, 565.0, 12.0, 654.0, 10.0
	}));
	
	_.test("Construct range for std::forward_list<int>", [](){
		std::forward_list<int> container{
			5, 89, 78, 565, 12, 654, 10
		};

		range<typename decltype(container)::iterator> rnge{container.begin(), container.end()};
		range<typename decltype(container)::const_iterator> crnge{container.cbegin(), container.cend()};
		
		expect(rnge.begin(), equal_to(container.begin()));
		expect(rnge.end(), equal_to(container.end()));
		
		expect(crnge.begin(), equal_to(container.cbegin()));
		expect(crnge.end(), equal_to(container.cend()));
	});
	
	_.test("Construct range for std::forward_list<int>", [](){
		std::forward_list<float> container{
			5.58, 89.59879, 78.0, 565.0, 12.0, 654.0, 10.0
		};

		range<typename decltype(container)::iterator> rnge{container.begin(), container.end()};
		range<typename decltype(container)::const_iterator> crnge{container.cbegin(), container.cend()};
		
		expect(rnge.begin(), equal_to(container.begin()));
		expect(rnge.end(), equal_to(container.end()));
		
		expect(crnge.begin(), equal_to(container.cbegin()));
		expect(crnge.end(), equal_to(container.cend()));
	});
	
	_.test("Test pop_front() on range<iterator>", [](){
		std::vector<int> vec{1, 2, 56, 78, 95, 27};
		
		range<typename decltype(vec)::iterator> rnge{vec.begin(), vec.end()};
		
		expect(rnge.begin(), equal_to(vec.begin()));
		rnge.pop_front();
		
		expect(rnge.begin(), equal_to(vec.begin() + 1));
		rnge.pop_front();
		
		expect(rnge.begin(), equal_to(vec.begin() + 2));
	});
	
	_.test("Test pop_front() on range<const_iterator>", [](){
		std::vector<int> vec{1, 2, 56, 78, 95, 27};
		
		range<typename decltype(vec)::const_iterator> rnge{vec.cbegin(), vec.cend()};
		
		expect(rnge.begin(), equal_to(vec.cbegin()));
		rnge.pop_front();
		
		expect(rnge.begin(), equal_to(vec.cbegin() + 1));
		rnge.pop_front();
		
		expect(rnge.begin(), equal_to(vec.cbegin() + 2));
	});
	
	_.test("Test pop_front() on range<reverse_iterator>", [](){
		std::vector<int> vec{1, 2, 56, 78, 95, 27};
		
		range<typename decltype(vec)::reverse_iterator> rnge{vec.rbegin(), vec.rend()};
		
		expect(rnge.begin(), equal_to(vec.rbegin()));
		rnge.pop_front();
		
		expect(rnge.begin(), equal_to(vec.rbegin() + 1));
		rnge.pop_front();
		
		expect(rnge.begin(), equal_to(vec.rbegin() + 2));
	});
	
	_.test("Test pop_front() on range<const_reverse_iterator>", [](){
		std::vector<int> vec{1, 2, 56, 78, 95, 27};
		
		range<typename decltype(vec)::const_reverse_iterator> rnge{vec.crbegin(), vec.crend()};
		
		expect(rnge.begin(), equal_to(vec.crbegin()));
		rnge.pop_front();
		
		expect(rnge.begin(), equal_to(vec.crbegin() + 1));
		rnge.pop_front();
		
		expect(rnge.begin(), equal_to(vec.crbegin() + 2));
	});
		
	_.test("Test pop_back() on range<iterator>", [](){
		std::vector<int> vec{1, 2, 56, 78, 95, 27};
		
		range<typename decltype(vec)::iterator> rnge{vec.begin(), vec.end()};
		
		expect(rnge.end(), equal_to(vec.end()));
		rnge.pop_back();
		
		expect(rnge.end(), equal_to(vec.end() - 1));
		rnge.pop_back();
		
		expect(rnge.end(), equal_to(vec.end() - 2));
	});
	
	_.test("Test pop_back() on range<const_iterator>", [](){
		std::vector<int> vec{1, 2, 56, 78, 95, 27};
		
		range<typename decltype(vec)::const_iterator> rnge{vec.cbegin(), vec.cend()};
		
		expect(rnge.end(), equal_to(vec.cend()));
		rnge.pop_back();
		
		expect(rnge.end(), equal_to(vec.cend() - 1));
		rnge.pop_back();
		
		expect(rnge.end(), equal_to(vec.cend() - 2));
	});
	
	_.test("Test pop_back() on range<reverse_iterator>", [](){
		std::vector<int> vec{1, 2, 56, 78, 95, 27};
		
		range<typename decltype(vec)::reverse_iterator> rnge{vec.rbegin(), vec.rend()};
		
		expect(rnge.end(), equal_to(vec.rend()));
		rnge.pop_back();
		
		expect(rnge.end(), equal_to(vec.rend() - 1));
		rnge.pop_back();
		
		expect(rnge.end(), equal_to(vec.rend() - 2));
	});
	
	_.test("Test pop_back() on range<const_reverse_iterator>", [](){
		std::vector<int> vec{1, 2, 56, 78, 95, 27};
		
		range<typename decltype(vec)::const_reverse_iterator> rnge{vec.crbegin(), vec.crend()};
		
		expect(rnge.end(), equal_to(vec.crend()));
		rnge.pop_back();
		
		expect(rnge.end(), equal_to(vec.crend() - 1));
		rnge.pop_back();
		
		expect(rnge.end(), equal_to(vec.crend() - 2));
	});
	
	_.test("Test back()", [](){
		std::vector<int> vec{1, 2, 56, 78, 95, 27};
		
		range<typename decltype(vec)::iterator> rnge{vec.begin(), vec.end()};
		
		expect(rnge.back(), equal_to(vec.back()));
		rnge.pop_back();
		
		expect(rnge.back(), equal_to(*(vec.end() - 2)));
	});
	
	_.test("Test front()", [](){
		std::vector<int> vec{1, 2, 56, 78, 95, 27};
		
		range<typename decltype(vec)::iterator> rnge{vec.begin(), vec.end()};
		
		expect(rnge.front(), equal_to(vec.front()));
		rnge.pop_front();
		
		expect(rnge.front(), equal_to(*(vec.begin() + 1)));
	});
	
	_.test("Emptiness conditions", [](){
		std::vector<int> vec{1, 2, 56, 78, 95, 27};
		
		range<typename decltype(vec)::iterator> rnge{vec.begin(), vec.begin()};
		range<typename decltype(vec)::iterator> rnge2{vec.begin(), vec.begin() + 1};
		
		expect(rnge.empty(), equal_to(true));
		expect(rnge.size(), equal_to(0));	
		
		expect(rnge2.empty(), equal_to(false));
		expect(rnge2.size(), equal_to(1));
		
		rnge2.pop_back();
		
		expect(rnge2.empty(), equal_to(true));
		expect(rnge.size(), equal_to(0));
	});
	
	_.test("Construction of a range from an iterator pair", [](){
		std::vector<int> vec{1, 2, 56, 78, 95, 27};
		
		using iterator = typename decltype(vec)::iterator;
		using const_iterator = typename decltype(vec)::const_iterator;
		
		std::pair<iterator, iterator> pair1{vec.begin(), vec.begin() + 2};
		range<iterator> rnge1{pair1};
		
		expect(rnge1.begin(), equal_to(pair1.first));
		expect(rnge1.begin(), equal_to(vec.begin()));
		
		std::pair<const_iterator, const_iterator> pair2{vec.cbegin(), vec.cend()};
		range<const_iterator> rnge2{pair2};
		range<const_iterator> rnge3{pair1};
		
		expect(rnge2.begin(), equal_to(pair2.first));
		expect(rnge2.begin(), equal_to(vec.cbegin()));
		
		expect(rnge3.begin(), equal_to(rnge1.begin()));
		expect(rnge3.begin(), equal_to(vec.cbegin()));
		expect(rnge3.begin(), equal_to(pair1.first));
	});
	
	_.test("Test range copying", [](){
		std::vector<int> vec{1, 2, 56, 78, 95, 27};
		
		using iterator = typename decltype(vec)::iterator;
		using const_iterator = typename decltype(vec)::const_iterator;

		range<iterator> rnge1{vec.begin(), vec.end()};
		range<iterator> rnge2{rnge1};
		
		expect(rnge1.begin(), equal_to(rnge2.begin()));
		expect(rnge1.end(), equal_to(rnge2.end()));
		expect(rnge1.size(), equal_to(rnge2.size()));
		
		range<const_iterator> rnge3{rnge1};	
		
		expect(rnge1.begin(), equal_to(rnge3.begin()));
		expect(rnge1.end(), equal_to(rnge3.end()));
		expect(rnge1.size(), equal_to(rnge3.size()));
	});
});