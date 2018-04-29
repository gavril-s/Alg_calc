#pragma once
#include "polynomial.h"
#include "monomial.h"

#include <sstream>
#include <vector>
#include <cmath>

bool is_valid_equation(polynomial p, int pow)
{
    if (p.members.size())
    {
        bool ok = true;
        var<monomial> var {'x'};
        bool found_var = false;

        for (int i = 0; i < p.members.size(); i++)
        {
            if (!p.members[i].vars.size())
                continue;

            if (!found_var && p.members[i].vars.size() == 1)
            {
                var = p.members[i].vars[0];
                found_var = true;
            }

            if (p.members[i].vars.size() > 1)
            {
                ok = false;
                break;
            }

            for (auto v : p.members[i].vars)
            {
                if (found_var && v.name != var.name)
                    return false;
                if (v.pow.n < 0 || v.pow.n > pow || v.pow.vars.size())
                    ok = false;
            }
        }

        return (ok && found_var);
    }
    else
        return false;
}

std::pair<var<monomial>, std::vector<double>> solve(std::string str)
{
    std::stringstream ss1;
    std::stringstream ss2;

    int i = 0;
    for (; i < str.length() && str[i] != '='; i++)
    {
        ss1 << str[i];
    }
    i++;
    for (; i < str.length(); i++)
    {
        ss2 << str[i];
    }

    ss1 << '\n';
    ss2 << '\n';

    polynomial left{ss1};
    polynomial right{ss2};

    for (int i = 0; i < right.members.size();)
    {
        right.members[i].n *= -1;
        left.members.push_back(right.members[i]);
        right.members.erase(right.members.begin() + i, right.members.begin() + i + 1);
    }
    right = polynomial{0};


    var<monomial> VAR;
    bool found_var = false;
    for (auto i : left.members)
    {
        if (i.vars.size())
        {
            if (!found_var)
            {
                VAR = i.vars[0];
                VAR.pow = 1;
                found_var = true;
            }
        }
    }

    if (!found_var)
        throw(std::string{"error: np variable!"});

    left = simplification(left);

    found_var = false;
    for (auto i : left.members)
    {
        if (i.vars.size())
        {
            if (!found_var)
                found_var = true;
        }
    }

    if (!found_var)
    {
        std::stringstream ss;
        if (left == 0)
            ss << VAR.name << " = (-inf; +inf)";
        else
            ss << VAR.name << " = ∅";
        throw(ss.str());
    }


    int exp = 1;
    for (;!is_valid_equation(left, exp) && exp < 5; exp++);

    polynomial fixed_left;
    for (int i = 0; i < left.members.size(); i++)
    {
        fixed_left.members.push_back(left.members[i]);

        int j = i;
        while (j <= left.members.size() - 2 && left.members[j].vars.size() &&
        ((!left.members[j+1].vars.size() && left.members[j].vars[0].pow > 1) ||
         (left.members[j+1].vars.size() && left.members[j].vars[0].pow - left.members[j+1].vars[0].pow != 1)))
        {
            monomial m;
            m.vars.push_back(var<monomial>{VAR.name, left.members[j].vars[0].pow - 1});
            m.n = 0;
            fixed_left.members.push_back(m);
            j++;
        }
    }

    if (fixed_left.members[fixed_left.members.size() - 1].vars.size())
        fixed_left.members.push_back(monomial{0});

    left = fixed_left;

    double D = 0; //discriminant

    switch (exp)
    {
    case 1:
        return std::make_pair(VAR, std::vector<double> {- left.members[1].n / left.members[0].n});
        break;
    case 2:
        D = left.members[1].n * left.members[1].n - 4*left.members[0].n*left.members[2].n;

        if (D > 0)
            return std::make_pair(var<monomial>{VAR.name}, std::vector<double>
                {(-left.members[1].n + std::sqrt(D)) / (2*left.members[0].n),
                (-left.members[1].n - std::sqrt(D)) / (2*left.members[0].n)});
        else if (D == 0)
            return std::make_pair(var<monomial>{VAR.name}, std::vector<double>{(-left.members[1].n + std::sqrt(D)) / 2*left.members[0].n});
        else if (D < 0)
            throw(std::string{"no solutions"});

        break;

    default:
        throw(std::string{"my program is too stupid to solve this("});
    }

    return std::make_pair(var<monomial> {'x'}, std::vector<double> {0});
}

std::pair<var<monomial>, std::vector<double>> solve(polynomial left, polynomial right)
{
    std::stringstream ss;
    ss << left << '=' << right;

    return solve(ss.str());
}

/*
std::pair<std::vector<var<monomial>>, std::vector<double>> solve_sys(std::vector<polynomial> v)
{
    for (polynomial& p : v)
        p = simplification(p);

    std::vector<var<monomial>> vars;
    for (polynomial p : v)
        for (monomial m : p.members)
            for (var<monomial> a : m.vars)
                if (!(std::find(vars.begin(), vars.end(), a) >= vars.begin && std::find(vars.begin(), vars.end(), a) < vars.end()))
                    vars.push_back(a);

    if (vars.size() > 2)
        throw(std::string{"too many variables"});

    return std::make_pair(std::vector<var<monomial>>, std::vector<double>);
}

std::pair<std::vector<var<monomial>>, std::vector<double>> solve_sys(std::vector<std::string> v)
{
    std::vector<polynomial> p (v.size());

    for (std::string str : v)
        p.push_back(polynomial{str});

    return solve_sys(p);
}
*/
