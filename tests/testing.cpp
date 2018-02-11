#include <MEL/Math/Functions.hpp>
#include <MEL/Utility/Console.hpp>

using namespace mel;

int main(int argc, char const* argv[]) {
    std::vector<double> x{1.598,  1.302,  1,      0.7,    0.399,  0.105,
                          -0.204, -0.497, -0.806, -1.103, -1.404, -1.702,
                          1.597,  1.299,  0.996,  0.705,  0.397,  0.1,
                          -0.202, -0.5,   -0.8,   -1.02,  -1.4,   -1.702};

    std::vector<double> y{-10.14, -8.91,  -6.817, -4.769, -2.704, -0.705,
                          1.413,  3.417,  5.535,  7.574,  9.636,  11.358,
                          -10.14, -8.868, -6.789, -4.808, -2.702, -0.664,
                          1.397,  3.462,  5.509,  7.576,  9.621,  11.358};

    std::vector<double> mb = linear_regression(x, y);
    print(mb);git

    return 0;
}
