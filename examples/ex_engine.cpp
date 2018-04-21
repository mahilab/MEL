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

#define MAX_COMPONENT_FAMILY_DEPTH 10

class BaseObject {
public:
    static int next_family_id;
};

int BaseObject::next_family_id(0);

template <typename T>
struct Comp : public BaseObject
{
    static int family_id;
    static int id;

    Comp()
    {
        if (family_id == 0) {
            family_id = next_family_id + MAX_COMPONENT_FAMILY_DEPTH;
            next_family_id += MAX_COMPONENT_FAMILY_DEPTH;
        }
    }


protected:
    ~Comp() // objects should never be removed through pointers of this type
    {
    }
};

template <typename T> int Comp<T>::family_id(0);

class X :public Comp<X>
{
    // ...
};

class Y : public Comp<Y>
{
    // ...
};

class Z : public Comp<X> {

};


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
// - add requirements with tuple!
// - can give unique ID with CRTP, e.g. class PositionSensor : public Component<PositionSensor>, but
//   inheritcan would have to look like class Encoder : public PositionSensor, so it'd have same ID