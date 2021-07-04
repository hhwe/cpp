#ifdef MYSTL_VECTOE_H_
#define MYSTL_VECTOE_H_

namespace mystl
{
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
} // namespace mystl

#endif // MYSTL_VECTOE_H_
