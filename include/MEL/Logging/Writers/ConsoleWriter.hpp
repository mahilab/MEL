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

#ifndef MEL_CONSOLEWRITER_HPP
#define MEL_CONSOLEWRITER_HPP

#include <io.h>
#include <MEL/Logging/WinApi.hpp>
#include <MEL/Logging/Writers/Writer.hpp>
#include <MEL/Utility/Mutex.hpp>
#include <iostream>

namespace mel {

//==============================================================================
// CLASS DECLARATION
//==============================================================================
//
template <class Formatter>
class ConsoleWriter : public Writer {
public:
#ifdef _WIN32
    ConsoleWriter() : m_isatty(!!_isatty(_fileno(stdout))), m_stdoutHandle() {
        if (m_isatty) {
            m_stdoutHandle = GetStdHandle(stdHandle::kOutput);
        }
    }
#else
    ConsoleAppender() : m_isatty(!!isatty(fileno(stdout))) {}
#endif

    virtual void write(const Record& record) {
        std::string str = Formatter::format(record);
        Lock lock(m_mutex);

        writestr(str);
    }

protected:
    void writestr(const std::string& str) {
#ifdef _WIN32
        if (m_isatty) {
            WriteConsoleA(m_stdoutHandle, str.c_str(),
                          static_cast<DWORD>(str.size()), NULL, NULL);
        } else {
            // std::cout << util::toNarrow(str, codePage::kActive) <<
            // std::flush;
            std::cout << str << std::flush;
        }
#else
        std::cout << str << std::flush;
#endif
    }

private:
protected:
    Mutex m_mutex;
    const bool m_isatty;
#ifdef _WIN32
    HANDLE m_stdoutHandle;
#endif
};
}  // namespace mel

#endif  // MEL_CONSOLEWRITER_HPP

//==============================================================================
// CLASS DOCUMENTATION
//==============================================================================
