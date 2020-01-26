/*
 * <pre>
 * Author: taylorcyang@tencent.com
 * Date:   2020-01-26
 * Time:   14:23
 * Life with Passion, Code with Creativity.
 * </pre>
 */

#include <gtest/gtest.h>
#include <string>
#include <utility>
#include <optional>
#include <variant>

using namespace std;

TEST(OPTIONAL, TEST) {
    optional<string> null(nullopt);
    optional<string> value(in_place, "hello world");
    optional<string> value2(in_place, "hello world"s);

    EXPECT_THROW({
                     null.value();
                     *null;
                 }, bad_optional_access);

    EXPECT_EQ(value.value(), "hello world"s);
    EXPECT_EQ(value2.value(), "hello world"s);
}

namespace {

template<typename T>
constexpr bool hasValue(const std::optional<T>& t) {
    return t.has_value();
}

template<typename T, typename... TT>
constexpr bool hasValue(const std::optional<T>& t, const std::optional<TT>& ... tt) {
    return t.has_value() && hasValue(tt...);
}

template<typename Fn>
auto lift(Fn&& func) {
    return [f = func](auto&& ... args) {
        using result_type = decltype(f(std::forward<decltype(args)>(args).value()...));
        if (hasValue(args...)) {
            return std::optional<result_type>(f(args.value()...));
        } else {
            return std::optional<result_type>();
        }
    };
}
}

namespace {
constexpr int increase(int n) {
    return n + 1;
}

ostream& operator<<(ostream& os, optional<int>(x)) {
    if (x) {
        os << '(' << *x << ')';
    } else {
        os << "(Nothing)";
    }
    return os;
}

TEST(OPTIONAL, LIFT) {
    auto inc_opt = lift(increase);
    auto plus_opt = lift(plus<int>());
    cout << inc_opt(optional<int>()) << endl;
    cout << inc_opt(make_optional(41)) << endl;
    cout << plus_opt(make_optional(41), optional<int>()) << endl;
    cout << plus_opt(make_optional(41), make_optional(1)) << endl;
}

}

TEST(VARIANT, TEST) {
    variant<int, string> v {"yes"};
    EXPECT_EQ(v.index(), 1);

    v.emplace<1>("hello world"s);
    EXPECT_EQ(v.index(), 1);
    EXPECT_EQ(get<1>(v), "hello world"s);
    EXPECT_EQ(get<string>(v), "hello world"s);

    v.emplace<0>(0);
    EXPECT_EQ(v.index(), 0);
}