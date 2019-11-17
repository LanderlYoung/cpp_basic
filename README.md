[toc]

# C++ 基础解惑

# 1 new

## 1 Placement new: 在给定地址上new

众所周知C++的new操作符实际上会做两件事：
1. 分配内存
2. 创建类结构并调用构造函数

因为C++语言使得程序员对内存具有完全的访问能力，假如需要在一个已经分配好的内存块上创建类要怎么做呢？这种需求通常发生在内存池、按需创建、动态创建等场景上。

``` cpp
void placement_new_delete() {
    char data[sizeof(A)];
    void *ptr = data;

    // 1 在地址ptr上创建类A
    A* a = new (ptr)A();
    // 2. 销毁 a
    a->~A();
}
```

上述1在地址ptr上创建类A，并返回ptr自身（cpp标准要求placement new必须返回原指针），而调用者需要保证ptr指向的内存要足够A使用(>=sizeof(A));

类似new，调用者需要自行delete。这里我们使用的是placement delete，即直接调用A的析构函数，你可能是第一次见这么使用，但是析构函数也确实是一个成员函数（你甚至可以讲他生命为private），因此直接调用并没有什么特别。只是需要保证不能多次调用。

2. delete[] VS delete
3. Override new 
4. Class specific new
5. https://en.cppreference.com/w/cpp/memory/new/operator_new
