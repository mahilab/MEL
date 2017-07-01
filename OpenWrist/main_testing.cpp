#include <iostream>
#include "Controller.h"
#include "ControlLoop.h"
#include "Clock.h"
#include "Daq.h"
#include "Q8Usb.h"


// Controller implementation minimum working example
class MyController : public mel::Controller {

    mel::Daq* q8;

    void start() override {
        std::cout << "Starting MyController" << std::endl;
        
        std::string id = "0";
        mel::channel_vec  ai_channels = { 0, 1, 2, 3 };
        mel::channel_vec  ao_channels = { 0, 1, 2, 3 };
        mel::channel_vec  di_channels = { 0, 1, 2, 3 };
        mel::channel_vec  do_channels = { 0, 1, 2, 3 };
        mel::channel_vec enc_channels = { 0, 1, 2, 3 };
        char options[] = "ch0_mode=2;ch0_kff=0;ch0_a0=-1.382;ch0_a1=8.03;ch0_a2=0;ch0_b0=-1;ch0_b1=0;ch0_post=1000;ch1_mode=2;ch1_kff=0;ch1_a0=-1.382;ch1_a1=8.03;ch1_a2=0;ch1_b0=-1;ch1_b1=0;ch1_post=1000;ch2_mode=2;ch2_kff=0;ch2_a0=1.912;ch2_a1=18.43;ch2_a2=0;ch2_b0=-1;ch2_b1=0;ch2_post=1000;update_rate=fast;ext_int_polarity=0;convert_polarity=1;watchdog_polarity=0;ext_int_watchdog=0;use_convert=0;pwm_immediate=0;decimation=1";

        q8 = new mel::Q8Usb(id, ai_channels, ao_channels, di_channels, do_channels, enc_channels, options);

        q8->activate();
        q8->start_watchdog(0.1);
    }
    void step() override {
        q8->read_all();

        q8->ai_channel(0).get_voltage();
        q8->ai_channel(1).get_voltage();
        q8->ai_channel(2).get_voltage();
        q8->ai_channel(3).get_voltage();

        q8->ao_channel(0).set_voltage(0);
        q8->ao_channel(1).set_voltage(0);
        q8->ao_channel(2).set_voltage(0);
        q8->ao_channel(3).set_voltage(0);

        q8->di_channel(0).get_signal();
        q8->di_channel(1).get_signal();
        q8->di_channel(2).get_signal();
        q8->di_channel(3).get_signal();

        q8->do_channel(0).set_signal(0);
        q8->do_channel(1).set_signal(1);
        q8->do_channel(2).set_signal(1);
        q8->do_channel(3).set_signal(1);

        q8->encoder_channel(0).get_count();
        q8->encoder_channel(1).get_count();
        q8->encoder_channel(2).get_count();
        q8->encoder_channel(3).get_count();

        q8->encrate_channel(0).get_rate();
        q8->encrate_channel(1).get_rate();
        q8->encrate_channel(2).get_rate();
        q8->encrate_channel(3).get_rate();

        q8->write_all();

        q8->log_data();

        q8->reload_watchdog();

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

int main(int argc, char * argv[]) {
    
    // create an
    mel::Controller* my_controller = new MyController();
    mel::Clock my_clock(1000, true);
    mel::ControlLoop my_loop(my_clock);

    std::vector<double> evan(2,1);
    evan[0] = 1;

    // request users permission to execute the controller
    std::cout << "Press ENTER to execute the controller. CTRL+C will stop the controller once it's started." << std::endl;
    getchar();

    // queue controllers
    my_loop.queue_controller(my_controller);

    // execute the controller
    my_loop.execute();

    // delete controller
    delete my_controller;

    return 0;
}