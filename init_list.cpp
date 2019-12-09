/**
* <pre>
* Author: taylorcyang@tencent.com
* Date:   2019-12-03
* Time:   16:08
* Life with Passion, Code with Creativity.
* </pre>
*/

#include <gtest/gtest.h>

using namespace std;

namespace {
struct X {
    X() { cout << "X" << endl; }

    ~X() { cout << "~X" << endl; }

    X(const X& copy) { cout << "X&" << endl; }

    X& operator=(const X& copy) {
        cout << "X&=" << endl;
        return *this;
    }

    X(X&& move) noexcept { cout << "X&&" << endl; }

    X& operator=(X&& move) noexcept {
        cout << "X&&=" << endl;
        return *this;
    }
};

void func(std::initializer_list<X> args) {
    cout << args.begin() << endl;
}

template<typename... T>
void func(T&& ... t) {
    func({std::forward<T>(t)...});
}

TEST(InitializerList, Init) {
    func({X(), X(), X()});
    func(X(), X(), X());
}

}