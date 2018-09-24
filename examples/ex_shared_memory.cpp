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

#include <MEL/Communications/SharedMemory.hpp>
#include <MEL/Core/Console.hpp>
#include <MEL/Communications/Packet.hpp>
#include <MEL/Logging/Log.hpp>
#include <vector>

// To run this example, open two terminals and run the following:
//
// Terminal 1: shared_memory.exe #A
// Terminal 2: shared_memory.exe #B
//
// where # is replaced with either 1, 2, or 3

using namespace mel;

int main(int argc, char *argv[]) {

    if (argc > 1) {
        std::string id = argv[1];
        if (id == "1A") {
            // create shared memory map (same string name as B)
            SharedMemory map("my_map", OpenOrCreate);
            // write message
            char msg[4] = "abc"; // 3 char + 1 null terminator = 4 char (aka byte)
            map.write(msg, 4);
            // receive data
            prompt("Press Enter after executing B");
            std::vector<double> data(4);
            map.read(&data[0], 32);
            print(data);
        }
        else if (id == "1B") {
            // create shared memory map (same string name as A)
            SharedMemory map("my_map", OpenOnly, 512);
            if (!map.is_mapped()) {
                print("You must run 1A first!");
                return -1;
            }
            // receive message
            char msg[4];
            map.read(msg, 4);
            print(msg);
            // write data
            std::vector<double> data = {1.0, 2.0, 3.0, 4.0};
            map.write(&data[0], 32); // 4 double * 8 byte/double = 32 bytes
            // need pointer to the first element of the vector hence &data[0]
        }
        else if (id == "2A") {
            SharedMemory map("my_map", OpenOrCreate);
            char msg[5] = "abcd";
            map.write(msg, 5);
            prompt("Press Enter after executing 2B");
            map.read(&msg, 5);
            print(msg); // axcd

        }
        else if (id == "2B") {
            SharedMemory map("my_map", OpenOnly);
            if (!map.is_mapped()) {
                print("You must run 2A first!");
                return -1;
            }
            char msg[5];
            map.read(msg, 5);
            print(msg); // abcd
            char x = 'x';
            map.write(&x, 1, 1);
        }
        else if (id == "3A") {
            SharedMemory map("my_map", OpenOrCreate);
            std::vector<int> data = {1, 2, 3, 4, 5};
            map.write(&data[0], 20);
            prompt("Press Enter after executing 3B");
            map.read(&data[0], 20);
            print(data); // {1, 2, 16, 4, 5}

        }
        else if (id == "3B") {
            SharedMemory map("my_map", OpenOnly);
            if (!map.is_mapped()) {
                print("You must run 3A first!");
                return -1;
            }
            int sixteen = 16;
            map.write(&sixteen, 4, 8);
        }
    }
    return 0;
}
