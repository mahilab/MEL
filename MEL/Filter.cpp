#include "Filter.h"

namespace mel {

    namespace math {

        Filter::Filter() :
            Filter(2, { 0.020083365564211, 0.040166731128423, 0.020083365564211 }, { 1.000000000000000, -1.561018075800718, 0.641351538057563 })
        { }

        Filter::Filter(int order, double_vec b, double_vec a) :
            Filter(1, order, b, a)
        { }

        Filter::Filter(int length, int order, double_vec b, double_vec a) :
            length_(length),
            n_(order),
            a_(a),
            b_(b),
            y_(double_vec(length, 0))
        {
            filter_implementations_ = std::vector<FilterImplementation>(length_, FilterImplementation(order, b, a));
        }

        Filter::FilterImplementation::FilterImplementation(int order, double_vec b, double_vec a) :
            n_(order),
            a_(a),
            b_(b),
            s_(double_vec(order - 1, 0))
        { }


        double Filter::FilterImplementation::filter(double x) {

            y_ = (s_[0] + b_[0] * x) / a_[0];
            for (int i = 0; i < n_; ++i) {
                s_[i] = s_[i + 1] + b_[i + 1] * x - a_[i + 1] * y_;
            }
            s_[n_ - 1] = b_[n_] * x - a_[n_] * y_;

            return y_;
        }

        double Filter::filter(double x) {

            if (length_ != 1) {
                std::cout << "Error: Input vector does not match size of filter bank" << std::endl;
                return { 1 };
            }

            y_[0] = filter_implementations_[0].filter(x);

            return y_[0];
        }

        double_vec Filter::filter(double_vec x) {

            if (x.size() != length_) {
                std::cout << "Error: Input vector does not match size of filter bank" << std::endl;
                return { 1 };
            }

            for (int j = 0; j < length_; ++j) {
                y_[j] = filter_implementations_[j].filter(x[j]);
            }

            return y_;
        }

        void Filter::reset() {
            for (int j = 0; j < length_; ++j) {
                filter_implementations_[j].reset();
            }
        }

        void Filter::FilterImplementation::reset() {
            s_ = double_vec(n_ - 1, 0.0);
        }

    }

}