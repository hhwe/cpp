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


## 重载 operator new/operator delete

全局的operator new/delete最好不要使用, 会影响所有内存的分配与释放

类内重载需要加上static, 如果不加编译器会自动给加上