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

    var() : pow{T{1}} {}
    var(char ch) : name{ch}, pow{T{1}} {}
    var(char ch, T p) : name{ch}, pow{p} {}
    var(char ch, int i) : name{ch}, pow{T{i}} {}

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

    friend std::ostream& operator<<(std::ostream& os, var<T> v)
    {
        os << v.name;
        if (v.pow != T{1})
            os << '^' << v.pow;

        return os;
    }
};

class monomial
{
private:
    static bool comp(var<monomial> v1, var<monomial> v2)
    { return v1.name < v2.name; }
public:
    std::vector<var<monomial>> vars;
    double n; //numerical value

    monomial() : n{0} {}
    monomial(double N) : n{N} {}
    monomial(int N) : n{(double)N} {}
    monomial(var<monomial> v)
    {
        vars.push_back(v);
    }
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

    void vars_sort()
    {
        if (n == 0)
        {
            vars = std::vector<var<monomial>> {};
            return;
        }

        for (int i = 0; i < vars.size(); i++)
            if (vars[i].pow == 0)
                vars.erase(vars.begin() + i, vars.begin() + i + 1);

        if (vars.size() <= 1)
            return;

        std::sort(vars.begin(), vars.end(), comp);

        for (int i = 0; i < vars.size() - 1; i++)
        {
            while (vars[i].name == vars[i+1].name)
            {
                vars[i].pow += vars[i+1].pow;
                vars.erase(vars.begin() + i + 1, vars.begin() + i + 2);
            }
        }
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
    bool operator==(int i)
    {
        return *this == monomial{i};
    }

    bool operator!=(monomial m)
    {
        return !(*this == m);
    }
    bool operator!=(double d)
    {
        return *this != monomial{d};
    }

    bool operator>(monomial m)
    {
        if (*this - m != 0)
            return true;
        return false;
    }

    bool operator>(int i)
    {
        return *this > monomial{i};
    }

    friend std::ostream& operator<<(std::ostream& os, monomial m)
    {
        if (m.n != 1 || !m.vars.size())
        {
            os << m.n;
            if (m.vars.size())
                os << " * ";
        }

        for (int i = 0; i < m.vars.size(); i++)
        {
            if (m.vars[i].pow > 1)
            {
                os << m.vars[i].name << '^';
                if (m.vars[i].pow.vars.size() >= 1)
                    std::cout << '(' << m.vars[i].pow << ')';
                else
                    std::cout << m.vars[i].pow;
            }
            else if (m.vars[i].pow == 1)
                os << m.vars[i].name;
            else if (m.vars[i].pow == 0)
                m.vars.erase(m.vars.begin() + i, m.vars.begin() + i + 1);

            if (i + 1 < m.vars.size())
                os << " * ";
        }

        return os;
    }
};

monomial monomial::operator+(monomial m)
{
    monomial res;
    res.n = n + m.n;

    if (m.vars.size() != vars.size())
        throw(std::string{"The letter parts of monomials are not equal!"});
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
        throw(std::string{"The letter parts of monomials are not equal!"});
    for (int i = 0; i < vars.size(); i++)
        if (vars[i].name != m.vars[i].name || vars[i].pow != m.vars[i].pow)
            throw(std::string{"The letter parts of monomials are not equal!"});

    res.vars = vars;
    return res;
}

monomial monomial::operator*(monomial m)
{
    monomial res;
    res.n = n * m.n;
    res.vars = vars;

    for (auto i : m.vars)
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
        throw(std::string{"The letter parts of monomials are not equal!"});
    for (int i = 0; i < vars.size(); i++)
        if (vars[i].name != m.vars[i].name)
            throw(std::string{"The letter parts of monomials are not equalss!"});

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
