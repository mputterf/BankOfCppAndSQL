// BankOfCppAndSQL.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <string>
#include <iostream>
#include <pqxx/pqxx>
#include <fstream>
#include <boost/filesystem.hpp>
#include "Credentials.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>

using namespace std;

class Bank;
class DBConfigurator;

class Bank
{
public:
 
    int openConnection(string host, string port, string database, string user, string password) {
        string connection_string("host=" + host + " port=" + port + " dbname=" + database + " user=" + user + " password=" + password);

        pqxx::connection C(connection_string.c_str());

        pqxx::work wrk(C);

        try
        {
            if (C.is_open())
            {
                cout << "Open connection to database " << C.dbname() << " successful" << endl;
            }
            else
            {
                cout << "Failed to open database" << endl;
                return 1;
            }

        }
        catch (const std::exception& e)
        {
            cerr << e.what() << endl;
            return 1;
        }

        return 0;
    }

    void returnAccounts()
    {
        
        cout << "temp" << endl;
    }

    void createAccount()
    {
        string name, address;
        double amount;

        cout << "Please enter the account holder's name:" << endl;
        cin >> name;
        cout << "Please enter account holder's address:" << endl;
        cin >> address;
        cout << "Please enter an initial deposit ammount" << endl;
        cin >> amount;

        // temp
        cout << "Name: " + name << endl;
        cout << "Address: " + address << endl;
        // cout doesn't work well with floats. Use printf and format to 2 decimal points
        printf("Amount in account: %.2f\n", amount);
    }
};

class DBConfigurator : public Credentials
{
public:
    void setHost(string hostFromConfig)
    {
        host = hostFromConfig;
    }

    void setPort(string portFromConfig)
    {
        port = portFromConfig;
    }

    void setUser(string userFromConfig)
    {
        user = userFromConfig;
    }

    void setPassword(string passwdFromConfig)
    {
        password = passwdFromConfig;
    }

    void setDatabase(string DBFromConfig)
    {
        database = DBFromConfig;
    }

    string getHost()
    {
        return host;
    }

    string getPort()
    {
        return port;
    }

    string getUser()
    {
        return user;
    }

    string getPassword()
    {
        return password;
    }

    string getDatabase()
    {
        return database;
    }

    void configParser()
    {
        boost::property_tree::ptree pt;
        boost::property_tree::ini_parser::read_ini("db_config.ini", pt);

        setHost(pt.get<std::string>("General.host"));
        setPort(pt.get<std::string>("General.port"));
        setUser(pt.get<std::string>("General.user"));
        setPassword(pt.get<std::string>("General.password"));
        setDatabase(pt.get<std::string>("General.database"));

    }

};

int main()
{
    
    int option;

    if (boost::filesystem::exists("db_config.ini"))
    {
        
        Bank bank;
        DBConfigurator psqlConf;

        psqlConf.configParser();
        bank.openConnection(psqlConf.getHost(), psqlConf.getPort(), psqlConf.getDatabase(), psqlConf.getUser(), psqlConf.getPassword());
       

        cout << "Hello and welcome to the Bank of C++ and SQL." << endl;
        do
        {
            cout << "Please select and option." << endl;
            cout << "0. Quit." << endl;
            cout << "1. Get bank accounts." << endl;
            cout << "2. Create account." << endl;

            cin >> option;

            switch (option)
            {
            case 0:
                cout << "Goodbye." << endl;
                break;

            case 1:
                bank.returnAccounts();
                break;

            case 2:
                bank.createAccount();
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