#include <iostream>
#include "Controller.h"
#include "ControlLoop.h"
#include "Clock.h"
#include "Daq.h"
#include "Q8Usb.h"
#include "Encoder.h"


// Controller implementation minimum working example
class MyController : public mel::Controller {

    mel::Daq* q8;
    mel::Daq::Do do0;
    mel::Daq::Ao ao0;
    mel::Daq::Ai ai0;
    mel::Daq::Encoder enc0;
    mel::Daq::EncRate encr0;

    mel::Encoder encoder0;

    void start() override {
        std::cout << "Starting MyController" << std::endl;
        
        //  Construct Q8 USB instance
        mel::uint32 id = 0;
        mel::channel_vec  ai_channels = { 0, 1, 2, 3 };
        mel::channel_vec  ao_channels = { 0, 1, 2, 3 };
        mel::channel_vec  di_channels = { 0, 1, 2, 3 };
        mel::channel_vec  do_channels = { 0, 1, 2, 3 };
        mel::channel_vec enc_channels = { 0, 1, 2, 3 };

        mel::Q8Usb::Options options;
        options.update_rate_ = mel::Q8Usb::Options::UpdateRate::Fast_8kHz;
        options.decimation_ = 1;
        options.ao_modes_[0] = mel::Q8Usb::Options::AoMode(mel::Q8Usb::Options::AoMode::CurrentMode1, 0, -1.382, 8.030, 0, -1, 0, 1000);
        options.ao_modes_[1] = mel::Q8Usb::Options::AoMode(mel::Q8Usb::Options::AoMode::CurrentMode1, 0, -1.382, 8.030, 0, -1, 0, 1000);
        options.ao_modes_[2] = mel::Q8Usb::Options::AoMode(mel::Q8Usb::Options::AoMode::CurrentMode1, 0,  1.912, 18.43, 0, -1, 0, 1000);

        q8 = new mel::Q8Usb(id, ai_channels, ao_channels, di_channels, do_channels, enc_channels, options);

        do0 = q8->do_(0);
        ao0 = q8->ao_(0);
        ai0 = q8->ai_(0);
        enc0 = q8->encoder_(2);
        encr0 = q8->encrate_(2);

        q8->activate();
        q8->start_watchdog(0.1);

        encoder0 = mel::Encoder("joint_0", 500 * 1 / ( 2 * mel::PI ), enc0, encr0);

        do0.set_signal(1);
        ao0.set_voltage(2);
        q8->zero_encoders();
    }
    void step() override {
        q8->read_all();
        q8->reload_watchdog();
        std::cout << enc0.get_count() << " " << encr0.get_rate() << " " << enc0.get_quadrature_factor() << " " << encoder0.get_position() * 0.234 / 6 * mel::RAD2DEG << std::endl;
        q8->write_all();

        //std::cout << ai0.get_voltage() << std::endl;
    }
    void stop() override {
        std::cout << "Stopping MyController" << std::endl;
        q8->deactivate();
    }
    void pause() override {
        std::cout << "Pausing MyController" << std::endl;
    }
    void resume() override {
        std::cout << "Resuming MyController" << std::endl;
    }
};

class ClockTester : public mel::Controller {
    void start() override { std::cout << "Starting ClockTest" << std::endl; }
    void step() override { std::cout << get_time() << std::endl; }
    void stop() override { std::cout << "Stopping ClockTester" << std::endl; }
};

int main(int argc, char * argv[]) {
    

    // create an
    mel::Controller* my_controller = new MyController();
    mel::Controller* clock_tester = new ClockTester();
    mel::Clock my_clock(1000, true);
    mel::ControlLoop my_loop(my_clock);

    // request users permission to execute the controller
    std::cout << "Press ENTER to execute the controller. CTRL+C will stop the controller once it's started." << std::endl;
    getchar();

    // queue controllers
    my_loop.queue_controller(my_controller);

    // execute the controller
    my_loop.execute();    

    // delete controller
    delete my_controller;
    delete clock_tester;

    return 0;
}