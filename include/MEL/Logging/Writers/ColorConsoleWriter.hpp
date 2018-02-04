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
#ifdef _WIN32
    ColorConsoleWriter() : m_originalAttr() {
        if (this->m_isatty) {
            CONSOLE_SCREEN_BUFFER_INFO csbiInfo;
            GetConsoleScreenBufferInfo(this->m_stdoutHandle, &csbiInfo);

            m_originalAttr = csbiInfo.wAttributes;
        }
    }
#else
    ColorConsoleAppender() {}
#endif

    virtual void write(const Record& record) {
        std::string str = Formatter::format(record);
        Lock lock(this->m_mutex);

        setColor(record.get_severity());
        this->writestr(str);
        resetColor();
    }

private:
    void setColor(Severity severity) {
        if (this->m_isatty) {
            switch (severity) {
#ifdef _WIN32
                case FATAL:
                    SetConsoleTextAttribute(
                        this->m_stdoutHandle,
                        foreground::kRed | foreground::kGreen |
                            foreground::kBlue | foreground::kIntensity |
                            background::kRed);  // white on red background
                    break;

                case ERROR:
                    SetConsoleTextAttribute(
                        this->m_stdoutHandle,
                        static_cast<WORD>(foreground::kRed |
                                          foreground::kIntensity |
                                          (m_originalAttr & 0xf0)));  // red
                    break;

                case WARNING:
                    SetConsoleTextAttribute(
                        this->m_stdoutHandle,
                        static_cast<WORD>(foreground::kRed |
                                          foreground::kGreen |
                                          foreground::kIntensity |
                                          (m_originalAttr & 0xf0)));  // yellow
                    break;

                case DEBUG:
                case VERBOSE:
                    SetConsoleTextAttribute(
                        this->m_stdoutHandle,
                        static_cast<WORD>(foreground::kGreen |
                                          foreground::kBlue |
                                          foreground::kIntensity |
                                          (m_originalAttr & 0xf0)));  // cyan
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

    void resetColor() {
        if (this->m_isatty) {
#ifdef _WIN32
            SetConsoleTextAttribute(this->m_stdoutHandle, m_originalAttr);
#else
            std::cout << "\x1B[0m\x1B[0K";
#endif
        }
    }

private:
#ifdef _WIN32
    WORD m_originalAttr;
#endif
};
}  // namespace mel

#endif  // MEL_COLORCONSOLEWRITER_HPP

//==============================================================================
// CLASS DOCUMENTATIOn
//==============================================================================
