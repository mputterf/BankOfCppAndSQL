#ifndef CREDENTIALS_H
#define CREDENTIALS_H
#endif // !CREDENTIALS_H

#include <iostream>
using namespace std;

class Credentials {
protected:
	string host;
	string port;
	string user;
	string password;
	string database;

public:
	virtual void setHost(string host) = 0;
	virtual void setPort(string port) = 0;
	virtual void setUser(string user) = 0;
	virtual void setPassword(string passwd) = 0;
	virtual void setDatabase(string database) = 0;

	virtual string getHost() = 0;
	virtual string getPort() = 0;
	virtual string getUser() = 0;
	virtual string getPassword() = 0;
	virtual string getDatabase() = 0;
};