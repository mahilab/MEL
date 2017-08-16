#pragma once
#include "util.h"

namespace mel {

    class Integrator {

    public:

        enum class Technique { Trapezoidal = 0, Simpsons = 1 };

        Integrator(double initial_value, Technique technique = Technique::Trapezoidal);

        double integrate(double x, double t);

    private:

        // integration technique to be used
        Technique technique_;

        // history
        uint32 step_count_ = 0;

        // previous state(s)
        double last_last_x_ = 0.0;
        double last_last_t_ = 0.0;
        double last_x_ = 0.0;
        double last_t_ = 0.0;

        // integrated value
        double integral_ = 0.0;


    };

}