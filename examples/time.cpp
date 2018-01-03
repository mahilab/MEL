#include <MEL/Utility/Clock.hpp>
#include <MEL/Utility/mel_util.hpp>
#include <MEL/Math/mel_math.hpp>
#include <vector>

int main() {
    // mel::enable_realtime();
    std::vector<double> v(1000);
    for (int i = 0; i < 1000; ++i) {
        mel::Time time = mel::microseconds(1000);
        mel::Clock clock;
        mel::sleep(time);
        mel::int64 el = clock.get_elapsed_time().as_microseconds();
        v[i] = (double)el;
    }
    // mel::disable_realtime();
    mel::print(mel::mean(v));
}
