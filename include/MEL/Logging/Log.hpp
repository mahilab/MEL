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

#ifndef MEL_LOG_HPP
#define MEL_LOG_HPP

#include <MEL/Logging/Logger.hpp>
#include <MEL/Logging/Formatters/CsvFormatter.hpp>
#include <MEL/Logging/Formatters/TxtFormatter.hpp>
#include <MEL/Logging/Writers/RollingFileWriter.hpp>
#include <cstring>

namespace mel
{
    namespace
    {
        bool is_csv(const char* fileName)
        {
            const char* dot = std::strrchr(fileName, '.');
            return dot && 0 == std::strcmp(dot, ".csv");
        }
    }

//==============================================================================
// LOGGING INITIALIZERS
//==============================================================================

    /// Empty initializer / one appender

    template<int instance>
    inline Logger<instance>& init_logger(Severity maxSeverity = NONE, Writer* appender = NULL)
    {
        static Logger<instance> logger(maxSeverity);
        return appender ? logger.add_writer(appender) : logger;
    }

    inline Logger<DEFAULT_MEL_LOGGER>& init_logger(Severity maxSeverity = NONE, Writer* appender = NULL)
    {
        return init_logger<DEFAULT_MEL_LOGGER>(maxSeverity, appender);
    }

    /// RollingFileAppender with any Formatter

    template<class Formatter, int instance>
    inline Logger<instance>& init_logger(Severity maxSeverity, const char* fileName, size_t maxFileSize = 0, int maxFiles = 0)
    {
        static RollingFileWriter<Formatter> rollingFileAppender(fileName, maxFileSize, maxFiles);
        return init_logger<instance>(maxSeverity, &rollingFileAppender);
    }

    template<class Formatter>
    inline Logger<DEFAULT_MEL_LOGGER>& init_logger(Severity maxSeverity, const char* fileName, size_t maxFileSize = 0, int maxFiles = 0)
    {
        return init_logger<Formatter, DEFAULT_MEL_LOGGER>(maxSeverity, fileName, maxFileSize, maxFiles);
    }

    /// RollingFileAppender with TXT/CSV chosen by file extension

    template<int instance>
    inline Logger<instance>& init_logger(Severity maxSeverity, const char* fileName, size_t maxFileSize = 0, int maxFiles = 0)
    {
        return is_csv(fileName) ? init_logger<CsvFormatter, instance>(maxSeverity, fileName, maxFileSize, maxFiles) : init_logger<TxtFormatter, instance>(maxSeverity, fileName, maxFileSize, maxFiles);
    }

    inline Logger<DEFAULT_MEL_LOGGER>& init_logger(Severity maxSeverity, const char* fileName, size_t maxFileSize = 0, int maxFiles = 0)
    {
        return init_logger<DEFAULT_MEL_LOGGER>(maxSeverity, fileName, maxFileSize, maxFiles);
    }

} // namespace mel

//==============================================================================
// LOGGING MACROS
//==============================================================================

/// Gets name of calling funcion
#ifdef _MSC_VER
#   define LOG_GET_FUNC()      __FUNCTION__
#else
#   define LOG_GET_FUNC()      __PRETTY_FUNCTION__
#endif

/// Can be turned on to capture file name with Record
#if LOG_CAPTURE_FILE
#   define LOG_GET_FILE()      __FILE__
#else
#   define LOG_GET_FILE()      ""
#endif

/// Log severity level checker for specific logger instance
#define IF_LOG_(instance, severity)             if (!get_logger<instance>() || !get_logger<instance>()->check_severity(severity)) {;} else
/// Log severity level checker for default logger instance
#define IF_LOG(severity)                        IF_LOG_(DEFAULT_MEL_LOGGER, severity)

/// Main logging macro for specific logger instance
#define LOG_(instance, severity)                IF_LOG_(instance, severity) (*get_logger<instance>()) += Record(severity, LOG_GET_FUNC(), __LINE__, LOG_GET_FILE())
/// Main logging macro for default logger instance
#define LOG(severity)                           LOG_(DEFAULT_MEL_LOGGER, severity)

/// Conditional logging macro for specific logger instance
#define LOG_IF_(instance, severity, condition)  if (!(condition)) {;} else LOG_(instance, severity)
/// Conditional logging macro for default logger instance
#define LOG_IF(severity, condition) LOG_IF_(DEFAULT_MEL_LOGGER, severity, condition)

#endif // MEL_LOG_HPP

