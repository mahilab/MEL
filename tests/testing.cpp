#include <MEL/Utility/Console.hpp>
#include <MEL/Core/Motor.hpp>
#include <MEL/Communications/Windows/MelShare.hpp>
#include <MEL/Logging/Log.hpp>
#include <MEL/Core/Amplifier.hpp>
#include <MEL/Daq/Quanser/Q8Usb.hpp>
#include <MEL/Utility/Timer.hpp>
#include <MEL/Math/Constants.hpp>
#include <MEL/Core/Robot.hpp>
#include <MEL/Math/Waveform.hpp>
#include <MEL/Core/PdController.hpp>

using namespace mel;

// create global stop variable CTRL-C handler function
ctrl_bool stop(false);
bool handler(CtrlEvent event) {
    stop = true;
    return true;
}

int main(int argc, char* argv[]) {





    return 0;
}
