/*
 * <pre>
 * Author: taylorcyang@tencent.com
 * Date:   2020-01-26
 * Time:   14:43
 * Life with Passion, Code with Creativity.
 * </pre>
 */

#include <gtest/gtest.h>
#include <type_traits>
#include <string>

TEST(TRAIT, IS_TRIVALLY_DESTRUCTIBLE) {
    struct X {
    };
    struct X0 {
        ~X0() = default;
    };
    struct X1 {
        ~X1() {}
    };
    struct X2 {
        std::string v;
    };
    struct X3 {
        std::string v;

        ~X3() {}
    };

    static_assert(std::is_trivially_destructible_v<X>);
    static_assert(!std::is_trivially_destructible_v<X1>);
    static_assert(!std::is_trivially_destructible_v<X2>);
    static_assert(!std::is_trivially_destructible_v<X3>);

}