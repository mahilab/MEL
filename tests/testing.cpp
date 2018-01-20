// #include <MEL/Communications/Windows/MelShare.hpp>
// #include <MEL/Utility/Windows/Keyboard.hpp>
// #include <MEL/Utility/Timer.hpp>
// #include <MEL/Core/Limiter.hpp>

// using namespace mel;

// int main() {

//     MelShare ms("limiter");
//     std::vector<double> data{0, 0, 3, 10};

//     Limiter limiter(3, 10, seconds(2));

//     Timer timer(milliseconds(1));
//     while(!Keyboard::is_key_pressed(Keyboard::Escape)) {

//         if (Keyboard::is_key_pressed(Keyboard::Up))
//             data[0] += 0.01;
//         else if (Keyboard::is_key_pressed(Keyboard::Down))
//             data[0] -= 0.01;

//         data[1] = limiter.limit(data[0]);

//         ms.write_data(data);

//         timer.wait();
//     }

//     return 0;
// }


#include <MEL/Utility/RingBuffer.hpp>
#include <MEL/Utility/Console.hpp>

using namespace mel;

int main() {
    RingBuffer<double> x(3);
    x.push_back(1);
    x.push_back(2);
    x.push_back(3);
    x[2] = 57;
    print(x[0]);
    print(x[1]);
    print(x[2]);
    x.pop_front();
    x.push_back(4);


    return 0;
}
