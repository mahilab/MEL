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
//
#include <MEL/Communications/MelShare.hpp>
#include <MEL/Logging/Log.hpp>
#include <MEL/Core/Console.hpp>
#include <MEL/Communications/Packet.hpp>
#include <vector>

// Usage:
// To run this example, open two terminals and run the following:
//
// Terminal 1: melshare.exe 1A
// Terminal 2: melshare.exe 1B

using namespace mel;

int main(int argc, char* argv[]) {
    static ColorConsoleWriter<TxtFormatter> consoleAppender;

    if (argc > 1) {
        // example with data and messages
        std::string id = argv[1];
        if (id == "1A") {
            MelShare ms("melshare", OpenOrCreate);
            ms.write_message("Hello from C++! Please send me some data.");
            prompt("Press Enter after running 1B ...");
            print(ms.read_data());
        }
        else if (id == "1B") {
            MelShare ms("melshare", OpenOnly);
            if (ms.is_mapped()) {
                print(ms.read_message());
                ms.write_data({ 1.0, 2.0, 3.0 });
            }
            else
                print("You must run 1A first!");
        }
        // exmple with Packet
        else if (id == "2A") {
            MelShare ms("melshare", OpenOrCreate);
            Packet packet;
            packet << 3.0f << "evan";
            ms.write(packet);
            prompt("Press Enter after running 2B ...");
            print(ms.read_data());
        }
        else if (id == "2B") {
            MelShare ms("melshare", OpenOnly);
            if (ms.is_mapped()) {
                Packet packet;
                float three;
                std::string evan;
                ms.read(packet);
                packet >> three >> evan;
                print(three);
                print(evan);
                packet.clear();
                packet << 1.0 << 2.0 << 3.0 << 4.0 << 5.0;
                ms.write(packet);
            }
            else
                print("You must run 2A first!");
        }

    }
    return 0;
}
