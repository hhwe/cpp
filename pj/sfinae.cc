#include <iostream>

// Implicit constructor: anything with a Show() function.
template <typename T, typename = decltype(&T::Show)>
void ShowType(const T &t)
{
    t.Show();
    return;
}

// Implicit constructor: map-like objects (std::map, std::unordered_map, etc)
template <class M, typename std::enable_if<
                       std::is_constructible<std::string, decltype(std::declval<M>().begin()->first)>::value && std::is_constructible<Json, decltype(std::declval<M>().begin()->second)>::value,
                       int>::type = 0>
Json(const M &m) : Json(object(m.begin(), m.end())) {}

// Implicit constructor: vector-like objects (std::list, std::vector, std::set, etc)
template <class V, typename std::enable_if<
                       std::is_constructible<Json, decltype(*std::declval<V>().begin())>::value,
                       int>::type = 0>
Json(const V &v) : Json(array(v.begin(), v.end())) {}

// This prevents Json(some_pointer) from accidentally producing a bool. Use
// Json(bool(some_pointer)) if that behavior is desired.
Json(void *) = delete;

class A
{
public:
    void Show() const { std::cout << "Hello World!" << std::endl; }
};

int main(int argc, char const *argv[])
{
    A t;
    ShowType(t);
    return 0;
}
