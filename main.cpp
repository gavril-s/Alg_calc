#include <iostream>
#include <string>
#include "polynomial.h"

using namespace std;

int main()
{
    while (true)
    {
        try
        {
            polynomial p{cin};
            p = simplification(p);
            cout << p << endl;
        }
        catch (std::string str)
        {
            std::cout << str << endl;
        }
    }

    return 0;
}
