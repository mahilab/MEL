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

#ifndef MEL_FUNCMESSAGEFORMATTER_HPP
#define MEL_FUNCMESSAGEFORMATTER_HPP

#include <MEL/Config.hpp>
#include <MEL/Logging/Record.hpp>

namespace mel {

//==============================================================================
// CLASS DECLARATIOn
//==============================================================================
//
class MEL_API FuncMessageFormatter {
public:
    static std::string header() { return std::string(); }

    static std::string format(const Record& record) {
        std::ostringstream ss;
        ss << record.get_func() << "@" << record.get_line() << ": "
           << record.get_message() << "\n";

        return ss.str();
    }
};
}  // namespace mel

#endif  // MEL_FUNCMESSAGEFORMATTER_HPP

//==============================================================================
// CLASS DOCUMENTATION
//==============================================================================
