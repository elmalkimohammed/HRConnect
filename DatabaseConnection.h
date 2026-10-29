#ifndef DATABASECONNECTION_H
#define DATABASECONNECTION_H

#include <mysql_driver.h>
#include <mysql_connection.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include <memory>
#include <string>

class DatabaseConnection {
public:
    static DatabaseConnection& getInstance();

    std::unique_ptr<sql::Connection> getConnection();

private:
    DatabaseConnection();
    ~DatabaseConnection();

    std::string host;
    std::string user;
    std::string password;
    std::string database;

    sql::mysql::MySQL_Driver* driver;
};

#endif // DATABASECONNECTION_H
