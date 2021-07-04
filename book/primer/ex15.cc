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

    virtual Quote *clone() const & { return new Quote(*this); }
    virtual Quote *clone() && { return new Quote(move(*this)); }

    string isbn() const { return bookNo; }
    virtual double net_price(size_t n) const { return n * price; }

    virtual void debug() const;
};

void Quote::debug() const
{
    cout << "bookNo: " << bookNo << endl;
    cout << "price: " << price << endl;
}

class Disc_quote : public Quote
{
protected:
    size_t quantity = 0;
    double discount = 0.0;

public:
    Disc_quote() = default;
    Disc_quote(const string &book, double price, size_t qty, double disc) : Quote(book, price), quantity(qty), discount(disc) {}

    double net_price(size_t) const = 0;
};

class Bulk_quote : public Disc_quote
{
public:
    Bulk_quote() = default;
    Bulk_quote(const string &book, double price, size_t qty, double disc) : Disc_quote(book, price, qty, disc) {}
    ~Bulk_quote() = default;

    virtual Quote *clone() const & { return new Bulk_quote(*this); }
    virtual Quote *clone() && { return new Bulk_quote(move(*this)); }

    double net_price(size_t) const override;
};

double Bulk_quote::net_price(size_t cnt) const
{
    if (cnt >= quantity)
    {
        cout << "111 " << cnt << endl;
        return cnt * (1 - discount) * price;
    }
    else
    {
        cout << "222 " << cnt << endl;
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

class Basket
{

public:
    void add_item(const Quote &sale) { items.insert(shared_ptr<Quote>(sale.clone())); }
    void add_item(const Quote &&sale) { items.insert(shared_ptr<Quote>(move(sale).clone())); }

    double total_receipt(ostream &) const;

private:
    static bool
    compare(const shared_ptr<Quote> &lhs, const shared_ptr<Quote> &rhs)
    {
        return lhs->isbn() < rhs->isbn();
    }
    multiset<shared_ptr<Quote>, decltype(compare) *> items{compare};
};

double Basket::total_receipt(ostream &os) const
{
    double sum = 0.0;
    for (auto iter = items.cbegin(); iter != items.cend(); iter = items.upper_bound(*iter))
    {
        sum += print_total(os, **iter, items.count(*iter));
    }
    os << "Total Sale " << sum << endl;
    return sum;
}

int main(int argc, char const *argv[])
{
    Quote q;
    cout << "bookNo: " << q.isbn() << " price" << q.net_price(1) << endl;
    print_total(cout, q, 2);
    Bulk_quote bq = Bulk_quote("123", 12, 1, 0.5);
    bq.debug();
    print_total(cout, bq, 2);

    vector<Quote> basket;

    basket.push_back(Quote("0-201-1", 50));
    basket.push_back(Bulk_quote("0-201-8", 50, 10, .25));
    cout << basket.back().net_price(15) << endl;

    vector<shared_ptr<Quote>> basket1;

    basket1.push_back(make_shared<Quote>("0-201-1", 50));
    basket1.push_back(make_shared<Bulk_quote>("0-201-8", 50, 10, .25));
    cout << basket1.back()->net_price(15) << endl;

    return 0;
}
