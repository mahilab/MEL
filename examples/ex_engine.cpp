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

class Encoder : public Component {
public:

    Encoder(int32 counts_per_revolution) :
        counts(0),
        counts_per_revolution(counts_per_revolution) {}

    void on_update() {
        if (Keyboard::is_key_pressed(Key::Up))
            counts += 1;
        else if (Keyboard::is_key_pressed(Key::Down))
            counts -= 1;
        position = ((double)counts) / ((double)counts_per_revolution);
    }

    int32 counts;
    int32 counts_per_revolution;
    double position;
};

class Transmission : public Component {
public:

    Transmission(double ratio) : ratio(ratio) {}
    double ratio;
};

class Joint : public Object {
public:

    Joint(const std::string& name, Object* parent = nullptr) :
        Object(name, parent)
    {
        add_component<Encoder>(4000);
        add_component<Transmission>(20);
    }

    double get_position() {
        return get_component<Encoder>()->position / get_component<Transmission>()->ratio;
    }

};

class Observer : public Component {
public:

    Observer() {
        add_requirement<Encoder>();
        add_requirement<Transmission>();
    }

    void on_update() override {
        print(get_component<Encoder>()->position / get_component<Transmission>()->ratio);
    }

};

class ComponentA : public Component {

};

class ComponentB : public Component {
public:
    ComponentB() {
        add_requirement<ComponentA>();
    }
};

class ComponentC : public Component {
public:
    ComponentC() {
        add_requirement<ComponentA>();
        add_requirement<ComponentB>();
    }
};

int main(int argc, char* argv[]) {

    init_logger(Verbose, Verbose);

    Object object1("object1");
    object1.add_component<ComponentA>();
    Object object2("object2", &object1);
    object2.add_component<ComponentA>();
    object2.add_component<ComponentB>();
    Object object3("object3", &object2);
    // object3.add_component<ComponentA>();
    object3.add_component<ComponentB>();
    object3.add_component<ComponentC>();

    object1.print_family_tree();

    Engine engine;
    engine.set_root_object(&object2);
    engine.run(hertz(1000), seconds(10));

    return 0;
}
