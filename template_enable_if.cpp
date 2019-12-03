/**
* <pre>
* Author: taylorcyang@tencent.com
* Date:   2019-11-25
* Time:   14:33
* Life with Passion, Code with Creativity.
* </pre>
*/
#include <gtest/gtest.h>

TEST(StdLib, Function) {
    struct A {
        void hello(int) {
        }
    };

    std::function<void(A*, int)> func = &A::hello;
    func = [](A*, int) {
    };

    // functor to std::function
    struct F {
        void operator()(int) {}
    };

    std::function<void(int)> ff = std::function<void(int)>(F());
    ff = F();
}

namespace {

struct Str {
};

Str newString(const std::string& p) {
    return {};
}

Str newString(const char* p) {
    return {};
}

bool newObj(const Str& str) {
    return true;
}

template<typename StringLike,
        std::enable_if_t<std::is_convertible_v<StringLike, const char*>, int> = 0>
bool newObj(StringLike&& s) {
    return newObj(newString(std::forward<StringLike>(s)));
}

TEST(UniversalReference, PerfectForward) {
    newObj(Str());

    newObj("");
    EXPECT_TRUE((std::is_convertible_v<std::string, std::string>));

    EXPECT_TRUE((std::is_convertible_v<char const (&)[], const char*>));

    EXPECT_TRUE((std::is_convertible_v<std::string, const std::string&>));

    EXPECT_TRUE((std::is_convertible_v<std::string, std::string>));
}


}