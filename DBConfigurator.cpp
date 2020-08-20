#include "Credentials.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>

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