#pragma once

#include <MEL/Utility/Types.hpp>
#include <MEL/Utility/Time.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <array>
#include <tuple>

namespace mel {

//==============================================================================
// FORWARD DECLARATIONS
//==============================================================================

//==============================================================================
// TUPLE PRINTING
//==============================================================================

namespace priv {
template<std::size_t...> struct seq{};

template<std::size_t N, std::size_t... Is>
struct gen_seq : gen_seq<N-1, N-1, Is...>{};

template<std::size_t... Is>
struct gen_seq<0, Is...> : seq<Is...>{};

template<class Ch, class Tr, class Tuple, std::size_t... Is>
void print_tuple(std::basic_ostream<Ch,Tr>& os, Tuple const& t, seq<Is...>){
  using swallow = int[];
  (void)swallow{0, (void(os << (Is == 0? "" : ", ") << std::get<Is>(t)), 0)...};
}
} // namespace priv

/// Outputs tuple to stream using << operator
template<class Ch, class Tr, class... Args>
auto operator<<(std::basic_ostream<Ch, Tr>& os, std::tuple<Args...> const& t)
    -> std::basic_ostream<Ch, Tr>&
{
  priv::print_tuple(os, t, priv::gen_seq<sizeof...(Args)>());
  return os;
}

//==============================================================================
// CONSOLE OUTPUT
//==============================================================================

/// Prints anything that works with std::cout.
/// end_line = true will print a new line after printing value
template <typename T>
void print(T value, bool end_line = true) {
    std::cout << value;
    if (end_line)
        std::cout << std::endl;
}

/// Prints STL vector
template <typename T>
void print(std::vector<T> v, bool end_line = true) {
    for (auto it = v.begin(); it != v.end(); ++it) {
        std::cout << *it << " ";
    }
    if (end_line)
        std::cout << std::endl;
}

/// Prints STL array
template <typename T, std::size_t N>
void print(std::array<T, N> a, bool end_line = true) {
    for (auto it = a.begin(); it != a.end(); ++it) {
        std::cout << *it << " ";
    }
    if (end_line)
        std::cout << std::endl;
}

/// Prints C-style array
template <typename T>
void print(T* a, std::size_t size, bool end_line = true) {
    for (int i = 0; i < size; i++) {
        std::cout << a[i] << " ";
    }
    if (end_line)
        std::cout << std::endl;
}

/// Prints MEL 2D array
template <typename T, std::size_t N, std::size_t M>
void print(array_2D<T,N,M> a, bool end_line = true) {
    for (auto it_row = a.begin(); it_row != a.end(); ++it_row) {
        for (auto it_col = (*it_row).begin(); it_col != (*it_row).end(); ++it_col) {
            std::cout << *it_col << " ";
        }
        if (end_line)
            std::cout << std::endl;
    }
}

void print(Time time, bool end_line = true);

//==============================================================================
// CONSOLE INPUT
//==============================================================================

/// Prompts the user with a message and waits for Enter to be pressed.
void prompt(std::string message);

/// Registers a function so that it is called when Ctrl+C is pressed.
/// The function must take an int argument and return void. If it is a class
/// function, it must be static and thus contain only static member data.
void register_ctrl_c_handler(void (*func)(int));

//==============================================================================
// FORMATTING
//==============================================================================

/// Formats string name in between angle brackets for MEL printing puproses
std::string namify(std::string name);

/// Turns basic types into a string
template <typename T>
std::string stringify(T value) {
    return std::to_string(value);
}

/// Turns STL vector into horizontal string
template <typename T>
std::string stringify(std::vector<T> v) {
    std::string s;
    for (auto it = v.begin(); it != v.end(); ++it)
        s += stringify(*it) + " ";
    s.pop_back();
    return s;
}

/// Turns STL array into horizontal string
template <typename T, std::size_t N>
std::string stringify(std::array<T, N> a) {
    std::string s;
    for (auto it = a.begin(); it != a.end(); ++it)
        s += stringify(*it) + " ";
    s.pop_back();
    return s;
}

/// Turns C-style array into horizontal string
template <typename T>
std::string stringify(T* a, std::size_t size) {
    std::string s;
    for (int i = 0; i < size; i++)
        s += stringify(a[i]) + " ";
    s.pop_back();
    return s;
}


}
