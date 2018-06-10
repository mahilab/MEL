#include <MEL/Engine/Static/Object.hpp>
#include <MEL/Engine/Static/Component.hpp>
#include <MEL/Utility/Console.hpp>
#include <array>

using namespace mel;

class Encoder : public Component {
public:

};

class Actuator : public Component {
public:

};

class Transmission : public Component {
public:
    Transmission() {
        msg_ = "void";
    }
    Transmission(std::string msg) {
        msg_ = msg;
    }

    std::string msg_;
};

typedef Object<Actuator,Encoder,Transmission> Joint;

template <int Size>
class Robot {
public:
    Joint& operator[](std::size_t i) {
        return joints_[i];
    }

    std::array<Joint,Size> joints_;
};

class OpenWrist : public Robot<3> {

};

int main(int argc, char* argv[]) {

    OpenWrist rob;
    print(rob[0].get<Transmission>().msg_);
    rob[0].get<Transmission>() = Transmission("evan");
    rob[0].init<Transmission>("joe");
    print(rob[0].get<Transmission>().msg_);

    std::array<int,3> x;

    return 0;
}

// HomogenousObject<typename T> overloads brackets (e.g. Robot)
