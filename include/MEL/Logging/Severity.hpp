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
    NONE    = 0,
    FATAL   = 1,
    ERROR   = 2,
    WARNING = 3,
    INFO    = 4,
    DEBUG   = 5,
    VERBOSE = 6
};

inline const char* severity_to_string(Severity severity) {
    switch (severity) {
        case FATAL:
            return "FATAL";
        case ERROR:
            return "ERROR";
        case WARNING:
            return "WARN";
        case INFO:
            return "INFO";
        case DEBUG:
            return "DEBUG";
        case VERBOSE:
            return "VERB";
        default:
            return "NONE";
    }
}

inline Severity string_to_severity(const char* str) {
    for (Severity severity = FATAL; severity <= VERBOSE;
         severity          = static_cast<Severity>(severity + 1)) {
        if (severity_to_string(severity)[0] == str[0]) {
            return severity;
        }
    }

    return NONE;
}
}  // namespace mel

#endif  // MEL_SEVERITY_HPP
