#pragma once
#include "mel_util.h"

namespace mel {

    namespace math {

        class Integrator {

        public:

            enum class Technique { Trapezoidal = 0, Simpsons = 1 };

            Integrator(double initial_value, Technique technique = Technique::Trapezoidal);

            /// Integrats x with respect to t
            double integrate(double x, double t);
            /// Resets the integrators
            void reset(double initial_value);

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

}