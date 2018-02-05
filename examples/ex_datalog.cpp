#include <MEL/Logging/DataLog.hpp>
#include <MEL/Utility/Console.hpp>
#include <MEL/Math/Constants.hpp>

using namespace mel;

class MyClass {
public:
    MyClass() : log_({"my_int", "my_double"}){ }
    DataLog<int, double> log_;
};


int main(int argc, char const *argv[]) {
    MyClass x;
    x.log_.set_format(Format::Scientific);
    x.log_.set_precision(10);
    for (int i = 0; i < 1000; ++i) {
        x.log_.add_row(i, i * PI);
    }
    x.log_.save_data("log0", "logs");
    x.log_.wait_for_save();
    return 0;
}

