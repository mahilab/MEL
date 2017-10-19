#pragma once
#include "mel_util.h"

namespace mel {

    namespace math {

        //-------------------------------------------------------------------------
        // INDIVIDUAL FILTER IMPLEMENTATION
        //-------------------------------------------------------------------------

        class FilterImplementation {

        public:

            FilterImplementation(const double_vec& b, const double_vec& a);

            void filter(const double& x, double& y);

            void reset(); /// sets the internal states s_ to all be zero

        private:

            int n_; /// order

            const double_vec b_; /// numerator coefficients
            const double_vec a_; /// denominator coefficients

            double_vec s_;
        };

        class Filter {

        public:

            Filter();
            Filter(const double_vec& b, const double_vec& a);
            Filter(int length, const double_vec& b, const double_vec& a);

            void filter(const double& x, double& y);
            void filter(const double_vec& x, double_vec& y);
            //void filter_offline(const double_vec& X, double_vec& Y);
            //void filter_offline(const std::vector<double_vec>& X, std::vector<double_vec>& Y);

            void reset(); /// sets the internal states s_ to all be zero

        private:

            const int length_;

            const double_vec b_; /// numerator coefficients
            const double_vec a_; /// denominator coefficients

            std::vector<FilterImplementation> filter_implementations_;
        };

    }

}

