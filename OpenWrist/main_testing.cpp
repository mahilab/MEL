#include <iostream>
#include "Task.h"
#include "Controller.h"
#include "Clock.h"
#include "Q8Usb.h"
#include "Encoder.h"
#include "OpenWrist.h"
#include <functional>
#include "MelShare.h"

class PdController : public mel::Task {

public:
    PdController(OpenWrist open_wrist, mel::Daq* daq) : Task("pd_controller"), open_wrist_(open_wrist), daq_(daq) {}

    OpenWrist open_wrist_;
    mel::Daq* daq_;
    mel::double_vec state = mel::double_vec(10, 0);

    mel::MelShare map_ = mel::MelShare("ow_state");

    void start() override {
       
        std::cout << "Press ENTER to activate Daq <" << daq_->name_ << ">" << std::endl;
        getchar();
        //daq_->activate();
        //daq_->zero_encoders();
        std::cout << "Press ENTER to enable OpenWrist" << std::endl;
        getchar();
        open_wrist_.enable();
        std::cout << "Press Enter to start the controller" << std::endl;
        getchar();
        //daq_->start_watchdog(0.1);
        std::cout << "Starting the controller ... " << std::endl;
    }

    void step() override {

        //daq_->read_all();
        //daq_->reload_watchdog();

        double traj0 = mel::sin_trajectory(80 * mel::DEG2RAD, 0.25, time());
        double traj1 = mel::sin_trajectory(60 * mel::DEG2RAD, 0.25, time());
        double traj2 = mel::sin_trajectory(30 * mel::DEG2RAD, 0.25, time());

        double torque0 = mel::pd_control_effort(25, 1.15, traj0, open_wrist_.joints_[0]->get_position(), 0, open_wrist_.joints_[0]->get_velocity());
        double torque1 = mel::pd_control_effort(25, 1.15, traj1, open_wrist_.joints_[1]->get_position(), 0, open_wrist_.joints_[1]->get_velocity());
        double torque2 = mel::pd_control_effort(25, 1.15, traj2, open_wrist_.joints_[2]->get_position(), 0, open_wrist_.joints_[2]->get_velocity());

        open_wrist_.joints_[0]->set_torque(torque0);
        open_wrist_.joints_[1]->set_torque(torque1);
        open_wrist_.joints_[2]->set_torque(torque2);

        state[0] = time();
        state[1] = traj0;// open_wrist_.joints_[0]->get_position();
        state[2] = traj1;// open_wrist_.joints_[1]->get_position();
        state[3] = traj2;// open_wrist_.joints_[2]->get_position();
        state[4] = open_wrist_.joints_[0]->get_velocity();
        state[5] = open_wrist_.joints_[1]->get_velocity();
        state[6] = open_wrist_.joints_[2]->get_velocity();

        //mel::MelShare::write_map("ow_state", state);
        map_.write(state);

        //daq_->write_all();

    }

    void stop() override {
        open_wrist_.disable();
        daq_->deactivate();
    }

};


int main(int argc, char * argv[]) {  
    
    /*
    // set Windows thread priority
    // https://msdn.microsoft.com/en-us/library/windows/desktop/ms685100(v=vs.85).aspx
    HANDLE hThread = GetCurrentThread();
    SetPriorityClass(hThread, HIGH_PRIORITY_CLASS); // use REALTIME_PRIORITY_CLASS with extreme care!
    SetThreadPriority(hThread, THREAD_PRIORITY_TIME_CRITICAL);
    */

    /*
    //  create a Q8Usb object
    mel::uint32 id = 0;

    mel::channel_vec  ai_channels = { 0, 1, 2 };
    mel::channel_vec  ao_channels = { 0, 1, 2 };
    mel::channel_vec  di_channels = { 0, 1, 2 };
    mel::channel_vec  do_channels = { 0, 1, 2 };
    mel::channel_vec enc_channels = { 0, 1, 2 };

    mel::Q8Usb::Options options;
    options.update_rate_ = mel::Q8Usb::Options::UpdateRate::Fast_8kHz;
    options.decimation_ = 1;
    options.ao_modes_[0] = mel::Q8Usb::Options::AoMode(mel::Q8Usb::Options::AoMode::CurrentMode1, 0, -1.382, 8.030, 0, -1, 0, 1000);
    options.ao_modes_[1] = mel::Q8Usb::Options::AoMode(mel::Q8Usb::Options::AoMode::CurrentMode1, 0, -1.382, 8.030, 0, -1, 0, 1000);
    options.ao_modes_[2] = mel::Q8Usb::Options::AoMode(mel::Q8Usb::Options::AoMode::CurrentMode1, 0,  1.912, 18.43, 0, -1, 0, 1000);
    mel::Daq* q8 = new mel::Q8Usb(id, ai_channels, ao_channels, di_channels, do_channels, enc_channels, options);

    // create and configure an OpenWrist object
    OpenWrist::Config config;
    for (int i = 0; i < 3; i++) {
        config.enable_[i] = q8->do_(i);
        config.command_[i] = q8->ao_(i);
        config.encoder_[i] = q8->encoder_(i);
        config.encrate_[i] = q8->encrate_(i);
        config.amp_gains_[i] = 1;
    }    

    // mel::Exo* open_wrist = new OpenWrist(config);
    OpenWrist open_wrist(config);

    // make a new Clock and Controller
    mel::Clock clock(1000, true); // 1000 Hz, clock logging enabled
    mel::Controller controller(clock);
    
    // queue Tasks for the Controller to execute
    controller.queue_task(new PdController(open_wrist, q8));

    // execute the controller
    controller.execute(); 
    */   

    // create shared memory maps (optionally define size in bytes, default = 256 bytes)
    // note that on Windows, the shared memory maps will only stay active for as long as
    // these instances stay in scope, so create them inside of your main loop or inside of a
    // class isntance that you expect to stay alive for the duration of the program (e.g. OpenWrist or ME-II).
    mel::MelShare map0("map0");
    mel::MelShare map1("map1");
    mel::MelShare map2("map2", 80); // 10 doubles * 8 bytes/double

    // create new data containers to write to map (vectos and C-style arrays can be used)
    mel::char_vec  my_chars = { 'a','b','c' };
    mel::int32_vec my_ints = { 1,1,2,3,5 };
    double my_doubles[10] = { 0,0.1,0.2,0.3,0.4,0.5,0.6,0.7,0.8,0.9 };

    std::cout << "Press ENTER to write values.";
    getchar();   

    // write the data to the maps
    map0.write(my_chars); // non-static version called using dot operator on a MelShare instance (fastest method)
    mel::MelShare::write_map("map1",my_ints); // alternately, static version that can be called anywhere if you know the map name (slightly slower)
    map2.write(my_doubles, 10); // C-style arrays can be used with both non-static and static versions, but you must input size manually
    
    std::cout << "Wrote: " << std::endl;
    std::cout << "map0:    ";  mel::print_vector(my_chars);
    std::cout << "map1:    ";  mel::print_vector(my_ints);
    std::cout << "map2:    ";  mel::print_array(my_doubles, 10);

    std::cout << "Run Python or C# code, then press ENTER to receive new values.";
    getchar();

    // Data in a map can change type and size, so long as the data stored does not exceed the initial bytes
    // size. The only caveat is that corresponding writer-reader arguement types must match, e.g. if you
    // last wrote a double type vector/array to a map, you must read the map back into a double type vector/array
    // for any meaningful interpretation.

    my_ints.resize(7); // here we expect the size of my_ints to increase by two (see Python or C#)

    // read the altered data from the maps
    map0.read(my_chars);
    map1.read(my_doubles, 10); // this demonstrates that we can read doubles from a map that previously wrote ints
    mel::MelShare::read_map("map2", my_ints); // and vice-versa

    std::cout << "Read: " << std::endl;
    std::cout << "map0:    ";  mel::print_vector(my_chars);
    std::cout << "map1:    ";  mel::print_array(my_doubles, 10);
    std::cout << "map2:    ";  mel::print_vector(my_ints);

    std::cout << "Press ENTER to exit and free the shared memory.";
    getchar();
    
    return 0;
}


