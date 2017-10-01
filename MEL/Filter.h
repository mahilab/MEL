#pragma once
#include "mel_util.h"

namespace mel {

    namespace math {

        class Filter {

        public:

            Filter();
            Filter(int order, double_vec b, double_vec a);
            Filter(int length, int order, double_vec b, double_vec a);

            double filter(double);
            double_vec filter(double_vec x);

            void reset(); /// sets the internal states s_ to all be zero

        private:

            //-------------------------------------------------------------------------
            // INDIVIDUAL FILTER IMPLEMENTATION
            //-------------------------------------------------------------------------

            class FilterImplementation {

            public:

                FilterImplementation(int order, double_vec b, double_vec a);

                double filter(double x);

                void reset(); /// sets the internal states s_ to all be zero

            private:

                const int n_;

                double_vec b_; /// numerator coefficients
                double_vec a_; /// denominator coefficients

                double_vec s_;
                double y_;
            };

            //-------------------------------------------------------------------------
            // MEMBERS OF THE FILTER CLASS
            //-------------------------------------------------------------------------

            const int length_;
            const int n_;

            double_vec a_;
            double_vec b_;

            double_vec y_;

            std::vector<FilterImplementation> filter_implementations_;
        };

    }

}

