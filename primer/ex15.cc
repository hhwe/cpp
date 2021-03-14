#include "common.h"

class Quote
{
private:
    string bookNo;

protected:
    double price = 0.0;

public:
    Quote() = default;
    Quote(const string &b, double p) : bookNo(b), price(p) {}
    virtual ~Quote() = default;

    string isbn() const { return bookNo; }
    virtual double net_price(size_t n) const { return n * price; }

    virtual void debug() const;
};

void Quote::debug() const
{
    cout << bookNo << endl;
    cout << price << endl;
}

class Disc_quote : public Quote
{
private:
    size_t quantity = 0;
    double discount = 0.0;

public:
    Disc_quote() = default;
    Disc_quote(const string &book, double price, size_t qty, double disc) : Quote(book, price), quantity(qty), discount(disc) {}

    double net_price(size_t) const = 0;
};

class Bulk_quote : public Disc_quote
{
private:
    size_t min_qty = 0;
    double discount = 0.0;

public:
    Bulk_quote() = default;
    Bulk_quote(const string &book, double price, size_t qty, double disc) : Disc_quote(book, price, qty, disc) {}
    ~Bulk_quote() = default;

    double net_price(size_t) const override;
};

double Bulk_quote::net_price(size_t cnt) const
{
    if (cnt >= min_qty)
    {
        return cnt * (1 - discount) * price;
    }
    else
    {
        return cnt * price;
    }
}

double print_total(std::ostream &os, const Quote &item, size_t n)
{
    double ret = item.net_price(n);

    os << "ISBN:" << item.isbn()
       << "# sold: " << n << " total due: " << ret << std::endl;

    return ret;
}

int main(int argc, char const *argv[])
{
    Quote q;
    cout << "bookNo: " << q.isbn() << " price" << q.net_price(1) << endl;
    print_total(cout, q, 2);
    Bulk_quote bq = Bulk_quote("123", 12, 1, 0.5);
    print_total(cout, bq, 2);
    return 0;
}
