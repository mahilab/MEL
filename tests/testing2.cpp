#include <MEL/Utility/Console.hpp>
#include <deque>
#include <string>
#include <tuple>

// Variadic Template Engine Design

using namespace mel;

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

template <typename TObject>
struct set_references_functor {
    set_references_functor(TObject* object) : object(object) {}

    template<typename T>
    void operator() (T&& t) {
        t.set_references(*object);
    }
    TObject* object;
};

class BaseObject {
public:
    virtual void update() {}
};

template <typename ... Components>
class Object : public BaseObject {
public:
    Object(const Components& ... arg) : components(arg...) {
        for_each_in_tuple(components, set_references_functor<Object>(this));
    }
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

#define GET_COMPONENT(T,t) \
        template <typename TObject> \
        void set_references(TObject& object) { \
            t = &object.template get<T>(); \
        } \
        T* t;

// USAGE

class E;

class F {
public:
    GET_COMPONENT(E, e)
    F(const std::string& name);
    void update();
    std::string name;
};

class E {
public:
    GET_COMPONENT(F, f)
    E(const std::string& name) : name(name), f(nullptr) {}
    void update() {
        print("E " + name + " knows F " + f->name);
    }
    std::string name;
};

F::F(const std::string& name) : name(name), e(nullptr) {}
void F::update() {
    print("F " + name + " knows E " + e->name);
}

class EF : public Object<E, F> {
public:
    EF() : Object<E,F>({ "Evan" }, { "Fred" } )
    {

    }
};


int main() {
    Object<E, F> object ( { "Evan" }, { "Fred" } );
    object.update();
    object.get<E>().name = "Elane";
    object.get<F>().name = "Frank";
    object.update();
    return 0;
}

/*
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
*/
