#include <iostream>
#include <string>
#include <pqxx/pqxx>
#include <iomanip>
#include <boost/algorithm/string/trim.hpp>

using namespace std;

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
        string sql = "SELECT id, last_name, first_name FROM accounts";

        pqxx::nontransaction N(C);

        try
        {
            pqxx::result R(N.exec(sql.c_str()));

            // If there is an empty table, return a message saying so instead of nothing
            if (R.empty())
            {
                cout << "There are no accounts in the bank." << endl;
            }

            // Considering tabulate for a better table output
            cout << "ID" << setw(15) << "Last Name" << setw(15) << "First Name" << endl;
            for (pqxx::result::const_iterator i = R.begin(); i != R.end(); ++i)
            {
                cout << i[0].as<int>() << setw(15) << i[1].as<string>() << setw(15) << i[2].as<string>() << endl;
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
        cout << "Please enter the account first holder's name:" << endl;
        cin >> firstName;
        cout << "Please enter an initial deposit ammount" << endl;
        cin >> amount;

        //pqxx::work W(C);
        sql = "INSERT INTO accounts(last_name, first_name, amount) VALUES (' " + lastName + "', '" + firstName + "', " + to_string(amount) + ");";
        N.exec(sql.c_str());

        // TODO:
        //Fails when trying to use pqxx::work. Tutorial on postgreSQL's site used pqxx::work and pqxx::in same function, but differnt order. Maybe that's it
        //possible work around https://stackoverflow.com/questions/26464056/pqxx-reuse-reactivate-a-work-transaction
        //W.exec(sql.c_str());
        //W.commit();
    }

    void deposit(pqxx::connection &C)
    {
        int id;

        cout << "Please enter the ID of the account you wish to deposit to" << endl;
        cin >> id;

        string sql = "SELECT * FROM accounts WHERE id=" + to_string(id);

        pqxx::nontransaction N(C);

        try
        {
            pqxx::result R(N.exec(sql.c_str()));

            // If there is an empty result, return a message saying so
            if (R.empty())
            {
                cout << "That account does not exist." << endl;
            }

            // There should only be one record, but not sure how you can read the row from R without itterating through it
            for (pqxx::result::const_iterator i = R.begin(); i != R.end(); ++i)
            {
                string yn;

                do
                {
                    cout << "Do you wish to deposit to the account belonging to " << i[2].as<string>() << " " << i[1].as<string>() << " (y/n)?" << endl;
                    cin >> yn;

                    // Change our read in option to uppercase for comparison
                    transform(yn.begin(), yn.end(), yn.begin(), ::toupper);
                } while (yn.compare("Y") != 0 && yn.compare("N") != 0); // I keep thinking this needs to be || but it needs to be &&

                // We should only reach here if the user answered y or n
                // If confirmed to deposit, read the amount from the database, add to it, and then update the record in the database
                if (yn.compare("Y") == 0)
                {
                    double deposit = 0, amount = i[3].as<double>();

                    cout << "How much do you wish to deposit?" << endl;
                    cin >> deposit;

                    amount += deposit;
                    sql = "UPDATE accounts SET amount = " + to_string(amount) + " WHERE id=" + to_string(id);
                    pqxx::result R(N.exec(sql.c_str()));

                    // We should only be updating one record so it should be safe to break out of the for loop after it's been updated
                    if (R.affected_rows() == 1)
                    {
                        cout << "Update successful" << endl;
                        break;
                    }
                }
                // If we're not going to deposit, just break out of the for loop
                else if (yn.compare("N") == 0)
                {
                    cout << "Aborting" << endl;
                    break;
                }
            }
        }
        catch (const pqxx::undefined_table &e)
        {
            std::cerr << e.what() << std::endl;
        }
    }

    void withdraw(pqxx::connection &C)
    {
        // TODO:
        // This is a copy/paste from the deposit code with minor modification.
        // Might combine the two and use some flag to determine if we are subtracting or adding
        int id;

        cout << "Please enter the ID of the account you wish to withdraw" << endl;
        cin >> id;

        string sql = "SELECT * FROM accounts WHERE id=" + to_string(id);

        pqxx::nontransaction N(C);

        try
        {
            pqxx::result R(N.exec(sql.c_str()));

            // If there is an empty result, return a message saying so
            if (R.empty())
            {
                cout << "That account does not exist." << endl;
            }

            // There should only be one record, but not sure how you can read the row from R without itterating through it
            for (pqxx::result::const_iterator i = R.begin(); i != R.end(); ++i)
            {
                string yn;

                do
                {
                    cout << "Do you wish to withdraw from the account belonging to " << i[2].as<string>() << " " << i[1].as<string>() << " (y/n)?" << endl;
                    cin >> yn;

                    // Change our read in option to uppercase for comparison
                    transform(yn.begin(), yn.end(), yn.begin(), ::toupper);
                } while (yn.compare("Y") != 0 && yn.compare("N") != 0); // I keep thinking this needs to be || but it needs to be &&

                // We should only reach here if the user answered y or n
                // If confirmed to withdraw, read the amount from the database, add to it, and then update the record in the database
                if (yn.compare("Y") == 0)
                {
                    double withdraw = 0, amount = i[3].as<double>();

                    cout << "How much do you wish to withdraw?" << endl;
                    cin >> withdraw;

                    amount -= withdraw;

                    if (amount < 0)
                    {
                        cout << "Error: Can't have negative balance. Aborting" << endl;
                        break;
                    }

                    sql = "UPDATE accounts SET amount = " + to_string(amount) + " WHERE id=" + to_string(id);
                    pqxx::result R(N.exec(sql.c_str()));

                    // We should only be updating one record so it should be safe to break out of the for loop after it's been updated
                    if (R.affected_rows() == 1)
                    {
                        cout << "Update successful" << endl;
                        break;
                    }
                }
                // If we're not going to deposit, just break out of the for loop
                else if (yn.compare("N") == 0)
                {
                    cout << "Aborting" << endl;
                    break;
                }
            }
        }
        catch (const pqxx::undefined_table &e)
        {
            std::cerr << e.what() << std::endl;
        }
    }

    void viewBalance(pqxx::connection &C)
    {
        int id;

        cout << "Please enter the ID of the account you view the balance of." << endl;
        cin >> id;

        string sql = "SELECT * FROM accounts WHERE id=" + to_string(id);

        pqxx::nontransaction N(C);

        try
        {

            pqxx::result R(N.exec(sql.c_str()));

            // If there is an empty result, return a message saying so
            if (R.empty())
            {
                cout << "That account does not exist." << endl;
            }

            // Same as above. Need to consider tabulate for a cleaner outpu
            cout << "ID" << setw(15) << "Last Name" << setw(15) << "First Name" << setw(15) << "Balance" << endl;
            for (pqxx::result::const_iterator i = R.begin(); i != R.end(); ++i)
            {
                cout << i[0].as<int>() << setw(15) << i[1].as<string>() << setw(15) << i[2].as<string>() << setw(15) << i[3].as<double>() << endl;
            }
        }
        catch (const pqxx::undefined_table &e)
        {
            std::cerr << e.what() << std::endl;
        }
    }

    void modifyAccount(pqxx::connection &C)
    {
        int id;

        cout << "Please enter the ID of the account you wish to modify" << endl;
        cin >> id;

        string sql = "SELECT * FROM accounts WHERE id=" + to_string(id);

        pqxx::nontransaction N(C);

        try
        {

            pqxx::result R(N.exec(sql.c_str()));

            // If there is an empty result, return a message saying so
            if (R.empty())
            {
                cout << "That account does not exist." << endl;
            }

            for (pqxx::result::const_iterator i = R.begin(); i != R.end(); ++i)
            {
                string lastName = i[1].as<string>();
                string firstName = i[2].as<string>();

                string newLastName = "";
                string newFirstName = "";

                cout << "Enter a new FIRST name for the account holder. Leave blank if you do not wish to change." << endl;

                // Use getline() so we can accept an empty string. Since "cin >>" leaves a newline (\n) in the buffer our getline will be ignored.
                // Use cin.ignore(numeric_limits<streamsize>::max(),'\n') to ignore everything the in the buffer until our newline.
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                getline(cin, newFirstName);

                // Trim the name the user gave. If the string is empty, then we won't reassign the account name.
                boost::algorithm::trim(newFirstName);
                if (!newFirstName.empty())
                {
                    firstName = newFirstName;
                }

                cout << "Enter a new LAST name for the account holder. Leave blank if you do not wish to change." << endl;
                getline(cin, newLastName);

                boost::algorithm::trim(newLastName);
                if (!newLastName.empty())
                {
                    lastName = newLastName;
                }

                sql = "UPDATE accounts SET last_name = '" + lastName + "', first_name = '" + firstName + "' WHERE id = " + to_string(id);
                pqxx::result R(N.exec(sql.c_str()));

                // We should only be updating one record so it should be safe to break out of the for loop after it's been updated
                if (R.affected_rows() == 1)
                {
                    cout << "Update successful" << endl;
                    break;
                }
            }
        }
        catch (const pqxx::undefined_table &e)
        {
            std::cerr << e.what() << std::endl;
        }
    }

    void deleteAccount(pqxx::connection &C)
    {
        int id;

        cout << "Please enter the ID of the account you wish to delete" << endl;
        cin >> id;

        string sql = "SELECT * FROM accounts WHERE id=" + to_string(id);

        pqxx::nontransaction N(C);

        try
        {

            pqxx::result R(N.exec(sql.c_str()));

            // If there is an empty result, return a message saying so
            if (R.empty())
            {
                cout << "That account does not exist." << endl;
            }

            for (pqxx::result::const_iterator i = R.begin(); i != R.end(); ++i)
            {
                string yn;

                do
                {
                    cout << "Do you wish to delete the account belonging to " << i[2].as<string>() << " " << i[1].as<string>() << " ?" << endl;
                    cout << "This action can't be undone! (y/n)" << endl;
                    cin >> yn;

                    // Change our read in option to uppercase for comparison
                    transform(yn.begin(), yn.end(), yn.begin(), ::toupper);
                } while (yn.compare("Y") != 0 && yn.compare("N") != 0); // I keep thinking this needs to be || but it needs to be &&

                // We should only reach here if the user answered y or n
                // If confirmed to withdraw, read the amount from the database, add to it, and then update the record in the database
                if (yn.compare("Y") == 0)
                {

                    sql = "DELETE FROM accounts WHERE id=" + to_string(id);
                    pqxx::result R(N.exec(sql.c_str()));

                    // We should only be updating one record so it should be safe to break out of the for loop after it's been updated
                    if (R.affected_rows() == 1)
                    {
                        cout << "Delete successful" << endl;
                        break;
                    }
                }
                // If we're not going to deposit, just break out of the for loop
                else if (yn.compare("N") == 0)
                {
                    cout << "Aborting" << endl;
                    break;
                }
            }
        }
        catch (const pqxx::undefined_table &e)
        {
            std::cerr << e.what() << std::endl;
        }
    }
};
