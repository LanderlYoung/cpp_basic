/**
* <pre>
* Author: taylorcyang@tencent.com
* Date:   2019-12-25
* Time:   11:42
* Life with Passion, Code with Creativity.
* </pre>
*/

#include <gtest/gtest.h>
#include <iostream>

TEST(Exception, ExceptionSafe) {
    using namespace std;
    struct X {
        std::string name_;

        explicit X(std::string&& name, bool throwInCtor)
                : name_(std::move(name)) {
            cout << "X() " << name_ << endl;
            if (throwInCtor) {
                throw std::exception();
            }
        }

        ~X() {
            cout << "~X() " << name_ << endl;
        }
    };


    struct ThrowInCtor {
        X x1;
        X x2;
        X x3;

        ThrowInCtor()
                : x1("x1", false),
                  x2("x2", true),
                  x3("x3", false) {
            // x2 throw exception
            // if a ctor throws exception, the class is considered construct failure.
            // already-constructed fields in this class is dtor in order.
            // be the dtor will not be called.
            // And if the class is create with new, the memory will also be deleted.

            // So, in here:
            // x1 got destructed
            // x2 won't (construct failed)
            // x3 won't (not constructed)
        }

        ~ThrowInCtor() {
            cout << "~ThrowInCtor()" << endl;
        }

        static void* operator new(size_t size) {
            cout << "new ThrowInCtor" << endl;
            return std::malloc(size);
        }

        static void operator delete(void* ptr) {
            cout << "delete ThrowInCtor" << endl;
            std::free(ptr);
        }
    };

    try {
        new ThrowInCtor();
    } catch (const std::exception&) {
    }

    // output:
    // new ThrowInCtor
    // X() x1
    // X() x2
    // ~X() x1
    // delete ThrowInCtor
}