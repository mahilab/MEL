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
#include <MEL/Math/Random.hpp>
#include <MEL/Math/Functions.hpp>
#include <MEL/Core/Timer.hpp>
#include <array>

using namespace mel;
using namespace std;

int main() {

    //=========================================================================
    // Basic CSV usage with free CSV functions
    //=========================================================================

    // container for header
    vector<string> header;
    // container for data
    array<array<int ,10>, 10> data;
    // make some data
    for (std::size_t c = 0; c < 10; ++c) {
        header.push_back("Col_" + std::to_string(c));
        for (std::size_t r = 0; r < 10; ++r) {
            data[r][c] = random(0,100);
        }
    }

    // path to csv file (my_files dir will be created)
    string filepath = "my_files/data1.csv";

    // write the header
    csv_write_row(filepath, header);    
    // append the data
    csv_append_rows(filepath, data);

    // read back subset of header with offset
    array<string, 5> row;
    csv_read_row(filepath, row, 0, 2);
    print(row);
    // read back subset of data with offset as doubles
    array<array<double,5>,5> rows;
    csv_read_rows(filepath, rows, 1, 2);
    for (auto& r : rows) 
        print(r);

    //=========================================================================
    // Advanced CSV usage with Csv instance
    //=========================================================================

    Csv csv("my_files/data2.csv");
    if (csv.is_open()) {
        // make a header
        csv.write_row("Time", "double", "string", "int", "vector[0]", "vector[1]", "vector[2]");
        // simulate a loop
        Timer timer(hertz(1000));
        Time  t;
        while (t < seconds(60)) {
            // make some data
            double a_double         = random(0.0, 100.0);
            string a_string         = "string" + std::to_string(timer.get_elapsed_ticks());
            int a_int               = random(0, 100);
            vector<double> a_vector = {random(0.0,1.0), random(1.0,2.0), random(2.0,3.0)};
            // write a row (note a_vector will count as 3 fields)
            csv.write_row(t, a_double, a_string, a_int, a_vector);
            t = timer.wait();
        }
        // print timer info to see performance
        print("Miss Rate: ", timer.get_miss_rate());
        print("Wait Ratio:", timer.get_wait_ratio());
    }

    return 0;
}
