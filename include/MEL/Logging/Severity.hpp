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

#ifndef MEL_SEVERITY_HPP
#define MEL_SEVERITY_HPP

namespace mel {
enum Severity {
    None    = 0,
    Fatal   = 1,
    Error   = 2,
    Warning = 3,
    Info    = 4,
    Debug   = 5,
    Verbose = 6
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
        case Debug:
            return "DEBUG";
        case Verbose:
            return "VERB";
        default:
            return "NONE";
    }
}

inline Severity string_to_severity(const char* str) {
    for (Severity severity = Fatal; severity <= Verbose;
         severity          = static_cast<Severity>(severity + 1)) {
        if (severity_to_string(severity)[0] == str[0]) {
            return severity;
        }
    }

    return None;
}
}  // namespace mel

#endif  // MEL_SEVERITY_HPP
