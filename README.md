
![MEL Logo](https://raw.githubusercontent.com/epezent/MEL/master/logo.png)

# MEL - Mechatronics Engine & Library

## Developers

- **[Evan Pezent](http://evanpezent.com)** (epezent@rice.edu)
- **[Craig McDonald](http://craiggmcdonald.com/)** (craig.g.mcdonald@gmail.com)

## About

MEL is an object-oriented mechatronics and robotics library built from the ground up to make developing and controlling real, custom hardware as simple as possible, without sacrificing user freedom. It is geared toward academics and small teams where speed of development is paramount and learning a large framework such as ROS may be infeasible, but the need for creating robust, safe, and sustainable systems is also required. MEL is also intended for developers who have found high-level or graphical languages such as Simulink and LabVIEW to be limiting, but don't want to dive straight into low-level C API's. MEL is not and will never be a library for robot motion planning, simulation, or artificial intelligence - it is strictly hardware focused.

At its core, MEL provides a light layer of hardware abstraction, wrapping APIs from common hardware manufacturers such as National Instruments and Quanser into DAQ classes. Readable and writable DAQ channels are then then distributed among the elements which comprise actual hardware, such as Joints, Position Sensors, and Actuators. Constructs such as precisely timed control loops and event driven state machines allow for sophisticated hardware control. In addition, MEL provides basic signal processing, error and data logging, and the ability to communicate with other applications and languages over a network or shared memory.

## Getting Started

To git started using MEL, visit the [GitHub Wiki](https://github.com/epezent/MEL/wiki) page. This guide will run you through process of setting up MEL for your target platform and hardware, building MEL, and creating your own projects.

## Examples

Several examples can be found in [examples](https://github.com/epezent/MEL/tree/master/examples). The Haptic Paddle example demonstrates a very good use case for new users. To see how MEL is being used for actual research hardware, consider checking out the [OpenWrist](https://github.com/epezent/OpenWrist) and [MAHI Exo-II](https://github.com/craigmc707/MEII) repositories.
