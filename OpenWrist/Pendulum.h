#pragma once
#include <array>
#include "util.h"
#include "Integrator.h"
#include "MelShare.h"

class Pendulum {

public:

    Pendulum();

    /// Steps the pendulum simulation
    void step_simulation(double time, double position_ref, double velocity_ref);

    // PENDULUM PARAMETERS
    double g = 9.81;
    std::array<double, 2> M = { 0.05, 0.25 };   // [kg]
    std::array<double, 2> L = { 0.45, 0.25 };   // [m]
    std::array<double, 2> B = { 0.005,0.005 };  // [N-s/m]
    std::array<double, 2> Fk = { 0.005,0.005 }; // [Nm]

    // PENDULUM COUPLING FORCES
    double K_player = 12;
    double B_player = 0.6;

    // STATE VARIABLES
    std::array<double, 2> Qdd = { 0,0 };
    std::array<double, 2> Qd = { 0,0 };
    std::array<double, 2> Q = { -mel::PI / 2  ,0 };
    std::array<double, 2> Tau = { 0, 0 };

private:

    // INTEGRATORS
    std::array<mel::Integrator, 2> Qdd2Qd = { mel::Integrator(Qd[0]), mel::Integrator(Qd[1]) };
    std::array<mel::Integrator, 2> Qd2Q = { mel::Integrator(Q[0]), mel::Integrator(Q[1]) };

    // MELSHARES
    mel::share::MelShare props = mel::share::MelShare("pendulum_props");
    mel::share::MelShare state = mel::share::MelShare("pendulum_state");

    // MELSHARE DATA
    std::array<double, 10> props_data = { M[0],M[1],L[0],L[1],B[0],B[1],Fk[0],Fk[1],K_player,B_player };
    std::array<double, 8>  state_data = { Qdd[0], Qdd[1], Qd[0], Qd[1], Q[0], Q[1], Tau[0], Tau[1] };

};