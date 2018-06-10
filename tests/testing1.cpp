#include <iostream>
#include <array>
#include <chrono>
#include <functional>
#include <string>
#include <cassert>
#include <map>
#include <unordered_map>
#include <algorithm>
#include <typeindex>
#include <map>

//=============================================================================
// UTILITY
//=============================================================================

template <typename T>
void print(T printable) {
    std::cout << printable << std::endl;
}

long long measure(const std::string& topic, const std::function<void()>& body) {
    std::cout << "Measure " << topic << ": ";
    auto begin = std::chrono::high_resolution_clock::now();
    body();
    auto end = std::chrono::high_resolution_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
    std::cout << ms << " ms" << std::endl;
    return ms;
}

static const std::array<int, 10> primes = { 2, 3, 5, 7, 11, 13, 17, 19, 23, 29 };
static int prime_index = 0;

//=============================================================================
// FAST CAST IMPLEMENTATION
//=============================================================================

class BaseObject {
public:
    int id = 1;
};

template <typename O>
class Object : public BaseObject {
public:
    static int type_id;
    Object() {
        if (type_id == 0)
            type_id = primes[prime_index++];
    }
};

template <typename C> int Object<C>::type_id(0);

//=============================================================================
// FAST CAST
//=============================================================================

class PositionSensor : public Object<PositionSensor> {
public:

};


//=============================================================================
// DYNAMIC CAST EQUIVALENT
//=============================================================================

class BaseD {
    virtual void base() {}
};


class Xd : public BaseD {

};

class Yd : public Xd {

};

class Zd : public Yd {
public:
    int method_z() { return 3; }
};

//=============================================================================
// BENCHMARK
//=============================================================================

int main() {



    print(PositionSensor::type_id);
    // Dynamic Cast Test

    //BaseD* xd = new Xd;
    //BaseD* yd = new Yd;
    //BaseD* zd = new Zd;

    //std::size_t num_iterations = 10000000;

    //measure("Dynamic Cast", [&]() {
    //    volatile size_t accumulated = 0;
    //    for (size_t i = 0; i < num_iterations; ++i) {
    //        Zd z;
    //        BaseD& b = z;
    //        accumulated += dynamic_cast<Zd&>(b).method_z();
    //    }
    //    print(accumulated);
    //});

    return 0;
}
