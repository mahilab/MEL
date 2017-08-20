#pragma once
#include "StateMachine.h"
#include "Q8Usb.h"
#include "MahiExoIIEmg.h"
#include "MelShare.h"
#include "Filter.h"
#include "Clock.h"
#include "util.h"
#include "mahiexoii_util.h"
#include "GuiFlag.h"
#include <boost/circular_buffer.hpp>

class EmgRTControlData : public mel::EventData {

public:

};

class EmgRTControl : public mel::StateMachine {

public:

    //---------------------------------------------------------------------
    // CONSTRUCTOR(S) / DESTRUCTOR(S)
    //---------------------------------------------------------------------

    EmgRTControl(mel::Clock& clock, mel::Daq* q8_emg, MahiExoIIEmg& meii, GuiFlag& gui_flag, int input_mode);

private:

    //-------------------------------------------------------------------------
    // STATE MACHINE SETUP
    //-------------------------------------------------------------------------

    // STATES
    enum States {
        ST_INIT,
        ST_TO_CENTER,
        ST_HOLD_CENTER,
        ST_PRESENT_TARGET,
        ST_PROCESS_EMG,
        ST_TRAIN_CLASSIFIER,
        ST_FINISH,
        ST_STOP,
        ST_NUM_STATES
    };

    // STATE FUNCTIONS
    void sf_init(const mel::NoEventData*);
    mel::StateAction<EmgRTControl, mel::NoEventData, &EmgRTControl::sf_init> sa_init;

    void sf_to_center(const mel::NoEventData*);
    mel::StateAction<EmgRTControl, mel::NoEventData, &EmgRTControl::sf_to_center> sa_to_center;

    void sf_hold_center(const mel::NoEventData*);
    mel::StateAction<EmgRTControl, mel::NoEventData, &EmgRTControl::sf_hold_center> sa_hold_center;

    void sf_present_target(const mel::NoEventData*);
    mel::StateAction<EmgRTControl, mel::NoEventData, &EmgRTControl::sf_present_target> sa_present_target;

    void sf_process_emg(const mel::NoEventData*);
    mel::StateAction<EmgRTControl, mel::NoEventData, &EmgRTControl::sf_process_emg> sa_process_emg;

    void sf_train_classifier(const mel::NoEventData*);
    mel::StateAction<EmgRTControl, mel::NoEventData, &EmgRTControl::sf_train_classifier> sa_train_classifier;

    void sf_finish(const mel::NoEventData*);
    mel::StateAction<EmgRTControl, mel::NoEventData, &EmgRTControl::sf_finish> sa_finish;

    void sf_stop(const mel::NoEventData*);
    mel::StateAction<EmgRTControl, mel::NoEventData, &EmgRTControl::sf_stop> sa_stop;

    // STATE MAP
    virtual const mel::StateMapRow* get_state_map() {
        static const mel::StateMapRow STATE_MAP[] = {
            &sa_init,
            &sa_to_center,
            &sa_hold_center,
            &sa_present_target,
            &sa_process_emg,
            &sa_train_classifier,
            &sa_finish,
            &sa_stop,
        };
        return &STATE_MAP[0];
    }

    // USER INPUT CONTROL
    int INPUT_MODE_;
    void wait_for_input();
    bool check_stop();
    bool stop_ = false;

    //-------------------------------------------------------------------------
    // EXPERIMENT SETUP
    //-------------------------------------------------------------------------

    int current_target_ = 0;
    std::vector<int> target_sequence_ = { 1,2,1,2,1,2 };
    mel::char_vec target_check_joint_ = { 1,1,1,1,1 };
    mel::double_vec target_tol_ = { 1.0 * mel::DEG2RAD, 1.0 * mel::DEG2RAD, 1.0 * mel::DEG2RAD, 1.0 * mel::DEG2RAD, 0.01 };
    double hold_center_time_ = 2.0; // time to hold at center target [s]
    double force_mag_goal_ = 1000; // [N^2]
    double force_mag_tol_ = 100; // [N]
    double force_mag_dwell_time_ = 1.0; // [s]
    double force_mag_maintained_ = 0; // [s]
    double force_mag_time_now_ = 0;
    double force_mag_time_last_ = 0;

    //-------------------------------------------------------------------------
    // EXPERIMENT COMPONENTS
    //-------------------------------------------------------------------------

    // GUI FLAGS
    GuiFlag& gui_flag_;

    // HARDWARE CLOCK
    mel::Clock clock_;

    // HARDWARE
    mel::Daq* q8_emg_;
    mel::Daq* q8_ati_;
    MahiExoIIEmg meii_;

    // MEII PARAMETERS
    mel::int8_vec backdrive_ = { 1,1,1,1,1 };

    // MEII POSITION CONTROL
    mel::double_vec kp_ = { 35, 7, 25, 30, 0 };
    mel::double_vec kd_ = { 0.25, 0.06, 0.05, 0.08, 0 };
    mel::double_vec init_pos_ = mel::double_vec(5, 0);
    mel::double_vec goal_pos_ = mel::double_vec(5, 0);
    double init_time_ = 0.0;
    mel::double_vec speed_ = { 0.25, 0.25, 0.125, 0.125, 0.0125 };
    mel::double_vec x_ref_ = mel::double_vec(5, 0);
    mel::double_vec set_points_ = mel::double_vec(5, 0);
    mel::double_vec new_torques_ = mel::double_vec(5, 0);
    mel::share::MelShare pos_data_ = mel::share::MelShare("MEII_pos");
    mel::double_vec data_p_ = mel::double_vec(5, 0);

    // FORCE SENSING
    mel::double_vec wrist_forces_ = mel::double_vec(6, 0);
    mel::double_vec wrist_forces_filt_ = mel::double_vec(6, 0);
    mel::Filter multi_lpf_ = mel::Filter(6, 4, { 0.009735570656078, -0.032135367809242, 0.045449986329302, -0.032135367809242, 0.009735570656078 }, { 1.000000000000000, -3.572942808701423, 4.807914652718555, -2.886325158284144, 0.652003706289986 });

    // EMG SENSING
    static const int num_emg_channels_ = 8;
    mel::double_vec emg_voltages_ = mel::double_vec(num_emg_channels_, 0);
    struct EmgDataBuffer {
        EmgDataBuffer(size_t num_channels, size_t length) :
            num_channels_(num_channels),
            length_(length)
        {
            for (size_t i = 0; i < num_channels_; ++i) {
                data_buffer_.push_back(boost::circular_buffer<double>(length_,0.0));
            }
        }
        void push_back(mel::double_vec data_vec) {
            if (data_vec.size() != num_channels_) {
                mel::print("ERROR: Incorrect number of rows when calling EmgDataBuffer::push_back().");
            }
            for (int i = 0; i < num_channels_; ++i) {
                data_buffer_[i].push_back(data_vec[i]);
            }
        }
        mel::double_vec at(int index) {
            mel::double_vec data_vec;
            for (int i = 0; i < num_channels_; ++i) {
                data_vec.push_back(data_buffer_[i][index]);
            }
            return data_vec;
        }
        mel::double_vec get_channel(int index) {
            mel::double_vec channel_vec;
            for (int i = 0; i < length_; ++i) {
                channel_vec.push_back(data_buffer_[index][i]);
            }
            return channel_vec;
        }
        size_t num_channels_;
        size_t length_;
        std::vector<boost::circular_buffer<double>> data_buffer_;
    };
    EmgDataBuffer emg_data_buffer_ = EmgDataBuffer(num_emg_channels_, 200);

    // EMG FEATURE EXTRACTION
    static const int num_features_ = 9;
    mel::double_vec feature_vec_ = mel::double_vec(num_features_*num_emg_channels_, 0);
    std::array<double, num_features_*num_emg_channels_> feature_array_;
    mel::double_vec feature_extract(EmgDataBuffer& emg_data_buffer);
    double rms_feature_extract(boost::circular_buffer<double> emg_channel_buffer);
    double mav_feature_extract(boost::circular_buffer<double> emg_channel_buffer);
    double wl_feature_extract(boost::circular_buffer<double> emg_channel_buffer);
    double zc_feature_extract(boost::circular_buffer<double> emg_channel_buffer);
    double ssc_feature_extract(boost::circular_buffer<double> emg_channel_buffer);
    void ar4_feature_extract(mel::double_vec& coeffs, const mel::double_vec& emg_channel_buffer);

    // EMG TRAINING DATA
    std::vector<std::array<double,num_features_*num_emg_channels_>> emg_training_data_;
    int N_train_ = 6;
    mel::share::MelShare trng_share_ = mel::share::MelShare("trng_share_");

    // STATE TRANSITION EVENTS
    bool target_reached_ = false;
    bool hold_center_time_reached_ = false;
    bool force_mag_reached_ = false;
    bool emg_data_processed_ = false;
    bool end_of_target_sequence_ = false;
    bool check_target_reached(mel::double_vec goal_pos, mel::double_vec current_pos, mel::char_vec check_joint, bool print_output = false);
    bool check_wait_time_reached(double wait_time, double init_time, double current_time);
    bool check_force_mag_reached(double force_mag_goal, double force_mag);

    // UNITY INPUT/OUTPUT
    int scene_num_share = 0;
    mel::share::MelShare scene_num_ = mel::share::MelShare("scene_num");
    int target_share_ = 0;
    mel::share::MelShare target_ = mel::share::MelShare("target");
    double force_share_ = 0;
    mel::share::MelShare force_mag_ = mel::share::MelShare("force_mag");

    // MELSCOPE VARIABLES
    mel::share::MelShare pos_share_ = mel::share::MelShare("pos_share");
    mel::share::MelShare vel_share_ = mel::share::MelShare("vel_share");
    mel::share::MelShare emg_share_ = mel::share::MelShare("emg_share");
    

};