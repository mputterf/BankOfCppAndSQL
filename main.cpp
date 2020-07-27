#include <iostream>
using namespace std;

class Bank
{
};

int main()
{

    int option;

    cout << "Hello and welcome to the Bank of C++ and SQL.\nPlease select and option." << endl;

    cin >> option;
    switch (option)
    {
    case 0:
        cout << "Goodbye." << endl;
        break;

    default:
        cout << "Invalid option. Please try again." << endl;
        break;
    }

    return 0;
}