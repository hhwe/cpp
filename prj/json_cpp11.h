#ifndef CPP_JSON11_H_
#define CPP_JSON11_H_

#include <string>
#include <vector>
#include <map>

namespace json11
{

    class Json final
    {
    public:
        enum Type
        {
            NUL,
            NUMBER,
            BOOL,
            STRING,
            ARRAY,
            OBJECT
        };

        typedef std::vector<Json> array;
        typedef std::map<std::string, Json> object;

        // Constructors for the various types of JSON value.
        Json() noexcept;                // NUL
        Json(std::nullptr_t) noexcept;  // NUL
        Json(double value);             // NUMBER
        Json(int value);                // NUMBER
        Json(bool value);               // BOOL
        Json(const std::string &value); // STRING
        Json(std::string &&value);      // STRING
        Json(const char *value);        // STRING
        Json(const array &values);      // ARRAY
        Json(array &&values);           // ARRAY
        Json(const object &values);     // OBJECT
        Json(object &&values);          // OBJECT

        // Implicit constructor: anything with a to_json() function.
        template <class T, class = decltype(&T::to_json)>
        Json(const T &t) : Json(t.to_json()) {}

        Type type() const;

        bool is_null() const { return type() == NUL; }
        bool is_number() const { return type() == NUMBER; }
        bool is_bool() const { return type() == BOOL; }
        bool is_string() const { return type() == STRING; }
        bool is_array() const { return type() == ARRAY; }
        bool is_object() const { return type() == OBJECT; }
    };

} // namespace json11

#endif