#pragma once
#include "util.h"

namespace mel {

    class Filter {

    public:

        Filter();
        Filter(int order, double_vec b, double_vec a);
        Filter(int length, int order, double_vec b, double_vec a);

        double filter(double);
        double_vec filter(double_vec x);

    private:

        class FilterImplementation {

        public:

            FilterImplementation(int order, double_vec b, double_vec a);

            double filter(double x);

        private:

            const int n_;

            double_vec a_;
            double_vec b_;

            double_vec s_;
            double y_;
        };

        const int length_;
        const int n_;

        double_vec a_;
        double_vec b_;

        double_vec y_;

        std::vector<FilterImplementation*> filter_implementations_;

    };

}

