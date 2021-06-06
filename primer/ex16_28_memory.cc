#include "common.h"

template <typename T>
class SharedPtr
{
    using DelFuncPtr = void (*)(T *);

public:
    SharedPtr(T *ptr = nullptr, DelFuncPtr del = nullptr)
        : ptr_(ptr), count_ptr_(new size_t(ptr != nullptr)), del_(del)
    {
    }

    ~SharedPtr()
    {
        if (!ptr_)
            return;
        if (--*count_ptr_ == 0)
        {
            del_ ? del_(ptr_) : delete ptr_;
            delete count_ptr_;
        }
        ptr_ = nullptr;
        count_ptr_ = nullptr;
    }

    SharedPtr(const SharedPtr &sp) : ptr_(sp.ptr_), count_ptr_(sp.count_ptr_), del_(sp.del_)
    {
        ++*count_ptr_;
    }

    SharedPtr &operator=(SharedPtr sp)
    {
        swap(sp);
        return *this;
    }

    SharedPtr(const SharedPtr &&sp) noexcept : SharedPtr() { swap(sp); }

    void reset(T *ptr = nullptr, DelFuncPtr del = nullptr)
    {
        SharedPtr tmp(ptr, del);
        swap(tmp);
    }

    void swap(SharedPtr &r) noexcept
    {
        using std::swap;
        swap(ptr_, r.ptr_);
        swap(count_ptr_, r.count_ptr_);
        swap(del_, r.del_);
    }

    T *get() const noexcept { return ptr_; }
    T &operator*() const noexcept { return *get(); }
    T *operator->() const noexcept { return get(); }
    size_t use_count() const noexcept { return *count_ptr_; }
    explicit operator bool() const noexcept { return ptr_ != nullptr; }

private:
    T *ptr_ = nullptr;
    size_t *count_ptr_ = nullptr;
    DelFuncPtr del_ = nullptr;
};

class Delete
{
public:
    template <typename T>
    void operator()(T *ptr) const { delete ptr; }
};

template <typename T, typename D = Delete>
class UniquePtr
{
public:
    UniquePtr(T *ptr = nullptr, const D &d = D()) noexcept : ptr_(ptr), deleter_(d) {}
    ~UniquePtr() { deleter_(ptr_); }

    UniquePtr(const UniquePtr &) = delete;
    UniquePtr &operator=(const UniquePtr &) = delete;

    UniquePtr(UniquePtr &&other) noexcept : ptr_(other.ptr_), deleter_(std::move(other.deleter_))
    {
        other.ptr_ = nullptr;
    }

    UniquePtr &operator=(UniquePtr &&other) noexcept
    {
        if (this != &other)
        {
            reset();
            ptr_ = other.ptr_;
            deleter_ = std::move(other.deleter_);
            other.ptr_ = nullptr;
        }
        return *this;
    }

    UniquePtr &operator=(std::nullptr_t) noexcept
    {
        reset();
        return *this;
    }

    T *release() noexcept
    {
        T *ret = ptr_;
        ptr_ = nullptr;
        return ret;
    }

    void reset(T *q = nullptr) noexcept
    {
        deleter_(q);
        ptr_ = q;
    }

    void swap(UniquePtr &other) noexcept
    {
        using std::swap;
        swap(ptr_, other.ptr_);
        swap(deleter_, other.deleter_);
    }

    T *get() const noexcept { return ptr_; }
    D &get_deleter() noexcept { return deleter_; }
    const D &get_deleter() const noexcept { return deleter_; }
    explicit operator bool() const noexcept { return ptr_ != nullptr; }
    T &operator*() const { return *ptr_; }
    T *operator->() const noexcept { return ptr_; }
    T &operator[](size_t i) const { return ptr_[i]; }

private:
    T *ptr_ = nullptr;
    D deleter_;
};

struct Foo
{
    Foo() { std::cout << "Foo()\n"; }
    ~Foo() { std::cout << "~Foo()\n"; }
    Foo(const Foo &) { std::cout << "Foo copy ctor\n"; }
    Foo(Foo &&) { std::cout << "Foo move ctor\n"; }
};

struct Fooo
{
    Fooo(int n = 0) noexcept : bar(n) { std::cout << "Fooo: constructor, bar = " << bar << '\n'; }
    ~Fooo() { std::cout << "Fooo: destructor, bar = " << bar << '\n'; }
    int GetBar() const noexcept { return bar; }

private:
    int bar;
};

struct D
{
    void bar() { std::cout << "Call deleter D::bar()...\n"; }
    void operator()(Foo *p) const
    {
        std::cout << "Call delete from function object...\n";
        delete p;
    }
};

// function-object class that calls delete on a given pointer
class DebugDelete
{
public:
    DebugDelete(const std::string &s,
                std::ostream &strm = std::cerr) : os(strm), type(s) {}
    // as with any function template, the type of T is deduced by the compiler
    template <typename T>
    void operator()(T *p) const
    {
        os << "deleting " << type << std::endl;
        delete p;
    }
    DebugDelete() : os(cout), type("") {}

private:
    std::ostream &os; // where to print debugging info
    std::string type; // what type of smart pointer we're deleting
};

int main()
{
    std::cout << "SharedPtr constructor with no managed object\n";
    {
        SharedPtr<Foo> sh1;
    }

    std::cout << "SharedPtr constructor with object\n";
    {
        SharedPtr<Foo> sh2(new Foo);
        std::cout << sh2.use_count() << '\n';
        SharedPtr<Foo> sh3(sh2);
        std::cout << sh3.use_count() << '\n';
    }

    std::cout << "SharedPtr constructor with deleter\n";
    {
        SharedPtr<Foo> sh5(new Foo, [](Foo *p) {
            std::cout << "Call delete from lambda...\n";
            delete p;
        });
    }

    {
        SharedPtr<Fooo> sptr(new Fooo(1));
        std::cout << "The first Fooo's bar is " << sptr->GetBar() << "\n";
        sptr.reset(new Fooo);
        std::cout << "The second Fooo's bar is " << sptr->GetBar() << "\n";
    }

    {
        std::cout << "======================\nUniquePtr constructor:\n";
        UniquePtr<Foo> up1;
        UniquePtr<Foo> up1b(nullptr);
        UniquePtr<Foo> up2(new Foo);

        DebugDelete d;
        UniquePtr<Foo, DebugDelete> up3(new Foo, d);
        UniquePtr<Foo, DebugDelete &> up3b(new Foo, d);
        UniquePtr<Foo, DebugDelete> up4(new Foo, DebugDelete());
        UniquePtr<Foo> up5b(std::move(up2));
        UniquePtr<Foo, DebugDelete> up6b(std::move(up3));

        UniquePtr<Foo> up7 = std::move(up5b);
        Foo *fp = up7.release();
        assert(up7.get() == nullptr);
        delete fp;

        up6b.reset(new Foo());
        up6b.reset(nullptr);

        UniquePtr<Fooo> up71(new Fooo(1));
        UniquePtr<Fooo> up72(new Fooo(2));

        up71.swap(up72);

        std::cout << "up71->val:" << up71->GetBar() << std::endl;
        std::cout << "up72->val:" << (up72.get())->GetBar() << std::endl;

        UniquePtr<Foo, D> up8(new Foo(), D());
        D &del = up8.get_deleter();
        del.bar();
    }
}
