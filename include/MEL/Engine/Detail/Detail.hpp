#ifndef MEL_ENGINE_DETAIL_HPP
#define MEL_ENGINE_DETAIL_HPP

#include <tuple>

namespace mel {

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

}

#endif //MEL_ENGINE_DETAIL_HPP