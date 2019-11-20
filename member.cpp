/**
* <pre>
* Author: taylorcyang@tencent.com
* Date:   2019-11-18
* Time:   10:24
* Life with Passion, Code with Creativity.
* </pre>
*/

#include <gtest/gtest.h>

TEST(Member, VariableInit) {
    struct X {
        int a;
        bool b;
        void* c;
    };

    char mem[sizeof(X)];
    std::memset(mem, 0xFF, sizeof(X));

    X* x = new(mem)X();
    EXPECT_EQ(x->a, 0);
    EXPECT_EQ(x->b, 0);
    EXPECT_EQ(x->c, nullptr);
}

TEST(Member, VariableInitWithDefault) {
    struct X {
        int a;
        // default initializers. (C++11)
        bool b = true;
        void* c;
    };

    char mem[sizeof(X)];
    std::memset(mem, 0xFF, sizeof(X));

    X* x = new(mem)X();
    EXPECT_EQ(x->a, 0);
    EXPECT_EQ(x->b, true);
    EXPECT_EQ(x->c, nullptr);

}

TEST(Member, VariableInitWithPartialInit) {
    struct X {
        int a;
        bool b = true;
        void* c;

        X() : a(1), b(false) {
        }
    };


    char mem[sizeof(X)];
    std::memset(mem, 0xFF, sizeof(X));

    X* x = new(mem)X();
    EXPECT_EQ(x->a, 1);
    EXPECT_EQ(x->b, false);
    EXPECT_EQ(x->c, nullptr);
}

TEST(Member, VariableInitDestructOrder) {
    using namespace std;
    struct A {
        A() { cout << "A" << endl; }

        ~A() { cout << "~A" << endl; }
    };
    struct B {
        B() { cout << "B" << endl; }

        ~B() { cout << "~B" << endl; }
    };

    struct X {
        A a;
        B b;

        X() : b(), a() {}
    };

    X x;
}

TEST(Member, PrimitiveVariableInitWithConstructorSyntax) {
    struct X {
        int a;
        bool b = true;
        float c;
        void* d;

        X() : a(), b(), c(), d() {
        }
    };

    char mem[sizeof(X)];
    std::memset(mem, 0xFF, sizeof(X));

    X* x = new(mem)X();
    EXPECT_EQ(x->a, 0);
    EXPECT_EQ(x->b, false);
    EXPECT_EQ(x->c, 0.0f);
    EXPECT_EQ(x->d, nullptr);

    /*
    0x104b140d0 <+0>:  pushq  %rbp
    0x104b140d1 <+1>:  movq   %rsp, %rbp
    0x104b140d4 <+4>:  movq   %rdi, -0x8(%rbp)
    0x104b140d8 <+8>:  movq   -0x8(%rbp), %rdi
->  0x104b140dc <+12>: movl   $0x0, (%rdi)
    0x104b140e2 <+18>: movb   $0x0, 0x4(%rdi)
    0x104b140e6 <+22>: xorps  %xmm0, %xmm0
    0x104b140e9 <+25>: movss  %xmm0, 0x8(%rdi)
    0x104b140ee <+30>: movq   $0x0, 0x10(%rdi)
    0x104b140f6 <+38>: popq   %rbp
    0x104b140f7 <+39>: retq
    0x104b140f8 <+40>: nopl   (%rax,%rax)
     */
}

TEST(Member, CompilerGeneratedFunction) {
    struct X {
        X() = default; // 1
        ~X() = default; // 2

        X(const X& copy) = default; // 3
        X& operator=(const X& copy) = default; // 4

        X(X&& move) = default; // 5
        X& operator=(X&& move) = default; //6
    };
}


TEST(Member, CompilerGeneratedFunctionTest) {
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
    };

    cout << ">>x1" << endl;
    X x1;

    cout << ">>x2" << endl;
    X x2(x1);

    cout << ">>x3" << endl;
    X x3 = x2;

    cout << ">>x4" << endl;
    X x4((X()));

    cout << ">>x5" << endl;
    X x5 = X();

    cout << ">>x6" << endl;
    X x6(std::move(x2));

    cout << ">>x7" << endl;
    X x7 = std::move(x3);

    cout << ">>8" << endl;
    x1 = x7; // 8

    cout << ">>9" << endl;
    x2 = std::move(x7); // 9

}
