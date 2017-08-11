#include "EmgTraining.h"


EmgTraining::EmgTraining(mel::Clock& clock, mel::Daq* q8_emg, mel::Daq* q8_ati, MahiExoII* exo) :
    StateMachine(4),
    q8_emg_(q8_emg),
    q8_ati_(q8_ati),
    exo_(exo),
    clock_(clock),
    raw_force_(Eigen::VectorXd::Zero(6)),
    calib_mat_(Eigen::MatrixXd::Zero(6,6)),
    calib_force_(Eigen::VectorXd::Zero(6)),
    set_points_({ 0,0,0,0,0 }),
    kp_({ 35, 7, 25, 30, 3500 }),
    kd_({ 0.25, 0.06, 0.05, 0.08, 5 }),
    init_pos_({ 0,0,0,0,0 }),
    goal_pos_({ 0,0,0,0,0 }),
    init_time_(0),
    speed_({ 0.25, 0.25, 0.125, 0.125, 0.0125}),
    x_ref_({ 0,0,0,0,0 }),
    new_torques_({ 0,0,0,0,0 }),
    backdrive_({ 0,0,0,0,0 }),
    data_p_({ 0, 0, 0, 0, 0 }),
    lpf_(mel::Filter(4, { 0.009735570656078, -0.032135367809242, 0.045449986329302, -0.032135367809242, 0.009735570656078 }, { 1.000000000000000, -3.572942808701423, 4.807914652718555, -2.886325158284144, 0.652003706289986 })),
    multi_lpf_(mel::Filter(6, 4, { 0.009735570656078, -0.032135367809242, 0.045449986329302, -0.032135367809242, 0.009735570656078 }, { 1.000000000000000, -3.572942808701423, 4.807914652718555, -2.886325158284144, 0.652003706289986 })),
    force_share_({ 0, 0, 0, 0, 0, 0}),
    force_filt_({ 0, 0, 0, 0, 0, 0 })

{
    calib_mat_ << 0.45676, 0.37263, 2.84454, -95.32922, -1.60986, 93.56974, -10.00557, 107.83272, 2.80461, -54.50607, 2.67834, -55.04209, 133.67479, 5.88886, 131.55424, 5.44438, 134.55104, 5.69414, -0.02942, 0.74195, -2.11485, -0.48201, 2.19007, -0.27703, 2.49045, 0.12279, -1.26019, 0.59413, -1.30218, -0.70275, 0.07348, -1.36804, 0.08441, -1.41171, 0.05780, -1.37930;
}

void EmgTraining::sf_init(const mel::NoEventData* data) {


    std::cout << "Press ENTER to activate Daq <" << q8_emg_->name_ << ">" << std::endl;
    getchar();
    q8_emg_->activate();
    std::cout << "Press ENTER to activate Daq <" << q8_ati_->name_ << ">" << std::endl;
    getchar();
    q8_ati_->activate();
    //std::cout << "Press ENTER to enable MahiExoII" << std::endl;
    //getchar();
    //exo_->enable();
    std::cout << "Press Enter to start the controller" << std::endl;
    getchar();
    q8_emg_->start_watchdog(0.1);
    std::cout << "Starting the controller ... " << std::endl;

    // get current position
    q8_emg_->read_all();
    exo_->update_kinematics();
    init_pos_ = exo_->get_anatomical_joint_positions();
    goal_pos_ = init_pos_;

    /*
    //write position data (UNITY DEBUG)
    data_p_ = exo_->get_anatomical_joint_positions();
    pos_data_.write(data_p_);
    mel::print(data_p_);
    */

    // transition to next state
    event(ST_HOLD_NEUTRAL);

    // entry into next state
    goal_pos_[0] = -0.6; // elbow neutral
    goal_pos_[1] = 0; // forearm neutral
    goal_pos_[2] = 0; // wrist f/e neutral
    goal_pos_[3] = 0; // wrist r/u neutral
    goal_pos_[4] = 0.09; // arm translation neutral

}

void EmgTraining::sf_to_neutral(const mel::NoEventData* data) {
    
    // reset and start the hardware clock
    clock_.reset();
    clock_.start();


    // enter the control loop
    while (true) {

        q8_emg_->reload_watchdog();

        q8_emg_->read_all();


        exo_->update_kinematics();

        // set torques
        init_time_ = 0;
        for (auto i = 0; i < 5; ++i) {

            x_ref_[i] = moving_set_point(init_pos_[i], goal_pos_[i], init_time_, clock_.time(), speed_[i]);

            new_torques_[i] = mel::pd_controller(kp_[i], kd_[i], x_ref_[i], exo_->get_anatomical_joint_position(i), 0, exo_->get_anatomical_joint_velocity(i));

            if (backdrive_[i] == 1) {
                new_torques_[i] = 0;
            }

        }
        /*
        data_p_ = exo_->get_anatomical_joint_positions();
        pos_data_.write(data_p_);
        mel::print(data_p_);*/

        //mel::print(exo_->get_anatomical_joint_position(0));
        //mel::print(new_torques_[2]);
        exo_->set_anatomical_joint_torques(new_torques_);
        q8_emg_->write_all();
    }

}

void EmgTraining::sf_hold_neutral(const mel::NoEventData* data) {


    // reset and start the hardware clock
    clock_.reset();
    clock_.start();


    // enter the control loop
    while (true) {


        q8_emg_->reload_watchdog();

        q8_emg_->read_all();
        q8_ati_->read_all();

        std::vector<double> v = q8_ati_->get_analog_voltages();
        double* ptr = &v[0];
        Eigen::Map<Eigen::VectorXd> raw_force(ptr, 6);

        raw_force_ = raw_force;

        calib_force_ = calib_mat_*raw_force_;
        force_share_.resize(calib_force_.size());
        Eigen::VectorXd::Map(&force_share_[0], calib_force_.size()) = calib_force_;

        force_filt_ = multi_lpf_.filter(force_share_);

        ati_data_.write(force_filt_);
    }

}

void EmgTraining::sf_stop(const mel::NoEventData* data) {
    std::cout << "State Stop " << clock_.time() << std::endl;
    
}

void EmgTraining::ctrl_c_task() {
    std::cout << "Program aborted" << std::endl;
    q8_emg_->deactivate();
    q8_ati_->deactivate();
}