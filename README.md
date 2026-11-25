*This project has been created as part of the 42 curriculum by kong.*

# CPP Module 06 — C++ Casts

## Description

This module is about **type conversion** in C++. C has one cast for every
situation. C++ splits it into four named casts, and each one states what
kind of conversion it does, so a reader (and the compiler) can check it.

Each exercise is solved with one specific type of cast, and the choice is
explained at the defense.

| Exercise | Topic | Class |
|---|---|---|
| `ex00` | Conversion of scalar types | `ScalarConverter`, with one static method `convert` |

`ex01` (serialization) and `ex02` (identify real type) are not done yet.

`ScalarConverter::convert` takes a C++ literal as a string, finds its type
(`char`, `int`, `float` or `double`), converts the string to that type, and
prints the value as all four types:

```
$ ./convert 42.0f
char: '*'
int: 42
float: 42.0f
double: 42.0
```

## Instructions

The exercise has its own `Makefile`, which builds one program:

| Exercise | Program |
|---|---|
| `ex00` | `convert` |

To build and run the exercise:

1. Go to the exercise directory: `cd ex00`.
2. Run `make`.
3. Run the program with one literal, for example `./convert 42.0f`.

To check for memory errors, run the program with valgrind:

```sh
valgrind --leak-check=full ./convert 42.0f
```

The `Makefile` rules:

```sh
make        # build the program
make clean  # remove the object files
make fclean # remove the object files and the program
make re     # run fclean, then build again
```

The compiler flags are `c++ -Wall -Wextra -Werror -std=c++98`.

About the tests:

- `main.cpp` passes its one argument to `ScalarConverter::convert`.
- With no argument, or more than one, the program prints an error and
  returns 1.
- Compare the four output lines with this table:

| Command | char | int | float | double |
|---|---|---|---|---|
| `./convert 0` | Non displayable | 0 | 0.0f | 0.0 |
| `./convert a` | 'a' | 97 | 97.0f | 97.0 |
| `./convert 42.0f` | '*' | 42 | 42.0f | 42.0 |
| `./convert -42` | impossible | -42 | -42.0f | -42.0 |
| `./convert nan` | impossible | impossible | nanf | nan |
| `./convert -inff` | impossible | impossible | -inff | -inf |
| `./convert 2147483647` | impossible | 2147483647 | 2147483648.0f | 2147483647.0 |
| `./convert 2147483648` | impossible | impossible | 2147483648.0f | 2147483648.0 |

- `./convert hello`, `./convert 4.2.1`, `./convert 42f` and `./convert ""`
  print `ex00: bad input`.

## Technical Concepts

### 1. Casts in C++

| Cast | What it converts | Checked |
|---|---|---|
| `static_cast` | Related types: number to number, base to derived class and back | At compile time |
| `dynamic_cast` | A base pointer or reference to a derived type | At runtime |
| `reinterpret_cast` | The same bits read as an unrelated type | Not checked |
| `const_cast` | Adds or removes `const` | At compile time |

- **`static_cast`** — asks for a conversion that the compiler knows how to
  do. It makes a **new value** of the other type and leaves the original
  alone. This is the cast of `ex00`.
- **No range check** — `static_cast` doesn't look at the value. A value
  that loses its decimals is fine; a value that doesn't fit is the
  programmer's problem (see section 6).

```cpp
int i = static_cast<int>(4.9);   // 4: the decimals are dropped
```

- **Why not the C-style cast** — `(int)d` tries several kinds of cast and
  uses the first that works, so a mistake can silently become a
  `reinterpret_cast`. A named cast refuses to compile instead, and is easy
  to search for.
- **Look out for hidden conversions** — a conversion can also happen with no
  cast at all. `std::isprint` takes an `int`, so passing it a `double`
  compiles and converts the value silently. Write the cast, so that it's
  visible:

```cpp
std::isprint(val)                      // hidden: double to int
std::isprint(static_cast<char>(val))   // visible
```

### 2. A class that can't be instantiated

- **The rule** — the subject: *"As this class doesn't need to store
  anything at all, it must not be instantiable by users."*
- **Default constructor** — a class with no constructor gets a public one
  from the compiler, so leaving it out isn't enough.
- **Private Orthodox Canonical Form** — the four members are declared
  `private`, so no code outside the class can create or copy an object:

```cpp
class ScalarConverter
{
private:
    ScalarConverter();
    ScalarConverter(const ScalarConverter& other);
    ScalarConverter& operator=(const ScalarConverter& other);
    ~ScalarConverter();
public:
    static void convert(const std::string& str);
};
```

```cpp
ScalarConverter::convert("42");   // ✅ no object needed
ScalarConverter s;                // ❌ error: the constructor is private
```

- **Static member function** — belongs to the class, not to an object. It
  has no `this`, so it's called with the class name.
- **Static function at file scope** — a different use of the same word: the
  function is visible only in its own `.cpp`. All the helpers (`get_type`,
  `parse_int`, `print_char` and the others) are written this way, so the
  class keeps only one static method.
- **Unnamed parameters** — the copy constructor and `operator=` don't use
  their parameter. With `-Werror`, an unused named parameter stops the
  build, so the `.cpp` leaves the name out:

```cpp
ScalarConverter::ScalarConverter(const ScalarConverter&) {}
```

### 3. Detecting the type of a literal

- **Literal** — a value written directly in code: `'a'`, `42`, `4.2f`,
  `4.2`. Its *shape* gives its type, so detection checks the text and
  converts nothing.
- **Whole string, not the first character** — `42` is also the start of
  `42.0f`, and `nan` starts with a letter like `hello`. Choosing the type
  from the first character alone doesn't work:

| Input | First character | Why it isn't enough |
|---|---|---|
| `+inf` | `+` | Looks like the start of a number, but it's a pseudo-literal |
| `*` | not a letter, not a digit | Fits no number or word, but it's a char |
| `hello` | a letter | Looks like a char or `nan`, but it's invalid |

- **Order of the checks** — each check returns as soon as it matches:

1. The empty string: invalid.
2. The six pseudo-literals, compared as whole strings.
3. One printable character that isn't a digit: `char`.
4. Anything else: the number scan.

- **The number scan** — walks the string once. The type starts as `INT`,
  and each part that's found changes it:

| Dot | Final `f` | Type |
|---|---|---|
| no | no | `int` |
| yes | no | `double` |
| yes | yes | `float` |
| no | yes | invalid: `42f` isn't a C++ literal |

- **Index by reference** — `skip_digits` moves the caller's index, so the
  scan continues where it stopped. It returns `false` if there was no digit:

```cpp
static bool skip_digits(const std::string& str, size_t& i);

if (!skip_digits(str, i))
    return (INVALID);
```

- **Why not `strtod`** — it accepts far more than the subject's literals:
  `0x1A`, `1e10`, `" 42"` with a space, `infinity`.
- **Technical choice: `42.` and `.5`** — both are rejected. The subject
  asks for the *"most common form"*, so at least one digit is needed on
  each side of the dot.

### 4. Characters and `<cctype>`

- **`isdigit`, `isprint`** — take an `int` that must be 0 to 255 or `EOF`.
  Any other value is undefined behavior.
- **Signed `char`** — on this platform `char` goes from -128 to 127, so a
  byte above 127 becomes negative. A user only has to type `é`:

| Character | Bytes as `char` | Bytes as `unsigned char` |
|---|---|---|
| `a` | 97 | 97 |
| `é` (UTF-8) | -61, -87 | 195, 169 |

- **The fix** — cast to `unsigned char` first:

```cpp
std::isdigit(static_cast<unsigned char>(str[i]))
```

- **Look out for `unsigned int`** — it isn't a substitute: -61 becomes
  4294967235, which turns back into -61 when it's passed as an `int`. Only
  `unsigned char` gives 195.

### 5. Parsing: from string to value

- **`std::strtol`** — reads an integer into a `long`, which is wider than
  `int` on 64-bit Linux. That makes the overflow visible:

```cpp
n = std::strtol(str.c_str(), NULL, 10);
if (n > INT_MAX || n < INT_MIN)
    return (false);
```

- **`std::strtod`** — reads a `double`, and stops at the first character it
  can't use. If the value is too large it returns infinity.
- **`std::strtof` isn't available** — it's C++11. `strtod` reads a float
  literal too, because it stops at the `f`.
- **Look out for the end pointer** — `strtol` and `strtod` can report where
  they stopped. For a float literal that's the `f`, not the end of the
  string, so a check for `'\0'` rejects every valid float:

```cpp
char*  end;
double d = std::strtod("42.0f", &end);
// d is 42.0, and end points at "f"
```

- **No end pointer needed here** — the number scan already checked the whole
  string, so `NULL` is passed instead.
- **Fallback to `double`** — `2147483648` doesn't fit in an `int`, but it
  fits in a `double`. `||` stops at the first `true`, so `parse_double` runs
  only when `parse_int` fails:

```cpp
ok = parse_int(str, val) || parse_double(str, val, false);
```

- **Too large for every type** — a `double` stops at about 1.8e308, so 309
  nines don't fit anywhere. The program prints
  `ex00: impossible, value is too large`.
- **Look out for infinity from an overflow** — the infinity that `strtod`
  returns looks the same as a real `+inf`. The result of the parsing must be
  checked for every type, before anything is printed:

```cpp
if (type == INVALID)
    return (print_err("ex00: bad input"));
if (!ok)
    return (print_err("ex00: impossible, value is too large"));
```

### 6. Range check before the cast

- **Undefined behavior** — casting a value that doesn't fit: a `double`
  outside the `int` range to `int`, or a finite `double` outside the `float`
  range to `float`. The check must come **before** the cast.
- **Check the wider type** — for a float, the `double` is checked, then
  cast. After the cast it's too late:

```cpp
if (d > max_f || d < -max_f)                        // check the double
    return (false);
val = static_cast<double>(static_cast<float>(d));   // then cast
```

- **`std::numeric_limits`** — the limits of each type, from `<limits>`:

| Call | Value |
|---|---|
| `std::numeric_limits<float>::max()` | Largest float, about 3.4e38 |
| `-std::numeric_limits<float>::max()` | Lowest float, about -3.4e38 |
| `std::numeric_limits<float>::min()` | Smallest *positive* normal float, about 1.2e-38. Not the lower limit! |
| `std::numeric_limits<double>::max()` | Largest double, about 1.8e308 |

- **What each line prints when the value doesn't fit:**

| Line | Prints `impossible` for | Other cases |
|---|---|---|
| char | nan, infinity, outside 0 to 127 | `Non displayable` if `isprint` says no |
| int | nan, infinity, outside `INT_MIN` to `INT_MAX` | |
| float | a finite value outside the float range | `nanf`, `+inff`, `-inff` |
| double | never: `parse_double` already rejected it | `nan`, `+inf`, `-inf` |

- **Technical choice: decimals** — the check runs before the decimals are
  dropped, so `127.5` prints `impossible` for char although 127 would fit.
  It's stricter, and never unsafe.

### 7. Special values: nan and infinity

- **Pseudo-literals** — `nan`, `+inf`, `-inf` and their float forms. Their
  values come from `std::numeric_limits`, not from digits:

```cpp
*dest = std::numeric_limits<double>::quiet_NaN();
*dest = std::numeric_limits<double>::infinity();
*dest = -std::numeric_limits<double>::infinity();
```

- **Look out for the sign** — `infinity()` is always positive. Minus infinity
  needs its own case, with a `-` in front.
- **nan isn't equal to anything** — not even to itself. That's the test for
  it, since `std::isnan` is C++11:

| Test | Normal number | nan |
|---|---|---|
| `val == val` | true | **false** |
| `val != val` | false | **true** |
| `val < 0`, `val > 127` | depends on the value | **false** |

- **Why a separate test** — the last row: nan passes every range check, so
  a range check alone would let it reach the cast.
- **Infinity in `float` and `double`** — the same value in types of
  different sizes. A cast keeps it, and comparing the two gives true, so one
  `set_literal` serves both.
- **Order in `print_float`** — nan, then infinity, then the range check.
  Infinity is also above the float maximum, so with the range check first
  `+inf` would print `impossible`.
- **The `+` sign** — `std::cout` prints positive infinity as `inf`, so
  `+inf` is printed as text.

### 8. One `double` for all the types

- **The idea** — every value is parsed into its own type, then kept in one
  `double`. The four print functions then work for every input:

```cpp
val = static_cast<double>(static_cast<int>(n));     // INT
val = static_cast<double>(static_cast<float>(d));   // FLOAT

print_char(val);
print_int(val);
print_float(val);
print_double(val);
```

- **Why it's safe** — a `double` holds every `char`, `int` and `float`
  exactly, so nothing changes on the way:

| Type | Bits of precision | Fits exactly in a `double` (53 bits) |
|---|---|---|
| `char` | 8 | yes |
| `int` | 32 | yes |
| `float` | 24 | yes |

- **The alternative** — cast from the detected type to the other three in
  each branch. It follows the subject word for word and gives the same
  results, but repeats the casts and the range checks four times.

### 9. Floating-point precision and output

- **Not every integer fits in a `float`** — with 24 bits of precision, a
  `float` is exact only up to 16777216. That's why `2147483647` prints as
  `2147483648.0f`: it's the nearest float.
- **`std::fixed` and `std::setprecision(1)`** — print exactly one decimal.
  Without them, `42.0` prints as `42`:

```cpp
std::cout << std::fixed << std::setprecision(1)
          << static_cast<float>(val) << "f";
```

| Format | `42.0` | `2147483647` as float |
|---|---|---|
| Default | `42` | `2.14748e+09` |
| `fixed`, precision 1 | `42.0` | `2147483648.0` |

- **The cost** — the output is rounded to one decimal: `0.1234` prints
  `0.1`, and `0.16` prints `0.2`. Only the output changes, not the value.

## Resources

**References**

- [cppreference — static_cast](https://en.cppreference.com/w/cpp/language/static_cast)
- [cppreference — Explicit type conversion (C-style cast)](https://en.cppreference.com/w/cpp/language/explicit_cast)
- [cppreference — std::numeric_limits](https://en.cppreference.com/w/cpp/types/numeric_limits)
- [cppreference — std::strtol](https://en.cppreference.com/w/cpp/string/byte/strtol)
- [cppreference — std::strtod](https://en.cppreference.com/w/cpp/string/byte/strtof)
- [cppreference — std::isprint](https://en.cppreference.com/w/cpp/string/byte/isprint)
- [cppreference — std::setprecision](https://en.cppreference.com/w/cpp/io/manip/setprecision)

**AI usage**

AI was used throughout this project as a learning and review tool.
Specific uses:

- **Conceptual explanation**
  - What `static_cast` does, and what it doesn't check
  - Why `isdigit` and `isprint` need an `unsigned char`
  - Why nan isn't equal to itself, and infinity in `float` and `double`
  - Why the range check comes before the cast
- **Design review**
  - The order of the type checks
  - The fallback to `double` for an `int` that's too large
  - One `double` for all the types
  - The choice to reject `42.` and `.5`
- **Requirement and code review**
  - Checked the code against the subject PDF
  - Compiled with `-std=c++98`, and ran edge cases with sanitizers and
    valgrind
  - Reviewed the overflow handling, the sign of infinity, the output format
    and the C++11 functions to avoid
- **Documentation**
  - Drafted this README from the code and the discussions

The final decisions were made manually. AI was used to
ask questions, review drafts, and reason through edge cases.
