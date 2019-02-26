#include <MEL/Math/TimeFunction.hpp>

namespace mel {

    double TimeFunction::operator()(Time t) {
        return evaluate(t);
    }

}