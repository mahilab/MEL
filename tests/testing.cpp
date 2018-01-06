#include <MEL/Daq/Quanser/Q8Usb.hpp>
#include <MEL/Utility/System.hpp>
#include <MEL/Utility/Console.hpp>
#include <memory>

using namespace mel;

class Base {
public:
    Base(std::string something) : something_(something) {}

    virtual void print_something() const {
        print("Base: " + something_);
    }

    std::string something_;
};

class Derived : public Base {
public:
    Derived(std::string something) : Base(something) {}

    void print_something() const override {
        print("Derived: " + something_);
    }
};

class Test {
public:
    Test(Base* b) : base(b) {}

    std::shared_ptr<Base> base;

    /** Something */
    const Base& get_base() {
        return *base;
    }


};

int main() {
    Test test(nullptr);
    test.base->print_something();
    test.get_base().print_something();
    const Base* x = &(test.get_base());
    x->print_something();
    delete x;
    return 0;
}