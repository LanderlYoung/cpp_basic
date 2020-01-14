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


}
