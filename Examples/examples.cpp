#include <iostream>
#include <boost/program_options.hpp>
#include "mel_util.h"
#include "MelShare.h"
#include "Clock.h"
#include "Integrator.h"
#include "ExternalApp.h"
#include "Input.h"
#include <cmath>
#include <random>
#include "mel_math.h"
#include "Q8Usb.h"
#include "OpenWrist.h"
#include "PerformanceMonitor.h"
#include <iostream>

// This is the MEL Examples program. It is divided in sections by comment headers.
// With the exception of PROGRAM OPTIONS, each section is self contained and 
// does not require any variables from other sections. To run a particular secition,
// use the appropriate program option argument when running the exe from the 
// command line.

using namespace mel; // this isn't necessary, but will keep the amount of typing down

int main(int argc, char * argv[]) {

    //-------------------------------------------------------------------------
    // PROGRAM OPTIONS:    >Examples.exe --help
    //-------------------------------------------------------------------------

    // use boost set up your program options 
    // this allows us to pass arguments through the command line like
    boost::program_options::options_description desc("Available Options");
    desc.add_options()
        ("help", "produces help message")
        ("melshare-data", "example demonstrating how to use MELShare for two way data sharing with C# or Python")
        ("melshare-msg", "example demonstrating how to send and recieve string messages over MELShare")
        ("melscope", "another MELShare demo that produces test data for also introducing in MELScope")
        ("external", "example of how to launch an external app or game from C++")
        ("q8", "example demonstrating how to set up a Q8 USB and becnhmark it's read/write speed")
        ("open-wrist", "example demonstrating how to control an OpenWrist in MEL")
        ("clock","tests clock wait function performance on your PC")
        ("log", "example demonstrating use of DataLog class")
        ("io", "example demonstrating use of print functions and Input class");

    boost::program_options::variables_map var_map;
    boost::program_options::store(boost::program_options::command_line_parser(argc, argv).options(desc).allow_unregistered().run(), var_map);
    boost::program_options::notify(var_map);

    // running "Examples.exe --help" at the command line will produce a nicely
    // formatted list of program options
    if (var_map.count("help")) {
        util::print(desc);
        return 0;
    }

    //-------------------------------------------------------------------------
    // MELSHARE DATA EXAMPLE:    >Examples.exe --melshare
    //-------------------------------------------------------------------------

    if (var_map.count("melshare-data")) {

        // create shared memory maps (optionally define size in bytes, default = 256 bytes)
        // note that on Windows, the shared memory maps will only stay active for as long as
        // these instances stay in scope, so create them inside of your main loop or inside of a
        // class isntance that you expect to stay alive for the duration of the program (e.g. OpenWrist or ME-II).
        comm::MelShare map0("map0");
        comm::MelShare map1("map1");
        comm::MelShare map2("map2", 80); // 10 doubles * 8 bytes/double

        // create new data containers to write to map (vectors, STL arrays, and C-style arrays can be used)
        std::vector<char> my_chars = { 'a','b','c' }; // STL vector
        std::array<int, 5> my_ints = { 1,1,2,3,5 };    // STL array
        double my_doubles[10] = { 0,0.1,0.2,0.3,0.4,0.5,0.6,0.7,0.8,0.9 }; // C-style array

        std::cout << "Press ENTER to write values.";
        getchar();

        // write the data to the maps
        map0.write(my_chars); // non-static version called using dot operator on a MelShare instance (fastest method)
        comm::write_map("map1", my_ints); // alternately, static version that can be called anywhere if you know the map name (slightly slower)
        map2.write(my_doubles, 10); // C-style arrays can be used with both non-static and static versions, but you must input size manually

        util::print("Wrote: ");
        std::cout << "map0:    ";  util::print(my_chars);
        std::cout << "map1:    ";  util::print(my_ints);
        std::cout << "map2:    ";  util::print(my_doubles, 10, true);

        std::cout << "Run Python or C# code, then press ENTER to receive new values.";
        getchar();

        // Data in a map can change type and size, so long as the data stored does not exceed the initial bytes
        // size. The only caveat is that corresponding writer-reader arguement types must match, e.g. if you
        // last wrote a double type vector/array to a map, you must read the map back into a double type vector/array
        // for any meaningful interpretation.

        // Here we expect the size of my_ints to increase by two (see Python or C#). If you require this sort of behavior, 
        // consider using a single vector instead of the multiple arrays like this example shows.
        std::array<int, 7> my_ints_bigger{ 0,1,0,0,0,0,0 };

        // read the altered data from the maps
        map0.read(my_chars);
        map1.read(my_doubles, 10); // this demonstrates that we can read doubles from a map that previously wrote ints
        comm::read_map("map2", my_ints_bigger); // and vice-versa

        util::print("Read: ");
        std::cout << "map0:    ";  util::print(my_chars);
        std::cout << "map1:    ";  util::print(my_doubles, 10, true);
        std::cout << "map2:    ";  util::print(my_ints_bigger);

        return 0;
    }

    //-------------------------------------------------------------------------
    // MELSHARE MESSAGING EXAMPLE:    >Examples-exe --melshare-msg
    //-------------------------------------------------------------------------

    if (var_map.count("melshare-msg")) {
        // create a MELShare
        comm::MelShare messenger("messenger");
        messenger.write_message("this is a message being sent to unity");
        getchar();
        util::print(messenger.read_message());
    }

    //-------------------------------------------------------------------------
    // MELSCOPE EXAMPLE:    >Examples.exe --melscope
    //-------------------------------------------------------------------------

    if (var_map.count("melscope")) {

        util::enable_realtime();

        // **Open example.scope in MELScope to see this example in action**

        // Create our MELShare maps. We will make one map, "integrals", to demonstate
        // MEL's Integrator class. We will also make three other maps, "cpp2py", 
        // "py2cpp_ampl", and "py2cpp_freq" to deomstate bidirectional communication in 
        // MELShare. While bidirectional communcation over a single MELShare map is possible 
        // with the read_write() functions, it can be tricky to get the timing right. 
        // It's best to just keep two maps open for this purpose.
        comm::MelShare integrals("integrals");
        comm::MelShare cpp2py("cpp2py");
        comm::MelShare py2cpp_ampl("py2cpp_ampl");
        comm::MelShare py2cpp_freq("py2cpp_freq");

        // create the data buffers for each map
        std::array<double, 4> integrals_data = { 0, 0, 0, 0 };
        std::array<double, 4> cpp2py_data = { 0, 0, 0, 0 };
        std::array<double, 4> py2cpp_ampl_data = { 10, 8, 6, 4 };
        std::array<double, 4> py2cpp_freq_data = { 0.4, 0.6, 0.8, 1.0 };

        // create a MEL Integrator for doing some integration. note the initial value 5
        math::Integrator integrator = math::Integrator(5, math::Integrator::Technique::Trapezoidal);

        // create a MEL Clock to control the rate of our data generating loop
        util::Clock clock(1000); // 1000 Hz clock

        // In this example, we will have the data in cpp2py be a funcion of the data
        // from py2cpp. Specifically, we will generate some periodic data, and use
        // scalers from py2cpp to change the amplitudes and frequencies. For that 
        // reason, we will start off by actually writing py2cpp so that it has some 
        // intial values. 
        py2cpp_ampl.write(py2cpp_ampl_data);
        py2cpp_freq.write(py2cpp_freq_data);

        // start the clock
        clock.start();

        // run the data loop for 60 seconds
        while (clock.time() < 60) {

            // generate new data for integrals
            integrals_data[0] = clock.time();                                          //     t
            integrals_data[1] = 0.5 * (cos(clock.time()) + 7 * cos(7 * clock.time())); // dx/dt = 1/2 * [ cos(t) + 7 * cos(7t) ] 
            integrals_data[2] = sin(4 * clock.time()) * cos(3 * clock.time()) + 5;     //     x = sin(4t) * cos(3t) + 5
            integrals_data[3] = integrator.integrate(integrals_data[1], clock.time()); //     x ~ integrate(dx/dt)

            // write integrals
            integrals.write(integrals_data);
            
            // read data from py2cpp_ampl and py2cpp_freq
            py2cpp_ampl.read(py2cpp_ampl_data);
            py2cpp_freq.read(py2cpp_freq_data);

            // generate new data for cpp2py
            cpp2py_data[0] = math::sin_wave(py2cpp_ampl_data[0], py2cpp_freq_data[0], clock.time());
            cpp2py_data[1] = math::square_wave(py2cpp_ampl_data[1], py2cpp_freq_data[1], clock.time());;
            cpp2py_data[2] = math::triangle_wave(py2cpp_ampl_data[2], py2cpp_freq_data[2], clock.time());
            cpp2py_data[3] = math::sawtooth_wave(py2cpp_ampl_data[3], py2cpp_freq_data[3], clock.time());

            // write cpp2py
            cpp2py.write(cpp2py_data);

            // wait the clock
            clock.hybrid_wait();
        }
    }

    //-------------------------------------------------------------------------
    // EXTERNAL APPLICATION EXAMPLE:    >Examples.exe --external
    //-------------------------------------------------------------------------

    if (var_map.count("external")) {
        util::ExternalApp my_app("my_python_shell", "C:\\dev\\Python27\\python.exe");
        my_app.launch();
    }

    //-------------------------------------------------------------------------
    // Q8 USB EXAMPLE:    >Examples.exe --q8
    //-------------------------------------------------------------------------

    if (var_map.count("q8")) {

        uint32 id = 0;

        channel_vec  ai_channels = { 0, 1, 2, 3, 4, 5, 6, 7 };
        channel_vec  ao_channels = { 0, 1, 2, 3, 4, 5, 6, 7 };
        channel_vec  di_channels = { 0, 1, 2, 3, 4, 5, 6, 7 };
        channel_vec  do_channels = { 0, 1, 2, 3, 4, 5, 6, 7 };
        channel_vec enc_channels = { 0, 1, 2, 3, 4, 5, 6, 7 };

        core::Daq* q8 = new dev::Q8Usb(id, ai_channels, ao_channels, di_channels, do_channels, enc_channels);

        util::print("Press ENTER to enable this Q8 USB", false);
        getchar();
        q8->enable();

        util::print("Press ENTER to benchmark this Q8 USB", false);
        getchar();
        q8->benchmark(1000000);

        q8->disable();
        delete q8;
    }

    //-------------------------------------------------------------------------
    // OPENWRIST EXAMPLE:    >Examples.exe --open-wrist
    //-------------------------------------------------------------------------
    
    if (var_map.count("open-wrist")) {

        // tell compiler to ignore CTRL-C signals from console
        // (we will handle them ourself in responsible way)
        util::Input::ignore_ctrl_c();

        // enable soft realtime
        util::enable_realtime();

        // create Q8Usb for OpenWrist
        uint32 id = 0;
        channel_vec  ai_channels = { 0, 1, 2 };
        channel_vec  ao_channels = { 0, 1, 2 };
        channel_vec  di_channels = { 0, 1, 2 };
        channel_vec  do_channels = { 0, 1, 2 };
        channel_vec enc_channels = { 0, 1, 2 };

        // configure the Q8 to run in CurrentMode for us with VoltPAQ-X4
        dev::Q8Usb::Options options_q8;
        options_q8.update_rate_ = dev::Q8Usb::Options::UpdateRate::Fast_8kHz;
        options_q8.decimation_ = 1;
        options_q8.ao_modes_[0] = dev::Q8Usb::Options::AoMode(dev::Q8Usb::Options::AoMode::CurrentMode1, 0, +2, 20, 0, -1, 0, 1000);
        options_q8.ao_modes_[1] = dev::Q8Usb::Options::AoMode(dev::Q8Usb::Options::AoMode::CurrentMode1, 0, +2, 20, 0, -1, 0, 1000);
        options_q8.ao_modes_[2] = dev::Q8Usb::Options::AoMode(dev::Q8Usb::Options::AoMode::CurrentMode1, 0, +2, 20, 0, -1, 0, 1000);

        // initialize Q8 object as DAQ pointer (polymorphism)
        core::Daq* q8 = new dev::Q8Usb(id, ai_channels, ao_channels, di_channels, do_channels, enc_channels, options_q8);

        // create OpenWrist object
        exo::OpenWrist::Config ow_config;
        for (int i = 0; i < 3; i++) {
            ow_config.enable_[i] = q8->do_(i);
            ow_config.command_[i] = q8->ao_(i);
            ow_config.sense_[i] = q8->ai_(i);
            ow_config.encoder_[i] = q8->encoder_(i);
            ow_config.encrate_[i] = q8->encrate_(i);
            ow_config.amp_gains_[i] = 1;
        }
        exo::OpenWrist ow(ow_config);

        // create a 1000 Hz Clock to run our controller on
        util::Clock clock(1000);

        // create some PD controllers that fill like light springs
        core::PdController pd0(2, 0.0125); // joint 0 ( Nm/rad , Nm-s/rad )
        core::PdController pd1(1, 0.0125); // joint 1 ( Nm/rad , Nm-s/rad )
        core::PdController pd2(1, 0.0125);  // joint 2 ( Nm/rad , Nm-s/rad )

        // request user input to begin
        util::Input::acknowledge("Press ENTER to start the controller.", util::Input::Return);

        // enable hardware
        q8->enable();
        q8->start_watchdog(0.1);
        ow.enable();

        // start the control loop
        clock.start();
        while (true) {

            // read and reload Q8
            q8->read_all();
            q8->reload_watchdog();

            // do something controlsy
            ow.joints_[0]->set_torque(pd0.calculate(0, ow.joints_[0]->get_position(), 0, ow.joints_[0]->get_velocity()));
            ow.joints_[1]->set_torque(pd1.calculate(0, ow.joints_[1]->get_position(), 0, ow.joints_[1]->get_velocity()));
            ow.joints_[2]->set_torque(pd2.calculate(0, ow.joints_[2]->get_position(), 0, ow.joints_[2]->get_velocity()));

            ow.joints_[0]->add_torque(ow.compute_gravity_compensation(0));
            ow.joints_[1]->add_torque(ow.compute_gravity_compensation(1));

            // update the OpenWrist's internal MELShare map so we can use MELScope
            ow.update_state_map();

            // check joint limits and react if necessary
            if (ow.check_all_joint_limits())
                break;

            // check for user request to stop
            if (util::Input::is_key_pressed(util::Input::LControl) && util::Input::is_key_pressed(util::Input::C))
                break;

            // write Q8
            q8->write_all();

            // wait for the next clock tick
            clock.hybrid_wait();
        }

        // disable hardware and cleanup
        ow.disable();
        q8->disable();
        util::disable_realtime();
        delete q8;
    }   

    //-------------------------------------------------------------------------
    // CLOCK BENCHMARK:    >Examples.exe --clock
    //-------------------------------------------------------------------------

    if (var_map.count("clock")) {


        uint32 seconds   = 10;
        uint32 frequency = 1000;

        double mean, stddev;

        util::PerformanceMonitor pm;

        util::Input::acknowledge("Press ENTER to start the benchmark.", util::Input::Return);

        util::print(pm.cpu_used_process());

        util::Clock clock(frequency);
        util::enable_realtime();
        
        util::print("Benchmarking Clock for " + std::to_string(static_cast<int>(seconds)) + " second(s) at " + std::to_string(frequency) + " Hz.");

        // accurate wait (default)
        util::print("ACCURATE WAIT:    ", false);
        clock.start();
        while (clock.time() < (double)seconds) {
            // fake busy code
            util::Clock::wait_for(0.0001);
            clock.wait();
            clock.log();
        }
        mean = math::mean(clock.log_.get_col("Tick [s]")) * 1000.0;
        stddev = math::stddev_p(clock.log_.get_col("Tick [s]")) * 1000.0;
        util::print("Elapsed: " + std::to_string(clock.log_.get_row(clock.log_.get_row_count() - 1)[2]) + " s    ", false);
        util::print("Avg. Step: " + std::to_string(mean) + " +/- " + std::to_string(stddev) + " ms    CPU: " + std::to_string(pm.cpu_used_process()) + "  %");

        // efficient wait
        util::print("EFFICIENT WAIT:   ", false);
        clock.start();
        while (clock.time() < (double)seconds) {
            // fake busy code
            util::Clock::wait_for(0.0001);
            clock.efficient_wait();
            clock.log();
        }
        mean = math::mean(clock.log_.get_col("Tick [s]")) * 1000.0;
        stddev = math::stddev_p(clock.log_.get_col("Tick [s]")) * 1000.0;
        util::print("Elapsed: " + std::to_string(clock.log_.get_row(clock.log_.get_row_count() - 1)[2]) + " s    ", false);
        util::print("Avg. Step: " + std::to_string(mean) + " +/- " + std::to_string(stddev) + " ms    CPU: " + std::to_string(pm.cpu_used_process()) + "  %");

        // efficient wait
        util::print("HYBRID WAIT:      ", false);
        clock.start();
        while (clock.time() < (double)seconds) {
            // fake busy code
            util::Clock::wait_for(0.0001);
            clock.hybrid_wait();
            clock.log();
        }
        mean =   math::mean(clock.log_.get_col("Tick [s]")) * 1000.0;
        stddev = math::stddev_p(clock.log_.get_col("Tick [s]")) * 1000.0;
        util::print("Elapsed: " + std::to_string(clock.log_.get_row(clock.log_.get_row_count() - 1)[2]) + " s    ", false);
        util::print("Avg. Step: " + std::to_string(mean) + " +/- " + std::to_string(stddev) + " ms    CPU: " + std::to_string(pm.cpu_used_process()) + " %");

        util::disable_realtime();
    }

    //-------------------------------------------------------------------------
    // DATALOG EXAMPLE:    >Examples.exe --data
    //-------------------------------------------------------------------------

    if (var_map.count("log")) {

        util::DataLog log;

        log.add_col("A").add_col("B").add_col("C").add_col("D").add_col("E");
        
        log.add_row(math::linspace(1, 5, 5));
        log.add_row(math::linspace(6, 10, 5));
        log.add_row(math::linspace(11, 15, 5));

        util::print("Row 1: ", false); util::print(math::mean(log.get_row(1)));
        util::print("Col A: ", false); util::print(math::stddev_p(log.get_col("A")));

        log.save_data("my_log", "my_logs", true);     

    }

    //-------------------------------------------------------------------------
    // IO EXAMPLE:    >Examples.exe --io
    //-------------------------------------------------------------------------

    if (var_map.count("io")) {

        util::Input::ignore_ctrl_c(); // ignore CTRL+C signals from the command window

        std::vector<int> door_options = { 1,2,3 };
        std::vector<util::Input::Key> key_options = { util::Input::Num1, util::Input::Num2, util::Input::Num3 };

        util::print("Welcome to Let's Make a Deal! I am your host Monty Hall."); // prints a string with a new line
        util::print("Which door is the car behind? Your options are: ", false);  // prints a string without a new line
        util::print(door_options); // prints a vector

        util::Input::Key key_pressed = util::Input::wait_for_keys(key_options); // wait for valid keys, requires console focus (default)

        switch (key_pressed) {
        case util::Input::Num1:
            util::print("Sorry, you pick the goat."); 
            break;
        case util::Input::Num2:
            util::print("Congratulations! You just won a brand new car!");
            break;
        case util::Input::Num3:
            util::print("Uh-oh, looks like your picked a goat!");
            break;
        }

        int i;
        std::cin >> i;

        util::print("you entered" + std::to_string(i));

        util::usleep(1000000);
    }

}


