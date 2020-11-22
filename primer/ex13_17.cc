#include "common.h"

class Numbered
{
private:
public:
    int mysn;
    static int unique;

    Numbered();
    Numbered(const Numbered &);
    Numbered &operator=(const Numbered &) = delete;

    ~Numbered() = default;
};

int Numbered::unique = 10;

Numbered::Numbered()
{
    cout << "init constructor" << endl;
    mysn = unique++;
}

Numbered::Numbered(const Numbered &)
{
    cout << "systhesized copy constructor" << endl;
    mysn = unique++;
}

// Numbered &Numbered::operator=(const Numbered &rhs)
// {
//     cout << "systhesized copy-asignment operator" << endl;
//     mysn = rhs.mysn;
//     return *this;
// }

void f(Numbered s)
{
    cout << s.mysn << endl;
}

void f1(Numbered &s)
{
    cout << s.mysn << endl;
}

class Employee
{
private:
    string name_;
    int id_;
    static int unique;

public:
    Employee() = default;
    Employee(string name);
    ~Employee();
};

int Employee::unique = 50;

Employee::Employee(string name)
{
    name_ = name;
    id_ = unique++;
}

Employee::~Employee()
{
}

class HasPtr
{
    friend void swap(HasPtr &, HasPtr &);

private:
    string *ps;
    int i;

public:
    HasPtr() : i(int(0)), ps(new string()) {}
    HasPtr(const HasPtr &rhs) : ps(new string(*rhs.ps)), i(rhs.i) {}
    HasPtr(const string &str) : ps(new string(str)), i(int(0)) {}
    ~HasPtr() { delete ps; }

    HasPtr &operator=(const HasPtr &rhs)
    {
        delete ps;
        string *newPs = new string(*rhs.ps);
        ps = newPs;
        i = rhs.i;
        return *this;
    }
    bool operator<(const HasPtr &rhs)
    {
        return i < rhs.i;
    }

    void swap(HasPtr &rhs)
    {
        using std::swap;
        swap(ps, rhs.ps);
        swap(i, rhs.i);
        cout << "call swap" << endl;
    }
    void show() const
    {
        cout << *ps << endl;
    }
};

inline void swap(HasPtr &lhs, HasPtr &rhs)
{
    using std::swap;
    swap(lhs.ps, rhs.ps);
    swap(lhs.i, rhs.i);
}

class Folder;
class Message;

class Folder
{
    friend Message;
    friend void swap(Folder &lhs, Folder &rhs);

private:
    set<Message *> messages;

    void add_to_Messages(const Folder &);
    void rmv_from_Messages();

public:
    Folder() = default;
    Folder(const Folder &);
    Folder &operator=(const Folder &);

    ~Folder();

    void addMsg(Message *);
    void rmvMsg(Message *);
    void print_debug();
};

void Folder::add_to_Messages(const Folder &f)
{
    for (auto m : f.messages)
    {
        m->add_Folder(this);
    }
}

void Folder::rmv_from_Messages()
{
    for (auto m : messages)
    {
        m->rmv_Folder(this);
    }
}

Folder::Folder(const Folder &f) : messages(f.messages)
{
    add_to_Messages(f);
}

Folder &Folder::operator=(const Folder &f)
{
    rmv_from_Messages();
    messages = f.messages;
    add_to_Messages(f);
    return *this;
}

Folder::~Folder()
{
    rmv_from_Messages();
}

void Folder::addMsg(Message *m)
{
    messages.insert(m);
}

void Folder::rmvMsg(Message *m)
{
    messages.erase(m);
}

void Folder::print_debug()
{
    for (auto m : messages)
        std::cout << m->contents << " ";
    std::cout << std::endl;
}

void swap(Folder &lhs, Folder &rhs)
{
    using std::swap;

    lhs.rmv_from_Messages();
    rhs.rmv_from_Messages();

    swap(lhs.messages, rhs.messages);

    lhs.add_to_Messages(lhs);
    rhs.add_to_Messages(rhs);
}

class Message
{
    friend class Folder;
    friend void swap(Message &lhs, Message &rhs);

private:
    string contents;
    set<Folder *> folders;

    void add_to_Folders(const Message &);
    void rmv_from_Folders();

    void add_Folder(Folder *f) { folders.insert(f); }
    void rmv_Folder(Folder *f) { folders.erase(f); }

public:
    explicit Message(const string &str = "") : contents(str) {}
    Message(const Message &);
    Message &operator=(const Message &);
    ~Message();

    void save(Folder &);
    void remove(Folder &);
    void print_debug();
};

void Message::add_to_Folders(const Message &m)
{
    for (auto f : m.folders)
    {
        f->addMsg(this);
    }
}

Message::Message(const Message &m) : contents(m.contents), folders(m.folders)
{
    add_to_Folders(m);
}

void Message::rmv_from_Folders()
{
    for (auto f : folders)
    {
        f->rmvMsg(this);
    }
}

Message::~Message()
{
    rmv_from_Folders();
}

Message &Message::operator=(const Message &m)
{
    rmv_from_Folders();
    contents = m.contents;
    folders = m.folders;
    add_to_Folders(m);
    return *this;
}

void Message::save(Folder &f)
{
    add_Folder(&f);
    f.addMsg(this);
}

void Message::remove(Folder &f)
{
    rmv_Folder(&f);
    f.rmvMsg(this);
}

void Message::print_debug()
{
    std::cout << contents << std::endl;
}

void swap(Message &lhs, Message &rhs)
{
    using std::swap;

    lhs.rmv_from_Folders();
    rhs.rmv_from_Folders();

    swap(lhs.contents, rhs.contents);
    swap(lhs.folders, rhs.folders);

    lhs.add_to_Folders(lhs);
    rhs.add_to_Folders(rhs);
}

int main(int argc, char const *argv[])
{
    Numbered a, b = a, c = a;
    f1(a);
    f1(b);
    f1(c);
    f(a);
    f(b);
    f(c);
    f1(a);
    f1(b);
    f1(c);

    HasPtr s1{"s"};
    HasPtr a1{"a"};
    HasPtr c1{"c"};
    std::vector<HasPtr> vec{s1, a1, c1};
    std::sort(vec.begin(), vec.end());

    for (auto const &elem : vec)
        elem.show();
    return 0;
    return 0;
}
