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

#include <MEL/Config.hpp>
#include <MEL/Logging/LogRecord.hpp>
#include <iomanip>

namespace mel {

//==============================================================================
// CLASS DECLARATION
//==============================================================================

class MEL_API CsvFormatter {
public:
    static std::string header() {
        return "Date, Time, Severity, TID, Function, Message\n";
    }

    static std::string format(const LogRecord& record) {
        std::ostringstream ss;
        ss << record.get_timestamp().yyyy_mm_dd() << ", ";
        ss << record.get_timestamp().hh_mm_ss_mmm() << ", ";
        ss << severity_to_string(record.get_severity()) << ", ";
        ss << record.get_tid_() << ", ";
        ss << record.get_func() << "@" << record.get_line() << ", ";

        std::string message = record.get_message();

        if (message.size() > kMaxMessageSize) {
            message.resize(kMaxMessageSize);
            message.append("...");
        }

        std::istringstream split(message);
        std::string token;

        while (!split.eof()) {
            std::getline(split, token, '"');
            ss << "\"" << token << "\"";
        }

        ss << "\n";

        return ss.str();
    }

    static const size_t kMaxMessageSize = 32000;
};
}  // namespace  mel

//==============================================================================
// CLASS DOCUMENTATION
//==============================================================================
