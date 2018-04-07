#pragma once
#include "fsm.h"
#include "monomial.h"

class polynomial
{
private:
    std::vector<monomial> members;

    enum states {inital, main_state, whole_part, fraction, letter, exponent};
public:
    polynomial() {}
    polynomial(std::istream&);
    polynomial(std::vector<monomial> m) : members{m} {}
    polynomial(int i)
    {
        members.push_back(monomial{i});
    }
    polynomial(std::string str)
    {
        std::stringstream ss;
        ss << str;
        *this = polynomial{ss};
    }

    polynomial operator+(polynomial);
    polynomial operator-(polynomial);
    polynomial operator*(polynomial);

    friend std::istream& operator>>(std::istream&, polynomial);
    friend std::ostream& operator<<(std::ostream&, polynomial);
};

polynomial::polynomial(std::istream& is)
{
    double n = 0;
    monomial m;
    m.n = 0;
    bool get = false;
    char ch = '0', last_ch;
    double length = 1;
    states state = inital;

    while (ch != '\n')
    {
        last_ch = ch;
        is.get(ch);
        //std::cout << ch << '\t' << n << '\t' << m << '\t' << *this << '\t' << state << std::endl;

        if (ch == ' ')
            continue;

        switch (state)
        {
            case inital:
                switch (ch)
                {
                case '+':
                    state = main_state;
                    break;
                case '-':
                    n = -1;
                    state = main_state;
                    break;
                default:
                    if (std::isdigit(ch))
                    {
                        is.unget();
                        state = whole_part;
                    }
                    else if (std::isalpha(ch))
                    {
                        is.unget();
                        state = letter;
                    }

                    break;
                }
                break;

            case main_state:
                switch (ch)
                {
                case '+':
                    break;
                case '-':
                    n *= -1;
                    break;
                case '*':
                    state = inital;
                    break;
                case '\n':
                    members.push_back(m);
                    break;
                default:
                    if (std::isdigit(ch))
                    {
                        is.unget();
                        state = whole_part;
                    }
                    else if (std::isalpha(ch))
                    {
                        is.unget();
                        state = letter;
                    }

                    break;
                }
                break;

            case whole_part:
                switch (ch)
                {
                case '+': case '-':
                    is.unget();
                    m.n *= n;
                    n = 0;
                    members.push_back(m);
                    m = monomial{};
                    get = false;
                    state = inital;
                    break;
                case '*':
                    m.n *= n;
                    n = 0;
                    state = inital;
                    break;
                case '.':
                    state = fraction;
                    break;
                case '\n':
                    //std::cout << "end!\n";
                    m.n *= n;
                    is.unget();
                    state = main_state;
                    break;
                default:
                    if (std::isdigit(ch))
                    {
                        if (!get)
                            m.n = 1;
                        get = true;

                        if (last_ch == '-')
                            n = -(ch - '0');
                        else if (n < 0)
                            n = n * 10 - (ch - '0');
                        else
                            n = n * 10 + (ch - '0');
                    }
                    else if (std::isalpha(ch))
                    {
                        m.n *= n;
                        n = 0;
                        is.unget();
                        state = letter;
                    }

                    break;
                }
                break;

            case fraction:
                switch (ch)
                {
                case '+': case '-':
                    is.unget();
                    m.n *= n;
                    n = 0;
                    length = 1;
                    members.push_back(m);
                    m = monomial{};
                    get = false;
                    state = inital;
                    break;
                case '*':
                    m.n *= n;
                    n = 0;
                    length = 1;
                    state = inital;
                    break;
                case '\n':
                    m.n *= n;
                    is.unget();
                    state = main_state;
                    break;
                default:
                    if (std::isalpha(ch))
                    {
                        m.n *= n;
                        n = 0;
                        length = 1;
                        is.unget();
                        state = letter;
                    }
                    else if (std::isdigit(ch))
                    {
                        length *= 10;
                        if (n < 0)
                            n -= (ch - '0') / length;
                        else
                            n += (ch - '0') / length;
                    }
                    break;
                }
                break;

            case letter:
                switch (ch)
                {
                case '+': case '-':
                    is.unget();
                    members.push_back(m);
                    n = 0;
                    m = monomial{};
                    get = false;
                    state = inital;
                    break;
                case '*':
                    state = inital;
                    break;
               /* case '^':
                    state = exponent;
                    break;*/
                default:
                    if (ch == '^')
                    {
                        state = exponent;
                    }
                    else if (std::isdigit(ch))
                    {
                        is.unget();
                        state = whole_part;
                    }
                    else if (std::isalpha(ch))
                    {
                        if (!get)
                            m.n = 1;
                        get = true;
                        m.vars.push_back(var<monomial>{ch});
                    }

                    break;
                }
                break;

            case exponent:
                std::string str;
                if (ch == '(')
                    while (ch != ')')
                    {
                        str += ch;
                        is.get(ch);
                    }
                else
                    while (ch != '*' && ch != '-' && ch != '+' && ch != '\n')
                    {
                        str += ch;
                        is.get(ch);
                    }

                str += '\n';
                std::stringstream ss;
                ss << str;
                m.vars[m.vars.size() - 1].pow = monomial{ss};

                if (ch == '*' || ch == '\n')
                    state = main_state;
                else if (ch == '-' || ch == '+')
                    state = inital;
                is.unget();

                break;
        }
    }
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
