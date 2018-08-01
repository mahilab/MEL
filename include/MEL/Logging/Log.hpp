// MIT License
//
// MEL - Mechatronics Engine & Library
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

#include <MEL/Config.hpp>
#include <MEL/Logging/Formatters/CsvFormatter.hpp>
#include <MEL/Logging/Formatters/TxtFormatter.hpp>
#include <MEL/Logging/Logger.hpp>
#include <MEL/Logging/Writers/ColorConsoleWriter.hpp>
#include <MEL/Logging/Writers/RollingFileWriter.hpp>
#include <cstring>

namespace mel {

/// Gets name of calling funcion
#ifdef _MSC_VER
#define LOG_GET_FUNC() __FUNCTION__
#else
#define LOG_GET_FUNC() __PRETTY_FUNCTION__
#endif

/// Can be turned on to capture file name with Record
#if LOG_CAPTURE_FILE
#define LOG_GET_FILE() __FILE__
#else
#define LOG_GET_FILE() ""
#endif

//==============================================================================
// CUSTOM LOGGING INITIALIZER FUNCTIONS
//==============================================================================

/// Initializes a logger with a custom Writer (specific logger instance)
template <int instance>
inline Logger<instance>& init_logger(Severity max_severity, Writer* writer) {
    static Logger<instance> logger(max_severity);
    logger.add_writer(writer);
    logger += Record(Info, LOG_GET_FUNC(), __LINE__, LOG_GET_FILE())
              << "Logger " << instance << " Initialized";
    return logger;
}

/// Initializes a logger with RollingFileWriter with any Formatter (specific
/// logger instance)
template <class Formatter, int instance>
inline Logger<instance>& init_logger(Severity max_severity,
    const char* filename,
    size_t max_file_size = 0,
    int max_files = 0) {
    static RollingFileWriter<Formatter> rollingFilewriter(
        filename, max_file_size, max_files);
    return init_logger<instance>(max_severity, &rollingFilewriter);
}

/// Initializes a logger with RollingFileWriter and Txt/CsvFormatter chosen by
/// file extension (specific logger instance)
template <int instance>
inline Logger<instance>& init_logger(Severity max_severity,
    const char* filename,
    size_t max_file_size = 0,
    int max_files = 0) {
    const char* dot = std::strrchr(filename, '.');
    if (dot && 0 == std::strcmp(dot, ".csv"))
        return init_logger<CsvFormatter, instance>(max_severity, filename,
            max_file_size, max_files);
    else
        return init_logger<TxtFormatter, instance>(max_severity, filename,
            max_file_size, max_files);
}

//==============================================================================
// DEFAULT MEL LOGGER
//==============================================================================

extern MEL_API Logger<DEFAULT_LOGGER>* MEL_LOGGER;

}  // namespace mel

//==============================================================================
// LOGGING MACRO FUNCTIONS
//==============================================================================

/// Log severity level checker for specific logger instance
#define IF_LOG_(instance, severity)                          \
    if (!get_logger<instance>() ||                           \
        !get_logger<instance>()->check_severity(severity)) { \
        ;                                                    \
    } else

/// Main logging macro for specific logger instance
#define LOG_(instance, severity) \
    IF_LOG_(instance, severity)  \
    (*get_logger<instance>()) += Record(severity, LOG_GET_FUNC(), __LINE__, LOG_GET_FILE())

/// Conditional logging macro for specific logger instance
#define LOG_IF_(instance, severity, condition) \
    if (!(condition)) {                        \
        ;                                      \
    } else                                     \
        LOG_(instance, severity)

/// Log severity level checker for default MEL logger
#define IF_LOG(severity)                                        \
    if (!MEL_LOGGER || !MEL_LOGGER->check_severity(severity)) { \
        ;                                                       \
    } else

/// Main logging macro for defaulter MEL logger
#define LOG(severity) \
    IF_LOG(severity)  \
        *MEL_LOGGER += Record(severity, LOG_GET_FUNC(), __LINE__, LOG_GET_FILE())

/// Conditional logging macro for default MEL logger
#define LOG_IF(severity, condition) \
    if (!(condition)) {             \
        ;                           \
    } else                          \
        LOG(severity)

#endif  // MEL_LOG_HPP
