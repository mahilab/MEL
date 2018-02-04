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

#ifndef MEL_NATIVEEOL_CONVERTER_HPP
#define MEL_NATIVEEOL_CONVERTER_HPP

#include <MEL/Logging/Converters/UTF8Converter.hpp>

namespace mel {
//==============================================================================
// CLASS DECLARATION
//==============================================================================
template <class NextConverter = UTF8Converter>
class NativeEOLConverter : public NextConverter {
#ifdef _WIN32
public:
    static std::string header(const std::string& str) {
        return NextConverter::header(fixLineEndings(str));
    }

    static std::string convert(const std::string& str) {
        return NextConverter::convert(fixLineEndings(str));
    }

private:
    static std::wstring fixLineEndings(const std::wstring& str) {
        std::wstring output;
        output.reserve(str.length() * 2);

        for (size_t i = 0; i < str.size(); ++i) {
            wchar_t ch = str[i];

            if (ch == L'\n') {
                output.push_back(L'\r');
            }

            output.push_back(ch);
        }

        return output;
    }
#endif
};
}  // namespace mel

#endif  // MEL_NATIVEEOL_CONVERTER_HPP

//==============================================================================
// CLASS DOCUMENTATION
//==============================================================================
