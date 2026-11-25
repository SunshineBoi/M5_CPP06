#include "Serializer.hpp"
#include <iostream>

// Send ptr through serialize() and deserialize(), and print each step.
static Data*	round_trip(Data* ptr)
{
	uintptr_t	raw = Serializer::serialize(ptr);
	Data*		back = Serializer::deserialize(raw);

	std::cout << "original pointer : " << ptr << std::endl;
	std::cout << "serialized (dec) : " << raw << std::endl;
	std::cout << "serialized (hex) : 0x" << std::hex << raw << std::dec << std::endl;
	std::cout << "restored pointer : " << back << std::endl;
	std::cout << "pointers equal   : " << (back == ptr ? "yes" : "no") << std::endl;
	return (back);
}

// data must not be NULL.
static void	print_data(const Data* data)
{
	std::cout << "data             : id = " << data->id
		<< ", name = " << data->name << std::endl;
}

int	main(void)
{
	std::cout << "===== 1. object on the stack =====" << std::endl;
	{
		Data	data;
		Data*	back;

		data.id = 42;
		data.name = "forty-two";
		back = round_trip(&data);
		print_data(back);
	}

	std::cout << "\n===== 2. same object, not a copy =====" << std::endl;
	{
		Data	data;
		Data*	back;

		data.id = 1;
		data.name = "before";
		back = round_trip(&data);
		back->name = "changed through back";
		print_data(&data);
	}

	std::cout << "\n===== 3. object on the heap =====" << std::endl;
	{
		Data*	data = new Data;
		Data*	back;

		data->id = 21;
		data->name = "twenty-one";
		back = round_trip(data);
		print_data(back);
		delete back;
	}

	std::cout << "\n===== 4. NULL pointer =====" << std::endl;
	{
		round_trip(NULL);
	}
	return (0);
}
