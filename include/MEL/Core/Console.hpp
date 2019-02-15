// MIT License
//
// MEL - Mechatronics Engine & Library
// Copyright (c) 2019 Mechatronics and Haptic Interfaces Lab - Rice University
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

#pragma once

#include <MEL/Core/Time.hpp>
#include <MEL/Utility/StlStreams.hpp>
#include <MEL/Core/Types.hpp>
#include <atomic>
#include <sstream>
#include <utility>

namespace mel {

//==============================================================================
// SIGNAL HANDLING
//==============================================================================

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
bool MEL_API register_ctrl_handler(bool (*handler)(CtrlEvent));

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
void MEL_API set_text_color(Color foreground, Color background = Color::None);

/// Resets the foreground and background text color to the default style
void MEL_API reset_text_color();

//==============================================================================
// CONSOLE INPUT
//==============================================================================

/// Determines if keyboard has been hit
int MEL_API kb_hit();

/// Get character without waiting for Return to be pressed (blocking).
int MEL_API get_ch();

/// Get character without waiting for Return to be pressed (non-blocking).
/// Returns 0 if no key was pressed.
int MEL_API get_ch_nb();

/// Reads a key press and returns a key code (blocking).
int MEL_API get_key();

/// Reads a key press and returns a key code (blocking).
/// Returns 0 if no key was pressed.
int MEL_API get_key_nb();

/// Prompts the user with a message and waits for Enter to be pressed.
void MEL_API prompt(const std::string& message);

//==============================================================================
// CONSOLE OUTPUT
//==============================================================================

/// Prints a string to the console using the fastest method the OS offers
void MEL_API print_string(const std::string& str);

/// Prints anything that works with stream operators and then starts a new line
template <typename T>
void print(const T& value) {
    std::stringstream ss;
    ss << value << "\n";
    print_string(ss.str());
}

// Prints variadic number of values with separating spaces and then starts a new line
template <typename Arg, typename... Args>
void print(Arg&& arg, Args&&... args) {
    std::stringstream ss;
    ss << std::forward<Arg>(arg);
    using expander = int[];
    (void)expander{0, (void(ss << ' ' << std::forward<Args>(args)), 0)...};
    ss << "\n";
    print_string(ss.str());
}

/// Print with color
template <typename T>
void color_print(const T& value, Color foreground, Color background = Color::None) {
    set_text_color(foreground, background);
    print(value);
    reset_text_color();
}

//==============================================================================
// FORMATTING
//==============================================================================

/// Turns basic types into a string
template <typename T>
std::string stringify(T value) {
    return std::to_string(value);
}

//==============================================================================
// MISC
//==============================================================================

/// Returns true if stdout is a character device (a terminal, console, printer, or
/// serial port)
bool MEL_API is_tty();

/// Causes the console to emit a beep sound
void MEL_API beep();

/// Clears screen, resets all attributes and moves cursor home.
void MEL_API cls();

/// Key Codes
enum {
    KEY_ESCAPE    = 0,
    KEY_ENTER     = 1,
    KEY_BACKSPACE = 8,
    KEY_SPACE     = 32,

    KEY_INSERT  = 2,
    KEY_HOME    = 3,
    KEY_PGUP    = 4,
    KEY_DELETE  = 5,
    KEY_END     = 6,
    KEY_PGDOWN  = 7,

    KEY_UP      = 14,
    KEY_DOWN    = 15,
    KEY_LEFT    = 16,
    KEY_RIGHT   = 17,

    KEY_F1      = 18,
    KEY_F2      = 19,
    KEY_F3      = 20,
    KEY_F4      = 21,
    KEY_F5      = 22,
    KEY_F6      = 23,
    KEY_F7      = 24,
    KEY_F8      = 25,
    KEY_F9      = 26,
    KEY_F10     = 27,
    KEY_F11     = 28,
    KEY_F12     = 29,

    KEY_NUMDEL  = 30,
    KEY_NUMPAD0 = 31,
    KEY_NUMPAD1 = 127,
    KEY_NUMPAD2 = 128,
    KEY_NUMPAD3 = 129,
    KEY_NUMPAD4 = 130,
    KEY_NUMPAD5 = 131,
    KEY_NUMPAD6 = 132,
    KEY_NUMPAD7 = 133,
    KEY_NUMPAD8 = 134,
    KEY_NUMPAD9 = 135
};

}  // namespace mel
