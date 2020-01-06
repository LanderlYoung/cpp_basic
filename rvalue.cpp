/**
* <pre>
* Author: taylorcyang@tencent.com
* Date:   2019-12-25
* Time:   20:42
* Life with Passion, Code with Creativity.
* </pre>
*/

#include <gtest/gtest.h>

// TODO: need to be completed.
TEST(RVALUE, ProlongLifeTime) {
    struct X {
        std::string name;
        ~X() { std::cout << name << std::endl; }
        X tmp() { return X{name + " >"}; }
    };

    const X& local = X{"one"};
    X&& r = X{"two"};
    X{"three"};

    {
        std::cout << "more tmp" << std::endl;
        const auto& str = X{"four"}.tmp().tmp().tmp().name;
        std::cout << "str:" << str << std::endl;
    }

    // output:
    // three
    // two
    // one

    // `const X&` and `X&&` prolong life-time of prvalue(temporary value).
}
