#include <MEL/Utility/Console.hpp>
#include <deque>
#include <string>
#include <tuple>
#include <MEL/Core/Time.hpp>

// Variadic Policy Design

using namespace mel;

//=============================================================================
// VERSION 1
//=============================================================================

class A {
public:
    void printA() {
        print("A");
    }
};

class B {
public:
    void printB() {
        print("B");
    }
};

template <typename... Components>
class Object1 : public Components ... {
public:

};

//=============================================================================
// VERSION 2
//=============================================================================

class C {
public:
    void printC() {
        print("C");
    }
    void update() {
        printC();
    }
};

class D {
public:
    void printD() {
        print("D");
    }
    void update() {
        printD();
    }
};

class BaseObject2 {
public:
    virtual void update() {}
};

template <typename ... Components>
class Object2 : public BaseObject2 {
public:
    void update() {
        int list[] = { (Components().update(),0)... };
        (void)list;
    }
};

//=============================================================================
// VERSION 3
//=============================================================================

// Template Metaprogramming Magic

namespace detail
{
    template<int... Is>
    struct seq { };

    template<int N, int... Is>
    struct gen_seq : gen_seq<N - 1, N - 1, Is...> { };

    template<int... Is>
    struct gen_seq<0, Is...> : seq<Is...> { };
}

namespace detail
{
    template<typename T, typename F, int... Is>
    void for_each(T&& t, F f, seq<Is...>)
    {
        auto l = { (f(std::get<Is>(t)), 0)... };
    }
}

template<typename... Ts, typename F>
void for_each_in_tuple(std::tuple<Ts...> & t, F f)
{
    detail::for_each(t, f, detail::gen_seq<sizeof...(Ts)>());
}

struct update_functor {
    template<typename T>
    void operator() (T&& t) {
        t.update();
    }
};

class E {
public:
    E(int e1, int e2) : e1(e1), e2(e2) {}
    void update() {
        std::cout << "E " << e1 << " " << e2 << "\n";
    }
    int e1, e2;
};

class F {
public:
    F(const std::string& name) : name(name), e(nullptr) {}
    void update() {
        print("F " + name);
    }
    std::string name;
    E* e;
};

template <typename Object>
class BaseObject3 {
public:
    virtual void update() {}

    template <typename T>
    T& get_component() {
        return static_cast<Object*>(this)->get<T>();
    }
};

template <typename ... Components>
class Object3 : public BaseObject3<Object3<Components...>> {
public:
    
    Object3(const Components& ... arg) : components(arg...) {}

    void update() {
        for_each_in_tuple(components, update_functor());
    }

    template <typename Component>
    Component& get() {
        return std::get<Component>(components);
    }

private:

    std::tuple<Components ...> components;
};

int main() {
    // version 1
    Object1<A, B> ab;
    ab.printA();
    ab.printB();
    // version 2
    Object2<C, D> cd;
    cd.update();
    // version 3
    Object3<E, F> ef ( 
        { 1,2 },
        { "evan" } 
    );

    //BaseObject3* ef_ptr = &ef;

    ef.get_component<F>().name = "john";

    ef.update();



    return 0;
}

