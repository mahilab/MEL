#include <MEL/Daq/Quanser/Q8Usb.hpp>
#include <MEL/Core.hpp>
#include <MEL/Math.hpp>

using namespace mel;

int main(int argc, char* argv[]) {

    Q8Usb daq;                            // instantiate Quanser Q8-USB DAQ
    daq.open();                           // open communication with DAQ
    daq.enable();                         // enable DAQ
    daq.update_input();                   // sync DAQ inputs with real-world
    double voltage = daq.AI.get_value(0); // get voltage of analog input channel 0
    daq.AO[0].set_value(voltage);         // set voltage of analog output channel 0
    daq.update_output();                  // sync DAQ outputs with real-world

    DigitalOutput::Channel do0 = daq.DO[0]; // get digital output channel 0
    AnalogOutput::Channel ao0 = daq.AO[0];  // get analog output channel 0
    do0.set_value(High);                    // set DO0 to High TTL
    ao0.set_value(5.0);                     // set AO0 to 5V
    do0.update();                           // sync DO0
    ao0.update();                           // sync AO0

    Encoder::Channel enc0 = daq.encoder[0];          // get encoder channel 0
    Amplifier amp("amc_12a8", High, do0, 1.3, ao0);  // create High enabled PWM amplifier with gain 1.3
    Motor motor("maxon_re30", 0.0538, amp);          // create DC motor torque constant 0.0538
    Joint joint("axis0", &motor, &enc0, &enc0, 20);  // create a robotic joint with transmission ratio 20
    Robot robot("simple_robot");                     // create a robot
    robot.add_joint(joint);                          // add joint to robot

    PdController pd(15.0, 0.5);                                     // create PD control with gains Kp 14 and Kd 0.5
    Waveform trajectory(Waveform::Sin, seconds(2.0));               // create sinwave trajectory
    double torque, pos_act, vel_act, pos_ref, vel_ref = 0.0;        // control variables
    Timer timer(hertz(1000));                                       // create 1000 Hz control loop timer
    Time t;                                                         // current time
    while ((t = timer.get_elapsed_time()) < seconds(60)) {          // loop for 1 minute
        daq.update_input();                                         // sync DAQ inputs with real-world
        pos_act = robot[0].get_position();                          // get robot joint position
        vel_act = robot[0].get_velocity();                          // get robot joint velocity
        pos_ref = trajectory(t);                                    // evaluate trajectory
        torque = pd.calculate(pos_act, pos_ref, vel_act, vel_ref);  // calculate PD torque
        robot[0].set_torque(torque);                                // set robot joint torque
        daq.update_output();                                        // sync DAQ outputs with real-world
        timer.wait();                                               // wait for 1 ms to elapse
    }

    return 0;
}
