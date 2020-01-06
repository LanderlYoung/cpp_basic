/**
* <pre>
* Author: taylorcyang@tencent.com
* Date:   2020-01-06
* Time:   15:22
* Life with Passion, Code with Creativity.
* </pre>
*/

#include <gtest/gtest.h>
#include <thread>

static void test(bool useThreadLocal) {
    if (!useThreadLocal) {
        std::cout << "just do it " << std::endl;
    } else {
        static thread_local struct DoIt {
            DoIt() {
                std::cout << "DoIt()" << std::endl;
            }

            ~DoIt() {
                std::cout << "~DoIt()" << std::endl;
            }

            void doooo() {
                std::cout << "doooo " << this << std::endl;
            }
        } emm;

        emm.doooo();
    }
}

TEST(ThreadLocal, Inside) {
    std::thread t([]() {
        test(false);
        test(false);
        test(true);
        test(true);
        test(false);
    });

    t.join();
}

