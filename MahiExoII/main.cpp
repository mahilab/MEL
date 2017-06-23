#include <iostream>
#include <csignal>
#include "Q8Usb.h"
#include "util.h"
#include "MahiExoII.h"

static bool        stop = false;                          /* flag used to stop the controller */
static const int   frequency = 1000;                  /* controller loop rate (Hz) */

/* Ctrl-C handler */
static void signal_handler(int signum) {
    stop = true;
}

int main(int argc, char * argv[]) {

    std::cout << "Have fun, Troy! - Craig" << std::endl;
    // register signal SIGINT and SIGBREAK with signal handler
    signal(SIGINT, signal_handler);
    signal(SIGBREAK, signal_handler);

    // instantiate Q8 USB for encoders, actuator controls, and EMG
    std::string id = "0";
    uint_vec  ai_channels = { 0, 1, 2, 3, 4, 5, 6, 7 };
    uint_vec  ao_channels = { 0, 1, 2, 3, 4 };
    uint_vec  di_channels = {};
    uint_vec  do_channels = { 1, 2, 3, 4, 5 };
    uint_vec enc_channels = { 0, 1, 2, 3, 4 };
    char options[] = "update_rate=fast;decimation=1";
    Daq *q8_0 = new Q8Usb(id, ai_channels, ao_channels, di_channels, do_channels, enc_channels, options);

    /*
    // instantiate Q8 USB for ATI and Futek force/torque sensors
    std::string id = "1";
    uint_vec  ai_channels = { 0, 1, 2, 3, 4, 5, 6, 7 };
    uint_vec  ao_channels = {};
    uint_vec  di_channels = {};
    uint_vec  do_channels = {};
    uint_vec enc_channels = {};
    char options[] = "update_rate=fast;decimation=1";
    Daq *q8_1 = new Q8Usb(id, ai_channels, ao_channels, di_channels, do_channels, enc_channels, options);
    */

    // instantiate MahiExoII
    
    MahiExoII exo = MahiExoII(q8_0,0,q8_0,1,
                              q8_0,0,q8_0,1,
                              q8_0,0,q8_0,1);
                              

    // initialize Q8 USB
    if (!q8_0->activate()) {
        std::cout << "Terminating controller" << std::endl;
        return -1;
    }

    // start watchdog
    q8_0->start_watchdog(0.1);

    // start time keeping variables
    std::chrono::high_resolution_clock::time_point time_start = std::chrono::high_resolution_clock::now();
    std::chrono::high_resolution_clock::time_point time_start_loop = time_start;
    std::chrono::high_resolution_clock::time_point time_now = time_start;
    std::chrono::nanoseconds time_elapsed_loop(0);
    std::chrono::nanoseconds time_elapsed_total(0);
    std::chrono::nanoseconds time_per_sample(1000000000 / frequency);
    std::chrono::nanoseconds time_per_sample_offset(200);
    double controller_time = 0;

    // start the control loop
    int sample_number = 0;
    while (stop == 0) {
        time_start_loop = std::chrono::high_resolution_clock::now();
        controller_time = (double)sample_number / frequency;

        // reloead watchdog
        q8_0->reload_watchdog();

        // START CONTROLLER SPECIFIC CODE 


        q8_0->read_all();

        //print_int_vec(q8_0->get_encoder_counts());
        std::cout << (180.0/PI)*exo.joint0_.get_position() << std::endl;

        q8_0->write_digital();


        // END CONTROLLER SPECIFIC CODE
        
        // log data 
        q8_0->log_data(controller_time);
        // increment sample number 
        sample_number += 1;
        // spinlock / busy wait the control loop until the loop rate has been reached 
        time_now = std::chrono::high_resolution_clock::now();
        time_elapsed_loop = time_now - time_start_loop;
        time_elapsed_total = time_now - time_start;
        //std::cout << time_elapsed_loop.count() / 1000000.0 << std::endl;
        while (time_elapsed_loop < time_per_sample - time_per_sample_offset) {
            time_now = std::chrono::high_resolution_clock::now();
            time_elapsed_loop = time_now - time_start_loop;
            time_elapsed_total = time_now - time_start;
        }
        time_now = std::chrono::high_resolution_clock::now();
        time_elapsed_loop = time_now - time_start_loop;
    }

    std::cout << "Ctrl-C pressed. Terminating control." << std::endl;

    // end control and clean up 
    q8_0->deactivate();

    return 0;
}
