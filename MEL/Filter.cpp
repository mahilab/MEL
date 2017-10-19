#include "Filter.h"

namespace mel {

    namespace math {

        Filter::Filter() :
            Filter({ 0.020083365564211, 0.040166731128423, 0.020083365564211 }, { 1.000000000000000, -1.561018075800718, 0.641351538057563 })
        { }

        Filter::Filter(const double_vec& b, const double_vec& a) :
            Filter(1, b, a)
        { }

        Filter::Filter(int length, const double_vec& b, const double_vec& a) :
            length_(length),
            b_(b),
            a_(a),
            filter_implementations_(std::vector<FilterImplementation>(length, FilterImplementation(b, a)))
        { }

        FilterImplementation::FilterImplementation(const double_vec& b, const double_vec& a) :
            b_(b),
            a_(a)
        {
            if (a_.size() != b_.size()) {
                std::cout << "ERROR: Coefficient vector sizes do not match." << std::endl;
            }
            else if (a_.size() < 2) {
                std::cout << "ERROR: Coefficient vector must be longer than 1." << std::endl;
            }
            else {
                n_ = a_.size() - 1;
                s_ = double_vec(n_, 0.0);
            }
        }


        void FilterImplementation::filter(const double& x, double& y) {

            y = (s_[0] + b_[0] * x) / a_[0];
            for (int i = 0; i < n_; ++i) {
                s_[i] = s_[i + 1] + b_[i + 1] * x - a_[i + 1] * y;
            }

            return;
        }

        void Filter::filter(const double& x, double& y) {

            if (length_ != 1) {
                std::cout << "ERROR: Input vector does not match size of filter bank." << std::endl;
                return;
            }

            filter_implementations_[0].filter(x, y);
            return;
        }

        void Filter::filter(const double_vec& x, double_vec& y) {

            if (x.size() != length_) {
                std::cout << "ERROR: Input vector does not match size of filter bank." << std::endl;
                return;
            }

            for (int j = 0; j < length_; ++j) {
                filter_implementations_[j].filter(x[j], y[j]);
            }
            return;
        }

        /*void Filter::filter_offline(const double_vec& X, double_vec& Y) {

            this->reset();

            if (length_ != 1) {
                std::cout << "ERROR: Number of rows in input matrix X do not match size of filter bank." << std::endl;
                return;
            }

            int num_samples = X.size();
            if (Y.size() != num_samples) {
                std::cout << "ERROR: Input and output matrix row lengths are inconsistent." << std::endl;
                return;
            }

            double_vec W(num_samples);

            for (int j = 0; j < num_samples; ++j) {
                W[j] = filter(X[j]);
            }

            std::reverse(W.begin(), W.end());

            for (int j = 0; j < num_samples; ++j) {
                Y[j] = filter(W[j]);
            }
        }*/


        /*void Filter::filter_offline(const std::vector<double_vec>& X, std::vector<double_vec>& Y) {

            this->reset();

            if (X.size() != length_) {
                std::cout << "ERROR: Number of rows in input matrix X do not match size of filter bank." << std::endl;
                return;
            }

            int num_samples = X[0].size();
            bool num_samples_inconsistent = false;
            for (int i = 0; i < length_; ++i) {
                if (X[i].size() != num_samples) {
                    num_samples_inconsistent = true;  
                }
            }
            if (num_samples_inconsistent) {
                std::cout << "ERROR: Input matrix row lengths are inconsistent." << std::endl;
                return;
            }
            if (Y.size() != num_samples) {
                std::cout << "ERROR: Input and output matrix row lengths are inconsistent." << std::endl;
                return;
            }

            double_vec x(length_);
            double_vec y(length_);

            for (int j = 0; j < num_samples; ++j) {
                for (int i = 0; i < length_; ++i) {
                    x[i] = X[i][j];
                }
                y = filter(x);
                for (int i = 0; i < length_; ++i) {
                    Y[i][j] = y[i];
                }
            }
        }*/

        void Filter::reset() {
            for (int j = 0; j < length_; ++j) {
                filter_implementations_[j].reset();
            }
        }

        void FilterImplementation::reset() {
            s_ = double_vec(n_, 0.0);
        }

    }

}