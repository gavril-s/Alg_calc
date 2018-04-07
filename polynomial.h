#pragma once
#include "fsm.h"
#include "monomial.h"

class polynomial
{
private:
    std::vector<monomial> members;

    enum states {I, W, D, F, V, EXP, ED, EF, EV, ERR};
    /*
        I - inital
        W - waiting for smt after +/-
        D - digit
        F - fraction
        V - variable
        EXP - exponent
        ED - exponent : digit
        EF - exponent : fraction
        ERR - error
    */
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
    states state = I;
    int M = 1, E = 1, S = 1;

    monomial m;
    m.n = 1;

    std::string str;
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
                members.push_back(m);
                m = monomial{};
                m.n = 1;
                M = E = S = 1;
                state = W;
                break;
            case '-':
                m.n *= M * S / (double)E;
                members.push_back(m);
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
            case '\n':
                m.n *= M * S / (double)E;
                members.push_back(m);
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
                members.push_back(m);
                m = monomial{};
                m.n = 1;
                M = E = S = 1;
                state = W;
                break;
            case '-':
                m.n *= M * S / (double)E;
                members.push_back(m);
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
            case '\n':
                m.n *= M * S / (double)E;
                members.push_back(m);
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
                members.push_back(m);
                m = monomial{};
                m.n = 1;
                S = 1;
                state = W;
                break;
            case '-':
                members.push_back(m);
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
            case '\n':
                members.push_back(m);
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
                members.push_back(m);
                m = monomial{};
                m.n = 1;
                M = E = S = 1;
                state = W;
                break;
            case '-':
                m.vars[m.vars.size() - 1].pow.n = M * S / (double)E;
                members.push_back(m);
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
            case '\n':
                m.vars[m.vars.size() - 1].pow.n = M * S / (double)E;
                members.push_back(m);
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
                members.push_back(m);
                m = monomial{};
                m.n = 1;
                M = E = S = 1;
                state = W;
                break;
            case '-':
                m.vars[m.vars.size() - 1].pow.n = M * S / (double)E;
                members.push_back(m);
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
            case '\n':
                m.vars[m.vars.size() - 1].pow.n = M * S / (double)E;
                members.push_back(m);
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
                members.push_back(m);
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
            case '\n':
                members.push_back(m);
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

            throw(std::string{"error: bad symbol:\n"} + str);
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
            if (!first)
                os << " - " << m;
            else
                os << '-' << m;
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
