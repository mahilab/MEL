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

#ifndef MEL_UTF8CONVERTER_HPP
#define MEL_UTF8CONVERTER_HPP

#include <string>

namespace mel {

//==============================================================================
// CLASS DECLARATION
//==============================================================================
//
class UTF8Converter {
public:
    static std::string header(const std::string& str) {
        const char kBOM[] = "\xEF\xBB\xBF";

        return std::string(kBOM) + convert(str);
    }

    static const std::string& convert(const std::string& str) { return str; }
};
}  // namespace mel

#endif  // MEL_UTF8CONVERTER_HPP

//==============================================================================
// CLASS DOCUMENTATION
//==============================================================================
