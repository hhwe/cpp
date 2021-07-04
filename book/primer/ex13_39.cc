#include "common.h"

class String
{
private:
    string *elements;
    string *first_string;
    allocator<string> alloc;

public:
    String(/* args */);
    ~String();
};

String::String(/* args */)
{
}

String::~String()
{
}

class StrVec
{
private:
    void chk_n_alloc()
    {
        if (size() == capacity())
            reallocate();
    }
    void free();
    void reallocate();
    void alloc_n_move(size_t n);
    pair<string *, string *> alloc_n_copy(const string *, const string *);
    void range_initialize(const string *begin, const string *end);

private:
    string *elements;
    string *first_free;
    string *cap;
    allocator<string> alloc;

public:
    StrVec() : elements(nullptr), first_free(nullptr), cap(nullptr) {}
    StrVec(const StrVec &);
    StrVec(initializer_list<string>);
    StrVec &operator=(const StrVec &);
    ~StrVec();

    void push_back(const string &);
    size_t size() const { return first_free - elements; }
    size_t capacity() const { return cap - elements; }
    string *begin() const { return elements; }
    string *end() const { return first_free; }

    string &at(size_t pos) { return *(elements + pos); }
    const string &at(size_t pos) const { return *(elements + pos); }

    void reserve(size_t n);
    void resize(size_t n);
    void resize(size_t n, const string &s);
};

// // 静态成员必须在定义类的文件中对静态成员变量进行初始化，否则会编译出错。
// allocator<string> StrVec::alloc = 0;

void StrVec::push_back(const string &s)
{
    chk_n_alloc();
    alloc.construct(first_free++, s);
}

pair<string *, string *> StrVec::alloc_n_copy(const string *b, const string *e)
{
    auto data = alloc.allocate(e - b);
    return {data, uninitialized_copy(b, e, data)};
}

void StrVec::free()
{
    if (elements)
    {
        // for (auto p = first_free; p != elements;)
        // {
        //     alloc.destroy(--p);
        // }
        for_each(elements, first_free, [this](string &rhs) { alloc.destroy(&rhs); });
        alloc.deallocate(elements, cap - elements);
    }
}

void StrVec::range_initialize(const string *begin, const string *end)
{
    auto newPair = alloc_n_copy(begin, end);
    elements = newPair.first;
    first_free = cap = newPair.second;
}

StrVec::StrVec(const StrVec &s)
{
    range_initialize(s.begin(), s.end());
}

StrVec::StrVec(initializer_list<string> li)
{
    range_initialize(li.begin(), li.end());
}

StrVec::~StrVec()
{
    free();
}

StrVec &StrVec::operator=(const StrVec &s)
{
    auto newPair = alloc_n_copy(s.begin(), s.end());
    free();
    elements = newPair.first;
    first_free = cap = newPair.second;
    return *this;
}

void StrVec::alloc_n_move(size_t n)
{
    auto newData = alloc.allocate(n);
    auto dest = newData;
    auto elem = elements;
    for (size_t i = 0; i < size(); i++)
    {
        alloc.construct(dest++, move(*elem++));
    }
    free();
    elements = newData;
    first_free = dest;
    cap = elements + n;
}

void StrVec::reallocate()
{
    auto newCapacity = size() ? 2 * size() : 1;
    alloc_n_move(newCapacity);
}

void StrVec::reserve(size_t n)
{
    if (n < capacity())
        return;
    alloc_n_move(n);
}

void StrVec::resize(size_t n)
{
    resize(n, string());
}

void StrVec::resize(size_t n, const string &s)
{
    if (n < size())
    {
        while (first_free != elements + n)
        {
            alloc.destroy(--first_free);
        }
    }
    else if (n > size())
    {
        while (first_free != elements + n)
        {
            alloc.construct(first_free++, s);
        }
    }
}

class QueryResult
{
    friend ostream &print(ostream &, const QueryResult &);

private:
    string word;
    shared_ptr<set<size_t>> nos;
    shared_ptr<StrVec> input;

public:
    QueryResult(const string &s, shared_ptr<set<size_t>> set, shared_ptr<StrVec> v) : word(s), nos(set), input(v) {}
    ~QueryResult();
};

QueryResult::~QueryResult()
{
}

ostream &print(ostream &out, const QueryResult &qr)
{
    out << qr.word << " occurs " << qr.nos->size() << (qr.nos->size() > 1 ? " times" : " time") << endl;
    for (auto i : *qr.nos)
        out << "\t(line " << i + 1 << ") " << qr.input->at(i) << endl;
    return out;
}

class TextQuery
{
private:
    shared_ptr<StrVec> input;
    map<string, shared_ptr<set<size_t>>> result;

public:
    TextQuery(ifstream &);
    ~TextQuery();

    QueryResult query(const string &) const;
};

TextQuery::TextQuery(ifstream &ifs) : input(new StrVec)
{
    size_t lineNo = 0;
    for (string line; getline(ifs, line); ++lineNo)
    {
        input->push_back(line);
        istringstream line_stream(line);
        for (string text, word; line_stream >> text; word.clear())
        {
            // remove_copy_if(text.begin(), text.end(), back_inserter(word), ispunct);
            auto &nos = result[word];
            if (!nos)
                nos.reset(new set<size_t>);
            nos->insert(lineNo);
        }
    }
}

QueryResult TextQuery::query(const string &str) const
{
    // use static just allocate once.
    static std::shared_ptr<std::set<size_t>> nodate(new std::set<size_t>);
    auto found = result.find(str);
    if (found == result.end())
        return QueryResult(str, nodate, input);
    else
        return QueryResult(str, found->second, input);
}

TextQuery::~TextQuery()
{
}

void runQueries(ifstream &infile)
{
    TextQuery tq(infile);
    while (true)
    {
        cout << "enter word to look for, or q to quit: ";
        string s;
        if (!(cin >> s) || s == "q")
            break;
        print(cout, tq.query(s)) << endl;
    }
}

void foo(String x)
{
    std::cout << x.c_str() << std::endl;
}

void bar(const String &x)
{
    std::cout << x.c_str() << std::endl;
}

String baz()
{
    String ret("world");
    return ret;
}

int main()
{
    char text[] = "world";

    String s0;
    String s1("hello");
    String s2(s0);
    String s3 = s1;
    String s4(text);
    s2 = s1;

    foo(s1);
    bar(s1);
    foo("temporary");
    bar("temporary");
    String s5 = baz();

    std::vector<String> svec;
    svec.reserve(8);
    svec.push_back(s0);
    svec.push_back(s1);
    svec.push_back(s2);
    svec.push_back(s3);
    svec.push_back(s4);
    svec.push_back(s5);
    svec.push_back(baz());
    svec.push_back("good job");

    for (const auto &s : svec)
    {
        std::cout << s.c_str() << std::endl;
    }

    ifstream file("data/storyDataFile.txt");
    runQueries(file);
}
