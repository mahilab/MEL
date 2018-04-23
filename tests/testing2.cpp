#include <MEL/Utility/Console.hpp>
#include <MEL/Engine/Object.hpp>
#include <deque>
#include <string>
#include <tuple>
#include <MEL/Logging/Log.hpp>


using namespace mel;

// COMPONENTS

class A;

class B {
public:
    B(const std::string& name);
    virtual void update();
    std::string name;
    A* a;
};

class A {
public:
    A(const std::string& name) : name(name), b(nullptr) {}
    virtual void update() {
        if (b)
            print("A " + name + " knows B " + b->name);
        else
            print("A " + name + " doesn't know an B");
    }
    std::string name;
    B* b;
};

B::B(const std::string& name) : name(name), a(nullptr) {}
void B::update() {
    if (a)
        print("B " + name + " knows A " + a->name);
    else
        print("B " + name + " doesn't know an A");
}

class C : public A {
public:
    C() : A("Carl") {}

};

class D  {
public:
    D(const std::string& name) : name(name), a(nullptr) {}

    void update() {
        if (a)
            print("D " + name + " knows A " + a->name);
        else
            print("B " + name + " doesn't know an A");
    }
    std::string name;
    A* a;
};


class AB : public Object<A, B> {
public:
    AB() : Object<A, B>("AB", { "Alex" }, { "Burt" }, nullptr) { }
};

class CD : public Object<C, D> {
public:
    CD() : Object<C, D>("CD", {}, { "Dane" }, nullptr) { }
};


int main() {

    init_logger(Verbose, Verbose);

    AB object1;
    object1.update();
    object1.get<A>()->b = object1.get<B>();
    object1.get<B>()->a = object1.get<A>();
    object1.get<A>()->name = "Andy";
    object1.get<B>()->name = "Beth";
    object1.update();

    CD object2;
    object2.get<D>()->a = object2.get<C>();
    object2.update();

    object2.add_child(&object1);
    //object1.print_family_tree();

    return 0;
}

