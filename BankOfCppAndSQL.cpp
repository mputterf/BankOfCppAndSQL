// BankOfCppAndSQL.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <string>
#include <iostream>
#include <pqxx/pqxx>
#include <fstream>
#include <filesystem>
#include "Credentials.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>


using namespace std;

class Bank
{
public:
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

class DBConfigurator : public Credentials {
public:
   
    void setHost(string hostFromConfig) {
        host = hostFromConfig;
    }

    void setPort(string portFromConfig) {
        port = portFromConfig;
    }

    void setUser(string userFromConfig) {
        user = userFromConfig;
    }

    void setPassword(string passwdFromConfig) {
        password = passwdFromConfig;
    }

    void setDatabase(string DBFromConfig) {
        database = DBFromConfig;
    }

    string getHost() {
        return host;
    }

    string getPort() {
        return port;

    }

    string getUser() {
        return user;
    }

    string getPassword() {
        return password;
    }

    string getDatabase() {
        return database;
    }

    void configParser() {
        boost::property_tree::ptree pt;
        boost::property_tree::ini_parser::read_ini("db_config.ini", pt);

        setHost(pt.get<std::string>("General.host"));
        setPort(pt.get<std::string>("General.port"));
        setUser(pt.get<std::string>("General.user"));
        setPassword(pt.get<std::string>("General.password"));
        setDatabase(pt.get<std::string>("General.database"));

        //cout << getHost() << endl;
        /*std::cout << pt.get<std::string>("General.host") << std::endl;
        std::cout << pt.get<std::string>("General.port") << std::endl;*/
    }

    //void configParser() {
    //    vector<string> config;
    //    string line;
    //    ifstream postgresConfig;

    //    postgresConfig.open("db_config.txt", ios::in);

    //    while (!postgresConfig.eof()) {
    //        getline(postgresConfig, line);
    //        config.push_back(line);
    //    }

    //    for (auto i : config) {
    //        //Thank you stack overflow. Search through each element of the string vector for a substring.
    //        //In this case find our config keywords so we can set the appropiate variables for psql access.
    //        if (find_if(config.begin(), config.end(), [](const string& str) { return str.find("host") != string::npos; } ) != config.end()) {
    //            //substring search and either return not "host =" or position 3 (should be the hostname)
    //            size_t equal = i.find("=");
    //            if (equal != string::npos) {
    //                //incomplete
    //                string parameter = i.substr(3, string::npos);
    //            }
    //            
    //            //temp
    //            //cout << "parameter: " << parameter << endl;
    //        }
    //    }
    //    
    //}
          
};


int main()
{
    Bank bank;
    DBConfigurator psqlConf;
    int option;
    
    if (std::filesystem::exists("db_config.ini")) {
        psqlConf.configParser();
    }
    else {
        cout << "Couldn't open file db_config.ini" << endl;
        return 1;
    }

    string connection_string("host=" + psqlConf.getHost() + " port=" + psqlConf.getPort() + " dbname=" + psqlConf.getDatabase() + " user=" + psqlConf.getUser() + " password=" + psqlConf.getPassword());

    pqxx::connection C(connection_string.c_str());

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

        // temp
        // C.disconnect();
    }
    catch (const std::exception& e)
    {
        cerr << e.what() << endl;
        return 1;
    }

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

    return 0;
}