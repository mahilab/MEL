#include <MEL/Communications/Windows/MelShare.hpp>
#include <MEL/Utility/Windows/Keyboard.hpp>
#include <MEL/Utility/Timer.hpp>
#include <MEL/Core/Limiter.hpp>

using namespace mel;

int main() {

    MelShare ms("limiter");
    std::vector<double> data{0, 0, 3, 10};

    Limiter limiter(3, 10, seconds(2));

    Timer timer(milliseconds(1));
    while(!Keyboard::is_key_pressed(Keyboard::Escape)) {

        if (Keyboard::is_key_pressed(Keyboard::Up))
            data[0] += 0.01;
        else if (Keyboard::is_key_pressed(Keyboard::Down))
            data[0] -= 0.01;

        data[1] = limiter.limit(data[0]);

        ms.write_data(data);

        timer.wait();
    }

    return 0;
}


