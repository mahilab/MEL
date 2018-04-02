#include <MEL/Math/TeagerKaiserEnergyOperator.hpp>

namespace mel {

TeagerKaiserEnergyOperator::TeagerKaiserEnergyOperator() :
    Process(),
    s_(std::vector<double>(n_, 0.0))
{}

double TeagerKaiserEnergyOperator::update( const double x, const Time& current_time) {
    Time unused = current_time; // unused
    double y = s_[0] * s_[0] - x * s_[1];
    s_[1] = s_[0];
    s_[0] = x;  
    return y;
}


void TeagerKaiserEnergyOperator::reset() {
    s_ = std::vector<double>(n_, 0.0);
}


} // mel