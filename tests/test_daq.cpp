#include <MEL/Daq/Input.hpp>
#include <MEL/Daq/Output.hpp>
#include <MEL/Daq/InputOutput.hpp>
#include <MEL/Utility/Console.hpp>
#include <MEL/Daq/Encoder.hpp>

using namespace mel;

class TestOutput : public InputOutput<double> {
public:

    TestOutput() {
        values_.set_default_value(1000);
    }

    bool update_channel(uint32 channel_number) override {
        std::cout << "Updated channel " << channel_number << std::endl;
        return true;
    }

    void debug() {
        print("----");
        print(get_channel_count());
        print(get_channel_numbers());
        print(values_);
    }
};


int main(int argc, char* argv[]) {

    TestOutput out;
    out.debug();

    out.set_channel_numbers({ 1,2,3 });
    out.set_values({ 10,20,30 });
    out.debug();

    out.set_channel_numbers({ 1,3,4 });
    out.debug();

    return 0;
}
