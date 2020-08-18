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
    pqxx::connection openConnection(string host, string port, string database, string user, string password)
    {

        string connection_string("host=" + host + " port=" + port + " dbname=" + database + " user=" + user + " password=" + password);

        pqxx::connection C(connection_string.c_str());

        try
        {
            if (C.is_open())
            {
                cout << "Open connection to database " << C.dbname() << " successful" << endl;

                return C;
            }
            else
            {
                cout << "Failed to open database" << endl;
                //should probably not be returning the connection if it failed, but I don't know what else
                //to return right now and get build errors in VS 2019 if it's a return with nothing
                return C;
            }
        }
        catch (const std::exception &e)
        {
            cerr << e.what() << endl;
            //Same as stated above.
            return C;
        }

        // return C;
    }

    void returnAccounts(pqxx::connection &C)
    {
        //Get the bank accounts from database
        string sql = "SELECT * FROM accounts";

        pqxx::nontransaction N(C);

        try
        {
            pqxx::result R(N.exec(sql.c_str()));

            // If there is an empty table, return a message saying so instead of nothing
            if (R.empty())
            {
                cout << "There are no accounts in the bank." << endl;
            }

            //temp to test read from database
            //this for loop is from postgreSQL's website
            for (pqxx::result::const_iterator i = R.begin(); i != R.end(); ++i)
            {
                cout << "ID = " << i[0].as<int>() << endl;
                cout << "Last Name = " << i[1].as<string>() << endl;
                cout << "First Name = " << i[2].as<string>() << endl;
                cout << "Amount = " << i[3].as<string>() << endl;
            }
        }
        catch (const pqxx::undefined_table &e)
        {
            std::cerr << e.what() << std::endl;
        }
    }

    void createAccount(pqxx::connection &C)
    {
        string lastName, firstName;
        double amount;

        //Create bank account table if it doesn't exist
        string sql = "CREATE TABLE IF NOT EXISTS accounts (id SERIAL PRIMARY KEY NOT NULL, last_name VARCHAR, first_name VARCHAR, amount NUMERIC(12, 2));";
        pqxx::nontransaction N(C);
        N.exec(sql.c_str());

        cout << "Please enter the account last holder's name:" << endl;
        cin >> lastName;
        cout << "Please enter the account last holder's name:" << endl;
        cin >> firstName;
        cout << "Please enter an initial deposit ammount" << endl;
        cin >> amount;

        //pqxx::work W(C);
        sql = "INSERT INTO accounts(last_name, first_name, amount) VALUES (' " + lastName + "', '" + firstName + "', " + to_string(amount) + ");";
        N.exec(sql.c_str());

        //Fails when trying to use pqxx::work. Tutorial on postgreSQL's site used pqxx::work and pqxx::in same function, but differnt order. Maybe that's it
        //possible work around https://stackoverflow.com/questions/26464056/pqxx-reuse-reactivate-a-work-transaction
        //W.exec(sql.c_str());
        //W.commit();
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
        pqxx::connection C = bank.openConnection(psqlConf.getHost(), psqlConf.getPort(), psqlConf.getDatabase(), psqlConf.getUser(), psqlConf.getPassword());

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
                bank.returnAccounts(C);
                break;

            case 2:
                bank.createAccount(C);
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