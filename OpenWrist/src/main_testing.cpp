#include <iostream>
#include "Controller.h"
#include "ControlLoop.h"

// Controller implementation minimum working example
class MyController : public mel::Controller {
    void start() override {
        std::cout << "Starting MyController" << std::endl;
    }
    void step() override {
        std::cout << get_time() << std::endl;
    }
    void stop() override {
        std::cout << "Stopping MyController" << std::endl;
    }
    void pause() override {
        std::cout << "Pausing MyController" << std::endl;
    }
    void resume() override {
        std::cout << "Resuming MyController" << std::endl;
    }
};

int main(int argc, char * argv[]) {
    
    // create an
    mel::Controller* my_controller = new MyController()
    mel::Clock my_clock(100);
    mel::ControlLoop my_loop(my_clock);

    std::vector<double> evan(2,1);
    evan[0] = 1;

    // request users permission to execute the controller
    std::cout << "Press ENTER to execute the controller. CTRL+C will stop the controller once it's started." << std::endl;
    getchar();

    // queue controllers
    my_loop.queue_controller(my_controller);

    // execute the controller
    my_loop.execute();

    // delete controller
    delete my_controller;

    return 0;
}