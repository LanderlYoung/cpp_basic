/*************************************************************************
  > File Name:     new.cpp
  > Author:        Landerl Young
  > Mail:          LanderlYoung@gmail.com
  > Created Time:  Sun Nov 17 20:33:10 2019
 ************************************************************************/

#include <iostream>
#include <cstdlib>
#include <gtest/gtest.h>

using namespace std;

struct A {
    A() {
        cout << "A@" << this << endl;
    }

    ~A() {
        cout << "~A@" << this << endl;
    }
};


TEST(New, NewDelete) {
    A* a = new A();
    cout << "a@" << a << endl;
    delete a;
}

TEST(New, PlacementNewDelete) {
    char data[sizeof(A)];
    void* ptr = data;

    cout << "ptr@" << ptr << endl;

    A* a = new(ptr)A();
    cout << "a@" << ptr << endl;
    a->~A();
}

TEST(New, DeleteArray) {
    A* array = new A[2]();
    delete[] array;

    array = new A[2];
    // UB!!!
    delete array;
}

/*
void* operator new(size_t len) {
    cout << "new " << len << endl;
    return std::malloc(len);
}

void operator delete(void* ptr) noexcept {
    cout << "free" << endl;
    std::free(ptr);
}
 */

TEST(New, OverrideNew) {
    cout << endl << __PRETTY_FUNCTION__ << endl;

    std::string str0 = "short string";
    std::string str1 = "this is a very long string";
}

