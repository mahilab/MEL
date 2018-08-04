#include <MEL/Daq/Quanser/Q8Usb.hpp>
#include <MEL/Utility/Console.hpp>
#include <MEL/Core/Amplifier.hpp>
#include <MEL/Core/Motor.hpp>
#include <MEL/Core/Joint.hpp>
#include <MEL/Core/Robot.hpp>
#include <MEL/Core/Timer.hpp>
#include <MEL/Core/PdController.hpp>
#include <MEL/Math/Waveform.hpp>

using namespace mel;

int main(int argc, char* argv[]) {
    Q8Usb daq;                            // instantiate Quanser Q8-USB DAQ
    daq.enable();                         // enable DAQ
    daq.update_input();                   // sync DAQ inputs with real-world
    double voltage = daq.AI.get_value(0); // get voltage of analog input channel 0
    daq.AO[0].set_value(voltage);         // set voltage of analog output channel 0
    daq.update_output();                  // sync DAQ outputs with real-world


    DigitalOutput::Channel do0 = daq.DO[0];   // get digital output channel 0
    AnalogOutput::Channel ao0 = daq.AO[0];    // get analog output channel 0
    do0.set_value(High);                    // set DO0 to High TTL
    ao0.set_value(5.0);                     // set AO0 to 5V
    do0.update();                           // sync DO0
    ao0.update();                           // sync AO0

    Encoder::Channel enc0 = daq.encoder[0];          // get encoder channel 0
    Velocity::Channel vel0 = daq.velocity[0];        // get encoder veloicty channel 0
    Amplifier amp("amc_12a8", High, do0, 1.3, ao0);  // create High enabled PWM amplifier with gain 1.3
    Motor motor("maxon_re30", 0.0538, amp);          // create DC motor torque constant 0.0538
    Joint joint("axis0", &motor, &enc0, &vel0, 20);  // create a robotic joint with transmission ratio 20
    Robot robot("simple_robot");                     // create a robot
    robot.add_joint(joint);                          // add joint to robot

    PdController pd(15.0, 0.5);
    Waveform trajectory(Waveform::Sin, seconds(2.0));
    double torque, pos_act, vel_act, pos_ref, vel_ref = 0.0;
    Timer timer(hertz(1000));
    Time time;
    while ((time = timer.get_elapsed_time()) < seconds(60)) {
        daq.update_input();
        pos_act = robot[0].get_position();
        vel_act = robot[0].get_velocity();
        pos_ref = trajectory.evaluate(time);                      // e
        torque = pd.calculate(pos_act, pos_ref, vel_act, vel_ref);  // calculate PD torque
        robot[0].set_torque(torque);
        daq.update_output();
        timer.wait();
    }



    return 0;
}
