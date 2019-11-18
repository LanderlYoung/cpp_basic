[toc]

# C++ 基础解惑

# 1 new

## 1.1 Placement new: 在给定地址上new

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

## 1.2 delete[] VS delete

我们知道可以使用 `new A[]` 的形式创建类 A 的数组，同时需要注意一定要对应的使用 `delete[] array`的形式来销毁之。 原因有几点：
1. new 和 new[] 是两个操作符，他们分配的内存可能不能使用同一的释放逻辑。
2. new[] 会 从0到N 依次调用构造函数，delete[] 会从N到0依次调用析构函数。

因此使用 `new[]` 创建的数组一定要使用 `delete[]` 释放之。

## 1.3 override global operator new

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

另外还可以重载 new[] 和 delete[]，此外类的operator new系列函数是默认static的（不然咧？？？）。

详见: https://en.cppreference.com/w/cpp/memory/new/operator_new

> 思考一下会发现C++的new关键字如上所述有两个动作，但是new操作符确只负责了内存分配。delete同理。

## 1.4 Class specific new

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
cpp_basic/new.cpp:95:12: error: call to deleted function 'operator new'
    C* c = new C();
           ^
cpp_basic/new.cpp:89:15: note: candidate function has been explicitly deleted
        void* operator new(size_t s) = delete;
              ^
cpp_basic/new.cpp:96:5: error: attempt to use a deleted function
    delete c;
    ^
cpp_basic/new.cpp:91:14: note: 'operator delete' has been explicitly marked deleted here
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

# 2 类

## 2.1 class vs struct
C++中struct与class完全等价，只有一处不同：
> struct的成员默认public，class默认private。

除此之外完全相同！

而且在使用struct类型时不需要struct xxx的形式，因此不建议类似C的 typedef 出现。

```cpp

// C style
typedef struct {

} CS;

CS cs;

// C++ style

struct {

} CPP_S;

CPP_S cppS;
```

## 2.2 成员变量


### 2.2.1 forward-declare
成员变量的类型可以是基本类型，指针，引用，或是其他类。当需要使用类类型是我们通常要include对应头文件，但是有时候为了避免头文件相互引用，需要使用forwar-decalred的形式。

```cpp

// forward declared A
class A;

class B {
    A* a;
    A getA();
    void passA(A a);
};
```

上述A是forward-declare的，不需要对应头文件，B就可以使用。但是此时的A是**incomplete-type**，即只有个名字，其他信息一概不知。因此在B的声明中只能声明A的指针或者引用，而不能声明成员为A的值类型（因为此时A的size和内存布局信息一概不知，而指针的内存布局都是统一的）。

另外，可以在成员函数的参数和返回值中使用A的值类型，因为函数的具体实现和定义是分开的，定义函数时不需要A的具体信息。

### 2.2.2 成员初始化——默认构造函数

这部分大家都很清楚了，初始化列表！但是其中还是有不少坑，我们来试试看！

quiz：试着说出下面定义的`struct X`在默认构造之后的各成员的值。

```cpp
struct X0 {
    int a;
    bool b;
    void* c;
};

struct X1 {
    int a;
    // default initializers. (C++11)
    bool b = true;
    void* c;
};

struct X2 {
    int a;
    bool b = true;
    void* c;

    X() : a(1), b(false) {}
};
```

> 结果详见test-case

X0 和 X1都有一个编译器生成的默认构造函数，X2有一个自定义的默认构造函数。因此像`X x;`这样的代码可以编译通过。

X0是一个[standard-layout](https://en.cppreference.com/w/cpp/types/is_standard_layout)类

[**隐式声明的默认构造函数**][default_constructor]: 及编译器生成的默认构造函数，当满足特定条件时才会生成。

[**trivaial default constructor:**][default_constructor]也就是什么都不咋说的构造函数，同样也不会初始化成员变量。

如cppreference所示，上述逻辑比较复杂，即使花时间研究清楚过段时间也很容易忘记，因此建议:

**默认构造函数**
> 全部显式的声明默认构造函数。

**成员变量初始化**
> 要么全部不初始化（交给编译器初始化默认值），要么全部初始化。永远不要初始化部分值。推荐使用 default initializers. (C++11)

```cpp

struct X {
    // 显式声明
    X(): ...
}

struct X {
    // 显示声明，并使用编译器生成的默认构造函数
    X() = default;
}
```

吐槽：C++就是这样，标准看似很严谨，但是有些完全不是人类的思维能力可以熟练使用的，C++更像是给机器写的（说不定N年后AI就会写C++代码了)。。。好在我们可以利用一些技巧避免踩坑。

![IDE_WARNING](https://i.loli.net/2019/11/18/MjVJwsulZUpG8qL.png)
不要忽略IDE的warning。

[default_constructor]: https://en.cppreference.com/w/cpp/language/default_constructor

### 2.2.2 成员初始化——构造析构顺序

quiz：试着说出下列程序中A，B的初始化和析构顺序。

```cpp
struct X {
    A a;
    B b;

    X() : b(), a() {}
};
```

答案：构造 A，B；析构 B，A。

原因：因为构造函数有多个，但是析构函数只能有一个因此成员的析构顺序必须固定，因此推导出了构造的顺序也必须固定。

结论：成员的构造顺序是其在class中声明的顺序，析构顺序是声明顺序的倒序。

建议：初始化列表中的顺序和声明顺序保持一致。

## 2.3 编译器默认生成的函数

Effective C++: Item5
Effective Modern C++: Item

当你在代码中声明一个这样的类的时候，
```cpp
struct X {

};
```
实际发生的情况可能远超出你的想象，他实际上等价于：

```cpp
struct X {
    X() = default; // 1
    ~X() = default; // 2

    X(const X& copy) = default; // 3
    X& operator=(const X& copy) = default; // 4

    X(X&& move) = default; // 5
    X& operator=(X&& move) = default; //6
};
```

不要被吓到，我们一个一个说。

| 函数 | 默认实现 |
| :--- | :--- |
| 1. 默认构造函数 | 逐个调用成员变量默认构造函数（详见上文）|
| 2. 析构函数 | 逆序逐个调用成员变量析构函数 |
| 3. 拷贝构造函数 | 逐个调用成员拷贝构造函数 |
| 4. 赋值操作符 | 逐个调用成员赋值操作 |
| 5. 移动构造函数 | 逐个调用成员移动构造函数 |
| 6. 移动赋值构造函数 | 逐个调用成员移动赋值操作|

quiz: 下列代码分别调用了什么函数？

```cpp
X x1;
X x2(x1);
X x3 = x2;
X x4((X()));
X x5 = X();
X x6(std::move(x2));
X x7 = std::move(x3);
x1 = x7; // 8
x2 = std::move(x7); // 9
```

- x1: 默认构造
- x2: 默认拷贝
- x3: 同上
- x4: 默认构造，因为[NVRO]，实际上只构造一个
- x5: 同上
- x6: 移动构造
- x7: 同上
-  8: 赋值
-  9: 移动赋值

[NVRO]: https://en.cppreference.com/w/cpp/language/copy_elision

**[move]语义概述：**
右值表示当一个临时的变量，通常不再使用时。move语义是基于右值引用的。

`X x = createX()` createX 返回的就是右值，因此调用到移动构造函数。

```cpp
X(X&& move) noexcept {
    // move不再使用，因此”偷走“他的资源，避免内存分配+拷贝
    this->bigMemory = move.bigMemory;
    move.bigMemory = nullptr;
}
```

哪些是右值：
1. 函数返回值
2. 使用 `std::move` 强制转成右值

move是C++11引入的一个重要特性，对性能有很大提升，并在在move的基础上实现了`std::uniqure_ptr`等重要类，建议读者认真了解。

[move]:https://en.cppreference.com/w/cpp/language/move_constructor

## 2.4 继承

## 2.5 继承后的内存布局与static_cast

https://coolshell.cn/articles/12176.html

## 2.6 虚函数
当一个类是多态类时，其多态函数，必需声明为`virtual`才能被子类覆盖。

```cpp
struct Animale {
    virtual void speech() = 0;
    std::string name();
}

struct Gog : public Animale {
    // override
    void speech() override;

    // invalid can't override function
    // std::string name();
}
```
## 2.7 虚析构函数

**多态类的析构函数必需为virtual**

## 2.8 纯虚函数

纯虚函数表示一个函数没有实现，子类必需实现，类似Java的interface，形式如：

```cpp
struct Animale {
    virtual void speech() = 0;
}
```

## 2.9 菱形继承与虚继承

## 2.10 在C++中实现OOP的方式

1. 用虚函数实现多态
2. 用纯虚函数表示接口
3. 尽量避免多继承
4. 可以继承多个纯虚类（类似java实现多个接口）

# 3 值，指针，引用

从Java到C++的程序员通常会被值类型搞得晕头转向，因为Java中只有引用类型（其实对应于C++应该是指针），这使得事件变得简单明了。而C++中的值和引用是的复杂度发生了指数级的上升。

这里我们讨论他们分别作为 函数临时变量、成员、函数参数、函数返回值 下的情况。

## 值
就像int等内置基础类型一样，值类型在使用时是直接构造，拷贝类的。如：

```cpp
TEST(Value, Value) {
    struct C {
        X x;
        X getX() { return x; }
        X& getXRef() { return x; }
        X* getXPtr() { return &x; }
    };

    C c; // 内部调用X默认构造函数

    // X拷贝构造，作用域结束即析构
    X x = c.getX();
    // 无X构造
    X& xr = c.getXRef();
    // 无X构造
    X* p = c.getXPtr();

    // X拷贝构造, hello调用完成即析构
    c.getX().hello();
    // 无X构造
    c.getXRef().hello();
    // 无X构造
    c.getXPtr()->hello();
}
```

值类型使用起来是简单的，因此是比较简单的的，但是需要注意内存拷贝的问题。

上述代码中C内部拥有X的值，因此x是c所独有的，又因为x是值类型，所以会在C析构的时候被默认析构掉。

注意，当C内部持有X的时候，getX推荐返回`X&`，如上getXRef，这样可以避免上面`getX()`调用者创建X的拷贝，一方面是性能问题，另一方面调用者实际调用的并不是同一个x的实例，可能会引起问题。

PS：一个比较完备的写法是：
```cpp
struct C {
    X x;

    X& getX() { return x; }

    const X& getX() const { return x; }
};
```
在C++中`getX()` 和 `getX() const`被视为两个函数，是重载关系。

## 指针

## 引用

| | 成员 | 函数参数 | 函数返回值 | 内存ownerness |
| :-- | :-- | :-- | :-- | :-- |
| 值 | | | | 拥有内存所有权，因为他总是copy一份内存。 |
| 指针 | | | | 不明确(需要文档说明) |
| 引用 | | | | 不具备内存所有权，他只能表示”借用“一块内存。 |

中间说明引用：

```cpp
A& getA();

void printA(A& a);
```

上述getA返回`A&`意味着

**声明成员时，用什么类型考虑顺序**：
1. 值类型，class内部独有，getter返回引用
2. 

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

# prefer STL

string, ttmalloc, ttfree

ttList


# prefer RAII
jni local ref

RenderContext
v8::Scope
