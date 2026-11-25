
#include "ScalarConverter.hpp"
#include <cctype>  // isdigit
#include <limits>
#include <climits>	 // INT_MAX
#include <cstdlib>  // strtol, strtod
#include <iomanip>  // setprecision

ScalarConverter::ScalarConverter() {}

ScalarConverter::ScalarConverter(const ScalarConverter&) {}

ScalarConverter& ScalarConverter::operator=(const ScalarConverter&) { return *this; }

ScalarConverter::~ScalarConverter() {}

typedef	enum e_type
{
	CHAR,
	INT,
	FLOAT,
	DOUBLE,
	INVALID
}	t_type;

// Move i past the digits. Return false if there are none.
static bool	skip_digits(const std::string& str, size_t& i)
{
	size_t	start = i;

	while (std::isdigit(static_cast<unsigned char>(str[i])))
		i++;
	return (i > start);
}

static t_type	get_num_type(const std::string& str)
{
	size_t	i = 0;
	t_type	type = INT;

	if (str[i] == '+' || str[i] == '-')
		i++;
	if (!skip_digits(str, i))
		return (INVALID);
	if (str[i] == '.')
	{
		type = DOUBLE;
		i++;
		if (!skip_digits(str, i))
			return (INVALID);
		if (str[i] == 'f')
		{
			type = FLOAT;
			i++;
		}
	}
	if (str[i] != '\0')
		return (INVALID);
	return (type);
}

static t_type	get_type(const std::string& str, bool *is_literal)
{
	if (str == "")
		return (INVALID);
	if (str == "nan" || str == "+inf" || str == "-inf")
	{
		*is_literal = true;
		return (DOUBLE);
	}
	else if (str == "nanf" || str == "+inff" || str == "-inff")
	{
		*is_literal = true;
		return (FLOAT);
	}
	else if (str.length() == 1
		&& std::isprint(static_cast<unsigned char>(str[0]))
		&& !std::isdigit(static_cast<unsigned char>(str[0])))
		return (CHAR);
	else
		return (get_num_type(str));
}

static void	print_char(double val)
{
	std::cout << "char: ";
	if (val < 0 || val > 127 || val != val)
		std::cout << "impossible";
	else if (!std::isprint(static_cast<char>(val)))
		std::cout << "Non displayable";
	else
		std::cout
			<< "'"
			<< static_cast<unsigned char>(val)
			<< "'";
	std::cout << std::endl;
}

static void	print_int(double val)
{
	std::cout << "int: ";
	if (val < INT_MIN || val > INT_MAX || val != val)
		std::cout << "impossible";
	else
		std::cout << static_cast<int>(val);	
	std::cout << std::endl;
}

static void	print_float(double val)
{
	float inf = std::numeric_limits<float>::infinity();
	float maxf = std::numeric_limits<float>::max();

	std::cout << "float: ";
	if (val != val)
		std::cout << "nanf";
	else if (val == inf)
		std::cout << "+inff";
	else if (val == -inf)
		std::cout << "-inff";
	else if (val < -maxf || val > maxf)
		std::cout << "impossible";
	else
		std::cout 
			<< std::fixed 
			<< std::setprecision(1)
			<< static_cast<float>(val)
			<< "f";
	std::cout << std::endl;
}

static void	print_double(double val)
{
	double inf = std::numeric_limits<double>::infinity();

	std::cout << "double: ";
	if (val != val)
		std::cout << "nan";
	else if (val == inf)
		std::cout << "+inf";
	else if (val == -inf)
		std::cout << "-inf";
	else
		std::cout 
			<< std::fixed 
			<< std::setprecision(1)
			<< val;
	std::cout << std::endl;
}

static void	print_err(const std::string& message)
{
	std::cerr
		<< "Error!\n"
		<< message << std::endl;
}

static void	print_result(double val)
{
	print_char(val);
	print_int(val);
	print_float(val);
	print_double(val);
}

static void	set_literal(const std::string& str, double* dest)
{
	if (str == "nan" || str == "nanf")
		*dest = std::numeric_limits<double>::quiet_NaN();
	else if (str == "+inf" || str == "+inff")
		*dest = std::numeric_limits<double>::infinity();
	else if (str == "-inf" || str == "-inff")
		*dest = -std::numeric_limits<double>::infinity();
}

static bool	parse_int(const std::string& str, double& val)
{
	long	n;

	n = std::strtol(str.c_str(), NULL, 10);
	if (n > INT_MAX || n < INT_MIN)
		return (false);
	val = static_cast<double>(static_cast<int>(n));
	return (true);
}

static bool	parse_float(const std::string& str, double& val, bool is_literal)
{
	double	d;
	double	max_f = std::numeric_limits<float>::max();

	if (is_literal)
		set_literal(str, &val);
	else
	{
		d = std::strtod(str.c_str(), NULL);  // inf if overflow
		if (d > max_f || d < -max_f)
			return (false);
		// reduce the precision first, then cast back to double
		val = static_cast<double>(static_cast<float>(d));
	}
	return (true);
}

static bool	parse_double(const std::string& str, double& val, bool is_literal)
{
	double	max_d = std::numeric_limits<double>::max();

	if (is_literal)
		set_literal(str, &val);
	else
	{
		val = std::strtod(str.c_str(), NULL);  // inf if overflow
		if (val > max_d || val < -max_d)
			return (false);
	}
	return (true);
}

void	ScalarConverter::convert(const std::string& str)
{
	bool	is_literal = false;
	bool	ok = true;
	double	val = 0.0;
	t_type	type = get_type(str, &is_literal);

	if (type == CHAR)
		val = static_cast<double>(str[0]);
	else if (type == INT)
		ok = parse_int(str, val) || parse_double(str, val, false);
	else if (type == FLOAT)
		ok = parse_float(str, val, is_literal) || parse_double(str, val, false);
	else if (type == DOUBLE)
		ok = parse_double(str, val, is_literal);
	if (type == INVALID)
		return (print_err("ex00: bad input"));
	if (!ok)
		return (print_err("ex00: impossible, value is too large"));
	print_result(val);
}
