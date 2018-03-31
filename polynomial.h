#pragma once

#include "monomial.h"

class polynomial
{
private:
    std::vector<monomial> members;
public:
    polynomial() {}
    polynomial(std::string);
    polynomial(std::vector<monomial> m) : members{m} {}

    polynomial operator+(polynomial);
    polynomial operator-(polynomial);
    polynomial operator*(polynomial);

    friend std::istream& operator>>(std::istream&, polynomial);
    friend std::ostream& operator<<(std::ostream&, polynomial);
};

polynomial::polynomial(std::string str)
{
    char ch;
    std::string temp;

    for (int i = 0; i < str.length(); i++)
    {
        ch = str[i];
        if (ch == '+' || ch == '-')
        {
            std::cout << temp << ' ' << ch << std::endl; //+++++
            members.push_back(monomial{temp});
            temp.clear();
        }
        temp += ch;
    }
    members.push_back(monomial{temp});
    std::cout << temp << std::endl; //+++++
}

std::istream& operator>>(std::istream& is, polynomial p)
{
    std::string str;
    char ch;
    is.get(ch);

    while ((ch >= 'a' && ch <= 'z') || (ch >= '0' && ch <= '9') || ch == '-' ||
    ch == '+' || ch == '*' || ch == '/' || ch == '^' || ch == ' ')
    {
        str += ch;
        is >> ch;
    }

    p = polynomial{str};
    return is;
}

std::ostream& operator<<(std::ostream& os, polynomial p)
{
    bool first = true;
    for (monomial m : p.members)
    {
        if (m.n > 0)
        {
            if (!first)
                os << " + ";
            os << m;
        }
        else if (m.n < 0)
        {
            m.n = abs(m.n);
            os << " - " << m;
        }

        if (first)
            first = false;
    }

    return os;
}

polynomial polynomial::operator+(polynomial p)
{
    polynomial res;
    res.members = members;

    for (auto i : p.members)
        res.members.push_back(i);

    return res;
}

polynomial polynomial::operator-(polynomial p)
{
    polynomial res;
    res.members = members;

    for(auto i : p.members)
    {
        i.n *= -1;
        res.members.push_back(i);
    }

    return res;
}

polynomial polynomial::operator*(polynomial p)
{
    polynomial res;

    for (auto i : members)
        for (auto j : p.members)
            res.members.push_back(i * j);

    return res;
}
