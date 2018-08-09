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

#include <MEL/Utility/Windows/Keyboard.hpp>
#include <MEL/Utility/Console.hpp>

using namespace mel;

int main() {

    print("Press Keys 1,2,3, A, B, C,or Escape");

    while(!Keyboard::is_key_pressed(Key::Escape)) {
        if (Keyboard::is_key_pressed(Key::Num1))
            print("1 pressed!");
        if (Keyboard::is_key_pressed(Key::Num2))
            print("2 pressed!");
        if (Keyboard::is_key_pressed(Key::Num3))
            print("3 pressed!");
        if (Keyboard::is_key_pressed(Key::A))
            print("A pressed!");
        if (Keyboard::is_key_pressed(Key::B))
            print("B pressed!");
        if (Keyboard::is_key_pressed(Key::C))
            print("C pressed!");
    }

    print("Escape pressed! Exiting application.");

    return 0;
}
