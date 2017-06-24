#include <iostream>
#include "Controller.h"
#include "ControlLoop.h"
#include "util.h"

class MyController : public Controller {
    int count = 0;
    void start() {
        std::cout << "Starting MyController" << std::endl;
    }
    void step() {
        std::cout << count++ << std::endl;
    }
    void stop() {
        std::cout << "Stopping MyController" << std::endl;
    }
    void pause() {
        std::cout << "Pausing MyController" << std::endl;
    }
    void resume() {
        std::cout << "Resuming MyController" << std::endl;
    }
};

int main(int argc, char * argv[]) {

    // create controller and control loop
    Controller* my_controller = new MyController();
    uint frequency = 1;
    ControlLoop loop(frequency);

    // request users permission to execute the controller
    std::cout << "Press ENTER to execute the controller. CTRL+C will stop the controller once it's started." << std::endl;
    getchar();

    // execute the controller
    loop.execute(my_controller);

    // delete controller
    delete my_controller;

    return 0;
}