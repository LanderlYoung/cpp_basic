/**
* <pre>
* Author: taylorcyang@tencent.com
* Date:   2019-11-18
* Time:   14:10
* Life with Passion, Code with Creativity.
* </pre>
*/

#include <gtest/gtest.h>

namespace {
using namespace std;

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

    void hello() {
        cout << "X::hello" << endl;
    }
};


}

TEST(Value, Value) {
    struct C {
        X x;

        X getX() { return x; }

        X& getXRef() { return x; }

        X* getXPtr() { return &x; }
    };

    C c;
    X x = c.getX();
    X& xr = c.getXRef();
    X* p = c.getXPtr();

    c.getX().hello();
    c.getXRef().hello();
    c.getXPtr()->hello();
}

TEST(Value, Pointer) {
    struct C {
        X* x;
        bool own;

        C() : x(new X()), own(true) {}

        explicit C(X* x) : x(x), own(false) {}

        ~C() { if (own) delete x; }

        X* getXPtr() { return x; }
    };

    C c;
    X* p = c.getXPtr();
    c.getXPtr()->hello();
}

TEST(Value, Reference) {
    struct C {
        X& x;

        explicit C(X& x): x(x) {}

        X& getXRef() { return x; }
    };

    X myX;
    C c(myX);

    X& xr = c.getXRef();
    c.getXRef().hello();
}

