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

#ifndef MEL_COLORCONSOLEWRITER_HPP
#define MEL_COLORCONSOLEWRITER_HPP

#include <MEL/Logging/Writers/ConsoleWriter.hpp>

namespace mel {

//==============================================================================
// CLASS DECLARATION
//==============================================================================

template <class Formatter>
class ColorConsoleWriter : public ConsoleWriter<Formatter> {
public:

    virtual void write(const Record& record) {
        std::string str = Formatter::format(record);
        Lock lock(mutex_);
        setColor(record.get_severity());
        print_string(str);
        reset_text_color();
    }

private:
    void setColor(Severity severity) {
        if (STDOUT_IS_A_TTY) {
            switch (severity) {
#ifdef _WIN32
                case Fatal:
                    set_text_color(Color::White, Color::Red);
                    break;

                case Error:
                    set_text_color(Color::Red);
                    break;

                case Warning:
                    set_text_color(Color::Yellow);
                    break;

                case Debug:
                case Verbose:
                    set_text_color(Color::Cyan);
                    break;
#else
                case Fatal:
                    std::cout << "\x1B[97m\x1B[41m";  // white on red background
                    break;

                case Error:
                    std::cout << "\x1B[91m";  // red
                    break;

                case Warning:
                    std::cout << "\x1B[93m";  // yellow
                    break;

                case Debug:
                case Verbose:
                    std::cout << "\x1B[96m";  // cyan
                    break;
#endif
                default:
                    break;
            }
        }
    }

};
}  // namespace mel

#endif  // MEL_COLORCONSOLEWRITER_HPP

//==============================================================================
// CLASS DOCUMENTATIOn
//==============================================================================
