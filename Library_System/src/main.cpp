#include <iostream>
#include "database.h"

int main() {
    try {
        // Initialize database connection
        DatabaseConnection::initialize("localhost", "library_user", "password123", "My_First_DB", 3306);
        
        // Get database instance
        DatabaseConnection& db = DatabaseConnection::getInstance();
        
        // Connect to database
        if (!db.connect()) {
            std::cerr << "Failed to connect to database: " << db.getLastError() << std::endl;
            return 1;
        }
        
        std::cout << "Successfully connected to database!" << std::endl;
        std::cout << "Server info: " << db.getServerInfo() << std::endl;
        std::cout << "Client info: " << db.getClientInfo() << std::endl;
        std::cout << "Current database: " << db.getCurrentDatabase() << std::endl;
        
        // Disconnect when done
        db.disconnect();
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}