/**
* <pre>
* Author: taylorcyang@tencent.com
* Date:   2019-12-25
* Time:   20:42
* Life with Passion, Code with Creativity.
* </pre>
*/

#include <gtest/gtest.h>

TEST(RVALUE, ProlongLifeTime) {
    struct X {
        std::string_view name;
        ~X() { std::cout << name << std::endl; }
    };

    const X& local = X{"one"};
    X&& r = X{"two"};
    X{"three"};

    // output:
    // three
    // two
    // one

    // `const X&` and `X&&` prolong life-time of prvalue(temporary value).
}
