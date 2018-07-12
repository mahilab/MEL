#ifndef MEL_ENGINE_DETAIL_HPP
#define MEL_ENGINE_DETAIL_HPP

#include <tuple>
#include <iostream>
#include <string>
#include <tuple>
#include <type_traits>

namespace mel {

// TUPLE GET

template <bool b, typename T = void>
using EnableIf = typename std::enable_if<b, T>::type;

template <int Index,
          template <typename T> class Search,
          int Which,
          typename,
          class First,
          class... Types>
struct get_internal : get_internal<Index + 1, Search, Which, void, Types...> {};

template <int Index,
          template <typename T> class Search,
          int Which,
          class First,
          class... Types>
struct get_internal<Index,
                    Search,
                    Which,
                    EnableIf<!Search<First>::value>,
                    First,
                    Types...>
    : get_internal<Index + 1, Search, Which, void, Types...> {};

template <int Index,
          template <typename T> class Search,
          int Which,
          class First,
          class... Types>
struct get_internal<Index,
                    Search,
                    Which,
                    EnableIf<Search<First>::value>,
                    First,
                    Types...>
    : get_internal<Index + 1, Search, Which - 1, void, Types...> {};

template <int Index,
          template <typename T> class Search,
          class First,
          class... Types>
struct get_internal<Index,
                    Search,
                    0,
                    EnableIf<Search<First>::value>,
                    First,
                    Types...> : std::integral_constant<int, Index> {
    typedef get_internal type;
};

template <template <typename> class Test, int Which = 0, class... Types>
auto get(std::tuple<Types...>& tuple) -> decltype(
    std::get<get_internal<0, Test, Which, void, Types...>::value>(tuple)) {
    return std::get<get_internal<0, Test, Which, void, Types...>::value>(tuple);
}

template <typename T>
struct is_type {
    template <typename U>
    using test = std::is_same<T, U>;
};

template <class T, int Which = 0, class... Types>
T& get(std::tuple<Types...>& tuple) {
    return get<is_type<T>::template test, Which>(tuple);
}



// TUPLE ITERATIOIN

namespace detail {
template <int... Is>
struct seq {};

template <int N, int... Is>
struct gen_seq : gen_seq<N - 1, N - 1, Is...> {};

template <int... Is>
struct gen_seq<0, Is...> : seq<Is...> {};

template <typename T, typename F, int... Is>
void for_each(T&& t, F f, seq<Is...>) {
    auto l = {(f(std::get<Is>(t)), 0)...};
}
}  // namespace detail

template <typename... Ts, typename F>
void for_each_in_tuple(std::tuple<Ts...>& t, F f) {
    detail::for_each(t, f, detail::gen_seq<sizeof...(Ts)>());
}

struct start_functor {
    template <typename T>
    void operator()(T&& t) {
        t.start();
    }
};

struct update_functor {
    template <typename T>
    void operator()(T&& t) {
        t.update();
    }
};

struct late_update_functor {
    template <typename T>
    void operator()(T&& t) {
        t.late_update();
    }
};

struct stop_functor {
    template <typename T>
    void operator()(T&& t) {
        t.stop();
    }
};

struct reset_functor {
    template <typename T>
    void operator()(T&& t) {
        t.reset();
    }
};



template <typename TObject>
struct set_references_functor {
    set_references_functor(TObject* object) : object(object) {}

    template <typename T>
    void operator()(T&& t) {
        t.set_references(*object);
    }
    TObject* object;
};
}

#endif  // MEL_ENGINE_DETAIL_HPP
