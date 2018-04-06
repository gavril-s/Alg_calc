#pragma once

#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <iostream>

template <typename T>
struct var
{
    char name;
    T pow;

    var(char ch) : name{ch}, pow{T{1}} {}
    var(char ch, T p) : name{ch}, pow{p} {}

    bool operator==(var<T> v)
    {
        if (name != v.name)
            return false;
        if (pow != v.pow)
            return false;
        return true;
    }
    bool operator!=(var<T> v)
    {
        return !(*this == v);
    }
};

class monomial
{
private:
    enum states {inital, main_state, whole_part, fraction, letter, exponent};

    static bool comp(var<monomial> v1, var<monomial> v2)
    { return v1.name < v2.name; }
public:
    std::vector<var<monomial>> vars;
    double n; //numerical value

    monomial() : n{0} {}
    monomial(std::istream&);
    monomial(double N) : n{N} {}
    monomial(int N) : n{(double)N} {}
    monomial(std::vector<var<monomial>> v) : n{1}
    {
        std::sort(v.begin(), v.end(), comp);
        vars = v;
    }
    monomial(double N, std::vector<var<monomial>> v) : n{N}
    {
        std::sort(v.begin(), v.end(), comp);
        vars = v;
    }

    monomial operator+(monomial);
    monomial operator-(monomial);
    monomial operator*(monomial);
    monomial operator/(monomial);

    void operator+=(monomial m)
    {
        *this = *this + m;
    }
    void operator-=(monomial m)
    {
        *this = *this - m;
    }
    void operator*=(monomial m)
    {
        *this = *this * m;
    }
    void operator/=(monomial m)
    {
        *this = *this / m;
    }


    bool operator==(monomial m)
    {
        if (n != m.n || vars.size() != m.vars.size())
            return false;
        for (int i = 0; i < vars.size(); i++)
            if (vars[i] != m.vars[i])
                return false;
        return true;
    }
    bool operator!=(monomial m)
    {
        return !(*this == m);
    }

    friend std::ostream& operator<<(std::ostream& os, monomial m)
    {
        os << m.n;
        for (var<monomial> i : m.vars)
            if (i.pow != monomial{1})
                os << '*' << i.name << '^' << i.pow;
            else
                os << '*' << i.name;

        return os;
    }
};

monomial::monomial(std::istream& is)
{
    /*states state = inital;
    char ch = '0';
    double length = 1;
    int i = 0;
    std::vector<int> v;

    while (int i = 0; ch != '\n'; v.push_back(1), i++)
    {
        is.get(ch);

        switch (state)
        {
        case inital:
            switch (ch)
            {
            case '-':
                v[i] *= -1;
                state = main_state;
                break;
            case '+'
                state = main_state;
                break;
            default:
                if (std::isalpha(ch))
                {
                    vars.push_back(var(ch));
                    state = letter;
                }
                else if (std::isdigit(ch))
                {
                    if (v[i] < 0)
                        v[i] = v[i]*10 - (ch - '0');
                    else
                        v[i] = v[i]*10 + (ch - '0');
                    state = whole_part;
                }
                break;
            }
            break;

        case main_state:
            switch (ch)
            {
            case '*':
                state = inital;
                break;
            default:
                if (std::isalpha(ch))
                {
                    vars.push_back(var(ch));
                    state = letter;
                }
                else if (std::isdigit(ch))
                {
                    if (v[i] < 0)
                        v[i] = v[i]*10 - (ch - '0');
                    else
                        v[i] = v[i]*10 + (ch - '0');
                    state = whole_part;
                }
                break;
            }
            break;

        case whole_part:
            switch (ch)
            {
            case '*':
                state = inital;
                break;
            case '.'
                state = fraction;
                break;
            default:
                if (std::isdigit(ch))
                {
                    if (v[i] < 0)
                        v[i] = v[i]*10 - (ch - '0');
                    else
                        v[i] = v[i]*10 + (ch - '0');
                    state = whole_part;
                }
                break;
            }
            break;

        case fraction:
            switch (ch)
            {
            case '*':
                state = inital;
                break;
            default:
                if (std::isalpha(ch))
                {
                    vars.push_back(var(ch));
                    state = letter;
                }
                else if (std::isdigit(ch))
                {
                    length *= 10;
                    if (v[i] < 0)
                        v[i] -= (ch - '0') / length;
                    else
                        v[i] += (ch - '0') / length;
                }
                break;
            }
            break;

        case letter
            switch (ch)
            {
            case '*':
                state = inital;
                break;
            case '^':
                state = exponent;
                break;
            default:
                if (std::isalpha(ch))
                    vars.push_back(var{ch});
                else if (std::isdigit(ch))
                {
                    if (v[i] < 0)
                        v[i] = v[i]*10 - (ch - '0');
                    else
                        v[i] = v[i]*10 + (ch - '0');
                    state = whole_part;
                }
                break;
            }
            break;

        case exponent:
            string str;
            is.get(ch);
            if (ch == '(')
            {
                is.get(ch);
                while (ch != ')')
                {
                    str += ch;
                    is.get(ch);
                }
            }
            else
            {
                is.get(ch);
                while (ch != '*')
                {
                    str += ch;
                    is.get(ch);
                }
            }
            str += '\n';
            stringstream ss;
            ss << str;
            vars[vars.size() - 1].pow = polynomial{ss};

            state = inital;

            break;
        }
    }

    n = 1;
    for (double d : v)
        n *= d;*/
}

monomial monomial::operator+(monomial m)
{
    monomial res;
    res.n = n + m.n;

    if (m.vars.size() != vars.size())
        throw(std::string{"Буквенные части одночленов не равны!"});
    for (int i = 0; i < vars.size(); i++)
        if (vars[i].name != m.vars[i].name || vars[i].pow != m.vars[i].pow)
            throw(std::string{"Буквенные части одночленов не равны!"});

    res.vars = vars;
    return res;
}

monomial monomial::operator-(monomial m)
{
    monomial res;
    res.n = n - m.n;

    if (m.vars.size() != vars.size())
        throw(std::string{"Буквенные части одночленов не равны!"});
    for (int i = 0; i < vars.size(); i++)
        if (vars[i].name != m.vars[i].name || vars[i].pow != m.vars[i].pow)
            throw(std::string{"Буквенные части одночленов не равны!"});

    res.vars = vars;
    return res;
}

monomial monomial::operator*(monomial m)
{
    monomial res;
    res.n = n * m.n;
    res.vars = vars;

    for (var<monomial> i : m.vars)
        res.vars.push_back(i);

    std::sort(res.vars.begin(), res.vars.end(), comp);
    for (int i = 0; i < res.vars.size(); i++)
    {
        while (i + 1 < res.vars.size() && res.vars[i].name == res.vars[i+1].name)
        {
            res.vars[i].pow += res.vars[i+1].pow;
            res.vars.erase(res.vars.begin() + i + 1, res.vars.begin() + i + 2);
        }
    }

    return res;
}

monomial monomial::operator/(monomial m)
{
    monomial res;
    res.n = n / m.n;

    if (m.vars.size() != vars.size())
        throw(std::string{"Буквенные части одночленов не равны!"});
    for (int i = 0; i < vars.size(); i++)
        if (vars[i].name != m.vars[i].name)
            throw(std::string{"Буквенные части одночленов не равны!"});

    res.vars = vars;
    for (int i = 0; i < res.vars.size(); i++)
    {
        res.vars[i].pow -= m.vars[i].pow;
        if (res.vars[i].pow == 0)
        {
            res.vars.erase(res.vars.begin() + i, res.vars.begin() + i + 1);
        }
    }

    return res;
}
