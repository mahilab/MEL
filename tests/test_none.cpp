// #include <MEL/Daq/Quanser/Q8Usb.hpp>
// #include <MEL/Core.hpp>
// #include <MEL/Communications.hpp>
// #include <MEL/Devices/Windows/Keyboard.hpp>

// using namespace mel;

// int main(int argc, char* argv[]) {

//     Q8Usb q8;

//     q8.AO.set_enable_values({1,2,3,4,5,6,7,8});
//     q8.AO.set_disable_values({-1,-2,-3,-4,-5,-6,-7,-8});
//     q8.AO.set_channel_numbers({0,1,2,3});


//     q8.open();
//     q8.enable();

//     print(q8.AO.get_channel_numbers());
//     print(q8.AO.get_values());

//     MelShare ms("test");

//     Timer timer(hertz(100));
//     while(true) {
//         if (Keyboard::is_key_pressed(Key::E)) {
//             q8.AO.enable();
//         }
//         if (Keyboard::is_key_pressed(Key::D)) {
//             q8.AO.disable();
//         }
//         if (q8.is_enabled()) {
//             q8.update_input();
//         }
//         ms.write_data({q8.AI[0]});
//         timer.wait();
//     }

//     return 0;
// }

int main(int argc, char* argv[]) {
    /* code */
    return 0;
}
