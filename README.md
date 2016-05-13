#ImprovedEnum
A small header-only utility library to allow iteratable and serialiazable enum in C++14. The goal of this library is to provide features yet lacking in standard C++ in order to make enum more useful. This was also a pretty good occasion to test C++ relaxed constexpr capabilities, which are pretty amazing.

#Features
Improved enumerations comes in two flavor : iteratable enums, and stringizable enums.

Iteratable enums are simple enums with added iteration capability. Thus, iteration from an element of the enum or in a range-based loop is supported. One can also retrieve the enum name using the ```getEnumName()``` static method, and the size via the ```size()``` static method.

Stringizable enums do have the same capabilities as the aftermentionned iteratable enums, with the added possibility to retrieve the name of an element of the enum using the ```toString()``` method.

In addition, all data generation and computation is done at compile time, no more work is needed at runtime. This is thus an almost cost-free abstraction.

#How to use
The code is header only, so dropping includes files into your project and including "ImprovedEnum.hxx" should do the trick. It is also possible to compile tests using :

```make test```

The tests will then be found under the bin/$(platform)/$(configuration)/test folder. 

The testing framework used is [mettle](https://github.com/jimporter/mettle), a nice little unit testing framework using C++14.

Writing code using this library is pretty straightforward. First of all, choose one of the two macro used to generate enumerations :
```ITERATABLE_ENUM(EnumName, underlyingType, ...)
IMPROVED_ENUM(EnumName, underlyingType, ...)```

Be aware that, while it can be more convenient, the IMPROVED_ENUM macro may take a slighty longer time to generate, due to the fact that it need to generate names in addition to all the other code to take care about iteration and such. The difference should be in many case, however, negligible. In the following examples, we will use IMPROVED_ENUM, but the declaration of the enum using ITERATABLE_ENUM is strictly the same.

Here is how to declare anu enumeration :
```C++
IMPROVED_ENUM(MyEnum, size_t,
	Foo,
	Bar,
	FooBar
);
```

This kind of declaration also supports initializers like normal enumerations. For example, this is a valid declaration :
```C++
IMPROVED_ENUM(MyEnum, size_t,
	Foo,
	Bar=6,
	FooBar // Will have the value 7, like in a normal enumeration
);
```

Then, access to elements of the enum is trivial :
```C++
MyEnum val = MyEnm::Foo;
std::cout << val.toUnderlying() << std::endl; // Output : 0

val = MyEnum::FooBar;
std::cout << val.toUnderlying() << std::endl; // Output : 7
```

Iteration comes also in multiple fashion, using range based loop, or construction using ```from()``` method or constructing iterator from enumeration value.
```C++
std::cout << "Now displaying every value inside the enumeration " << MyEnum::getEnumName() << " using range based loop." << std::endl;
for(auto val : MyEnum::iterable())
{
	std::cout << val << std::endl;
}

std::cout << "Using iterable now." << std::endl;
for(auto it = MyEnum::iterable().begin(); it != MyEnum::iterable().end(); ++it)
{
	std::cout << *it << std::endl;
}

std::cout << "From MyEnum::Bar." << std::endl;
for(auto it = MyEnum::iterator{MyEnum::Bar}; auto it != MyEnum::iterable().end(); ++it)
{
	std::cout << *it << std::endl;
}
```

Last but not the least, we have the stringification of the enumeration values, like this :
```C++
MyEnum val = MyEnum::Bar;
std::cout << val.toString() << std::endl; // Output : Bar

val = MyEnum::FooBar;
std::cout << val.toString() << std::endl; // Output : FooBar
```

#Performances
As said above, in term of runtime performance this code is almost optimal, as everything is done at compile time. However, due to the metaprogramming used under the hood, it can increase compilation time. However, to add one second to the compilation time on a fairly old machine (laptop with i3 CPU), you need to declare a dozens of enumerations with 10-15 elements in each. There is obviously optimizations to make in some places, but relying on template to keep type safety has the side effect of slowing down compile time anyway, whatever you're trying to optimize.

If this seems a too big constrain to you, check the alternatives below.

#Similar projects
There is few similar projects around the internet, and more specificaly this one :
https://github.com/aantron/better-enums

Admittedly, this other project is far more mature, has some additional features that this project does not have, and is compatible with C++98, when this project requires a modern C++14 compiler. However, the iteration on the enum is less convenient, which was the initial goal of this project. Still, always nice to be aware of alternatives !
