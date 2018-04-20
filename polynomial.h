#pragma once
#include "monomial.h"
#include "fsm.h"

#include <iostream>
#include <vector>
#include <sstream>
#include <algorithm>

class polynomial
{
private:
    std::vector<monomial> members;

    enum states {I, W, D, F, V, EXP, ED, EF, EV, B, BE, BED, ERR};
    /*
        I - inital
        W - waiting for smt after +/-
        D - digit
        F - fraction
        V - variable
        EXP - exponent
        ED - exponent : digit
        EF - exponent : fraction
        EV - exponent : variable
        B - brackets
        BE - brackets : exponent
        BED - brackets : exponent : digit
        ERR - error
    */

    static bool comp(monomial m1, monomial m2)
    {
        if (!m1.vars.size() && m2.vars.size())
            return false;
        else if (m1.vars.size() && !m2.vars.size())
            return true;
        else if (!m1.vars.size() && !m2.vars.size())
            return m1.n < m2.n;

        int i = 0;
        bool b = false;
        for (; i < m1.vars.size() && i < m2.vars.size(); i++)
        {
            if (m1.vars[i].name != m2.vars[i].name)
            {
                b = true;
                break;
            }
        }

        if (b)
            return m1.vars[i].name < m2.vars[i].name;
        else
        {
            int j = 0;
            for (; j < m1.vars.size() && j < m2.vars.size(); j++)
            {
                if (!m1.vars[j].pow.vars.size() && !m1.vars[j].pow.vars.size()
                    && m1.vars[j].pow.n != m2.vars[j].pow.n)
                {
                    break;
                }
            }

            return m1.vars[j].pow.n > m2.vars[j].pow.n;
        }
    }
public:
    polynomial() {}
    polynomial(std::istream&);
    polynomial(std::vector<monomial> m) : members{m} {}
    polynomial(monomial m)
    {
        members.push_back(m);
    }
    polynomial(int i)
    {
        members.push_back(monomial{i});
    }
    polynomial(std::string str)
    {
        std::stringstream ss;
        ss << str << '\n';
        *this = polynomial{ss};
    }

    polynomial operator+(polynomial);
    polynomial operator-(polynomial);
    polynomial operator*(polynomial);

    void operator+=(polynomial p)
    {
        *this = *this + p;
    }
    void operator-=(polynomial p)
    {
        *this = *this - p;
    }
    void operator*=(polynomial p)
    {
        *this = *this * p;
    }

    polynomial operator+(monomial m)
    {
        return *this + polynomial{m};
    }
    polynomial operator-(monomial m)
    {
        return *this - polynomial{m};
    }
    polynomial operator*(monomial m)
    {
        return *this * polynomial{m};
    }

    friend std::istream& operator>>(std::istream& is, polynomial& p)
    {
        p = polynomial{is};
        return is;
    }
    friend std::ostream& operator<<(std::ostream&, polynomial);

    friend polynomial simplification(polynomial);

    bool operator==(polynomial p)
    {
        if (members.size() != p.members.size())
            return false;

        for (int i = 0; i < members.size(); i++)
            if (members[i] != p.members[i])
                return false;

        return true;
    }

    bool operator!=(polynomial p)
    {
        return !(*this == p);
    }

    bool operator==(int i)
    {
        if (i == 0)
        {
            if (!members.size())
                return true;
        }
        return *this == polynomial{i};
    }

    bool operator!=(int i)
    {
        return *this != polynomial{i};
    }

    polynomial pow(int n)
    {
        if (n == 0)
            return polynomial{1};
        return *this * (*this).pow(n - 1);
    }

    friend std::pair<var<monomial>, std::vector<double>> solve(std::string);
    friend std::pair<var<monomial>, std::vector<double>> solve(polynomial, polynomial);
    friend bool is_valid_equation(polynomial, int);
};

std::ostream& operator<<(std::ostream& os, polynomial p)
{
    if (!p.members.size())
        os << '0';
    else
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

                if (!first)
                    os << " - " << m;
                else
                    os << '-' << m;
            }
            else
            {
                if (!first)
                    os << " + " << m;
                else
                    os << m;
            }

            if (first)
                first = false;
        }
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

polynomial simplification(polynomial p)
{
    if (p.members.size() <= 1)
        return p;

    for (auto i : p.members)
        i.vars_sort();

    std::sort(p.members.begin(), p.members.end(), &polynomial::comp);

    for (int i = 0; i < p.members.size() - 1; i++)
    {
        if (p.members[i].vars.size() != p.members[i+1].vars.size())
            continue;

        bool b = true;

        for (int j = 0; j < p.members[i].vars.size(); j++)
        {
            if (p.members[i].vars[j].name != p.members[i+1].vars[j].name ||
                p.members[i].vars[j].pow != p.members[i+1].vars[j].pow)
            {
                b = false;
                break;
            }
        }

        if (b)
        {
            p.members[i].n += p.members[i+1].n;
            p.members.erase(p.members.begin() + i + 1, p.members.begin() + i + 2);
            i--;
        }
    }

    for (int i = 0; i < p.members.size(); i++)
    {
        if (p.members[i].n == 0)
        {
            p.members.erase(p.members.begin() + i, p.members.begin() + i + 1);
            i--;
        }
    }

    return p;
}

polynomial::polynomial(std::istream& is)
{
    states state = I;
    int M = 1, E = 1, S = 1;

    polynomial temp;
    polynomial p{1};
    monomial m;
    m.n = 1;

    static std::string str;
    char ch = '0';

    while (ch != '\n')
    {
        is.get(ch);
        str += ch;

        if (ch == ' ')
            continue;

        switch (state)
        {
        case I:
            switch (ch)
            {
            case '+':
                state = W;
                break;
            case '-':
                S = -1;
                state = W;
                break;
            case DECIMAL_DIGITS:
                M = ch - '0';
                state = D;
                break;
            case LATIN:
                m.vars.push_back(var<monomial> {ch});
                state = V;
                break;
            case '(':
                temp = polynomial{is};
                state = B;
                break;
            default:
                state = ERR;
            }
            break;

        case W:
            switch (ch)
            {
            case DECIMAL_DIGITS:
                M = ch - '0';
                state = D;
                break;
            case LATIN:
                m.n *= M * S / (double)E;
                M = E = S = 1;
                m.vars.push_back(var<monomial> {ch});
                state = V;
                break;
            case '(':
                m.n *= M * S / (double)E;
                M = E = S = 1;
                temp = polynomial{is};
                state = B;
                break;
            default:
                state = ERR;
            }
            break;

        case D:
            switch (ch)
            {
            case '*':
                m.n *= M * S / (double)E;
                M = E = S = 1;
                state = I;
                break;
            case '+':
                m.n *= M * S / (double)E;
                if (!p.members.size())
                    members.push_back(m);
                else
                    *this += p * m;
                p = polynomial{1};
                m = monomial{};
                m.n = 1;
                M = E = S = 1;
                state = W;
                break;
            case '-':
                m.n *= M * S / (double)E;
                if (!p.members.size())
                    members.push_back(m);
                else
                    *this += p * m;
                p = polynomial{1};
                m = monomial{};
                m.n = 1;
                M = E = 1;
                S = -1;
                state = W;
                break;
            case DECIMAL_DIGITS:
                M = M * 10 + (ch - '0');
                break;
            case '.':
                state = F;
                break;
            case LATIN:
                m.n *= M * S / (double)E;
                M = E = S = 1;
                m.vars.push_back(var<monomial> {ch});
                state = V;
                break;
            case '(':
                m.n *= M * S / (double)E;
                M = E = S = 1;
                temp = polynomial{is};
                state = B;
                break;
            case ')':
                m.n *= M * S / (double)E;
                M = E = S = 1;
                if (!p.members.size())
                    members.push_back(m);
                else
                    *this += p * m;
                ch = '\n';
                break;
            case '\n':
                str.clear();
                m.n *= M * S / (double)E;
                if (!p.members.size())
                    members.push_back(m);
                else
                    *this += p * m;
                p = polynomial{1};
                break;
            default:
                state = ERR;
            }
            break;

        case F:
            switch (ch)
            {
            case '*':
                m.n *= M * S / (double)E;
                M = E = S = 1;
                state = I;
                break;
            case '+':
                m.n *= M * S / (double)E;
                if (!p.members.size())
                    members.push_back(m);
                else
                    *this += p * m;
                p = polynomial{1};
                m = monomial{};
                m.n = 1;
                M = E = S = 1;
                state = W;
                break;
            case '-':
                m.n *= M * S / (double)E;
                if (!p.members.size())
                    members.push_back(m);
                else
                    *this += p * m;
                p = polynomial{1};
                m = monomial{};
                m.n = 1;
                M = E = 1;
                S = -1;
                state = W;
                break;
            case DECIMAL_DIGITS:
                M = M * 10 + (ch - '0');
                E *= 10;
                break;
            case LATIN:
                m.n *= M * S / (double)E;
                M = E = S = 1;
                m.vars.push_back(var<monomial> {ch});
                state = V;
                break;
            case '(':
                m.n *= M * S / (double)E;
                M = E = S = 1;
                temp = polynomial{is};
                state = B;
                break;
            case ')':
                m.n *= M * S / (double)E;
                M = E = S = 1;
                if (!p.members.size())
                    members.push_back(m);
                else
                    *this += p * m;
                ch = '\n';
                break;
            case '\n':
                str.clear();
                m.n *= M * S / (double)E;
                if (!p.members.size())
                    members.push_back(m);
                else
                    *this += p * m;
                p = polynomial{1};
                break;
            default:
                state = ERR;
            }
            break;

        case V:
            switch (ch)
            {
            case '*':
                state = I;
                break;
            case '+':
                if (!p.members.size())
                    members.push_back(m);
                else
                    *this += p * m;
                p = polynomial{1};
                m = monomial{};
                m.n = 1;
                S = 1;
                state = W;
                break;
            case '-':
                if (!p.members.size())
                    members.push_back(m);
                else
                    *this += p * m;
                p = polynomial{1};
                m = monomial{};
                m.n = 1;
                S = -1;
                state = W;
                break;
            case LATIN:
                m.vars.push_back(var<monomial> {ch});
                break;
            case DECIMAL_DIGITS:
                M = ch - '0';
                state = D;
                break;
            case '^':
                state = EXP;
                break;
            case '(':
                temp = polynomial{is};
                state = B;
                break;
            case ')':
                if (!p.members.size())
                    members.push_back(m);
                else
                    *this += p * m;
                ch = '\n';
                break;
            case '\n':
                str.clear();
                if (!p.members.size())
                    members.push_back(m);
                else
                    *this += p * m;
                p = polynomial{1};
                break;
            default:
                state = ERR;
            }
            break;

        case EXP:
            switch (ch)
            {
            case '+':
                S = 1;
                break;
            case '-':
                S = -1;
                break;
            case DECIMAL_DIGITS:
                M = ch - '0';
                state = ED;
                break;
            case LATIN:
                m.vars[m.vars.size() - 1].pow.vars.push_back(var<monomial> {ch});
                state = EV;
                break;
            default:
                state = ERR;
            }
            break;

        case ED:
            switch (ch)
            {
            case '*':
                m.vars[m.vars.size() - 1].pow.n = M * S / (double)E;
                M = E = S = 1;
                state = I;
                break;
            case '+':
                m.vars[m.vars.size() - 1].pow.n = M * S / (double)E;
                if (!p.members.size())
                    members.push_back(m);
                else
                    *this += p * m;
                p = polynomial{1};
                m = monomial{};
                m.n = 1;
                M = E = S = 1;
                state = W;
                break;
            case '-':
                m.vars[m.vars.size() - 1].pow.n = M * S / (double)E;
                if (!p.members.size())
                    members.push_back(m);
                else
                    *this += p * m;
                p = polynomial{1};
                m = monomial{};
                m.n = 1;
                M = E = 1;
                S = -1;
                state = W;
                break;
            case DECIMAL_DIGITS:
                M = 10 * M + (ch - '0');
                break;
            case '.':
                state = EF;
                break;
            case LATIN:
                m.vars[m.vars.size() - 1].pow.n = M * S / (double)E;
                m.vars[m.vars.size() - 1].pow.vars.push_back(var<monomial> {ch});
                M = E = S = 1;
                state = EV;
                break;
            case '(':
                m.vars[m.vars.size() - 1].pow.n = M * S / (double)E;
                M = E = S = 1;
                temp = polynomial{is};
                state = B;
                break;
            case ')':
                m.vars[m.vars.size() - 1].pow.n = M * S / (double)E;
                M = E = S = 1;
                if (!p.members.size())
                    members.push_back(m);
                else
                    *this += p * m;
                ch = '\n';
                break;
            case '\n':
                str.clear();
                m.vars[m.vars.size() - 1].pow.n = M * S / (double)E;
                if (!p.members.size())
                    members.push_back(m);
                else
                    *this += p * m;
                p = polynomial{1};
                break;
            default:
                state = ERR;
            }
            break;

        case EF:
            switch (ch)
            {
            case '*':
                m.vars[m.vars.size() - 1].pow.n = M * S / (double)E;
                M = E = S = 1;
                state = I;
                break;
            case '+':
                m.vars[m.vars.size() - 1].pow.n = M * S / (double)E;
                if (!p.members.size())
                    members.push_back(m);
                else
                    *this += p * m;
                p = polynomial{1};
                m = monomial{};
                m.n = 1;
                M = E = S = 1;
                state = W;
                break;
            case '-':
                m.vars[m.vars.size() - 1].pow.n = M * S / (double)E;
                if (!p.members.size())
                    members.push_back(m);
                else
                    *this += p * m;
                p = polynomial{1};
                m = monomial{};
                m.n = 1;
                M = E = 1;
                S = -1;
                state = W;
                break;
            case DECIMAL_DIGITS:
                M = M * 10 + (ch -'0');
                E *= 10;
                break;
            case LATIN:
                m.vars[m.vars.size() - 1].pow.n = M * S / (double)E;
                m.vars[m.vars.size() - 1].pow.vars.push_back(var<monomial> {ch});
                M = E = S = 1;
                state = EV;
                break;
            case '(':
                m.vars[m.vars.size() - 1].pow.n = M * S / (double)E;
                M = E = S = 1;
                temp = polynomial{is};
                state = B;
                break;
            case ')':
                m.vars[m.vars.size() - 1].pow.n = M * S / (double)E;
                M = E = S = 1;
                if (!p.members.size())
                    members.push_back(m);
                else
                    *this += p * m;
                ch = '\n';
                break;
            case '\n':
                str.clear();
                m.vars[m.vars.size() - 1].pow.n = M * S / (double)E;
                if (!p.members.size())
                    members.push_back(m);
                else
                    *this += p * m;
                p = polynomial{1};
                break;
            default:
                state = ERR;
            }
            break;

        case EV:
            switch (ch)
            {
            case '*':
                state = I;
                break;
            case '+':
                state = W;
                break;
            case '-':
                if (!p.members.size())
                    members.push_back(m);
                else
                    *this += p * m;
                p = polynomial{1};
                m = monomial{};
                m.n = 1;
                S = -1;
                state = W;
                break;
            case LATIN:
                m.vars[m.vars.size() - 1].pow.vars.push_back(var<monomial> {ch});
                break;
            case DECIMAL_DIGITS:
                M = ch - '0';
                state = ED;
                break;
            case '(':
                temp *= polynomial{is};
                state = B;
                break;
            case ')':
                if (!p.members.size())
                    members.push_back(m);
                else
                    *this += p * m;
                ch = '\n';
                break;
            case '\n':
                str.clear();
                if (!p.members.size())
                    members.push_back(m);
                else
                    *this += p * m;
                p = polynomial{1};
                break;
            default:
                state = ERR;
            }
            break;

        case B:
            switch (ch)
            {
            case '*':
                p *= temp;
                temp = polynomial{};
                state = I;
                break;
            case '+':
                *this += p * temp * m;
                p = polynomial{1};
                temp = polynomial{};
                m = monomial{};
                m.n = 1;
                state = W;
                break;
            case '-':
                *this += p * temp * m;
                S = -1;
                p = polynomial{1};
                temp = polynomial{};
                m = monomial{};
                m.n = 1;
                state = W;
                break;
            case DECIMAL_DIGITS:
                p *= temp;
                temp = polynomial{};
                M = ch - '0';
                state = D;
                break;
            case LATIN:
                p *= temp;
                temp = polynomial{};
                m.vars.push_back(var<monomial> {ch});
                state = V;
                break;
            case '^':
                state = BE;
                break;
            case '(':
                p *= temp;
                temp = polynomial{is};
                break;
            case ')':
                *this += p * temp * m;
                ch = '\n';
                break;
            case '\n':
                *this += p * temp * m;
                break;
            default:
                state = ERR;
            }
            break;

        case BE:
            switch (ch)
            {
            case DECIMAL_DIGITS:
                M = ch - '0';
                state = BED;
                break;
            default:
                state = ERR;
            }
            break;

        case BED:
            switch (ch)
            {
            case '*':
                p *= temp.pow(M * S / (double)E);
                temp = polynomial{};
                state = I;
                break;
            case '+':
                temp = temp.pow(M * S / (double)E);
                *this += p * temp * m;
                M = E = S = 1;
                temp = polynomial{};
                p = polynomial{1};
                m = monomial{};
                m.n = 1;
                state = W;
                break;
            case '-':
                temp = temp.pow(M * S / (double)E);
                *this += p * temp * m;
                M = E = 1;
                S = -1;
                temp = polynomial{};
                p = polynomial{1};
                m = monomial{};
                m.n = 1;
                state = W;
                break;
            case DECIMAL_DIGITS:
                M = M * 10 + (ch - '0');
                break;
            case ')':
                temp = temp.pow(M * S / (double)E);
                *this += p * temp * m;
                ch = '\n';
                break;
            case '\n':
                str.clear();
                temp = temp.pow(M * S / (double)E);
                *this += p * temp * m;
                break;
            default:
                state = ERR;
            }
            break;

        case ERR:
            int index = str.size() - 2;
            do {
                is.get(ch);
                str += ch;
            } while(ch != '\n');

            for (int i = 0; i < index; i++)
                str += ' ';
            str += '^';

            std::string s = str;
            str.clear();
            throw(std::string{"error: bad symbol:\n"} + s);
            break;
        }
    }
}
