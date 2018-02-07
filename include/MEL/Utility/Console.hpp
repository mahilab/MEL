// MIT License
//
// MEL - MAHI Exoskeleton Library
// Copyright (c) 2018 Mechatronics and Haptic Interfaces Lab - Rice University
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// Author(s): Evan Pezent (epezent@rice.edu)
//            Craig McDonald (craig.g.mcdonald@gmail.com)

#ifndef MEL_CONSOLE_HPP
#define MEL_CONSOLE_HPP

#include <MEL/Utility/Time.hpp>
#include <MEL/Utility/Types.hpp>
#include <MEL/Utility/StlStreams.hpp>
#include <sstream>
#include <atomic>

namespace mel {

//==============================================================================
// CONSOLE OUTPUT
//==============================================================================

/// Prints a string to the console using the fastest method the OS offers
void print_string(const std::string& str);

/// Prints anything that works with stream operators (appends new line character)
template<typename T>
void print(T value) {
    std::stringstream ss;
    ss << value << "\n";
    print_string(ss.str());
}

/// Prints MEL 2D array
template <typename T, std::size_t N, std::size_t M>
void print(const array_2D<T, N, M>& a, bool end_line = true) {
    for (auto it_row = a.begin(); it_row != a.end(); ++it_row) {
        for (auto it_col = (*it_row).begin(); it_col != (*it_row).end();
             ++it_col) {
            std::cout << *it_col << " ";
        }
        if (end_line)
            std::cout << "\n";
    }
}

//==============================================================================
// CONSOLE INPUT & SIGNAL HANDLING
//==============================================================================

/// Prompts the user with a message and waits for Enter to be pressed.
void prompt(const std::string& message);

/// Registers a function so that it is called when Ctrl+C is pressed.
///
/// The handler should have an unsigned long input parameter and return an int.
/// The input can be checked against the the control signal values below for
/// further processsing. If this function handles the event, it should return 1,
/// otherwise it should return 0 in which case the next handler will be processed
bool register_ctrl_handler(int (*handler)(unsigned long));

#define CTRL_C_EVENT        0  ///< Ctrl+C pressed
#define CTRL_BREAK_EVENT    1  ///< Ctrl+Break pressed
#define CTRL_CLOSE_EVENT    2  ///< console closed
#define CTRL_LOGOFF_EVENT   5  ///< user is logging off
#define CTRL_SHUTDOWN_EVENT 6  ///< system is shutting down

/// Special bool type that can safely be used by a Ctrl handler
typedef volatile std::atomic<bool> ctrl_bool;

//==============================================================================
// CONSOLE FORMAT
//==============================================================================

/// Represents a console text color
enum class Color {
    None,
    Black,
    Gray,
    White,
    Red,
    DarkRed,
    Green,
    DarkGreen,
    Blue,
    DarkBlue,
    Cyan,
    Aqua,
    Magenta,
    Purple,
    Yellow,
    Gold
};

/// Sets the foreground and background text color in the console
void set_text_color(Color foreground, Color background = Color::None);

/// Resets the foreground and background text color to the default style
void reset_text_color();

/// Taste the rainbow
void rainbow();

//==============================================================================
// MISC
//==============================================================================

/// True if stdout is a character device (a terminal, console, printer, or serial port)
extern const bool STDOUT_IS_A_TTY;

//==============================================================================
// FORMATTING
//==============================================================================

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

}  // namespace mel

#endif  // MEL_CONSOLE_HPP
