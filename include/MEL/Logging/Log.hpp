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
#include <MEL/Logging/Writers/ColorConsoleWriter.hpp>
#include <cstring>

namespace mel
{

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

//==============================================================================
// LOGGING INITIALIZER FUNCTIONS
//==============================================================================

    /// Initializes logger with a custom Writer (specific logger instance)
    template<int instance>
    inline Logger<instance>& init_logger(Severity max_severity, Writer* writer) {
        static Logger<instance> logger(max_severity);
        logger.add_writer(writer);
        logger += Record(Info, LOG_GET_FUNC(), __LINE__, LOG_GET_FILE()) << "Logger " << instance << " Initialized";
        return logger;
    }

    /// Initializes logger with a custom Writer (default logger instance)
    inline Logger<DEFAULT_LOGGER>& init_logger(Severity max_severity, Writer* writer) {
        return init_logger<DEFAULT_LOGGER>(max_severity, writer);
    }

    /// Initializes logger with RollingFileWriter with any Formatter (specific logger instance)
    template<class Formatter, int instance>
    inline Logger<instance>& init_logger(Severity max_severity, const char* filename, size_t max_file_size = 0, int max_files = 0) {
        static RollingFileWriter<Formatter> rollingFilewriter(filename, max_file_size, max_files);
        return init_logger<instance>(max_severity, &rollingFilewriter);
    }

    /// Initializes logger with RollingFileWriter with any Formatter (default logger instance)
    template<class Formatter>
    inline Logger<DEFAULT_LOGGER>& init_logger(Severity max_severity, const char* filename, size_t max_file_size = 0, int max_files = 0) {
        return init_logger<Formatter, DEFAULT_LOGGER>(max_severity, filename, max_file_size, max_files);
    }

    /// Initializes logger with RollingFileWriter and Txt/CsvFormatter chosen by file extension (specific logger instance)
    template<int instance>
    inline Logger<instance>& init_logger(Severity max_severity, const char* filename, size_t max_file_size = 0, int max_files = 0) {
        const char* dot = std::strrchr(filename, '.');
        if (dot && 0 == std::strcmp(dot, ".csv"))
            return init_logger<CsvFormatter, instance>(max_severity, filename, max_file_size, max_files);
        else
            return init_logger<TxtFormatter, instance>(max_severity, filename, max_file_size, max_files);
    }

    /// Initializes logger with RollingFileWriter and Txt/CsvFormatter chosen by file extension (default logger instance)
    inline Logger<DEFAULT_LOGGER>& init_logger(Severity max_severity, const char* filename, size_t max_file_size = 0, int max_files = 0) {
        return init_logger<DEFAULT_LOGGER>(max_severity, filename, max_file_size, max_files);
    }

    /// Initalizes the default MEL logger
    inline Logger<DEFAULT_LOGGER>& init_logger(Severity file_max_severity = Verbose, Severity console_max_severity = Info) {
        static ColorConsoleWriter<TxtFormatter> console_writer(console_max_severity);
        Logger<DEFAULT_LOGGER>& logger = init_logger(file_max_severity, "MEL.log", 0, 0).add_writer(&console_writer);
        return logger;
    }

} // namespace mel

//==============================================================================
// LOGGING MACRO FUNCTIONS
//==============================================================================




/// Log severity level checker for specific logger instance
#define IF_LOG_(instance, severity)             if (!get_logger<instance>() || !get_logger<instance>()->check_severity(severity)) {;} else
/// Log severity level checker for default logger instance
#define IF_LOG(severity)                        IF_LOG_(DEFAULT_LOGGER, severity)

/// Main logging macro for specific logger instance
#define LOG_(instance, severity)                IF_LOG_(instance, severity) (*get_logger<instance>()) += Record(severity, LOG_GET_FUNC(), __LINE__, LOG_GET_FILE())
/// Main logging macro for default logger instance
#define LOG(severity)                           LOG_(DEFAULT_LOGGER, severity)

/// Conditional logging macro for specific logger instance
#define LOG_IF_(instance, severity, condition)  if (!(condition)) {;} else LOG_(instance, severity)
/// Conditional logging macro for default logger instance
#define LOG_IF(severity, condition) LOG_IF_(DEFAULT_LOGGER, severity, condition)

#endif // MEL_LOG_HPP

