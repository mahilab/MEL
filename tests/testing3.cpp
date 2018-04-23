#include <MEL/Communications/Windows/MelShare.hpp>
#include <MEL/Core/Timer.hpp>
#include <MEL/Engine/Component.hpp>
#include <MEL/Engine/Engine.hpp>
#include <MEL/Engine/Object.hpp>
#include <MEL/Logging/Log.hpp>
#include <MEL/Math/Constants.hpp>
#include <MEL/Math/Waveform.hpp>
#include <MEL/Utility/Console.hpp>
#include <MEL/Utility/Windows/Keyboard.hpp>
#include <string>
#include <vector>

using namespace mel;

MelShare actuator_torque("torque");

class Actuator : public Component {
public:
    void late_update() override { dummy = torque; }
    double torque;
    double dummy;
};

class PositionSensor : public Component {
public:
    double position;
};

class Encoder : public PositionSensor {
public:
    Encoder(int counts_per_rev) : counts_per_rev(counts_per_rev) {}

    void update() override {
        counts++;
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
    Joint() {}

    void start() override {
        actuator     = get<Actuator>();
        pos_sensor   = get<PositionSensor>();
        transmission = get<Transmission>();
    }

    void update() override {
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
    Monitor(const std::string& name) : ms(name), data(3) {}

    void start() override {
        encoder      = get<Encoder>();
        joint        = get<Joint>();
        transmission = get<Transmission>();
    }

    void update() override {
        data[0] = encoder->counts;
        data[1] = joint->position;
        data[2] = transmission->ratio;
        // ms.write_data(data);
    }
    Encoder* encoder           = nullptr;
    Joint* joint               = nullptr;
    Transmission* transmission = nullptr;
    MelShare ms;
    std::vector<double> data;
};

int main(int argc, char* argv[]) {
    init_logger(Verbose, Verbose);
    enable_realtime();

    Object joint1("joint1");
    joint1.add<Encoder>(2000);
    joint1.add<Actuator>();
    joint1.add<Transmission>(0.05);
    joint1.add<Joint>();
    joint1.add<Monitor>("monitor1");

    Engine engine;
    engine.set_root_object(&joint1);
    engine.run(10000000);

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
// - order components and objects linearly in memory (this is impossible without
// a custom pool allocator)
// - multipe-root objects (make engine "root object")
