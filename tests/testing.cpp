#include <MEL/Math/Butterworth.hpp>
#include <MEL/Utility/Frequency.hpp>
#include <MEL/Utility/Console.hpp>

using namespace mel;

int main(int argc, char* argv[]) {
    Butterworth butter(5, hertz(25), hertz(1000), Butterworth::Highpass);
    print(butter.get_b());
    print(butter.get_a());
    return 0;
}
