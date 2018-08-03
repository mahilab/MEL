
![MEL Logo](https://raw.githubusercontent.com/epezent/MEL/master/logo.png)

# MEL - Mechatronics Engine & Library

## Developers

- **[Evan Pezent](http://evanpezent.com)** (epezent@rice.edu)
- **[Craig McDonald](http://craiggmcdonald.com/)** (craig.g.mcdonald@gmail.com)

## About

MEL is an object-oriented mechatronics and robotics library built from the ground up to make developing and controlling real, custom hardware as simple as possible, without sacrificing user freedom. It is geared toward academics and small teams where speed of development is paramount and learning a large framework such as ROS may be infeasible, but the need for creating robust, safe, and sustainable systems is also required. MEL is also intended for developers who have found high-level or graphical languages such as Simulink and LabVIEW to be limiting, but don't want to dive straight into low-level C API's. MEL is not and will never be a library for robot motion planning, simulation, or artificial intelligence - it is strictly hardware focused.

At its core, MEL provides a light layer of hardware abstraction, wrapping APIs from common hardware manufacturers such as National Instruments and Quanser into DAQ classes. Readable and writable DAQ channels are then then distributed among the elements which comprise actual hardware, such as Joints, Position Sensors, and Actuators. Constructs such as precisely timed control loops and event driven state machines allow for sophisticated hardware control. In addition, MEL provides basic signal processing, error and data logging, and the ability to communicate with other applications and languages over a network or shared memory.

## MEL At A Glance

### DAQs

One of the primary benefits of using MEL is that it provide a common DAQ interface for interacting with commercially available hardware. At present, MEL supports DAQs from Quanser such as the **Q8-USB** and **Q2-USB**, and embedded systems form NI such as the **myRIO**, with future plans for integrating more devices from these and other manufactures.

```cpp
Q8Usb daq;                            // instantiate Quanser Q8-USB DAQ
daq.enable();                         // enable DAQ
daq.update_input();                   // sync DAQ inputs with real-world
double voltage = daq.AI.get_value(0); // get voltage of analog input channel 0
daq.AO[0].set_value(voltage);         // set voltage of analog output channel 0
daq.update_output();                  // sync DAQ outputs with real-world
```

Each DAQ may have multiple channels of different signal types, from analog and digital I/O to encoder counters. Individual channels can be retrieved, used independently, and pass around as needed:

```cpp
DigitalOutput::Channel do0 = daq.DO[0]; // get digital output channel 0
AnalogOutput::Channel ao0 = daq.AO[0];  // get analog output channel 0
Encoder::Channel enc0 = daq.encoder[0]; // get encode channel 0
do0.set_value(High);                    // set DO0 to High TTL
ao0.set_value(5.0);                     // set AO0 to 5V
do0.update();                           // sync DO0
ao0.update();                           // sync AO0
enc0.update();                          // sync encoder 0
int32 counts = enc0.get_value();        // get encoder counts
```

### Mechatronic Primitives

MEL provides several mechatronics primitives, which can be bound to generic DAQ channels:

```cpp
Amplifier amp("amc_12a8", High, do0, 1.3, ao0);            // create PWM amplifier
Motor motor("maxon_re30", 0.0538, amp);                    // create DC motor
PositionSensor* pos_sensor = &enc0;                        // create position sensor from encoder
VelocitySensor* vel_sensor = &daq.velocity[0];             // create velocity sensor from DAQ encoder velocity channel
Joint joint("axis0", &motor, pos_sensor, vel_sensor, 20);  // create a robotic joint
Robot robot("simple_robot");                               // create a robot
robot.add_joint(joint);                                    // add joint to robot
```


## Getting Started

To get started using MEL, visit the [GitHub Wiki](https://github.com/epezent/MEL/wiki) page. This guide will run you through process of setting up MEL for your target platform and hardware, building MEL, and creating your own projects.

## Examples

Several examples can be found in [examples](https://github.com/epezent/MEL/tree/master/examples). The Haptic Paddle example demonstrates a very good use case for new users. To see how MEL is being used for actual research hardware, consider checking out the [OpenWrist](https://github.com/epezent/OpenWrist) and [MAHI Exo-II](https://github.com/craigmc707/MEII) repositories.
