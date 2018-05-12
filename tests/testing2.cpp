#include <MEL/Communications/MelShare.hpp>
#include <MEL/Core/Clock.hpp>
#include <MEL/Engine/Static/Component.hpp>
#include <MEL/Engine/Static/Object.hpp>
#include <MEL/Logging/Log.hpp>
#include <MEL/Math/Constants.hpp>
#include <MEL/Utility/Console.hpp>
#include <deque>
#include <string>
#include <tuple>

using namespace mel;

MelShare actuator_torque("torque");

class Actuator : public Component {
public:
    void late_update() override {
        // actuator_torque.write_data({ torque });
        dummy = torque;
    }
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

    Actuator* actuator         = nullptr;
    PositionSensor* pos_sensor = nullptr;
    Transmission* transmission = nullptr;

    double position;
    double torque;
};

MelShare ms("monitor");

class Monitor : public Component {
public:
    Monitor() : data(3) {}
    void update() override {
        data[0] = encoder->counts;
        data[1] = joint->position;
        data[2] = transmission->ratio;
        // ms.write_data(data);
    }
    Encoder* encoder           = nullptr;
    Joint* joint               = nullptr;
    Transmission* transmission = nullptr;
    std::vector<double> data;
};

int main() {
    init_logger(Verbose, Verbose);
    enable_realtime();

    Object<Encoder, Actuator, Transmission, Joint, Monitor> object1(
        "object1", nullptr, {2000}, {}, {0.05}, {}, {});
    object1.get<Joint>()->actuator       = object1.get<Actuator>();
    object1.get<Joint>()->pos_sensor     = object1.get<Encoder>();
    object1.get<Joint>()->transmission   = object1.get<Transmission>();
    object1.get<Monitor>()->encoder      = object1.get<Encoder>();
    object1.get<Monitor>()->joint        = object1.get<Joint>();
    object1.get<Monitor>()->transmission = object1.get<Transmission>();

    object1.start();
    Clock clock;
    for (std::size_t i = 0; i < 10000000; i++) {
        object1.update();
        object1.late_update();
    }
    print(clock.get_elapsed_time());
    object1.stop();

    return 0;
}
