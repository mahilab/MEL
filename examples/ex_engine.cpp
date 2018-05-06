#include <MEL/Engine/Component2.hpp>
#include <MEL/Engine/Engine.hpp>
#include <MEL/Engine/Object.hpp>
#include <MEL/Logging/Log.hpp>
#include <MEL/Utility/Console.hpp>
#include <MEL/Utility/Types.hpp>

using namespace mel;

struct ComponentA;
struct ComponentB;
struct ComponentC;

class Daq : public Component2 {
public:
    using Component2::Component2;
    void update() {
        print("Getting DAQ Inputs");
    }

    void late_update() {
        print("Setting DAQ Outputs");
    }
};

class PositionSensor : public Component2 {
public:
    using Component2::Component2;
    void update() { print("Getting PositionSensor Positions"); }
};

class VelocitySensor : public Component2 {
public:
    using Component2::Component2;
    void update() { print("Getting VelocitySensor Velocities"); }
};

class Actuator : public Component2 {
public:
    using Component2::Component2;
    void late_update() { print("Setting Actuator Torques"); }
};

class Joint : public Component2 {
public:
    Joint(const std::string& name) : Component2(name) {
        //order(Update, AfterChildrenForward);
        //order(LateUpdate, BeforeChildrenReverse);
    }
    void update() { print("Getting Joint Positions / Velocities"); }
};

class Controller : public Component2 {
public:
    using Component2::Component2;
    void update() {
        print("Computing Control Law");
    }
};

int main(int argc, char* argv[]) {

    init_logger(Verbose, Verbose);

    Component2 root("root");
    print(root.family_tree());



    return 0;
}

// ORDER OF OPERATIONS
// 1) get daq inputs
// 2) get sensor positions / velocities
// 3) get joints positions / velocities
// 4) controller
// 5) controller
// 6) set joint torques
// 7) set actuator torques
// 8) set daq outputs

// q8()
// q8.add<robot>()
// q8.get<robot>().add<joint>();
// q8.get<robot>().get<joint>().add<encoder>();
// q8.get<robot>().get<joint>().add<velocity>();
// q8.get<robot>().get<joint>().add<actuator>();
// q8.get<robot>().get<joint>().add<transmission();
// q8.get<robot>().get<joint>().set_mode(Mode::ChildrenFirst);
// q8.get<robot>().clone<joint>(0);
// q8.get<robot>().clone<joint>(0);

// with / and functions

// COMPONENTS
// Actuator -> Motor
// Force Sensor
// Joint (if this then that)
// Limiter (different types)
// PDController
// PositionSensor -> Encoder
// VelocitySensor -> VirtualVelocitySensor
// Amplifier
// Module -> Input, Output, InputOutput, Encoder (rename), Velocity

// PREFAB OBJECTS
// Device (can be merged with Components)
// Robot (container of Joints)
// DAQ -> Q8USB

// CORE
// Clock
// Frequency
// Time
// Timer

// IDEAS
// - should DAQ derive from Engine?
// - order components and objects linearly in memory (this is impossible without
// a custom pool allocator)
// - multipe-root objects (make engine "root object")
// - circular parenting checks
// - DAQ should be part of engine, analogous to "renderer" or "pyhsics " engine

// on read , on write


/*
a1 [ComponentA]
|---b1 [ComponentB]
|   |---c1 [ComponentC]
|   |---c2 [ComponentC]
|---b2 [ComponentB]
|---c3 [ComponentC]
*/
