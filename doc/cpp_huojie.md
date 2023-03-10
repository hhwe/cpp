# 侯捷_c++面向对象高级编程

## return by reference

如果在函数调用之前已有空间保存返回值, 优先使用reference

## typename() 临时变量

``` c++
class A
{};

A a1;
A a1();
A(); // temp
```

临时变量的语法类似 `new/new []`


## 不改变实例的操作添加const

``` c++
class Complex 
{
    Complex operator+(double x) { // error: passing ‘const Complex’ as ‘this’ argument discards qualifiers [-fpermissive]
        return Complex(re + x, im);
    }
}
```

(Complex+Complex)的逻辑是将两个对象相加赋值给另一个对象，两个对象本身不应该改变，需要加上const

``` c++
class Complex 
{
    Complex operator+(double x) const {
        return Complex(re + x, im);
    }
}
```


## 部分操作符不能作为成员函数

operator+

``` c++
Complex c1,c2,c3;
c3 = 2.5 + c1; // error: no match for ‘operator+’ (operand types are ‘double’ and ‘const Complex’)
```

因为如果用友元全局函数的话，第一个参数也可以接受隐式类型转换构造Complex临时对象的传入方式； 
可是如果是类内置型操作符重载的话，编译器只会根据第一个参数的实际类型查找其对应的成员函数，看其是否符合匹配。 
比如第一个参数是2.5，则编译器只会查找double::operator +（显然这里double只是个内置类型，不可能包含成员）。而不会考虑将double隐式转换成其他类型了


operator<<、operator>>

``` c++
Complex c1,c2,c3;
cout <<c1;
c1 << cout; // error
```

如果是成员函数的话，使用方式是`c1 << cout`违背数据流使用常理，`<<`\`>>`必须左边是iostream类型 


## 类的实例互为friend

``` c++
class A
{
public:
    void F(const A& a) {
        a = a.pr; // 相同类型的对象互为friend
    }
private:
    int pr;
};
```



## 转换函数

``` c++
class Fraction {
public:
    Fraction(int num, int den = 1) :
        numerator(num), denominator(den) {
    }
    operator double() const {
        return (double)numerator / denominator;
    }

private:
    int numerator;
    int denominator;
};

Fraction a(3, 5);
double f = 4 + a; // 
```

可以通过 `operator type()` 实现转换其他类型, 通过 `no-expilict-one-argument-ctor`的构造方式将其他类型转换成本类型

但是注意互相转换可能会引起歧义(ambiguous)

## pointer-like class

1. 智能指针
2. 迭代器


## template template parameter

如果我们想要允许指定存储Stack元素的容器，是这么做的：
``` c++
template <typename T, typename Cont = std::deque<T>> 
class Stack {
private:
  Cont elems; // elements
  ......
};
Stack<double, std::vector<double>> dblStack;
```

但是这样的缺点是需要指定元素类型两次，然而这两个类型是一样的。

使用模板的模板参数（Template Template Parameters），允许我们在声明Stack类模板的时候只指定容器的类型而不去指定容器中
元素的类型。例如：
``` c++
template <typename T, template <typename> typename SmartPtr>
class XCls {
public:
    XCls() :
        sp(new T) {
    }

private:
    SmartPtr<T> sp;
};
XCls<std::string, std::shared_ptr> p;
```


## variadic templates

``` c++
void print() {
}

template <typename T, typename... Types>
void print(const T& firstArg, const Types&... args) {
    std::cout << sizeof...(args) << std::endl;
    std::cout << firstArg << std::endl;
    print(args...);
}
```

## override signature

函数重载签名是根据`函数名+参数(忽略参数内const等限定符)+const(成员函数)`
``` c++
class T
{
    operator[](int pos) const {} // 不必考虑COW(copy on write)
    operator[](int pos) {} // 需要考虑COW(copy on write), no-const object优先调用
}
```

## this

所有成员函数都包含一个默认的参数`this`, Template Method就是使用这种

## vptr/vtbl 多态 动态绑定 

多态: 指针,up-cast,虚函数,

通过动态绑定来实现, 通过对象指针找到虚指针, 再通过虚指针找到最终调用的虚函数


# 内存管理

## new/delete/new []/delete []/new()

必需成对使用, 否则如果new[]直接使用delete, 只会调用一次析构, 而不会对每个对象执行析构, 导致内存泄漏

``` c++
// new
string* ps = new string("hello");
==>>
void* mem = operator new(sizeof("hello")); // 申请内存, operator new内部调用malloc(n)
ps = static_cast<string*>(mem); // 转型
ps->string::string("hello"); // 构造函数

// placement new
string* pc = new(pc) string("placement new");
==>>
void* mem = operator new(sizeof("hello"), pc); // 这个重载函数不申请内存, 直接返回传入的pc
ps = static_cast<string*>(mem); // 转型
ps->string::string("hello"); // 构造函数

// delete
delete ps;
==>>
string::~string(ps); // 析构
operator delete(ps); // 释放内存, operator delete内部调用free(ps)

```

由于 `new/delete` 是表达式,所以无法重载,但是可以重载内部的操作符 `operator new/operator delete`

operator new就像operator+一样，是可以重载的，但是不能在全局对原型为void operator new(size_t size)这个原型进行重载，一般只能在类中进行重载。如果类中没有重载operator new，那么调用的就是全局的::operator new来完成堆的分配。同理，operator new[]、operator delete、operator delete[]也是可以重载的，一般你重载了其中一个，那么最好把其余三个都重载一遍。

placement new是operator new的一个重载版本，只是我们很少用到它。如果你想在已经分配的内存中创建一个对象，使用new是不行的。也就是说placement new允许你在一个已经分配好的内存中（栈或堆中）构造一个新的对象。原型中void*p实际上就是指向一个已经分配好的内存缓冲区的的首地址。

我们知道使用new操作符分配内存需要在堆中查找足够大的剩余空间，这个操作速度是很慢的，而且有可能出现无法分配内存的异常（空间不够）。placement new就可以解决这个问题。我们构造对象都是在一个预先准备好了的内存缓冲区中进行，不需要查找内存，内存分配的时间是常数；而且不会出现在程序运行中途出现内存不足的异常。所以，placement new非常适合那些对时间要求比较高，长时间运行不希望被打断的应用程序。

## 访问权限

- public: 能被类成员函数、子类函数、友元访问，也能被类的对象访问。
- private: 只能被类成员函数及友元访问，不能被其他任何访问，本身的类对象也不行。
- protected: 只能被类成员函数、子类函数及友元访问，不能被其他任何访问，本身的类对象也不行。

## new运算符和operator new()

new：指我们在C++里通常用到的运算符，比如A* a = new A;  对于new来说，有new和::new之分，前者位于std

operator new()：指对new的重载形式，它是一个函数，并不是运算符。对于operator new来说，分为全局重载和类重载，全局重载是void* ::operator new(size_t size)，在类中重载形式 void* A::operator new(size_t size)。还要注意的是这里的operator new()完成的操作一般只是分配内存，事实上系统默认的全局::operator new(size_t size)也只是调用malloc分配内存，并且返回一个void*指针。而构造函数的调用(如果需要)是在new运算符中完成的

### new和operator new之间的关系

A* a = new A；我们知道这里分为两步：1.分配内存，2.调用A()构造对象。事实上，分配内存这一操作就是由operator new(size_t)来完成的，如果类A重载了operator new，那么将调用A::operator new(size_t )，如果没有重载，就调用::operator new(size_t )，全局new操作符由C++默认提供。因此前面的两步也就是：1.调用operator new 2.调用构造函数。
（1）new ：不能被重载，其行为总是一致的。它先调用operator new分配内存，然后调用构造函数初始化那段内存。

new 操作符的执行过程：
1. 调用operator new分配内存 ；
2. 调用构造函数生成类对象；
3. 返回相应指针。

（2）operator new：要实现不同的内存分配行为，应该重载operator new，而不是new。

operator new就像operator + 一样，是可以重载的。如果类中没有重载operator new，那么调用的就是全局的::operator new来完成堆的分配。同理，operator new[]、operator delete、operator delete[]也是可以重载的。

### 只能建立在堆上（设置析构函数为Protected）

类对象只能建立在堆上，就是不能静态建立类对象，即不能直接调用类的构造函数。

容易想到将构造函数设为私有。在构造函数私有之后，无法在类外部调用构造函数来构造类对象，只能使用new运算符来建立对象。然而，前面已经说过，new运算符的执行过程分为两步，C++提供new运算符的重载，其实是只允许重载operator new()函数，而operator()函数用于分配内存，无法提供构造功能。因此，这种方法不可以。

当对象建立在栈上面时，是由编译器分配内存空间的，调用构造函数来构造栈对象。当对象使用完后，编译器会调用析构函数来释放栈对象所占的空间。编译器管理了对象的整个生命周期。如果编译器无法调用类的析构函数，情况会是怎样的呢？比如，类的析构函数是私有的，编译器无法调用析构函数来释放内存。所以，编译器在为类对象分配栈空间时，会先检查类的析构函数的访问性，其实不光是析构函数，只要是非静态的函数，编译器都会进行检查。如果类的析构函数是私有的，则编译器不会在栈空间上为类对象分配内存。

因此，将析构函数设为私有，类对象就无法建立在栈上了。代码如下：

``` c++
class A  
{  
public:  
    A(){}  
    void destory(){delete this;}  
private:  
    ~A(){}  
};  
```

试着使用A a;来建立对象，编译报错，提示析构函数无法访问。这样就只能使用new操作符来建立对象，构造函数是公有的，可以直接调用。类中必须提供一个destory函数，来进行内存空间的释放。类对象使用完成后，必须调用destory函数。

上述方法的一个缺点就是，无法解决继承问题。如果A作为其它类的基类，则析构函数通常要设为virtual，然后在子类重写，以实现多态。因此析构函数不能设为private。还好C++提供了第三种访问控制，protected。将析构函数设为protected可以有效解决这个问题，类外无法访问protected成员，子类则可以访问。

另一个问题是，类的使用很不方便，使用new建立对象，却使用destory函数释放对象，而不是使用delete。（使用delete会报错，因为delete对象的指针，会调用对象的析构函数，而析构函数类外不可访问）这种使用方式比较怪异。为了统一，可以将构造函数设为protected，然后提供一个public的static函数来完成构造，这样不使用new，而是使用一个函数来构造，使用一个函数来析构。代码如下，类似于单例模式：

``` c++
class A  
{  
protected:  
    A(){}  
    ~A(){}  
public:  
    static A* create()  
    {  
        return new A();  
    }  
    void destory()  
    {  
        delete this;  
    }  
};  
```

这样，调用create()函数在堆上创建类A对象，调用destory()函数释放内存。

### 只能建立在栈上(重载new函数设为私有)

只有使用new运算符，对象才会建立在堆上，因此，只要禁用new运算符就可以实现类对象只能建立在栈上。将operator new()设为私有即可。代码如下：
``` c++
class A  
{  
private:  
    void* operator new(size_t t){}     // 注意函数的第一个参数和返回值都是固定的  
    void operator delete(void* ptr){} // 重载了new就需要重载delete  
public:  
    A(){}  
    ~A(){}  
};
```

## 重载 operator new/operator delete

全局的operator new/delete最好不要使用, 会影响所有内存的分配与释放

类内重载需要加上static, 如果不加编译器会自动给加上

``` c++

```


## embedded pointer

到现在看来，简单的空闲链表的主要开销就是一个next指针域，在32位系统上是4个字节。可是如果我们的CMyObject本来就很小，只有4个字节,那额外开销可就是100%阿，如果系统中用到几万，十几万，甚至上百万个CMyObject，就不容小视了！
我们仔细来分析一下刚才的例子，什么时候会用到next指针呢？
1. 第一次分配对象，建立整个freeStore的链表的时候，初始化各个实例的next指针
2. 每次给申请者一个实例时，要修改freeStore = freeStore->next
3. 每次归还一个实例时，链表插入操作要用到next.
这里我们的一个观察是，next指针的使用都是在申请和归还的时候，也就是说申请者在使用这块内存的时候从来都不需要next！既然next指针所占的4个字节不会被同时使用，那我们是不是可以想一个办法，让使用时和申请/归还重用同一块内存呢。感谢C语言的union，给了我们实现的可能性。

## VC6下内存管理

### SBH

16个Header

32个Group

64个FreeList


# STL源码剖析

## 一、概述

### 1. 准备

- C++编译器，最好是GCC
- SGI STL的源码：http://www.sgi.com/tech/stl/download.html
- C++基础
- 少量STL使用经验
- 少量算法和数据结构基础

### 2. STL简介

STL版本

STL是一个标准，只规定了STL的接口，内部实现没有要求。STL有许多实现版本，PJ STL（被Visual C++采用），RW STL等。

SGI STL版本注释丰富，结构清晰，可读性最强，同时它也被GCC采用，所以是最流行的版本。

注意，SGI STL并不是原封不动的被用于GCC，所以在GCC中使用STL可能会和SGI STL有一些微小的区别。

STL组件

STL分为六大组件：

- **容器(container)**：常用数据结构，大致分为两类，序列容器，如vector，list，deque，关联容器，如set，map。在实现上，是类模板(class template)
- **迭代器(iterator)**：一套访问容器的接口，行为类似于指针。它为不同算法提供的相对统一的容器访问方式，使得设计算法时无需关注过多关注数据。（“算法”指广义的算法，操作数据的逻辑代码都可认为是算法）
- **算法(algorithm)**：提供一套常用的算法，如sort，search，copy，erase … 在实现上，可以认为是一种函数模板(function template)。
- **配置器(allocator)**：为容器提供空间配置和释放，对象构造和析构的服务，也是一个class template。
- **仿函数(functor)**：作为函数使用的对象，用于泛化算法中的操作。
- **配接器(adapter)**：将一种容器修饰为功能不同的另一种容器，如以容器vector为基础，在其上实现stack，stack的行为也是一种容器。这就是一种配接器。除此之外，还有迭代器配接器和仿函数配接器。

### 3. STL文件概览

这是部分STL文件的包含关系：

![stl文件关系](pic/mystl/file_relationship.jpg)

大约可以将文件分三类：

- STL标准头文件（无文件后缀），如vector, deque, list, map, algorithm, functional ...
- SGI STL内部文件（STL的真正实现），如`stl_vector.h`, `stl_deque.h`, `stl_list.h`, `stl_map.h`, `stl_algo.h`, `stl_function.h` ...
- `HP规范的头文件，如vector.h`, `deque.h`, `list.h`, `map.h`, `algo.h`, `function.h`
第三类通常不推荐使用，所以我们不用考虑，可以认为它的功能与第一类文件一样（实际有区别）。

此图只为厘清STL内部的结构，图中只包含了上一小节的前两部分文件（也是部分的），即STL标准头文件（不带后缀的）和STL内部文件。

#### 空间配置器(allocator)

图中左上角为空间配置器，通常我们可以在代码中用"#include <memory>"来包含它，它的实际实现在三个文件中：

- `stl_construct.h`: 对象的构造和析构
- `stl_alloc.h`: 空间的配置和释放
- `stl_uninitialized.h`: 内存基本工具，用于提升性能
注意看所有蓝色箭头，所有的容器都直接或间接的包含了allocator，说明所有的容器都依赖allocator来管理空间。

#### 容器(container)

图中不带后缀的文件就是标准头文件，比如vector；图中带后缀的就是标准头文件对应的实现文件，如stl_vector.h；容器vector的真正实现在stl_vector.h中。可以看到，大多数文件都有对应的关系：deque对应stl_queue.h，stack对应stl_stack ...

容器大致分为序列容器和关联容器，序列容器即逻辑上是序列化访问的容器，关联容器即通过键值对访问的容器。

你会发现，序列容器queue和序列容器stack对应了不止一个文件stl_XXX.h文件，这是因为它们是一种配接器，是基于其他容器实现的。

关联容器中，set和map，即集合与映射，set和map的底层基于红黑树实现（stl_tree.h文件），查询时间复杂度为O(log(n))，空间复杂度为O(n)。

hash_set和hash_map的底层基于哈希表实现（stl_hashtable.h文件），查询时间复杂度为O(1)，空间复杂度比set和map高。

#### 算法(algorithm)

大多数容器或多或少包含了算法文件，这是因为许多容器内部的实现会用到一些基本算法。

通常我们用"#include <algorithm>"来使用STL内部的算法，其背后的实现在stl_algobase.h（基本算法）和stl_algo.h（其他算法）。

numeric为数值型算法。

#### 迭代器(iterator)

你肯定会问迭代器去哪儿了。

在STL中，迭代器不是单独实现的（虽然有iterator头文件，但他只是用于实现一些特殊的迭代器），迭代器向用户和算法提供统一的数据访问方式，理应集中实现。但是各种容器千差万别，迭代器的内部实现对容器依赖较强，所以迭代器由每个容器自身包含并实现，对外开放统一的接口。

所以迭代器的实现不会存在于单独的文件中。

### 5. 最后

虽然本篇没有一行代码，仅仅是对STL的概览，并建立初步认识，但是依然要求有一定的数据结构基础，C++基础和一些STL使用经验，否则在阅读之后的文章中会同时面临许多问题。


## 二、源码分析

### 10、技术基础

泛化：模板实现 
特化：模板参数指定具体类型
偏特化：个数或者范围偏特化，局部模板参数特化或者特定模板参数带修饰（const/*/&等)


``` c++
// 泛化
template <typename T, typename Allocator_T>
class MyVector {
public:
    MyVector() {
        std::cout << "Normal version." << std::endl;
    }
};
// 特化
template <>
class MyVector<int, DefaultAllocator> {
public:
    MyVector() {
        std::cout << "Partial version." << std::endl;
    }
};
// 偏特化
template <typename T>
class MyVector<T, DefaultAllocator> {
public:
    MyVector() {
        std::cout << "Partial version." << std::endl;
    }
};
// 偏特化
template <typename T, typename Allocator_T>
class MyVector<T*, const _T*> {
public:
    MyVector() {
        std::cout << "Partial version." << std::endl;
    }
};

MyVector<int, MyAnotherAllocator> v1;
MyVector<> v2;
MyVector<int, DefaultAllocator> v3;
MyVector<int*, const DefaultAllocator*> v3;
```

### 11、分配器-allocator

allocator.allocate -> ::operator new() -> malloc;
allocator.deallocate -> ::operator delete();

gcc 9 取消了
__pool_alloc：可以实现多个元素依次分配，节约malloc分配的管理内存

### 12、

### 13、 list

list只保留了一份指针

iterator操作中pre++和post++不一样，且返回类型不一样
self operator++(_Tp) // 后置++，返回对象，无法对原有对象进行操作
self& operator++()  // 前置++，返回引用，可以继续进行++操作

### 15、iterator

algrothm >> iterator >> container
算法提问，iterator回答

iterator一般包含5种特性
1. pointer
2. reference
3. value_type
4. difference_type
5. iterator_category

还需要萃取器来辅助区分class iterators和non-class iterators（比如指针，一种退化iterator），

这个就是iterator traits，在STL中可以使用偏特化特性来实现


### 16、

### 18、19、deque/queue/stack

deque内部实际分段连续
insert()会判断当前要插入元素离头端比较近还是尾端比较近，来选择插入

queue\stack 默认基于deque实现，也可以使用其他容器来实现_Sequence，可以理解为container adapter


### 20、RB tree

RB-Tree高度平衡二分查找树，提供两个函数insert_unique()和insert_equal()


### 21、set/multiset，map/multimap

set/multiset元素的key和value合一：value就是key

无法时使用iterator修改key和value修改，内部使用const_iterator禁止修改key、value，也是一种container adapter，内部都是使用rb_tree实现

map/multimap中，key和value不合一，无法使用iterator修改key但是可以修改data，内部自动将key_type设为const便可禁止修改key，还提供operator[]来实现插入一个元素的动作

### 23、hashtable

### 28、iterator分类

``` c++
  ///  Marking input iterators.
  struct input_iterator_tag { };

  ///  Marking output iterators.
  struct output_iterator_tag { };

  /// Forward iterators support a superset of input iterator operations.
  struct forward_iterator_tag : public input_iterator_tag { };

  /// Bidirectional iterators support a superset of forward iterator
  /// operations.
  struct bidirectional_iterator_tag : public forward_iterator_tag { };

  /// Random-access iterators support a superset of bidirectional
  /// iterator operations.
  struct random_access_iterator_tag : public bidirectional_iterator_tag { };
```

两个特殊的迭代器分类：ostream_iterator\istream_itertor