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

## 2 delete[] VS delete

我们知道可以使用 `new A[]` 的形式创建类 A 的数组，同时需要注意一定要对应的使用 `delete[] array`的形式来销毁之。 原因有几点：
1. new 和 new[] 是两个操作符，他们分配的内存可能不能使用同一的释放逻辑。
2. new[] 会 从0到N 依次调用构造函数，delete[] 会从N到0依次调用析构函数。

因此使用 `new[]` 创建的数组一定要使用 `delete[]` 释放之。

## 3 override global operator new

我们知道new new[] delete delete[]都是操作符，cpp标准库提供默认实现（通常是调用malloc和free），但是我们也可以自己提供自己的实现，比如使用jemalloc，dlmalloc达到更高的性能，甚至是提供GC能力。

全局new操作符，也就是定义在顶层命名空间的new，即：`::new` 与 `::new[]`。

``` cpp
void* operator new(size_t len) {
    cout << "new " << len << endl;
    return std::malloc(len);
}

void operator delete(void* ptr) noexcept {
    cout << "free" << endl;
    std::free(ptr);
}

void override_new() {
    std::string str0 = "short string";
    std::string str1 = "this is a very long string";
}
```

这段程序会输出
```
new 16
free
```
如果你上debugger会发现new调用时在STL的string内部的，这也就意味着重载**全局new操作符**之后对程序的所有new调用都适用。

仔细想一下会发现一个盲点，明明有两个string为什么只new了一次。（注意std::string的语义是创建一个string的copy并拥有自己的内存）。

这里实际上是和STL实现相关的，测试用的clang的libc++做了small objects optimization，避免小字符串的动态内存分配。该技术在C++中广泛应用（例如std::function内部也有使用）。

另外还可以重载 new[] 和 delete[]

详见: https://en.cppreference.com/w/cpp/memory/new/operator_new

> 思考一下会发现C++的new关键字如上所述有两个动作，但是new操作符确只负责了内存分配。delete同理。

## 4 Class specific new

除了上述全局的new操作符，每个类甚至客户可以定义自己的new操作符。


```cpp
TEST(New, ClassSpecifiedNew) {
    class C {
    public:
        void* operator new(size_t s) {
            cout << "new C" << endl;
            return std::malloc(s);
        }

        void operator delete(void* ptr) {
            cout << "delete C" << endl;
            std::free(ptr);
        }
    };

    C* c = new C();
    delete c;
}
```
上述代码会如约输出：

```
new C
delete C
```

通常class-specific new的用处甚少用于内存分配，更多的是**禁止类的动态创建**，即不能使用new关键字创建类，只能在栈上创建，也即当做值类型使用。只需要将new操作符定义为private 或 delete （C++11）。

```cpp
TEST(New, ClassSpecificNewDelete) {
    class C {
    public:
        void* operator new(size_t s) = delete;

        void operator delete(void* ptr) = delete;
    };

    // can't compile
    C* c = new C();
    delete c;
}
```

编译输出：

```
[ 83%] Building CXX object CMakeFiles/cpp_basic.dir/new.cpp.o
/Users/young/Desktop/cpp_basic/new.cpp:95:12: error: call to deleted function 'operator new'
    C* c = new C();
           ^
/Users/young/Desktop/cpp_basic/new.cpp:89:15: note: candidate function has been explicitly deleted
        void* operator new(size_t s) = delete;
              ^
/Users/young/Desktop/cpp_basic/new.cpp:96:5: error: attempt to use a deleted function
    delete c;
    ^
/Users/young/Desktop/cpp_basic/new.cpp:91:14: note: 'operator delete' has been explicitly marked deleted here
        void operator delete(void* ptr) = delete;
```

上述用法因为过于普遍，主要应用于RAII相关的类中，如Google V8 的 ContextScope。因此可以定义一个通用的宏。

``` cpp
#define SCRIPT_ENGINE_DISALLOW_NEW()                    \
    static void* operator new(std::size_t) = delete;    \
    static void* operator new[](std::size_t) = delete;  \
    static void operator delete(void*) = delete;        \
    static void operator delete[](void*) = delet
```


# 3 值，指针，引用

从Java到C++的程序员通常会被值类型搞得晕头转向，因为Java中只有引用类型（其实对应于C++应该是指针），这使得事件变得简单明了。而C++中的值和引用是的复杂度发生了指数级的上升。

## 值

## 指针

## 引用

## 内存ownerness

1. 值类型拥有内存所有权，因为他总是copy一份内存。
2. 指针内存所有权不明确。
3. 引用不具备内存所有权，他只能表示”借用“一块内存。

中间说明引用：

```cpp
A& getA();

void printA(A& a);
```

上述getA返回`A&`意味着

# prefer nullptr

为了表示空指针，在java中我们有关键字 `null`，在C中有标准库的宏`NULL`，在C++中其实一直没有一个标准的做法，直到C++11。

在C++11之前，推荐的做法是使用`0`来代表空指针，为什么不能用`NULL`呢？是因为C++中不允许`void*`类型隐士的转换成其他指针，但是反过来可以，这主要是为了类型的安全性考虑。
因此：
```cpp

// C 的NULL定义
#define NULL ((void*)0);

// 编译不通过
char* name = NULL; 
// OK
char* name = 0;
```

但是使用0始终还是有问题，毕竟他不是指针类型，考虑下面的方法重载场景：
```cpp
void setAge(Age* age);
void setAge(int ageNum);

// reset bob
bob->setAge(0);
```
上面本意是调用空指针，但是由于方法重载决议实际调用的是int型参数，这就会带来很多问题。

因此C++11之后推荐使用`nullptr`关键字，其作用等同于java的`null`关键字。
相应的，（为了更学院派），C++中的`nullptr`有自己的类型`nullptr_t`，他允许被隐式转换成任意指针类型。


# prefer auto
C++11中重新定义了几乎没人使用的auto关键字，使其具备自动类型推导的能力。（类似kotlin的var）。相应的还有一个`decltype`类型用来直接求出值得类型。

看一下auto怎么用。
```cpp
TEST(AUTO, Decltype) {
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
```

顺便吐槽一下，C++允许auto加不加`&`和`*`表达同样的语义的设计是的代码变得模棱两可，个人并不喜欢这种逻辑，笔者更喜欢的是明确一种固定的方式。


auto的出现使得代码更简洁易懂，使用起来如下代码所示：

```cpp

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
```

对比一下用auto和不用auto的是不是有着天壤之别，尤其是下面那个map的例子。

> Life is short, use auto!

