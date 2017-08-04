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
    //std::cout << "Press ENTER to enable MahiExoII" << std::endl;
    //getchar();
    //exo_->enable();
    std::cout << "Press Enter to start the controller" << std::endl;
    getchar();
    daq_->start_watchdog(0.1);
    std::cout << "Starting the controller ... " << std::endl;


    event(ST_NEUTRAL);
}

void EmgTraining::ST_Neutral(const mel::NoEventData* data) {
    


    
}

void EmgTraining::ST_Stop(const mel::NoEventData* data) {
    std::cout << "Stop " << clock_.time() << std::endl;
    //std::cout << get_current_state() << std::endl;
}