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
