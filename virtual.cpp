/**
* <pre>
* Author: taylorcyang@tencent.com
* Date:   2019-12-20
* Time:   10:58
* Life with Passion, Code with Creativity.
* </pre>
*/

#include <gtest/gtest.h>

#define EXPECT_PTR_EQ(lhs, rhs) EXPECT_EQ((void*)(lhs), (void*)(rhs))
#define EXPECT_PTR_NE(lhs, rhs) EXPECT_NE((void*)(lhs), (void*)(rhs))

namespace {


TEST(Virtual, Inherit) {

}


TEST(Virtual, CtorDtor) {

}


using namespace std;

class A {
    int a = 0;
public:
    virtual ~A() = default;

    char greet_a() {
        cout << "a" << endl;
        return 'a';
    }
};

class B {
    int b = 0;
public:
    virtual ~B() = default;

    char greet_b() {
        cout << "b" << endl;
        return 'b';
    }
};

class C {
    int c = 0;
public:
    virtual ~C() = default;

    char greet_c() {
        cout << "c" << endl;
        return 'c';
    }
};

class Derived : public A, public B, public C {
};

TEST(Virtual, ClassStructurePointers) {
    auto d = new Derived();

    A* a = d;
    B* b = d;
    C* c = d;


    EXPECT_PTR_EQ(d, a);
    EXPECT_PTR_NE(d, b);
    EXPECT_PTR_NE(d, c);

    EXPECT_PTR_EQ(d, dynamic_cast<Derived*>(a));
    EXPECT_PTR_EQ(d, dynamic_cast<Derived*>(b));
    EXPECT_PTR_EQ(d, dynamic_cast<Derived*>(c));

    EXPECT_EQ(d, a);
    EXPECT_EQ(d, b);
    EXPECT_EQ(d, c);

    // !!!
    auto* dd = reinterpret_cast<Derived*>(b);
    EXPECT_PTR_EQ(dd, b);
    EXPECT_EQ('c', dd->greet_c());
}

}

