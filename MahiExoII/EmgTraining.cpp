#include "EmgTraining.h"


EmgTraining::EmgTraining(mel::Clock& clock, mel::Daq* q8_emg, mel::Daq* q8_ati, MahiExoII* exo) :
    StateMachine(clock, 3),
    q8_emg_(q8_emg),
    q8_ati_(q8_ati),
    exo_(exo),
    raw_force_(Eigen::VectorXd::Zero(6)),
    calib_mat_(Eigen::MatrixXd::Zero(6,6)),
    calib_force_(Eigen::VectorXd::Zero(6))
{
    calib_mat_ << 0.45676, 0.37263, 2.84454, -95.32922, -1.60986, 93.56974, -10.00557, 107.83272, 2.80461, -54.50607, 2.67834, -55.04209, 133.67479, 5.88886, 131.55424, 5.44438, 134.55104, 5.69414, -0.02942, 0.74195, -2.11485, -0.48201, 2.19007, -0.27703, 2.49045, 0.12279, -1.26019, 0.59413, -1.30218, -0.70275, 0.07348, -1.36804, 0.08441, -1.41171, 0.05780, -1.37930;
}

void EmgTraining::ST_Init(const mel::NoEventData* data) {


    std::cout << "Press ENTER to activate Daq <" << q8_emg_->name_ << ">" << std::endl;
    getchar();
    q8_emg_->activate();
    std::cout << "Press ENTER to activate Daq <" << q8_ati_->name_ << ">" << std::endl;
    getchar();
    q8_ati_->activate();
    std::cout << "Press ENTER to enable MahiExoII" << std::endl;
    getchar();
    exo_->enable();
    std::cout << "Press Enter to start the controller" << std::endl;
    getchar();
    q8_emg_->start_watchdog(0.1);
    std::cout << "Starting the controller ... " << std::endl;
    event(ST_NEUTRAL);

}

void EmgTraining::ST_Neutral(const mel::NoEventData* data) {
    
    q8_emg_->reload_watchdog();

    q8_ati_->read_analogs();

    std::vector<double> v = q8_ati_->get_analog_voltages();
    double* ptr = &v[0];
    Eigen::Map<Eigen::VectorXd> raw_force(ptr, 6);

    raw_force_ = raw_force;

    calib_force_ = calib_mat_*raw_force_;

    mel::print(calib_force_.transpose());

}

void EmgTraining::ST_Stop(const mel::NoEventData* data) {
    std::cout << "State Stop " << clock_.time() << std::endl;
    
}

void EmgTraining::ctrl_c_task() {
    std::cout << "You pressed Ctrl+C" << std::endl;
    q8_emg_->deactivate();
    q8_ati_->deactivate();
}