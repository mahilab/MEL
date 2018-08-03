#include <MEL/Daq/Quanser/Q8Usb.hpp>
#include <MEL/Utility/Console.hpp>
#include <MEL/Core/Amplifier.hpp>
#include <MEL/Core/Motor.hpp>
#include <MEL/Core/Joint.hpp>
#include <MEL/Core/Robot.hpp>

using namespace mel;

int main(int argc, char* argv[]) {
    Q8Usb daq;                            // instantiate Quanser Q8-USB DAQ
    daq.enable();                         // enable DAQ
    daq.update_input();                   // sync DAQ inputs with real-world
    double voltage = daq.AI.get_value(0); // get voltage of analog input channel 0
    daq.AO[0].set_value(voltage);         // set voltage of analog output channel 0
    daq.update_output();                  // sync DAQ outputs with real-world


    DigitalOutput::Channel do0 = daq.DO[0]; // get digital output channel 0
    AnalogOutput::Channel ao0 = daq.AO[0];  // get analog output channel 0
    Encoder::Channel enc0 = daq.encoder[0]; // get encode channel 0
    do0.set_value(High);                    // set DO0 to High TTL
    ao0.set_value(5.0);                     // set AO0 to 5V
    do0.update();                           // sync DO0
    ao0.update();                           // sync AO0
    enc0.update();                          // sync encoder 0
    int32 counts = enc0.get_value();        // get encoder counts

    Amplifier amp("amc_12a8", High, do0, 1.3, ao0);            // create PWM amplifier
    Motor motor("maxon_re30", 0.0538, amp);                    // create DC motor
    PositionSensor* pos_sensor = &enc0;                        // create position sensor from encoder
    VelocitySensor* vel_sensor = &daq.velocity[0];             // create velocity sensor from DAQ encoder velocity channel
    Joint joint("axis0", &motor, pos_sensor, vel_sensor, 20);  // create a robotic joint
    Robot robot("simple_robot");                               // create a robot
    robot.add_joint(joint);                                    // add joint to robot

    
    return 0;
}
