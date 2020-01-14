/**
* <pre>
* Author: taylorcyang@tencent.com
* Date:   2020-01-14
* Time:   14:01
* Life with Passion, Code with Creativity.
* </pre>
* https://time.geekbang.org/column/article/185899
*/

#include <gtest/gtest.h>
#include <vector>
#include <numeric>

namespace {
template<typename T>
constexpr auto sum(T t) {
    return t;
}

template<typename T1, typename T2, typename... Args>
constexpr auto sum(T1 t1, T2 t2, Args... args) {
    return sum(t1 + t2, args...);
}


TEST(TEMPLATE, SUM) {
    auto x = sum(1, 2, 3.5, 5);
    EXPECT_EQ(x, 11.5);
    auto same = std::is_same_v<decltype(x), double>;
    EXPECT_TRUE(same);
}

}

namespace {

// (f1 * f2)(x) == f1(f2(x))

template<typename F1>
decltype(auto) compose(F1 f1) { return f1; }

// compose more
template<typename F1, typename F2, typename... Fun>
decltype(auto) compose(F1 f1, F2 f2, Fun... fun) {
    return [f1, f2, fun...](auto&& ... args) {
        return f1(compose(f2, fun...)(std::forward<decltype(args)>(args)...));
    };
}

inline namespace method2 {

template<typename F1, typename F2>
decltype(auto) operator*(F1 f1, F2 f2) {
    return [f1, f2](auto&& ... args) {
        return f1(f2(std::forward<decltype(args)>(args)...));
    };
}

template<typename F1>
decltype(auto) compose_many(F1 f1) {
    return f1;
}

template<typename F1, typename... Fun>
decltype(auto) compose_many(F1 f1, Fun... fun) {
    return f1 * compose_many(fun...);
}

}

TEST(TEMPLATE, COMPOSE) {
    auto add = [](int a1, int a2) { return a1 + a2; };
    auto square = [](int a1) { return a1 * a1; };
    auto negative = [](int a) { return -a; };

    auto fun = compose(square, add);
    auto fun2 = compose(negative, square, add);

    auto manySquare = compose_many(square, square, square, square);

    EXPECT_EQ(fun(1, 2), 9);
    EXPECT_EQ(fun2(1, 2), -9);
    EXPECT_EQ(manySquare(2), 65536);
}

template<template<typename, typename> class OutContainer = std::vector, typename F, class R>
auto fmap(F&& f, R&& inputs) {
    using result_type =  std::decay_t<decltype(f(*inputs.begin()))>;
    OutContainer<result_type, std::allocator<result_type>> result;
    for (auto&& item : inputs) {
        result.push_back(f(item));
    }
    return result;
}

TEST(TEMPLATE, COMPOSE_FMAP) {
    auto sqaure_list = [](auto&& container) {
        return fmap([](auto i) { return i * i; }, container);
    };

    auto sum_list = [](auto&& container) {
        return std::accumulate(std::begin(container), std::end(container), 0);
    };

    auto com_square = compose(sum_list, sqaure_list);
    std::vector v{1, 2, 3, 4, 5};
    EXPECT_EQ(com_square(v), 55);
}


}

namespace {

template<typename F>
void for_each(F) {
}

template<typename F, typename T, typename... TT>
void for_each(F f, T&& item, TT&& ... items) {
    f(std::forward<decltype(item)>(item));
    for_each(f, std::forward<decltype(items)>(items)...);
}


TEST(TEMPLATE, VARIADIC_FOR_EACH) {
    std::vector<int> v;
    for_each([&v](int x) {
        v.push_back(x);
    }, 1, 2, 3);

    EXPECT_EQ(v, (std::vector<int>{1, 2, 3}));
}

}


namespace {

template<class T, T... Ints>
struct integer_sequence {
};

template<size_t... Ints>
using index_sequence =  integer_sequence<size_t, Ints...>;

template<size_t N, size_t... Ints>
struct index_sequence_helper {
    typedef typename index_sequence_helper<N - 1, N - 1, Ints...>::type type;
};

template<size_t... Ints>
struct index_sequence_helper<0, Ints...> {
    typedef integer_sequence<size_t, Ints...> type;
};

template<size_t N>
using make_index_sequence = typename index_sequence_helper<N>::type;


namespace my {

template<typename T, T... seq>
struct integer_sequence {
};

template<size_t... seq>
using index_sequence = integer_sequence<size_t, seq...>;

template<size_t N, size_t... seq>
struct __make_index_sequence_helper {
    using type = typename __make_index_sequence_helper<N - 1, N - 1, seq...>::type;
};

// partial specialize
template<size_t... seq>
struct __make_index_sequence_helper<0, seq...> {
    using type = index_sequence<seq...>;
};

template<size_t N>
using make_index_sequence = typename __make_index_sequence_helper<N>::type;

}

template<typename T>
struct Debug;

make_index_sequence<10> x{};
index_sequence_helper<10>::type;
//Debug<my::make_index_sequence<10>> y;

template<size_t... idx>
std::vector<size_t> getIndex(index_sequence<idx...> sequence) {
    std::vector<size_t> v;
    for_each([&v](size_t x) {
        v.push_back(x);
    }, idx...);
    return v;
}

template<size_t... idx>
std::vector<size_t> myGetIndex(my::index_sequence<idx...> sequence) {
    std::vector<size_t> v;
    for_each([&v](size_t x) {
        v.push_back(x);
    }, idx...);
    return v;
}

TEST(TEMPLATE, SEQUENCE) {
    EXPECT_EQ(
            getIndex(make_index_sequence<5>{}),
            (std::vector<size_t>{0, 1, 2, 3, 4})
    );

    EXPECT_EQ(
            myGetIndex(my::make_index_sequence<5>{}),
            (std::vector<size_t>{0, 1, 2, 3, 4})
    );
}

namespace {

// inverse variadic params

template<size_t N, typename F, typename Arg, typename... Args>
struct __reverse_variadic_helper {
    static auto call(F&& f, Arg&& arg, Args&& ... args) {
        return __reverse_variadic_helper<N - 1, F, decltype(args)..., decltype(arg)>::call(
                std::forward<F>(f), std::forward<Args>(args)..., std::forward<Arg>(arg));
    }
};

template<typename F, typename Arg, typename... Args>
struct __reverse_variadic_helper<0, F, Arg, Args...> {
    static auto call(F&& f, Arg&& arg, Args&& ... args) {
        return f(std::forward<Args>(args)..., std::forward<Arg>(arg));
    }
};

/**
 * reverse_variadic(f,1,2,3) => f(3,2,1)
 */
template<typename F, typename... Args>
auto reverse_variadic(F&& f, Args&& ... args) {
    return __reverse_variadic_helper<sizeof...(args) - 1, F, decltype(args)...>::call(
            std::forward<F>(f), std::forward<Args>(args)...);
}


}

// TODO: not done
TEST(TEMPLATE, REVERSE_VARIADIC) {
    std::vector<int> v;
    reverse_variadic([&](auto&& ...x) {
        for_each([&](auto&& item) {
            v.push_back(item);
        }, x...);
    }, 1, 2, 3);

    EXPECT_EQ(v, (std::vector<int>{3,2,1}));
}

}
