#include <MEL/Communications/SerialPort.hpp>
#include <MEL/Core/Console.hpp>

using namespace mel;

int main() {

    SerialPort comm4(3, 9600);
    SerialPort comm5(4, 9600);

    comm4.open();
    comm5.open();

    unsigned char send[5] = "abcd";

    comm4.send_data(send, 5);
    prompt("Press ENTER to receive");

    unsigned char recv[5];
    comm5.receive_data(recv, 5);
    for (std::size_t i = 0; i < 5; ++i)
        std::cout << recv[i] << std::endl;

    return 0;
}
