#include <MEL/Utility/Console.hpp>
#include <MEL/Utility/Windows/XboxController.hpp>   
#include <MEL/Utility/Timer.hpp>

using namespace mel;
using Button = XboxController::Button;
using Axis = XboxController::Axis;

int main(int argc, char* argv[]) {

    XboxController xb(0);
    xb.set_deadzone(XboxController::LX, 0.05);

    print(xb.is_connected());

    Timer timer(hertz(100));

    while (true) {

        if (xb.is_button_pressed(Button::A))
            xb.vibrate(0.5, 0.5);
        if (xb.is_button_pressed(Button::B))
            xb.vibrate(1.0, 1.0);
        if (xb.is_button_pressed(Button::X))
            xb.vibrate();

        std::cout << xb.get_axis(Axis::LX) << " "
            << xb.get_axis(Axis::LY) << " "
            << xb.get_axis(Axis::LT) << " "
            << xb.get_axis(Axis::RX) << " "
            << xb.get_axis(Axis::RY) << " "
            << xb.get_axis(Axis::RT) << "\n";

        timer.wait();
    }

    prompt("Press ENTER to exit");

    return 0;
}
