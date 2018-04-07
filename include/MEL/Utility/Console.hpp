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

#include <MEL/Core/Time.hpp>
#include <MEL/Utility/StlStreams.hpp>
#include <MEL/Utility/Types.hpp>
#include <atomic>
#include <sstream>

namespace mel {

//==============================================================================
// CONSOLE INPUT & SIGNAL HANDLING
//==============================================================================

/// Prompts the user with a message and waits for Enter to be pressed.
void prompt(const std::string& message);

/// Types of events that can be caught in in a console
enum class CtrlEvent {
    CtrlC    = 0,  ///< user pressed Ctrl+C
    CtrlQuit = 1,  ///< user pressed Ctrl+Break (Windows) or Ctrl+\ (Unix)
    Close    = 2,  ///< user has closed the console (Windows only)
    Logoff   = 3,  ///< user is logging off (Windows only)
    Shutdown = 4   ///< system is shutting down (Windows only)
};

/// Registers a function so that it is called when Ctrl+C is pressed.
///
/// The handler should have an unsigned long input parameter and return an int.
/// The input can be checked against the the control event values above for
/// further processsing. If this function handles the event, it should return
/// true, otherwise it should return false.
bool register_ctrl_handler(bool (*handler)(CtrlEvent));

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

//==============================================================================
// CONSOLE OUTPUT
//==============================================================================

/// Prints a string to the console using the fastest method the OS offers
void print_string(const std::string& str);

/// Prints anything that works with stream operators (appends new line
/// character)
template <typename T>
void print(T value) {
    std::stringstream ss;
    ss << value << "\n";
    print_string(ss.str());
}

/// Print with color
template <typename T>
void print(T value, Color foreground, Color background = Color::None) {
    set_text_color(foreground, background);
    print(value);
    reset_text_color();
}

//==============================================================================
// MISC
//==============================================================================

/// True if stdout is a character device (a terminal, console, printer, or
/// serial port)
extern const bool STDOUT_IS_A_TTY;

/// Causes the console to emit a beep sound
void beep();

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
