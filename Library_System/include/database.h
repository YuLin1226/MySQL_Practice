#ifndef DATABASE_H
#define DATABASE_H

#include <string>
#include <memory>
#include <mysql/mysql.h>

class DatabaseConnection {
private:
    std::string host_;
    std::string user_;
    std::string password_;
    std::string database_;
    unsigned int port_;
    MYSQL* connection_;
    
    // Singleton instance
    static std::unique_ptr<DatabaseConnection> instance_;
    
    // Private constructor for singleton pattern
    DatabaseConnection(const std::string& host, 
                      const std::string& user,
                      const std::string& password,
                      const std::string& database,
                      unsigned int port = 3306);

public:
    // Prevent copying and assignment
    DatabaseConnection(const DatabaseConnection&) = delete;
    DatabaseConnection& operator=(const DatabaseConnection&) = delete;
    
    // Destructor
    ~DatabaseConnection();
    
    // Singleton access method
    static DatabaseConnection& getInstance();
    
    // Initialize the database connection
    static void initialize(const std::string& host,
                         const std::string& user,
                         const std::string& password,
                         const std::string& database,
                         unsigned int port = 3306);
    
    // Connection management
    bool connect();
    void disconnect();
    bool isConnected() const;
    
    // Basic database information
    std::string getCurrentDatabase() const;
    std::string getServerInfo() const;
    std::string getClientInfo() const;
    
    // Error handling
    std::string getLastError() const;
    
    // Get raw connection (for advanced usage)
    MYSQL* getRawConnection() { return connection_; }
};

#endif // DATABASE_H