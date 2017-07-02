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
        
        std::string id = "0";
        mel::channel_vec  ai_channels = { 0, 1, 2, 3 };
        mel::channel_vec  ao_channels = { 0, 1, 2, 3 };
        mel::channel_vec  di_channels = { 0, 1, 2, 3 };
        mel::channel_vec  do_channels = { 0, 1, 2, 3 };
        mel::channel_vec enc_channels = { 0, 1, 2, 3 };
        char options[] = "ch0_mode=2;ch0_kff=0;ch0_a0=-1.382;ch0_a1=8.03;ch0_a2=0;ch0_b0=-1;ch0_b1=0;ch0_post=1000;ch1_mode=2;ch1_kff=0;ch1_a0=-1.382;ch1_a1=8.03;ch1_a2=0;ch1_b0=-1;ch1_b1=0;ch1_post=1000;ch2_mode=2;ch2_kff=0;ch2_a0=1.912;ch2_a1=18.43;ch2_a2=0;ch2_b0=-1;ch2_b1=0;ch2_post=1000;update_rate=fast;ext_int_polarity=0;convert_polarity=1;watchdog_polarity=0;ext_int_watchdog=0;use_convert=0;pwm_immediate=0;decimation=1";
        mel::Q8Usb::Options q8_options;

        q8 = new mel::Q8Usb(id, ai_channels, ao_channels, di_channels, do_channels, enc_channels, options);

        do0 = q8->do_(0);
        ao0 = q8->ao_(0);
        ai0 = q8->ai_(0);
        enc0 = q8->encoder_(0);
        encr0 = q8->encrate_(0);

        q8->activate();
        q8->start_watchdog(0.1);

        do0.set_signal(1);
        ao0.set_voltage(2);
        q8->zero_encoders();
    }
    void step() override {
        q8->read_all();
        q8->reload_watchdog();
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
    mel::Clock my_clock(100, true);
    mel::ControlLoop my_loop(my_clock);

    // request users permission to execute the controller
    std::cout << "Press ENTER to execute the controller. CTRL+C will stop the controller once it's started." << std::endl;
    getchar();

    // queue controllers
    my_loop.queue_controller(clock_tester);

    // execute the controller
    my_loop.execute();    

    // delete controller
    delete my_controller;
    delete clock_tester;

    return 0;
}