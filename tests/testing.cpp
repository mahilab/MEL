#include <MEL/Utility/Console.hpp>
#include <typeindex>
#include <unordered_map>


class ModuleX {
public:
    ModuleX(std::string name) : name_(name) { }
    virtual void print() {
        mel::print("Module X: " + name_);
    }
protected:
    std::string name_;
};

class ModuleA : public ModuleX {
public:
    using ModuleX::ModuleX;
    void print() override {
        mel::print("Module A: " + name_);
    }
    void A() {
        mel::print("I'm an A");
    }
};

class ModuleB : public ModuleX {
public:
    using ModuleX::ModuleX;
    void print() override {
        mel::print("Module B: " + name_);
    }
    void B() {
        mel::print("I'm a B");
    }
};

class Daq {

public:

    Daq() : modx("eggs"), moda("eh"), modb("bee") {
        add_module(&modx);
        //add_module(&moda);
        //add_module`(static_cast<ModuleX*>(&modb));
    }

    template <class T>
    T& get_module() {
        return *static_cast<T*>(modules[std::type_index(typeid(T))]);
    }

private:

    ModuleX modx;
    ModuleA moda;
    ModuleB modb;

    template <class T>
    void add_module(T* module) {
        modules[std::type_index(typeid(T))] = module;
    }

    std::unordered_map<std::type_index, void*> modules;
};


int main(int argc, char const *argv[]) {
    Daq daq;
    daq.get_module<ModuleA>().print();
    //daq.get_module<ModuleA>().print();
    //daq.get_module<ModuleB>().print();
    return 0;
}

