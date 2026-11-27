#include <iostream>
#include <cstdlib>
#include <ctime>
#include <exception>
#include "Base.hpp"
#include "A.hpp"
#include "B.hpp"
#include "C.hpp"

Base*	generate(void)
{
	int r = std::rand() % 3;

	if (r == 0)
		return (new A());
	else if (r == 1)
		return (new B());
	else
		return (new C());
}

void	identify(Base* p)
{
	if (dynamic_cast<A*>(p) != NULL)
		std::cout << "A" << std::endl;
	else if (dynamic_cast<B*>(p) != NULL)
		std::cout << "B" << std::endl;
	else if (dynamic_cast<C*>(p) != NULL)
		std::cout << "C" << std::endl;
	else
		std::cout << "Unknown type" << std::endl;
}

void	identify(Base& p)
{
	try
	{
		(void)dynamic_cast<A&>(p);
		std::cout << "A" << std::endl;
		return ;
	}
	catch (std::exception&) {}
	try
	{
		(void)dynamic_cast<B&>(p);
		std::cout << "B" << std::endl;
		return ;
	}
	catch (std::exception&) {}
	try
	{
		(void)dynamic_cast<C&>(p);
		std::cout << "C" << std::endl;
		return ;
	}
	catch (std::exception&)
	{
		std::cout << "Unknown type" << std::endl;
	}
}

static void	test_known(const char* expected, Base& obj)
{
	std::cout << "expected " << expected << std::endl;
	std::cout << "  pointer   : ";
	identify(&obj);
	std::cout << "  reference : ";
	identify(obj);
}

int	main(void)
{
	std::srand(static_cast<unsigned int>(std::time(NULL)));

	// Test 1: random objects. The two answers must be the same.
	std::cout << "=== Test 1: random objects ===" << std::endl;
	for (int i = 1; i <= 10; i++)
	{
		Base*	p = generate();

		std::cout << "object " << i << std::endl;
		std::cout << "  pointer   : ";
		identify(p);
		std::cout << "  reference : ";
		identify(*p);
		delete p;
	}

	// Test 2: objects of a known class. Each answer must match "expected".
	std::cout << std::endl << "=== Test 2: known objects ===" << std::endl;
	{
		A		a;
		B		b;
		C		c;
		Base	base;

		test_known("A", a);
		test_known("B", b);
		test_known("C", c);
		test_known("Unknown type", base);
	}

	// Test 3: NULL. Only the pointer version can take it.
	std::cout << std::endl << "=== Test 3: NULL pointer ===" << std::endl;
	{
		Base*	none = NULL;

		std::cout << "expected Unknown type" << std::endl;
		std::cout << "  pointer   : ";
		identify(none);
	}
	return (0);
}
