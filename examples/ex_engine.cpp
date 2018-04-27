#include <MEL/Engine/Component.hpp>
#include <MEL/Engine/Engine.hpp>
#include <MEL/Engine/Object.hpp>
#include <MEL/Logging/Log.hpp>
#include <MEL/Utility/Types.hpp>
#include <MEL/Utility/Console.hpp>

using namespace mel;

struct ComponentA;
struct ComponentB;
struct ComponentC;

struct ComponentA : public Component {
    void start() override {
        print(get_object()->name + " start(A)");
    }

    void update() override {
        print(get_object()->name + " update(A)");
    }

    void late_update() override {
        print(get_object()->name + " late_update(A)");
    }

    void stop() override {
        print(get_object()->name + " stop(A)");
    }
};

struct ComponentB : public Component {
    void start() override {
        print(get_object()->name + " start(B)");
    }

    void update() override {
        print(get_object()->name + " update(B)");
    }

    void late_update() override {
        print(get_object()->name + " late_update(B)");
    }

    void stop() override {
        print(get_object()->name + " stop(B)");
    }
};

struct ComponentC : public Component {

    void start() override {
        print(get_object()->name + " start(C)");
    }

    void update() override {
        print(get_object()->name + " update(C)");
    }

    void late_update() override {
        print(get_object()->name + " late_update(C)");
    }

    void stop() override {
        print(get_object()->name + " stop(C)");
    }
};

class Evan {
public:
    int x = 0;
    template <typename T>
    void operator()() {x++;}
};

int main(int argc, char* argv[]) {

    init_logger(Verbose,Verbose);

    // Object object1("object1");
    // object1.add<ComponentA>();

    // Object object2("object2");
    // object2.add<ComponentC>();

    // Object object3("object3");
    // object3.add<ComponentB>();

    // Object object4("object4");
    // object4.add<ComponentC>();

    // object1.add_child(&object2);
    // object2.add_child(&object3);
    // object1.add_child(&object4);

    // object1.print_family_tree();

    // Engine engine(&object1);
    // engine.run(1);
    //

    Evan e;
    print(e.x);


    return 0;
}

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
// - order components and objects linearly in memory (this is impossible without a custom pool allocator)
// - multipe-root objects (make engine "root object")
// - circular parenting checks
