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

#pragma once



namespace mel {

/// Represents a logging severity level
enum Severity {
    None    = 0,  ///< always written
    Fatal   = 1,  ///< error that forces application abort
    Error   = 2,  ///< error that is fatal to operation, but not application
    Warning = 3,  ///< error that may cause issues, but has been accounted for
    Info    = 4,  ///< useful information needed during normal operation
    Verbose = 5,  ///< useful information not needed during normal operation
    Debug   = 6,  ///< useful information needed for diagnostics
};

inline const char* severity_to_string(Severity severity) {
    switch (severity) {
        case Fatal:
            return "FATAL";
        case Error:
            return "ERROR";
        case Warning:
            return "WARN";
        case Info:
            return "INFO";
        case Verbose:
            return "VERB";
        case Debug:
            return "DEBUG";
        default:
            return "NONE";
    }
}

inline Severity string_to_severity(const char* str) {
    for (Severity severity = Fatal; severity <= Debug;
         severity          = static_cast<Severity>(severity + 1)) {
        if (severity_to_string(severity)[0] == str[0]) {
            return severity;
        }
    }

    return None;
}
}  // namespace mel
