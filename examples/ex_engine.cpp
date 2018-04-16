#include <MEL/Core/Timer.hpp>
#include <MEL/Engine/Component.hpp>
#include <MEL/Engine/Engine.hpp>
#include <MEL/Engine/Object.hpp>
#include <MEL/Logging/Log.hpp>
#include <MEL/Utility/Console.hpp>
#include <MEL/Utility/Windows/Keyboard.hpp>
#include <string>
#include <vector>

using namespace mel;

// Basic Component
class ComponentA : public Component {
public:

    void on_start() override { print(object->name + "::ComponentA::start()"); }

    void on_update() override {
        print(object->name + "::ComponentA::update()");
    }

    void on_stop() override { print(object->name + "::ComponentA::stop()"); }
};

// Component w/ internal data
class ComponentB : public Component {
public:

    void on_start() override { print(object->name + "::ComponentB::start()"); }

    void on_update() override {
        print(object->name + "::ComponentB::update()");
        ++i;
    }

    void on_stop() override { print(object->name + "::Actuator::stop()"); }

    int i;
};

// Component that communicates with another Component
class ComponentC : public Component {
public:

    void on_start() override {
        print(object->name + "::ComponentC::start()");
        b = get_component<ComponentB>();
        if (b) {
            std::cout << "    b->i = " << b->i << std::endl;
        }
    }

    void on_update() override {
        print(object->name + "::ComponentC::update()");
        if (b) {
            std::cout << "    b->i = " << b->i << std::endl;
        }
    }

    void on_stop() override { print(object->name + "::ComponentC::stop()"); }

    ComponentB* b;
};

// Component w/ constructor and destructor
class ComponentD : public Component {
public:

    ComponentD(const std::string& message) {
        print("ComponentD: " + message);
    }

    ~ComponentD() {
        print(object->name + "::ComponentD destructed");
    }

    void on_start() override { print(object->name + "::ComponentD::start()\n"); }

    void on_update() override {
        print(object->name + "::ComponentD::update()\n");
    }

    void on_stop() override { print(object->name + "::ComponentD::stop()\n"); }
};

int main(int argc, char* argv[]) {
    init_logger(Verbose, Verbose);

    Object parent("parent");
    parent.add_component<ComponentA>();

    Object child("child", &parent);
    child.add_component<ComponentB>();
    child.add_component<ComponentC>();

    Object gchild("gchild");
    gchild.add_component<ComponentD>("Hello, World");
    child.add_child(&gchild); // another way to parent gchild

    // create Engine with root Object
    Engine engine(&parent);
    // run Engine at 10Hz for 1 second
    engine.run(hertz(10), seconds(1));

    return 0;
}
