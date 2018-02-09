#include <MEL/Daq/Quanser/Q8Usb.hpp>
#include <MEL/Utility/Options.hpp>
#include <MEL/Communications/Windows/MelShare.hpp>
#include <MEL/Utility/System.hpp>
#include <MEL/Utility/Console.hpp>
#include <MEL/Utility/Timer.hpp>
#include <MEL/Logging/Log.hpp>
#include <MEL/Utility/Windows/Keyboard.hpp>
#include <MEL/Math/TeagerKaiserEnergyOperator.hpp>
#include <MEL/Math/Filter.hpp>
#include <MEL/Math/Rectifier.hpp>
#include <MEL/Math/SignalProcessor.hpp>
#include <vector>
#include <sstream>

using namespace mel;

ctrl_bool stop(false);
int handler(unsigned long param) {
    stop = true;
    return 1;
}


int main(int argc, char *argv[]) {

    // handle inputs
    int N_emg; // number of EMG channels
    if (argc > 1) {
        std::stringstream ss(argv[1]);
        ss >> N_emg;
        if (N_emg < 1 || N_emg > 8) {
            return 0;
        }
    }
    else {
        return 0;
    }

    // register ctrl-c handler
    register_ctrl_handler(handler);

    // initialize logger
    init_logger();

    // make MelShares
    MelShare ms_raw_emg("ms_raw_emg");
    MelShare ms_hp_emg("ms_hp_emg");
    MelShare ms_rect_emg("ms_rect_emg");
    MelShare ms_env_emg("ms_env_emg");
    MelShare ms_tkeo_hp_emg("ms_tkeo_hp_emg");
    MelShare ms_tkeo_emg("ms_tkeo_emg");
    MelShare ms_tkeo_rect_emg("ms_tkeo_rect_emg");
    MelShare ms_tkeo_env_emg("ms_tkeo_env_emg");

    // enable Windows realtime
    enable_realtime();

    // make Q8 USB and configure
    Q8Usb q8;
    q8.digital_output.set_enable_values(std::vector<Logic>(8, High));
    q8.digital_output.set_disable_values(std::vector<Logic>(8, High));
    q8.digital_output.set_expire_values(std::vector<Logic>(8, High));

    // make EMG Signals
    std::vector<AnalogInput::Channel> emg_channels;
    for (size_t i = 0; i < N_emg; ++i) {
        emg_channels.push_back(q8.analog_input[i]);
    }

    // make myoelectric signal processors
    std::vector<Filter> hp_filter(N_emg, Filter({ 0.814254556886246, -3.257018227544984, 4.885527341317476, -3.257018227544984, 0.814254556886246 }, { 1.000000000000000, -3.589733887112175, 4.851275882519415, -2.924052656162457, 0.663010484385890 })); // 4th-order Butterworth High-Pass at 0.05 normalized cutoff frequency
    std::vector<Rectifier> rect(N_emg, Rectifier());
    std::vector<Filter> lp_filter(N_emg, Filter({ 0.058451424277128e-6, 0.233805697108513e-6, 0.350708545662770e-6, 0.233805697108513e-6, 0.058451424277128e-6 }, { 1.000000000000000, -3.917907865391990, 5.757076379118074, -3.760349507694534, 0.921181929191239 })); // 4th-order Butterworth Low-Pass at 0.01 normalized cutoff frequency
    std::vector<Filter> tkeo_hp_filter(N_emg, Filter({ 0.814254556886246, -3.257018227544984, 4.885527341317476, -3.257018227544984, 0.814254556886246 }, { 1.000000000000000, -3.589733887112175, 4.851275882519415, -2.924052656162457, 0.663010484385890 })); // 4th-order Butterworth High-Pass at 0.05 normalized cutoff frequency
    std::vector<TeagerKaiserEnergyOperator> tkeo(N_emg, TeagerKaiserEnergyOperator());
    std::vector<Rectifier> tkeo_rect(N_emg, Rectifier());
    std::vector<Filter> tkeo_lp_filter(N_emg, Filter({ 0.058451424277128e-6, 0.233805697108513e-6, 0.350708545662770e-6, 0.233805697108513e-6, 0.058451424277128e-6 }, { 1.000000000000000, -3.917907865391990, 5.757076379118074, -3.760349507694534, 0.921181929191239 })); // 4th-order Butterworth Low-Pass at 0.01 normalized cutoff frequency
    std::vector<SignalProcessor> mes_standard_processor;
    std::vector<SignalProcessor> mes_tkeo_processor;
    for (int i = 0; i < N_emg; ++i) {
        mes_standard_processor.push_back(SignalProcessor({ &hp_filter[i], &rect[i], &lp_filter[i] }));
        mes_tkeo_processor.push_back(SignalProcessor({ &tkeo_hp_filter[i], &tkeo[i], &tkeo_rect[i], &tkeo_lp_filter[i] }));
    }
    
    // create data containers
    std::vector<double> raw_emg(N_emg);
    std::vector<double> hp_emg(N_emg);
    std::vector<double> rect_emg(N_emg);
    std::vector<double> env_emg(N_emg);
    std::vector<double> tkeo_hp_emg(N_emg);
    std::vector<double> tkeo_emg(N_emg);
    std::vector<double> tkeo_rect_emg(N_emg);
    std::vector<double> tkeo_env_emg(N_emg);

    // write to MelShares
    ms_raw_emg.write_data(raw_emg);
    ms_hp_emg.write_data(hp_emg);
    ms_rect_emg.write_data(rect_emg);
    ms_env_emg.write_data(env_emg);
    ms_tkeo_hp_emg.write_data(tkeo_hp_emg);
    ms_tkeo_emg.write_data(tkeo_emg);
    ms_tkeo_rect_emg.write_data(tkeo_rect_emg);
    ms_tkeo_env_emg.write_data(tkeo_env_emg);

    // enable DAQ
    q8.enable();

    // construct timer in hybrid mode to avoid using 100% CPU
    Timer timer(milliseconds(1), Timer::Hybrid);

    // wait for user input
    print("Press Enter to continue.");
    Keyboard::wait_for_key(Key::Enter);

    // start loop
    q8.watchdog.start();
    while (!stop) {

        // update all DAQ input channels
        q8.update_input();

        // run signal processing on MES
        for (int i = 0; i < N_emg; ++i) {       

            // standard processing
            mes_standard_processor[i].process(emg_channels[i].get_value());
            raw_emg[i] = mes_standard_processor[i].get_signal(0);
            hp_emg[i] = mes_standard_processor[i].get_signal(1);
            rect_emg[i] = mes_standard_processor[i].get_signal(2);
            env_emg[i] = mes_standard_processor[i].get_signal(3);

            // tkeo processing
            mes_tkeo_processor[i].process(emg_channels[i].get_value());
            tkeo_hp_emg[i] = mes_tkeo_processor[i].get_signal(1);
            tkeo_emg[i] = mes_tkeo_processor[i].get_signal(2);
            tkeo_rect_emg[i] = mes_tkeo_processor[i].get_signal(3);
            tkeo_env_emg[i] = mes_tkeo_processor[i].get_signal(4);

        }

        // write to MelShares
        ms_raw_emg.write_data(raw_emg);
        ms_hp_emg.write_data(hp_emg);
        ms_rect_emg.write_data(rect_emg);
        ms_env_emg.write_data(env_emg);
        ms_tkeo_hp_emg.write_data(tkeo_hp_emg);
        ms_tkeo_emg.write_data(tkeo_emg);
        ms_tkeo_rect_emg.write_data(tkeo_rect_emg);
        ms_tkeo_env_emg.write_data(tkeo_env_emg);

        // kick watchdog
        if (!q8.watchdog.kick())
            stop = true;

        // wait for remainder of sample period
        timer.wait();

    }

    disable_realtime();
    return 0;
}

