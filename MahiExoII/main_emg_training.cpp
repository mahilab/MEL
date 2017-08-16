#include <iostream>
#include <csignal>
#include "Q8Usb.h"
#include "util.h"
#include "mahiexoii_util.h"
#include "MahiExoII.h"
#include "MahiExoIIEmgFrc.h"
#include "Controller.h"
#include "Task.h"
#include <boost/program_options.hpp>
#include "Exo.h"
#include "EmgTraining.h"
#include "MelShare.h"
#include "Integrator.h"

/*
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
*/

namespace po = boost::program_options;


int main(int argc, char * argv[]) {

    /*
    //Test MelShare Scope
    mel::Clock clock_test(1000);
    mel::Controller controller(clock_test);
    mel::Task* task = new MelScopeTest();
    controller.queue_task(task);
    controller.execute();
    return 0;
    */

    // set up program options 
    po::options_description desc("Available Options");
    desc.add_options()
        ("help", "produces help message")
        ("zero", "zeros encoder counts on startup");

    po::variables_map var_map;
    po::store(po::parse_command_line(argc, argv, desc), var_map);
    po::notify(var_map);

    if (var_map.count("help")) {
        std::cout << desc << "\n";
        return -1;
    }


    mel::uint32 id_emg = 0;
    mel::uint32 id_ati = 1;
    if (!check_digital_loopback(0, 7)) {
        mel::print("Warning: Digital loopback not connected to Q8Usb 0");
        id_emg = 1;
        id_ati = 0;
    }

    //  create a Q8Usb object
    mel::channel_vec  ai_channels = { 0, 1, 2, 3, 4, 5, 6, 7 };
    mel::channel_vec  ao_channels = { 0, 1, 2, 3, 4 };
    mel::channel_vec  di_channels = { 0, 1, 2, 3, 4, 5, 6, 7 };
    mel::channel_vec  do_channels = { 0, 1, 2, 3, 4, 5, 6, 7 };
    mel::channel_vec enc_channels = { 0, 1, 2, 3, 4 };

    mel::Q8Usb::Options options;
    for (int i = 0; i < 8; ++i) {
        options.do_initial_signals_[i] = 1;
        options.do_final_signals_[i] = 1;
        options.do_expire_signals_[i] = 1;
    }

    mel::Daq* q8_emg = new mel::Q8Usb(id_emg, ai_channels, ao_channels, di_channels, do_channels, enc_channels, options);


    //  create a second Q8Usb object
    ai_channels = { 0, 1, 2, 3, 4, 5 };
    ao_channels = {};
    di_channels = {};
    do_channels = {};
    enc_channels = {};

    mel::Daq* q8_ati = new mel::Q8Usb(id_ati, ai_channels, ao_channels, di_channels, do_channels, enc_channels);


    // create and configure a MahiExoII object
    /*MahiExoII::Config config;
    for (int i = 0; i < 5; ++i) {
    config.enable_[i] = q8_emg->do_(i);
    config.command_[i] = q8_emg->ao_(i);
    config.encoder_[i] = q8_emg->encoder_(i);
    config.encrate_[i] = q8_emg->encrate_(i);
    }
    MahiExoII* meii = new MahiExoII(config);*/
    /*MahiExoIIFrc::Config config;
    for (int i = 0; i < 5; ++i) {
    config.enable_[i] = q8_emg->do_(i);
    config.command_[i] = q8_emg->ao_(i);
    config.encoder_[i] = q8_emg->encoder_(i);
    config.encrate_[i] = q8_emg->encrate_(i);
    }
    for (int i = 0; i < 6; ++i) {
    config.wrist_force_sensor_[i] = q8_ati->ai_(i);
    }
    MahiExoIIFrc* meii = new MahiExoIIFrc(config);*/
    /*MahiExoIIEmg::Config config;
    for (int i = 0; i < 5; ++i) {
        config.enable_[i] = q8_emg->do_(i);
        config.command_[i] = q8_emg->ao_(i);
        config.encoder_[i] = q8_emg->encoder_(i);
        config.encrate_[i] = q8_emg->encrate_(i);
    }
    for (int i = 0; i < 8; ++i) {
        config.emg_[i] = q8_emg->ai_(i);
    }
    MahiExoIIEmg* meii = new MahiExoIIEmg(config);*/

    MahiExoIIEmgFrc::Config config;
    for (int i = 0; i < 5; ++i) {
        config.enable_[i] = q8_emg->do_(i);
        config.command_[i] = q8_emg->ao_(i);
        config.encoder_[i] = q8_emg->encoder_(i);
        config.encrate_[i] = q8_emg->encrate_(i);
    }
    for (int i = 0; i < 8; ++i) {
        config.emg_[i] = q8_emg->ai_(i);
    }
    for (int i = 0; i < 6; ++i) {
        config.wrist_force_sensor_[i] = q8_ati->ai_(i);
    }
    MahiExoIIEmgFrc* meii = new MahiExoIIEmgFrc(config);

    // manual zero joint positions
    if (var_map.count("zero")) {
        q8_emg->enable();
        q8_emg->offset_encoders({ 0, -33259, 29125, 29125, 29125 });
        q8_emg->disable();
        return 0;
    }

    // create clock
    mel::Clock clock(1000);


    EmgTraining sm = EmgTraining(clock, q8_emg, q8_ati, meii);

    sm.execute();


    return 0;

}