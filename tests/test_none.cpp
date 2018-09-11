#include <MEL/Core/Console.hpp>
#include <MEL/Communications/MelShare.hpp>
#include <MEL/Core/Timer.hpp>
#include <MEL/Math/Waveform.hpp>
#include <MEL/Logging/Log.hpp>
#include <MEL/Devices/Windows/Keyboard.hpp>
#include <MEL/Daq/Quanser/Q8Usb.hpp>
#include <MEL/Core.hpp>
#include <MEL/Mechatronics.hpp>
#include <MEL/Mechatronics.hpp>
#include <MEL/Communications/UdpSocket.hpp>
#include <MEL/Communications/Packet.hpp>
#include <MEL/Devices/Windows/XboxController.hpp>

using namespace mel;

ctrl_bool STOP(false);
bool my_handler(CtrlEvent event) {
    if (event == CtrlEvent::CtrlC) {
        print("Ctrl-C Pressed");
        STOP = true;
    }
    return 1;
}


//int main(void) {
//
//  UdpSocket udp;
//  udp.bind(55001);
//
//  Packet packet;
//
//  char str[] = "7 1 20 120 20 0.5";
//
//  udp.send(str, sizeof(str), "127.0.0.1", 55002);
//
//  return 0;
//
//}

int main(void)
{
    register_ctrl_handler(my_handler);

    Q8Usb q8;


    Amplifier escon242("escon_24/2", High, q8.DO[0], 0.1, q8.AO[0]);

    MelShare ms("poppy");
    Timer timer(hertz(10));

    XboxController xbox;


    double voltage = 0;
    double current = 0;

    q8.open();
    q8.enable();

    while (!STOP) {

        q8.update_input();

        if (Keyboard::is_key_pressed(Key::E) || xbox.is_button_pressed(XboxController::Start))
            escon242.enable();
        if (Keyboard::is_key_pressed(Key::D) || xbox.is_button_pressed(XboxController::Back))
            escon242.disable();

        // if (Keyboard::is_key_pressed(Key::Num1))
        //     current = 0.2;
        // else if (Keyboard::is_key_pressed(Key::Num2))
        //     current = 0.4;
        // else if (Keyboard::is_key_pressed(Key::Num3))
        //     current = 0.6;
        // else if (Keyboard::is_key_pressed(Key::Num4))
        //     current = 0.8;
        // else if (Keyboard::is_key_pressed(Key::Num5))
        //     current = 1.0;

        // if (Keyboard::is_key_pressed(Key::Up))
        //     escon242.set_current(current);
        // else if (Keyboard::is_key_pressed(Key::Down))
        //     escon242.set_current(-current);
        // else
        //     escon242.set_current(0);


        current = xbox.get_axis(XboxController::LT) - xbox.get_axis(XboxController::RT);
        escon242.set_current(current);
        escon242.set_current(current);
        // print(current);

        ms.write_data({ current, -current, voltage });

        q8.update_output();
        timer.wait();
    }

    return 0;
 }


