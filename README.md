# BankOfCppAndSQL
## About
This is a simple bank management project written in C++ for practice. It also served as a way to learn how to use the libpqxx and boost libraries since I never used external libraries before.

## Executables
Please look in `Releases` and under your operating system.

## How to use
1. Please create a bank database in PostgreSQL first. A simple one-liner `CreateBank.sql` file is provided if you would like to use.
2. Create a `db_config.ini` with your database configuration. It must follow the format below.

Example config:
```
[General]
host = localhost
port = 5432
user = postgres
password = <your postgres user password here>
database = bank
```

## How to build
### Prerequisites
Please make sure you have `libpq` and `libpqxx` installed. For Linux, they should be in your distro's repository. For Mac, you can install them through brew. For Windows, I recommend visiting this [github](https://github.com/GordonLElliott/Easy-PQXX-Build-for-Windows-Visual-Studio) for an easier time building libpqxx on windows (libpq comes with PostgreSQL)

### Linux and Mac
This project was built in vscode. You can select `Run task` in the Terminal menu and `Build DBConfigurator.o`, `Build Bank.o` and then `C/C++: g++ build main program` and hopefully it will build.

### Windows
TODO (but it was built in Visual Studio 2019)