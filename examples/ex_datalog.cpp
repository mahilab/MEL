#include <MEL/Utility/DataLog.hpp>
#include <MEL/Utility/Console.hpp>

using namespace mel;

class MyClass {
public:
    MyClass() : log_({"my_int", "my_double"}){ }
    DataLog<int, double> log_;
};

template<typename T>
T sum(T v) {
    std::cout << __PRETTY_FUNCTION__ << "\n";
    return v;
}

template <typename T, typename ... Ts>
T sum(T first, Ts ... rest) {
    std::cout << __PRETTY_FUNCTION__ << "\n";
    return first + sum(rest...);
}

int main(int argc, char const *argv[]) {

    int s = sum(1,2,3,4,5,6,7,8,9,10);
    print(s);

    MyClass x;
    x.log_.set_format(Format::Scientific);
    x.log_.set_precision(10);
    for (int i = 0; i < 1000; ++i) {
        x.log_.add_row(i, i * 3.14);
    }
    x.log_.save_data("log0", "logs");
    x.log_.wait_for_save();
    return 0;
}

