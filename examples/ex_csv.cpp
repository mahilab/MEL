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

#include <MEL/Logging/Csv.hpp>
#include <MEL/Core/Console.hpp>
#include <MEL/Utility/System.hpp>
#include <MEL/Math/Random.hpp>
#include <array>

using namespace mel;
using namespace std;

int main()
{
    vector<string> header;
    array<array<char,10>,10> data1;
    array<array<int ,10>,10> data2;
    for (std::size_t r = 0; r < 10; ++r) {
        header.push_back("Col " + std::to_string(r));
        for (std::size_t c = 0; c < 10; ++c) {
            data1[r][c] = (char)random(97,122);
            data2[r][c] = random(0,100);
        }
    }

    string filepath = "my_files/data.csv";

    Csv::write_row(filepath, header);
    Csv::append_rows(filepath, data1);
    Csv::append_rows(filepath, data2);

    return 0;
}
