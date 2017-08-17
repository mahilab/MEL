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
    void reset();

    // PENDULUM PARAMETERS
    double g = 9.81;
    std::array<double, 2> M = { 0.01, 0.125 };   // [kg]
    std::array<double, 2> L = { 0.45, 0.375 };   // [m]
    std::array<double, 2> B = { 0.0025,0.0025 };  // [N-s/m]
    std::array<double, 2> Fk = { 0.001,0.001 }; // [Nm]

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
    std::array<double, 10> props_data;
    std::array<double, 8>  state_data; 

};