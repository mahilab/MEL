#pragma once
#include "mel_util.h"
#include <Eigen\Dense>

namespace mel {

class Model {

public:

    Model(int N) : N_(N) {}
    virtual ~Model() {}

    virtual Eigen::MatrixXd compute_M(Eigen::VectorXd q) = 0;
    virtual Eigen::VectorXd compute_V(Eigen::VectorXd q, Eigen::VectorXd q_dot) = 0;
    virtual Eigen::VectorXd compute_G(Eigen::VectorXd q) = 0;

private:

    int N_;

};

} // namespace mel
