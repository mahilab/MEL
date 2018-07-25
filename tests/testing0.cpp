#include <MEL/Core/Timer.hpp>
#include <MEL/Math/Waveform.hpp>
#include <MEL/Utility/Console.hpp>
#include <MEL/Logging/Log.hpp>
#include <MEL/Daq/NI/MyRio/MyRio.hpp>
#include <string>

using namespace mel;
using namespace std;

struct PartBase {
    PartBase(string name) : m_part(name) {}
    string m_part;
};

class Vehicle {
public:
    Vehicle(string name) : m_name(name) {}
    string m_name;
};

class Boat : public virtual Vehicle {
public:
    Boat(string name) : Vehicle(name) {}
    struct Part : public virtual PartBase {
        Part() : PartBase("boat_part") {}
    };

    Part getPart() {
        return Part();
    }
};

class Plane : public virtual Vehicle {
public:
    Plane(string name) : Vehicle(name) {}
    struct Part : public virtual PartBase {
        Part() : PartBase("plane_part") {}
    };

    Part getPart() {
        return Part();
    }
};

class BoatPlane : public Boat, public Plane {
public:
    BoatPlane(string name) : Vehicle(name), Boat("boat"), Plane("plane") {}
    struct Part : public Boat::Part, Plane::Part {
        Part() : PartBase("boatplane_part") {}
    };

    Part getPart() {
        return Part();
    }
};

void printVehicleName(Vehicle& x) {
    print(x.m_name);
}

void printBoatName(Boat& x) {
    print(x.m_name);
    print(x.getPart().m_part);
}

void printPlaneName(Plane& x) {
    print(x.m_name);
    print(x.getPart().m_part);
}

void printBoatPlaneName(BoatPlane& x) {
    print(x.m_name);
    print(x.getPart().m_part);
}

int main(int argc, char **argv)
{

    BoatPlane bp("boatplane");

    printVehicleName(bp);
    printBoatName(bp);
    printPlaneName(bp);
    printBoatPlaneName(bp);



    // init_logger(Verbose,Verbose);
    // MyRio myrio("myrio");
    // myrio.enable();
    // Waveform sinwave(Waveform::Sin, seconds(1), 2.5, 2.5);
    // Timer timer(hertz(100));
    // while (timer.get_elapsed_time() < seconds(1)) {
    //     myrio.update_input();
    //     print(myrio.C.AI[0].get_value());
    //     double value = sinwave.evaluate(timer.get_elapsed_time());
    //     myrio.C.AO[0].set_value(value);
    //     myrio.update_output();
    //     timer.wait();
    // }
}

