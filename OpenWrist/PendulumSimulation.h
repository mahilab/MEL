#pragma once
#include "Task.h"
#include "MelShare.h"
#include "Integrator.h"
#include <array>
#include "util.h"

class PendulumSimulation : public mel::Task {

 public:

     PendulumSimulation(OpenWrist* open_wrist, mel::Daq* daq) : Task("pendulum"), ow_(open_wrist), daq_(daq) {}

     mel::Daq* daq_;
     OpenWrist* ow_;

     mel::share::MelShare pendulum = mel::share::MelShare("pendulum");


     // PENDULUM PARAMETERS
     std::array<double, 2> M = { 0.05, 0.5 };
     std::array<double, 2> L = { 0.25, 0.25 };
     std::array<double, 2> B = { 0.005,0.005 };
     std::array<double, 2> Fk = { 0.005,0.005 };

     std::array<double, 2> Qdd = { 0,0 };
     std::array<double, 2> Qd = { 0,0 };
     std::array<double, 2> Q = { -PI/2 ,0 };
     std::array<double, 2> Tau = { 0, 0 };

     std::array<double, 4> full_state = { 0,0,0,0 };

     double g = 9.81;

     std::array<mel::Integrator, 2> Qdd2Qd = { mel::Integrator(Qd[0]), mel::Integrator(Qd[1]) };
     std::array<mel::Integrator, 2> Qd2Q = { mel::Integrator(Q[0]), mel::Integrator(Q[1]) };

     // PENDULUM COUPLING FORCES
     double K_player = 10;
     double B_player = 0.5;        

    void start() override {
        std::cout << "Press ENTER to activate Daq <" << daq_->name_ << ">.";
        getchar();
        daq_->activate();
        std::cout << "Press ENTER to enable OpenWrist.";
        getchar();
        ow_->enable();
        daq_->zero_encoders();
        std::cout << "Press ENTER to start the controller.";
        getchar();
        daq_->start_watchdog(0.1);
        std::cout << "Executing the controller. Press CTRL+C to stop." << std::endl;
    }

    void step() override {

        daq_->reload_watchdog();
        daq_->read_all();



        Tau[0] = K_player * (ow_->joints_[0]->get_position() - PI/2 - Q[0]) + B_player * (ow_->joints_[0]->get_velocity() - Qd[0]);

        ow_->joints_[0]->set_torque(ow_->compute_gravity_compensation(0) + 0.5*ow_->compute_friction_compensation(0) - Tau[0]);
        ow_->joints_[1]->set_torque(ow_->compute_gravity_compensation(1) + 0.5*ow_->compute_friction_compensation(1));
        //ow_->joints_[2]->set_torque(ow_->compute_friction_compensation(2) * 0.5);

        Qdd[0] = -((L[0] * L[1] * M[1] * sin(Q[1])*pow(Qd[0], 2) - Tau[1] + Fk[1] * tanh(10 * Qd[1]) + B[1] * Qd[1] + g*L[1] * M[1] * cos(Q[0] + Q[1])) / (pow(L[1], 2) * M[1]) - (-L[0] * L[1] * M[1] * sin(Q[1])*pow(Qd[1], 2) - 2 * L[0] * L[1] * M[1] * Qd[0] * sin(Q[1])*Qd[1] - Tau[0] + Fk[0] * tanh(10 * Qd[0]) + B[0] * Qd[0] + g*L[1] * M[1] * cos(Q[0] + Q[1]) + g*L[0] * M[0] * cos(Q[0]) + g*L[0] * M[1] * cos(Q[0])) / (L[1] * M[1] * (L[1] + L[0] * cos(Q[1])))) / ((M[1] * pow(L[1], 2) + L[0] * M[1] * cos(Q[1])*L[1]) / (pow(L[1], 2) * M[1]) - (pow(L[0], 2) * M[0] + pow(L[0], 2) * M[1] + pow(L[1], 2) * M[1] + 2 * L[0] * L[1] * M[1] * cos(Q[1])) / (L[1] * M[1] * (L[1] + L[0] * cos(Q[1]))));
        Qdd[1] = ((-L[0] * L[1] * M[1] * sin(Q[1])*pow(Qd[1], 2) - 2 * L[0] * L[1] * M[1] * Qd[0] * sin(Q[1])*Qd[1] - Tau[0] + Fk[0] * tanh(10 * Qd[0]) + B[0] * Qd[0] + g*L[1] * M[1] * cos(Q[0] + Q[1]) + g*L[0] * M[0] * cos(Q[0]) + g*L[0] * M[1] * cos(Q[0])) / (pow(L[0], 2) * M[0] + pow(L[0], 2) * M[1] + pow(L[1], 2) * M[1] + 2 * L[0] * L[1] * M[1] * cos(Q[1])) - (L[0] * L[1] * M[1] * sin(Q[1])*pow(Qd[0], 2) - Tau[1] + Fk[1] * tanh(10 * Qd[1]) + B[1] * Qd[1] + g*L[1] * M[1] * cos(Q[0] + Q[1])) / (L[1] * M[1] * (L[1] + L[0] * cos(Q[1])))) / (L[1] / (L[1] + L[0] * cos(Q[1])) - (M[1] * pow(L[1], 2) + L[0] * M[1] * cos(Q[1])*L[1]) / (pow(L[0], 2) * M[0] + pow(L[0], 2) * M[1] + pow(L[1], 2) * M[1] + 2 * L[0] * L[1] * M[1] * cos(Q[1])));

        Qd[0] = Qdd2Qd[0].integrate(Qdd[0], time());
        Qd[1] = Qdd2Qd[1].integrate(Qdd[1], time());

        Q[0] = Qd2Q[0].integrate(Qd[0], time());
        Q[1] = Qd2Q[1].integrate(Qd[1], time());

        full_state[0] = Q[0];
        full_state[1] = Q[1];
        full_state[2] = Tau[0];
        full_state[3] = Tau[1];

        ow_->update_state_map();
        pendulum.write(full_state);

        daq_->write_all();
    }

    void stop() override {
        ow_->disable();
        daq_->deactivate();
    }

};