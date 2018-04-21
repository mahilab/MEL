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
#include <MEL/Math/Constants.hpp>

using namespace mel;

MelShare actuator_torque("torque");

double get_counts() {
    static double counts = 0;
    if (Keyboard::is_key_pressed(Key::Up))
        counts += 1;
    if (Keyboard::is_key_pressed(Key::Down))
        counts -= 1;
    return counts;
}

class Actuator : public Component {
public:
    void on_late_update() override {
        actuator_torque.write_data({ torque });
    }
    double torque;
};

class PositionSensor : public Component {
public:
    double position;
};


class Encoder : public PositionSensor {
public:

    Encoder(int counts_per_rev) : counts_per_rev(counts_per_rev) {}

    void on_update() override {
        counts = get_counts();
        position = 2 * PI * static_cast<double>(counts) /
                   static_cast<double>(counts_per_rev);
    }

    int counts_per_rev;
    int counts;
};

class Transmission : public Component {
public:
    Transmission(double ratio) : ratio(ratio) {}
    const double ratio;
};

class Joint : public Component {
public:
    Joint() {
        add_requirement<Actuator>();
        add_requirement<PositionSensor>();
        add_requirement<Transmission>();
    }

    void on_start() override {
        actuator     = get_component<Actuator>();
        pos_sensor = get_component<PositionSensor>();
        transmission = get_component<Transmission>();
    }

    void on_update() override {
        if (pos_sensor)
            position = pos_sensor->position;
        else
            position = 0.0;
        if (transmission)
            position *= transmission->ratio;
        if (actuator && transmission)
            actuator->torque = torque * transmission->ratio;
        else if (actuator)
            actuator->torque = torque;
    }    

    Actuator* actuator;
    PositionSensor* pos_sensor;
    Transmission* transmission;

    double position;
    double torque;
};

class Monitor : public Component {
public:
    Monitor() : ms("monitor"), data(3) {}
    void on_update() override {
        data[0] = get_component<Encoder>()->counts;
        data[1] = get_component<Joint>()->position;
        data[2] = get_component<Transmission>()->ratio;
        ms.write_data(data);
    }
    MelShare ms;
    std::vector<double> data;
};

int main(int argc, char* argv[]) {

    init_logger(Verbose, Warning);
    enable_realtime();

    Object joint1("joint1");
    joint1.add_component<Encoder>(2000);
    joint1.add_component<Joint>();
    joint1.add_component<Actuator>();
    joint1.add_component<Transmission>(1.0 / 20.0);
    joint1.add_component<Monitor>();
    Engine engine;
    engine.set_root_object(&joint1);
    engine.run(hertz(1000), seconds(60));

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