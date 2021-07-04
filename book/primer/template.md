# 模板与泛型编程

## 定义模板

模板定义以关键字template开始，后跟一个模板参数列表，这是一个都好分割的一个或多个模板参数的列表。当使用模板是，我们隐式或者显示的指定模板实参，将其绑定到模板参数上。编译器用推断出的模板参数来实例化一个特定版本的函数。

类型参数看作类型说明符，通过关键字class或者typename来指定类型参数。
非类型参数表示一个值而非一个类型，我们通过一个特定类型名而非关键字class或者typename来指定非类型参数。

``` c++
template <typename T>
int compare(const T &v1, const T &v2)
{
    if (v1 < v2)
        return -1;
    // if (v1 > v2) // 
    if (v2 < v1)
        return 1;
    return 0;
}

template <typename T, class U>
calc(const T &, const &U);
```


非类型模板参数，模板实参必须是常量表达式

``` c++
template <unsigned N, unsigned M>
int compare(const char (&p1)[n], const char (&p2)[M])
{
    return strcmp(p1, p2);
}
compare("hi", "mom");
```

- 模板中的函数参数是const的引用。通过const引用，我们保证了函数可以用于不能拷贝的类型，而且这种设计策略还能使函数运行的更快。
- 函数体内的条件判断仅使用<比较运算。编写代码时只使用<运算符，我们就降低了compare函数对处理的类型要求。这些类型必须支持<，但不必同时支持>。

``` c++
template <typename T>
int compare(const T &v1, const T &v2)
{
    if (less<T>()(v1, v2))
        return -1;
    if (less<T>()(v2, v1))
        return 1;
    return 0;
}
```

**模板程序应该尽量减少对实参类型的要求**

**函数模板和类模板成员函数的定义通常放在头文件中**

``` c++
template <typename T> 
class Blob
{
public: 
    typedef T value_type;
    Blob();
    Bloc(std::initializer_list<T>) il);
    BlobPtr& operator++(); // 在一个类模板的作用域内，我们可以直接使用模板名而不必指定模板实参。
private:
    std::shared_ptr<std::vector<T>> data;
}

template <typename T>
ret-type Bloc<T>::member-name(para-list);
```

**对于一个实例化的类模板，其成员只有在使用时才被实例化。**

在一个类模板的作用域内，我们可以直接使用模板名而不必指定模板实参。

类模板与另一个模板间友好关系的最常见形式是建立对应实列及其友元间友好关系。列如blob类应该将BlobPtr类和一个模板版本的Blob相等运算符定义为友元。

**一个特定文件所需要的所有模板的声明通常一起放置在文件开始位置，出现于任何使用这些模板的代码之前**

当我们希望通知编译器一个名字表示类型时，必须使用关键字typename，而不能使用class

``` c++
template <typename T， typename F = less<T>>
int compare(const T &v1, const T &v2, F f = F())
{
    if (f(v1 < v2))
        return -1;
    // if (v1 > v2) // 
    if (f(v2 < v1))
        return 1;
    return 0;
}

template <typename T, class U>
calc(const T &, const &U);

tmpelate <class T = int> 
class Numbers 
{
public:
    Numbers(T v = 0): val(v) {}
private:
    T val;
}
Numbers<> average_precision;
```

控制实例化，当模板被使用时才会进行实例化这一特性意味着，相同的实例可能出现在多个对象文件中，每个文件中都会有该模板的一个实例。可以通过显示实例化来避免这种开销

``` c++
extern template class Blob<string> // 声明
template int compare(const int &, const int &); // 定义
```

在一个类模板的实例化定义中，所有类型必须你呢惯用语模板的所有成员函数。

和其他类型一样，数组形参可声明为数组的引用。如果形参是数组的引用，编译器不会将数组实参转化为指针，而是传递数组的引用本身。在这种情况下，数组大小成为形参和实参类型的一部分。编译器检查数组的实参的大小与形参的大小是否匹配：



``` c++
// 显示模板参数
template <typename T1, typename T2, typename T3>
T1 sum(T2, T3);

// 调用显示指定T1的类型，T2和T3时则由编译器从i、j推断出来。显示版面实参按由左至右的顺序与对应模板参数皮
auto val3 = sum<long long>(i, j); // long long sum(int, long);

template <typename T> T fobj(T,T);
template <typename T> T freb(const &T,const &T);
 
int a[20],b[42];
 
// 入参时const &，数组无法退化成指针，
fobj(a,b);  //ok
freb(a,b);  //error: array types can't match, arguments aren't converted to pointers.

// 尾置返回，参数列表之后声明返回类型
tmpelate <typename It>
auto fcn(It beg, It end) -> decltype(*beg)
{
    return *beg;
}

tmpelate <typename It>
auto fcn(It beg, It end) -> typename remove_reference<decltype(*beg)>::type
{
    return *beg;
}
```

当参数是一个函数模板实例的地址时，程序上下文必须满足：对每个模板参数，能唯一确定其类型或值。

模板实参推断和引用

``` c++
// 尾置返回，参数列表之后声明返回类型
tmpelate <typename T>
void f1(T&);
f1(i);
f1(ci);
f1(5); // error
```

C++引用—临时变量、引用参数和const引用和左值右值引用

const修饰左值引用
int & r = val + 1; //此句不合法，因为右值无法赋值给左值引用
const int& r = val + 1；//合法

右值引用
在上面的代码中，我们无法建立 int &rb = a + 1; 这样的语法，因为a + 1 此时是作为一个右值来使用的，我们无法把一个右值赋值给一个左值引用。（也就是左值引用相当于把一个变量的地址付给另一个变量，这两个变量可以访问同一个内存，右值仅仅是一个数，而非内存中的某块地址，因此无法把右值复制给左值引用）。

声明方法：Type && 右值引用名 = 右值表达式；

std::move()的用法
可以直接把左值或者右值转换成右值引用，使用方法：

int && rrval = std::move(val);

但是这里需要注意：在调用完std::move之后，不能再使用val，只能使用 rrval，这一点用于基本类型可能没什么直接影响，当应用到类函数的时候，用好std::move 可以减少构造函数数的次数

右值引用
为了解决移动语义及完美转发问题，C++11标准引入了右值引用（rvalue reference）这一重要的新概念。右值引用采用T&&这一语法形式，比传统的引用T&（如今被称作左值引用 lvalue reference）多一个&。
如果把经由T&&这一语法形式所产生的引用类型都叫做右值引用，那么这种广义的右值引用又可分为以下三种类型：

- 无名右值引用
- 具名右值引用
- 转发型引用

无名右值引用和具名右值引用的引入主要是为了解决移动语义问题。
转发型引用的引入主要是为了解决完美转发问题。

### 无名右值引用

无名右值引用（unnamed rvalue reference）是指由右值引用相关操作所产生的引用类型。
无名右值引用主要通过返回右值引用的类型转换操作产生， 其语法形式如下：
static_cast<T&&>(t)
标准规定该语法形式将把表达式 t 转换为T类型的无名右值引用。
无名右值引用是右值，标准规定无名右值引用和传统的右值一样具有潜在的可移动性，即它所占有的资源可以被移动（窃取）。

 std::move()

由于无名右值引用是右值，借助于类型转换操作产生无名右值引用这一手段，左值表达式就可以被转换成右值表达式。为了便于利用这一重要的转换操作，标准库为我们提供了封装这一操作的函数，这就是std::move()。
假设左值表达式 t 的类型为T&，利用以下函数调用就可以把左值表达式 t 转换为T类型的无名右值引用（右值，类型为T&&）。
std::move(t)

### 具名右值引用

如果某个变量或参数被声明为T&&类型，并且T无需推导即可确定，那么这个变量或参数就是一个具名右值引用（named rvalue reference）。
具名右值引用是左值，因为具名右值引用有名字，和传统的左值引用一样可以用操作符&取地址。
与广义的右值引用相对应，狭义的右值引用仅限指具名右值引用。
传统的左值引用可以绑定左值，在某些情况下也可绑定右值。与此不同的是，右值引用只能绑定右值。
右值引用和左值引用统称为引用（reference），它们具有引用的共性，比如都必须在初始化时绑定值，都是左值等等。

struct X {};

X a;

X&& b = static_cast<X&&>(a);

X&& c = std::move(a);

//static_cast<X&&>(a) 和 std::move(a) 是无名右值引用，是右值

//b 和 c 是具名右值引用，是左值

X& d = a;

X& e = b;

const X& f = c;

const X& g = X();

X&& h = X();

//左值引用d和e只能绑定左值（包括传统左值：变量a以及新型左值：右值引用b）

//const左值引用f和g可以绑定左值（右值引用c），也可以绑定右值（临时对象X()）
//右值引用b，c和h只能绑定右值（包括新型右值：无名右值引用std::move(a)以及传统右值：临时对象X()
