#include <MEL/Core/Timer.hpp>
#include <MEL/Engine/Component.hpp>
#include <MEL/Engine/Engine.hpp>
#include <MEL/Engine/Object.hpp>
#include <MEL/Logging/Log.hpp>
#include <MEL/Utility/Console.hpp>
#include <MEL/Utility/Windows/Keyboard.hpp>
#include <string>
#include <vector>
#include <MEL/Communications/Windows/MelShare.hpp>

using namespace mel;

class ComponentA : public Component {

};

class ComponentB : public Component {
public:

};

class ComponentC : public ComponentB {
public:

};

int main(int argc, char* argv[]) {

    init_logger(Verbose, Verbose);

    Object object1("object1");
    object1.add_component<ComponentA>();
    object1.add_component<ComponentB>();
    object1.add_component<ComponentC>();
    if (object1.get_derived_component<ComponentB>())
        print(object1.get_derived_component<ComponentB>()->get_type_name());

    Clock clock;
    for (size_t i = 0; i < 1000; i++)
    {
        object1.get_component<ComponentC>();
    }
    print(clock.get_elapsed_time());

    //Engine engine;
    //engine.set_root_object(&object2);
    //engine.run(hertz(1000), seconds(10));

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