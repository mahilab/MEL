#include <MEL/Math/Waveform.hpp>
#include <MEL/Math/Functions.hpp>
#include <MEL/Math/Constants.hpp>

namespace mel {

//==============================================================================
// CLASS DEFINITIONS
//==============================================================================

Waveform::Waveform(Type type, Time period, double amplitude, double offset) :
    type_(type),
    period_(period),
    amplitude_(amplitude),
    offset_(offset)
{
}

double Waveform::evaluate(Time t) {
    double seconds = t.as_seconds();
    double frequency = 1.0 / period_.as_seconds();
    double value = 0.0;
    switch(type_) {
        case Sin:
            value = sin(2.0 * PI * frequency * seconds);
            break;
        case Cos:
            value = cos(2.0 * PI * frequency * seconds);
            break;
        case Square:
            value = sin(2.0 * PI * frequency * seconds);
            if (value == 0.0)
                value = 0.0;
            else if (value > 0.0)
                value = 1.0;
            else
                value = -1.0;
            break;
        case Triangle:
            value = 2 / PI * asin(sin(2.0 * PI * frequency * seconds));
            break;
        case Sawtooth:
            value = -2 / PI * atan(cos(PI * frequency * seconds) / sin(PI * frequency * seconds));
            break;
    }
    value *= amplitude_;
    value += offset_;
    return value;
}

} // namespace mel


