#include <MEL/Devices/Myo/MyoBand.hpp>  
#include <MEL/Utility/System.hpp>
#include <MEL/Logging/Log.hpp>
#include <MEL/Utility/Console.hpp>
#include <MEL/Communications/Windows/MelShare.hpp>
#include <MEL/Utility/Windows/Keyboard.hpp>
#include <MEL/Core/Clock.hpp>
#include <MEL/Core/Timer.hpp>

using namespace mel;


int main() {

    // handle inputs 
    std::vector<uint32> emg_channel_numbers = { 0,1,2,3,4,5,6,7 };

    MyoBand myo("my_myo");

    // initialize logger
    init_logger();

	// make MelShares
	MelShare ms_mes("mes"); // myoelectric signal

    // initialize testing conditions
    Time Ts = milliseconds(1); // sample period

    // construct timer in hybrid mode to avoid using 100% CPU
    Timer timer(Ts, Timer::Hybrid);
	bool stop = false;

	// prompt the user for input
	print("Open MelScope to view Myo signals.");
	print("Press 'Escape' to exit.");

    // enable hardware
    myo.enable();

    while (!stop) {

        // update hardware
        myo.update();

        // write to MelShares
        ms_mes.write_data(myo.get_values());

        // check for exit key
        if (Keyboard::is_key_pressed(Key::Escape)) {
            stop = true;
        }

        // wait for remainder of sample period
        timer.wait();

    } // end control loop

    return 0;
}

