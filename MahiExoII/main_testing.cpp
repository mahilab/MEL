#include <iostream>
#include <csignal>
#include "Q8Usb.h"
#include "util.h"
#include "MahiExoII.h"
#include "Controller.h"
#include "Task.h"
#include <boost/program_options.hpp>
#include "Exo.h"
#include "MyStateMachine.h"

int main(int argc, char * argv[]) {


    MyStateMachine my_sm;
    my_sm.Start();
  
    return 0;
}