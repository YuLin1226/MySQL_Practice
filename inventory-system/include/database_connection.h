// database_connection.h
#pragma once
#include <soci/soci.h>
#include <soci/mysql/soci-mysql.h>
#include <string>
#include <memory>

class DatabaseConnection {
private:
    std::unique_ptr<soci::session> sql_;
    static std::unique_ptr<DatabaseConnection> instance_;
    
    // Private constructor for singleton
    DatabaseConnection(const std::string& connectString);

public:
    ~DatabaseConnection() = default;
    
    // Prevent copying
    DatabaseConnection(const DatabaseConnection&) = delete;
    DatabaseConnection& operator=(const DatabaseConnection&) = delete;
    
    // Initialize database connection
    static void initialize(const std::string& host,
                         const std::string& user,
                         const std::string& password,
                         const std::string& database,
                         unsigned int port = 3306);
                         
    static DatabaseConnection& getInstance();
    
    // Get the SOCI session
    soci::session& getSession() { return *sql_; }
    
    // Basic operations
    bool isConnected() const;
    std::string getCurrentDatabase() const;
};

// database_connection.cpp
#include "database_connection.h"
#include <sstream>
#include <stdexcept>

std::unique_ptr<DatabaseConnection> DatabaseConnection::instance_ = nullptr;

DatabaseConnection::DatabaseConnection(const std::string& connectString) {
    try {
        sql_ = std::make_unique<soci::session>(
            *soci::factory_mysql(), 
            connectString
        );
    }
    catch (const std::exception& e) {
        throw std::runtime_error("Failed to connect to database: " + std::string(e.what()));
    }
}

void DatabaseConnection::initialize(const std::string& host,
                                 const std::string& user,
                                 const std::string& password,
                                 const std::string& database,
                                 unsigned int port) {
    if (!instance_) {
        std::stringstream ss;
        ss << "db=" << database << " "
           << "user=" << user << " "
           << "password='" << password << "' "
           << "host=" << host << " "
           << "port=" << port;
        
        instance_.reset(new DatabaseConnection(ss.str()));
    }
}

DatabaseConnection& DatabaseConnection::getInstance() {
    if (!instance_) {
        throw std::runtime_error("Database not initialized. Call initialize() first.");
    }
    return *instance_;
}

bool DatabaseConnection::isConnected() const {
    try {
        int dummy;
        soci::statement st = (sql_->prepare << "SELECT 1", soci::into(dummy));
        st.execute(true);
        return true;
    }
    catch (...) {
        return false;
    }
}

std::string DatabaseConnection::getCurrentDatabase() const {
    std::string dbName;
    *sql_ << "SELECT DATABASE()", soci::into(dbName);
    return dbName;
}