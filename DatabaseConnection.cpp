#include "DatabaseConnection.h"
#include <stdexcept>

DatabaseConnection& DatabaseConnection::getInstance() {
    static DatabaseConnection instance;
    return instance;
}

DatabaseConnection::DatabaseConnection() {
    host = "tcp://127.0.0.1:3306";
    user = "root"; // Change to your MySQL username
    password = ""; // Change to your MySQL password
    database = "GestionRH";

    try {
        driver = sql::mysql::get_mysql_driver_instance();
    } catch (sql::SQLException &e) {
        throw std::runtime_error("Could not get MySQL driver instance: " + std::string(e.what()));
    }
}

DatabaseConnection::~DatabaseConnection() {
    // Cleanup if needed
}

std::unique_ptr<sql::Connection> DatabaseConnection::getConnection() {
    try {
        std::unique_ptr<sql::Connection> con(driver->connect(host, user, password));
        con->setSchema(database);
        return con;
    } catch (sql::SQLException &e) {
        throw std::runtime_error("Could not connect to database: " + std::string(e.what()));
    }
}
