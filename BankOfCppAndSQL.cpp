// BankOfCppAndSQL.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <string>
#include <iostream>
#include <pqxx/pqxx>
#include <boost/filesystem.hpp>
#include "Bank.cpp"
#include "DBConfigurator.cpp"

using namespace std;

class Bank;
class DBConfigurator;

int main()
{

    int option;

    if (boost::filesystem::exists("db_config.ini"))
    {

        Bank bank;
        DBConfigurator psqlConf;

        psqlConf.configParser();
        pqxx::connection C = bank.openConnection(psqlConf.getHost(), psqlConf.getPort(), psqlConf.getDatabase(), psqlConf.getUser(), psqlConf.getPassword());

        cout << "Hello and welcome to the Bank of C++ and SQL." << endl;
        do
        {
            cout << "Please select and option." << endl;
            cout << "0. Quit." << endl;
            cout << "1. Get bank accounts." << endl;
            cout << "2. Create account." << endl;
            cout << "3. Deposit." << endl;
            cout << "4. Withdraw." << endl;
            cout << "5. View balance of an account" << endl;
            cout << "6. Modify an account." << endl;
            cout << "7. Delete an account." << endl;

            cin >> option;

            switch (option)
            {
            case 0:
                cout << "Goodbye." << endl;
                break;

            case 1:
                bank.returnAccounts(C);
                break;

            case 2:
                bank.createAccount(C);
                break;

            case 3:
                bank.deposit(C);
                break;

            case 4:
                bank.withdraw(C);
                break;

            case 5:
                bank.viewBalance(C);
                break;

            case 6:
                bank.modifyAccount(C);
                break;

            case 7:
                bank.deleteAccount(C);
                break;

            default:
                cout << "Invalid option. Please try again." << endl;
                break;
            }
        } while (option != 0);
    }
    else
    {
        cout << "Couldn't open file db_config.ini" << endl;
        return 1;
    }

    return 0;
}