#include <MEL/Math/Filter.hpp>

namespace mel {

class Butterworth {
public:
    Butterworth(int order, double Wn);

private:
    int order_;

};

};
