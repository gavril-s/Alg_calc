#include <iostream>
#include <string>
#include "polynomial.h"

using namespace std;

int main()
{
    string str;
    cin >> str;
    polynomial p1{str};
    str.clear();
    cin >> str;
    polynomial p2{str};

    try
    {
        cout << p1 << endl << p2 << endl;
        cout << p1 << " * " << p2 << " == " << p1 * p2 << endl;
        cout << p1 << " + " << p2 << " == " << p1 + p2 << endl;
        cout << p1 << " - " << p2 << " == " << p1 - p2 << endl;
    }
    catch(string str)
    {
        cout << endl << str << endl;
    }

    return 0;
}
