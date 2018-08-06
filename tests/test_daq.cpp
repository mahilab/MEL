#include <MEL/Daq/Input.hpp>
#include <MEL/Daq/Output.hpp>
#include <MEL/Daq/InputOutput.hpp>
#include <MEL/Utility/Console.hpp>
#include <MEL/Daq/Encoder.hpp>

using namespace mel;

class TestOutput : public InputOutput<double> {
public:

    bool update_channel(uint32 channel_number) override {
        std::cout << "Updated channel " << channel_number << std::endl;
        return true;
    }

    void debug() {
        print("----");
        print(get_channel_count());
        print(get_channel_numbers());
        print(values_);
        print(min_values_);
        print(max_values_);
        print(enable_values_);
        print(disable_values_);
        print(expire_values_);
        print(directions_);
        print(input_channel_numbers_);
        print(output_channel_numbers_);
    }

};

class TestEncoder : public Encoder {
public:
    bool update_channel(uint32 channel_number) override {
        std::cout << "Updated channel " << channel_number << std::endl;
        return true;
    }

    bool reset_counts(const std::vector<int32>& counts) override {
        return true;
    }

    bool reset_count(uint32 channel_number, int32 count) override {
        return true;
    }

    bool set_quadrature_factors(const std::vector<QuadFactor>& factors) override {
        return Encoder::set_quadrature_factors(factors);
    }

    bool set_quadrature_factor(uint32 channel_number, QuadFactor factor) override {
        return Encoder::set_quadrature_factor(channel_number, factor);
    }


    void debug() {
        print("----");
        print(get_channel_count());
        print(get_channel_numbers());
        print(values_);
        print(min_values_);
        print(max_values_);
        print(factors_);
        print(units_per_count_);
        print(get_positions());
    }
};


int main(int argc, char* argv[]) {
    TestOutput out;
    out.set_channel_numbers({ 1,2,3 });

    out.debug();
    return 0;
}
