#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <iostream>

struct var
{
    char name;
    int pow;

    var(char ch) : name{ch}, pow{1} {}
    var(char ch, int p) : name{ch}, pow{p} {}
};

class monomial
{
private:
    static bool comp(var v1, var v2)
    { return v1.name < v2.name; }
public:
    std::vector<var> vars;
    double n; //numerical value

    monomial() : n{0} {}
    monomial(std::string);
    monomial(double N) : n{N} {}
    monomial(std::vector<var> v) : n{1}
    {
        std::sort(v.begin(), v.end(), comp);
        vars = v;
    }
    monomial(double N, std::vector<var> v) : n{N}
    {
        std::sort(v.begin(), v.end(), comp);
        vars = v;
    }

    monomial operator+(monomial);
    monomial operator-(monomial);
    monomial operator*(monomial);
    monomial operator/(monomial);

    friend std::ostream& operator<<(std::ostream& os, monomial m)
    {
        os << m.n;
        for (var i : m.vars)
            if (i.pow != 1)
                os << '*' << i.name << '^' << i.pow;
            else
                os << '*' << i.name;

        return os;
    }
};

monomial::monomial(std::string str)
{
    std::stringstream ss;
    ss << str;
    ss >> n;

    while(!ss.eof())
    {
        char ch;
        ss >> ch;
        if (ch == '*' || ch == ' ')
            continue;
        if (ch == '^')
        {
            int p = 0;
            ss >> p;
            vars[vars.size() - 1].pow += p;
            continue;
        }
        vars.push_back(var{ch});
    }
    vars.erase(vars.begin() + vars.size() - 1, vars.end());

    std::sort(vars.begin(), vars.end(), comp);
    for (int i = 0; i < vars.size(); i++)
    {
        while (i + 1 < vars.size() && vars[i].name == vars[i+1].name)
        {
            vars[i].pow += vars[i+1].pow;
            vars.erase(vars.begin() + i + 1, vars.begin() + i + 2);
        }
    }
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

    for (var i : m.vars)
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
        if (vars[i].name != m.vars[i].name || vars[i].pow != m.vars[i].pow)
            throw(std::string{"Буквенные части одночленов не равны!"});

    res.vars = std::vector<var> {};
    return res;
}
