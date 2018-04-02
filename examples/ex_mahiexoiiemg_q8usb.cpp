#include <MEL/Daq/Quanser/Q8Usb.hpp>
#include <MEL/Exoskeletons/MahiExoII/MahiExoIIEmg.hpp>
#include <MEL/Communications/Windows/MelShare.hpp>
#include <MEL/Utility/Options.hpp>
#include <MEL/Utility/Timer.hpp>
#include <MEL/Utility/Clock.hpp>
#include <MEL/Math/Functions.hpp>
#include <MEL/Logging/DataLogger.hpp>
#include <MEL/Utility/Console.hpp>
#include <MEL/Utility/System.hpp>
#include <MEL/Utility/Windows/Keyboard.hpp>
#include <MEL/Logging/Log.hpp>
#include <MEL/Utility/RingBuffer.hpp>
#include <MEL/Utility/Windows/ExternalApp.hpp>
#include <algorithm>
#include <limits>


using namespace mel;

// create global stop variable CTRL-C handler function
ctrl_bool stop(false);
bool handler(CtrlEvent event) {
    stop = true;
    return true;
}

//class RealTimeClassifier {
//
//public:
//
//    /// Constructor
//    RealTimeClassifier(Time sample_period, Time classification_period, Time feature_period = Time::Zero, Time classification_overlap = microseconds(std::numeric_limits<int64>::max()));
//
//    /// Update called every sample period, taking in new input sample. Must have already done training.
//    void update(std::vector<double> sample);
//
//    /// Provide training data for binary classification. Must be done before predictions can begin. 
//    void train(const std::vector<std::vector<double>>& class_0_data, const std::vector<std::vector<double>>& class_1_data);
//
//    /// Provide the linear classification model without training data.
//    void set_classifier(const std::vector<double>& w, double w_0);
//
//    /// Get latest prediction of the class label since calling update.
//    int get_class() const;
//
//private:
//
//    /// Convert signal bin into single feature for classification ( a kind of downsampling )
//    double feature_extraction(const std::vector<double>& signal_bin);
//
//    /// Method of converting all predictions in a classification window into a single prediction.
//    int classification_heuristic(const std::vector<double>& probabilities);
//
//
//private:
//
//    Time Ts_; ///< sample period    
//    std::size_t classification_window_size_; ///< number of samples on which to perform classification
//    uint32 feature_window_size_; ///< number of samples on which to extract features
//
//    std::size_t pred_counter_; ///< keeps track of relative spacing of predictions
//    std::size_t pred_spacing_; ///< controls relative spacing of predictions
//
//    RingBuffer<double> classification_buffer_; ///< contains most resent posterior probabilities 
//    std::vector<RingBuffer<double>> feature_buffer_; ///< contains most recent samples to be converted to single feature for classification
//    std::size_t sample_dim_; ///< size of sample vector
//
//    std::vector<double> w_; ///< classification weighting coefficients
//    double w_0_; ///< classification intercept
//    std::vector<double> x_; ///< classificatio input
//    double y_1_; ///< classification output
//    double p_1_; ///< class 1 posterior probability
//    int c_; ///< class prediction
//
//    bool trained_; ///< whether or not a classifier has been trained
//};
//
//RealTimeClassifier::RealTimeClassifier(Time sample_period, Time classification_period, Time feature_period, Time classification_overlap) :
//    Ts_(sample_period),
//    classification_window_size_(classification_period.as_seconds() / sample_period.as_seconds()),
//    feature_window_size_(feature_period.as_seconds() / sample_period.as_seconds()),
//    pred_counter_(0),
//    pred_spacing_(classification_overlap > classification_period ? 1 : (classification_period.as_seconds() - classification_overlap.as_seconds()) / sample_period.as_seconds()),
//    classification_buffer_(classification_window_size_),
//    trained_(false)
//{ }
//
//void RealTimeClassifier::update(std::vector<double> sample) {
//    if (!trained_)
//        return;
//    for (std::size_t i = 0; i < sample_dim_; ++i) {
//        feature_buffer_[i].push_back(sample[i]);        
//    }
//
//    if (std::any_of(feature_buffer_.begin(), feature_buffer_.end(), [](RingBuffer<double> rb) {return !rb.full(); }))
//        return;
//    y_1_ = w_0_;
//    for (std::size_t i = 0; i < sample_dim_; ++i) {
//        x_[i] = feature_extraction(feature_buffer_[i].get_vector());
//        y_1_ += w_[i] * x_[i];
//    }
//    p_1_ = sigmoid(y_1_);
//    
//    classification_buffer_.push_back(p_1_);    
//    if (++pred_counter_ == classification_window_size_)
//        pred_counter_ = 0;
//    if (classification_buffer_.full() && (pred_counter_ % pred_spacing_ == 0)) {
//        c_ = classification_heuristic(downsample(classification_buffer_.get_vector(), feature_window_size_));
//    }
//}
//
//void RealTimeClassifier::train(const std::vector<std::vector<double>>& class_0_data, const std::vector<std::vector<double>>& class_1_data) {
//    sample_dim_ = class_0_data.size();
//    if (class_1_data.size() != sample_dim_) {
//        LOG(Error) << "Training data must be of the same size for RealTimeClassifier.";
//        return;
//    }
//    std::vector<std::vector<double>> binned_data;
//    std::vector<std::vector<double>> class_0_feature_data(sample_dim_);
//    std::vector<std::vector<double>> class_1_feature_data(sample_dim_);
//    for (std::size_t i = 0; i < sample_dim_; ++i) {
//        binned_data = bin_signal(class_0_data[i], feature_window_size_);
//        class_0_feature_data[i].resize(binned_data.size());
//        for (std::size_t j = 0; j < binned_data.size(); ++j) {            
//            class_0_feature_data[i][j] = feature_extraction(binned_data[j]);
//        }
//        binned_data = bin_signal(class_1_data[i], feature_window_size_);
//        class_1_feature_data[i].resize(binned_data.size());
//        for (std::size_t j = 0; j < binned_data.size(); ++j) {
//            
//            class_1_feature_data[i][j] = feature_extraction(binned_data[j]);
//        }
//    }
//    
//    bin_linear_discriminant_model(class_0_feature_data, class_1_feature_data, w_, w_0_);
//    trained_ = true;
//    feature_buffer_ = std::vector<RingBuffer<double>>(sample_dim_, RingBuffer<double>(feature_window_size_)); 
//    x_.resize(sample_dim_);
//}
//
//void RealTimeClassifier::set_classifier(const std::vector<double>& w, double w_0) {
//    sample_dim_ = w.size();
//    w_ = w;
//    w_0_ = w_0;
//    trained_ = true;
//    feature_buffer_ = std::vector<RingBuffer<double>>(sample_dim_, RingBuffer<double>(feature_window_size_));
//    x_.resize(sample_dim_);
//}
//
//int RealTimeClassifier::get_class() const {
//    return c_;
//}
//
//double RealTimeClassifier::feature_extraction(const std::vector<double>& signal_window) {
//    return mean(signal_window);
//}
//
//int RealTimeClassifier::classification_heuristic(const std::vector<double>& probabilities) {
//
//    // take the average of the probabilities, then classify
//    //double p_mean = 0;
//    //for (std::size_t i = 0; i < probabilities.size(); ++i) {
//    //    p_mean += probabilities[i];
//    //}
//    //p_mean = p_mean / (double)probabilities.size();
//    //if (p_mean > 0.5) {
//    //    return 1;
//    //}
//    //else {
//    //    return 0;
//    //}
//
//    // do classification on all, then majority vote
//    std::vector<double> votes(probabilities.size(), 0.0);
//    for (std::size_t i = 0; i < probabilities.size(); ++i) {
//        if (probabilities[i] > 0.5) {
//            votes[i] = 1.0;
//        }
//    }
//    if (sum(votes) > probabilities.size() / 2.0) {
//        return 1;
//    }
//    else {
//        return 0;
//    }
//}


int main(int argc, char *argv[]) {

    // make options
    Options options("ex_mahiexoiiemg_q8usb.exe", "MahiExoIIEmg Q8 USB Demo");
    options.add_options()
        ("c,calibrate", "Calibrates the MAHI Exo-II")
        ("s,scope", "Runs the MAHI Exo-II EMG MelScope demo")
        ("t,trigger","Runs the MAHI Exo-II EMG teleoperation demo in trigger control mode")
        ("d,direct", "Runs the MAHI Exo-II EMG teleoperation demo in direct control mode")
        ("h,help", "Prints this help message");

    auto result = options.parse(argc, argv);

    if (result.count("help") > 0) {
        print(options.help());
        return 0;
    }

    // enable Windows realtime
    enable_realtime();

    // register ctrl-c handler
    register_ctrl_handler(handler);

    // initialize default MEL logger
    init_logger();

    // make MelShares
    MelShare ms_pos("melscope_pos");
    MelShare ms_vel("melscope_vel");
    MelShare ms_trq("melscope_trq");
    MelShare ms_mes_raw("melscope_mes_raw");
    MelShare ms_mes_demean("melscope_mes_demean");
    MelShare ms_mes_env("melscope_mes_env");
    MelShare ms_mes_tkeo_env("melscope_mes_tkeo_env");
    MelShare ms_active_prob("melscope_active_prob");
    MelShare ms_pred_state("melscope_pred_state");  

    // make and launch MelScope
    ExternalApp melscope = ExternalApp("melscope", "C:\\Program Files\\MELScope\\MELScope.exe");
    melscope.launch();

    // make Q8 USB and configure    
    Q8Usb q8(QOptions(), true, true, { 3 }); // specify all EMG channels
    q8.digital_output.set_enable_values(std::vector<Logic>(8, High));
    q8.digital_output.set_disable_values(std::vector<Logic>(8, High));
    q8.digital_output.set_expire_values(std::vector<Logic>(8, High));
    if (!q8.identify(7)) {
        LOG(Error) << "Incorrect DAQ";
        return 0;
    }
    std::vector<uint32> emg_channel_numbers = q8.analog_input.get_channel_numbers();
    std::size_t emg_channel_count = q8.analog_input.get_channel_count();

    // create MahiExoII and bind Q8 channels to it
    std::vector<Amplifier> amplifiers;
    std::vector<double> amp_gains;
    for (uint32 i = 0; i < 2; ++i) {
        amplifiers.push_back(
            Amplifier("meii_amp_" + std::to_string(i),
                Low,
                q8.digital_output[i + 1],
                1.8,
                q8.analog_output[i + 1])
        );
    }
    for (uint32 i = 2; i < 5; ++i) {
        amplifiers.push_back(
            Amplifier("meii_amp_" + std::to_string(i),
                Low,
                q8.digital_output[i + 1],
                0.184,
                q8.analog_output[i + 1])
        );
    }
   
    MeiiConfiguration config(q8, q8.watchdog, q8.encoder[{1, 2, 3, 4, 5}], q8.velocity[{1, 2, 3, 4, 5}], amplifiers, q8.analog_input[emg_channel_numbers]);
    MahiExoIIEmg meii(config);
    std::size_t mes_window_size = meii.get_mes_buffer_capacity();


    // create robot data log
    DataLogger robot_log(WriterType::Buffered, false);    
    std::vector<std::string> robot_log_header = { "Time [s]", "MEII EFE Position [rad]", "MEII EFE Velocity [rad/s]", "MEII EFE Commanded Torque [Nm]",
        "MEII FPS Position [rad]", "MEII FPS Velocity [rad/s]", "MEII FPS Commanded Torque [Nm]",
        "MEII RPS L1 Position [m]", "MEII RPS L1 Velocity [m/s]", "MEII RPS L1 Commanded Force [N]",
        "MEII RPS L2 Position [m]", "MEII RPS L2 Velocity [m/s]", "MEII RPS L2 Commanded Force [N]",
        "MEII RPS L3 Position [m]", "MEII RPS L3 Velocity [m/s]", "MEII RPS L3 Commanded Force [N]" };
    robot_log.set_header(robot_log_header);
    robot_log.set_record_format(DataFormat::Default, 12);
    std::vector<double> robot_log_row(robot_log_header.size());

    // create data log for EMG data
    DataLogger emg_log(WriterType::Buffered, false);
    std::vector<std::string> emg_log_header;
    emg_log_header.push_back("Time [s]");
    for (std::size_t i = 0; i < emg_channel_count; ++i) {
        emg_log_header.push_back("MES RAW " + stringify(emg_channel_numbers[i]));
    }
    for (std::size_t i = 0; i < emg_channel_count; ++i) {
        emg_log_header.push_back("MES DM " + stringify(emg_channel_numbers[i]));
    }
    for (std::size_t i = 0; i < emg_channel_count; ++i) {
        emg_log_header.push_back("MES ENV " + stringify(emg_channel_numbers[i]));
    }
    for (std::size_t i = 0; i < emg_channel_count; ++i) {
        emg_log_header.push_back("MES TKEO ENV " + stringify(emg_channel_numbers[i]));
    }
    emg_log.set_header(emg_log_header);
    std::vector<double> emg_log_row(emg_log_header.size());

    // create data log for EMG features
    DataLogger feat_log(WriterType::Buffered, false);
    std::vector<std::string> feat_log_header;
    feat_log_header.push_back("Time [s]");
    for (int i = 0; i < emg_channel_count * meii.get_all_features_count(); ++i) {
        feat_log_header.push_back("Feat. " + std::to_string(i));
    }
    feat_log.set_header(feat_log_header);
    std::vector<double> feat_log_row(feat_log_header.size());

    // calibrate - manually zero the encoders (right arm supinated)
    if (result.count("calibrate") > 0) {
        meii.calibrate(stop);
        return 0;
    }

    // display EMG activity on scope
    if (result.count("scope") > 0) {

        LOG(Info) << "Initializing MAHI Exo-II";

        // create initial setpoint
        std::vector<double> setpoint = { -35 * DEG2RAD, 0 * DEG2RAD, 0 * DEG2RAD, 0 * DEG2RAD,  0.10 };

        // set up state machine
        uint16 state = 0;
        Time backdrive_time = seconds(3);

        // create data containers
        std::vector<double> rj_positions(meii.N_rj_);
        std::vector<double> rj_velocities(meii.N_rj_);
        std::vector<double> aj_positions(meii.N_aj_);
        std::vector<double> aj_velocities(meii.N_aj_);
        std::vector<double> command_torques(meii.N_aj_);
        std::vector<double> rps_command_torques(meii.N_qs_);
        std::vector<double> mes_raw(emg_channel_count);
        std::vector<double> mes_demean(emg_channel_count);
        std::vector<double> mes_env(emg_channel_count);
        std::vector<double> mes_tkeo_env(emg_channel_count);

        // enable DAQ and exo
        q8.enable();
        meii.enable();

        // initialize controller
        meii.set_rps_control_mode(0);

        // construct timer in hybrid mode to avoid using 100% CPU
        Timer timer(milliseconds(1), Timer::Hybrid);

        // construct clock to regulate feature computation
        Clock feature_refract_clock;
        Time feature_refract_time = seconds(1);

        // start while loop
        q8.watchdog.start();
        while (!stop) {

            // update all DAQ input channels
            q8.update_input();

            // update MahiExoII kinematics
            meii.update_kinematics();

            // store most recent readings from DAQ
            for (int i = 0; i < meii.N_rj_; ++i) {
                rj_positions[i] = meii[i].get_position();
                rj_velocities[i] = meii[i].get_velocity();
            }
            for (int i = 0; i < meii.N_aj_; ++i) {
                aj_positions[i] = meii.get_anatomical_joint_position(i);
                aj_velocities[i] = meii.get_anatomical_joint_velocity(i);
            }

            switch (state) {
            case 0: // backdrive

                    // command zero torque
                meii.set_joint_torques(command_torques);

                // check for wait period to end
                if (timer.get_elapsed_time() >= backdrive_time) {
                    meii.rps_init_par_ref_.start(meii.get_wrist_parallel_positions(), timer.get_elapsed_time());
                    state = 1;
                }
                break;

            case 1: // initialize rps

                    // calculate commanded torques
                rps_command_torques = meii.set_rps_pos_ctrl_torques(meii.rps_init_par_ref_, timer.get_elapsed_time());
                std::copy(rps_command_torques.begin(), rps_command_torques.end(), command_torques.begin() + 2);

                // check for RPS Initialization target reached
                if (meii.check_rps_init()) {
                    meii.set_rps_control_mode(1); // platform height backdrivable
                    meii.anat_ref_.start(setpoint, meii.get_anatomical_joint_positions(), timer.get_elapsed_time());
                    state = 2;
                    LOG(Info) << "Recording EMG to MelScope and Data Logger. Press Enter to compute and record EMG features. Press Escape to exit program.";
                }
                break;

            case 2: // read emg

                // emg signal processing
                meii.update_emg();
                mes_raw = meii.get_mes_raw();
                mes_demean = meii.get_mes_dm();
                mes_env = meii.get_mes_env();
                mes_tkeo_env = meii.get_mes_tkeo_env();

                // write to emg data log
                emg_log_row[0] = timer.get_elapsed_time().as_seconds();
                auto it = std::copy(mes_raw.begin(), mes_raw.end(), emg_log_row.begin() + 1);
                it = std::copy(mes_demean.begin(), mes_demean.end(), it);
                it = std::copy(mes_env.begin(), mes_env.end(), it);
                std::copy(mes_tkeo_env.begin(), mes_tkeo_env.end(), it);
                emg_log.buffer(emg_log_row);

                // compute emg features and store to log
                if (Keyboard::is_key_pressed(Key::Enter)) {
                    if (meii.is_mes_buffer_full()) {
                        if (feature_refract_clock.get_elapsed_time() > feature_refract_time) {
                            meii.compute_all_features();
                            feat_log_row[0] = timer.get_elapsed_time().as_seconds();
                            std::copy(meii.get_all_features().begin(), meii.get_all_features().end(), feat_log_row.begin() + 1);
                            feat_log.buffer(feat_log_row);
                            LOG(Info) << "Features recorded from current EMG window.";
                            feature_refract_clock.restart();
                        }
                    }
                }

                // calculate commanded torques
                command_torques = meii.set_anat_pos_ctrl_torques(meii.anat_ref_, timer.get_elapsed_time());

                break;
            }

            // write to MelShares
            ms_pos.write_data(aj_positions);
            ms_vel.write_data(aj_velocities);
            ms_trq.write_data(command_torques);
            ms_mes_raw.write_data(mes_raw);
            ms_mes_demean.write_data(mes_demean);
            ms_mes_env.write_data(mes_env);
            ms_mes_tkeo_env.write_data(mes_tkeo_env);

            // update all DAQ output channels
            q8.update_output();

            // write to robot data log
            robot_log_row[0] = timer.get_elapsed_time().as_seconds();
            for (int i = 0; i < meii.N_rj_; ++i) {
                robot_log_row[3 * i + 1] = meii[i].get_position();
                robot_log_row[3 * i + 2] = meii[i].get_velocity();
                robot_log_row[3 * i + 3] = meii[i].get_torque();
            }
            robot_log.buffer(robot_log_row);

            // check for exit key
            if (Keyboard::is_key_pressed(Key::Escape)) {
                stop = true;
            }

            // kick watchdog
            if (!q8.watchdog.kick() || meii.any_limit_exceeded())
                stop = true;

            // wait for remainder of sample period
            timer.wait();

        } // end while loop       

        print("Do you want to save the robot data log? (Y/N)");
        Key key = Keyboard::wait_for_any_keys({ Key::Y, Key::N });
        if (key == Key::Y) {
            robot_log.save_data("example_meii_robot_data_log.csv", ".", false);
            robot_log.wait_for_save();
            robot_log.clear_data();
        }
        sleep(seconds(0.5));

        print("Do you want to save the EMG data log? (Y/N)");
        key = Keyboard::wait_for_any_keys({ Key::Y, Key::N });
        if (key == Key::Y) {
            emg_log.save_data("example_meii_emg_data_log.csv", ".", false);
            emg_log.wait_for_save();
            emg_log.clear_data();
        }
        sleep(seconds(0.5));

        print("Do you want to save the EMG feature data log? (Y/N)");
        key = Keyboard::wait_for_any_keys({ Key::Y, Key::N });
        if (key == Key::Y) {
            feat_log.save_data("example_meii_feat_data_log.csv", ".", false);
            feat_log.wait_for_save();
            feat_log.clear_data();
        }

    } // display EMG activity on scope


    // teleoperate the MAHI Exo-II in EMG triggered mode
    if (result.count("trigger") > 0) {

    //    // construct timer in hybrid mode to avoid using 100% CPU
    //    Time Ts = milliseconds(1); // sample period
    //    Timer timer(Ts, Timer::Hybrid);

    //    // methodological and control options - to be changed as desired
    //    int num_classes = 3; // number of active classes (up to 4)   
    //    std::vector<RealTimeClassifier> active_detectors(num_classes, RealTimeClassifier(Ts, milliseconds(20), milliseconds(5)));
    //    
    //    bool active_peak_finding = true; // when capturing new active data, uses a smaller mes_active_window_size-sample window centered at the max value 
    //    Time mes_rest_capture_period = seconds(1);
    //    Time mes_active_capture_period = seconds(3);
    //    Time mes_active_period = milliseconds(200);
    //    

    //    // initialize methodological and control variables - not to be changed        
    //    bool have_rest_data = false;
    //    std::vector<bool> have_active_data(num_classes,false);
    //    bool classifier_computed = false;
    //    std::vector<Key> active_keys = { Key::Num1, Key::Num2, Key::Num3, Key::Num4 };
    //    std::size_t mes_rest_capture_window_size = mes_rest_capture_period.as_seconds() / Ts.as_seconds();
    //    std::size_t mes_active_capture_window_size = mes_active_capture_period.as_seconds() / Ts.as_seconds();
    //    mes_window_size = std::max(mes_rest_capture_window_size, mes_active_capture_window_size);
    //    if (meii.get_mes_buffer_capacity() != mes_window_size) {
    //        meii.resize_mes_buffer(mes_window_size);
    //    }
    //    std::size_t mes_active_window_size = mes_active_period.as_seconds() / Ts.as_seconds();

    //    // initialize data containers
    //    std::vector<double> mes_raw(emg_channel_count);
    //    std::vector<double> mes_demean(emg_channel_count);
    //    std::vector<double> mes_env(emg_channel_count);
    //    std::vector<double> mes_tkeo_env(emg_channel_count);
    //    std::vector<std::vector<double>> mes_tkeo_env_new_data(emg_channel_count);
    //    std::vector<std::vector<double>> mes_tkeo_env_rest_data(emg_channel_count);
    //    std::vector<std::vector<std::vector<double>>> mes_tkeo_env_active_data(num_classes, std::vector<std::vector<double>>(emg_channel_count));
    //    std::vector<double> mes_tkeo_env_new_data_sum(mes_active_capture_window_size);
    //    std::size_t max_tkeo_env_idx;
    //    std::vector<int> pred_class(num_classes);
    //    int state = 0;

    //    // enable DAQ
    //    q8.enable();
    //    

    //    // construct clock to regulate interaction
    //    Clock keypress_refract_clock;
    //    Time keypress_refract_time = seconds(1);

    //    // start while loop
    //    q8.watchdog.start();

    //    while (!stop) {

    //        // update all DAQ input channels
    //        q8.update_input();

    //        // emg signal processing
    //        meii.update_emg();
    //        mes_raw = meii.get_mes_raw();
    //        mes_demean = meii.get_mes_dm();
    //        mes_env = meii.get_mes_env();
    //        mes_tkeo_env = meii.get_mes_tkeo_env();

    //        // write to emg data log
    //        emg_log_row[0] = timer.get_elapsed_time().as_seconds();
    //        auto it = std::copy(mes_raw.begin(), mes_raw.end(), emg_log_row.begin() + 1);
    //        it = std::copy(mes_demean.begin(), mes_demean.end(), it);
    //        it = std::copy(mes_env.begin(), mes_env.end(), it);
    //        std::copy(mes_tkeo_env.begin(), mes_tkeo_env.end(), it);
    //        emg_log.buffer(emg_log_row);

    //        // predict state
    //        for (std::size_t k = 0; k < num_classes; ++k) {
    //            active_detectors[k].update(meii.get_mes_tkeo_env());
    //            pred_class[k] = active_detectors[k].get_class();
    //        }
    //        if (std::any_of(pred_class.begin(), pred_class.end(), [](int c) {return c == 1; }))
    //            state = 1;
    //        else
    //            state = 0;
    //        
    //        
    //        // clear rest data
    //        if (Keyboard::are_all_keys_pressed({ Key::Space, Key::R })) {
    //            if (have_rest_data) {
    //                for (std::size_t i = 0; i < emg_channel_count; ++i) {
    //                    mes_tkeo_env_rest_data[i].clear();
    //                }
    //                have_rest_data = false;
    //                print("Cleared rest data.");
    //                keypress_refract_clock.restart();
    //            }
    //        }

    //        // capture rest data
    //        if (Keyboard::is_key_pressed(Key::R)) {
    //            if (meii.is_mes_buffer_full()) {
    //                if (keypress_refract_clock.get_elapsed_time() > keypress_refract_time) {
    //                    mes_tkeo_env_new_data = meii.get_mes_tkeo_env_buffer_data(mes_rest_capture_window_size);
    //                    for (std::size_t i = 0; i < emg_channel_count; ++i) {
    //                        mes_tkeo_env_rest_data[i].insert(mes_tkeo_env_rest_data[i].end(), mes_tkeo_env_new_data[i].begin(), mes_tkeo_env_new_data[i].end());
    //                    }
    //                    have_rest_data = true;
    //                    classifier_computed = false;
    //                    print("Collected rest data.");
    //                    keypress_refract_clock.restart();
    //                }
    //            }
    //        }

    //        // clear active data
    //        for (std::size_t k = 0; k < num_classes; ++k) {
    //            if (Keyboard::are_all_keys_pressed({ Key::Space, Key::A, active_keys[k] })) {
    //                if (have_active_data[k]) {
    //                    for (std::size_t i = 0; i < emg_channel_count; ++i) {
    //                        mes_tkeo_env_active_data[k][i].clear();
    //                    }
    //                    have_active_data[k] = false;
    //                    print("Cleared active data " + stringify(k) + ".");
    //                    keypress_refract_clock.restart();
    //                }
    //            }
    //        }

    //        // capture active data
    //        for (std::size_t k = 0; k < num_classes; ++k) {
    //            if (Keyboard::are_all_keys_pressed({ Key::A, active_keys[k] })) {
    //                if (meii.is_mes_buffer_full()) {
    //                    if (keypress_refract_clock.get_elapsed_time() > keypress_refract_time) {
    //                        mes_tkeo_env_new_data = meii.get_mes_tkeo_env_buffer_data(mes_active_capture_window_size);                            
    //                        if (active_peak_finding) {                                
    //                            for (std::size_t j = 0; j < mes_window_size; ++j) {
    //                                mes_tkeo_env_new_data_sum[j] = 0.0;
    //                                for (std::size_t i = 0; i < emg_channel_count; ++i) {
    //                                    mes_tkeo_env_new_data_sum[j] += mes_tkeo_env_new_data[i][j];
    //                                }                                  
    //                            }
    //                            max_tkeo_env_idx = std::distance(mes_tkeo_env_new_data_sum.begin(), std::max_element(mes_tkeo_env_new_data_sum.begin(), mes_tkeo_env_new_data_sum.end()));
    //                            if (max_tkeo_env_idx <  (mes_active_window_size / 2)) {
    //                                max_tkeo_env_idx = mes_active_window_size / 2;
    //                            }
    //                            if (mes_window_size - max_tkeo_env_idx < ((mes_active_window_size + 1) / 2)) {
    //                                max_tkeo_env_idx = mes_window_size - ((mes_active_window_size + 1) / 2);
    //                            }                              
    //                            for (std::size_t i = 0; i < emg_channel_count; ++i) {
    //                                mes_tkeo_env_new_data[i].erase(mes_tkeo_env_new_data[i].begin(), mes_tkeo_env_new_data[i].begin() + max_tkeo_env_idx - (mes_active_window_size / 2));
    //                                mes_tkeo_env_new_data[i].erase(mes_tkeo_env_new_data[i].begin() + mes_active_window_size, mes_tkeo_env_new_data[i].end());
    //                            }
    //                        }
    //                        for (std::size_t i = 0; i < emg_channel_count; ++i) {
    //                            mes_tkeo_env_active_data[k][i].insert(mes_tkeo_env_active_data[k][i].end(), mes_tkeo_env_new_data[i].begin(), mes_tkeo_env_new_data[i].end());
    //                        }
    //                        have_active_data[k] = true;
    //                        classifier_computed = false;
    //                        print("Collected active data " + stringify(k) + ".");
    //                        keypress_refract_clock.restart();
    //                    }
    //                }
    //            }
    //        }          

    //        // compute the active/rest classifiers
    //        if (Keyboard::is_key_pressed(Key::Enter)) {
    //            if (!classifier_computed) {
    //                if (have_rest_data && std::all_of(have_active_data.begin(), have_active_data.end(), [](bool b) { return b; })) {
    //                    if (keypress_refract_clock.get_elapsed_time() > keypress_refract_time) {
    //                        for (std::size_t k = 0; k < num_classes; ++k) {
    //                            active_detectors[k].train(mes_tkeo_env_rest_data, mes_tkeo_env_active_data[k]);
    //                        }
    //                        classifier_computed = true;
 
    //                        print("Computed new active/rest classifiers.");
    //                        keypress_refract_clock.restart();
    //                    }
    //                }
    //            }
    //        }           

    //        // write to MelShares
    //        ms_mes_raw.write_data(mes_raw);
    //        ms_mes_demean.write_data(mes_demean);
    //        ms_mes_env.write_data(mes_env);
    //        ms_mes_tkeo_env.write_data(mes_tkeo_env);
    //        ms_pred_state.write_data({ (double)state });

    //        // check for exit key
    //        if (Keyboard::is_key_pressed(Key::Escape)) {
    //            stop = true;
    //        }

    //        // kick watchdog
    //        if (!q8.watchdog.kick()) // REMOVED LIMIT CHECK WHILE ROBOT NOT BEING USED
    //            stop = true;

    //        // wait for remainder of sample period
    //        timer.wait();

    //    } // end while loop       

    //    print("Do you want to save the EMG data log? (Y/N)");
    //    Key key = Keyboard::wait_for_any_keys({ Key::Y, Key::N });
    //    if (key == Key::Y) {
    //        emg_log.save_data("example_meii_emg_data_log.csv", ".", false);
    //        emg_log.wait_for_save();
    //        emg_log.clear_data();
    //    }
    //    sleep(seconds(0.5));

    //    print("Do you want to save the EMG feature data log? (Y/N)");
    //    key = Keyboard::wait_for_any_keys({ Key::Y, Key::N });
    //    if (key == Key::Y) {
    //        feat_log.save_data("example_meii_feat_data_log.csv", ".", false);
    //        feat_log.wait_for_save();
    //        feat_log.clear_data();
    //    }
    } // teleoperate the MAHI Exo-II in EMG triggered

    disable_realtime();
    return 0;
}

