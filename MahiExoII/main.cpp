#include <iostream>
#include <csignal>
#include "Q8Usb.h"
#include "util.h"
#include "MahiExoII.h"
#include "Controller.h"
#include "Task.h"
#include <boost/program_options.hpp>
#include "Exo.h"
#include "MelShare.h"
#include "Integrator.h"

namespace po = boost::program_options;

class MelScopeTest : public mel::Task {

public:

    MelScopeTest() : Task("scope_test") {};

    mel::share::MelShare cpp2py = mel::share::MelShare("cpp2py");
    mel::share::MelShare py2cpp = mel::share::MelShare("py2cpp");
    mel::share::MelShare integrals = mel::share::MelShare("integrals");
    mel::share::MelShare MEII_state = mel::share::MelShare("MEII_state");

    std::array<double, 10> data_r = { 0.25, 0.25, 0.25, 0.25, 0.25, 0.25, 0.25, 0.25, 0.25, 0.25 };
    std::array<double, 10> data_w = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

    std::array<double, 3> data_int = { 0,0,0 };

    std::array<double, 10> data_MEII = { 0,0,0,0,0,0,0,0,0,0 };

    mel::Integrator integrator = mel::Integrator(5, mel::Integrator::Technique::Simpsons);

    void start() override {
        py2cpp.write(data_r);
    }

    void step() override {

        py2cpp.read(data_r);

        data_w[0] = mel::sin_wave(10, data_r[0], time());
        data_w[1] = mel::sin_wave(20, data_r[1], time());
        data_w[2] = mel::square_wave(10, data_r[2], time());;
        data_w[3] = mel::triangle_wave(10, data_r[3], time());
        data_w[4] = mel::sawtooth_wave(10, data_r[4], time());
        data_w[5] = mel::sin_wave(10, data_r[0], time());
        data_w[6] = mel::sin_wave(20, data_r[1], time());
        data_w[7] = mel::square_wave(10, data_r[2], time());;
        data_w[8] = mel::triangle_wave(10, data_r[3], time());
        data_w[9] = mel::sawtooth_wave(10, data_r[4], time());
        cpp2py.write(data_w);

        //data_int[0] = 0.5 * (cos(time()) + 7 * cos(7 * time())); // dx
        //data_int[1] = sin(4 * time()) * cos(3 * time()) + 5; // x, analytical
        //data_int[2] = integrator.integrate(data_int[0], time()); // x, numerical
        //integrals.write(data_int);

        //for (int i = 0; i < 12; i++)
        //    data_ow[i] = sin_wave.GetValue(time(), i, i);
        //MEII_state.write(data_w);

    }

    void stop() override {}

};

class MyTask : public mel::Task {

public:

    MyTask(MahiExoII exo, mel::Daq* daq) : Task("testing"), exo_(exo), daq_(daq) {}

    MahiExoII exo_;
    mel::Daq* daq_;
    mel::double_vec setpoints_ = { 0,0,0,0 };
    mel::double_vec traj_ = { 0,0,0,0 };
    mel::double_vec torques_ = { 0,0,0,0 };

    void start() override {
        std::cout << "Press ENTER to activate Daq <" << daq_->name_ << ">" << std::endl;
        getchar();
        daq_->activate();
        std::cout << "Press ENTER to enable MahiExoII" << std::endl;
        getchar();
        exo_.enable();
        std::cout << "Press Enter to start the controller" << std::endl;
        getchar();
        daq_->start_watchdog(0.1);
        std::cout << "Starting the controller ... " << std::endl;

        daq_->read_all();
        daq_->reload_watchdog();
        exo_.update_kinematics();
        setpoints_[0] = exo_.get_anatomical_joint_position(0);
        setpoints_[1] = exo_.get_anatomical_joint_position(1);
        setpoints_[2] = exo_.get_anatomical_joint_position(2);
        setpoints_[3] = exo_.get_anatomical_joint_position(3);

    }

    void step() override {

        daq_->read_all();
        daq_->reload_watchdog();

        exo_.update_kinematics(); // must call this to update anatomical joint positions and velocities


        traj_[0] = setpoints_[0];
        traj_[1] = setpoints_[1];
        //traj_[2] = setpoints_[2];
        traj_[3] = setpoints_[3];

        //exo_.anatomical_joint_set_points(traj_);

        //traj_[0] = setpoints_[0] + mel::sin_trajectory(5 * mel::DEG2RAD, 0.25, time());
        //traj_[1] = setpoints_[1] + mel::sin_trajectory(5 * mel::DEG2RAD, 0.25, time());
        traj_[2] = setpoints_[2] + mel::sin_wave(10 * mel::DEG2RAD, 0.25, time());
        //traj_[3] = setpoints_[3] + mel::sin_trajectory(10 * mel::DEG2RAD, 0.25, time());

        torques_[0] = mel::pd_controller(35, 0.25, traj_[0], exo_.get_anatomical_joint_position(0), 0, exo_.get_anatomical_joint_velocity(0));
        torques_[1] = mel::pd_controller(7, 0.06, traj_[1], exo_.get_anatomical_joint_position(1), 0, exo_.get_anatomical_joint_velocity(1));
        torques_[2] = mel::pd_controller(25, 0.05, traj_[2], exo_.get_anatomical_joint_position(2), 0, exo_.get_anatomical_joint_velocity(2));
        torques_[3] = mel::pd_controller(30, 0.08, traj_[3], exo_.get_anatomical_joint_position(3), 0, exo_.get_anatomical_joint_velocity(3));

        exo_.set_anatomical_joint_torques(torques_);

        daq_->write_all();


    }


    void stop() override {
        std::cout << "Stopping MyController" << std::endl;

        daq_->deactivate();
    }
    void pause() override {
        std::cout << "Pausing MyController" << std::endl;
    }
    void resume() override {
        std::cout << "Resuming MyController" << std::endl;
    }
};

class TroysTask : public mel::Task {

    TroysTask(mel::Daq* daq) : Task("troys_task"), daq_(daq) {}

    mel::Daq* daq_;

    void start() override {
         
    }
    void step() override {}
    void stop() override {}

};

int main(int argc, char * argv[]) {


    // set up program options 
    po::options_description desc("Available Options");
    desc.add_options()
        ("help", "produces help message")
        ("zero", "zeros encoder counts on startup")
        ("troy", "testing force sensor");

    po::variables_map var_map;
    po::store(po::parse_command_line(argc, argv, desc), var_map);
    po::notify(var_map);

    if (var_map.count("help")) {
        std::cout << desc << "\n";
        return -1;
    }

    //  create a Q8Usb object
    mel::uint32 id = 0;

    mel::channel_vec  ai_channels = { 0, 1, 2, 3, 4, 5, 6, 7 };
    mel::channel_vec  ao_channels = { 0, 1, 2, 3, 4 };
    mel::channel_vec  di_channels = {};
    mel::channel_vec  do_channels = { 0, 1, 2, 3, 4 };
    mel::channel_vec enc_channels = { 0, 1, 2, 3, 4 };

    mel::Q8Usb::Options options;
    for (auto it = do_channels.begin(); it != do_channels.end(); ++it) {
        options.do_initial_signals_[it - do_channels.begin()] = 1;
        options.do_final_signals_[it - do_channels.begin()] = 1;
    }




    mel::Daq* q8 = new mel::Q8Usb(id, ai_channels, ao_channels, di_channels, do_channels, enc_channels, options);

    // create and configure an MahiExoII object
    MahiExoII::Config config;
    for (int i = 0; i < 5; i++) {
        config.enable_[i] = q8->do_(i);
        config.command_[i] = q8->ao_(i);
        config.encoder_[i] = q8->encoder_(i);
        config.encrate_[i] = q8->encrate_(i);
        config.amp_gains_[i] = 1;
    }
    MahiExoII exo(config);


    // manual zero joint positions
    if (var_map.count("zero")) {
        q8->activate();
        q8->offset_encoders({ 0, -33259, 29125, 29125, 29125 });
        return 0;
    }

    if (var_map.count("troy")) {
        mel::Clock troys_clock(1000);
        mel::Controller troys_controller(troys_clock);

        troys_controller.queue_task();
    }


    // make a new Clock and Controller
    mel::Clock clock(1000, true); // 1000 Hz, clock logging enabled
    mel::Controller controller(clock);

    // queue Tasks for the Controller to execute
    controller.queue_task(new MyTask(exo, q8));

    // execute the controller
    controller.execute();

    return 0;
}