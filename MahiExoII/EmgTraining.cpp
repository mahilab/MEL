#include "EmgTraining.h"


EmgTraining::EmgTraining(mel::Clock& clock, mel::Daq* daq, MahiExoII* exo) :
    StateMachine(clock, 3),
    daq_(daq),
    exo_(exo)
{

}

void EmgTraining::ST_Init(const mel::NoEventData* data) {


    std::cout << "Press ENTER to activate Daq <" << daq_->name_ << ">" << std::endl;
    getchar();
    daq_->activate();
    std::cout << "Press ENTER to enable MahiExoII" << std::endl;
    getchar();
    exo_->enable();
    std::cout << "Press Enter to start the controller" << std::endl;
    getchar();
    daq_->start_watchdog(0.1);
    std::cout << "Starting the controller ... " << std::endl;

    event(ST_NEUTRAL);

}

void EmgTraining::ST_Neutral(const mel::NoEventData* data) {
    
    std::cout << "State Neutral" << clock_.time() << std::endl;
    daq_->reload_watchdog();

}

void EmgTraining::ST_Stop(const mel::NoEventData* data) {
    std::cout << "State Stop " << clock_.time() << std::endl;
    
}

void EmgTraining::ctrl_c_task() {
    std::cout << "You pressed Ctrl+C" << std::endl;
    daq_->deactivate();
}