#include <iostream>
#include "Controller.h"
#include "ControlLoop.h"
#include "util.h"

class MyController : public mel::Controller {
    int count = 0;
    void start() override {
        std::cout << "Starting MyController" << std::endl;
    }
    void step() override {
        std::cout << count++ << std::endl;
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

    // create controller and control loop
    mel::Controller* my_controller = new MyController();
    mel::uint frequency = 1;
    mel::ControlLoop loop(frequency);

    // request users permission to execute the controller
    std::cout << "Press ENTER to execute the controller. CTRL+C will stop the controller once it's started." << std::endl;
    getchar();

    // execute the controller
    loop.execute(my_controller);

    // delete controller
    delete my_controller;

    return 0;
}