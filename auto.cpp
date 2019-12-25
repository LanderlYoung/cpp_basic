/*
 * <pre>
 * Author: taylorcyang@tencent.com
 * Date:   2019-11-17
 * Time:   22:18
 * Life with Passion, Code with Creativity.
 * </pre>
 */
#include <gtest/gtest.h>
#include <vector>

TEST(Auto, Decltype) {
    int a;

    // decltype
    static_assert(std::is_same_v<int, decltype(a)>);

    auto b = a; // 等价于 decltype(a) b = a; 即 int b = a;

    auto& c = a; // 等价于 decltype(a)& c = a; 即 int& c = a;

    auto& d = c;
    static_assert(std::is_same_v<int&, decltype(c)>);
    static_assert(std::is_same_v<int&, decltype(d)>);
    // 等价于 decltype(c)& c = a;
    // 但是由于不能声明引用的引用类型（不同于指针）, 因此等价于
    // int& d = c;

    const auto A = a; // 等价于 const decltype(a) A = a; 即 const int A = a;
    static_assert(std::is_same_v<const int, decltype(A)>);

    const auto B = A;
    static_assert(std::is_same_v<const int, decltype(A)>);
    // 等价于 const decltype(A) B = A;
    // 但是由于const不能重复，因此等价于
    // 即 const int B = A;

    auto p = &a; // 等价于 decltype(&a) p = &a; 即 int* p = &a;
    static_assert(std::is_same_v<int*, decltype(p)>);

    auto* pp = &a; // 由于&a 的类型固定，因此pp的类型仍然是 int*
    static_assert(std::is_same_v<int*, decltype(pp)>);
}

TEST(Auto, Vector) {
    std::vector<int> pi{3, 1, 4, 1, 5, 9, 2, 6};

    std::vector<int>::iterator it0 = std::find(pi.begin(), pi.end(), 7);
    auto it1 = std::find(pi.begin(), pi.end(), 7);
    static_assert(std::is_same_v<decltype(it0), decltype(it1)>);

    std::map<int, std::pair<std::string, std::chrono::milliseconds>> someMap;
    std::map<int, std::pair<std::string, std::chrono::milliseconds>>::iterator it2 = someMap.find(0);
    auto it3 = someMap.find(0);
    static_assert(std::is_same_v<decltype(it2), decltype(it3)>);

}

template<typename T>
struct Test;


TEST(Auto, UniversalRef) {
    std::vector<bool> x;

    // `std::vector<bool>::iterator:operator *()`
    // returns a value, not a reference, can't bind to a reference.
//    for (auto& b: x) {
//    }


//    for (auto&& b: x) {
//        static_assert(std::is_same_v<decltype(b), bool&>);
//        ::Test<decltype(b)> _;
//    }

//    for (auto& b: (std::vector<bool>{true, false})) {
//    }

    for (auto&& b: (std::vector<bool>{true, false})) {
    }
}
