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
        bool b = true;
        void* c;
    };

    char mem[sizeof(X)];
    std::memset(mem, 0xFF, sizeof(X));

    X* x = new(mem)X();
    EXPECT_EQ(x->a, 0);
    EXPECT_EQ(x->b, true);
    EXPECT_EQ(x->c, nullptr);
    // note：这个用例在clang上能通过，但是按照标准他应该和下面的等价
    // 因此也不能通过。
    // 结论，要么全部不初始化（交给编译器初始化默认值），要么全部初始化
    // 永远不要初始化部分值
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