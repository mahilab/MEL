#pragma once
#include <vector>
#include <MEL/Utility/mel_types.hpp>

namespace mel {

class DenavitHartenberg {

public:

    DenavitHartenberg(int N, std::vector<double> a, std::vector<double> alpha, std::vector<double> d, std::vector<double> theta);

private:

    const std::vector<double> a_;
    const std::vector<double> alpha_;
    const std::vector<double> d_;
    const std::vector<double> theta_;

};

}


