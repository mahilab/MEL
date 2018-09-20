
![MEL Logo](https://raw.githubusercontent.com/mahilab/MEL/master/util/logo.png)

# MEL - Mechatronics Engine & Library

MEL is an object-oriented mechatronics and robotics library built from the ground up to make developing and controlling real, custom hardware as simple as possible, without sacrificing user freedom. It is geared toward academics and small teams where speed of development is paramount and learning a large framework such as ROS may be infeasible, but the need for creating robust, safe, and sustainable systems is also required. MEL is also intended for developers who have found high-level or graphical languages such as Simulink and LabVIEW to be limiting, but don't want to dive straight into low-level C API's. MEL is not and will never be a library for robot motion planning, simulation, or artificial intelligence - it is strictly hardware focused.

MEL is actively maintained by the [Mechatronics and Haptic Interfaces Lab](http://mahilab.rice.edu/) at Rice University, where it powers exoskeleton hardware such as the [OpenWrist](https://www.youtube.com/watch?v=CfdejS77hO8&t) and [MAHI Exo-II](https://www.youtube.com/watch?v=Q6znZVT0L1o), [wearable haptic devices](https://www.youtube.com/watch?v=NkcyAI6h9dQ&t=73s), [teaching hardware](http://mahilab.rice.edu/content/hands-haptics-haptic-paddle), and a variety of [student projects](https://www.youtube.com/watch?v=xUzqJCcLZRo).

### DAQ Interfaces

One of the primary benefits of using MEL is that it provide a common DAQ interface for interacting with commercially available hardware. At present, MEL supports DAQs from Quanser such as the **Q8-USB** and **Q2-USB**, and embedded systems form NI such as the **myRIO**, with future plans for integrating more devices from these and other manufactures.

```cpp
Q8Usb daq;                            // instantiate Quanser Q8-USB DAQ
daq.open();                           // open communication with DAQ
daq.enable();                         // enable DAQ
daq.update_input();                   // sync DAQ inputs with real-world
double voltage = daq.AI.get_value(0); // get voltage of analog input channel 0
daq.AO[0].set_value(voltage);         // set voltage of analog output channel 0
daq.update_output();                  // sync DAQ outputs with real-world
```

Each DAQ may have multiple channels of different signal types, from analog and digital I/O to encoder counters. Individual channels can be retrieved, used independently, and passed around as needed:

```cpp
DigitalOutput::Channel do0 = daq.DO[0]; // get digital output channel 0
AnalogOutput::Channel ao0 = daq.AO[0];  // get analog output channel 0
do0.set_value(High);                    // set DO0 to High TTL
ao0.set_value(5.0);                     // set AO0 to 5V
do0.update();                           // sync DO0
ao0.update();                           // sync AO0
```

### Mechatronics Primitives

MEL provides several mechatronics primitive classes to create abstract interfaces for real world hardware. Usually, these objects will be bound to a particular DAQ input or output depending on its functionality.

```cpp
Encoder::Channel enc0 = daq.encoder[0];          // get encoder channel 0
Amplifier amp("amc_12a8", High, do0, 1.3, ao0);  // create High enabled PWM amplifier with gain 1.3
Motor motor("maxon_re30", 0.0538, amp);          // create DC motor torque constant 0.0538
Joint joint("axis0", &motor, &enc0, &enc0, 20);  // create a robotic joint with transmission ratio 20
Robot robot("simple_robot");                     // create a robot
robot.add_joint(joint);                          // add joint to robot
```

### Hardware Control

Once hardware is defined, MEL gives you the basics needed to create precisely timed loops and controller functionality:

```cpp
PdController pd(15.0, 0.5);                                     // create PD control with gains Kp 15 and Kd 0.5
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
```

### Other Features

In addition to it's core functionality, MEL comes packed with other features that mechatronics designers will find useful, including:
- real-time data visualization and scoping
- basic math functions and classes
- error and data logging
- basic signal processing such high/low pass filters
- communication over UDP, TCP, and shared memory
- console interactivity
- keyboard and joystick support

## Getting Started

To get started using MEL, visit the [GitHub Wiki](https://github.com/mahilab/MEL/wiki) page. This guide will run you through process of setting up MEL for your target platform and hardware, building the library, and creating your own MEL driven projects.

## Examples

Several [examples](https://github.com/mahilab/MEL/tree/master/examples) are provided. The [Haptic Paddle repository](https://github.com/mahilab/HapticPaddle) demonstrates a very good use case for new users. To see how MEL is being used for actual research hardware, consider checking out the [OpenWrist](https://github.com/mahilab/OpenWrist) and [MAHI Exo-II](https://github.com/mahilab/MEII) repositories.
