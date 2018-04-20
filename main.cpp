#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include "monomial.h"
#include "polynomial.h"
#include "equations.h"

using namespace std;

void get_str(string& str)
{
    char ch;
    cin.get(ch);

    while (ch != '\n')
    {
        str += ch;
        cin.get(ch);
    }
}

int main()
{
    const polynomial EXIT {"exit"};
    const polynomial QUIT {"quit"};
    const polynomial CLEAR {"clear"};

    while (true)
    {
        try
        {
            cout << "> ";

            string str;
            get_str(str);

            bool equation = false;
            for (char ch : str)
                if (ch == '=')
                {
                    equation = true;
                    break;
                }

            if (equation)
            {
                pair<var<monomial>, vector<double>> p = solve(str);

                sort(p.second.begin(), p.second.end());

                cout << p.first << " = ";
                for (int i = 0; i < p.second.size(); i++)
                {
                    cout << p.second[i];
                    if (i + 1 < p.second.size())
                        cout << "; ";
                }
                cout << endl;
            }
            else
            {
                polynomial p {str};

                if (p == CLEAR)
                {
                    system("clear");
                    continue;
                }

                if (p == EXIT || p == QUIT)
                    break;

                p = simplification(p);
                cout << p << endl;
            }
        }
        catch (std::string str)
        {
            std::cout << str << endl;
        }
    }

    return 0;
}
